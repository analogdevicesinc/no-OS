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
*
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "ad5421.h"

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
int AD5421_Init(ad5421_dev **device,
		ad5421_init_param init_param)
{
	ad5421_dev *dev;
    int spiData  = 0;
    /* Set bits for initialization process. */
    int cmdCheck = (CTRL_AUTO_FAULT_RDBK |
                    CTRL_SEL_ADC_INPUT   |
                    CTRL_ONCHIP_ADC      |
                    !CTRL_SPI_WATCHDOG);
    int retValue = -1;

	dev = (ad5421_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	gpio_get(&dev->gpio_ldac, init_param.gpio_ldac);
	gpio_get(&dev->gpio_faultin, init_param.gpio_faultin);

    /* Setup the SPI interface. */
	spi_init(&dev->spi_desc, init_param.spi_init);
    /* Setup AD5421 control register. */
    /* Write to the control register. */
    spiData = AD5421_CMD(AD5421_CMDWRCTRL);
    /* Set certain bits. */
    spiData += (CTRL_AUTO_FAULT_RDBK |
                CTRL_SEL_ADC_INPUT   |
                CTRL_ONCHIP_ADC		 |
                CTRL_SPI_WATCHDOG);
    /* Send data via SPI. */
    AD5421_Set(dev,
	       &spiData);
    /* Read from the control register. */
    spiData = AD5421_CMD(AD5421_CMDRDCTRL);
    AD5421_Set(dev,
	       &spiData);
    /* Receive data via SPI. */
    spiData = AD5421_Get(dev);

    if (spiData & cmdCheck)
    {
        /* Check the echo. */
        retValue = 0;
    }
    /* Set GPIO pins */
    AD5421_LDAC_OUT;
    AD5421_LDAC_LOW;
    AD5421_FAULT_IN;

	*device = dev;

    return(retValue);
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD5421_Init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t AD5421_remove(ad5421_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	ret |= gpio_remove(dev->gpio_ldac);
	ret |= gpio_remove(dev->gpio_faultin);

	free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Write to the DAC register.
 *
 * @param dev      - The device structure.
 * @param dacValue - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5421_SetDac(ad5421_dev *dev,
		   int dacValue)
{
    int spiData = 0;

    /* Load data in DAC register. */
    spiData = AD5421_CMD(AD5421_CMDWRDAC);
    spiData += (dacValue);
    AD5421_Set(dev,
	       &spiData);
}

/**************************************************************************//**
 * @brief Write to the Offset Adjust register.
 *
 * @param dev         - The device structure.
 * @param offsetValue - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5421_SetOffset(ad5421_dev *dev,
		      int offsetValue)
{
    int spiData = 0;

    /* Load data in OFFSET register. */
    spiData = AD5421_CMD(AD5421_CMDWROFFSET);
    spiData += (offsetValue);
    AD5421_Set(dev,
	       &spiData);
}

/**************************************************************************//**
 * @brief Write to the Gain Adjust register
 *
 * @param dev       - The device structure.
 * @param gainValue - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5421_SetGain(ad5421_dev *dev,
		    int gainValue)
{
    int spiData = 0;

    /* Load data in GAIN register. */
    spiData = AD5421_CMD(AD5421_CMDWRGAIN);
    spiData += (gainValue);
    AD5421_Set(dev,
	       &spiData);
}

/**************************************************************************//**
 * @brief Read from the DAC register.
 *
 * @param dev - The device structure.
 *
 * @return dacValue - value read from the register.
******************************************************************************/
int AD5421_GetDac(ad5421_dev *dev)
{
    int spiData  = 0;
    int dacValue = 0;

    /* Read data from DAC register. */
    spiData = AD5421_CMD(AD5421_CMDRDDAC);
    AD5421_Set(dev,
	       &spiData);
    dacValue = AD5421_Get(dev);

    return(dacValue);
}

/**************************************************************************//**
 * @brief Read from the Offset Adjust register.
 *
 * @param dev - The device structure.
 *
 * @return offsetValue - value calculated according to the datasheet formula.
******************************************************************************/
int AD5421_GetOffset(ad5421_dev *dev)
{
    int spiData     = 0;
    int offsetValue = 0;

    /* Read data from OFFSET register. */
    spiData = AD5421_CMD(AD5421_CMDRDOFFSET);
    AD5421_Set(dev,
	       &spiData);
    offsetValue = AD5421_Get(dev);

    return(offsetValue);
}

/**************************************************************************//**
 * @brief Read from the Gain Adjust register
 *
 * @param dev - The device structure.
 *
 * @return gainValue - value calculated according to the datasheet formula
******************************************************************************/
int AD5421_GetGain(ad5421_dev *dev)
{
    int spiData   = 0;
    int gainValue = 0;

    /* Read from GAIN register. */
    spiData = AD5421_CMD(AD5421_CMDRDGAIN);
    AD5421_Set(dev,
	       &spiData);
    gainValue = AD5421_Get(dev);

    return(gainValue);
}

/**************************************************************************//**
 * @brief Read from the Fault register
 *
 * @param dev - The device structure.
 *
 * @return value - register content
******************************************************************************/
int AD5421_GetFault(ad5421_dev *dev)
{
    int spiData = 0;
    int value   = 0;

    /* Read from GAIN register. */
    spiData = AD5421_CMD(AD5421_CMDRDFAULT);
    AD5421_Set(dev,
	       &spiData);
    value = AD5421_Get(dev);

    return(value);
}


/**************************************************************************//**
 * @brief Read the temperature from Fault register.
 *
 * @param dev - The device structure.
 *
 * @return tempValue - temperature value in Celsius degrees.
******************************************************************************/
int AD5421_GetTemp(ad5421_dev *dev)
{
    int   spiData       = 0;
    int   tempValue     = 0;
    /* Constant taken from datasheet formula */
    float tempConstant1 = -1.559;
    /* Constant taken from datasheet formula */
    int   tempConstant2 = 312;

    /* Set CONTROL Register to measure temperature. */
    spiData = AD5421_CMD(AD5421_CMDWRCTRL);
    spiData += (CTRL_AUTO_FAULT_RDBK |
    			CTRL_SEL_ADC_INPUT   |
    			CTRL_ONCHIP_ADC      |
    			CTRL_SPI_WATCHDOG);
    AD5421_Set(dev,
	       &spiData);
    /* Initiate temperature measurement. */
    spiData = 0;
    spiData = AD5421_CMD(AD5421_CMDMEASVTEMP);
    AD5421_Set(dev,
	       &spiData);
    /* Read temperature from Fault register. */
    spiData = 0;
    spiData = AD5421_CMD(AD5421_CMDRDFAULT);
    AD5421_Set(dev,
	       &spiData);
    tempValue = AD5421_Get(dev) & 0x0FF;
    /* Calculate temperature according to the datasheet formula. */
    tempValue = (int) (tempConstant1 * tempValue) + tempConstant2;

    return(tempValue);
}

/**************************************************************************//**
 * @brief Read VLoop-COM from Fault register.
 *
 * @param dev - The device structure.
 *
 * @return vlopValue - value of the voltage drop between Vloop and COM.
******************************************************************************/
float AD5421_GetVloop(ad5421_dev *dev)
{
    int   spiData               = 0;
    float vloopValue            = 0;
    /* Constant taken from datasheet formula */
    float divide_constant_vloop = 0.009765625;

    /* Set CONTROL Register to measure Vloop. */
    spiData = AD5421_CMD(AD5421_CMDWRCTRL);
    spiData += (CTRL_AUTO_FAULT_RDBK |
    			CTRL_ONCHIP_ADC      |
    			CTRL_SPI_WATCHDOG);
    AD5421_Set(dev,
	       &spiData);
    /* Initiate VLOOP measurement. */
    spiData = 0;
    spiData = AD5421_CMD(AD5421_CMDMEASVTEMP);
    AD5421_Set(dev,
	       &spiData);
    /* Read VLOOP from Fault register. */
    spiData = 0;
    spiData = AD5421_CMD(AD5421_CMDRDFAULT);
    AD5421_Set(dev,
	       &spiData);
    /* Calculate Vloop according to the datasheet formula. */
    vloopValue = (float) (AD5421_Get(dev) & 0x0FF);
    vloopValue = divide_constant_vloop * vloopValue;

    return(vloopValue);
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
int AD5421_Set(ad5421_dev *dev,
	       int *value)
{
    unsigned char txBuffer[3] = {0, 0, 0};
    char          status      = 0;

	/* Arrange the data to be sent in 8 bit packets. */
    txBuffer[0] = (unsigned char) ((*value & 0xff0000) >> 16);
    txBuffer[1] = (unsigned char) ((*value & 0x00ff00) >> 8);
    txBuffer[2] = (unsigned char) ((*value & 0x0000ff) >> 0);
    /* Do a write operation via SPI. */
	status = spi_write_and_read(dev->spi_desc,
				    txBuffer,
				    3);
    if (status != 3)
    {
    	return -1;
    }
    else
    {
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
int AD5421_Get(ad5421_dev *dev)
{
    int           returnVal   = 0;
    unsigned char rxBuffer[3] = {0, 0, 0};
    char          status      = 0;

	status = spi_write_and_read(dev->spi_desc,
				    rxBuffer,
				    3);
    if (status != 3)
    {
    	return -1;
    }
    returnVal |= (int) (rxBuffer[1] << 8);
    returnVal |= (int) (rxBuffer[2] << 0);

    return(returnVal);
}

/**************************************************************************//**
 * @brief Resets the AD5421 device.
 *
 * @param dev - The device structure.
 *
 * @return None.
******************************************************************************/
void AD5421_Reset(ad5421_dev *dev)
{
    int spiData = 0;

    spiData = AD5421_CMD(AD5421_CMDRST);
    AD5421_Set(dev,
	       &spiData);
}
