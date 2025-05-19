/*!
 * \brief     RX top level API definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RX
 * @{
 */
#ifndef __ADI_APOLLO_RX_H__
#define __ADI_APOLLO_RX_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_rx_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configures RX data path blocks for device side
 * 
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_rxpath_t
 * \param[in] jtx_config    JESD JTx parameters for Rx data path. \ref adi_apollo_jesd_tx_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rx_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_rxpath_t *config, adi_apollo_jesd_tx_cfg_t *jtx_config);

/**
 * \brief  Configures RX coarse DDC
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx           Index of block to configure. \ref adi_apollo_cddc_idx_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_cddc_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rx_cddc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_cddc_idx_e idx, adi_apollo_cddc_cfg_t *config);

/**
 * \brief  Configures RX fine DDC
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx           Index of block to configure. \ref adi_apollo_fddc_idx_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_fddc_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rx_fddc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_fddc_idx_e idx, adi_apollo_fddc_cfg_t *config);

/**
 * \brief  Configures RX pfilt
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx           Index of block to configure. \ref adi_apollo_pfilt_idx_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_pfilt_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rx_pfilt_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_pfilt_idx_e idx, adi_apollo_pfilt_cfg_t *config);

/**
 * \brief  Configures RX crossbars and low sample enable
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_rxpath_misc_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rx_rxpath_misc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_rxpath_misc_t *config);

/**
 * \brief  configures rx cfir
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx           Index of block to configure. \ref adi_apollo_cfir_idx_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_cfir_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rx_cfir_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_cfir_idx_e idx, adi_apollo_cfir_cfg_t *config);

/**
 * \brief  Configures RX fsrc
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx           Index of block to configure. \ref adi_apollo_fsrc_idx_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_fsrc_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rx_fsrc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_fsrc_idx_e idx, adi_apollo_fsrc_cfg_t *config);

/**
 * \brief  Configure DFORMAT parameters
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  side             Target side \ref adi_apollo_sides_e
 * \param[in]  link_idx         Link index. There 2 per side.
 * \param[in]  config           Ptr to Dformat configuration. (1 per link) \ref adi_apollo_dformat_cfg_t
 * \param[in]  jtx_link_config  Ptr to JTx link configuration (1 per link) \ref adi_apollo_jesd_tx_link_cfg_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rx_dformat_configure(adi_apollo_device_t* device, adi_apollo_sides_e side, adi_apollo_jesd_links_e link_idx,
    adi_apollo_dformat_cfg_t *config, adi_apollo_jesd_tx_link_cfg_t* jtx_link_config);


#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_RX_H__ */
/*! @} */
