/*!
 * \brief     TXEN Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TXEN
 * @{
 */
#ifndef __ADI_APOLLO_TXEN_H__
#define __ADI_APOLLO_TXEN_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_txen_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configure the txen block
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure 
 * \param[in]  tx_enables   tx enable selections. \ref adi_apollo_txen_en_select_e
 * \param[in]  config       Pointer to txen block config. \ref adi_apollo_txen_blk_config_t
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txen_config_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t tx_enables, const adi_apollo_txen_blk_config_t *config);

/**
 * \brief  Configures tx_pwrup_ctrl block
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  dacs         DAC Selection. \ref adi_apollo_txen_dac_select_e
 * \param[in]  config       Pointer to tx_pwrup_ctrl block paramaters. \ref adi_apollo_txen_pwrup_blk_config_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txen_pwrup_ctrl_config_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, const adi_apollo_txen_pwrup_blk_config_t *config);

/**
 * \brief  Configures tx_pwrup_ctrl reg
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  dacs         DAC Selection. \ref adi_apollo_txen_dac_select_e
 * \param[in]  config       Pointer to tx_pwrup_ctrl reg paramaters. \ref adi_apollo_txen_pwrup_ctrl_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txen_pwrup_ctrl_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, const adi_apollo_txen_pwrup_ctrl_t *config);

/**
 * \brief  Configures edges of dac_en, dig_en, pa_en signals
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  dacs         DAC Selection. \ref adi_apollo_txen_dac_select_e
 * \param[in]  config       Pointer to tx_pwrup_ctrl block paramaters. \ref adi_apollo_txen_pwrup_ctrl_edge_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txen_pwrup_ctrl_edge_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, const adi_apollo_txen_pwrup_ctrl_edge_t *config);

/**
 * \brief  Sets maximum programmed counter values 
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  dacs         DAC Selection. \ref adi_apollo_txen_dac_select_e
 * \param[in]  config       Pointer to tx_pwrup_ctrl_count_set paramaters. \ref adi_apollo_txen_pwrup_ctrl_count_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txen_pwrup_ctrl_count_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, const adi_apollo_txen_pwrup_ctrl_count_t *config);

/**
 * \brief  Configures txen pin sel mapping
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  dacs         DAC Selection. \ref adi_apollo_txen_dac_select_e
 * \param[in]  pin          Powerup control enable pin select. \ref adi_apollo_puc_en_pin_sel_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txen_pwrup_ctrl_pin_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, adi_apollo_puc_en_pin_sel_e pin);

/**
 * \brief  Selects the txen clock rate
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  dacs         DAC Selection. \ref adi_apollo_txen_dac_select_e
 * \param[in]  sel_cnt_rate clock rate. \ref adi_apollo_puc_clk_rate_sel_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txen_pwrup_ctrl_sel_cnt_rate_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t sel_cnt_rate);

/**
 * \brief  Enable txen powerup control state machine
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  dacs         DAC Selection. \ref adi_apollo_txen_dac_select_e
 * \param[in]  sm_en        Enable or Disable State Machine.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txen_pwrup_ctrl_sm_en_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t sm_en);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_TXEN_H__ */
/*! @} */
