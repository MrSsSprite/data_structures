#ifndef __BIT_OPS_H
#define __BIT_OPS_H

#include <stddef.h>

/*------------------------ CLZ (Count Leading Zeros) -------------------------*/
#if defined(__GNUC__) || defined(__clang__)
#define ds_clz(x) _Generic((x), \
    unsigned char:      __builtin_clz(x) - (int)((sizeof(unsigned int) - sizeof(unsigned char)) * 8), \
    signed char:        __builtin_clz((unsigned char)(x)) - (int)((sizeof(unsigned int) - sizeof(char)) * 8), \
    unsigned short:     __builtin_clz(x) - (int)((sizeof(unsigned int) - sizeof(unsigned short)) * 8), \
    short:              __builtin_clz((unsigned short)(x)) - (int)((sizeof(unsigned int) - sizeof(short)) * 8), \
    unsigned int:       __builtin_clz(x), \
    int:                __builtin_clz((unsigned int)(x)), \
    unsigned long:      __builtin_clzl(x), \
    long:               __builtin_clzl((unsigned long)(x)), \
    unsigned long long: __builtin_clzll(x), \
    long long:          __builtin_clzll((unsigned long long)(x)), \
    default:            ds_clz_fallback((unsigned long long)(x), sizeof(x)) \
)
#else
#define ds_clz(x) ds_clz_fallback((unsigned long long)(x), sizeof(x))
#endif

// Fallback function with fixed loop boundaries
static inline int ds_clz_fallback(unsigned long long x, size_t type_size)
{
   int total_bits = type_size * 8;
   if (x == 0) return total_bits;

   int cnt = 0;
   for (int i = total_bits - 1; i >= 0; i--) {
      if ((x >> i) & 1) break;
      cnt++;
   }
   return cnt;
}
/*---------------------- CLZ (Count Leading Zeros) END -----------------------*/


#endif
