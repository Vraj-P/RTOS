/**************************************************************************//**
 * @file        ae_tasks4_G30.c
 * @brief       Simple test suite for P2
 *
 * @version     V1.0
 * @authors     Hanu
 * @date        2022 June
 *
 *
 *****************************************************************************/
 
#include "uart_polling.h"
#include "printf.h"
#include "ae_util.h"
#include "ae_tasks_util.h"

#define     BUF_LEN         128

#define     NUM_INIT_TASKS  1       // number of tasks during initialization
TASK_INIT   g_init_tasks[NUM_INIT_TASKS];

U8 g_buf1[BUF_LEN];
U8 g_buf2[BUF_LEN];
task_t g_tasks[MAX_TASKS];
int passed_tests = 0;
int failed_tests = 0;

void task1(void);
void test_task(void);

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
        tasks[i].prio = MEDIUM;
        tasks[i].priv = 1;
    }
    tasks[0].priv  = 1;
    tasks[0].ptask = &test_task;
}

/**************************************************************************//**
 * @brief:      a task that prints 00000, 11111, 22222,....,99999 on each line.
 *              It yields the cpu every 6 lines are printed
 *              before printing these lines indefinitely, it does the following:
 *              - creates a new task
 *              - obtains the task information. 
 *              - changes the newly created task's priority.
 *              - allocates some memory
 *              - deallocates some memory
 *              - calls memory dump function
 *****************************************************************************/


void test_task(void)
{   
		printf("G30-TS1: START\r\n");
		
		task_t tid;
		tsk_create(&tid, &task1, HIGH, 0x401);

	
		printf("G30-TS1: %d/%d tests PASSED\r\n", passed_tests, passed_tests + failed_tests);
		printf("G30-TS1: %d/%d tests FAILED\r\n", failed_tests, passed_tests + failed_tests);
		printf("G30-TS1: END\r\n");

    tsk_exit();
}

void task1(void) {
	 static RTX_TASK_INFO task_info;
	 task_t tid = tsk_gettid();
	 tsk_get(tid, &task_info);
	

		if(task_info.u_stack_size == 0x800) {
			passed_tests++;
		} else {
			failed_tests++;
		}
		
		
		tsk_exit();
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
