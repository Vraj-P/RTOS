/**************************************************************************//**
 * @file        ae_tasks13_G30.c
 * @brief       test cases for P4
 *
 * @version     V1.0
 * @authors     Vinay Chaudhari
 * @date        2022 July
 *
 *
 *****************************************************************************/
 
#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae_util.h"
#include "ae_tasks_util.h"

#define     NUM_INIT_TASKS  3       // number of tasks during initialization
TASK_INIT   g_init_tasks[NUM_INIT_TASKS];
task_t g_tids[MAX_TASKS];

int passed_tests = 0;
int failed_tests = 0;

void test_task(void);
void task1(void);
void task2(void);
void task3(void);
void task4(void);

typedef struct WLCLK_time_object 
{
    U8 hour;
    U8 minute;
    U8 second;
} WLCLK_TIME;


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
    tasks[0].ptask = &task3;
    
    tasks[1].priv  = 1;
    tasks[1].ptask = &task2;
    
    tasks[2].priv  = 1;
    tasks[2].ptask = &task1;
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

    int return_msg = tsk_create(&g_tids[3], &task1, HIGH, 0x200);	

    return_msg = tsk_create(&g_tids[2], &task2, HIGH, 0x200);

    return_msg = tsk_create(&g_tids[1], &task3, HIGH, 0x200);
	
    return_msg = tsk_create(&g_tids[4], &task4, HIGH, 0x200);

    printf("%d/%d tests PASSED\r\n", passed_tests, passed_tests + failed_tests);
    printf("%d/%d tests FAILED\r\n", failed_tests, passed_tests + failed_tests);
    printf("END\r\n");

    tsk_exit();
}

// two functions, 4 tasks
// one of the functions sets rt
// log the tid
// suspend
// exit

void task1(void)
{   
    TIMEVAL period;
    period.sec = 1;
    period.usec = 0;
    rt_tsk_set(&period);

    for(int i = 0 ; i < 2 ; i ++) {
        printf("Currently running TID=%d\r\n", 1);
        rt_tsk_susp();
    }
		printf("Currently running TID=%d\r\n", 1);
    tsk_exit();
}

void task2(void)
{   
    TIMEVAL period;
    period.sec = 0;
    period.usec = 500000;
    rt_tsk_set(&period);

    for(int i = 0 ; i < 3 ; i ++) {
        printf("Currently running TID=%d\r\n", 2);
        rt_tsk_susp();
    }
		printf("Currently running TID=%d\r\n", 2);
    tsk_exit();
}

void task3(void)
{   
    TIMEVAL period;
    period.sec = 0;
    period.usec = 250000;
    rt_tsk_set(&period);

    for(int i = 0 ; i < 5 ; i ++) {
        printf("Currently running TID=%d\r\n", 3);
        rt_tsk_susp();
    }
		printf("Currently running TID=%d\r\n", 3);
    tsk_exit();
}

void task4(void)
{   
    while(1) {
    }
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
