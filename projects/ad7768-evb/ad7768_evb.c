/***************************************************************************//**
 *   @file   ad7768evb.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>
#include "platform_drivers.h"
#include "ad7768.h"
#include "adc_core.h"
#include <xil_io.h>
#include <stdio.h>

#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define SPI_AD7768_CS				0
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET					32 + 54
#define AD7768_DMA_BASEADDR			XPAR_AD7768_DMA_BASEADDR
#define ADC_DDR_BASEADDR			XPAR_DDR_MEM_BASEADDR + 0x800000
#define GPIO_RESET_N				GPIO_OFFSET + 8
#define GPIO_START_N				GPIO_OFFSET + 9
#define GPIO_SYNC_IN_N				GPIO_OFFSET + 10
#define GPIO_SYNC_OUT_N				GPIO_OFFSET + 11
#define GPIO_MODE_0_GPIO_0			GPIO_OFFSET + 16
#define GPIO_MODE_1_GPIO_1			GPIO_OFFSET + 17
#define GPIO_MODE_2_GPIO_2			GPIO_OFFSET + 18
#define GPIO_MODE_3_GPIO_3			GPIO_OFFSET + 19
#define GPIO_FILTER_GPIO_4			GPIO_OFFSET + 20

/* HDL Control Interface */

#define GPIO_UP_SSHOT				GPIO_OFFSET + 4
#define GPIO_UP_FORMAT_1			GPIO_OFFSET + 3
#define GPIO_UP_FORMAT_0			GPIO_OFFSET + 2
#define GPIO_UP_CRC_ENABLE			GPIO_OFFSET + 1
#define GPIO_UP_CRC_4_OR_16_N		GPIO_OFFSET + 0
#define GPIO_STATUS_DEVICE_ID		XPAR_AD7768_GPIO_DEVICE_ID
#define GPIO_STATUS_OFFSET			0
#define UP_STATUS_CLR_OFFSET		GPIO_STATUS_OFFSET + 0
#define UP_STATUS_OFFSET			GPIO_STATUS_OFFSET + 0

ad7768_init_param default_init_param = {
	/* SPI */
	SPI_AD7768_CS,			// spi_chip_select
	SPI_MODE_0,				// spi_mode
	PS7_SPI,				// spi_type
	SPI_DEVICE_ID,			// spi_device_id
	/* GPIO */
	PS7_GPIO,				// gpio_type
	GPIO_DEVICE_ID,			// gpio_device_id
	GPIO_RESET_N,			// gpio_reset
	GPIO_HIGH,				// gpio_reset_value
	GPIO_MODE_0_GPIO_0,		// gpio_mode0
	GPIO_MODE_1_GPIO_1,		// gpio_mode1
	GPIO_MODE_2_GPIO_2,		// gpio_mode2
	GPIO_MODE_3_GPIO_3,		// gpio_mode3
	/* Configuration */
	1,						// pin_spi_input_value;
	AD7768_ACTIVE,			// sleep_mode
	AD7768_FAST,			// power_mode
	AD7768_MCLK_DIV_4,		// mclk_div
	AD7768_DCLK_DIV_1,		// dclk_div
	AD7768_STANDARD_CONV,	// conv_op
	AD7768_CRC_16,			// crc_sel
};

/***************************************************************************//**
* @brief ad7768evb_clear_status
*******************************************************************************/
void ad7768_evb_clear_status(gpio_device *gpio_dev)
{
	uint8_t i;

	for (i = 0; i < 32; i ++)
	{
		gpio_set_direction(gpio_dev,
				UP_STATUS_OFFSET + i, GPIO_OUT);
		gpio_set_value(gpio_dev,
				UP_STATUS_OFFSET + i, 1);
	}
}

/***************************************************************************//**
* @brief ad7768evb_verify_status
*******************************************************************************/
uint8_t ad7768_evb_verify_status(gpio_device *gpio_dev)
{
	uint8_t i;
	uint8_t value;
	uint8_t status = 0;

	for (i = 0; i < 32; i ++)
	{
		gpio_set_direction(gpio_dev,
				UP_STATUS_OFFSET + i, GPIO_IN);
		gpio_get_value(gpio_dev,
				UP_STATUS_OFFSET + i, &value);
		status += value;
	}

	return status;
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	ad7768_dev	*dev;
	gpio_device	gpio_dev;
	adc_core	core;

	ad7768_setup(&dev, default_init_param);

	/* Configure HDL */
	gpio_set_direction(&dev->gpio_dev, GPIO_UP_FORMAT_1, GPIO_OUT);
	gpio_set_value(&dev->gpio_dev, GPIO_UP_FORMAT_1, 0);
	gpio_set_direction(&dev->gpio_dev, GPIO_UP_FORMAT_0, GPIO_OUT);
	gpio_set_value(&dev->gpio_dev, GPIO_UP_FORMAT_0, 0);
	gpio_set_direction(&dev->gpio_dev, GPIO_UP_CRC_ENABLE, GPIO_OUT);
	gpio_set_value(&dev->gpio_dev, GPIO_UP_CRC_ENABLE,
			dev->crc_sel ? 1 : 0);
	gpio_set_direction(&dev->gpio_dev, GPIO_UP_CRC_4_OR_16_N, GPIO_OUT);
	gpio_set_value(&dev->gpio_dev, GPIO_UP_CRC_4_OR_16_N,
			dev->crc_sel == AD7768_CRC_4 ? 1 : 0);

	gpio_dev.type = AXI_GPIO;
	gpio_dev.device_id = XPAR_AD7768_GPIO_DEVICE_ID;
	gpio_init(&gpio_dev);

	ad7768_evb_clear_status(&gpio_dev);
	if (ad7768_evb_verify_status(&gpio_dev))
		printf("Interface errors\n");
	else
		printf("Interface OK\n");

	core.dmac_baseaddr = AD7768_DMA_BASEADDR;
	core.no_of_channels = 8;
	core.resolution = 24;

	printf("Capture samples...\n");
	adc_capture(core, 1024, ADC_DDR_BASEADDR);
	printf("Capture done\n");

	if (ad7768_evb_verify_status(&gpio_dev))
		printf("Interface errors\n");
	else
		printf("Interface OK\n");

	return 0;
}
