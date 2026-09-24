/*!
 * \brief     GPIO select definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_GPIO
 * @{
 */
#ifndef __ADI_APOLLO_GPIO_HOP_H__
#define __ADI_APOLLO_GPIO_HOP_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_gpio.h"
#include "adi_apollo_gpio_hop_types.h"
#include "adi_apollo_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Set up GPIO profile selection for user-defined gpio configuration (no quick config)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  profile        Profile select struct \ref adi_apollo_gpio_hop_profile_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_profile_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_profile_t *profile);

/**
 * \brief  GPIO profile select bit calculation
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Profile select struct \ref adi_apollo_gpio_hop_profile_t
 * \param[in]  profile        Profile number to select
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_profile_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_profile_t *config, uint8_t profile, uint64_t *mask, uint64_t *value);

/**
 * \brief  GPIO profile select bit calculation for given quick config
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Quick config profile (valid 1234568) \ref adi_apollo_gpio_quick_cfg_profile_e
 * \param[in]  profile        Profile number to select
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_profile_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, uint8_t profile, uint64_t *mask, uint64_t *value);


/**
 * \brief  Set up GPIO block selection for user-defined gpio configuration (no quick config)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Block select struct \ref adi_apollo_gpio_hop_block_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_block_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_block_t *config);

/**
 * \brief  GPIO block select bit calculation
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Block select struct \ref adi_apollo_gpio_hop_block_t
 * \param[in]  block          Blocks to select \ref adi_apollo_gpio_hop_block_e
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_block_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_block_t *config, adi_apollo_gpio_hop_block_e block, uint64_t *mask, uint64_t *value);

/**
 * \brief  GPIO block select bit calculation for given quick config
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Quick config profile (valid 123458) \ref adi_apollo_gpio_quick_cfg_profile_e
 * \param[in]  block          Blocks to select \ref adi_apollo_gpio_hop_block_e
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_block_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, adi_apollo_gpio_hop_block_e block, uint64_t *mask, uint64_t *value);

/**
 * \brief  Set up GPIO side selection for user-defined gpio configuration (no quick config)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Side select struct \ref adi_apollo_gpio_hop_side_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_side_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_side_t *config);

/**
 * \brief  GPIO side select bit calculation
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Side select struct \ref adi_apollo_gpio_hop_side_t
 * \param[in]  side           Sides to select \ref adi_apollo_side_select_e
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_side_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_side_t *config, adi_apollo_side_select_e side, uint64_t *mask, uint64_t *value);

/**
 * \brief  GPIO side select bit calculation for given quick config
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Quick config profile (valid 12458) \ref adi_apollo_gpio_quick_cfg_profile_e
 * \param[in]  side           Sides to select \ref adi_apollo_side_select_e
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_side_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, adi_apollo_side_select_e side, uint64_t *mask, uint64_t *value);

/**
 * \brief  Set up GPIO slice selection for user-defined gpio configuration (no quick config)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Slice select struct \ref adi_apollo_gpio_hop_slice_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_slice_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_slice_t *config);

/**
 * \brief  GPIO slice select bit calculation
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Slice select struct \ref adi_apollo_gpio_hop_slice_t
 * \param[in]  slice          Slice number
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_slice_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_slice_t *config, uint8_t slice, uint64_t *mask, uint64_t *value);

/**
 * \brief  GPIO slice select bit calculation for given quick config
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Quick config profile (valid 12458) \ref adi_apollo_gpio_quick_cfg_profile_e
 * \param[in]  slice          Slice number
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_slice_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, uint8_t slice, uint64_t *mask, uint64_t *value);

/**
 * \brief  Set up GPIO terminal selection for user-defined gpio configuration (no quick config)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Slice select struct \ref adi_apollo_gpio_hop_terminal_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_terminal_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_terminal_t *config);

/**
 * \brief  GPIO terminal select bit calculation
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Terminal select struct \ref adi_apollo_gpio_hop_terminal_t
 * \param[in]  terminal       Terminal \ref adi_apollo_gpio_hop_terminal_e
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_terminal_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_terminal_t *config, adi_apollo_gpio_hop_terminal_e terminal, uint64_t *mask, uint64_t *value);

/**
 * \brief  GPIO terminal select bit calculation for given quick config
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  config         Quick config profile (valid 1234568) \ref adi_apollo_gpio_quick_cfg_profile_e
 * \param[in]  terminal       Terminal \ref adi_apollo_gpio_hop_terminal_e
 * \param[out] mask           GPIO mask to set
 * \param[out] value          GPIO values if corresponding bit in mask is set
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_terminal_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, adi_apollo_gpio_hop_terminal_e terminal, uint64_t *mask, uint64_t *value);

/**
 * \brief  GPIO/SPI select for block profile hopping
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  select         Hopping select \ref adi_apollo_gpio_hop_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_block_select_set(adi_apollo_device_t *device, adi_apollo_gpio_hop_select_e select);

/**
 * \brief  Block set for profile hopping (block_select must be set to SPI)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  block          Blocks to select \ref adi_apollo_gpio_hop_block_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_block_set(adi_apollo_device_t *device, adi_apollo_gpio_hop_block_e block);

/**
 * \brief  GPIO/SPI select for slice profile hopping. If SPI, overrides GPIO select for slice, side, and terminal.
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  block          Target block \ref adi_apollo_gpio_hop_block_e
 * \param[in]  select         Hopping select \ref adi_apollo_gpio_hop_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_slice_select_set(adi_apollo_device_t *device, adi_apollo_gpio_hop_block_e block, adi_apollo_gpio_hop_select_e select);

/**
 * \brief  Enable hopping for selected FNCOs (slice_select must be set to SPI)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal       Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fncos          FNCOs to select \ref adi_apollo_fnco_select_e
 * \param[in]  enable         Hopping enable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_fnco_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t fncos, uint8_t enable);

/**
 * \brief  Enable hopping for selected CNCOs (slice_select must be set to SPI)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal       Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cncos          CNCOs to select \ref adi_apollo_cnco_select_e
 * \param[in]  enable         Hopping enable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_cnco_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t cncos, uint8_t enable);

/**
 * \brief  Enable hopping for selected CFIR datapaths (slice_select must be set to SPI)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal       Target terminal \ref adi_apollo_terminal_e
 * \param[in]  cfirs          CFIRs to select \ref adi_apollo_cfir_select_e
 * \param[in]  paths          CFIR datapaths to select \ref adi_apollo_cfir_dp_sel
 * \param[in]  enable         Hopping enable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_cfir_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t cfirs, uint32_t paths, uint8_t enable);

/**
 * \brief  Enable hopping for selected PFILTs (slice_select must be set to SPI)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal       Target terminal \ref adi_apollo_terminal_e
 * \param[in]  sides          Sides to select \ref adi_apollo_side_select_e
 * \param[in]  enable         Hopping enable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_pfilt_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t sides, uint8_t enable);

/**
 * \brief  Enable hopping for selected reconfig blocks (slice_select must be set to SPI)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal       Target terminal \ref adi_apollo_terminal_e
 * \param[in]  fdrcs          FDRCs to select \ref adi_apollo_reconfig_select_e
 * \param[in]  enable         Hopping enable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_dynamic_config_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t fdrcs, uint8_t enable);

/**
 * \brief  Enable hopping for selected BMEMs (delay mode only) (slice_select must be set to SPI)
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems          Target BMEMs \ref adi_apollo_bmem_sel_e
 * \param[in]  enable         Hopping enable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_hop_bmem_delay_enable_set(adi_apollo_device_t *device, uint32_t bmems, uint8_t enable);

#ifdef __cplusplus
}
#endif
    
#endif /*__ADI_APOLLO_GPIO_HOP_H__ */
/*! @} */