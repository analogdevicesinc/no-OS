/***************************************************************************//**
* @file ad77681_evb.c
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

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD77681_DMA_1_BASEADDR				XPAR_AXI_AD77681_DMA_1_BASEADDR
#define AD77681_DMA_2_BASEADDR				XPAR_AXI_AD77681_DMA_2_BASEADDR
#define AD77681_SPI1_ENGINE_BASEADDR		XPAR_SPI_ADC1_AXI_1_BASEADDR
#define AD77681_SPI2_ENGINE_BASEADDR		XPAR_SPI_ADC2_AXI_2_BASEADDR
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

#define GPIO_1_0							GPIO_OFFSET + 22 // 54
#define GPIO_1_1							GPIO_OFFSET + 21 // 53
#define GPIO_1_2							GPIO_OFFSET + 20 // 52
#define GPIO_1_3							GPIO_OFFSET + 19 // 51
#define GPIO_1_SYNC_IN						GPIO_OFFSET + 18 // 50
#define GPIO_1_SYNC_OUT						GPIO_OFFSET + 17 // 49
#define GPIO_1_RESET						GPIO_OFFSET + 16 // 48

uint32_t spi_msg_cmds[6] = {CS_DEASSERT, CS_ASSERT, CS_DEASSERT, TRANSFER_BYTES_W(2), TRANSFER_BYTES_R(4), CS_ASSERT};

struct ad77681_init_param ADC_default_init_param = {
	/* SPI */
	{
		AD77681_SPI1_ENGINE_BASEADDR,	// adc_baseaddr
		AD77681_SPI_CS,			// chip_select
		1,				// cs_delay
		SPI_ENGINE_CONFIG_CPOL |
		SPI_ENGINE_CONFIG_CPHA,		// spi_config
		1000000,			// spi_clk_hz
		1000000,			// spi_clk_hz_reg_access
		100000000,			// ref_clk_hz
		1,				// spi_offload_rx_support_en
		AD77681_DMA_1_BASEADDR,		// spi_offload_rx_dma_baseaddr
		1,				// spi_offload_tx_support_en
		AD77681_DMA_1_BASEADDR,		// spi_offload_tx_dma_baseaddr
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

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void mdelay(uint32_t msecs)
{
	usleep(msecs * 1000);
}

#define SPI_ENGINE_OFFLOAD_EXAMPLE	0

int main()
{
	struct ad77681_dev	*adc_dev;
	spi_eng_msg 		*msg;
	uint8_t			adc_data[5];
	uint8_t 		*data;
	uint32_t 		i;

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
	} else { // offload example
		msg = (spi_eng_msg *)malloc(sizeof(*msg));
		if (!msg)
			return -1;

		msg->spi_msg_cmds = malloc(sizeof(spi_msg_cmds));
		msg->spi_msg_cmds = spi_msg_cmds;
		msg->rx_buf_addr = 0x800000;
		msg->tx_buf_addr = 0xA000000;
		msg->msg_cmd_len = sizeof(spi_msg_cmds) / sizeof(uint32_t);
		msg->tx_buf[0] = AD77681_REG_READ(AD77681_REG_ADC_DATA);
		msg->tx_buf[1] = 0x00;

		spi_eng_offload_load_msg(adc_dev->spi_desc, msg);
		spi_eng_transfer_multiple_msgs(adc_dev->spi_desc, 8);

		data = (uint8_t*)adc_dev->spi_desc->rx_dma_startaddr;

		mdelay(10000);

		for(i = 0; i < adc_dev->spi_desc->rx_length; i++) {
			printf("%x\r\n", *data);
			data += sizeof(uint8_t);
		}
		free(msg);
	}

	printf("Bye\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
