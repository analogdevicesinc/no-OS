/***************************************************************************//**
 *   @file   ad738x.h
 *   @brief  Header file for AD738x Driver.
 *   @author SPopa (stefan.popa@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef SRC_AD738X_H_
#define SRC_AD738X_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_util.h"
#if defined(USE_STANDARD_SPI)
#include "no_os_spi.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/*
 * AD738X registers definition
 */
#define AD738X_REG_NOP                  0x00
#define AD738X_REG_CONFIG1              0x01
#define AD738X_REG_CONFIG2              0x02
#define AD738X_REG_ALERT                0x03
#define AD738X_REG_ALERT_LOW_TH         0x04
#define AD738X_REG_ALERT_HIGH_TH        0x05

/*
 * AD738X_REG_CONFIG1
 */
#define AD738X_CONFIG1_OS_MODE_MSK      NO_OS_BIT(9)
#define AD738X_CONFIG1_OS_MODE(x)       (((x) & 0x1) << 9)
#define AD738X_CONFIG1_OSR_MSK          NO_OS_GENMASK(8, 6)
#define AD738X_CONFIG1_OSR(x)           (((x) & 0x7) << 6)
#define AD738X_CONFIG1_CRC_W_MSK        NO_OS_BIT(5)
#define AD738X_CONFIG1_CRC_W(x)         (((x) & 0x1) << 5)
#define AD738X_CONFIG1_CRC_R_MSK        NO_OS_BIT(4)
#define AD738X_CONFIG1_CRC_R(x)         (((x) & 0x1) << 4)
#define AD738X_CONFIG1_ALERTEN_MSK      NO_OS_BIT(3)
#define AD738X_CONFIG1_ALERTEN(x)       (((x) & 0x1) << 3)
#define AD738X_CONFIG1_RES_MSK          NO_OS_BIT(2)
#define AD738X_CONFIG1_RES(x)           (((x) & 0x1) << 2)
#define AD738X_CONFIG1_REFSEL_MSK       NO_OS_BIT(1)
#define AD738X_CONFIG1_REFSEL(x)        (((x) & 0x1) << 1)
#define AD738X_CONFIG1_PMODE_MSK        NO_OS_BIT(0)
#define AD738X_CONFIG1_PMODE(x)         (((x) & 0x1) << 0)

/*
 * AD738X_REG_CONFIG2
 */
#define AD738X_CONFIG2_SDO2_MSK         NO_OS_BIT(8)
#define AD738X_CONFIG2_SDO2(x)          (((x) & 0x1) << 8)
#define AD738X_CONFIG2_SDO4_MSK         NO_OS_GENMASK(9, 8)
#define AD738X_CONFIG2_SDO4(x)          (((x) & 0x3) << 8)
#define AD738X_CONFIG2_RESET_MSK        NO_OS_GENMASK(7, 0)
#define AD738X_CONFIG2_RESET(x)         (((x) & 0xFF) << 0)

/*
 * AD738X_REG_ALERT_LOW_TH
 */
#define AD738X_ALERT_LOW_MSK            NO_OS_GENMASK(11, 0)
#define AD738X_ALERT_LOW(x)             (((x) & 0xFFF) << 0)

/*
 * AD738X_REG_ALERT_HIGH_TH
 */
#define AD738X_ALERT_HIGH_MSK           NO_OS_GENMASK(11, 0)
#define AD738X_ALERT_HIGH(x)            (((x) & 0xFFF) << 0)

/* Write to register x */
#define AD738X_REG_WRITE(x)             ((1 << 7) | ((x & 0x7) << 4))
/* Read from register x */
#define AD738X_REG_READ(x)              ((x & 0x7) << 4)

/*****************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum ad738x_conv_mode {
	TWO_WIRE_MODE,
	ONE_WIRE_MODE
};

enum ad738x_os_mode {
	NORMAL_OS_MODE,
	ROLLING_OS_MODE
};

enum ad738x_os_ratio {
	OSR_DISABLED,
	OSR_X2,
	OSR_X4,
	OSR_X8,
	OSR_X16,
	OSR_X32,
};

enum ad738x_resolution {
	RES_16_BIT,
	RES_18_BIT
};

enum ad738x_reset_type {
	SOFT_RESET,
	HARD_RESET
};

enum ad738x_pwd_mode {
	NORMAL_PWDM,
	FULL_PWDM
};

enum ad738x_ref_sel {
	INT_REF,
	EXT_REF
};

struct ad738x_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
#if !defined(USE_STANDARD_SPI)
	/** SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
#endif
	/* Device Settings */
	enum ad738x_conv_mode 	conv_mode;
	enum ad738x_ref_sel		ref_sel;
	uint32_t		ref_voltage_mv;
	enum ad738x_resolution 	resolution;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

struct ad738x_init_param {
	/* SPI */
	struct no_os_spi_init_param		*spi_param;
#if !defined(USE_STANDARD_SPI)
	/** SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
#endif
	/* Device Settings */
	enum ad738x_conv_mode	conv_mode;
	enum ad738x_ref_sel		ref_sel;
	uint32_t		ref_voltage_mv;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/** Initialize the device. */
int32_t ad738x_init(struct ad738x_dev **device,
		    struct ad738x_init_param *init_param);
/** Free the resources allocated by ad738x_init(). */
int32_t ad738x_remove(struct ad738x_dev *dev);
/** Read from device. */
int32_t ad738x_spi_reg_read(struct ad738x_dev *dev,
			    uint8_t reg_addr,
			    uint16_t *reg_data);
/** Write to device. */
int32_t ad738x_spi_reg_write(struct ad738x_dev *dev,
			     uint8_t reg_addr,
			     uint16_t reg_data);
/** Read conversion result from device. */
int32_t ad738x_spi_single_conversion(struct ad738x_dev *dev,
				     uint16_t *adc_data);
/** SPI write to device using a mask. */
int32_t ad738x_spi_write_mask(struct ad738x_dev *dev,
			      uint8_t reg_addr,
			      uint32_t mask,
			      uint16_t data);
/** Conversion mode */
int32_t ad738x_set_conversion_mode(struct ad738x_dev *dev,
				   enum ad738x_conv_mode mode);
/** Device reset over SPI. */
int32_t ad738x_reset(struct ad738x_dev *dev,
		     enum ad738x_reset_type reset);
/** Set the oversampling mode and ratio. */
int32_t ad738x_oversampling_config(struct ad738x_dev *dev,
				   enum ad738x_os_mode os_mode,
				   enum ad738x_os_ratio os_ratio,
				   enum ad738x_resolution res);
/** Device power down. */
int32_t ad738x_power_down_mode(struct ad738x_dev *dev,
			       enum ad738x_pwd_mode pmode);
/** Enable internal or external reference. */
int32_t ad738x_reference_sel(struct ad738x_dev *dev,
			     enum ad738x_ref_sel ref_sel);
/** Read data from device. */
int32_t ad738x_read_data(struct ad738x_dev *dev,
			 uint32_t *buf,
			 uint16_t samples);
#endif /* SRC_AD738X_H_ */
