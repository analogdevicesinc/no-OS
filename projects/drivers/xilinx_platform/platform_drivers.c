/***************************************************************************//**
 *   @file   platform_drivers.c
 *   @brief  Implementation of Xilinx Platform Drivers.
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
#include <stdlib.h>
#include <sleep.h>
#include "platform_drivers.h"
#include "xil_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param init_param - The structure that contains the I2C parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_init(struct i2c_desc **desc,
		 const struct i2c_init_param *param)
{
#ifdef XIIC_H
	i2c_desc *dev;
	xil_i2c_desc *xil_dev;
	xil_i2c_init_param *xil_param;
	int32_t ret;

	dev = calloc(1, sizeof *dev);
	if(!dev)
		return FAILURE;

	dev->extra = calloc(1, sizeof *xil_dev);
	if(!(dev->extra)){
		free(dev);
		return FAILURE;
	}

	xil_dev = dev->extra;
	xil_param = param->extra;

	xil_dev->type = xil_param->type;
	xil_dev->id = xil_param->id;
	dev->max_speed_hz = param->max_speed_hz;
	dev->slave_address = param->slave_address;

	xil_dev->config = XIic_LookupConfig(xil_dev->id);
	if (xil_dev->config == NULL)
		goto error;

	ret = XIic_CfgInitialize(&xil_dev->instance, xil_dev->config,
				 xil_dev->config->BaseAddress);
	if(ret != 0)
		goto error;

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				     XIic_InterruptHandler, &xil_dev->instance);

	ret = XIic_Start(&xil_dev->instance);
	if(ret != 0)
		goto error;

	ret = XIic_SetAddress(&xil_dev->instance, XII_ADDR_TO_SEND_TYPE,
			      dev->slave_address);
	if(ret != 0)
		goto error;

	*desc = dev;

#endif
	return SUCCESS;
#ifdef XIIC_H
error:
	free(dev);

	return FAILURE;
#endif
}

/**
 * @brief Free the resources allocated by i2c_init().
 * @param desc - The I2C descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_remove(struct i2c_desc *desc)
{
#ifdef XIIC_H
	int32_t ret;
	xil_i2c_desc *xil_desc;
	xil_desc = desc->extra;

	ret = XIic_Stop(&xil_desc->instance);
	if(ret != 0)
		return FAILURE;

	free(desc);
#endif
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
int32_t i2c_write(struct i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit)
{
#ifdef XIIC_H
	xil_i2c_desc *xil_desc;
	xil_desc = desc->extra;
	return XIic_Send(xil_desc->instance.BaseAddress, desc->slave_address, data,
			 bytes_number, stop_bit ? XIIC_STOP : XIIC_REPEATED_START);
#else
	return SUCCESS;
#endif
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
int32_t i2c_read(struct i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit)
{
#ifdef XIIC_H
	xil_i2c_desc *xil_desc;
	xil_desc = desc->extra;

	return XIic_Recv(xil_desc->instance.BaseAddress, desc->slave_address, data,
			 bytes_number, stop_bit ? XIIC_STOP : XIIC_REPEATED_START);
#else
	return SUCCESS;
#endif
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param init_param - The structure that contains the SPI parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_init(struct spi_desc **desc,
		 const struct spi_init_param *param)
{
	spi_desc *descriptor;
	xil_spi_desc *xil_descriptor;
	xil_spi_init_param *xil_param;
	int32_t ret;

	descriptor = calloc(1, sizeof *descriptor);
	if (!descriptor)
		return FAILURE;

	descriptor->extra = calloc(1, sizeof *xil_descriptor);
	if (!(descriptor->extra)){
		free(descriptor);
		return FAILURE;
	}
	
	xil_descriptor = descriptor->extra;
	xil_param = param->extra;

	descriptor->mode = param->mode;
	descriptor->chip_select = param->chip_select;
	xil_descriptor->flags = xil_param->flags;

#ifdef _XPARAMETERS_PS_H_
	xil_descriptor->config = XSpiPs_LookupConfig(xil_param->id);
	if (xil_descriptor->config == NULL)
		goto error;

	ret = XSpiPs_CfgInitialize(&xil_descriptor->instance,
				   xil_descriptor->config, xil_descriptor->config->BaseAddress);
	if (ret != 0)
		goto error;

	XSpiPs_SetOptions(&xil_descriptor->instance,
			  XSPIPS_MASTER_OPTION |
			  ((xil_descriptor->flags & SPI_CS_DECODE) ?
			   XSPIPS_DECODE_SSELECT_OPTION : 0) |
			  XSPIPS_FORCE_SSELECT_OPTION |
			  ((descriptor->mode & SPI_CPOL) ?
			   XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			  ((descriptor->mode & SPI_CPHA) ?
			   XSPIPS_CLK_PHASE_1_OPTION : 0));

	XSpiPs_SetClkPrescaler(&xil_descriptor->instance,
			       XSPIPS_CLK_PRESCALE_64);

	XSpiPs_SetSlaveSelect(&xil_descriptor->instance, 0xf);
#else
	ret = XSpi_Initialize(&xil_descriptor->instance, xil_param->id);
	if (ret != 0)
		goto error;

	XSpi_SetOptions(&xil_descriptor->instance,
			XSP_MASTER_OPTION |
			((descriptor->mode & SPI_CPOL) ?
			 XSP_CLK_ACTIVE_LOW_OPTION : 0) |
			((descriptor->mode & SPI_CPHA) ?
			 XSP_CLK_PHASE_1_OPTION : 0));

	XSpi_Start(&xil_descriptor->instance);

	XSpi_IntrGlobalDisable(&xil_descriptor->instance);
#endif

	*desc = descriptor;

	return SUCCESS;

error:
	free(descriptor);

	return FAILURE;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_remove(struct spi_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */

int32_t spi_write_and_read(struct spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number)
{
	xil_spi_desc *xil_desc;
	xil_desc = desc->extra;

#ifdef _XPARAMETERS_PS_H_
	XSpiPs_SetOptions(&xil_desc->instance,
			  XSPIPS_MASTER_OPTION |
			  ((xil_desc->flags & SPI_CS_DECODE) ?
			   XSPIPS_DECODE_SSELECT_OPTION : 0) |
			  XSPIPS_FORCE_SSELECT_OPTION |
			  ((desc->mode & SPI_CPOL) ?
			   XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			  ((desc->mode & SPI_CPHA) ?
			   XSPIPS_CLK_PHASE_1_OPTION : 0));

	XSpiPs_SetSlaveSelect(&xil_desc->instance,
			      desc->chip_select);
	XSpiPs_PolledTransfer(&xil_desc->instance,
			      data, data, bytes_number);
#else
	XSpi_SetOptions(&xil_desc->instance,
			XSP_MASTER_OPTION |
			((desc->mode & SPI_CPOL) ?
			 XSP_CLK_ACTIVE_LOW_OPTION : 0) |
			((desc->mode & SPI_CPHA) ?
			 XSP_CLK_PHASE_1_OPTION : 0));

	XSpi_SetSlaveSelect(&xil_desc->instance,
			    0x01 << desc->chip_select);

	XSpi_Transfer(&desc->instance,
		      data, data, bytes_number);
#endif
	return SUCCESS;
}

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param gpio_number - The number of the GPIO.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(struct gpio_desc **desc,
		 uint8_t gpio_number)
{
	gpio_desc *descriptor;
	xil_gpio_desc *xil_descriptor;
	int32_t ret;

	descriptor = calloc(1, sizeof *descriptor);
	if (!descriptor)
		return FAILURE;

	descriptor->extra = calloc(1, sizeof *xil_descriptor);
	if (!(descriptor->extra)){
		free(descriptor);
		return FAILURE;
	}

	xil_descriptor = descriptor->extra;

#ifdef _XPARAMETERS_PS_H_
	xil_descriptor->config = XGpioPs_LookupConfig(0);
	if (xil_descriptor->config == NULL)
		goto error;

	ret = XGpioPs_CfgInitialize(&xil_descriptor->instance,
				    xil_descriptor->config, xil_descriptor->config->BaseAddr);
	if (ret != 0)
		goto error;
#else
	ret = XGpio_Initialize(&xil_descriptor->instance, 0);
	if (ret != 0)
		goto error;
#endif

	descriptor->number = gpio_number;

	*desc = descriptor;

	return SUCCESS;

error:
	free(descriptor);

	return FAILURE;
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_remove(struct gpio_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
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
	if (desc) {
		// Unused variable - fix compiler warning
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
	xil_gpio_desc *xil_desc;
	xil_desc = desc->extra;

#ifdef _XPARAMETERS_PS_H_
	XGpioPs_SetDirectionPin(&xil_desc->instance, desc->number, 1);

	XGpioPs_SetOutputEnablePin(&xil_desc->instance, desc->number, 1);

	XGpioPs_WritePin(&xil_desc->instance, desc->number, value);
#else
	uint8_t pin = desc->number;
	uint8_t channel;
	uint32_t reg_val;

	if (pin >= 32) {
		channel = 2;
		pin -= 32;
	} else
		channel = 1;

	reg_val = XGpio_GetDataDirection(&xil_desc->instance, channel);
	reg_val &= ~(1 << pin);
	XGpio_SetDataDirection(&xil_desc->instance, channel, reg_val);

	reg_val = XGpio_DiscreteRead(&xil_desc->instance, channel);
	if(value)
		reg_val |= (1 << pin);
	else
		reg_val &= ~(1 << pin);
	XGpio_DiscreteWrite(&xil_desc->instance, channel, reg_val);
#endif

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
int32_t gpio_get_direction(struct gpio_desc *desc,
			   uint8_t *direction)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (direction) {
		// Unused variable - fix compiler warning
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
int32_t gpio_set_value(struct gpio_desc *desc,
		       uint8_t value)
{
	xil_gpio_desc *xil_desc;
	xil_desc = desc->extra;

#ifdef _XPARAMETERS_PS_H_
	XGpioPs_WritePin(&xil_desc->instance, desc->number, value);
#else
	uint8_t pin = desc->number;
	uint8_t channel;
	uint32_t reg_val;

	if (pin >= 32) {
		channel = 2;
		pin -= 32;
	} else
		channel = 1;

	reg_val = XGpio_DiscreteRead(&xil_desc->instance, channel);
	if(value)
		reg_val |= (1 << pin);
	else
		reg_val &= ~(1 << pin);
	XGpio_DiscreteWrite(&xil_desc->instance, channel, reg_val);
#endif

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
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (value) {
		// Unused variable - fix compiler warning
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
#ifdef _XPARAMETERS_PS_H_
	usleep(msecs * 1000);
#else
	usleep(msecs * 50);	// FIXME
#endif
}
