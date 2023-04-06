/*************************************************
 * FILE START
 *************************************************/
#ifndef LIST_OLD_H_
#define LIST_OLD_H_
/*************************************************
 * INCLUDES
 *************************************************/
#include "common.h"

/*************************************************
 * PUBLIC TYPEDEFS
 *************************************************/
typedef struct LL_Node
{
    LL_Node_t *prev;
    LL_Node_t *curr;
    LL_Node_t *next;
} LL_Node_t;

/*************************************************
 FUNCTION HEADERS
*************************************************/
/**
 * @brief Inserts a node into the front of the linked list
 *
 * @param head - Head of the linked list
 * @param node - Node you want to insert
 * @return LL_Node_t * - The node you have inserted if the insertion was succesful
 */
LL_Node_t *LL_Insert_Front(LL_Node_t **head, LL_Node_t *node);

/**
 * @brief Inserts a node into the back of the linked list
 *
 * @param head - Head of the linked list
 * @param node - Node you want to insert
 * @return LL_Node_t * - The node you have inserted if the insertion was succesful
 */
LL_Node_t *LL_Insert_Back(LL_Node_t **head, LL_Node_t *node);

/**
 * @brief Insert a node after the node specified
 *
 * @param curr_node - The reference node you will be inserting after
 * @param insertion_node - The node you will be inserting into the list
 * @return LL_Node_t* - The node you have inserted
 */
LL_Node_t *LL_Insert_After(LL_Node_t *curr_node, LL_Node_t *insertion_node);

/**
 * @brief Insert a node before the node specified
 *
 * @param curr_node - The reference node you will be inserting before
 * @param insertion_node - The node you will be inserting into the list
 * @return LL_Node_t* - The node you have inserted
 */
LL_Node_t *LL_Insert_Before(LL_Node_t *curr_node, LL_Node_t *insertion_node);

/**
 * @brief Removes any node from within the linked list
 *
 * @param head - Head of the linked list you will be removing from
 * @param node - The node you will be inserting
 * @return LL_Node_t* - The node you have removed from the list
 */
LL_Node_t *LL_Remove_Node(LL_Node_t **head, LL_Node_t *node);

/**
 * @brief Removes the head of the linked list
 *
 * @param head - Head of the linked list you will be removing from
 * @return LL_Node_t* - The node you have removed from the list
 */
LL_Node_t *LL_Remove_Front(LL_Node_t **head);

/**
 * @brief Removes the tail of the linked list of which the head has been provided
 *
 * @param head - Head of the linked list you will be removing from
 * @return LL_Node_t* - The node you have removed from the list
 */
LL_Node_t *LL_Remove_Back(LL_Node_t **head);

LL_Node_t *insert(LL_Node_t *head, LL_Node_t *node);
LL_Node_t *remove_n(LL_Node_t **head, LL_Node_t *node); // returning the address
LL_Node_t *pop(LL_Node_t *head);
#endif

/*************************************************
 * END OF FILE
 *************************************************/
