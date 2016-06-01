/*
 *  Copyright (c) 2015, United States government as represented by the
 *  administrator of the National Aeronautics Space Administration.
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 */

/**
 * \file iodriver_memory_io.h
 *
 * \date May 31, 2016
 * \author joseph.p.hickey@nasa.gov
 *
 */


#ifndef IODRIVER_MEMORY_IO_H_
#define IODRIVER_MEMORY_IO_H_

/* Include all base definitions */
#include "iodriver_base.h"

/**
 * API container for memory write commands.
 *
 * Associates a device address, buffer pointer and a buffer size.
 */
typedef struct
{
    uint32 DeviceAddress;
    uint32 BufferSize;
    const void *BufferMem;
} IODriver_WriteMemoryBuffer_t;

/**
 * API container for memory write commands.
 *
 * Associates a device address, buffer pointer and a buffer size.
 */
typedef struct
{
    uint32 DeviceAddress;
    uint32 BufferSize;
    void *BufferMem;
} IODriver_ReadMemoryBuffer_t;


/**
 * Opcodes specific to memory devices or other direct register-oriented interfaces
 */
enum
{
    IODRIVER_MEMORY_IO_NOOP = IODRIVER_MEMORY_IO_CLASS_BASE,

    IODRIVER_MEMORY_IO_READ_32,     /**< IODriver_ReadMemoryBuffer_t argument, use 32 bit access */
    IODRIVER_MEMORY_IO_WRITE_32,    /**< IODriver_WriteMemoryBuffer_t argument, use 32 bit access */
    IODRIVER_MEMORY_IO_READ_16,     /**< IODriver_ReadMemoryBuffer_t argument, use 16 bit access */
    IODRIVER_MEMORY_IO_WRITE_16,    /**< IODriver_WriteMemoryBuffer_t argument, use 16 bit access */
    IODRIVER_MEMORY_IO_READ_8,      /**< IODriver_ReadMemoryBuffer_t argument, use 8 bit access */
    IODRIVER_MEMORY_IO_WRITE_8,     /**< IODriver_WriteMemoryBuffer_t argument, use 8 bit access */
    IODRIVER_MEMORY_IO_READ_BLOCK,  /**< IODriver_ReadMemoryBuffer_t argument, use any appropriate access cycle (generic) */
    IODRIVER_MEMORY_IO_WRITE_BLOCK, /**< IODriver_WriteMemoryBuffer_t argument, use any appropriate access cycle (generic) */

    IODRIVER_MEMORY_IO_MAX
};


#endif /* IODRIVER_MEMORY_IO_H_ */
