/*!
 * \brief     Apollo ADC types headers
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_ADC
 * @{
 */
#ifndef __ADI_APOLLO_ADC_TYPES_H__
#define __ADI_APOLLO_ADC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"
#include "adi_apollo_mailbox_types.h"

/*!
 * \brief Enumerates ADC sync path delay types
 */
typedef enum {
    ADI_APOLLO_SYNC_PATH_DELAY_ALL_HIGH    = 0,
    ADI_APOLLO_SYNC_PATH_DELAY_SAME_AS_CLK = 1,
    ADI_APOLLO_SYNC_PATH_DELAY_MID_SCALE   = 2,
    ADI_APOLLO_SYNC_PATH_DELAY_ALL_LOW     = 3
} adi_apollo_sync_path_delay_e;

/*!
 * \brief Enumerates ADC Slice Modes
 */
typedef enum {
    ADI_APOLLO_ADC_MODE_RANDOM     = 0,
    ADI_APOLLO_ADC_MODE_SEQUENTIAL = 1,
    ADI_APOLLO_ADC_MODE_DISABLED   = 255
} adi_apollo_adc_mode_e;

/*!
 * \brief Enumerates methods available for ADC Slice mode switching
 */
typedef enum {
    ADI_APOLLO_ADC_MODE_SWITCH_BY_COMMAND  = 0,
    ADI_APOLLO_ADC_MODE_SWITCH_BY_REGMAP   = 1,
    ADI_APOLLO_ADC_MODE_SWITCH_BY_GPIO     = 2
} adi_apollo_adc_mode_switch_method_e;

/*!
 * \brief Enumerates current state of ADC Tracking / BG Calibration
 */
typedef enum {
    ADI_APOLLO_ADC_BGCAL_STATE_SUSPENDED    = APOLLO_CALFRMWRK_STATE_SUSPENDED,     /*!< code: 0x01 Cal's timer is not running */
    ADI_APOLLO_ADC_BGCAL_STATE_RESUMED      = APOLLO_CALFRMWRK_STATE_RESUMED,       /*!< code: 0x02 Cal's timer is running */
    ADI_APOLLO_ADC_BGCAL_STATE_INACTIVE     = APOLLO_CALFRMWRK_STATE_INACTIVE,      /*!< code: 0x04 Cal's Main function is not executing */
    ADI_APOLLO_ADC_BGCAL_STATE_RUNNING      = APOLLO_CALFRMWRK_STATE_RUNNING,       /*!< code: 0x08 Cal's Main function is executing */
    ADI_APOLLO_ADC_BGCAL_STATE_ENABLED      = APOLLO_CALFRMWRK_STATE_ENABLED,       /*!< code: 0x10 Cal is enabled back from the host */
    ADI_APOLLO_ADC_BGCAL_STATE_DISABLED     = APOLLO_CALFRMWRK_STATE_DISABLED,      /*!< code: 0x20 Cal is disabled from the host */
    ADI_APOLLO_ADC_BGCAL_STATE_ERROR        = APOLLO_CALFRMWRK_STATE_ERROR,         /*!< code: 0x40 Cal is errored out */
    ADI_APOLLO_ADC_BGCAL_STATE_UNKNOWN      = 0xFFFF
} adi_apollo_adc_bgcal_state_e;

/*!
 * \brief ADC Background Calibration Gating Groups
 */
typedef enum {
    ADI_APOLLO_ADC_CAL_GROUP_ALL_INP_DEP      = 0x01U,   /*!< All Input Dependent Calibrations */
    ADI_APOLLO_ADC_CAL_GROUP_IL_GAIN_0        = 0x02U,   /*!< Interleaving Gain Option 0 */
    ADI_APOLLO_ADC_CAL_GROUP_IL_GAIN_1        = 0x04U,   /*!< Interleaving Gain Option 1 */
    ADI_APOLLO_ADC_CAL_GROUP_IL_OFFSET        = 0x08U,   /*!< Interleaving Offset */
    ADI_APOLLO_ADC_CAL_GROUP_IL_TIMING        = 0x10U,   /*!< Interleaving Timing */
    ADI_APOLLO_ADC_CAL_GROUP_FE_NLE           = 0x20U,   /*!< Frontend Non-linearity */
} adi_apollo_adc_cal_group_gate_e;

/*!
 * \brief ADC Over-Range Protection Threshold
 */
typedef enum {
    ADI_APOLLO_ADC_OVR_THRESH_M0P5_DB      = 0x00U,   /*!< Engage Over-Range at -0.5 dBFS */
    ADI_APOLLO_ADC_OVR_THRESH_M1P0_DB      = 0x01U,   /*!< Engage Over-Range at -1.0 dBFS */
    ADI_APOLLO_ADC_OVR_THRESH_M1P5_DB      = 0x02U,   /*!< Engage Over-Range at -1.5 dBFS */
    ADI_APOLLO_ADC_OVR_THRESH_M2P0_DB      = 0x03U,   /*!< Engage Over-Range at -2.0 dBFS */
    ADI_APOLLO_ADC_OVR_THRESH_M2P5_DB      = 0x04U,   /*!< Engage Over-Range at -2.5 dBFS */
    ADI_APOLLO_ADC_OVR_THRESH_M3P0_DB      = 0x05U,   /*!< Engage Over-Range at -3.0 dBFS */
    ADI_APOLLO_ADC_OVR_THRESH_M3P5_DB      = 0x06U,   /*!< Engage Over-Range at -3.5 dBFS */
    ADI_APOLLO_ADC_OVR_THRESH_M4P0_DB      = 0x07U,   /*!< Engage Over-Range at -4.0 dBFS */
} adi_apollo_adc_ovr_threshold_e;

/*!
 * \brief ADC Tracking / BG Cal error and status struct
 */
typedef struct {
    uint32_t state_valid;           /* 1 = bgcal_error and bgcal_state fields have valid values, 0 = state unknown */
    uint32_t bgcal_error;           /* 0 = no error */
    uint32_t bgcal_state;           /* cal state bits \ref adi_apollo_adc_bgcal_state_e */
} adi_apollo_adc_bgcal_state_t;

/*!
 * \brief ADC Fast Detect Configuration struct
 */
typedef struct {
    uint8_t enable;                     /*!< Enable (1) or disable (0) fast detect */
    uint32_t upper_threshold;           /*!< Upper threshold value [threshold = ( 10^(dbfs/20) * 2^11 )] */
    uint32_t lower_threshold;           /*!< Lower threshold value [threshold = ( 10^(dbfs/20) * 2^11 )] */
    uint32_t dwell_cycles;              /*!< Dwell cycles number */
} adi_apollo_adc_fast_detect_pgm_t;

/*!
 * \brief ADC Init (FG) Calibration Status struct
 */
typedef struct {
    uint8_t err_status[ADI_APOLLO_ADC_NUM];       /*!< True if calibrations reporting an error [indices A0 -> A3 B0 -> B3] */
    uint32_t duration_ms;                         /*!< Duration in msec of the last initial calibration run */
    uint8_t since_power_up[ADI_APOLLO_ADC_NUM];   /*!< True if calibrations run since power up for each channel [indices A0 -> A3 B0 -> B3] */
    uint8_t last_run[ADI_APOLLO_ADC_NUM];         /*!< True if calibrations run in during the previous runInitCals() call for each channel [indices A0 -> A3 B0 -> B3] */
} adi_apollo_adc_fg_cal_status_t;

/*!
 * \brief ADC Tracking (BG) Calibration Status struct
 */
typedef struct {
    uint8_t enabled[ADI_APOLLO_ADC_NUM];         /*!< which ADC Rx tracking calibrations are enabled per channel. [indices A0 -> A3 B0 -> B3] \ref adi_apollo_adc_select_e */
    uint32_t error[ADI_APOLLO_ADC_NUM];          /*!< ADC Rx tracking calibration error per channel [indices A0 -> A3 B0 -> B3] */
    uint32_t state[ADI_APOLLO_ADC_NUM];          /*!< ADC Rx tracking calibration state per channel [indices A0 -> A3 B0 -> B3] \ref adi_apollo_adc_bgcal_state_e */
} adi_apollo_adc_bg_cal_status_t;

/*!
 * \brief ADC Calibration Status struct
 */
typedef struct {
    adi_apollo_adc_fg_cal_status_t fg_cal;   /*!< Foreground cal information \ref adi_apollo_adc_fg_cal_status_t */
    adi_apollo_adc_bg_cal_status_t bg_cal;   /*!< Background cal information \ref adi_apollo_adc_bg_cal_status_t */
    uint8_t mode[ADI_APOLLO_ADC_NUM];        /*!< ADC slice modes \ref adi_apollo_adc_mode_e [indices A0 -> A3 B0 -> B3] */
} adi_apollo_adc_status_t;

/*!
 * \brief   ADC input signal status monitored during calibration.
            When the status goes high, it will remain high until it's read back.
 *
 */
typedef struct {
    uint8_t power;              /*!< 1: ADC input signal does not have enough power  */
    uint8_t amp_diversity;      /*!< 1: Diversity of the amplitudes of the input samples fell below a predetermined threshold */
    uint8_t over_range;         /*!< 1: Over-range detected */
} adi_apollo_adc_input_status_t;


#endif /* __ADI_APOLLO_ADC_TYPES_H__ */
/*! @} */
