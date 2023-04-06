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
 * @file        k_task.c
 * @brief       task management C file
 * @version     V1.2021.05
 * @authors     Yiqing Huang
 * @date        2021 MAY
 *
 * @attention   assumes NO HARDWARE INTERRUPTS
 * @details     The starter code shows one way of implementing context switching.
 *              The code only has minimal sanity check.
 *              There is no stack overflow check.
 *              The implementation assumes only three simple tasks and
 *              NO HARDWARE INTERRUPTS.
 *              The purpose is to show how context switch could be done
 *              under stated assumptions.
 *              These assumptions are not true in the required RTX Project!!!
 *              Understand the assumptions and the limitations of the code before
 *              using the code piece in your own project!!!
 *
 *****************************************************************************/


#include "k_inc.h"
#include "k_task.h"
#include "k_rtx.h"
#include "common.h"
#include "k_mem.h"
#include "freelist.h"
#include "math.h"

/**
TODOs
	* "Once a task exits, the memory for its mailbox must be deallocated by the kernel"
	* Update tsk_set_prio to change the priority of a TCB that is blocked
	* Update rtx_init to intialise the mailbox for each TCB
	
*/

/*
 *==========================================================================
 *                            GLOBAL VARIABLES
 *==========================================================================
 */

TCB *gp_current_task = NULL;    // the current RUNNING task
TCB g_tcbs[MAX_TASKS] = {0};          // an array of TCBs
TCB *g_null_task = NULL;       // TCB for the null task
TASK_INIT g_null_task_info;           // The null task info
U32 g_num_active_tasks = 0;     // number of non-dormant tasks
FL_Node_t *g_pq_arr[K_TASK_NUM_PRIO]; // Linked list for the priority queue
/*---------------------------------------------------------------------------
The memory map of the OS image may look like the following:
                   RAM1_END-->+---------------------------+ High Address
                              |                           |
                              |                           |
                              |       MPID_IRAM1          |
                              |   (for user space heap  ) |
                              |                           |
                 RAM1_START-->|---------------------------|
                              |                           |
                              |  unmanaged free space     |
                              |                           |
&Image$$RW_IRAM1$$ZI$$Limit-->|---------------------------|-----+-----
                              |         ......            |     ^
                              |---------------------------|     |
                              |      PROC_STACK_SIZE      |  OS Image
              g_p_stacks[2]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[1]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |                           |  OS Image
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |     |                
             g_k_stacks[15]-->|---------------------------|     |
                              |                           |     |
                              |     other kernel stacks   |     |                              
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |  OS Image
              g_k_stacks[2]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |                      
              g_k_stacks[1]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |
              g_k_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |---------------------------|     |
                              |        TCBs               |  OS Image
                      g_tcbs->|---------------------------|     |
                              |        global vars        |     |
                              |---------------------------|     |
                              |                           |     |          
                              |                           |     |
                              |        Code + RO          |     |
                              |                           |     V
                 IRAM1_BASE-->+---------------------------+ Low Address
    
---------------------------------------------------------------------------*/

/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

/**
 * @brief Inserts a tcb into the priority queue
 * @param new_tcb TCB to insert into the priority queue
 * @return NULL if success, 1 if failure
 * @note You cannot insert a the NULL task into the priority queue
 * @pre new_tcb_node must have the current pointer pointing to a tcb struct
 */
void *scheduler_triage(TCB *new_tcb, U8 preempt) {
    // Init Priority queue local var
    FL_Node_t **pq = NULL;

    // Check invalid argument
    if (new_tcb == NULL || new_tcb->prio > PRIO_DEAD || new_tcb->prio < HIGH)
        return (void *) 1;

        // Null priority does not need to be triaged
    else if (new_tcb->prio == PRIO_NULL || new_tcb->tid == 0)
        return (void *) 0;

        // Get the correct priority queue (0x80 is the ENUM for 0th index)
    else
        pq = &g_pq_arr[new_tcb->prio - TASK_BASE];


    // Set the state to ready to denote that it is no longer running
    new_tcb->state = new_tcb->prio != PRIO_DEAD ? READY : DORMANT;

    // Insert the new tcb node into the priority queue and return its success status
    new_tcb->pq_node.next = NULL;
    new_tcb->pq_node.prev = NULL;

    // The new task should not run before all others
    if (preempt == 0)
        FL_Insert_Back(pq, &(new_tcb->pq_node));
        // We have chosen that this new task should preempt all others
    else
        FL_Insert(pq, &(new_tcb->pq_node));
#ifdef DEBUG_0
    FL_Print(*pq, new_tcb->prio - TASK_BASE);
#endif
    return 0;
}

/**************************************************************************//**
 * @brief   scheduler, pick the TCB of the next to run task
 *
 * @return  TCB pointer of the next to run task
 * @post    gp_curret_task is updated
 * @note    you need to change this one to be a priority scheduler
 *
 *****************************************************************************/
TCB *scheduler(void) {

    // Find the next task we want to run
    FL_Node_t *next_task_node = NULL;
    for (int i = 0; i < (K_TASK_NUM_PRIO - 1); ++i) {
        if (g_pq_arr[i] != NULL) {
            next_task_node = g_pq_arr[i];
            FL_Pop(&g_pq_arr[i]);
            break;
        }
    }

    // Set the next task tcb pointer for later use
    TCB *next_task = NULL;
    if (next_task_node == NULL)
        next_task = g_null_task;
    else
        next_task = next_task_node->current;

    // Record that we have replaced the running task by setting the correct fields
    next_task->state = RUNNING;
    gp_current_task = next_task;

    return gp_current_task;
}

/**
 * @brief Inserts a message into the waiting message queue of the receiver
 * @param task_t receiver_tid - ID of the task whose mailbox we will be triaging a message in
 * @param msg_t message - Pointer to some message that will be triaged
 * @return RTX_OK
 * @note the pointer to the message must not go out of scope or the data will be invalid
*/
int k_tsk_msg_triage(task_t receiver_tid, msg_t *message) {
    if ( receiver_tid >= MAX_TASKS )
        return RTX_ERR;

    FL_Node_t *message_node = k_mpool_alloc(MPID_IRAM2, sizeof(FL_Node_t));
    message_node->next = NULL;
    message_node->prev = NULL;
    message_node->current = message;

    FL_Insert_Back(&g_tcbs[receiver_tid].mailbox.msg_pq_arr[g_tcbs[message->header.sender_tid].prio - TASK_BASE],
                   message_node);

    return RTX_OK;
}

/**
 * @brief initialzie the first task in the system
 */
void k_tsk_init_first(TASK_INIT *p_task) {
    p_task->prio = PRIO_NULL;
    p_task->priv = 0;
    p_task->tid = TID_NULL;
    p_task->ptask = &task_null;
    p_task->u_stack_size = PROC_STACK_SIZE;
}

/**
 * @brief initialzie the KCD task in the system
 */
void k_tsk_init_KCD(TASK_INIT *p_task) {
    p_task->prio = HIGH;
    p_task->priv = 0;
    p_task->tid = TID_KCD;
    p_task->ptask = &task_kcd;
    p_task->u_stack_size = PROC_STACK_SIZE;
}

/**
 * @brief initialzie the CDISP task in the system
 */
void k_tsk_init_CDISP(TASK_INIT *p_task) {
    p_task->prio = HIGH;
    p_task->priv = 1;
    p_task->tid = TID_CON;
    p_task->ptask = &task_cdisp;
    p_task->u_stack_size = PROC_STACK_SIZE;
}



/**************************************************************************//**
 * @brief       initialize all boot-time tasks in the system,
 *
 *
 * @return      RTX_OK on success; RTX_ERR on failure
 * @param       task_info   boot-time task information structure pointer
 * @param       num_tasks   boot-time number of tasks
 * @pre         memory has been properly initialized
 * @post        none
 * @see         k_tsk_create_first
 * @see         k_tsk_create_new
 *****************************************************************************/

int k_tsk_init(TASK_INIT *task, int num_tasks) {
    if (num_tasks > MAX_TASKS - 1) {
        return RTX_ERR;
    }

    TASK_INIT taskinfo;

    k_tsk_init_first(&taskinfo);
    if (k_tsk_create_new(&taskinfo, &g_tcbs[TID_NULL], TID_NULL) == RTX_OK) {
        g_num_active_tasks = 1;
        gp_current_task = &g_tcbs[TID_NULL];

        // Record the null task
        g_null_task = gp_current_task;
    } else {
        g_num_active_tasks = 0;
        return RTX_ERR;
    }
		
		k_tsk_init_KCD(&taskinfo);
    if (k_tsk_create_new(&taskinfo, &g_tcbs[TID_KCD], TID_KCD) == RTX_OK) {
        g_num_active_tasks += 1;
        gp_current_task = &g_tcbs[TID_KCD];
        scheduler_triage(&g_tcbs[TID_KCD], 0);
    } else {
        return RTX_ERR;
    }
        
    k_tsk_init_CDISP(&taskinfo);
    if (k_tsk_create_new(&taskinfo, &g_tcbs[TID_CON], TID_CON) == RTX_OK) {
        g_num_active_tasks += 1;
        gp_current_task = &g_tcbs[TID_CON];
        scheduler_triage(&g_tcbs[TID_CON], 0);
    } else {
        return RTX_ERR;
    }
		

    // create the rest of the tasks
    for (int i = 0; i < num_tasks; i++) {
        TCB *p_tcb = &g_tcbs[i + 1];
        if (k_tsk_create_new(&task[i], p_tcb, i + 1) == RTX_OK) {
            g_num_active_tasks++;

            // Add the task to the priority queue
            scheduler_triage(p_tcb, 0);
        }
    }

    return RTX_OK;
}

/**************************************************************************//**
 * @brief       initialize a new task in the system,
 *              one dummy kernel stack frame, one dummy user stack frame
 *
 * @return      RTX_OK on success; RTX_ERR on failure
 * @param       p_taskinfo  task initialization structure pointer
 * @param       p_tcb       the tcb the task is assigned to
 * @param       tid         the tid the task is assigned to
 *
 * @details     From bottom of the stack,
 *              we have user initial context (xPSR, PC, SP_USR, uR0-uR3)
 *              then we stack up the kernel initial context (kLR, kR4-kR12, PSP, CONTROL)
 *              The PC is the entry point of the user task
 *              The kLR is set to SVC_RESTORE
 *              20 registers in total
 * @note        YOU NEED TO MODIFY THIS FILE!!!
 *****************************************************************************/
int k_tsk_create_new(TASK_INIT *p_taskinfo, TCB *p_tcb, task_t tid) {
    extern U32 SVC_RTE;

    U32 *usp;
    U32 *ksp;

    if (p_taskinfo == NULL || p_tcb == NULL) {
        return RTX_ERR;
    }

    /*---------------------------------------------------------------
     *  Step1: allocate user stack for the task
     *         stacks grows down, stack base is at the high address
     * ATTENTION: you need to modify the following three lines of code
     *            so that you use your own dynamic memory allocator
     *            to allocate variable size user stack.
     * -------------------------------------------------------------*/

    // Allocate a minimum of PROC_STACK_SIZE
    if (p_taskinfo->u_stack_size < PROC_STACK_SIZE) {
        p_tcb->u_sp_base = (U32) k_mpool_alloc(MPID_IRAM2, PROC_STACK_SIZE) + PROC_STACK_SIZE;
        p_tcb->u_stack_size = PROC_STACK_SIZE;
    }

        // Otherwise, allocate the closest power of 2 that can fit p_taskinfo->u_stack_size
    else {
        p_tcb->u_stack_size = upper_base2(p_taskinfo->u_stack_size);
        p_tcb->u_sp_base = (U32)(k_mpool_alloc(MPID_IRAM2, p_tcb->u_stack_size)) + p_tcb->u_stack_size;
    }

    // In the event allocation failed...
    if (p_tcb->u_sp_base == p_tcb->u_stack_size) {
        p_tcb->u_stack_size = 0;
        errno = ENOMEM;
        return RTX_ERR;
    }

    usp = (U32 * )(p_tcb->u_sp_base);


    /*-------------------------------------------------------------------
     *  Step2: create task's thread mode initial context on the user stack.
     *         fabricate the stack so that the stack looks like that
     *         task executed and entered kernel from the SVC handler
     *         hence had the exception stack frame saved on the user stack.
     *         This fabrication allows the task to return
     *         to SVC_Handler before its execution.
     *
     *         8 registers listed in push order
     *         <xPSR, PC, uLR, uR12, uR3, uR2, uR1, uR0>
     * -------------------------------------------------------------*/

    // if kernel task runs under SVC mode, then no need to create user context stack frame for SVC handler entering
    // since we never enter from SVC handler in this case

    *(--usp) = INITIAL_xPSR;             // xPSR: Initial Processor State
    *(--usp) = (U32)(p_taskinfo->ptask);// PC: task entry point

    // uR14(LR), uR12, uR3, uR3, uR1, uR0, 6 registers
    for (int j = 0; j < 6; j++) {

#ifdef DEBUG_0
        *(--usp) = 0xDEADAAA0 + j;
#else
        *(--usp) = 0x0;
#endif
    }

    // allocate kernel stack for the task if one has not already been allocated
    if (p_tcb->k_sp_base == NULL) {
        p_tcb->k_sp_base = (U32)(k_mpool_alloc(MPID_IRAM2, KERN_STACK_SIZE)) + KERN_STACK_SIZE;
        p_tcb->k_stack_size = KERN_STACK_SIZE;
    }
    // Check if there was a succesful allocation
    if (p_tcb->k_sp_base == KERN_STACK_SIZE) {
        errno = ENOMEM;
        return RTX_ERR;
    }

    // Setting the ksp to point to the max value
    ksp = (U32 * )(p_tcb->k_sp_base);


    /*---------------------------------------------------------------
     *  Step3: create task kernel initial context on kernel stack
     *
     *         12 registers listed in push order
     *         <kLR, kR4-kR12, PSP, CONTROL>
     * -------------------------------------------------------------*/
    // a task never run before directly exit
    *(--ksp) = (U32)(&SVC_RTE);
    // kernel stack R4 - R12, 9 registers
#define NUM_REGS 9    // number of registers to push
    for (int j = 0; j < NUM_REGS; j++) {
#ifdef DEBUG_0
        *(--ksp) = 0xDEADCCC0 + j;
#else
        *(--ksp) = 0x0;
#endif
    }

    // put user sp on to the kernel stack
    *(--ksp) = (U32) usp;

    // save control register so that we return with correct access level
    if (p_taskinfo->priv == 1) {  // privileged
        *(--ksp) = __get_CONTROL() & ~BIT(0);
    } else {                      // unprivileged
        *(--ksp) = __get_CONTROL() | BIT(0);
    }

    p_tcb->msp = ksp;
    p_tcb->psp = usp;
    p_tcb->tid = tid;
    p_tcb->state = READY;
    p_tcb->prio = p_taskinfo->prio;
    p_tcb->priv = p_taskinfo->priv;
    p_tcb->pq_node.current = p_tcb;
    p_tcb->ptask = p_taskinfo->ptask;
    p_tcb->blk_tsk = tid;
    p_tcb->mailbox.status = MSG_BUF_INACTIVE;
    p_tcb->mailbox.id = tid;

    return RTX_OK;
}

/**************************************************************************//**
 * @brief       switching kernel stacks of two TCBs
 * @param       p_tcb_old, the old tcb that was in RUNNING
 * @return      RTX_OK upon success
 *              RTX_ERR upon failure
 * @pre         gp_current_task is pointing to a valid TCB
 *              gp_current_task->state = RUNNING
 *              gp_crrent_task != p_tcb_old
 *              p_tcb_old == NULL or p_tcb_old->state updated
 * @note        caller must ensure the pre-conditions are met before calling.
 *              the function does not check the pre-condition!
 * @note        The control register setting will be done by the caller
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * @attention   CRITICAL SECTION
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *****************************************************************************/
__asm void k_tsk_switch(TCB *p_tcb_old)
{
        PRESERVE8
        EXPORT  K_RESTORE
        
        PUSH    {R4-R12, LR}                // save general pupose registers and return address
        MRS     R4, CONTROL                 
        MRS     R5, PSP
        PUSH    {R4-R5}                     // save CONTROL, PSP
        STR     SP, [R0, #TCB_MSP_OFFSET]   // save SP to p_old_tcb->msp
K_RESTORE
        LDR     R1, =__cpp(&gp_current_task)
        LDR     R2, [R1]
        LDR     SP, [R2, #TCB_MSP_OFFSET]   // restore msp of the gp_current_task
        POP     {R4-R5}
        MSR     PSP, R5                     // restore PSP
        MSR     CONTROL, R4                 // restore CONTROL
        ISB                                 // flush pipeline, not needed for CM3 (architectural recommendation)
        POP     {R4-R12, PC}                // restore general purpose registers and return address
}


__asm void k_tsk_start(void)
{
        PRESERVE8
        B K_RESTORE
}



/**************************************************************************//**
 * @brief       run a new thread. The caller becomes old_tcb_state and
 *              the scheduler picks the next ready to run task.
 * @param				U8 old_tcb_state - The state to set the TCB that is being switched out (must be READY or DORMANT)
 * @return      RTX_ERR on error and zero on success
 * @pre         gp_current_task != NULL && gp_current_task == RUNNING
 * @post        gp_current_task gets updated to next to run task
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * @attention   CRITICAL SECTION
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *****************************************************************************/
int k_tsk_run_new(U8 old_tcb_state, U8 preempt) {
    TCB *p_tcb_old = NULL;

    if (gp_current_task == NULL) {
        return RTX_ERR;
    }

    p_tcb_old = gp_current_task;

    // Add the current task to the priority queue
    if (old_tcb_state != BLK_RECV && old_tcb_state != BLK_SEND)
        scheduler_triage(gp_current_task, preempt);

    gp_current_task = scheduler();

    if (gp_current_task == NULL) {
        gp_current_task = p_tcb_old;        // revert back to the old task
        return RTX_ERR;
    }

    // at this point, gp_current_task != NULL and p_tcb_old != NULL
    if (gp_current_task != p_tcb_old) {
        //gp_current_task->state = RUNNING;   // change state of the to-be-switched-in  tcb
        p_tcb_old->state = old_tcb_state;   // change state of the to-be-switched-out tcb
        k_tsk_switch(p_tcb_old);            // switch kernel stacks
    }

    return RTX_OK;
}


/**************************************************************************//**
 * @brief       yield the cpu
 * @return:     RTX_OK upon success
 *              RTX_ERR upon failure
 * @pre:        gp_current_task != NULL &&
 *              gp_current_task->state = RUNNING
 * @post        gp_current_task gets updated to next to run task
 * @note:       caller must ensure the pre-conditions before calling.
 *****************************************************************************/
int k_tsk_yield(void) {
    return k_tsk_run_new(READY, 0);
}

/**
 * @brief   get task identification
 * @return  the task ID (TID) of the calling task
 */
task_t k_tsk_gettid(void) {
    return gp_current_task->tid;
}

/*
 *===========================================================================
 *                             TO BE IMPLEMETED IN LAB2
 *===========================================================================
 */

int k_tsk_create(task_t *task, void (*task_entry)(void), U8 prio, U32 stack_size) {
#ifdef DEBUG_1
    printf("k_tsk_create: entering...\n\r");
    printf("task = 0x%x, task_entry = 0x%x, prio=%d, stack_size = %d\n\r", task, task_entry, prio, stack_size);
#endif /* DEBUG_1 */

    if (g_num_active_tasks >= MAX_TASKS) { //cant create a new task if we alr at max
        errno = EAGAIN;
        return RTX_ERR;
    }

    if (prio > LOWEST || prio < HIGH) { // prio < 0x80 or prio > 0x83
        errno = EINVAL;
        return RTX_ERR;
    }

    TCB *task_n = NULL;
    U8 tid = 1;

    // Look for any previously created tasks
    if (g_pq_arr[PRIO_DEAD - TASK_BASE] != NULL) {
        task_n = g_pq_arr[PRIO_DEAD - TASK_BASE]->current;
        tid = task_n->tid;
        FL_Pop(&g_pq_arr[PRIO_DEAD - TASK_BASE]);
    }
        // We need to create a new task
    else {
        for (tid = 1; tid < MAX_TASKS; ++tid) {
            if (g_tcbs[tid].state == DORMANT) {
                task_n = &(g_tcbs[tid]); //if the index is dormatnt we can make it a new ready task
                break;
            }
        } //now we have the task
    }


    TASK_INIT task_init;
    task_init.ptask = task_entry;
    task_init.u_stack_size = stack_size;
    task_init.tid = tid;
    task_init.prio = prio;
    task_init.priv = 0;

    if (k_tsk_create_new(&task_init, task_n, tid) == RTX_ERR &&
        errno == ENOMEM) { //taking into account no space for task
        return RTX_ERR;
    }

    *task = tid;
    ++g_num_active_tasks;


    if (task_n->prio < gp_current_task->prio) { //priorites written in descending hex values
        scheduler_triage(task_n, 1);
        k_tsk_run_new(READY, 0);
    } else
        scheduler_triage(task_n, 0);

    return RTX_OK;

}

void k_tsk_exit(void) {
#ifdef DEBUG_1
    printf("k_tsk_exiting TID=%d...\n\r", gp_current_task->tid);
#endif /* DEBUG_1 */

    // Deallocate the user stack in mempory pool 2

    k_mpool_dealloc(MPID_IRAM2, (void *) (gp_current_task->u_sp_base - gp_current_task->u_stack_size));
    gp_current_task->u_sp_base = NULL;
    gp_current_task->psp = NULL;
    gp_current_task->u_stack_size = 0;

    // Reset kernel stack pointer
    gp_current_task->msp = (U32 * )(gp_current_task->k_sp_base);

    // Set privilege to 0
    gp_current_task->priv = 0;

    // Set priority to NULL
    gp_current_task->prio = PRIO_DEAD;

    --g_num_active_tasks;

    gp_current_task->pq_node.next = NULL;
    gp_current_task->pq_node.prev = NULL;

    // Destroy the mailbox
    k_mbx_destroy(gp_current_task->tid);

    // Run a new task
    k_tsk_run_new(DORMANT, 0);

    return;
}

int k_tsk_set_prio(task_t task_id, U8 prio) {
#ifdef DEBUG_1
    printf("k_tsk_set_prio: entering...\n\r");
    printf("task_id = %d, prio = %d.\n\r", task_id, prio);
#endif /* DEBUG_1 */

    // Look for invalid arguments
    if (prio < HIGH || prio > LOWEST || task_id > (MAX_TASKS - 1) || task_id == 0) {
#ifdef DEBUG_1
        printf("RTX ERROR: EINVAL in k_tsk_set_prio()\n\r");
#endif /* DEBUG_0 */
        errno = EINVAL;
        return RTX_ERR;
    }
    // Look for privilege errors
    if (gp_current_task->priv == 0 && g_tcbs[task_id].priv == 1) {
#ifdef DEBUG_1
        printf("RTX ERROR: EPERM in k_tsk_set_prio()\n\r");
#endif /* DEBUG_0 */
        errno = EPERM;
        return RTX_ERR;
    }

    FL_Node_t *blk_msg_node = NULL;
    switch (g_tcbs[task_id].state) {
        // If the current running task is changing its own priority
        case RUNNING:
            if (g_tcbs[task_id].prio < prio) {
                g_tcbs[task_id].prio = prio;

                // Run a new task and set the current one to ready
                k_tsk_run_new(READY, 0);
            } else
                g_tcbs[task_id].prio = prio;
            break;

        case READY:
            // Remove it from its current priority queue
            FL_Remove(&g_pq_arr[(g_tcbs[task_id].prio - TASK_BASE)], &(g_tcbs[task_id].pq_node));

            // Change the priority
            g_tcbs[task_id].prio = prio;

            // Add it to the correct priority queue
            scheduler_triage(&g_tcbs[task_id], 0);

            // If we just moved something into a higher priority, then run that!
            if (prio < gp_current_task->prio)
                k_tsk_run_new(READY, 1);
            break;

        case BLK_SEND:

            // Get the head node of the priority queue where the blocked message belonging to task task_id should be stored
            blk_msg_node = g_tcbs[g_tcbs[task_id].blk_tsk].mailbox.msg_pq_arr[g_tcbs[task_id].prio - TASK_BASE];

            // Search for the message belonging to task_id in the correct level of the priority queue
            while (blk_msg_node != NULL && ((msg_t *) blk_msg_node->current)->header.sender_tid != task_id)
                blk_msg_node = blk_msg_node->next;

            // We did not find the blocked message
            if (blk_msg_node == NULL)
                return RTX_ERR;

            // Remove the message from its current place in the priority queue
            FL_Remove(&g_tcbs[g_tcbs[task_id].blk_tsk].mailbox.msg_pq_arr[g_tcbs[task_id].prio - TASK_BASE],
                      blk_msg_node);

            // Move the tasks priority within the mailbox waiting queue
            g_tcbs[task_id].prio = prio;

            // Add message to the correct priority queue
            k_tsk_msg_triage(g_tcbs[task_id].blk_tsk, blk_msg_node->current);

            // Deallocate the old node that contained the message
            k_mpool_dealloc(MPID_IRAM2, blk_msg_node);

            break;

        case BLK_RECV:
            g_tcbs[task_id].prio = prio;
            break;

        case DORMANT:
        default:
            return RTX_OK;
    }

    return RTX_OK;
}

/**
 * @brief   Retrieve task internal information
 * @note    this is a dummy implementation, you need to change the code
 */
int k_tsk_get(task_t tid, RTX_TASK_INFO *buffer) {
#ifdef DEBUG_1
    printf("k_tsk_get: entering...\n\r");
    printf("tid = %d, buffer = 0x%x.\n\r", tid, buffer);
#endif /* DEBUG_1 */
    if (buffer == NULL) {
        errno = EFAULT;
        return RTX_ERR;
    }
    if (tid > 9) {
        errno = EINVAL;
        return RTX_ERR;
    }
    /* The code fills the buffer with some fake task information.
       You should fill the buffer with correct information    */
    TCB task = g_tcbs[tid];
    buffer->tid = task.tid;
    buffer->prio = task.prio;

    buffer->u_stack_size = task.u_stack_size;
    buffer->u_sp_base = task.u_sp_base;

    buffer->k_sp_base = task.k_sp_base;
    buffer->k_stack_size = task.k_stack_size;

    buffer->state = task.state;
    buffer->priv = task.priv;
    buffer->ptask = task.ptask;
    if (gp_current_task->tid != tid) {
        buffer->k_sp = (U32) task.msp;
        buffer->u_sp = (U32) task.psp;
    } else {
        buffer->k_sp = __get_MSP();
        buffer->u_sp = __get_PSP();
    }
    return RTX_OK;
}

int k_tsk_ls(task_t *buf, size_t count) {
#ifdef DEBUG_1
    printf("k_tsk_ls: buf=0x%x, count=%u\r\n", buf, count);
#endif /* DEBUG_1 */
    if (buf == NULL || count == 0) {
        errno = EFAULT;
        return RTX_ERR;
    }

    size_t task_count = 0;
    for (size_t i = 0; i < MAX_TASKS; i++) {
        if (g_tcbs[i].state != DORMANT) {
            if (task_count < count)
                buf[task_count] = (task_t) g_tcbs[i].tid;
            else
                break;
            task_count++;
        }

    }
    return task_count;
}

/*
 *===========================================================================
 *                             TO BE IMPLEMETED IN LAB4
 *===========================================================================
 */
int k_rt_tsk_set(TIMEVAL *p_tv) {
#ifdef DEBUG_0
    printf("k_rt_tsk_set: p_tv = 0x%x\r\n", p_tv);
#endif /* DEBUG_0 */
    return RTX_OK;
}

int k_rt_tsk_susp(void) {
#ifdef DEBUG_0
    printf("k_rt_tsk_susp: entering\r\n");
#endif /* DEBUG_0 */
    return RTX_OK;
}

int k_rt_tsk_get(task_t tid, TIMEVAL *buffer) {
#ifdef DEBUG_0
    printf("k_rt_tsk_get: entering...\n\r");
    printf("tid = %d, buffer = 0x%x.\n\r", tid, buffer);
#endif /* DEBUG_0 */
    if (buffer == NULL) {
        return RTX_ERR;
    }

    /* The code fills the buffer with some fake rt task information. 
       You should fill the buffer with correct information    */
    buffer->sec = 0xABCD;
    buffer->usec = 0xEEFF;

    return RTX_OK;
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

