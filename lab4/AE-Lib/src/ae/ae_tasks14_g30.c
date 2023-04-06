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

#define     NUM_INIT_TASKS  1       // number of tasks during initialization
TASK_INIT   g_init_tasks[NUM_INIT_TASKS];
task_t g_tids[MAX_TASKS];

int passed_tests = 0;
int failed_tests = 0;

void test_task(void);
void task1(void);
void task2(void);
void task3(void);
void task4(void);
void task5(void);
void dummyTask(void);
void dummyRtTask(void);

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

    int return_msg = tsk_create(&g_tids[1], &task1, HIGH, 0x200);	

    return_msg = tsk_create(&g_tids[2], &task2, HIGH, 0x200);

    return_msg = tsk_create(&g_tids[4], &task3, HIGH, 0x200);

    return_msg = tsk_create(&g_tids[6], &task4, HIGH, 0x200);

    return_msg = tsk_create(&g_tids[7], &task5, HIGH, 0x200);

    printf("%d/%d tests PASSED\r\n", passed_tests, passed_tests + failed_tests);
    printf("%d/%d tests FAILED\r\n", failed_tests, passed_tests + failed_tests);
    printf("END\r\n");

    tsk_exit();
}

void dummyTask(void) {
    printf("%s", "dummy task PASSED\r\n\n");
    tsk_exit();
}

void dummyRtTask(void) {
    printf("entering task 1\r\n");
    TIMEVAL period;
    period.sec = 0;
    period.usec = 50000;
    rt_tsk_set(&period);

    for(int i = 0 ; i < 3 ; i ++) {
        printf("%s", "DUMMY RT TASK\r\n");
        rt_tsk_susp();
    }
    tsk_exit();
}

// These tests are for checking the errno value for rt_tsk_set
void task1(void)
{ 
    errno = 0;  
    printf("entering task 1\r\n");
    TIMEVAL period;
    period.sec = 1;
    period.usec = 0;
    rt_tsk_set(&period);

    rt_tsk_set(&period);

    if (errno == EPERM) {
        printf("task 1 pass\r\n\n");
        passed_tests++;
    } else {
        printf("task 1 fail\r\n\n");
        failed_tests++;
    }

    tsk_exit();
}

void task2(void)
{  
    errno = 0;    
    printf("entering task 2\r\n");
    TIMEVAL period;
    period.sec = 0;
    period.usec = 800;
    rt_tsk_set(&period);

    if (errno == EINVAL) {
        printf("task 2 pass\r\n\n");
        passed_tests++;
    } else {
        printf("task 2 fail\r\n\n");
        failed_tests++;
    }

    tsk_exit();
}

// These tests are for checking the errno value for rt_tsk_susp

void task3(void)
{
    errno = 0;
    printf("entering task 3\r\n");
    rt_tsk_susp();
    if (errno == EPERM) {
        printf("task 3 pass\r\n\n");
        passed_tests++;
    } else {
        printf("task 3 fail\r\n\n");
        failed_tests++;
    }

    tsk_exit();
}

// These tests are for checking the errno value for rt_tsk_get

void task4(void)
{   
    errno = 0;   
    printf("entering task 4\r\n");
    TIMEVAL period;
    period.sec = 1;
    period.usec = 0;
    task_t tid;
    int return_msg = tsk_create(&g_tids[1], &dummyTask, HIGH, 0x200);	

    rt_tsk_set(&period);

    rt_tsk_get(tid, &period);

    if (errno == EINVAL) {
        printf("task 4 pass\r\n\n");
        passed_tests++;
    } else {
        printf("task 4 fail\r\n\n");
        failed_tests++;
    }

    tsk_exit();
}

void task5(void)
{      
    errno = 0;
    printf("entering task 5\r\n");
    TIMEVAL period;
    period.sec = 1;
    period.usec = 0;
    task_t tid;
    int return_msg = tsk_create(&tid, &dummyRtTask, HIGH, 0x200);	

    rt_tsk_get(tid, &period);

    if (errno == EPERM) {
        printf("task 5 pass\r\n\n");
        passed_tests++;
    } else {
        printf("task 5 fail\r\n\n");
        failed_tests++;
    }

    tsk_exit();
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
