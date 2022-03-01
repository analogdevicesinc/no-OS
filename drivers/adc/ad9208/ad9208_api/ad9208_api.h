// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad9208.h
 *
 * \brief AD9208 API interface header file
 *
 * This file contains all the publicly exposed methods and data structures to
 * interface with the AD9208 API.
 *
 * Release 1.0.X
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */
#ifndef __AD9208API_H__
#define __AD9208API_H__

#include "api_def.h"
#include "no_os_util.h"

/** Enumerates Data Format Type Complex/Real*/
typedef enum {
	AD9208_DATA_FRMT_REAL = 0x0,  /**< Real Data*/
	AD9208_DATA_FRMT_COMPLEX      /**< Complex Data */
} ad9208_adc_data_frmt_t;

/** Enumerates ADC Channels*/
typedef enum {
	AD9208_ADC_CH_NONE = 0x0,   /**< No ADC Channel*/
	AD9208_ADC_CH_A,	    /**< ADC Channel A */
	AD9208_ADC_CH_B,	    /**< ADC Channel B*/
	AD9208_ADC_CH_ALL	    /**< ALL ADC Channels*/
} ad9208_adc_ch_t;

/** Enumerates ADC Full Scale Range Modes*/
typedef enum {
	AD9208_ADC_SCALE_2P04_VPP = 0,	/**< 2.04 Vpp Differential*/
	AD9208_ADC_SCALE_1P13_VPP,	/**< 1.13 Vpp Differential */
	AD9208_ADC_SCALE_1P25_VPP,	/**< 1.25 Vpp Differential */
	AD9208_ADC_SCALE_1P7_VPP,	/**< 1.70 Vpp Differential*/
	AD9208_ADC_SCALE_1P81_VPP,	/**< 1.81 Vpp Differential*/
	AD9208_ADC_SCALE_1P93_VPP	/**< 1.93 Vpp Differential*/
} ad9208_adc_scale_range_t;

/** Enumerates ADC Input Buffer Currents */
typedef enum {
	AD9208_ADC_BUFF_CURR_400_UA = 0x4,   /**< Buffer Current set to 400 uA*/
	AD9208_ADC_BUFF_CURR_500_UA = 0x9,   /**< Buffer Current set to 500 uA*/
	AD9208_ADC_BUFF_CURR_600_UA = 0x1E,  /**< Buffer Current set to 600 uA*/
	AD9208_ADC_BUFF_CURR_700_UA = 0x23,  /**< Buffer Current set to 700 uA*/
	AD9208_ADC_BUFF_CURR_800_UA = 0x28,  /**< Buffer Current set to 800 uA*/
	AD9208_ADC_BUFF_CURR_1000_UA = 0x32, /**< Buffer Current set to 1000 uA*/
} ad9208_adc_buff_curr_t;

/** Enumerates ADC NCO Modes*/
typedef enum {
	AD9208_ADC_NCO_VIF = 0,	  /**< Variable IF Mode*/
	AD9208_ADC_NCO_ZIF = 1,	  /**< Zero IF Mode */
	AD9208_ADC_NCO_TEST = 3,  /**< Test Mode*/
	AD9208_ADC_NCO_INVLD = 4  /**< Invalid NCO Mode*/
} ad9208_adc_nco_mode_t;

/** Enumerates AD9208 Powerdown Modes*/
typedef enum {
	AD9208_POWERUP = 0x0,	/**< Normal Operational Powerup*/
	AD9208_STANDBY = 0x2,	/**< Standby Mode Powerup */
	AD9208_POWERDOWN = 0x3	/**< Full Powerdown Mode*/
} ad9208_pdn_mode_t;

/** Enumerates SERDES PLL Status Flags*/
typedef enum {
	AD9208_PLL_LOCK_STAT = 0x8, /**< Serdes PLL lock Status Flag*/
	AD9208_PLL_LOSSLOCK = 0x4   /**< Serdes PLL Lost Lock Status Flag*/
} ad9208_jesd_serdes_pll_flg_t;

/** AD9208 API handle */
typedef struct {
	void *user_data;	/**< Void pointer to user defined data for HAL initialization */
	spi_xfer_t dev_xfer;	/**< Function Pointer to HAL SPI access function*/
	reset_pin_ctrl_t reset_pin_ctrl;
	/**< Function Point to HAL RESETB Pin Ctrl Function*/
	delay_us_t delay_us;	/**< Function Pointer to HAL delay function*/
	hw_open_t hw_open;	/**< Function Pointer to HAL initialization function*/
	hw_close_t hw_close;	/**< Function Pointer to HAL de-initialization function*/
	uint64_t adc_clk_freq_hz;   /**< ADC Clock Frequency in Hz. Valid range 2.5GHz to 3.1 GHz */
} ad9208_handle_t;

/**
 * \brief Initialize AD9208 Device
 * This API must be called first before any other API calls.
 * It performs internal API initialization of the memory and API states.
 * If AD9208 handle member hw_open is not NULL it shall call the function
 * to which it points. This feature may be used to get and initialize the
 * hardware resources required by the API and the AD9208 devices.
 * For example GPIO, SPI etc.
 *
 * Its is recommended to call the Reset API after this API to ensure all
 * spi registers are reset to ADI recommended defaults.
 *
 * \param h Pointer to the AD9208 device reference handle.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_HANDLE_PTR  Invalid reference handle
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_HW_OPEN HW Open Function Failed
 */
int ad9208_init(ad9208_handle_t *h);

/**
 * \brief De-initialize the AD9208 Device.
 *
 * This API must be called last. No other API should be called after this call.
 * It performs internal API de-initialization of the memory and API states.
 * If AD9208 handle member hw_close, is not NULL it shall call the function
 * to which it points. This feature may be used to de-initialize and release
 * any hardware resources required by the API and the AD9208 Device.
 * For example GPIO, SPI etc
 *
 * \param h Pointer to the AD9208 device reference handle.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_HW_CLOSE HW Close Function Failed
 */
int ad9208_deinit(ad9208_handle_t *h);

/**
 * \brief Reset the AD9208
 *
 * Issues a hard reset or soft reset of the device.
 * Performs a full reset of AD9208 via the hardware pin (hard) or
 * via the spi register (soft).
 * Resetting all SPI registers to default and triggering the required
 * initialization sequence.
 *
 * \param h         Pointer to the AD9208 device reference handle.
 * \param hw_reset  A parameter to indicate if the reset issues is to be via the
 *                  hardware pin or SPI register.
 *                  A value of 1 indicates a hardware reset is required.
 *                  A value of 0 indicates a software reset is required.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 * \retval API_ERROR_RESET_PIN_CTRL HW Reset Pin Function Failed
 */
int ad9208_reset(ad9208_handle_t *h, uint8_t hw_reset);

/**
 * \brief Sets the operation of the Power Down pin the AD9208
 *
 * Sets the Operation of the External power down pin of the AD9208. This pin can
 * configured so that it control entering and exiting powerdown or Standby modes
 * based on the signal applied to the pin.
 * It may also be configured to be disabled so that it does not effect
 * the powerdown mode.
 *
 *
 * \param h        Pointer to the AD9208 device reference handle.
 * \param pin_enable Powerdown pin enable/disable.
 *				   		1 - Powerdown pin controls powerdown status
 *				   		0 - Powerdown pin disable. Does not effect powerdown Status.
 * \param pin_mode Powerdown mode that Powerdown pin status, if enabled, shall control.
 *				   Valid options are
 *				   AD9208_STANDBY  -Pin status will control AD9208 Standby Status
 *				   AD9208_POWERDOWN -Pin status will control AD9208 Powerdown Status
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_set_pdn_pin_mode(ad9208_handle_t *h,
			    uint8_t pin_enable, ad9208_pdn_mode_t pin_mode);

/**
 * \brief Set AD9208 Input Sample Clock Configuration
 *
 * Value of the ADC Clock shall be Input Sample Clock/Clock Divider setting.
 * Valid range of Input Clock is 2.5 GHz to 6 GHz
 * The ADC sample clock shall be derived from the input clock and the configured divider setting.
 * Fs = clk_freq_hz/div. Valid range of ADC Clock is 2.5 GHz to 3.5 GHz.
 *
 * \param h            Pointer to the AD9208 device reference handle.
 * \param clk_freq_hz  ADC CLK applied to AD9208
 * \param div          The desired CLK input divider setting.
 *                     Valid values are 1,2 and 4.
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_set_input_clk_cfg(ad9208_handle_t *h,
			     uint64_t clk_freq_hz, uint8_t div);

/**
 * \brief Set AD9208 Input Sample Clock Duty Cycle Enablers
 *
 * When its not possible to provide a higher frequency clock,
 * it is recommended to turn on the Dyty Cycle Stabilizers (DCS)
 *
 * \param h           Pointer to the AD9208 device reference handle.
 * \param en          Desired Enable setting for the targeted DCS
 *                     0= Disabled
 *                     1= Enabled.
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_set_input_clk_duty_cycle_stabilizer(ad9208_handle_t *h, uint8_t en);

/**
 * \brief Get AD9208 ADC Sampling Clock (Fs) value
 *
 *
 * \param h                 Pointer to the AD9208 device reference handle.
 * \param *adc_clk_freq_hz  Pointer to the a uint64_t variable to store
 *                          the ADC Clk Frequency
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_get_adc_clk_freq(ad9208_handle_t *h, uint64_t *adc_clk_freq_hz);

/**
 * \brief Set AD9208 ADC Channel Select
 *
 * Selects the ADC Channels to be active for configuration.
 * Once an ADC Channel is selected to be active subsequent API calls
 * related to ADC Channel configuration shall be applied to that API
 *
 * \param h    Pointer to the AD9208 device reference handle.
 * \param ch   Bit wise representation of ADC Channels to be made active
 *             ADC channels are enumerated by ad9208_adc_ch_t
 *             For example
 *             ch =  AD9208_ADC_CH_NONE  ....No Channel selected
 *             ch =  AD9208_ADC_CH_A | AD9208_ADC_CH_B ... Channel A and Channel B Select
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_channel_select(ad9208_handle_t *h, uint8_t ch);

/**
 * \brief Get AD9208 ADC Channel Select
 *
 * Returns the ADC Channels currently Selected
 *
 * \param h    Pointer to the AD9208 device reference handle.
 * \param *ch  A pointer to a Bit wise representation of ADC Channels
 *             that are currently active
 *             ADC channels are enumerated by ad9208_adc_ch_t
 *             For example:
 *             *ch =  AD9208_ADC_CH_NONE  ....No Channel selected
 *             *ch =  AD9208_ADC_CH_A | AD9208_ADC_CH_B
 *                   ... Channel A and Channel B Select
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_get_channel_select(ad9208_handle_t *h, uint8_t *ch);

/**
 * \brief Set ADC Channel Power Down Mode
 *
 * Sets the Powerdown mode of the AD9208 Channels. The AD9208 has 3 powerdown settings
 * PowerUp/Normal Operation, StandBy Mode, Powerdown Mode
 * This API may be used to set the desired powerdown mode via software.
 * Note 1: The AD9208 also features a Hardware CTRL PIN that may be configured as
 * a global Power down Pin.
 * Note 2: This API should be used in conjunction with ad9208_adc_set_channel_select
 * To set the target channel.
 *
 * \param h         Pointer to the AD9208 device reference handle.
 * \param mode  	Desired Powerdown mode
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_ch_pdn_mode(ad9208_handle_t *h, ad9208_pdn_mode_t mode);

/**
 * \brief Set AD9208 ADC Clock Phase Adjust
 *
 * Note Phase can be applied independantly to each ADC.
 * Use ad9208_adc_select_ch prior to selecting ADC
 * Note 1: This API should be used in conjunction with ad9208_adc_set_channel_select
 * To set the target channel.
 *
 * \param h          Pointer to the AD9208 device reference handle.
 * \param phase_adj  Number of 1/2 cycle delays to apply. Range 0-15
 *                   0 = No Delay
 *                   1 = 1/2 Input clock Cycle delay (Invert Clock)
 *                   2 = 1 input clock cycle delay
 *                   ....
 *                   15 = 7.5 Input clock Cyle delay
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_clk_phase(ad9208_handle_t *h, uint8_t phase_adj);

/**
 * \brief Set AD9208 ADC Input Configuration
 *
 * The AD9208 analog inputs are internally biased to the comon mode voltage.
 * For AC coupled applications an external Vref may be used.
 * For DC coupled applications the internal common mode buffer may be disabled.
 * And its recommended to export the internal common mode voltage reference
 * to the input buffer via the Vref pin. Refer to the Datasheet for more details.
 * Finally the internal voltage reference may be adjusted to modify the full-scale
 * voltage.
 * Refer to the Datasheet for full details.
 *
 * \param h                  Pointer to the AD9208 device reference handle.
 * \param analog_input_mode  ADC Analog Input mode. Enumerated by signal_coupling_t
 *                           COUPLING_AC - AC Coupled Mode
 *                           COUPLING_DC-  DC Coupled Mode
 * \param ext_vref           External Voltage Reference mode of AC Coupled Applications.
 *                           This parameter shall be ignored in DC Coupled Applications
 *                           0 - AC Coupled Mode
 *                           1-  DC Coupled Mode
 * \param full_scale_range   Full Scale Voltage settings.
 *                           Valid options are defined by ad9208_adc_scale_range_t
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */

int ad9208_adc_set_input_cfg(ad9208_handle_t *h,
			     signal_coupling_t analog_input_mode,
			     uint8_t ext_vref,
			     ad9208_adc_scale_range_t full_scale_range);

/**
 * \brief Set AD9208 Input Buffer Configuration
 *
 * The AD9208 analog inputs are internally biased to the comon mode voltage.
 * For DC coupled applications the internal common mode buffer may be disabled.
 * And its recommended to export the
 * internal common mode voltage to the input buffer via the Vref pin.
 * The buffer currents may be adjusted to optimize of SFDR over various input
 * frequencies and bandwidth.
 *
 * \param h              Pointer to the AD9208 device reference handle.
 * \param buff_curr_n    N-Input buffer current.
 *                       Valid values defined by ad9208_adc_buff_curr_t enumeration
 * \param buff_curr_p    P-Input buffer current.
 *                       Valid values defined by ad9208_adc_buff_curr_t enumeration
 * \param vcm_buff       VCM buffer current.
 *                       Valid values defined by ad9208_adc_buff_curr_t enumeration
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_input_buffer_cfg(ad9208_handle_t *h,
				    ad9208_adc_buff_curr_t buff_curr_n,
				    ad9208_adc_buff_curr_t buff_curr_p,
				    ad9208_adc_buff_curr_t vcm_buff);

/**
 * \brief Enable AD9208 ADC DC Offset Calibaration
 *
 * The AD9208 contains a digital filter to remove the dc offset from the output of the ADC.
 * This API enables or disables this optional digital Filter
 *
 * \param h        Pointer to the AD9208 device reference handle.
 * \param en       Desired enable setting for the DC Offset Calibration
 *                 0 - Disable DC Offset Calibration
 *                 1 - Disable DC Offset Calibration
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_dc_offset_filt_en(ad9208_handle_t *h, uint8_t en);

/**
 * \brief Set AD9208 ADC Operational Mode-Number of Channels
 *
 * The AD9208 contains a configurable ADC signal path to support
 * a wide range of applications. This API configures the ADC
 * operational mode based on the Carrier Frequency (Fc) Channels on input signal.
 *
 * \param h              Pointer to the AD9208 device reference handle.
 * \param fc_ch          Number of Carrier Frequencies. Valid Values
 *                       0 - Full Bandwidth
 *                       1 - One Channel
 *                       2 - Two Channel
 *                       4 - Four Channel
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_fc_ch_mode(ad9208_handle_t *h, uint8_t fc_ch);

/**
 * \brief Set AD9208 ADC Operational Mode-decimation rate
 *
 * The AD9208 contains a configurable ADC signal path to support
 * a wide range of applications. This API configures the ADC
 * decimation mode based desired requred ADC sampling rate (Fs) and
 * the desired output sample rate (Fout). Fs/Fout
 *
 * NOTE: The ADC DCM rate shall be set to the lowest Channel DCM value.
 *
 * \param h     Pointer to the AD9208 device reference handle.
 * \param dcm   Set ADC Decimation Ratio Range 1-48
 *		TODO: Verify All this modes are really Valid
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_dcm_mode(ad9208_handle_t *h, uint8_t dcm);

/**
 * \brief Set AD9208 ADC Data Format
 *
 *
 * \param h             Pointer to the AD9208 device reference handle.
 * \param ip_data_frmt  Input data format Real or Complex
 * \param op_data_frmt  Output data format Real or Complex
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_data_format(ad9208_handle_t *h,
			       ad9208_adc_data_frmt_t ip_data_frmt,
			       ad9208_adc_data_frmt_t op_data_frmt);

/**
 * \brief Set AD9208 ADC Synchronized Update mode Mode
 *
 * DDC NCO Parameters and Programable Filter Registers may be updated either
 * instantaneously or in a synchronized update.
 * In Synchronized Mode update may be trigger by a SPI bit trigger or
 * alternatively a GPIO pin may be configured to trigger synchronized update.
 * This api enables or disables the synchronized update mode.
 * This affects updates to FTW, POW, MAW & MBW and programmable filter configuration.
 *
 * \param h       Pointer to the AD9208 device reference handle.
 * \param en 	  Enable Synchronized update
 *                0- Synchronized Update Mode Disabled. Instantaneous update
 *                1- Synchronized Update Mode Enabled.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_sync_update_mode_enable(ad9208_handle_t *h, uint8_t en);

/**
 * \brief Select ADC Source for each DDC Channel
 *
 *
 * \param h           Pointer to the AD9208 device reference handle.
 * \param ddc_ch      DDC Channel on which to set Decimation Filters Valid Range 0-3
 * \param i_data_src  ADC data source for DDC I data channel.
 *                    AD9208_ADC_CH_A - ADC Channel A
 *                    AD9208_ADC_CH_B - ADC Channel B
 * \param q_data_src  ADC data source for DDC Q data channel.
 *                    AD9208_ADC_CH_A - ADC Channel A
 *                    AD9208_ADC_CH_B - ADC Channel B
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_ddc_ip_mux(ad9208_handle_t *h,
			      uint8_t ddc_ch, ad9208_adc_ch_t i_data_src,
			      ad9208_adc_ch_t q_data_src);

/**
 * \brief Set AD9208 Decimation Filters
 *
 * Each DDC Channel supports a range of Decimation filters
 * This API enables the appropriate Decimation filters per Channel
 * based on the desired decimation.
 *
 *
 * \param h       Pointer to the AD9208 device reference handle.
 * \param ddc_ch  DDC Channel on which to set Decimation Filters. Valid Range 0 to 3
 * \param dcm     DDC Channel Decimation. Valid Range 1 - 48
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_ddc_dcm(ad9208_handle_t *h, uint8_t ddc_ch, uint8_t dcm);

/**
 * \brief API to set DDC Frequency Translation Mode
 *
 * Each DDC Channel supports an NCO for frequency translation.
 * This API enables or disables the NCO to achieve
 * Variable IF mode( NCO enabled) or Zero IF Mode (NCO disabled).
 * Alternatively there a test mode may be enabled where the NCO directly
 * injects a ramp into the signal path.
 *
 * \param h       Pointer to the AD9208 device reference handle.
 * \param ddc_ch  DDC Channel on which to enable the NCO. Range 0-3.
 * \param mode  Desired NCO mode for target DDC Channel.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_ddc_nco_mode(ad9208_handle_t *h,
				uint8_t ddc_ch, ad9208_adc_nco_mode_t mode);

/**
 * \brief API to directly set the NCO Parameters
 *
 * Each DDC Channel supports an NCO for frequency translation
 * This API configures NCO parameters for the desired frequency Translation.
 * Alternatively the NCO may be configured based on the carrier Frequency.
 * Refer to ad9208_adc_set_ddc_nco
 *
 * \param h       Pointer to the AD9208 device reference handle.
 * \param ddc_ch  DDDC Channel on which to set NCO Frequency Translation. Range 0 to 3
 * \param ftw     Desired 48-bit FTW value for the NCO on the Target DDC Channel
 * \param mod_a   Desired 48-bit MAW value for the NCO on the Target DDC Channel
 * \param mod_b   Desired 48-bit MBW value for the NCO on the Target DDC Channel
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_ddc_nco_ftw(ad9208_handle_t *h, uint8_t ddc_ch,
			       uint64_t ftw, uint64_t mod_a, uint64_t mod_b);

/**
 * \brief Set AD9208 NCO based on the Channel Carrier Frequency
 *
 * Each DDC Channel support an NCO for frequency translation
 * This API configures NCO based on the Channel Carrier Frequency and
 * the adc_clk_freq_hz.
 *
 * \param h       Pointer to the AD9208 device reference handle.
 * \param ddc_ch  DDC Channel on which to set NCO Frequency Translation. Range 0 to 3
 * \param carrier_freq_hz Desired Channel Carrier Frequency in Hz
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_ddc_nco(ad9208_handle_t *h, uint8_t ddc_ch,
			   const int64_t carrier_freq_hz);

/**
 * \brief Set AD9208 NCO Phase Offset
 *
 * Each DDC Channel support an NCO for frequency translation
 * This API configures NCO parameters for the desired frequency Translation.
 *
 * \param h       Pointer to the AD9208 device reference handle.
 * \param ddc_ch  DDC Channel on which to enable the NCO, Range 0-3
 * \param po      Desired 48-bit NCO Phase offset
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_ddc_nco_phase(ad9208_handle_t *h, uint8_t ddc_ch,
				 uint64_t po);

/**
 * \brief Set AD9208 DDC Gain Stage
 *
 * Each DDC Channel contains an independently controlled gain stage.
 * For the AD9208 a 0dB or 6dB gain can be applied. This API sets
 * the gain for a particular DDC.
 *
 *
 * \param h       Pointer to the AD9208 device reference handle.
 * \param ddc_ch  DDC Channel on which to set the Gain Stage. Range 0 to 3
 * \param gain_db Desired gain setting. 0dB or 6dB
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_set_ddc_gain(ad9208_handle_t *h,
			    uint8_t ddc_ch, uint8_t gain_db);
/**
 * \brief API to trigger a DDC Soft Reset
 *
 * Following an update NCOs may be synchronized via DDC software Reset
 * This may be required as alternative to SYSREF signal Synchronization.
 * All NCO will be synchronized.
 *
 *
 * \param h       Pointer to the AD9208 device reference handle.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 * \retval API_ERROR_INIT_SEQ_FAIL Init sequence failed
 * \retval API_ERROR_SPI_SDO    Invalid SPI configuration
 */
int ad9208_adc_set_ddc_nco_reset(ad9208_handle_t *h);
/**
 * \brief Configure the JESD Interface for AD9208
 *
 * Configure JESD interface parameters
 *
 * \param h                Pointer to the AD9208 device reference handle.
 * \param jesd_param       The desired JESD link parameters for which to
 *                         configure the JESD Interface to AD9208.
 *                         jesd_param_t structure defines the parameter their
 *                         ranges supported by the AD9208.
 * \param *lane_rate_kbps  Pointer to uint64_t to return the determined
 *                         Lane Rate. Set to NULL if not required.
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_set_if_config(ad9208_handle_t *h,
			      jesd_param_t jesd_param,
			      uint64_t *lane_rate_kbps);

/**
 * \brief Read back all current JESD parameter settings.
 *
 * Read-back all the currently configured JESD Interface parameters.
 *
 * \param h           Pointer to the AD9208 device reference handle.
 * \param jesd_param  Pointer to a variable that will be set will the
 *                    current values of the JESD interface parameters.
 *
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_get_cfg_param(ad9208_handle_t *h, jesd_param_t *jesd_param);

/**
 * \brief Configure the Lane Cross Bar in the JESD datalink layer
 *
 * Configure AD9208 Lane Cross Bar to route the physical JESD lanes
 * to the desired logical lanes.
 *
 * \param h              Pointer to the AD9208 device reference handle.
 * \param physical_lane  uint8_t value indicating the Physical Lanes
 *                       to be routed to the serdes logical indicated
 *                       by the logical_lane parameter. Valid Range 0 -7
 * \param logical_lane   uint8_t value indicating the corresponding logical
 *                       lane for the physical lane listed
 *                       in parameter physical_lane. Valid Range 0 -7
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_set_lane_xbar(ad9208_handle_t *h,
			      uint8_t logical_lane, uint8_t physical_lane);

/**
 * \brief Get current Lane Cross Bar configuration for the JESD datalink layer
 *
 * Return the physical to logical lane mapping set by the configured by the
 * current Lane Cross Bar configuration.
 *
 * \param h             Pointer to the AD9208 device reference handle.
 * \param phy_log_map   Pointer a 8 deep uint8_t array.Each element of the array
 *                      represents the physical lane 0 - 7 and
 *                      the value represents the logical lane assigned to
 *                      that physical lane.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_get_lane_xbar(ad9208_handle_t *h, uint8_t *phy_log_map);

/**
 * \brief Enable the scrambler for the JESD Link
 *
 * Enable or Disable the scrambler for the JESD Link
 *
 * \param h   Pointer to the AD9208 device reference handle.
 * \param en  Enable control for JESD Scrambler.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_enable_scrambler(ad9208_handle_t *h, uint8_t en);

/**
 *
 * \brief  Enable JESD Link
 *
 * Enable SERDES Link to start JESD tranmission
 *
 * \param h     Pointer to the AD9208 device reference handle.
 * \param en    Enable control for the JESD Link
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_enable_link(ad9208_handle_t *h, uint8_t en);

/**
 * \brief  Get SERDES PLL Status Flags
 *
 * Read Serdes PLL status and return the status via the
 * pll_status parameter.
 *
 *
 * \param h            Pointer to the AD9208 device reference handle.
 * \param *pll_status  Pointer to the variable that will be set with
 *                     the PLL status. Lock Status bits are enumerated by
 *                     ad9208_jesd_serdes_pll_flg_t;
 *                     bit[7] => PLL Lock Status
 *                     bit[3] => PLL Lost Lock Status
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_get_pll_status(ad9208_handle_t *h, uint8_t *pll_status);

/**
 * \brief     Set JESD SYNCHRONIZATION Mode
 *
 * \param h         Pointer to the AD9208 device reference handle.
 * \param subclass  Desired Jesd Subclass Mode. Valid Range 0 to 1
 *                  0 - Sub Class 0
 *                  1 - Sub Class 1
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_subclass_set(ad9208_handle_t *h, uint8_t subclass);

/**
 * \brief     Set JESD SYNCHRONIZATION Mode
 *
 * \param h             Pointer to the AD9208 device reference handle.
 * \param mode          Sysref Synchronization Mode.
 *                      SYREF_NONE - NO SYSREF, Subclass 1 only
 *                      SYSREF_ONESHOT - single sysref signal mode
 *                      SYSREF_CONT    - Continuous sysref signal mode
 * \param sysref_count  Number of initial sysref sgnals to ignore in sysref n-shot mode 0-15
 *                        Valid only for N-Shot Mode.
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_syref_mode_set(ad9208_handle_t *h,
			       jesd_sysref_mode_t mode, uint8_t sysref_count);

/**
 * \brief     Set SYSREF Signal Capture settings
 *
 * \param h                Pointer to the AD9208 device reference handle.
 * \param sysref_edge_sel  Set the transition on which SYSREF is valid. Valid values are:
 *                          0 LOW to HIGH transition
 *                          1 HIGH to LOW transition
 * \param clk_edge_sel     Set the Sysref Capture clock Edge. Valid values are:
 *                          0 Rising clock Edge
 *                          1 Falling Clock Edge
 * \param neg_window_skew   Skew sample clock by which captured sysref is ignored.
 *                          Skew set in clock cycles. Valid range 0-3
 * \param pos_window_skew   Skew sample clock by which captured sysref is ignored.
 *                          Skew set in clock cycles. Valid range 0-3
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_syref_config_set(ad9208_handle_t *h,
				 uint8_t sysref_edge_sel, uint8_t clk_edge_sel,
				 uint8_t neg_window_skew,
				 uint8_t pos_window_skew);

/**
 * \brief     Get SYSREF status for sysref monitoring
 *
 * \param h             Pointer to the AD9208 device reference handle.
 * \param *hold_status  Pointer to a uint8_t variable to which the Sysref +/- Hold Status will be set
 * \param *setup_status Pointer to a uint8_t varaiable to whch the Sysref +/- Setup Status will be set
 * \param *phase_status Point to a uint8_t variable to which the
 *                      Sysref Divider Phase during Sysref Capture will be set.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_syref_status_get(ad9208_handle_t *h,
				 uint8_t *hold_status, uint8_t *setup_status,
				 uint8_t *phase_status);

/**
 * \brief     Set JESD SYNCHRONIZATION Time Stamp Mode
 *
 * \param h             Pointer to the AD9208 device reference handle.
 * \param timestamp_en  Enable Timestamp Mode, Sysref signal will trigger a timestamp sample.
 *                      0 - Timestamp Mode Enable, Sysref will trigger a timestamp smaple.
 *                      1 - Timestamp Mode Disable, Sysref will reset internal Synchronization.
 * \param control_bit   Desired control bit for Sysref time_stamp control.
 * \param delay         Delay of sysref timestamp in smaple clock cycles
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_sysref_timestamp_set(ad9208_handle_t *h,
				     uint8_t timestamp_en, uint8_t control_bit,
				     uint8_t delay);
/**
 * \brief     Set JESD LMFC Offset
 *
 * \param h       Pointer to the AD9208 device reference handle.
 * \param offset  Adjust the LMFC phase offset in frame clock cycles.
 *                For F = 1, only 4-frame shifts are valid
 *                For F = 2, Only 2-frame shifts are valid
 *                In all other cases for F, 1 frame shift are valid.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_syref_lmfc_offset_set(ad9208_handle_t *h, uint8_t offset);

/**
 * \brief  Get ADC Over range Status.
 *
 * Returns overrange detection at input to the ADC.
 * Clears the overange detection status signal.
 *
 * \param h   Pointer to the AD9208 device reference handle.
 * \param status A bit wise representation of the overrange status of each
 *               virtual converter. A bit value of 1 represents overange detection.
 *               A bit value of 0 indicates no overange detected.
 *               For example
 *               Status[0] - Overrange Status for Virtual Converter 0
 *               0 - ADC Input in Range
 *               1 - ADC Input overrange Detected.
 *               Status[1] - Overrange Status for Virtual Converter 1
 *               0 - ADC Input in Range
 *               1 - ADC Input overrange Detected.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_get_overange_status(ad9208_handle_t *h, uint8_t *status);

/**
 * \brief  Configure The Fast Detect Overange Signal Thresholds
 *
 * Configure the parameters, the upper and lower threshold levels and the dwell time,
 * that govern the triggering of the fast overange detection signal.
 *
 * \param h             Pointer to the AD9208 device reference handle.
 * \param upper_dbfs    A 13 bit value representing the Upper magnitude threshold level
 *                      in DBFS.
 *
 * \param lower_dbfs    A 13 bit value representing the lower magnitude threshold level
 *                      in DBFS.
 *
 * \param dwell_time    A 13 bit value representing the Upper magnitude threshold level
 *                      in DBFS.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */

int ad9208_adc_set_fd_thresholds(ad9208_handle_t *h,
				 uint16_t upper_dbfs, uint16_t lower_dbfs,
				 uint16_t dwell_time);
/**
 * \brief Enable Temp Sensor
 *
 * AD9208 Contains diode-based temperature sensors. And the diode output
 * voltages may be output to the VREF Pin.
 *
 * Note 1: the VREF pin may be used for the ADC reference voltage DC coupled applications
 * and External VREF for the ADC in AC coupled modes.
 * If VREF pin is used for the ADC Analog Input, VREF cannot be used for Temperature Sensor.
 * Refer to the Datasheet and the ad9208_adc_set_input_cfg API
 *
 * Note 2: This API should be used in conjunction with ad9208_adc_set_channel_select
 * To set the target channel.
 *
 * \param h    Pointer to the AD9208 device reference handle.
 * \param en   Enable the Temp Sensor Diodes.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_adc_temp_sensor_set_enable(ad9208_handle_t *h, uint8_t en);
/**
 * \brief Get Chip Identification Data
 *
 * read-back Product type, identification and revision data
 *
 * \param h          Pointer to the AD9208 device reference handle.
 * \param chip_id    Pointer to a variable of type  ad9208_chip_id_t to
 *                   which the product data read-back from chip shall be stored.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_get_chip_id(ad9208_handle_t *h, adi_chip_id_t *chip_id);

/**
 * \brief Perform SPI register write access to AD9208 Device
 *
 * \param h        Pointer to the AD9208 device reference handle.
 * \param address  AD9208 Device SPI address to which the value of data
 *                 parameter shall be written
 * \param data     8-bit value to be written to SPI register defined
 *                 by the address parameter.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 */
int ad9208_register_write(ad9208_handle_t *h,
			  const uint16_t address, const uint8_t data);

/**
 * \brief Perform SPI register read access to AD9208 Device.
 *
 *
 * \param h        Pointer to the AD9208 device reference handle.
 * \param address  AD9208 Device SPI address from which the value of data
 *                 parameter shall be read,
 * \param data     Pointer to an 8-bit variable to which the value of the
 *                 SPI register at the address defined by address parameter
 *                 shall be stored.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 */
int ad9208_register_read(ad9208_handle_t *h,
			 const uint16_t address, uint8_t *data);
/**
 * \brief Get API Revision Data
 *
 * \param h             Pointer to the AD9208 device reference handle.
 * \param rev_major     Pointer to variable to store the Major Revision Number
 * \param rev_minor     Pointer to variable to store the Minor Revision Number
 * \param rev_rc        Pointer to variable to store the RC Revision Number
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_get_revision(ad9208_handle_t *h, uint8_t *rev_major,
			uint8_t *rev_minor, uint8_t *rev_rc);

int ad9208_adc_set_input_scale(ad9208_handle_t *h,
			       ad9208_adc_scale_range_t full_scale_range);

int ad9208_get_decimation(ad9208_handle_t *h, uint8_t *dcm);

#endif /* !__AD9208API_H__ */
