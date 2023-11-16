/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file
 *
 * \brief   Contains ADRV904X stream id definitions
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADRV904X_STREAM_ID_H__
#define __ADRV904X_STREAM_ID_H__

#define ADRV904X_STREAM_ID__CORE__RADIO_SEQ_RISE_EVENT_00                                    (128U)
#define ADRV904X_STREAM_ID__CORE__RADIO_SEQ_RISE_EVENT_01                                    (129U)
#define ADRV904X_STREAM_ID__CORE__ANT_CAL_SET_OUTPUT_GPIO_SECONDARY_PATH_NORMAL_OPERATION    (172U)   /* Use what was RADIO_SEQ_FALL_EVENT_12 */
#define ADRV904X_STREAM_ID__CORE__ANT_CAL_SET_OUTPUT_GPIO_SECONDARY_PATH_UL_CAL              (173U)   /* Use what was RADIO_SEQ_FALL_EVENT_13 */
#define ADRV904X_STREAM_ID__CORE__ANT_CAL_SET_OUTPUT_GPIO_SECONDARY_PATH_DL_CAL              (174U)   /* Use what was RADIO_SEQ_FALL_EVENT_14 */
/* The RS Fall Events 15 to 31 (Streams 175 to 191) will not be used as initially planned and they're re-purposed as custom streams */
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_ATTEN_ORX0                 (105U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_ATTEN_ORX1                 (106U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_NCO_ORX0                   (107U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_NCO_ORX1                   (108U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_CFG_GPIO_PIN_STATE_FOR_ORX0              (109U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_CFG_GPIO_PIN_STATE_FOR_ORX1              (110U)
#define ADRV904X_STREAM_ID__CORE__EA_CLEAR_HDLR_STRM                                         (111U)
#define ADRV904X_STREAM_ID__CORE__POWERUP                                                    (124U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_API_TRIGGER                              (125U)
#define ADRV904X_STREAM_ID__CORE__DFE_ACT_LUT_COPY_TX_REQUEST_SET                            (144U)
#define ADRV904X_STREAM_ID__CORE__DFE_ACT_LUT_COPY_TX_IN_PROGRESS_CLEAR                      (145U)
#define ADRV904X_STREAM_ID__CORE__EA_UPDATE_ALARM_OUTPUTS_STRM                               (153U)
#define ADRV904X_STREAM_ID__CORE__TRIGGER_TX_STREAM_USED_BY_DFE                              (156U)
#define ADRV904X_STREAM_ID__CORE__TRIGGER_RX_STREAM_USED_BY_DFE                              (157U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_CFG_GPIO_ALL_OFF_PIN_STATE_FOR_ORX0      (180U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_CFG_GPIO_ALL_OFF_PIN_STATE_FOR_ORX1      (181U)
#define ADRV904X_STREAM_ID__CORE__UNUSED3                                                    (188U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ORX_ATT_TABLE_UPDATE                     (202U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ORX_NCO_FREQ_TABLE_UPDATE                (203U)
#define ADRV904X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ONLY_LOL_NCO_FREQ_TABLE_UPDATE           (204U)
#define ADRV904X_STREAM_ID__CORE__EA_OC_FUSE_EN_SET_STRM                                     (221U)
#define ADRV904X_STREAM_ID__CORE__CLGC_POWER_METERS_RESTART_ORX0                             (226U)
#define ADRV904X_STREAM_ID__CORE__CLGC_POWER_METERS_RESTART_ORX1                             (227U)
#define ADRV904X_STREAM_ID__CORE__UPDATE_SLICE_SCRATCHPAD_REG                                (229U)
#define ADRV904X_STREAM_ID__CORE__TRIGGER_TX_STREAM                                          (236U)
#define ADRV904X_STREAM_ID__CORE__TRIGGER_RX_STREAM                                          (237U)
#define ADRV904X_STREAM_ID__CORE__ANT_CAL_SET_OUTPUT_GPIO_MAIN_PATH_NORMAL_OPERATION         (243U)
#define ADRV904X_STREAM_ID__CORE__ANT_CAL_SET_OUTPUT_GPIO_MAIN_PATH_UL_CAL                   (244U)
#define ADRV904X_STREAM_ID__CORE__ANT_CAL_SET_OUTPUT_GPIO_MAIN_PATH_DL_CAL                   (245U)
#define ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_0                                   (7U)    /* Use what was TX__RADIO_SEQ_RISE_3 */
#define ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_1                                   (8U)    /* Use what was TX__RADIO_SEQ_RISE_4 */
#define ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_2                                   (9U)    /* Use what was TX__RADIO_SEQ_RISE_5 */
#define ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_3                                   (10U)   /* Use what was TX__RADIO_SEQ_RISE_6 */
#define ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_PREPARE_4                                   (11U)   /* Use what was TX__RADIO_SEQ_RISE_7 */
#define ADRV904X_STREAM_ID__TX__DTX_MANUAL_ACTIVATE                                          (36U)
#define ADRV904X_STREAM_ID__TX__DTX_MANUAL_DEACTIVATE                                        (37U)
#define ADRV904X_STREAM_ID__TX__TX_LOOPBACK1_ENABLE_REQUEST                                  (41U)
#define ADRV904X_STREAM_ID__TX__TX_LOOPBACK1_DISABLE_REQUEST                                 (42U)
#define ADRV904X_STREAM_ID__TX__TX_LOOPBACK2_ENABLE_REQUEST                                  (43U)
#define ADRV904X_STREAM_ID__TX__TX_LOOPBACK2_DISABLE_REQUEST                                 (44U)
#define ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_TX_FALL_SET                                 (52U)
#define ADRV904X_STREAM_ID__TX__TXLB_ADC_RAMP_ENABLE                                         (53U)
#define ADRV904X_STREAM_ID__TX__TXLB_ADC_RAMP_DISABLE                                        (54U)
#define ADRV904X_STREAM_ID__TX__DPD_MODEL_SWITCH_TX_FALL_CLEAR                               (57U)
#define ADRV904X_STREAM_ID__TX__WRITE_DTX_MODE_CONFIG                                        (63U)
#define ADRV904X_STREAM_ID__TX__SET_CAPTURE_TRIGGER_BIT                                      (64U)
#define ADRV904X_STREAM_ID__TX__CLEAR_CAPTURE_TRIGGER_BIT                                    (65U)
#define ADRV904X_STREAM_ID__TX__SET_IMMEDIATE_CAPTURE_ENABLE_BIT                             (66U)
#define ADRV904X_STREAM_ID__TX__SET_CAPTURE_PENDING_BIT                                      (67U)
#define ADRV904X_STREAM_ID__TX__CLEAR_CAPTURE_PENDING_BIT                                    (68U)
#define ADRV904X_STREAM_ID__TX__PA_PROT_RECOVERY                                             (85U)
#define ADRV904X_STREAM_ID__TX__WRITE_DTX_MODE_CONFIG_CLRSTATUS                              (87U)
#define ADRV904X_STREAM_ID__TX__CFR_CFG_SET_CHANGE_REQ                                       (88U)
#define ADRV904X_STREAM_ID__RX__RX_CUSTOM_RISE                     (21U)
#define ADRV904X_STREAM_ID__RX__RX_CUSTOM_FALL                     (22U)
#define ADRV904X_STREAM_ID__RX__RX_SET_DDC0_RSSI_REQ_FLAG          (36U)
#define ADRV904X_STREAM_ID__RX__RX_SET_DDC1_RSSI_REQ_FLAG          (37U)
#define ADRV904X_STREAM_ID__ORX__ORX_ADC_RAMP_ENABLE         (33U)
#define ADRV904X_STREAM_ID__ORX__ORX_ADC_RAMP_DISABLE        (34U)
#define ADRV904X_STREAM_ID__ORX__ORX_ADC_STBY                (37U)
#define ADRV904X_STREAM_ID__ORX__ORX_ADC_STBY_EN             (38U)
#define ADRV904X_STREAM_ID__ORX__ORX_ADC_STBY_DIS            (39U)
#define ADRV904X_STREAM_ID__ORX__ORX_ADC_NOT_STBY            (40U)

/* !!! This dummy stream must be the last stream!!! */
#define ADRV904X_STREAM_ID__ORX__ORX_DUMMY                   (41U)

#define ADRV904X_STREAM_ID__ORX__ORX_CUSTOM_RISE             (21U)
#define ADRV904X_STREAM_ID__ORX__ORX_CUSTOM_FALL             (22U)
#endif /* __ADRV904X_STREAMID_H__ */


