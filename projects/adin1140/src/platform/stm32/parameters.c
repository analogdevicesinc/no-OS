/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by ad405x project.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
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

#include <stdbool.h>
#include <stdint.h>
#include "parameters.h"

struct stm32_uart_init_param adin1140_uart_extra_ip = {
    .huart = UART_INSTANCE,
    /* Bounded transaction timeout (ms) so no_os_uart_read() returns
     * -ETIMEDOUT instead of blocking forever (the default is HAL_MAX_DELAY).
     * The boot-time iperf menu relies on this to poll for a keypress and fall
     * back when none arrives. Comfortably larger than any pr_info line's TX
     * time at 115200 baud, so console output is never truncated. */
    .timeout = 200,
};

#if defined(DWT) && defined(CONFIG_LWIP)
/*
 * Microsecond time source for lwiperf's UDP jitter/transit measurement. The
 * generic sys_now() is derived from HAL_GetTick() and only has millisecond
 * resolution, which rounds the sub-millisecond transit deltas on the 10 Mbit
 * link down to zero. Override lwiperf's weak default with the Cortex DWT cycle
 * counter, accumulating cycle deltas into a monotonic microsecond counter
 * (keeping the sub-microsecond remainder so there is no drift and no problem
 * when the 32-bit cycle counter wraps). Only compiled when lwIP is in the
 * build (the symbol it overrides lives in lwiperf); uint32_t == lwIP's u32_t.
 */
uint32_t lwiperf_udp_now_us(void)
{
	static uint32_t last_cyc;
	static uint64_t acc_us;
	static uint32_t acc_cyc;
	static bool initialized;
	uint32_t per_us = SystemCoreClock / 1000000U;
	uint32_t cyc, delta;

	if (per_us == 0U)
		per_us = 1U;

	if (!initialized) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= 1U;
		last_cyc = DWT->CYCCNT;
		initialized = true;
		return 0;
	}

	cyc = DWT->CYCCNT;
	delta = cyc - last_cyc;
	last_cyc = cyc;

	acc_cyc += delta;
	acc_us += acc_cyc / per_us;
	acc_cyc %= per_us;

	return (uint32_t)acc_us;
}
#endif

static uint32_t get_spi1_clock(void)
{
	return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1);
}

struct stm32_spi_init_param adin1140_spi_extra_ip = {
	.chip_select_port =  GPIO_CS_PORT,
	.get_input_clock = get_spi1_clock,
};

/* GPIO */
struct stm32_gpio_init_param gpio_init = {
	.mode = GPIO_MODE_INPUT,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_gpio_irq_init_param adin1140_gpio_irq_extra_ip = {
	.port_nb = 3, /* Port D */
};
