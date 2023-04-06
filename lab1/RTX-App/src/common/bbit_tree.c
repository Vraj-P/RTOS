/*************************************************
 * FILE START
 *************************************************/
/*************************************************
 * INCLUDE STATEMENTS
 *************************************************/
#include "bbit_tree.h"

/*************************************************
 * MACRO DEFINITIONS
 *************************************************/

/*************************************************
 * GLOBAL VARIABLES
 *************************************************/

/*************************************************
 * FUNCTION HEADERS
 * TODO - implement function definitions
 *************************************************/

/* Insert */
// Assuming index starts at 0
void BBit_Tree_Toggle_Node_IDX(BBit_Tree_t *tree, uint32_t node_idx)
{
    tree->arr[node_idx / 8] ^= 1 << (node_idx % 8);
}

// calc func(calc) -> toggle(convert(inputs))
int BBit_Tree_Toggle_Level_IDX(BBit_Tree_t *tree, uint16_t node_idx, uint16_t level_idx)
{
    return (1 << level_idx) - 1 + node_idx;
}

uint8_t BBit_Tree_Get_Node_Val(BBit_Tree_t *tree, uint32_t idx)
{
    return tree->arr[idx / 8] & 1 << (idx % 8);
}
