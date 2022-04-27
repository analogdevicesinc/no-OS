/***************************************************************************//**
 *   @file   admv1013.h
 *   @brief  Header file for admv1013 Driver.
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

#ifndef ADMV1013_H_
#define ADMV1013_H_

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

/* ADMV1013 Register Map */
#define ADMV1013_REG_SPI_CONTROL		0x00
#define ADMV1013_REG_ALARM			0x01
#define ADMV1013_REG_ALARM_MASKS		0x02
#define ADMV1013_REG_ENABLE			0x03
#define ADMV1013_REG_LO_AMP_I			0x05
#define ADMV1013_REG_LO_AMP_Q			0x06
#define ADMV1013_REG_OFFSET_ADJUST_I		0x07
#define ADMV1013_REG_OFFSET_ADJUST_Q		0x08
#define ADMV1013_REG_QUAD			0x09
#define ADMV1013_REG_VVA_TEMP_COMP		0x0A

/* ADMV1013_REG_SPI_CONTROL Map */
#define ADMV1013_PARITY_EN_MSK			NO_OS_BIT(15)
#define ADMV1013_SPI_SOFT_RESET_MSK		NO_OS_BIT(14)
#define ADMV1013_CHIP_ID_MSK			NO_OS_GENMASK(11, 4)
#define ADMV1013_CHIP_ID			0xA
#define ADMV1013_REVISION_ID_MSK		NO_OS_GENMASK(3, 0)

/* ADMV1013_REG_ALARM Map */
#define ADMV1013_PARITY_ERROR_MSK		NO_OS_BIT(15)
#define ADMV1013_TOO_FEW_ERRORS_MSK		NO_OS_BIT(14)
#define ADMV1013_TOO_MANY_ERRORS_MSK		NO_OS_BIT(13)
#define ADMV1013_ADDRESS_RANGE_ERROR_MSK	NO_OS_BIT(12)

/* ADMV1013_REG_ENABLE Map */
#define ADMV1013_VGA_PD_MSK			NO_OS_BIT(15)
#define ADMV1013_MIXER_PD_MSK			NO_OS_BIT(14)
#define ADMV1013_QUAD_PD_MSK			NO_OS_GENMASK(13, 11)
#define ADMV1013_BG_PD_MSK			NO_OS_BIT(10)
#define ADMV1013_MIXER_IF_EN_MSK		NO_OS_BIT(7)
#define ADMV1013_DET_EN_MSK			NO_OS_BIT(5)

/* ADMV1013_REG_LO_AMP Map */
#define ADMV1013_LOAMP_PH_ADJ_FINE_MSK		NO_OS_GENMASK(13, 7)
#define ADMV1013_MIXER_VGATE_MSK		NO_OS_GENMASK(6, 0)

/* ADMV1013_REG_OFFSET_ADJUST Map */
#define ADMV1013_MIXER_OFF_ADJ_P_MSK		NO_OS_GENMASK(15, 9)
#define ADMV1013_MIXER_OFF_ADJ_N_MSK		NO_OS_GENMASK(8, 2)

/* ADMV1013_REG_QUAD Map */
#define ADMV1013_QUAD_SE_MODE_MSK		NO_OS_GENMASK(9, 6)
#define ADMV1013_QUAD_FILTERS_MSK		NO_OS_GENMASK(3, 0)

/* ADMV1013_REG_VVA_TEMP_COMP Map */
#define ADMV1013_VVA_TEMP_COMP_MSK		NO_OS_GENMASK(15, 0)

/* Specifications */
#define ADMV1013_BUFF_SIZE_BYTES		3
#define ADMV1013_SPI_READ_CMD			NO_OS_BIT(7)
#define ADMV1013_SPI_WRITE_CMD			(0 << 7)

#define MIXER_GATE_0_to_1_8_V(x)		((2389 * x/ 1000000 + 8100) / 100)
#define MIXER_GATE_1_8_to_2_6_V(x)		((2375 * x/ 1000000 + 125) / 100)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum admv1013_input_mode
 * @brief Switch Intermediate Frequency or I/Q Mode
 */
enum admv1013_input_mode {
	ADMV1013_IQ_MODE,
	ADMV1013_IF_MODE
};

/**
 * @enum admv1013_quad_se_mode
 * @brief Switch Differential/Single-Ended Modes
 */
enum admv1013_quad_se_mode {
	ADMV1013_SE_MODE_POS = 6,
	ADMV1013_SE_MODE_NEG = 9,
	ADMV1013_SE_MODE_DIFF = 12
};

/**
 * @enum admv1013_quad_filters
 * @brief LO Filters BW Selection
 */
enum admv1013_quad_filters {
	LO_BAND_8_62_TO_10_25_GHZ = 0,
	LO_BAND_6_6_TO_9_2_GHZ = 5,
	LO_BAND_5_4_TO_8_GHZ = 10,
	LO_BAND_5_4_TO_7_GHZ = 15
};

/**
 * @struct admv1013_init_param
 * @brief ADMV1013 Initialization Parameters structure.
 */
struct admv1013_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	/** LO Input Frequency */
	unsigned long long		lo_in;
	/** Input Mode */
	enum admv1013_input_mode	input_mode;
	/** Quad SE Mode */
	enum admv1013_quad_se_mode	quad_se_mode;
	/** Detector Enable */
	bool				det_en;
	/** Common-Mode Voltage (uV) */
	unsigned int			vcm_uv;
};

/**
 * @struct admv1013_dev
 * @brief ADMV1013 Device Descriptor.
 */
struct admv1013_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** LO Input Frequency */
	unsigned long long		lo_in;
	/** Input Mode */
	enum admv1013_input_mode	input_mode;
	/** Quad SE Mode */
	enum admv1013_quad_se_mode	quad_se_mode;
	/** Detector Enable */
	bool				det_en;
	/** Common-Mode Voltage (uV) */
	unsigned int			vcm_uv;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** ADMV1013 SPI write */
int admv1013_spi_write(struct admv1013_dev *dev, uint8_t reg_addr,
		       uint16_t data);

/* ADMV1013 Register Update */
int admv1013_spi_update_bits(struct admv1013_dev *dev, uint8_t reg_addr,
			     uint16_t mask, uint16_t data);

/** ADMV1013 SPI Read */
int admv1013_spi_read(struct admv1013_dev *dev, uint8_t reg_addr,
		      uint16_t *data);

/** Set I/Q Phase Accuracy */
int admv1013_set_iq_phase(struct admv1013_dev *dev, uint8_t i_phase,
			  uint8_t q_phase);

/** Get I/Q Phase Accuracy */
int admv1013_get_iq_phase(struct admv1013_dev *dev, uint8_t *i_phase,
			  uint8_t *q_phase);

/** Set I/Q Offset Accuracy */
int admv1013_set_iq_offset(struct admv1013_dev *dev, uint8_t i_offset_p,
			   uint8_t i_offset_n, uint8_t q_offset_p,
			   uint8_t q_offset_n);

/** Get I/Q Offset Accuracy */
int admv1013_get_iq_offset(struct admv1013_dev *dev, uint8_t *i_offset_p,
			   uint8_t *i_offset_n, uint8_t *q_offset_p,
			   uint8_t *q_offset_n);

/** ADMV1013 Initialization */
int admv1013_init(struct admv1013_dev **device,
		  struct admv1013_init_param *init_param);

/** ADMV1013 Resources Deallocation */
int admv1013_remove(struct admv1013_dev *dev);

#endif /* ADMV1013_H_ */
