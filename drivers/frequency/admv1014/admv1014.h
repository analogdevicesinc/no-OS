/***************************************************************************//**
 *   @file   admv1014.h
 *   @brief  Header file for admv1014 Driver.
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

#ifndef ADMV1014_H_
#define ADMV1014_H_

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
/* ADMV1014 Register Map */
#define ADMV1014_REG_SPI_CONTROL		0x00
#define ADMV1014_REG_ALARM			0x01
#define ADMV1014_REG_ALARM_MASKS		0x02
#define ADMV1014_REG_ENABLE			0x03
#define ADMV1014_REG_QUAD			0x04
#define ADMV1014_REG_LO_AMP_PHASE_ADJUST1	0x05
#define ADMV1014_REG_MIXER			0x07
#define ADMV1014_REG_IF_AMP			0x08
#define ADMV1014_REG_IF_AMP_BB_AMP		0x09
#define ADMV1014_REG_BB_AMP_AGC			0x0A
#define ADMV1014_REG_VVA_TEMP_COMP		0x0B

/* ADMV1014_REG_SPI_CONTROL Map */
#define ADMV1014_PARITY_EN_MSK			NO_OS_BIT(15)
#define ADMV1014_SPI_SOFT_RESET_MSK		NO_OS_BIT(14)
#define ADMV1014_CHIP_ID_MSK			NO_OS_GENMASK(11, 4)
#define ADMV1014_CHIP_ID			0x9
#define ADMV1014_REVISION_ID_MSK		NO_OS_GENMASK(3, 0)

/* ADMV1014_REG_ALARM Map */
#define ADMV1014_PARITY_ERROR_MSK		NO_OS_BIT(15)
#define ADMV1014_TOO_FEW_ERRORS_MSK		NO_OS_BIT(14)
#define ADMV1014_TOO_MANY_ERRORS_MSK		NO_OS_BIT(13)
#define ADMV1014_ADDRESS_RANGE_ERROR_MSK	NO_OS_BIT(12)

/* ADMV1014_REG_ENABLE Map */
#define ADMV1014_IBIAS_PD_MSK			NO_OS_BIT(14)
#define ADMV1014_P1DB_COMPENSATION_MSK		NO_OS_GENMASK(13, 12)
#define ADMV1014_IF_AMP_PD_MSK			NO_OS_BIT(11)
#define ADMV1014_QUAD_BG_PD_MSK			NO_OS_BIT(9)
#define ADMV1014_BB_AMP_PD_MSK			NO_OS_BIT(8)
#define ADMV1014_QUAD_IBIAS_PD_MSK		NO_OS_BIT(7)
#define ADMV1014_DET_EN_MSK			NO_OS_BIT(6)
#define ADMV1014_BG_PD_MSK			NO_OS_BIT(5)

/* ADMV1014_REG_QUAD Map */
#define ADMV1014_QUAD_SE_MODE_MSK		NO_OS_GENMASK(9, 6)
#define ADMV1014_QUAD_FILTERS_MSK		NO_OS_GENMASK(3, 0)

/* ADMV1014_REG_LO_AMP_PHASE_ADJUST1 Map */
#define ADMV1014_LOAMP_PH_ADJ_I_FINE_MSK	NO_OS_GENMASK(15, 9)
#define ADMV1014_LOAMP_PH_ADJ_Q_FINE_MSK	NO_OS_GENMASK(8, 2)

/* ADMV1014_REG_MIXER Map */
#define ADMV1014_MIXER_VGATE_MSK		NO_OS_GENMASK(15, 9)
#define ADMV1014_DET_PROG_MSK			NO_OS_GENMASK(6, 0)

/* ADMV1014_REG_IF_AMP Map */
#define ADMV1014_IF_AMP_COARSE_GAIN_I_MSK	NO_OS_GENMASK(11, 8)
#define ADMV1014_IF_AMP_FINE_GAIN_Q_MSK		NO_OS_GENMASK(7, 4)
#define ADMV1014_IF_AMP_FINE_GAIN_I_MSK		NO_OS_GENMASK(3, 0)

/* ADMV1014_REG_IF_AMP_BB_AMP Map */
#define ADMV1014_IF_AMP_COARSE_GAIN_Q_MSK	NO_OS_GENMASK(15, 12)
#define ADMV1014_BB_AMP_OFFSET_Q_MSK		NO_OS_GENMASK(9, 5)
#define ADMV1014_BB_AMP_OFFSET_I_MSK		NO_OS_GENMASK(4, 0)

/* ADMV1014_REG_BB_AMP_AGC Map */
#define ADMV1014_BB_AMP_REF_GEN_MSK		NO_OS_GENMASK(6, 3)
#define ADMV1014_BB_AMP_GAIN_CTRL_MSK		NO_OS_GENMASK(2, 1)
#define ADMV1014_BB_SWITCH_HIGH_LOW_CM_MSK	NO_OS_BIT(0)

/* ADMV1014_REG_VVA_TEMP_COMP Map */
#define ADMV1014_VVA_TEMP_COMP_MSK		NO_OS_GENMASK(15, 0)

/* ADMV1014 Miscellaneous Defines */
#define ADMV1014_READ				NO_OS_BIT(7)
#define ADMV1014_REG_ADDR_READ_MSK		NO_OS_GENMASK(6, 1)
#define ADMV1014_REG_ADDR_WRITE_MSK		NO_OS_GENMASK(22, 17)
#define ADMV1014_REG_DATA_MSK			NO_OS_GENMASK(16, 1)
#define ADMV1014_NUM_REGULATORS			9

/* Specifications */
#define ADMV1014_BUFF_SIZE_BYTES		3
#define ADMV1014_SPI_READ_CMD			NO_OS_BIT(7)
#define ADMV1014_SPI_WRITE_CMD			(0 << 7)

/**
 * @enum admv1014_input_mode
 * @brief Switch Intermediate Frequency or I/Q Mode
 */
enum admv1014_input_mode {
	ADMV1014_IQ_MODE,
	ADMV1014_IF_MODE,
};

/**
 * @enum admv1014_quad_se_mode
 * @brief Switch Differential/Single-Ended Modes
 */
enum admv1014_quad_se_mode {
	ADMV1014_SE_MODE_POS = 6,
	ADMV1014_SE_MODE_NEG = 9,
	ADMV1014_SE_MODE_DIFF = 12,
};

/**
 * @enum admv1014_quad_filters
 * @brief LO Filters BW Selection
 */
enum admv1014_quad_filters {
	LO_BAND_8_62_TO_10_25_GHZ = 0,
	LO_BAND_6_6_TO_9_2_GHZ = 5,
	LO_BAND_5_4_TO_8_GHZ = 10,
	LO_BAND_5_4_TO_7_GHZ = 15
};

/**
 * @enum admv1014_det_prog
 * @brief Digital Rx Detector Program.
 */
enum admv1014_det_prog {
	DET_PROG_NEG_12_DBM_TO_POS_4DBM = 0,
	DET_PROG_NEG_13_DBM_TO_POS_3DBM = 1,
	DET_PROG_NEG_14_DBM_TO_POS_2DBM = 2,
	DET_PROG_NEG_15_DBM_TO_POS_1DBM = 4,
	DET_PROG_NEG_15_5_DBM_TO_POS_0_5_DBM = 8,
	DET_PROG_NEG_16_25_DBM_TO_NEG_0_25_DBM = 16,
	DET_PROG_NEG_17_DBM_TO_NEG_1DBM = 32,
	DET_PROG_NEG_18_DBM_TO_NEG_2DBM = 64,
};
/**
 * @struct admv1014_init_param
 * @brief ADMV1014 Initialization Parameters structure.
 */
struct admv1014_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	/** LO Input Frequency */
	unsigned long long		lo_in;
	/** Input Mode */
	enum admv1014_input_mode	input_mode;
	/** Quad SE Mode */
	enum admv1014_quad_se_mode	quad_se_mode;
	/** Detector Enable */
	bool				det_en;
	/** Common-Mode Voltage (mV) */
	unsigned int			vcm_mv;
	/** P1DB Compensation Enable */
	bool				p1db_comp_en;
};

/**
 * @struct admv1014_dev
 * @brief ADMV1014 Device Descriptor.
 */
struct admv1014_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** LO Input Frequency */
	unsigned long long		lo_in;
	/** Input Mode */
	enum admv1014_input_mode	input_mode;
	/** Quad SE Mode */
	enum admv1014_quad_se_mode	quad_se_mode;
	/** Detector Enable */
	bool				det_en;
	/** Common-Mode Voltage (mV) */
	unsigned int			vcm_mv;
	/** P1DB Compensation Enable */
	bool				p1db_comp_en;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** ADMV1014 SPI write */
int admv1014_spi_write(struct admv1014_dev *dev, uint8_t reg_addr,
		       uint16_t data);

/* ADMV1014 Register Update */
int admv1014_spi_update_bits(struct admv1014_dev *dev, uint8_t reg_addr,
			     uint16_t mask, uint16_t data);

/** ADMV1014 SPI Read */
int admv1014_spi_read(struct admv1014_dev *dev, uint8_t reg_addr,
		      uint16_t *data);

/** Set Digital Rx Detector */
int admv1014_set_det_prog(struct admv1014_dev *dev,
			  enum admv1014_det_prog det_prog);

/** Get Digital Rx Detector */
int admv1014_get_det_prog(struct admv1014_dev *dev,
			  enum admv1014_det_prog *det_prog);

/** Set Baseband Amp Gain */
int admv1014_set_bb_amp_gain(struct admv1014_dev *dev, uint8_t gain);

/** Get Baseband Amp Gain */
int admv1014_get_bb_amp_gain(struct admv1014_dev *dev, uint8_t *gain);

/** Set LO Amp Phase */
int admv1014_set_phase(struct admv1014_dev *dev, uint8_t i_phase,
		       uint8_t q_phase);

/** Get LO Amp Phase */
int admv1014_get_phase(struct admv1014_dev *dev, uint8_t *i_phase,
		       uint8_t *q_phase);

/** Set IF Amp Gain */
int admv1014_set_if_amp_gain(struct admv1014_dev *dev, uint8_t i_coarse_gain,
			     uint8_t q_coarse_gain, uint8_t i_fine_gain, uint8_t q_fine_gain);

/** Get IF Amp Gain */
int admv1014_get_if_amp_gain(struct admv1014_dev *dev, uint8_t *i_coarse_gain,
			     uint8_t *q_coarse_gain, uint8_t *i_fine_gain, uint8_t *q_fine_gain);

/** ADMV1014 Initialization */
int admv1014_init(struct admv1014_dev **device,
		  struct admv1014_init_param *init_param);

/** ADMV1014 Resources Deallocation */
int admv1014_remove(struct admv1014_dev *dev);

#endif /* ADMV1014_H_ */
