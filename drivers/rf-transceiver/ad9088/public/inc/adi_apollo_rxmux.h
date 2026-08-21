/*!
 * \brief     Rx Crossbar Mux functional block API prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXMUX
 * @{
 */
#ifndef __ADI_APOLLO_RXMUX_H__
#define __ADI_APOLLO_RXMUX_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_rxmux_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configure the ADC crossbar mux1
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  side_sel             Target bit-wise side select  \ref adi_apollo_side_select_e
 * \param[in]  cbout_from_adc_sel   Array of cross bar outputs from ADC selects. \ref adi_apollo_rx_mux0_sel_e
 * \param[in]  length               Length of cbout_from_adc_sel array param. (2 for 4T4R, 4 for 8T8R devices)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxmux_xbar1_set(adi_apollo_device_t *device,  adi_apollo_side_select_e side_sel, 
                adi_apollo_rx_mux0_sel_e cbout_from_adc_sel[], uint32_t length);

/**
 * \brief  Configure the Rx CDDC-to-FDDC crossbar
 *
 * \param[in]  device                   Context variable - Pointer to the APOLLO device data structure
 * \param[in]  side_sel                 Target bit-wise side select  \ref adi_apollo_side_select_e
 * \param[in]  coarse_to_fine_xbar_sel  Array of CDDC to FDDC configurations  \ref adi_apollo_rx_mux2_sel_e
 * \param[in]  length                   Length of coarse_to_fine_xbar_sel array param. Must be set to ADI_APOLLO_RX_MUX2_NUM
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxmux_xbar2_set(adi_apollo_device_t *device,  adi_apollo_side_select_e side_sel, adi_apollo_rx_mux2_sel_e coarse_to_fine_xbar_sel[], uint32_t length);

/**
 * \brief  Configure the Rx sample crossbar
 *
 * \param[in]  device                   Context variable - Pointer to the APOLLO device data structure
 * \param[in]  link_sel                 Target bit-wise link select  \ref adi_apollo_jesd_link_select_e
 * \param[in]  sample_offset            Value at which to start programming the xbar
 * \param[in]  sample_sel               Sample select xbar values \ref adi_apollo_rxmux_sample_sel_e
 * \param[in]  length                   Number of sbar values to program
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxmux_sample_xbar_set(adi_apollo_device_t *device, uint16_t link_sel, uint8_t sample_offset, adi_apollo_rxmux_sample_sel_e sample_sel[], uint32_t length);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_RXMUX_H__ */
/*! @} */
