/**************************************************************************//**
*   @file   spi_interface.c
*   @brief  SPI interface functions implementation.
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
#include "spi_interface.h"

/*****************************************************************************/
/************************ Constants Definitions ******************************/
/*****************************************************************************/
static const stDevConfig devConfig[] = 
{
    /* AD9122 */
    {
        8, 8, 
        (SPI_4_WIRE_MODE | SPI_CS_LOW_AT_TRANFER_END | 
        SPI_SAMPLE_AT_CLK_MIDDLE | SPI_TX_ON_CLK_FALL |
        SPI_CLK_IDLE_LOW | SPI_CLK_FOSC_DIV_4),
        (1 << SPI_SEL_AD9122)
    },
    /* AD9643 */
    {
        16, 8, 
        (SPI_3_WIRE_MODE | SPI_CS_LOW_AT_TRANFER_END | 
        SPI_SAMPLE_AT_CLK_MIDDLE | SPI_TX_ON_CLK_FALL |
        SPI_CLK_IDLE_LOW | SPI_CLK_FOSC_DIV_4),
        (1 << SPI_SEL_AD9643)
    },
    /* AD9548 */
    {
        16, 8, 
        (SPI_3_WIRE_MODE | SPI_CS_LOW_AT_TRANFER_END | 
        SPI_SAMPLE_AT_CLK_MIDDLE | SPI_TX_ON_CLK_FALL |
        SPI_CLK_IDLE_LOW | SPI_CLK_FOSC_DIV_4),
        (1 << SPI_SEL_AD9548)
    },
    /* AD9523 */
    {
        16, 8, 
        (SPI_3_WIRE_MODE | SPI_CS_LOW_AT_TRANFER_END | 
        SPI_SAMPLE_AT_CLK_MIDDLE | SPI_TX_ON_CLK_FALL |
        SPI_CLK_IDLE_LOW | SPI_CLK_FOSC_DIV_4),
        (1 << SPI_SEL_AD9523)
    },
    /* ADF4351_RX */
    {
        0, 32, 
        (SPI_4_WIRE_MODE | SPI_CS_LOW_AT_TRANFER_END | 
        SPI_SAMPLE_AT_CLK_MIDDLE | SPI_TX_ON_CLK_FALL |
        SPI_CLK_IDLE_LOW | SPI_CLK_FOSC_DIV_4),
        (1 << SPI_SEL_ADF4351_RX)
    },
    /* ADF4351_TX */
    {
        0, 32, 
        (SPI_4_WIRE_MODE | SPI_CS_LOW_AT_TRANFER_END | 
        SPI_SAMPLE_AT_CLK_MIDDLE | SPI_TX_ON_CLK_FALL |
        SPI_CLK_IDLE_LOW | SPI_CLK_FOSC_DIV_4),
        (1 << SPI_SEL_ADF4351_TX)
    },
    /* AD8366_TX */
    {
        0, 16, 
        (SPI_4_WIRE_MODE | SPI_CS_LOW_AT_TRANFER_END | 
        SPI_SAMPLE_AT_CLK_MIDDLE | SPI_TX_ON_CLK_FALL |
        SPI_CLK_IDLE_LOW | SPI_CLK_FOSC_DIV_4),
        (1 << SPI_SEL_AD8366)
    }
};

/**************************************************************************//**
* @brief Configures the PIC for the next data transfer with the device
*
* @param spiSel - SPI CS number
* @param rxCnt - Number of bytes to read from the device
* @param csState - State of CS line at the end of the transfer
*
* @return None
******************************************************************************/
void PIC_Config(uint32_t spiSel, uint32_t rxCnt, uint8_t csState)
{
    uint8_t wrSize;
    uint8_t wrBuf[8];
    uint32_t spiConfig = devConfig[spiSel].spiConfig;

    /* Add to the SPI configuration the Rx size and the CS state */
    spiConfig += SPI_RX_TRANSFER_CNT(rxCnt);
    spiConfig |= csState;
    
    /* Build the PIC configuration command */
    wrSize = 5;
    wrBuf[0] = CTRL_WRITE;
    wrBuf[1] = ((spiConfig >> 8) & 0xFF);
    wrBuf[2] = (spiConfig) & 0xFF;
    wrBuf[3] = (devConfig[spiSel].spiCS >> 8) & 0xFF;
    wrBuf[4] = (devConfig[spiSel].spiCS) & 0xFF;

    /* Write data to the PIC */
    I2C_Write(IICSEL_PIC, -1, wrSize, wrBuf);
}

/**************************************************************************//**
* @brief Writes data to the PIC
*
* @param spiSel - SPI CS number
* @param size - Number of bytes to be written
* @param data - Data to be written
*
* @return Returns the number of written bytes
******************************************************************************/
uint32_t PIC_Write(uint32_t spiSel, uint8_t size, uint32_t data)
{
    uint8_t wrSize;
    uint8_t wrBuf[8];
   
    /* Build the write buffer */
    wrSize = size + 1;    
    wrBuf[0] = DATA_WRITE;
    while(size)
    {
        wrBuf[size] = data & 0xFF;
        data >>= 8;
        size--;
    }

    /* Write data to the  PIC */
    return (I2C_Write(IICSEL_PIC, -1, wrSize, wrBuf) - 1);
}

/**************************************************************************//**
* @brief Reads data from the PIC
*
* @param spiSel - SPI CS number
* @param size - The number of bytes to be read
* @param data - Variable to store the read data
*
* @return Returns the number of bytes read from the device
******************************************************************************/
uint32_t PIC_Read(uint32_t spiSel, uint8_t size, uint32_t* data)
{
    int32_t i = 0;
    uint32_t rSize = size;
    uint8_t rdBuf[8];

    /* Read data from the  PIC */
    rSize = I2C_Read(IICSEL_PIC, -1, size, rdBuf);
    
    /* Build the result from the read data */
    *data = 0;
    for(i = 0; i < size; i++)
    {
        *data = (*data << 8) | rdBuf[i];
    }

    return rSize;
}

/**************************************************************************//**
* @brief Reads the PIC firmware version
*
* @return Returns the PIC firmware version number or -1 in case of error
******************************************************************************/
int32_t PIC_ReadFwVersion()
{
	int32_t i;
	int32_t ret;
	uint8_t wrBuf[1] = {REV_READ};
	uint8_t rdBuf[PIC_FW_REV_LEN];

	ret = I2C_Write(IICSEL_PIC, -1, 1, wrBuf);
	if(ret == 0)
		return -1;

	ret = I2C_Read(IICSEL_PIC, -1, PIC_FW_REV_LEN, rdBuf);
	if(ret != PIC_FW_REV_LEN)
		return -1;

	for(i = 0; i < PIC_FW_REV_LEN - 1; i++)
	{
		if(rdBuf[i] == 'F' && rdBuf[i+1] == 'W')
			break;
	}
	if(i < PIC_FW_REV_LEN - 5)
		ret = (rdBuf[i+2] - 0x30)*100 + (rdBuf[i+3] - 0x30)*10 + (rdBuf[i+4] - 0x30)*1;
	else
		ret = 0;

	return ret;
}

/**************************************************************************//**
* @brief Initializes the communication with the PIC
*
* @return Returns -1 in case of error, 0 for success
******************************************************************************/
int32_t SPI_Init()
{
	uint8_t wrBuf[1] = {0x02};

	int ret = I2C_Write(IICSEL_PIC, -1,
						sizeof(wrBuf)/sizeof(unsigned char), wrBuf);

	return (ret == 0 ? -1 : 0);
}

/**************************************************************************//**
* @brief Reads data from the selected device
*
* @param spiSel - SPI CS number
* @param regAddr - The adress of the register to be read
* @param data - Buffer to store the read data
*
* @return Returns -1 in case of error, 0 for success
******************************************************************************/
int32_t SPI_Read(uint32_t spiSel, uint32_t regAddr, uint32_t* data)
{
    uint32_t addr;
    uint32_t rSize;

    /* Write the address */
    if (devConfig[spiSel].addrWidth) 
    {
        addr = regAddr;

        PIC_Config(spiSel, 0, SPI_CS_LOW_AT_TRANFER_END);
        PIC_Write(spiSel, devConfig[spiSel].addrWidth / 8, addr);
    }

    /* Configure the PIC for a read operation */
    PIC_Config(spiSel, devConfig[spiSel].dataWidth / 8, SPI_CS_HIGH_AT_TRANFER_END);

    /* Read data from the device */
    rSize = PIC_Read(spiSel, devConfig[spiSel].dataWidth / 8, data);

    return ((rSize != devConfig[spiSel].dataWidth / 8) ? -1 : 0);
}

/**************************************************************************//**
* @brief Writes data to the selected device
*
* @param spiSel - SPI CS number
* @param regAddr - The adress of the register to be written
* @param data - Data to be written to the specified register
*
* @return Returns -1 in case of error, 0 for success
******************************************************************************/
int32_t SPI_Write(uint32_t spiSel, uint32_t regAddr, uint32_t data)
{
    uint32_t wData;
    uint32_t wSize;

    wData = (regAddr << devConfig[spiSel].dataWidth) | data;

    /* Configure the PIC */
    PIC_Config(spiSel, 0, SPI_CS_HIGH_AT_TRANFER_END);

    /* Write data to the device */
    wSize = PIC_Write(spiSel, (devConfig[spiSel].dataWidth + devConfig[spiSel].addrWidth) / 8, wData);

    return ((wSize != (devConfig[spiSel].dataWidth + devConfig[spiSel].addrWidth) / 8) ? -1 : 0);
}
