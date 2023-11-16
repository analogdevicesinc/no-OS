/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file
 * \brief DFE capture service module definitions and types
 *
 * DFE SDK API Version: 2.9.0.4
 */

#ifndef ADI_ADRV904X_DFE_SVC_DFE_VSWR_CMN_T_H_
#define ADI_ADRV904X_DFE_SVC_DFE_VSWR_CMN_T_H_

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"

#define ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES    1023U /*!< VSWR playback data maximum number of samples */

/**
 * \brief VSWR playback data. It contains 1023 samples maximum
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_VswrPlaybackData
{
    int16_t  playbackDataReal[ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES]; /*!< An array of samples for VSWR playback - 16bit real samples */
    int16_t  playbackDataImag[ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES]; /*!< An array of samples for VSWR playback - 16bit imag samples */
    uint16_t numSamples;                                                       /*!< No. of samples contained in playbackDataReal and playbackDataImag arrays */
}adi_adrv904x_VswrPlaybackData_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief VSWR interleaved I/Q playback data. It contains 1023 samples maximum
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcCaptureVswrPlaybackInterleavedData
{
    int16_t  playbackData[ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES * 2]; /*!< An array of I/Q interleaved samples for VSWR playback */
    uint16_t numSamples;                                                       /*!< No. of I/Q interleaved samples contained in playbackData */
}adi_adrv904x_DfeSvcCaptureVswrPlaybackInterleavedData_t;
ADI_ADRV904X_PACK_FINISH
#endif /* ADI_ADRV904X_DFE_SVC_DFE_VSWR_CMN_T_H_ */

