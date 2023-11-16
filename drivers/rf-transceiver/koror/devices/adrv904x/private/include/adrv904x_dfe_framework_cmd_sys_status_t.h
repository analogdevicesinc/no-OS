/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adrv904x_dfe_framework_cmd_sys_status_t.h
 *
 * \brief type definitions for ADI_ADRV904X_DFE_FRAMEWORK_CMD_ID_GET_SYS_STATUS
 *
 * DFE SDK Version: 2.9.0.4
 */

#ifndef __FRAMEWORK_CMD_SYS_STATUS_T_H__
#define __FRAMEWORK_CMD_SYS_STATUS_T_H__

#include "adrv904x_dfe_framework_cmd_t.h"

/*!< maximum length of the SYS_STATUS payload */
#define ADRV904X_DFE_FRAMEWORK_CMD_SYS_STATUS_MAX_PAYLOAD_LEN    (256)   /*!< Maximum payload length of system status command */

/*!< Dummy driver object, used for testing */
#define ADRV904X_DFE_APP_FRAMEWORK_CMD_OBJID_DRV_DUMMY           (0xF0u) /*!< Dummy driver object for testing */

/**
 * \brief System status type enumeration
 */
typedef enum adrv904x_DfeAppFrameworkCmdSysStatusType
{
    ADRV904X_DFE_FRAMEWORK_CMD_SYS_STATUS_PUBLIC,        /*!< Public System status */
    ADRV904X_DFE_FRAMEWORK_CMD_SYS_STATUS_PRIVATE        /*!< Private System status */
} adrv904x_DfeFrameworkCmdSysStatusType_e;

/**
 * \brief adrv904x_DfeFrameworkCmdSysStatusType_e API size translation
 */
typedef uint8_t adrv904x_DfeAppFrameworkCmdSysStatusType_t;

/**
 * \brief GET_SYS_STATUS command structure
 */
typedef struct adrv904x_DfeAppFrameworkCmdGetSysStatus
{
    adrv904x_DfeAppFrameworkCmdSysStatusType_t statusType; /*!< System statustype to be retrieved */
    uint32_t                             sysObjId;   /*!< Object ID of system component */
    uint32_t                             channelNum; /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
} adrv904x_DfeAppFrameworkCmdGetSysStatus_t;


/**
 * \brief System status type enumeration
 */
typedef enum adrv904x_DfeAppFrameworkCmdGetSysStatus_ErrType
{
    ADRV904X_DFE_FRAMEWORK_CMD_SYS_STATUS_NO_ERROR,       /*!< no error in response */
    ADRV904X_DFE_FRAMEWORK_CMD_SYS_STATUS_NOT_SUPP        /*!< the (sub)system status requested is not supported */
} adrv904x_DfeFrameworkCmdGetSysStatus_ErrType_e;

/**
 * \brief adrv904x_DfeFrameworkCmdGetSysStatus_ErrType_e API size translation
 */
typedef uint16_t adrv904x_DfeAppFrameworkCmdGetSysStatus_ErrType_t;


/**
 * \brief GET_SYS_STATUS command response structure
 */
typedef struct adrv904x_DfeAppFrameworkCmdGetSysStatusResp
{
    adrv904x_DfeAppFrameworkCmdGetSysStatus_ErrType_t status;      /*!< Command status */
    /* System status payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* sysStatus[];
     */
} adrv904x_DfeAppFrameworkCmdGetSysStatusResp_t;

/**
 * \brief System status payload union.
 *        Used to determine the maximum system status payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef union adrv904x_DfeAppFrameworkCmdSysStatusMaxSize
{
    uint8_t sysStatusMax[ADRV904X_DFE_FRAMEWORK_CMD_SYS_STATUS_MAX_PAYLOAD_LEN];  /*!< Sys response buffer */
} adrv904x_DfeAppFrameworkCmdSysStatusMaxSize_t;

/**
 * \brief Status payload size struct.
 *        Used to determine the maximum cal status payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv904x_DfeAppFrameworkCmdGetSysStatusMaxSize
{
    adrv904x_DfeAppFrameworkCmdGetSysStatusResp_t getSysStatusCmdResp; /*!< Sys command response          */
    adrv904x_DfeAppFrameworkCmdSysStatusMaxSize_t sysStatusMaxSize;    /*!< Sys command response max size */
} adrv904x_DfeAppFrameworkCmdGetSysStatusMaxSize_t;

#endif /* __FRAMEWORK_CMD_SYS_STATUS_T_H__ */



