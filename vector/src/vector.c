#include "../vector.h"
#include <stdlib.h>
#include <string.h>


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
static inline
int vec_realloc(struct vector *self, size_t new_cap)
{
   void *new_beg;

   if (new_cap == 0)
    {
      free(self->beg);
      self->beg = self->cav = self->end = NULL;
    }

   new_beg = realloc(self->beg, new_cap);
   if (new_beg == NULL) return 1;
   self->cav = (char*)new_beg + _vec_len_bytes(self);
   self->end = (char*)new_beg + new_cap;
   self->beg = new_beg;

   return 0;
}


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


static inline
int vec_shrink(struct vector *self)
{
   size_t min_bytes = _vec_len_bytes(self),
          actual_bytes = _vec_cap_bytes(self);

   while (actual_bytes / 4 >= min_bytes && actual_bytes)
      actual_bytes /= 2;

   return vec_realloc(self, actual_bytes);
}


static inline
_Bool vec_can_shrink(struct vector *self)
{ return _vec_cap_bytes(self) / 4 >= _vec_len_bytes(self); }
/*-------------------------- Private Fucntions END ---------------------------*/
