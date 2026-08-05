/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by ad405x project.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param ad405x_uart_extra_ip = {
	.huart = UART_INSTANCE,
};

#ifdef AD405X_I3C

struct no_os_dma_desc *ad405x_dma_desc;

struct no_os_dma_init_param i3c_dma_ip = {
	.id = 0,
	.num_ch = 3,
	.platform_ops = DMA_OPS,
};

/* STM32 CR DMA channel extra init params */
struct stm32_dma_channel crdma_channel = {
	.hdma = I3C_CRDMA_INSTANCE,
	.ch_num = I3C_CRDMA_CHANNEL_NUM,
	.mem_increment = true,
	.per_increment = false,
	.mem_data_alignment = DATA_ALIGN_WORD,
	.per_data_alignment = DATA_ALIGN_WORD,
	.dma_mode = DMA_NORMAL_MODE
};

/* STM32 Tx DMA channel extra init params */
struct stm32_dma_channel txdma_channel = {
	.hdma = I3C_TXDMA_INSTANCE,
	.ch_num = I3C_TXDMA_CHANNEL_NUM,
	.mem_increment = true,
	.per_increment = false,
	.mem_data_alignment = DATA_ALIGN_BYTE,
	.per_data_alignment = DATA_ALIGN_BYTE,
	.dma_mode = DMA_NORMAL_MODE
};

/* STM32 Rx DMA channel extra init params */
struct stm32_dma_channel rxdma_channel = {
	.hdma = I3C_RXDMA_INSTANCE,
	.ch_num = I3C_RXDMA_CHANNEL_NUM,
	.mem_increment = true,
	.per_increment = false,
	.mem_data_alignment = DATA_ALIGN_BYTE,
	.per_data_alignment = DATA_ALIGN_BYTE,
	.dma_mode = DMA_NORMAL_MODE,
};

struct stm32_i3c_dma_desc i3c_dma_desc;
__attribute__((constructor))
static void init_stm32_i3c_dma_desc(void)
{
	no_os_dma_init(&ad405x_dma_desc, &i3c_dma_ip);

	ad405x_dma_desc->channels[0].id = (uint32_t)crdma_channel.hdma;
	ad405x_dma_desc->channels[0].extra = &crdma_channel;
	ad405x_dma_desc->channels[0].irq_num = CR_DMA_IRQ_ID;

	ad405x_dma_desc->channels[1].id = (uint32_t)rxdma_channel.hdma;
	ad405x_dma_desc->channels[1].extra = &rxdma_channel;
	ad405x_dma_desc->channels[1].irq_num = RX_DMA_IRQ_ID;

	ad405x_dma_desc->channels[2].id = (uint32_t)txdma_channel.hdma;
	ad405x_dma_desc->channels[2].extra = &txdma_channel;
	ad405x_dma_desc->channels[2].irq_num = TX_DMA_IRQ_ID;

	i3c_dma_desc.dma_desc = ad405x_dma_desc;
	i3c_dma_desc.crdma_ch = &ad405x_dma_desc->channels[0];
	i3c_dma_desc.rxdma_ch = &ad405x_dma_desc->channels[1];
	i3c_dma_desc.txdma_ch = &ad405x_dma_desc->channels[2];
	i3c_dma_desc.srdma_ch = NULL;
}

struct stm32_i3c_init_param ad405x_i3c_extra_ip = {
	.hi3c = I3C_INSTANCE,
	.irq_id = I3C_IRQ_ID,
	.i3c_dma_desc = &i3c_dma_desc,
};
#endif

#ifdef AD405X_SPI
struct stm32_spi_init_param ad405x_spi_extra_ip = {
	.chip_select_port =  GPIO_CS_PORT,
	.get_input_clock = HAL_RCC_GetPCLK2Freq,
};
#endif

/* GPIO */
struct stm32_gpio_init_param gpio_init = {
	.mode = GPIO_MODE_INPUT,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};
