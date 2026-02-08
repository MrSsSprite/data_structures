#include "../vector.h"
#include <stdlib.h>
#include <string.h>
#include "../../compat/bit_ops.h"


//TEMP
#include <stdio.h>


/*----------------------------- Private Structs ------------------------------*/
struct vector
{
   void *beg, *cav, *end;
   const size_t elem_sz;
};
/*--------------------------- Private Structs END ----------------------------*/


/*------------------------------ Private Macros ------------------------------*/
#define _vec_len_bytes(self) ((char*)(self)->cav - (char*)(self)->beg)
#define _vec_cap_bytes(self) ((char*)(self)->end - (char*)(self)->beg)
/*---------------------------- Private Macros END ----------------------------*/


/*----------------------- Private Function Declaration -----------------------*/
static inline int vec_realloc(struct vector *self, size_t new_cap);
static inline int vec_grow(struct vector *self, size_t size_min);
static inline int vec_shrink(struct vector *self);
static inline _Bool vec_can_shrink(struct vector *self);
/*--------------------- Private Function Declaration END ---------------------*/


/*----------------------------- Public Functions -----------------------------*/
struct vector *vec_new(size_t elem_size)
{
   struct vector *ret = malloc(sizeof(struct vector));

   if (ret == NULL) return NULL;
   ret->beg = ret->cav = ret->end = NULL;
   *(size_t*)&ret->elem_sz = elem_size;

   return ret;
}


void vec_free(struct vector *self)
{
   free(self->beg);
   free(self);
}


int vec_resize(struct vector *self, size_t size)
{
   size_t cap = vec_cap(self);

   if (cap < size) vec_grow(self, size);
   //TODO
}


int vec_push(struct vector *self, const void *elem)
{
   // if not enough capacity
   if (self->cav == self->end && vec_grow(self, vec_len(self) + 1))
      return 1;

   memcpy(self->cav, elem, self->elem_sz);
   self->cav = (char*)self->cav + self->elem_sz;
   return 0;
}


int vec_pop(struct vector *self)
{
   if (self->cav == self->beg) return -1;

   self->cav = (char*)self->cav - self->elem_sz;
   if (vec_can_shrink(self) && vec_shrink(self))
      return -1;

   return 0;
}


int vec_insert
 (struct vector *self, void *dst, const void *restrict src, size_t cnt)
{
   size_t tail_sz = (char*)self->cav - (char*)dst,
          cnt_bytes = cnt * self->elem_sz;
   if ((size_t)((char*)self->end - (char*)self->cav) < cnt * self->elem_sz)
    {
      size_t dst_offset = (char*)dst - (char*)self->beg;
      if (vec_grow(self, vec_len(self) + cnt))
         return 1;
      dst = (char*)self->beg + dst_offset;
    }

   memmove((char*)dst + cnt_bytes, dst, tail_sz);
   memcpy(dst, src, cnt_bytes);
   self->cav = (char*)self->cav + cnt_bytes;

   return 0;
}


int vec_erase(struct vector *self, void *pos, size_t cnt)
{
   size_t cnt_bytes = cnt * self->elem_sz;
   void *keep = (char*)pos + cnt_bytes;

   memmove(pos, keep, (char*)self->cav - (char*)keep);
   if (vec_can_shrink(self) && vec_shrink(self))
      return -1;
   self->cav = (char*)self->cav - cnt_bytes;

   return 0;
}


void *vec_arr(struct vector *self)
{ return self->beg; }


size_t vec_len(struct vector *self)
{ return _vec_len_bytes(self) / self->elem_sz; }


size_t vec_cap(struct vector *self)
{ return _vec_cap_bytes(self) / self->elem_sz; }


_Bool vec_is_empty(struct vector *self)
{ return self->cav == self->beg; }
/*--------------------------- Public Functions END ---------------------------*/


/*---------------------------- Private Fucntions -----------------------------*/
/**
 * @brief   Reallocates the internal buffer to a specific size in bytes.
 *
 * @param[in,out] self     The vector instance.
 * @param[in]     new_cap  The new capacity in **bytes**.
 * @return  0 on success.
 * @reval   1 if memory allocation failed; the original buffer remains intact.
 *
 * @note This is a low-level memory management function. It directly wraps the
 *       standard `realloc` and updates the internal pointers of the vector.
 * 
 * @note If @p new_cap is 0, the buffer is freed and all internal pointers
 *       (@c beg, @c cav, @c end) are set to @p NULL.
 *
 * @warning Unlike @ref vec_grow, this function expects @p new_cap in
 *          **bytes**, not element count. The caller is responsible for 
 *          calculating `count * elem_sz`.
 * @sa   vec_grow
 */
static inline
int vec_realloc(struct vector *self, size_t new_cap)
{
   void *new_beg;

   if (new_cap == 0)
    {
      free(self->beg);
      self->beg = self->cav = self->end = NULL;
      return 0;
    }

   new_beg = realloc(self->beg, new_cap);
   if (new_beg == NULL) return 1;
   self->cav = (char*)new_beg + _vec_len_bytes(self);
   self->end = (char*)new_beg + new_cap;
   self->beg = new_beg;

   return 0;
}


/**
 * @brief   Ensures the vector has enough capacity for at least @p size_min 
 *          elements.
 *
 * @param[in,out] self     The vector instance to grow
 * @param[in]     size_min The minimum # of elements to guarantee space for.
 * @return     0 on success.
 * @reval      non-0 Failure code returned by @ref vec_realloc if memory
 *             allocation fails.
 *
 * @note This function uses an exponential growth strategy (doubling). This
 *       ensures that $n$ insertions result in $O(1)$ amortized time
 *       complexity.
 *
 * @attention  If @p size_min is less than the current capacity, the function
 *             may still perform a reallocation depending on the internal state
 *             of @ref vec_realloc.
 *
 * @warning This function does not check for integer overflow if
 *          $size\_min \times elem\_size$ exceeds `SIZE_MAX`. Ensure
 *          @p size_min is validated before calling this function.
 */
static inline
int vec_grow(struct vector *self, size_t size_min)
{
   size_t min_bytes = size_min * self->elem_sz,
          actual_bytes = _vec_cap_bytes(self);

   if (actual_bytes == 0) actual_bytes = self->elem_sz;
   while (actual_bytes < min_bytes)
      actual_bytes *= 2;

   return vec_realloc(self, actual_bytes);
}


/**
 * @brief   Reduces the vector's capacity when it is significantly underutilized.
 *
 * @param[in,out] self  The vector instance to shrink.
 * @return     0 on success.
 * @reval      non-0 Failure code returned by @ref vec_realloc if memory
 *             reallocation fails.
 *
 * @note This function targets a capacity of 2x the current length (rounded up
 *       to the next power of 2). This provides a balance between memory usage
 *       and preventing frequent reallocations if elements are added again.
 *
 * @note If the vector is empty (length == 0), all internal memory is freed
 *       and pointers are set to @p NULL.
 *
 * @attention This function will never increase the vector's capacity. If the
 *             calculated target capacity is >= the current capacity, the
 *             function returns 0 without modifying the vector.
 *
 * @sa vec_can_shrink
 * @sa vec_grow
 */
static inline
int vec_shrink(struct vector *self)
{
   size_t min_bytes = _vec_len_bytes(self) * 2, new_cap;

   if (min_bytes == 0) return vec_realloc(self, 0);

   new_cap = (size_t)1 << (sizeof(size_t) * 8 - ds_clz(size_t, min_bytes - 1));
   // Make sure it doesn't grow
   if (new_cap >= _vec_cap_bytes(self)) return 0;

   return vec_realloc(self, new_cap);
}


static inline
_Bool vec_can_shrink(struct vector *self)
{ return  _vec_len_bytes(self) <= _vec_cap_bytes(self) / 4; }
/*-------------------------- Private Fucntions END ---------------------------*/
