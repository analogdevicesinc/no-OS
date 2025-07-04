/*!
 * \brief     Multi Chip Sync Calibration user defined types definition header
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_MCS_CAL
 * @{
 */
#ifndef __ADI_APOLLO_MCS_CAL_TYPES_H__
#define __ADI_APOLLO_MCS_CAL_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

#ifdef DOXYGEN_RUN
#define ADI_APOLLO_PACKED(s) s
#else
#include "adi_apollo_platform_pack.h"
#endif

#ifdef __GNUC__
#define ADI_APOLLO_PACK_ENUM __attribute__((packed))
#else
#define ADI_APOLLO_PACK_ENUM
#endif

/*============================================================================*
 *                             D E F I N E S                                  *
 *============================================================================*/
#define ADF4382_MAX_COARSE_CODE                     (15U)
#define ADF4382_MAX_FINE_CODE                       (511U)
#define ADF4382_NUM_STROBES_TO_COVER_FINE_RANGE     (128U)
#define MCS_MAX_NUM_INST                            (3U)
#define MCS_MAX_NUM_EVENTS                          (3U)

/*============================================================================*
 *                                E N U M S                                   *
 *============================================================================*/
/**
 * MCS Parameter Enumerations
 */
typedef enum {
    MCS_PARAMS_ALL                                  = 0,       /*!< Set the entire MCS FW config structure. */
    MCS_USE_SIDE_A_AS_REFERENCE_UINT8               = 1,       /*!< Set to 1, Side A is used as reference when aligning or comparing against Side B while using Center TDC. */
    MCS_USE_GAPPED_SYSREF_UINT8                     = 2,       /*!< Set to 1, External SysRef is 'Gapped-Periodc'. Meaning, the signal randomly misses cycles. The nominal period of External SysRef must be Exactly the same as internal SysRef period.*/
    MCS_LEAVE_SYSREF_RECEIVER_ON_POST_SYNC_UINT8    = 3,       /*!< If set to 1, Apollo's External SysRef receiver is left turned on post MCS init Calibration.Set to 1 if running MCS Tracking calibration. */
    MCS_TRACK_ABORT_UINT8                           = 4,       /*!< Set to 1, MCS Tracking calibration will clear configuration variables, including ADF4382 Current DAC correction values, and will enter idle, waiting for users to configure the next action. */
    MCS_TRACK_HALT_UINT8                            = 5,       /*!< Set to 1, MCS Tracking calibration will bypass MCS tracking calibration. It will not use TDC or send commands to ADF4382.*/
    MCS_TRACK_INITIALIZE_UINT8                      = 6,       /*!< Set to 1, MCS Tracking calibration will take the user-defined track_start_coarse, track_start_fine, track_start_sleed_pol, maxFineCode, maxCoarseCode track_win, and adi_apollo_track_target as initial values for MCS Tracking calibration. */
    MCS_TRACK_FOREGROUND_UINT8                      = 7,       /*!< It can only be used after track_initialize has finished. Set to 1, track_foreground will run TDC measurements and send ADF4382 correction signals at a faster rate than track_backround. */
    MCS_TRACK_FORCE_FOREGROUND_UINT8                = 8,       /*!< It can only be used after track_initialize has finished. Set to 1, track_force_foreground will execute track_foreground. It is self-clearing. It can be written to 1 to re-run foreground. */
    MCS_TRACK_BACKGROUND_0_UINT8                    = 9,       /*!< Set to 1, MCS tracking calibration will measure time differences with TDC, and send correction signals to the single or side A's ADF4382. Up to 1 correction signal will be sent per second.*/
    MCS_TRACK_BACKGROUND_1_UINT8                    = 10,      /*!< Set to 1, MCS tracking calibration will measure time differences with TDC, and send correction signals to Side B's ADF4382. Up to 1 correction signal will be sent per second.*/
    MCS_TRACK_COARSE_JUMP_0_UINT8                   = 11,      /*!< Set to 1 to allow the single or side A's ADF4382 to do one Coarse DAC adjustment. */
    MCS_TRACK_COARSE_JUMP_1_UINT8                   = 12,      /*!< Set to 1 to allow side B's ADF4382 to do one Coarse DAC adjustment. */
    MCS_TRACK_FORCE_BACKGROUND_STEP_0_UINT8         = 13,      /*!< Allows for one track_background calibration cycle to be performed with the single or side A's ADF4382 while tracking calibration is halted. It may or may not send a strobe signal to the single or side A's ADF4382 */
    MCS_TRACK_FORCE_BACKGROUND_STEP_1_UINT8         = 14,      /*!< Allows for one track_background calibration cycle to be performed with side B's ADF4382 while tracking calibration is halted. It may or may not send a strobe signal to the single or side A's ADF4382 */
    MCS_MEASUREMENT_DECIMATION_RATE_UINT16          = 15,      /*!< Controls the number of External sysref measurements that MCS digital performs using TDC. If set to 65535, the total number of measurements are: 16*(65535 + 1) = 1048576. */
    MCS_REFERENCE_PERIOD_C_UINT64                   = 16,      /*!< External sysref period driven to the center sysref receiver in femtoseconds. Must be non-zero in any MCS init or tracking mode. */
    MCS_REFERENCE_PERIOD_A_UINT64                   = 17,      /*!< Usually, External sysref period driven to the Side A's sysref receiver in femtoseconds. Must be non-zero in any MCS init or tracking mode. If MCS Init Cal mode = 3, 4, or 5, and use_side_A_as_reference == 0; then, the Internal SysRef period in femtoseconds of the B-side's Internal Sysref should be provided. */
    MCS_REFERENCE_PERIOD_B_UINT64                   = 18,      /*!< Usually, External sysref period driven to the Side B's sysref receiver in femtoseconds. Must be non-zero in any MCS init or tracking mode. If MCS Init Cal mode = 3, 4, or 5, and use_side_A_as_reference == 1; then, the Internal SysRef period in femtoseconds of the B-side's Internal Sysref should be provided. */
    MCS_OFFSET_C_FEMTOSECONDS_INT64                 = 19,      /*!< Desired time difference between the rising edge of Center MCS's Internal sysref and the rising edge of Center External SysRef. Notice, we can only adjust in steps of 1 high-speed clock period during MCS init Cal. */
    MCS_OFFSET_A_FEMTOSECONDS_INT64                 = 20,      /*!< Desired time difference between the rising edge of Side A MCS's Internal sysref and the rising edge of its reference sysref. Notice, we can only adjust in steps of 1 high-speed clock period during MCS init Cal. */
    MCS_OFFSET_B_FEMTOSECONDS_INT64                 = 21,      /*!< Desired time difference between the rising edge of Side B MCS's Internal sysref and the rising edge of it reference sysref. Notice, we can only adjust in steps of 1 high-speed clock period during MCS init Cal. */
    MCS_ADF4382_DEL_MODE_UINT8                      = 22,      /*!< Equivalent to DEL_MODE in ADF4382 datasheet. */
    MCS_ADF4382_TRACK_START_COARSE_UINT8            = 23,      /*!< User-Defined starting value for Coarse DAC Correction. */
    MCS_ADF4382_TRACK_START_BLEED_POL_UINT8         = 24,      /*!< User-Defined starting value for DAC Bleed Polarity. */
    MCS_ADF4382_TRACK_POLARITY_SELECT_UINT8         = 25,      /*!< Set to 1 to invert the polarity of correction. */
    MCS_ADF4382_PHASE_ADJUSTMENT_UINT8              = 26,      /*!< Increments on the ADF4382's Bleed Current Correction DAC per strobe. Must be programmed to match register phase_adjustment on ADF4382. */
    MCS_ADF4382_TRACK_START_FINE_UINT16             = 27,      /*!< User-Defined starting value for Fine DAC Correction. */
    MCS_ADF4382_DELSTR_GPIO_0_UINT8                 = 28,      /*!< AD9084's GPIO to be used as DELSTR for the single or Side A's ADF4382. */
    MCS_ADF4382_DELSTR_GPIO_1_UINT8                 = 29,      /*!< AD9084's GPIO to be used as DELSTR for Side B's ADF4382. */
    MCS_ADF4382_DELADJ_GPIO_0_UINT8                 = 30,      /*!< AD9084's GPIO to be used as DELADJ for the single or Side A's ADF4382. */
    MCS_ADF4382_DELADJ_GPIO_1_UINT8                 = 31,      /*!< AD9084's GPIO to be used as DELADJ for Side B's ADF4382. */
    MCS_ADF4382_TRACK_WIN_UINT32                    = 32,      /*!< Time in in absolute-value femtoseconds allowed for internal sysref to drift with respect to external sysref before MCS Tracking Calibration sends adjustments to ADF4382. */
    MCS_ADF4382_TRACK_TARGET_0_INT32                = 33,      /*!< Time offset in picoseconds between the rising edge of internal and External sysref for the single or Side A's ADF4382. */
    MCS_ADF4382_TRACK_TARGET_1_INT32                = 34,      /*!< Time offset in picoseconds between the rising edge of internal and External sysref for Side B's ADF4382. */
    MCS_PARAMETER_LAST                              = 0xFF  
} ADI_APOLLO_PACK_ENUM adi_apollo_mcs_parameter_e;

/**
 * @brief MCS init cal state
 *
 */
typedef enum {
    MCS_START        = 0U,        /*!< Start state */
    MCS_SETUP_MEAS   = 1U,        /*!< State to setup and start measurement */
    MCS_EVENT_WAIT   = 2U,        /*!< Event wait state, interrupt or polling */
    MCS_PROCESS_MEAS = 3U,        /*!< State to process measurement results */
    MCS_ALIGN_CLOCKS = 4U,        /*!< State to align clocks */
    MCS_CLEANUP      = 5U,        /*!< State to cleanup */
    MCS_FINAL        = 6U,        /*!< Dummy Final state to indicate completion */
} ADI_APOLLO_PACK_ENUM adi_apollo_mcs_fsmstate_e;

/**
 * @brief Tracking cal state definition.
 *
 */
typedef enum {
    track_state_reset                           = 0,
    track_state_init_done                       = 1,
    track_state_foreground_done                 = 2,                      /*!< Foreground is complete */
    track_state_background_running              = 3,                      /*!< Tracking is good */
    track_state_background_running_active_fault = 4,                      /*!< There is an error that needs adjusting */
    track_state_last_state                      = 5,                      /*!< Marks last state */
} ADI_APOLLO_PACK_ENUM adi_apollo_mcs_cal_track_state_e;

/**
 * @brief Representation of the mode types that ADF4382 can operate.
 * Mirror of bit 5 (DEL_MODE) at address 0x32 in ADF4382 IC.
 */
typedef enum {
    CpBld_Mode = 0, /*!< Charge Pump Bleed current mode (Integer mode) */
    Frac0_Mode = 1  /*!< Fractional-N operation with the fraction=0 mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_drv_adf4382_delay_mode_e;

/*============================================================================*
 *                             S T R U C T S                                  *
 *============================================================================*/
/**
 * @brief Structure to hold the ADF4382' GPIO control pins.
 *
 */
typedef struct {
    uint8_t clock_deladj[2];
    uint8_t clock_delstr[2];
} adi_apollo_drv_adf4382_gpio_map_t;

/**
 * @brief ADF4382 specific configuration object.
 *
 */
ADI_APOLLO_PACKED(
typedef struct {
    adi_apollo_drv_adf4382_delay_mode_e del_mode;       /*!< 0: Integer = 0, frac-N = 1 */
    uint8_t track_start_coarse;                         /*!< 1: what is the initial coarse code applied to adf4382 */
    uint8_t track_start_bleed_pol;                      /*!< 2: Direction the fine and coarse DACs are setup for 0 = positive DAC, 1 = negative DAC */
    uint8_t track_polarity_select;                      /*!< 3: Variable used just in case feedback to adf4382 is wrong direction. */
    uint8_t phase_adjustment;                           /*!< 4: Number of fine steps taken on each DELSTR */
    uint8_t pad8;                                       /*!< 5: 1-byte padding */
    uint16_t track_start_fine;                          /*!< 6: what is the initial fine code applied to adf4382 */
    uint8_t DELSTR_gpio[2];  /*!< 7: Which GPIO to use for DELSTR */
    uint8_t pad[2];
    uint8_t DELADJ_gpio[2];  /*!< 8: Which GPIO to use for DELADJ */
    uint8_t pad2[2];
    uint32_t track_win;                                 /*!< 9: Tracking window size in femtoseconds */
    int32_t track_target[2]; /*!< 10: Track target (offset) in femtoseconds for mode with one TDC per ADF4382 */
} adi_apollo_drv_adf4382_config_t;)

/**
 * @brief ADF4382 specific status object.
 *
 */
ADI_APOLLO_PACKED(
typedef struct {
    uint8_t bleed_pol;           /*!< 0: 1 = Negative, 0 = positive */
    int8_t current_coarse_value; /*!< 1: Should be less than +/- coarse range */
    int16_t current_fine_value;  /*!< 2: INT mode: Within Fine range; FRAC mode: Any integer */
    uint8_t EOR_POS;             /*!< 4: Positive edge of fine range */
    uint8_t EOR_NEG;             /*!< 5: Negative edge of fine range */
    uint8_t EOR_Coarse;          /*!< 6: Coarse out of range */
    uint8_t padding_byte;        /*!< 7: Padding bytes */
} adi_apollo_drv_adf4382_status_t;)

/**
 * \brief Data structure to hold common init and tracking calibration status information
 */
ADI_APOLLO_PACKED(
typedef struct {
    uint32_t errorCode;
    uint32_t percentComplete;
    uint32_t performanceMetric;
    uint32_t iterCount;
    uint32_t updateCount;
} adi_apollo_CalStatus_t;)

/**
 * @brief MCS Calibration Configuration
 *
 */
ADI_APOLLO_PACKED(
typedef struct {
    uint8_t use_side_A_as_reference;                                    /*!< For ADI_APOLLO_MCS_INIT_MODE3/3/4, 0 = Use Side B Ref, 1 = Use Side A Ref */
    uint8_t use_gapped_sysref;                                          /*!< If 1, detecting true period of gapped sysref signal is enabled */
    uint8_t pad8;
    uint8_t leave_sysref_receiver_ON_post_sync;                         /*!< 0 = switch off sysref receiver, 1 = leave sysref receiver on */
    uint8_t track_abort;                                                /*!< Set to 1 to abort MCS Tracking calibration and Disable AD9084's TDC Hardware. */
    uint8_t track_halt;                                                 /*!< Set to 1 to bypass MCS Tracking Calibration cycle. */
    uint8_t track_initialize;                                           /*!< Set to 1 to capture the user-defined initial settings and enable AD9084's TDC and GPIO Hardware. */
    uint8_t pad8_2;
    uint8_t track_foreground;                                           /*!< Set to 1 to run fast tunning of internal and external sysref. Requires halt = 0. Suggested usage: post MCS Init Calibration. */
    uint8_t track_force_foreground;                                     /*!< Set to 1 to run fast tunning of internal and external sysref. Bypasses halt = 1. */
    uint8_t track_background[2];               /*!< Set to 1 to enable MCS tracking calibration cycle: TDC run, if needed, Send Strobe. Requires halt = 0. One calibration cycle per second. */
    uint8_t track_coarse_jump[2];              /*!< Set to 1 to allow crossing of coarse bleed current DAC values on ADF4382. */
    uint8_t track_force_background_step[2];    /*!< Set to 1 to enable MCS tracking calibration cycle: TDC run, if needed, Send Strobe. Bypasses halt = 1. One calibration cycle per second. */
    uint16_t measurement_decimation_rate;                               /*!< Decimation Rate used by AD9084's TDC digital. Total SysRef measurements = 16*(decimation + 1) */
    uint16_t pad16;
    uint64_t reference_period_C_femtoseconds;                           /*!< Reference Period in femtoseconds of side center MCS */
    uint64_t reference_period_A_femtoseconds;                           /*!< Reference Period in femtoseconds of side A MCS */
    uint64_t reference_period_B_femtoseconds;                           /*!< Reference Period in femtoseconds of side B MCS */
    int64_t offset_C_femtoseconds;                                      /*!< 64 Bits of Offset in femtoseconds of center MCS */
    int64_t offset_A_femtoseconds;                                      /*!< 64 Bits of Offset in femtoseconds of side A MCS */
    int64_t offset_B_femtoseconds;                                      /*!< 64 Bits of Offset in femtoseconds of side B MCS */
    adi_apollo_drv_adf4382_config_t adf4382_specific_config;
} adi_apollo_mcs_cal_config_t;)

/**
 * @brief MCS Private Calibration Status
 *
 */
ADI_APOLLO_PACKED(
typedef struct {
    uint8_t foreground_done;                                                                /*!< 0: Indicate if Foreground cal completed. 1 byte */
    adi_apollo_mcs_cal_track_state_e track_state[2];               /*!< 1: 0 = init, 1 = track good, 2 = track error; 2 bytes */
    uint8_t track_lock[2];                                         /*!< 2: Signals are within tracking window distance of one another; 2 bytes */
    uint8_t halt_active;                                                                    /*!< 3: Indicate if the calibration executed the halt command; 1 byte */
    uint8_t force_background_done[2];                              /*!< 4: Indicates if a Force Background Step Complete; 2 bytes */
    uint8_t abort_done;                                                                     /*!< 5: Indicates if abort has completed. 1 bytes */
    uint8_t pad_to_64bits[7];                                                               /*!< 6: Padding to bring to long long size. 7 bytes */
    adi_apollo_drv_adf4382_status_t adf4382_specific_status[2];    /*!< 7: ADF4382 specific status; 16 bytes */
    int64_t current_measure[2];                                    /*!< 8: Current measurement before last adjustment [femtoseconds] */
} adi_apollo_mcs_private_cal_status_t;)

/**
 * @brief MCS Calibration Status
 *
 */
ADI_APOLLO_PACKED(
typedef struct {
    adi_apollo_CalStatus_t hdr;                                  /*!< Header. */
    uint32_t pad32;
    adi_apollo_mcs_private_cal_status_t mcs_tracking_cal_status; /*!< MCS Tracking Calibration Status and Data. */
} adi_apollo_mcs_cal_status_t;)

/**
 * @brief MCS Calibration Data
 *
 * Interpretation of adi_apollo_mcs_cal_data_t:
 * Center, Side A and Side B here do not refer to the MCS which is being used, but the internl sysref which is being aligned, and they
 * may or not be the same.
 * ADI_APOLLO_MCS_MODE0: diff_C_Before_femtoseconds/Aft, internal_period_C_femtoseconds, is_C_Locked.
 * ADI_APOLLO_MCS_MODE1: diff_B_Before_femtoseconds/Aft, internal_period_B_femtoseconds, is_B_Locked, diff_A_Before_femtoseconds/Aft, internal_period_A_femtoseconds, is_A_Locked.
 * ADI_APOLLO_MCS_MODE2: if use_side_A_as_reference == 0, diff_A_Before_femtoseconds/Aft, internal_period_A_femtoseconds, is_A_Locked (Side B internal sysref taken as reference).
 *                       if use_side_A_as_reference == 1, diff_B_Before_femtoseconds/Aft, internal_period_B_femtoseconds, is_B_Locked (Side A internal sysref taken as reference).
 * ADI_APOLLO_MCS_MODE3: if use_side_A_as_reference == 0, diff_A_Before_femtoseconds, internal_period_A_femtoseconds (Side B internal sysref is taken as reference).
 *                       if use_side_A_as_reference == 1, diff_B_Before_femtoseconds, internal_period_B_femtoseconds (Side A internal sysref is taken as reference).
 * ADI_APOLLO_MCS_MODE4: diff_B_Before_femtoseconds/Aft, internal_period_B_femtoseconds, is_B_Locked, diff_A_Before_femtoseconds/Aft, internal_period_A_femtoseconds, is_A_Locked. use_side_A_as_reference == 0
 *                       means that Side B TDC is used for Step 1 and Side B internal sysref is aligned, and in Step 2, Side A internal
 *                       sysref is aligned. If use_side_A_as_reference == 1, Side A TDC is used for Step 1 and Side A internal sysref is aligned,
 *                       and in Step 2, Side B internal sysref is aligned
 */
ADI_APOLLO_PACKED(
typedef struct {
    uint8_t is_C_Locked;                       /*!< Is Center Int Sysref Locked? */
    uint8_t is_A_Locked;                       /*!< Is Side A Int Sysref Locked? */
    uint8_t is_B_Locked;                       /*!< Is Side A Int Sysref Locked? */
    uint8_t pad8[5];
    int64_t diff_C_Before_femtoseconds;        /*!< Time Difference Bef Alignment of Ctr Int Sysref in femtosecond */
    int64_t diff_A_Before_femtoseconds;        /*!< Time Difference Bef Alignment of Side A Int Sysref in femtosecond */
    int64_t diff_B_Before_femtoseconds;        /*!< Time Difference Bef Alignment of Side B Int Sysref in femtosecond */
    int64_t internal_period_C_femtoseconds;    /*!< Period of (usually internal) sysref for Center MCS in femtosecond */
    int64_t internal_period_A_femtoseconds;    /*!< Period of (usually internal) sysref for Side A MCS in femtosecond */
    int64_t internal_period_B_femtoseconds;    /*!< Period of (usually internal) sysref for Side B MCS in femtosecond */
    int64_t diff_C_After_femtoseconds;         /*!< Time Difference Aft Alignment of Ctr Int Sysref in femtosecond */
    int64_t diff_A_After_femtoseconds;         /*!< Time Difference Aft Alignment of Side A Int Sysref in femtosecond */
    int64_t diff_B_After_femtoseconds;         /*!< Time Difference Aft Alignment of Side B Int Sysref in femtosecond */
    int64_t recommended_offset_C_femtoseconds; /*!< Recommended offset Aft Alignment of Ctr Int Sysref in femtosecond */
    int64_t recommended_offset_A_femtoseconds; /*!< Recommended offset Aft Alignment of Side A Int Sysref in femtosecond */
    int64_t recommended_offset_B_femtoseconds; /*!< Recommended offset Aft Alignment of Side B Int Sysref in femtosecond */
} adi_apollo_mcs_cal_data_t;)

/**
 * @brief MCS calInit Status structure
 *
 */
ADI_APOLLO_PACKED(
typedef struct {
    adi_apollo_CalStatus_t hdr;     /*!< Header. */
    uint32_t mcsErr;                /*!< MCS Init Calibration error code. */
    adi_apollo_mcs_cal_data_t data; /*!< MCS Init Calibration Status and Data. */
} adi_apollo_mcs_cal_init_status_t;)

/**
 * @brief MCS Calibration State
 *
 * firstPassDone is only relevant to ADI_APOLLO_MCS_MODE4 and identifies step 1 or step 2
 * alignDone identifies measurement or remeasurement phase
 * event_waiting is the event being waited for in MCS_EVENT_WAIT state
 */
ADI_APOLLO_PACKED(
typedef struct {
    adi_apollo_mcs_fsmstate_e fsmState;     /*!< FSM State */
    adi_apollo_mcs_fsmstate_e prevFsmState; /*!< Previous FSM State */
    adi_apollo_mcs_fsm_cmd_e fsmCmd;        /*!< FSM Command */
    adi_apollo_mcs_fsm_cmd_e prevFsmCmd;    /*!< Previous FSM Command */
    uint8_t event_waiting;                  /*!< Event being waited for*/
    uint8_t alignDone;                      /*!< Is Alignment Done */
    uint8_t is_Side_A_Done;                 /*!< Is Side A Done Flag */
    uint8_t is_Side_B_Done;                 /*!< Is Side B Done Flag */
    uint8_t firstPassDone;                  /*!< First Pass Done */
    uint8_t isDone;                         /*!< Is MCS Operation Done */
    uint8_t extraSyncStep;                  /*!< Extra sync phase for modes 2 and 4 */
    uint8_t partial_alignment_done;         /*!< Counter Measure for Extra rotations from divG */
    uint8_t tdc_precondition_is_completed;  /*!< Flag that will indicate if we alredy completed the TDC precondition */
    uint8_t pad8_cal_state[3];              /*!< Padding */
} adi_apollo_mcs_cal_state_t;)

#ifndef CLIENT_IGNORE    
/**
 * @brief Top level MCS object
 *
 */
ADI_APOLLO_PACKED(
typedef struct {
    uint8_t *p_base_mcs_tdc_regmap[MCS_MAX_NUM_INST];   /*!< Pointers to base address of MCS TDC regmaps */
    adi_apollo_mcs_cal_config_t *config;                /*!< Pointer to config object */
    adi_apollo_mcs_cal_init_status_t cal_init_status;   /*!< Status data for the init cal */
    adi_apollo_mcs_cal_status_t tracking_status;        /*!< Status data for the tracking cal */
    adi_apollo_mcs_cal_state_t state;                   /*!< State object */
    uint8_t event_flags[MCS_MAX_NUM_EVENTS];            /*!< Event flag array */
    uint8_t pad;
} adi_apollo_mcs_t;)
#endif // !CLIENT_IGNORE

#endif /* __ADI_APOLLO_MCS_CAL_TYPES_H__ */
/*! @} */