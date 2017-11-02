/***************************************************************************//**
 *   @file   platform_drivers.c
 *   @brief  Implementation of Generic Platform Drivers.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
#include <stdint.h>
#include "platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_init(i2c_device *dev)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Write data to a slave device.
 * @param dev - The device structure.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_write(i2c_device *dev,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	if(data) {
		// Unused variable - fix compiler warning
	}

	if(bytes_number) {
		// Unused variable - fix compiler warning
	}

	if(stop_bit) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Read data from a slave device.
 * @param dev - The device structure.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_read(i2c_device *dev,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	if(data) {
		// Unused variable - fix compiler warning
	}

	if(bytes_number) {
		// Unused variable - fix compiler warning
	}

	if(stop_bit) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_init(spi_device *dev)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Write and read data to/from SPI.
 * @param dev - The device structure.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_write_and_read(spi_device *dev,
			   uint8_t *data,
			   uint8_t bytes_number)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	if(data) {
		// Unused variable - fix compiler warning
	}

	if(bytes_number) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Initialize the GPIO controller.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_init(gpio_device *dev)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Set the direction of the specified GPIO.
 * @param dev - The device structure.
 * @param gpio_num - The GPIO number.
 * @param direction - The direction.
 *                    Example: GPIO_OUT
 *                             GPIO_IN
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_direction(gpio_device *dev,
			  uint8_t gpio_num,
			  uint8_t direction)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	if(gpio_num) {
		// Unused variable - fix compiler warning
	}

	if(direction) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param dev - The device structure.
 * @param gpio_num - The GPIO number.
 * @param direction - The direction.
 *                    Example: GPIO_OUT
 *                             GPIO_IN
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_direction(gpio_device *dev,
			  uint8_t gpio_num,
			  uint8_t *direction)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	if(gpio_num) {
		// Unused variable - fix compiler warning
	}

	if(direction) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param dev - The device structure.
 * @param gpio_num - The GPIO number.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_value(gpio_device *dev,
		       uint8_t gpio_num,
		       uint8_t value)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	if(gpio_num) {
		// Unused variable - fix compiler warning
	}

	if(value) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param dev - The device structure.
 * @param gpio_num - The GPIO number.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_value(gpio_device *dev,
		       uint8_t gpio_num,
		       uint8_t *value)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	if(gpio_num) {
		// Unused variable - fix compiler warning
	}

	if(value) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void mdelay(uint32_t msecs)
{
	if(msecs) {
		// Unused variable - fix compiler warning
	}
}
