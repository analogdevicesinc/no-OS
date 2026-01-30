/**
 * \file adrv904x_cpu_cmd_force_exception.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_FORCE_EXCEPTION
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_FORCE_EXCEPTION
 *
 * ADRV904X API Version: 2.15.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV904X_CPU_CMD_FORCE_EXCEPTION_H__
#define __ADRV904X_CPU_CMD_FORCE_EXCEPTION_H__

#include "adi_adrv904x_platform_pack.h"

/**
 * NOTE: Unlike other commands, the force exception command will be indicated by
 * the hardware mailbox opcode value, which is used as the link id for other
 * commands.
 */


/** \var Force exception opcode/link id */
typedef uint8_t adrv904x_MailboxOpcodes_t;

/**
 * \brief Force exception opcode/link id enumeration
 */
typedef enum adrv904x_MailboxOpcodes
{
    ADRV904X_MAILBOX_FORCE_EXCEPTION = 0x3FU  /*!< 0x3F - Force exception */
} adrv904x_MailboxOpcodes_e;


/* Type and enumeration for CPU exception status */
typedef uint32_t adrv904x_CpuExceptionFlag_t;

typedef enum adrv904x_CpuExceptionFlag
{
    ADRV904X_CPU_NO_EXCEPTION        = 0u,           /*!< 0x00000000 - CPU running normally */
    ADRV904X_CPU_EXCEPTION_ENTERED   = 0x11111111u,  /*!< 0x11111111 - Exception handler entered, no snapshot data  */
    ADRV904X_CPU_EXCEPTION_COMPLETED = 0x33333333u   /*!< 0x33333333 - Exception handler complete, valid snapshot data */
} adrv904x_CpuExceptionFlag_e;

#endif /* __ADRV904X_CPU_CMD_FORCE_EXCEPTION_H__ */


