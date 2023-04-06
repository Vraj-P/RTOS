/**
 * @brief The KCD Task Template File
 * @note  The file name and the function name can be changed
 * @see   k_tasks.h
 */

#include "rtx.h"
#include "common.h"
#include "k_task.h"
#include "msg_buf.h"

#define NO_COMMAND "Command not found\r"
#define INVALID_COMMAND "Invalid Command\r"

void print_all_tasks(void);
void print_mailbox_tasks(void);
void print_error(char type[]);

void wall_clock_reset(void);
void wall_clock_set(char* new_time);
void wall_clock_terminate(void);

typedef struct command
{
    struct command *next; // try static linked list, if doesnt owrk change to something else
    char *cmd;
    int length;
    task_t task;
} CMD;

void task_kcd(void) // takes all the messages and does something
{
    // get mailbox
    mbx_create(KCD_MBX_SIZE); // might not need to create depending on how mailbox is implemented

    CMD *cmds = NULL;
    char cmd_buf[KCD_CMD_BUF_SIZE] = "";
    int cmd_cur_size = 0;

    while (1)
    {
        int i = 1;
        void *buf = NULL;
        while (1)
        {
            buf = mem_alloc(MSG_HDR_SIZE + MIN_BLK_SIZE * i);
            if (recv_msg(buf, MSG_HDR_SIZE + MIN_BLK_SIZE * i) == RTX_OK)
            { // no space
                break;
            }
            mem_dealloc(buf);
            i++;
        }
        RTX_MSG_HDR *msg = (void *)buf;

        if (msg->type == KCD_REG)
        {
            char *cmd = mem_alloc(msg->length - MSG_HDR_SIZE);
            sprintf(cmd, (char *)buf + MSG_HDR_SIZE);
            if (cmd[0] == 'L' || cmd[0] == 'W')
            {
                continue;
            }

            CMD *command = mem_alloc(sizeof(CMD));
            command->next = cmds;
            command->cmd = cmd;
            command->length = msg->length - MSG_HDR_SIZE;
            command->task = msg->sender_tid;
            cmds = command;
        }
        else if (msg->type == KEY_IN)
        { // user typing command in
            // send msg to console
            msg->type = DISPLAY;
            msg->sender_tid = tsk_gettid();
            send_msg(TID_CON, (void *)msg);
            // queue input keys until enter
            char cmd = *((char *)buf + MSG_HDR_SIZE);
            if (cmd != '\r' && cmd_cur_size < KCD_CMD_BUF_SIZE)
            {
                cmd_buf[cmd_cur_size++] = cmd;
                continue;
            }

            // manage commands
            if (cmd_cur_size < KCD_CMD_BUF_SIZE && cmd_buf[0] == '%')
            {
                if (cmd_cur_size == 3 && cmd_buf[1] == 'L' && cmd_buf[2] == 'T')
                {
                    print_all_tasks();
                }
                else if (cmd_cur_size == 3 && cmd_buf[1] == 'L' && cmd_buf[2] == 'M')
                {
                    print_mailbox_tasks();
                }
                else if (cmd_cur_size == 3 && cmd_buf[1] == 'W' && cmd_buf[2] == 'R')
                {
                    wall_clock_reset();
                }
                else if (cmd_cur_size == 12 && cmd_buf[1] == 'W' && cmd_buf[2] == 'S')
                {
                    wall_clock_set(cmd_buf + 2);
                }
                else if (cmd_cur_size == 3 && cmd_buf[1] == 'W' && cmd_buf[2] == 'T')
                {
                    wall_clock_terminate();
                }
                else
                {
                    // find registed command if exists
                    sprintf(cmd_buf, cmd_buf + 1);

                    CMD *curr = cmds;
                    while (curr != NULL)
                    {
                        int found_flag = 1;
                        for (int i = 0; i < curr->length; i++)
                        {
                            if (curr->cmd[i] != cmd_buf[i])
                            {
                                found_flag = 0;
                                break;
                            }
                        }
                        if (found_flag)
                        {
                            break;
                        }
                        curr = curr->next;
                    }

                    if (curr == NULL)
                    {
                        print_error(NO_COMMAND);
                        sprintf(cmd_buf, "");
                        cmd_cur_size = 0;
                        continue;
                    }

                    cmd_buf[cmd_cur_size - 1] = 0;

                    char *buf_send = mem_alloc(MSG_HDR_SIZE + cmd_cur_size - 1);
                    RTX_MSG_HDR *rtx_msg_hdr = (void *)buf_send;
                    rtx_msg_hdr->length = MSG_HDR_SIZE + cmd_cur_size - 1;
                    rtx_msg_hdr->type = KCD_CMD;
                    rtx_msg_hdr->sender_tid = tsk_gettid();
                    sprintf(buf_send + MSG_HDR_SIZE, cmd_buf);
                    send_msg(curr->task, (void *)rtx_msg_hdr);
                    mem_dealloc(buf_send);
                }
            }
            else
            {
                print_error(INVALID_COMMAND);
            }
            sprintf(cmd_buf, "");
            cmd_cur_size = 0;
        }
        mem_dealloc(buf);
    }
}

void print_all_tasks(void) {
    for(size_t i = 0; i < MAX_TASKS; i++) {
        if(g_tcbs[i].state != DORMANT) {
            char *buf = mem_alloc(MSG_HDR_SIZE + 25);
            RTX_MSG_HDR *rtx_msg_hdr = (void *)buf;
            rtx_msg_hdr->length = MSG_HDR_SIZE + 25;
            rtx_msg_hdr->type = DISPLAY;
            rtx_msg_hdr->sender_tid = tsk_gettid();
            sprintf(buf + MSG_HDR_SIZE, "tid: %d, state: %d \r", g_tcbs[i].tid, g_tcbs[i].state);
            send_msg(TID_CON, (void *) rtx_msg_hdr);
            mem_dealloc(buf);
        }
    }
}

void print_mailbox_tasks(void) {
    for(size_t i = 0; i < MAX_TASKS; i++) {
        if(g_tcbs[i].state != DORMANT) {
            if (g_tcbs[i].mailbox.status == MSG_BUF_ACTIVE) {
                char *buf = mem_alloc(MSG_HDR_SIZE + 30);
                RTX_MSG_HDR *rtx_msg_hdr = (void *)buf;
                rtx_msg_hdr->length = MSG_HDR_SIZE + 30;
                rtx_msg_hdr->type = DISPLAY;
                rtx_msg_hdr->sender_tid = tsk_gettid();
                sprintf(buf + MSG_HDR_SIZE, "tid: %d, state: %d, free: %d\r", g_tcbs[i].tid, g_tcbs[i].state, mbx_get(g_tcbs[i].tid));
                send_msg(TID_CON, (void *) rtx_msg_hdr);
                mem_dealloc(buf);
            }
        }
    }
}


void print_error(char type[]) {
    char *buf = mem_alloc(MSG_HDR_SIZE + 25);
    RTX_MSG_HDR *rtx_msg_hdr = (void *)buf;
    rtx_msg_hdr->length = MSG_HDR_SIZE + 25;
    rtx_msg_hdr->type = DISPLAY;
    rtx_msg_hdr->sender_tid = tsk_gettid();
    sprintf(buf + MSG_HDR_SIZE, type);
    send_msg(TID_CON, (void *) rtx_msg_hdr);
	mem_dealloc(buf);
}

void wall_clock_reset()
{
    printf("in wall clock reset\r\n");
    char *buf = mem_alloc(MSG_HDR_SIZE + 1);
    RTX_MSG_HDR *rtx_msg_hdr = (void *)buf;
    rtx_msg_hdr->length = MSG_HDR_SIZE + 1;
    rtx_msg_hdr->type = KCD_CMD;
    rtx_msg_hdr->sender_tid = tsk_gettid();
    sprintf(buf + MSG_HDR_SIZE, "R");
    int ret = send_msg(TID_WCLCK, (void *)rtx_msg_hdr);
    mem_dealloc(buf);
}

void wall_clock_set(char* new_time)
{
    printf("in wall clock reset\r\n");
    char *buf = mem_alloc(MSG_HDR_SIZE + 10);
    RTX_MSG_HDR *rtx_msg_hdr = (void *)buf;
    rtx_msg_hdr->length = MSG_HDR_SIZE + 10;
    rtx_msg_hdr->type = KCD_CMD;
    rtx_msg_hdr->sender_tid = tsk_gettid();
    sprintf(buf + MSG_HDR_SIZE, new_time);
    int ret = send_msg(TID_WCLCK, (void *)rtx_msg_hdr);
    mem_dealloc(buf);
}

void wall_clock_terminate()
{
    printf("in wall clock terminate\r\n");
    char *buf = mem_alloc(MSG_HDR_SIZE + 1);
    RTX_MSG_HDR *rtx_msg_hdr = (void *)buf;
    rtx_msg_hdr->length = MSG_HDR_SIZE + 1;
    rtx_msg_hdr->type = KCD_CMD;
    rtx_msg_hdr->sender_tid = tsk_gettid();
    sprintf(buf + MSG_HDR_SIZE, "T");
    int ret = send_msg(TID_WCLCK, (void *)rtx_msg_hdr);
    mem_dealloc(buf);
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
