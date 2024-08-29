/***************************************************************************//**
* @file cn0561.c
* @brief Implementation of Main Function.
* @authors Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2022(c) Analog Devices, Inc.
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
#include <string.h>
#include <xil_cache.h>
#include <xparameters.h>
#include <math.h>
#include "xil_printf.h"
#include "spi_engine.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "parameters.h"
#include "no_os_pwm.h"
#include "axi_pwm_extra.h"
#include "clk_axi_clkgen.h"
#include "axi_dmac.h"

#ifdef IIO_SUPPORT
#include "no_os_irq.h"
#include "xilinx_irq.h"
#include "no_os_uart.h"
#include "xilinx_uart.h"
#include "iio_ad713x.h"
#include "iio.h"
#include "iio_app.h"
#endif // IIO_SUPPORT

static uint32_t adc_buffer[ADC_BUFFER_SIZE] __attribute__((aligned));

int main()
{
	struct axi_clkgen *clkgen_cn0561;
	struct axi_clkgen_init clkgen_cn0561_init = {
		.base = XPAR_AXI_CN0561_CLKGEN_BASEADDR,
		.name = "cn0561_clkgen",
		.parent_rate = 100000000
	};
	uint32_t i = 0, j;
	uint32_t adc_channel;
	int32_t ret;
	const float lsb = 4.096 / (pow(2, 23));
	float data;
	uint32_t spi_eng_dma_flg = DMA_LAST | DMA_PARTIAL_REPORTING_EN;
	uint32_t max_speed_hz = CORA_Z7S_DATA_CLK_FREQ_HZ;
	struct spi_engine_offload_init_param spi_engine_offload_init_param;
	struct spi_engine_offload_message spi_engine_offload_message;
	uint32_t spi_eng_msg_cmds[6];
	static struct xil_spi_init_param spi_engine_init_params = {
		.type = SPI_PS,
	};
	struct xil_gpio_init_param gpio_extra_param;
	struct no_os_gpio_init_param cn0561_pnd = {
		.number = GPIO_PDN,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_spi_desc *spi_eng_desc;
	struct spi_engine_init_param spi_eng_init_param  = {
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = CN0561_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 32,
		.ref_clk_hz = CN0561_SPI_ENG_REF_CLK_FREQ_HZ
	};
	const struct no_os_spi_init_param spi_eng_init_prm  = {
		.chip_select = CN0561_SPI_CS,
		.max_speed_hz = max_speed_hz,
		.mode = NO_OS_SPI_MODE_1,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	};

	struct no_os_pwm_desc *axi_pwm;
	struct axi_pwm_init_param axi_zed_pwm_init_trigger = {
		.base_addr = XPAR_ODR_GENERATOR_BASEADDR,
		.ref_clock_Hz = 100000000,
		.channel = 0
	};
	struct axi_pwm_init_param axi_zed_pwm_init_odr = {
		.base_addr = XPAR_ODR_GENERATOR_BASEADDR,
		.ref_clock_Hz = 100000000,
		.channel = 1
	};
	struct no_os_pwm_init_param axi_pwm_init_trigger = {
		.period_ns = 1000,
		.duty_cycle_ns = 1,
		.phase_ns = 45,
		.platform_ops = &axi_pwm_ops,
		.extra = &axi_zed_pwm_init_trigger
	};
	struct no_os_pwm_init_param axi_pwm_init_odr = {
		.period_ns = 1000,
		.duty_cycle_ns = 130,
		.phase_ns = 0,
		.platform_ops = &axi_pwm_ops,
		.extra = &axi_zed_pwm_init_odr
	};

	gpio_extra_param.device_id = GPIO_DEVICE_ID;
	gpio_extra_param.type = GPIO_PS;

	spi_eng_msg_cmds[0] = SPI_ENGINE_CMD_CONFIG(
					    SPI_ENGINE_CMD_DATA_TRANSFER_LEN,
					    24);
	spi_eng_msg_cmds[1] = CS_LOW;
	spi_eng_msg_cmds[2] = READ(1);
	spi_eng_msg_cmds[3] = SPI_ENGINE_CMD_CONFIG(
					    SPI_ENGINE_CMD_DATA_TRANSFER_LEN,
					    8);
	spi_eng_msg_cmds[4] = READ(1);
	spi_eng_msg_cmds[5] = CS_HIGH;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = axi_clkgen_init(&clkgen_cn0561, &clkgen_cn0561_init);
	if (ret != 0)
		return -1;

	ret = axi_clkgen_set_rate(clkgen_cn0561, CN0561_SPI_ENG_REF_CLK_FREQ_HZ);
	if (ret != 0)
		return -1;

	ret = no_os_pwm_init(&axi_pwm, &axi_pwm_init_trigger);
	if (ret != 0)
		return ret;

	ret = no_os_pwm_init(&axi_pwm, &axi_pwm_init_odr);
	if (ret != 0)
		return ret;

	no_os_mdelay(1000);

	spi_engine_offload_init_param.rx_dma_baseaddr = CN0561_DMA_BASEADDR;
	spi_engine_offload_init_param.offload_config = OFFLOAD_RX_EN;
	spi_engine_offload_init_param.dma_flags = spi_eng_dma_flg;

	ret = no_os_spi_init(&spi_eng_desc, &spi_eng_init_prm);
	if (ret != 0)
		return -1;

	ret = spi_engine_offload_init(spi_eng_desc, &spi_engine_offload_init_param);
		if (ret != 0)
			return -1;

	spi_engine_offload_message.commands = spi_eng_msg_cmds;
	spi_engine_offload_message.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	spi_engine_offload_message.commands_data = NULL;
	spi_engine_offload_message.rx_addr = (uint32_t)adc_buffer;
	spi_engine_offload_message.tx_addr = 0xA000000;

	while (1) {
		
		ret = spi_engine_offload_transfer(spi_eng_desc, spi_engine_offload_message,
						(CN0561_FMC_CH_NO * CN0561_FMC_SAMPLE_NO)*2);
		if (ret != 0)
			return ret;

		Xil_DCacheInvalidateRange((INTPTR)adc_buffer,
					CN0561_FMC_SAMPLE_NO * CN0561_FMC_CH_NO *
					sizeof(uint32_t));

		for(i = 0; i < CN0561_FMC_SAMPLE_NO; i++) {
			j = 0;
			printf("%lu: ", i);
			while(j < CN0561_FMC_CH_NO) {
				adc_buffer[CN0561_FMC_CH_NO*i+j] &= 0xffffff00;
				adc_buffer[CN0561_FMC_CH_NO*i+j] >>= 8;
				data = lsb * (int32_t)adc_buffer[CN0561_FMC_CH_NO*i+j];
				if(data > 4.095)
					data = data - 8.192;
				printf("CH%lu: 0x%08lx = %+1.5fV ", j,
					adc_buffer[CN0561_FMC_CH_NO*i+j], data);
				if(j == (CN0561_FMC_CH_NO - 1))
					printf("\n");
				j++;
			}
		}
	}
	
	print("Bye\n\r");

	return 0;
}
