/*
 *  Copyright (c) 2015, United States government as represented by the
 *  administrator of the National Aeronautics Space Administration.
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 */

/**
 * \file iodriver_packet_io.h
 *
 * \date Mar 4, 2016
 * \author joseph.p.hickey@nasa.gov
 *
 */


#ifndef IODRIVER_PACKET_IO_H_
#define IODRIVER_PACKET_IO_H_

/* Include all base definitions */
#include "iodriver_base.h"

/**
 * API container for packet read/write commands.
 *
 * Associates a buffer pointer and a buffer size.
 * For "write" operations the size reflects the actual size of the packet,
 * and the buffer memory should not be modified by the driver (const).
 */
typedef struct
{
    uint32 OutputSize;                    ///< Number of channels in the i/o structure (length of "samples" array)
    const void *BufferMem;
} IODriver_WritePacketBuffer_t;

/**
 * API container for packet read/write commands.
 *
 * Associates a buffer pointer and a buffer size.
 * For "read" operations the size reflects maximum (allocated) size.  It
 * must be adjusted to the actual size of the packet recieved.
 */
typedef struct
{
    uint32 BufferSize;                    ///< Number of channels in the i/o structure (length of "samples" array)
    void *BufferMem;
} IODriver_ReadPacketBuffer_t;

/**
 * Opcodes specific to packet oriented interfaces
 */
enum
{
    IODRIVER_PACKET_IO_NOOP = IODRIVER_PACKET_IO_CLASS_BASE,

    IODRIVER_PACKET_IO_READ,   /**< IODriver_ReadPacketBuffer_t argument */
    IODRIVER_PACKET_IO_WRITE,  /**< IODriver_WritePacketBuffer_t argument */

    IODRIVER_PACKET_IO_MAX
};

/**
 * Additional error codes specific to Packet I/O
 *
 * These are based from the IODRIVER_PACKET_IO_CLASS_BASE so as to not conflict with other classes of I/O
 */
enum
{
    IODRIVER_PACKET_ERROR_BASE =  -(IODRIVER_PACKET_IO_CLASS_BASE + 0xFFFF),
    IODRIVER_PACKET_LENGTH_ERROR,
    IODRIVER_PACKET_CRC_ERROR
};


#endif /* IODRIVER_PACKET_IO_H_ */
