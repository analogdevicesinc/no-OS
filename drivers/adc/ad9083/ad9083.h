/***************************************************************************//**
 *   @file   ad9083.h
 *   @brief  Header file of ad9083 Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#ifndef __AD9083_H__
#define __AD9083_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "adi_ad9083.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct ad9083_init_param
 * @brief Structure holding the parameters for ad9083 initialization.
 */
struct ad9083_init_param {
	/* SPI */
	struct no_os_spi_init_param	*spi_init;
	/* GPIO reset */
	struct no_os_gpio_init_param	*gpio_reset;
	/* GPIO power down */
	struct no_os_gpio_init_param	*gpio_pd;
	/* Settings selection */
	uint8_t uc;
	/* jesd receive clock */
	struct no_os_clk *jesd_rx_clk;
};

/**
 * @struct ad9083_phy
 * @brief Structure holding ad9083 descriptor.
 */
struct ad9083_phy {
	/* SPI */
	struct no_os_spi_desc 	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_reset;
	/* GPIO power down */
	struct no_os_gpio_desc	*gpio_pd;
	/* GPIO reference selection */
	struct no_os_gpio_desc	*gpio_ref_sel;
	/* adi ad9083 device*/
	adi_ad9083_device_t	adi_ad9083;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the device. */
int32_t ad9083_init(struct ad9083_phy **device,
		    struct ad9083_init_param *init_param);

/* Remove the device. */
int32_t ad9083_remove(struct ad9083_phy *device);

/* Read device register. */
int32_t ad9083_reg_get(struct ad9083_phy *device, uint32_t reg,
		       uint8_t *readval);

/* Write device register. */
int32_t ad9083_reg_set(struct ad9083_phy *device, uint32_t reg,
		       uint8_t writeval);

#endif // __AD9083_H__
