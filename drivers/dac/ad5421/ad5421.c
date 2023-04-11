/**************************************************************************//**
*   @file   AD5421.h
*   @brief  Header file of AD5421 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
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
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdlib.h>
#include "ad5421.h"
#include "no_os_alloc.h"

/*****************************************************************************/
/************************* Functions Definitions *****************************/
/*****************************************************************************/

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5421 Board.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return retValue - Result of the initialization.
 * 					  Example: 0 - if initialization was successful;
 *							  -1 - if initialization was unsuccessful.
******************************************************************************/
int32_t ad5421_init(struct ad5421_dev **device,
		    struct ad5421_init_param init_param)
{
	struct ad5421_dev *dev;
	int32_t spi_data = 0;
	/* Set bits for initialization process. */
	int32_t cmd_check = (CTRL_AUTO_FAULT_RDBK |
			     CTRL_SEL_ADC_INPUT   |
			     CTRL_ONCHIP_ADC      |
			     !CTRL_SPI_WATCHDOG);
	int32_t ret_value = -1;

	dev = (struct ad5421_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	no_os_gpio_get(&dev->gpio_ldac, &init_param.gpio_ldac);
	no_os_gpio_get(&dev->gpio_faultin, &init_param.gpio_faultin);

	/* Setup the SPI interface. */
	no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	/* Setup AD5421 control register. */
	/* Write to the control register. */
	spi_data = AD5421_CMD(AD5421_CMDWRCTRL);
	/* Set certain bits. */
	spi_data += (CTRL_AUTO_FAULT_RDBK |
		     CTRL_SEL_ADC_INPUT   |
		     CTRL_ONCHIP_ADC		 |
		     CTRL_SPI_WATCHDOG);
	/* Send data via SPI. */
	ad5421_set(dev,
		   &spi_data);
	/* Read from the control register. */
	spi_data = AD5421_CMD(AD5421_CMDRDCTRL);
	ad5421_set(dev,
		   &spi_data);
	/* Receive data via SPI. */
	spi_data = ad5421_get(dev);

	if (spi_data & cmd_check) {
		/* Check the echo. */
		ret_value = 0;
	}
	/* Set GPIO pins */
	AD5421_LDAC_OUT;
	AD5421_LDAC_LOW;
	AD5421_FAULT_IN;

	*device = dev;

	return(ret_value);
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5421_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad5421_remove(struct ad5421_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_ldac);
	ret |= no_os_gpio_remove(dev->gpio_faultin);

	no_os_free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Write to the DAC register.
 *
 * @param dev       - The device structure.
 * @param dac_value - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5421_set_dac(struct ad5421_dev *dev,
		    int32_t dac_value)
{
	int32_t spi_data = 0;

	/* Load data in DAC register. */
	spi_data = AD5421_CMD(AD5421_CMDWRDAC);
	spi_data += (dac_value);
	ad5421_set(dev,
		   &spi_data);
}

/**************************************************************************//**
 * @brief Write to the Offset Adjust register.
 *
 * @param dev          - The device structure.
 * @param offset_value - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5421_set_offset(struct ad5421_dev *dev,
		       int32_t offset_value)
{
	int32_t spi_data = 0;

	/* Load data in OFFSET register. */
	spi_data = AD5421_CMD(AD5421_CMDWROFFSET);
	spi_data += (offset_value);
	ad5421_set(dev,
		   &spi_data);
}

/**************************************************************************//**
 * @brief Write to the Gain Adjust register
 *
 * @param dev        - The device structure.
 * @param gain_value - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5421_set_gain(struct ad5421_dev *dev,
		     int32_t gain_value)
{
	int32_t spi_data = 0;

	/* Load data in GAIN register. */
	spi_data = AD5421_CMD(AD5421_CMDWRGAIN);
	spi_data += (gain_value);
	ad5421_set(dev,
		   &spi_data);
}

/**************************************************************************//**
 * @brief Read from the DAC register.
 *
 * @param dev - The device structure.
 *
 * @return dacValue - value read from the register.
******************************************************************************/
int32_t ad5421_get_dac(struct ad5421_dev *dev)
{
	int32_t spi_data = 0;
	int32_t dac_value = 0;

	/* Read data from DAC register. */
	spi_data = AD5421_CMD(AD5421_CMDRDDAC);
	ad5421_set(dev,
		   &spi_data);
	dac_value = ad5421_get(dev);

	return(dac_value);
}

/**************************************************************************//**
 * @brief Read from the Offset Adjust register.
 *
 * @param dev - The device structure.
 *
 * @return offsetValue - value calculated according to the datasheet formula.
******************************************************************************/
int32_t ad5421_get_offset(struct ad5421_dev *dev)
{
	int32_t spi_data = 0;
	int32_t offset_value = 0;

	/* Read data from OFFSET register. */
	spi_data = AD5421_CMD(AD5421_CMDRDOFFSET);
	ad5421_set(dev,
		   &spi_data);
	offset_value = ad5421_get(dev);

	return(offset_value);
}

/**************************************************************************//**
 * @brief Read from the Gain Adjust register
 *
 * @param dev - The device structure.
 *
 * @return gainValue - value calculated according to the datasheet formula
******************************************************************************/
int32_t ad5421_get_gain(struct ad5421_dev *dev)
{
	int32_t spi_data = 0;
	int32_t gain_value = 0;

	/* Read from GAIN register. */
	spi_data = AD5421_CMD(AD5421_CMDRDGAIN);
	ad5421_set(dev,
		   &spi_data);
	gain_value = ad5421_get(dev);

	return(gain_value);
}

/**************************************************************************//**
 * @brief Read from the Fault register
 *
 * @param dev - The device structure.
 *
 * @return value - register content
******************************************************************************/
int32_t ad5421_get_fault(struct ad5421_dev *dev)
{
	int32_t spi_data = 0;
	int32_t value = 0;

	/* Read from GAIN register. */
	spi_data = AD5421_CMD(AD5421_CMDRDFAULT);
	ad5421_set(dev,
		   &spi_data);
	value = ad5421_get(dev);

	return(value);
}


/**************************************************************************//**
 * @brief Read the temperature from Fault register.
 *
 * @param dev - The device structure.
 *
 * @return tempValue - temperature value in Celsius degrees.
******************************************************************************/
int32_t ad5421_get_temp(struct ad5421_dev *dev)
{
	int32_t spi_data = 0;
	int32_t temp_value = 0;
	/* Constant taken from datasheet formula */
	float temp_constant1 = -1.559;
	/* Constant taken from datasheet formula */
	int32_t temp_constant2 = 312;

	/* Set CONTROL Register to measure temperature. */
	spi_data = AD5421_CMD(AD5421_CMDWRCTRL);
	spi_data += (CTRL_AUTO_FAULT_RDBK |
		     CTRL_SEL_ADC_INPUT   |
		     CTRL_ONCHIP_ADC      |
		     CTRL_SPI_WATCHDOG);
	ad5421_set(dev,
		   &spi_data);
	/* Initiate temperature measurement. */
	spi_data = 0;
	spi_data = AD5421_CMD(AD5421_CMDMEASVTEMP);
	ad5421_set(dev,
		   &spi_data);
	/* Read temperature from Fault register. */
	spi_data = 0;
	spi_data = AD5421_CMD(AD5421_CMDRDFAULT);
	ad5421_set(dev,
		   &spi_data);
	temp_value = ad5421_get(dev) & 0x0FF;
	/* Calculate temperature according to the datasheet formula. */
	temp_value = (int32_t) (temp_constant1 * temp_value) + temp_constant2;

	return(temp_value);
}

/**************************************************************************//**
 * @brief Read VLoop-COM from Fault register.
 *
 * @param dev - The device structure.
 *
 * @return vlopValue - value of the voltage drop between Vloop and COM.
******************************************************************************/
float ad5421_get_vloop(struct ad5421_dev *dev)
{
	int32_t spi_data = 0;
	float vloop_value = 0;
	/* Constant taken from datasheet formula */
	float divide_constant_vloop = 0.009765625;

	/* Set CONTROL Register to measure Vloop. */
	spi_data = AD5421_CMD(AD5421_CMDWRCTRL);
	spi_data += (CTRL_AUTO_FAULT_RDBK |
		     CTRL_ONCHIP_ADC      |
		     CTRL_SPI_WATCHDOG);
	ad5421_set(dev,
		   &spi_data);
	/* Initiate VLOOP measurement. */
	spi_data = 0;
	spi_data = AD5421_CMD(AD5421_CMDMEASVTEMP);
	ad5421_set(dev,
		   &spi_data);
	/* Read VLOOP from Fault register. */
	spi_data = 0;
	spi_data = AD5421_CMD(AD5421_CMDRDFAULT);
	ad5421_set(dev,
		   &spi_data);
	/* Calculate Vloop according to the datasheet formula. */
	vloop_value = (float) (ad5421_get(dev) & 0x0FF);
	vloop_value = divide_constant_vloop * vloop_value;

	return(vloop_value);
}

/**************************************************************************//**
 * @brief Send data via SPI
 *
 * @param dev   - The device structure.
 * @param value - value to be sent via SPI
 *
 * @return - Result of the initialization.
 * 		   		Example: 0 - if initialization was successful;
 *				        -1 - if initialization was unsuccessful.
******************************************************************************/
int32_t ad5421_set(struct ad5421_dev *dev,
		   int32_t *value)
{
	uint8_t tx_buffer [ 3 ] = {0, 0, 0};
	int8_t status = 0;

	/* Arrange the data to be sent in 8 bit packets. */
	tx_buffer[0] = (uint8_t) ((*value & 0xff0000) >> 16);
	tx_buffer[1] = (uint8_t) ((*value & 0x00ff00) >> 8);
	tx_buffer[2] = (uint8_t) ((*value & 0x0000ff) >> 0);
	/* Do a write operation via SPI. */
	status = no_os_spi_write_and_read(dev->spi_desc,
					  tx_buffer,
					  3);
	if (status != 3) {
		return -1;
	} else {
		return 0;
	}
}

/**************************************************************************//**
 * @brief Read data via SPI
 *
 * @param dev - The device structure.
 *
 * @return returnVal - Data read
******************************************************************************/
int32_t ad5421_get(struct ad5421_dev *dev)
{
	int32_t return_val = 0;
	uint8_t rx_buffer[3] = {0, 0, 0};
	int8_t status = 0;

	status = no_os_spi_write_and_read(dev->spi_desc,
					  rx_buffer,
					  3);
	if (status != 3) {
		return -1;
	}
	return_val |= (int32_t) (rx_buffer[1] << 8);
	return_val |= (int32_t) (rx_buffer[2] << 0);

	return(return_val);
}

/**************************************************************************//**
 * @brief Resets the ad5421 device.
 *
 * @param dev - The device structure.
 *
 * @return None.
******************************************************************************/
void ad5421_reset(struct ad5421_dev *dev)
{
	int32_t spi_data = 0;

	spi_data = AD5421_CMD(AD5421_CMDRST);
	ad5421_set(dev,
		   &spi_data);
}
