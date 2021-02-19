/*!
 * @brief     API header file
 *            This file contains all the publicly exposed methods and data
 *            structures to interface with API.
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup ADI_AD9083
 * @{
 */
#ifndef __ADI_AD9083__
#define __ADI_AD9083__


/*============= G R O U P S ============*/
// Define groups to section APIs
/*!

* @defgroup dev_init    1.0 Device Initialization & Clock Configuration
* @brief    The initialize API functions are used to retrieve the API revision
*           for information, perform a soft reset initialize the device structure,
*           and set up all the necessary internal clocks of the chip based on the
*           desired mode of operation. These are system high level APIs that can 
*           be called alone to set up the part for the init and clock blocks without
*           needing to use any lower-level API functions
*
* @defgroup dev_config    1.1 Device Init/Deinit and Hardware Platform API
* @ingroup  dev_init
* @brief    These API functions are mid-level hardware init/deinit API calls that can be   
*           implemented as desired to get additional chip and platform information.
*
* @defgroup clock_config    1.2 Block Level Clock API
* @ingroup  dev_init
* @brief    These API functions are mid-level clock config block API calls underneath 
*           the top system high level clock configuration API function call listed in Section 1.0. 
*           These API methods typically are not necessary to call directly unless specific 
*           modification is needed by the user.
*
* @defgroup vco_adc_config    2.0 VCO ADC Configuration
* @brief    This VCO ADC API function is used to set up all of the necessary settings for
*           the VCO ADC block configuration. This is a system high level API function that
*           can be called alone and does not require any additional API calls directly to set up 
*           the VCO ADC functions and features.  All lower level calculations and setup needed
*           are covered by the mid-level VCO ADC config block helper APIs listed in Section 2.1.
*
* @defgroup vco_adc_helper    2.1 VCO ADC Block Helper APIs
* @brief    These API functions are mid-level VCO ADC config block API calls underneath
*           the top system high level VCO ADC configuration API function call listed in Section 2.0.
*           These API methods typically are not necessary to call directly unless specific 
*           modification is needed by the user.
*
* @defgroup adc_dp_config    3.0 ADC Rx Digital Datapath Configuration
* @brief    This ADC Rx Digital Datapath API function is used to set up all of the necessary settings for
*           the ADC Rx digital datapath blocks configuration. This is a system high level API function that
*           can be called alone and does not require any additional API calls directly to set up 
*           the ADC digital datapath functions and features.  All lower level calculations and setup needed
*           are covered by the mid-level VCO ADC config block helper APIs listed in Section 3.1.
*
* @defgroup adc_dp_helper    3.1 ADC Rx Digital Datapath Helper APIs
* @brief    These API functions are mid-level ADC Rx digital datapath config block API calls
*           underneath the top system high level ADC Rx digital datapath configuration API function
*           call listed in Section 3.0. These API methods typically are not necessary to call 
*           directly unless specific modification is needed by the user.
*
* @defgroup adc_link_setup    4.0 SERDES Link Establishment & Monitoring
* @brief    This SERDES Link Establishment API function is used to set up all of the necessary settings for
*           the ADC SERDES blocks configuration. This is a system high level API function that
*           can be called alone and does not require any additional API calls directly to set up 
*           the SERDES link functions and features.  All lower level calculations and setup needed
*           are covered by the mid-level SERDES config block helper APIs listed in Section 4.1.
*
* @defgroup adc_serdes_helper    4.1 SERDES Helper APIs
* @brief    These API functions are mid-level SERDES link config block API calls underneath 
*           the top system high level SERDES link configuration API function call listed in 
*           Section 4.0. These API methods typically are not necessary to call directly unless 
*           specific modification is needed by the user.
****
*
* @defgroup appendix    Appendix: Additional Digital Feature Blocks
* @brief    This Appendix section includes additional API function calls for other features
*           and functional blocks that exist in the ADC that are not covered in previous sections.
*
* @defgroup appdx_tm    A1.0 Test Modes
* @ingroup  appendix
* @brief    These API methods cover various test modes inside the ADC. These functions are not
*           required to be called in order to fully set up the ADC chip in a working state.
*
* @defgroup appdx_tempsense    A2.0 Temperature Sensor
* @ingroup  appendix
* @brief    These API methods set up and configure the on-chip temperature sensor.
*
* @defgroup appdx_spi       A3.0  SPI Controls
* @ingroup  appendix
* @brief    These API methods set up and configure lower-level SPI calls not directly needed to 
*           configure the setup of the ADC.
*/

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"

/*============= D E F I N E S ==============*/
#define R1
#define AD9083_ID                     0x9083
#define AD9083_REF_CLK_FREQ_HZ_MIN    50000000ull     /*!< 50 MHz */
#define AD9083_REF_CLK_FREQ_HZ_MAX    500000000ull    /*!< 500MHz */
#define AD9083_ADC_CLK_FREQ_HZ_MIN    1000000000ull   /*!< 1  GHz */
#define AD9083_ADC_CLK_FREQ_HZ_MAX    2000000000ull   /*!< 2  GHz */

#define AD9083_JESD_SER_COUNT         4

/*!
 * @brief Enumerates Link select
 */
typedef enum {
    AD9083_SOFT_RESET = 0,                            /*!< Soft reset */
    AD9083_HARD_RESET = 1,                            /*!< Hard reset */
    AD9083_SOFT_RESET_AND_INIT = 2,                   /*!< Soft reset then init */
    AD9083_HARD_RESET_AND_INIT = 3                    /*!< Hard reset then init */
}adi_ad9083_reset_e;

/*!
 * @brief Enumerates Low Power Mode
 */
typedef enum {
    AD9083_POWER_ON = 0,                              /*!< Full on */
    AD9083_STANDBY = 1,                               /*!< Some of blocks are keep running */
    AD9083_FULL_POWER_DOWN = 2,                       /*!< Almost all of blocks are held in power down */
}adi_ad9083_low_power_mode_e;

/*!
 * @brief Enumerates ADC Power Down Mode
 */
typedef enum {
    AD9083_ADC_PD_DISABLE = 0,                        /*!< Power down pin disabled */
    AD9083_ADC_PD_CLK = 1,                            /*!< Power down pin disables clock path only */
    AD9083_ADC_PD_CLK_VTI = 2,                        /*!< Power down pin disables adc clock and vti bias */
    AD9083_ADC_PD_CLK_VTI_MASTER_BIAS = 3,            /*!< Power down pin disables adc clock, vti bias and adc masterbias block */
}adi_ad9083_adc_pd_pin_mode_e;

/*!
 * @brief Enumerates Termination Resistance
 */
typedef enum {
    AD9083_ADC_TERM_RES_OPEN = 0,                     /*!< Open */
    AD9083_ADC_TERM_RES_200 = 1,                      /*!< 200 Ohm */
    AD9083_ADC_TERM_RES_100 = 2,                      /*!< 100 Ohm */
}adi_ad9083_adc_term_res_e;

/*!
 * @brief Enumerates CIC Decimation Rate
 */
typedef enum {
    AD9083_CIC_DEC_4 = 0,                             /*!< Decimation by 4 */
    AD9083_CIC_DEC_8 = 1,                             /*!< Decimation by 8 */
    AD9083_CIC_DEC_16 = 2,                            /*!< Decimation by 16 */
}adi_ad9083_cic_dec_rate_e;

/*!
 * @brief Enumerates J Decimation Rate
 */
typedef enum {
    AD9083_J_DEC_1 = 0,                               /*!< Bypass */
    AD9083_J_DEC_4 = 1,                               /*!< Decimation by 4 */
    AD9083_J_DEC_8 = 2,                               /*!< Decimation by 8 */
    AD9083_J_DEC_16 = 3,                              /*!< Decimation by 16 */
    AD9083_J_DEC_12 = 6,                              /*!< Decimation by 12 */
    AD9083_J_DEC_24 = 7,                              /*!< Decimation by 24 */
    AD9083_J_DEC_10 = 9,                              /*!< Decimation by 10 */
    AD9083_J_DEC_20 = 10,                             /*!< Decimation by 20 */
    AD9083_J_DEC_30 = 14,                             /*!< Decimation by 30 */
    AD9083_J_DEC_40 = 11,                             /*!< Decimation by 40 */
    AD9083_J_DEC_60 = 15,                             /*!< Decimation by 60 */
}adi_ad9083_j_dec_rate_e;

/*!
 * @brief Enumerates H Decimation Rate
 */
typedef enum {
    AD9083_H_DEC_1  = 1,                              /*!< Decimation by 1  */
    AD9083_H_DEC_12 = 12,                             /*!< Decimation by 12 */
    AD9083_H_DEC_14 = 14,                             /*!< Decimation by 14 */
    AD9083_H_DEC_16 = 16,                             /*!< Decimation by 16 */
    AD9083_H_DEC_18 = 18,                             /*!< Decimation by 18 */
    AD9083_H_DEC_24 = 24,                             /*!< Decimation by 24 */
    AD9083_H_DEC_28 = 28,                             /*!< Decimation by 28 */
    AD9083_H_DEC_32 = 32,                             /*!< Decimation by 32 */
    AD9083_H_DEC_36 = 36,                             /*!< Decimation by 36 */
}adi_ad9083_h_dec_rate_e;

/*!
 * @brief Enumerates G Decimation Rate
 */
typedef enum {
    AD9083_G_DEC_NA = 0,                              /*!< NA */
    AD9083_G_DEC_8  = 8,                              /*!< Decimation by 8  */
    AD9083_G_DEC_16 = 16,                             /*!< Decimation by 16 */
}adi_ad9083_g_dec_rate_e;

/*!
 * @brief Enumerates JESD Rx Test Data Source (Inject Point)
 */
typedef enum {
    AD9083_JESD_RX_PRBS_TEST_DATA_SRC_LANE = 0x0,     /*!< Lane Data As Test Data Source */
    AD9083_JESD_RX_PRBS_TEST_DATA_SRC_SAMPLE = 0x1    /*!< Sample Data As Test Data Source (M0 Only) */
}adi_ad9083_jesd_rx_prbs_test_data_src_e;

/*!
 * @brief Enumerates JESD Rx PRBS Test Mode
 */
typedef enum {
    AD9083_JESD_RX_PRBS_TEST_MODE_OFF = 0x0,          /*!< Disable PRBS Test Mode */
    AD9083_JESD_RX_PRBS_TEST_MODE_PRBS7 = 0x1,        /*!< PRBS7 */
    AD9083_JESD_RX_PRBS_TEST_MODE_PRBS9 = 0x2,        /*!< PRBS9 */
    AD9083_JESD_RX_PRBS_TEST_MODE_PRBS15 = 0x3,       /*!< PRBS15 */
    AD9083_JESD_RX_PRBS_TEST_MODE_PRBS31 = 0x4        /*!< PRBS31 */
}adi_ad9083_jesd_rx_prbs_test_mode_e;

/*!
 * @brief Enumerates JESD Tx Test Data Source (Inject Point)
 */
typedef enum {
    AD9083_JESD_TX_TEST_DATA_SAMPLE = 0x0,            /*!< Sample Data As Test Data Source */
    AD9083_JESD_TX_TEST_DATA_PHY = 0x1,               /*!< PHY Data As Test Data Source */
    AD9083_JESD_TX_TEST_DATA_SCRAMBLER_INPUT = 0x2    /*!< Scrambler Input Data As Data Source */
}adi_ad9083_jesd_tx_test_data_src_e;

/*!
 * @brief Enumerates JESD Tx Test Mode
 */
typedef enum {
    AD9083_JESD_TX_TEST_MODE_DISABLED = 0x0,          /*!< Disable Test Mode */
    AD9083_JESD_TX_TEST_MODE_CHECKER_BOARD = 0x1,     /*!< Checker Board Test Mode */
    AD9083_JESD_TX_TEST_MODE_WORD_TOGGLE = 0x2,       /*!< Word Toggle Test Mode */
    AD9083_JESD_TX_TEST_MODE_PN31 = 0x3,              /*!< PN31 Test Mode */
    AD9083_JESD_TX_TEST_MODE_PN15 = 0x5,              /*!< PN15 Test Mode */
    AD9083_JESD_TX_TEST_MODE_PN7 = 0x7,               /*!< PN7  Test Mode */
    AD9083_JESD_TX_TEST_MODE_RAMP = 0x8,              /*!< Ramp Test Mode */
    AD9083_JESD_TX_TEST_MODE_USER_REPEAT = 0xE,       /*!< Repeated User Data Test Mode */
    AD9083_JESD_TX_TEST_MODE_USER_SINGLE = 0xF        /*!< Single Time User Data Test Mode */
}adi_ad9083_jesd_tx_test_mode_e;

/*!
 * @brief Tmode resolution I/Q channel
 */
typedef enum {
    AD9083_RX_TMODE_RES16 = 0,                        /*!< Resolution 16-bit */
    AD9083_RX_TMODE_RES15 = 1,                        /*!< Resolution 15-bit */
    AD9083_RX_TMODE_RES14 = 2,                        /*!< Resolution 14-bit */
    AD9083_RX_TMODE_RES13 = 3,                        /*!< Resolution 13-bit */
    AD9083_RX_TMODE_RES12 = 4                         /*!< Resolution 12-bit */
}adi_ad9083_rx_tmode_resolution_e;

/*!
 * @brief Datapath Mode
 */
typedef enum {
    AD9083_DATAPATH_ADC_CIC         = 1,              /*!< ADC -> CIC -> output */
    AD9083_DATAPATH_ADC_CIC_NCO_J   = 2,              /*!< ADC -> CIC -> NCO -> J -> output */
    AD9083_DATAPATH_ADC_CIC_J       = 3,              /*!< ADC -> CIC -> J -> output */
    AD9083_DATAPATH_ADC_J           = 4,              /*!< ADC -> J -> output */
    AD9083_DATAPATH_ADC_CIC_NCO_G   = 5,              /*!< ADC -> CIC -> NCO -> G -> output */
    AD9083_DATAPATH_ADC_CIC_NCO_G_H = 6               /*!< ADC -> CIC -> NCO -> G -> H output */
}adi_ad9083_datapath_mode_e;

/*!
 * @brief Enumerates JESD Serializer Swing Settings
 */
typedef enum {
    AD9083_SER_SWING_1000 = 0,                        /*!< 1000 mV Swing */
    AD9083_SER_SWING_850 = 1,                         /*!< 850 mV Swing */
    AD9083_SER_SWING_750 = 2,                         /*!< 750 mV Swing */
    AD9083_SER_SWING_500 = 3                          /*!< 500 mV Swing */
}adi_ad9083_ser_swing_e;

/*!
 * @brief Enumerates JESD Serializer Pre-Emphasis Settings
 */
typedef enum {
    AD9083_SER_PRE_EMP_0DB = 0,                       /*!< 0 db Pre-Emphasis */
    AD9083_SER_PRE_EMP_3DB = 1,                       /*!< 3 db Pre-Emphasis */
    AD9083_SER_PRE_EMP_6DB = 2                        /*!< 6 db Pre-Emphasis */
}adi_ad9083_ser_pre_emp_e;

/*!
 * @brief Enumerates JESD Serializer Post-Emphasis Settings
 */
typedef enum {
    AD9083_SER_POST_EMP_0DB = 0,                      /*!< 0 db Post-Emphasis */
    AD9083_SER_POST_EMP_3DB = 1,                      /*!< 3 db Post-Emphasis */
    AD9083_SER_POST_EMP_6DB = 2,                      /*!< 6 db Post-Emphasis */
    AD9083_SER_POST_EMP_9DB = 3,                      /*!< 9 db Post-Emphasis */
    AD9083_SER_POST_EMP_12DB = 4                      /*!< 12 db Post-Emphasis */
}adi_ad9083_ser_post_emp_e;

/*!
 * @brief Per lane JESD Serializer Settings
 */
typedef struct {
    adi_ad9083_ser_swing_e    swing_setting;
    adi_ad9083_ser_pre_emp_e  pre_emp_setting;
    adi_ad9083_ser_post_emp_e post_emp_setting;
}adi_ad9083_indv_ser_settings_t;

/*!
 * @brief Full JESD Serializer Settings Structure
 */
typedef struct {
    adi_ad9083_indv_ser_settings_t indv_ser_lane_settings[4];
    uint8_t                      tx_invert_mask;
}adi_ad9083_ser_settings_t;

/*!
 * @brief Results from the JESD PRBS Test
 */
typedef struct {
    uint32_t phy_prbs_err_cnt;                       /*!< PRBS Test Error Count */
    uint8_t  phy_prbs_pass;                          /*!< PRBS Test Status */
    uint8_t  phy_src_err_cnt;                        /*!< PRBS Test Source Error Count */
}adi_ad9083_prbs_test_t;

/*!
 * @brief Device hardware abstract layer (HAL) structure
 */
typedef struct {
    void *                    user_data;
    adi_cms_spi_sdo_config_e  sdo;                   /*!< SPI interface 3/4 wire mode configuration */
    adi_cms_spi_msb_config_e  msb;                   /*!< SPI interface MSB/LSB bit order configuration */
    adi_cms_spi_addr_inc_e    addr_inc;              /*!< SPI interface address increment configuration */
    adi_cms_signal_type_e     syncoutb;              /*!< Desired Signal type for SYNCOUTB signal */
    adi_cms_signal_coupling_e sysref;                /*!< Desired Input coupling for SysRef signal */

    adi_spi_xfer_t            spi_xfer;              /*!< Function Pointer to HAL SPI access function */
    adi_delay_us_t            delay_us;              /*!< Function Pointer to HAL delay function */
    adi_hw_open_t             hw_open;               /*!< Function Pointer to HAL initialization function */
    adi_hw_close_t            hw_close;              /*!< Function Pointer to HAL De-initialization function */
    adi_log_write_t           log_write;             /*!< Function Pointer to HAL log write function */
    adi_tx_en_pin_ctrl_t      tx_en_pin_ctrl;        /*!< Function Pointer to HAL TX_ENABLE Pin Control function */
    adi_reset_pin_ctrl_t      reset_pin_ctrl;        /*!< Function Pointer to HAL RESETB Pin Control Function */
}adi_ad9083_hal_t;

/*!
 * @brief Device internal information structure
 */
typedef struct {
    uint32_t dev_freq_hz;                            /*!< Device Clock Input frequency in KHz. Valid range 50MHz to 1GHz */
    uint32_t adc_freq_hz;                            /*!< ADC Clock Frequency in KHz. Valid range 1GHz to 2GHz */
}adi_ad9083_info_t;

/*!
 * @brief Device structure
 */
typedef struct {
    adi_ad9083_hal_t  hal_info;
    adi_ad9083_info_t dev_info;
}adi_ad9083_device_t;

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/*===== 1 . 0   D E V I C E   I N I T   &  C L O C K I N G =====*/
/**
 * @ingroup dev_init
 * @brief  System Top Level API. \n Get API revision
 *
 * @param  device             Pointer to the device structure.
 * @param  rev_major          Pointer to variable to store the major revision
 * @param  rev_minor          Pointer to variable to store the minor revision
 * @param  rev_rc             Pointer to variable to store the rc    revision
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_api_revision_get(adi_ad9083_device_t *device, uint8_t *rev_major, uint8_t *rev_minor, uint8_t *rev_rc);

/**
 * @ingroup dev_init
 * @brief  System Top Level API. \n Reset device
 *         Issues a hard reset or soft reset of the device.
 *         Performs a full reset of device via the hardware pin (hard) or
 *         via the SPI register (soft).
 *         Resetting all SPI registers to default and triggering the required
 *         initialization sequence.
 *
 * @param  device             Pointer to the device structure
 * @param  operation          A parameter to indicate reset operation. @see adi_ad9083_reset_e
 *                            AD9083_SOFT_RESET indicates a software reset is required.
 *                            AD9083_HARD_RESET indicates a hardware reset is required.
 *                            AD9083_SOFT_RESET_AND_INIT indicates a software reset + initialization is required.
 *                            AD9083_HARD_RESET_AND_INIT indicates a hardware reset + initialization is required.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_reset(adi_ad9083_device_t *device, adi_ad9083_reset_e operation);

/**
 * @ingroup dev_init
 * @brief  System Top Level API. \n Initialize ad9083 device
 *
 * @param  device             Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. Check adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_init(adi_ad9083_device_t *device);

/**
 * @ingroup dev_init
 * @brief  System Top Level API. \n Configure the clock circuitry based on the desired clocks frequencies
 *
 * @param  device             Pointer to the device structure
 * @param  adc_clk_hz         Desired adc clock frequency
 * @param  ref_clk_hz         Reference clock frequency, if ref_clk_hz >= AD9083_REF_CLK_FREQ_HZ_MAX, bypass PLL
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_clock_config_set(adi_ad9083_device_t *device, uint64_t adc_clk_hz, uint64_t ref_clk_hz);







/*===== 1 . 1   D E V I C E   I N I T / D E I N I T  &  H W  P L A T F O R M =====*/
/**
 * @ingroup dev_config
 * @brief  Get chip identification data
 *         Read-back product type, identification and revision data
 *
 * @param  device             Pointer to the device structure.
 * @param  chip_id            Pointer to a variable of type adi_cms_chip_id_t to
 *                            which the product data read-back from chip shall be stored.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_chip_id_get(adi_ad9083_device_t *device, adi_cms_chip_id_t *chip_id);

/**
 * @ingroup dev_config
 * @brief  Get DIE ID
 *
 * @param  device             Pointer to the device structure.
 * @param  id                 Pointer to silicon DIE id.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_die_id_get(adi_ad9083_device_t *device, uint8_t *id);

/**
 * @ingroup dev_config
 * @brief  Open hardware platform
 *
 * @param  device             Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. Check adi_cms_error_e for details.
 */
 int32_t adi_ad9083_device_hw_open(adi_ad9083_device_t *device);

 /**
 * @ingroup dev_config
 * @brief  Close hardware platform
 *
 * @param  device             Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. Check adi_cms_error_e for details.
 */
 int32_t adi_ad9083_device_hw_close(adi_ad9083_device_t *device);

/**
 * @ingroup dev_config
 * @brief  De-initialize device.
 *
 * @param  device             Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. Check adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_deinit(adi_ad9083_device_t *device);

/**
 * @ingroup dev_config
 * @brief  Perform SPI interface configuration
 *
 * @param  device             Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_spi_config(adi_ad9083_device_t *device);

/**
 * @ingroup dev_config
 * @brief  Perform SPI register write access to device
 *
 * @param  device             Pointer to the device structure
 * @param  addr               SPI address to which the value of data parameter shall be written
 * @param  data               8-bit value to be written to SPI register defined by the address parameter.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_spi_register_set(adi_ad9083_device_t *device, uint16_t addr, uint8_t data);

/**
 * @ingroup dev_config
 * @brief  Perform SPI register read access to device
 *
 *
 * @param  device             Pointer to the device structure
 * @param  addr               SPI address from which the value of data parameter shall be read,
 * @param  data               Pointer to an 8-bit variable to which the value of the
 *                            SPI register at the address defined by address parameter shall be stored.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_spi_register_get(adi_ad9083_device_t *device, uint16_t addr, uint8_t *data);

/**
 * @ingroup dev_config
 * @brief  Check part power up status, if power on failed, write Msg to log file
 *
 * @param  device             Pointer to the device structure
 * @param  
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_power_status_check(adi_ad9083_device_t *device);








/*===== 1 . 2   B L O C K  L E V E L  C L O C K  A P I =====*/
/**
 * @ingroup clock_config
 * @brief  Get pll lock status
 *
 * @param  device             Pointer to the device structure
 * @param  status             Pointer to pll lock fast (bit1) and slow status (bit0)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_pll_lock_status_get(adi_ad9083_device_t *device, uint8_t *status);

/**
 * @ingroup clock_config
 * @brief  Enable PLL test output
 *
 * @param  device             Pointer to the device structure
 * @param  enable             Enable PLL test output, 0 - No action, 1 - enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_pll_test_enable_set(adi_ad9083_device_t *device, uint8_t enable);







/*===== 2 . 0   V C O  A D C  C O N F I G U R A T I O N =====*/
/**
 * @ingroup vco_adc_config
 * @brief  System Top Level API. \n Configure the VCO ADC settings.
 *
 * @param  device             Pointer to the device structure
 * @param  fc                 LPF -3dB frequency
 * @param  vmax               diff peak-to-peak input full-scale
 * @param  rterm              100ohm, 200ohm or open
 * @param  en_hp              1 - enable the replica path in the vcoadc
 * @param  backoff            the backoff in dB in terms of noise
 * @param  finmax             max input frequency
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_adc_config_set(adi_ad9083_device_t *device, uint32_t vmax, uint32_t fc, uint8_t rterm, uint8_t en_hp, uint32_t backoff, uint64_t finmax);







/*===== 2 . 1   V C O  A D C  H E L P E R  A P I S =====*/
/**
 * @ingroup vco_adc_helper
 * @brief  config VTI
 *
 * @param  device             Pointer to the device structure
 * @param  fc                 LPF -3dB frequency
 * @param  vmax               diff peak-to-peak input full-scale
 * @param  en_hp              1 - enable the replica path in the vcoadc
 * @param  orc1mode           denotes if orc1 method is enabled
 * @param  backoff            the backoff in dB in terms of noise
 * @param  finmax             max input frequency
 * @param  kgain              output value pointer
 * @param  bcenter_os         output value pointer
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_adc_vti_set(adi_ad9083_device_t *device, uint32_t fc, uint32_t vmax, uint8_t en_hp, 
    uint32_t orc1mode, uint32_t backoff, uint64_t finmax, uint32_t *kgain, uint8_t *bcenter_os);

/**
 * @ingroup vco_adc_helper
 * @brief  Calibrate ADC
 *
 * @param  device             Pointer to the device structure
 * @param  gcenter            Ring oscillator center current
 * @param  kgain              register KGAIN_VAL value
 * @param  bcenter_os         register BCENTER_OFFSET value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_adc_cal_set(adi_ad9083_device_t *device, uint16_t gcenter, uint32_t kgain, uint8_t bcenter_os);







/*===== 3 . 0   A D C  R X  D A T A P A T H  C O N F I G U R A T I O N =====*/
/**
 * @ingroup adc_dp_config
 * @brief  System Top Level API. \n Config the Rx Digital Datapath
 *
 * @param  device             Pointer to the device structure
 * @param  mode               Refer to Enum adi_ad9083_datapath_mode_e
 * @param  dec                decimation, including cic, j, g, h
 * @param  nco_freq_hz        nco shift, unit: hz
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_datapath_config_set(adi_ad9083_device_t *device, adi_ad9083_datapath_mode_e mode, uint8_t dec[4], uint64_t nco_freq_hz[3]);







/*===== 3 . 1   A D C  R X  D A T A P A T H  H E L P E R  A P I S =====*/
/**
 * @ingroup adc_dp_helper
 * @brief  Enable/disable Datapath
 *
 * @param  device             Pointer to the device structure
 * @param  enable             0 - disable, 1 - enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_datapath_enable_set(adi_ad9083_device_t *device, uint8_t enable);

/**
 * @ingroup adc_dp_helper
 * @brief  Cal total decimation
 *
 * @param  device             Pointer to the device structure
 * @param  total_dec          Total decimation
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_datapath_total_dec_get(adi_ad9083_device_t *device, uint16_t *total_dec);

/**
 * @ingroup adc_dp_helper
 * @brief  Set ADC test mode resolution
 *
 * @param  device             Pointer to the device structure
 * @param  i_resolution       0 - 16-bit, 1 - 15-bit, 2 - 14-bit, 3 - 13-bit, 4 - 12-bit
 * @param  q_resolution       0 - 16-bit, 1 - 15-bit, 2 - 14-bit, 3 - 13-bit, 4 - 12-bit
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_test_mode_res_set(adi_ad9083_device_t *device, adi_ad9083_rx_tmode_resolution_e i_resolution, adi_ad9083_rx_tmode_resolution_e q_resolution);

/**
 * @ingroup adc_dp_helper
 * @brief  Set ADC test mode type
 *
 * @param  device             Pointer to the device structure
 * @param  i_type             Refer to Yoda
 * @param  q_type             Refer to Yoda
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_test_mode_type_set(adi_ad9083_device_t *device, uint8_t i_type, uint8_t q_type);

/**
 * @ingroup adc_dp_helper
 * @brief  Set ADC test mode selection
 *
 * @param  device             Pointer to the device structure
 * @param  enable             Pattern
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_test_mode_sel_all_set(adi_ad9083_device_t *device, uint8_t enable);

/**
 * @ingroup adc_dp_helper
 * @brief  Enable/disable ADC test mode
 *
 * @param  device             Pointer to the device structure
 * @param  enable             0 - Disable, 1 - Enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_test_mode_enable_set(adi_ad9083_device_t *device, uint8_t enable);

/**
 * @ingroup adc_dp_helper
 * @brief  Configure NCO FTW word
 *
 * @param  device             Pointer to the device structure
 * @param  index              NCO # to program
 * @param  ftw              NCO FTW value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_nco_ftw_set (adi_ad9083_device_t *device, uint8_t index, uint8_t ftw);

/**
 * @ingroup adc_dp_helper
 * @brief  Select low power mode
 *
 * @param  device             Pointer to the device structure
 * @param  mode               @see adi_ad9083_low_power_mode_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_low_power_mode_set(adi_ad9083_device_t *device, adi_ad9083_low_power_mode_e mode);

/**
 * @ingroup adc_dp_helper
 * @brief  Read back the current ADC clock frequency
 *
 * @param  device             Pointer to the device structure
 * @param  adc_clk_hz         Pointer to variable to store the current clk frequency in Hz
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_adc_clk_freq_get(adi_ad9083_device_t *device, uint64_t *adc_clk_hz);










/*===== 4 . 0   S E R D E S  L I N K  =====*/
/**
 * @ingroup adc_link_setup
 * @brief  System Top Level API. \n Startup and configure the ADC SERDES link.
 *
 * @param  device             Pointer to the device structure
 * @param  jtx_param          JTx parameter pointer
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jtx_startup(adi_ad9083_device_t *device, adi_cms_jesd_param_t *jtx_param);








/*===== 4 . 1   S E R D E S  H E L P E R  A P I S  =====*/
/**
 * @ingroup adc_serdes_helper
 * @brief  Get jtx link status
 *
 * @param  device             Pointer to the device structure
 * @param  status             Pointer to jtx status
 *                                bit[3:0] - QBF status
 *                                bit4     - frame sync status
 *                                bit5     - jtx pll locked
 *                                bit6     - phase established
 *                                bit7     - jtx invalid mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_link_status_get(adi_ad9083_device_t *device, uint16_t *status);

/**
 * @ingroup adc_serdes_helper
 * @brief  Get jtx pll lock status
 *
 * @param  device             Pointer to the device structure
 * @param  pll_locked         Pointer to jtx status, 0 - unlocked, 1 - locked
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_lcpll_status_get(adi_ad9083_device_t *device, uint8_t *pll_locked);

/**
 * @ingroup adc_serdes_helper
 * @brief  Configure the JTx link settings
 *
 * @param  device             Pointer to the device structure
 * @param  jesd_param         @see adi_cms_jesd_param_t
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_link_config_set(adi_ad9083_device_t *device, adi_cms_jesd_param_t *jesd_param);

/**
 * @ingroup adc_serdes_helper
 * @brief  Configure the JESD Tx lane cross bar between physical lane and logical lane
 *
 * @param  device             Pointer to the device structure
 * @param  logical_lane       Logical lane index (0~3)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_lanes_xbar_set(adi_ad9083_device_t *device, uint8_t logic_lanes[4]);

/**
 * @ingroup adc_serdes_helper
 * @brief  Enable lanes
 *
 * @param  device             Pointer to the device structure
 * @param  lane_num           Lane number, from 0
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_lanes_enable_set(adi_ad9083_device_t *device, uint8_t lane_num);

/**
 * @ingroup adc_serdes_helper
 * @brief  Enable jtx phy
 *
 * @param  device             Pointer to the device structure
 * @param  lane_num           Lane number, from 0
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_phy_enable_set(adi_ad9083_device_t *device, uint8_t lane_num);

/**
 * @ingroup adc_serdes_helper
 * @brief  Set lane ID
 *
 * @param  device             Pointer to the device structure
 * @param  lid                Lane ID
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_lane_id_set(adi_ad9083_device_t *device, uint8_t lid[4]);

/**
 * @ingroup adc_serdes_helper
 * @brief  Disable all lanes
 *
 * @param  device             Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_lanes_disable_all_set(adi_ad9083_device_t *device);












/*===== A 1 . 0   T E S T  M O D E S   =====*/
/**
 * @ingroup appdx_tm
 * @brief  Run PRBS Test for JESD Transmitter (phy has prbs data)
 *
 * @param device              Pointer to the device reference handle.
 * @param prbs_pattern        PRBS pattern identifier, @see adi_cms_jesd_prbs_pattern_e
 *                            PRBS7, PRBS15, PRBS31
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_phy_prbs_test(adi_ad9083_device_t *device, adi_cms_jesd_prbs_pattern_e prbs_pattern);

/**
 * @ingroup appdx_tm
 * @brief  Enable Continuous D21.5 Test Mode
 *
 * @param device              Pointer to the device reference handle
 * @param lane_id             Lane index (0~3)
 * @param enable              Test mode enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_jesd_tx_continuous_d215_enable_set(adi_ad9083_device_t *device, uint8_t lane_id, uint8_t enable);

/**
 * @ingroup appdx_tm
 * @brief  Set ADC test mode pattern
 *
 * @param  device             Pointer to the device structure
 * @param  i_pattern          Pattern
 * @param  q_pattern          Pattern
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_rx_test_mode_usr_pattern_set(adi_ad9083_device_t *device, uint16_t i_pattern[8], uint16_t q_pattern[8]);









/*===== A 2 . 0   T E M P E R A T U R E  S E N S O R   =====*/
/**
 * @ingroup appdx_tempsense
 * @brief  Enable/disable temp sensor
 *
 * @param  device             Pointer to the device structure
 * @param  enable             0 - Disable, 1 - Enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_temp_sensor_enable_set(adi_ad9083_device_t *device, uint8_t enable);

/**
 * @ingroup appdx_tempsense
 * @brief  Select temp sensor mode
 *
 * @param  device             Pointer to the device structure
 * @param  sel                0 - Default, 1 - 1x diode, 2 - 20x diode, 3 - GND
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_temp_sensor_sel_set(adi_ad9083_device_t *device, uint8_t sel);

/**
 * @ingroup appdx_tempsense
 * @brief  Select temp sensor mode
 *
 * @param  device             Pointer to the device structure
 * @param  vbe_1              vbe_1 must be in unit mV
 * @param  vbe_2              vbe_2 must be in unit mV
 * @param  temp               unit: mili-degree
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_temp_get(adi_ad9083_device_t *device, int32_t vbe_1, int32_t vbe_2, int32_t *temp);








/*===== A 3 . 0   S P I  C O N T R O L S   =====*/
/**
 * @ingroup appdx_spi
 * @brief  Perform SPI write access to device PLL cbus registers
 *
 * @param  device             Pointer to the device structure
 * @param  addr               SPI address to which the value of data parameter shall be written
 * @param  data               8-bit value to be written to SPI register defined by the address parameter.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_cbuspll_register_set(adi_ad9083_device_t *device, uint8_t addr, uint8_t  data);

/**
 * @ingroup appdx_spi
 * @brief  Perform SPI read access to device PLL cbus registers
 *
 *
 * @param  device             Pointer to the device structure
 * @param  addr               SPI address from which the value of data parameter shall be read,
 * @param  data               Pointer to an 8-bit variable to which the value of the
 *                            SPI register at the address defined by address parameter shall be stored.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9083_device_cbuspll_register_get(adi_ad9083_device_t *device, uint8_t addr, uint8_t* data);
















/*===== x . x         =====*/
























#ifdef __cplusplus
}
#endif

#endif /* __ADI_AD9083_H__ */
/*! @} */