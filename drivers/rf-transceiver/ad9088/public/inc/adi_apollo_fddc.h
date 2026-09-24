/*!
 * \brief     Fine DDC functional block API prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FDDC
 * @{
 */
#ifndef __ADI_APOLLO_FDDC_H__
#define __ADI_APOLLO_FDDC_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_fddc_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Set the fine DDC decimation
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fddcs      Target FDDC. ADI_APOLLO_FDDC_A0, _A0-_A7, _B0-_B7, _ALL \ref adi_apollo_fine_ddc_select_e
 * \param[in]  dcm        Fine decimation enum. ADI_APOLLO_FDDC_RATIO_1, _2, _4, _8, _16, _32 \ref adi_apollo_fddc_ratio_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fddc_dcm_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, adi_apollo_fddc_ratio_e dcm);

/**
 * \brief  Set the fine DDC link num
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fddcs      Target FDDC \ref adi_apollo_fine_ddc_select_e
 * \param[in]  link_num   Jesd link num fddc is sent to. 0 = link0, 1 = link1
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fddc_link_num_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, uint8_t link_num);

/**
 * \brief  Set state of fine DDC debug clocks
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fddcs      Target FDDC \ref adi_apollo_fine_ddc_select_e
 * \param[in]  clkoff_n   Set to 0xff for normal operation. See bitfield FINE_CLK_DEBUG.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fddc_debug_clkoff_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, uint8_t clkoff_n);

/**
 * \brief  Configure FDDC parameters
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fddcs      Target FDDC \ref adi_apollo_fine_ddc_select_e
 * \param[in]  config     \ref adi_apollo_fddc_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fddc_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, adi_apollo_fddc_pgm_t *config);

/**
 * \brief  Inspect FDDC parameters
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   fddc       Single Target FDDC \ref adi_apollo_fine_ddc_select_e
 * \param[out]  fddc_inspect     Pointer to fddc inspect structure. \ref adi_apollo_fddc_inspect_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fddc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddc, adi_apollo_fddc_inspect_t *fddc_inspect);

/**
 * \brief  Determine the numeric FDDC decimation value from the associated enum. Enum vals are HW specific
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bf_enum          Enum that contains register bitfield value for decimation. 
 * \param[out] val              Numeric value of decimation bf enum.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fddc_dcm_bf_to_val(adi_apollo_device_t *device, adi_apollo_fddc_ratio_e bf_enum, uint32_t *val);

/**
 * \brief  Enable the FDDC datapath HB1 +6dB gain
 * 
 * The HB1 filter contains an optional +6dB gain block.
 * This write needs to be performed before sync.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fddcs        Fine FDDC selection. \ref adi_apollo_fine_ddc_select_e
 * \param[in]  enable       Enable flag. 0: disable, 1: enable 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fddc_gain_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, uint8_t enable);

/**
 * \brief Get state of the FDDC datapath +6dB gain
 * 
 * The HB1 filter contains an optional +6dB gain block.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fddcs        Fine FDDC selection. Single selection only. \ref adi_apollo_fine_ddc_select_e
 * \param[out] enable       Enable flag. 0: disable, 1: enable 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fddc_gain_enable_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, uint8_t *enable);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_FDDC_H__ */
/*! @} */
