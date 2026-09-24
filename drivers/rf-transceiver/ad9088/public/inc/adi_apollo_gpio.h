/*!
 * \brief     GPIO definition headers
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
#ifndef __ADI_APOLLO_GPIO_H__
#define __ADI_APOLLO_GPIO_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_gpio_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  GPIO Quick Config Mode
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] profile        Quick Config Profile \ref adi_apollo_gpio_quick_cfg_profile_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_quick_config_mode_set(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e profile);

/**
 * \brief  Configure CMOS GPIO & SYNC pads in CMOS Debug mode 
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] gpio_index        GPIO num : 0-46
 *                              0-30 CMOS GPIO's, 31-46 for SYNC PADS in CMOS Mode
 * \param[in] debug_stage       Debug Stage sel \ref adi_apollo_gpio_cmos_debug_stage_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_cmos_debug_mode_set(adi_apollo_device_t *device, uint8_t gpio_index,
                                            adi_apollo_gpio_cmos_debug_stage_e debug_stage);

/**
 * \brief  Configure CMOS GPIO & SYNC pads in CMOS Functional mode 
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] gpio_index        GPIO num : 0-46
 *                              0-30 CMOS GPIO's, 31-46 for SYNC PADS in CMOS Mode
 * \param[in] func_num          Function number to be attached with GPIO
 *                              \ref adi_apollo_gpio_func_e for details.
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_cmos_func_mode_set(adi_apollo_device_t *device, uint8_t gpio_index,
                                           adi_apollo_gpio_func_e func_num);

/**
 * \brief  Configure SYNC pads in CMOS or LVDS mode
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] pad_index         Sync pad index \ref adi_apollo_gpio_sync_pad_e for details
 * \param[in] mode              CMOS or LVDS mode \ref adi_apollo_gpio_sync_pad_mode_e
 *                    
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_sync_pad_cmos_lvds_mode_set(adi_apollo_device_t *device, uint8_t pad_index,
                                                    adi_apollo_gpio_sync_pad_mode_e mode);

/**
 * \brief  Configure CMOS GPIO & SYNC pads in CMOS GPIO mode 
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] gpio_index        GPIO num : 0-46
 *                              0-30 CMOS GPIO's, 31-46 for SYNC PADS in CMOS Mode
 * \param[in] gpio_dir          GPIO Direction. \ref adi_apollo_gpio_dir_e for details.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_cmos_gpio_mode_set(adi_apollo_device_t *device, uint8_t gpio_index, adi_apollo_gpio_dir_e gpio_dir);

/**
 * \brief  Configure  SYNC pads in LVDS Debug mode 
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] sync_pad          SYNC_PAD \ref adi_apollo_gpio_sync_pad_e for details.
 * \param[in] debug_func_num    Debug Function number to be attached with SYNC PAD
 *                              \ref adi_apollo_gpio_debug_func_e for details
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_sync_pad_lvds_debug_mode_set(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e sync_pad,
                                                     adi_apollo_gpio_debug_func_e debug_func_num);

/**
 * \brief  Configure  SYNC pads in LVDS Functional mode 
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] sync_pad          SYNC_PAD \ref adi_apollo_gpio_sync_pad_e for details.
 * \param[in] func_num          Function number to be attached with SYNC PAD.\ref adi_apollo_gpio_func_e for details. 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_sync_pad_lvds_func_mode_set(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e sync_pad, 
                                                    adi_apollo_gpio_func_e func_num);

/**
 * \brief  Sets data to be driven on GPIO pin
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] gpio_index        GPIO num : 0-46
 *                              0-30 CMOS GPIO's, 31-46 for SYNC PADS in CMOS Mode
 * \param[in] gpio_data         Data to be written to GPIO
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_cmos_output_set(adi_apollo_device_t *device, uint8_t gpio_index, uint8_t gpio_data);

/**
 * \brief  Reads the data from input GPIO pin
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] gpio_index        GPIO num : 0-46
 *                              0-30 CMOS GPIO's, 31-46 for SYNC PADS in CMOS Mode 
 * \param[out] gpio_data        Data from GPIO pin
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_cmos_input_get(adi_apollo_device_t *device, uint8_t gpio_index, uint8_t *gpio_data);

/**
 * \brief  Read back the data driven on SYNCIN Pads
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] sync_pad          SYNC_PAD \ref adi_apollo_gpio_sync_pad_e for details.
 * \param[out] pad_data         Data from SYNCIN Pad
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_sync_pad_lvds_input_get(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e sync_pad,
                                                uint8_t *pad_data);

/**
 * \brief  Enable(1) / Disable(0) Schmitt trigger in input buffer of GPIOs in CMOS mode. 
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure.
 * \param[in] gpio_index        GPIO num : 0-46.
 *                              0-30 CMOS GPIO's, 31-46 for SYNC PADS in CMOS Mode.
 * \param[in] st_enable         Enable (1) the schmitt trigger on input buffer or Disable (0).
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_cmos_st_enable(adi_apollo_device_t *device, uint8_t gpio_index, uint8_t st_enable);


/**
 * \brief  Enable(1) or Disable(0) On-chip 100 Ohm Termination on SYNCIN Pad's receive buffer.
 *
 * \param[in] device                Context variable - Pointer to the APOLLO device data structure
 * \param[in] syncin_pad            SYNCIN_PAD index. \ref adi_apollo_gpio_sync_pad_e for details.
 * \param[in] termination_enable    Enable (1) the termination on receive buffer or Disable (0).
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_syncin_pad_termination_enable(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e syncin_pad, uint8_t termination_enable);

/**
 * \brief  Enable(1) or Disable(0) Apollo LVDS pads sync mode 
 *
 * \param[in] device                Context variable - Pointer to the APOLLO device data structure
 * \param[in] sync_pad              SYNC_PAD index. \ref adi_apollo_gpio_sync_pad_e for details.
 * \param[in] enable                Enable (1) the LVDS sync mode or Disable (0).
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_sync_pad_lvds_enable(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e sync_pad, uint8_t enable);

/**
 * \brief  Configure GPIO pins for JESD 204B
 *
 * \param[in] device                Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_gpio_jesd_204b_configure(adi_apollo_device_t *device);


#ifdef __cplusplus
}
#endif
    
#endif /*__ADI_APOLLO_GPIO_H__ */
/*! @} */