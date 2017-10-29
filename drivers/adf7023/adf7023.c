/***************************************************************************//**
 *   @file   ADF7023.c
 *   @brief  Implementation of ADF7023 Driver.
 *   @author DBogdan (Dragos.Bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "adf7023_config.h"
#include "adf7023.h"

/******************************************************************************/
/*************************** Macros Definitions *******************************/
/******************************************************************************/
#define ADF7023_CS_ASSERT   gpio_set_value(&dev->gpio_dev, dev->gpio_cs, GPIO_LOW)
#define ADF7023_CS_DEASSERT gpio_set_value(&dev->gpio_dev, dev->gpio_cs, GPIO_HIGH)

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/


/***************************************************************************//**
 * @brief Transfers one byte of data.
 *
 * @param dev - The device structure.
 * @param writeByte - Write data.
 * @param readByte - Read data.
 *
 * @return None.
*******************************************************************************/
void ADF7023_WriteReadByte(adf7023_dev *dev,
			   unsigned char writeByte,
                           unsigned char* readByte)
{
    unsigned char data = 0;

    data = writeByte;
	spi_write_and_read(&dev->spi_dev,
			   &data,
			   1);
    if(readByte)
    {
        *readByte = data;
    }
}

/***************************************************************************//**
 * @brief Initializes the ADF7023.
 *
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return ret - Result of the initialization procedure.
 *               Example: 0 - if initialization was successful;
 *                        -1 - if initialization was unsuccessful.
*******************************************************************************/
int32_t ADF7023_Init(adf7023_dev **device,
		     adf7023_init_param init_param)
{
	adf7023_dev *dev;
    unsigned char  miso    = 0;
    unsigned short timeout = 0;
    unsigned char  status  = 0;
	int32_t ret = 0;

	dev = (adf7023_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	dev->spi_dev.type = init_param.spi_type;
	dev->spi_dev.id = init_param.spi_id;
	dev->spi_dev.max_speed_hz = init_param.spi_max_speed_hz;
	dev->spi_dev.mode = init_param.spi_mode;
	dev->spi_dev.chip_select = init_param.spi_chip_select;
	ret = spi_init(&dev->spi_dev);

	/* GPIO */
	dev->gpio_dev.id = init_param.gpio_id;
	dev->gpio_dev.type = init_param.gpio_type;
	ret |= gpio_init(&dev->gpio_dev);

	dev->gpio_cs = init_param.gpio_cs;
	dev->gpio_miso = init_param.gpio_miso;

    dev->ADF7023_BBRAMCurrent = ADF7023_BBRAMDefault;

	if (dev->gpio_cs >= 0) {
		ret |= gpio_set_direction(&dev->gpio_dev,
					  dev->gpio_cs,
					  GPIO_OUT);
		ret |= gpio_set_value(&dev->gpio_dev,
				      dev->gpio_cs,
				      GPIO_HIGH);
	}

    ADF7023_CS_ASSERT;

    while ((miso == 0) && (timeout < 1000))
    {
	gpio_get_value(&dev->gpio_dev, dev->gpio_miso, &miso);
        timeout++;
    }
    if(timeout == 1000)
    {
        ret = -1;
    }
    while(!(status & STATUS_CMD_READY))
    {
        ADF7023_GetStatus(dev, &status);
    }
    ADF7023_SetRAM(dev, 0x100, 64, (unsigned char*)&dev->ADF7023_BBRAMCurrent);
    ADF7023_SetCommand(dev, CMD_CONFIG_DEV);

	*device = dev;

    return ret;
}

/***************************************************************************//**
 * @brief Reads the status word of the ADF7023.
 *
 * @param dev - The device structure.
 * @param status - Status word.
 *
 * @return None.
*******************************************************************************/
void ADF7023_GetStatus(adf7023_dev *dev,
		       unsigned char* status)
{
    ADF7023_CS_ASSERT;
    ADF7023_WriteReadByte(dev, SPI_NOP, 0);
    ADF7023_WriteReadByte(dev, SPI_NOP, status);
    ADF7023_CS_DEASSERT;
}

/***************************************************************************//**
 * @brief Initiates a command.
 *
 * @param dev - The device structure.
 * @param command - Command.
 *
 * @return None.
*******************************************************************************/
void ADF7023_SetCommand(adf7023_dev *dev,
			unsigned char command)
{
    ADF7023_CS_ASSERT;
    ADF7023_WriteReadByte(dev, command, 0);
    ADF7023_CS_DEASSERT;
}

/***************************************************************************//**
 * @brief Sets a FW state and waits until the device enters in that state.
 *
 * @param dev - The device structure.
 * @param fwState - FW state.
 *
 * @return None.
*******************************************************************************/
void ADF7023_SetFwState(adf7023_dev *dev,
			unsigned char fwState)
{
    unsigned char status = 0;

    switch(fwState)
    {
        case FW_STATE_PHY_OFF:
            ADF7023_SetCommand(dev, CMD_PHY_OFF);
            break;
        case FW_STATE_PHY_ON:
            ADF7023_SetCommand(dev, CMD_PHY_ON);
            break;
        case FW_STATE_PHY_RX:
            ADF7023_SetCommand(dev, CMD_PHY_RX);
            break;
        case FW_STATE_PHY_TX:
            ADF7023_SetCommand(dev, CMD_PHY_TX);
            break;
        default:
            ADF7023_SetCommand(dev, CMD_PHY_SLEEP);
    }
    while((status & STATUS_FW_STATE) != fwState)
    {
        ADF7023_GetStatus(dev, &status);
    }
}

/***************************************************************************//**
 * @brief Reads data from the RAM.
 *
 * @param dev - The device structure.
 * @param address - Start address.
 * @param length - Number of bytes to write.
 * @param data - Read buffer.
 *
 * @return None.
*******************************************************************************/
void ADF7023_GetRAM(adf7023_dev *dev,
		    unsigned long address,
                    unsigned long length,
                    unsigned char* data)
{
    ADF7023_CS_ASSERT;
    ADF7023_WriteReadByte(dev, SPI_MEM_RD | ((address & 0x700) >> 8), 0);
    ADF7023_WriteReadByte(dev, address & 0xFF, 0);
    ADF7023_WriteReadByte(dev, SPI_NOP, 0);
    while(length--)
    {
        ADF7023_WriteReadByte(dev, SPI_NOP, data++);
    }
    ADF7023_CS_DEASSERT;
}

/***************************************************************************//**
 * @brief Writes data to RAM.
 *
 * @param dev - The device structure.
 * @param address - Start address.
 * @param length - Number of bytes to write.
 * @param data - Write buffer.
 *
 * @return None.
*******************************************************************************/
void ADF7023_SetRAM(adf7023_dev *dev,
		    unsigned long address,
                    unsigned long length,
                    unsigned char* data)
{
    ADF7023_CS_ASSERT;
    ADF7023_WriteReadByte(dev, SPI_MEM_WR | ((address & 0x700) >> 8), 0);
    ADF7023_WriteReadByte(dev, address & 0xFF, 0);
    while(length--)
    {
        ADF7023_WriteReadByte(dev, *(data++), 0);
    }
    ADF7023_CS_DEASSERT;
}

/***************************************************************************//**
 * @brief Receives one packet.
 *
 * @param dev - The device structure.
 * @param packet - Data buffer.
 * @param length - Number of received bytes.
 *
 * @return None.
*******************************************************************************/
void ADF7023_ReceivePacket(adf7023_dev *dev,
			   unsigned char* packet,
			   unsigned char* length)
{
    unsigned char interruptReg = 0;

    ADF7023_SetFwState(dev, FW_STATE_PHY_ON);
    ADF7023_SetFwState(dev, FW_STATE_PHY_RX);
    while(!(interruptReg & BBRAM_INTERRUPT_MASK_0_INTERRUPT_CRC_CORRECT))
    {
        ADF7023_GetRAM(dev, MCR_REG_INTERRUPT_SOURCE_0,
                       0x1,
                       &interruptReg);
    }
    ADF7023_SetRAM(dev, MCR_REG_INTERRUPT_SOURCE_0,
                   0x1,
                   &interruptReg);
    ADF7023_GetRAM(dev, 0x10, 1, length);
    ADF7023_GetRAM(dev, 0x12, *length - 2, packet);
}

/***************************************************************************//**
 * @brief Transmits one packet.
 *
 * @param dev - The device structure.
 * @param packet - Data buffer.
 * @param length - Number of bytes to transmit.
 *
 * @return None.
*******************************************************************************/
void ADF7023_TransmitPacket(adf7023_dev *dev,
			    unsigned char* packet,
			    unsigned char length)
{
    unsigned char interruptReg = 0;
    unsigned char header[2]    = {0, 0};

    header[0] = 2 + length;
    header[1] = dev->ADF7023_BBRAMCurrent.addressMatchOffset;
    ADF7023_SetRAM(dev, 0x10, 2, header);
    ADF7023_SetRAM(dev, 0x12, length, packet);
    ADF7023_SetFwState(dev, FW_STATE_PHY_ON);
    ADF7023_SetFwState(dev, FW_STATE_PHY_TX);
    while(!(interruptReg & BBRAM_INTERRUPT_MASK_0_INTERRUPT_TX_EOF))
    {
        ADF7023_GetRAM(dev, MCR_REG_INTERRUPT_SOURCE_0,
                       0x1,
                       &interruptReg);
    }
}

/***************************************************************************//**
 * @brief Sets the channel frequency.
 *
 * @param dev - The device structure.
 * @param chFreq - Channel frequency.
 *
 * @return None.
*******************************************************************************/
void ADF7023_SetChannelFrequency(adf7023_dev *dev,
				 unsigned long chFreq)
{
    chFreq = (unsigned long)(((float)chFreq / 26000000) * 65535);
    dev->ADF7023_BBRAMCurrent.channelFreq0 = (chFreq & 0x0000FF) >> 0;
    dev->ADF7023_BBRAMCurrent.channelFreq1 = (chFreq & 0x00FF00) >> 8;
    dev->ADF7023_BBRAMCurrent.channelFreq2 = (chFreq & 0xFF0000) >> 16;
    ADF7023_SetRAM(dev, 0x100, 64, (unsigned char*)&dev->ADF7023_BBRAMCurrent);
}

/***************************************************************************//**
 * @brief Sets the data rate.
 *
 * @param dev - The device structure.
 * @param dataRate - Data rate.
 *
 * @return None.
*******************************************************************************/
void ADF7023_SetDataRate(adf7023_dev *dev,
			 unsigned long dataRate)
{
    dataRate = (unsigned long)(dataRate / 100);
    dev->ADF7023_BBRAMCurrent.radioCfg0 =
        BBRAM_RADIO_CFG_0_DATA_RATE_7_0((dataRate & 0x00FF) >> 0);
    dev->ADF7023_BBRAMCurrent.radioCfg1 &= ~BBRAM_RADIO_CFG_1_DATA_RATE_11_8(0xF);
    dev->ADF7023_BBRAMCurrent.radioCfg1 |=
        BBRAM_RADIO_CFG_1_DATA_RATE_11_8((dataRate & 0x0F00) >> 8);
    ADF7023_SetRAM(dev, 0x100, 64, (unsigned char*)&dev->ADF7023_BBRAMCurrent);
    ADF7023_SetFwState(dev, FW_STATE_PHY_OFF);
    ADF7023_SetCommand(dev, CMD_CONFIG_DEV);
}

/***************************************************************************//**
 * @brief Sets the frequency deviation.
 *
 * @param dev - The device structure.
 * @param freqDev - Frequency deviation.
 *
 * @return None.
*******************************************************************************/
void ADF7023_SetFrequencyDeviation(adf7023_dev *dev,
				   unsigned long freqDev)
{
    freqDev = (unsigned long)(freqDev / 100);
    dev->ADF7023_BBRAMCurrent.radioCfg1 &=
        ~BBRAM_RADIO_CFG_1_FREQ_DEVIATION_11_8(0xF);
    dev->ADF7023_BBRAMCurrent.radioCfg1 |=
        BBRAM_RADIO_CFG_1_FREQ_DEVIATION_11_8((freqDev & 0x0F00) >> 8);
    dev->ADF7023_BBRAMCurrent.radioCfg2 =
        BBRAM_RADIO_CFG_2_FREQ_DEVIATION_7_0((freqDev & 0x00FF) >> 0);
    ADF7023_SetRAM(dev, 0x100, 64, (unsigned char*)&dev->ADF7023_BBRAMCurrent);
    ADF7023_SetFwState(dev, FW_STATE_PHY_OFF);
    ADF7023_SetCommand(dev, CMD_CONFIG_DEV);
}
