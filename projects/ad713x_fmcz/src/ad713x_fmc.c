/***************************************************************************//**
* @file ad713x_fmc.c
* @brief Implementation of Main Function.
* @authors SPopa (stefan.popa@analog.com)
* @authors Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
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
#include "iio_dual_ad713x.h"
#include "iio_ad713x.h"
#include "iio.h"
#include "iio_app.h"
#endif // IIO_SUPPORT

static uint32_t adc_buffer[ADC_BUFFER_SIZE] __attribute__((aligned));

int main()
{
	struct axi_clkgen *clkgen_7134;
	struct axi_clkgen_init clkgen_7134_init = {
		.base = XPAR_AXI_AD7134_CLKGEN_BASEADDR,
		.name = "ad7134_clkgen",
		.parent_rate = 100000000
	};
	struct ad713x_dev *ad713x_dev_1;
	struct ad713x_dev *ad713x_dev_2;
	struct ad713x_init_param ad713x_init_param_1;
	struct ad713x_init_param ad713x_init_param_2;
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
	struct no_os_gpio_init_param ad7134_1_dclkio = {
		.number = GPIO_DCLKIO_1,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad7134_2_dclkio = {
		.number = GPIO_DCLKIO_2,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad7134_1_dclkmode = {
		.number = GPIO_DCLKMODE,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad7134_2_dclkmode = {
		.number = GPIO_DCLKMODE,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad7134_1_mode = {
		.number = GPIO_MODE_1,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad7134_2_mode = {
		.number = GPIO_MODE_2,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad7134_1_pnd = {
		.number = GPIO_PDN_1,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad7134_2_pnd = {
		.number = GPIO_PDN_2,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad7134_1_resetn = {
		.number = GPIO_RESETN_1,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad7134_2_resetn = {
		.number = GPIO_RESETN_2,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_spi_desc *spi_eng_desc;
	struct spi_engine_init_param spi_eng_init_param  = {
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = AD7134_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 32,
		.ref_clk_hz = AD713x_SPI_ENG_REF_CLK_FREQ_HZ
	};
	const struct no_os_spi_init_param spi_eng_init_prm  = {
		.chip_select = AD7134_1_SPI_CS,
		.max_speed_hz = 48000000,
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
		.period_ns = 3333,
		.duty_cycle_ns = 600,
		.phase_ns = 0,
		.platform_ops = &axi_pwm_ops,
		.extra = &axi_zed_pwm_init
	};

	gpio_extra_param.device_id = GPIO_DEVICE_ID;
	gpio_extra_param.type = GPIO_PS;

	ad713x_init_param_1.adc_data_len = ADC_24_BIT_DATA;
	ad713x_init_param_1.clk_delay_en = false;
	ad713x_init_param_1.crc_header = CRC_6;
	ad713x_init_param_1.dev_id = ID_AD7134;
	ad713x_init_param_1.format = QUAD_CH_PO;
	ad713x_init_param_1.gpio_dclkio = &ad7134_1_dclkio;
	ad713x_init_param_1.gpio_dclkmode = &ad7134_1_dclkmode;
	ad713x_init_param_1.gpio_mode = &ad7134_1_mode;
	ad713x_init_param_1.gpio_pnd = &ad7134_1_pnd;
	ad713x_init_param_1.gpio_resetn = &ad7134_1_resetn;
	ad713x_init_param_1.mode_master_nslave = false;
	ad713x_init_param_1.dclkmode_free_ngated = false;
	ad713x_init_param_1.dclkio_out_nin = false;
	ad713x_init_param_1.pnd = true;
	ad713x_init_param_1.spi_init_prm.chip_select = AD7134_1_SPI_CS;
	ad713x_init_param_1.spi_init_prm.device_id = SPI_DEVICE_ID;
	ad713x_init_param_1.spi_init_prm.max_speed_hz = 10000000;
	ad713x_init_param_1.spi_init_prm.mode = NO_OS_SPI_MODE_3;
	ad713x_init_param_1.spi_init_prm.platform_ops = &xil_spi_ops;
	ad713x_init_param_1.spi_init_prm.extra = (void *)&spi_engine_init_params;
	ad713x_init_param_1.spi_common_dev = 0;

	ad713x_init_param_2.adc_data_len = ADC_24_BIT_DATA;
	ad713x_init_param_2.clk_delay_en = false;
	ad713x_init_param_2.crc_header = CRC_6;
	ad713x_init_param_2.dev_id = ID_AD7134;
	ad713x_init_param_2.format = QUAD_CH_PO;
	ad713x_init_param_2.gpio_dclkio = &ad7134_2_dclkio;
	ad713x_init_param_2.gpio_dclkmode = &ad7134_2_dclkmode;
	ad713x_init_param_2.gpio_mode = &ad7134_2_mode;
	ad713x_init_param_2.gpio_pnd = &ad7134_2_pnd;
	ad713x_init_param_2.gpio_resetn = &ad7134_2_resetn;
	ad713x_init_param_2.mode_master_nslave = false;
	ad713x_init_param_2.dclkmode_free_ngated = false;
	ad713x_init_param_2.dclkio_out_nin = false;
	ad713x_init_param_2.pnd = true;
	ad713x_init_param_2.spi_init_prm.device_id = SPI_DEVICE_ID;
	ad713x_init_param_2.spi_init_prm.chip_select = AD7134_2_SPI_CS;
	ad713x_init_param_2.spi_init_prm.max_speed_hz = 10000000;
	ad713x_init_param_2.spi_init_prm.mode = NO_OS_SPI_MODE_3;
	ad713x_init_param_2.spi_init_prm.platform_ops = &xil_spi_ops;
	ad713x_init_param_2.spi_init_prm.extra = (void *)&spi_engine_init_params;
	ad713x_init_param_2.spi_common_dev = 0;

	spi_eng_msg_cmds[0] = READ(4);

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = axi_clkgen_init(&clkgen_7134, &clkgen_7134_init);
	if (ret != 0)
		return -1;

	ret = axi_clkgen_set_rate(clkgen_7134, AD713x_SPI_ENG_REF_CLK_FREQ_HZ);
	if (ret != 0)
		return -1;

	ret = no_os_pwm_init(&axi_pwm, &axi_pwm_init);
	if (ret != 0)
		return ret;

	ret = ad713x_init(&ad713x_dev_1, &ad713x_init_param_1);
	if (ret != 0)
		return -1;

	no_os_mdelay(1000);
	ad713x_init_param_2.spi_common_dev = ad713x_dev_1->spi_desc;
	ret = ad713x_init(&ad713x_dev_2, &ad713x_init_param_2);
	if (ret != 0)
		return -1;
	no_os_mdelay(1000);

	spi_engine_offload_init_param.rx_dma_baseaddr = AD7134_DMA_BASEADDR;
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

#ifdef IIO_SUPPORT
	struct iio_ad713x *iio_ad713x;
	/**
	 * iio devices corresponding to every device.
	 */
	struct iio_device *ad713x_dev_desc;

	struct iio_ad713x_init_par iio_ad713x_init_par = {
		.dev = ad713x_dev_2,
		.num_channels = 8,
		.spi_eng_desc = spi_eng_desc,
		.spi_engine_offload_message = &spi_engine_offload_message,
		.dcache_invalidate_range = (void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange,
	};

	struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
#endif
	};

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = iio_dual_ad713x_init(&iio_ad713x, &iio_ad713x_init_par);
	if(ret < 0)
		return ret;

	iio_dual_ad713x_get_dev_descriptor(iio_ad713x, &ad713x_dev_desc);

	struct iio_data_buffer rd_buff = {
		.buff = (void *)adc_buffer,
		.size = ADC_BUFFER_SIZE
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("dual_ad7134", iio_ad713x, ad713x_dev_desc,
			       &rd_buff, NULL, NULL),
		IIO_APP_DEVICE("ad7134_1", ad713x_dev_1, &ad713x_iio_desc,
			       NULL, NULL, NULL),
		IIO_APP_DEVICE("ad7134_2", ad713x_dev_2, &ad713x_iio_desc,
			       NULL, NULL, NULL)

	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	iio_app_run(app);

#endif /* IIO_SUPPORT */

	ret = spi_engine_offload_transfer(spi_eng_desc, spi_engine_offload_message,
					  (AD7134_FMC_CH_NO * AD7134_FMC_SAMPLE_NO));
	if (ret != 0)
		return ret;

	Xil_DCacheInvalidateRange((INTPTR)adc_buffer,
				  AD7134_FMC_SAMPLE_NO * AD7134_FMC_CH_NO *
				  sizeof(uint32_t));

	for(i = 0; i < AD7134_FMC_SAMPLE_NO; i++) {
		j = 0;
		printf("%lu: ", i);
		while(j < 8) {
			adc_buffer[AD7134_FMC_CH_NO*i+j] &= 0xffffff00;
			adc_buffer[AD7134_FMC_CH_NO*i+j] >>= 8;
			data = lsb * (int32_t)adc_buffer[AD7134_FMC_CH_NO*i+j];
			if(data > 4.095)
				data = data - 8.192;
			printf("CH%lu: 0x%08lx = %+1.5fV ", j,
			       adc_buffer[AD7134_FMC_CH_NO*i+j], data);
			if(j == 7)
				printf("\n");
			j++;
		}
	}

	ad713x_remove(ad713x_dev_1);
	ad713x_remove(ad713x_dev_2);
	print("Bye\n\r");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
