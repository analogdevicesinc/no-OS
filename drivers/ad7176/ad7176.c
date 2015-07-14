/**************************************************************************//**
*   @file   AD7176.c
*   @brief  AD7176 implementation file.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
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

#define AD7176_INIT

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "AD7176.h"

/******************************************************************************/
/************************ Local variables and types ***************************/
/******************************************************************************/
enum CRC_MODE
{
	disable,
	use_CRC,
	use_XOR,
};

struct AD7176_state
{
	enum CRC_MODE useCRC;
}AD7176_st;

/***************************************************************************//**
* @brief Reads the value of the specified register.
*
* @param pReg - Pointer to the register structure holding info about the
*               register to be read. The read value is stored inside the
*               register structure.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7176_ReadRegister(st_reg* pReg)
{
	int32_t ret       = 0;
	uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i         = 0;
	uint8_t check8    = 0;
	uint8_t msgBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	/* Build the Command word */
	buffer[0] = COMM_REG_WEN | COMM_REG_RD | pReg->addr;

	/* Read data from the device */
	ret = SPI_Read(AD7176_SLAVE_ID,
			buffer,
			((AD7176_st.useCRC != disable) ? pReg->size + 1
							: pReg->size) + 1);
	if(ret < 0)
		return ret;

	/* Check the CRC */
	if(AD7176_st.useCRC == use_CRC)
	{
		msgBuf[0] = COMM_REG_WEN | COMM_REG_RD | pReg->addr;
		for(i = 1; i < pReg->size + 2; ++i)
		{
			msgBuf[i] = buffer[i];
		}
		check8 = AD7176_ComputeCRC8(msgBuf, pReg->size + 2);
	}
	if(AD7176_st.useCRC == use_XOR)
	{
		msgBuf[0] = COMM_REG_WEN | COMM_REG_RD | pReg->addr;
		for(i = 1; i < pReg->size + 2; ++i)
		{
			msgBuf[i] = buffer[i];
		}
		check8 = AD7176_ComputeXOR8(msgBuf, pReg->size + 2);
	}

	if(check8 != 0)
	{
		/* ReadRegister checksum failed. */
		return -1;
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
* @param reg - Register structure holding info about the register to be written
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7176_WriteRegister(st_reg reg)
{
	int32_t ret      = 0;
	int32_t regValue = 0;
	uint8_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i        = 0;
	uint8_t crc8     = 0;

	/* Build the Command word */
	wrBuf[0] = COMM_REG_WEN | COMM_REG_WR | reg.addr;

	/* Fill the write buffer */
	regValue = reg.value;
	for(i = 0; i < reg.size; i++)
	{
		wrBuf[reg.size - i] = regValue & 0xFF;
		regValue >>= 8;
	}

	/* Compute the CRC */
	if(AD7176_st.useCRC != disable)
	{
		crc8 = AD7176_ComputeCRC8(wrBuf, reg.size + 1);
		wrBuf[reg.size + 1] = crc8;
	}

	/* Write data to the device */
	ret = SPI_Write(AD7176_SLAVE_ID,
			wrBuf,
			(AD7176_st.useCRC != disable) ? reg.size + 2
							: reg.size + 1);

	return ret;
}

/***************************************************************************//**
* @brief Resets the device.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7176_Reset(void)
{
	int32_t ret = 0;
	uint8_t wrBuf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	ret = SPI_Write(AD7176_SLAVE_ID, wrBuf, 8);

	return ret;
}

/***************************************************************************//**
* @brief Waits until a new conversion result is available.
*
* @param timeout - Count representing the number of polls to be done until the
*                  function returns if no new data is available.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7176_WaitForReady(uint32_t timeout)
{
	int32_t ret;
	int8_t ready = 0;

	while(!ready && --timeout)
	{
		/* Read the value of the Status Register */
		ret = AD7176_ReadRegister(&AD7176_regs[Status_Register]);
		if(ret < 0)
			return ret;

		/* Check the RDY bit in the Status Register */
		ready = (AD7176_regs[Status_Register].value & STATUS_REG_RDY) != 0;
	}

	return timeout ? 0 : -1;
}

/***************************************************************************//**
* @brief Reads the conversion result from the device.
*
* @param pData - Pointer to store the read data.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7176_ReadData(int32_t* pData)
{
	int32_t ret;

	/* Read the value of the Status Register */
	ret = AD7176_ReadRegister(&AD7176_regs[Data_Register]);

	/* Get the read result */
	*pData = AD7176_regs[Data_Register].value;

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
uint8_t AD7176_ComputeCRC8(uint8_t * pBuf, uint8_t bufSize)
{
	uint8_t i   = 0;
	uint8_t crc = 0;

	while(bufSize)
	{
		for(i = 0x80; i != 0; i >>= 1)
		{
			if(((crc & 0x80) != 0) != ((*pBuf & i) != 0)) //MSB of CRC register XOR input Bit from Data
			{
				crc <<= 1;
				crc ^= CRC8_POLYNOMIAL_REPRESENTATION;
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
uint8_t AD7176_ComputeXOR8(uint8_t * pBuf, uint8_t bufSize)
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
* @return None.
*******************************************************************************/
void AD7176_UpdateCRCSetting(void)
{
	/* Get CRC State. */
	if(INTF_MODE_REG_CRC_STAT(AD7176_regs[Interface_Mode_Register].value))
	{
		AD7176_st.useCRC = use_CRC;
	}
	else if(INTF_MODE_REG_XOR_STAT(AD7176_regs[Interface_Mode_Register].value))
	{
		AD7176_st.useCRC = use_XOR;
	}
	else
	{
		AD7176_st.useCRC = disable;
	}
}

/***************************************************************************//**
* @brief Initializes the AD7176.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7176_Setup(void)
{
	int32_t ret;
	enum AD7176_registers regNr;

	/* Initialize the SPI communication. */
	SPI_Init(0, 1000000, 1, 0);

	/*  Reset the device interface.*/
	AD7176_Reset();

	/* Initialize ADC mode register. */
	ret = AD7176_WriteRegister(AD7176_regs[ADC_Mode_Register]);
	if(ret < 0)
		return ret;

	/* Initialize Interface mode register. */
	ret = AD7176_WriteRegister(AD7176_regs[Interface_Mode_Register]);
	if(ret < 0)
		return ret;

	/* Get CRC State */
	AD7176_UpdateCRCSetting();

	/* Initialize registers Data_Register through Filter_Config_4. */
	for(regNr = Data_Register; (regNr < Offset_1) && !(ret < 0); regNr++)
	{
		ret = AD7176_WriteRegister(AD7176_regs[regNr]);
	}

	return ret;
}
