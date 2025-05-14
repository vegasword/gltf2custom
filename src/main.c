/*
  Copyright (c) 2025 Alexandre Perché (@vegasword)

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.  
*/

/*
  TODO: Use meshopt C API direct for further optimizations
  BUG: Only models vertices exported from blender works somehow
*/

#include "stdio.h"
#include "stdint.h"
#include "assert.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "type.c"
#include "arena.c"

#define CHECK(condition, message, ...) if (!(condition)) { fprintf(stderr, message, __VA_ARGS__); return 1; }
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef __declspec(align(16)) struct Vertex {
  u16 x, y, z, pad0;
  i8 nx, ny, nz, pad1;
  u16 u, v;
} Vertex;

typedef struct MetallicRoughnessMaterial {
  f32 baseColorFactor[4];
  f32 metallicFactor;
  f32 roughnessFactor;
} MetallicRoughnessMaterial;

typedef struct Model {
  u32 indicesCount;
  u32 indicesSize;
  u32 verticesCount;
  u32 verticesSize;
  f32 uvScale[2];
  f32 uvOffset[2];
  u16 minBoundary[3];
  u16 maxBoundary[3];
  MetallicRoughnessMaterial material;
  u16 *indices;
  Vertex *vertices;
} Model;

i32 main(i32 argc, char **argv)
{ 
  if (argc != 3) {
    printf("Usage: gltf2custom [input: *.gltf/glb] [output]");
    return 1;
  }
  
  Arena arena = {0};
  InitArena(&arena, VirtualAlloc(NULL, 1*GB, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE), 1*GB);

  // glTF parsing and validation
  
  Model model = {0};
  char *inputPath = argv[1], *outputPath = argv[2];
  
  cgltf_options options = {0};  
  cgltf_data* data = NULL;
  
  CHECK(cgltf_parse_file(&options, inputPath, &data) == cgltf_result_success,  "Failed to parse %s", argv[1]);
  CHECK(strncmp(data->asset.generator, "gltfpack", 8) == 0, "Model vertices should be gltfpack optimized")
  CHECK(data->meshes && data->meshes_count == 1 && data->meshes->primitives_count == 1, "Model must be merged into a single mesh");
  CHECK(data->accessors_count > 0, "Model doesn't contains any accessors (required to get its boundaries)")
  
  cgltf_mesh *mesh = &data->meshes[0];  
  cgltf_primitive *primitive = mesh->primitives;
  cgltf_accessor *accessors = data->accessors;
  
  CHECK(primitive->type == cgltf_primitive_type_triangles, "Model must be triangulated")
  
  // Fetching metallic-roughness material
  // NOTE: For now I support only one material
  
  cgltf_material *material = data->materials;
  CHECK(material && material->has_pbr_metallic_roughness, "Model must have a metallic roughness PBR material");
  
  cgltf_pbr_metallic_roughness pbrMetallicRoughness = material->pbr_metallic_roughness;
  
  model.material.baseColorFactor[0] = pbrMetallicRoughness.base_color_factor[0];
  model.material.baseColorFactor[1] = pbrMetallicRoughness.base_color_factor[1];
  model.material.baseColorFactor[2] = pbrMetallicRoughness.base_color_factor[2];
  model.material.baseColorFactor[3] = pbrMetallicRoughness.base_color_factor[3];
  
  model.material.metallicFactor = pbrMetallicRoughness.metallic_factor;
  model.material.roughnessFactor = pbrMetallicRoughness.roughness_factor;
  
  // Fetching texture transform
  
  cgltf_texture_transform transform = material->pbr_metallic_roughness.base_color_texture.transform;
  
  model.uvOffset[0] = transform.offset[0];
  model.uvOffset[1] = transform.offset[1];
  model.uvScale[0] = transform.scale[0];
  model.uvScale[1] = transform.scale[1];  
  
  // Reading buffer file (usually *.bin)
  
  char *bufferUri = data->buffer_views->buffer->uri;
  HANDLE bufferFile = CreateFileA(bufferUri, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  cgltf_size bufferSize = data->buffers->size;
  uc *bufferData = (uc *)Alloc(&arena, bufferSize);
  
  CHECK(ReadFile(bufferFile, bufferData, (DWORD)bufferSize, NULL, NULL), "Failed to read file");
  CloseHandle(bufferFile);
  
  // Fetching indices

  cgltf_accessor *indices = primitive->indices;
  
  model.indicesCount = (u32)indices->count;
  model.indicesSize = (u32)indices->buffer_view->size;  
  model.indices = (u16 *)Alloc(&arena, model.indicesSize);
  memcpy(model.indices, bufferData + indices->buffer_view->offset, model.indicesSize);      

  // Fetching vertices and boundaries
  
  cgltf_attribute *attributes = primitive->attributes;
  cgltf_size attributesCount = primitive->attributes_count;
  
  model.verticesCount = (u32)attributes->data->count;
  model.verticesSize = model.verticesCount * sizeof(Vertex);
  model.vertices = (Vertex *)Alloc(&arena, model.verticesSize);
    
  for (u32 i = 0; i < attributesCount; ++i)
  {
    cgltf_attribute attribute = attributes[i];
    cgltf_buffer_view bufferView = *attribute.data->buffer_view;
    uc *pBuffer = bufferData + bufferView.offset;
    size_t stride = attribute.data->stride;
    
    TmpArena tmp = {0};
    TmpBegin(&tmp, &arena);
    
    switch (attribute.type)
    {          
      case cgltf_attribute_type_position: {    
          
        CHECK(stride, "Invalid stride for fetching vertices positions");
          
        if (accessors->has_min && data->accessors->has_max)
        {
          model.minBoundary[0] = (u16)accessors->min[0];
          model.maxBoundary[0] = (u16)accessors->max[0];
          
          model.minBoundary[1] = (u16)accessors->min[1];
          model.maxBoundary[1] = (u16)accessors->max[1];
          
          model.maxBoundary[2] = (u16)accessors->max[2];
          model.maxBoundary[2] = (u16)accessors->max[2];
          
          for (u32 j = 0; j < model.indicesCount; ++j)
          {
            u16 *position = (u16 *)((uc *)pBuffer + j * stride);
            Vertex *vertex = &model.vertices[j];
            vertex->x = position[0];
            vertex->y = position[1];
            vertex->z = position[2];
          }
        }
        else
        {
          model.minBoundary[0] = model.minBoundary[1] = model.minBoundary[2] = UINT16_MAX;
          model.maxBoundary[0] = model.maxBoundary[1] = model.maxBoundary[2] = 0;
          
          for (u32 j = 0; j < model.indicesCount; ++j)
          {
            u16 *position = (u16 *)((uc *)pBuffer + j * stride);
            u16 x = position[0];
            u16 y = position[1];
            u16 z = position[2];
            
            Vertex *vertex = &model.vertices[j];
            vertex->x = x;
            vertex->y = y;
            vertex->z = z;
            
            model.minBoundary[0] = MIN(model.minBoundary[0], x);
            model.minBoundary[1] = MIN(model.minBoundary[1], y);
            model.minBoundary[2] = MIN(model.minBoundary[2], z);

            model.maxBoundary[0] = MAX(model.maxBoundary[0], x);
            model.maxBoundary[1] = MAX(model.maxBoundary[1], y);
            model.maxBoundary[2] = MAX(model.maxBoundary[2], z);
          }
        }
        
      } break;

      case cgltf_attribute_type_normal: {
          
        CHECK(stride, "Invalid stride for fetching vertices normals");
        
        for (u32 j = 0; j < model.verticesCount; ++j)
        {          
          i8 *normal = (i8 *)((uc *)pBuffer + j * stride);
          Vertex *vertex = &model.vertices[j];
          vertex->nx = normal[0];
          vertex->ny = normal[1];
          vertex->nz = normal[2];
        }
        
      } break;
      
      case cgltf_attribute_type_texcoord: {
          
        CHECK(stride, "Invalid stride for fetching vertices texcoords");
        
        for (u32 j = 0; j < model.verticesCount; ++j)
        {
          u16 *texcoord = (u16 *)((uc *)pBuffer + j * stride);
          Vertex *vertex = &model.vertices[j];
          vertex->u = texcoord[0];
          vertex->v = texcoord[1];          
        }
        
      } break;

      default: break;
    }
    
    TmpEnd(&tmp);
  }
    
  // Writting to output
  
  HANDLE output = CreateFile(outputPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
  
  CHECK(output != INVALID_HANDLE_VALUE, "Failed to write to %s", argv[2]);
  CHECK(WriteFile(output, &model.indicesCount, 4 * sizeof(u32), 0, NULL), "Failed to write indices or vertices metadata in the header");
  CHECK(WriteFile(output, model.uvScale, 4 * sizeof(f32), 0, NULL), "Failed to write uv metadata in the header");
  CHECK(WriteFile(output, model.minBoundary, 6 * sizeof(u16), 0, NULL), "Failed to write boundaries in the header");
  CHECK(WriteFile(output, &model.material.baseColorFactor[0], 6 * sizeof(f32), 0, NULL), "Failed to write metallic-roughness material");
  CHECK(WriteFile(output, model.indices, model.indicesSize, 0, NULL), "Failed to write indices");
  CHECK(WriteFile(output, model.vertices, model.verticesSize, 0, NULL), "Failed to write vertices");
    
  CloseHandle(output);
  return 0;
}
