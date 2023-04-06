/*************************************************
 * FILE START
*************************************************/
#ifndef FREELIST_H_
#define FREELIST_H_
/*************************************************
 * INCLUDE STATEMENTS
*************************************************/
#include "common.h"
/*************************************************
 * TYPE DEFINITIONS
*************************************************/
#ifndef DEBUG_1
typedef enum {
    NODE_FREELIST_TYPE = 0,
    NODE_TASK_TYPE
} Node_Types;
#endif

typedef struct FL_Node
{
    struct FL_Node *next;
    struct FL_Node *prev;
    void * current;
#ifndef DEBUG_1
    U8 node_type;
#endif
} FL_Node_t;

/*************************************************
 * GLOBAL VARIABLES
*************************************************/

/*************************************************
 * FUNCTION HEADERS
*************************************************/

/**
 * @brief Insert a node prior to the head
 *
 * @param head Reference node to insert the other parameter before
 * @param node Node you will be inserting
 * @return void*
 */
void *FL_Insert(FL_Node_t **head, FL_Node_t *node);

/**
 * @brief Insert node after the head node provided
 *
 * @param head Reference node to insert after
 * @param node Node you will be inserting
 */
void * FL_Insert_After(FL_Node_t ** head, FL_Node_t * node);

/**
 * @brief Insert a node at the back of the linked list where head is the front
 *
 * @param head Front of the linked list you wish to insert to
 * @param node Node that will be inserted
 * @return uint8_t 0 if succesful 1 if error
 */
void * FL_Insert_Back(FL_Node_t ** head, FL_Node_t * node);
/**
 * @brief Remove a given node from a list
 *
 * @param head Head of the list you want to remove from
 * @param node Node you wish to remove from the list
 * @return void*
 */
void *FL_Remove(FL_Node_t **head, FL_Node_t *node);

/**
 * @brief Insert a node into a sorted free list
 * @param head
 * @param node
 * @param compare Returns 1 if the node should be inserted after the node we are comparing to
 * @return
 */
void *FL_Sorted_Insert(FL_Node_t **head, FL_Node_t *node, void *(compare)(void *, void *));

void *FL_Pop(FL_Node_t **head);

int FL_Print(FL_Node_t *head, unsigned int size);

#endif
