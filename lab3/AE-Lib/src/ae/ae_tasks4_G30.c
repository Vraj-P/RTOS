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

const char   PREFIX[]      = "G30-TS4";
const char   PREFIX_LOG[]  = "G30-TS4-LOG";
const char   PREFIX_LOG2[] = "G30-TS4-LOG2";

#define     BUF_LEN         128
#define NUM_TESTS       1       // number of tests
#define NUM_INIT_TASKS  2       // number of tasks during initialization
TASK_INIT   g_init_tasks[NUM_INIT_TASKS];

U8 g_test_over = 0;
U8 g_buf1[BUF_LEN];
U8 g_buf2[BUF_LEN];
task_t g_tasks[MAX_TASKS];

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

uint8_t iterations = 0;


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
    tasks[0].ptask = &priv_task1;
    tasks[1].priv  = 0;
    tasks[1].ptask = &task1;
}

void init_ae_tsk_test(void)
{
    g_ae_xtest.test_id = 0;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests = NUM_TESTS;
    g_ae_xtest.num_tests_run = 0;
    
    for ( int i = 0; i< NUM_TESTS; i++ ) {
        g_tsk_cases[i].p_ae_case = &g_ae_cases[i];
        g_tsk_cases[i].p_ae_case->results  = 0x0;
        g_tsk_cases[i].p_ae_case->test_id  = i;
        g_tsk_cases[i].p_ae_case->num_bits = 0;
        g_tsk_cases[i].pos = 0;  // first avaiable slot to write exec seq tid
        // *_expt fields are case specific, deligate to specific test case to initialize
    }
    printf("%s: START\r\n", PREFIX);
}

void update_ae_xtest(int test_id)
{
    g_ae_xtest.test_id = test_id;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests_run++;
}
void task_null2(void)
{
#ifdef DEBUG_0
    task_t tid = tsk_gettid();
#endif
    while (1) {
#ifdef DEBUG_0
        for ( int i = 0; i < 5; i++ ){
            printf("==============Task NULL: TID = %d ===============\r\n", tid);
        }
#endif  
				tsk_yield();
    }
}
/**
 * @brief Testing on IRAM2
 */

/**
 * @brief: Exit Task
 */
void task3(void)
{
    uart1_put_string("LS task: entering \r\n");
    /* do something */
		task_t buf[10] = {0};
		int num_tasks = tsk_ls(buf, 10);
		
		printf("LS task: printing active task IDs...\r\n");
		
		for ( int i = 0; i < 10; i++)
			printf("%d\r\n", buf[i]);
		
		// 7 null tasks + default null task + current task
		if (num_tasks > (7 + 1 + 1))
			printf("%s Test failed - not all tasks exited\r\n", PREFIX);
		
		else
			printf("%s Test passed - all tests that could exit have exited\r\n", PREFIX);
    /* terminating */
    tsk_exit();
}
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
    while (g_test_over == 0) {
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
		task_t new_tid = 0;
		tsk_create(&new_tid , task3, LOW, 1);
		tsk_exit();
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
void task1(void)
{
    long int x = 0;
    int i = 0;
    int j = 0;
    static RTX_TASK_INFO task_info; /* our stack space is small, so make it static local */
    task_t tid;

    int *ptr = mem_alloc(sizeof(int));
    printf("ptr = 0x%x\r\n", ptr); 
    mem_dealloc(ptr);
    mem_dump();

    tid = tsk_gettid();
    printf("task1: TID =%d\r\n", tid); 
    for (i = 1; g_test_over == 0;i++) {
        char out_char = '0' + i%10;
        for (j = 0; j < 5; j++ ) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\n\r");
        
        for ( x = 0; x < DELAY; x++); // some artifical delay
        if ( i%6 == 0 ) {
            uart1_put_string("task1 before yielding cpu.\n\r");
            int ret_val = 10;
            ret_val = tsk_yield();
            uart1_put_string("task1 after yielding cpu.\n\r");
            printf("task1: tid = %d, ret_val=%d\n\r", tid, ret_val);
        }
				
				printf("task1: tid = %d, **CREATING NEW TASK**\r\n", tid);
				task_t new_tid;
				tsk_create(&new_tid, task_null2, LOWEST, 1);
				if (errno == 11) {
					g_test_over = 1;
					tsk_exit();
				}
    }

}

/**
 * @brief: a dummy task2
 */
void task2(void)
{
    task_t tid;
    RTX_TASK_INFO task_info;
		task_t buf[9];
    uart1_put_string("task2: entering \r\n");
		printf("task2: TID =%d\r\n", tsk_gettid());
    /* do something */
	
    tsk_create(&tid, &task2, HIGH, 0x200);  /*create a user task */
		if (errno != NULL)
		{
			printf("task2 CREATE ERROR: TID=%d, ERRNO=%d\r\n", tsk_gettid(), errno);
			errno = NULL;
		}
        tsk_ls(buf, 9);

		tsk_create(&tid, &task2, HIGH, 0x200);  /*create a user task */
		if (errno != NULL)
		{
			printf("task2 CREATE ERROR: TID=%d, ERRNO=%d\r\n", tsk_gettid(), errno);
			errno = NULL;
		}
		tsk_ls(buf, 5);
		
		tsk_get(tid, &task_info);
    if (errno != NULL)
		{
			printf("task2 GET ERROR: TID=%d, ERRNO=%d\r\n", tsk_gettid(), errno);
			errno = NULL;
		}
		/* terminating */
    tsk_exit();
		if (errno != NULL)
		{
			printf("task2 ERROR: TID=%d, ERRNO=%d\r\n", tsk_gettid(), errno);
			errno = NULL;
		}
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
