/**
 * \file adrv903x_cpu_cmd_getset_lofreq.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_GET_LOFREQ,
 *          ADRV903X_CPU_CMD_ID_SET_LOFREQ, ADRV903X_CPU_CMD_ID_GET_LOOPFILTER,
 *          ADRV903X_CPU_CMD_ID_SET_LOOPFILTER and ADRV903X_CPU_CMD_ID_GET_RXTXLOFREQ
 *
 * \details Command definition for ADRV903X_CPU_CMD_ID_GET_LOFREQ,
 *          ADRV903X_CPU_CMD_ID_SET_LOFREQ, ADRV903X_CPU_CMD_ID_GET_LOOPFILTER,
 *          ADRV903X_CPU_CMD_ID_SET_LOOPFILTER and ADRV903X_CPU_CMD_ID_GET_RXTXLOFREQ
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_GETSET_LOFREQ_H__
#define __ADRV903X_CPU_CMD_GETSET_LOFREQ_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_lo_types.h"
#include "adrv903x_cpu_error_codes_types.h"
#include "adrv903x_cpu_device_profile_types.h"

typedef uint8_t adi_adrv903x_LoName_t;    /* associates with adi_adrv903x_LoName_e */
typedef uint32_t adi_adrv903x_LoOption_t; /* associates with adi_adrv903x_LoOption_e */
typedef uint32_t adi_adrv903x_LoMcsOption_t; /* associates with adi_adrv903x_LoMcsOption_e */

/*
 * \brief lofreq set command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetLoFreq
{
    adi_adrv903x_LoName_t             loName;                /*!< Select the target RF LO source */
    uint64_t                          loFrequency_Hz;        /*!< Desired RF LO frequency in Hz */
    adi_adrv903x_LoOption_t           loConfigSel;           /*!< Select for LO config */
    adi_adrv903x_LoMcsOption_t        loMcsType;             /*!< Select the type of MCS for RF PLL. For Debug */
    uint8_t                           disableBleedCal;       /*!< Disable bleed ramp cal (1) */
    uint8_t                           enablePfdOverride;     /*!< Enable PFD override (1) */
    uint32_t                          pfdSetting;            /*!< PFD override value when enablePfdOverride is set*/
} adrv903x_CpuCmd_SetLoFreq_t;)

/**
 * \brief lofreq set command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetLoFreqResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv903x_CpuCmd_SetLoFreqResp_t;)

/**
 * \brief lofreq get command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetLoFreq
{
    adi_adrv903x_LoName_t             loName;                /*!< Select the target RF LO source to read back */
} adrv903x_CpuCmd_GetLoFreq_t;)

/**
 * \brief lofreq set command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetLoFreqResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint64_t loFrequency_Hz;         /*!< RF LO frequency in Hz */    
} adrv903x_CpuCmd_GetLoFreqResp_t;)


/**
 * \brief loopfilter set command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetLoopfilter
{
    adi_adrv903x_LoName_t             loName;                /*!< Select the target RF LO source */
    uint32_t                          phaseMargin;           /*!< Phase margin in degrees */
    uint32_t                          loopBandwidth;         /*!< Loop bandwidth in hz*/    
} adrv903x_CpuCmd_SetLoopfilter_t;)

/**
 * \brief loopfilter set command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetLoopfilterResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv903x_CpuCmd_SetLoopfilterResp_t;)

/**
 * \brief loopfilter get command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetLoopfilter
{
    adi_adrv903x_LoName_t             loName;                /*!< Select the target RF LO source to read back */
} adrv903x_CpuCmd_GetLoopfilter_t;)

/**
 * \brief loopfilter set command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetLoopfilterResp
{
    adrv903x_CpuErrorCode_e     status;             /*!< CPU error status code */
    uint32_t                    phaseMargin;        /*!< Phase margin in degrees */
    uint32_t                    loopBandwidth;      /*!< Loop bandwidth in hz*/    
} adrv903x_CpuCmd_GetLoopfilterResp_t;)

/**
 * \brief loopfilter RX/TX LO command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetRxTxLoFreqResp
{
    adrv903x_CpuErrorCode_e status;                          /*!< CPU error status code */
    adi_adrv903x_LoName_t   rxLoName[ADRV903X_RX_CHAN_LEN];  /*!< PLL number of Rx Channels */    
    uint32_t                rxFreq[ADRV903X_RX_CHAN_LEN];    /*!< Freq of Rx Channels */  
    adi_adrv903x_LoName_t   txLoName[ADRV903X_TX_CHAN_LEN];  /*!< PLL number of Tx Channels */    
    uint32_t                txFreq[ADRV903X_TX_CHAN_LEN];    /*!< Freq of Tx Channels */  
} adrv903x_CpuCmd_GetRxTxLoFreqResp_t;)
	
/**
 * \brief Data structure to hold the Channel Controls to the PLL's.
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_ChanCtrlToPlls
{
	uint8_t rf0MuxTx0_3;       /* If set to 1 East Tx channels (Tx0-3) are connected to PLL0 (East PLL). If set to 0 Tx0-3 are connected to PLL1 (West PLL). */
	uint8_t rf0MuxTx4_7;       /* If set to 1 West Tx channels (Tx4-7) are connected to PLL0 (East PLL). If set to 0 Tx4-7 are connected to PLL1 (West PLL). */
	uint8_t rf0MuxRx0_3;       /* If set to 1 East Rx channels (Rx0-3) are connected to PLL0 (East PLL). If set to 0 Rx0-3 are connected to PLL1 (West PLL). */
	uint8_t rf0MuxRx4_7;       /* If set to 1 West Rx channels (Rx4-7) are connected to PLL0 (East PLL). If set to 0 Rx4-7 are connected to PLL1 (West PLL). */  
} adrv903x_CpuCmd_ChanCtrlToPlls_t;)
	
/**
 * \brief Data structure to hold the Channel Controls to the PLL's.
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_ChanCtrlToPllsResp
{
	adrv903x_CpuErrorCode_e     status; /*!< CPU error status code */
} adrv903x_CpuCmd_ChanCtrlToPllsResp_t;)

/**
 * \brief Reprogram PLL command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_ReprogramPll
{
    uint8_t pllSel;    /*!< PLL to be Reprogrammed according to adi_adrv903x_Pll_e */
} adrv903x_CpuCmd_ReprogramPll_t;)

/**
 * \brief Reprogram PLL command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_ReprogramPllResp
{
    adi_adrv903x_CpuErrorCode_t status;  /*!< CPU error status code */
} adrv903x_CpuCmd_ReprogramPllResp_t;)

#endif /* __ADRV903X_CPU_CMD_GETSET_LOFREQ_H__ */


