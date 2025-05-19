/*!
 * \brief     TX top level API definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TX
 * @{
 */
#ifndef __ADI_APOLLO_TX_H__
#define __ADI_APOLLO_TX_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_tx_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configures TX data path blocks for device side
 * 
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_txpath_t
 * \param[in] jrx_config    JESD JRx parameters for Tx data path. \ref adi_apollo_jesd_rx_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tx_configure(adi_apollo_device_t* device, adi_apollo_sides_e side, adi_apollo_txpath_t *config, adi_apollo_jesd_rx_cfg_t *jrx_config);

/**
 * \brief  Configures TX coarse DUC
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx           Index of block to configure. \ref adi_apollo_cduc_idx_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_cduc_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tx_cduc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_cduc_idx_e idx, adi_apollo_cduc_cfg_t *config);

/**
 * \brief  Configures TX fine DUC
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx           Index of block to configure. \ref adi_apollo_fduc_idx_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_fduc_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tx_fduc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_fduc_idx_e idx, adi_apollo_fduc_cfg_t *config);

/**
 * \brief  Configures TX pfilt
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx           Index of block to configure. \ref adi_apollo_pfilt_idx_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_pfilt_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tx_pfilt_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_pfilt_idx_e idx, adi_apollo_pfilt_cfg_t *config);

/**
 * \brief  Configures TX cfir
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] side          Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx           Index of block to configure. \ref adi_apollo_cfir_idx_e
 * \param[in] config        Configuration parameters. \ref adi_apollo_cfir_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tx_cfir_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_cfir_idx_e idx, adi_apollo_cfir_cfg_t *config);

/**
 * \brief  Configures TX fsrc
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] side              Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx               Index of block to configure. \ref adi_apollo_fsrc_idx_e
 * \param[in] config            FSRC config parameters. \ref adi_apollo_fsrc_cfg_t
 * \param[in] jrx_link_config   JRX link config parameters. \ref adi_apollo_jesd_rx_link_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tx_fsrc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_fsrc_idx_e idx, 
    adi_apollo_fsrc_cfg_t *config, adi_apollo_jesd_rx_link_cfg_t *jrx_link_config);

/**
 * \brief  Configures TX misc (xbars, summer, low samp)
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] side              Device side, A or B \ref adi_apollo_sides_e
 * \param[in] config            TX misc config parameters (xbars, summer, low samp) \ref adi_apollo_txpath_misc_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tx_txpath_misc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_txpath_misc_t *config);

/**
 * \brief  Configures TX inv sinc
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] side              Device side, A or B \ref adi_apollo_sides_e
 * \param[in] idx               Index of block to configure. \ref adi_apollo_cduc_path_idx_e
 * \param[in] enable            inverse since enable. 1 to enable, 0 to disable
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tx_inv_sinc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_cduc_path_idx_e idx, uint8_t enable);


	
#ifndef CLIENT_IGNORE
/**
 * \brief  Configures TX/JRX rate matching with sample repeat if FSRC is not in use
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] side              Device side, A or B \ref adi_apollo_sides_e
 * \param[in] link_idx          Link index. \ref adi_apollo_jesd_links_e
 * \param[in] tx_path_config    Tx path for side from device profile. \ref adi_apollo_txpath_t
 * \param[in] jrx_config        JRX link config parameters. \ref adi_apollo_jesd_rx_cfg_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_tx_sample_repeat_configure(adi_apollo_device_t *device, adi_apollo_sides_e side,
                                              adi_apollo_jesd_links_e link_idx, adi_apollo_txpath_t *tx_path_config, adi_apollo_jesd_rx_cfg_t *jrx_config);
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_TX_H__ */
/*! @} */
