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
 * \addtogroup ADI_APOLLO_SNIFFER
 * @{
 */
 
#ifndef __ADI_APOLLO_SNIFFER_TYPES_H__
#define __ADI_APOLLO_SNIFFER_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_SNIFFER_FFT_LENGTH                512
#define ADI_APOLLO_SNIFFER_NUM                       2

/*!
* \brief Enumerates selection between gpio and regmap input
*/
typedef enum {
   ADI_APOLLO_SNIFFER_GPIO_INPUT        = 0x0,             /*!< Select GPIO Input */
   ADI_APOLLO_SNIFFER_REGMAP_INPUT      = 0x1              /*!< Select Regmap Input */
} adi_apollo_sniffer_gpio_regmap_select_e;

/*!
* \brief Enumerates sniffer for side A and B
*/
typedef enum {
   ADI_APOLLO_SNIFFER_A        = 0x1,             /*!< Select Sniffer Side A */
   ADI_APOLLO_SNIFFER_B        = 0x2,             /*!< Select Sniffer Side B */
   ADI_APOLLO_SNIFFER_ALL      = 0x3              /*!< Select Sniffer Side A&B (same for both 4T4R and 8T8R)*/
} adi_apollo_sniffer_select_e;

/*!
* \brief Enumerates Spectrum Sniffer Modes
*/
typedef enum {
   ADI_APOLLO_SNIFFER_NORMAL_MAGNITUDE   = 0x0,                 /*!< Normal Magnitude mode */
   ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE  = 0x1,                 /*!< Instantaneous/Debug Magnitude mode  */
   ADI_APOLLO_SNIFFER_NORMAL_IQ          = 0x2,                 /*!< Normal IQ mode */
   ADI_APOLLO_SNIFFER_INSTANT_IQ         = 0x3                  /*!< Instantaneous/Debug IQ mode */
} adi_apollo_sniffer_mode_e;

/*!
* \brief Spectrum Sniffer Init Parameters 
*/
typedef struct {
   uint8_t sniffer_enable;          /*!< 0: Disable spec Sniffer 1:Enable spec sniffer */
   uint8_t fft_hold_sel;            /*!< 0: Select via GPIO 1:Select via Regmap
                                         \ref adi_apollo_sniffer_gpio_regmap_select_e  */
   uint8_t fft_enable_sel;          /*!< 0: Select via GPIO 1:Select via Regmap
                                         \ref adi_apollo_sniffer_gpio_regmap_select_e */    
   uint8_t real_mode;               /*!< 0: Complex FFT, 1 : Real FFT
                                         \ref adi_apollo_sniffer_fft_type_e*/
   uint8_t max_threshold;           /*!< Maximum Threshold for Magnitude */
   uint8_t min_threshold;           /*!< Minimum Threshold for Magnitude */
} adi_apollo_sniffer_init_t;

/*!
* \brief Spectrum Sniffer Programming Parameters 
*/
typedef struct {
   uint8_t bottom_fft_enable;                   /*!< 1: Bottom FFT Enable, 0: Bottom FFT Disable */      
   uint8_t window_enable;                       /*!< 1: Windowing Enable, 0: Windowing Disable */ 
   uint8_t sort_enable;                         /*!< 1: Sorting Enable, 0: Sorting Disable */
   uint8_t low_power_enable;                    /*!< 1: low power Enable, 0: Normal mode */
   uint8_t dither_enable;                       /*!< 1: dither Enable, 0: dither Disable */
   uint8_t continuous_mode;                     /*!< 1: continuous mode, 0: single mode
                                                      \ref adi_apollo_sniffer_continuous_mode_e */
   uint8_t alpha_factor;                        /*!<  Alpha Factor. 0 - 9 valid values
                                                       0 : Exponential Averaging is Disabled
                                                       1-9 : Exponential Averaging is Enabled  */
   uint16_t adc;                                /*!< Single ADC select \ref adi_apollo_adc_idx_e */
   adi_apollo_sniffer_mode_e sniffer_mode;      /*!< \ref adi_apollo_sniffer_mode_e */

} adi_apollo_sniffer_pgm_t;

/*!
* \brief Spectrum Sniffer Reading Parameters 
*/
typedef struct {
   uint8_t run_fft_engine_background;       /*!< If 0, stop FFT engine when reading data to save power */
   uint64_t timeout_us;                     /*!< Timeout: if set to 0, no timeout */ 
} adi_apollo_sniffer_read_t;


/*!
* \brief Spectrum Sniffer Parameters 
*/
typedef struct {
   adi_apollo_sniffer_init_t init;
   adi_apollo_sniffer_pgm_t pgm;
   adi_apollo_sniffer_read_t read;
} adi_apollo_sniffer_param_t;

/*!
* \brief Spectrum Sniffer data 
*/
typedef struct {
   uint16_t mag_i_data[ADI_APOLLO_SNIFFER_FFT_LENGTH];             /*!< Contents of Magnitude / I data registers */
   uint16_t bin_q_data[ADI_APOLLO_SNIFFER_FFT_LENGTH];             /*!< Contents of Bin # / Q data registers */
   uint8_t bin_above_threshold[ADI_APOLLO_SNIFFER_FFT_LENGTH];     /*!< Contents of bin above threshold data registers (Magnitude mode only) */                                         
   uint8_t bin_below_threshold[ADI_APOLLO_SNIFFER_FFT_LENGTH];     /*!< Contents of bin below threshold data registers (Magnitude mode only) */

   uint16_t max_threshold;                               /*!< Max threshold value */
   uint16_t min_threshold;                               /*!< Min threshold value */
   uint16_t valid_data_length;                           /*!< Length of valid data in arrays (512 complex, 256 real) */
   uint8_t fft_is_complex;                               /*!< Whether or not the FFT is complex or real */
   adi_apollo_sniffer_mode_e data_mode;                  /*!< Sniffer mode during data capture \ref adi_apollo_sniffer_mode_e */
} adi_apollo_sniffer_fft_data_t; 

#endif /* __ADI_APOLLO_SNIFFER_TYPES_H__ */
/*! @} */