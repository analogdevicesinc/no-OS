/***************************************************************************//**
 *   @file   admv8818.h
 *   @brief  Header file for admv8818 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#ifndef ADMV8818_H_
#define ADMV8818_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* ADMV8818 Register Map */
#define ADMV8818_REG_SPI_CONFIG_A		0x0
#define ADMV8818_REG_SPI_CONFIG_B		0x1
#define ADMV8818_REG_CHIPTYPE			0x3
#define ADMV8818_REG_PRODUCT_ID_L		0x4
#define ADMV8818_REG_PRODUCT_ID_H		0x5
#define ADMV8818_REG_FAST_LATCH_POINTER		0x10
#define ADMV8818_REG_FAST_LATCH_STOP		0x11
#define ADMV8818_REG_FAST_LATCH_START		0x12
#define ADMV8818_REG_FAST_LATCH_DIRECTION	0x13
#define ADMV8818_REG_FAST_LATCH_STATE		0x14
#define ADMV8818_REG_WR0_SW			0x20
#define ADMV8818_REG_WR0_FILTER			0x21
#define ADMV8818_REG_WR1_SW			0x22
#define ADMV8818_REG_WR1_FILTER			0x23
#define ADMV8818_REG_WR2_SW			0x24
#define ADMV8818_REG_WR2_FILTER			0x25
#define ADMV8818_REG_WR3_SW			0x26
#define ADMV8818_REG_WR3_FILTER			0x27
#define ADMV8818_REG_WR4_SW			0x28
#define ADMV8818_REG_WR4_FILTER			0x29
#define ADMV8818_REG_LUT0_SW			0x100
#define ADMV8818_REG_LUT0_FILTER		0x101
#define ADMV8818_REG_LUT127_SW			0x1FE
#define ADMV8818_REG_LUT127_FILTER		0x1FF

/* ADMV8818_REG_SPI_CONFIG_A Map */
#define ADMV8818_SOFTRESET_N_MSK		NO_OS_BIT(7)
#define ADMV8818_LSB_FIRST_N_MSK		NO_OS_BIT(6)
#define ADMV8818_ENDIAN_N_MSK			NO_OS_BIT(5)
#define ADMV8818_SDOACTIVE_N_MSK		NO_OS_BIT(4)
#define ADMV8818_SDOACTIVE_MSK			NO_OS_BIT(3)
#define ADMV8818_ENDIAN_MSK			NO_OS_BIT(2)
#define ADMV8818_LSBFIRST_MSK			NO_OS_BIT(1)
#define ADMV8818_SOFTRESET_MSK			NO_OS_BIT(0)

/* ADMV8818_REG_SPI_CONFIG_B Map */
#define ADMV8818_SINGLE_INSTRUCTION_MSK		NO_OS_BIT(7)
#define ADMV8818_CSB_STALL_MSK			NO_OS_BIT(6)
#define ADMV8818_MASTER_SLAVE_RB_MSK		NO_OS_BIT(5)
#define ADMV8818_MASTER_SLAVE_TRANSFER_MSK	NO_OS_BIT(0)

/* ADMV8818_REG_WR0_SW Map */
#define ADMV8818_SW_IN_SET_WR0_MSK		NO_OS_BIT(7)
#define ADMV8818_SW_OUT_SET_WR0_MSK		NO_OS_BIT(6)
#define ADMV8818_SW_IN_WR0_MSK			NO_OS_GENMASK(5, 3)
#define ADMV8818_SW_OUT_WR0_MSK			NO_OS_GENMASK(2, 0)

/* ADMV8818_REG_WR0_FILTER Map */
#define ADMV8818_HPF_WR0_MSK			NO_OS_GENMASK(7, 4)
#define ADMV8818_LPF_WR0_MSK			NO_OS_GENMASK(3, 0)

/* Specifications */
#define ADMV8818_BUFF_SIZE_BYTES		2
#define ADMV8818_CHIP_ID			NO_OS_BIT(0)
#define ADMV8818_SPI_READ_CMD			NO_OS_BIT(7)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum admv8818_filter_mode
 * @brief Filter Modes of the ADMV8818.
 */
enum admv8818_filter_mode {
	ADMV8818_AUTO,
	ADMV8818_MANUAL,
};

/**
 * @struct admv8818_init_param
 * @brief ADMV8818 Initialization Parameters structure.
 */
struct admv8818_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	/** LO Input Frequency */
	unsigned long long		rf_in;
	/* Filter Mode */
	enum admv8818_filter_mode	mode;
};

/**
 * @struct admv8818_dev
 * @brief ADMV8818 Device Descriptor.
 */
struct admv8818_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** LO Input Frequency */
	unsigned long long		rf_in;
	/* Filter Mode */
	enum admv8818_filter_mode	mode;
};

/** ADMV8818 SPI write */
int admv8818_spi_write(struct admv8818_dev *dev, uint8_t reg_addr,
		       uint8_t data);

/* ADMV8818 Register Update */
int admv8818_spi_update_bits(struct admv8818_dev *dev, uint8_t reg_addr,
			     uint8_t mask, uint8_t data);

/** ADMV8818 SPI Read */
int admv8818_spi_read(struct admv8818_dev *dev, uint8_t reg_addr,
		      uint8_t *data);

/** Set the HPF Frequency */
int admv8818_hpf_select(struct admv8818_dev *dev, unsigned long long freq);

/** Get the HPF Frequency */
int admv8818_read_hpf_freq(struct admv8818_dev *dev, unsigned long long *freq);

/** Set the LPF Frequency */
int admv8818_lpf_select(struct admv8818_dev *dev, unsigned long long freq);

/** Get the LPF Frequency */
int admv8818_read_lpf_freq(struct admv8818_dev *dev, unsigned long long *freq);

/** Set the RF Input Band Select */
int admv8818_rfin_select(struct admv8818_dev *dev);

/** ADMV8818 Initialization */
int admv8818_init(struct admv8818_dev **device,
		  struct admv8818_init_param *init_param);

/** ADMV8818 Resources Deallocation */
int admv8818_remove(struct admv8818_dev *dev);

#endif /* ADMV8818_H_ */
