/***************************************************************************//**
 * @file adaq7980_sdz.c
 * @brief Implementation of Main Function.
 * @author ADI
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xil_cache.h>
#include <xparameters.h>
#include "platform_drivers.h"
#include "adaq7980.h"
#include "spi_engine.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADAQ7980_DMA_BASEADDR			XPAR_AXI_ADAQ7980_DMA_BASEADDR
#define ADAQ7980_SPI_ENGINE_BASEADDR	XPAR_SPI_AXI_BASEADDR
#define SPI_ADAQ7980_CS					0
#define GPIO_DEVICE_ID					XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET						32 + 54
#define GPIO_0							GPIO_OFFSET + 0
#define GPIO_1							GPIO_OFFSET + 1
#define GPIO_2							GPIO_OFFSET + 2
#define GPIO_3							GPIO_OFFSET + 3
#define GPIO_4							GPIO_OFFSET + 4
#define GPIO_5							GPIO_OFFSET + 5
#define GPIO_6							GPIO_OFFSET + 6
#define GPIO_7							GPIO_OFFSET + 7
#define GPIO_REF_PUB					GPIO_OFFSET + 8
#define GPIO_RBUF_PUB					GPIO_OFFSET + 9
#define ADC_DDR_BASEADDR				XPAR_DDR_MEM_BASEADDR + 0x800000

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/

adaq7980_init_param default_init_param = {
	/* SPI */
	SPI_ADAQ7980_CS,	// spi_chip_select
	SPI_MODE_2,			// spi_mode
	SPI_ENGINE,			// spi_type
	-1,					// spi_device_id
	/* GPIO */
	PS7_GPIO,			// gpio_type
	GPIO_DEVICE_ID,		// gpio_device_id
	GPIO_0,				// gpio_ld_ldo
	GPIO_REF_PUB,		// gpio_ref_pd
	GPIO_RBUF_PUB,		// gpio_rbuf_pd
};

uint32_t spi_msg_cmds[6] = {CS_DEASSERT, CS_ASSERT, SLEEP(5000), CS_DEASSERT, TRANSFER_R(4), CS_ASSERT};

spi_init_param spi_default_init_param = {
		ADAQ7980_SPI_ENGINE_BASEADDR,	// adc_baseaddr
		0,								// chip_select
		SPI_ENGINE_CONFIG_CPHA,			// spi_config
		1000000,						// spi_clk_hz
		100000000,						// ref_clk_hz
		1,								// spi_offload_rx_support_en
		ADAQ7980_DMA_BASEADDR,			// spi_offload_rx_dma_baseaddr
		0,								// spi_offload_tx_support_en
		ADAQ7980_DMA_BASEADDR,			// spi_offload_tx_dma_baseaddr
};

#define SPI_ENGINE_OFFLOAD_EXAMPLE	0

/***************************************************************************//**
 * @brief main
 *******************************************************************************/
int main(void)
{
	adaq7980_dev	*dev;
	spi_dev			*spi_dev;
	spi_msg 		*msg;
	uint8_t			*data;
	uint32_t		i;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	adaq7980_setup(&dev, default_init_param);
	spi_eng_setup(&spi_dev, spi_default_init_param);

	msg = (spi_msg *)malloc(sizeof(*msg));
	if (!msg)
		return -1;

	msg->rx_buf_addr = 0x10000000;
	msg->spi_msg_cmds = spi_msg_cmds;
	msg->msg_cmd_len = sizeof(spi_msg_cmds) / sizeof(uint32_t);

	if (SPI_ENGINE_OFFLOAD_EXAMPLE == 0) {

		while(1){
			spi_eng_transfer_message(spi_dev, msg);
			mdelay(1000);
		}

	} else {
		spi_eng_offload_load_msg(spi_dev, msg);
		spi_eng_transfer_multiple_msgs(spi_dev, 8);

        data = (uint8_t*)spi_dev->rx_dma_startaddr;

        for(i = 0; i < spi_dev->rx_length; i++) {
    		printf("%x\r\n", *data);
    		data += sizeof(uint8_t);
        }
	}

	printf("Bye\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
