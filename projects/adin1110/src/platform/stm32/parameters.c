/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definitions specific to STM32 platform used by the ADIN1110
 *           project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param adin1110_uart_extra_ip = {
	.huart = UART_INSTANCE,
};

struct stm32_gpio_init_param adin1110_reset_gpio_extra_ip = {
	.mode = GPIO_MODE_OUTPUT_OD,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_spi_init_param adin1110_spi_extra_ip = {
	.chip_select_port = SPI_CS_PORT,
	/*
	 * SPI2 is clocked from APB1. Without this the driver sees an input
	 * clock of 0, the prescaler math collapses to the /2 default and the
	 * bus runs at PCLK1/2 (60 MHz here) - far above the ADIN1110's 25 MHz
	 * maximum, corrupting MISO sampling. Providing the real clock lets
	 * SPI_BAUDRATE select a valid /8 -> 15 MHz prescaler.
	 */
	.get_input_clock = HAL_RCC_GetPCLK1Freq,
};

#if defined(CONFIG_OA_TC6)
/* SPI2 RX on DMA1 channel 1, TX on DMA1 channel 2, routed through DMAMUX1 */
static DMA_HandleTypeDef adin1110_spi_rx_hdma = {
	.Instance = DMA1_Channel1,
	.Init.Priority = DMA_PRIORITY_HIGH,
};

static DMA_HandleTypeDef adin1110_spi_tx_hdma = {
	.Instance = DMA1_Channel2,
	.Init.Priority = DMA_PRIORITY_MEDIUM,
};

static struct stm32_dma_chan_extra_config adin1110_spi_rx_dma = {
	.hdma = &adin1110_spi_rx_hdma,
	.ch_num = DMA_REQUEST_SPI2_RX,
};

static struct stm32_dma_chan_extra_config adin1110_spi_tx_dma = {
	.hdma = &adin1110_spi_tx_hdma,
	.ch_num = DMA_REQUEST_SPI2_TX,
};

static struct stm32_spi_extra_config adin1110_capi_spi_extra = {
	.hspi = &hspi2,
	.get_input_clock = HAL_RCC_GetPCLK1Freq,
	.rxdma_ch_id = CAPI_SPI_RX_DMA_CH,
	.txdma_ch_id = CAPI_SPI_TX_DMA_CH,
	.rxdma_extra = &adin1110_spi_rx_dma,
	.txdma_extra = &adin1110_spi_tx_dma,
};

static struct stm32_spi_device_extra adin1110_capi_spi_dev_extra = {
	.chip_select_port = SPI_CS_PORT,
	.chip_select_pin = SPI_CS,
};

struct capi_dma_config adin1110_capi_dma_cfg = {
	.num_chans = CAPI_DMA_NUM_CHANS,
	.ops = &stm32_capi_dma_ops,
};

/* dma_handle is filled in once capi_dma_init() has run */
struct capi_spi_config adin1110_capi_spi_cfg = {
	.ops = &stm32_capi_spi_ops,
	.identifier = (uint64_t)(uintptr_t)SPI2,
	.extra = &adin1110_capi_spi_extra,
};

/* controller is filled in once capi_spi_init() has run */
struct capi_spi_device adin1110_capi_spi_dev = {
	.max_speed_hz = SPI_BAUDRATE,
	.mode = CAPI_SPI_MODE_0,
	.extra = &adin1110_capi_spi_dev_extra,
};

struct capi_dma_handle *adin1110_capi_dma;
volatile uint32_t adin1110_irq_events;
volatile uint32_t adin1110_idle_cycles;

static struct stm32_gpio_irq_init_param adin1110_int_extra = {
	.port_nb = ADIN1110_INT_PORT,
};

static struct no_os_irq_init_param adin1110_int_ip = {
	.irq_ctrl_id = ADIN1110_INT_PIN,
	.platform_ops = &stm32_gpio_irq_ops,
	.extra = &adin1110_int_extra,
};

static void adin1110_int_handler(void *ctx)
{
	adin1110_irq(ctx);
	adin1110_irq_events++;
}

int adin1110_int_init(struct adin1110_desc *desc)
{
	struct no_os_irq_ctrl_desc *irq;
	struct no_os_callback_desc cb = {
		.callback = adin1110_int_handler,
		.ctx = desc,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};
	int ret;

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	ret = no_os_irq_ctrl_init(&irq, &adin1110_int_ip);
	if (ret)
		return ret;

	ret = no_os_irq_register_callback(irq, ADIN1110_INT_PIN, &cb);
	if (ret)
		return ret;

	ret = no_os_irq_trigger_level_set(irq, ADIN1110_INT_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		return ret;

	return no_os_irq_enable(irq, ADIN1110_INT_PIN);
}

/*
 * Sleep until the next interrupt unless one arrived since events_seen was
 * read. WFI wakes on a pending interrupt even with PRIMASK set, so an event
 * landing between the check and the WFI is not lost.
 */
void adin1110_cpu_idle(uint32_t events_seen)
{
	uint32_t start;

	__disable_irq();
	if (adin1110_irq_events == events_seen) {
		start = DWT->CYCCNT;
		__WFI();
		adin1110_idle_cycles += DWT->CYCCNT - start;
	}
	__enable_irq();
}

uint32_t adin1110_cpu_cycles(void)
{
	return DWT->CYCCNT;
}

uint32_t adin1110_cpu_hz(void)
{
	return SystemCoreClock;
}

void adin1110_capi_dma_irq_enable(void)
{
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
}

void DMA1_Channel1_IRQHandler(void)
{
	capi_dma_isr(adin1110_capi_dma);
	adin1110_irq_events++;
}

void DMA1_Channel2_IRQHandler(void)
{
	capi_dma_isr(adin1110_capi_dma);
	adin1110_irq_events++;
}
#endif
