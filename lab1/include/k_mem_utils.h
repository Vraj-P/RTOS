#ifndef K_MEM_UTILS_H_
#define K_MEM_UTILS_H_

#include "rtx_errno.h"
#include "common.h"
#include "math.h"
#include "freelist.h"
#include "bit_array.h"

void *check_size(unsigned int size, unsigned int max_size);
unsigned int upper_base2(unsigned int size);
unsigned int get_FL_layer(unsigned int size, unsigned int upper_size_log2);
void split(FL_Node_t **freelist_array, char *bit_array, unsigned int FL_index, size_t iram_base, unsigned int upper_size_log2);
unsigned int xk_to_bit(unsigned int x, unsigned int FL_index);
unsigned int get_x_value(size_t address, size_t iram_base, size_t size);
unsigned int get_size_from_FL_index(unsigned int FL_index, unsigned int upper_size_log2);
void *coalesce(FL_Node_t **freelist_array, char *bit_array, size_t *node_index, size_t FL_index, size_t iram_base, unsigned int upper_size_log2);
unsigned int x_value_to_address(unsigned int x, size_t iram_base, size_t size);

#endif
