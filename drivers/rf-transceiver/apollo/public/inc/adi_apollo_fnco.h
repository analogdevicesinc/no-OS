/*!
 * \brief     Fine NCO functional block API prototypes
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
#ifndef __ADI_APOLLO_FNCO_H__
#define __ADI_APOLLO_FNCO_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_fnco_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Load a FINE NCO profile
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos      Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  word_sel   Select the phase incr or offset \ref adi_apollo_nco_profile_word_sel_e
 * \param[in]  first      Profile number to load (0 - 15)
 * \param[in]  words      Array of phase increment or offset words (32-bit)
 * \param[in]  length     Number of profiles to load. (first + length must be < 16)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_profile_load(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos,
                adi_apollo_nco_profile_word_sel_e word_sel, uint8_t first, uint32_t words[], uint32_t length);

/**
 * \brief  FINE NCO HOP Enable
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos      Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  config     \ref adi_apollo_fine_nco_hop_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_hop_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_fine_nco_hop_t *config);

/**
 * \brief  Configure Fine NCO channel parameters
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos        Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  profile_num  Profile to update
 * \param[in]  config       \ref adi_apollo_fine_nco_chan_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_chan_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t profile_num, adi_apollo_fine_nco_chan_pgm_t *config);

/**
 * \brief  Configure Fine NCO main parameters
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos        Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  config       \ref adi_apollo_fine_nco_main_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_main_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_fine_nco_main_pgm_t *config);

/**
 * \brief  Configure Fine NCO Rx parameters
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos      Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  config     \ref adi_apollo_fnco_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_fnco_pgm_t *config);

/**
 * \brief  Configure Fine NCO Rx parameters
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fnco       Single Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[out]  fnco_inspect Pointer to fnco inspect structure. \ref adi_apollo_fnco_inspect_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fnco, adi_apollo_fnco_inspect_t *fnco_inspect);

/**
 * \brief  Sets the fine NCO mixer for real or complex
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos      Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  mixer      Select real or complex mixer mode. \ref adi_apollo_drc_mixer_sel_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_mixer_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_drc_mixer_sel_e mixer);

/**
 * \brief  Sets the fine NCO mode for variable IF, ZIF, Fs/4 or test mode
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos      Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  mode       Select variable IF, zero IF, Fs/4 or test NCO mode. \ref adi_apollo_nco_mixer_mode_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_nco_mixer_mode_e mode);

/**
 * \brief  Sets the fine main NCO phase increment value.
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos      Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  ftw        Frequency tuning word value
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_main_phase_inc_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint64_t ftw);

/**
 * \brief  Sets the fine main NCO phase offset value.
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos      Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  pow        Phase offset tuning word value
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 * 
 * \note Hopping must be disabled to use this function. See \ref adi_apollo_fnco_hop_enable
 */
int32_t adi_apollo_fnco_main_phase_offset_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint64_t pow);


/**
 * \brief  Enables fine NCO instances
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos      Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  enable     1 to enable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 *
 * \note Hopping must be disabled to use this function. See \ref adi_apollo_fnco_hop_enable
 */
int32_t adi_apollo_fnco_enable_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t enable);


/**
 * \brief  Sets the fine NCO test mode input value
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos        Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  val          Mixer DC Test mode input value \ref adi_apollo_fnco_mxr_test_sel_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_test_mode_val_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint16_t val);


/**
 * \brief  Sets fine NCO hop freq tuning word (phase increment)
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos        Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  profile_num  The index in profile table to update
 * \param[in]  active_en    If true, will also set profile_num as the active profile. If in direct spi/hsci mode, output will be immediate.
 * \param[in]  ftw          Frequency tuning word value
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 * 
 * \note Hopping must be enabled to use this function. See \ref adi_apollo_fnco_hop_enable.
 */
int32_t adi_apollo_fnco_ftw_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t profile_num, uint8_t active_en, uint32_t ftw);

/**
 * \brief  Sets fine NCO phase hop offset word (phase offset)
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos        Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  profile_num  The index in profile table to update
 * \param[in]  active_en    If true, will also set profile_num as the active profile. If in direct spi/hsci mode, output will be immediate.
 * \param[in]  pow          Phase offset word value
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 * 
 * \note Hopping must be enabled to use this function. See \ref adi_apollo_fnco_hop_enable.
 */
int32_t adi_apollo_fnco_pow_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t profile_num, uint8_t active_en, uint32_t pow);

/**
 * \brief  Sets the profile selection mode
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos            Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  profile_sel_mode Profile selection mode (e.g. direct reg) \ref adi_apollo_nco_profile_sel_mode_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_profile_sel_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_nco_profile_sel_mode_e profile_sel_mode);

/**
 * \brief  Sets the active profile when profile select mode is direct register
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos            Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  profile_num      Profile table index for NCO
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_active_profile_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t profile_num);

/**
 * \brief  Enable or disable fine NCO hopping
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos      Target FNCOs \ref adi_apollo_fine_nco_select_e
 * \param[in]  hop_en     NCO hop enable. 1: enable hopping, 0: disable hopping
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_hop_enable(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t hop_en);

/**
 * \brief  Sets the fine NCO next hop number for trigger based hopping
 *
 * \note  Freq and phase can hop together or individually. See \ref adi_apollo_fnco_trig_hop_sel_e
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos            Target FNCOs \ref adi_apollo_coarse_nco_select_e
 * \param[in]  hop_num_freq     The freq profile number to hop to on next trigger.
 * \param[in]  hop_num_phase    The phase profile number to hop to on next trigger.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fnco_next_hop_num_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, int16_t hop_num_freq, int16_t hop_num_phase);


#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_FNCO_H__ */
/*! @} */
