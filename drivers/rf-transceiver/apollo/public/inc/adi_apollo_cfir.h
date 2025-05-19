/*!
 * \brief     CFIR Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CFIR
 * @{
 */
#ifndef __ADI_APOLLO_CFIR_H__
#define __ADI_APOLLO_CFIR_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_cfir_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configure CFIR parameters
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal            Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs               Target CFIR  \ref adi_apollo_cfir_sel_e
 * \param[in]  config              \ref adi_apollo_cfir_pgm_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_pgm(adi_apollo_device_t* device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, adi_apollo_cfir_pgm_t *config);


/**
 * \brief  Load CFIR coefficients for one or more profiles
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal             Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs                Target CFIR \ref adi_apollo_cfir_sel_e
 * \param[in]  cfir_profiles        CFIR profiles bit-or selector \ref adi_apollo_cfir_profile_sel_e
 * \param[in]  cfir_dps             CFIR data path bit-or selector \ref adi_apollo_cfir_dp_sel
 * \param[in]  cfir_coeff_i         Array of 16-bit coefficients I channel
 * \param[in]  cfir_coeff_q         Array of 16-bit coefficients Q channel
 * \param[in]  len                  Length of cfir_coeff_i and cfir_coeff_q arrays. Must be 16.
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_coeff_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint16_t cfir_coeff_i[], uint16_t cfir_coeff_q[], uint32_t len);

/**
 * \brief  Load CFIR complex scalar values for one or more profiles
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal             Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs                Target CFIR \ref adi_apollo_cfir_sel_e
 * \param[in]  cfir_profiles        CFIR profiles bit-or selector \ref adi_apollo_cfir_profile_sel_e
 * \param[in]  cfir_dps             CFIR data path bit-or selector \ref adi_apollo_cfir_dp_sel
 * \param[in]  scalar_i             Complex scalar for I stream (0x7fff default)
 * \param[in]  scalar_q             Complex scalar for Q stream (0x0000 default)
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_scalar_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint16_t scalar_i, uint16_t scalar_q);

/**
 * \brief  Load CFIR gain adjustment values for one or more profiles
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal             Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs                Target CFIR \ref adi_apollo_cfir_sel_e
 * \param[in]  cfir_profiles        CFIR profiles bit-or selector \ref adi_apollo_cfir_profile_sel_e
 * \param[in]  cfir_dps             CFIR data path bit-or selector \ref adi_apollo_cfir_dp_sel
 * \param[in]  gain                 Gain db adjustment \ref adi_apollo_cfir_gain_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_gain_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint8_t gain);

/**
 * \brief  Load CFIR sparse mode coefficient sel (hsel) for one or more profiles
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal             Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs                Target CFIR \ref adi_apollo_cfir_sel_e
 * \param[in]  cfir_profiles        CFIR profiles bit-or selector \ref adi_apollo_cfir_profile_sel_e
 * \param[in]  cfir_dps             CFIR data path bit-or selector \ref adi_apollo_cfir_dp_sel
 * \param[in]  cfir_coeff_sel       Array sparse mode coeff sel. Values range from 0..63 and depends on mem_sel setting
 * \param[in]  len                  Length of cfir_coeff_sel. Must be 16.
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_sparse_coeff_sel_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint16_t cfir_coeff_sel[], uint32_t len);

/**
 * \brief  Load CFIR sparse mode mem_sel values (mem_sel) for one or more profiles
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal             Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs                Target CFIR \ref adi_apollo_cfir_sel_e
 * \param[in]  cfir_profiles        CFIR profiles bit-or selector \ref adi_apollo_cfir_profile_sel_e
 * \param[in]  cfir_dps             CFIR data path bit-or selector \ref adi_apollo_cfir_dp_sel
 * \param[in]  cfir_mem_sel         Select which data store slices go into the data path mux. configurable per data path. (valid values 0..3)
 * \param[in]  len                  Length of cfir_coeff_sel. Must be 3.
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_sparse_mem_sel_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint8_t cfir_mem_sel[], uint32_t len);


/**
 * \brief  Configure the CFIR for a specific profile
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal             Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs                Target CFIR \ref adi_apollo_cfir_sel_e
 * \param[in]  cfir_dps             CFIR data path bit-or selector \ref adi_apollo_cfir_dp_sel
 * \param[in]  coeff_profile_sel    Profile to configure the CFIR \ref adi_apollo_cfir_profile_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_profile_sel(adi_apollo_device_t* device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_dps, uint8_t coeff_profile_sel);

/**
 * \brief  Enable or bypass the CFIR
 * 
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal             Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs                Target CFIR \ref adi_apollo_cfir_sel_e
 * \param[in]  mode                 Enable or bypass the CFIR.  \ref adi_apollo_cfir_enable_sel_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_mode_enable_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, adi_apollo_cfir_enable_sel_e mode);

/**
 * \brief  Inspect CFIR parameters
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal             Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs                Target CFIR  \ref adi_apollo_cfir_sel_e
 * \param[out]  cfir_inspect        Pointer to cfilt inspect struct \ref adi_apollo_cfir_inspect_t
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, adi_apollo_cfir_inspect_t *cfir_inspect);

/**
 * \brief  Set the CFIR profile selection and hopping mode
 * 
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs            Target CFIR (ADI_APOLLO_CFIR_A0, A1, B0, B1) \ref adi_apollo_cfir_sel_e
 * \param[in]  prof_sel_mode    Profile select mode. Can be direct spi, regmap and trig based. \ref adi_apollo_cfir_profile_sel_mode_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_profile_sel_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, const adi_apollo_cfir_profile_sel_mode_e prof_sel_mode);

/**
 * \brief  Set the next CFIR profile selection or hop
 * 
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs            Target CFIR (ADI_APOLLO_CFIR_A0, A1, B0, B1) \ref adi_apollo_cfir_sel_e
 * \param[in]  cfir_dps         CFIR data path bit-or selector (ADI_APOLLO_CFIR_DP_0, 1, 2, 3) \ref adi_apollo_cfir_dp_sel
 * \param[in]  hop_num          Profile (e.g. coeff set) to hop to. Will be immediate if in direct regmap mode, or scheduled for next trigger.
 * 
 * The behavior of the function is influenced by adi_apollo_cfir_profile_sel_mode_set().
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfir_next_hop_num_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_dps, int16_t hop_num);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_CFIR_H__ */
/*! @} */

