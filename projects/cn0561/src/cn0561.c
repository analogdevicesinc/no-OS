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
#include "ad713x.h"
#include "no_os_spi.h"
#include "spi_extra.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "gpio_extra.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "parameters.h"
#include "no_os_pwm.h"
#include "axi_pwm_extra.h"
#include "clk_axi_clkgen.h"
#include "axi_dmac.h"

#ifdef IIO_SUPPORT
#include "no_os_irq.h"
#include "irq_extra.h"
#include "no_os_uart.h"
#include "uart_extra.h"
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
	struct ad713x_dev *cn0561_dev;
	struct ad713x_init_param cn0561_init_param;
	uint32_t i = 0, j;
	int32_t ret;
	const float lsb = 4.096 / (pow(2, 23));
	float data;
	uint32_t spi_eng_dma_flg = DMA_LAST | DMA_PARTIAL_REPORTING_EN;
	struct spi_engine_offload_init_param spi_engine_offload_init_param;
	struct spi_engine_offload_message spi_engine_offload_message;
	uint32_t spi_eng_msg_cmds[1];
	static struct xil_spi_init_param spi_engine_init_params = {
		.type = SPI_PS,
	};
	struct xil_gpio_init_param gpio_extra_param;
	struct no_os_gpio_init_param cn0561_pnd = {
		.number = GPIO_PDN,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
#ifdef CN0561_ZED_CARRIER
	struct no_os_gpio_init_param cn0561_mode = {
		.number = GPIO_MODE,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param cn0561_resetn = {
		.number = GPIO_RESETN,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
#endif
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
		.max_speed_hz = 24000000,
		.mode = NO_OS_SPI_MODE_1,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	};

	struct no_os_pwm_desc *axi_pwm;
	struct axi_pwm_init_param axi_zed_pwm_init = {
		.base_addr = XPAR_ODR_GENERATOR_BASEADDR,
		.ref_clock_Hz = 100000000,
		.channel = 0
	};

	struct no_os_pwm_init_param axi_pwm_init = {
		.period_ns = 3400,
		.duty_cycle_ns = 600,
		.phase_ns = 0,
		.extra = &axi_zed_pwm_init
	};

	gpio_extra_param.device_id = GPIO_DEVICE_ID;
	gpio_extra_param.type = GPIO_PS;

	cn0561_init_param.adc_data_len = ADC_24_BIT_DATA;
	cn0561_init_param.clk_delay_en = false;
	cn0561_init_param.crc_header = CRC_6;
	cn0561_init_param.dev_id = ID_AD4134;
	cn0561_init_param.format = QUAD_CH_PO;
	cn0561_init_param.gpio_dclkio = NULL;
	cn0561_init_param.gpio_dclkmode = NULL;
	cn0561_init_param.gpio_pnd = &cn0561_pnd;
#ifdef CN0561_ZED_CARRIER
	cn0561_init_param.gpio_mode = &cn0561_mode;
	cn0561_init_param.gpio_resetn = &cn0561_resetn;
#else
	cn0561_init_param.gpio_mode = NULL;
	cn0561_init_param.gpio_resetn = NULL;
#endif
	cn0561_init_param.mode_master_nslave = false;
	cn0561_init_param.dclkmode_free_ngated = false;
	cn0561_init_param.dclkio_out_nin = false;
	cn0561_init_param.pnd = true;
	cn0561_init_param.spi_init_prm.chip_select = CN0561_SPI_CS;
	cn0561_init_param.spi_init_prm.device_id = SPI_DEVICE_ID;
	cn0561_init_param.spi_init_prm.max_speed_hz = 1000000;
	cn0561_init_param.spi_init_prm.mode = NO_OS_SPI_MODE_0;
	cn0561_init_param.spi_init_prm.platform_ops = &xil_spi_ops;
	cn0561_init_param.spi_init_prm.extra = (void *)&spi_engine_init_params;
	cn0561_init_param.spi_common_dev = 0;

	spi_eng_msg_cmds[0] = READ(4);

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = axi_clkgen_init(&clkgen_cn0561, &clkgen_cn0561_init);
	if (ret != 0)
		return -1;

	ret = axi_clkgen_set_rate(clkgen_cn0561, CN0561_SPI_ENG_REF_CLK_FREQ_HZ);
	if (ret != 0)
		return -1;

	ret = no_os_pwm_init(&axi_pwm, &axi_pwm_init);
	if (ret != 0)
		return ret;

	ret = ad713x_init(&cn0561_dev, &cn0561_init_param);
	if (ret != 0)
		return -1;

	no_os_mdelay(1000);

	ret = ad713x_spi_reg_write(cn0561_dev, AD713X_REG_GPIO_DIR_CTRL, 0xE7);
	if (ret != 0)
		return -1;
	ret = ad713x_spi_reg_write(cn0561_dev, AD713X_REG_GPIO_DATA, 0x84);
	if (ret != 0)
		return -1;

	spi_engine_offload_init_param.rx_dma_baseaddr = CN0561_DMA_BASEADDR;
	spi_engine_offload_init_param.offload_config = OFFLOAD_RX_EN;
	spi_engine_offload_init_param.dma_flags = &spi_eng_dma_flg;

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

#ifdef IIO_SUPPORT
	struct iio_data_buffer rd_buff = {
		.buff = (void *)adc_buffer,
		.size = ADC_BUFFER_SIZE
	};
	struct ad713x_iio *iio_desc;
	struct ad713x_iio_init_param iio_desc_param = {
		.drv_dev = cn0561_dev,
		.vref_int = 4,
		.vref_micro = 96000,
		.spi_eng_desc = spi_eng_desc,
		.dcache_invalidate_range =
		(void (*)(uint32_t,  uint32_t))Xil_DCacheInvalidateRange,
		.iio_dev = &ad713x_iio_desc
	};

	ret = iio_ad713x_init(&iio_desc, &iio_desc_param);
	if (ret < 0)
		return ret;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad4134", iio_desc, &ad713x_iio_desc,
			       &rd_buff, NULL),
	};

	return iio_app_run(devices, NO_OS_ARRAY_SIZE(devices));

#endif /* IIO_SUPPORT */

	ret = spi_engine_offload_transfer(spi_eng_desc, spi_engine_offload_message,
					  (CN0561_FMC_CH_NO * CN0561_FMC_SAMPLE_NO));
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

	ad713x_remove(cn0561_dev);
	print("Bye\n\r");

	return 0;
}
