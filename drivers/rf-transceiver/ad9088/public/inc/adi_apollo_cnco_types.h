/*!
 * \brief   Coarse NCO functional block definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CNCO
 * @{
 */
#ifndef __ADI_APOLLO_CNCO_TYPES_H__
#define __ADI_APOLLO_CNCO_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_CNCO_NUM 8                   /* Sized for 8T8R */
#define ADI_APOLLO_CNCO_PER_SIDE_NUM 4

/*!
 * \brief Enumerates RX coarse NCO select
 */
typedef enum
{
    ADI_APOLLO_CNCO_NONE = 0x00,    /*!< No COARSE NCO */
    ADI_APOLLO_CNCO_A0 = 0x01,      /*!< COARSE NCO 0 of SIDE A */
    ADI_APOLLO_CNCO_A1 = 0x02,      /*!< COARSE NCO 1 of SIDE A */
    ADI_APOLLO_CNCO_A2 = 0x04,      /*!< COARSE NCO 2 of SIDE A */
    ADI_APOLLO_CNCO_A3 = 0x08,      /*!< COARSE NCO 3 of SIDE A */
    ADI_APOLLO_CNCO_B0 = 0x10,      /*!< COARSE NCO 0 of SIDE B */
    ADI_APOLLO_CNCO_B1 = 0x20,      /*!< COARSE NCO 1 of SIDE B */
    ADI_APOLLO_CNCO_B2 = 0x40,      /*!< COARSE NCO 2 of SIDE B */
    ADI_APOLLO_CNCO_B3 = 0x80,      /*!< COARSE NCO 3 of SIDE B */
    ADI_APOLLO_CNCO_ALL = 0xFF,     /*!< ALL COARSE NCOs (8T8R) */
    ADI_APOLLO_CNCO_ALL_4T4R = 0x33 /*!< ALL COARSE NCOs (4T4R) */
} adi_apollo_coarse_nco_select_e;

/*!
* \brief Enumerates cnco profile chan selection mode
*/
typedef enum {
    ADI_APOLLO_CNCO_CHAN_SEL_DIRECT_REGMAP  = 0x00,                 /*!< Direct spi/hsci nco profile select. Apply profile immediately */
    ADI_APOLLO_CNCO_CHAN_SEL_DIRECT_GPIO    = 0x04,                 /*!< Direct GPIO profile select. Apply profile immediately */
    ADI_APOLLO_CNCO_CHAN_SEL_TRIGGER_BASED  = 0x0d                  /*!< Trigger based hopping. Trig options: external, internal (timer based) and spi/hsci. */
} adi_apollo_cnco_profile_sel_mode_e;

/*!
* \brief Enumerates cnco phase handling after FTW hop
*/
typedef enum {
    ADI_APOLLO_CNCO_HOP_PHASE_COHERENT   = 0x00,                 /*!< Hopped phase coherent with initial phase */
    ADI_APOLLO_CNCO_HOP_PHASE_CONTINUOUS = 0x01,                 /*!< Maintain phase between hops */
    ADI_APOLLO_CNCO_HOP_PHASE_RESET      = 0x02                  /*!< Reset phase after hop */
} adi_apollo_cnco_hop_phase_handling_e;

/*!
* \brief Enumerates cnco profile select mode for trigger based hopping
*/
typedef enum {
    ADI_APOLLO_CNCO_TRIG_PROF_SEL_AUTO    = 0x0,                    /*!< Auto Hopping Mode selection in Trigger based hopping */
    ADI_APOLLO_CNCO_TRIG_PROF_SEL_REGMAP  = 0x1,                    /*!< Scheduled Regmap Hopping Mode selection in Trigger based Hopping */
    ADI_APOLLO_CNCO_TRIG_PROF_SEL_GPIO    = 0x2                     /*!< Scheduled GPIO hopping mode selection in Trigger based hopping */
}  adi_apollo_cnco_trig_profile_sel_mode_e;

/*!
* \brief Enumerates cnco mixer dc test mode values selection
*/
typedef enum {
    ADI_APOLLO_CNCO_MXR_TEST_RX_FS        = 0x7FF,                    /*!< Rx path Full scale value */
    ADI_APOLLO_CNCO_MXR_TEST_RX_FS_BY2    = 0x3FF,                    /*!< Rx path Full scale by 2 value */
    ADI_APOLLO_CNCO_MXR_TEST_RX_FS_BY4    = 0x1FF,                    /*!< Rx path Full scale by 4 value */
    ADI_APOLLO_CNCO_MXR_TEST_TX_FS        = 0x1FFF,                   /*!< Tx path Full scale value */
    ADI_APOLLO_CNCO_MXR_TEST_TX_FS_BY2    = 0xFFF,                    /*!< Tx path Full scale by 2 value */
    ADI_APOLLO_CNCO_MXR_TEST_TX_FS_BY4    = 0x7FF,                    /*!< Tx path Full scale by 4 value */
}  adi_apollo_cnco_mxr_test_sel_e;

/*!
 * \brief Tx coarse nco parameters
 */
typedef struct {
    uint32_t ftw;                                       /*!< Freq tuning work (phase increment) */
    adi_apollo_nco_mixer_mode_e mode;                   /*!< NCO mode. (ADI_APOLLO_MXR_VAR_IF_MODE, _ZERO_IF_MODE, _FS_BY_4_MODE, _TEST_MODE) \ref adi_apollo_nco_mixer_mode_e  */
    adi_apollo_drc_mixer_sel_e mixer;                   /*!< NCO mixer real or complex mode. (ADI_APOLLO_DRC_MIXER_REAL, _COMPLEX)\ref adi_apollo_drc_mixer_sel_e */
} adi_apollo_tx_cnco_t;

/*!
* \brief COARSE NCO programming data
*/
typedef struct {
    adi_apollo_nco_mixer_mode_e if_mode;                    /*!< \ref adi_apollo_nco_mixer_mode_e */
    adi_apollo_drc_mixer_sel_e mixer_sel;                   /*!< \ref adi_apollo_drc_mixer_sel_e */
    adi_apollo_nco_cmplx_mult_scale_e cmplx_mxr_scale_en;   /*!< Enables the complex mixer scaling (0.7x or 1x scaling) */
    adi_apollo_nco_profile_sel_mode_e profile_sel_mode;     /*!< Profile selection mode (direct reg, gpio or trigger) */
    uint8_t profile_num;                                    /*!< Profile index to program and set active (for direct spi/hsci mode)*/
    uint32_t drc_phase_inc;                                 /*!< NCO phase increment. Complex mixing freq = (drc_phase_inc * Fs) / 2^32 */
    uint32_t drc_phase_offset;                              /*!< NCO phase offset. Fs/32 clocks in 4T4R and Fs/16 clocks in 8T8R */
    uint32_t drc_phase_inc_frac_a;                          /*!< Numerator correction term for modulus phase accumulator */
    uint32_t drc_phase_inc_frac_b;                          /*!< Denominator correction term for modulus phase accumulator */
    uint16_t dc_testmode_value;                             /*!< dc multiplier for nco test mode */
    uint8_t drc_en;                                         /*!< Enabling Coarse DDC/DUC */
    uint8_t debug_drc_clkoff_n;                             /*!< If this bit is zero, NCO mixer Clock is shut off */
} adi_apollo_cnco_pgm_t;

/*!
* \brief COARSE NCO inspect params
*/
typedef struct {
    adi_apollo_cnco_cfg_t dp_cfg;                           /*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */
    
    uint8_t profile_num;                                    /*!< Shadow Register index for rd/wr */
    uint8_t active_profile_num;                             /*!< Profile index to program and set active (for direct spi/hsci mode)*/
    
    uint32_t active_phase_inc;                              /*!< Active phase inc */
    uint32_t active_phase_offset;                           /*!< Active phase offset */
    uint8_t drc_phase_dither_en;                            /*!< Enabling Phase Dither Correction in NCO output */
    uint8_t drc_amp_dither_en;                              /*!< Enabling Amplitude Dither Correction in NCO output */
} adi_apollo_cnco_inspect_t;

/*!
* \brief Coarse NCO hop programming data
*/
typedef struct {
    adi_apollo_nco_profile_sel_mode_e profile_sel_mode;         /*!< Profile selection mode (e.g. direct or trigger based) */
    adi_apollo_nco_auto_flip_incdir_e auto_mode;                /*!< Selects auto flip, increment or decrement when auto hop enabled. \ref adi_apollo_nco_auto_flip_incdir_e */
    uint8_t high_limit;                                         /*!< Upper hop limit profile num when auto hop flip is enabled */
    uint8_t low_limit;                                          /*!< Lower hop limit profile num when auto hop flip is enabled */
    uint8_t next_hop_number_wr_en;                              /*!< 1 = Enable writing next hop number. 0 = disable */
    uint8_t hop_ctrl_init;                                      /*!< 1 = Enable loading the initial profile to start hopping from. 0 = disable */
    uint8_t phase_handling;                                     /*!< Phase handling after frequency hop \ref adi_apollo_cnco_hop_phase_handling_e */
} adi_apollo_coarse_nco_hop_t;

/*!
* \brief COARSE NCO channel programming data
*/
typedef struct {
    uint32_t drc_phase_inc;                         /*!< NCO Phase Increment Value. Phase Increment Value for the NCO. Complex mixing frequency = (drc_phase_inc * Fs) / 2^10.*/
    uint32_t drc_phase_offset;                      /*!< DDC/DUC phase offset input. Registered in Fs/32 clock in 4T4R and Fs/16 clock in 8T8R */
    uint32_t drc_phase_inc_frac_a;                  /*!< Numerator correction term for modulus phase accumulator */
    uint32_t drc_phase_inc_frac_b;                  /*!< Denominator correction term for modulus phase accumulator */
} adi_apollo_coarse_nco_chan_pgm_t;

#endif /* __ADI_APOLLO_CNCO_TYPES_H__ */
/*! @} */
