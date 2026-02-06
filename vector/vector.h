#ifndef VECTOR_H
#define VECTOR_H

/*----------------------------- Public Includes ------------------------------*/
#include <stddef.h>
/*--------------------------- Public Includes END ----------------------------*/

/*------------------------------ Public Structs ------------------------------*/
struct vector;
/*---------------------------- Public Structs END ----------------------------*/


/*----------------------------- Public Functions -----------------------------*/
struct vector *vec_new(size_t elem_size);
void vec_free(struct vector *self);

int vec_resize(struct vector *self, size_t size);
int vec_push(struct vector *self, const void *elem);
int vec_pop(struct vector *self);
int vec_insert
 (struct vector *self, void *dst, const void *restrict src, size_t cnt);
int vec_erase(struct vector *self, void *pos, size_t cnt);

void *vec_arr(struct vector *self);
size_t vec_len(struct vector *self);
size_t vec_cap(struct vector *self);
_Bool vec_is_empty(struct vector *self);
/*--------------------------- Public Functions END ---------------------------*/


/*-------------------------------- Templates ---------------------------------*/
//TODO
#include <stdlib.h>
#include <string.h>
#define DECLARE_VEC(T) \
   typedef struct vec_##T Vec##T;
/*------------------------------ Templates END -------------------------------*/

#endif
