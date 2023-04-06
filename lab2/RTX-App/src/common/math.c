/*************************************************
 * START OF FILE
*************************************************/
#include "math.h"

/*************************************************
 * PRIVATE GLOBAL VARIABLES
*************************************************/
const int tab32[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31};

/*************************************************
 * FUNCTION DEFINITIONS
*************************************************/
/* Power of 2 */
int pow_base_2(int power)
{
    return 1 << power;
}

/* Fast Log Base 2 */
int log2_32 (unsigned int value)
{
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return tab32[(unsigned int)(value*0x07C4ACDD) >> 27];
}

/* Calculate the NUmber of Nodes in Tree */
int num_nodes_in_bbit_tree(unsigned int h)
{
    return pow_base_2(h) - 1; //s^(h) - 1
}

/* Calculate the closest power of 2 greater than size */
unsigned int upper_base2(unsigned int size)
{
    int log2_size = log2_32(size);                               // 432 -> 8
    return size == 1 << log2_size ? size : 1 << (log2_size + 1); // if (432 == 2^8) // 2^9
}
/*************************************************
 * END OF FILE
*************************************************/
