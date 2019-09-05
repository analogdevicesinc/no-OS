/***************************************************************************//**
 *   @file   adum7701.c
 *   @brief  Implementation of adum7701 Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#include "stdio.h"
#include "stdlib.h"
#include "platform_drivers.h"
#include "adum7701.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adum7701_init(struct adum7701_dev **device,
		      struct adum7701_init_param init_param)
{
	struct adum7701_dev *dev;
	int32_t ret;

	dev = (struct adum7701_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* GPIO */
	ret = gpio_get(&dev->dec_ratio, init_param.dec_ratio);
	ret |= gpio_get(&dev->filter_reset, init_param.filter_reset);

	/* Set the decimation ratio */
	ret |= gpio_set_value(dev->dec_ratio, GPIO_HIGH);

	/* Perform a reset */
	ret |= gpio_set_value(dev->filter_reset, GPIO_HIGH);
	ret |= gpio_set_value(dev->filter_reset, GPIO_LOW);

	*device = dev;

	if (!ret)
		printf("adum7701 successfully initialized\n");
	mdelay(1000);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adum7701_init().
 * @param dev - The device structure.
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t adum7701_remove(struct adum7701_dev *dev)
{
	int32_t ret;

	ret = gpio_remove(dev->dec_ratio);
	ret |= gpio_remove(dev->filter_reset);

	free(dev);

	return ret;
}
