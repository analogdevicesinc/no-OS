/**
 * \file adrv904x_cpu_cmd_radio.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_RADIO
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_RADIO
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2022 - 2022 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_RADIO_H__
#define __ADRV904X_CPU_CMD_RADIO_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"

/**
 *  \brief Enum of radio subcommands
 */
typedef enum adrv904x_CpuCmd_RadioSubcommand
{
    ADI_ADRV904X_RADIO_PING = 0,                  /*!< Radio ping command */
    ADI_ADRV904X_RADIO_TXLOL_SET_PARAMS,          /*!< Radio TxLOL set correction parameters */
    ADI_ADRV904X_RADIO_TXLOL_GET_PARAMS,          /*!< Radio TxLOL get correction parameters */
    ADI_ADRV904X_RADIO_TXLOL_SET_UPDATE_ENABLE,   /*!< Radio TxLOL set enable  */
    ADI_ADRV904X_RADIO_TXLOL_GET_UPDATE_ENABLE,   /*!< Radio TxLOL set enable  */
} adrv904x_CpuCmd_RadioSubcommand_e;

/**
 * \brief Radio TxLOL parameters set command structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_CpuCmd_RadioTxLolParamsSet
{
    uint8_t txChan;      /*!< Tx channel (0 - 7) */
    uint8_t hpAtten;     /*!< Tx attenuation (1 - 64, 0xFF) */
    int16_t offsetI;     /*!< I correction parameter */
    int16_t offsetQ;     /*!< Q correction parameter */
} adrv904x_CpuCmd_RadioTxLolParamsSet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Radio TxLOL parameters get command structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_CpuCmd_RadioTxLolParamsGet
{
    uint8_t txChan;      /*!< Tx channel (0 - 7) */
    uint8_t hpAtten;     /*!< Tx attenuation (1 - 64) */
} adrv904x_CpuCmd_RadioTxLolParamsGet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Radio TxLOL parameters set command response structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_CpuCmd_RadioTxLolParamsSetResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv904x_CpuCmd_RadioTxLolParamsSetResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Radio TxLOL parameters get command response structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_CpuCmd_RadioTxLolParamsGetResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint8_t txChan;                  /*!< Tx channel (0 - 7) */
    uint8_t hpAtten;                 /*!< Tx attenuation (1 - 64) */
    int16_t offsetI;                 /*!< I correction parameter */
    int16_t offsetQ;                 /*!< Q correction parameter */
} adrv904x_CpuCmd_RadioTxLolParamsGetResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Radio TxLOL update enable set command structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_CpuCmd_RadioTxLolUpdateEnableSet
{
    uint8_t txChan;      /*!< Tx channel (0 - 7) */
    uint8_t enable ;     /*!< enable corrections updates */
} adrv904x_CpuCmd_RadioTxLolUpdateEnableSet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Radio TxLOL update enable set command response structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_CpuCmd_RadioTxLolUpdateEnableSetResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv904x_CpuCmd_RadioTxLolUpdateEnableSetResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Radio TxLOL update enable get command structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_CpuCmd_RadioTxLolUpdateEnableGet
{
    uint8_t txChan;      /*!< Tx channel (0 - 7) */
} adrv904x_CpuCmd_RadioTxLolUpdateEnableGet_t;
ADI_ADRV904X_PACK_FINISH


/**
 * \brief Radio TxLOL update enable get command response structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_CpuCmd_RadioTxLolUpdateEnableGetResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint8_t txChan;                  /*!< Tx channel (0 - 7) */
    uint8_t enable;                  /*!< enable updates flag */
} adrv904x_CpuCmd_RadioTxLolUpdateEnableGetResp_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_CPU_CMD_RADIO_H__ */

