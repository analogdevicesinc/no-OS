/***************************************************************************//**
 *   @file   ad5161.h
 *   @brief  Header file for the ad5161 digital potentiometer drivers
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef AD5161_H_
#define AD5161_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "dpot.h"
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


/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
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
 * @struct ad525x_dpot_dev.
 * @brief ad525x digital potentiometer device descriptor parameters.
 */
struct ad5161_dpot_dev {
	enum dpot_intf_type intf_type;
	/* I2C descriptor */
	struct no_os_i2c_desc *i2c_desc;
	
	struct no_os_spi_desc *spi_desc;
	/* Digital interface select GPIO descriptor */
	struct no_os_gpio_desc *dis_gpio_desc;	
};
#endif	// ad525x_H_
