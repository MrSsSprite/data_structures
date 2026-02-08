#ifndef __BIT_OPS_H
#define __BIT_OPS_H

#include <stddef.h>

static inline
int ds_clz_size_t(size_t x)
{
#if defined(__GNUC__) || defined(__clang__)
   if (sizeof(size_t) == sizeof(unsigned long))
      return __builtin_clzl((unsigned long)x);
   if (sizeof(size_t) == sizeof(unsigned long long))
      return __builtin_clzll((unsigned long long)x);
   if (sizeof(size_t) == sizeof(unsigned int))
      return __builtin_clz((unsigned int)x);
#else
   int count = 0, total_bits = sizeof(size_t) * 8;
   for (int i = total_bits - 1; i; i--)
    {
      if ((x >> i) & 1) break;
      count++;
    }
   return count;
#endif
}

#define DEFINE_DS_CLZ(T) \
   static inline int ds_clz_##T ( T x) \
   {


/*------------------------ CLZ (Count Leading Zeros) -------------------------*/
#if defined(__GNUC__) || defined(__clang__)
   #define ds_clz(T, x) \
      ((sizeof(T) == sizeof(unsigned long)) ? \
         __builtin_clzl((unsigned long)(x)) : \
       (sizeof(T) == sizeof(unsigned long long)) ? \
         __builtin_clzll((unsigned long long)(x)) : \
         __builtin_clz((unsigned int)(x)))
#else
   #define ds_clz(T, x) ds_clz_fallback(x, sizeof(T))
#endif

static inline int ds_clz_fallback(unsigned long long x, size_t type_size)
{
   int cnt, i;
   for (i = type_size * 8 - 1, cnt = 0; i; i--, cnt++)
      if ((x >> i) & 1) break;
   return cnt;
}
/*---------------------- CLZ (Count Leading Zeros) END -----------------------*/


#endif
