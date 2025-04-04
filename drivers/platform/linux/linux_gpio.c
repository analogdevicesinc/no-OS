/***************************************************************************//**
 *   @file   linux/linux_gpio.c
 *   @brief  Implementation of Linux platform GPIO Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
 *   @author Jamila Macagba (Jamila.Macagba@analog.com)
********************************************************************************
 * Copyright 2020, 2025(c) Analog Devices, Inc.
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

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
#include <string.h>

/**
 * @struct linux_gpio_desc
 * @brief Linux platform specific GPIO descriptor
 */
struct linux_gpio_desc {
	/** GPIO Chip file descriptor */
	int chip_fd;
	/** GPIO Line file descriptor */
	int line_fd;
};

#define GPIO_TIMEOUT_MS 1000

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_gpio_get(struct no_os_gpio_desc **desc,
		       const struct no_os_gpio_init_param *param)
{
	struct linux_gpio_desc *linux_desc;
	struct no_os_gpio_desc *descriptor;
	char path[64];
	int ret;
	int timeout;
	struct gpiochip_info chip_info = {0};
	struct gpio_v2_line_info line_info = {0};
	struct gpio_v2_line_request line_request = {0};

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -1;

	linux_desc = no_os_calloc(1, sizeof(*linux_desc));
	if (!linux_desc)
		goto free_desc;

	descriptor->extra = linux_desc;
	descriptor->number = param->number;

	sprintf(path, "/dev/gpiochip%d", descriptor->port);
	timeout = GPIO_TIMEOUT_MS;
	while (--timeout) {
		linux_desc->chip_fd = open(path, O_RDONLY);
		if (linux_desc->chip_fd >= 0)
			break;
		no_os_mdelay(1);
	}
	if (linux_desc->chip_fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		goto close_dir;
	}

	/* Get Chip Info */
	ret = ioctl(linux_desc->chip_fd, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
	if (ret < 0) {
		printf("%s: Can't get chipinfo\n\r", __func__);
		goto close_dir;
	}

	/* Get Line Info */
	line_info.offset = descriptor->number;
	ret = ioctl(linux_desc->chip_fd, GPIO_V2_GET_LINEINFO_IOCTL, &line_info);
	if (ret < 0) {
		printf("%s: Can't get lineinfo\n\r", __func__);
		goto close_dir;
	}

	/* Get Line Request */
	line_request.offsets[0] = descriptor->number;
	line_request.num_lines = 1;
	ret = ioctl(linux_desc->chip_fd, GPIO_V2_GET_LINE_IOCTL, &line_request);
	if (ret < 0) {
		printf("%s: Can't get line request\n\r", __func__);
		goto close_dir;
	}
	linux_desc->line_fd = line_request.fd;

	*desc = descriptor;

	return 0;

close_dir:
	close(linux_desc->chip_fd);
free_linux_desc:
	no_os_free(linux_desc);
free_desc:
	no_os_free(descriptor);

	return -1;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_gpio_get_optional(struct no_os_gpio_desc **desc,
				const struct no_os_gpio_init_param *param)
{
	linux_gpio_get(desc, param);

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_gpio_remove(struct no_os_gpio_desc *desc)
{
	struct linux_gpio_desc *linux_desc;
	int ret;

	linux_desc = desc->extra;

	ret = close(linux_desc->chip_fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return -1;
	}

	ret = close(linux_desc->line_fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return -1;
	}

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_gpio_set_value(struct no_os_gpio_desc *desc,
			     uint8_t value)
{
	struct linux_gpio_desc *linux_desc;
	int ret;
	struct gpio_v2_line_values line_value = {0};

	linux_desc = desc->extra;

	/* Set Line Value */
	line_value.bits = value;
	line_value.mask = 1; /* Only set one line (or one GPIO) */
	ret = ioctl(linux_desc->line_fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &line_value);
	if (ret < 0) {
		printf("%s: Can't set line value\n\r", __func__);
		return -1;
	}

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_gpio_get_value(struct no_os_gpio_desc *desc,
			     uint8_t *value)
{
	struct linux_gpio_desc *linux_desc;
	int ret;
	struct gpio_v2_line_values line_value = {0};

	linux_desc = desc->extra;

	/* Get Line Value */
	line_value.mask = 1; /* Only get one line (or one GPIO) */
	ret = ioctl(linux_desc->line_fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &line_value);
	if (ret < 0) {
		printf("%s: Can't get line value\n\r", __func__);
		return -1;
	}

	if (line_value.bits == 0)
		*value = NO_OS_GPIO_LOW;
	else
		*value = NO_OS_GPIO_HIGH;

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	struct linux_gpio_desc *linux_desc;
	int ret;
	struct gpio_v2_line_config line_config = {0};

	linux_desc = desc->extra;

	/* Set Line Config */
	line_config.flags = GPIO_V2_LINE_FLAG_INPUT;
	ret = ioctl(linux_desc->line_fd, GPIO_V2_LINE_SET_CONFIG_IOCTL, &line_config);
	if (ret < 0) {
		printf("%s: Can't config line\n\r", __func__);
		return -1;
	}

	return 0;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_gpio_direction_output(struct no_os_gpio_desc *desc,
				    uint8_t value)
{
	struct linux_gpio_desc *linux_desc;
	int ret;
	struct gpio_v2_line_config line_config = {0};

	linux_desc = desc->extra;

	/* Set Line Config */
	line_config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
	ret = ioctl(linux_desc->line_fd, GPIO_V2_LINE_SET_CONFIG_IOCTL, &line_config);
	if (ret < 0) {
		printf("%s: Can't config line\n\r", __func__);
		return -1;
	}

	linux_gpio_set_value(desc, value);

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_gpio_get_direction(struct no_os_gpio_desc *desc,
				 uint8_t *direction)
{
	struct linux_gpio_desc *linux_desc;
	int ret;
	struct gpio_v2_line_info line_info = {0};

	linux_desc = desc->extra;

	/* Get Line Info */
	line_info.offset = desc->number;
	ret = ioctl(linux_desc->chip_fd, GPIO_V2_GET_LINEINFO_IOCTL, &line_info);
	if (ret < 0) {
		printf("%s: Can't get lineinfo\n\r", __func__);

		return -1;
	}

	if (line_info.flags & GPIO_V2_LINE_FLAG_OUTPUT)
		*direction = NO_OS_GPIO_OUT;
	else if (line_info.flags & GPIO_V2_LINE_FLAG_INPUT)
		*direction = NO_OS_GPIO_IN;

	return 0;
}

/**
 * @brief Linux platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops linux_gpio_ops = {
	.gpio_ops_get = &linux_gpio_get,
	.gpio_ops_get_optional = &linux_gpio_get_optional,
	.gpio_ops_remove = &linux_gpio_remove,
	.gpio_ops_direction_input = &linux_gpio_direction_input,
	.gpio_ops_direction_output = &linux_gpio_direction_output,
	.gpio_ops_get_direction = &linux_gpio_get_direction,
	.gpio_ops_set_value = &linux_gpio_set_value,
	.gpio_ops_get_value = &linux_gpio_get_value,
};
