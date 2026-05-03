#ifndef __LIST_TEMP_H
#define __LIST_TEMP_H

/*----------------------------- Public Includes ------------------------------*/
#include <stddef.h>
#include <stdlib.h>
/*--------------------------- Public Includes END ----------------------------*/

/*------------------------------ Public Strcut -------------------------------*/
struct list_node_i
{
   struct list_node_i *next;
   int value;
};
struct list_i
{
   struct list_node *head;
   size_t size;
};
/*---------------------------- Public Strcut END -----------------------------*/

/*----------------------------- Public Typedefs ------------------------------*/
typedef struct list_i *List_i;
typedef struct list_node_i *List_Node_i;
/*--------------------------- Public Typedefs END ----------------------------*/

/*------------------------ Public Function Prototypes ------------------------*/
List_i list_i_init(void);
void list_i_destroy(List_i this);
int list_i_push(List_i this, const void *restrict elem);
void list_i_pop(List_i this);
List_Node_i *list_i_head(List_i this);
size_t list_i_size(List_i this);
int list_i_insert (List_i this, List_Node_i *pos,
                 const void *restrict first, size_t n);
void list_i_erase(List_i this, List_Node_i *pos, size_t n);
struct list_i_node **list_i_find
(struct list_i *this, struct list_i_node **pos, const void *val,
 int(*cmp)(const void*, const void*));
/*---------------------- Public Function Prototypes END ----------------------*/

#endif
