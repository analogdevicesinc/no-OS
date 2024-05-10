/**
 * \file adrv904x_cpu_cmd_getset_lofreq.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_GET_LOFREQ,
 *          ADRV904X_CPU_CMD_ID_SET_LOFREQ, ADRV904X_CPU_CMD_ID_GET_LOOPFILTER,
 *          ADRV904X_CPU_CMD_ID_SET_LOOPFILTER and ADRV904X_CPU_CMD_ID_GET_RXTXLOFREQ
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_GET_LOFREQ,
 *          ADRV904X_CPU_CMD_ID_SET_LOFREQ, ADRV904X_CPU_CMD_ID_GET_LOOPFILTER,
 *          ADRV904X_CPU_CMD_ID_SET_LOOPFILTER and ADRV904X_CPU_CMD_ID_GET_RXTXLOFREQ
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_GETSET_LOFREQ_H__
#define __ADRV904X_CPU_CMD_GETSET_LOFREQ_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_lo_types.h"
#include "adrv904x_cpu_error_codes_types.h"
#include "adrv904x_cpu_device_profile_types.h"

typedef uint8_t adi_adrv904x_LoName_t;    /* associates with adi_adrv904x_LoName_e */
typedef uint32_t adi_adrv904x_LoOption_t; /* associates with adi_adrv904x_LoOption_e */
typedef uint32_t adi_adrv904x_LoMcsOption_t; /* associates with adi_adrv904x_LoMcsOption_e */

/*
 * \brief lofreq set command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetLoFreq
{
    adi_adrv904x_LoName_t             loName;                /*!< Select the target RF LO source */
    uint64_t                          loFrequency_Hz;        /*!< Desired RF LO frequency in Hz */
    adi_adrv904x_LoOption_t           loConfigSel;           /*!< Select for LO config */
    adi_adrv904x_LoMcsOption_t        loMcsType;             /*!< Select the type of MCS for RF PLL. For Debug */
    uint8_t                           disableBleedCal;       /*!< Disable bleed ramp cal (1) */
    uint8_t                           enablePfdOverride;     /*!< Enable PFD override (1) */
    uint32_t                          pfdSetting;            /*!< PFD override value when enablePfdOverride is set*/
} adrv904x_CpuCmd_SetLoFreq_t;)

/**
 * \brief lofreq set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetLoFreqResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv904x_CpuCmd_SetLoFreqResp_t;)

/**
 * \brief lofreq get command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetLoFreq
{
    adi_adrv904x_LoName_t             loName;                /*!< Select the target RF LO source to read back */
} adrv904x_CpuCmd_GetLoFreq_t;)

/**
 * \brief lofreq set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetLoFreqResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint64_t loFrequency_Hz;         /*!< RF LO frequency in Hz */    
} adrv904x_CpuCmd_GetLoFreqResp_t;)


/**
 * \brief loopfilter set command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetLoopfilter
{
    adi_adrv904x_LoName_t             loName;                /*!< Select the target RF LO source */
    uint32_t                          phaseMargin;           /*!< Phase margin in degrees */
    uint32_t                          loopBandwidth;         /*!< Loop bandwidth in hz*/    
} adrv904x_CpuCmd_SetLoopfilter_t;)

/**
 * \brief loopfilter set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetLoopfilterResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv904x_CpuCmd_SetLoopfilterResp_t;)

/**
 * \brief loopfilter get command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetLoopfilter
{
    adi_adrv904x_LoName_t             loName;                /*!< Select the target RF LO source to read back */
} adrv904x_CpuCmd_GetLoopfilter_t;)

/**
 * \brief loopfilter set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetLoopfilterResp
{
    adrv904x_CpuErrorCode_e     status;             /*!< CPU error status code */
    uint32_t                    phaseMargin;        /*!< Phase margin in degrees */
    uint32_t                    loopBandwidth;      /*!< Loop bandwidth in hz*/    
} adrv904x_CpuCmd_GetLoopfilterResp_t;)

/**
 * \brief loopfilter RX/TX LO command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetRxTxLoFreqResp
{
    adrv904x_CpuErrorCode_e status;                          /*!< CPU error status code */
    adi_adrv904x_LoName_t   rxLoName[ADRV904X_RX_CHAN_LEN];  /*!< PLL number of Rx Channels */    
    uint32_t                rxFreq[ADRV904X_RX_CHAN_LEN];    /*!< Freq of Rx Channels */  
    adi_adrv904x_LoName_t   txLoName[ADRV904X_TX_CHAN_LEN];  /*!< PLL number of Tx Channels */    
    uint32_t                txFreq[ADRV904X_TX_CHAN_LEN];    /*!< Freq of Tx Channels */  
} adrv904x_CpuCmd_GetRxTxLoFreqResp_t;)
	
/**
 * \brief Data structure to hold the Channel Controls to the PLL's.
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_ChanCtrlToPlls
{
	uint8_t rf0MuxTx0_3;       /* If set to 1 East Tx channels (Tx0-3) are connected to PLL0 (East PLL). If set to 0 Tx0-3 are connected to PLL1 (West PLL). */
	uint8_t rf0MuxTx4_7;       /* If set to 1 West Tx channels (Tx4-7) are connected to PLL0 (East PLL). If set to 0 Tx4-7 are connected to PLL1 (West PLL). */
	uint8_t rf0MuxRx0_3;       /* If set to 1 East Rx channels (Rx0-3) are connected to PLL0 (East PLL). If set to 0 Rx0-3 are connected to PLL1 (West PLL). */
	uint8_t rf0MuxRx4_7;       /* If set to 1 West Rx channels (Rx4-7) are connected to PLL0 (East PLL). If set to 0 Rx4-7 are connected to PLL1 (West PLL). */  
} adrv904x_CpuCmd_ChanCtrlToPlls_t;)
	
/**
 * \brief Data structure to hold the Channel Controls to the PLL's.
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_ChanCtrlToPllsResp
{
	adrv904x_CpuErrorCode_e     status; /*!< CPU error status code */
} adrv904x_CpuCmd_ChanCtrlToPllsResp_t;)

/**
 * \brief Reprogram PLL command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_ReprogramPll
{
    uint8_t pllSel;    /*!< PLL to be Reprogrammed according to adi_adrv904x_Pll_e */
} adrv904x_CpuCmd_ReprogramPll_t;)

/**
 * \brief Reprogram PLL command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_ReprogramPllResp
{
    adi_adrv904x_CpuErrorCode_t status;  /*!< CPU error status code */
} adrv904x_CpuCmd_ReprogramPllResp_t;)

#endif /* __ADRV904X_CPU_CMD_GETSET_LOFREQ_H__ */


