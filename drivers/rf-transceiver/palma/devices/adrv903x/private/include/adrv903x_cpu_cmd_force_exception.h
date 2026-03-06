/**
 * \file adrv903x_cpu_cmd_force_exception.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_FORCE_EXCEPTION
 *
 * \details Command definition for ADRV903X_CPU_CMD_ID_FORCE_EXCEPTION
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_FORCE_EXCEPTION_H__
#define __ADRV903X_CPU_CMD_FORCE_EXCEPTION_H__

#include "adi_adrv903x_platform_pack.h"

/**
 * NOTE: Unlike other commands, the force exception command will be indicated by
 * the hardware mailbox opcode value, which is used as the link id for other
 * commands.
 */


/** \var Force exception opcode/link id */
typedef uint8_t adrv903x_MailboxOpcodes_t;

/**
 * \brief Force exception opcode/link id enumeration
 */
typedef enum adrv903x_MailboxOpcodes
{
    ADRV903X_MAILBOX_FORCE_EXCEPTION = 0x3FU  /*!< 0x3F - Force exception */
} adrv903x_MailboxOpcodes_e;


/* Type and enumeration for CPU exception status */
typedef uint32_t adrv903x_CpuExceptionFlag_t;

typedef enum adrv903x_CpuExceptionFlag
{
    ADRV903X_CPU_NO_EXCEPTION        = 0u,           /*!< 0x00000000 - CPU running normally */
    ADRV903X_CPU_EXCEPTION_ENTERED   = 0x11111111u,  /*!< 0x11111111 - Exception handler entered, no snapshot data  */
    ADRV903X_CPU_EXCEPTION_COMPLETED = 0x33333333u   /*!< 0x33333333 - Exception handler complete, valid snapshot data */
} adrv903x_CpuExceptionFlag_e;

#endif /* __ADRV903X_CPU_CMD_FORCE_EXCEPTION_H__ */


