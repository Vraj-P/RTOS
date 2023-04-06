/**
 * @brief The Wall Clock Display Task Template File
 * @note  The file name and the function name can be changed
 * @see   k_tasks.h
 */

#include "rtx.h"
#include "common.h"
#include "k_task.h"
#include "msg_buf.h"

#define WALL_CLOCK_MBX_SIZE 0x80

/**
 * @brief Time object used to display onto the UART
 */
typedef struct WLCLK_time_object
{
    U8 hour;
    U8 minute;
    U8 second;
} WLCLK_TIME;

void display_wall_clock(WLCLK_TIME time)
{
    char *display_buf = mem_alloc(MSG_HDR_SIZE + 27);
    RTX_MSG_HDR *rtx_msg_hdr = (void *)display_buf;
    rtx_msg_hdr->length = MSG_HDR_SIZE + 27;
    rtx_msg_hdr->type = DISPLAY;
    rtx_msg_hdr->sender_tid = tsk_gettid();
		sprintf(display_buf + MIN_MSG_SIZE, "\033[s\033[0;71H%d%d:%d%d:%d%d\033[u", time.hour/10, time.hour % 10, time.minute/10, time.minute % 10, time.second/10, time.second % 10);
    send_msg(TID_CON, (void *)rtx_msg_hdr);
    mem_dealloc(display_buf);
}

void task_wall_clock(void)
{

    TIMEVAL period;
    period.sec = 0;
    period.usec = 250000;
    k_rt_tsk_set(&period);

    mbx_create(WALL_CLOCK_MBX_SIZE);

    WLCLK_TIME time;
    time.hour = 0;
    time.minute = 0;
    time.second = 0;
    void *recv_buf = NULL;

    U16 period_counter = 0;
    U8 iteration_display_flag;
    U8 display_flag = 1;
    recv_buf = mem_alloc(MSG_HDR_SIZE + MIN_BLK_SIZE);

    while (1)
    {
        int ret = recv_msg_nb(recv_buf, MSG_HDR_SIZE + MIN_BLK_SIZE);

        char* msg_data = ((char *)recv_buf) + MSG_HDR_SIZE;
			
        if (ret == RTX_ERR)
        {
            if (period_counter == 4)
            {
                time.second += 1;

                if (time.second == 60)
                {
                    time.second = 0;
                    time.minute += 1;
                }

                if (time.minute == 60)
                {
                    time.minute = 0;
                    time.hour += 1;
                }

                if (time.hour == 24)
                {
                    time.hour = 0;
                }

                period_counter = 0;

                display_flag = iteration_display_flag;
            }
            else
            {
                period_counter += 1;

                if (display_flag == 1)
                {
                    iteration_display_flag = display_flag;
                }

                display_flag = 0;
            }
        }
        else if (msg_data[0] == 'R')
        {
            time.hour = 0;
            time.minute = 0;
            time.second = 0;

            period_counter = 0;

            display_flag = 1;
        }
        else if (msg_data[0] == 'S')
        {

            // convert S HH:MM:SS to fit into the time object.
            if (msg_data[4] != ':' || msg_data[7] != ':')
                continue;

            // verify what to do if user enters the wrong formula
            U8 hour = (msg_data[2] - '0') * 10 + (msg_data[3] - '0');
            U8 minute = (msg_data[5] - '0') * 10 + (msg_data[6] - '0');
            U8 second = (msg_data[8] - '0') * 10 + (msg_data[9] - '0');

            if (hour > 23 || minute > 59 || second > 59)
            {
                continue;
            }

            time.hour = hour;
            time.minute = minute;
            time.second = second;

            period_counter = 0;

            display_flag = 1;
        }
        else if (msg_data[0] == 'T')
        {
            display_flag = 0;
            iteration_display_flag = 0;

            char *display_buf = mem_alloc(MSG_HDR_SIZE + 27);
            RTX_MSG_HDR *rtx_msg_hdr = (void *)display_buf;
            rtx_msg_hdr->length = MSG_HDR_SIZE + 27;
            rtx_msg_hdr->type = DISPLAY;
            rtx_msg_hdr->sender_tid = tsk_gettid();
		    sprintf(display_buf + MIN_MSG_SIZE, "\033[s\033[0;71H        \033[u");
            send_msg(TID_CON, (void *)rtx_msg_hdr);
            mem_dealloc(display_buf);
        }

        if (display_flag == 1)
        {
            display_wall_clock(time);
        }


        rt_tsk_susp();
    }
		
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
