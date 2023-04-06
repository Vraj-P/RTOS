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
#define     NUM_ITERATIONS  10

#define     NUM_INIT_TASKS  1       // number of tasks during initialization
TASK_INIT   g_init_tasks[NUM_INIT_TASKS];

U8 g_buf1[BUF_LEN];
U8 g_buf_return1[BUF_LEN];
U8 g_buf2[BUF_LEN];
U8 g_buf_return2[BUF_LEN];
task_t g_tasks[MAX_TASKS];
task_t g_tids[MAX_TASKS];
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
    if(return_msg == RTX_ERR)
        failed_tests += 1;
    else
        passed_tests += 1;

    return_msg = tsk_create(&g_tids[2], &task2, HIGH, 0x200);
    if(return_msg == RTX_ERR)
        failed_tests += 1;
    else
        passed_tests += 1;
	
    g_tids[0] = tsk_gettid();
    tsk_set_prio(g_tids[0], MEDIUM);

    printf("%d/%d tests PASSED\r\n", passed_tests, passed_tests + failed_tests);
    printf("%d/%d tests FAILED\r\n", failed_tests, passed_tests + failed_tests);
    printf("END\r\n");

    tsk_exit();
}

void task1(void)
{   
    printf("entering task 1\r\n");

    mbx_t mbx_id = mbx_create(8);
    if(mbx_id < 0)
    {
        failed_tests += 1;
        tsk_exit();
    }
    else
    {
        passed_tests += 1;
    }
    tsk_yield();

    printf("task 1 is receiving first message\r\n");
    int ret_val = recv_msg(g_buf_return1, BUF_LEN);
    if(ret_val == RTX_ERR)
    {
        printf("failed first receive\r\n");
        failed_tests += 1;
    }
    else
    {
        passed_tests += 1;
    }
    tsk_yield();

    printf("task 1 is receiving second message\r\n");
    ret_val = recv_msg(g_buf_return1, BUF_LEN);
    if(ret_val == RTX_ERR)
    {
        printf("failed second receive\r\n");
        failed_tests += 1;
    }
    else
    {
        passed_tests += 1;
    }
    tsk_yield();

    tsk_exit();
}

void task2(void)
{   
    printf("entering task 2\r\n");

    size_t msg_hdr_size = sizeof(struct rtx_msg_hdr);
    U8  *buf = &g_buf2[0];
    struct rtx_msg_hdr *ptr = (void *)buf;

    ptr->length = msg_hdr_size + 1;
    ptr->type = DEFAULT;
    ptr->sender_tid = g_tids[2];
    buf += msg_hdr_size;                        
    *buf = 'A';

    int ret_val = send_msg(g_tids[1], (void *)ptr);
    if(ret_val == RTX_ERR)
    {
        printf("failed first send\r\n");
        failed_tests += 1;
    }
    else
    {
        passed_tests += 1;
    }
    printf("task 2 filled the mailbox\r\n");

    ret_val = send_msg(g_tids[1], (void *)ptr);
    if(ret_val == RTX_ERR)
    {
        printf("failed second send\r\n");
        failed_tests += 1;
    }
    else
    {
        passed_tests += 1;
    }
    printf("task 2 sent first blocked message\r\n");

    ret_val = send_msg(g_tids[1], (void *)ptr);
    if(ret_val == RTX_ERR)
    {
        printf("failed third send\r\n");
        failed_tests += 1;
    }
    else
    {
        passed_tests += 1;
    }
    printf("task 2 sent second blocked message\r\n");

    tsk_exit();
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
