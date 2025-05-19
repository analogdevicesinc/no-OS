/*!
 * \brief     Test Mode Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TMODE
 * @{
 */
#ifndef __ADI_APOLLO_TMODE_H__
#define __ADI_APOLLO_TMODE_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_tmode_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Set ADC Test Mode 
 *
 * \param[in] device             Context variable - Pointer to the APOLLO device data structure
 * \param[in] links              Target links \ref adi_apollo_jesd_link_select_e
 * \param[in] converter_mask     Target converters selection mask \ref adi_apollo_rx_tmode_sel_e
 * \param[in] mode               Test mode, \ref adi_apollo_rx_tmode_type_sel_e
 * \param[in] res                Test mode output  resolution, \ref adi_apollo_rx_tmode_res_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tmode_config_set(adi_apollo_device_t *device, 
    const uint16_t links,
    const uint8_t converter_mask,
    adi_apollo_rx_tmode_type_sel_e mode,
    adi_apollo_rx_tmode_res_e res);

/**
 * \brief  Set ADC User Pattern of Test Mode
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links          Target links \ref adi_apollo_jesd_link_select_e
 * \param[in]  usr_pattern    User pattern
 * \param[in]  length         User pattern array length
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tmode_usr_pattern_set(adi_apollo_device_t *device,
    const uint16_t links,
    uint16_t usr_pattern[],
    uint32_t length);

/**
 * \brief  Set ADC Test Mode
 *
 * \param[in] device         Context variable -Pointer to the device structure
 * \param[in] links          Target links
 * \param[in] mode           Test mode, \ref adi_apollo_rx_tmode_type_sel_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tmode_type_sel_set(adi_apollo_device_t *device, 
    const uint16_t links,
    adi_apollo_rx_tmode_type_sel_e mode);

/**
 * \brief Set the adc test mode resolution.
 * 
 * \param[in] device         Context variable -Pointer to the device structure
 * \param[in] links          Target link select
 * \param[in] res            Test mode output resolution, \ref adi_apollo_rx_tmode_res_e
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0  
 */
int32_t adi_apollo_tmode_resolution_set(adi_apollo_device_t *device, 
    const uint16_t links,
    adi_apollo_rx_tmode_res_e res);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_TMODE_H__ */
/*! @} */
