/***************************************************************************//**
 * @file ad9553.h
 * @brief Header file of AD9553 Driver.
 * @author DHotolea (dan.hotoleanu@analog.com)
 ********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AD9553_H_
#define AD9553_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD9553_SPI_CONFIG				        0x000
#define AD9553_PLL_CHARGE_PUMP_PFD_CTRL	        		0x00B
#define AD9553_PLL_CTRL	        				0x00D
#define AD9553_P1_DIV_HIGH     					0x015
#define AD9553_P1_DIV_LOW_P2_DIV_HIGH   			0x016
#define AD9553_P2_DIV_LOW				   	0x017
#define AD9553_P0_DIV					   	0x018
#define AD9553_N_DIV_HIGH				   	0x012
#define AD9553_N_DIV_MEDIUM				   	0x013
#define AD9553_N_DIV_LOW				   	0x014
#define AD9553_REFA_DIV_HIGH				   	0x01F
#define AD9553_REFA_DIV_LOW					0x020
#define AD9553_K_VALUE					   	0x021
#define AD9553_REFA_DIFF				   	0x029
#define AD9553_OUT1_DRIVER_CTRL			   		0x032
#define AD9553_OUT2_DRIVER_CTRL			   		0x034
#define AD9553_IO_UPDATE				   	0x005

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ad9553_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

struct ad9553_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t ad9553_reg_read(struct ad9553_dev *dev, uint16_t reg_addr,
			uint8_t *reg_data);

int32_t ad9553_reg_write(struct ad9553_dev *dev, uint16_t reg_addr,
			 uint8_t reg_data);

int32_t ad9553_setup(struct ad9553_dev **device,
		     const struct ad9553_init_param *init_param);

int32_t ad9553_remove(struct ad9553_dev *dev);

#endif
