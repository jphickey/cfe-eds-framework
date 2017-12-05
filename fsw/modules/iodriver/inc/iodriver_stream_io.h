/*
 *  Copyright (c) 2015, United States government as represented by the
 *  administrator of the National Aeronautics Space Administration.
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 */

/**
 * \file iodriver_stream_io.h
 *
 * \date Apr 4, 2017
 * \author lisa.b.vanderaar@nasa.gov
 *
 * \description Modification of the iodriver_packet_io.h to be stream specific
 *
 */


#ifndef IODRIVER_STREAM_IO_H_
#define IODRIVER_STREAM_IO_H_

/* Include all base definitions */
#include "iodriver_base.h"

/**
 * API container for stream write commands.
 * -- Associates a buffer pointer and a buffer size.
 */
typedef struct
{
    uint32 BufferSize;         ///< Size of data buffer
    const void *BufferMem;     ///< Pointer to data buffer to write
} IODriver_WriteStreamBuffer_t;

/**
 * API container for stream read commands.
 * -- Associates a buffer pointer and a buffer size.
 */
typedef struct
{
    uint32 BufferSize;          ///< Size of data buffer
    void *BufferMem;            ///< Pointer to data buffer to store read data
} IODriver_ReadStreamBuffer_t;

/**
 * Opcodes specific to stream oriented interfaces
 * FIX: Right now these are the same as the packet interface and need to be changed.
 */
enum
{
    IODRIVER_STREAM_IO_NOOP = IODRIVER_STREAM_IO_CLASS_BASE,

    IODRIVER_STREAM_IO_READ,   /**< IODriver_ReadStreamBuffer_t argument */
    IODRIVER_STREAM_IO_WRITE,  /**< IODriver_WriteStreamBuffer_t argument */

    IODRIVER_STREAM_IO_MAX
};

/**
 * Additional error codes specific to Stream I/O
 *
 * These are based from the IODRIVER_STREAM_IO_CLASS_BASE so as to not conflict with other classes of I/O
 */
enum
{
    IODRIVER_STREAM_ERROR_BASE =  -(IODRIVER_STREAM_IO_CLASS_BASE + 0xFFFF),
    IODRIVER_STREAM_LENGTH_ERROR,
    IODRIVER_STREAM_CRC_ERROR
};


#endif /* IODRIVER_STREAM_IO_H_ */
