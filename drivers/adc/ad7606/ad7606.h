/***************************************************************************//**
 *   @file   ad7606.h
 *   @brief  Header file for the ad7606 Driver.
 *   @author Stefan Popa (stefan.popa@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019, 2021(c) Analog Devices, Inc.
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
#ifndef AD7606_H_
#define AD7606_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

#include "no_os_pwm.h"
#include "clk_axi_clkgen.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7606_REG_STATUS			0x01
#define AD7606_REG_CONFIG			0x02
#define AD7606_REG_RANGE_CH_ADDR(ch)		(0x03 + ((ch) >> 1))
#define AD7606_REG_BANDWIDTH			0x07
#define AD7606_REG_OVERSAMPLING			0x08
#define AD7606_REG_GAIN_CH(ch)			(0x09 + (ch))
#define AD7606_REG_OFFSET_CH(ch)		(0x11 + (ch))
#define AD7606_REG_PHASE_CH(ch)			(0x19 + (ch))
#define AD7606_REG_DIGITAL_DIAG_ENABLE		0x21
#define AD7606_REG_DIGITAL_DIAG_ERR		0x22
#define AD7606_REG_OPEN_DETECT_ENABLE		0x23
#define AD7606_REG_OPEN_DETECTED		0x24
#define AD7606_REG_AIN_OV_UV_DIAG_ENABLE	0x25
#define AD7606_REG_AIN_OV_DIAG_ERROR		0x26
#define AD7606_REG_AIN_UV_DIAG_ERROR		0x27
#define AD7606_REG_DIAGNOSTIC_MUX_CH(ch)	(0x28 + ((ch) >> 1))
#define AD7606_REG_OPEN_DETECT_QUEUE		0x2C
#define AD7606_REG_CLK_FS_COUNTER		0x2D
#define AD7606_REG_CLK_OS_COUNTER		0x2E
#define AD7606_REG_ID				0x2F

/* AD7606_REG_STATUS */
#define AD7606_STATUS_CHANNEL_MSK		NO_OS_GENMASK(2,0)
#define AD7606_AIN_UV_ERR_MSK			NO_OS_BIT(3)
#define AD7606_AIN_OV_ERR_MSK			NO_OS_BIT(4)
#define AD7606_OPEN_DETECTED_MSK		NO_OS_BIT(5)
#define AD7606_DIGITAL_ERROR_MSK		NO_OS_BIT(6)
#define AD7606_RESET_DETECT_MSK			NO_OS_BIT(7)

/* AD7606_REG_CONFIG */
#define AD7606_CONFIG_OPERATION_MODE_MSK	NO_OS_GENMASK(1,0)
#define AD7606_CONFIG_DOUT_FORMAT_MSK		NO_OS_GENMASK(4,3)
#define AD7606_CONFIG_EXT_OS_CLOCK_MSK		NO_OS_BIT(5)
#define AD7606_CONFIG_STATUS_HEADER_MSK		NO_OS_BIT(6)

/* AD7606_REG_RANGE_CH_X_Y */
#define AD7606_RANGE_CH_MSK(ch)		(NO_OS_GENMASK(3, 0) << (4 * ((ch) % 2)))
#define AD7606_RANGE_CH_MODE(ch, mode)	\
	((NO_OS_GENMASK(3, 0) & mode) << (4 * ((ch) % 2)))

/* AD7606_REG_OVERSAMPLING */
#define AD7606_OS_PAD_MSK			NO_OS_GENMASK(7,4)
#define AD7606_OS_RATIO_MSK			NO_OS_GENMASK(3,0)

/* AD7606_REG_ID */
#define AD7606_ID_DEVICE_ID_MSK			NO_OS_GENMASK(7,4)
#define AD7606_ID_SILICON_REVISION_MSK		NO_OS_GENMASK(3,0)

/* AD7606_REG_GAIN_CH */
#define AD7606_GAIN_MSK				NO_OS_GENMASK(5,0)

/* AD7606_REG_DIGITAL_DIAG_ENABLE */
#define AD7606_ROM_CRC_ERR_EN_MSK		NO_OS_BIT(0)
#define AD7606_MM_CRC_ERR_EN_MSK		NO_OS_BIT(1)
#define AD7606_INT_CRC_ERR_EN_MSK		NO_OS_BIT(2)
#define AD7606_SPI_WRITE_ERR_EN_MSK		NO_OS_BIT(3)
#define AD7606_SPI_READ_ERR_EN_MSK		NO_OS_BIT(4)
#define AD7606_BUSY_STUCK_HIGH_ERR_EN_MSK	NO_OS_BIT(5)
#define AD7606_CLK_FS_OS_COUNTER_EN_MSK		NO_OS_BIT(6)
#define AD7606_INTERFACE_CHECK_EN_MSK		NO_OS_BIT(7)

/* AD7606_REG_DIAGNOSTIC_MUX_CH */
#define AD7606_DIAGN_MUX_CH_MSK(ch)		(NO_OS_GENMASK(2, 0) << (3 * (ch & 0x1)))

#define AD7606_RD_FLAG_MSK(x)		(NO_OS_BIT(6) | ((x) & 0x3F))
#define AD7606_WR_FLAG_MSK(x)		((x) & 0x3F)

#define AD7606_MAX_CHANNELS		8

/**
 * @enum ad7606_device_id
 * @brief Device ID definitions
 */
enum ad7606_device_id {
	/** 4-Channel DAS with 16-Bit, Bipolar Input, Simultaneous Sampling ADC */
	ID_AD7605_4,
	/** 4-Channel DAS with 16-Bit, Bipolar Input, Simultaneous Sampling ADC */
	ID_AD7606_4,
	/** 6-Channel DAS with 16-Bit, Bipolar Input, Simultaneous Sampling ADC */
	ID_AD7606_6,
	/** 8-Channel DAS with 16-Bit, Bipolar Input, Simultaneous Sampling ADC */
	ID_AD7606_8,
	/** 8-Channel DAS with 16-Bit, 800 kSPS, Bipolar Input, Simultaneous Sampling ADC */
	ID_AD7606B,
	/** 8-Channel DAS with 16-Bit, 1 MSPS, Bipolar Input, Simultaneous Sampling ADC */
	ID_AD7606C_16,
	/** 8-Channel DAS with 18-Bit, 1 MSPS, Bipolar Input, Simultaneous Sampling ADC */
	ID_AD7606C_18,
	/** 8-Channel DAS with 18-Bit, Bipolar, Simultaneous Sampling ADC */
	ID_AD7608,
	/** 8-Channel Differential DAS with 18-Bit, Bipolar, Simultaneous Sampling ADC */
	ID_AD7609,
};

/**
 * @enum ad7606_osr
 * @brief Oversampling ratio
 */
enum ad7606_osr {
	/** Oversample by 1 */
	AD7606_OSR_1,
	/** Oversample by 2 */
	AD7606_OSR_2,
	/** Oversample by 4 */
	AD7606_OSR_4,
	/** Oversample by 8 */
	AD7606_OSR_8,
	/** Oversample by 16 */
	AD7606_OSR_16,
	/** Oversample by 32 */
	AD7606_OSR_32,
	/** Oversample by 64 */
	AD7606_OSR_64,
	/** Oversample by 128, available for chips that have software mode only */
	AD7606_OSR_128,
	/** Oversample by 256, available for chips that have software mode only */
	AD7606_OSR_256
};

/**
 * @enum ad7606_op_mode
 * @brief Operation mode
 */
enum ad7606_op_mode {
	/** Normal operation mode */
	AD7606_NORMAL,
	/** Standby mode, all the PGAs, and all the SAR ADCs enter a low power mode */
	AD7606_STANDBY,
	/** Autostandby mode, available only in software mode */
	AD7606_AUTOSTANDBY,
	/** Shutdown mode, all circuitry is powered down */
	AD7606_SHUTDOWN
};

/**
 * @enum ad7606_dout_format
 * @brief Number of DOUT lines
 */
enum ad7606_dout_format {
	/** DOUT A line is used */
	AD7606_1_DOUT,
	/** DOUT A,B lines are used. */
	AD7606_2_DOUT,
	/** DOUT A,B,C,D lines are used. */
	AD7606_4_DOUT,
	/** DOUT A,B,C,D,E,F,G,H lines are used. */
	AD7606_8_DOUT
};

/**
 * @struct ad7606_config
 * @brief AD7606_REG_CONFIG configuration parameters
 */
struct ad7606_config {
	/** Operation mode */
	enum ad7606_op_mode op_mode;
	/** Number of DOUT lines */
	enum ad7606_dout_format dout_format;
	/** External oversampling clock switch */
	bool ext_os_clock;
	/** Status header switch */
	bool status_header;
};

/**
 * @struct ad7606_range
 * @brief Operation range as specified in datasheet (in uV)
 */
struct ad7606_range {
	/** Minimum range value (may be negative) */
	int32_t min;
	/** Maximum range value */
	int32_t max;
	/** Whether the range is differential */
	bool differential;
};

/**
 * @struct ad7606_digital_diag
 * @brief Oversampling settings
 */
struct ad7606_oversampling {
	/** Oversampling padding */
	uint8_t os_pad:4;
	/** Oversampling ratio */
	enum ad7606_osr os_ratio:4;
};

/**
 * @struct ad7606_digital_diag
 * @brief Digital diagnostics configuration switches
 */
struct ad7606_digital_diag {
	/** ROM CRC check switch */
	bool rom_crc_err_en: 1;
	/** Mempry map CRC check switch */
	bool mm_crc_err_en: 1;
	/** Conversion and register data CRC check switch */
	bool int_crc_err_en: 1;
	/** SPI write error switch  */
	bool spi_write_err_en: 1;
	/** SPI read error switch  */
	bool spi_read_err_en: 1;
	/** Busy stuck high for more than 4us error switch  */
	bool busy_stuck_high_err_en: 1;
	/** Frame sync and oversampling clock counter switch  */
	bool clk_fs_os_counter_en: 1;
	/** Interface check switch */
	bool interface_check_en: 1;
};

/**
 * @struct ad7606_dev
 * @brief Device driver structure
 */
struct ad7606_dev;

/**
 * @struct ad7606_axi_init_param
 * @brief AXI driver(s) initialization parameters
 */
struct ad7606_axi_init_param {
	/* Clock generator init parameters */
	struct axi_clkgen_init *clkgen_init;
	/* Clock generator rate */
	uint32_t axi_clkgen_rate;
	/* PWM generator init structure */
	struct no_os_pwm_init_param *trigger_pwm_init;
};

/**
 * @struct ad7606_init_param
 * @brief Device driver initialization parameters
 */
struct ad7606_init_param {
	/** SPI initialization parameters */
	struct no_os_spi_init_param spi_init;
	/* AXI initialization parameters */
	struct ad7606_axi_init_param *axi_init;
	/** RESET GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_reset;
	/** CONVST GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_convst;
	/** BUSY GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_busy;
	/** STBYn GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_stby_n;
	/** RANGE GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_range;
	/** OS0 GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_os0;
	/** OS1 GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_os1;
	/** OS2 GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_os2;
	/** PARn/SER GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_par_ser;
	/** Device ID */
	enum ad7606_device_id device_id;
	/** Oversampling settings */
	struct ad7606_oversampling oversampling;
	/** Whether the device is running in hardware or software mode */
	bool sw_mode;
	/** Configuration register settings */
	struct ad7606_config config;
	/** Digital diagnostics register settings */
	struct ad7606_digital_diag digital_diag_enable;
	/** Channel offset calibration */
	int8_t offset_ch[AD7606_MAX_CHANNELS];
	/** Channel phase calibration */
	uint8_t phase_ch[AD7606_MAX_CHANNELS];
	/** Channel gain calibration */
	uint8_t gain_ch[AD7606_MAX_CHANNELS];
	/** Channel operating range */
	struct ad7606_range range_ch[AD7606_MAX_CHANNELS];
};

int32_t ad7606_spi_reg_read(struct ad7606_dev *dev,
			    uint8_t reg_addr,
			    uint8_t *reg_data);
int32_t ad7606_spi_reg_write(struct ad7606_dev *dev,
			     uint8_t reg_addr,
			     uint8_t reg_data);
int32_t ad7606_spi_write_mask(struct ad7606_dev *dev,
			      uint32_t addr,
			      uint32_t mask,
			      uint32_t val);
int32_t ad7606_spi_data_read(struct ad7606_dev *dev,
			     uint32_t *data);
int32_t ad7606_read_samples(struct ad7606_dev *dev,
			    uint32_t *data,
			    uint32_t samples);
int32_t ad7606_convst(struct ad7606_dev *dev);
int32_t ad7606_reset(struct ad7606_dev *dev);
int32_t ad7606_set_oversampling(struct ad7606_dev *dev,
				struct ad7606_oversampling oversampling);
int32_t ad7606_set_ch_range(struct ad7606_dev *dev, uint8_t ch,
			    struct ad7606_range range);
int32_t ad7606_set_ch_offset(struct ad7606_dev *dev, uint8_t ch,
			     int8_t offset);
int32_t ad7606_set_ch_phase(struct ad7606_dev *dev, uint8_t ch,
			    uint8_t phase);
int32_t ad7606_set_ch_gain(struct ad7606_dev *dev, uint8_t ch,
			   uint8_t gain);
int32_t ad7606_set_config(struct ad7606_dev *dev,
			  struct ad7606_config config);
int32_t ad7606_set_digital_diag(struct ad7606_dev *dev,
				struct ad7606_digital_diag diag);
int32_t ad7606_init(struct ad7606_dev **device,
		    struct ad7606_init_param *init_param);
int32_t ad7606_remove(struct ad7606_dev *dev);
#endif /* AD7606_H_ */
