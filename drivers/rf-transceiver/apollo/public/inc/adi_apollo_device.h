/*!
 * \brief     API header file
 *            This file contains all the publicly exposed methods and data
 *            structures to interface with API.
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_DEVICE
 * @{
 */
#ifndef __ADI_APOLLO_DEVICE_H__
#define __ADI_APOLLO_DEVICE_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_common.h"
#include "adi_apollo_device_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Perform SPI register write access to device
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 * \param[in]  addr     SPI address to which the value of data parameter shall be written
 * \param[in]  data     8-bit value to be written to SPI register defined by the address parameter.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_spi_register_set(adi_apollo_device_t *device, uint32_t addr, uint8_t data);

/**
 * \brief  Perform SPI register read access from device
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 * \param[in]  addr     SPI address from which the value of data parameter shall be read,
 * \param[out]  data     Pointer to an 8-bit variable to which the value of the
 *                       SPI register at the address defined by address parameter shall be stored.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_spi_register_get(adi_apollo_device_t *device, uint32_t addr, uint8_t *data);

/**
 * \brief  Open hardware platform
 *         Just call user callback for function pointer 'hw_open'. Please note this is optional if user
 *         configure hardware platform by themselves.
 * 
 * \param[in]  device Context variable - Pointer to the APOLLO device data structure
 * \param[in]  reset_opt A parameter to indicate reset operation. \ref adi_apollo_reset_e
 *                   ADI_APOLLO_NO_RESET indicates no reset is required
 *                   ADI_APOLLO_SOFT_RESET indicates a software reset is required.
 *                   ADI_APOLLO_HARD_RESET indicates a hardware reset is required.
 *                   ADI_APOLLO_SOFT_RESET_AND_INIT indicates a software reset + initialization is required.
 *                   ADI_APOLLO_HARD_RESET_AND_INIT indicates a hardware reset + initialization is required.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_hw_open(adi_apollo_device_t *device, adi_apollo_reset_e reset_opt);

/**
 * \brief  Close hardware platform
 *         Just call user callback for function pointer 'hw_close'. Please note this is optional if user
 *         close hardware platform by themselves.
 *
 * \param[in]  device Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_hw_close(adi_apollo_device_t *device);

/**
 * \brief  Initialize apollo device
 *         This API will configure device SPI working mode, and check power supplies status. Must be called
 *         after platform SPI master is already initialized and adi_apollo_device_reset() is called.
 *
 * \param[in]  device Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_init(adi_apollo_device_t *device);

/**
 * \brief  De-initialize device
 *		   This API will do hard then soft reset.
 *
 * \param[in]  device Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_deinit(adi_apollo_device_t *device);

/**
 * \brief  Reset device
 *         Issues a hard reset or soft reset of the device.
 *         Performs a full reset of device via the hardware pin (hard) or
 *         via the SPI register (soft).
 *         Resetting all SPI registers to default and triggering the required
 *         initialization sequence.
 *         adi_apollo_device_init() will be called if operation is ADI_APOLLO_SOFT_RESET_AND_INIT or
 *         ADI_APOLLO_HARD_RESET_AND_INIT.
 *
 * \param[in]  device    Context variable - Pointer to the APOLLO device data structure
 * \param[in]  reset_opt A parameter to indicate reset operation. \ref adi_apollo_reset_e
 *                   ADI_APOLLO_NO_RESET indicates no reset is required
 *                   ADI_APOLLO_SOFT_RESET indicates a software reset is required.
 *                   ADI_APOLLO_HARD_RESET indicates a hardware reset is required.
 *                   ADI_APOLLO_SOFT_RESET_AND_INIT indicates a software reset + initialization is required.
 *                   ADI_APOLLO_HARD_RESET_AND_INIT indicates a hardware reset + initialization is required.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_reset(adi_apollo_device_t *device, adi_apollo_reset_e reset_opt);

/**
 * \brief  Get chip identification data
 *         Read-back product type, identification and revision data.
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 * \param[out]  chip_id  \ref adi_cms_chip_id_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_chip_id_get(adi_apollo_device_t *device, adi_cms_chip_id_t *chip_id);

/**
 * \brief  Get API revision
 *
 * \param[in]  device    Context variable - Pointer to the APOLLO device data structure
 * \param[out]  rev_major Pointer to variable to store the major revision
 * \param[out]  rev_minor Pointer to variable to store the minor revision
 * \param[out]  rev_rc    Pointer to variable to store the rc    revision
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_api_revision_get(adi_apollo_device_t *device, uint16_t *rev_major,
                                           uint16_t *rev_minor, uint16_t *rev_rc);

/**
 * \brief  Get Laminate ID
 *
 * \param[in]  device    Context variable - Pointer to the APOLLO device data structure
 * \param[out]  id        Pointer to silicon laminate id.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_laminate_id_get(adi_apollo_device_t *device, uint8_t *id);

/**
 * \brief  Get DIE ID
 *
 * \param[in]  device    Context variable - Pointer to the APOLLO device data structure
 * \param[out]  id        Pointer to silicon DIE id.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_die_id_get(adi_apollo_device_t *device, uint8_t *id);

/**
 * \brief  Get Silicon Grade ID
 *
 * \param[in]   device      Context variable - Pointer to the APOLLO device data structure
 * \param[out]  si_grade    Pointer to Silicon Grade ID.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_si_grade_get(adi_apollo_device_t *device, uint8_t *si_grade);

/**
 * \brief Configure SPI settings
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  spi_config   Ptr to spi configuration \ref adi_apollo_device_spi_settings_t
 * \param[in]  spi_num      Spi num (0 = SPI0, 1 = SPI1)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_spi_configure(adi_apollo_device_t *device, adi_apollo_device_spi_settings_t *spi_config, uint8_t spi_num);

/**
 * \brief Configure HSCI settings
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  hsci_config  Ptr to hsci configuration \ref adi_apollo_device_hsci_settings_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_hsci_configure(adi_apollo_device_t *device, adi_apollo_device_hsci_settings_t *hsci_config);

/**
 * \brief  Get UUID
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[out]  uuid        Array of uuid
 * \param[in]  uuid_len     Length of uuid array param. Must be set to ADI_APOLLO_UUID_NUM_BYTES
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_uuid_get(adi_apollo_device_t *device, uint8_t uuid[], uint32_t uuid_len);

/**
 * \brief  Get TMU Data
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[out]  tmu_data    Temperature data returned by CPU
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_tmu_get(adi_apollo_device_t *device, adi_apollo_device_tmu_data_t *tmu_data);

/**
 * \brief  Enable TMU
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_device_tmu_enable(adi_apollo_device_t *device);

#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_DEVICE_H__ */
/*! @} */
