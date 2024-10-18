#define KB 1024LL
#define MB KB*1000
#define GB MB*1000

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

typedef struct Memory {
  size_t cur;
  size_t prev;
  size_t capacity;
  uc *data;
} Memory;

typedef struct TmpMemory {
  size_t prev;
  size_t cur;
  Memory *memory;
} TmpMemory;

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
  if (modulo != 0)
    p += a - modulo;
  return p;
}

void *MemAllocAlign(Memory *memory, size_t size, size_t align)
{
  uintptr_t curr_ptr = (uintptr_t)memory->data + (uintptr_t)memory->cur;
  uintptr_t offset = AlignForward(curr_ptr, align);
  offset -= (uintptr_t)memory->data;

  if (offset + size <= memory->capacity)
  {
    void *ptr = &memory->data[offset];
    memory->prev = offset;
    memory->cur = offset + size;
    memset(ptr, 0, size);
    return ptr;
  }
  else
  {
    return NULL;
  }
}

void *MemAlloc(Memory *memory, size_t size) 
{
  return MemAllocAlign(memory, size, DEFAULT_ALIGNMENT);
}

void MemInit(Memory *memory, void *backBuffer, size_t backBufferLength)
{
  memory->data = (uc *)backBuffer;
  memory->capacity = backBufferLength;
  memory->cur = 0;
  memory->prev = 0;
}

void MemDestroy(Memory *memory)
{
  memset(memory->data, 0, memory->capacity);
  memory->cur = 0;
  memory->prev = 0;
}

void MemTmpBegin(TmpMemory *tmp, Memory *src)
{
  tmp->memory = src;
  tmp->prev = src->prev;
  tmp->cur = src->cur;
}

void MemTmpEnd(TmpMemory *tmp)
{
  memset(tmp->memory->data + tmp->cur, 0, tmp->memory->cur - tmp->cur);
  tmp->memory->prev = tmp->prev;
  tmp->memory->cur = tmp->cur;
}
