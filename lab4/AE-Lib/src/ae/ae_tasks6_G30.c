/**************************************************************************//**
 * @file        ae_tasks4_G30.c
 * @brief       Simple test suite for P2
 *
 * @version     V1.0
 * @authors     Karim Al-Atrash
 * @date        2022 June
 *
 *
 *****************************************************************************/
 
#include "ae_tasks.h"
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

void test_task(void);
void task1(void);

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
        tasks[i].prio = LOW;
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
    printf("START\r\n");
    
    task_t tid;
    int return_msg = tsk_create(&tid, &task1, HIGH, 0x200);
		
		if(return_msg == RTX_ERR)
        {
            failed_tests += 1;
        }
        else
        {
            passed_tests += 1;
        }
	
    printf("%d/%d tests PASSED\r\n", passed_tests, passed_tests + failed_tests);
    printf("%d/%d tests FAILED\r\n", failed_tests, passed_tests + failed_tests);
    printf("END\r\n");

    tsk_exit();
}

void task1(void)
{   
    printf("entering test 4\r\n");

    static RTX_TASK_INFO task_info; /* our stack space is small, so make it static local */
    task_t tid = tsk_gettid();
    tsk_get(tid, &task_info);

    printf("task ID: %d\r\n", tid);
    printf("task priority: %x\r\n", task_info.prio);
		
    task_t tid_old = tid;
    int return_msg = tsk_create(&tid, &task1, HIGH, 0x20);
    tsk_set_prio(tid_old, MEDIUM);
    tsk_get(tid_old, &task_info);

    printf("task ID: %d\r\n", tid_old);
    printf("task priority: %x\r\n", task_info.prio);

    if (return_msg == RTX_ERR && tid_old != 9)
    {
        failed_tests += 1;
    }
    else
    {
        passed_tests += 1;
    }

    tsk_exit();
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
