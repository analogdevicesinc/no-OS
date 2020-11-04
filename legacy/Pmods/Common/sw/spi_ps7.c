/**************************************************************************//**
*   @file   spi_ps7.c
*   @brief  ZYNQ Hardware SPI functions implementation.
*   @author ATofan (alexandru.tofan@analog.com)
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
#include "system_config.h"
#include "xparameters.h"
#include "xil_io.h"

#if(USE_PS7 == 1)
/*****************************************************************************/
/************************ Constants Definitions ******************************/
/*****************************************************************************/
#define MAX_TX_SIZE	16 /* Maximum number of bytes that can be transmitted in one SPI transfer */

typedef struct _stHwSpiConfig
{
    u32     hwBaseAddr;
    u32     config;
}stHwSpiConfig;

static stHwSpiConfig    hwSpiConfig[2] = {{0, 0}, {0, 0}};
static char emptyTxBuf[MAX_TX_SIZE];

/**************************************************************************//**
* @brief Initializes the communication with the Microblaze SPI peripheral.
*
* @param hwBaseAddr - Microblaze SPI peripheral AXI base address.
* @param lsbFirst - Set to 1 if the data is transmitted LSB first.
* @param cpha - Set to 1 if CPHA mode is used.
* @param cpol - Set to 1 if CPOL mode is used.
* @return TRUE
******************************************************************************/
u32 SPI_Init_ps7(u32 hwBaseAddr, char lsbFirst, char cpha, char cpol)
{
    u32 i = 0;
	u32 cfgValue = 0;

	//set the empty Tx buffer to 0
	for(i = 0; i < MAX_TX_SIZE; i++)
	{
		emptyTxBuf[i] = 0;
	}

	// Configuration Register Settings
	cfgValue |= (1 << Modefail_gen_en)      |
                (0 << Man_start_com)        |
                (0 << Man_start_en)         |
                (1 << Manual_CS)            |
                (0x0F << CS_Bits)           |
                (0 << PERI_SEL)             |
                (0 << SPI_REF_CLK)          |
                (4 << SPI_BAUD_RATE_DIV)    |
                (cpha << SPI_CLK_PH)        |
                (cpol << SPI_CLK_POL)       |
                (1 << SPI_MODE_SEL);
                
    //initialize the SPI configuration
    for(i = 0; i < sizeof(hwSpiConfig) / sizeof(stHwSpiConfig); i++)
    {
    	if(hwSpiConfig[i].hwBaseAddr == 0)
    	{
    		hwSpiConfig[i].hwBaseAddr = hwBaseAddr;
    		hwSpiConfig[i].config	  = cfgValue;
    		break;
    	}
    }

    Xil_Out32(hwBaseAddr + HW_SPI_CONFIG_REG, cfgValue);

    // Clear RX FIFO
    for(i = 0; i < 128; i++)
    {
    	Xil_In32(hwBaseAddr + HW_SPI_RXDATA_REG);
    }

    return TRUE;
}

/**************************************************************************//**
* @brief Transfers data to and from a SPI slave.
*
* @param axiBaseAddr - Microblaze SPI peripheral AXI base address.
* @param txSize - Number of bytes to transmit to the SPI slave.
* @param txBuffer - Buffer which holds the data to be transmitted to the SPI slave.
* @param rxSize - Number of bytes to receive from the SPI slave.
* @param txBuffer - Buffer to store the data read to the SPI slave.
* @param ssNo - Slave select line on which the slave is connected.
* @return TRUE.
******************************************************************************/
u32 SPI_TransferData_ps7(u32 hwBaseAddr, char txSize, char* txBuf, char rxSize, char* rxBuf, char ssNo)
{
    u32 i 			= 0;
	u32 cfgValue  	= 0;
    u32 SPIStatus 	= 0;
    u32 rxCnt 		= 0;
    u32 txCnt 		= 0;
    u32 flushCnt	= 0;
    u32 flush		= 0;

    // Get the configuration of the SPI core
    for(i = 0; i < sizeof(hwSpiConfig) / sizeof(stHwSpiConfig); i++)
	{
		if(hwSpiConfig[i].hwBaseAddr == hwBaseAddr)
		{
			cfgValue = hwSpiConfig[i].config;
			break;
		}
	}

    // Check if the transmit buffer is empty
    if(txSize == 0)
    {
    	txSize = rxSize;
    	txBuf = emptyTxBuf;
    }

    // Write configuration data to Master SPI Device
    Xil_Out32(hwBaseAddr + HW_SPI_CONFIG_REG, cfgValue);

    // Write configuration data to manually assert CS
    Xil_Out32(hwBaseAddr + HW_SPI_CONFIG_REG, cfgValue & ~(1 << CS_Bits));

    // Write TX FIFO Not Full Level
	Xil_Out32(hwBaseAddr + HW_SPI_TX_THRES_REG, 0x01);

	// Enable TX FIFO Not Full Interrupts
	Xil_Out32(hwBaseAddr + HW_SPI_INTR_STS_REG, (1 << SPI_TX_FIFO_not_full));
	Xil_Out32(hwBaseAddr + HW_SPI_INTR_EN_REG, (1 << SPI_TX_FIFO_not_full));

    // Write initial data to TX FIFO
	Xil_Out32(hwBaseAddr + HW_SPI_TXDATA_REG, txBuf[txCnt]);
	txCnt++;

	// Write rest of the data to TX FIFO
	while(txCnt < txSize)
	{
		Xil_Out32(hwBaseAddr + HW_SPI_TXDATA_REG, txBuf[txCnt]);
		txCnt++;
	}

	// Enable Controller
	Xil_Out32(hwBaseAddr + HW_SPI_EN_REG, 0x01);

	// Wait for TX FIFO to empty
	do
	{
		SPIStatus = Xil_In32(hwBaseAddr + HW_SPI_INTR_STS_REG);
	}
	while((SPIStatus & (1 << 2)) == 0x00);

	// Clear Interrupt
	Xil_Out32(hwBaseAddr + HW_SPI_INTR_STS_REG, (1 << 2));

	// Deassert CS
	Xil_Out32(hwBaseAddr + HW_SPI_CONFIG_REG, cfgValue);

	while(rxCnt < rxSize)
	{
		rxBuf[rxCnt] = Xil_In32(hwBaseAddr + HW_SPI_RXDATA_REG);
		rxCnt++;
	}

	while(flushCnt < txSize - rxSize)
	{
		flush = Xil_In32(hwBaseAddr + HW_SPI_RXDATA_REG);
		flushCnt++;
	}

	// Disable SPI
	Xil_Out32(hwBaseAddr + HW_SPI_EN_REG, 0x00);

	return(TRUE);
}
#endif
