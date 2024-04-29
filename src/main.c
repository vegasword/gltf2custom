// NOTE: Impl for single mesh model, w/o textures, materials & samplers
// CURRENT FILE STRUCTURE
// +-------------------------- +
// |          HEADER           |
// +-------------------------- +
// |IndicesCount      : u32    | ✔️
// |IndicesSize       : u32    | ✔️
// |VerticesSize      : u32    | ❌
// +-------------------------- +
// |        MODEL DATA         |
// +-------------------------- +
// |Indices    : u32 *         | ✔️
// |Vertices   : f32 *         | <- VVVVNNNNUUUU
// +-------------------------- +

#include "stdio.h"
#include "stdarg.h"
#include "assert.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "typedefs.c"
#include "linear_alloc.c"

typedef struct { f32 x, y, z, w, nx, ny, nz, nw, u, v; } Vertex, *Vertices;
typedef u16* Indices;

typedef struct Model
{
  u32 indicesCount;
  u32 indicesSize;
  u32 verticesSize;
  Indices indices;
  Vertices vertices;
} Model;

void Log(const char *format, ...)
{  
  char message[4096];
  va_list args;
  va_start(args, format);
  if (args != 0) vsnprintf(message, sizeof(message), format, args);  
  va_end(args);
  strncat(message, "\n", 1);
  fprintf_s(stdout, message, args);
}

int main(int argc, char **argv)
{ 
  if (argc != 3) {
    Log("Usage: gltf2custom [input: *.gltf/glb] [output]");
    return 1;
  }

  Model model = {0};
  char *inputPath = argv[1];
  char *outputPath = argv[2];
  
  Memory memory = {0};
  MemInit(
    &memory, 
    VirtualAlloc(NULL, 1*GB, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE),
    4*GB
  );

  cgltf_options options = {0};
  cgltf_data* gltf = NULL;
  if (cgltf_parse_file(&options, inputPath, &gltf) == cgltf_result_success)
  {        
    char *bufferUri = gltf->buffer_views->buffer->uri;
    HANDLE bufferFile = CreateFileA(bufferUri, GENERIC_READ, FILE_SHARE_READ, 
                                    NULL, OPEN_EXISTING, 0, NULL);
    
    u64 bufferSize = gltf->buffers->size;
    uc *data = MemAlloc(&memory, bufferSize);
    if (ReadFile(bufferFile, data, bufferSize, NULL, NULL))
    {
      model.indicesCount = gltf->meshes->primitives->indices->count;
      model.indicesSize = gltf->meshes->primitives->indices->buffer_view->size;
      u32 indicesOffset = gltf->meshes->primitives->indices->buffer_view->offset;
      model.indices = MemAlloc(&memory, model.indicesSize);
      memcpy(model.indices, data + indicesOffset, model.indicesSize);      
    }
    else
    {
      Log("Failed to read file (%x)", GetLastError());
      return 1;
    }
    
    CloseHandle(bufferFile);
    cgltf_free(gltf);
  }
       
  // HANDLE file = CreateFile(outputPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
  // Check(file != INVALID_HANDLE_VALUE, "Failed to write to %s !", argv[2]);
  // WriteFile(file, &m->indicesCount, sizeof(u32), 0, NULL);
  // WriteFile(file, &m->indicesSize, sizeof(u32), 0, NULL);
  // WriteFile(file, &m->verticesSize, sizeof(u32), 0, NULL);
  // WriteFile(file, &m->indices, m->indicesSize, 0, NULL);
  // WriteFile(file, &m->vertices, m->verticesSize, 0, NULL);
  
  return 0;
}
