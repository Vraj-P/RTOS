/*************************************************
 * FILE START
 *************************************************/
/*************************************************
 * INCLUDES
 *************************************************/
#include "list.old.h"

//#include "common.h"

/*************************************************
 FUNCTION DEFINITIONS
*************************************************/
/**
 * @brief Removes any node from within the linked list
 *
 * @param head - Head of the linked list you will be removing from
 * @param node - The node you will be inserting
 * @return LL_Node_t* - The node you have removed from the list
 */
LL_Node_t *LL_Remove_Node(LL_Node_t **head, LL_Node_t *node)
{
    if (head == NULL || node == NULL)
        return NULL;

    if (node->prev != NULL)
    {
        node->prev->next = node->next; // for getting rid of current nodes previous conneciton
    }
    else
    { // currentis the head
        *head = node->next;
    }

    if (node->next != NULL)
    {
        node->next->prev = node->prev; // for getting rid of current nodes next connection
    }

    node->next = NULL;
    node->prev = NULL;

    return node;
}

/**
 * @brief Removes the head of the linked list
 *
 * @param head - Head of the linked list you will be removing from
 * @return LL_Node_t* - The node you have removed from the list
 */
LL_Node_t *LL_Remove_Front(LL_Node_t **head)
{
    // Remove the head from the list
    return LL_Remove_Node(head, *head);
}

/**
 * @brief Removes the tail of the linked list of which the head has been provided
 *
 * @param head - Head of the linked list you will be removing from
 * @return LL_Node_t* - The node you have removed from the list
 */
LL_Node_t *LL_Remove_Back(LL_Node_t **head)
{
    LL_Node_t *tail = *head;

    // Find tail in the linked list
    while (tail->next != NULL)
    {
        tail = tail->next;
    }

    // Remove the tail from the linked list
    return LL_Remove_Node(head, tail);
}

LL_Node_t *insert(LL_Node_t *head, LL_Node_t *LL_Node)
{
    if (head == NULL)
    { // when initializing arrya initialize evrythign to NULL
        LL_Node->next = NULL;
        LL_Node->prev = NULL;
        return LL_Node; // next iteration LL_Node is head
    }

    LL_Node_t *current = head;

    while (current->next != NULL && current < LL_Node)
    { // is there a memory afer thats free and if the current is before the LL_Node we wanna add
        current = current->next;
    }

    if (current->next == NULL && current < LL_Node)
    { // adding after current
        LL_Node->next = NULL;
        LL_Node->prev = current;
        current->next = LL_Node;
        return head; //
    }

    LL_Node->next = current; // adding before current
    LL_Node->prev = current->prev;

    if (current->prev != NULL)
    {
        current->prev->next = LL_Node; // adding in middle of list
    }
    else
    {
        head = LL_Node; // the one before current is null
    }

    current->prev = LL_Node;
    return head;
}

LL_Node_t *remove_n(LL_Node_t **head, LL_Node_t *node)
{
    if (*head == NULL)
    {
        return *head;
    }

    LL_Node_t *current = *head;

    // Find node in the linked list
    while (current != NULL && current != node)
    {
        current = current->next;
    }

    // node was not found in the linked list
    if (current == NULL)
    {
        return NULL;
    }

    if (current->prev != NULL)
    {
        current->prev->next = current->next; // for getting rid of current nodes previous conneciton
    }
    else
    { // currentis the head
        *head = current->next;
    }

    if (current->next != NULL)
    {
        current->next->prev = current->prev; // for getting rid of current nodes next connection
    }

    current->next = NULL;
    current->prev = NULL;

    return current;

} // returning the address

LL_Node_t *pop(LL_Node_t *head)
{
    return remove_n(head, head);
}
