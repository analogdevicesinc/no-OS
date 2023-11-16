/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adrv904x_stream_proc_types.h
 *
 * \brief   Contains ADRV904X stream process macros and enums.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADRV904X_STREAM_PROC_TYPES_H__
#define __ADRV904X_STREAM_PROC_TYPES_H__



#include "adrv904x_stream_id_types.h"


#define ADRV904X_STREAM_NUM_RX_SLICE      (8)
#define ADRV904X_STREAM_NUM_TX_SLICE      (8)
#define ADRV904X_STREAM_NUM_ORX_SLICE     (2)
#define ADRV904X_STREAM_NUM_MAIN_SLICE    (1)

typedef enum
{
    ADRV904X_STREAM_ORX_ADC_DAC_ENABLE_RAMP  = ADRV904X_STREAM_ID__ORX__ORX_ADC_RAMP_ENABLE,    /*!< Custom stream: Enable ORx DAC ramp */
    ADRV904X_STREAM_ORX_ADC_DAC_DISABLE_RAMP = ADRV904X_STREAM_ID__ORX__ORX_ADC_RAMP_DISABLE,   /*!< Custom stream: Disable ORx DAC ramp */
    ADRV904X_STREAM_ORX_CUSTOM_RISE          = ADRV904X_STREAM_ID__ORX__ORX_CUSTOM_RISE,        /*!< Not defined yet */
    ADRV904X_STREAM_ORX_CUSTOM_FALL          = ADRV904X_STREAM_ID__ORX__ORX_CUSTOM_FALL,        /*!< Not defined yet */
    ADRV904X_STREAM_ORX_ADC_STBY_EN          = ADRV904X_STREAM_ID__ORX__ORX_ADC_STBY_EN,        /*!< Custom stream: Put ORx ADC into Standby */
    ADRV904X_STREAM_ORX_ADC_STBY_DIS         = ADRV904X_STREAM_ID__ORX__ORX_ADC_STBY_DIS,       /*!< Custom stream: Take ORx ADC out of Standby */
} adrv904x_StreamOrxCustomStreams_e;

typedef enum
{
    ADRV904X_STREAM_RX_CUSTOM_RISE            = ADRV904X_STREAM_ID__RX__RX_CUSTOM_RISE,            /*!< Not defined yet */
    ADRV904X_STREAM_RX_CUSTOM_FALL            = ADRV904X_STREAM_ID__RX__RX_CUSTOM_FALL,            /*!< Not defined yet */

    ADRV904X_STREAM_RX_SET_DDC0_RSSI_REQ_FLAG = ADRV904X_STREAM_ID__RX__RX_SET_DDC0_RSSI_REQ_FLAG, /*!< Custom stream: Set DDC0 RSSI Enable requested flag */
    ADRV904X_STREAM_RX_SET_DDC1_RSSI_REQ_FLAG = ADRV904X_STREAM_ID__RX__RX_SET_DDC1_RSSI_REQ_FLAG, /*!< Custom stream: Set DDC1 RSSI Enable requested flag */
} adrv904x_StreamRxCustomStreams_e;

typedef enum
{
    ADRV904X_STREAM_TX_VGA_OUT_ENABLE_TO_LB             = ADRV904X_STREAM_ID__TX__TX_LOOPBACK1_ENABLE_REQUEST,      /*!< Custom stream: Enable Tx VGA output to loopback path */
    ADRV904X_STREAM_TX_VGA_OUT_DISABLE_TO_LB            = ADRV904X_STREAM_ID__TX__TX_LOOPBACK1_DISABLE_REQUEST,     /*!< Custom stream: Disable Tx VGA output to loopback path */
    ADRV904X_STREAM_TX_VGA_IN_ENABLE_TO_LB              = ADRV904X_STREAM_ID__TX__TX_LOOPBACK2_ENABLE_REQUEST,      /*!< Custom stream: Enable Tx VGA input to loopback path */
    ADRV904X_STREAM_TX_VGA_IN_DISABLE_TO_LB             = ADRV904X_STREAM_ID__TX__TX_LOOPBACK2_DISABLE_REQUEST,     /*!< Custom stream: Disable Tx VGA input to loopback path */
    ADRV904X_STREAM_TX_ADC_DAC_ENABLE_RAMP              = ADRV904X_STREAM_ID__TX__TXLB_ADC_RAMP_ENABLE,             /*!< Custom stream: Enable TxLB ADC DAC ramp */
    ADRV904X_STREAM_TX_ADC_DAC_DISABLE_RAMP             = ADRV904X_STREAM_ID__TX__TXLB_ADC_RAMP_DISABLE,            /*!< Custom stream: Disable TxLB ADC DAC ramp */
    ADRV904X_STREAM_TX_WRITE_DTX_MODE_CONFIG            = ADRV904X_STREAM_ID__TX__WRITE_DTX_MODE_CONFIG,            /*!< Custom stream: Write DTX Mode Config */
    ADRV904X_STREAM_TX_WRITE_DTX_MODE_CONFIG_CLRSTATUS  = ADRV904X_STREAM_ID__TX__WRITE_DTX_MODE_CONFIG_CLRSTATUS,  /*!< Custom stream: Write DTX Mode Config And Set Clear-Status bits */
    ADRV904X_STREAM_TX_PA_PROT_RECOVERY                 = ADRV904X_STREAM_ID__TX__PA_PROT_RECOVERY,                 /*!< Custom stream: PA Protection Recovery */

    ADRV904X_STREAM_TX_SET_IMMEDIATE_CAPTURE_ENABLE_BIT = ADRV904X_STREAM_ID__TX__SET_IMMEDIATE_CAPTURE_ENABLE_BIT, /*!< Custom stream: Set immediate capture enable bit */
    ADRV904X_STREAM_TX_SET_CAPTURE_PENDING_BIT          = ADRV904X_STREAM_ID__TX__SET_CAPTURE_PENDING_BIT,          /*!< Custom stream: Set capture pending bit */
    ADRV904X_STREAM_TX_CLEAR_CAPTURE_PENDING_BIT        = ADRV904X_STREAM_ID__TX__CLEAR_CAPTURE_PENDING_BIT,        /*!< Custom stream: Clear capture pending bit */
    ADRV904X_STREAM_TX_WRITE_DTX_MANUAL_ACTIVATE        = ADRV904X_STREAM_ID__TX__DTX_MANUAL_ACTIVATE,              /*!< Custom stream: Manual DTX activate */
    ADRV904X_STREAM_TX_WRITE_DTX_MANUAL_DEACTIVATE      = ADRV904X_STREAM_ID__TX__DTX_MANUAL_DEACTIVATE,            /*!< Custom stream: Manual DTX activate */
    ADRV904X_STREAM_TX_SET_CFR_CFG_SET_CHANGE_REQ       = ADRV904X_STREAM_ID__TX__CFR_CFG_SET_CHANGE_REQ,           /*!< Custom stream: Set CFR config set change request bit */
    ADRV904X_STREAM_TX_DPD_MODEL_SWITCH_PREPARE_0       = ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_0,       /*!< Custom stream: Prepare to switch to DPD Model 0 */
    ADRV904X_STREAM_TX_DPD_MODEL_SWITCH_PREPARE_1       = ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_1,       /*!< Custom stream: Prepare to switch to DPD Model 1 */
    ADRV904X_STREAM_TX_DPD_MODEL_SWITCH_PREPARE_2       = ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_2,       /*!< Custom stream: Prepare to switch to DPD Model 2 */
    ADRV904X_STREAM_TX_DPD_MODEL_SWITCH_PREPARE_3       = ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_3,       /*!< Custom stream: Prepare to switch to DPD Model 3 */
    ADRV904X_STREAM_TX_DPD_MODEL_SWITCH_PREPARE_4       = ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_4,       /*!< Custom stream: Prepare to switch to DPD Model 4 */
    ADRV904X_STREAM_TX_DPD_MODEL_SWITCH_TX_FALL_SET     = ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_TX_FALL_SET,     /*!< Custom stream: Switch DPD model in Tx Fall stream */
    ADRV904X_STREAM_TX_DPD_MODEL_SWITCH_TX_FALL_CLEAR   = ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_TX_FALL_CLEAR,   /*!< Custom stream: Don't switch DPD model in Tx Fall stream */
} adrv904x_StreamTxCustomStreams_e;

typedef enum
{
    ADRV904X_STREAM_MAIN_POWER_UP                           = ADRV904X_STREAM_ID__CORE__POWERUP,                                          /*!< Power up stream in main stream */
    ADRV904X_STREAM_MAIN_TX_TO_ORX_MAP                      = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_API_TRIGGER,                    /*!< Stream triggered by adi_adrv904x_TxToOrxMappingSet */
    ADRV904X_STREAM_MAIN_SET_ORX0_ATT                       = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_ATTEN_ORX0,       /*!< Custom stream: Set ORx attenuation for ORx0 */
    ADRV904X_STREAM_MAIN_SET_ORX1_ATT                       = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_ATTEN_ORX1,       /*!< Custom stream: Set ORx attenuation for ORx1 */
    ADRV904X_STREAM_MAIN_SET_ORX0_NCO_FREQ                  = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_NCO_ORX0,         /*!< Custom stream: Set ORx ADC/DP NCO frequency for ORx0 */
    ADRV904X_STREAM_MAIN_SET_ORX1_NCO_FREQ                  = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_NCO_ORX1,         /*!< Custom stream: Set ORx ADC/DP NCO frequency for ORx1 */
    ADRV904X_STREAM_MAIN_ORX_ATT_TABLE_UPDATE               = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ORX_ATT_TABLE_UPDATE,           /*!< Custom stream: Update ORx attenuation table */
    ADRV904X_STREAM_MAIN_ORX_NCO_FREQ_TABLE_UPDATE          = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ORX_NCO_FREQ_TABLE_UPDATE,      /*!< Custom stream: Update ORx ADC/DP and LOL NCO frequency tables */
    ADRV904X_STREAM_MAIN_ONLY_LOL_NCO_FREQ_TABLE_UPDATE     = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ONLY_LOL_NCO_FREQ_TABLE_UPDATE, /*!< Custom stream: Update only LOL NCO frequency tables */
    ADRV904X_STREAM_MAIN_TRIGGER_TX_STREAM                  = ADRV904X_STREAM_ID__CORE__TRIGGER_TX_STREAM,                                /*!< Core stream used to trigger TX streams (see ADRV904X_TRIGGER_SLICE_STREAM_[NUM|MASK]) */
    ADRV904X_STREAM_MAIN_TRIGGER_RX_STREAM                  = ADRV904X_STREAM_ID__CORE__TRIGGER_RX_STREAM,                                /*!< Core stream used to trigger RX streams (see ADRV904X_TRIGGER_SLICE_STREAM_[NUM|MASK]) */


    ADRV904X_STREAM_MAIN_TRIGGER_TX_STREAM_USED_BY_DFE      = ADRV904X_STREAM_ID__CORE__TRIGGER_TX_STREAM_USED_BY_DFE,                         /*!< Core stream used to trigger TX streams by DFE (see ADRV904X_TRIGGER_SLICE_STREAM_[NUM|MASK]) */
    ADRV904X_STREAM_MAIN_TRIGGER_RX_STREAM_USED_BY_DFE      = ADRV904X_STREAM_ID__CORE__TRIGGER_RX_STREAM_USED_BY_DFE,                         /*!< Core stream used to trigger RX streams by DFE (see ADRV904X_TRIGGER_SLICE_STREAM_[NUM|MASK]) */
    ADRV904X_STREAM_MAIN_CFG_TX_ORX0_MAPPING_IN_RS_MODE     = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_CFG_GPIO_PIN_STATE_FOR_ORX0,         /*!< Custom stream: Configure Tx to ORx0 mapping in RS mode */
    ADRV904X_STREAM_MAIN_CFG_TX_ORX1_MAPPING_IN_RS_MODE     = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_CFG_GPIO_PIN_STATE_FOR_ORX1,         /*!< Custom stream: Configure Tx to ORx1 mapping in RS mode */
    ADRV904X_STREAM_MAIN_ACT_TX_ORX0_MAPPING_IN_RS_MODE     = ADRV904X_STREAM_ID__CORE__RADIO_SEQ_RISE_EVENT_00,                               /*!< Custom stream: Activate Tx to ORx0 mapping in RS mode */
    ADRV904X_STREAM_MAIN_ACT_TX_ORX1_MAPPING_IN_RS_MODE     = ADRV904X_STREAM_ID__CORE__RADIO_SEQ_RISE_EVENT_01,                               /*!< Custom stream: Activate Tx to ORx1 mapping in RS mode */
    ADRV904X_STREAM_MAIN_ORX0_ALL_OFF_PIN_STATE_IN_RS_MODE  = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_CFG_GPIO_ALL_OFF_PIN_STATE_FOR_ORX0, /*!< Custom stream: Configure and Activate All Off pin state for ORX0 in RS mode */
    ADRV904X_STREAM_MAIN_ORX1_ALL_OFF_PIN_STATE_IN_RS_MODE  = ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_CFG_GPIO_ALL_OFF_PIN_STATE_FOR_ORX1, /*!< Custom stream: Configure and Activate All Off pin state for ORX0 in RS mode */
    ADRV904X_STREAM_EXT_ALARM_STATE_CLEAR                   = ADRV904X_STREAM_ID__CORE__EA_CLEAR_HDLR_STRM,                                    /*!< Custom stream: Ext alarm state clear */
    ADRV904X_STREAM_EXT_ALARM_UPDATE_ALARM_OUTPUTS          = ADRV904X_STREAM_ID__CORE__EA_UPDATE_ALARM_OUTPUTS_STRM,                          /*!< Custom stream: Update alarm controlled outputs */
    ADRV904X_STREAM_OC_FUSE_EN_OUT                          = ADRV904X_STREAM_ID__CORE__EA_OC_FUSE_EN_SET_STRM,                                /*!< Custom stream: OC Fuse En State */
    ADRV904X_STREAM_DFE_ACT_DMA_TX_REQUEST_SET              = ADRV904X_STREAM_ID__CORE__DFE_ACT_LUT_COPY_TX_REQUEST_SET,                       /*!< Custom stream: Set flag for DFE Act LUT TX Copy Request */
    ADRV904X_STREAM_DFE_ACT_DMA_TX_IN_PROGRESS_CLEAR        = ADRV904X_STREAM_ID__CORE__DFE_ACT_LUT_COPY_TX_IN_PROGRESS_CLEAR,                 /*!< Custom stream: Clear flag DFE Act LUT TX Copy In Progress */
    ADRV904X_STREAM_FB_SW_00                                = ADRV904X_STREAM_ID__CORE__RADIO_SEQ_RISE_EVENT_00,                               /*!< Custom stream: Clear FB SW  */
    ADRV904X_STREAM_FB_SW_01                                = ADRV904X_STREAM_ID__CORE__RADIO_SEQ_RISE_EVENT_01,                               /*!< Custom stream: Clear FB SW  */
    ADRV904X_STREAM_DFE_CLGC_POWER_METERS_RESTART_ORX0      = ADRV904X_STREAM_ID__CORE__CLGC_POWER_METERS_RESTART_ORX0,                        /*!< Custom stream: Reset CLGC power meters in RCI mode */
    ADRV904X_STREAM_DFE_CLGC_POWER_METERS_RESTART_ORX1      = ADRV904X_STREAM_ID__CORE__CLGC_POWER_METERS_RESTART_ORX1,                        /*!< Custom stream: Reset CLGC power meters in RCI mode */
    ADRV904X_STREAM_ANT_CAL_UPDATE_SLICE_SCRATCHPAD_REG     = ADRV904X_STREAM_ID__CORE__UPDATE_SLICE_SCRATCHPAD_REG                            /*!< Custom stream: Update Rx/Tx slice scratchpad registers */
} adrv904x_StreamMainCustomStreams_e;

#endif /* __ADRV904X_STREAM_PROC_TYPES_H__ */


