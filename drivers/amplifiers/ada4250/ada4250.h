/***************************************************************************//**
 *   @file   ada4250.h
 *   @brief  Header file for ada4250 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#ifndef ADA4250_H_
#define ADA4250_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* ADA4250 Register Map */
#define ADA4250_REG_GAIN_MUX        0x00
#define ADA4250_REG_REFBUF_EN       0x01
#define ADA4250_REG_RESET           0x02
#define ADA4250_REG_SNSR_CAL_VAL    0x04
#define ADA4250_REG_SNSR_CAL_CNFG   0x05
#define ADA4250_REG_DIE_REV         0x18
#define ADA4250_REG_CHIP_ID1        0x19
#define ADA4250_REG_CHIP_ID2        0x1a

/* ADA4250_REG_GAIN_MUX Map */
#define ADA4250_GAIN_MUX_MSK        NO_OS_GENMASK(2, 0)
#define ADA4250_GAIN_MUX(x)         no_os_field_prep(ADA4250_GAIN_MUX_MSK, x)

/* ADA4250_REG_REFBUF Map */
#define ADA4250_REFBUF_MSK          NO_OS_BIT(0)
#define ADA4250_REFBUF(x)           no_os_field_prep(ADA4250_REFBUF_MSK, x)

/* ADA4250_REG_REFBUFF_EN Bit Definition */
#define ADA4250_BUF_ENABLE          0x01
#define ADA4250_BUF_DISABLE         0x00

/* ADA4250_REG_RESET Map */
#define ADA4250_RESET_MSK           NO_OS_BIT(0)
#define ADA4250_RESET(x)            no_os_field_prep(ADA4250_RESET_MSK, x)

/* ADA4250_REG_RESET Bit Definition */
#define ADA4250_RESET_ENABLE        0x01
#define ADA4250_RESET_DISABLE       0x00

/* ADA4250_REG_SNSR_CAL_VAL Map */
#define ADA4250_SNSR_CAL_VAL_MSK    NO_OS_GENMASK(7, 0)
#define ADA4250_SNSR_CAL_VAL(x)     no_os_field_prep(ADA4250_SNSR_CAL_VAL_MSK, x)

/* ADA4250_REG_SNSR_CAL_CNFG Bit Definition */
#define ADA4250_BIAS_SET_MSK        NO_OS_GENMASK(3, 2)
#define ADA4250_BIAS_SET(x)         no_os_field_prep(ADA4250_BIAS_SET_MSK, x)
#define ADA4250_RANGE_SET_MSK       NO_OS_GENMASK(1, 0)
#define ADA4250_RANGE_SET(x)        no_os_field_prep(ADA4250_RANGE_SET_MSK, x)

/* Specifications */
#define ADA4250_SPI_WRITE_CMD		0x0
#define ADA4250_BUFF_SIZE_BYTES     2
#define ADA4250_SPI_READ_CMD		NO_OS_BIT(7)
#define ADA4250_DIE_REV             0x0
#define ADA4250_CHIP_ID             0x4250

/* ADA4250 Extra Definitions */
#define ADA4250_SPI_DUMMY_DATA		0x00

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
  * @enum ada4250_id
  * @brief Current bias settings
  */
enum ada4250_id {
	ADA4230,
	ADA4250,
};

/**
  * @enum ada4250_bias
  * @brief Current bias settings
  */
enum ada4250_bias {
	ADA4250_BIAS_DISABLE,
	ADA4250_BIAS_BANDGAP_REF,
	ADA4250_BIAS_AVDD
};

/**
  * @enum ada4250_offset_range
  * @brief Sensor offset trim range.
  */
enum ada4250_offset_range {
	ADA4250_RANGE1,
	ADA4250_RANGE2,
	ADA4250_RANGE3,
	ADA4250_RANGE4,
};

/**
  * @enum ada4250_gain
  * @brief Gain value.
  */
enum ada4250_gain {
	ADA4250_GAIN_1,
	ADA4250_GAIN_2,
	ADA4250_GAIN_4,
	ADA4250_GAIN_8,
	ADA4250_GAIN_16,
	ADA4250_GAIN_32,
	ADA4250_GAIN_64,
	ADA4250_GAIN_128,
};

/**
  * @enum ada4250_bandwidth
  * @brief Bandwidth modes.
  */
enum ada4250_bandwidth {
	ADA4250_BANDWIDTH_LOW,
	ADA4250_BANDWIDTH_HIGH
};

/**
  * @enum ada4250_power_mode
  * @brief Power Modes.
  */
enum ada4250_power_mode {
	ADA4250_POWER_NORMAL,
	ADA4250_POWER_SLEEP,
	ADA4250_POWER_SHUTDOWN,
};

/**
 * @struct ada4250_init_param
 * @brief ADA4250 Initialization Parameters structure.
 */
struct ada4250_init_param {
	/* Device ID */
	enum ada4250_id device_id;
	/* SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	/* GPIO Initialization parameters */
	struct no_os_gpio_init_param	*gpio_g2_param;
	struct no_os_gpio_init_param	*gpio_g1_param;
	struct no_os_gpio_init_param	*gpio_g0_param;
	struct no_os_gpio_init_param	*gpio_bw_param;
	struct no_os_gpio_init_param	*gpio_bufen_param;
	struct no_os_gpio_init_param	*gpio_slp;
	struct no_os_gpio_init_param	*gpio_shtdwn;
	/* AVDD value in milliVolts */
	int32_t avdd_v;
	/* Reference Buffer Enable */
	bool refbuf_en;
	/* Gain Value */
	enum ada4250_gain gain;
	/* Bias Set */
	enum ada4250_bias bias;
	/* Bandwidth Value */
	enum ada4250_bandwidth bandwidth;
	/* Offset Calibration Value */
	int32_t offset_nv;
};

/**
 * @struct ada4250_dev
 * @brief ADA4250 Device Descriptor.
 */
struct ada4250_dev {
	/* Device ID */
	enum ada4250_id device_id;
	/* SPI Initialization parameters */
	struct no_os_spi_desc	*spi_desc;
	/* GPIO Descriptors */
	struct no_os_gpio_desc	*gpio_g2;
	struct no_os_gpio_desc	*gpio_g1;
	struct no_os_gpio_desc	*gpio_g0;
	struct no_os_gpio_desc	*gpio_bw;
	struct no_os_gpio_desc	*gpio_bufen;
	struct no_os_gpio_desc	*gpio_slp;
	struct no_os_gpio_desc	*gpio_shtdwn;
	/* AVDD value in milliVolts */
	int32_t avdd_v;
	/* Reference Buffer Enable */
	bool refbuf_en;
	/* Gain Value */
	enum ada4250_gain gain;
	/* Offset Range */
	enum ada4250_offset_range offset_range;
	/* Bias Set */
	enum ada4250_bias bias;
	/* Bandwidth Value */
	enum ada4250_bandwidth bandwidth;
	/* Power Mode */
	enum ada4250_power_mode power_mode;
	/* Offset Calibration Value in nV*/
	int32_t offset_nv;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** ADA4250 SPI write */
int32_t ada4250_write(struct ada4250_dev *dev, uint8_t reg_addr,
		      uint8_t data);

/** ADA4250 SPI Read */
int32_t ada4250_read(struct ada4250_dev *dev, uint8_t reg_addr,
		     uint8_t *data);

/* ADA4250 Register Update */
int32_t ada4250_update(struct ada4250_dev *dev, uint8_t reg_addr,
		       uint8_t mask, uint8_t data);

/* ADA4250 Device Descriptor Update */
int32_t ada4250_update_desc(struct ada4250_dev *dev);

/* Software Reset */
int32_t ada4250_soft_reset(struct ada4250_dev *dev);

/* Set Reference Buffer */
int32_t ada4250_en_refbuf(struct ada4250_dev *dev, bool refbuf);

/* Set Current Bias */
int32_t ada4250_set_bias(struct ada4250_dev *dev, enum ada4250_bias bias);

/* Set gain */
int32_t ada4250_set_gain(struct ada4250_dev *dev, enum ada4250_gain gain);

/* Set offset value */
int32_t ada4250_set_offset(struct ada4250_dev *dev, int64_t offset);

/* Set bandwidth mode */
int32_t ada4250_set_bandwidth(struct ada4250_dev *dev,
			      enum ada4250_bandwidth bw);

/* Set sleep/shutdown mode */
int32_t ada4250_set_slp_shtdwn_mode(struct ada4250_dev *dev,
				    enum ada4250_power_mode pwrmode);

/* Set normal mode */
int32_t ada4250_set_normal_mode(struct ada4250_dev *dev, bool reconfig);

/* ADA4250 Initialization */
int32_t ada4250_init(struct ada4250_dev **device,
		     struct ada4250_init_param *init_param);

/** ADA4250 Resources Deallocation */
int32_t ada4250_remove(struct ada4250_dev *dev);

#endif /* ADA4250_H_ */
