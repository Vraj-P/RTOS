/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
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
 * @file        ae_tasks300.c
 * @brief       P3 Test Suite 300  - Basic Non-blocking Message Passing
 *
 * @version     V1.2021.07
 * @authors     Yiqing Huang
 * @date        2021 Jul
 *
 * @note        Each task is in an infinite loop. These Tasks never terminate.
 *
 *****************************************************************************/

#include "ae.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */

#define     NUM_TESTS       2       // number of tests
#define     NUM_INIT_TASKS  2       // number of tasks during initialization
#define     BUF_LEN         64     // receiver buffer length
#define     MY_MSG_TYPE     100     // some customized message type

/*
 *===========================================================================
 *                             GLOBAL VARIABLES
 *===========================================================================
 */

TASK_INIT    g_init_tasks[NUM_INIT_TASKS];
void task0(void);
void task1(void);
/* The following arrays can also be dynamic allocated to reduce ZI-data size
 *  They do not have to be global buffers (provided the memory allocator has no bugs)
 */
 
 
void set_ae_tasks(TASK_INIT *tasks, int num)
{
    for (int i = 0; i < num; i++ ) {
        tasks[i].u_stack_size = PROC_STACK_SIZE;
        tasks[i].prio = HIGH;
        tasks[i].priv = 0;
    }

    tasks[0].ptask = &task0;
    tasks[1].ptask = &task1;

}

void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num)
{
    *p_num = NUM_INIT_TASKS;
    *pp_tasks = g_init_tasks;
    set_ae_tasks(*pp_tasks, *p_num);
}

void task0(void)
{	
		printf("task0\r\n");
		mbx_create(128);
		char *buf_send = mem_alloc(MSG_HDR_SIZE + 1);
		RTX_MSG_HDR *rtx_msg_hdr = (void *)buf_send;
		rtx_msg_hdr->length = MSG_HDR_SIZE + 1;
		rtx_msg_hdr->type = KCD_REG;
		rtx_msg_hdr->sender_tid = tsk_gettid();
		buf_send += MSG_HDR_SIZE;
		*buf_send = 'k';
		send_msg(TID_KCD, (void *) rtx_msg_hdr);
		mem_dealloc(buf_send);
		
		char *buf = mem_alloc(MSG_HDR_SIZE + 25);
		recv_msg(buf, MSG_HDR_SIZE + 25);
		char msg[25];
		sprintf(msg, buf + MSG_HDR_SIZE);
		printf("task0 got: %s\r\n",msg);
		tsk_exit();
		return;
}

void task1(void)
{
		printf("task1\r\n");
		mbx_create(128);
		char *buf_send = mem_alloc(MSG_HDR_SIZE + 5);
		RTX_MSG_HDR *rtx_msg_hdr = (void *)buf_send;
		rtx_msg_hdr->length = MSG_HDR_SIZE + 1;
		rtx_msg_hdr->type = KCD_REG;
		rtx_msg_hdr->sender_tid = tsk_gettid();
		buf_send += MSG_HDR_SIZE;
		sprintf(buf_send, "hello");
		send_msg(TID_KCD, (void *) rtx_msg_hdr);
		mem_dealloc(buf_send);
		
		char *buf = mem_alloc(MSG_HDR_SIZE + 25);
		recv_msg(buf, MSG_HDR_SIZE + 25);
		char msg[25];
		sprintf(msg, buf + MSG_HDR_SIZE);
		printf("task1 got: %s\r\n",msg);
		tsk_exit();
		return;
}
