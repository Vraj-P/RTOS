/*************************************************
 * FILE START
 *************************************************/
#ifndef __BIN_TREE__
#define __BIN_TREE__
// #include "common.h"
#include "k_stdint.h"
#include "math.h"

/*************************************************
 * PUBLIC MACRO DEFINITIONS
 *************************************************/
/*************************************************
 * PUBLIC TYPEDEFS
 *************************************************/
typedef struct BBit_Tree
{
    // Number of bits used to represent a node in the binary tree
    uint32_t num_nodes;
    // Pointer to the byte array storing the node
    uint8_t *arr;
    // Height of the tree
    uint16_t height;
} BBit_Tree_t;
/*************************************************
 * PUBLIC VARIABLES
 *************************************************/

/*************************************************
 FUNCTION HEADERS
*************************************************/

/**
 * BBit_Tree_Init
 * @param BBit_Tree_t* Tree structure to initialise
 * @param uint32_t Number of nodes to record as the max size of the tree
 * @param uint8_t Pointer to the array where the tree will be stored. Min size of 8*num_nodes
 */
void BBit_Tree_Init(BBit_Tree_t *tree, uint32_t num_nodes, uint8_t *arr);

/**
 * @brief Gets the node value given the index of the node in the binary tree
 * @param BBit_Tree_t* Tree structure to operate on
 * @param uint32_t Index of the node to get the value of
 * @returns uint8_t Binary value of the node
 */
uint8_t BBit_Tree_Get_Node_Val(BBit_Tree_t *tree, uint32_t idx);

/**
 * BBit_Tree_Insert_Node_IDX
 * Insert into the Nth node of the binary tree
 * @param BBit_Tree_t* Tree structure to initialise
 * @param uint32_t The index of the node within the bit array
 */
void BBit_Tree_Insert_Node_IDX(BBit_Tree_t *tree, uint32_t node_idx);

/**
 * BBit_Tree_Insert_Node_IDX
 * Insert into the Kth level and Nth node within that level
 * @param BBit_Tree_t* Tree structure to initialise
 * @param uint16_t The index of the node on the Kth level
 * @param uint16_t The Kth level of the binary tree
 */
void BBit_Tree_Insert_Level_IDX(BBit_Tree_t *tree, uint16_t node_idx, uint16_t level_idx);

/**
 * BBit_Tree_Insert_Node_IDX
 * Remove the Nth node of the binary tree
 * @param BBit_Tree_t* Tree structure to initialise
 * @param uint32_t The index of the node within the bit array
 */
void BBit_Tree_Remove_Node_IDX(BBit_Tree_t *tree, uint32_t node_idx);

/**
 * BBit_Tree_Insert_Node_IDX
 * Remove the Kth level and Nth node within that level
 * @param BBit_Tree_t* Tree structure to initialise
 * @param uint16_t The index of the node on the Kth level
 * @param uint16_t The Kth level of the binary tree
 */
void BBit_Tree_Remove_Level_IDX(BBit_Tree_t *tree, uint16_t node_idx, uint16_t level_idx);

/**
 * BBit_Tree_Insert_Node_IDX
 * Print bit array for debugging
 * @param BBit_Tree_t* Tree structure to initialise
 */
void BBit_Tree_Print_Arr(BBit_Tree_t *tree);

/**
 * @brief Fetches the parent of a node in the tree given its index number
 * @param BBit_Tree_t tree - Tree to operate on
 * @param uint32_t idx - Index of the node in the tree's array
 * @returns uint32_t index of the parent
 */
uint32_t BBit_Tree_Parent_IDX(BBit_Tree_t *tree, uint32_t idx);
/**
 * @brief Fetches the index of the right child of a node in the tree given its index number
 * @param BBit_Tree_t tree - Tree to operate on
 * @param uint32_t idx - Index of the node in the tree's array
 * @returns uint32_t index of the right child
 */
uint32_t BBit_Tree_Right_Child_IDX(BBit_Tree_t *tree, uint32_t idx);
/**
 * @brief Fetches the left child of a node in the tree given its index number
 * @param BBit_Tree_t tree - Tree to operate on
 * @param uint32_t idx - Index of the node in the tree's array
 * @returns uint32_t index of the left child
 */
uint32_t BBit_Tree_Left_Child_IDX(BBit_Tree_t *tree, uint32_t idx);
/**
 * BBit_Tree_Num_Nodes
 * Calculate the number of nodes in the Bit Binary Tree given the height
 * @param uint32_t The height of the Bit Binary Tree
 */
uint16_t BBit_Tree_Num_Nodes(uint32_t h);

/**
 * @brief Gets the sibling index given some index within the tree
 *
 * @param tree Tree we will be finding the sibling index on
 * @param idx The index of which the sibling is of interest
 * @return uint32_t Index of the sibling within the tree
 */
uint32_t BBit_Tree_Get_Sibling(BBit_Tree_t *tree, uint32_t idx);
#endif

/*************************************************
 * END OF FILE
 *************************************************/
