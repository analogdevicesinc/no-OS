/***************************************************************************//**
 *   @file   linux/linux_gpio.c
 *   @brief  Implementation of Linux platform GPIO Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

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

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct linux_gpio_desc
 * @brief Linux platform specific GPIO descriptor
 */
struct linux_gpio_desc {
	/** /sys/class/gpio/gpio"number"/direction file descriptor */
	int direction_fd;
	/** /sys/class/gpio/gpio"number"/value file descriptor */
	int value_fd;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

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
	int fd;
	int len;
	int ret;
	int timeout;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -1;

	linux_desc = no_os_calloc(1, sizeof(*linux_desc));
	if (!linux_desc)
		goto free_desc;

	descriptor->extra = linux_desc;
	descriptor->number = param->number;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		goto free_linux_desc;
	}

	len = sprintf(path, "%d", descriptor->number);
	ret = write(fd, path, len);
	if ((ret < 0) && (errno != EBUSY)) {
		printf("%s: Can't write to file\n\r", __func__);
		close(fd);
		goto free_linux_desc;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		goto free_linux_desc;
	}

	sprintf(path, "/sys/class/gpio/gpio%d/direction", descriptor->number);
	timeout = GPIO_TIMEOUT_MS;
	while (--timeout) {
		linux_desc->direction_fd = open(path, O_WRONLY);
		if (linux_desc->direction_fd >= 0)
			break;
		no_os_mdelay(1);
	}
	if (linux_desc->direction_fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		goto free_linux_desc;
	}

	sprintf(path, "/sys/class/gpio/gpio%d/value", descriptor->number);
	timeout = GPIO_TIMEOUT_MS;
	while (--timeout) {
		linux_desc->value_fd = open(path, O_WRONLY);
		if (linux_desc->value_fd >= 0)
			break;
		no_os_mdelay(1);
	}
	if (linux_desc->value_fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		goto close_dir;
	}

	*desc = descriptor;

	return 0;

close_dir:
	close(linux_desc->direction_fd);
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
	char path[64];
	int fd;
	int len;
	int ret;

	linux_desc = desc->extra;

	ret = close(linux_desc->direction_fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return -1;
	}

	ret = close(linux_desc->value_fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return -1;
	}

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return -1;
	}

	len = sprintf(path, "%d", desc->number);
	ret = write(fd, path, len);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return -1;
	}

	ret = close(fd);
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

	linux_desc = desc->extra;

	if (value)
		ret = write(linux_desc->value_fd, "1", 2);
	else
		ret = write(linux_desc->value_fd, "0", 2);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
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
	char data;
	int ret;

	linux_desc = desc->extra;

	ret = read(linux_desc->value_fd, &data, 1);
	if (ret < 0) {
		printf("%s: Can't read from file\n\r", __func__);
		return -1;
	}

	if(data == '0')
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

	linux_desc = desc->extra;

	ret = write(linux_desc->direction_fd, "in", 3);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
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

	linux_desc = desc->extra;

	ret = write(linux_desc->direction_fd, "out", 4);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return -1;
	}

	ret = linux_gpio_set_value(desc, value);
	if (ret != 0) {
		printf("%s: Can't set value\n\r", __func__);
		return -1;
	}

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
	char data;
	int ret;

	linux_desc = desc->extra;

	ret = read(linux_desc->direction_fd, &data, 1);
	if (ret < 0) {
		printf("%s: Can't read from file\n\r", __func__);
		return -1;
	}

	if (data == 'o')
		*direction = NO_OS_GPIO_OUT;
	else
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
