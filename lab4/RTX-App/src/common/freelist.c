/*************************************************
 * FILE START
*************************************************/
/*************************************************
 * INCLUDE STATEMENTS
*************************************************/

#include "freelist.h"
// #include <stdlib.h>
// #include <stdio.h>

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
/**
 * @brief Insert node after the head node provided
 *
 * @param head Reference node to insert after
 * @param node Node you will be inserting
 * @return uint8_t 0 if succesful, 1 if error
 */
void * FL_Insert_After(FL_Node_t ** head, FL_Node_t * node)
{
    if (!node || !head)
			return (void *)1;


    // Remove any connections the node already has
    node->next = NULL;
    node->prev = NULL;

    // Head is empty
    if ( *head == NULL )
    {
        node->next = NULL;
        node->prev = NULL;
        *head = node;
    }
        // Head == tail
    else if ( (*head)->next == NULL )
    {
        node->prev = *head;
        node->next = NULL;
        (*head)->next = node;
    }
        // Head has at least on forward connection
    else
    {
        // Join the node to the conneciton between head and the next element
        node->next = (*head)->next;
        node->prev = *head;

        // Replace the current head's connection with a conneciton to node
        ((*head)->next)->prev = node;
        (*head)->next = node;
    }
    return (void *)0;
}

/**
 * @brief Insert a node at the back of the linked list where head is the front
 *
 * @param head Front of the linked list you wish to insert to
 * @param node Node that will be inserted
 * @return uint8_t 0 if succesful 1 if error
 */
void * FL_Insert_Back(FL_Node_t ** head, FL_Node_t * node)
{
    // We cannot insert when we do not have a place to insert or something to insert
    if (node == NULL || head == NULL)
        return (void *)1;

    // Traverse the list to find the back
    FL_Node_t * temp = *head;
		
		// Case list is empty
		if ( temp == NULL )
			return FL_Insert(head, node);
		
    while (temp->next != NULL)
    {
        temp = temp->next;

        // Check if we have accidentally created a loop
        if ( temp == (*head) || temp->next == temp )
            return (void *)1; 
    }

    // Use the back as a reference node and insert node after it
    return FL_Insert_After(&temp, node);

}
/**
 * @brief Remove a given node from a list
 *
 * @param head Head of the list you want to remove from
 * @param node Node you wish to remove from the list
 * @return void*
 */

void *FL_Remove(FL_Node_t **head, FL_Node_t *node)
{
    if (!node)
        return (void *)1;

    if (node->prev)
        node->prev->next = node->next;
    else
        *head = node->next;

    if (node->next)
        node->next->prev = node->prev;

    node->next = NULL;
    node->prev = NULL;

    return NULL;
}

void *FL_Sorted_Insert(FL_Node_t **head, FL_Node_t *node, void * (*compare)(void *, void *))
{
    FL_Node_t * current = *head;

    // If the list is null, insert the node at the head
    if ( current == NULL )
        return FL_Insert(head, node);
		
		// Check if we should insert before the head
		if ( current->next == NULL && compare(node->current, current->current) )
			return FL_Insert(head, node);
		
		// Insert after the head in a sorted order
    while ( current->next != NULL )
    {
        // If the node should be inserted after current, break
        if ( compare( node->current, current->next->current ) != NULL )
            break;

        // Move current over
        current = current->next;
    }

    // Insert after the node that we found (either at the end of the list or after a true compare() call)
    return FL_Insert_After(&current, node);
}
void *FL_Pop(FL_Node_t **head)
{
    return FL_Remove(head, *head);
}

int FL_Print(FL_Node_t *head, unsigned int size) 
{   
    FL_Node_t *current = head;
    int total_blocks = 0;

    while (current) 
    {
        //printf("0x%x: 0x%x\r\n", current, size);
        ++total_blocks;
        current = current->next;
    }

    return total_blocks;
}


/*************************************************
 * END OF FILE
*************************************************/

