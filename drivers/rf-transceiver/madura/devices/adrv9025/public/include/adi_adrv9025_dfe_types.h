/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_dfe_types.h
* \brief Contains ADRV9025 API DFE data types
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_DFE_TYPES_H_
#define _ADI_ADRV9025_DFE_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include "adi_adrv9025_cals_types.h"
#include "adi_adrv9025_user.h"

#ifdef __cplusplus
extern
"C"
 {
#endif

#define ADI_ADRV9025_MAX_DPD_FILTER 15u
#define ADI_ADRV9025_NUM_BYTES_PER_FEATURE 12u
#define ADI_ADRV9025_NUM_BYTES_DPD_MODEL_METADATA 4u
#define ADI_ADRV9025_NUM_BYTES_DPD_MODEL_CONFIG ((ADI_ADRV9025_NUM_BYTES_DPD_MODEL_METADATA) + ((ADI_ADRV9025_MAX_DPD_FEATURES)*(ADI_ADRV9025_NUM_BYTES_PER_FEATURE)))

#define ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN 512u
#define ADI_ADRV9025_MAX_TDD_LUT_MODELA_DELAY 4u

/**
* \brief Enumerated list of options to compare values used for DPD fault conditions
*/
typedef enum adi_adrv9025_DpdComparator
{
    ADI_ADRV9025_DPD_COMPARATOR_LESS_THAN = 0,      /*!< Comparator for less than */
    ADI_ADRV9025_DPD_COMPARATOR_GREATER_THAN = 1    /*!< Comparator for greater than */
} adi_adrv9025_DpdComparator_e;

/**
* \brief Enumerated list of options to select Tx DPD LUT for programming
*/
typedef enum adi_adrv9025_DpdLut
{
    ADI_ADRV9025_DPD_LUT0 = 0,    /*!< Selects DPD LUT 0 */
    ADI_ADRV9025_DPD_LUT1 = 1,    /*!< Selects DPD LUT 1 */
    ADI_ADRV9025_DPD_LUT2 = 2,    /*!< Selects DPD LUT 2 */
    ADI_ADRV9025_DPD_LUT3 = 3,    /*!< Selects DPD LUT 3 */
    ADI_ADRV9025_DPD_LUT4 = 4,    /*!< Selects DPD LUT 4 */
    ADI_ADRV9025_DPD_LUT5 = 5,    /*!< Selects DPD LUT 5 */
    ADI_ADRV9025_DPD_LUT6 = 6,    /*!< Selects DPD LUT 6 */
    ADI_ADRV9025_DPD_LUT7 = 7,    /*!< Selects DPD LUT 7 */
    ADI_ADRV9025_DPD_LUT8 = 8,    /*!< Selects DPD LUT 8 */
    ADI_ADRV9025_DPD_LUT9 = 9,    /*!< Selects DPD LUT 9 */
    ADI_ADRV9025_DPD_LUT10 = 10,  /*!< Selects DPD LUT 10 */
    ADI_ADRV9025_DPD_LUT11 = 11,  /*!< Selects DPD LUT 11 */
    ADI_ADRV9025_DPD_LUT12 = 12,  /*!< Selects DPD LUT 12 */
    ADI_ADRV9025_DPD_LUT13 = 13,  /*!< Selects DPD LUT 13 */
    ADI_ADRV9025_DPD_LUT14 = 14,  /*!< Selects DPD LUT 14 */
    ADI_ADRV9025_DPD_LUT15 = 15,  /*!< Selects DPD LUT 15 */
    ADI_ADRV9025_DPD_LUT16 = 16,  /*!< Selects DPD LUT 16 */
    ADI_ADRV9025_DPD_LUT17 = 17,  /*!< Selects DPD LUT 17 */
    ADI_ADRV9025_DPD_LUT18 = 18,  /*!< Selects DPD LUT 18 */
    ADI_ADRV9025_DPD_LUT19 = 19,  /*!< Selects DPD LUT 19 */
    ADI_ADRV9025_DPD_LUT20 = 20,  /*!< Selects DPD LUT 20 */
    ADI_ADRV9025_DPD_LUT21 = 21,  /*!< Selects DPD LUT 21 */
    ADI_ADRV9025_DPD_LUT22 = 22,  /*!< Selects DPD LUT 22 */
    ADI_ADRV9025_DPD_LUT23 = 23,  /*!< Selects DPD LUT 23 */
    ADI_ADRV9025_DPD_LUT24 = 24,  /*!< Selects DPD LUT 24 */
    ADI_ADRV9025_DPD_LUT25 = 25,  /*!< Selects DPD LUT 25 */
    ADI_ADRV9025_DPD_LUT26 = 26,  /*!< Selects DPD LUT 26 */
    ADI_ADRV9025_DPD_LUT27 = 27,  /*!< Selects DPD LUT 27 */
    ADI_ADRV9025_DPD_LUT28 = 28,  /*!< Selects DPD LUT 28 */
    ADI_ADRV9025_DPD_LUT29 = 29,  /*!< Selects DPD LUT 29 */
    ADI_ADRV9025_DPD_LUT30 = 30,  /*!< Selects DPD LUT 30 */
    ADI_ADRV9025_DPD_NUM_LUTS
} adi_adrv9025_DpdLut_e;

/**
* \brief Enumerated list of options to select DPD models for programming the model config
*/
typedef enum adi_adrv9025_DpdModelSel
{
    ADI_ADRV9025_DPD_MODEL0 = 0,  /*!< Selects DPD Model 0 */
    ADI_ADRV9025_DPD_MODEL1 = 1,  /*!< Selects DPD Model 1. Currently Not Supported */
    ADI_ADRV9025_DPD_MODEL2 = 2,  /*!< Selects DPD Model 2. Currently Not Supported */
    ADI_ADRV9025_DPD_MODEL3 = 3   /*!< Selects DPD Model 3. Currently Not Supported */
} adi_adrv9025_DpdModelSel_e;

/**
* \brief Enumerated list of options to select the DPD LUT model compander size
*/
typedef enum adi_adrv9025_DpdActuatorCompanderSize
{
    ADI_ADRV9025_DPD_ACT_COMPANDER_8_BITS = 0, /*!< Selects an 8 bit compander. 4 banks / LUT memory for an 8 bit compander */
    ADI_ADRV9025_DPD_ACT_COMPANDER_9_BITS = 1  /*!< Selects a 9 bit compander. 2 banks / LUT memory for a 9 bit compander */
} adi_adrv9025_DpdActuatorCompanderSize_e;


/**
* \brief Enumerated list of options to reset Tx DPD module
*/
typedef enum adi_adrv9025_DpdResetMode
{
    ADI_ADRV9025_DPD_RESET_FULL          = 1,        /*!< Resets the DPD hardware and the firmware */
    ADI_ADRV9025_DPD_LUT_RESTORE         = 2,        /*!< Restores DPD LUT M Table */
    ADI_ADRV9025_DPD_COEFF_SAVE          = 3,        /*!< Save off DPD Coefficients M Table */
    ADI_ADRV9025_DPD_LUT_RESTORE_C_TABLE = 4,        /*!< Restores DPD LUT C Table */
    ADI_ADRV9025_DPD_COEFF_SAVE_C_TABLE  = 5,        /*!< Save off DPD Coefficients C Table */
    ADI_ADRV9025_DPD_LUT_RESTORE_R_TABLE = 6,        /*!< Restores DPD LUT R Table */
    ADI_ADRV9025_DPD_COEFF_SAVE_R_TABLE  = 7         /*!< Save off DPD Coefficients R Table */
} adi_adrv9025_DpdResetMode_e;

/**
* \brief Enumerated list of options to select DPD tracking update mode
*/
typedef enum adi_adrv9025_DpdTrackingUpdateMode
{
    ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_0 = 0,    /*!< The DPD tracking calibration updates coefficients corresponding to the GMP model on every scheduled DPD update. This mode offers the best sustained performance for any signal at the expense of transient emissions when the signal changes rapidly. */
    ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_1,        /*!< The DPD algorithm updates the coefficients corresponding to the GMP model only when the RMS power measured by the DPD on the captured samples during the current update exceeds the previously recorded maximum RMS power level. The recorded maximum RMS power level decays by 0.2dB per update.
                                                        DPD Mode 1 offers the best mitigation of transient emissions when the signal changes rapidly, at the expense of sustained performance */
    ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_2         /*!< This offers a compromise between modes 0 and 1. There is some mitigation of transient emissions when the signal changes rapidly and sustained performance in many signaling conditions. */ /*Depending on the RMS power measured by the DPD on the samples captured for coefficient computation, the DPD algorithm either switches to the high power look up table(M-Table) or the low power look up table(C-Table), and the same look up table is active until the next DPD update.*/ /*M-Table operates similar to DPD mode1 and C-Table operates similar to DPD Mode 0*/
} adi_adrv9025_DpdTrackingUpdateMode_e;

/**
* \brief Enumerated list of DPD Errors
*/
typedef enum adi_adrv9025_DpdError
{
    ADI_ADRV9025_DPD_NO_ERROR = 0,
    ADI_ADRV9025_DPD_ORX_ON_DIS_ERROR = 0x3401,                             /*!< Deprecated - Error code to convey that the Orx is disabled */
    ADI_ADRV9025_DPD_TX_ON_DIS_ERROR = 0x3402,                              /*!< Deprecated - Error code to convey that the Tx is disabled */
    ADI_ADRV9025_DPD_NO_PATHDELAY_ERROR = 0x3403,                           /*!< Error code to convey that the external path delay calibration was not run */
    ADI_ADRV9025_DPD_NO_INIT_ERROR = 0x3404,                                /*!< Deprecated - Error code to convey that the DPD initial calibration has not run */
    ADI_ADRV9025_DPD_ORX_SIGNAL_TOO_SMALL_ERROR = 0x3405,                   /*!< Error code to convey that the ORx signal is too small to perform DPD adaptation */
    ADI_ADRV9025_DPD_ORX_SIGNAL_SATURATING_ERROR = 0x3406,                  /*!< Error code to convey that the ORx signal is saturating - check for Rx gain */
    ADI_ADRV9025_DPD_TX_SIGNAL_TOO_SMALL_ERROR = 0x3407,                    /*!< Error code to convey that the Tx signal is too small to perform DPD adaptation */
    ADI_ADRV9025_DPD_TX_SIGNAL_SATURATING_ERROR = 0x3408,                   /*!< Error code to convey that the Orx signal is saturating */
    ADI_ADRV9025_DPD_MODELING_ERROR_HIGH_ERROR = 0x3409,                    /*!< Deprecated - Error code to convey that the DPD modeling error over model_err_thres (over saturating) */
    ADI_ADRV9025_DPD_AM_AM_OUTLIERS_ERROR = 0x340A,                         /*!< Error code to convey that the DPD adaptation has encountered too many AM-AM outliers */
    ADI_ADRV9025_DPD_COEFFICIENTS_UNAVAILABLE_ERROR = 0x340B,                        /*!< Deprecated - Error code to convey that the profile for DPD is invalid */
    ADI_ADRV9025_DPD_CAPTURE_LOOP_TIMEOUT_ERROR  = 0x340C,                  /*!< Error codeL DPD data capture loop time out */
    ADI_ADRV9025_DPD_UNITY_MODEL_UNAVAILABLE_ERROR = 0x340D,                /*!< Error code to convey that unity model isn't available for gain monitoring feature */
    ADI_ADRV9025_DPD_LDL_SOLVER_ERROR = 0x340E,                             /*!< Error code to convey that there is an LDL solver error */
    ADI_ADRV9025_DPD_MAX_PARTITIONS_REACHED_ERROR = 0x340F,                 /*!< Error code to convey that the max partitions are reached */
    ADI_ADRV9025_DPD_RPC_FAILED_ERROR = 0x3410,                             /*!< Error code to convey that RPC send failed */
    ADI_ADRV9025_DPD_UNKNOWN_RPC_ERROR = 0x3411,                            /*!< Error code to convey that unknown RPC message is received */
    ADI_ADRV9025_DPD_MESSAGE_WAIT_TIMEOUT_ERROR = 0x3412,                   /*!< Error code to convey that FW timed out waiting for RPC message */
    ADI_ADRV9025_DPD_MUTEX_CREATION_ERROR = 0x3413,                         /*!< Error code to convey that FW couldn't create mutex */
    ADI_ADRV9025_DPD_ACT_I_ASSIGNMENT_CONFLICT_ERROR = 0x3414,              /*!< Error code to convey that conflicting memory terms were assigned to an LUT */
    ADI_ADRV9025_DPD_ACT_K_ASSIGNMENT_EXCEED_LIMIT_ERROR = 0x3415,          /*!< Error code to convey that the DPD actuator power term in the GMP polynomial expression has exceeded the range */
    ADI_ADRV9025_DPD_ACT_MULTIPLIER_ROW_ASSIGNMENT_CONFLICT_ERROR = 0x3416, /*!< Error code to convey that conflicting cross terms were encountered in the feature set provided. */
    ADI_ADRV9025_DPD_ACT_LUT_OUT_OF_RANGE_ERROR = 0x3417,                   /*!< Error code to convey that the LUT assignment for a feature is invalid. */
    ADI_ADRV9025_DPD_ACT_NO_FREE_MULTIPLIER_ERROR = 0x3418,                 /*!< Error code to convey that the roaming LUTs(LUTs 26,27,28,29) assigned to a feature could not find a free multiplier row */
    ADI_ADRV9025_DPD_ADP_WRITE_LUT_ERROR = 0x3419,                          /*!< Error code to convey that the firmware could not update the DPD actuator look up tables */
    ADI_ADRV9025_DPD_HARDWARE_IN_USE_ERROR = 0x341A,                        /*!< Error code to convey that the DPD hardware is in use by another feature */
    ADI_ADRV9025_DPD_DATA_CAPTURE_ERROR = 0x341B,                           /*!< Error code to convey that there was a DPD data capture failure */
    ADI_ADRV9025_DPD_DATA_XACC_ERROR = 0x341C,                              /*!< Error code to convey that the cross correlation caused an error */
    ADI_ADRV9025_DPD_STABILITY_ERROR = 0x341D,                              /*!< Error code to convey that the DPD stability error occurs */
    ADI_ADRV9025_DPD_CHOL_SOLVER_ERROR = 0x341E,                            /*!< Error code to convey that the DPD cholesky diagonal term is too small */
    ADI_ADRV9025_DPD_TRACK_CLGC_SYNC_ERROR = 0x341F,                        /*!< Error code to convey that DPD-CLGC synchronization error */
    ADI_ADRV9025_DPD_ACT_LUT_ENTRY_SAT_ERROR = 0x3420,                      /*!< Error code to convey that DPD actuator entry is saturated */
    ADI_ADRV9025_DPD_DATA_CAPTURE_TIMEOUT_ERROR = 0x3421                    /*!< Error code to convey that DPD Data capture timed out */
} adi_adrv9025_DpdError_e;

/**
* \brief Enumerated list of DPD Model Table Selection
*/
typedef enum adi_adrv9025_DpdModelTableSel
{
    ADI_ADRV9025_DPD_MODEL_TABLE_M = 0, /*!< In DPD Mode 0, the model defined by M-Table is updated on every DPD iteration if there are no errors and update criteria is met.
                                         *   In DPD Mode 1, the model defined by M-Table is updated only when the rms power measured by DPD exceeds previously recorded maximum rms power
                                         *   In DPD Mode 2, the model defined by M-Table is updated only when the rms power measured by DPD exceeds the M-Threshold value specified by adi_adrv9025_DpdTrackingConfig_t.dpdMThreshold AND the previously recorded maximum rms power*/
    ADI_ADRV9025_DPD_MODEL_TABLE_C = 1, /*!< The model defined by C-Table is a low power model only applicable only in DPD MODE2 when the rms power of DPD capture samples is below the M-Threshold value specified by adi_adrv9025_DpdTrackingConfig_t.dpdMThreshold */
    ADI_ADRV9025_DPD_MODEL_TABLE_R = 2, /*!< R-Table or recovery table is a recovery model that stores the coefficients generated from the highest power data captured by DPD since the last reset. The maximum power recorded by the recovery model doesn’t decay unlike the maximum power recorded by M-Table(which decays by 0.2 dB per update period).. */
    ADI_ADRV9025_DPD_MODEL_TABLE_U = 3  /*!< Unity gain model in which the DPD actuator output is equal to the DPD actuator input. This model is usually activated in low power conditions where pre-distortion is not necessary OR while the baseband is running antenna cals.. */
} adi_adrv9025_DpdModelTableSel_e;

/**
* \brief Enumerated list of DPD Recovery actions
*/
typedef enum adi_adrv9025_DpdRecoveryAction
{
    ADI_ADRV9025_DPD_RECOVERY_ACTION_SKIP_LUTS_UPDATE       = 0x0001, /*!< Abandon DPD adaptation on the current iteration. */
    ADI_ADRV9025_DPD_RECOVERY_ACTION_REVERT_LUTS_TO_UNITY   = 0x0002, /*!< Revert all DPD Models to unity gain coefficients */
    ADI_ADRV9025_DPD_RECOVERY_ACTION_RESET_ADAPTATION_STATE = 0x0004, /*!< Reset DPD firmware internal state variables that includes max power recorded by DPD applicable to M-Table in DPD Mode 1 and DPD Mode 2 that is relevant to the user. */
    ADI_ADRV9025_DPD_RECOVERY_ACTION_SWITCH_LUTS_TO_M       = 0x0008, /*!< Switch to the max Power model. Please refer to documentation on adi_adrv9025_DpdModelTableSel_e for more details on M-Table */
    ADI_ADRV9025_DPD_RECOVERY_ACTION_SWITCH_LUTS_TO_R       = 0x0010, /*!< Switch to Recovery model defined as the model estimated on the highest power data seen by the DPD tracking calibration since reset. */
    ADI_ADRV9025_DPD_RECOVERY_ACTION_6DB_DIG_ATTEN          = 0x0020, /*!< Deprecated  NOTE: Currently this action is NOT Supported. */
    ADI_ADRV9025_DPD_RECOVERY_ACTION_RESET_FIRST_DPD_FLAG   = 0x0040  /*!< Resetting the first DPD flag runs 3 back to back DPD updates in indirect learning mode similar to the DPD behavior on very first update to speed up convergence. */
} adi_adrv9025_DpdRecoveryAction_e;

/**
* \brief Enumerated list of DPD Metrics evaluated by FW
*/
typedef enum adi_adrv9025_DpdMetric
{
     ADI_ADRV9025_DPD_METRIC_MEAN_TU_POWER   = 0x0001, /*!< Pre-DPD Actuator Transmit capture RMS power */
     ADI_ADRV9025_DPD_METRIC_PEAK_TU_POWER   = 0x0002, /*!< Pre-DPD Actuator Transmit capture peak power */
     ADI_ADRV9025_DPD_METRIC_MEAN_TX_POWER   = 0x0004, /*!< Post-DPD Actuator Transmit capture RMS power */
     ADI_ADRV9025_DPD_METRIC_PEAK_TX_POWER   = 0x0008, /*!< Post-DPD Actuator Transmit capture peak power */
     ADI_ADRV9025_DPD_METRIC_MEAN_ORX_POWER  = 0x0010, /*!< Observed capture RMS power */
     ADI_ADRV9025_DPD_METRIC_PEAK_ORX_POWER  = 0x0020, /*!< Observed capture peak power */
     ADI_ADRV9025_DPD_METRIC_DIRECT_EVM      = 0x0040, /*!< Pre-DPD capture Tx to ORx EVM (Direct EVM) which is a measure of DPD linearization performance */
     ADI_ADRV9025_DPD_METRIC_INDIRECT_EVM    = 0x0080, /*!< Post-DPD Capture Tx to ORx EVM (Indirect EVM) which is a measure of non-linearity in the gain line up*/
     ADI_ADRV9025_DPD_METRIC_SELECT_ERROR    = 0x0100, /*!< Select error that is same as indirect error, but computed on samples with rms power greater than -30dBFS */
     ADI_ADRV9025_DPD_METRIC_INDIRECT_ERROR  = 0x0200  /*!< Indirect error that indicates if the pre-distorted samples match the expected result after experiencing PA distortion */
} adi_adrv9025_DpdMetric_e;

/**
* \brief Enumerated list of Dpd error states
*/
typedef enum adi_adrv9025_DpdErrorState
{
    ADI_ADRV9025_DPD_ERR_STATE_0 = 0,           /*!< Error state where threshold 0 has been surpassed */
    ADI_ADRV9025_DPD_ERR_STATE_1,               /*!< Error state where threshold 1 has been surpassed */
    ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_0,    /*!< Error state where threshold 0 has been surpassed a certain number of times specified by adi_adrv9025_DpdFaultCondition_t.persistentCount over successive dpd iterations*/
    ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_1     /*!< Error state where threshold 1 has been surpassed a certain number of times specified by adi_adrv9025_DpdFaultCondition_t.persistentCount over successive dpd iterations*/
} adi_adrv9025_DpdErrorState_e;

/**
* \brief Enumerated list of options to select CFR mode of operation
*/
typedef enum adi_adrv9025_CfrModeSel
{
    ADI_ADRV9025_CFR_MODE1 = 0, /*!< User to provide the final correction pulse in this mode which will be used by the CFR engine */
    ADI_ADRV9025_CFR_MODE2 = 1, /*!< NOTE : CURRENTLY NOT SUPPORTED User to provide the base correction pulse in this mode which will be post processed by the ADRV9025 firmware to generate the final correction pulse */
    ADI_ADRV9025_CFR_MODE3 = 2  /*!< NOTE : CURRENTLY NOT SUPPORTED The firmware uses a set of internal base pulses corresponding to a set of pre-defined carrier signals to generate the final correction pulse */
} adi_adrv9025_CfrModeSel_e;

/**
* \brief Enumerated list of options to select CFR interpolation applied to input before peak detection
*/
typedef enum adi_adrv9025_CfrInterpolationSel
{
    ADI_ADRV9025_CFR_INTERPOLATION_1X = 0, /*!< Selects the interpolation factor to 1 */
    ADI_ADRV9025_CFR_INTERPOLATION_2X = 1, /*!< Selects the interpolation factor to 2 */
    ADI_ADRV9025_CFR_INTERPOLATION_4X = 2  /*!< Selects the interpolation factor to 4 */
} adi_adrv9025_CfrInterpolationSel_e;

/**
* \brief Enumerated list of CFR Errors
*/
typedef enum adi_adrv9025_CfrError
{
    ADI_ADRV9025_CFR_NO_ERROR = 0,
    ADI_ADRV9025_CFR_CONFIGURATION_ERROR = 0x1D01,                     /*!< Error code to convey that the mandatory CFR configs were not done. Not active/used */
    ADI_ADRV9025_CFR_PROG_PULSE_MODE_ERROR = 0x1D02,                   /*!< Error code to convey that an unsupported pulse mode was selected. */
    ADI_ADRV9025_CFR_INPUT_RATE_HIGH_ERROR = 0x1D03,                   /*!< Error code to convey that the Tx channel sample rate is higher than 245.76 Mhz */
    ADI_ADRV9025_CFR_CTRL_CMD_NOT_SUPPORTED_ERROR = 0x1D04             /*!< Error code to convey that control command is not supported */
} adi_adrv9025_CfrError_e;

/**
* \brief Enumerated list of target CFR correction pulses for carrier config hot swapping
*/
typedef enum adi_adrv9025_CfrCarrierHotSwapCorrPulseSel
{
    ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_1 = 0,   /*!< Sets the active CFR correction pulse to pulse 1 when 2 correction pulses of half pulse lengths 256 or less are programmed */
    ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_2 = 1    /*!< Sets the active CFR correction pulse to pulse 2 when 2 correction pulses of half pulse lengths 256 or less are programmed */
} adi_adrv9025_CfrCarrierHotSwapCorrPulseSel_e;

/**
* \brief Enumerated list of CLGC runtime states
*/
typedef enum adi_adrv9025_ClgcRunState
{
    ADI_ADRV9025_CLGC_INITIAL_RUN = 0,            /*!< CLGC algorithm is initializing. If Tx-ORx loop gain control is not enabled, CLGC remains in this state perpetually */
    ADI_ADRV9025_CLGC_NORMAL_RUN = 1,             /*!< CLGC algorithm is running normally */
    ADI_ADRV9025_CLGC_LO_LIMIT_TX_ATTEN = 2,      /*!< Tx Attenuation has reached it's lower limit */
    ADI_ADRV9025_CLGC_HI_LIMIT_TX_ATTEN = 3,      /*!< Tx Attenuation has reached it's upper limit */
    ADI_ADRV9025_CLGC_USER_CHANGE_LOOP_GAIN = 4,  /*!< An expected loop gain change from the user has been detected and CLGC will re-initialize */
    ADI_ADRV9025_CLGC_USER_CHANGE_TX_ATTEN = 5,   /*!< A Tx attenuation change from the user has been detected and CLGC will re-initialize */
    ADI_ADRV9025_CLGC_MAX_LIMIT_NUM_BATCHES = 6,  /*!< The limit for the maximum no. of batches per CLGC run has been breached */
    ADI_ADRV9025_CLGC_ASSERT_PA_PROTECTION = 7    /*!< NOTE CLGC will not take any actions on assertion of PA protection. CLGC algorithm has detected a PA protection assertion */
} adi_adrv9025_ClgcRunState_e;

/**
* \brief Enumerated list of CLGC Tx-ORx data capture status for adapting closed loop gain control
*/
typedef enum adi_adrv9025_ClgcCaptureStatus
{
    ADI_ADRV9025_CLGC_CAP_DONE_OK = 0,            /*!< Previous Tx-ORx capture for CLGC control/measurement completed successfully */
    ADI_ADRV9025_CLGC_CAP_INIT = 1,               /*!< CLGC capture is initializing */
    ADI_ADRV9025_CLGC_CAP_REG = 2,                /*!< CLGC capture is being registered */
    ADI_ADRV9025_CLGC_CAP_START_FUNC_OK = 3,      /*!< CLGC capture has started */
    ADI_ADRV9025_CLGC_CAP_START_FUNC_ORX_ERR = 4, /*!< CLGC ORx capture did not complete successfully */
    ADI_ADRV9025_CLGC_CAP_STOP_FUNC_OK = 5,       /*!< CLGC capture stop completed successfully */
    ADI_ADRV9025_CLGC_CAP_SAVE_FUNC_OK = 6,       /*!< CLGC capture data saved successfully */
    ADI_ADVR9025_CLGC_CAP_SAVE_FUNC_DONE = 7,     /*!< CLGC capture data saved but there was an error */
    ADI_ADRV9025_CLGC_CAP_SAVE_FUNC_ERR = 8,      /*!< CLGC capture data did not complete successfully */
    ADI_ADRV9025_CLGC_CAP_DONE_ERR = 9            /*!< CLGC capture aborted due to error */
} adi_adrv9025_ClgcCaptureStatus_e;

/**
* \brief Enumerated list allowable dpd sample sizes per capture
*/
typedef enum adi_adrv9025_dpdSamplesPerCapture
{
    ADI_ADRV9025_SAMPLES_PER_CAP_1024 = 1024,   /*!< Captures 1024 samples per batch for eDPD adaptation and total number of smaples specified by adi_adrv9025_EnhancedDpdTrackingConfig_t.dpdSamples*/
    ADI_ADRV9025_SAMPLES_PER_CAP_2048 = 2048,   /*!< Captures 2048 samples per batch for eDPD adaptation and total number of smaples specified by adi_adrv9025_EnhancedDpdTrackingConfig_t.dpdSamples*/
    ADI_ADRV9025_SAMPLES_PER_CAP_4096 = 4096    /*!< Captures 4096 samples per batch for eDPD adaptation and total number of smaples specified by adi_adrv9025_EnhancedDpdTrackingConfig_t.dpdSamples*/
} adi_adrv9025_dpdSamplesPerCapture_e;

/**
* \brief Data structure to hold Tx DPD Feature Row
*        This structure holds the configuration details for every feature in the DPD model,
*        where {i, j, k} represent the various delay and power terms associated with an "alpha" coefficient
*        in the generalized memory polynomial(GMP).
*        The structure also holds the LUT associated with a given feature, as well as the real and
*        imaginary parts of any preloaded coefficients.
*/
typedef struct adi_adrv9025_DpdFeature
{
    uint8_t i;                    /*!< Memory term of the DPD feature */
    uint8_t j;                    /*!< Cross term of the DPD feature */
    uint8_t k;                    /*!< Power term of the DPD feature */
    adi_adrv9025_DpdLut_e lut;    /*!< Target LUT index for this feature. Valid values are LUT0 - LUT30 */
    float coeffReal;              /*!< Real part of the complex co-efficient for this feature */
    float coeffImaginary;         /*!< Imaginary part of the complex co-efficient for this feature */
} adi_adrv9025_DpdFeature_t;

/**
* \brief Data structure to hold Tx DPD Model initialization parameters
*/
typedef struct adi_adrv9025_DpdModelConfig
{
    uint32_t txChannelMask;                                               /*!< Reserved Param. Currently Tx channel mask is not supported. Settings are applied to the channel for which DPD init cal was run */
    uint8_t dpdNumFeatures;                                               /*!< No. of features contained in the DPD model. Currently 190 features are supported */
    adi_adrv9025_DpdFeature_t dpdFeatures[ADI_ADRV9025_MAX_DPD_FEATURES]; /*!< Array consisting of the set of basis features to be initialized for DPD adaptation */
} adi_adrv9025_DpdModelConfig_t;

/**
* \brief Data structure to hold Dpd metrics mask and recovery actions mask associated with it
*        The metric mask and action mask in the table indicate error conditions and each error condition has a unique metrics-mask to which multiple fault condition metrics can be mapped by ‘OR’ing the metrics.
*/
typedef struct adi_adrv9025_DpdMetricActionPair
{
    uint16_t dpdMetricsMask;    /*!< Bitmask of metrics listed in adi_adrv9025_DpdMetric_e which are in fault conditions
                                 *   ADI_ADRV9025_DPD_METRIC_MEAN_TU_POWER   = 0x0001
                                 *   ADI_ADRV9025_DPD_METRIC_PEAK_TU_POWER   = 0x0002
                                 *   ADI_ADRV9025_DPD_METRIC_MEAN_TX_POWER   = 0x0004
                                 *   ADI_ADRV9025_DPD_METRIC_PEAK_TX_POWER   = 0x0008
                                 *   ADI_ADRV9025_DPD_METRIC_MEAN_ORX_POWER  = 0x0010
                                 *   ADI_ADRV9025_DPD_METRIC_PEAK_ORX_POWER  = 0x0020
                                 *   ADI_ADRV9025_DPD_METRIC_DIRECT_EVM      = 0x0040
                                 *   ADI_ADRV9025_DPD_METRIC_INDIRECT_EVM    = 0x0080
                                 *   ADI_ADRV9025_DPD_METRIC_SELECT_ERROR    = 0x0100
                                 *   ADI_ADRV9025_DPD_METRIC_INDIRECT_ERROR  = 0x0200  */
    uint16_t dpdActionMask;     /*!< Bitmask of recovery actions listed in adi_adrv9025_DpdRecoveryAction_e which have been taken to address the fault conditions
                                 *   ADI_ADRV9025_DPD_RECOVERY_ACTION_SKIP_LUTS_UPDATE       = 0x0001
                                 *   ADI_ADRV9025_DPD_RECOVERY_ACTION_REVERT_LUTS_TO_UNITY   = 0x0002
                                 *   ADI_ADRV9025_DPD_RECOVERY_ACTION_RESET_ADAPTATION_STATE = 0x0004
                                 *   ADI_ADRV9025_DPD_RECOVERY_ACTION_SWITCH_LUTS_TO_M       = 0x0008
                                 *   ADI_ADRV9025_DPD_RECOVERY_ACTION_SWITCH_LUTS_TO_R       = 0x0010
                                 *   ADI_ADRV9025_DPD_RECOVERY_ACTION_6DB_DIG_ATTEN          = 0x0200
                                 *   ADI_ADRV9025_DPD_RECOVERY_ACTION_RESET_FIRST_DPD_FLAG   = 0x0400 */
} adi_adrv9025_DpdMetricActionPair_t;

/**
* \brief Data structure to hold DPD Statistics
*/
typedef struct adi_adrv9025_DpdStatistics
{
    float dpdMeanTuPower;   /*!< Measured absolute pre-DPD Tx power averaged in one update duration. Can be converted to dBFS as 20log10(sqrt(dpdMeanTuPower)/FULL_SCALE) */
    float dpdPeakTuPower;   /*!< Measured absolute pre-DPD Tx power peaked in one update duration. Can be converted to dBFS as 20log10(sqrt(dpdPeakTuPower)/FULL_SCALE) */
    float dpdMeanTxPower;   /*!< Measured absolute post-DPD Tx power averaged in one update duration. Can be converted to dBFS as 20log10(sqrt(dpdMeanTxPower)/FULL_SCALE) */
    float dpdPeakTxPower;   /*!< Measured absolute post-DPD Tx power peaked in one update duration. Can be converted to dBFS as 20log10(sqrt(dpdPeakTxPower)/FULL_SCALE) */
    float dpdMeanOrxPower;  /*!< Measured absolute ORx power averaged in one update duration. Can be converted to dBFS as 20log10(sqrt(dpdMeanOrxPower)/FULL_SCALE) */
    float dpdPeakOrxPower;  /*!< Measured absolute ORx power peaked in one update duration. Can be converted to dBFS as 20log10(sqrt(dpdPeakOrxPower)/FULL_SCALE) */
    float dpdDirectEvm;     /*!< EVM error between pre-DPD actuator data and the ORx data in one update duration. Multiply by 100 to get value in % */
    float dpdIndirectEvm;   /*!< EVM error between post-DPD actuator data and the ORx data in one update duration. Multiply by 100 to get value in % */
    float dpdSelectError;   /*!< Select error is same as indirect error, but computed on samples with rms power greater than -30dBFS. Multiply by 100 to get value in % */
    float dpdIndirectError; /*!< Indirect error indicates if the pre-distorted samples match the expected result after experiencing PA distortion. Multiply by 100 to get value in % */
} adi_adrv9025_DpdStatistics_t;

/**
* \brief Data structure to hold Tx DPD Status
*/
typedef struct adi_adrv9025_DpdStatus
{
    adi_adrv9025_DpdError_e dpdErrorCode; /*!< Error code from Tx DPD tracking calibration. If the DPD is functioning correctly, this parameter should return ADI_ADRV9025_DPD_NO_ERROR.*/
    uint32_t dpdPercentComplete;          /*!< Percentage completion of DPD adaptation. Range 0 to 100 */
    uint32_t dpdIterCount;                /*!< Running counter that increments each time the DPD adaptation is scheduled to run */
    uint32_t dpdUpdateCount;              /*!< Running counter that increments each time the cal updates the correction/actuator hardware successfully*/
    adi_adrv9025_TrackingCalSyncStatus_e dpdSyncStatus; /*!< Enum to denote the status of CLGC and DPD synchronization. This member returns SYNC_OK if both DPD and CLGC are synchronized. If CLGC is not enabled, then this field can be ignored. */
    adi_adrv9025_DpdModelTableSel_e dpdModelTable;      /*!< DPD Model table currently active  DPD model(M-Table/C-Table/U-Table) */
    adi_adrv9025_DpdStatistics_t dpdStatistics;         /*!< DPD Statistics */
    adi_adrv9025_DpdMetricActionPair_t dpdErrorStatus0; /*!< Error status containing error state and recovery actions taken based on threshold 0, specified in DPD stability config */
    adi_adrv9025_DpdMetricActionPair_t dpdErrorStatus1; /*!< Error status containing error state and recovery actions taken based on threshold 1, specified in DPD stability config */
    adi_adrv9025_DpdMetricActionPair_t dpdPersistentErrorStatus0; /*!< Persistent error status containing error state and recovery actions taken based on threshold 0 and persistent count specified in DPD stability config */
    adi_adrv9025_DpdMetricActionPair_t dpdPersistentErrorStatus1; /*!< Persistent error status containing error state and recovery actions taken based on threshold 1 and persistent count specified in DPD stability config */
    uint32_t dpdPerformanceMetric; /*!< Reserved for future use */
    uint32_t reservedPM; /*!< Reserved for future use */
    uint32_t reservedTP; /*!< Reserved for future use */
    uint32_t reservedPR; /*!< Reserved for future use */
} adi_adrv9025_DpdStatus_t;

/**
* \brief Data structure to hold Tx DPD Filter Coefficient Weight
*/
typedef struct adi_adrv9025_DpdFilterCoeffWeight
{
    int8_t real; /*!< weight of real part of the filter coefficient */
    int8_t imag; /*!< weight of imaginary part of the filter coefficient */
} adi_adrv9025_DpdFilterCoeffWeight_t;

/**
* \brief Data structure to hold Tx DPD Tracking Configuration
*/
typedef struct adi_adrv9025_DpdTrackingConfig
{
    uint32_t txChannelMask;                     /*!< Tx channels to which DPD tracking config is applied. There is only 1 DPD tracking config shared across channels */
    uint8_t  dpdIndirectRegularizationValue;    /*!< Regularization value when DPD indirect learning mode is enabled. Increasing DPD regularization makes the DPD coefficient estimation less sensitive to missing data and prevent over-fitting at the cost of ACLR performance Maximum value of 63. Default value is 20 */
    uint8_t  dpdDirectRegularizationValue;      /*!< Regularization value when DPD indirect learning mode is enabled. Increasing DPD regularization makes the DPD coefficient estimation less sensitive to missing data and prevent over-fitting at the cost of ACLR performance Maximum value of 63. Default value is 35 */
    uint16_t dpdSamples;                        /*!< Number of samples to capture per DPD adaptation */
    uint32_t dpdMThreshold;                     /*!< M table threshold when ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_2 (hybrid) is used. Maximum value of 2147483648, This parameter takes a linear scale value squared. To convert the MThreshold value from dBFS to Linear scale use ((10^MThreshold_dBFS/20)*FULL_SCALE_CODE)^2 */
    uint32_t dpdPeakSearchWindowSize;           /*!< Number of samples at Tx IQ rate to search for a peak to perform DPD sample capture. Maximum value of 16777215 */
    adi_adrv9025_DpdTrackingUpdateMode_e dpdUpdateMode; /*!< Update mode of DPD tracking. 0: simple, 1: simple + max, 2: hybrid refer to adi_adrv9025_DpdTrackingUpdateMode_e for more information*/
    adi_adrv9025_DpdFilterCoeffWeight_t dpdFilterCoeffWeight[ADI_ADRV9025_MAX_DPD_FILTER]; /*!< Reserved for future use Please use the default value returned by API adi_adrv9025_DpdTrackingConfigGet()*/
    uint16_t minAvgSignalLevel;                 /*!< DPD Tx low power threshold in linear scale below which DPD stops tracking. The default value is -36dBFS. It can be converted to dBFS as 20log10(minAvgSignalLevel/FULL_SCALE) */
    uint8_t dpdMu;                              /*!< DPD direct learning step size values range from 0 to 100 a value of 0 will stop the tracking of DPD in direct learning mode.Default value is 50 */
    uint16_t minAvgSignalLevelOrx;              /*!< DPD ORx low power threshold ORX in linear scale below which DPD stops tracking. The default value is -36dBFS. It can be converted to dBFS as 20log10(minAvgSignalLevel/FULL_SCALE) */
    uint16_t dpdFilterSel;                      /*!< OBW filter select either  0.5*fs or 0.8*fs (valid values 0-1 respectively). This filter is only applicable in 983 MHz actuator rate profiles + direct learning cases. */
    uint8_t  enableDirectLearning;              /*!< Option to enable direct learning (defaulte 0 which is off , set to 1 to enable direct learning If the no. of DPD coefficients in a DPD model specified by adi_adrv9025_DpdModelConfig_t.dpdNumFeatures exceeds 95 coefficients, DPD direct learning is automatically enforced regardless of the value configured for this parameter.*/
    uint16_t  dpdIndirectRegularizationLowPowerValue;       /*!< Indirect Learning Mode low power regularization value applied to C table, only valid when in DPD mode 2, Maximum value of 63 Default value is 20*/
} adi_adrv9025_DpdTrackingConfig_t;

typedef struct adi_adrv9025_EnhancedDpdTrackingConfig
{
    uint32_t txChannelMask;                     /*!< Tx channels to which DPD tracking config is applied. There is only 1 DPD tracking config shared across channels */
    uint8_t  dpdIndirectRegularizationValue;    /*!< Regularization value when DPD indirect learning mode is enabled. Increasing DPD regularization makes the DPD coefficient estimation less sensitive to missing data and prevent over-fitting at the cost of ACLR performance Maximum value of 63. Default value is 20 */
    uint8_t  dpdDirectRegularizationValue;      /*!< Regularization value when DPD indirect learning mode is enabled. Increasing DPD regularization makes the DPD coefficient estimation less sensitive to missing data and prevent over-fitting at the cost of ACLR performance Maximum value of 63. Default value is 35 */
    uint16_t dpdSamples;                        /*!< Number of samples to capture per adaptation */
    uint32_t dpdMThreshold;                     /*!< M table threshold when ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_2 (hybrid) is used. Maximum value of 2147483648, This parameter takes a linear scale value squared. To convert the MThreshold value from dBFS to Linear scale use ((10^MThreshold_dBFS/20)*FULL_SCALE_CODE)^2 */
    uint32_t dpdPeakSearchWindowSize;           /*!< Number of samples at Tx IQ rate to search for a peak to perform DPD sample capture. Maximum value of 16777215 */
    adi_adrv9025_DpdTrackingUpdateMode_e dpdUpdateMode; /*!< Update mode of DPD tracking. 0: simple, 1: simple + max, 2: hybrid */
    adi_adrv9025_DpdFilterCoeffWeight_t dpdFilterCoeffWeight[ADI_ADRV9025_MAX_DPD_FILTER]; /*!< Reserved for future use Please use the default value returned by the API adi_adrv9025_DpdTrackingConfigGet().*/
    uint16_t minAvgSignalLevel;                 /*!< DPD Tx low power threshold in linear scale below which DPD stops tracking. The default value is -36dBFS. It can be converted to dBFS as 20log10(minAvgSignalLevel/FULL_SCALE) */
    uint8_t  dpdMu;                             /*!< DPD direct learning step size values range from 0 to 100 */
    uint16_t minAvgSignalLevelOrx;              /*!< DPD ORx low power threshold ORX in linear scale below which DPD stops tracking. The default value is -36dBFS. It can be converted to dBFS as 20log10(minAvgSignalLevel/FULL_SCALE) */
    uint16_t dpdFilterSel;                      /*!< OBW filter select either  0.5*fs or 0.8*fs (valid values 0-1 respectively)*/
    uint8_t  enableDirectLearning;              /*!< Option to enable direct learning (defaulte 0 which is off , set to 1 to enable direct learning If the no. of DPD coefficients in a DPD model specified by adi_adrv9025_DpdModelConfig_t.dpdNumFeatures exceeds 95 coefficients, DPD direct learning is automatically enforced regardless of the value configured for this parameter.*/
    uint16_t dpdIndirectRegularizationLowPowerValue;   /*!< Indirect Learning Mode low power regularization value applied to C table, only valid when in DPD mode 2, Maximum value of 63 Default value is 20*/
    uint16_t enhancedDpdPeakSearchSize;                /*!< 0-65535 eDPD EVM search window size */
    uint16_t enhancedDPDCaptures;                      /*!< 0-16 eDPD EVM captures (0: DPD, >= 1: eDPD) */
    uint16_t enhancedDPDMinRandCapDelay;               /*!< 0-65535 eDPD minimum randomizer capture delay */
    adi_adrv9025_dpdSamplesPerCapture_e samplesPerCap; /*!< eDPD number of samples per capture */
    uint16_t enhancedDPDMaxRandCapDelay;               /*!< 0-65535 eDPD maximum randomizer capture delay */
    uint16_t tddLutSwitchModelADelay[ADI_ADRV9025_MAX_TDD_LUT_MODELA_DELAY]; /*!< TDD LUT switch Model A delay array, value unit: us */
    uint16_t tddLutSwitchModelBDelay;          /*!< TDD LUT switch Model B delay value, unit: us */
    uint8_t  enableTddLutSwitch;               /*!< TDD LUT switch feature enable, (1: enable, 0: disable) */
    uint8_t  enableWidebandRegularization;     /*!< Wideband regularization feature enable  (1: enable, 0: disable) for maintaining channel gain flatness under dynamic signaling conditions*/
    float    widebandRegularizationFactor;     /*!< Wideband regularization weighting factor beta that defines how much wideband reg training signals to mix with the DPD capture samples, The wideband regularization training signal is programmed through the API adi_adrv9025_EnhancedDpdWidebandRegularizationConfigSet() , range from 0 to 1 default value is 0.0001, step: 0.00001 */
    float    widebandRegularizationFactorAlpha;  /*!< Wideband regularization weighting factor alpha that defines how much wideband reg training signals to mix with the DPD capture samples, The wideband regularization training signal is programmed through the API adi_adrv9025_EnhancedDpdWidebandRegularizationConfigSet() , range from 0 to 1 default value is 0.0001, step: 0.00001 */
    uint8_t  widebandRegularizationDnSampleRate; /*!< Wideband regularization down sampling ratio for loop gain estimating */
} adi_adrv9025_EnhancedDpdTrackingConfig_t;
/**
* \brief Data structure to hold DPD fault condition settings
*/
typedef struct adi_adrv9025_DpdFaultCondition
{
    adi_adrv9025_DpdMetric_e dpdMetric;     /*!< DPD metric evaluated for the fault condition */
    adi_adrv9025_DpdComparator_e comparator;/*!< less than or greater than. Operation to be done between metric's value and threshold's value to cause fault condition. 0 = less than, 1 = greater than */
    int16_t threshold0;                     /*!< One of two thresholds for fault condition to occur. Threshold 0 is intended to be a warning when metric's value crosses this threshold. For power, threshold will be in dBFS, for EVM, select and indirect error, threshold will be in % */
    int16_t threshold1;                     /*!< One of two thresholds for fault condition to occur. Threshold 1 is intended to be an error when metric's value crosses this threshold. For power, threshold will be in dBFS, for EVM, select and indirect error, threshold will be in % */
    uint16_t persistentCount;               /*!< When threshold0/1 has been crossed this many times, dpdPersistentErrorStatus0/1 that can be found in adi_adrv9025_DpdStatus_t for that particular metric and threshold will be raised */
} adi_adrv9025_DpdFaultCondition_t;

/**
* \brief Data structure to hold DPD recovery action settings
*/
typedef struct adi_adrv9025_DpdRecoveryActionConfig
{
    adi_adrv9025_DpdErrorState_e dpdErrorState;             /*!< DPD Error State for which the recovery action is configured for */
    adi_adrv9025_DpdMetricActionPair_t dpdRecoveryAction;   /*!< Pair of bitmask for which fault conditions and the recovery actions it will trigger */
} adi_adrv9025_DpdRecoveryActionConfig_t;

typedef struct adi_adrv9025_DpdActGainMonitorThresh
{
    uint16_t dpdGainMonitorQualThresh;        /*!< DPD Power Monitor Qualifying Threshold Limit.
                                              This threshold corresponds to the MSB 16 bits of (I^2 + Q^2) 32 bit data */
    uint8_t dpdGainMonitorLowerThresh;        /*!< DPD Gain Monitor Minimum Gain Threshold Limit.
                                              A threshold violation is triggered if DPD_OUT < (DPD_IN x dpdGainMonitorLowerThresh)
                                              which can be used to auto load a DPD model specified by adi_adrv9025_DpdActGainMonitorCtrl.dpdGainMonitorLowerThreshModelSel.
                                              The gain value is specified in frac 2.6 fixed point format. */
    uint8_t dpdGainMonitorUpperThresh;        /*!< DPD Gain Monitor Maximum Gain Threshold Limit.
                                              A threshold violation is triggered if DPD_OUT > (DPD_IN x dpdGainMonitorUpperThresh)
                                              which can be used to auto load a DPD model specified by adi_adrv9025_DpdActGainMonitorCtrl.dpdGainMonitorUpperThreshModelSel.
                                              The gain value is in frac 2.6 fixed point format. */
} adi_adrv9025_DpdActGainMonitorThresh_t;

typedef struct adi_adrv9025_DpdActGainMonitorCtrl {
    uint8_t dpdGainMonitorEnable;             /*!< 0: DPD Gain monitoring disable - 1:DPD Gain monitoring enable. */
    uint8_t dpdGainMonitorIIREnable;          /*!< 0: DPD Gain monitoring IIR filters disable - 1:DPD Gain monitoring IIR filters enable. */
    uint8_t dpdGainMonitorIIRDecay;           /*!< DPD Power Monitor IIR Decay rate in the range of 0-16. Controls the decay rate of the DPD power monitor IIR filters to average samples for gain threshold violation detection
                                              A value of 0 written to this param exercises sample by sample gain threshold violation detection if gain monitoring is enabled
                                              Decay rate can be calculated as follows; N = (65536/(averaging_window_size/2)) where the decay rate will be equal to log2(N)*/
    adi_adrv9025_DpdModelSel_e dpdGainMonitorLowerThreshModelSel;  /*!< DPD model select when a DPD output gain underrange is detected.
                                                                   If gain monitoring is enabled, a threshold violation is triggered if DPD_OUT < (DPD_IN x adi_adrv9025_DpdActGainMonitorThresh_t.dpdGainMonitorLowerThresh)
                                                                   which can be used to auto load a model specified by this parameter.*/
    adi_adrv9025_DpdModelSel_e dpdGainMonitorUpperThreshModelSel;  /*!< DPD model select when a DPD output gain overrange is detected.
                                                                   If gain monitoring is enabled A threshold violation is triggered if DPD_OUT > (DPD_IN x adi_adrv9025_DpdActGainMonitorThresh_t.dpdGainMonitorUpperThresh)
                                                                   which can be used to auto load a model specified by this parameter.*/
    uint8_t lowGainModelAutoLoadEnable;		   /*!< 0:Don't switch to low gain model when low gain threshold is exceeded, 1:Switch to low gain model when low gain threshold is exceeded */
    uint8_t highGainModelAutoLoadEnable;		/*!< 0:Don't switch to high gain model when high gain threshold is exceeded, 1:Switch to high gain model when high gain threshold is exceeded */
}adi_adrv9025_DpdActGainMonitorCtrl_t;

/**
*\brief Data structure to hold the DPD actuator Gain Monitoring control and threshold configurations
*/
typedef struct adi_adrv9025_DpdActGainMonitorConfig
{
    uint32_t txChannelMask; /*!< A mask consisting of the OR'ed Tx channels for which the gain monitor configuration is requested to be applied */
    adi_adrv9025_DpdActGainMonitorCtrl_t dpdGainMonitorCtrl; /*!< Gain monitoring control can be used to enable/disable gain monitoring,
                                                             setup gain overrange and underrange model selects and setup the gain monitoring IIR decay rate  */
    adi_adrv9025_DpdActGainMonitorThresh_t dpdGainMonitorThresh; /*!< Sets up the gain monitoring threshold qualifying limit, upper gain threshold and the lower gain threshold */
} adi_adrv9025_DpdActGainMonitorConfig_t;

/**
* \brief Data structure to hold CFR Hard Clipper config
*/
typedef struct adi_adrv9025_CfrHardClipperConfig
{
    uint32_t txChannelMask; /*!< Mask consisting of 'OR'ed Tx channels for which the hard clipper config will be applied */
    uint8_t  cfrHardClipperEnable; /*!< 1- Enable hard clipper on the channels requested, 0 - Disable hard clipper */
    float    cfrHardClipperThreshold; /*!< Normalized threshold for the hard clipper in the range 0 to 1.
                                           The threshold is relative to 0dBFS which means that a threshold value of 1 corresponds to 0dBFS.
                                           The threshold is applied to an approximation of SQRT(I^2 + Q^2). The hard clipper threshold can
                                           be calculated as cfrHardClipperThreshold = 10^(threshold_dBFS/20)*/
} adi_adrv9025_CfrHardClipperConfig_t;

/**
* \brief Data structure to hold CFR Core control config settings
*/
typedef struct adi_adrv9025_CfrCtrlConfig
{
    uint32_t txChannelMask;                                    /*!< Mask consisting of 'OR'ed Tx channels for which the CFR core config will be applied */
    adi_adrv9025_CfrModeSel_e cfrMode;                         /*!< Selects the mode in which CFR is required to operate in. Currently, Mode 1 is the only mode supported */
    uint16_t cfrTxDelay;                                       /*!< Sets CFR delay per engine in units of samples at the CFR input rate (JESD 204b Tx rate) */
    float cfrPeakThreshold;                                    /*!< Sets the target CFR peak detection and correction threshold. The threshold is calculated as cfrPeakThreshold = 10^(Target PAR_dB / 20) * RMS_Input. The peak threshold is set to 0.79 by default*/
    float cfrEngine1PeakThresholdScaler;                       /*!< Threshold Scaler for engine CFR engine 1 */
    float cfrEngine2PeakThresholdScaler;                       /*!< Threshold Scaler for engine CFR engine 2 */
    float cfrEngine3PeakThresholdScaler;                       /*!< Threshold Scaler for engine CFR engine 3 */
    float cfrCorrectionThresholdScaler;                        /*!< Threshold Scaler for CFR correction */
    adi_adrv9025_CfrInterpolationSel_e cfrInterpolationFactor; /*!< Selects the interpolation factor to be applied to CFR input before peak detection. The CFR peak detectors can run at a higher (interpolated) rate to enable better peak detection */
    uint8_t cfrEngine1MaxNumOfPeaks;                           /*!< Sets the maximum number of peaks to remove in one group for CFR engine 1. Range [0-5]. Default value is 5. User needs to set this to 0 when engine is disabled. It's suggested to have descending order of max number of peaks where Engine1 has the highest value */
    uint8_t cfrEngine2MaxNumOfPeaks;                           /*!< Sets the maximum number of peaks to remove in one group for CFR engine 2. Range [0-5]. Default value is 5. User needs to set this to 0 when engine is disabled. It's suggested to have descending order of max number of peaks where Engine1 has the highest value */
    uint8_t cfrEngine3MaxNumOfPeaks;                           /*!< Sets the maximum number of peaks to remove in one group for CFR engine 3. Range [0-5]. Default value is 5. User needs to set this to 0 when engine is disabled. It's suggested to have descending order of max number of peaks where Engine1 has the highest value */
} adi_adrv9025_CfrCtrlConfig_t;

/**
* \brief Data structure to hold CFR engine enable/disable config for the 3 CFR engines
*/
typedef struct adi_adrv9025_CfrEnable
{
    uint32_t txChannelMask; /*!< Mask consisting of 'OR'ed Tx channels for which the CFR core config will be applied */
    uint8_t  cfrEngine1Enable; /*!< Enable(1) or disable(0) CFR Engine 1 */
    uint8_t  cfrEngine1BypassEnable; /*!< Enable CFR Engine 1 in bypass mode. This is valid only when cfrEngine1Enable is set to 1 */
    uint8_t  cfrEngine2Enable; /*!< Enable(1) or disable(0) CFR Engine 2 */
    uint8_t  cfrEngine2BypassEnable; /*!< Enable CFR Engine 2 in bypass mode. This is valid only when cfrEngine2Enable is set to 1 */
    uint8_t  cfrEngine3Enable; /*!< Enable(1) or disable(0) CFR Engine 3 */
    uint8_t  cfrEngine3BypassEnable; /*!< Enable CFR Engine 3 in bypass mode. This is valid only when cfrEngine3Enable is set to 1 */
} adi_adrv9025_CfrEnable_t;

/**
* \brief Data structure to hold Tx CFR engine statistics
*/
typedef struct adi_adrv9025_CfrStatistics
{
    uint64_t cfrEngine1PeaksDetected;    /*!< No. of peaks detected by CFR engine 1 since last reset */
    uint64_t cfrEngine1PeaksSkipped;     /*!< No. of peaks skipped by CFR engine 1 since last reset */
    uint64_t cfrEngine2PeaksDetected;    /*!< No. of peaks detected by CFR engine 2 since last reset */
    uint64_t cfrEngine2PeaksSkipped;     /*!< No. of peaks skipped by CFR engine 2 since last reset */
    uint64_t cfrEngine3PeaksDetected;    /*!< No. of peaks detected by CFR engine 3 since last reset */
    uint64_t cfrEngine3PeaksSkipped;     /*!< No. of peaks skipped by CFR engine 3 since last reset */
    uint64_t cfrNumSamplesClipped;       /*!< No. of samples clipped by the CFR engine since last reset */
} adi_adrv9025_CfrStatistics_t;

/**
* \brief Data structure to hold Tx CFR Status
*/
typedef struct adi_adrv9025_CfrStatus
{
    adi_adrv9025_CfrError_e cfrErrorCode;       /*!< Error code from Tx CFR */
    adi_adrv9025_CfrStatistics_t cfrStatistics; /*!< Statistics from the CFR module which includes peaks skipped, detected and clipped for each CFR engine */
} adi_adrv9025_CfrStatus_t;

/**
* \brief Data structure to hold Tx CFR Correction Pulse
*/
typedef struct adi_adrv9025_CfrCorrectionPulse
{
    int16_t  coeffRealHalfPulse[ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN];  /*!< An array consisting of the first half of the Real part of the complex CFR correction pulse coefficients */
    int16_t  coeffImaginaryHalfPulse[ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN]; /*!< An array consisting of the first half of the Imaginary part of the complex CFR correction pulse coefficients */
    uint16_t numCoeffs; /*!< No. of coefficients contained in coeffReal and coeffImaginary arrays */
} adi_adrv9025_CfrCorrectionPulse_t;

/**
* \brief Data structure to hold Closed Loop Gain Control(CLGC) configuration
*/
typedef struct adi_adrv9025_ClgcConfig
{
    uint32_t txChannelMask;                   /*!< Mask consisting of 'OR'ed Tx channels for which the CLGC config will be applied */
    uint8_t  clgcEnableGainControl;           /*!< This parameter enables the tracking and adjustment of loop gain in CLGC.
                                                   If this is set to 0, the CLGC tracking cal only measures the power levels and does not
                                                   execute loop gain control */
    uint16_t clgcMeasurementBatchTime_us;     /*!< CLGC sampling period in microseconds */
    uint32_t clgcMaxSampleBatchesPerClgcRun;  /*!< Unused */
    float    clgcExpectedLoopGain_dB;         /*!< Expected loop gain setting (ORx Power/Tx Power) for Closed Loop Gain Control */
    float    clgcExpectedLoopGainRipple_dB;   /*!< Unused */
    float    clgcTxQualifyingThreshold_dBFS;  /*!< Minimum threshold for the Tx signal required to run CLGC tracking */
    float    clgcOrxQualifyingThreshold_dBFS; /*!< Minimum threshold for the Orx signal required to run CLGC tracking */
    float    clgcOrxMinimumSnr_dB;            /*!< Unused */
    float    clgcMaxGainAdjustmentStepSize_dB;/*!< Maximum loop gain adjustment step size(Range 0 to 6dB) for Tx Attenuation in dB */
    float    clgcDampingParam;                /*!< Unused */
    float    clgcMinTxAttenAdjust_dB;         /*!< Minimum Tx attenuation in dB allowed to adjust to */
    float    clgcMaxTxAttenAdjust_dB;         /*!< Maximum Tx attenuation in dB allowed to adjust to */
} adi_adrv9025_ClgcConfig_t;

/**
* \brief Data structure to hold Closed Loop Gain Control(CLGC) Tx and ORx power measurements and tracking cal status
*/
typedef struct adi_adrv9025_ClgcStatus
{
    float clgcLoopGain;                 /*!< Represents the Tx - ORx loop gain equal to (ORx RMS Power)/(Tx RMS Power). The loop gain in
                                            dB can be computed as 20log10(clgcLoopGain) */
    float clgcTxRmsPower;               /*!< Measured absolute Tx RMS power by the CLGC module during the previous measurement cycle. The
                                            measured Tx power can be converted to dBFS as 20log10(clgcTxRmsPower/FULL_SCALE) */
    float clgcOrxRmsPower;              /*!< Measured absolute Orx RMS power by the CLGC module during the previous measurement cycle. The
                                            measured Orx power can be converted to dBFS as 20log10(clgcOrxRmsPower/FULL_SCALE) */
    uint16_t activeTxAttenIndex;        /*!< Active Tx attenuation table index applied to the Tx path.
                                            If using the default attenuation table, the attenuation applied to the Tx channel is equal to (activeTxAttenIndex x 0.05) dB */
    uint8_t  activeOrxGainIndex;        /*!< Active gain index of the ORx channel mapped to the Tx channel for which measurements are requested */
    adi_adrv9025_ClgcTrackingCalStatus_t clgcTrackingCalStatus; /*!< Contains the CLGC tracking cal status */
    adi_adrv9025_ClgcCaptureStatus_e clgcCaptureStatus;         /*!< Enum to denote the CLGC Tx-ORx data capture status for loop gain control/power measurement */
    adi_adrv9025_TrackingCalSyncStatus_e clgcSyncStatus;        /*!< Enum to denote the status of CLGC and DPD synchronization */
    adi_adrv9025_ClgcRunState_e clgcState;                      /*!< Enum to denote the current CLGC runtime state */
} adi_adrv9025_ClgcStatus_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_DFE_TYPES_H_ */
