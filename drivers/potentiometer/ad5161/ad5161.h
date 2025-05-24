/***************************************************************************//**
 *   @file   ad5161.h
 *   @brief  Header file for the ad5161 digital potentiometer drivers
********************************************************************************
Copyright 2025(c) Analog Devices, Inc.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef AD5161_H_
#define AD5161_H_

#include "../common/dpot.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/* Control word for READ and WRITE to AD5161*/
#define AD5161_CTL_FOR_READ_WRITE 0x00
/* Control word for placing the AD5161 in shutdown condition*/
#define AD5161_CTL_FOR_EN_SHUTWOWN 0x20
/* Control word for placing the AD5161 in normal condition*/
#define AD5161_CTL_FOR_DIS_SHUTWOWN 0x80
/* Control word for placing the AD5161 in reset state(mid range ) */
#define AD5161_CTL_FOR_RESET_POSITION 0x40

/**
 * @struct ad516x_dpot_init_param.
 * @brief ad516x digital potentiometer init parameters.
 */
struct ad516x_dpot_init_param {
	/* Interface type */
	enum dpot_intf_type eintf_type;
	/* SPI init parameters */
	struct no_os_spi_init_param *spi_init;
	/* I2C init parameters */
	struct no_os_i2c_init_param *i2c_init;
	/* Digital interface select GPIO init parameters */
	struct no_os_gpio_init_param *dis_gpio_init;
};



/**
 * @struct ad5161_dpot_dev.
 * @brief ad5161 digital potentiometer device descriptor parameters.
 */
struct ad5161_dpot_dev {
	enum dpot_intf_type intf_type;
	/* I2C descriptor */
	struct no_os_i2c_desc *i2c_desc;

	struct no_os_spi_desc *spi_desc;
	/* Digital interface select GPIO descriptor */
	struct no_os_gpio_desc *dis_gpio_desc;
};
#endif	// ad5161_H_
