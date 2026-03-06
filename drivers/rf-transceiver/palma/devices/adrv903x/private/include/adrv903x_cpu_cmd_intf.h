/**
 * \file adrv903x_cpu_cmd_interface.h
 * 
 * \brief   Contains device command interface definition
 *
 * \details Contains device command interface definition
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_INTF_H__
#define __ADRV903X_CPU_CMD_INTF_H__

#include "adi_adrv903x_platform_pack.h"
#ifndef ADI_ADRV903X_FW
#include "adrv903x_platform_byte_order.h"
#endif

/** \var CPU command ID type */
typedef uint16_t adrv903x_CpuCmdId_t;

/** \var CPU command transaction ID type */
typedef uint16_t adrv903x_CpuCmdTransactionId_t;

/** \var CPU command status type */
typedef uint16_t adrv903x_CpuCmdStatus_t;

/**
 * \brief CPU command status enumeration
 */
typedef enum adrv903x_CpuCmdStatus
{
    ADRV903X_CPU_CMD_STATUS_NO_ERROR,                   /*!< No error */
    ADRV903X_CPU_CMD_STATUS_GENERIC,                    /*!< Unspecified/unknown error */
    ADRV903X_CPU_CMD_STATUS_LINK_ERROR,                 /*!< Link (lower level) error */
    ADRV903X_CPU_CMD_STATUS_UNEXPECTED_TRANSACTION_ID,  /*!< Unexpected/invalid transaction ID received */
    ADRV903X_CPU_CMD_STATUS_CMD_FAILED,                 /*!< Command-specific failure. See command-specific response for details. */
    ADRV903X_CPU_CMD_STATUS_CMD_ID_INVALID              /*!< Invalid command ID */
} adrv903x_CpuCmdStatus_e;

/**
* \brief Common CPU command structure
*/
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd
{
    adrv903x_CpuCmdId_t cmdId;           /*!< Command ID. Value is from adrv903x_CpuCmdId_e enumeration. */
    adrv903x_CpuCmdTransactionId_t tId;  /*!< Transaction ID. Unique ID for a command/response pair. */

    /* Command payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* payload[];
     */
} adrv903x_CpuCmd_t;)

/**
* \brief Common CPU command response structure
*/
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmdResp
{
    adrv903x_CpuCmdId_t cmdId;           /*!< Command ID. Value is from adrv903x_CpuCmdId_e enumeration. */
    adrv903x_CpuCmdTransactionId_t tId;  /*!< Transaction ID. Unique ID for a command/response pair. */
    adrv903x_CpuCmdStatus_t status;      /*!< Command status. Value is from adrv903x_CpuCmdStatus_e enumeration. */

    /* Command response payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* payload[];
     */
} adrv903x_CpuCmdResp_t;)

#endif /* __ADRV903X_CPU_CMD_INTF_H__ */

