/*************************************************
 * START OF FILE
*************************************************/
#ifndef __MATH_H__
#define __MATH_H__
/***************************
 * USEFUL FUNCTIONS
***************************/
#include "stdint.h"
/***************************
 * USEFUL FUNCTIONS
***************************/
/* Power of 2 */
int pow_base_2(int power);
/* Log Base 2 */
int log2_32(uint32_t value);
/* Calculate the NUmber of Nodes in Tree*/
int num_nodes_in_bbit_tree(uint32_t h);
#endif
/*************************************************
 * END OF FILE
*************************************************/
