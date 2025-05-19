/*!
 * \brief     Top level configuration definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CFG
 * @{
 */
#ifndef __ADI_APOLLO_CFG_H__
#define __ADI_APOLLO_CFG_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_cfg_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configures digital data paths from device profile
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] config        Configuration parameters. \ref adi_apollo_top_t
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_data_path(adi_apollo_device_t *device, adi_apollo_top_t *config);

/**
 * \brief  Configures the initial cal configuration for ADCs
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          ADC selection mask \ref adi_apollo_adc_select_e
 * \param[in] init_cal_cfg  Initial cal configuration \ref adi_apollo_init_cal_cfg_e
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_adc_init_cal_cfg_set(adi_apollo_device_t *device, const uint16_t adcs, adi_apollo_init_cal_cfg_e init_cal_cfg);

/**
 * \brief  Configures the initial cal configuration for DACs
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] dacs          DAC selection mask \ref adi_apollo_dac_select_e
 * \param[in] init_cal_cfg  Initial cal configuration \ref adi_apollo_init_cal_cfg_e
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_dac_init_cal_cfg_set(adi_apollo_device_t *device, const uint16_t dacs, adi_apollo_init_cal_cfg_e init_cal_cfg);

/**
 * \brief  Configures the initial cal configuration for Rx SERDES (JRx)
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] serdes        Serdes selection mask \ref adi_apollo_serdes_12pack_select_e
 * \param[in] init_cal_cfg  Initial cal configuration \ref adi_apollo_init_cal_cfg_e
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_serdes_rx_init_cal_cfg_set(adi_apollo_device_t *device, const uint16_t serdes, adi_apollo_init_cal_cfg_e init_cal_cfg);

/**
 * \brief  Configures the initial cal configuration for Tx SERDESs (JTx)
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] serdes        Serdes selection mask \ref adi_apollo_serdes_12pack_select_e
 * \param[in] init_cal_cfg  Initial cal configuration \ref adi_apollo_init_cal_cfg_e
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_serdes_tx_init_cal_cfg_set(adi_apollo_device_t *device, const uint16_t serdes, adi_apollo_init_cal_cfg_e init_cal_cfg);

/**
 * \brief Loads ADC calibration data
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          ADC selection mask. Selected units will get same cal_data \ref adi_apollo_adc_select_e
 * \param[in] mode          ADC mode selection. Random(0) or Sequential(1).
 * \param[in] cal_data      Array of calibration raw data for one ADC
 * \param[in] len           Number of bytes in cal_data (may include crc)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_adc_cal_data_set(adi_apollo_device_t *device, const uint16_t adcs, uint8_t mode, uint8_t cal_data[], uint32_t len);

/**
 * \brief Loads DAC calibration data
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] dacs          DAC selection mask. Selected units will get same cal_data \ref adi_apollo_dac_select_e
 * \param[in] cal_data      Array of calibration raw data for one DAC
 * \param[in] len           Number of bytes in cal_data (may include crc)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_dac_cal_data_set(adi_apollo_device_t *device, const uint16_t dacs, uint8_t cal_data[], uint32_t len);

/**
 * \brief Loads SERDES RX calibration data
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] serdes        Serdes selection mask \ref adi_apollo_serdes_12pack_select_e
 * \param[in] cal_data      Array of calibration raw data for one SERDES pack
 * \param[in] len           Number of bytes in cal_data (may include crc)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_serdes_rx_cal_data_set(adi_apollo_device_t *device, const uint16_t serdes, uint8_t cal_data[], uint32_t len);

/**
 * \brief Loads SERDES TX calibration data
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] serdes        Serdes selection mask \ref adi_apollo_serdes_12pack_select_e
 * \param[in] cal_data      Array of calibration raw data for one SERDES pack
 * \param[in] len           Number of bytes in cal_data (may include crc)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_serdes_tx_cal_data_set(adi_apollo_device_t *device, const uint16_t serdes, uint8_t cal_data[], uint32_t len);

/**
 * \brief Read the current ADC calibration data from device.
 *
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs          ADC selection mask. Only a single ADC may be selected. \ref adi_apollo_adc_select_e
 * \param[in]   mode          ADC mode selection. Random(0) or Sequential(1).
 * \param[out]  cal_data      Array to store calibration data. Memory is allocated by the caller.
 * \param[in]   len           Number of bytes in cal_data (includes room for ending checksum.)
 *                            The cal_data array length can be retrieved from \ref adi_apollo_cfg_adc_cal_data_len_get()
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_adc_cal_data_get(adi_apollo_device_t* device, const uint16_t adcs, uint8_t mode, uint8_t cal_data[], uint32_t len);

/**
 * \brief Read the current DAC calibration data from device.
 *
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dacs          DAC selection mask. Only a single DAC may be selected. \ref adi_apollo_dac_select_e
 * \param[out]  cal_data      Array to store calibration data. Memory is allocated by the caller.
 * \param[in]   len           Number of bytes in cal_data (includes room for ending checksum.)
 *                            The cal_data array length can be retrieved from \ref adi_apollo_cfg_dac_cal_data_len_get()
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_dac_cal_data_get(adi_apollo_device_t* device, const uint16_t dacs, uint8_t cal_data[], uint32_t len);

/**
 * \brief Read the current SERDES Rx calibration data from device.
 *
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   serdes        SERDES selection mask. Only a single SERDES may be selected. \ref adi_apollo_serdes_12pack_select_e
 * \param[out]  cal_data      Array to store calibration data. Memory is allocated by the caller.
 * \param[in]   len           Number of bytes in cal_data (includes room for ending checksum.)
 *                            The cal_data array length can be retrieved from \ref adi_apollo_cfg_serdes_rx_cal_data_len_get()
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_serdes_rx_cal_data_get(adi_apollo_device_t* device, const uint16_t serdes, uint8_t cal_data[], uint32_t len);

/**
 * \brief Read the current SERDES Tx calibration data from device.
 *
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   serdes        SERDES selection mask. Only a single SERDES may be selected. \ref adi_apollo_serdes_12pack_select_e
 * \param[out]  cal_data      Array to store calibration data. Memory is allocated by the caller.
 * \param[in]   len           Number of bytes in cal_data (includes room for ending checksum.)
 *                            The cal_data array length can be retrieved from \ref adi_apollo_cfg_serdes_tx_cal_data_len_get()
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_serdes_tx_cal_data_get(adi_apollo_device_t* device, const uint16_t serdes, uint8_t cal_data[], uint32_t len);

/**
 * \brief Get size (in bytes) of ADC calibration data. Includes ending 32-bit checksum.
 *        All ADC instances have the same cal data length.
 *
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   mode          ADC mode selection. Random(0) or Sequential(1).
 * \param[out]  len           Number of bytes in cal_data (includes ending 32-bit checksum)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_adc_cal_data_len_get(adi_apollo_device_t* device, uint8_t mode, uint32_t* len);

/**
 * \brief Get size (in bytes) of DAC calibration data. Includes ending 32-bit checksum.
 *        All DAC instances have the same cal data length.
 *
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[out]  len           Number of bytes in cal_data (includes ending 32-bit checksum)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_dac_cal_data_len_get(adi_apollo_device_t* device, uint32_t* len);

/**
 * \brief Get size (in bytes) of SERDES Rx calibration data. Includes ending 32-bit checksum.
 *        All SERDES RX instances have the same cal data length.
 *
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[out]  len           Number of bytes in cal_data (includes ending 32-bit checksum)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_serdes_rx_cal_data_len_get(adi_apollo_device_t* device, uint32_t* len);

/**
 * \brief Get size (in bytes) of SERDES Tx calibration data. Includes ending 32-bit checksum.
 *        All SERDES Tx instances have the same cal data length.
 *
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[out]  len           Number of bytes in cal_data (includes ending 32-bit checksum)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_serdes_tx_cal_data_len_get(adi_apollo_device_t* device, uint32_t* len);

/**
 * \brief  Get API profile type version
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 * \param[out] ver      Pointer to struct to store the version info(major, minor and patch) \ref adi_apollo_profile_version_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_profile_type_version_get(adi_apollo_device_t *device, adi_apollo_profile_version_t *ver);

/**
 * \brief  Configures the number of time bridging cal run for Rx SERDES (JRx) after init cal
 * \note   Call this API after loading FW bins and before loading device profile onto Apollo.
 *
 * \param[in] device                Context variable - Pointer to the APOLLO device data structure
 * \param[in] bridging_cal_runs     Num times bridging cal runs after init cal. Default 3. Range 0 - 3.
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cfg_serdes_rx_bridging_cal_cfg_set(adi_apollo_device_t *device, uint8_t bridging_cal_runs);


#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_CFG_H__ */
/*! @} */

