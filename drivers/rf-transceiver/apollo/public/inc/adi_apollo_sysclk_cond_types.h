/*! ****************************************************************************
 *
 * @file:    adi_apollo_sysclk_cond_types.h
 *
 * @brief:   Apollo system clock calibration structure
 *
 * @details: This file contains all the System Clock calibrations
 *           prototypes referenced.
 *
 *******************************************************************************
  Copyright(c) 2019-2020 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 *******************************************************************************
 */

#ifndef __ADI_APOLLO_SYSCLK_COND_TYPES_H__
#define __ADI_APOLLO_SYSCLK_COND_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_SYSCLK_COND_CENTER_MAX_TO 15         /*!< Max timeout for clock conditioning centering to complete */

/*!
* \brief Enumerates sysclk cond errors
*/
typedef enum {
    ADI_APOLLO_SYSCLK_COND_ERR_OK                   = 0,    /*!< No error */
    ADI_APOLLO_SYSCLK_COND_ERR_TOTAL_VALID_OFST     = -1,   /*!< Total valid offset error */
    ADI_APOLLO_SYSCLK_COND_ERR_REC_VALID_OFST       = -2    /*!< Recommended offset error */
} adi_apollo_sysclk_cond_err_e;


/*!
* \brief System clock conditioning calibration config structure
*/
typedef struct adi_apollo_sysclk_cond_calconfig
{
    uint8_t     adc_centering_capture_length_Kb;        /*!< Number ADC samples stored per BMEM capture, in KiloSamples. 8=8192 ADC samples */
    uint8_t     adc_centering_capture_avg;              /*!< Number of averaged sweep runs used for offset estimation.*/
    uint8_t     adc_centering_capture_runs;             /*!< Number of BMEM captures done per each individual offset setting, during a particular sweep.*/
    uint8_t     adc_centering_done;                     /*!< Output flag. Read-Only. This flag is 0=in progress, and 1=completed */
    uint8_t     adc_centering_use_debubbler;            /*!< Enable for measurement debubbling. If set to 1, this will enable removal of false-positive measurements. */
    uint8_t     adc_centering_use_noise_not_spread;     /*!< Enable for measurements using noise variance instead of ADC code spread.
                                                             If set to 1, an ADC sample variance estimate over each BMEM capture is used as measurement.
                                                             If set to 0, ADC code spread (Max - Min) is used as measurement. */
    uint8_t     adc_use_caldata;                        /*!< Enable for direct coefficient usage. If set to 1, coefficients stored in calData's adc01CalCoeffs are used for clock trimming. Recommended value is 0 */
    int8_t      adc_centering_min_valid_offset;         /*!< Sets the lowest offset used during sweeps. The lowest value this variable can take is -26. Recommended value is -26 (0xE6 in uint8) */
    int8_t      adc_centering_max_valid_offset;         /*!< Sets the highest offset used during sweeps. The highest value this variable can take is 14. Recommended value is 14 */
    uint8_t     padding[3];                             /*!< Unused */
    uint32_t    adc_centering_low_threshold;            /*!< Sets the lower-bound on measurements that are considered as good. Recommended value is 3395 */
    uint32_t    adc_centering_high_threshold;           /*!< Sets the upper-bound on measurements that are considered as good. Recommneded value is 2142397 */
    uint16_t    adc_centering_precapture_delayus;       /*!< Delay added before each BMEM capture. Its units are microseconds. Recommended value is 10000 */
    uint16_t    adc_post_correction_delayus;            /*!< Delay added after each analog-domain correction. Recommended value is 0 */
} adi_apollo_sysclk_cond_calconfig_t;

typedef struct adi_apollo_sysclk_cond_caldata
{
    int8_t      adc01_recommended_offset;               /*!< These is the recommended offset to be used for optimal ADC performance. */
    uint8_t     total_valid_offsets;                    /*!< The total number of offset found to be within the criteria set by calconfig_t */
    int8_t      current_offset;                         /*!< The offset currently used. It is automatically set to adc01_recommended_offset whenever re-centering is run.
                                                             However, the user can set any desired current_offset, and run trimming without re-centering and the desired offset will be applied */
    int8_t      adc0_min_offset;                        /*!< Minimum offset found for ADC Channel 0 (Per side), that meets the criteria set in calconfig_t */
    int8_t      adc1_min_offset;                        /*!< Minimum offset found for ADC Channel 1 (Per side), that meets the criteria set in calconfig_t */
    int8_t      adc0_max_offset;                        /*!< Maximum offset found for ADC Channel 0 (Per side), that meets the criteria set in calconfig_t */
    int8_t      adc1_max_offset;                        /*!< Maximum offset found for ADC Channel 4 (Per side), that meets the criteria set in calconfig_t */
    int8_t      adc01_calcoeffs[6];                     /*!< Coefficients used for direct trimming of the ADC path */
    uint8_t     padding64[3];
    /* the compiler wants to place adc_std_combined_measurements at multiples of 8 bytes */
    uint64_t    adc_std_combined_measurements[41];      /*!< Resulting measurements after the re-centering routine completes. */
    uint32_t    crc;
} adi_apollo_sysclk_cond_caldata_t;

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_SYSCLK_COND_TYPES_H__ */
/*! @} */