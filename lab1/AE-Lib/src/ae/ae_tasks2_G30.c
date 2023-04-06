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
#include	"rtx_errno.h"

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

int test_mem1()
{
		
    return 0;
}

int test_mem2(void) {
	void *p[8];
	
	//TEST1 0 size ON ALLOC
	p[0] = mem2_alloc(0);
	if(p[0] == NULL && errno == 0) {
		passed_tests++;
	} else {
		failed_tests++;
	}
	
	//printf("%d\r\n", errno);
	
	//TEST2 ALLOC MORE THAN AVAILABLE
	p[0] = mem2_alloc(0x10000);
	if(p[0] == NULL && errno == ENOMEM) {
		passed_tests++;
	} else {
		failed_tests++;
	}
		//printf("%d\r\n", errno);

	
	//TEST3 ALLOC WHEN NO SPACE
	p[0] = mem2_alloc(0x8000);
	p[1] = mem2_alloc(0x100);
	
	if(p[1] == NULL && errno == ENOMEM) {
		passed_tests++;
	} else {
		failed_tests++;
	}
		//printf("%d\r\n", errno);

	
	mem2_dealloc(p[0]);
	
	//TEST3 DEALLOC OUTSIDE OF ADDRESS
	p[0] = (void *)0x1;
	
	printf("p[0] = 0x%x\r\n", p[0]);
	 
	int ret = mem2_dealloc(p[0]);
	
	printf("RETURN: %d\r\n", ret);
	printf("ERRNO: %d\r\n", errno);
	
	
	if(ret == RTX_ERR && errno == EFAULT) {
		passed_tests++;
	} else {
		failed_tests++;
	}
	 
}
int test_case1() {
	U32 num_blocks = 0x1000/32;
	unsigned long long *block_list[0x1000/32];
	uint8_t error_occured = 0;

	// Allocate every block in the pool
	for (U32 i = 0; i < num_blocks; i++) {
		block_list[i] = mem_alloc(32); // also try mem2_alloc(32)
		unsigned long long val_to_assign = i % 2 ? 0xFFFFFFFFFFFF : 0;
		for (int l = 0; l < 4; l++) {
			block_list[i][l] = val_to_assign;
		}
	}

	// Deallocate every block in the pool
	for (int i = 0; i < num_blocks; i++) {
		unsigned long long val_assigned = i % 2 ? 0xFFFFFFFFFFFF : 0;

		for (int l = 0; l < 4; l++) {
			if ( (block_list[i][l]) != val_assigned )
				error_occured += 1;
		}
		mem_dealloc(block_list[i]);
	}

	// If an error occured, error_occured will be non zero
	if (error_occured != 0)
		failed_tests += 1;
	
passed_tests += 1;
	printf("Errors in test 1: %d\r\n", error_occured);
	
	return error_occured;

}

int test_case2() {
	uint8_t error_occured = test_case1();
	if (mem_alloc(4096) != (void *)0x10007000){
	failed_tests += 1;
		error_occured += 1;
		printf("Errors in test 2: %d\r\n", error_occured);
		return RTX_ERR;
	}
	mem_dealloc((void *)0x10007000);
	printf("Errors in test 2: %d\r\n", error_occured);
passed_tests += 1;
	return RTX_OK;
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
		printf("G30-TS2: START\r\n");
    test_mem1();    // test on IRAM1 
    test_mem2();    // test on IRAM2
	test_case1();
	test_case2();
		printf("G30-TS2: %d/%d tests PASSED\r\n", passed_tests, passed_tests + failed_tests);
		printf("G30-TS2: %d/%d tests FAILED\r\n", failed_tests, passed_tests + failed_tests);
		printf("G30-TS2: END\r\n");


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
