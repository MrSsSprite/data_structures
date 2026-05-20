#ifndef __LIST_TEMP_H
#define __LIST_TEMP_H

/*----------------------------- Public Includes ------------------------------*/
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "../wrapper_fn/cstdlib.h"
/*--------------------------- Public Includes END ----------------------------*/

#define List__declare(T, S) \
struct list_node__##S \
{ \
   struct list_node__##S *next; \
   T value; \
}; \
typedef struct list__##S *List__##S; \
typedef struct list_node__##S *List_Node__##S; \
List__##S list__##S##_init(void); \
void list__##S##_deinit(struct list__##S *self); \
int list__##S##_push(struct list__##S *self, T val); \
void list__##S##_pop(struct list__##S *self); \
struct list_node__##S **list__##S##_head(struct list__##S *self); \
size_t list__##S##_size(struct list__##S *self); \
int list__##S##_insert(struct list__##S *self, struct list_node__##S **pos, \
                  const T *restrict first, size_t n); \
void list__##S##_erase(struct list__##S *self, struct list_node__##S **pos, size_t n); \
struct list_node__##S **list__##S##_find \
 (struct list_node__##S **pos, T val, T(*cmp)(const void*, const void*)); \

#define List__define(T, S) \
struct list_node__##S; \
struct list_node__##S \
{ \
   struct list_node__##S *next; \
   T value; \
}; \
struct list__##S \
{ \
   struct list_node__##S *head; \
   size_t size; \
}; \
typedef struct list__##S *List__##S; \
typedef struct list_node__##S *List_Node__##S; \
List__##S list__##S##_init(void) \
{ \
   struct list__##S *self = malloc(sizeof(struct list__##S)); \
   if (self == NULL) \
      return NULL; \
   self->size = 0; \
   self->head = NULL; \
   return self; \
} \
void list__##S##_deinit(struct list__##S *self) \
{ \
   struct list_node__##S *iter[2]; \
   uint_fast8_t i = 0; \
   if (self == NULL) return; \
   iter[i] = self->head; \
   while (iter[i]) \
   { \
      iter[i ^ 1] = iter[i]->next; \
      free(iter[i]); \
      i ^= 1; \
   } \
   free(self); \
} \
/**
 * @brief   push an element to the head of the list
 * @param   elem  element to be pushed
 * @return  0 on success; non-0 otherwise
 * @retval  1  malloc failure
 */ \
int list__##S##_push(struct list__##S *self, T val) \
{ \
   struct list_node__##S *new_node = malloc(sizeof(struct list_node__##S)); \
   if (new_node == NULL) return 1; \
   new_node->next = self->head; \
   new_node->value = val; \
   self->head = new_node; \
   self->size++; \
   return 0; \
} \
void list__##S##_pop(struct list__##S *self) \
{ \
   struct list_node__##S *det = self->head; \
   self->head = det->next; \
   self->size--; \
   free(det); \
} \
struct list_node__##S **list__##S##_head(struct list__##S *self) \
{ return &self->head; } \
size_t list__##S##_size(struct list__##S *self) \
{ return self->size; } \
/**
 * @brief   Insert node(s) into list
 *
 * @param   self  target list structure.
 * @param   pos   address of @c next member of the node after which the
 *                elements are inserted.
 * @param   first pointer to the first element in the source array.
 * @param   n     number of elements to copy
 *
 * @return  status code
 * @retval  0  success
 * @retval  1  malloc failure
 *
 * @note On error, list is returned back to the state before function call.
 */ \
int list__##S##_insert(struct list__##S *self, struct list_node__##S **pos, \
                  const T *restrict first, size_t n) \
{ \
   struct list_node__##S **iter = pos; \
   struct list_node__##S *new_node, *orig_next = *iter; \
   for (const T *ed = first + n; \
        first < ed; first++, iter = &(*iter)->next) \
   { \
      new_node = malloc(sizeof(struct list_node__##S)); \
      *iter = new_node; \
      if (new_node == NULL) \
      { \
         /* remove every node after preceder but before orig_next */ \
         struct list_node__##S *err_iter; \
         new_node = *pos; \
         while ((err_iter = new_node)) \
         { \
            new_node = err_iter->next; \
            free(err_iter); \
         } \
         *pos = orig_next; \
         return 1; \
      } \
      new_node->value = *first; \
   } \
   *iter = orig_next; \
   self->size += n; \
   return 0; \
} \
void list__##S##_erase(struct list__##S *self, struct list_node__##S **pos, size_t n) \
{ \
   struct list_node__##S *del_node = *pos, *next_node; \
   for (size_t i = 0; i < n; i++) \
   { \
      next_node = del_node->next; \
      free(del_node); \
      del_node = next_node; \
   } \
   /* Do not use next_node as it's not initialized if n == 0 */ \
   *pos = del_node; \
   self->size -= n; \
} \
/**
 * @brief   find the node with value that euqals val
 *
 * @param   pos   position indicator. the function start searching at @c *pos
 *                (i.e., all node(s) before *pos are neglected) It's either
 *                @c &this->head or @c &node->next .
 * @param   val   target value
 * @param   cmp   compare function. @c &val and the address of value member in
 *                a node is passed to the function. The function is expected to
 *                return 0 iff two parameters equals. If NULL is passed, memcmp
 *                is used.
 * @return  position indicator indicating the first matching node on success.
 * @retval  NULL  Not Found
 */ \
struct list_node__##S **list__##S##_find \
 (struct list_node__##S **pos, T val, int(*cmp)(const void*, const void*)) \
{ \
   if (cmp == NULL) \
   { \
      _wrapper_n = sizeof(T); \
      cmp = _wpr_memcmp_count; \
   } \
   for (; *pos; pos = &(*pos)->next) \
      if (cmp(&val, &(*pos)->value) == 0) \
         return pos; \
   return NULL; \
}

#endif
