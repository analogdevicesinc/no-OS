/*!
 * \brief     Coarse DDC functional block API prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CDDC
 * @{
 */
#ifndef __ADI_APOLLO_CDDC_H__
#define __ADI_APOLLO_CDDC_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_cddc_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Set the coarse DDC decimation
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cddcs      Target CDDC. ADI_APOLLO_CDDC_A0, _A0 to A3, B0 to B3, _ALL \ref adi_apollo_cddc_select_e
 * \param[in]  dcm        Coarse decimation. ADI_APOLLO_CDDC_DCM_1, _2, _3, _4, _6, _12 \ref adi_apollo_coarse_ddc_dcm_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cddc_dcm_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, adi_apollo_coarse_ddc_dcm_e dcm);

/**
 * \brief  Set the coarse DDC link num
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cddcs      Target CDDC \ref adi_apollo_cddc_select_e
 * \param[in]  link_num   Jesd link num cddc is sent to (req's if fddc bypassed) 0 = link0, 1 = link1
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cddc_link_num_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint8_t link_num);

/**
 * \brief  Set state of coarse DDC debug clocks
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cddcs      Target CDDC \ref adi_apollo_cddc_select_e
 * \param[in]  clkoff_n   Set to 0xff for normal operation. See bitfield COARSE_CLK_DEBUG.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cddc_debug_clkoff_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint8_t clkoff_n);

/**
 * \brief  Configure CDDC parameters
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cddcs      Target CDDC \ref adi_apollo_cddc_select_e
 * \param[in]  config     CDDC configuration \ref adi_apollo_cddc_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cddc_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, adi_apollo_cddc_pgm_t *config);

/**
 * \brief  Inspect CDDC parameters
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   cddc       Single Target CDDC \ref adi_apollo_cddc_select_e
 * \param[out]  cddc_inspect     Pointer to cddc inspect structure. \ref adi_apollo_cddc_inspect_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cddc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddc, adi_apollo_cddc_inspect_t *cddc_inspect);

/**
 * \brief  Sets the fine ddc bypass
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cddcs            Target CDDC \ref adi_apollo_cddc_select_e
 * \param[in]  bypass_enable    0 = don't bypass fddc, 1 = bypass fddc. CDDC output is sent directly to dformat
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cddc_fine_bypass_enable(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint8_t bypass_enable);

/**
 * \brief  Determine the numeric CDDC decimation value from the associated enum. Enum vals are HW specific
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bf_enum          Enum that contains register bitfield value for decimation. 
 * \param[out] val              Numeric value of decimation bf enum.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cddc_dcm_bf_to_val(adi_apollo_device_t *device, adi_apollo_coarse_ddc_dcm_e bf_enum, uint32_t *val);

/**
 * \brief  Enable or disable CDDC +6dB filter gains
 * 
 * The CDDC block contains +6dB gain options for the HB1 and TB1 filters.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cddcs            Target CDDC \ref adi_apollo_cddc_select_e
 * \param[in]  filt_sel         Filter selections. Selections include HB1 or TB1. \ref adi_apollo_cddc_gain_select_e
 * \param[in]  enable           Enable flag. 0: disable, 1: enable 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cddc_gain_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint16_t filt_sel, uint8_t enable);

/**
 * \brief Get state of the CDDC HB1 or TB1 +6dB gain
 * 
 * The CDDC block contains +6dB gain options for the HB1 and TB1 filters.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cddcs        Target CDDC. Single selection only. \ref adi_apollo_cddc_select_e
 * \param[in]  filt_sel     Filter selection. Single selection only. \ref adi_apollo_cddc_gain_select_e
 * \param[out] enable       Enable flag. 0: disable, 1: enable 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cddc_gain_enable_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint16_t filt_sel, uint8_t *enable);


#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_CDDC_H__ */
/*! @} */
