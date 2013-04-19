/**************************************************************************//**
*   @file   spi.c
*   @brief  SPI functions implementations.
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
#include "spi.h"
#include "xil_io.h"

extern void delay_ms(u32 ms_count);

/*****************************************************************************/
/************************ Constants Definitions ******************************/
/*****************************************************************************/
#define MAX_TX_SIZE	16 /*!< Maximum number of bytes that can be transmitted in one SPI transfer */

/*****************************************************************************/
/************************ Variables Definitions ******************************/
/*****************************************************************************/
typedef struct _stSpiConfig
{
	u32 	axiBaseAddr; /*!< Base address of the SPI Microblaze IP */
	u32		config;	 	 /*!< Configuration of the SPI Microblaze IP  */
}stSpiConfig;

static stSpiConfig spiConfig[2] = {{0,0}, {0,0}};
static char emptyTxBuf[MAX_TX_SIZE];

/**************************************************************************//**
* @brief Initializes the communication with the Microblaze SPI peripheral.
*
* @param axiBaseAddr - Microblaze SPI peripheral AXI base address.
* @param lsbFirst    - Set to 1 if the data is transmitted LSB first.
* @param cpha        - Set to 1 if CPHA mode is used.
* @param cpol        - Set to 1 if CPOL mode is used.
* @return TRUE
******************************************************************************/
u32 SPI_Init(u32 axiBaseAddr, char lsbFirst, char cpha, char cpol)
{
    u32 i        = 0;
	u32 cfgValue = 0;

	//set the empty Tx buffer to 0
	for(i = 0; i < MAX_TX_SIZE; i++)
	{
		emptyTxBuf[i] = 0;
	}

	// Configuration Register Settings
	cfgValue |= (lsbFirst 	<< LSBFirst)         | // MSB First transfer format
				(1 			<< MasterTranInh)    | // Master transactions disabled
				(1 			<< ManualSlaveAssEn) | // Slave select output follows data in slave select register
				(1 			<< RxFifoReset)      | // Receive FIFO normal operation
				(0 			<< TxFifoReset)      | // Transmit FIFO normal operation
				(cpha 		<< CHPA)             | // Data valid on first SCK edge
				(cpol 		<< CPOL)             | // Active high clock, SCK idles low
				(1 			<< Master)           | // SPI in Master configuration mode
				(1 			<< SPE)              | // SPI enabled
				(0 			<< LOOP);              // Normal operation

    //initialize the SPI configuration
    for(i = 0; i < sizeof(spiConfig) / sizeof(stSpiConfig); i++)
    {
    	if(spiConfig[i].axiBaseAddr == 0)
    	{
    		spiConfig[i].axiBaseAddr = axiBaseAddr;
    		spiConfig[i].config	 	 = cfgValue;
    		break;
    	}
    }

    //set the slave select register to all ones
    Xil_Out32(axiBaseAddr + SPISSR, 0xFFFFFFFF);

    // Set corresponding value to the Configuration Register
    Xil_Out32(axiBaseAddr + SPICR, cfgValue);

    return TRUE;
}

/**************************************************************************//**
* @brief Transfers data to and from a SPI slave.
*
* @param axiBaseAddr - Microblaze SPI peripheral AXI base address.
* @param txSize      - Number of bytes to transmit to the SPI slave.
* @param txBuffer    - Buffer which holds the data to be transmitted to the SPI slave.
* @param rxSize      - Number of bytes to receive from the SPI slave.
* @param txBuffer    - Buffer to store the data read to the SPI slave.
* @param ssNo        - Slave select line on which the slave is connected.
* @return TRUE.
******************************************************************************/
u32 SPI_TransferData(u32 axiBaseAddr, char txSize, char* txBuf, char rxSize, char* rxBuf, char ssNo)
{
    u32 i         = 0;
    u32 cfgValue  = 0;
    u32 SPIStatus = 0;
    u32 rxCnt     = 0;
    u32 txCnt     = 0;
    u32 timeout   = 0xFFFF;

    // Get the configuration of the SPI core
    for(i = 0; i < sizeof(spiConfig) / sizeof(stSpiConfig); i++)
	{
		if(spiConfig[i].axiBaseAddr == axiBaseAddr)
		{
			cfgValue = spiConfig[i].config;
			break;
		}
	}

    // Check if the transmit buffer is empty
    if(txSize == 0)
    {
    	txSize = rxSize;
    	txBuf  = emptyTxBuf;
    }

    // Write configuration data to master SPI device SPICR
    Xil_Out32(axiBaseAddr + SPICR, cfgValue);

    // Write to SPISSR to manually assert SSn
    Xil_Out32(axiBaseAddr + SPISSR, ~(0x00000001 << (ssNo - 1)));

    // Write initial data to master SPIDTR register
    Xil_Out32(axiBaseAddr + SPIDTR, txBuf[0]);

    // Enable the master transactions
    cfgValue &= ~(1 << MasterTranInh);
    Xil_Out32(axiBaseAddr + SPICR, cfgValue);

    // Send and receive the data
    while(txCnt < txSize)
    {
    	// Poll status for completion
    	do
    	{
    		SPIStatus = Xil_In32(axiBaseAddr + SPISR);
    	}
    	while(((SPIStatus & 0x01) == 1) && timeout--);
    	if(timeout == -1)
    	{
        	// Disable the master transactions
    		cfgValue |= (1 << MasterTranInh);
    		Xil_Out32(axiBaseAddr + SPICR, cfgValue);

    		//reset the SPI core
			Xil_Out32(axiBaseAddr + SRR, 0x0000000A);

			//set the slave select register to all ones
			Xil_Out32(axiBaseAddr + SPISSR, 0xFFFFFFFF);

			// Set corresponding value to the Configuration Register
			Xil_Out32(axiBaseAddr + SPICR, cfgValue);

			return FALSE;
    	}
    	timeout = 0xFFFF;

    	// Read received data from SPI Core buffer
    	if(rxCnt < rxSize)
    	{
    		rxBuf[rxCnt] = Xil_In32(axiBaseAddr + SPIDRR);
    		rxCnt++;
    	}
		// Send next data
    	txCnt++;
    	if(txCnt < txSize)
    	{
        	// Disable the master transactions
    		cfgValue |= (1 << MasterTranInh);
    		Xil_Out32(axiBaseAddr + SPICR, cfgValue);

    		// Write data
    		Xil_Out32(axiBaseAddr + SPIDTR, txBuf[txCnt]);

    		/// Enable the master transactions
			cfgValue &= ~(1 << MasterTranInh);
			Xil_Out32(axiBaseAddr + SPICR, cfgValue);
    	}
    }

    // Disable the master transactions
    cfgValue |= (1 << MasterTranInh);
    Xil_Out32(axiBaseAddr + SPICR, cfgValue);

    // Write all ones to SPISSR
    Xil_Out32(axiBaseAddr + SPISSR, 0xFFFFFFFF);

    delay_ms(1);

    return TRUE;
}

