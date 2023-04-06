//
// Created by Karim Alatrash on 2022-06-19.
//

/*
 * ------------------------------------------------------------------------
 *                             FILE BEGIN
 * ------------------------------------------------------------------------
 */

#include "cbuf.h"
#include "k_mem.h"

/*
 * ------------------------------------------------------------------------
 *                             PRIVATE FUNCTION DEFINITION
 * ------------------------------------------------------------------------
 */

/**
 * @details Copies contents from 1 circulat buffer to another.
 * @param dest
 * @param src
 * @param dest_size Max size of the destination
 * @param src_size Max size of the source
 * @param dest_idx Index of the destination you wish to begin copying to
 * @param src_idx Index of the source you wish to begin copying from
 * @param size Number of bytes to copy
 */
void cbuf_deepcopy(uint8_t * dest, uint8_t * src, uint32_t dest_size, uint32_t src_size, uint32_t dest_idx, uint32_t src_idx, uint32_t size)
{
    // Deep copy the buffer contents starting at the back address
    for ( size_t i = 0; i < size; i++ )
        dest[(dest_idx + i) % dest_size] = src[ (i + src_idx) % src_size];
}
/*
 * ------------------------------------------------------------------------
 *                             PUBLIC FUNCTION DEFINITION
 * ------------------------------------------------------------------------
 */


CBUF_STATUS cbuf_init( cbuf_t * circle_buffer, uint32_t size )
{
    // Check for null arguments
    if ( circle_buffer == NULL || size == 0)
        return CBUF_FAIL;

    // Dynamically allocate the circular buffer
    circle_buffer->buf = k_mpool_alloc(MPID_IRAM2, size);

    // Check for errors with the allocation
    if ( circle_buffer->buf == NULL && errno != 0 )
        return CBUF_FAIL;

    // Set the start
    circle_buffer->start = 0;

    // Set the max size
    circle_buffer->max_size = size;

    // Set the size
    circle_buffer->size = 0;

    // Return succesful
    return CBUF_OK;
}

CBUF_STATUS cbuf_destroy( cbuf_t * circle_buffer )
{
    // Check for null arguments
    if ( circle_buffer == NULL )
        return CBUF_FAIL;

    // Deallocating the memory for the circular buffer
    if ( k_mpool_dealloc(MPID_IRAM2, circle_buffer->buf) == RTX_ERR && errno != 0 )
        return CBUF_FAIL;

    // Resetting the variables for this element
    circle_buffer->start = NULL;
    circle_buffer->buf = NULL;
    circle_buffer->size = 0;

    // Return succesful
    return CBUF_OK;
}

CBUF_STATUS cbuf_insert_back(cbuf_t * circle_buffer, void * buffer, uint32_t size)
{
		// If we are told to write 0 bytes, do nothing
		if ( size == 0 )
			return CBUF_OK;
		
    // Check for null arguments
    if ( circle_buffer == NULL || buffer == NULL)
        return CBUF_FAIL;

		
		
    // Check there is enough space
    if ( size + circle_buffer->size < circle_buffer->max_size )
    {
        // If there is enough space, get the address of the back
        uint32_t back = (circle_buffer->start + circle_buffer->size) % circle_buffer->max_size;

        // Deep copy the buffer contents starting at the back address
        cbuf_deepcopy((uint8_t *)circle_buffer->buf, (uint8_t *)buffer, circle_buffer->max_size, size, back, 0, size);

        // Record the size
        circle_buffer->size += size;

        // Return success
        return CBUF_OK;
    }

    // Else, return failure
    return CBUF_FULL;

}

CBUF_STATUS cbuf_remove_front(cbuf_t * circle_buffer, uint8_t * dest_buf, uint32_t size)
{
		// Try and fetch the info from the buffer
    if ( cbuf_peek_front(circle_buffer, dest_buf, size) != CBUF_OK )
			return CBUF_FAIL;
		
    // Advance the start index if we pulled data from the circular buffer
    circle_buffer->start = (circle_buffer->start + size) % circle_buffer->max_size;
		
		// Reduce the size recorded in the circle buffer
		circle_buffer->size -= size;

    return CBUF_OK;
}

CBUF_STATUS cbuf_peek_front( cbuf_t * circle_buffer, uint8_t * dest_buf, uint32_t size )
{
    // Check for null parameters
    if ( circle_buffer == NULL || dest_buf == NULL )
        return CBUF_FAIL;
		
		// If we are told to read 0 bytes, do nothing
		if ( size == 0 )
			return CBUF_OK;
		
    // Check if there is enough bytes in the cbuf to remove
    if ( circle_buffer->size < size )
        return CBUF_FAIL;

    // Deep copy
    cbuf_deepcopy((uint8_t *)dest_buf, (uint8_t *)circle_buffer->buf, size, circle_buffer->max_size, 0, circle_buffer->start, size);

    return CBUF_OK;
}
