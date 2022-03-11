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
#include "clk_axi_clkgen.h"
#include "xil_printf.h"
#include "ad77681.h"
#include "spi_engine.h"
#include "parameters.h"
#include "no_os_delay.h"
#include "no_os_error.h"

uint32_t spi_msg_cmds[6] = {CS_LOW, CS_HIGH, CS_LOW, WRITE_READ(1), CS_HIGH};

struct spi_engine_init_param spi_eng_init_param  = {
	.type = SPI_ENGINE,
	.spi_engine_baseaddr = AD77681_SPI1_ENGINE_BASEADDR,
	.cs_delay = 0,
	.data_width = 32,
	.ref_clk_hz = AD77681_SPI_ENG_REF_CLK_FREQ_HZ,
};

struct ad77681_init_param ADC_default_init_param = {
	/* SPI */
	{
		.chip_select = AD77681_SPI_CS,
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_3,
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
	0, 							// status_bit
	AD77681_VCM_HALF_VCC,		/* VCM setup*/
	AD77681_AINn_ENABLED,		/* AIN- precharge buffer*/
	AD77681_AINp_ENABLED,		/* AIN+ precharge buffer*/
	AD77681_BUFn_ENABLED,		/* REF- buffer*/
	AD77681_BUFp_ENABLED,		/* REF+ buffer*/
	AD77681_FIR,			/* FIR Filter*/
	AD77681_SINC5_FIR_DECx32,	/* Decimate by 32*/
	0,				/* OS ratio of SINC3*/
	4096,				/* Reference voltage*/
	16384,				/* MCLK in kHz*/
};

#define SPI_ENGINE_OFFLOAD_EXAMPLE	0

struct axi_clkgen_init clkgen_init = {
	.name = "rx_clkgen",
	.base = XPAR_SPI_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

int main()
{
	struct ad77681_dev	*adc_dev;
	struct ad77681_status_registers *adc_status;
	struct axi_clkgen *clkgen;
	uint8_t			adc_data[5];
	uint8_t 		*data;
	uint32_t 		i;
	int32_t ret;
	uint8_t commands_data[4] = {0, 0, 0, AD77681_REG_READ(AD77681_REG_ADC_DATA)};
	struct spi_engine_offload_init_param spi_engine_offload_init_param = {
		.offload_config = (OFFLOAD_RX_EN | OFFLOAD_TX_EN),
		.rx_dma_baseaddr = AD77681_DMA_1_BASEADDR,
		.tx_dma_baseaddr = AD77681_DMA_1_BASEADDR
	};
	struct spi_engine_offload_message spi_engine_offload_message;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = axi_clkgen_init(&clkgen, &clkgen_init);
	if (ret != 0) {
		printf("error: axi_clkgen_init() failed\n");
		return -1;
	}

	ret = axi_clkgen_set_rate(clkgen, 80000000);
	if (ret != 0) {
		return -1;
	}

	ad77681_setup(&adc_dev, ADC_default_init_param, &adc_status);

	if (SPI_ENGINE_OFFLOAD_EXAMPLE == 0) {
		while(1) {
			ad77681_spi_read_adc_data(adc_dev, adc_data,
						  AD77681_CONTINUOUS_DATA_READ);
			printf("[ADC DATA]: 0x");
			for(i = 0; i < sizeof(adc_data); i++) {
				printf("%x", adc_data[i]);
			}
			printf("\r\n");
			no_os_mdelay(1000);
		}
	} else {
		ret = spi_engine_offload_init(adc_dev->spi_desc,
					      &spi_engine_offload_init_param);
		if (ret != 0)
			return -1;

		spi_engine_offload_message.commands = spi_msg_cmds;
		spi_engine_offload_message.no_commands = NO_OS_ARRAY_SIZE(spi_msg_cmds);
		spi_engine_offload_message.commands_data = commands_data;
		spi_engine_offload_message.rx_addr = 0x800000;
		spi_engine_offload_message.tx_addr = 0xA000000;

		ret = spi_engine_offload_transfer(adc_dev->spi_desc, spi_engine_offload_message,
						  AD77681_EVB_SAMPLE_NO);
		if (ret != 0)
			return ret;

		no_os_mdelay(1000);

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
