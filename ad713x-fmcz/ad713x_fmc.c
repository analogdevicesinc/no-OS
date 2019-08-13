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
#include "xil_printf.h"
#include "spi_engine.h"
#include "platform_drivers.h"
#include "ad713x.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SPI_DEVICE_ID                   XPAR_PS7_SPI_0_DEVICE_ID
#define AD7134_DMA_BASEADDR             XPAR_AXI_AD7134_DMA_BASEADDR
#define AD7134_SPI_ENGINE_BASEADDR      XPAR_DUAL_AD7134_AXI_BASEADDR
#define AD7134_SPI_CS                   0
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

spi_eng_init_param spi_eng_init_params = {
	AD7134_SPI_ENGINE_BASEADDR,    // adc_baseaddr
	AD7134_SPI_CS,                 // chip_select
	0,                             // cs delay
	SPI_MODE_1,                    // spi_config
	2000000,                       // spi_clk_hz
	2000000,                       // spi_clk_hz_reg_access
	100000000,                     // ref_clk_hz
	1,                             // spi_offload_rx_support_en
	AD7134_DMA_BASEADDR,           // spi_offload_rx_dma_baseaddr
	1,                             // spi_offload_tx_support_en
	AD7134_DMA_BASEADDR,           // spi_offload_tx_dma_baseaddr
};

struct ad713x_init_param ad713x_default_init_param = {
	/* SPI */
	{
		PS7_SPI,
		SPI_DEVICE_ID,
		100000000,
		SPI_MODE_0,
		AD7134_SPI_CS,
	},
	/* GPIO */
	/* gpio_mode1 */
	{
		PS7_GPIO, GPIO_DEVICE_ID, GPIO_MODE_1
	},
	/* gpio_mode2 */
	{
		PS7_GPIO, GPIO_DEVICE_ID, GPIO_MODE_2
	},
	/* gpio_dclkmode */
	{
		PS7_GPIO, GPIO_DEVICE_ID, GPIO_DCLKMODE
	},
	/* gpio_dclkio1 */
	{
		PS7_GPIO, GPIO_DEVICE_ID, GPIO_DCLKIO_1
	},
	/* gpio_dclkio2 */
	{
		PS7_GPIO, GPIO_DEVICE_ID, GPIO_DCLKIO_2
	},
	/* gpio_resetn1 */
	{
		PS7_GPIO, GPIO_DEVICE_ID, GPIO_RESETN_1
	},
	/* gpio_resetn2 */
	{
		PS7_GPIO, GPIO_DEVICE_ID, GPIO_RESETN_2
	},
	/* gpio_pnd1 */
	{
		PS7_GPIO, GPIO_DEVICE_ID, GPIO_PDN_1
	},
	/* gpio_pnd2 */
	{
		PS7_GPIO, GPIO_DEVICE_ID, GPIO_PDN_2
	},
	/* Configuration */
	ID_AD7134, 		// dev_id
	ADC_24_BIT_DATA, 	// adc_data_len
	NO_CRC,			// crc_header
};

int main()
{
	struct ad713x_dev *dev;
	spi_eng_desc *desc;
	uint32_t *offload_data;
	spi_eng_msg *msg;
	uint32_t i;
	uint32_t spi_eng_msg_cmds[2];

	spi_eng_msg_cmds[0] = SLEEP(2000); // 2us
	spi_eng_msg_cmds[1] = TRANSFER_BYTES_R(3);

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ad713x_init(&dev, &ad713x_default_init_param);

	spi_eng_init(&desc, spi_eng_init_params);

	ad713x_dig_filter_sel_ch(dev, SINC3, CH0);

	/* For this example, only offload is supported*/
	msg = (spi_eng_msg *)malloc(sizeof(*msg));
	if (!msg)
		return -1;

	msg->spi_msg_cmds = malloc(sizeof(spi_eng_msg_cmds));
	msg->spi_msg_cmds = spi_eng_msg_cmds;
	msg->msg_cmd_len = ARRAY_SIZE(spi_eng_msg_cmds);
	msg->rx_buf_addr = 0x800000;
	msg->tx_buf_addr = 0xA000000;

	/* Init the rx buffer with 0s */
	for (i = 0; i < ARRAY_SIZE(msg->rx_buf); i++)
		msg->rx_buf[i] = 0;

	spi_eng_offload_load_msg(desc, msg);
	spi_eng_transfer_multiple_msgs(desc, 1024);

	mdelay(10000);

	free(msg);

	offload_data = (uint32_t*)desc->rx_dma_startaddr;
	for(i = 0; i < (desc->rx_length/4); i++) {
		printf("CH%lu: 0x%lx\r\n", i%8, *offload_data);
		offload_data += 1; // go to the next address in memory
	}

	spi_eng_remove(desc);
	ad713x_remove(dev);
	print("Bye\n\r");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

