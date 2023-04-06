#include "k_mem_utils.h"

void *check_size(unsigned int size, unsigned int max_size)
{
    if (!size)
        return NULL;
    else if (size > max_size)
    {
        errno = ENOMEM;
        return NULL;
    }

    return (void *)1;
}

unsigned int upper_base2(unsigned int size)
{
    int log2_size = log2_32(size);                               // 432 -> 8
    return size == 1 << log2_size ? size : 1 << (log2_size + 1); // if (432 == 2^8) // 2^9
}

unsigned int get_FL_layer(unsigned int size_base2, unsigned int upper_size_log2)
{
    return upper_size_log2 - size_base2;
}

void split(FL_Node_t **freelist_array, char *bit_array, unsigned int FL_index, size_t iram_base, unsigned int upper_size_log2)
{
    size_t current_node = (size_t)(freelist_array[FL_index]);
	//printf("in split= 0x%x\n", current_node);
    FL_Remove(&freelist_array[FL_index], freelist_array[FL_index]); // we remove first available node so in this case its the head
    BA_Toggle(bit_array, xk_to_bit(get_x_value(current_node, iram_base, get_size_from_FL_index(FL_index, upper_size_log2)), FL_index));

    size_t right_child = current_node + get_size_from_FL_index(FL_index + 1, upper_size_log2); // calc adress of right child to add in from left child
    FL_Insert(&freelist_array[FL_index + 1], (FL_Node_t *)right_child);                         // add that bitch in
	  FL_Insert(&freelist_array[FL_index + 1], (FL_Node_t *)current_node);                        // left child has same address as parent so just add it to layer below

}

unsigned int xk_to_bit(unsigned int x, unsigned int FL_index)
{
    return (1 << FL_index) + x - 1; // 2^k + x  -1
}

unsigned int get_x_value(size_t address, size_t iram_base, size_t size)
{
    return (address - iram_base) / size;
}

unsigned int x_value_to_address(unsigned int x, size_t iram_base, size_t size)
{
    return x * size + iram_base;
}

unsigned int get_size_from_FL_index(unsigned int FL_index, unsigned int upper_size_log2)
{
    return 1 << (upper_size_log2 - FL_index);
}

void *coalesce(FL_Node_t **freelist_array, char *bit_array, size_t *node_index, size_t FL_index, size_t iram_base, unsigned int upper_size_log2) // just byu moving one adress up we are changing the space
{
    if (BA_Get(bit_array, xk_to_bit(*node_index ^ 1, FL_index)) == 0)
    {   
        FL_Remove(&freelist_array[FL_index], (FL_Node_t *)x_value_to_address(*node_index ^ 1, iram_base, get_size_from_FL_index(FL_index, upper_size_log2))); // if the budy is a free block then move up a lyer
        *node_index /= 2; //move up
        return (void *)1;
    }

    return NULL;
}
