/***************************************************************************//**
 * @file adaq7980.c
 * @brief Implementation adaq7980 Driver.
 * @author ADI
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "adaq7980.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/**
 * @brief Read from device.
 *        Enter register mode to read/write registers
 * @param [in] dev - adaq7980_dev device handler.
 * @param [out] buf - data buffer.
 * @param [in] samples - sample number.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad7980_read_data(struct adaq7980_dev *dev,
			 uint16_t *buf,
			 uint16_t samples)
{
	struct spi_engine_offload_message msg;
	uint32_t spi_eng_msg_cmds[3] = {CS_LOW, WRITE_READ(2), CS_HIGH};
	uint32_t commands_data[2] = {0xFF, 0xFF};
	int32_t ret;

	ret = spi_engine_offload_init(dev->spi_desc, dev->offload_init_param);
	if (ret != 0)
		return ret;

	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	msg.rx_addr = (uint32_t)buf;
	msg.commands_data = commands_data;

	ret = spi_engine_offload_transfer(dev->spi_desc, msg, samples);
	if (ret != 0)
		return ret;

	return 0;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adaq7980_setup(struct adaq7980_dev **device,
		       struct adaq7980_init_param *init_param)
{
	struct adaq7980_dev *dev;
	int32_t ret;

	dev = (struct adaq7980_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	ret = no_os_gpio_get_optional(&dev->gpio_pd_ldo, init_param->gpio_pd_ldo);
	if (ret != 0)
		goto error_dev;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret != 0)
		goto error_dev;

	if (init_param->gpio_pd_ldo) {
		ret = no_os_gpio_direction_output(dev->gpio_pd_ldo, NO_OS_GPIO_LOW);
		if (ret != 0)
			goto error_dev;

		no_os_mdelay(10);
		ret = no_os_gpio_set_value(dev->gpio_pd_ldo, NO_OS_GPIO_HIGH);
		if (ret != 0)
			goto error_dev;

		no_os_mdelay(10);
	}
	ret = no_os_pwm_init(&dev->trigger_pwm_desc, init_param->trigger_pwm_init);
	if (ret != 0)
		goto error_spi;

	no_os_pwm_enable(dev->trigger_pwm_desc);

	dev->offload_init_param = init_param->offload_init_param;

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return -1;
}
