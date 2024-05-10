/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_cpu_macros.h
 * \brief Contains ADRV904X API miscellaneous macro definitions for CPU
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADRV904X_CPU_MACROS_H_
#define _ADRV904X_CPU_MACROS_H_

#include <stdint.h>
#include "adrv904x_cpu_memory.h"

#define ADRV904X_CPU_0MD_ERRCODE(armOpCode, armObjId, armErrorFlag)  \
    ((armOpCode << ADRV904X_CPU_OPCODE_SHIFT) |                      \
     (armObjId << ADRV904X_CPU_OBJ_ID_SHIFT) |                       \
     armErrorFlag)

#define ADRV904X_CPU_OPCODE_VALID(a) \
    (((a) != 0) && (((a) % 2) || ((a) > 30)))

/* CPU memory map */
#define ADRV904X_CPU_0_ADDR_PROG_START                (ADRV904X_CPU0_PM_START)

/* CPU memory map */
#define ADRV904X_CPU_1_ADDR_PROG_START                (ADRV904X_CPU1_PM_START)

/* CPU FW version address */
#define ADRV904X_ADDR_FW_VERSION                       (ADRV904X_PM_DEVICE_REV_DATA)

#define ADRV904X_MAX_CPU_FW_PATH 128u
#define ADRV904X_CPU_FW_PATH_SEP ';'
    
#define ADRV904X_CPU_STREAM_TRIGGER_OPCODE 0x1FU


#endif /* _ADRV904X__MACROS_H_ */
