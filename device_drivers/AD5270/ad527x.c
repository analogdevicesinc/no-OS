/**************************************************************************//**
*   @file   ad527x.c
*   @brief  Implementation of ad527x Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/


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
