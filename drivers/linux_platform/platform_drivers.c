/***************************************************************************//**
 *   @file   platform_drivers.c
 *   @brief  Implementation of Linux Userspace Platform Drivers.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "platform_drivers.h"
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param init_param - The structure that contains the I2C parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_init(i2c_desc **desc,
		 i2c_init_param param)
{
	i2c_desc *descriptor;

	descriptor = (i2c_desc *)malloc(sizeof(*descriptor));
	if (!descriptor)
		return FAILURE;

	descriptor->fd = open(param.pathname, O_RDWR);
	if (descriptor->fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return FAILURE;
	}

	descriptor->slave_address = param.slave_address;

	*desc = descriptor;

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by i2c_init().
 * @param desc - The I2C descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_remove(i2c_desc *desc)
{
	int ret;

	ret = close(desc->fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	free(desc);

	return SUCCESS;
}

/**
 * @brief Write data to a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_write(i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit)
{
	int ret;

	ret = ioctl(desc->fd, I2C_SLAVE, desc->slave_address);
	if (ret < 0) {
		printf("%s: Can't select device\n\r", __func__);
		return FAILURE;
	}

	ret = write(desc->fd, data, bytes_number);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	if (stop_bit) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Read data from a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_read(i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit)
{
	int ret;

	ret = ioctl(desc->fd, I2C_SLAVE, desc->slave_address);
	if (ret < 0) {
		printf("%s: Can't select device\n\r", __func__);
		return FAILURE;
	}

	ret = read(desc->fd, data, bytes_number);
	if (ret < 0) {
		printf("%s: Can't read from file\n\r", __func__);
		return FAILURE;
	}

	if (stop_bit) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param init_param - The structure that contains the SPI parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_init(spi_desc **desc, const spi_init_param *param)
{
	spi_desc *descriptor;
	int ret;

	descriptor = (spi_desc *)malloc(sizeof(*descriptor));
	if (!descriptor)
		return FAILURE;

	descriptor->fd = open(param->pathname, O_RDWR);
	if (descriptor->fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return FAILURE;
	}

	ret = ioctl(descriptor->fd, SPI_IOC_WR_MODE,
			&param->mode);
	if (ret == -1) {
		printf("%s: Can't set mode\n\r", __func__);
		return FAILURE;
	}

	ret = ioctl(descriptor->fd, SPI_IOC_WR_MAX_SPEED_HZ,
			&param->max_speed_hz);
	if (ret == -1) {
		printf("%s: Can't set speed\n\r", __func__);
		return FAILURE;
	}

	*desc = descriptor;

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_remove(spi_desc *desc)
{
	int ret;

	ret = close(desc->fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	free(desc);

	return SUCCESS;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_write_and_read(spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number)
{
	struct spi_ioc_transfer transfer = {
		.tx_buf = (unsigned long)data,
		.rx_buf = (unsigned long)data,
		.len = bytes_number,
	};
	int32_t ret;

	ret = ioctl(desc->fd, SPI_IOC_MESSAGE(1), &transfer);
	if (ret == 1) {
		printf("%s: Can't send spi message\n\r", __func__);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param gpio_number - The number of the GPIO.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(gpio_desc **desc,
		 uint8_t gpio_number)
{
	gpio_desc *descriptor;
	char buf[100];
	int fd;
	int len;
	int ret;

	descriptor = (gpio_desc *)malloc(sizeof(*descriptor));
	if (!descriptor)
		return FAILURE;

	descriptor->number = gpio_number;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return FAILURE;
	}

	len = sprintf(buf, "%d", gpio_number);
	ret = write(fd, buf, len);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	*desc = descriptor;

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_remove(gpio_desc *desc)
{
	char buf[100];
	int fd;
	int len;
	int ret;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return FAILURE;
	}

	len = sprintf(buf, "%d", desc->number);
	ret = write(fd, buf, len);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	free(desc);

	return SUCCESS;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_input(gpio_desc *desc)
{
	char buf[100];
	int fd;
	int ret;

	sprintf(buf, "/sys/class/gpio/gpio%d/direction", desc->number);
	fd = open(buf, O_WRONLY);
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
int32_t gpio_direction_output(gpio_desc *desc,
			      uint8_t value)
{
	char buf[100];
	int fd;
	int ret;

	sprintf(buf, "/sys/class/gpio/gpio%d/direction", desc->number);
	fd = open(buf, O_WRONLY);
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

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: GPIO_OUT
 *                             GPIO_IN
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_direction(gpio_desc *desc,
			   uint8_t *direction)
{
	char buf[100];
	char data;
	int fd;
	int ret;

	sprintf(buf, "/sys/class/gpio/gpio%d/direction", desc->number);
	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		printf("%s: Can't open file\n\r", __func__);
		return FAILURE;
	}

	ret = read(fd, &data, 1);
	if (ret < 0) {
		printf("%s: Can't read from file\n\r", __func__);
		return FAILURE;
	}

	if(data == 'o')
		 *direction = GPIO_OUT;
	else
		 *direction = GPIO_IN;

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

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
int32_t gpio_set_value(gpio_desc *desc,
		       uint8_t value)
{
	char buf[100];
	int fd;
	int ret;

	sprintf(buf, "/sys/class/gpio/gpio%d/value", desc->number);
	fd = open(buf, O_WRONLY);
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
int32_t gpio_get_value(gpio_desc *desc,
		       uint8_t *value)
{
	char buf[100];
	char data;
	int fd;
	int ret;

	sprintf(buf, "/sys/class/gpio/gpio%d/value", desc->number);
	fd = open(buf, O_RDONLY);
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
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 * @return None.
 */
void udelay(uint32_t usecs)
{
	usleep(usecs);
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void mdelay(uint32_t msecs)
{
	usleep(msecs * 1000);
}
