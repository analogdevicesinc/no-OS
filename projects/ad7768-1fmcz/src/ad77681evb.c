/***************************************************************************//**
* @file ad77681evb.c
* @brief Implementation of Main Function.
* @author SPopa (stefan.popa@analog.com)
********************************************************************************
* Copyright 2017(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#include <stdio.h>
#include <sleep.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <xil_cache.h>
#include <xparameters.h>
#include "xil_printf.h"
#include "ad77681.h"
#include "spi_engine.h"
#include "delay.h"
#include "error.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD77681_EVB_SAMPLE_NO				8
#define AD77681_DMA_1_BASEADDR				XPAR_AXI_AD77681_DMA_BASEADDR
#define AD77681_SPI1_ENGINE_BASEADDR		XPAR_SPI_ADC_AXI_REGMAP_BASEADDR
#define AD77681_SPI_CS						0

#define GPIO_DEVICE_ID						XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET							32

#define GPIO_0_0							GPIO_OFFSET + 6 // 38
#define GPIO_0_1							GPIO_OFFSET + 5 // 37
#define GPIO_0_2							GPIO_OFFSET + 4 // 36
#define GPIO_0_3							GPIO_OFFSET + 3 // 35
#define GPIO_0_SYNC_IN						GPIO_OFFSET + 2 // 34
#define GPIO_0_SYNC_OUT						GPIO_OFFSET + 1 // 33
#define GPIO_0_RESET						GPIO_OFFSET + 0 // 32

uint32_t spi_msg_cmds[6] = {CS_LOW, CS_HIGH, CS_LOW, WRITE(2), READ(4), CS_HIGH};

struct spi_engine_init_param spi_eng_init_param  = {
	.type = SPI_ENGINE,
	.spi_engine_baseaddr = AD77681_SPI1_ENGINE_BASEADDR,
	.cs_delay = 0,
	.data_width = 32,
};

struct ad77681_init_param ADC_default_init_param = {
	/* SPI */
	{
		.chip_select = AD77681_SPI_CS,
		.max_speed_hz = 1000000,
		.mode = SPI_MODE_3,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	},
	/* Configuration */
	AD77681_FAST,				// power_mode
	AD77681_MCLK_DIV_8,			// mclk_div
	AD77681_CONV_CONTINUOUS,	// conv_mode
	AD77681_POSITIVE_FS,		// diag_mux_sel
	false,						// conv_diag_sel
	AD77681_CONV_16BIT,			// conv_len
	AD77681_CRC, 				// crc_sel
	0 							// status_bit
};

#define SPI_ENGINE_OFFLOAD_EXAMPLE	0

int main()
{
	struct ad77681_dev	*adc_dev;
	uint8_t			adc_data[5];
	uint8_t 		*data;
	uint32_t 		i;
	int32_t ret;
	uint8_t commands_data[2] = {AD77681_REG_READ(AD77681_REG_ADC_DATA), 0};
	struct spi_engine_offload_init_param spi_engine_offload_init_param;
	struct spi_engine_offload_message spi_engine_offload_message;
	struct spi_desc *spi_eng_desc;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ad77681_setup(&adc_dev, ADC_default_init_param);

	if (SPI_ENGINE_OFFLOAD_EXAMPLE == 0) {
		while(1) {
			ad77681_spi_read_adc_data(adc_dev, adc_data);
			printf("[ADC DATA]: 0x");
			for(i = 0; i < sizeof(adc_data) / sizeof(uint8_t); i++) {
				printf("%x", adc_data[i]);
			}
			printf("\r\n");
			mdelay(1000);
		}
	} else {
		ret = spi_engine_offload_init(spi_eng_desc, &spi_engine_offload_init_param);
		if (ret != SUCCESS)
			return FAILURE;

		spi_engine_offload_message.commands = spi_msg_cmds;
		spi_engine_offload_message.no_commands = ARRAY_SIZE(spi_msg_cmds);
		spi_engine_offload_message.commands_data = commands_data;
		spi_engine_offload_message.rx_addr = 0x800000;
		spi_engine_offload_message.tx_addr = 0xA000000;

		ret = spi_engine_offload_transfer(spi_eng_desc, spi_engine_offload_message,
						  AD77681_EVB_SAMPLE_NO);
		if (ret != SUCCESS)
			return ret;

		mdelay(1000);

		Xil_DCacheInvalidateRange(spi_engine_offload_message.rx_addr,
					  AD77681_EVB_SAMPLE_NO * 4);

		for (i = 0; i < AD77681_EVB_SAMPLE_NO * 4 ; i++) {
			printf("%x\r\n", *data);
			data += sizeof(uint8_t);
		}
	}

	printf("Bye\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
