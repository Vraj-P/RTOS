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

int test_mem1()
{
    return 0;
}

int test_mem2(void) {
	void *p[8];
	//TEST1 ALLOCATE 3 BLOCKS
	p[0] = mem2_alloc(0x2000);
	p[1] = mem2_alloc(0x2000);
	p[2] = mem2_alloc(0x2000);
	
	printf("test1: p0 = 0x%x\r\n", p[0]); // \r is a character 
	printf("test1: p1 = 0x%x\r\n", p[1]); 
	printf("test1: p2 = 0x%x\r\n", p[2]); 
	
	int ret = mem2_dump();
	 if (ret == 1) {
		 passed_tests++;
	 } else {
		 failed_tests++;
	 }
	 
	 //TEST2 REMOVE 2 ALLOCATED
	 mem2_dealloc(p[0]);
	 //printf("first dealloc\r\n");
	 mem2_dealloc(p[1]);
	 
	 
	 ret = mem2_dump();
	 if (ret == 2) {
		 passed_tests++;
	 } else {
		 failed_tests++;
	 }
	 
	 ///TEST3 more complex allocation
	 p[0] = p[2];
	 p[1] = mem2_alloc(0x2000);
	 p[2] = mem2_alloc(0x2000);
	 p[3] = mem2_alloc(0x1000);
	 p[4] = mem2_alloc(0x100);
	 p[5] = mem2_alloc(0x200);
	 p[6] = mem2_alloc(0x100);
	 
	printf("test3: p0 = 0x%x\r\n", p[0]); // \r is a character 
	printf("test3: p1 = 0x%x\r\n", p[1]); 
	printf("test3: p2 = 0x%x\r\n", p[2]);
	printf("test3: p3 = 0x%x\r\n", p[3]); // \r is a character 
	printf("test3: p4 = 0x%x\r\n", p[4]); 
	printf("test3: p5 = 0x%x\r\n", p[5]);
	printf("test3: p6 = 0x%x\r\n", p[6]); 	 
	
	 
	 ret = mem2_dump();
	 if (ret == 2) { 
		 passed_tests++;
	 } else {
		 failed_tests++;
	 }
	 
	 //TEST4 coleasing dealloc
	 mem2_dealloc(p[6]);
	 mem2_dealloc(p[4]);
	 mem2_dealloc(p[3]);
	 mem2_dealloc(p[2]);
	 
	 ret = mem2_dump();
	 
	 mem2_dealloc(p[5]);
	 
	 ret = mem2_dump();
	 
	 mem2_dealloc(p[0]);
	 mem2_dealloc(p[1]);
	 
	 ret = mem2_dump();
	 
	 if (ret == 1) { 
		 passed_tests++;
	 } else {
		 failed_tests++;
	 }	 
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
		printf("G30-TS1: START\r\n");
    test_mem1();    // test on IRAM1 
    test_mem2();    // test on IRAM2
		printf("G30-TS1: %d/%d tests PASSED\r\n", passed_tests, passed_tests + failed_tests);
		printf("G30-TS1: %d/%d tests FAILED\r\n", failed_tests, passed_tests + failed_tests);
		printf("G30-TS1: END\r\n");


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
