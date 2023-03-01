/***************************************************************************//**
 *   @file   max11205.c
 *   @brief  Implementation of max11205.c
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "max11205.h"
#include "errno.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief GPIO interrupt handler for data ready.
 * @param dev - The device structure.
 */
static void max11205_irq_handler(void *dev)
{
	struct max11205_dev *desc = dev;
	uint8_t data[2] = {0};
	int ret;

	if (!dev)
		return;

	/* MISO line is used also as data ready signal, thus GPIO irq has to
	   be disabled while receiving data over MISO. */
	ret = no_os_irq_disable(desc->irq_ctrl, desc->gpio_rdy->number);
	if (ret)
		return;

	ret = no_os_spi_write_and_read(desc->spi_desc, data, MAX11205_DATA_SIZE_BYTES);
	if (ret)
		return;

	desc->adc_data_raw = (int16_t)no_os_get_unaligned_be16(data);
	desc->data_updated = true;

	ret = no_os_irq_enable(desc->irq_ctrl,
			       desc->gpio_rdy->number);
	if (ret)
		return;
}

/**
 * @brief Initializes the communication peripheral.
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		               parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int max11205_init(struct max11205_dev **device,
		  struct max11205_init_param init_param)
{
	struct max11205_dev *dev;
	int ret;

	if (!init_param.irq_ctrl)
		return -EINVAL;

	if (init_param.vref_mv > MAX11205_VREF_MAX_MV)
		return -EINVAL;

	dev = (struct max11205_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->vref_mv = init_param.vref_mv;

	struct no_os_callback_desc irq_cb = {
		.callback = max11205_irq_handler,
		.ctx = dev,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ
	};

	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret)
		goto error_dev;

	ret = no_os_gpio_get_optional(&dev->gpio_rdy, init_param.gpio_rdy);
	if (ret)
		goto error_spi;

	if (dev->gpio_rdy) {
		ret = no_os_gpio_direction_input(dev->gpio_rdy);
		if (ret)
			goto error_gpio;
	}

	ret = no_os_irq_register_callback(init_param.irq_ctrl,
					  dev->gpio_rdy->number, &irq_cb);
	if (ret)
		goto error_gpio;

	dev->irq_cb = irq_cb;

	ret = no_os_irq_trigger_level_set(init_param.irq_ctrl,
					  dev->gpio_rdy->number, NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error_irq;

	ret = no_os_irq_enable(init_param.irq_ctrl,
			       dev->gpio_rdy->number);
	if (ret)
		goto error_irq;

	dev->irq_ctrl = init_param.irq_ctrl;
	dev->data_updated = false;

	*device = dev;

	return ret;

error_irq:
	no_os_irq_unregister_callback(init_param.irq_ctrl, dev->gpio_rdy->number,
				      &irq_cb);
error_gpio:
	no_os_gpio_remove(dev->gpio_rdy);
error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Get the converted data.
 * @param dev - Device handler.
 * @param new_data_avail - True if data has been updated since last read, false if same data is read again.
 * @param data_raw - Pointer to raw adc value.
 * @return 0 in case of success, negative error code otherwise.
 */
int max11205_get_data_raw(struct max11205_dev *dev, bool *new_data_avail,
			  int16_t *data_raw)
{
	if (!dev || !data_raw || !new_data_avail)
		return -EINVAL;

	*new_data_avail = dev->data_updated;
	*data_raw = dev->adc_data_raw;
	dev->data_updated = false;

	return 0;
}

/**
 * @brief Get the converted data.
 * @param dev - Device handler.
 * @param raw_data - ADC raw data to be converted to millivolts.
 * @param data_mv - Pointer to adc value measured in millivolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int max11205_get_data_mv(struct max11205_dev *dev, int16_t raw_data,
			 int32_t *data_mv)
{
	if (!dev || !data_mv)
		return -EINVAL;

	*data_mv = NO_OS_DIV_ROUND_CLOSEST(raw_data*dev->vref_mv, MAX11205_SCALE);

	return 0;
}

/**
 * @brief Free memory allocated by max11205_init().
 * @param dev - Device handler.
 * @return 0 in case of success, negative error code otherwise.
 */
int max11205_remove(struct max11205_dev *dev)
{
	int ret;

	/* Disable GPIO RDY interrupt */
	ret = no_os_irq_disable(dev->irq_ctrl, dev->gpio_rdy->number);
	if (ret)
		return ret;

	/* Remove GPIO RDY interrupt handler */
	ret = no_os_irq_unregister_callback(dev->irq_ctrl, dev->gpio_rdy->number,
					    &dev->irq_cb);
	if (ret)
		return ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_rdy);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
