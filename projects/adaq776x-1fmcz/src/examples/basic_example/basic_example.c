/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of basic example for adaq776x-1fmcz project.
 *   @author Celine Joy Capua (celinejoy.capua@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "basic_example.h"
#include <stdio.h>
#include <sleep.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "clk_axi_clkgen.h"
#include "xil_printf.h"
#include "xilinx_irq.h"
#include "ad77681.h"
#include "spi_engine.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_error.h"

#define SPI_ENGINE_OFFLOAD_EXAMPLE	0

int basic_example_main()
{
	struct ad77681_dev	*adc_dev;
	struct ad77681_status_registers *adc_status;
	struct axi_clkgen *clkgen;
	uint8_t			adc_data[5];
	uint8_t 		*data;
	uint32_t 		i;
	uint32_t spi_msg_cmds[6] = {CS_LOW, CS_HIGH, CS_LOW, WRITE_READ(1), CS_HIGH};
	int32_t ret;
	uint32_t commands_data[4] = {0, 0, 0, AD77681_REG_READ(AD77681_REG_ADC_DATA)};
	struct spi_engine_offload_message spi_engine_offload_message;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = axi_clkgen_init(&clkgen, &clkgen_init);
	if (ret != 0) {
		printf("error: axi_clkgen_init() failed\n");
		return -1;
	} else {
		printf("success: axi_clkgen_init() passed\n");
	}

	ret = axi_clkgen_set_rate(clkgen, 80000000);
	if (ret != 0) {
		return -1;
	} else {
		printf("success: axi_clkgen_set_rate() passed\n");
	}

	ad77681_setup(&adc_dev, adaq776x1_init_param, &adc_status);
	if (ret != 0) {
		printf("error: ad77681_setup() failed\n");
		return -1;
	} else {
		printf("success: ad77681_setup() passed\n");
	}

	if (SPI_ENGINE_OFFLOAD_EXAMPLE == 0) {
		printf("Reading ADC data using SPI engine...\n");
		while (1) {
			printf("im in the loop\n");
			ad77681_spi_read_adc_data(adc_dev, adc_data,
						  AD77681_REGISTER_DATA_READ);
			printf("[ADC DATA]: 0x");
			for (i = 0; i < sizeof(adc_data); i++) {
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
