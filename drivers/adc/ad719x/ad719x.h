/***************************************************************************//**
 *   @file   AD719X.h
 *   @brief  Header file of AD7190/2/3/4/5 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __AD719X_H__
#define __AD719X_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

/******************************************************************************/
/******************************** AD719X **************************************/
/******************************************************************************/

/* SPI slave device ID */
#define AD719X_SLAVE_ID         1

/* AD719X Register Map */
#define AD719X_REG_COMM         0 // Communications Register (WO, 8-bit)
#define AD719X_REG_STAT         0 // Status Register         (RO, 8-bit)
#define AD719X_REG_MODE         1 // Mode Register           (RW, 24-bit
#define AD719X_REG_CONF         2 // Configuration Register  (RW, 24-bit)
#define AD719X_REG_DATA         3 // Data Register           (RO, 24/32-bit)
#define AD719X_REG_ID           4 // ID Register             (RO, 8-bit)
#define AD719X_REG_GPOCON       5 // GPOCON Register         (RW, 8-bit)
#define AD719X_REG_OFFSET       6 // Offset Register         (RW, 24-bit
#define AD719X_REG_FULLSCALE    7 // Full-Scale Register     (RW, 24-bit)

/* Communications Register Bit Designations (AD719X_REG_COMM) */
#define AD719X_COMM_WEN         (1 << 7)           // Write Enable.
#define AD719X_COMM_WRITE       (0 << 6)           // Write Operation.
#define AD719X_COMM_READ        (1 << 6)           // Read Operation.
#define AD719X_COMM_ADDR(x)     (((x) & 0x7) << 3) // Register Address.
#define AD719X_COMM_CREAD       (1 << 2)           // Continuous Read of Data Register.

/* Status Register Bit Designations (AD719X_REG_STAT) */
#define AD719X_STAT_RDY         (1 << 7) // Ready.
#define AD719X_STAT_ERR         (1 << 6) // ADC error bit.
#define AD719X_STAT_NOREF       (1 << 5) // Error no external reference.
#define AD719X_STAT_PARITY      (1 << 4) // Parity check of the data register.
#define AD719X_STAT_CH3         (1 << 3) // Channel 3.
#define AD719X_STAT_CH2         (1 << 2) // Channel 2.
#define AD719X_STAT_CH1         (1 << 1) // Channel 1.
#define AD719X_STAT_CH0         (1 << 0) // Channel 0.

/* Mode Register Bit Designations (AD719X_REG_MODE) */
#define AD719X_MODE_SEL(x)      (((uint32_t)(x) & 0x7) << 21) // Operation Mode Select.
#define AD719X_MODE_DAT_STA     ((uint32_t)1 << 20)           // Status Register transmission.
#define AD719X_MODE_CLKSRC(x)   (((uint32_t)(x) & 0x3) << 18) // Clock Source Select.
#define AD719X_MODE_AVG(x)      (((uint32_t)(x) & 0x3) << 16) // Fast settling filter.
#define AD719X_MODE_SINC3       (1 << 15)                          // SINC3 Filter Select.
#define AD719X_MODE_ENPAR       (1 << 13)                          // Parity Enable.
#define AD719X_MODE_CLKDIV      (1 << 12)                          // Clock divide by 2 (AD7190/2 only).
#define AD719X_MODE_SCYCLE      (1 << 11)                          // Single cycle conversion.
#define AD719X_MODE_REJ60       (1 << 10)                          // 50/60Hz notch filter.
#define AD719X_MODE_RATE(x)     ((x) & 0x3FF)                      // Filter Update Rate Select.

/* Mode Register: AD719X_MODE_AVG(x) options */
#define AD719X_AVG_NONE                 0 // No averaging (fast settling mode disabled).
#define AD719X_AVG_BY_2                 1 // Average by 2.
#define AD719X_AVG_BY_8                 2 // Average by 8.
#define AD719X_AVG_BY_16                3 // Average by 16.

/* Configuration Register Bit Designations (AD719X_REG_CONF) */
#define AD719X_CONF_CHOP        ((uint32_t)1 << 23)            // CHOP enable.
#define AD719X_CONF_REFSEL      ((uint32_t)1 << 20)            // REFIN1/REFIN2 Reference Select.
#define AD719X_CONF_PSEUDO      ((uint32_t)1 << 18)            // Pseudo differential analog inputs.
#define AD719X_CONF_CHAN(x)     ((uint32_t)((x) & 0x3FF) << 8) // Channel select.
#define AD719X_CONF_BURN        (1 << 7)                            // Burnout current enable.
#define AD719X_CONF_REFDET      (1 << 6)                            // Reference detect enable.
#define AD719X_CONF_BUF         (1 << 4)                            // Buffered Mode Enable.
#define AD719X_CONF_UNIPOLAR    (1 << 3)                            // Unipolar/Bipolar Enable.
#define AD719X_CONF_GAIN(x)     ((x) & 0x7)                         // Gain Select.

/* Channel Mask */
#define AD719X_CH_MASK(channel)		NO_OS_BIT(channel)

/* Configuration Register: AD719X_CONF_CHAN(x) options */
#define AD719X_CH_0      0
#define AD719X_CH_1      1
#define AD719X_CH_2      2
#define AD719X_CH_3      3
#define AD719X_CH_4      4
#define AD719X_CH_5      5
#define AD719X_CH_6      6
#define AD719X_CH_7      7
#define AD719X_CH_TEMP   8
#define AD719X_CH_SHORT  9

/* ID Register Bit Designations (AD7193_REG_ID) */
#define AD7190_4_ID_MASK			0x0F
#define AD7195_ID_MASK				0xFF

/* GPOCON Register Bit Designations (AD719X_REG_GPOCON) */
#define AD719X_GPOCON_BPDSW     (1 << 6) // Bridge power-down switch enable
#define AD719X_GPOCON_GP32EN    (1 << 5) // Digital Output P3 and P2 enable
#define AD719X_GPOCON_GP10EN    (1 << 4) // Digital Output P1 and P0 enable
#define AD719X_GPOCON_P3DAT     (1 << 3) // P3 state
#define AD719X_GPOCON_P2DAT     (1 << 2) // P2 state
#define AD719X_GPOCON_P1DAT     (1 << 1) // P1 state
#define AD719X_GPOCON_P0DAT     (1 << 0) // P0 state

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum ad719x_adc_gain {
//                           ADC Gain (CONF Reg)
	AD719X_ADC_GAIN_1 = 0, 		// Gain 1
	AD719X_ADC_GAIN_8 = 3, 		// Gain 8
	AD719X_ADC_GAIN_16 = 4,		// Gain 16
	AD719X_ADC_GAIN_32 = 5,		// Gain 32
	AD719X_ADC_GAIN_64 = 6,		// Gain 64
	AD719X_ADC_GAIN_128 = 7		// Gain 128
};

enum ad719x_adc_clock {
	// External crystal. The external crystal is connected from MCLK1 to MCLK2.
	AD719X_EXT_CRYSTAL_MCLK1_MCLK2,
	// External Clock applied to MCLK2
	AD719X_EXT_CRYSTAL_MCLK2,
	// Internal 4.92 MHz clock. Pin MCLK2 is tristated.
	AD719X_INT_CLK_4_92_MHZ_TRIST,
	// Internal 4.92 MHz clock. The internal clock is available on MCLK2.
	AD719X_INT_CLK_4_92_MHZ
};

enum ad719x_adc_modes {
	// Continuous Conversion Mode
	AD719X_MODE_CONT,
	// Single Conversion Mode
	AD719X_MODE_SINGLE,
	// Idle Mode
	AD719X_MODE_IDLE,
	// Power-Down Mode
	AD719X_MODE_PWRDN,
	// Internal Zero-Scale Calibration
	AD719X_MODE_CAL_INT_ZERO,
	// Internal Full-Scale Calibration4
	AD719X_MODE_CAL_INT_FULL,
	// System Zero-Scale Calibration5
	AD719X_MODE_CAL_SYS_ZERO,
	// System Full-Scale Calibration
	AD719X_MODE_CAL_SYS_FULL,
};

enum ad719x_chip_id {
	AD7190 = 0x4,
	AD7192 = 0x0,
	AD7193 = 0x2,
	AD7194 = 0x3,
	AD7195 = 0xA6
};

struct ad719x_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_miso;
	struct no_os_gpio_desc	*sync_pin;
	/* Device Settings */
	uint8_t			current_polarity;
	enum ad719x_adc_gain	current_gain;
	enum ad719x_adc_modes	operating_mode;
	uint16_t    		data_rate_code;
	enum ad719x_adc_clock	clock_source;
	uint8_t			input_mode;
	uint8_t			buffer;
	uint8_t     		bpdsw_mode;
	enum ad719x_chip_id chip_id;
};

struct ad719x_init_param {
	/* SPI */
	struct no_os_spi_init_param		*spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	*gpio_miso;
	/* Optional GPIO pin - only for multiple devices */
	struct no_os_gpio_init_param	*sync_pin;
	/* Device Settings */
	uint8_t			current_polarity;
	enum ad719x_adc_gain	current_gain;
	enum ad719x_adc_modes	operating_mode;
	uint16_t    		data_rate_code;
	enum ad719x_adc_clock	clock_source;
	uint8_t			input_mode;
	uint8_t			buffer;
	uint8_t     		bpdsw_mode;
	enum ad719x_chip_id chip_id;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Checks if the AD7139 part is present. */
int ad719x_init(struct ad719x_dev **device,
		struct ad719x_init_param init_param);

/*! Free the resources allocated by ad719x_init(). */
int ad719x_remove(struct ad719x_dev *dev);

/*! Writes data into a register. */
int ad719x_set_register_value(struct ad719x_dev *dev, uint8_t reg_addr,
			      uint32_t reg_value, uint8_t bytes_number);

/*! Reads the value of a register. */
int ad719x_get_register_value(struct ad719x_dev *dev, uint8_t reg_addr,
			      uint8_t bytes_number, uint32_t *reg_data);

/* Write masked data into device register. */
int ad719x_set_masked_register_value(struct ad719x_dev *dev,
				     uint8_t reg_addr, uint32_t mask, uint32_t data,
				     uint8_t bytes);

/*! Resets the device. */
int ad719x_reset(struct ad719x_dev *dev);

/*! Set the device into specified operating mode. */
int ad719x_set_operating_mode(struct ad719x_dev *dev,
			      enum ad719x_adc_modes opt_mode);

/*! Waits for RDY pin to go low. */
int ad719x_wait_rdy_go_low(struct ad719x_dev *dev);

/*! Selects the channels to be enabled. */
int ad719x_channels_select(struct ad719x_dev *dev, uint16_t chn_mask);

/*! Performs the given calibration to the specified channel. */
int ad719x_calibrate(struct ad719x_dev *dev,
		     uint8_t mode, uint8_t channel);

/*! Configures the input mode of the ADC */
int ad719x_config_input_mode(struct ad719x_dev *dev, uint8_t mode);

/*! Enables or disables the buffer on the ADC input channels */
int ad719x_buffer_select(struct ad719x_dev *dev, uint8_t buff_en);

/*! Selects the filter output data rate of the ADC */
int ad719x_output_rate_select(struct ad719x_dev *dev,
			      uint16_t out_rate_code);

/*! Selects the clock source of the ADC */
int ad719x_clock_select(struct ad719x_dev *dev,
			enum ad719x_adc_clock clk_select);

/*! Opens or closes the bridge power-down switch of the ADC */
int ad719x_set_bridge_switch(struct ad719x_dev *dev, uint8_t bpdsw_select);

/*! Selects the polarity of the conversion and the ADC input range. */
int ad719x_range_setup(struct ad719x_dev *dev,
		       uint8_t polarity, enum ad719x_adc_gain range);

/*! Returns the result of a single conversion. */
int ad719x_single_conversion(struct ad719x_dev *dev, uint32_t *reg_data);

/*! Returns the average of several conversion results. */
int ad719x_continuous_read_avg(struct ad719x_dev *dev,
			       uint8_t sample_number, uint32_t *samples_avg);

/*! Read data from temperature sensor and converts it to Celsius degrees. */
int ad719x_temperature_read(struct ad719x_dev *dev, float *temp);

/*! Converts 24-bit raw data to volts. */
float ad719x_convert_to_volts(struct ad719x_dev *dev,
			      uint32_t raw_data, float v_ref);

#endif /* __AD719X_H__ */
