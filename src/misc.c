// Faster strcmp (1.6x) by Markus Grönholm 
// @ https://mgronhol.github.io/fast-strcmp/
s32 fast_strcmp(const char *ptr0, const char *ptr1, s32 len)
{
  s32 fast = len/sizeof(size_t) + 1;
  s32 offset = (fast-1)*sizeof(size_t);
  s32 current_block = 0;

  if (len <= sizeof(size_t)) fast = 0;


  size_t *lptr0 = (size_t*)ptr0;
  size_t *lptr1 = (size_t*)ptr1;

  while (current_block < fast)
  {
    if (lptr0[current_block] ^ lptr1[current_block])
    {
      s32 pos;
      for (pos = current_block*sizeof(size_t); pos < len ; ++pos)
      {
        if ((ptr0[pos] ^ ptr1[pos]) || (ptr0[pos] == 0) || (ptr1[pos] == 0))
          return  (s32)((uc)ptr0[pos] - (uc)ptr1[pos]);
      }
    }

    ++current_block;
  }

  while (len > offset)
  {
    if (ptr0[offset] ^ ptr1[offset])
      return (s32)((uc)ptr0[offset] - (uc)ptr1[offset]); 
    ++offset;
  }
	
   return 0;
}
