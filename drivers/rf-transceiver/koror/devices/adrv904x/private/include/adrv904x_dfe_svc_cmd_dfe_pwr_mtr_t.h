/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adrv904x_dfe_svc_cmd_dfe_pwr_mtr_t.h
 *
 * \brief   Contains shared SDK/DFE power meter command typedefs
 *
 * \details Contains DFE error code definitions
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADRV904X_DFE_SVC_CMD_DFE_PWR_MTR_T_H__
#define __ADRV904X_DFE_SVC_CMD_DFE_PWR_MTR_T_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dfe_svc_err_codes_t.h"
#include "adrv904x_dfe_svc_cmd_t.h"
#include "adi_adrv904x_dfe_svc_dfe_pwr_mtr_t.h"

/**
 * \brief SDK DFE power meter TSSI config set structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_CONFIG_SET)
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrTssiConfigSet
{
    uint8_t                    chanMask;
    uint8_t                    meter;   /* of type adi_adrv904x_DfeSvcDfePwrMtrTssi_e */
    uint8_t                    source;  /* of type adi_adrv904x_DfeSvcDfePwrMtrTssiSrc_e */
    adi_adrv904x_DfeSvcDfePwrMtrTssiCfg_t pwrMtrTssiCfg;
} adrv904x_DfeSvcCmdDfeMtrTssiConfigSet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter TSSI config get structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_CONFIG_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrTssiConfigGet
{
    uint8_t chanMask;
    uint8_t meter;    /* of type adi_adrv904x_DfeSvcDfePwrMtrTssi_e */
} adrv904x_DfeSvcCmdDfeMtrTssiConfigGet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter TSSI config get response structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_CONFIG_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct _adrv904x_DfeSvcCmdDfeMtrTssiConfigGetResp
{
    adi_adrv904x_DfeSvcErrCode_e          status;
    uint8_t                    chanMask;
    uint8_t                    meter;     /* of type adi_adrv904x_DfeSvcDfePwrMtrTssi_e */
    adi_adrv904x_DfeSvcDfePwrMtrTssiCfg_t pwrMtrTssiCfg;
} adrv904x_DfeSvcCmdDfeMtrTssiConfigGetResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter TSSI enable command structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_ENABLE)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrTssiEnable
{
    uint8_t chanMask;
    uint8_t enable;
    uint8_t meter;      /* of type adi_adrv904x_DfeSvcDfePwrMtrTssi_e */
} adrv904x_DfeSvcCmdDfeMtrTssiEnable_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter CLGC TSSI enable command structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_CLGC_TSSI_ENABLE)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrClgcTssiEnable
{
    uint8_t chanMask;
    uint8_t enable;
    uint8_t useTxMeter1;  /* use ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_TX_1 */
    uint8_t useTxMeter2;  /* use ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_TX_2 */
} adrv904x_DfeSvcCmdDfeMtrClgcTssiEnable_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter TSSI manual start command structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_MANUAL_START)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrTssiManualStart
{
    uint8_t chanMask;
    uint8_t start;
    uint8_t meter;       /* of type adi_adrv904x_DfeSvcDfePwrMtrTssi_e */
} adrv904x_DfeSvcCmdDfeMtrTssiManualStart_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter TSSI readback command structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_READBACK)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrTssiReadback
{
    uint8_t chanMask;
    uint8_t meter;         /* of type adi_adrv904x_DfeSvcDfePwrMtrTssi_e */
} adrv904x_DfeSvcCmdDfeMtrTssiReadback_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter TSSI readback response structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_READBACK)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrTssiReadbackResp
{
    adi_adrv904x_DfeSvcErrCode_e status;
    uint8_t           chanMask;            /* only one channel allowed */
    uint8_t           meter;               /* of type adi_adrv904x_DfeSvcDfePwrMtrTssi_e */
    uint8_t           pwrMeasDb;           /* power in dB (resolution is 0.25 dB) */
    uint64_t          pwrMeasLinear;       /* linear power */
    uint8_t           thresExceeded;
} adrv904x_DfeSvcCmdDfeMtrTssiReadbackResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter CLGC/TPC TSSI readback command structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_CLGC_TSSI_READBACK)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrClgcTssiReadback
{
    uint8_t txChanMask;                   /* Tx channel mask - only one channel allowed */
    bool    useTxMeter1;                  /* of type bool */
    bool    useTxMeter2;                  /* of type bool */
} adrv904x_DfeSvcCmdDfeMtrClgcTssiReadback_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter CLGC/TPC TSSI readback response structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_CLGC_TSSI_READBACK)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrClgcTssiReadbackResp
{
    adi_adrv904x_DfeSvcErrCode_e status;
    uint8_t           txChanMask;          /* Tx channel mask - copied from request cmd */
    bool              useTxMeter1;         /* of type bool - copied from request cmd */
    bool              useTxMeter2;         /* of type bool - copied from request cmd */
    uint64_t          pwrMeasTx1;          /* linear power for TSSI Tx 1 */
    uint64_t          pwrMeasTx2;          /* linear power for TSSI Tx 2 */
    uint64_t          pwrMeasOrx;          /* linear power for TSSI ORx  */
} adrv904x_DfeSvcCmdDfeMtrClgcTssiReadbackResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter DPD In/out config set command (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_CONFIG_SET, ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_CONFIG_SET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrDpdConfig
{
    adi_adrv904x_DfeSvcErrCode_e         status;
    uint8_t                   txChanMask;
    adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t pwrMtrDpdConfig;
} adrv904x_DfeSvcCmdDfeMtrDpdConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter DPD In/out config get response (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_CONFIG_GET, ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_CONFIG_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrDpdConfigGetResp
{
    adi_adrv904x_DfeSvcErrCode_e         status;
    uint8_t                   txChanMask;
    adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t pwrMtrDpdConfig;
} adrv904x_DfeSvcCmdDfeMtrDpdConfigGetResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter DPD In/out config get command (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_CONFIG_GET, ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_CONFIG)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrDpdConfigGet
{
    uint8_t txChanMask;
} adrv904x_DfeSvcCmdDfeMtrDpdConfigGet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter DPD In/out enable command (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_ENABLE, ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_ENABLE)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrDpdEnable
{
    uint8_t txChanMask;
    uint8_t enable;
} adrv904x_DfeSvcCmdDfeMtrDpdEnable_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter DPD In/out pause command (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_PAUSE, ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_PAUSE)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrDpdPause
{
    uint8_t txChanMask;
    uint8_t pause;
} adrv904x_DfeSvcCmdDfeMtrDpdPause_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter DPD In/out readback command structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_OUT_READBACK)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrDpdReadback
{
    uint8_t txChannel;
} adrv904x_DfeSvcCmdDfeMtrDpdReadback_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter DPD In/out readback response structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_OUT_READBACK)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrDpdReadbackResp
{
    adi_adrv904x_DfeSvcErrCode_e             status;
    uint8_t                       txChannel;
    adi_adrv904x_DfeSvcDfePwrMtrDpdResults_t pwrMtrResp;
} adrv904x_DfeSvcCmdDfeMtrDpdReadbackResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Tx carrier power measurement config set command structure (ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_CONFIG_SET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasConfig
{
    uint32_t                           channelMask;
    uint32_t                           carrierMask;
    adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg_t cfg;
} adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Tx carrier power measurement config get command structure (ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_CONFIG_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasConfigGet
{
    uint32_t channelMask;
    uint32_t carrierMask;
} adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasConfigGet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Tx carrier power measurement config get response structure (ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_CONFIG_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasConfigGetResp
{
    adi_adrv904x_DfeSvcErrCode_e                  status;
    uint32_t                           channelMask;
    uint32_t                           carrierMask;
    adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg_t cfg;
} adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasConfigGetResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Tx carrier power measurement enable command structure (ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_ENABLE)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasEnable
{
    uint32_t channelMask;
    uint32_t carrierMask;
    uint8_t  enable;
} adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasEnable_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief  SDK DFE Tx carrier power measurement manual start command structure (ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MANUAL_START)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasManualStart
{
    uint32_t channelMask;
    uint32_t carrierMask;
    uint8_t  start;
} adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasManualStart_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Tx carrier power measurement readback command structure (ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasGet
{
    uint32_t channelMask;
    uint32_t carrierMask;
} adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasGet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Tx carrier power measurement readback response structure (ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasGetResp
{
    adi_adrv904x_DfeSvcErrCode_e                     status;
    uint32_t                              channelMask;
    uint32_t                              carrierMask;
    adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasResult_t result;
} adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasGetResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter RSSI (DDC/CDDC) config set structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_CONFIG_SET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrRssiConfigSet
{
    uint8_t                    meterMask;   /* multiple meters allowed (if of the same type) */
    uint8_t                    chanMask;    /* multiple channels allowed */
    adi_adrv904x_DfeSvcDfePwrMtrRssiCfg_t pwrMtrRssiCfg;
} adrv904x_DfeSvcCmdDfeMtrRssiConfigSet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter RSSI (DDC/CDDC) config get structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_CONFIG_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrRssiConfigGet
{
    uint8_t meterMask;   /* only one meter allowed */
    uint8_t chanMask;    /* only one channel allowed */
} adrv904x_DfeSvcCmdDfeMtrRssiConfigGet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter RSSI (DDC/CDDC) config get response structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_CONFIG_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct _adrv904x_DfeSvcCmdDfeMtrRssiConfigGetResp
{
    adi_adrv904x_DfeSvcErrCode_e          status;
    uint8_t                    meterMask;   /* only one meter allowed */
    uint8_t                    chanMask;    /* only one channel allowed */
    adi_adrv904x_DfeSvcDfePwrMtrRssiCfg_t pwrMtrRssiCfg;
} adrv904x_DfeSvcCmdDfeMtrRssiConfigGetResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter RSSI (DDC/CDDC) enable command structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_ENABLE)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrRssiEnable
{
    uint8_t meterMask;   /* multiple meters allowed */
    uint8_t chanMask;    /* multiple channels allowed */
    uint8_t enable;
} adrv904x_DfeSvcCmdDfeMtrRssiEnable_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter RSSI( DDC/CDDC) readback command structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_READBACK)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrRssiReadback
{
    uint8_t meterMask; /* only one meter allowed */
    uint8_t chanMask;  /* only one channel allowed */
} adrv904x_DfeSvcCmdDfeMtrRssiReadback_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE power meter RSSI (DDC/CDDC) readback response structure (ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_READBACK)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeMtrRssiReadbackResp
{
    adi_adrv904x_DfeSvcErrCode_e status;
    uint8_t           meterMask;     /* only one meter allowed */
    uint8_t           chanMask;      /* only one channel allowed */
    uint8_t           pwrMeasDb;     /* db power */
    uint64_t          pwrMeasLinear; /* linear power */
} adrv904x_DfeSvcCmdDfeMtrRssiReadbackResp_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_DFE_SVC_CMD_DFE_PWR_MTR_T_H__ */



