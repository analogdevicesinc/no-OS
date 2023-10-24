/***************************************************************************//**
 *   @file   ad7768.h
 *   @brief  Header file of AD7768 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#ifndef AD7768_H_
#define AD7768_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7768_REG_CH_STANDBY				0x00
#define AD7768_REG_CH_MODE_A				0x01
#define AD7768_REG_CH_MODE_B				0x02
#define AD7768_REG_CH_MODE_SEL				0x03
#define AD7768_REG_PWR_MODE				0x04
#define AD7768_REG_GENERAL_CFG				0x05
#define AD7768_REG_DATA_CTRL				0x06
#define AD7768_REG_INTERFACE_CFG			0x07
#define AD7768_REG_BIST_CTRL				0x08
#define AD7768_REG_DEV_STATUS				0x09
#define AD7768_REG_REV_ID				0x0A
#define AD7768_REG_DEV_ID_MSB				0x0B
#define AD7768_REG_DEV_ID_LSB				0x0C
#define AD7768_REG_SW_REV_ID				0x0D
#define AD7768_REG_GPIO_CTRL				0x0E
#define AD7768_REG_GPIO_WR_DATA				0x0F
#define AD7768_REG_GPIO_RD_DATA				0x10
#define AD7768_REG_PRECHARGE_BUF_1			0x11
#define AD7768_REG_PRECHARGE_BUF_2			0x12
#define AD7768_REG_POS_REF_BUF				0x13
#define AD7768_REG_NEG_REF_BUF				0x14
#define AD7768_REG_CH_OFFSET_1(ch)			(0x1E + (ch) * 3)
#define AD7768_REG_CH_OFFSET_2(ch)			(0x1F + (ch) * 3)
#define AD7768_REG_CH_OFFSET_3(ch)			(0x20 + (ch) * 3)
#define AD7768_REG_CH_GAIN_1(ch)			(0x36 + (ch) * 3)
#define AD7768_REG_CH_GAIN_2(ch)			(0x37 + (ch) * 3)
#define AD7768_REG_CH_GAIN_3(ch)			(0x38 + (ch) * 3)
#define AD7768_REG_CH_SYNC_OFFSET(ch)			(0x4E + (ch) * 3)
#define AD7768_REG_DIAG_METER_RX			0x56
#define AD7768_REG_DIAG_CTRL				0x57
#define AD7768_REG_DIAG_MOD_DELAY_CTRL			0x58
#define AD7768_REG_DIAG_CHOP_CTRL			0x59

/* AD7768_REG_CH_STANDBY */
#define AD7768_CH_STANDBY(x)				(1 << (x))

/* AD7768_REG_CH_MODE_x */
#define AD7768_CH_MODE_FILTER_TYPE			(1 << 3)
#define AD7768_CH_MODE_DEC_RATE_MSK			NO_OS_GENMASK(2, 0)
#define AD7768_CH_MODE_DEC_RATE(x)			(((x) & 0x7) << 0)

/* AD7768_REG_CH_MODE_SEL */
#define AD7768_CH_MODE(x)				(1 << (x))

/* AD7768_REG_PWR_MODE */
#define AD7768_PWR_MODE_POWER_MODE_MSK			NO_OS_GENMASK(5, 4)
#define AD7768_PWR_MODE_SLEEP_MODE			(1 << 7)
#define AD7768_PWR_MODE_POWER_MODE(x)			(((x) & 0x3) << 4)
#define AD7768_PWR_MODE_LVDS_ENABLE			(1 << 3)
#define AD7768_PWR_MODE_MCLK_DIV_MSK			NO_OS_GENMASK(1, 0)
#define AD7768_PWR_MODE_MCLK_DIV(x)			(((x) & 0x3) << 0)
#define ad7768_map_power_mode_to_regval(x)		((x) ? ((x) + 1) : 0)

/* AD7768_REG_DATA_CTRL */
#define AD7768_DATA_CTRL_SPI_SYNC			(1 << 7)
#define AD7768_DATA_CTRL_SINGLE_SHOT_EN			(1 << 4)
#define AD7768_DATA_CTRL_SPI_RESET(x)			(((x) & 0x3) << 0)
#define AD7768_DATA_CONTROL_SPI_SYNC_MSK		NO_OS_BIT(7)
#define AD7768_DATA_CONTROL_SPI_SYNC			NO_OS_BIT(7)
#define AD7768_DATA_CONTROL_SPI_SYNC_CLEAR		0

/* AD7768_REG_INTERFACE_CFG */
#define AD7768_INTERFACE_CFG_CRC_SEL(x)			(((x) & 0x3) << 2)
#define AD7768_INTERFACE_CFG_DCLK_DIV(x)		(((x) & 0x3) << 0)
#define AD7768_INTERFACE_CFG_DCLK_DIV_MSK		NO_OS_GENMASK(1, 0)
#define AD7768_INTERFACE_CFG_DCLK_DIV_MODE(x)		(4 - no_os_find_first_set_bit(x))
#define AD7768_MAX_DCLK_DIV				8

#define AD7768_RESOLUTION				24
#define AD7768_SAMPLE_SIZE				32
#define AD7768_MAX_FREQ_PER_MODE			6
#define AD7768_NUM_CHANNELS				8

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef enum {
	AD7768_ACTIVE,
	AD7768_SLEEP,
} ad7768_sleep_mode;

typedef enum {
	AD7768_ECO = 0,
	AD7768_MEDIAN = 2,
	AD7768_FAST = 3,
} ad7768_power_mode;

enum ad7768_power_modes_raw {
	AD7768_LOW_POWER_MODE,
	AD7768_MEDIAN_MODE,
	AD7768_FAST_MODE,
	AD7768_NUM_POWER_MODES
};

typedef enum {
	AD7768_MCLK_DIV_32 = 0,
	AD7768_MCLK_DIV_8 = 2,
	AD7768_MCLK_DIV_4 = 3,
} ad7768_mclk_div;

typedef enum {
	AD7768_DCLK_DIV_8,
	AD7768_DCLK_DIV_4,
	AD7768_DCLK_DIV_2,
	AD7768_DCLK_DIV_1,
} ad7768_dclk_div;

typedef enum {
	AD7768_PIN_CTRL,
	AD7768_SPI_CTRL,
} ad7768_pin_spi_ctrl;

typedef enum {
	AD7768_STANDARD_CONV,
	AD7768_ONE_SHOT_CONV,
} ad7768_conv_op;

typedef enum {
	AD7768_NO_CRC,
	AD7768_CRC_4,
	AD7768_CRC_16,
	AD7768_CRC_16_2ND,
} ad7768_crc_sel;

typedef enum {
	AD7768_CH0,
	AD7768_CH1,
	AD7768_CH2,
	AD7768_CH3,
	AD7768_CH4,
	AD7768_CH5,
	AD7768_CH6,
	AD7768_CH7,
	AD7768_CH_NO
} ad7768_ch;

typedef enum {
	AD7768_ENABLED,
	AD7768_STANDBY,
} ad7768_ch_state;

typedef enum {
	AD7768_MODE_A,
	AD7768_MODE_B,
} ad7768_ch_mode;

typedef enum {
	AD7768_FILTER_WIDEBAND,
	AD7768_FILTER_SINC,
} ad7768_filt_type;

typedef enum {
	AD7768_DEC_X32,
	AD7768_DEC_X64,
	AD7768_DEC_X128,
	AD7768_DEC_X256,
	AD7768_DEC_X512,
	AD7768_DEC_X1024,
	AD7768_DEC_X1024_2ND,
	AD7768_DEC_X1024_3RD,
} ad7768_dec_rate;

static const int ad7768_dec_rate_vals[6] = {
	32, 64, 128, 256, 512, 1024
};

static const int ad7768_mclk_div_vals[3] = {
	32, 8, 4
};

struct ad7768_freq_config {
	unsigned int freq;
	unsigned int dec_rate;
};

struct ad7768_avail_freq {
	unsigned int n_freqs;
	struct ad7768_freq_config freq_cfg[AD7768_MAX_FREQ_PER_MODE];
};

typedef struct {
	struct no_os_spi_desc		*spi_desc;
	struct no_os_gpio_desc		*gpio_reset;
	uint8_t				gpio_reset_value;
	struct no_os_gpio_desc		*gpio_mode0;
	struct no_os_gpio_desc		*gpio_mode1;
	struct no_os_gpio_desc		*gpio_mode2;
	struct no_os_gpio_desc		*gpio_mode3;
	uint8_t				pin_spi_input_value;
	ad7768_pin_spi_ctrl		pin_spi_ctrl;
	ad7768_sleep_mode		sleep_mode;
	ad7768_power_mode		power_mode;
	enum ad7768_power_modes_raw	power_mode_raw;
	ad7768_mclk_div			mclk_div;
	ad7768_dclk_div			dclk_div;
	ad7768_conv_op			conv_op;
	ad7768_crc_sel			crc_sel;
	ad7768_ch_state			ch_state[8];
	ad7768_ch_mode			ch_mode[8];
	ad7768_filt_type		filt_type[2];
	ad7768_dec_rate			dec_rate[2];
	unsigned int			mclk;
	unsigned int			datalines;
	unsigned int			sampling_freq;
	struct ad7768_avail_freq	avail_freq[AD7768_NUM_POWER_MODES];
} ad7768_dev;

typedef struct {
	/* SPI */
	struct no_os_spi_init_param		spi_init;
	/* GPIO */
	struct no_os_gpio_init_param		gpio_reset;
	uint8_t					gpio_reset_value;
	struct no_os_gpio_init_param		gpio_mode0;
	struct no_os_gpio_init_param		gpio_mode1;
	struct no_os_gpio_init_param		gpio_mode2;
	struct no_os_gpio_init_param		gpio_mode3;
	/* Configuration */
	uint8_t					pin_spi_input_value;
	ad7768_sleep_mode			sleep_mode;
	ad7768_power_mode			power_mode;
	ad7768_mclk_div				mclk_div;
	ad7768_dclk_div				dclk_div;
	ad7768_conv_op				conv_op;
	ad7768_crc_sel				crc_sel;
	unsigned int				mclk;
	unsigned int				datalines;
} ad7768_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* SPI read from device. */
int32_t ad7768_spi_read(ad7768_dev *dev,
			uint8_t reg_addr,
			uint8_t *reg_data);
/* SPI write to device. */
int32_t ad7768_spi_write(ad7768_dev *dev,
			 uint8_t reg_addr,
			 uint8_t reg_data);
/* SPI read from device using a mask. */
int32_t ad7768_spi_read_mask(ad7768_dev *dev,
			     uint8_t reg_addr,
			     uint8_t mask,
			     uint8_t *data);
/* SPI write to device using a mask. */
int32_t ad7768_spi_write_mask(ad7768_dev *dev,
			      uint8_t reg_addr,
			      uint8_t mask,
			      uint8_t data);
/* Set the device sleep mode. */
int32_t ad7768_set_sleep_mode(ad7768_dev *dev,
			      ad7768_sleep_mode mode);
/* Get the device sleep mode. */
int32_t ad7768_get_sleep_mode(ad7768_dev *dev,
			      ad7768_sleep_mode *mode);
/* Set the device power mode. */
int32_t ad7768_set_power_mode(ad7768_dev *dev,
			      ad7768_power_mode mode);
/* Get the device power mode. */
int32_t ad7768_get_power_mode(ad7768_dev *dev,
			      ad7768_power_mode *mode);
/* Set the MCLK divider. */
int32_t ad7768_set_mclk_div(ad7768_dev *dev,
			    ad7768_mclk_div clk_div);
/* Get the MCLK divider. */
int32_t ad7768_get_mclk_div(ad7768_dev *dev,
			    ad7768_mclk_div *clk_div);
/* Set the DCLK divider. */
int32_t ad7768_set_dclk_div(ad7768_dev *dev,
			    ad7768_dclk_div clk_div);
/* Get the DCLK divider. */
int32_t ad7768_get_dclk_div(ad7768_dev *dev,
			    ad7768_dclk_div *clk_div);
/* Set the conversion operation mode. */
int32_t ad7768_set_conv_op(ad7768_dev *dev,
			   ad7768_conv_op conv_op);
/* Get the conversion operation mode. */
int32_t ad7768_get_conv_op(ad7768_dev *dev,
			   ad7768_conv_op *conv_op);
/* Set the CRC selection. */
int32_t ad7768_set_crc_sel(ad7768_dev *dev,
			   ad7768_crc_sel crc_sel);
/* Get the CRC selection. */
int32_t ad7768_get_crc_sel(ad7768_dev *dev,
			   ad7768_crc_sel *crc_sel);
/* Set the channel state. */
int32_t ad7768_set_ch_state(ad7768_dev *dev,
			    ad7768_ch ch,
			    ad7768_ch_state state);
/* Get the channel state. */
int32_t ad7768_get_ch_state(ad7768_dev *dev,
			    ad7768_ch ch,
			    ad7768_ch_state *state);
/* Set the mode configuration. */
int32_t ad7768_set_mode_config(ad7768_dev *dev,
			       ad7768_ch_mode mode,
			       ad7768_filt_type filt_type,
			       ad7768_dec_rate dec_rate);
/* Get the mode configuration. */
int32_t ad7768_get_mode_config(ad7768_dev *dev,
			       ad7768_ch_mode mode,
			       ad7768_filt_type *filt_type,
			       ad7768_dec_rate *dec_rate);
/* Set the channel mode. */
int32_t ad7768_set_ch_mode(ad7768_dev *dev,
			   ad7768_ch ch,
			   ad7768_ch_mode mode);
/* Get the channel mode. */
int32_t ad7768_get_ch_mode(ad7768_dev *dev,
			   ad7768_ch ch,
			   ad7768_ch_mode *mode);
/* Initialize the device. */
int32_t ad7768_setup(ad7768_dev **device,
		     ad7768_init_param init_param);
/* Free the resources allocated by ad7768_setup(). */
int ad7768_remove(ad7768_dev *dev);
/* Begin initializing the device. */
int32_t ad7768_setup_begin(ad7768_dev **device,
			   ad7768_init_param init_param);
/* Finish initializing the device. */
int32_t ad7768_setup_finish(ad7768_dev *dev,
			    ad7768_init_param init_param);
/* Set available sampling frequency. */
void ad7768_set_available_sampl_freq(ad7768_dev *dev);
/* Set power mode and sampling frequency. */
int ad7768_set_power_mode_and_sampling_freq(ad7768_dev *dev,
		enum ad7768_power_modes_raw mode);
#endif // AD7768_H_
