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
#include "spi.h"
#include "util.h"

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
#define ADA4250_GAIN_MUX_MSK        GENMASK(2, 0)
#define ADA4250_GAIN_MUX(x)         field_prep(ADA4250_GAIN_MUX_MSK, x)

/* ADA4250_REG_GAIN_MUX Bit Definition */
#define ADA4250_GAIN(x)             log2(x)

/* ADA4250_REG_REFBUF Map */
#define ADA4250_REFBUF_MSK          BIT(0)
#define ADA4250_REFBUF(x)           field_prep(ADA4250_REFBUF_MSK, x)

/* ADA4250_REG_REFBUFF_EN Bit Definition */
#define ADA4250_BUF_ENABLE          0x01
#define ADA4250_BUF_DISABLE         0x00

/* ADA4250_REG_RESET Map */
#define ADA4250_RESET_MSK           BIT(0)
#define ADA4250_RESET(x)            field_prep(ADA4250_RESET_MSK, x)

/* ADA4250_REG_RESET Bit Definition */
#define ADA4250_RESET_ENABLE        0x01
#define ADA4250_RESET_DISABLE       0x00

/* ADA4250_REG_SNSR_CAL_VAL Map */
#define ADA4250_SNSR_CAL_VAL_MSK    GENMASK(7, 0)
#define ADA4250_SNSR_CAL_VAL(x)     field_prep(ADA4250_SNSR_CAL_VAL_MSK, x)

/* ADA4250_REG_SNSR_CAL_CNFG Bit Definition */
#define ADA4250_BIAS_SET_MSK        GENMASK(1, 0)
#define ADA4250_BIAS_SET(x)         field_prep(ADA4250_SNSR_CAL_VAL_MSK, x)
#define ADA4250_RANGE_SET_MSK       GENMASK(3, 2)
#define ADA4250_RANGE_SET(x)        field_prep(ADA4250_SNSR_CAL_VAL_MSK, x)

#define ADA4250_RANGE(x)           (x - 1)

/* Specifications */
#define ADA4250_SPI_WRITE_CMD		0x0
#define ADA4250_BUFF_SIZE_BYTES     2
#define ADA4250_SPI_READ_CMD		BIT(7)
#define ADA4250_DIE_REV             0x0
#define ADA4250_CHIP_ID             0x4250

/* ADA4250 Extra Definitions */
#define ADA4250_SPI_DUMMY_DATA		0x00

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

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
 * @struct ada4250_init_param
 * @brief ADA4250 Initialization Parameters structure.
 */
struct ada4250_init_param {
	/* SPI Initialization parameters */
	struct spi_init_param	*spi_init;
	/* Reference Buffer Enable */
	bool refbuf_en;
	/* Gain Value */
	uint8_t gain;
	/* Bias Set */
	enum ada4250_bias bias;
	/* Offset Range */
	uint8_t offset_range;
	/* Offset Calibration Value */
	int8_t offset_val;
};

/**
 * @struct ada4250_dev
 * @brief ADA4250 Device Descriptor.
 */
struct ada4250_dev {
	/* SPI Initialization parameters */
	struct spi_desc	*spi_desc;
	/* Reference Buffer Enable */
	bool refbuf_en;
	/* Gain Value */
	uint8_t gain;
	/* Bias Set */
	enum ada4250_bias bias;
	/* Offset Range */
	uint8_t offset_range;
	/* Offset Calibration Value */
	int8_t offset_val;
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

/* Software Reset */
int32_t ada4250_soft_reset(struct ada4250_dev *dev);

/* Set Reference Buffer */
int32_t ada4250_en_refbuf(struct ada4250_dev *dev, bool refbuf);

/* Set Current Bias */
int32_t ada4250_set_bias(struct ada4250_dev *dev, enum ada4250_bias bias);

/* Set offset trim range */
int32_t ada4250_set_range(struct ada4250_dev *dev, uint8_t range);

/* Set gain */
int32_t ada4250_set_gain(struct ada4250_dev *dev, uint8_t gain);

/* Set offset value */
int32_t ada4250_set_offset(struct ada4250_dev *dev, int8_t offset);

/* ADA4250 Initialization */
int32_t ada4250_init(struct ada4250_dev **device,
		     struct ada4250_init_param *init_param);

/** ADA4250 Resources Deallocation */
int32_t ada4250_remove(struct ada4250_dev *dev);

#endif /* ADA4250_H_ */
