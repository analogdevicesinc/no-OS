/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_app_capture_sequencer_t.h
 *
 * \brief   Contains ADRV904X Capture sequencer types
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef ADRV904X_DFE_APP_CAPTURE_SEQUENCER_T_H__
#define ADRV904X_DFE_APP_CAPTURE_SEQUENCER_T_H__

#include "adi_adrv904x_platform_pack.h"


ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCaptureSequencerCfg
{
    uint8_t  skipCapturePeriodIfNoRequest; /*!< 1: Skip scheduling capture period if there is no pending request from application thread
                                            *    0: Schedule capture period even if there is no pending request from application thread */
    uint8_t  singleCapturePeriod;          /*!< 1: Allow only one single capture period to be active at a time (either Orx0 or Orx1)
                                            *   0: Allow Orx0 and Orx1 simultaneous capture periods */
    uint8_t  dummyCapPerNumOfMultiframes;  /*!< When running dummy capture periods to let radio cals to get mapping, use this number of multiframes in RS default (non-override)mode. Min value:1 */
    uint8_t  dummyCapPerLengthMs;          /*!< When running dummy capture periods to let radio cals to get mapping, use this length[ms] in RCI and RS mapping override modes. Min value:2 */
    uint16_t dummyCapPerMinimumPeriodMs;   /*!< This is the minimum scheduling period for dummy capture periods (per channel). Min value:100, Max value:5000 */
} adi_adrv904x_DfeAppCaptureSequencerCfg_t;
ADI_ADRV904X_PACK_FINISH

#endif /* ADRV904X_DFE_APP_CAPTURE_SEQUENCER_T_H__ */

