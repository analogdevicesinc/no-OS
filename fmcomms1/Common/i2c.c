/**************************************************************************//**
*   @file   i2c.c
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
#include "i2c.h"
#include "xparameters.h"
#include "xil_io.h"
#include "timer.h"

/*****************************************************************************/
/******************* I2C Registers Definitions *******************************/
/*****************************************************************************/
#define GIE          0x01C
#define ISR       	 0x020
#define IER       	 0x028
#define SOFTR      	 0x040
#define CR      	 0x100
#define SR      	 0x104
#define TX_FIFO  	 0x108
#define RX_FIFO  	 0x10C
#define ADR       	 0x110
#define TX_FIFO_OCY	 0x114
#define RX_FIFO_OCY	 0x118
#define TEN_ADDR     0x11C
#define RX_FIFO_PIRQ 0x120
#define GPO			 0x124

/*****************************************************************************/
/************************ Variables/Constants Definitions ********************/
/*****************************************************************************/
#define I2C_DELAY	 200 //delay in us between I2C operations
#define I2C_TIMEOUT	 0xFFFFFF 	//timeout for I2C operations

/**************************************************************************//**
* @brief Delays the program execution with the specified number of us.
*
* @param us_count Number of us with which the program must be delayed.
* @return None.
******************************************************************************/
void delay_us(uint32_t us_count)
{
	TIMER0_WAIT(XPAR_AXI_TIMER_0_BASEADDR, us_count*1000);
}

/**************************************************************************//**
* @brief Initializes the communication with the Microblaze I2C peripheral.
*
* @param i2cAddr - The address of the I2C slave.
* @return Returns 0 or negative error code.
******************************************************************************/
uint32_t I2C_Init(uint32_t i2cAddr)
{
	//disable the I2C core
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x00);
	//set the Rx FIFO depth to maximum
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + RX_FIFO_PIRQ), 0x0F);
	//reset the I2C core and flush the Tx fifo
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x02);
	//enable the I2C core
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x01);

	return 0;
}

/**************************************************************************//**
* @brief Reads data from an I2C slave.
*
* @param i2cAddr - The address of the I2C slave.
* @param regAddr - Address of the I2C register to be read. 
*				   Must be set to -1 if it is not used.
* @param rxSize - Number of bytes to read from the slave.
* @param rxBuf - Buffer to store the read data.
* @return Returns the number of bytes read.
******************************************************************************/
uint32_t I2C_Read(uint32_t i2cAddr, 
                  uint32_t regAddr, uint32_t rxSize, uint8_t* rxBuf)
{
	uint32_t rxCnt = 0;
	uint32_t timeout = I2C_TIMEOUT;

	// Reset tx fifo
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x002);
	// Enable iic
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x001);
	delay_us(I2C_DELAY);

	if(regAddr != -1)
	{
		// Set the slave I2C address
		Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + TX_FIFO), (0x100 | (i2cAddr << 1)));
		// Set the slave register address
		Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + TX_FIFO), regAddr);
	}

	// Set the slave I2C address
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + TX_FIFO), (0x101 | (i2cAddr << 1)));
	// Start a read transaction
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + TX_FIFO), 0x200 + rxSize);

	// Read data from the I2C slave
	while(rxCnt < rxSize)
	{
		//wait for data to be available in the RxFifo
		while((Xil_In32(XPAR_AXI_IIC_0_BASEADDR + SR) & 0x00000040) && (timeout--));
		if(timeout == -1)
		{
			//disable the I2C core
			Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x00);
			//set the Rx FIFO depth to maximum
			Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + RX_FIFO_PIRQ), 0x0F);
			//reset the I2C core and flush the Tx fifo
			Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x02);
			//enable the I2C core
			Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x01);			
			return rxCnt;
		}
		timeout = I2C_TIMEOUT;

		//read the data
		rxBuf[rxCnt] = Xil_In32(XPAR_AXI_IIC_0_BASEADDR + RX_FIFO) & 0xFFFF;

		//increment the receive counter
		rxCnt++;
	}

	delay_us(I2C_DELAY);

	return rxCnt;
}

/**************************************************************************//**
* @brief Writes data to an I2C slave.
*
* @param i2cAddr - The address of the I2C slave.
* @param regAddr - Address of the I2C register to be read. 
*				   Must be set to -1 if it is not used.
* @param txSize - Number of bytes to write to the slave.
* @param txBuf - Buffer to store the data to be transmitted.
* @return Returns the number of bytes written.
******************************************************************************/
uint32_t I2C_Write(uint32_t i2cAddr, 
                   uint32_t regAddr, uint32_t txSize, uint8_t* txBuf)
{
	uint32_t txCnt = 0;
	uint32_t timeout = I2C_TIMEOUT;

	// Reset tx fifo
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x002);
	// enable iic
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + CR), 0x001);
	delay_us(I2C_DELAY);

	// Set the I2C address
	Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + TX_FIFO), (0x100 | (i2cAddr << 1)));
	if(regAddr != -1)
	{
		// Set the slave register address
		Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + TX_FIFO), regAddr);
	}

	// Write data to the I2C slave
	while((txCnt < txSize) && (timeout))
	{
		timeout = I2C_TIMEOUT;
		// put the Tx data into the Tx FIFO
		Xil_Out32((XPAR_AXI_IIC_0_BASEADDR + TX_FIFO), (txCnt == txSize - 1) ? (0x200 | txBuf[txCnt]) : txBuf[txCnt]);
		while (((Xil_In32(XPAR_AXI_IIC_0_BASEADDR + SR) & 0x80) == 0x00) && (--timeout));
		txCnt++;
	}
	delay_us(I2C_DELAY);

	return (timeout ? txCnt : 0);
}
