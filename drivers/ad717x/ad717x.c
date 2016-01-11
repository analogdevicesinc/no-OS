/***************************************************************************//**
*   @file    AD717X.c
*   @brief   AD717X implementation file.
*   @devices AD7172-2, AD7172-4, AD7173-8, AD7175-2, AD7175-8, AD7176-2
*            AD7177-2
*   @author  acozma (andrei.cozma@analog.com)
*            dnechita (dan.nechita@analog.com)
*
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "AD717x.h"

/* Error codes */
#define INVALID_VAL -1 /* Invalid argument */
#define COMM_ERR    -2 /* Communication error on receive */
#define TIMEOUT     -3 /* A timeout has occured */

/***************************************************************************//**
* @brief  Searches through the list of registers of the driver instance and
*         retrieves a pointer to the register that matches the given address.
* @param device - The handler of the instance of the driver.
* @param reg_address - The address to be used to find the register.
*
* @return A pointer to the register if found or 0.
*******************************************************************************/
ad717x_st_reg *AD717X_GetReg(struct ad717x_device *device, uint8_t reg_address)
{
	uint8_t i;
	ad717x_st_reg *reg = 0;

	if (!device || !device->regs)
		return 0;

	for (i = 0; i < device->num_regs; i++)
	{
		if (device->regs[i].addr == reg_address)
		{
			reg = &device->regs[i];
			break;
		}
	}

	return reg;
}

/***************************************************************************//**
* @brief Reads the value of the specified register.
*
* @param device - The handler of the instance of the driver.
* @addr - The address of the register to be read. The value will be stored
*         inside the register structure that holds info about this register.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_ReadRegister(struct ad717x_device *device, uint8_t addr)
{
	int32_t ret       = 0;
	uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i         = 0;
	uint8_t check8    = 0;
	uint8_t msgBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	ad717x_st_reg *pReg;

	if(!device)
		return INVALID_VAL;
	
	pReg = AD717X_GetReg(device, addr);
	if (!pReg)
	  	return INVALID_VAL;

	/* Build the Command word */
	buffer[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD |
			AD717X_COMM_REG_RA(pReg->addr);

	/* Read data from the device */
	ret = SPI_Read(device->slave_select_id,
			buffer,
			((device->useCRC != AD717X_DISABLE) ? pReg->size + 1
							: pReg->size) + 1);
	if(ret < 0)
		return ret;

	/* Check the CRC */
	if(device->useCRC == AD717X_USE_CRC)
	{
		msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD |
			AD717X_COMM_REG_RA(pReg->addr);
		for(i = 1; i < pReg->size + 2; ++i)
		{
			msgBuf[i] = buffer[i];
		}
		check8 = AD717X_ComputeCRC8(msgBuf, pReg->size + 2);
	}
	if(device->useCRC == AD717X_USE_XOR)
	{
		msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD |
				AD717X_COMM_REG_RA(pReg->addr);
		for(i = 1; i < pReg->size + 2; ++i)
		{
			msgBuf[i] = buffer[i];
		}
		check8 = AD717X_ComputeXOR8(msgBuf, pReg->size + 2);
	}

	if(check8 != 0)
	{
		/* ReadRegister checksum failed. */
		return COMM_ERR;
	}

	/* Build the result */
	pReg->value = 0;
	for(i = 1; i < pReg->size + 1; i++)
	{
		pReg->value <<= 8;
		pReg->value += buffer[i];
	}

	return ret;
}

/***************************************************************************//**
* @brief Writes the value of the specified register.
*
* @param device - The handler of the instance of the driver.
* @param reg - Register structure holding info about the register to be written
* @param addr - The address of the register to be written with the value stored
*               inside the register structure that holds info about this
*               register.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_WriteRegister(struct ad717x_device *device, uint8_t addr)
{
	int32_t ret      = 0;
	int32_t regValue = 0;
	uint8_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i        = 0;
	uint8_t crc8     = 0;
	ad717x_st_reg *preg;

	if(!device)
		return INVALID_VAL;
	
	preg = AD717X_GetReg(device, addr);
	if (!preg)
	  	return INVALID_VAL;

	/* Build the Command word */
	wrBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_WR |
			AD717X_COMM_REG_RA(preg->addr);

	/* Fill the write buffer */
	regValue = preg->value;
	for(i = 0; i < preg->size; i++)
	{
		wrBuf[preg->size - i] = regValue & 0xFF;
		regValue >>= 8;
	}

	/* Compute the CRC */
	if(device->useCRC != AD717X_DISABLE)
	{
		crc8 = AD717X_ComputeCRC8(wrBuf, preg->size + 1);
		wrBuf[preg->size + 1] = crc8;
	}

	/* Write data to the device */
	ret = SPI_Write(device->slave_select_id,
			wrBuf,
			(device->useCRC != AD717X_DISABLE) ? preg->size + 2
							: preg->size + 1);

	return ret;
}

/***************************************************************************//**
* @brief Resets the device.
*
* @param device - The handler of the instance of the driver.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_Reset(struct ad717x_device *device)
{
	int32_t ret = 0;
	uint8_t wrBuf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	if(!device)
		return INVALID_VAL;

	ret = SPI_Write(device->slave_select_id, wrBuf, 8);

	return ret;
}

/***************************************************************************//**
* @brief Waits until a new conversion result is available.
*
* @param device - The handler of the instance of the driver.
* @param timeout - Count representing the number of polls to be done until the
*                  function returns if no new data is available.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_WaitForReady(struct ad717x_device *device, uint32_t timeout)
{
	ad717x_st_reg *statusReg;
	int32_t ret;
	int8_t ready = 0;

	if(!device || !device->regs)
		return INVALID_VAL;

	statusReg = AD717X_GetReg(device, AD717X_STATUS_REG);
	if (!statusReg)
	  	return INVALID_VAL;

	while(!ready && --timeout)
	{
		/* Read the value of the Status Register */
		ret = AD717X_ReadRegister(device, AD717X_STATUS_REG);
		if(ret < 0)
			return ret;

		/* Check the RDY bit in the Status Register */
		ready = (statusReg->value & AD717X_STATUS_REG_RDY) != 0;
	}

	return timeout ? 0 : TIMEOUT;
}

/***************************************************************************//**
* @brief Reads the conversion result from the device.
*
* @param device - The handler of the instance of the driver.
* @param pData - Pointer to store the read data.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_ReadData(struct ad717x_device *device, int32_t* pData)
{
	ad717x_st_reg *dataReg;
	int32_t ret;

	if(!device || !device->regs)
		return INVALID_VAL;

	dataReg = AD717X_GetReg(device, AD717X_DATA_REG);
	if (!dataReg)
		return INVALID_VAL;

	/* Read the value of the Status Register */
	ret = AD717X_ReadRegister(device, AD717X_DATA_REG);

	/* Get the read result */
	*pData = dataReg->value;

	return ret;
}

/***************************************************************************//**
* @brief Computes the CRC checksum for a data buffer.
*
* @param pBuf - Data buffer
* @param bufSize - Data buffer size in bytes
*
* @return Returns the computed CRC checksum.
*******************************************************************************/
uint8_t AD717X_ComputeCRC8(uint8_t * pBuf, uint8_t bufSize)
{
	uint8_t i   = 0;
	uint8_t crc = 0;

	while(bufSize)
	{
		for(i = 0x80; i != 0; i >>= 1)
		{
			if(((crc & 0x80) != 0) != ((*pBuf & i) != 0)) /* MSB of CRC register XOR input Bit from Data */
			{
				crc <<= 1;
				crc ^= AD717X_CRC8_POLYNOMIAL_REPRESENTATION;
			}
			else
			{
				crc <<= 1;
			}
		}
		pBuf++;
		bufSize--;
	}
	return crc;
}

/***************************************************************************//**
* @brief Computes the XOR checksum for a data buffer.
*
* @param pBuf - Data buffer
* @param bufSize - Data buffer size in bytes
*
* @return Returns the computed XOR checksum.
*******************************************************************************/
uint8_t AD717X_ComputeXOR8(uint8_t * pBuf, uint8_t bufSize)
{
	uint8_t xor = 0;

	while(bufSize)
	{
		xor ^= *pBuf;
		pBuf++;
		bufSize--;
	}
	return xor;
}

/***************************************************************************//**
* @brief Updates the CRC settings.
*
* @param device - The handler of the instance of the driver.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_UpdateCRCSetting(struct ad717x_device *device)
{
	ad717x_st_reg *interfaceReg;

	if(!device || !device->regs)
		return INVALID_VAL;

	interfaceReg = AD717X_GetReg(device, AD717X_IFMODE_REG);
	if (!interfaceReg)
		return INVALID_VAL;

	/* Get CRC State. */
	if(AD717X_IFMODE_REG_CRC_STAT(interfaceReg->value))
	{
		device->useCRC = AD717X_USE_CRC;
	}
	else if(AD717X_IFMODE_REG_XOR_STAT(interfaceReg->value))
	{
		device->useCRC = AD717X_USE_XOR;
	}
	else
	{
		device->useCRC = AD717X_DISABLE;
	}

	return 0;
}

/***************************************************************************//**
* @brief Initializes the AD717X.
*
* @param device - The handler of the instance of the driver.
* @param dev_type - The type of the device that the driver should be
*                   instantiated with.
* @param slave_select - The Slave Chip Select Id to be passed to the SPI calls.
* @param regs - The list of registers of the device (initialized or not) to be
*               added to the instance of the driver.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_Setup(struct ad717x_device *device, uint8_t slave_select,
			ad717x_st_reg *regs, uint8_t num_regs)
{
	int32_t ret;
	ad717x_st_reg *preg;

	if(!device || !regs)
		return INVALID_VAL;

	device->regs = regs;
	device->slave_select_id = slave_select;
	device->num_regs = num_regs;

	/* Initialize the SPI communication. */
	ret = SPI_Init(0, 1000000, 1, 0);
	if (ret < 0)
		return ret;

	/*  Reset the device interface.*/
	ret = AD717X_Reset(device);
	if (ret < 0)
		return ret;

	/* Initialize ADC mode register. */
	ret = AD717X_WriteRegister(device, AD717X_ADCMODE_REG);
	if(ret < 0)
		return ret;

	/* Initialize Interface mode register. */
	ret = AD717X_WriteRegister(device, AD717X_IFMODE_REG);
	if(ret < 0)
		return ret;

	/* Get CRC State */
	ret = AD717X_UpdateCRCSetting(device);
	if(ret < 0)
		return ret;

	/* Initialize registers AD717X_GPIOCON_REG through AD717X_OFFSET0_REG */
	preg = AD717X_GetReg(device, AD717X_GPIOCON_REG);
	if (!preg)
	  	return INVALID_VAL;
	
	while (preg && preg->addr != AD717X_OFFSET0_REG)
	{
		if (preg->addr == AD717X_ID_REG)
		{
			preg ++;
			continue;
		}
		
		ret = AD717X_WriteRegister(device, preg->addr);
		if (ret < 0)
			break;
		preg ++;
	}

	return ret;
}
