//
// Created by Karim Alatrash on 2022-06-20.
//

#include "msg_buf.h"

/*
 *===========================================================================
 *                             FUNCTION DEFINITIONS
 *===========================================================================
 */

MSG_BUF_STATUS msg_buf_init(msg_buf_t * mailbox, mbx_t id, uint32_t size)
{
    
    if ( mailbox == NULL || size < MSG_HDR_SIZE )
        return MSG_BUF_FAIL;
    
		// Set the mailbox ID
    mailbox->id = id;
		
		// Set the mailbox's priority queue to all NULL
		for (int i = 0; i < K_TASK_NUM_PRIO; i++) {
			mailbox->msg_pq_arr[i] = NULL;
		}
    
		// Create the circular buffer with the correct size
    if ( cbuf_init(&mailbox->buf, size) != CBUF_OK)
        return MSG_BUF_FAIL;

    // Success case
    return MSG_BUF_OK;
}

MSG_BUF_STATUS msg_buf_destroy(msg_buf_t * mailbox)
{
    // Check arguments
    if (mailbox == NULL)
        return MSG_BUF_FAIL;

    // Deallocate the circular buffer
    if (cbuf_destroy(&mailbox->buf) != CBUF_OK)
        return MSG_BUF_FAIL;

		mailbox->status = MSG_BUF_INACTIVE;
		
    // Success case
    return MSG_BUF_OK;
}

MSG_BUF_STATUS msg_buf_send(msg_buf_t * mailbox, msg_t message)
{
    // Check for valid arguments
    if (mailbox == NULL || message.header.length < MSG_HDR_SIZE )
        return MSG_BUF_FAIL;

    // Check there is enough space to send the message
    if ( message.header.length > msg_buf_space(mailbox) )
        return MSG_BUF_FULL;

    // Add the header to the buffer
    if (cbuf_insert_back( &mailbox->buf, &message.header, sizeof(RTX_MSG_HDR)) != CBUF_OK)
        return MSG_BUF_FAIL;

    // Add the message to the buffer, where the size of the message is 6 bytes less than the length specified in header
    if (cbuf_insert_back( &mailbox->buf, message.data, (message.header.length - sizeof(RTX_MSG_HDR)) ) != CBUF_OK)
        return MSG_BUF_FAIL;

    // Return success
    return MSG_BUF_OK;
}

MSG_BUF_STATUS msg_buf_recieve(msg_buf_t * mailbox, uint8_t * message, uint32_t max_length)
{
    // Check for valid arguments
    if ( mailbox == NULL || message == NULL )
        return MSG_BUF_FAIL;
		
		// Check we have enough room to read the header
		if ( max_length < MSG_HDR_SIZE )
			return MSG_BUF_SMALL;
		
    // Verify there is at least 1 message in the mailbox
    if (msg_buf_check_space_used(mailbox, MSG_HDR_SIZE) != MSG_BUF_OK)
        return MSG_BUF_FAIL;

    // Retrieve the header
    if ( cbuf_remove_front( &mailbox->buf, message, MSG_HDR_SIZE ) != CBUF_OK )
        return MSG_BUF_FAIL;

		RTX_MSG_HDR * header = (RTX_MSG_HDR *)message;
		
		// Check we have enough room to fetch the body
		if ( max_length < header->length )
			return MSG_BUF_SMALL;
		
    // Retrieve the rest of the message as specified by the header
    if ( cbuf_remove_front( &mailbox->buf, (message + MSG_HDR_SIZE), (header->length - MSG_HDR_SIZE) ) != CBUF_OK )
        return MSG_BUF_FAIL;

    // Success
    return MSG_BUF_OK;
}

MSG_BUF_STATUS msg_buf_pq_full( msg_buf_t * mailbox, uint8_t prio_level )
{
    // Return failure if the priority level is out of bounds
    if ( prio_level >= K_TASK_NUM_PRIO)
        return MSG_BUF_FAIL;

    // Search each priority level including the lowest one specified
    for ( int i = 0; i <= prio_level; i++ )
    {
        if ( mailbox->msg_pq_arr[i] != NULL )
            return MSG_BUF_FULL;
    }

    // Success
    return MSG_BUF_OK;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
