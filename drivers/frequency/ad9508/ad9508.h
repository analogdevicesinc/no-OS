/***************************************************************************//**
 * @file ad9508.h
 * @brief Header file of AD9508 Driver.
 * @author DHotolea (dan.hotoleanu@analog.com)
 ********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AD9508_H_
#define AD9508_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD9508_SPI_CONFIG				    0x000
#define AD9508_PART_ID_LOW				    0x00C
#define AD9508_PART_ID_HIGH				    0x00D
#define AD9508_OUT1_DIV_RATIO_LOW			    0x01B
#define AD9508_OUT1_DIV_RATIO_HIGH			    0x01C
#define AD9508_OUT1_PHASE_LOW			   	    0x01D
#define AD9508_OUT1_PHASE_HIGH			   	    0x01E
#define AD9508_PART_ID_VALUE			   	    0x005
#define AD9508_DIVIDE_RATIO_ONE			   	    0x000

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ad9508_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

struct ad9508_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t ad9508_reg_read(struct ad9508_dev *dev, uint16_t reg_addr,
			uint8_t *reg_data);

int32_t ad9508_reg_write(struct ad9508_dev *dev, uint16_t reg_addr,
			 uint8_t reg_data);

int32_t ad9508_setup(struct ad9508_dev **device,
		     const struct ad9508_init_param *init_param);

int32_t ad9508_remove(struct ad9508_dev *dev);

#endif
