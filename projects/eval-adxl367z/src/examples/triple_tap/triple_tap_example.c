/***************************************************************************//**
 *   @file   triple_tap_example.c
 *   @brief  Triple Tap example source for eval-adxl367z project
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include "triple_tap_example.h"
#include "common_data.h"
#include "adxl367.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

struct no_os_irq_ctrl_desc *int1_irq_desc;
bool tap = false;

void int1_callback_fcn(void *ctx)
{
	tap = true;

	no_os_irq_disable(int1_irq_desc, GPIO_INT1_PIN_NUM);
}

/***************************************************************************//**
 * @brief Triple Tap example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int triple_tap_example_main()
{
	uint8_t reg_val;
	int ret = 0;

	/* Device ADXL367 instance. */
	struct adxl367_dev *dev;

	struct no_os_callback_desc int1_cb = {
		.callback = int1_callback_fcn,
		/** Parameter to be passed when the callback is called. */
		.ctx = dev,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = NULL
	};

	/** INT1 Pin Interrupt Controller */
	struct no_os_irq_init_param int1_irq_desc_param = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA
	};

	init_param.spi_init.chip_select = SPI_CS;
	init_param.spi_init.max_speed_hz = SPI_BAUDRATE;
	ret = adxl367_init(&dev, init_param);
	if (ret)
		goto exit;

	ret = adxl367_self_test(dev);
	if (ret)
		goto free_dev;

	ret = adxl367_set_register_value(dev, 50, ADXL367_REG_TAP_THRESH);
	if (ret)
		goto free_dev;

	ret = adxl367_set_register_value(dev, 32, ADXL367_REG_TAP_DUR);
	if (ret)
		goto free_dev;

	ret = adxl367_set_register_value(dev, 16, ADXL367_REG_TAP_LATENT);
	if (ret)
		goto free_dev;

	ret = adxl367_set_register_value(dev, 240, ADXL367_REG_TAP_WINDOW);
	if (ret)
		goto free_dev;

	//map double tap for int 1
	ret = adxl367_set_register_value(dev, 0x03, ADXL367_REG_INTMAP1_UPPER);
	if (ret)
		goto free_dev;

	//enable tap axis for z
	ret = adxl367_set_register_value(dev, 0x20, ADXL367_REG_THRESH_ACT_H);
	if (ret)
		goto free_dev;

	//filter range to 2g
	ret = adxl367_set_range(dev, ADXL367_2G_RANGE);
	if (ret)
		goto free_dev;

	//odr to 100hz
	ret = adxl367_set_output_rate(dev, ADXL367_ODR_100HZ);
	if (ret)
		goto free_dev;

	//measurement mode
	ret = adxl367_set_power_mode(dev, ADXL367_OP_MEASURE);
	if (ret)
		goto free_dev;

	// Configure no-OS interrupt for INT1 pin.
	ret = no_os_irq_ctrl_init(&int1_irq_desc, &int1_irq_desc_param);
	if (ret)
		goto free_dev;

	ret = no_os_irq_register_callback(int1_irq_desc, GPIO_INT1_PIN_NUM,
					  &int1_cb);
	if (ret)
		goto remove_gpio_irq_ctrl;

	ret = no_os_irq_trigger_level_set(int1_irq_desc, GPIO_INT1_PIN_NUM,
					  NO_OS_IRQ_EDGE_RISING);
	if (ret)
		goto gpio_irq_unregister_callback;

	ret = no_os_irq_set_priority(int1_irq_desc, GPIO_INT1_PIN_NUM, 1);
	if (ret)
		goto gpio_irq_unregister_callback;

	while (1) {
		// Enable no-OS interrupt for INT1 pin.
		ret = no_os_irq_enable(int1_irq_desc, GPIO_INT1_PIN_NUM);
		if (ret)
			goto gpio_irq_unregister_callback;

		// Wait for rising edge interrupt to happen.
		while (!tap) { };

		no_os_mdelay(2);
		ret = adxl367_get_register_value(dev, &reg_val, ADXL367_REG_STATUS_2, 1);
		if (ret)
			goto free_dev;

		if (reg_val & NO_OS_BIT(1)) {
			no_os_mdelay(600);
			ret = adxl367_get_register_value(dev, &reg_val, ADXL367_REG_STATUS_2, 1);
			if (ret)
				goto free_dev;

			if (reg_val & NO_OS_BIT(0)) {
				adxl367_get_register_value(dev, &reg_val, ADXL367_REG_STATUS_2, 1);
				no_os_mdelay(2);
				pr_info("Triple Tap!\n");
			} else {
				adxl367_get_register_value(dev, &reg_val, ADXL367_REG_STATUS_2, 1);
				no_os_mdelay(2);
				pr_info("Double Tap!\n");
			}
		} else if (reg_val & NO_OS_BIT(0)) {
			pr_info("Single Tap!\n");
			adxl367_get_register_value(dev, &reg_val, ADXL367_REG_STATUS_2, 1);
			no_os_mdelay(2);
		}
	}

gpio_irq_unregister_callback:
	no_os_irq_unregister_callback(int1_irq_desc, GPIO_INT1_PIN_NUM, &int1_cb);
remove_gpio_irq_ctrl:
	no_os_irq_ctrl_remove(int1_irq_desc);
free_dev:
	adxl367_remove(dev);
exit:
	pr_info("Error! 0x%x\n", ret);
	return ret;
}
