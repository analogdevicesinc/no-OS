/***************************************************************************//**
 *   @file   iio_k_gpio.h
 *   @brief  IIO device exposing GPIO lines (read/write + direction).
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright (C) 2026 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef IIO_K_GPIO_H_
#define IIO_K_GPIO_H_

#include <stdint.h>

struct iio_device;
struct no_os_gpio_desc;
struct no_os_gpio_init_param;

#define IIO_K_GPIO_MAX_LINES	8

/**
 * @struct iio_k_gpio_line
 * @brief  One exposed GPIO line.
 */
struct iio_k_gpio_line {
	const char			*name;
	struct no_os_gpio_init_param	*init_param;	/* provides .number + ops */
};

/**
 * @struct iio_k_gpio_desc
 * @brief  State for the k-gpio IIO device.
 */
struct iio_k_gpio_desc {
	struct no_os_gpio_desc	*gpio[IIO_K_GPIO_MAX_LINES];
	uint32_t		num_lines;
};

struct iio_k_gpio_init_param {
	struct iio_k_gpio_line	lines[IIO_K_GPIO_MAX_LINES];
	uint32_t		num_lines;
};

int iio_k_gpio_init(struct iio_k_gpio_desc **desc,
		    const struct iio_k_gpio_init_param *init);
void iio_k_gpio_remove(struct iio_k_gpio_desc *desc);
void iio_k_gpio_get_dev_descriptor(struct iio_k_gpio_desc *desc,
				   struct iio_device **dev_descriptor);

#endif /* IIO_K_GPIO_H_ */
