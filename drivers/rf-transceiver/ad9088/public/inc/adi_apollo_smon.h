/*!
 * \brief     SMON Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SMON
 * @{
 */
#ifndef __ADI_APOLLO_SMON_H__
#define __ADI_APOLLO_SMON_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_smon_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configure SMON parameters
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons               Target SMON  \ref adi_apollo_smon_sel_e
 * \param[in]  config              \ref adi_apollo_smon_pgm_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, adi_apollo_smon_pgm_t *config);

/**
 * \brief  Configure SMON DFOR parameters
 *
 * \deprecated  Use \ref adi_apollo_dformat_ctrl_bit_sel_set() and \ref adi_apollo_dformat_smon_fd_fddc_set() instead.
 *              This function will be removed in the future.
 * 
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links               Target links
 * \param[in]  fddcs               Target FDDC \ref adi_apollo_fine_ddc_select_e
 * \param[in]  config              \ref adi_apollo_smon_dfor_pgm_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_dfor_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t links, adi_apollo_blk_sel_t fddcs, adi_apollo_smon_dfor_pgm_t *config);

/**
 * \brief  Update SMON STATUS
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons               Target SMON  \ref adi_apollo_smon_sel_e
 * \param[in]  update              A pulse on this signal will cause the status value to change in the regmap. 0: Status not updated, 1: Status updated.
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_status_update(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, uint8_t update);

/**
 * \brief  Read SMON STATUS
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons               Target SMON  \ref adi_apollo_smon_sel_e
 * \param[out] data                \ref adi_apollo_smon_read_t 
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_read(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, adi_apollo_smon_read_t *data);

/**
 * \brief  Inspect SMON parameters
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons        Target SMON (only one per call) \ref adi_apollo_smon_sel_e
 * \param[out] smon_inspect Pointer to SMON inspect structure. \ref adi_apollo_smon_inspect_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, adi_apollo_smon_inspect_t *smon_inspect);

/**
 * \brief  Get the SMON peak value from register. Updates automatically.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons        Target SMON (only one per call) \ref adi_apollo_smon_sel_e
 * \param[out] pk_val       Pointer to SMON peadk value result
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_peak_val_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, uint16_t *pk_val);

/**
 * \brief  Set the SMON high/low thresholds for GPIO output
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons        Target SMON \ref adi_apollo_smon_sel_e
 * \param[in]  thresh_low   Lower threshold value (11-bit val)
 * \param[in]  thresh_high  Upper threshold value (11-bit val)
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_thresh_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, uint16_t thresh_low, uint16_t thresh_high);

/**
 * \brief  Get the SMON high/low thresholds for GPIO output
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons        Target SMON (only one select per call) \ref adi_apollo_smon_sel_e
 * \param[out] thresh_low   Pointer to lower threshold value (11-bit val)
 * \param[out] thresh_high  Pointer to upper threshold value (11-bit val)
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_thresh_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, uint16_t *thresh_low, uint16_t *thresh_high);

/**
 * \brief  Get the SMON peak value from an array of samples
 *         This function automatically determines the framer type.
 * 
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons        Target SMON (only one select per call) \ref adi_apollo_smon_sel_e
 * \param[in]  cap_array    Pointer array of samples
 * \param[in]  cap_len      Number of samples in 'cap_array'
 * \param[in]  offset       First sample in 'cap_arrray' to gebin deserialization
 * \param[in]  smon_ctrl_bit_mask Mask applied to each sample to extract the bit value
 * \param[out] sof_idx      Pointer to the start of frame offset found. Can be used to get the next frame via 'offset'
 * \param[out] pk_val       Pointer to the deserialized peak value found
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_peak_val_from_cap_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, 
                                              int16_t cap_array[], uint32_t cap_len, uint32_t offset,
                                              uint16_t smon_ctrl_bit_mask,
                                              uint32_t *sof_idx, uint16_t *pk_val);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_SMON_H__ */
/*! @} */
