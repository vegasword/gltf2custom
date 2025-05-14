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

#define KB 1024LL
#define MB KB*1000
#define GB MB*1000

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

typedef struct Arena {
  size_t cur;
  size_t prev;
  size_t capacity;
  uc *data;
} Arena;

typedef struct TmpArena {
  size_t prev;
  size_t cur;
  Arena *arena;
} TmpArena;

i32 IsPowerOfTwo(uintptr_t x)
{ 
  return (x & (x - 1)) == 0;
}

uintptr_t AlignForward(uintptr_t ptr, size_t align)
{
  uintptr_t p, a, modulo;
  assert(IsPowerOfTwo(align));
  p = ptr;
  a = (uintptr_t)align;
  modulo = p & (a - 1);
  if (modulo != 0) p += a - modulo;
  return p;
}

void *AllocAlign(Arena *arena, size_t size, size_t align)
{
  uintptr_t curr_ptr = (uintptr_t)arena->data + (uintptr_t)arena->cur;
  uintptr_t offset = AlignForward(curr_ptr, align);
  offset -= (uintptr_t)arena->data;

  if (offset + size <= arena->capacity)
  {
    void *ptr = &arena->data[offset];
    arena->prev = offset;
    arena->cur = offset + size;
    memset(ptr, 0, size);
    return ptr;
  }
  else
  {
    return NULL;
  }
}

#define Alloc(arena, size) AllocAlign(arena, size, DEFAULT_ALIGNMENT)

void InitArena(Arena *arena, void *backBuffer, size_t backBufferLength)
{
  arena->data = (uc *)backBuffer;
  arena->capacity = backBufferLength;
  arena->cur = 0;
  arena->prev = 0;
}

void Destroy(Arena *arena)
{
  memset(arena->data, 0, arena->capacity);
  arena->cur = 0;
  arena->prev = 0;
}

void TmpBegin(TmpArena *tmp, Arena *src)
{
  tmp->arena = src;
  tmp->prev = src->prev;
  tmp->cur = src->cur;
}

void TmpEnd(TmpArena *tmp)
{
  tmp->arena->prev = tmp->prev;
  tmp->arena->cur = tmp->cur;
}
