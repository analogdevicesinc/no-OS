/*!
 * \brief     Loopback control functions
 *
 * \copyright copyright(c) 2023 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_LOOPBACK
 * @{
 */
#ifndef __ADI_APOLLO_LOOPBACK_H__
#define __ADI_APOLLO_LOOPBACK_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"
#include "adi_apollo_loopback_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Enable/disable loopback write FIFO clock in RX.
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sides      Side select \ref adi_apollo_side_select_e for designators.
 * \param[in]  enable     1 to enable side(s), 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb0_rx_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable);

/**
 * \brief  Enable/disable loopback for TX
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adcs       ADC select \ref adi_apollo_adc_select_e for designators.
 * \param[in]  enable     1 to enable adc(s), 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb0_tx_enable_set(adi_apollo_device_t *device, uint16_t adcs, uint8_t enable);

/**
 * \brief  Set Loopback0 ADC to DAC mux0 (hsdout) using fixed 1-to-1 mapping 
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sides               Side select (single side) \ref adi_apollo_side_select_e for designators.
 * \param[in]  adcs                Array of adcs to map to dacs: {DAC_0,...,DAC_N} \ref adi_apollo_adc_idx_e for designators.
 * \param[in]  adc_map_length      Length of adc map (2 for 4t4r, 4 for 8t8r)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb0_tx_xbar_set(adi_apollo_device_t *device, uint16_t sides, uint16_t adcs[], uint32_t adc_map_length);

/**
 * \brief  Set Loopback0 ADC to DAC mux0 (hsdout) using custom mapping 
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sides               Side select (single side) \ref adi_apollo_side_select_e for designators.
 * \param[in]  adcs                Array of adcs to map. \ref adi_apollo_adc_idx_e for designators.
 * \param[in]  dacs                Array of dacs to map to adcs: {adcs[0]->dacs[0], adcs[1]->dacs[1], ...} \ref adi_apollo_dac_idx_e for designators.
 * \param[in]  adc_map_length      Length of adc map (2 for 4t4r, 4 for 8t8r)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb0_tx_xbar_custom_set(adi_apollo_device_t *device, uint16_t sides, uint16_t adcs[], uint16_t dacs[], uint32_t adc_map_length);


/**
 * \brief  Set read pointer reset value
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adcs       ADC select \ref adi_apollo_adc_select_e for designators.
 * \param[in]  value      FIFO for read pointer reset (0, 1, or 2)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb0_read_ptr_rst_set(adi_apollo_device_t *device, uint16_t adcs, uint8_t value);

/**
 * \brief  Set write pointer reset value
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sides      Side select \ref adi_apollo_side_select_e for designators.
 * \param[in]  value      FIFO for write pointer reset (0, 1, or 2)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb0_write_ptr_rst_set(adi_apollo_device_t *device, uint16_t sides, uint8_t value);

/**
 * \brief  Get overflow status and clear
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs       ADC select \ref adi_apollo_adc_select_e for designators.
 * \param[out]  status     Status of overflow
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb0_overflow_status_get(adi_apollo_device_t *device, uint16_t adcs, uint8_t *status);

/**
 * \brief  Enable/disable loopback debug data mode
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sides      Side select \ref adi_apollo_side_select_e for designators.
 * \param[in]  enable     1 to enable side(s), 0 to disable
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb0_debug_data_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable);

/**
 * \brief  Enable/disable loopback from BMEM.
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sides      Side select \ref adi_apollo_side_select_e for designators.
 * \param[in]  enable     1 to enable side(s), 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb0_bmem_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable);

/**
 * \brief  Enable/disable JESD loopback
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sides      Side select \ref adi_apollo_side_select_e for designators.
 * \param[in]  enable     1 to enable side(s), 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_jesd_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable);

/**
 * \brief  Enable/disable LB1
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure 
 * \param[in]  sides      Side select \ref adi_apollo_side_select_e for designators.
 * \param[in]  enable     1 to enable, 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb1_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable);

/**
 * \brief  Enable/disable LB1 per CDUC (can be done dynamically)
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure 
 * \param[in]  cducs      CDUC select \ref adi_apollo_cduc_select_e for designators.
 * \param[in]  enable     1 to enable, 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb1_cduc_enable_set(adi_apollo_device_t *device, uint16_t cducs, uint8_t enable);

/**
 * \brief  Set LB1 blending mode. 
 * \note   Use FDUC gain enable blocks to scale Tx DP data to prevent overflow
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure 
 * \param[in]  cducs            Bit selector for CDUCs \ref adi_apollo_cduc_select_e for designators.
 * \param[in]  mode             Blend mode to apply to loopback1 data \ref adi_apollo_loopback_lb1_blend_mode_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb1_blend_set(adi_apollo_device_t *device, uint16_t cducs, adi_apollo_loopback_lb1_blend_mode_e mode);

/**
 * \brief  Enable/disable LB2
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure 
 * \param[in]  sides      Side select \ref adi_apollo_side_select_e for designators.
 * \param[in]  enable     1 to enable, 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb2_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable);

/**
 * \brief  Enable/disable LB2 per FDUC (can be done dynamically)
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure 
 * \param[in]  fducs      FDUC select \ref adi_apollo_fduc_select_e for designators.
 * \param[in]  enable     1 to enable, 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_loopback_lb2_fduc_enable_set(adi_apollo_device_t *device, uint16_t fducs, uint8_t enable);


#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_LOOPBACK_H__ */
/*! @} */
