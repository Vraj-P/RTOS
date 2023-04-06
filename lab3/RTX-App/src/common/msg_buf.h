//
// Created by Karim Alatrash on 2022-06-20.
//

/*
 *===========================================================================
 *                             START OF FILE
 *===========================================================================
 */
#ifndef ECE350_SPRING2022_LAB_G30_MSG_BUF_H
#define ECE350_SPRING2022_LAB_G30_MSG_BUF_H

#include "cbuf.h"
#include "common.h"
#include "freelist.h"

/*
 *===========================================================================
 *                             ENUMS
 *===========================================================================
 */

typedef enum {
    MSG_BUF_OK = 0,
    MSG_BUF_FULL,
    MSG_BUF_FAIL,
    MSG_BUF_SMALL,
} MSG_BUF_STATUS;

typedef enum {
    MSG_SEND_OK = 0,
    MSG_SEND_PENDING,
    MSG_SEND_FAIL,
} MSG_SEND_STATUS;

typedef enum {
    MSG_BUF_INACTIVE = 0,
		MSG_BUF_ACTIVE,
} MSG_BUF_ACTIVITY;

/*
 *===========================================================================
 *                             TYPE DEFINITIONS
 *===========================================================================
 */
typedef struct msg_buf {
    mbx_t id;
    MSG_BUF_ACTIVITY status;
    FL_Node_t *msg_pq_arr[K_TASK_NUM_PRIO]; // Stores a free list of messages associated with the sender TCB
    cbuf_t buf;
} msg_buf_t;

typedef struct msg {
    RTX_MSG_HDR header;
    void *data;
} msg_t;

/*
 *===========================================================================
 *                             FUNCTION HEADERS
 *===========================================================================
 */

/**
 * @details Initialises a message buffer with some ID
 * @param mailbox Message buffer to initialise
 * @param id ID of the message buffer
 * @param size Size of the message buffer
 */
MSG_BUF_STATUS msg_buf_init(msg_buf_t *mailbox, mbx_t id, uint32_t size);

/**
 * @details Safely destroys message buffer
 * @param mailbox
 */
MSG_BUF_STATUS msg_buf_destroy(msg_buf_t *mailbox);

/**
 * @details Adds a message to the message buf queue
 * @param mailbox
 * @param sender_tid
 * @param length
 * @param type
 * @return MSG_BUF_STATUS
 */
MSG_BUF_STATUS msg_buf_send(msg_buf_t *mailbox, msg_t message);

/**
 * @details Reads one message from the message buffer
 * @param mailbox
 * @param message
 * @return
 */
MSG_BUF_STATUS msg_buf_recieve(msg_buf_t * mailbox, uint8_t * message, uint32_t max_length);

/**
 * @details Finds the size currently available in the mailbox
 * @param mailbox
 * @return Size available in the mailbox
 */
static inline uint32_t msg_buf_space(msg_buf_t *mailbox) {
    return (mailbox->buf.max_size - mailbox->buf.size);
}

static inline MSG_BUF_STATUS msg_buf_check_space_used(msg_buf_t *mailbox, uint32_t min_space_used) {
    return mailbox->buf.size >= min_space_used ? MSG_BUF_OK : MSG_BUF_FAIL;
}

/**
 * @details Returns status on whether or not elements exist in the priority queue at or above the given prio
 * @param mailbox - mailbox to check the priority queue of
 * @param prio - base prio to look at
 * @return MSG_BUF_FULL if there are elements  in the priority queue at or above the given prio, MSG_BUF_OK otherwise
 */
MSG_BUF_STATUS msg_buf_pq_full( msg_buf_t * mailbox, uint8_t prio );

#endif //ECE350_SPRING2022_LAB_G30_MSG_BUF_H

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
