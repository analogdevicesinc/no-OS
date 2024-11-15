/**************************************************************************//**
*   @file   ad527x.c
*   @brief  Implementation of ad527x Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ad527x.h"
#include "Console.h"
#include "Time.h"
/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
typedef enum
{
	com_spi,
	com_i2c
} comm_type_t; 

struct ad527x_chip_info {
    unsigned int bitsNumber;
    comm_type_t  communication;
};

static const struct ad527x_chip_info ad527x_chip_info[] = {
    [ID_AD5270] = {
        .bitsNumber    = 10,
        .communication = com_spi,
    },
    [ID_AD5271] = {
        .bitsNumber    = 8,
        .communication = com_spi,
    },
    [ID_AD5272] = {
        .bitsNumber    = 10,
        .communication = com_i2c,
    },
    [ID_AD5274] = {
        .bitsNumber    = 8,
        .communication = com_i2c,
    }
};

AD527x_type act_device;
unsigned char ad527xDevAddr = 0;

/******************************************************************************/
/************************** Functions Definitions *****************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Writes data to AD527x.
 *
 * @param command - Desired command.
 * @param data    - Data to be loaded.
 *
 * @return None.
*******************************************************************************/
void AD527x_SetRegister(unsigned char command, unsigned short data)
{
	unsigned char dataBuf[2] = {0, 0};


	data <<= (MAX_BITS_NUMBER - ad527x_chip_info[act_device].bitsNumber);
	data &= 0x3FF;
	command &= 0x0F;
	dataBuf[0] = (command << 2) + ((data & 0x0300) >> 8);
	dataBuf[1] = (data & 0x00FF);

	if (ad527x_chip_info[act_device].communication == com_i2c)
	{
		I2C_Write(AD527x_I2C_ADDR_0, dataBuf, 2, 1);
	}
	else
	{
		if (ad527x_chip_info[act_device].communication == com_spi)
		{
			SPI_Write(AD527x_SLAVE_ID, dataBuf, 2);
		}
	}
}

/***************************************************************************//**
 * @brief Reads data from AD527x.
 *
 * @param command    - Desired command.
 * @param memReg50TP - one of the 50-TP memory registers (param needed only for
 * 					   command 5).
 *
 * @return None.
*******************************************************************************/
unsigned short AD527x_GetRegister(unsigned char command, unsigned char memReg50TP)
{
	long          returnVal = 0;
	unsigned char rxBuf[2]  = {0, 0};
	unsigned char txBuf[2]  = {0, 0};
	char          status    = 0;

	command &= 0x0F;
	txBuf[0] = command << 2;
	memReg50TP &= 0x3F;
	txBuf[1] = memReg50TP;

	if (ad527x_chip_info[act_device].communication == com_spi)
	{
		status = SPI_Write(AD527x_SLAVE_ID, txBuf, 2);
	    if(status != 2)
	    {
	        return -1;
	    }
		status = SPI_Read(AD527x_SLAVE_ID, rxBuf, 2);
		if (status != 2)
		{
			return -1;
		}
	}
	else
	{
		if (ad527x_chip_info[act_device].communication == com_i2c)
		{
			status = I2C_Write(AD527x_I2C_ADDR_0, txBuf, 2, 1);
		    if(status != 2)
		    {
		        return -1;
		    }
			status = I2C_Read(AD527x_I2C_ADDR_0, rxBuf, 2, 1);
		    if(status != 2)
		    {
		        return -1;
		    }
		}
	}

	returnVal |= (short) ((rxBuf[0] & 0x03) << 8);
	returnVal |= (short) (rxBuf[1] << 0);

	return(returnVal);
}

/***************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @param devAddr - AD5272/AD5274 device address.
 * 					Example: AD5272_I2C_ADDR_0 - ADDR_Pin=GND;
 * 							 AD5272_I2C_ADDR_1 - ADDR_Pin=VDD;
 * 							 AD5272_I2C_ADDR_2 - ADDR_Pin=NC.
 * 		  device  - supported devices:
 * 		  			Example: AD5270, AD5271, AD5272, AD5274.
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char AD527x_Init(unsigned char devAddr, AD527x_type device)
{
	char status = -1;

	ad527xDevAddr = devAddr;
	act_device = device;

	if (ad527x_chip_info[act_device].communication == com_spi)
	{
		status = SPI_Init(0, 1000000, 0, 0);
	}
	else
	{
		if (ad527x_chip_info[act_device].communication == com_i2c)
		{
			status = I2C_Init(100000);
		}
	}

	AD527x_RESET_OUT;
    AD527x_RESET_HIGH;

    /* setup AD527x */
    /* enable 50TP, allow update of RDAC register */
	AD527x_SetRegister(AD527x_WRITE_CONTROL,(AD527x_C1 | AD527x_C0));

	return status;
}
