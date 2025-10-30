/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file
 *
 * \brief   Contains ADRV903X stream id definitions
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADRV903X_STREAM_ID_H__
#define __ADRV903X_STREAM_ID_H__

#define ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_ATTEN_ORX0          (1U)
#define ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_ATTEN_ORX1          (2U)
#define ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_NCO_ORX0            (3U)
#define ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_IMMEDIATE_SET_NCO_ORX1            (4U)
#define ADRV903X_STREAM_ID__CORE__POWERUP                                             (128U)
#define ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_API_TRIGGER                       (129U)
#define ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ORX_ATT_TABLE_UPDATE              (140U)
#define ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ORX_NCO_FREQ_TABLE_UPDATE         (141U)
#define ADRV903X_STREAM_ID__CORE__TX_TO_ORX_MAPPING_ONLY_LOL_NCO_FREQ_TABLE_UPDATE    (143U)
#define ADRV903X_STREAM_ID__CORE__TRIGGER_TX_STREAM                                   (176U)
#define ADRV903X_STREAM_ID__CORE__TRIGGER_RX_STREAM                                   (177U)
#define ADRV903X_STREAM_ID__TX__DTX_POWER_UP                                          (5U)
#define ADRV903X_STREAM_ID__TX__TX_LOOPBACK1_ENABLE_REQUEST                           (17U)
#define ADRV903X_STREAM_ID__TX__TX_LOOPBACK1_DISABLE_REQUEST                          (18U)
#define ADRV903X_STREAM_ID__TX__TX_LOOPBACK2_ENABLE_REQUEST                           (19U)
#define ADRV903X_STREAM_ID__TX__TX_LOOPBACK2_DISABLE_REQUEST                          (20U)
#define ADRV903X_STREAM_ID__TX__TXLB_ADC_RAMP_ENABLE                                  (29U)
#define ADRV903X_STREAM_ID__TX__TXLB_ADC_RAMP_DISABLE                                 (30U)
#define ADRV903X_STREAM_ID__TX__WRITE_DTX_MODE_CONFIG                                 (33U)
#define ADRV903X_STREAM_ID__TX__TX_SET_INIT_CAL_RUNNING_BIT                           (38U)
#define ADRV903X_STREAM_ID__TX__TX_CLR_INIT_CAL_RUNNING_BIT                           (39U)
#define ADRV903X_STREAM_ID__TX__PA_PROT_CLEAR_ACTIVE_FLAG                             (41U)
#define ADRV903X_STREAM_ID__TX__WRITE_DTX_MODE_CONFIG_CLRSTATUS                       (42U)
#define ADRV903X_STREAM_ID__TX__PA_PROT_ACTIVATE                                      (43U)
#define ADRV903X_STREAM_ID__TX__PA_PROT_CHECK_IF_ACTIVE                               (44U)
#define ADRV903X_STREAM_ID__RX__RX_CUSTOM_RISE                                        (21U)
#define ADRV903X_STREAM_ID__RX__RX_CUSTOM_FALL                                        (22U)
#define ADRV903X_STREAM_ID__ORX__ORX_ADC_RAMP_ENABLE                                  (8U)
#define ADRV903X_STREAM_ID__ORX__ORX_ADC_RAMP_DISABLE                                 (9U)
#define ADRV903X_STREAM_ID__ORX__ORX_ADC_STBY                                         (12U)
#define ADRV903X_STREAM_ID__ORX__ORX_ADC_STBY_EN                                      (13U)
#define ADRV903X_STREAM_ID__ORX__ORX_ADC_STBY_DIS                                     (14U)
#define ADRV903X_STREAM_ID__ORX__ORX_ADC_NOT_STBY                                     (15U)
/* !!! This dummy stream must be the last stream!!! */
#define ADRV903X_STREAM_ID__ORX__ORX_DUMMY                                            (16U)

#define ADRV903X_STREAM_ID__ORX__ORX_CUSTOM_RISE                                      (21U)
#define ADRV903X_STREAM_ID__ORX__ORX_CUSTOM_FALL                                      (22U)
#endif /* __ADRV903X_STREAMID_H__ */


