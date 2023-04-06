/**
 * @brief The Console Display Task Template File 
 * @note  The file name and the function name can be changed
 * @see   k_tasks.h
 */

#include "rtx.h"
#include "common.h"
#include "uart_def.h"
#include "LPC17xx.h"
#include "printf.h"


void task_cdisp(void)
{
    LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *) LPC_UART0;     
    mbx_create(CON_MBX_SIZE);
    while(1) {
        int i = 1;
        void *buf = NULL;
        while (1) {
            buf = mem_alloc(MSG_HDR_SIZE + MIN_BLK_SIZE * i);
            if (recv_msg(buf, MSG_HDR_SIZE + MIN_BLK_SIZE * i) == RTX_OK) { //no space
                break;
            }
            mem_dealloc(buf);
            i++;
        }
    
        RTX_MSG_HDR *msg = (void *)buf;
        if (msg->type == DISPLAY) {
					msg->sender_tid = tsk_gettid();
					if (send_msg(TID_UART, (void *) msg) == RTX_OK) {
						while (pUart->IER & IER_THRE) {};
						pUart->IER |= IER_THRE;
					}
        } 
        mem_dealloc(buf);
    }
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

