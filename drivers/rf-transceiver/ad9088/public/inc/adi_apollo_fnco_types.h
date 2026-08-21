/*!
 * \brief   Fine NCO functional block definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FNCO
 * @{
 */
#ifndef __ADI_APOLLO_FNCO_TYPES_H__
#define __ADI_APOLLO_FNCO_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_FNCO_NUM 16      /* Sized for 8T8R */
#define ADI_APOLLO_FNCO_PER_SIDE_NUM 8

/*!
 * \brief Enumerates RX fine NCO select
 */
typedef enum
{
    ADI_APOLLO_FNCO_NONE = 0x0000,    /*!< No FINE NCO */
    ADI_APOLLO_FNCO_A0 = 0x0001,      /*!< FINE NCO 0 of SIDE A */
    ADI_APOLLO_FNCO_A1 = 0x0002,      /*!< FINE NCO 1 of SIDE A */
    ADI_APOLLO_FNCO_A2 = 0x0004,      /*!< FINE NCO 2 of SIDE A */
    ADI_APOLLO_FNCO_A3 = 0x0008,      /*!< FINE NCO 3 of SIDE A */
    ADI_APOLLO_FNCO_A4 = 0x0010,      /*!< FINE NCO 4 of SIDE A */
    ADI_APOLLO_FNCO_A5 = 0x0020,      /*!< FINE NCO 5 of SIDE A */
    ADI_APOLLO_FNCO_A6 = 0x0040,      /*!< FINE NCO 6 of SIDE A */
    ADI_APOLLO_FNCO_A7 = 0x0080,      /*!< FINE NCO 7 of SIDE A */
    ADI_APOLLO_FNCO_B0 = 0x0100,      /*!< FINE NCO 0 of SIDE B */
    ADI_APOLLO_FNCO_B1 = 0x0200,      /*!< FINE NCO 1 of SIDE B */
    ADI_APOLLO_FNCO_B2 = 0x0400,      /*!< FINE NCO 2 of SIDE B */
    ADI_APOLLO_FNCO_B3 = 0x0800,      /*!< FINE NCO 3 of SIDE B */
    ADI_APOLLO_FNCO_B4 = 0x1000,      /*!< FINE NCO 4 of SIDE B */
    ADI_APOLLO_FNCO_B5 = 0x2000,      /*!< FINE NCO 5 of SIDE B */
    ADI_APOLLO_FNCO_B6 = 0x4000,      /*!< FINE NCO 6 of SIDE B */
    ADI_APOLLO_FNCO_B7 = 0x8000,      /*!< FINE NCO 7 of SIDE B */
    ADI_APOLLO_FNCO_ALL = 0xFFFF,     /*!< ALL FINE NCOs (8T8R) */
    ADI_APOLLO_FNCO_A_ALL = 0x00FF,    /*!< ALL FINE NCOs SIDE A */
    ADI_APOLLO_FNCO_B_ALL = 0xFF00,    /*!< ALL FINE NCOs SIDE B */
    ADI_APOLLO_FNCO_ALL_4T4R = 0x0F0F, /*!< ALL FINE NCOs (4T4R) */
    ADI_APOLLO_FNCO_A_ALL_4T4R = 0x000F,  /*!< ALL FINE NCOs SIDE A (4T4R) */
    ADI_APOLLO_FNCO_B_ALL_4T4R = 0x0F00   /*!< ALL FINE NCOs SIDE B (4T4R) */
} adi_apollo_fine_nco_select_e;

/*!
* \brief Enumerates fnco mixer dc test mode values selection
*/
typedef enum {
    ADI_APOLLO_FNCO_MXR_TEST_RX_FS        = 0x1FFF,                /*!< Rx path Full scale value */
    ADI_APOLLO_FNCO_MXR_TEST_RX_FS_BY2    = 0xFFF,                 /*!< Rx path Full scale by 2 value */
    ADI_APOLLO_FNCO_MXR_TEST_RX_FS_BY4    = 0x7FF,                 /*!< Rx path Full scale by 4 value */
    ADI_APOLLO_FNCO_MXR_TEST_TX_FS        = 0x7FFF,                /*!< Tx path Full scale value */
    ADI_APOLLO_FNCO_MXR_TEST_TX_FS_BY2    = 0x3FFF,                /*!< Tx path Full scale by 2 value */
    ADI_APOLLO_FNCO_MXR_TEST_TX_FS_BY4    = 0x1FFF,                /*!< Tx path Full scale by 4 value */
}  adi_apollo_fnco_mxr_test_sel_e;

/*!
* \brief Enumerates fnco phase handling after FTW hop
*/
typedef enum {
    ADI_APOLLO_FNCO_HOP_PHASE_COHERENT   = 0x00,                 /*!< Hopped phase coherent with initial phase */
    ADI_APOLLO_FNCO_HOP_PHASE_CONTINUOUS = 0x01,                 /*!< Maintain phase between hops */
    ADI_APOLLO_FNCO_HOP_PHASE_RESET      = 0x02                  /*!< Reset phase after hop */
} adi_apollo_fnco_hop_phase_handling_e;

/*!
* \brief FINE NCO programming data
*/
typedef struct {
    uint8_t drc_en;                                         /*!< Fine nco enable. 1: enable, 0: disable */
    uint8_t debug_drc_clkoff_n;                             /*!< If this bit is zero, NCO mixer clock is shut off */
    uint8_t hop_mode_en;                                    /*!< 1: Fine NCO hopping enabled, 0: Fine NCO hopping disabled */
    adi_apollo_nco_mixer_mode_e if_mode;                    /*!< Select IF modes. For example, var if, zero if, fs/4 and DC test. \ref adi_apollo_nco_mixer_mode_e */
    uint16_t dc_testmode_value;                             /*!< dc multiplier for nco test mode */
    adi_apollo_drc_mixer_sel_e mixer_sel;                   /*!< Selects real or complex mixing. \ref adi_apollo_drc_mixer_sel_e */
    adi_apollo_nco_cmplx_mult_scale_e cmplx_mxr_scale_en;   /*!< Enables the complex mixer scaling. 0.7x or 1x scaling. \ref adi_apollo_nco_cmplx_mult_scale_e */
    adi_apollo_nco_profile_sel_mode_e profile_sel_mode;     /*!< Profile selection mode (direct reg, gpio or trigger) */
    uint8_t profile_num;                                    /*!< Profile index to program and set active (for direct spi/hsci mode)*/
    uint64_t main_phase_inc;                                /*!< Main NCO Phase Increment Value. Two's Complement Phase Increment Value for the NCO. Complex mixing frequency = (ddc_phase_inc * Fs) / 2^48. */
    uint64_t main_phase_offset;                             /*!< Main NCO Phase Offset. */
    uint64_t drc_phase_inc_frac_a;                          /*!< Two's Complement Numerator correction term for modulus phase accumulator */
    uint64_t drc_phase_inc_frac_b;                          /*!< Two's Complement Denominator correction term for modulus phase accumulator */
} adi_apollo_fnco_pgm_t;

/*!
* \brief FINE NCO inspect params
*/
typedef struct {
    adi_apollo_fnco_cfg_t dp_cfg;            	            /*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */
    
    uint8_t hop_mode_en;                                    /*!< 1: Fine NCO hopping enabled, 0: Fine NCO hopping disabled */
    uint8_t profile_num;                                    /*!< Profile index to program (for direct spi/hsci mode) */
    uint8_t regmap_hopprof;                                 /*!< Regmap based hopping profile number (0-31) */
    uint64_t active_phase_inc;                              /*!< Active phase inc */
    uint64_t active_phase_offset;                           /*!< Active phase offset */
    uint8_t drc_phase_dither_en;                            /*!< Enabling Phase Dither Correction in NCO output */
    uint8_t drc_amp_dither_en;                              /*!< Enabling Amplitude Dither Correction in NCO output */
} adi_apollo_fnco_inspect_t;

/*!
* \brief Fine NCO hop programming data
*/
typedef struct {
    adi_apollo_fnco_trig_hop_sel_e    nco_trig_hop_sel;         /*!< Enables hoping for freq, phase offset or both */
    adi_apollo_nco_profile_sel_mode_e profile_sel_mode;         /*!< Profile selection mode (e.g. direct or trigger based) */
    adi_apollo_nco_auto_flip_incdir_e phase_inc_auto_mode;      /*!< Selects freq auto flip, increment or decrement when auto hop enabled. \ref adi_apollo_nco_auto_flip_incdir_e */
    adi_apollo_nco_auto_flip_incdir_e phase_offset_auto_mode;   /*!< Selects phase offset auto flip, increment or decrement when auto hop enabled. \ref adi_apollo_nco_auto_flip_incdir_e */
    uint8_t phase_inc_high_limit;                               /*!< Upper freq hop limit profile num when auto hop flip is enabled */
    uint8_t phase_inc_low_limit;                                /*!< Lower freq hop limit profile num when auto hop flip is enabled */
    uint8_t phase_offset_high_limit;                            /*!< Upper phase offset hop limit profile num when auto hop flip is enabled */
    uint8_t phase_offset_low_limit;                             /*!< Lower phase offset hop limit profile num when auto hop flip is enabled */
    uint8_t phase_handling;                                     /*!< Phase handling after frequency hop \ref adi_apollo_fnco_hop_phase_handling_e */
} adi_apollo_fine_nco_hop_t;

/*!
* \brief FINE NCO main programming data
*/
typedef struct {
    uint64_t main_phase_inc;                            /*!< Main NCO Phase Increment Value. Two's Complement Phase Increment Value for the NCO. Complex mixing frequency = (ddc_phase_inc * Fs) / 2^48. */
    uint64_t main_phase_offset;                         /*!< Main NCO Phase Offset. */
    uint64_t drc_phase_inc_frac_a;                      /*!< Two's Complement Numerator correction term for modulus phase accumulator */
    uint64_t drc_phase_inc_frac_b;                      /*!< Two's Complement Denominator correction term for modulus phase accumulator */
} adi_apollo_fine_nco_main_pgm_t;

/*!
* \brief FINE NCO channel programming data
*/
typedef struct {
    uint32_t drc_phase_inc;                              /*!< Channel NCO Phase Increment Value. Two's Complement Phase Increment Value for the NCO. Complex mixing frequency = (ddc_phase_inc * Fs) / 2^32. */
    uint32_t drc_phase_offset;                           /*!< Channel NCO Phase Offset. */
} adi_apollo_fine_nco_chan_pgm_t;

#endif /* __ADI_APOLLO_FNCO_TYPES_H__ */
/*! @} */
