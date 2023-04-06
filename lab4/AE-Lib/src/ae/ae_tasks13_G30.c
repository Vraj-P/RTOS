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
    if(return_msg == RTX_ERR)
        failed_tests += 1;
    else
        passed_tests += 1;

    return_msg = tsk_create(&g_tids[2], &task1, HIGH, 0x200);
    if(return_msg == RTX_ERR)
        failed_tests += 1;
    else
        passed_tests += 1;

    return_msg = tsk_create(&g_tids[3], &task1, HIGH, 0x200);
    if(return_msg == RTX_ERR)
        failed_tests += 1;
    else
        passed_tests += 1;
	
    return_msg = tsk_create(&g_tids[4], &task2, HIGH, 0x200);
    if(return_msg == RTX_ERR)
        failed_tests += 1;
    else
        passed_tests += 1;

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
	
	/*
			uint8_t ret_val = 10;
    task_t tid = tsk_gettid();
    g_tids[0] = tid;
    int     test_id    = 0;
    U8      *p_index   = &(g_ae_xtest.index);
    int     sub_result = 0;
		uint8_t send_buf[50] = {0};
		*/
    TIMEVAL period;
    period.sec = 0;
    period.usec = 100000;
    rt_tsk_set(&period);
		
		/*
		// Create a mailbox of size 7
		ret_val = mbx_create(7);
		(*p_index)++;
		sprintf(g_ae_xtest.msg, "TID=%u, Creating mailbox", tid);
		sub_result = ret_val == RTX_OK ? 1 : 0;
		process_sub_result(test_id, *p_index, sub_result);
		*/
    for(int i = 0 ; i < 1000 ; i ++) {
        printf("Currently running TID=%d\r\n", tsk_gettid());
        rt_tsk_susp();
			/*
				// Load the message into the buffer
				RTX_MSG_HDR * header = (RTX_MSG_HDR *)&send_buf[0];
			
				header->length = 50;
				header->sender_tid = tid;
				header->type = 100;
			
				char body_msg = 'D'; 
				send_buf[
				if ( tsk_gettid() != 1 )
				{
					// Send the message
					ret_val = send_msg(1, send_buf);  // create a mailbox for itself
					(*p_index)++;
					sprintf(g_ae_xtest.msg, "TID = %u, task1: Sending message of 50 bytes to TID=%u", tid, g_tids[0]);
					sub_result = (ret_val == RTX_OK) ? 1 : 0;
					process_sub_result(test_id, *p_index, sub_result);
				}
				*/
    }

    tsk_exit();
}

void task2(void)
{   
    while(1) {
			tsk_yield();
    }
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
