/**
 * \file
 * \brief Contains ADRV9001 Frequency Hopping related private data types
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2021 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADRV9001_FH_TYPES_H_
#define _ADRV9001_FH_TYPES_H_

#include "adi_adrv9001.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Settings for HOP frame information to match ARM firmware struct type
 */
typedef struct {
    uint32_t hopFrequencyHz_LSB;		/*!< Lower 32-bits of Operating frequency in Hz */
    uint32_t hopFrequencyHz_MSB;		/*!< Upper 32-bits of Operating frequency in Hz */
    int32_t  rx1OffsetFrequencyHz;		/*!< Rx1 Offset frequency. This field is ignored by firmware if frame is not Rx, or if profile does not operate with an IF */
    int32_t  rx2OffsetFrequencyHz;		/*!< Rx2 Offset frequency. This field is ignored by firmware if frame is not Rx, or if profile does not operate with an IF */
	uint8_t  tx1Attenuation_fifthdB;	/*!< Tx1 attenuation expressed in 0.2dBs. LSB = 0.2dB. Range 0 to 209 (0 to 41.8 dB). This field is ignored if frame is Rx */
	uint8_t  tx2Attenuation_fifthdB;	/*!< Tx2 attenuation expressed in 0.2dBs. Range 0 to 209 (0 to 41.8 dB). This field is ignored if frame is Rx */
    uint8_t  rx1GainIndex;				/*!< Starting Rx1 gain index for hop frame. This field is ignored if frame is Tx */
	uint8_t  rx2GainIndex;				/*!< Starting Rx2 gain index for hop frame. This field is ignored if frame is Tx */
} adrv9001_FhHopFrame_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADRV9001_FH_TYPES_H_ */
