/***************************************************************************//**
 * @file ad9553.h
 * @brief Header file of AD9553 Driver.
 * @author DHotolea (dan.hotoleanu@analog.com)
 ********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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
