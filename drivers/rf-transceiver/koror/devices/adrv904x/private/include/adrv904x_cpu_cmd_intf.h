/**
 * \file adrv904x_cpu_cmd_interface.h
 * 
 * \brief   Contains device command interface definition
 *
 * \details Contains device command interface definition
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_INTF_H__
#define __ADRV904X_CPU_CMD_INTF_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_platform_byte_order.h"

/** \var CPU command ID type */
typedef uint16_t adrv904x_CpuCmdId_t;

/** \var CPU command transaction ID type */
typedef uint16_t adrv904x_CpuCmdTransactionId_t;

/** \var CPU command status type */
typedef uint16_t adrv904x_CpuCmdStatus_t;

/**
 * \brief CPU command status enumeration
 */
typedef enum adrv904x_CpuCmdStatus
{
    ADRV904X_CPU_CMD_STATUS_NO_ERROR,                   /*!< No error */
    ADRV904X_CPU_CMD_STATUS_GENERIC,                    /*!< Unspecified/unknown error */
    ADRV904X_CPU_CMD_STATUS_LINK_ERROR,                 /*!< Link (lower level) error */
    ADRV904X_CPU_CMD_STATUS_UNEXPECTED_TRANSACTION_ID,  /*!< Unexpected/invalid transaction ID received */
    ADRV904X_CPU_CMD_STATUS_CMD_FAILED,                 /*!< Command-specific failure. See command-specific response for details. */
    ADRV904X_CPU_CMD_STATUS_CMD_ID_INVALID              /*!< Invalid command ID */
} adrv904x_CpuCmdStatus_e;

/**
* \brief Common CPU command structure
*/
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd
{
    adrv904x_CpuCmdId_t cmdId;           /*!< Command ID. Value is from adrv904x_CpuCmdId_e enumeration. */
    adrv904x_CpuCmdTransactionId_t tId;  /*!< Transaction ID. Unique ID for a command/response pair. */

    /* Command payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* payload[];
     */
} adrv904x_CpuCmd_t;)

/**
* \brief Common CPU command response structure
*/
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmdResp
{
    adrv904x_CpuCmdId_t cmdId;           /*!< Command ID. Value is from adrv904x_CpuCmdId_e enumeration. */
    adrv904x_CpuCmdTransactionId_t tId;  /*!< Transaction ID. Unique ID for a command/response pair. */
    adrv904x_CpuCmdStatus_t status;      /*!< Command status. Value is from adrv904x_CpuCmdStatus_e enumeration. */

    /* Command response payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* payload[];
     */
} adrv904x_CpuCmdResp_t;)

#endif /* __ADRV904X_CPU_CMD_INTF_H__ */

