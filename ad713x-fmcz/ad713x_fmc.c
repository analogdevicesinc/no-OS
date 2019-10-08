/***************************************************************************//**
* @file ad713x_fmc.c
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
#include <math.h>
#include "xil_printf.h"
#include "spi_engine_offload.h"
#include "ad713x.h"
#include "xilinx_platform_drivers.h"
#include "spi.h"
#include "delay.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SPI_DEVICE_ID                   XPAR_PS7_SPI_0_DEVICE_ID
#define AD7134_DMA_BASEADDR             XPAR_AXI_AD7134_DMA_BASEADDR
#define AD7134_SPI_ENGINE_BASEADDR      XPAR_DUAL_AD7134_AXI_BASEADDR
#define AD7134_1_SPI_CS                   0
#define AD7134_2_SPI_CS                   1
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
#define ARRAY_SIZE(Array)		(sizeof(Array) / sizeof((Array)[0]))
#define AD7134_FMC_CH_NO		8
#define AD7134_FMC_SAMPLE_NO	1024

int main()
{
	struct ad713x_dev *ad713x_dev_1;
	struct ad713x_dev *ad713x_dev_2;
	struct ad713x_init_param ad713x_init_param_1;
	struct ad713x_init_param ad713x_init_param_2;
	struct xil_spi_init_param spi_ps_extra;
	struct spi_desc *spi_eng_desc;
	uint32_t *offload_data;
	struct spi_eng_msg *msg;
	uint32_t i;
	uint32_t spi_eng_msg_cmds[2];
	struct spi_init_param spi_eng_inits;
	struct spi_init_param_extra extra_param;

	spi_eng_inits.chip_select = AD7134_1_SPI_CS;
	spi_eng_inits.max_speed_hz = 100000000UL;
	spi_eng_inits.mode = SPI_MODE_1;
	extra_param.cs_delay = 0;
	extra_param.spi_baseaddr = AD7134_SPI_ENGINE_BASEADDR;
	extra_param.spi_clk_hz = 50000000;
	extra_param.spi_clk_hz_reg_access = 50000000;
	extra_param.spi_offload_rx_dma_baseaddr = AD7134_DMA_BASEADDR;
	extra_param.spi_offload_rx_support_en = 1;
	extra_param.spi_offload_tx_dma_baseaddr = AD7134_DMA_BASEADDR;
	extra_param.spi_offload_tx_support_en = 1;
	spi_eng_inits.extra = &extra_param;

	spi_ps_extra.flags = 0;
	spi_ps_extra.id = SPI_DEVICE_ID;
	spi_ps_extra.type = XILINX_SPI;

	ad713x_init_param_1.adc_data_len = ADC_24_BIT_DATA;
	ad713x_init_param_1.clk_delay_en = false;
	ad713x_init_param_1.crc_header = NO_CRC;
	ad713x_init_param_1.dev_id = ID_AD7134;
	ad713x_init_param_1.format = QUAD_CH_PO;
	ad713x_init_param_1.gpio_dclkio = GPIO_DCLKIO_1;
	ad713x_init_param_1.gpio_dclkmode = GPIO_DCLKMODE;
	ad713x_init_param_1.gpio_mode = GPIO_MODE_1;
	ad713x_init_param_1.gpio_pnd = GPIO_PDN_1;
	ad713x_init_param_1.gpio_resetn = GPIO_RESETN_1;
	ad713x_init_param_1.mode_master_nslave = false;
	ad713x_init_param_1.dclkmode_free_ngated = false;
	ad713x_init_param_1.dclkio_out_nin = false;
	ad713x_init_param_1.pnd = true;
	ad713x_init_param_1.spi_init_prm.chip_select = AD7134_1_SPI_CS;
	ad713x_init_param_1.spi_init_prm.max_speed_hz = 100000000;
	ad713x_init_param_1.spi_init_prm.mode = SPI_MODE_0;
	ad713x_init_param_1.spi_init_prm.extra = (void *)&spi_ps_extra;

	ad713x_init_param_2.adc_data_len = ADC_24_BIT_DATA;
	ad713x_init_param_2.clk_delay_en = false;
	ad713x_init_param_2.crc_header = NO_CRC;
	ad713x_init_param_2.dev_id = ID_AD7134;
	ad713x_init_param_2.format = QUAD_CH_PO;
	ad713x_init_param_2.gpio_dclkio = GPIO_DCLKIO_2;
	ad713x_init_param_2.gpio_dclkmode = GPIO_DCLKMODE;
	ad713x_init_param_2.gpio_mode = GPIO_MODE_2;
	ad713x_init_param_2.gpio_pnd = GPIO_PDN_2;
	ad713x_init_param_2.gpio_resetn = GPIO_RESETN_2;
	ad713x_init_param_2.mode_master_nslave = true;
	ad713x_init_param_2.dclkmode_free_ngated = false;
	ad713x_init_param_2.dclkio_out_nin = false;
	ad713x_init_param_2.pnd = true;
	ad713x_init_param_2.spi_init_prm.chip_select = AD7134_2_SPI_CS;
	ad713x_init_param_2.spi_init_prm.max_speed_hz = 100000000;
	ad713x_init_param_2.spi_init_prm.mode = SPI_MODE_0;
	ad713x_init_param_2.spi_init_prm.extra = (void *)&spi_ps_extra;

	spi_eng_msg_cmds[0] = SLEEP(300); // 2us
	spi_eng_msg_cmds[1] = TRANSFER_BYTES_R(1);

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ad713x_init(&ad713x_dev_1, &ad713x_init_param_1);

	ad713x_init(&ad713x_dev_2, &ad713x_init_param_2);

	spi_eng_offload_init(&spi_eng_desc, &spi_eng_inits);

	/* For this example, only offload is supported */
	msg = calloc(1, sizeof *msg);
	if (!msg)
		return -1;

	spi_eng_set_transfer_length(spi_eng_desc, 24);

	msg->spi_msg_cmds = malloc(sizeof(spi_eng_msg_cmds));
	msg->spi_msg_cmds = spi_eng_msg_cmds;
	msg->msg_cmd_len = ARRAY_SIZE(spi_eng_msg_cmds);
	msg->rx_buf_addr = 0x800000;
	msg->tx_buf_addr = 0xA000000;
	msg->rx_buf = calloc((AD7134_FMC_CH_NO * AD7134_FMC_SAMPLE_NO),
			sizeof(uint32_t));

	spi_eng_offload_load_msg(spi_eng_desc, msg);
	spi_eng_transfer_multiple_msgs(spi_eng_desc,
			(AD7134_FMC_CH_NO * AD7134_FMC_SAMPLE_NO));

	mdelay(5000);

//	Xil_DCacheInvalidateRange(0x800000, 16384 * 16);

	free(msg);

	const float lsb = 4.096 / (pow(2, 23));
	float data;
	uint8_t j;

	offload_data = (uint32_t*)((struct spi_desc_extra *)spi_eng_desc->extra)->
			rx_dma_startaddr;
	for(i = 0;
			i < AD7134_FMC_SAMPLE_NO;
			i++) {
		j = 0;
		while(j < 8) {
			printf("CH%lu: 0x%lx\r\n", j, *(offload_data + j));
//			if(j == 4) {
//				printf("%d CH%lu: 0x%lx ", i, j, *(offload_data + j));
//				data = lsb * (*(offload_data + j) & 0xFFFFFF);
//				if(data > 4.09599)
//					data = data - 8.192;
//				printf("%fV\n", data);
//			}
			j++;
		}
		offload_data += j; /* go to the next address in memory */
	}

	free(msg->spi_msg_cmds);
	free(msg->rx_buf);

	ad713x_remove(ad713x_dev_1);
	ad713x_remove(ad713x_dev_2);
	print("Bye\n\r");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

