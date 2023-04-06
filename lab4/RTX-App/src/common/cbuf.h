//
// Created by Karim Alatrash on 2022-06-19.
//

/*
 * ------------------------------------------------------------------------
 *                             FILE BEGIN
 * ------------------------------------------------------------------------
 */

#ifndef ECE350_SPRING2022_LAB_G30_CBUF_H
#define ECE350_SPRING2022_LAB_G30_CBUF_H

#include "stdint.h"
/*
 * ------------------------------------------------------------------------
 *                             TYPE DEFINITIONS
 * ------------------------------------------------------------------------
 */

typedef struct cbuf {
    // Index of the start of the circular buffer
    uint32_t start;

    // Pointer to the start of the buffer in contiguous memory for deallocation
    void * buf;

    // Tracks the current size of the circular buffer
    uint32_t size;

    // Holds the max size of the circular buffer
    uint32_t max_size;
} cbuf_t;

/*
 * ------------------------------------------------------------------------
 *                             ENUMS
 * ------------------------------------------------------------------------
 */
typedef enum {
    CBUF_OK = 0,
    CBUF_FULL,
    CBUF_FAIL
} CBUF_STATUS;
/*
 * ------------------------------------------------------------------------
 *                             FUNCTION PROTOTYPES
 * ------------------------------------------------------------------------
 */

/**
 * @details Initializes a circular buffer to some size
 * @param circle_buffer
 * @param size
 * @return The status on if the circular buffer was succesfully created, 0 is succesful
 */
CBUF_STATUS cbuf_init( cbuf_t * circle_buffer, uint32_t size );

/**
 * @details Deinitialises the circular buffer passed as param
 * @param circle_buffer
 * @return Status of deallocation of resources, 0 is succesful
 */
CBUF_STATUS cbuf_destroy( cbuf_t * circle_buffer );

/**
 * @details Deep copies into the back of the circular buffer data structure
 * @param circle_buffer
 * @param buffer
 * @param size
 * @return 1 if unsuccesful, 0 if succesful
 */
CBUF_STATUS cbuf_insert_back(cbuf_t * circle_buffer, void * buffer, uint32_t size);

/**
 * @details Removes the oldest element from the circular buffer
 * @param circle_buffer
 * @param size Number of bytes to take from the circular buffer
 * @return A dynamically allocated buffer! Must be freed to prevent leaks
 */
CBUF_STATUS cbuf_remove_front(cbuf_t * circle_buffer, uint8_t * dest_buf, uint32_t size);

/**
 * @details Peeks the first n bytes of the circular buffer but does not remove
 * @param circle_buffer
 * @param size Number of bytes to peek
 * @return A dynamically allocated buffer! Must be freed to prevent leaks
 */
CBUF_STATUS cbuf_peek_front( cbuf_t * circle_buffer, uint8_t * dest_buf, uint32_t size );

#endif //ECE350_SPRING2022_LAB_G30_CBUF_H

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
