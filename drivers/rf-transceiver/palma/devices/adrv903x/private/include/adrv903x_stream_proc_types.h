/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adrv903x_stream_proc_types.h
 *
 * \brief   Contains ADRV903X stream process macros and enums.
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADRV903X_STREAM_PROC_TYPES_H__
#define __ADRV903X_STREAM_PROC_TYPES_H__



#include "adrv903x_stream_id_types.h"


#define ADRV903X_STREAM_NUM_RX_SLICE      (8)
#define ADRV903X_STREAM_NUM_TX_SLICE      (8)
#define ADRV903X_STREAM_NUM_ORX_SLICE     (2)
#define ADRV903X_STREAM_NUM_MAIN_SLICE    (1)

typedef enum
{
    ADRV903X_STREAM_ORX_ADC_DAC_ENABLE_RAMP  = ADRV903X_STREAM_ID__ORX__ORX_ADC_RAMP_ENABLE,    /*!< Custom stream: Enable ORx DAC ramp */
    ADRV903X_STREAM_ORX_ADC_DAC_DISABLE_RAMP = ADRV903X_STREAM_ID__ORX__ORX_ADC_RAMP_DISABLE,   /*!< Custom stream: Disable ORx DAC ramp */
    ADRV903X_STREAM_ORX_CUSTOM_RISE          = ADRV903X_STREAM_ID__ORX__ORX_CUSTOM_RISE,        /*!< Not defined yet */
    ADRV903X_STREAM_ORX_CUSTOM_FALL          = ADRV903X_STREAM_ID__ORX__ORX_CUSTOM_FALL,        /*!< Not defined yet */
    ADRV903X_STREAM_ORX_ADC_STBY_EN          = ADRV903X_STREAM_ID__ORX__ORX_ADC_STBY_EN,        /*!< Custom stream: Put ORx ADC into Standby */
    ADRV903X_STREAM_ORX_ADC_STBY_DIS         = ADRV903X_STREAM_ID__ORX__ORX_ADC_STBY_DIS,       /*!< Custom stream: Take ORx ADC out of Standby */
} adrv903x_StreamOrxCustomStreams_e;

typedef enum
{
    ADRV903X_STREAM_RX_CUSTOM_RISE            = ADRV903X_STREAM_ID__RX__RX_CUSTOM_RISE,            /*!< Not defined yet */
    ADRV903X_STREAM_RX_CUSTOM_FALL            = ADRV903X_STREAM_ID__RX__RX_CUSTOM_FALL,            /*!< Not defined yet */
} adrv903x_StreamRxCustomStreams_e;

typedef enum
{
    ADRV903X_STREAM_TX_VGA_OUT_ENABLE_TO_LB             = ADRV903X_STREAM_ID__TX__TX_LOOPBACK1_ENABLE_REQUEST,      /*!< Custom stream: Enable Tx VGA output to loopback path */
    ADRV903X_STREAM_TX_VGA_OUT_DISABLE_TO_LB            = ADRV903X_STREAM_ID__TX__TX_LOOPBACK1_DISABLE_REQUEST,     /*!< Custom stream: Disable Tx VGA output to loopback path */
    ADRV903X_STREAM_TX_VGA_IN_ENABLE_TO_LB              = ADRV903X_STREAM_ID__TX__TX_LOOPBACK2_ENABLE_REQUEST,      /*!< Custom stream: Enable Tx VGA input to loopback path */
    ADRV903X_STREAM_TX_VGA_IN_DISABLE_TO_LB             = ADRV903X_STREAM_ID__TX__TX_LOOPBACK2_DISABLE_REQUEST,     /*!< Custom stream: Disable Tx VGA input to loopback path */
    ADRV903X_STREAM_TX_ADC_DAC_ENABLE_RAMP              = ADRV903X_STREAM_ID__TX__TXLB_ADC_RAMP_ENABLE,             /*!< Custom stream: Enable TxLB ADC DAC ramp */
    ADRV903X_STREAM_TX_ADC_DAC_DISABLE_RAMP             = ADRV903X_STREAM_ID__TX__TXLB_ADC_RAMP_DISABLE,            /*!< Custom stream: Disable TxLB ADC DAC ramp */
    ADRV903X_STREAM_TX_WRITE_DTX_MODE_CONFIG            = ADRV903X_STREAM_ID__TX__WRITE_DTX_MODE_CONFIG,            /*!< Custom stream: Write DTX Mode Config */
    ADRV903X_STREAM_TX_WRITE_DTX_MODE_CONFIG_CLRSTATUS  = ADRV903X_STREAM_ID__TX__WRITE_DTX_MODE_CONFIG_CLRSTATUS,  /*!< Custom stream: Write DTX Mode Config And Set Clear-Status bits */
    ADRV903X_STREAM_TX_DTX_POWER_UP                     = ADRV903X_STREAM_ID__TX__DTX_POWER_UP,                     /*!< Custom stream: Exit from DTX mode */
    ADRV903X_STREAM_TX_SET_INIT_CAL_RUNNING_BIT         = ADRV903X_STREAM_ID__TX__TX_SET_INIT_CAL_RUNNING_BIT,      /*!< Custom stream: Set Init Cal Running Bit */
    ADRV903X_STREAM_TX_CLR_INIT_CAL_RUNNING_BIT         = ADRV903X_STREAM_ID__TX__TX_CLR_INIT_CAL_RUNNING_BIT,      /*!< Custom stream: Clear Init Cal Running Bit */
    ADRV903X_STREAM_TX_PA_PROT_CLEAR_ACTIVE_FLAG        = ADRV903X_STREAM_ID__TX__PA_PROT_CLEAR_ACTIVE_FLAG,        /*!< Custom stream: PAP Active Bit clear */
    ADRV903X_STREAM_TX_PA_PROT_CHECK_IF_ACTIVE          = ADRV903X_STREAM_ID__TX__PA_PROT_CHECK_IF_ACTIVE,          /*!< Custom stream: PAP Active Bit check */
    ADRV903X_STREAM_TX_PA_PROT_ACTIVATE                 = ADRV903X_STREAM_ID__TX__PA_PROT_ACTIVATE,                 /*!< Custom stream: PAP Activate Handling*/
} adrv903x_StreamTxCustomStreams_e;

typedef enum
{
    ADRV903X_STREAM_MAIN_POWER_UP                           = ADRV903X_STREAM_ID__CORE__POWERUP,                                          /*!< Power up stream in main stream */
    ADRV903X_STREAM_MAIN_TX_TO_ORX_MAP                      = ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_API_TRIGGER,                    /*!< Stream triggered by adi_adrv903x_TxToOrxMappingSet */
    ADRV903X_STREAM_MAIN_SET_ORX0_ATT                       = ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_ATTEN_ORX0,       /*!< Custom stream: Set ORx attenuation for ORx0 */
    ADRV903X_STREAM_MAIN_SET_ORX1_ATT                       = ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_ATTEN_ORX1,       /*!< Custom stream: Set ORx attenuation for ORx1 */
    ADRV903X_STREAM_MAIN_SET_ORX0_NCO_FREQ                  = ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_NCO_ORX0,         /*!< Custom stream: Set ORx ADC/DP NCO frequency for ORx0 */
    ADRV903X_STREAM_MAIN_SET_ORX1_NCO_FREQ                  = ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_NCO_ORX1,         /*!< Custom stream: Set ORx ADC/DP NCO frequency for ORx1 */
    ADRV903X_STREAM_MAIN_ORX_ATT_TABLE_UPDATE               = ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ORX_ATT_TABLE_UPDATE,           /*!< Custom stream: Update ORx attenuation table */
    ADRV903X_STREAM_MAIN_ORX_NCO_FREQ_TABLE_UPDATE          = ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ORX_NCO_FREQ_TABLE_UPDATE,      /*!< Custom stream: Update ORx ADC/DP and LOL NCO frequency tables */
    ADRV903X_STREAM_MAIN_ONLY_LOL_NCO_FREQ_TABLE_UPDATE     = ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ONLY_LOL_NCO_FREQ_TABLE_UPDATE, /*!< Custom stream: Update only LOL NCO frequency tables */
    ADRV903X_STREAM_MAIN_TRIGGER_TX_STREAM                  = ADRV903X_STREAM_ID__CORE__TRIGGER_TX_STREAM,                                /*!< Core stream used to trigger TX streams (see ADRV903X_TRIGGER_SLICE_STREAM_[NUM|MASK]) */
    ADRV903X_STREAM_MAIN_TRIGGER_RX_STREAM                  = ADRV903X_STREAM_ID__CORE__TRIGGER_RX_STREAM,                                /*!< Core stream used to trigger RX streams (see ADRV903X_TRIGGER_SLICE_STREAM_[NUM|MASK]) */

} adrv903x_StreamMainCustomStreams_e;

#endif /* __ADRV903X_STREAM_PROC_TYPES_H__ */


