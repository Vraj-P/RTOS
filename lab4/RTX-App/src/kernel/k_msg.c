/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTX LAB
 *
 *                     Copyright 2020-2022 Yiqing Huang
 *                          All rights reserved.
 *---------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------*/


/**************************************************************************//**
 * @file        k_msg.c
 * @brief       kernel message passing routines
 * @version     V1.2021.06
 * @authors     Yiqing Huang
 * @date        2021 JUN
 *****************************************************************************/

#include "k_inc.h"
#include "k_rtx.h"
#include "k_msg.h"
#include "msg_buf.h"
#include "k_task.h"

/**
 * @brief Empties as many messages from the mailbox waiting list as possible
 * @param TCB task - TCB whose mailbox we want to unblock messages from
 * @return 1 if we unblocked any tasks, 0 otherwise
*/
uint8_t k_mbx_empty_pq(msg_buf_t * mbx) {
    uint8_t task_unblocked = 0;

    // For each level in the waiting queue
    for (int i = 0; i < K_TASK_NUM_PRIO; ++i) {
        FL_Node_t *blocked_msg_node = mbx->msg_pq_arr[i];
        if (blocked_msg_node == NULL)
            continue;

        msg_t *blocked_msg = blocked_msg_node->current;

        // If we cannot schedule the next element in line, do not schedule anything
        if (blocked_msg->header.length > msg_buf_space(mbx))
            break;

        // Remove the element from the priority queue array
        FL_Pop(&mbx->msg_pq_arr[i]);

        // Deallocate the dynamically allocated freelist node
        k_mpool_dealloc(MPID_IRAM2, blocked_msg_node);

        // Place the blocked message into the receivers mailbox
        msg_buf_send(mbx, *blocked_msg);

        if ( blocked_msg->header.sender_tid < MAX_TASKS )
        {
            // Unblock and schedule the first-in-line task
            g_tcbs[blocked_msg->header.sender_tid].state = READY;
            scheduler_triage(&g_tcbs[blocked_msg->header.sender_tid], 0);

            // Set a flag if we unblocked a TCB
            task_unblocked = 1;

            // Reset the unblocked TCBs blk_tsk field
            g_tcbs[blocked_msg->header.sender_tid].blk_tsk = blocked_msg->header.sender_tid;
        }

        // In case there is still elements in the current priority level
        --i;
    }

    // Only run a new task if we are not currently in an IRQ
    if (task_unblocked && g_current_irq == NO_IRQ)
        k_tsk_run_new(READY, 1);

    // Return if we unblocked any TCBs
    return task_unblocked;
}

int k_mbx_sender_unblock_receiver(task_t receiver_tid, task_t sender_tid) {
	
		if (receiver_tid == TID_UART) {
			return RTX_OK;
		}
		
    // If the receiving task is blocked, unblock it and run!
    if (g_tcbs[receiver_tid].state == BLK_RECV) {
        g_tcbs[receiver_tid].state = READY;

        // Schedule the receiving task at the back of its queue
        scheduler_triage(&g_tcbs[receiver_tid], 0);

        // If the sender was not a task, then do not reschedule
        if ( sender_tid >= MAX_TASKS )
            return RTX_OK;

        // If the mailbox sending task-waiting list is not empty, The scheduler then makes new scheduling decision.
        if (g_tcbs[sender_tid].prio > g_tcbs[receiver_tid].prio) {
            k_tsk_run_new(READY, 1);
        }

        // If the mailbox sending task-waiting list is not empty, The scheduler then makes new scheduling decision.
        else {
            U8 run_new = 0;
            for (int i = 0; i < K_TASK_NUM_PRIO; ++i) {
                if (g_tcbs[receiver_tid].mailbox.msg_pq_arr[i] != NULL)
                    run_new = 1;
            }
            if (run_new)
                k_tsk_run_new(READY, 1);
        }

    }
    return RTX_OK;
}

int k_mbx_create(size_t size) {
#ifdef DEBUG_0
    printf("k_mbx_create: size = %u\r\n", size);
#endif /* DEBUG_0 */

    // Verify parameters
    if (size < MIN_MSG_SIZE) {
        errno = EINVAL;
        return RTX_ERR;
    }

    // Get the active mailbox
    msg_buf_t * mbx = g_current_irq == NO_IRQ ? &gp_current_task->mailbox : &g_irq_mbxs[g_current_irq - IRQ_ID_OFFSET];
    mbx_t mbx_id = g_current_irq == NO_IRQ ? gp_current_task->tid : g_current_irq;
		
    // Check if there is already a mailbox in this task
    if (mbx->status == MSG_BUF_ACTIVE) {
        errno = EEXIST;
        return RTX_ERR;
    }

    // Create the circular buffer of the correct size
    if (msg_buf_init(mbx, mbx_id, size) != MSG_BUF_OK) {
        errno = ENOMEM;
        return RTX_ERR;
    }

    mbx->status = MSG_BUF_ACTIVE;

    // Succesfully created
    return RTX_OK;
}

int k_send_msg(task_t receiver_tid, const void *buf) {
#ifdef DEBUG_0
    printf("k_send_msg: receiver_tid = %d, buf=0x%x\r\n", receiver_tid, buf);
#endif /* DEBUG_0 */

    // Check that we can operate on the buffer
    if (buf == NULL) {
        errno = EFAULT;
        return RTX_ERR;
    }

    const RTX_MSG_HDR *header = buf;

    // Check for invalid size
    if ( header->length < MIN_MSG_SIZE || receiver_tid == TID_NULL || receiver_tid == TID_UNK ) {
        errno = EINVAL;
        return RTX_ERR;
    }

    // Check if the receiver TID is invalid or in an invalid range
    if ( receiver_tid >= MAX_TASKS && receiver_tid <= NO_IRQ )
    {
        errno = EINVAL;
        return RTX_ERR;
    }

    // Get the receiver mailbox
    msg_buf_t * mbx = receiver_tid < MAX_TASKS ? &g_tcbs[receiver_tid].mailbox : &g_irq_mbxs[receiver_tid-IRQ_ID_OFFSET];

    // Check that the mailbox exists
    if (mbx->status != MSG_BUF_ACTIVE) {
        errno = ENOENT;
        return RTX_ERR;
    }

    // Check for message size too large
    if (mbx->buf.max_size < header->length) {
        errno = EMSGSIZE;
        return RTX_ERR;
    }

    // Load the header into the message struct
    msg_t message = {0};
    message.header = *header;

    // Get the priority of the sender if the sender is not an IRQ handler
    uint8_t sender_prio = 0xFF;
    if ( message.header.sender_tid < MAX_TASKS )
        sender_prio = g_tcbs[message.header.sender_tid].prio;

    // Check if we should assign the data pointer to NULL or to the data attached
    if (message.header.length > MSG_HDR_SIZE)
        message.data = (uint8_t * )(buf) + MSG_HDR_SIZE;
    else
        message.data = NULL;

    // Check if we should block because the mailbox is full or the priority queue is null
    // NOTE: This will fail if the sender prio is invalid! IE for IRQs
    if ( msg_buf_space(mbx) < header->length
        || msg_buf_pq_full(mbx, sender_prio - TASK_BASE) == MSG_BUF_FULL )
    {
        // Block sending task until there is enough room to recieve the message
        g_tcbs[header->sender_tid].state = BLK_SEND;

        // Record the task id of the task mailbox whos mailbox the sender id will be blocked on
        g_tcbs[header->sender_tid].blk_tsk = receiver_tid;

        // Allocaate memory for a message that will persist when ours goes out of scope
        msg_t *persistent_msg = k_mpool_alloc(MPID_IRAM2, sizeof(msg_t));
        persistent_msg->header = *header;
        persistent_msg->data = message.data;

        // Add the sending task id to the blocked list of the mailbox
        if (k_tsk_msg_triage(receiver_tid, persistent_msg) == RTX_ERR) {
            errno = EINVAL;
            return RTX_ERR;
        }

        k_tsk_run_new(BLK_SEND, 0);

        // Deallocate the persistent message
        k_mpool_dealloc(MPID_IRAM2, persistent_msg);

        // When coming back from being blocked check if the mailbox is still exists
        if (g_tcbs[receiver_tid].mailbox.status != MSG_BUF_ACTIVE) {
            // The receivers mailbox has been destroyed
            errno = ENOENT;
            return RTX_ERR;
        }

            // Otherwise, the message was succesfully copied by the receiving task.
        else {
            // If the receiving task is blocked, unblock it and run!
            k_mbx_sender_unblock_receiver(receiver_tid, header->sender_tid);
            return RTX_OK;
        }
    }

    // Send the message
    if (msg_buf_send(mbx, message) != MSG_BUF_OK) {
        errno = ENOSPC;
        return RTX_ERR;
    }

    // If the receiving task is blocked, unblock it and run!
    k_mbx_sender_unblock_receiver(receiver_tid, header->sender_tid);

    return RTX_OK;
}

int k_send_msg_nb(task_t receiver_tid, const void *buf) {
#ifdef DEBUG_0
    printf("k_send_msg_nb: receiver_tid = %d, buf=0x%x\r\n", receiver_tid, buf);
#endif /* DEBUG_0 */

    // Check that we can operate on the buffer
    if (buf == NULL) {
        errno = EFAULT;
        return RTX_ERR;
    }

    const RTX_MSG_HDR *header = buf;

    // Check for invalid inputs
    if (header->length<MIN_MSG_SIZE) {
        errno = EINVAL;
        return RTX_ERR;
    }

    // Check if the receiver TID is invalid or in an invalid range
    if ( receiver_tid == TID_NULL || receiver_tid == TID_UNK || (receiver_tid >= MAX_TASKS && receiver_tid <= NO_IRQ) )
    {
        errno = EINVAL;
        return RTX_ERR;
    }

    // Get the receiver mailbox
    msg_buf_t * mbx = receiver_tid < MAX_TASKS ? &g_tcbs[receiver_tid].mailbox : &g_irq_mbxs[receiver_tid-IRQ_ID_OFFSET];


    // Check that the mailbox exists
    if (mbx->status != MSG_BUF_ACTIVE) {
        errno = ENOENT;
        return RTX_ERR;
    }

    // Check for message size too large
    if (mbx->buf.max_size < header->length) {
        errno = EMSGSIZE;
        return RTX_ERR;
    }

    // Check if there is enough room in the message buffer
    if (msg_buf_space(mbx) < header->length) {
        errno = ENOSPC;
        return RTX_ERR;
    }

    // Load the message into the message struct
    msg_t message = {0};
    message.header = *header;
    if (message.header.length > sizeof(RTX_MSG_HDR))
        message.data = (uint8_t * )(buf) + sizeof(RTX_MSG_HDR);
    else
        message.data = NULL;

    // Send the message
    if (msg_buf_send(mbx, message) != MSG_BUF_OK) {
        errno = ENOSPC;
        return RTX_ERR;
    }

    // If the receiving task is blocked, unblock it and run!
    k_mbx_sender_unblock_receiver(receiver_tid, header->sender_tid);

    // Success
    return RTX_OK;

}

int k_recv_msg(void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg: buf=0x%x, len=%d\r\n", buf, len);
#endif /* DEBUG_0 */

    // Check for possible segfault
    if (buf == NULL) {
        errno = EFAULT;
        return RTX_ERR;
    }

    // Get the active mailbox
    msg_buf_t * mbx = g_current_irq == NO_IRQ ? &gp_current_task->mailbox : &g_irq_mbxs[g_current_irq - IRQ_ID_OFFSET];

    // Check if the mailbox is active
    if (mbx->status != MSG_BUF_ACTIVE) {
        errno = ENOENT;
        return RTX_ERR;
    }

    // Check that there is at least 1 message to retrieve
    if (msg_buf_check_space_used(mbx, MIN_MSG_SIZE) != MSG_BUF_OK) {
        // If there is no message, block!
        k_tsk_run_new(BLK_RECV, 0);
    }

    // Retreive a message
    MSG_BUF_STATUS recv_stat = msg_buf_recieve(mbx, buf, len);

    // Check if our buffer was too small
    if (recv_stat == MSG_BUF_SMALL) {
        errno = ENOSPC;
        return RTX_ERR;
    }

        // If we failed for a different reason do not continue
    else if (recv_stat != MSG_BUF_OK)
        return RTX_ERR;

    // Deliver as many messages as possible then make a new scheduling decision if we freed any
    k_mbx_empty_pq(mbx);

    return RTX_OK;
}

int k_recv_msg_nb(void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg_nb: buf=0x%x, len=%d\r\n", buf, len);
#endif /* DEBUG_0 */

    // Check for possible segfault
    if (buf == NULL) {
        errno = EFAULT;
        return RTX_ERR;
    }

    // Get the active mailbox
    msg_buf_t * mbx = g_current_irq == NO_IRQ ? &gp_current_task->mailbox : &g_irq_mbxs[g_current_irq - IRQ_ID_OFFSET];

    // Check if the mailbox is active
    if (mbx->status != MSG_BUF_ACTIVE) {
        errno = ENOENT;
        return RTX_ERR;
    }

    // Check that there is at least 1 message to retrieve
    if (msg_buf_check_space_used(mbx, MIN_MSG_SIZE) != MSG_BUF_OK) {
        errno = ENOMSG;
        return RTX_ERR;
    }

    // Retreive a message
    MSG_BUF_STATUS recv_stat = msg_buf_recieve(mbx, buf, len);

    // Check if our buffer was too small
    if (recv_stat == MSG_BUF_SMALL) {
        errno = ENOSPC;
        return RTX_ERR;
    }

        // If we failed for a different reason do not continue
    else if (recv_stat != MSG_BUF_OK)
        return RTX_ERR;

    // Deliver as many messages as possible then make a new scheduling decision if we freed any
    k_mbx_empty_pq(mbx);

    return RTX_OK;
}

int k_mbx_ls(task_t *buf, size_t count) {
#ifdef DEBUG_0
    printf("k_mbx_ls: buf=0x%x, count=%u\r\n", buf, count);
#endif /* DEBUG_0 */
    if (buf == NULL) {
        errno = EFAULT;
        return RTX_ERR;
    }
    
    size_t task_count = 0;
    for (size_t i = 0 ; i < MAX_TASKS ; i++) {
        if (g_tcbs[i].mailbox.status == MSG_BUF_ACTIVE) {
            if (task_count < count) {
                buf[task_count] = (task_t)g_tcbs[i].tid;
            }
            else
                break;
            task_count++;
        }
    }
    return task_count;
}

int k_mbx_get(task_t tid) {
#ifdef DEBUG_0
    printf("k_mbx_get: tid=%u\r\n", tid);
#endif /* DEBUG_0 */
    if (g_tcbs[tid].mailbox.status != MSG_BUF_ACTIVE) {
        errno = ENOENT;
        return RTX_ERR;
    }

    return msg_buf_space(&g_tcbs[tid].mailbox);
}

int k_mbx_destroy(uint8_t mbx_id) {
#ifdef DEBUG_0
    printf("k_mbx_get: tid=%u\r\n", tid);
#endif /* DEBUG_0 */

    // Check if the receiver TID is invalid or in an invalid range
    if ( mbx_id == TID_NULL || mbx_id == TID_UNK || (mbx_id >= MAX_TASKS && mbx_id <= NO_IRQ) )
    {
        return RTX_ERR;
    }

    // Get the receiver mailbox
    msg_buf_t * mbx = mbx_id < MAX_TASKS ? &g_tcbs[mbx_id].mailbox : &g_irq_mbxs[mbx_id-IRQ_ID_OFFSET];

    // Check that the status is active
    if ( mbx->status == MSG_BUF_INACTIVE )
        return RTX_ERR;

    // Unblock all of the tasks waiting the message priority queue
    for (int i = 0; i < K_TASK_NUM_PRIO; i++) {
        // Get the blocked message
        FL_Node_t *blocked_msg_node = mbx->msg_pq_arr[i];
        if (blocked_msg_node == NULL)
            continue;

        msg_t *blocked_msg = blocked_msg_node->current;

        // Deallocate the freelist node
        k_mpool_dealloc(MPID_IRAM2, blocked_msg_node);

        // Remove the blocked msg from the mailbox priority queue
        FL_Pop(&blocked_msg_node);

        // Schedule the blocked TCB
        if ( blocked_msg->header.sender_tid < MAX_TASKS )
        {
            TCB *blocked_tcb = &g_tcbs[blocked_msg->header.sender_tid];
            blocked_tcb->state = READY;
            scheduler_triage(blocked_tcb, 0);
        }

        // Decrement iterator i since there may be more in the same level priority queue
        --i;
    }

    if (msg_buf_destroy(mbx) != MSG_BUF_OK)
        return RTX_ERR;

    return RTX_OK;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
