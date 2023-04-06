/**************************************************************************//**
 * @file        ae_tasks8_G30.c
 * @brief       Simple test suite for P2
 *
 * @version     V1.0
 * @authors     Vinay
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
void task2(void);
void task3(void);
void task4(void);
void task5(void);
void task6(void);
void task7(void);
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
		printf("START\r\n");
		
		task_t tid;
		tsk_create(&tid, &task1, HIGH, 0x200);

	
		printf("%d/%d tests PASSED\r\n", passed_tests, passed_tests + failed_tests);
		printf("%d/%d tests FAILED\r\n", failed_tests, passed_tests + failed_tests);
		printf("END\r\n");

    tsk_exit();
}

void task(void){
    uart1_put_string("task: entering \r\n");
    /* do something */
	  //for ( int x = 0; x < 10000000; x++); // some artificial delay
    /* terminating */
    tsk_exit();
}

// TSK_CREATE
// not enough memory to support operation
void task1(void){
    printf( "start task 1\r\n");
    task_t tid;
    tsk_create(&tid, &task2, HIGH, 0x8001);
    if (errno == ENOMEM){
        passed_tests++;
				errno = NULL;
    } else {
        failed_tests++;
    }
	  tsk_create(&tid, &task2, HIGH, 0x200);
    tsk_exit();
}
// system has reached maximum number of tasks
void task2(void){
    printf( "start task 2\r\n");
    task_t tid[11];
    for (int i = 0; i < 11; i++){
        tsk_create(&tid[i], &task, HIGH, 0x1);
    }
    if (errno == EAGAIN){
        passed_tests++;
    } else {
        failed_tests++;
    }
    // I need to get rid of the tasks I created above
    task_t tid2;
    tsk_yield();
	tsk_create(&tid2, &task3, HIGH, 0x200);
    tsk_exit();
}
// prio value is not valid
void task3(void){
    printf( "start task 3\r\n");
    task_t tid;
    tsk_create(&tid, &task, 0x84, 0x200);
    // do I need to exit this task because i reused the tid below?
    if (errno == EINVAL){
        passed_tests++;
    } else {
        failed_tests++;
    }
	tsk_create(&tid, &task4, HIGH, 0x200);
    tsk_exit();
}

// TASK_SET_PRIO
// invalid task ID or invalid priority level, operation is not permitted
void task4(void){
    printf( "start task 4\r\n");
		tsk_set_prio(1,HIGH);
    if (errno == EPERM){
        passed_tests++;
    } else {
        failed_tests++;
    }
    tsk_set_prio(1,0x79);
    if (errno == EINVAL){
        passed_tests++;
    } else {
        failed_tests++;
    }
    tsk_set_prio(10,HIGH);
    if (errno == EINVAL){
        passed_tests++;
    } else {
        failed_tests++;
    }

    task_t tid;
	tsk_create(&tid, &task5, HIGH, 0x200);
    tsk_exit();
}

// TSK_GET
// tsk_id is invalid
void task5(void){
    printf( "start task 6\r\n");
    task_t tid;
    RTX_TASK_INFO buf;
    tsk_get(0, &buf);
    if (errno == EINVAL){
        passed_tests++;
    } else {
        failed_tests++;
    }
    tsk_get(10, &buf);
    if (errno == EINVAL){
        passed_tests++;
    } else {
        failed_tests++;
    }
	tsk_create(&tid, &task6, HIGH, 0x200);
    tsk_exit();
}
// buffer is a null pointer
void task6(void){
    printf( "start task 7\r\n");
    tsk_get(2, NULL);
    if (errno == EFAULT){
        passed_tests++;
    } else {
        failed_tests++;
    }
    task_t tid;
	tsk_create(&tid, &task7, HIGH, 0x200);
    tsk_exit();
}

// TSK_LS
// buf is null or count is 0
void task7(void){
    printf( "start task 8\r\n");
    task_t buf;
    size_t count = 5;
    tsk_ls(NULL, count);
    if (errno == EFAULT){
        passed_tests++;
    } else {
        failed_tests++;
    }
    tsk_ls(&buf, 0);
    if (errno == EFAULT){
        passed_tests++;
    } else {
        failed_tests++;
    }
    tsk_exit();
}
