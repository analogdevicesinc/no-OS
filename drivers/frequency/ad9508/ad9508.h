/***************************************************************************//**
 * @file ad9508.h
 * @brief Header file of AD9508 Driver.
 * @author DHotolea (dan.hotoleanu@analog.com)
 ********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef AD9508_H_
#define AD9508_H_

#include "no_os_spi.h"

#define AD9508_SPI_CONFIG				    0x000
#define AD9508_PART_ID_LOW				    0x00C
#define AD9508_PART_ID_HIGH				    0x00D
#define AD9508_OUT1_DIV_RATIO_LOW			    0x01B
#define AD9508_OUT1_DIV_RATIO_HIGH			    0x01C
#define AD9508_OUT1_PHASE_LOW			   	    0x01D
#define AD9508_OUT1_PHASE_HIGH			   	    0x01E
#define AD9508_PART_ID_VALUE			   	    0x005
#define AD9508_DIVIDE_RATIO_ONE			   	    0x000

struct ad9508_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

struct ad9508_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
};

int32_t ad9508_reg_read(struct ad9508_dev *dev, uint16_t reg_addr,
			uint8_t *reg_data);

int32_t ad9508_reg_write(struct ad9508_dev *dev, uint16_t reg_addr,
			 uint8_t reg_data);

int32_t ad9508_setup(struct ad9508_dev **device,
		     const struct ad9508_init_param *init_param);

int32_t ad9508_remove(struct ad9508_dev *dev);

#endif
