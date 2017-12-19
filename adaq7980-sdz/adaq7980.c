/***************************************************************************//**
 * @file adaq7980.c
 * @brief Implementation adaq7980 Driver.
 * @author ADI
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "platform_drivers.h"
#include "adaq7980.h"

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adaq7980_setup(adaq7980_dev **device,
					 adaq7980_init_param init_param)
{
	adaq7980_dev *dev;
	int32_t ret = 0;

	dev = (adaq7980_dev *)malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	dev->spi_dev.chip_select = init_param.spi_chip_select;
	dev->spi_dev.mode = init_param.spi_mode;
	dev->spi_dev.device_id = init_param.spi_device_id;
	dev->spi_dev.type = init_param.spi_type;

	dev->gpio_dev.device_id = init_param.gpio_device_id;
	dev->gpio_dev.type = init_param.gpio_type;
	ret |= gpio_init(&dev->gpio_dev);

	dev->gpio_pd_ldo = init_param.gpio_pd_ldo;
	ret |= gpio_set_direction(&dev->gpio_dev, dev->gpio_pd_ldo, GPIO_OUT);
	ret |= gpio_set_value(&dev->gpio_dev, dev->gpio_pd_ldo, GPIO_LOW);
	mdelay(10);
	ret |= gpio_set_value(&dev->gpio_dev, dev->gpio_pd_ldo, GPIO_HIGH);
	mdelay(10);

	*device = dev;

	if (!ret)
		printf("adaq7980 successfully initialized\n");

	return ret;
}
