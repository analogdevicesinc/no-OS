/***************************************************************************//**
 *   @file   adxl38x.h
 *   @brief  Header file of ADXL38x Driver.
 *   @author BRajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef __ADXL38x_H__
#define __ADXL38x_H__


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_i2c.h"
#include "no_os_spi.h"

/******************************************************************************/
/******************************** ADXL38x *************************************/
/******************************************************************************/
/* Constants and Macros */

/* SPI */
#define ADXL38x_SPI_READ          0x01
#define ADXL38x_SPI_WRITE         0x00

/* Register Map fo ADXL38x (Rev H DS)*/
#define ADXL38x_DEVID_AD				0x00
#define ADXL38x_DEVID_MST				0x01
#define ADXL38x_PART_ID					0x02
#define ADXL38x_PART_ID_REV_ID			0x03
#define ADXL38x_SERIAL_NUMBER_0			0x04
#define ADXL38x_SERIAL_NUMBER_1			0x05
#define ADXL38x_SERIAL_NUMBER_2			0x06
#define ADXL38x_SERIAL_NUMBER_3			0x07
#define ADXL38x_SERIAL_NUMBER_4			0x08
#define ADXL38x_SERIAL_NUMBER_5			0x09
#define ADXL38x_SERIAL_NUMBER_6			0x0A
#define ADXL38x_DEV_DELTA_Q_X			0x0B
#define ADXL38x_DEV_DELTA_Q_Y			0x0C
#define ADXL38x_DEV_DELTA_Q_Z			0x0D
#define ADXL38x_DEV_DELTA_F0_X			0x0E
#define ADXL38x_DEV_DELTA_F0_Y			0x0F
#define ADXL38x_DEV_DELTA_F0_Z			0x10
#define ADXL38x_STATUS0					0x11
#define ADXL38x_STATUS1					0x12
#define ADXL38x_STATUS2					0x13
#define ADXL38x_STATUS3					0x14
#define ADXL38x_XDATA_H					0x15
#define ADXL38x_XDATA_L					0x16
#define ADXL38x_YDATA_H					0x17
#define ADXL38x_YDATA_L					0x18
#define ADXL38x_ZDATA_H					0x19
#define ADXL38x_ZDATA_L					0x1A
#define ADXL38x_TDATA_H					0x1B
#define ADXL38x_TDATA_L					0x1C
#define ADXL38x_FIFO_DATA				0x1D
#define ADXL38x_FIFO_STATUS0			0x1E
#define ADXL38x_FIFO_STATUS1			0x1F
#define ADXL38x_MISC0					0x20
#define ADXL38x_MISC1					0x21
#define ADXL38x_SENS_DSM				0x24
#define ADXL38x_CLK_CTRL				0x25
#define ADXL38x_OP_MODE					0x26
#define ADXL38x_DIG_EN					0x27
#define ADXL38x_SAR_I2C					0x28
#define ADXL38x_NVM_CTL					0x29
#define ADXL38x_REG_RESET				0x2A
#define ADXL38x_INT0_MAP0				0x2B
#define ADXL38x_INT0_MAP1				0x2C
#define ADXL38x_INT1_MAP0				0x2D
#define ADXL38x_INT1_MAP1				0x2E
#define ADXL38x_TEST_EN					0x2F
#define ADXL38x_FIFO_CFG0				0x30
#define ADXL38x_FIFO_CFG1				0x31
#define ADXL38x_SPT_CFG0				0x32
#define ADXL38x_SPT_CFG1				0x33
#define ADXL38x_SPT_CFG2				0x34
#define ADXL38x_SYNC_CFG				0x35
#define ADXL38x_PDM_CFG					0x36
#define ADXL38x_ACT_INACT_CTL			0x37
#define ADXL38x_SNSR_AXIS_EN			0x38
#define ADXL38x_THRESH_ACT_H			0x39
#define ADXL38x_THRESH_ACT_L			0x3A
#define ADXL38x_TIME_ACT_H				0x3B
#define ADXL38x_TIME_ACT_M				0x3C
#define ADXL38x_TIME_ACT_L				0x3D
#define ADXL38x_THRESH_INACT_H			0x3E
#define ADXL38x_THRESH_INACT_L			0x3F
#define ADXL38x_TIME_INACT_H			0x40
#define ADXL38x_TIME_INACT_M			0x41
#define ADXL38x_TIME_INACT_L			0x42
#define ADXL38x_TAP_THRESH				0x43
#define ADXL38x_TAP_DUR					0x44
#define ADXL38x_TAP_LATENT				0x45
#define ADXL38x_TAP_WINDOW				0x46
#define ADXL38x_TAP_CFG					0x47
#define ADXL38x_OR_CFG					0x48
#define ADXL38x_TRIG_CFG				0x49
#define ADXL38x_X_SAR_OFFSET			0x4A
#define ADXL38x_Y_SAR_OFFSET			0x4B
#define ADXL38x_Z_SAR_OFFSET			0x4C
#define ADXL38x_X_DSM_OFFSET			0x4D
#define ADXL38x_Y_DSM_OFFSET			0x4E
#define ADXL38x_Z_DSM_OFFSET			0x4F
#define ADXL38x_FILTER					0x50
#define ADXL38x_USER_TEMP_SENS_0		0x55
#define ADXL38x_USER_TEMP_SENS_1		0x56
#define ADXL38x_MISO					0x58
#define ADXL38x_SOUT0					0x59
#define ADXL38x_MCLK					0x5A
#define ADXL38x_BCLK					0x5B
#define ADXL38x_FSYNC					0x5C
#define ADXL38x_INT0					0x5D
#define ADXL38x_INT1					0x5E

/* Register reset value for ADXL38x */
#define ADXL38x_RESET_ZERO					0x00
#define ADXL38x_RESET_DEVID_AD 				0xAD
#define ADXL38x_RESET_DEVID_MST 			0x1D
#define ADXL38x_RESET_PART_ID 				0x17
#define ADXL38x_RESET_PART_ID_REV_ID 		0xC1
#define ADXL38x_RESET_STATUS0 				0x80
#define ADXL38x_RESET_STATUS2 				0x04
#define ADXL38x_RESET_INT0_MAP0 			0x80
#define ADXL38x_RESET_INT1_MAP1 			0x80
#define ADXL38x_RESET_SPT_CFG1 				0x08
#define ADXL38x_RESET_SPT_CFG2 				0x1A

/* Register masks */
#define ADXL38x_MASK_RANGE				0xC0
#define ADXL38x_MASK_OP_MODE			0x0F
#define ADXL38x_MASK_CHEN_DIG_EN		0xF0
#define ADXL38x_NEG_ACC_MSK				NO_OS_GENMASK(31, 16)

/* Pre-defined codes */
#define ADXL38x_RESET_CODE            0x52


/* Scale Factors */
/*
 * ADXL380
 * At +/- 4g with 16-bit resolution, scale from datasheet = 133.3ug/LSB.
 * 1333 * (10^-7)
 * For +/- 8g range a multiplier with value 2 is used.
 * For +/-16g range, a multiplier with value 4 is used.
 */
#define ADXL380_ACC_SCALE_FACTOR_GEE_MUL  (int64_t) 1333
/*
 * ADXL382
 * At +/- 15g with 16-bit resolution, scale from datasheet = 500ug/LSB.
 * 5000 * (10^-7)
 * For +/- 30g range a multiplier with value 2 is used.
 * For +/-60g range, a multiplier with value 4 is used.
 */
#define ADXL382_ACC_SCALE_FACTOR_GEE_MUL  (int64_t) 5000
/* Common denominator for ADXL380 and ADXL382 */
#define ADXL38x_ACC_SCALE_FACTOR_GEE_DIV  (int32_t)	10000000
/*
 * ADXL380/382 Temperature NOTE: Rev0 Values. Replace with RevA
 * scale is 168 LSB/C.
 * Offset = 9630LSB at 25C
 * offset at 0C = 9630 - (168 * 25) LSB
 * offset at 0C = 5430LSB
 */
#define ADXL38x_TEMP_OFFSET (int32_t) 5430
#define ADXL38x_TEMP_SCALE (int32_t) 168

/* Sensitivity */
/*
 * ADXL380
 * At +/- 4g with 16-bit resolution, sensitivity from datasheet = 7500LSB/g.
 * For +/- 8g range a divisor with value 2 is used.
 * For +/-16g range, a divisor with value 4 is used.
 */
#define ADXL380_ACC_SENSITIVITY (int32_t) 7500
/*
 * ADXL382
 * At +/- 15g with 16-bit resolution, sensitivity from datasheet = 2000LSB/g.
 * For +/- 30g range a divisor with value 2 is used.
 * For +/-60g range, a divisor with value 4 is used.
 */
#define ADXL382_ACC_SENSITIVITY (int32_t) 2000

/* Self Test Limits */
/* ADXL380 */
#define ADXL380_XY_ST_LIMIT_MIN 29
#define ADXL380_Z_ST_LIMIT_MIN 27
#define ADXL380_XY_ST_LIMIT_MAX 50
#define ADXL380_Z_ST_LIMIT_MAX 32
/* ADXL382 */
#define ADXL382_XY_ST_LIMIT_MIN 31
#define ADXL382_Z_ST_LIMIT_MIN 26
#define ADXL382_XY_ST_LIMIT_MAX 42
#define ADXL382_Z_ST_LIMIT_MAX 34

#define ADXL38x_ST_LIMIT_DENOMINATOR 10

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct device_variant
 * @brief ADXL38x Device selection.
 */
enum adxl38x_device_variant {
	ID_ADXL380,
	ID_ADXL382,
};

enum adxl38x_comm_type {
	ADXL38x_SPI_COMM,
	ADXL38x_I2C_COMM
};

/**
 * @struct adxl38x_range
 * @brief ADXL38x Range (OP MODE[7:6]).
 */
enum adxl38x_range {
	ADXL380_RANGE_4G = 0,
	ADXL382_RANGE_15G = 0,
	ADXL380_RANGE_8G = 1,
	ADXL382_RANGE_30G = 1,
	ADXL380_RANGE_16G = 2,
	ADXL382_RANGE_60G = 2
};

/**
 * @struct adxl38x_pdm
 * @brief ADXL38x PDM (OP MODE[5]).
 */
enum adxl38x_pdm {
	ADXL38x_PDM_OFF = 0,
	ADXL38x_PDM_ON = 1,
};

//may or maynot be supported in initial driver
/**
 * @struct adxl38x_aud_mode
 * @brief ADXL38x Audio mode (OP MODE[4]).
 */
enum adxl30_aud_mode {
	ADXL38x_AUD_OFF = 0,
	ADXL38x_AUD_ON = 1
};

/**
 * @struct adxl38x_op_mode
 * @brief ADXL38x Operating modes (OP MODE[3:0]).
 */
enum adxl38x_op_mode {
	ADXL38x_MODE_STDBY = 0,
	ADXL38x_MODE_HRT_SND = 1,
	ADXL38x_MODE_ULP = 2,
	ADXL38x_MODE_VLP = 3,
	ADXL38x_MODE_LP = 4,
	ADXL38x_MODE_LP_SERIAL_ULP_OP = 6,
	ADXL38x_MODE_LP_SERIAL_VLP_OP = 7,
	ADXL38x_MODE_RBW = 8,
	ADXL38x_MODE_RBW_SERIAL_ULP_OP = 10,
	ADXL38x_MODE_RBW_SERIAL_VLP_OP = 11,
	ADXL38x_MODE_HP = 12,
	ADXL38x_MODE_HP_SERIAL_ULP_OP = 14,
	ADXL38x_MODE_HP_SERIAL_VLP_OP = 15,
};

/**
 * @struct adxl38x_channel_slct
 * @brief ADXL38x Channel enable (DIG_EN[7:4] - TZYX).
 */
enum adxl38x_ch_select {
	ADXL38x_CH_DSB_ALL = 0,
	ADXL38x_CH_EN_X = 1,
	ADXL38x_CH_EN_Y = 2,
	ADXL38x_CH_EN_XY = 3,
	ADXL38x_CH_EN_Z = 4,
	ADXL38x_CH_EN_YZ = 6,
	ADXL38x_CH_EN_XYZ = 7,
	ADXL38x_CH_EN_T = 8,
	ADXL38x_CH_EN_ZT = 12,
	ADXL38x_CH_EN_YZT = 14,
	ADXL38x_CH_EN_XYZT = 15
};

/**
 * @struct adxl38x_fractional_val
 * @brief Data format in which the values are converted.
 */
struct adxl38x_fractional_val {
	int64_t integer;
	int32_t fractional;
};

union adxl38x_comm_desc {
	/** I2C Descriptor */
	struct no_os_i2c_desc *i2c_desc;
	/** SPI Descriptor */
	struct no_os_spi_desc *spi_desc;
};

union adxl38x_comm_init_param {
	/** I2C Initialization structure. */
	struct no_os_i2c_init_param i2c_init;
	/** SPI Initialization structure. */
	struct no_os_spi_init_param spi_init;
} ;

/**
 * @struct adxl38x_dev
 * @brief ADXL38x Device structure.
 */
struct adxl38x_dev {
	/** Device type*/
	enum adxl38x_device_variant dev_type;
	/** Device communication descriptor (Selecting communication protocol descriptor) */
	union adxl38x_comm_desc com_desc;
	/** Device Communication type (Selecting protocol)*/
	enum adxl38x_comm_type comm_type;
	/** Range */
	enum adxl38x_range range;
	/** Modes - Measurement, Standby */
	enum adxl38x_op_mode op_mode;

	uint8_t comm_buff[320];
};

/**
 * @struct adxl38x_init_param
 * @brief Structure holding the parameters for ADXL38x device initialization.
 */
struct adxl38x_init_param {
	/** Device Communication initialization structure: either SPI or I2C */
	union adxl38x_comm_init_param comm_init;
	/** Device Communication type: ADXL38x_SPI_COMM, ADXL38x_I2C_COMM */
	enum adxl38x_comm_type comm_type;
	/** Device type: ADXL380 or 382 */
	enum adxl38x_device_variant dev_type;
};

//NOTE: Fields are set based on RevH datasheet
struct _adxl38x_sts_reg_flags {
	//Status 3
	uint8_t DATA_READY : 1;
	uint8_t RESERVED2 : 7;
	//Status 2
	uint8_t UV_FLAG : 1;
	uint8_t UV_FLAG_STICKY : 1;
	uint8_t RESERVED1 : 1;
	uint8_t NVM_CRC_ERR : 1;
	uint8_t NVM_CRC_DONE : 1;
	uint8_t NVM_ECC_DET : 1;
	uint8_t RESERVED : 1;
	uint8_t NVM_ECC_DONE : 1;
	//Status 1
	uint8_t SINGLE_TAP : 1;
	uint8_t DOUBLE_TAP : 1;
	uint8_t TRIPLE_TAP : 1;
	uint8_t OVER_RANGE : 1;
	uint8_t OVER_RANGE_STICKY : 1;
	uint8_t ACT : 1;
	uint8_t INACT : 1;
	uint8_t NVM_IRQ : 1;
	//Status 0
	uint8_t PARITY_ERR_STICKY : 1;
	uint8_t FIFO_FULL : 1;
	uint8_t FIFO_OVR : 1;
	uint8_t FIFO_WATERMARK : 1;
	uint8_t FIFO_READY : 1;
	uint8_t EFUSE_BUSY_REGERR_STICKY : 1;
	uint8_t NVM_DONE : 1;
	uint8_t NVM_BUSY_STATUS : 1;
};

union adxl38x_sts_reg_flags {
	struct _adxl38x_sts_reg_flags fields;
	uint32_t value;
};


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/


int adxl38x_init(struct adxl38x_dev **device,
		 struct adxl38x_init_param init_param);

int adxl38x_remove(struct adxl38x_dev *dev);

int  adxl38x_get_register_value (struct adxl38x_dev,
				 uint8_t register_address);

int adxl38x_set_register_value(struct adxl38x_dev *dev,
			       uint8_t register_address,
			       uint8_t register_value);

#endif /* __ADXL38x_H__ */