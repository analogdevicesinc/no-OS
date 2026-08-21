/*!
 * \brief     Coarse NCO functional block API prototypes
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
#ifndef __ADI_APOLLO_CNCO_H__
#define __ADI_APOLLO_CNCO_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_cnco_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Load coarse nco phase increment profiles
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos      Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  word_sel   Select the phase incr or offset \ref adi_apollo_nco_profile_word_sel_e
 * \param[in]  first      Profile number to load (0 - 15)
 * \param[in]  words      Array of phase increment or offset words (32-bit)
 * \param[in]  length     Number of profiles to load. (first + length must be < 16)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_profile_load(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, 
                adi_apollo_nco_profile_word_sel_e word_sel, uint8_t first, uint32_t words[], uint32_t length);

/**
 * \brief  COARSE NCO HOP Enable
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos      Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  config     \ref adi_apollo_coarse_nco_hop_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_hop_enable(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, adi_apollo_coarse_nco_hop_t *config);

/**
 * \brief  Configure Coarse NCO channel parameters
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos        Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  profile_num  Profile to update
 * \param[in]  config       NCO channel config \ref adi_apollo_coarse_nco_chan_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_chan_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, uint8_t profile_num, adi_apollo_coarse_nco_chan_pgm_t *config);

/**
 * \brief  Configure Coarse NCO parameters
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos      Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  config     \ref adi_apollo_cnco_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, adi_apollo_cnco_pgm_t *config);

/**
 * \brief  Inspect Coarse NCO parameters
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cnco       Single Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[out]  cnco_inspect Pointer to cnco inspect structure. \ref adi_apollo_cnco_inspect_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cnco, adi_apollo_cnco_inspect_t *cnco_inspect);

/**
 * \brief  Enables coarse NCO instances
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos        Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  enable       1 to enable the NCO
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_enable_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, uint8_t enable);

/**
 * \brief  Sets the coarse NCO mode for variable IF, ZIF, Fs/4 or test mode
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos        Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  mode         Select variable IF, zero IF, Fs/4 or test NCO mode. \ref adi_apollo_nco_mixer_mode_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, adi_apollo_nco_mixer_mode_e mode);

/**
 * \brief  Sets the coarse NCO test mode input value
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos        Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  val          Mixer DC Test mode input value \ref adi_apollo_cnco_mxr_test_sel_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_test_mode_val_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, uint16_t val);

/**
 * \brief  Sets coarse NCO hop freq tuning word (phase increment)
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos        Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  profile_num  The index in profile table to update
 * \param[in]  active_en    If true, will also set profile_num as the active profile. If in direct spi/hsci mode, output will be immdediate. 
 * \param[in]  ftw          Frequency tuning word value
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_ftw_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, uint8_t profile_num, uint8_t active_en, uint32_t ftw);

/**
 * \brief  Sets coarse phase hop offset word (phase offset)
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos        Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  profile_num  The index in profile table to update
 * \param[in]  active_en    If true, will also set profile_num as the active profile. If in direct spi/hsci mode, output will be immdediate. 
 * \param[in]  pow          Phase offset word
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_pow_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, uint8_t profile_num, uint8_t active_en, uint32_t pow);

/**
 * \brief  Sets coarse NCO hop modulus params
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos        Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  frac_a       Numerator of modulus ratio
 * \param[in]  frac_b       Denominator of modulus ratio
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_mod_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, uint32_t frac_a, uint32_t frac_b);

/**
 * \brief  Sets the coarse NCO mixer for real or complex
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos        Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  mixer        Select real or complex mixer mode. \ref adi_apollo_drc_mixer_sel_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_mixer_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, adi_apollo_drc_mixer_sel_e mixer);

/**
 * \brief  Sets the profile selection mode
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos            Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  profile_sel_mode Profile selection mode (e.g. direct reg) \ref adi_apollo_nco_profile_sel_mode_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_profile_sel_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, adi_apollo_nco_profile_sel_mode_e profile_sel_mode);

/**
 * \brief  Sets the active profile when profile select mode is direct register
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos            Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  profile_num      Profile for NCO 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_active_profile_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, uint8_t profile_num);

/**
 * \brief  Sets the coarse NCO next hop number for trigger based hopping
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos        Target CNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  hop_num      The profile number to hop to on next trigger
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cnco_next_hop_num_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, int16_t hop_num);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_CNCO_H__ */
/*! @} */
