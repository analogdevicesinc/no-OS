/***************************************************************************//**
 *   @file   projects/adf4377_sdz/src/adf4377_sdz.c
 *   @brief  Implementation of Main Function.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>
#include "spi.h"
#include "spi_extra.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "error.h"
#include "adf4377.h"
#include "parameters.h"

#define LOG_LEVEL 6
#include "print_log.h"

int main(void)
{
	int32_t ret;
	struct adf4377_dev *dev;

	struct xil_spi_init_param xil_spi_init = {
		.device_id = SPI_DEVICE_ID,
		.flags = 0,
		.type = SPI_PS
	};

	struct xil_gpio_init_param xil_gpio_init = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS
	};

	struct gpio_init_param gpio_ce_param = {
		.number = GPIO_CE,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_init
	};

	struct spi_init_param spi_init = {
		.max_speed_hz = 2000000,
		.chip_select = SPI_ADF4377_CS,
		.mode = SPI_MODE_0,
		.bit_order = SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &xil_platform_ops,
		.extra = &xil_spi_init
	};

	struct adf4377_init_param adf4377_param = {
		/* SPI Initialization Structure */
		.spi_init = &spi_init,
		/* GPIO Chip Enable Initialization Structure */
		.gpio_ce_param = &gpio_ce_param,
	};

	ret = adf4377_init(&dev, &adf4377_param);
	if (ret != SUCCESS) {
		pr_err("ADF4377 Initialization failed!\n");
		return FAILURE;
	}

	ret = adf4377_remove(dev);

	return ret;
}
