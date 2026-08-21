/*!
 * \brief     API SPI and HSCI device header file
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
 
#ifndef __ADI_APOLLO_DEVICE_TYPES_H__
#define __ADI_APOLLO_DEVICE_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"

#define ADI_APOLLO_UUID_NUM_BYTES     16

/*!
 * @brief  Enumerates SW variant Tx/Rx/Mxfe
 */
typedef enum {
    ADI_APOLLO_DEVICE_SW_VARIANT_TX_ONLY = 0x0, /*!< TX only part */
    ADI_APOLLO_DEVICE_SW_VARIANT_RX_ONLY = 0x1, /*!< RX only part */
    ADI_APOLLO_DEVICE_SW_VARIANT_MXFE    = 0x2, /*!< MXFE part */
    ADI_APOLLO_DEVICE_SW_VARIANT_INVALID = 0xFF /*!< Invalid SW variant */
} adi_apollo_device_sw_variant_mxfe_e;

/*!
 * @brief  Enumerates SW variant Trim
 */
typedef enum {
    ADI_APOLLO_DEVICE_SW_VARIANT_TRIM_NOT_SET = 0x0, /*!< SW Trim is not set yet */
    ADI_APOLLO_DEVICE_SW_VARIANT_TRIM_SW1     = 0x1, /*!< This part is suitable for SW1, SW3, SW5 trim levels. */
    ADI_APOLLO_DEVICE_SW_VARIANT_TRIM_SW3     = 0x3, /*!< This part is suitable for SW3, SW5 trim levels. */
    ADI_APOLLO_DEVICE_SW_VARIANT_TRIM_SW5     = 0x5, /*!< This part is suitable only for SW5 trim levels. */
    ADI_APOLLO_DEVICE_SW_VARIANT_TRIM_INVALID = 0xFF /*!< Invalid SW Trim */
} adi_apollo_device_sw_variant_trim_e;

/*!
 * @brief  Enumerates Rev Trx Config
 */
typedef enum {
    ADI_APOLLO_DEVICE_REV_TRX_CONFIG_4T4R = 0x0, /*!< 4T4R */
    ADI_APOLLO_DEVICE_REV_TRX_CONFIG_8T8R = 0x1, /*!< 8T8R */
} adi_apollo_device_rev_trx_config_e;

/*!
 * @brief  Enumerates Rev RX Type
 */
typedef enum {
    ADI_APOLLO_DEVICE_REV_RX_DIFF = 0x0,  /*!< differential rx */
    ADI_APOLLO_DEVICE_REV_RX_SE   = 0x1,    /*!< single ended rx */
} adi_apollo_device_rev_rx_type_e;

/*!
 * @brief  board variant and rev info structure
 */
typedef struct {
    adi_apollo_device_sw_variant_mxfe_e mxfe;
    adi_apollo_device_sw_variant_trim_e sw_trim;
    adi_apollo_device_rev_trx_config_e  trx_cfg;
    adi_apollo_device_rev_rx_type_e     rx_type;
    uint8_t                             si_grade;
    uint8_t                             die_id;
} adi_apollo_device_board_var_and_rev_info_t;

/*!
 * @brief  SPI mode settings
 */
typedef enum {                  
    ADI_APOLLO_DEVICE_SPI_NONE = 0,                               /*!< Keep this for test */
    ADI_APOLLO_DEVICE_SPI_SDO = 1,                                /*!< SDO  active, 4-wire only */
    ADI_APOLLO_DEVICE_SPI_SDIO = 2,                               /*!< SDIO active, 3-wire only */
    ADI_APOLLO_DEVICE_SPI_CONFIG_MAX = 3                          /*!< Keep it last */
} adi_apollo_device_spi_sdo_config_e;

/*!
 * @brief  SPI bit order settings
 */
typedef enum {
    ADI_APOLLO_DEVICE_SPI_MSB_LAST  = 0,                          /*!< LSB first */
    ADI_APOLLO_DEVICE_SPI_MSB_FIRST = 1                           /*!< MSB first */
} adi_apollo_device_spi_msb_config_e;

/*!
 * @brief  SPI address increment settings
 */
typedef enum {
    ADI_APOLLO_DEVICE_SPI_ADDR_DEC_AUTO = 0,                      /*!< auto decremented */
    ADI_APOLLO_DEVICE_SPI_ADDR_INC_AUTO = 1                       /*!< auto incremented */
} adi_apollo_device_spi_addr_inc_e;

/*!
 * @brief  SPI config
 */
typedef struct
{
    adi_apollo_device_spi_sdo_config_e     sdo;        /*!< SPI interface 3/4 wire mode configuration */
    adi_apollo_device_spi_msb_config_e     msb;        /*!< SPI interface MSB/LSB bit order configuration */
    adi_apollo_device_spi_addr_inc_e       addr_inc;   /*!< SPI interface address increment configuration */
} adi_apollo_device_spi_settings_t;


/*!
 * @brief  HSCI address increment settings
 */
typedef enum {
    ADI_APOLLO_DEVICE_HSCI_ADDR_DEC_AUTO = 0,          /*!< auto decremented */
    ADI_APOLLO_DEVICE_HSCI_ADDR_INC_AUTO = 1           /*!< auto incremented */
} adi_apollo_device_hsci_addr_inc_e;

/*!
*\brief HSCI config
*/
typedef struct {
    adi_apollo_device_hsci_addr_inc_e  addr_inc;    /*!< HSCI interface address increment configuration */
    uint8_t                     loopback_en;        /*!< Enable/Disable HSCI Loopback mode */
    uint8_t                     auto_linkup_en;     /*!< Enable/Disable HSCI auto linkup mode */
} adi_apollo_device_hsci_settings_t;

/*!
* \brief Temperature readings from all temperature sensors
*/
typedef enum {
    ADI_APOLLO_DEVICE_TMU_SERDES_PLL = 1,
    ADI_APOLLO_DEVICE_TMU_MPU_A = 2,
    ADI_APOLLO_DEVICE_TMU_MPU_B = 3,
    ADI_APOLLO_DEVICE_TMU_ADC_A = 4,
    ADI_APOLLO_DEVICE_TMU_CLK_A = 5,
    ADI_APOLLO_DEVICE_TMU_ADC_B = 6,
    ADI_APOLLO_DEVICE_TMU_CLK_B = 7,
    ADI_APOLLO_DEVICE_TMU_CLK_C = 8
} adi_apollo_device_tmu_index_e;

/**
 *  \brief API function wrap for temperature data returned by CPU
 */
typedef struct {
    int16_t temp_degrees_celsius[9];     /*!< Temperature readings from all temperature sensors \ref adi_apollo_device_tmu_index_e for details.*/
    int16_t temp_degrees_celsius_avg;    /*!< Average temperature reading of temperature sensors specified in avgMask */
    uint16_t avg_mask;                   /*!< Bitmask indicating which temperature sensors are averaged in tempDegreesCelciusAvg - see \ref adi_apollo_mailbox_temp_sensor_mask_e */
    int16_t max_temp_degrees_celsius;    /*!< Max temperature reading of temperature sensors specified in avgMask */
    int16_t min_temp_degrees_celsius;    /*!< Min temperature reading of temperature sensors specified in avgMask */
} adi_apollo_device_tmu_data_t;

typedef struct {
    uint32_t    rx_lockout_mask;           /*!< rx device hw feature mask (1 means locked out) */
    uint32_t    tx_lockout_mask;           /*!< tx device hw feature mask (1 means locked out) */
} adi_apollo_device_lockout_t;

#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#endif /* __ADI_APOLLO_DEVICE_TYPES_H__ */
/*! @} */
