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

/**************************************************************************//**
 * @file        ae_dummy_tasks_p1.c
 * @brief       dummy tasks for P1
 *
 * @version     V1.2022.05
 * @authors     Yiqing Huang
 * @date        2022 MAY
 *
 * @note        Each task is in an infinite loop. These Tasks never terminate.
 *
 *****************************************************************************/

#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae_util.h"
#include "ae_tasks_util.h"

#define     NUM_INIT_TASKS  2       // number of tasks during initialization
TASK_INIT   g_init_tasks[NUM_INIT_TASKS];

task_t g_tasks[MAX_TASKS];
int passed_tests = 0;
int failed_tests = 0;



void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num)
{
    *p_num = NUM_INIT_TASKS;
    *pp_tasks = g_init_tasks;
    set_ae_tasks(*pp_tasks, *p_num);
}

void set_ae_tasks(TASK_INIT *tasks, int num)
{
    for (int i = 0; i < num; i++ ) {                                                 
        tasks[i].u_stack_size = PROC_STACK_SIZE;    
        tasks[i].prio = HIGH;
        tasks[i].priv = 1;
    }
    tasks[0].priv  = 1;
    tasks[0].ptask = &priv_task1;
    tasks[1].priv  = 0;
    tasks[1].ptask = &task1;
}

/**
 * @brief Testing on IRAM2
 */

#ifdef ECE350_P1

int test_case3()
{
	printf("Starting Test 3\r\n");
	int result = 0;
	if ( mem_alloc(0xffffffff) != NULL )
	{
		failed_tests += 1;
		printf("Error in Test 3 - RAM 1\r\n");
	}
	else
		passed_tests += 1;
	if ( mem2_alloc(0xffffffff) != NULL )
	{
		failed_tests += 1;
		printf("Error in Test 3 - RAM 2\r\n");
	}
	else
		passed_tests += 1;
	
	return result;
}

int test_case4()
{
	
	U32 num_blocks = 0x1000/32;
	unsigned long long *block_list[0x1000/32];
	
	int error_occured = 0;
	
	printf("Starting Test 4\r\n");
	
	// Allocate every block in the pool
	for(U32 i = 0; i < num_blocks; i++)
	{
		
		block_list[i] = mem_alloc(4); // also try mem2_alloc(4)
		if (block_list[i] == NULL)
		{
			error_occured++;
			continue;
		}
		
		unsigned long long val_to_assign = (i % 2 == 0) ? 0xFFFFFFFFFFFF : 0;

		for (U8 l = 0; l < 4; l++)
		{
			*(unsigned long long *)(block_list[i] + l) = val_to_assign;
		}
	}
	
		// Deallocate every block in the pool
	for (U32 g = 0; g < num_blocks; g++)
	{
		unsigned long long val_assigned = (g % 2 == 0) ? 0xFFFFFFFFFFFF : 0;

		for (U8 l = 0; l < 4; l++)
		{
			if ( *(unsigned long long *)(block_list[g] + l) != val_assigned )
			{
				error_occured += 1;
			}
		}
		mem_dealloc(block_list[g]);
	}
	
	if (mem_dump() != 1)
	{
		printf("Test 4: Dealloc did not work\r\n");
		error_occured++;
	}
	if (error_occured != 0)
		failed_tests += 1;
	else
		passed_tests += 1;
	printf("Errors in test 4: %d\r\n", error_occured);
	
	return error_occured;
}

int test_case5(){
	U32 num_blocks = 0x1000/128;
	unsigned long long *block_list[0x1000/128];
	
	int error_occured = 0;
	
	printf("Starting Test case 5\n");

	// Allocate every block in the pool
	for(U32 i = 0; i < num_blocks; i++)
	{
		block_list[i] = mem_alloc(128); // also try mem2_alloc(128)
		if (block_list[i] == NULL)
		{
			error_occured++;
			continue;
		}
		
		uint64_t val_to_assign = i % 2 ? 0x111111111111 : 0;
		
		for(U8 l = 0; l < 16; l++)
		{
			*(unsigned long long *)(block_list[i] + l) = val_to_assign;
		}
	}

	// Deallocate every other block in the pool
	for (U32 g = 0; g < num_blocks; g += 2)
	{
		uint64_t val_assigned = g % 2 ? 0x111111111111 : 0;

		for (U8 l = 0; l < 16; l++)
		{
			if (*(unsigned long long *)(block_list[g] + l)!= val_assigned )
				error_occured += 1;
		}
		mem_dealloc(block_list[g]);

	}

	if (mem_alloc(256) != NULL)
		return RTX_ERR;
	
	// If an error occured, error_occured will be non zero
	if (error_occured != 0)
		failed_tests += 1;
	else
		passed_tests += 1;
	printf("num error in test 5: %d\n", error_occured);
	return error_occured == 0 ? RTX_OK : RTX_ERR;
}

#endif // ECE350_P1

/**************************************************************************//**
 * @brief       a task that prints AAAAA, BBBBB, CCCCC,...., ZZZZZ on each line.
 *              It yields the cpu every 6 lines are printed.
 *****************************************************************************/

#ifdef ECE350_P1
void priv_task1(void) {

    task_t tid = tsk_gettid();
    //printf("priv_task1: TID =%d\r\n", tid);
		printf("G30-TS3: START\r\n");
    test_case3();
		test_case4();
		test_case5();
		printf("G30-TS3: %d/%d tests PASSED\r\n", passed_tests, passed_tests + failed_tests);
		printf("G30-TS3: %d/%d tests FAILED\r\n", failed_tests, passed_tests + failed_tests);
		printf("G30-TS3: END\r\n");
}
#endif


void task1(void) {
    while (1) {
        uart1_put_string("task1: executing\r\n");
        for ( int x = 0; x < DELAY; x++); // some artificial delay
    }
}

void task2(void)
{
    while (1) {
        uart1_put_string("task2: executing\r\n");
        for ( int x = 0; x < DELAY; x++); // some artificial delay
    }
}


/**
 * @brief: a dummy task3
 */
void task3(void)
{
    uart1_put_string("task3: entering \r\n");
    /* do something */
    /* terminating */
    tsk_exit();
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
