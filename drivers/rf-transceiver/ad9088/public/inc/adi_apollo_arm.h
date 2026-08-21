/*!
 * \brief     Apollo ARM headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_ARM
 * @{
 */
#ifndef __ADI_APOLLO_ARM_H__
#define __ADI_APOLLO_ARM_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_arm_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Clear firmware boot status.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[out] status           Boot status - Pointer to boot status variable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_boot_status(adi_apollo_device_t *device, uint8_t *status);

/**
 * \brief  CPU ram boot error check.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_ram_boot_error_check(adi_apollo_device_t *device);

/**
 * \brief  Get firmware profile CRC status
 *
 * \param[in]  device Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return API_CMS_ERROR_PROFILE_CRC            Invalid profile CRC
 */
int32_t adi_apollo_arm_profile_crc_valid_get(adi_apollo_device_t *device);

/**
 * \brief  CPU rom ec transfer validation.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_rom_ec_transfer_check(adi_apollo_device_t *device);

/**
 * \brief  Prapare cpu for firmware loading.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_fwload_pre_config(adi_apollo_device_t *device);

/**
 * \brief  Post firmware loading setup to boot cpu using uploaded fw binary.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_fwload_post_config(adi_apollo_device_t *device);

/**
 * \brief  Load memory to CPU ram
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  binary_info      Pointer to the cpu binary loading config structure
 * \param[in]  chunk_sz_bytes   Number of bytes to transfer to CPU at a time
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_memload(adi_apollo_device_t *device, adi_apollo_arm_binary_info_t *binary_info, uint16_t chunk_sz_bytes);

/**
 * \brief  Load device profile to CPU
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  profile          adi_apollo_top_t structure pointer
 * \param[in]  chunk_sz_bytes   Number of bytes to transfer to CPU at a time
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_profile_write(adi_apollo_device_t *device, adi_apollo_top_t *profile, uint16_t chunk_sz_bytes);

/**
 * \brief  Send device profile memory transfer complete indication to firmware
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  profile          adi_apollo_top_t structure pointer
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_profile_write_post_config(adi_apollo_device_t *device, adi_apollo_top_t *profile);

/**
 * \brief  Load device firmware to CPU
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  core                 Target core. \ref adi_apollo_cpu_id_e
 * \param[in]  fw_image_buf         Firmware binary byte array
 * \param[in]  fw_image_size_bytes  Firmware image size
 * \param[in]  chunk_sz_bytes       Number of bytes to transfer to CPU at a time
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_firmware_buf_write(adi_apollo_device_t *device, uint8_t core, uint8_t fw_image_buf[], uint32_t fw_image_size_bytes, uint16_t chunk_sz_bytes);

/**
 * \brief  Load device firmware to CPU
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  mem_addr_to_load     Memory address where the firware buffer needs to be written.
 * \param[in]  fw_image_buf         Firmware binary byte array
 * \param[in]  fw_image_size_bytes  Firmware image size
 * \param[in]  chunk_sz_bytes       Number of bytes to transfer to CPU at a time
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_tye_firmware_buf_write(adi_apollo_device_t *device, uint32_t mem_addr_to_load, uint8_t fw_image_buf[], uint32_t fw_image_size_bytes, uint16_t chunk_sz_bytes);

/**
 * \brief  Load device profile byte array to CPU
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  profile_buf          Device profile byte array
 * \param[in]  profile_size_bytes   Device profile buffer size
 * \param[in]  chunk_sz_bytes       Number of bytes to transfer to CPU at a time
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_profile_buf_write(adi_apollo_device_t *device, uint8_t profile_buf[], uint32_t profile_size_bytes, uint16_t chunk_sz_bytes);

/**
 * \brief  Check if Tiny Enclave is bypassed or not
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[out]  is_bypassed     1 = Tiny Enclave is bypassed, load standard binary. 0 = not bypassed, load TyE App pack binaries.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_tye_bypassed_get(adi_apollo_device_t *device, uint8_t *is_bypassed);

/**
 * \brief   Triggers the arm cores to boot TyE FW
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_tye_boot_ready_set(adi_apollo_device_t *device);

/**
 * \brief   Validate if secure boot passes
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   is_encrypted    FW in App pack is encrypted(1) or not(0)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_tye_validate_boot_completion(adi_apollo_device_t *device, uint8_t is_encrypted);

/**
 * \brief   Get Firmware error codes
 *
 * \param[in]   device      Context variable - Pointer to the APOLLO device data structure
 * \param[out]  cpu_errs    Struct containing firmware error codes
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_err_codes_get(adi_apollo_device_t *device, adi_apollo_cpu_errors_t *cpu_errs);

/**
 * \brief  Get FW IRQ OUT status 
 *
 * \param[in]  device            Context variable - Pointer to the APOLLO device data structure
 * \param[out] fw_irq_out        Status of FW interrupt triggering - 1 triggered, 0 not triggered
 * \param[out] irq_source        Source of IRQ trigger \ref adi_apollo_fw_irq_out_source_e
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_fw_irq_out_get(adi_apollo_device_t *device, uint8_t *fw_irq_out, uint32_t *irq_source);

/**
 * \brief  Clear FW IRQ sticky bits
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_arm_fw_irq_out_clear(adi_apollo_device_t *device);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_ARM_H__ */
/*! @} */
