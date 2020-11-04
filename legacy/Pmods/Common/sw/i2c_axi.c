/**************************************************************************//**
*   @file   i2c_axi.c
*   @brief  I2C functions implementation.
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
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "i2c_axi.h"
#include "xil_io.h"

/*****************************************************************************/
/************************ Variables Definitions ******************************/
/*****************************************************************************/

/**************************************************************************//**
* @brief Delays the program execution with the specified number of ms.
*
* @param ns_count Number of ms with which the program must be delayed.
* @return None.
******************************************************************************/
void delay_ms(u32 ms_count)
{
	u32 count;
	for (count = 0; count < ((ms_count * 100000) + 1); count++)
	{
	  asm("nop");
	}
}

/**************************************************************************//**
* @brief Initializes the communication with the Microblaze I2C peripheral.
*
* @param axiBaseAddr - Microblaze I2C peripheral AXI base address.
* @param i2cAddr - The address of the I2C slave.
* @return TRUE.
******************************************************************************/
u32	I2C_Init_axi(u32 axiBaseAddr, u32 i2cAddr)
{
	//disable the I2C core
	Xil_Out32((axiBaseAddr + CR), 0x00);
	//set the Rx FIFO depth to maximum
	Xil_Out32((axiBaseAddr + RX_FIFO_PIRQ), 0x0F);
	//reset the I2C core and flush the Tx fifo
	Xil_Out32((axiBaseAddr + CR), 0x02);
	//enable the I2C core
	Xil_Out32((axiBaseAddr + CR), 0x01);

	return TRUE;
}

/**************************************************************************//**
* @brief Reads data from an I2C slave.
*
* @param axiBaseAddr - Microblaze I2C peripheral AXI base address.
* @param i2cAddr - The address of the I2C slave.
* @param regAddr - Address of the I2C register to be read. 
*				   Must be set to -1 if it is not used.
* @param rxSize - Number of bytes to read from the slave.
* @param rxBuf - Buffer to store the read data.
* @return Returns the number of bytes read.
******************************************************************************/
u32 I2C_Read_axi(u32 axiBaseAddr, u32 i2cAddr, u32 regAddr, u32 rxSize, unsigned char* rxBuf)
{
	u32 rxCnt   = 0;
	u32 timeout = 0xFFFFFF;

	// Reset tx fifo
	Xil_Out32((axiBaseAddr + CR), 0x002);
	// Enable iic
	Xil_Out32((axiBaseAddr + CR), 0x001);
	delay_ms(10);

	if(regAddr != -1)
	{
		// Set the slave I2C address
		Xil_Out32((axiBaseAddr + TX_FIFO), (0x100 | (i2cAddr << 1)));
		// Set the slave register address
		Xil_Out32((axiBaseAddr + TX_FIFO), regAddr);
	}

	// Set the slave I2C address
	Xil_Out32((axiBaseAddr + TX_FIFO), (0x101 | (i2cAddr << 1)));
	// Start a read transaction
	Xil_Out32((axiBaseAddr + TX_FIFO), 0x200 + rxSize);

	// Read data from the I2C slave
	while(rxCnt < rxSize)
	{
		//wait for data to be available in the RxFifo
		while((Xil_In32(axiBaseAddr + SR) & 0x00000040) && (timeout--));
		if(timeout == -1)
		{
			//disable the I2C core
			Xil_Out32((axiBaseAddr + CR), 0x00);
			//set the Rx FIFO depth to maximum
			Xil_Out32((axiBaseAddr + RX_FIFO_PIRQ), 0x0F);
			//reset the I2C core and flush the Tx fifo
			Xil_Out32((axiBaseAddr + CR), 0x02);
			//enable the I2C core
			Xil_Out32((axiBaseAddr + CR), 0x01);			
			return rxCnt;
		}
		timeout = 0xFFFFFF;

		//read the data
		rxBuf[rxCnt] = Xil_In32(axiBaseAddr + RX_FIFO) & 0xFFFF;

		//increment the receive counter
		rxCnt++;
	}

	delay_ms(10);

	return rxCnt;
}

/**************************************************************************//**
* @brief Writes data to an I2C slave.
*
* @param axiBaseAddr - Microblaze I2C peripheral AXI base address.
* @param i2cAddr - The address of the I2C slave.
* @param regAddr - Address of the I2C register to be read. 
*				   Must be set to -1 if it is not used.
* @param txSize - Number of bytes to write to the slave.
* @param txBuf - Buffer to store the data to be transmitted.
* @return Returns the number of bytes written.
******************************************************************************/
u32 I2C_Write_axi(u32 axiBaseAddr, u32 i2cAddr, u32 regAddr, u32 txSize, unsigned char* txBuf)
{
	u32 txCnt = 0;

	// Reset tx fifo
	Xil_Out32((axiBaseAddr + CR), 0x002);
	// enable iic
	Xil_Out32((axiBaseAddr + CR), 0x001);
	delay_ms(10);

	// Set the I2C address
	Xil_Out32((axiBaseAddr + TX_FIFO), (0x100 | (i2cAddr << 1)));
	if(regAddr != -1)
	{
		// Set the slave register address
		Xil_Out32((axiBaseAddr + TX_FIFO), regAddr);
	}

	// Write data to the I2C slave
	while(txCnt < txSize)
	{
		// put the Tx data into the Tx FIFO
		Xil_Out32((axiBaseAddr + TX_FIFO), (txCnt == txSize - 1) ? (0x200 | txBuf[txCnt]) : txBuf[txCnt]);
		txCnt++;
	}

	delay_ms(10);

	return txCnt;
}
