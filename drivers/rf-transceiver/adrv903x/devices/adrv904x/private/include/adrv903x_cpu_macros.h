/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adrv903x_cpu_macros.h
 * \brief Contains ADRV903X API miscellaneous macro definitions for CPU
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef _ADRV903X_CPU_MACROS_H_
#define _ADRV903X_CPU_MACROS_H_

#include "adi_library_types.h"
#include "adrv903x_cpu_memory.h"

#define ADRV903X_CPU_0MD_ERRCODE(armOpCode, armObjId, armErrorFlag)  \
    ((armOpCode << ADRV903X_CPU_OPCODE_SHIFT) |                      \
     (armObjId << ADRV903X_CPU_OBJ_ID_SHIFT) |                       \
     armErrorFlag)

#define ADRV903X_CPU_OPCODE_VALID(a) \
    (((a) != 0) && (((a) % 2) || ((a) > 30)))

/* CPU memory map */
#define ADRV903X_CPU_0_ADDR_PROG_START                (ADRV903X_CPU0_PM_START)

/* CPU memory map */
#define ADRV903X_CPU_1_ADDR_PROG_START                (ADRV903X_CPU1_PM_START)

/* CPU FW version address */
#define ADRV903X_ADDR_FW_VERSION                       (ADRV903X_PM_DEVICE_REV_DATA)

#define ADRV903X_MAX_CPU_FW_PATH 128u
#define ADRV903X_CPU_FW_PATH_SEP ';'
    
#define ADRV903X_CPU_STREAM_TRIGGER_OPCODE 0x1FU


#endif /* _ADRV903X__MACROS_H_ */
