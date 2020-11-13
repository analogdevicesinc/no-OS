/***************************************************************************//**
* @file ad738x_fmc.c
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
#include "spi_engine.h"
#include "ad738x.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD738X_DMA_BASEADDR             XPAR_AXI_AD738X_DMA_BASEADDR
#define AD738X_SPI_ENGINE_BASEADDR      XPAR_SPI_AXI_BASEADDR
#define AD738X_SPI_CS                   0

ad738x_init_param ad738x_default_init_param = {
	/* SPI engine*/
	{
		AD738X_SPI_ENGINE_BASEADDR, // adc_baseaddr
		AD738X_SPI_CS,              // chip_select
		SPI_ENGINE_CONFIG_CPOL,     // spi_config
		1000000,                    // spi_clk_hz
		100000000,                  // ref_clk_hz
		1,                          // spi_offload_rx_support_en
		AD738X_DMA_BASEADDR,        // spi_offload_rx_dma_baseaddr
		1,                          // spi_offload_tx_support_en
		AD738X_DMA_BASEADDR         // spi_offload_tx_dma_baseaddr
	},
	/* Configuration */
	ONE_WIRE_MODE,		// conv_mode
	INT_REF,		// ref_sel
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
	ad738x_dev *dev;
	uint16_t adc_data[2];
	uint16_t *offload_data;
	spi_eng_msg *msg;
	int i;
	uint32_t spi_eng_msg_cmds[6] = {CS_DEASSERT, TRANSFER_BYTES_W(2), CS_ASSERT,
					CS_DEASSERT, TRANSFER_BYTES_R(2), CS_ASSERT
				       };

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ad738x_init(&dev, ad738x_default_init_param);

	if (SPI_ENGINE_OFFLOAD_EXAMPLE == 0) {
		while(1) {
			ad738x_spi_single_conversion(dev, adc_data);
			xil_printf("ADC0: %d\n", adc_data[0]);
			if (dev->conv_mode == ONE_WIRE_MODE)
				xil_printf("ADC1: %d\n", adc_data[1]);
		}
	}
	/* Offload example */
	else {
		msg = (spi_eng_msg *)malloc(sizeof(*msg));
		if (!msg)
			return -1;

		msg->spi_msg_cmds = malloc(sizeof(spi_eng_msg_cmds));
		msg->spi_msg_cmds = spi_eng_msg_cmds;
		msg->msg_cmd_len = ARRAY_SIZE(spi_eng_msg_cmds);
		msg->rx_buf_addr = 0x800000;
		msg->tx_buf_addr = 0xA000000;
		msg->tx_buf[0] = 0x00;

		/* Init the rx buffer with 0s */
		for (i = 0; i < ARRAY_SIZE(msg->rx_buf); i++)
			msg->rx_buf[i] = 0;

		spi_eng_offload_load_msg(dev->spi_desc, msg);
		spi_eng_transfer_multiple_msgs(dev->spi_desc, 256);

		mdelay(10000);

		free(msg);

		offload_data = (uint16_t*)dev->spi_desc->rx_dma_startaddr;
		/* Data for ADC0 and ADC1 is interleaved */
		for(i = 0; i < dev->spi_desc->rx_length / 2; i++) {
			printf("ADC%d: %d\r\n", (i % 2), *offload_data);
			offload_data += sizeof(uint8_t);
		}
	}

	ad738x_remove(dev);

	printf("Bye\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
