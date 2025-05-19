/*!
 * \brief     DDC Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_DFORMAT
 * @{
 */
#ifndef __ADI_APOLLO_DFORMAT_H__
#define __ADI_APOLLO_DFORMAT_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_dformat_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief  Configure DFORMAT parameters
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links      Target link select \ref adi_apollo_jesd_link_select_e
 * \param[in]  config     \ref adi_apollo_dformat_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_dformat_pgm(adi_apollo_device_t *device, const uint16_t links, adi_apollo_dformat_pgm_t *config);

/**
 * \brief  Configure chip output resolution
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link select \ref adi_apollo_jesd_link_select_e
 * \param[in]  resolution      Chip output resolution, \ref adi_apollo_chip_output_res_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_dformat_res_sel_set(adi_apollo_device_t *device, 
    const uint16_t links, 
    adi_apollo_chip_output_res_e resolution);

/**
 * \brief  Select output data format
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link select \ref adi_apollo_jesd_link_select_e
 * \param[in]  format          0: 2's complement, 1: offset binary, 2: gray code
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_dformat_format_sel_set(adi_apollo_device_t *device, const uint16_t links,
    uint8_t format);

/**
 * \brief  Select converter control bit function
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link select \ref adi_apollo_jesd_link_select_e
 * \param[in]  bit0_sel        0: overrange, 1: low, 2: SMON, 3: Fast Detection, 5: SYSREF, 8/9/10/11: NCO Chan Sel 0/1/2/3
 * \param[in]  bit1_sel        0: overrange, 1: low, 2: SMON, 3: Fast Detection, 5: SYSREF, 8/9/10/11: NCO Chan Sel 0/1/2/3
 * \param[in]  bit2_sel        0: overrange, 1: low, 2: SMON, 3: Fast Detection, 5: SYSREF, 8/9/10/11: NCO Chan Sel 0/1/2/3
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_dformat_ctrl_bit_sel_set(adi_apollo_device_t *device, const uint16_t links,
    uint8_t bit0_sel, uint8_t bit1_sel, uint8_t bit2_sel);

/**
 * \brief  Inspect DFORMAT parameters for a link
 *
 * \param[in]  device             Context variable - Pointer to the APOLLO device data structure
 * \param[in]  link               Target link select (ADI_APOLLO_LINK_A0, A1, B0, B1) \ref adi_apollo_jesd_link_select_e
 * \param[out] inspect            Pointer to DFormat inspect structure.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_dformat_inspect(adi_apollo_device_t *device, const uint16_t link, adi_apollo_dformat_inspect_t *inspect);

/**
 * \brief  Get DFORMAT overflow status
 *
 * \param[in]   device    Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link      Target link select (ADI_APOLLO_LINK_A0, A1, B0, B1) \ref adi_apollo_jesd_link_select_e
 * \param[in]   clear     Clear sticky overflow status bits
 * \param[out]  status    Overflow status
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_dformat_overflow_status_get(adi_apollo_device_t *device, const uint16_t link, uint8_t clear, uint8_t *status);

/**
 * \brief  Clear DFORMAT overflow status
 *
 * \param[in]   device    Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link      Target link select (ADI_APOLLO_LINK_A0, A1, B0, B1) \ref adi_apollo_jesd_link_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_dformat_overflow_status_clear(adi_apollo_device_t *device, const uint16_t link);

#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_DFORMAT_H__ */
/*! @} */
