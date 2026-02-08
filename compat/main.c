#include <stdio.h>
#include "bit_ops.h"


int main(void)
{
   unsigned int arr[] = { 0x1, 0x5, 0x4 };
   unsigned long long arr1[] = { 0x1, 0x5, 0x4 };

   for (int i = 0, ed = sizeof(arr) / sizeof(*arr); i < ed; i++)
      printf("%d: %d\n", arr[i], ds_clz(unsigned int, arr[i]));
   
   for (int i = 0, ed = sizeof(arr) / sizeof(*arr); i < ed; i++)
      printf("%d: %d\n", arr[i], ds_clz(unsigned long long, arr1[i]));

   return 0;
}
