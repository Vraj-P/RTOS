#ifndef FREELIST_H_
#define FREELIST_H_

#include "common.h"
#include "printf.h"

typedef struct FL_Node
{
    struct FL_Node *next;
    struct FL_Node *prev;
} FL_Node_t;

void *FL_Insert(FL_Node_t **head, FL_Node_t *node);
void *FL_Remove(FL_Node_t **head, FL_Node_t *node);
void *FL_Pop(FL_Node_t **head);
int FL_Print(FL_Node_t *head, unsigned int size);


#endif
