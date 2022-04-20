/***************************************************************************//**
 *   @file   adrf6780.h
 *   @brief  Header file for adrf6780 Driver.
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

#ifndef ADRF6780_H_
#define ADRF6780_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* ADRF6780 Register Map */
#define ADRF6780_REG_CONTROL			0x00
#define ADRF6780_REG_ALARM_READBACK		0x01
#define ADRF6780_REG_ALARM_MASKS		0x02
#define ADRF6780_REG_ENABLE			0x03
#define ADRF6780_REG_LINEARIZE			0x04
#define ADRF6780_REG_LO_PATH			0x05
#define ADRF6780_REG_ADC_CONTROL		0x06
#define ADRF6780_REG_ADC_OUTPUT			0x0C

/* ADRF6780_REG_CONTROL Map */
#define ADRF6780_PARITY_EN_MSK			NO_OS_BIT(15)
#define ADRF6780_SOFT_RESET_MSK			NO_OS_BIT(14)
#define ADRF6780_CHIP_ID_MSK			NO_OS_GENMASK(11, 4)
#define ADRF6780_CHIP_ID			0xA
#define ADRF6780_CHIP_REVISION_MSK		NO_OS_GENMASK(3, 0)

/* ADRF6780_REG_ALARM_READBACK Map */
#define ADRF6780_PARITY_ERROR_MSK		NO_OS_BIT(15)
#define ADRF6780_TOO_FEW_ERRORS_MSK		NO_OS_BIT(14)
#define ADRF6780_TOO_MANY_ERRORS_MSK		NO_OS_BIT(13)
#define ADRF6780_ADDRESS_RANGE_ERROR_MSK	NO_OS_BIT(12)

/* ADRF6780_REG_ENABLE Map */
#define ADRF6780_VGA_BUFFER_EN_MSK		NO_OS_BIT(8)
#define ADRF6780_DETECTOR_EN_MSK		NO_OS_BIT(7)
#define ADRF6780_LO_BUFFER_EN_MSK		NO_OS_BIT(6)
#define ADRF6780_IF_MODE_EN_MSK			NO_OS_BIT(5)
#define ADRF6780_IQ_MODE_EN_MSK			NO_OS_BIT(4)
#define ADRF6780_LO_X2_EN_MSK			NO_OS_BIT(3)
#define ADRF6780_LO_PPF_EN_MSK			NO_OS_BIT(2)
#define ADRF6780_LO_EN_MSK			NO_OS_BIT(1)
#define ADRF6780_UC_BIAS_EN_MSK			NO_OS_BIT(0)

/* ADRF6780_REG_LINEARIZE Map */
#define ADRF6780_RDAC_LINEARIZE_MSK		NO_OS_GENMASK(7, 0)

/* ADRF6780_REG_LO_PATH Map */
#define ADRF6780_LO_SIDEBAND_MSK		NO_OS_BIT(10)
#define ADRF6780_Q_PATH_PHASE_ACCURACY_MSK	NO_OS_GENMASK(7, 4)
#define ADRF6780_I_PATH_PHASE_ACCURACY_MSK	NO_OS_GENMASK(3, 0)

/* ADRF6780_REG_ADC_CONTROL Map */
#define ADRF6780_VDET_OUTPUT_SELECT_MSK		NO_OS_BIT(3)
#define ADRF6780_ADC_START_MSK			NO_OS_BIT(2)
#define ADRF6780_ADC_EN_MSK			NO_OS_BIT(1)
#define ADRF6780_ADC_CLOCK_EN_MSK		NO_OS_BIT(0)

/* ADRF6780_REG_ADC_OUTPUT Map */
#define ADRF6780_ADC_STATUS_MSK			NO_OS_BIT(8)
#define ADRF6780_ADC_VALUE_MSK			NO_OS_GENMASK(7, 0)

/* Specifications */
#define ADRF6780_BUFF_SIZE_BYTES		3
#define ADRF6780_SPI_READ_CMD			NO_OS_BIT(7)
#define ADRF6780_SPI_WRITE_CMD			(0 << 7)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct adrf6780_init_param
 * @brief ADRF6780 Initialization Parameters structure.
 */
struct adrf6780_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	/** LO Input Frequency */
	unsigned long long		lo_in;
	/** VGA Buffer Enable */
	bool				vga_buff_en;
	/** LO Buffer Enable */
	bool				lo_buff_en;
	/** IF Mode Enable */
	bool				if_mode_en;
	/** LO x2 Enable */
	bool				lo_x2_en;
	/** LO Enable */
	bool				lo_en;
	/** UC Bias Enable */
	bool				uc_bias_en;
	/** LO Sideband */
	bool				lo_sideband;
	/** VDET Output Select Enable */
	bool				vdet_out_en;
};

/**
 * @struct adrf6780_dev
 * @brief ADRF6780 Device Descriptor.
 */
struct adrf6780_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** LO Input Frequency */
	unsigned long long		lo_in;
	/** VGA Buffer Enable */
	bool				vga_buff_en;
	/** LO Buffer Enable */
	bool				lo_buff_en;
	/** IF Mode Enable */
	bool				if_mode_en;
	/** LO x2 Enable */
	bool				lo_x2_en;
	/** LO Enable */
	bool				lo_en;
	/** UC Bias Enable */
	bool				uc_bias_en;
	/** LO Sideband */
	bool				lo_sideband;
	/** VDET Output Select Enable */
	bool				vdet_out_en;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** ADRF6780 SPI write */
int adrf6780_spi_write(struct adrf6780_dev *dev, uint8_t reg_addr,
		       uint16_t data);

/* ADRF6780 Register Update */
int adrf6780_spi_update_bits(struct adrf6780_dev *dev, uint8_t reg_addr,
			     uint16_t mask, uint16_t data);

/** ADRF6780 SPI Read */
int adrf6780_spi_read(struct adrf6780_dev *dev, uint8_t reg_addr,
		      uint16_t *data);

/** Set IMD Performance Improvement */
int adrf6780_set_rdac_linearize(struct adrf6780_dev *dev, uint8_t rdac_lin);

/** Get IMD Performance Improvement */
int adrf6780_get_rdac_linearize(struct adrf6780_dev *dev, uint8_t *rdac_lin);

/** Set I/Q Phase Accuracy */
int adrf6780_set_cdac_iq_phase_accuracy(struct adrf6780_dev *dev,
					uint8_t i_data, uint8_t q_data);

/** Get I/Q Phase Accuracy */
int adrf6780_get_cdac_iq_phase_accuracy(struct adrf6780_dev *dev,
					uint8_t *i_data, uint8_t *q_data);

/** ADRF6780 Read ADC Raw Data */
int adrf6780_read_adc_raw(struct adrf6780_dev *dev, uint16_t *data);

/* Software Reset */
int adrf6780_soft_reset(struct adrf6780_dev *dev);

/** ADRF6780 Initialization */
int adrf6780_init(struct adrf6780_dev **device,
		  struct adrf6780_init_param *init_param);

/** ADRF6780 Resources Deallocation */
int adrf6780_remove(struct adrf6780_dev *dev);

#endif /* ADRF6780_H_ */
