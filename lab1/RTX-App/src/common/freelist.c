#include "freelist.h"
// #include <stdlib.h>
// #include <stdio.h>


void *FL_Insert(FL_Node_t **head, FL_Node_t *node)
{
    if (!node)
        return NULL;

    node->next = *head;
    node->prev = NULL;

    if (*head)
        (*head)->prev = node;

    *head = node;
    
    return (void *)1;
}

void *FL_Remove(FL_Node_t **head, FL_Node_t *node)
{
    if (!node)
        return NULL;

    if (node->prev)
        node->prev->next = node->next;
    else
        *head = node->next;

    if (node->next)
        node->next->prev = node->prev;

    node->next = NULL;
    node->prev = NULL;

    return (void *)1;
}

void *FL_Pop(FL_Node_t **head) // static inline replaces the function with whats inside
{
    return FL_Remove(head, *head);
}

int FL_Print(FL_Node_t *head, unsigned int size) 
{   
    FL_Node_t *current = head;
    int total_blocks = 0;

    while (current) 
    {
        printf("0x%x: 0x%x\r\n", current, size);
        ++total_blocks;
        current = current->next;
    }

    return total_blocks;
}
