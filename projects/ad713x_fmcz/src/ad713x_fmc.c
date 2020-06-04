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
#include <xil_cache.h>
#include <xparameters.h>
#include <math.h>
#include "xil_printf.h"
#include "spi_engine.h"
#include "ad713x.h"
#include "spi.h"
#include "spi_extra.h"
#include "delay.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "util.h"
#include "error.h"

#ifdef IIO_SUPPORT
#include "irq.h"
#include "irq_extra.h"
#include "uart.h"
#include "uart_extra.h"
#include "tinyiiod.h"
#include "iio_ad713x.h"
#include "iio.h"
#include "iio_app.h"
#endif // IIO_SUPPORT

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define AD7134_DMA_BASEADDR		XPAR_AXI_AD7134_DMA_BASEADDR
#define AD7134_SPI_ENGINE_BASEADDR	XPAR_DUAL_AD7134_AXI_BASEADDR
#define AD7134_1_SPI_CS			0
#define AD7134_2_SPI_CS			1
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET			54
#define GPIO_RESETN_1			GPIO_OFFSET + 32
#define GPIO_RESETN_2			GPIO_OFFSET + 33
#define GPIO_PDN_1			GPIO_OFFSET + 34
#define GPIO_PDN_2			GPIO_OFFSET + 35
#define GPIO_MODE_1			GPIO_OFFSET + 36
#define GPIO_MODE_2			GPIO_OFFSET + 37
#define GPIO_0				GPIO_OFFSET + 38
#define GPIO_1				GPIO_OFFSET + 39
#define GPIO_2				GPIO_OFFSET + 40
#define GPIO_3				GPIO_OFFSET + 41
#define GPIO_4				GPIO_OFFSET + 42
#define GPIO_5				GPIO_OFFSET + 43
#define GPIO_6				GPIO_OFFSET + 44
#define GPIO_7				GPIO_OFFSET + 45
#define GPIO_DCLKIO_1			GPIO_OFFSET + 46
#define GPIO_DCLKIO_2			GPIO_OFFSET + 47
#define GPIO_PINBSPI			GPIO_OFFSET + 48
#define GPIO_DCLKMODE			GPIO_OFFSET + 49
#define AD7134_FMC_CH_NO		8
#define AD7134_FMC_SAMPLE_NO		1024

#ifdef IIO_SUPPORT
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID

struct uart_desc *uart_device;
ssize_t iio_uart_write(const char *buf, size_t len)
{
	return uart_write(uart_device, (const uint8_t *)buf, len);
}

ssize_t iio_uart_read(char *buf, size_t len)
{
	return uart_read(uart_device, (uint8_t *)buf, len);
}
#endif // IIO_SUPPORT

int main()
{
	struct ad713x_dev *ad713x_dev_1;
	struct ad713x_dev *ad713x_dev_2;
	struct ad713x_init_param ad713x_init_param_1;
	struct ad713x_init_param ad713x_init_param_2;
	uint32_t *offload_data;
	uint32_t i;
	int32_t ret;
	struct spi_engine_offload_init_param spi_engine_offload_init_param;
	struct spi_engine_offload_message spi_engine_offload_message;
	uint32_t spi_eng_msg_cmds[2];
	static struct xil_spi_init_param spi_engine_init_params = {
		.type = SPI_PS,
	};
	struct xil_gpio_init_param gpio_extra_param;
	struct gpio_init_param ad7134_1_dclkio = {
		.number = GPIO_DCLKIO_1,
		.extra = &gpio_extra_param
	};
	struct gpio_init_param ad7134_2_dclkio = {
		.number = GPIO_DCLKIO_2,
		.extra = &gpio_extra_param
	};
	struct gpio_init_param ad7134_1_dclkmode = {
		.number = GPIO_DCLKMODE,
		.extra = &gpio_extra_param
	};
	struct gpio_init_param ad7134_2_dclkmode = {
		.number = GPIO_DCLKMODE,
		.extra = &gpio_extra_param
	};
	struct gpio_init_param ad7134_1_mode = {
		.number = GPIO_MODE_1,
		.extra = &gpio_extra_param
	};
	struct gpio_init_param ad7134_2_mode = {
		.number = GPIO_MODE_2,
		.extra = &gpio_extra_param
	};
	struct gpio_init_param ad7134_1_pnd = {
		.number = GPIO_PDN_1,
		.extra = &gpio_extra_param
	};
	struct gpio_init_param ad7134_2_pnd = {
		.number = GPIO_PDN_2,
		.extra = &gpio_extra_param
	};
	struct gpio_init_param ad7134_1_resetn = {
		.number = GPIO_RESETN_1,
		.extra = &gpio_extra_param
	};
	struct gpio_init_param ad7134_2_resetn = {
		.number = GPIO_RESETN_2,
		.extra = &gpio_extra_param
	};
	struct spi_desc *spi_eng_desc;
	struct spi_engine_init_param spi_eng_init_param  = {
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = AD7134_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 32,
	};
	const struct spi_init_param spi_eng_init_prm  = {
		.chip_select = AD7134_1_SPI_CS,
		.max_speed_hz = 10000000,
		.mode = SPI_MODE_3,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,

	};

	gpio_extra_param.device_id = GPIO_DEVICE_ID;
	gpio_extra_param.type = GPIO_PS;

	ad713x_init_param_1.adc_data_len = ADC_24_BIT_DATA;
	ad713x_init_param_1.clk_delay_en = false;
	ad713x_init_param_1.crc_header = CRC_8;
	ad713x_init_param_1.dev_id = ID_AD7134;
	ad713x_init_param_1.format = QUAD_CH_PO;
	ad713x_init_param_1.gpio_dclkio = &ad7134_1_dclkio;
	ad713x_init_param_1.gpio_dclkmode = &ad7134_1_dclkmode;
	ad713x_init_param_1.gpio_mode = &ad7134_1_mode;
	ad713x_init_param_1.gpio_pnd = &ad7134_1_pnd;
	ad713x_init_param_1.gpio_resetn = &ad7134_1_resetn;
	ad713x_init_param_1.mode_master_nslave = true;
	ad713x_init_param_1.dclkmode_free_ngated = false;
	ad713x_init_param_1.dclkio_out_nin = false;
	ad713x_init_param_1.pnd = true;
	ad713x_init_param_1.spi_init_prm.chip_select = AD7134_1_SPI_CS;
	ad713x_init_param_1.spi_init_prm.max_speed_hz = 10000000;
	ad713x_init_param_1.spi_init_prm.mode = SPI_MODE_3;
	ad713x_init_param_1.spi_init_prm.platform_ops = &xil_platform_ops;
	ad713x_init_param_1.spi_init_prm.extra = (void *)&spi_engine_init_params;
	ad713x_init_param_1.spi_common_dev = 0;

	ad713x_init_param_2.adc_data_len = ADC_24_BIT_DATA;
	ad713x_init_param_2.clk_delay_en = false;
	ad713x_init_param_2.crc_header = CRC_8;
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
	ad713x_init_param_2.spi_init_prm.chip_select = AD7134_2_SPI_CS;
	ad713x_init_param_2.spi_init_prm.max_speed_hz = 10000000;
	ad713x_init_param_2.spi_init_prm.mode = SPI_MODE_3;
	ad713x_init_param_2.spi_init_prm.platform_ops = &xil_platform_ops;
	ad713x_init_param_2.spi_init_prm.extra = (void *)&spi_engine_init_params;
	ad713x_init_param_2.spi_common_dev = 0;

	spi_eng_msg_cmds[0] = SLEEP(100);
	spi_eng_msg_cmds[1] = READ(1);

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = ad713x_init(&ad713x_dev_1, &ad713x_init_param_1);
	if (ret != SUCCESS)
		return FAILURE;
	mdelay(1000);
	ad713x_init_param_2.spi_common_dev = ad713x_dev_1->spi_desc;
	ret = ad713x_init(&ad713x_dev_2, &ad713x_init_param_2);
	if (ret != SUCCESS)
		return FAILURE;
	mdelay(1000);

	spi_engine_offload_init_param.rx_dma_baseaddr = AD7134_DMA_BASEADDR;
	spi_engine_offload_init_param.offload_config = OFFLOAD_RX_EN;

	ret = spi_init(&spi_eng_desc, &spi_eng_init_prm);
	if (ret != SUCCESS)
		return FAILURE;

	ret = spi_engine_offload_init(spi_eng_desc, &spi_engine_offload_init_param);
	if (ret != SUCCESS)
		return FAILURE;

	spi_engine_offload_message.commands = spi_eng_msg_cmds;
	spi_engine_offload_message.no_commands = ARRAY_SIZE(spi_eng_msg_cmds);
	spi_engine_offload_message.commands_data = NULL;
	spi_engine_offload_message.rx_addr = 0x800000;
	spi_engine_offload_message.tx_addr = 0xA000000;

#ifdef IIO_SUPPORT
	struct iio_ad713x *iio_ad713x;
	struct iio_server_ops uart_iio_server_ops;
	struct iio_app_init_param iio_app_init_par;

	struct xil_irq_init_param xil_irq_init_par = {
		.type = IRQ_PS,
	};

	struct irq_init_param irq_init_param = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.extra = &xil_irq_init_par,
	};

	struct irq_ctrl_desc *irq_desc;

	struct xil_uart_init_param xil_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID,
		.irq_desc = irq_desc,
	};

	struct iio_app_desc *iio_app_desc;

	struct uart_init_param uart_init_par = {
		.baud_rate = 115200,
		.device_id = UART_DEVICE_ID,
	};

	char dev_name[] = "adc";
	struct iio_ad713x_init_par iio_ad713x_init_par = {
		.name = dev_name,
		.num_channels = 8,
		.spi_eng_desc = spi_eng_desc,
		.spi_engine_offload_message = &spi_engine_offload_message,
		.dcache_invalidate_range = (void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange,
	};

	ret = iio_ad713x_init(&iio_ad713x, &iio_ad713x_init_par);
	if(ret < 0)
		return ret;

	uart_iio_server_ops = (struct iio_server_ops) {
		.read = iio_uart_read,
		.write = iio_uart_write,
	};

	iio_app_init_par = (struct iio_app_init_param) {
		.iio_server_ops = &uart_iio_server_ops,
	};

	ret = iio_app_init(&iio_app_desc, &iio_app_init_par);
	if (ret < 0)
		return ret;

	ret = irq_ctrl_init(&irq_desc, &irq_init_param);
	if(ret < 0)
		return ret;

	xil_uart_init_par.irq_desc = irq_desc;
	uart_init_par.extra = &xil_uart_init_par;
	ret = uart_init(&uart_device, &uart_init_par);
	if(ret < 0)
		return ret;

	ret = irq_global_enable(irq_desc);
	if (ret < 0)
		return ret;

	return iio_app(iio_app_desc);

#endif /* IIO_SUPPORT */

	ret = spi_engine_offload_transfer(spi_eng_desc, spi_engine_offload_message,
					  (AD7134_FMC_CH_NO * AD7134_FMC_SAMPLE_NO));
	if (ret != SUCCESS)
		return ret;

	mdelay(1000);

	Xil_DCacheInvalidateRange(0x800000, 16384 * 16);

	const float lsb = 4.096 / (pow(2, 23));
	float data;
	uint8_t j;

	offload_data = (uint32_t*)spi_engine_offload_message.rx_addr;

	for(i = 0; i < AD7134_FMC_SAMPLE_NO; i++) {
		j = 0;
		while(j < 8) {
			*(offload_data + j) <<= 1;
			*(offload_data + j) &= 0xffffff00;
			*(offload_data + j) >>= 8;

			data = lsb * ((int32_t)*(offload_data + j) & 0xFFFFFF);
			if(data > 4.095)
				data = data - 8.192;
			printf("CH%d: 0x%lx = %fV ", j, *(offload_data + j),
			       data);
			if(j == 7)
				printf("\n");
			j++;
		}
		offload_data += j; /* go to the next address in memory */
	}

	ad713x_remove(ad713x_dev_1);
	ad713x_remove(ad713x_dev_2);
	print("Bye\n\r");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
