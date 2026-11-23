/**
 * \file
 * \brief Contains ADRV910X API Calibration data types
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2024 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV910X_CAL_TYPES_H_
#define _ADI_ADRV910X_CAL_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "adi_adrv910x_types.h"

/**
 * @brief Enum to select single or differential mode settings for Lo interface calibration
*/
typedef enum adi_adrv910x_LoGenDifferentialMode
{
    ADI_ADRV910X_CAL_LOGEN_DIFFERENTIAL, /* Positive Input to Positive Output, Negative Input to Negative Output */
    ADI_ADRV910X_CAL_LOGEN_SINGLE_ENDED_NEGATIVE_INPUT, /* Inverted Negative Input to Positive Output, Negative Input to Negative Output */
    ADI_ADRV910X_CAL_LOGEN_SINGLE_ENDED_POSITIVE_INPUT, /* Positive Input to Positive Output, Inverted Positive Input to Negative Output */
    ADI_ADRV910X_CAL_LOGEN_DIFFERENTIAL_INVERTED /* Inverted Positive Input to Negative Output, Inverted Negative Input to Positive Output */
}adi_adrv910x_LoGenDifferentialMode_e;

/**
 * @brief Structure which holds the LO interface Calibration parameters
 */
typedef struct adi_adrv910x_LoGenCalSettings
{
	uint8_t termRes; /* Set termination resistance of LO interface (Range: 0-15, 0: No resistance, 1: max resistance) */
    bool vcmBiasEnable; /* Enable Common mode bias voltage to set to VDD/2*/
    adi_adrv910x_LoGenDifferentialMode_e s2dMode; /* Set single or differential mode */
} adi_adrv910x_LoGenCalSettings_t;


#endif /* _ADI_ADRV910X_CAL_TYPES_H_ */
