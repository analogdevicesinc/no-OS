/*!
 * \brief     RXEN Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXEN
 * @{
 */
#ifndef __ADI_APOLLO_RXEN_H__
#define __ADI_APOLLO_RXEN_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_rxen_types.h"
#include "adi_apollo_private_blk_sel_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   Configure the entire rxen powerup control block. 
 *          Generates enable signals for converters and digital blocks.
 * 
 * \note Configures pwrup ctrl, edges, counters and pin sel.
 * 
 * \param[in]   device  Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs    ADC selections. \ref adi_apollo_rxen_adc_select_e
 * \param[in]   config  Configuration parameters. \ref adi_apollo_rxen_pwrup_blk_config_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxen_pwrup_ctrl_config_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_rxen_pwrup_blk_config_t *config);

/**
 * \brief   Configure rxen powerup control input selects. 
 * 
 * \param[in]   device  Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs    ADC selections. \ref adi_apollo_rxen_adc_select_e
 * \param[in]   config  Configuration parameters. \ref adi_apollo_rxen_pwrup_ctrl_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxen_pwrup_ctrl_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_rxen_pwrup_ctrl_t *config);

/**
 * \brief   Configure rxen rise/fall control for adc, digital and pa.
 * 
 * \param[in]   device  Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs    ADC selections. \ref adi_apollo_rxen_adc_select_e
 * \param[in]   config  Configuration parameters. \ref adi_apollo_rxen_pwrup_ctrl_edge_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxen_pwrup_ctrl_edge_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_rxen_pwrup_ctrl_edge_t *config);

/**
 * \brief   Configure rxen powerup control counters for sequencing 
 * 
 * \param[in]   device  Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs    ADC selections. \ref adi_apollo_rxen_adc_select_e
 * \param[in]   config  Configuration parameters. \ref adi_apollo_rxen_pwrup_ctrl_count_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxen_pwrup_ctrl_count_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_rxen_pwrup_ctrl_count_t *config);

/**
 * \brief   Configure rxen powerup ctrl enable pin selects
 * 
 * \param[in]   device  Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs    ADC selections. \ref adi_apollo_rxen_adc_select_e
 * \param[in]   pin     Enable pin select. \ref adi_apollo_puc_en_pin_sel_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxen_pwrup_ctrl_pin_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_puc_en_pin_sel_e pin);

/**
 * \brief   Set rxen powerup control spi enable 
 * 
 * \param[in]   device  Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs    ADC selections. \ref adi_apollo_rxen_adc_select_e
 * \param[in]   spi_en  0 = disable, 1 = enable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxen_pwrup_ctrl_spien_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint8_t spi_en);

/**
 * \brief   Configures digital blocks to enable/disable from powerup controller 
 * 
 *          Uses dig_en outputs from the power control block to 
 *          enable selected digital blocks.
 * 
 * \param[in]   device  Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dig_enables Dig enable selections from pwrup ctrl
 * \param[in]   config  Configuration parameters. \ref adi_apollo_rxen_blk_config_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxen_config_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dig_enables, const adi_apollo_rxen_blk_config_t *config);

/**
 * \brief   Selects spi or digital input to control which blocks are controlled by rxen 
 * 
 * \param[in]   device  Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dig_enables Dig enable selections from pwrup ctrl
 * \param[in]   config  Configuration parameters. \ref adi_apollo_rxen_ctrl_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxen_ctrl_set(adi_apollo_device_t *device,  adi_apollo_blk_sel_t dig_enables, const adi_apollo_rxen_ctrl_t *config);

/**
 * \brief   Sets respective rxen enable via spi control
 * 
 * \note This function is only applicable when spien_en is set. \ref adi_apollo_rxen_ctrl_t
 * 
 * \param[in]   device  Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dig_enables dig enable selections from pwrup ctrl
 * \param[in]   spi_en  Sets hi/lo the respective enable via spi. 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxen_spien_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t  dig_enables, uint8_t spi_en);


#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_RXEN_H__ */
/*! @} */
