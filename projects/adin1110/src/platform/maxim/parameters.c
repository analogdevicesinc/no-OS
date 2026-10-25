/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by the ADIN1110 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param adin1110_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param adin1110_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param adin1110_reset_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

#if defined(CONFIG_OA_TC6)
#include "mxc_device.h"
#include "tmr.h"
#include "no_os_irq.h"
#include "maxim_gpio_irq.h"

volatile uint32_t adin1110_irq_events;
volatile uint32_t adin1110_idle_cycles;

static void adin1110_dma_notify(void *ctx)
{
	(void)ctx;

	adin1110_irq_events++;
}

static struct max_capi_dma_extra adin1110_capi_dma_extra = {
	.irq_priority = 1,
	.irq_notify = adin1110_dma_notify,
};

static struct max_capi_spi_extra adin1110_capi_spi_extra = {
	.vssel = MAX_CAPI_GPIO_VSSEL_VDDIOH,
	.chip_select = 1U << SPI_CS,
	.rx_dma_ch_id = 0,
	.tx_dma_ch_id = 1,
};

struct capi_dma_config adin1110_capi_dma_cfg = {
	.num_chans = CAPI_DMA_NUM_CHANS,
	.ops = &max_capi_dma_ops,
	.extra = &adin1110_capi_dma_extra,
};

struct capi_spi_config adin1110_capi_spi_cfg = {
	.ops = &max_capi_spi_ops,
	.identifier = SPI_DEVICE_ID,
	.clk_freq_hz = SPI_BAUDRATE,
	.extra = &adin1110_capi_spi_extra,
};

struct capi_spi_device adin1110_capi_spi_dev = {
	.max_speed_hz = SPI_BAUDRATE,
	.mode = CAPI_SPI_MODE_0,
	.native_cs = SPI_CS,
};

struct capi_dma_handle *adin1110_capi_dma;

/* The DMA channel vectors are installed by the CAPI DMA driver */
void adin1110_capi_dma_irq_enable(void)
{
}

#if defined(ADIN1110_INT_PORT)
static struct no_os_irq_init_param adin1110_int_ip = {
	.irq_ctrl_id = ADIN1110_INT_PORT,
	.platform_ops = &max_gpio_irq_ops,
};

static void adin1110_int_handler(void *ctx)
{
	adin1110_irq(ctx);
	adin1110_irq_events++;
}
#endif

/* No DWT cycle counter on these parts: a free-running timer at PCLK instead */
#define ADIN1110_LOAD_TMR	MXC_TMR1

static void adin1110_load_timer_init(void)
{
	mxc_tmr_cfg_t cfg = {
		.pres = MXC_TMR_PRES_1,
		.mode = MXC_TMR_MODE_CONTINUOUS,
#if TARGET_NUM == 32690
		.bitMode = MXC_TMR_BIT_MODE_32,
		.clock = MXC_TMR_APB_CLK,
		.cmp_cnt = 0xFFFFFFFF,
#endif
	};

	MXC_TMR_Shutdown(ADIN1110_LOAD_TMR);
#if TARGET_NUM == 32690
	MXC_TMR_Init(ADIN1110_LOAD_TMR, &cfg, false);
#else
	MXC_TMR_Init(ADIN1110_LOAD_TMR, &cfg);
#endif
	ADIN1110_LOAD_TMR->cmp = 0xFFFFFFFF;
	ADIN1110_LOAD_TMR->cnt = 0;
	MXC_TMR_Start(ADIN1110_LOAD_TMR);
}

int adin1110_int_init(struct adin1110_desc *desc)
{
	adin1110_load_timer_init();

#if defined(ADIN1110_INT_PORT)
	struct no_os_irq_ctrl_desc *irq;
	struct no_os_callback_desc cb = {
		.callback = adin1110_int_handler,
		.ctx = desc,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};
	mxc_gpio_cfg_t pin = {
		.port = MXC_GPIO_GET_GPIO(ADIN1110_INT_PORT),
		.mask = 1U << ADIN1110_INT_PIN,
		.func = MXC_GPIO_FUNC_IN,
		.pad = MXC_GPIO_PAD_WEAK_PULL_UP,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};
	int ret;

	MXC_GPIO_Config(&pin);

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

	ret = no_os_irq_enable(irq, ADIN1110_INT_PIN);
	if (ret)
		return ret;

	NVIC_SetPriority(MXC_GPIO_GET_IRQ(ADIN1110_INT_PORT), 1);
	NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(ADIN1110_INT_PORT));
#else
	(void)desc;
#endif

	return 0;
}

void adin1110_cpu_idle(uint32_t events_seen)
{
#if defined(ADIN1110_INT_PORT)
	uint32_t start;

	__disable_irq();
	if (adin1110_irq_events == events_seen) {
		start = adin1110_cpu_cycles();
		__WFI();
		adin1110_idle_cycles += adin1110_cpu_cycles() - start;
	}
	__enable_irq();
#else
	(void)events_seen;
#endif
}

/* Core cycles, in steps of SystemCoreClock / PeripheralClock */
uint32_t adin1110_cpu_cycles(void)
{
	return ADIN1110_LOAD_TMR->cnt * (SystemCoreClock / PeripheralClock);
}

uint32_t adin1110_cpu_hz(void)
{
	return SystemCoreClock;
}
#endif
