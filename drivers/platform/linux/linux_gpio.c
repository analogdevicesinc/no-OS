/***************************************************************************//**
 *   @file   linux/linux_gpio.c
 *   @brief  Implementation of Linux platform GPIO Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "error.h"
#include "gpio.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(struct gpio_desc **desc,
		 const struct gpio_init_param *param)
{
	gpio_desc *descriptor;
	char path[64];
	int fd;
	int len;
	int ret;

	descriptor = (gpio_desc *)malloc(sizeof(*descriptor));
	if (!descriptor)
		return FAILURE;

	descriptor->number = param->number;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		free(descriptor);
		return FAILURE;
	}

	len = sprintf(path, "%d", descriptor->number);
	ret = write(fd, path, len);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		close(fd);
		free(descriptor);
		return FAILURE;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		free(descriptor);
		return FAILURE;
	}

	*desc = descriptor;

	return SUCCESS;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_optional(struct gpio_desc **desc,
			  const struct gpio_init_param *param)
{
	gpio_get(desc, param);

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_remove(struct gpio_desc *desc)
{
	char path[64];
	int fd;
	int len;
	int ret;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return FAILURE;
	}

	len = sprintf(path, "%d", desc->number);
	ret = write(fd, path, len);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	free(desc->extra);
	free(desc);

	return SUCCESS;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_value(struct gpio_desc *desc,
		       uint8_t value)
{
	char path[64];
	int fd;
	int ret;

	sprintf(path, "/sys/class/gpio/gpio%d/value", desc->number);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return FAILURE;
	}

	if (value)
		ret = write(fd, "1", 2);
	else
		ret = write(fd, "0", 2);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_value(struct gpio_desc *desc,
		       uint8_t *value)
{
	char path[64];
	char data;
	int fd;
	int ret;

	sprintf(path, "/sys/class/gpio/gpio%d/value", desc->number);
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		printf("%s: Can't open file\n\r", __func__);
		return FAILURE;
	}

	ret = read(fd, &data, 1);
	if (ret < 0) {
		printf("%s: Can't read from file\n\r", __func__);
		return FAILURE;
	}

	if(data == '0')
		*value = GPIO_LOW;
	else
		*value = GPIO_HIGH;

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_input(struct gpio_desc *desc)
{
	char path[64];
	int fd;
	int ret;

	sprintf(path, "/sys/class/gpio/gpio%d/direction", desc->number);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return FAILURE;
	}

	ret = write(fd, "in", 3);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_output(struct gpio_desc *desc,
			      uint8_t value)
{
	char path[64];
	int fd;
	int ret;

	sprintf(path, "/sys/class/gpio/gpio%d/direction", desc->number);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return FAILURE;
	}

	ret = write(fd, "out", 4);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	ret = gpio_set_value(desc, value);
	if (ret != SUCCESS) {
		printf("%s: Can't set value\n\r", __func__);
		return FAILURE;
	}

	return SUCCESS;
}
