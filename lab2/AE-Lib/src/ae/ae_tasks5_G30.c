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
#define			TOTAL_ITERATIONS	5
#define     NUM_INIT_TASKS  3       // number of tasks during initialization
TASK_INIT   g_init_tasks[NUM_INIT_TASKS];
const char   PREFIX[]      = "G30-TS5";
const char   PREFIX_LOG[]  = "G30-TS5-LOG";
const char   PREFIX_LOG2[] = "G30-TS5-LOG2";

U8 g_buf1[BUF_LEN];
U8 g_buf2[BUF_LEN];
task_t g_tasks[MAX_TASKS];

uint32_t num_tests_failed = 0;
uint32_t num_tests_passed = 0;
uint8_t num_iterations = 0;

void task3(void)
{
    uart1_put_string("LS task: entering \r\n");
    /* do something */
		task_t buf[10] = {0};
		int num_tasks = tsk_ls(buf, 10);
		
		printf("LS task: printing active task IDs...\r\n");
		
		for ( int i = 0; i < 10; i++)
			printf("%d\r\n", buf[i]);
		
		// default null task + current task
		if (num_tasks > (2))
			printf("%s Test failed - not all tasks exited\r\n", PREFIX);
		
		else
			printf("%s Test passed - all tests that could exit have exited\r\n", PREFIX);
		
		printf("%s Overall stats: %d/%d\r\n", PREFIX, num_tests_passed, (num_tests_passed + num_tests_failed) );
		
    /* terminating */
    tsk_exit();
}

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
    tasks[1].ptask = &task2;
		
		tasks[2].u_stack_size = PROC_STACK_SIZE;
		tasks[2].prio = LOWEST;
		tasks[2].priv = 1;
		tasks[2].ptask = &task3;
}

/**
 * @brief Testing on IRAM2
 */



/**************************************************************************//**
 * @brief       a task that prints AAAAA, BBBBB, CCCCC,...., ZZZZZ on each line.
 *              It yields the cpu every 6 lines are printed.
 *****************************************************************************/
void priv_task1(void)
{
    int i = 0;
    int j = 0;
    long int x = 0;
    
    task_t tid = tsk_gettid();
    
    printf("priv_task1: TID =%d\r\n", tid);
    while (num_iterations < TOTAL_ITERATIONS) {
        char out_char = 'A' + i%26;
        for (j = 0; j < 5; j++ ) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\n\r");
        
        for ( x = 0; x < DELAY; x++); // some artificial delay
        if ( (++i)%6 == 0 ) {
            uart1_put_string("priv_task1 before yielding cpu.\n\r");
            int ret_val = 10;
            ret_val = tsk_yield();
            uart1_put_string("priv_task1 after yielding cpu.\n\r");
            printf("priv_task1: tid = %d, ret_val=%d\n\r", tid, ret_val);
        }
    }
		
		tsk_exit();
}



/**
 * @brief: a dummy task2
 */
void task2(void)
{
    task_t tid;
    RTX_TASK_INFO task_info;
		task_t buf[5];
    uart1_put_string("task2: entering \r\n");
		printf("task2: TID =%d\r\n", tsk_gettid());
    /* do something */
		
		if (num_iterations < TOTAL_ITERATIONS)
			tsk_create(&tid, &task2, HIGH, 0x200);  /*create a user task */
		
		num_iterations++;
		
		if (errno != NULL)
		{
			errno = NULL;
			num_tests_failed++;
		}
		else
			num_tests_passed++;
		
		uint8_t num_tasks = tsk_ls(buf, 5);

		
		tsk_get(tid, &task_info);
    
		if ( (tid - 1) == tsk_gettid() || (tid + 1) != tsk_gettid() )
			num_tests_passed++;
		else
			num_tests_failed++;
		
		if (errno != NULL)
		{
			errno = NULL;
			num_tests_failed++;
		}
		else
			num_tests_passed++;
		
		/* terminating */
    tsk_exit();
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
