/***************************************************************************//**
 *   @file   linux_gpio_irq.h
 *   @brief  GPIO IRQ ops for the Linux platform using the GPIO character
 *           device (no libgpiod dependency).
 *   @author Villy Jay Tolentino
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#ifndef LINUX_GPIO_IRQ_H_
#define LINUX_GPIO_IRQ_H_

#include <stdint.h>
#include "no_os_irq.h"

/**
 * @struct linux_gpio_irq_init_param
 * @brief  Linux GPIO IRQ controller specific init parameters.
 *
 * Pass a pointer to this struct via ``no_os_irq_init_param::extra`` when
 * calling ``no_os_irq_ctrl_init()``.
 */
struct linux_gpio_irq_init_param {
	/** GPIO chip number, i.e. N in /dev/gpiochipN. */
	int chip;
};

/**
 * @brief Get the last latched level for a line, as captured by the most
 *        recent edge event (1 == HIGH/rising, 0 == LOW/falling).
 * @param desc   - The IRQ controller descriptor.
 * @param irq_id - The line offset (GPIO number).
 * @param value  - Output: 1 for HIGH, 0 for LOW.
 * @return 0 on success, negative errno otherwise.
 */
int linux_gpio_irq_get_value(struct no_os_irq_ctrl_desc *desc,
			     uint32_t irq_id, uint8_t *value);

/** Linux platform specific GPIO IRQ platform ops structure. */
extern const struct no_os_irq_platform_ops linux_gpio_irq_ops;

#endif /* LINUX_GPIO_IRQ_H_ */