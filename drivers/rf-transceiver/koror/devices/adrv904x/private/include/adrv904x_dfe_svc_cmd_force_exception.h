/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_cmd_force_exception.h
 *
 * \brief type definitions for ADI_ADRV904X_DFE_SVC_CMD_ID_FORCE_EXCEPTION
 *
 * DFE SDK Version: 2.9.0.4
 */

#ifndef __ADI_ADRV904X_DFE_SVC_CMD_FORCE_EXCEPTION_H__
#define __ADI_ADRV904X_DFE_SVC_CMD_FORCE_EXCEPTION_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_dfe_platform_exception_handler_t.h"

/**
 * NOTE: Unlike other commands, the force exception command will be indicated by
 * the hardware mailbox opcode value, which is used as the link id for other
 * commands.
 */

/** \var Force exception opcode/link id */
typedef uint8_t adrv904x_DfeSvcCmdDfeMailboxOpcodes_t;


/**
 * \brief Force exception opcode/link id enumeration
 */
typedef enum adrv904x_DfeSvcCmdDfeMailboxOpcodes
{
    ADRV904X_DFE_SVC_CMD_DFE_MAILBOX_FORCE_EXCEPTION = 0x3FU  /*!< 0x3F - Force exception */
}       adrv904x_DfeSvcCmdDfeMailboxOpcodes_e;

typedef adrv904x_DfePlatformExceptionData_t adrv904x_DfeSvcCmdDfeExceptionData_t;

#endif /* __ADI_ADRV904X_DFE_SVC_CMD_FORCE_EXCEPTION_H__ */



