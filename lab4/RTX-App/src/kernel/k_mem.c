/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2022 Yiqing Huang
 *                          All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice and the following disclaimer.
 *
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************
 */

/**************************************************************************/ /**
                                                                              * @file        k_mem.c
                                                                              * @brief       Kernel Memory Management API C Code
                                                                              *
                                                                              * @version     V1.2021.01.lab2
                                                                              * @authors     Yiqing Huang
                                                                              * @date        2021 JAN
                                                                              *
                                                                              * @note        skeleton code
                                                                              *
                                                                              *****************************************************************************/

#include "k_inc.h"
#include "k_mem.h"

/*---------------------------------------------------------------------------
The memory map of the OS image may look like the following:
                   RAM1_END-->+---------------------------+ High Address
                              |                           |
                              |                           |
                              |       MPID_IRAM1          |
                              |   (for user space heap  ) |
                              |                           |
                 RAM1_START-->|---------------------------|
                              |                           |
                              |  unmanaged free space     |
                              |                           |
&Image$$RW_IRAM1$$ZI$$Limit-->|---------------------------|-----+-----
                              |         ......            |     ^
                              |---------------------------|     |
                              |                           |     |
                              |---------------------------|     |
                              |                           |     |
                              |      other data           |     |
                              |---------------------------|     |
                              |      PROC_STACK_SIZE      |  OS Image
              g_p_stacks[2]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[1]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |                           |  OS Image
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |     |
             g_k_stacks[15]-->|---------------------------|     |
                              |                           |     |
                              |     other kernel stacks   |     |
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |  OS Image
              g_k_stacks[2]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |
              g_k_stacks[1]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |
              g_k_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |---------------------------|     |
                              |        TCBs               |  OS Image
                      g_tcbs->|---------------------------|     |
                              |        global vars        |     |
                              |---------------------------|     |
                              |                           |     |
                              |                           |     |
                              |        Code + RO          |     |
                              |                           |     V
                 IRAM1_BASE-->+---------------------------+ Low Address

---------------------------------------------------------------------------*/

/*
 *===========================================================================
 *                            GLOBAL VARIABLES
 *===========================================================================
 */
// kernel stack size, referred by startup_a9.s
const U32 g_k_stack_size = KERN_STACK_SIZE;
// task proc space stack size in bytes, referred by system_a9.c
const U32 g_p_stack_size = PROC_STACK_SIZE;

// task kernel stacks
//U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));

// task process stack (i.e. user stack) for tasks in thread mode
// remove this bug array in your lab2 code
// the user stack should come from MPID_IRAM2 memory pool
// U32 g_p_stacks[MAX_TASKS][PROC_STACK_SIZE >> 2] __attribute__((aligned(8)));
//U32 g_p_stacks[NUM_TASKS][PROC_STACK_SIZE >> 2] __attribute__((aligned(8)));

char IRAM1_Bit_Array[(1 << (RAM1_SIZE_LOG2 - MIN_BLK_SIZE_LOG2 - 2))];
char IRAM2_Bit_Array[(1 << (RAM2_SIZE_LOG2 - MIN_BLK_SIZE_LOG2 - 2))];

FL_Node_t *IRAM1_FL_Array[RAM1_SIZE_LOG2 - MIN_BLK_SIZE_LOG2 + 1];
FL_Node_t *IRAM2_FL_Array[RAM2_SIZE_LOG2 - MIN_BLK_SIZE_LOG2 + 1];

U32 RAM1_END_u = 0;
U32 RAM2_END_u = 0;

/* 
 * MPool 1 Global Variables 
*/
U32 mpool1_size = 0;
U32 log2_mpool1_size = 0;

/* 
 * MPool 2 Global Variables 
*/
U32 mpool2_size = 0;
U32 log2_mpool2_size = 0;

/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

/* note list[n] is for blocks with order of n */
mpool_t
k_mpool_create(int algo, U32 start, U32 end) //start and end are random pointers
{
    mpool_t mpid = MPID_IRAM1;

#ifdef DEBUG_0
    printf("k_mpool_init: algo = %d\r\n", algo);
    printf("k_mpool_init: RAM range: [0x%x, 0x%x].\r\n", start, end);
#endif /* DEBUG_0 */

    if (algo != BUDDY)
    {
        errno = EINVAL;
        return RTX_ERR;
    }

    if (start == RAM1_START)
    {
        mpid = MPID_IRAM1;
			
				// Initialise the size of mempool 1
				mpool1_size = end - start + 1;
				log2_mpool1_size = log2_32(mpool1_size);

        FL_Insert(&IRAM1_FL_Array[0], (FL_Node_t *)start);
        RAM1_END_u = end;
    }
    else if (start == RAM2_START)
    {
			  mpid = MPID_IRAM2;
				
				// Initialise the size of mempool 2
				mpool2_size = end - start + 1;
				log2_mpool2_size = log2_32(mpool2_size);
        
				FL_Insert(&IRAM2_FL_Array[0], (FL_Node_t *)start);
        RAM2_END_u = end;
    }
    else
    {
        errno = EINVAL;
        return RTX_ERR;
    }

    return mpid;
}

void *k_mpool_alloc(mpool_t mpid, size_t size)
{
#ifdef DEBUG_0
    printf("k_mpool_alloc: mpid = %d, size = %d, 0x%x\r\n", mpid, size, size);
#endif /* DEBUG_0 */
	if (mpid != MPID_IRAM1 && mpid != MPID_IRAM2)
    {
        errno = EINVAL;
        return NULL;
    }

		size_t IRAM_BASE = (mpid == MPID_IRAM1) ? RAM1_START : RAM2_START;
    size_t IRAM_SIZE = (mpid == MPID_IRAM1) ? mpool1_size : mpool2_size;
    size_t IRAM_SIZE_LOG2 = (mpid == MPID_IRAM1) ? log2_mpool1_size : log2_mpool2_size;

    char *IRAM_Bit_Array = (mpid == MPID_IRAM1) ? IRAM1_Bit_Array : IRAM2_Bit_Array;
    FL_Node_t **IRAM_FL_Array = (mpid == MPID_IRAM1) ? IRAM1_FL_Array : IRAM2_FL_Array;
		
    if (!check_size(size, IRAM_SIZE))
        return NULL;	

    size_t desired_size = size < MIN_BLK_SIZE ? MIN_BLK_SIZE : upper_base2(size);
		
    size_t FL_index = get_FL_layer(log2_32(desired_size), IRAM_SIZE_LOG2);
		
    int first_free_index = 0;
    for (first_free_index = FL_index; first_free_index > 0; --first_free_index)
    {
        if (IRAM_FL_Array[first_free_index])
            break;
    }
	
		// if every list from start is NULL all the way up, no free mem left to allocate the specifed space
    if (!IRAM_FL_Array[first_free_index]) 
    {
        errno = ENOMEM;
        return NULL;
    }

		// splitting loop to find desired desired block
    for (unsigned int i = first_free_index; i < FL_index; ++i) 
    {
				// stop as soon as one of the child in desired layer with the size needed
        split(IRAM_FL_Array, IRAM_Bit_Array, i, IRAM_BASE, IRAM_SIZE_LOG2);
    }

    // we pulling out the child we want and we alloc that
    size_t current_node = (size_t)(IRAM_FL_Array[FL_index]);
    FL_Remove(&IRAM_FL_Array[FL_index], IRAM_FL_Array[FL_index]); // we remove first available node so in this case its the head
    BA_Toggle(IRAM_Bit_Array, xk_to_bit(get_x_value(current_node, IRAM_BASE, get_size_from_FL_index(FL_index, IRAM_SIZE_LOG2)), FL_index));

    return (void *)current_node;
}

int k_mpool_dealloc(mpool_t mpid, void *ptr)
{
#ifdef DEBUG_0
    printf("k_mpool_dealloc: mpid = %d, ptr = 0x%x\r\n", mpid, ptr);
#endif /* DEBUG_0 */
    if (mpid != MPID_IRAM1 && mpid != MPID_IRAM2)
    {
        errno = EINVAL;
        return RTX_ERR;
    }
		
		size_t IRAM_BASE = (mpid == MPID_IRAM1) ? RAM1_START : RAM2_START;
    size_t IRAM_SIZE = (mpid == MPID_IRAM1) ? mpool1_size : mpool2_size;
    size_t IRAM_SIZE_LOG2 = (mpid == MPID_IRAM1) ? log2_mpool1_size : log2_mpool2_size;

    char *IRAM_Bit_Array = (mpid == MPID_IRAM1) ? IRAM1_Bit_Array : IRAM2_Bit_Array;
    FL_Node_t **IRAM_FL_Array = (mpid == MPID_IRAM1) ? IRAM1_FL_Array : IRAM2_FL_Array;

		//BA_Print(IRAM_Bit_Array, 50);
    if (ptr < (void *)IRAM_BASE || ptr > (void *)(IRAM_BASE + IRAM_SIZE))
    { // checking if pointer is within memory (from lab manual)
        errno = EFAULT;
        return RTX_ERR;
    }

    // this whole block is the high level alg0
    size_t node_index = ((size_t)ptr - IRAM_BASE) / MIN_BLK_SIZE; // finding bottom most node
    int FL_index = 0;
    for (FL_index = IRAM_SIZE_LOG2 - MIN_BLK_SIZE_LOG2; FL_index > 0; --FL_index)
    { // loop to go up the layers

        if (BA_Get(IRAM_Bit_Array, xk_to_bit(node_index, FL_index)) == 1)
            break;
				node_index /= 2; //when we go up a layer number of nodes down by 2, so /2 indexes
    }
		
		
		//printf("GOT OUT OF FIRST FOR LOOP\n");
		//printf("FL_INDEX: %d\n", FL_index);

		while(FL_index > 0)
    {
        BA_Toggle(IRAM_Bit_Array, xk_to_bit(node_index, FL_index));
				
			//printf("out col: 0x%x\n", IRAM_FL_Array[FL_index]);
        if (!coalesce(IRAM_FL_Array, IRAM_Bit_Array, &node_index, FL_index, IRAM_BASE, IRAM_SIZE_LOG2))
            break;
				--FL_index;
    }

    if (FL_index == 0)
        BA_Toggle(IRAM_Bit_Array, xk_to_bit(node_index, FL_index));
		
		//printf("FL_index = %d\n", FL_index);
		//printf("hi\n");

    FL_Insert(&IRAM_FL_Array[FL_index], (FL_Node_t *)x_value_to_address(node_index, IRAM_BASE, get_size_from_FL_index(FL_index, IRAM_SIZE_LOG2))); // if the buddy is used then we can insert back into the buddt
		
		//printf("done\n");
    return RTX_OK;
}

int k_mpool_dump(mpool_t mpid)
{
#ifdef DEBUG_0
    printf("k_mpool_dump: mpid = %d\r\n", mpid);
#endif /* DEBUG_0 */

    if (mpid != MPID_IRAM1 && mpid != MPID_IRAM2)
    {
        errno = EINVAL;
        return RTX_ERR;
    }

    size_t IRAM_SIZE_LOG2 = (mpid == MPID_IRAM1) ? RAM1_SIZE_LOG2 : RAM2_SIZE_LOG2;
    FL_Node_t **IRAM_FL_Array = (mpid == MPID_IRAM1) ? IRAM1_FL_Array : IRAM2_FL_Array;

    int total_blocks = 0;
    for (size_t FL_index = 0; FL_index <= IRAM_SIZE_LOG2 - MIN_BLK_SIZE_LOG2; ++FL_index)
    {
        total_blocks += FL_Print(IRAM_FL_Array[FL_index], get_size_from_FL_index(FL_index, IRAM_SIZE_LOG2));
    }

    printf("%d free memory block(s) found \r\n", total_blocks);
    return total_blocks;
}

int k_mem_init(int algo)
{
#ifdef DEBUG_0
    printf("k_mem_init: algo = %d\r\n", algo);
#endif /* DEBUG_0 */

    if (k_mpool_create(algo, RAM1_START, RAM1_END) < 0)
    {
        return RTX_ERR;
    }

    if (k_mpool_create(algo, RAM2_START, RAM2_END) < 0)
    {
        return RTX_ERR;
    }

    return RTX_OK;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
