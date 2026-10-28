/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <stdint.h>
#include "stm32_hal.h"
#include "capi_platform.h"

#ifdef CONFIG_CAPI_UART
#include "stm32_capi_uart.h"
#endif

/**
 * @brief One-call STM32 platform initialization.
 *
 * Delegates to stm32_init(), the STM32CubeMX-generated bring-up routine that
 * configures the MPU, the HAL, the system clock and all enabled peripherals.
 *
 * @return 0 on success, negative error code on failure.
 */
int capi_platform_init(void)
{
	return stm32_init();
}

/**
 * @brief Get the STM32 CPU (HCLK) clock frequency in Hz.
 *
 * Returns the CMSIS-maintained SystemCoreClock, which the HAL updates to the
 * active core clock after the system clock is configured.
 *
 * @param [out] clock_hz CPU clock frequency in Hz.
 * @return 0 on success, -EINVAL on a NULL argument.
 */
int capi_platform_get_clock_hz(uint64_t *clock_hz)
{
	if (!clock_hz)
		return -EINVAL;

	*clock_hz = (uint64_t)SystemCoreClock;

	return 0;
}

/**
 * @brief STM32 platform tick handler.
 *
 * Advances the HAL millisecond time base. HAL_IncTick() only increments the
 * HAL tick counter, so it is safe to call from the SysTick (or any) ISR, which
 * is the context this is invoked from.
 */
void capi_platform_tick(void)
{
	HAL_IncTick();
}

/**
 * @brief Redirect stdio (printf/scanf) to the given CAPI UART handle.
 *
 * Delegates to the STM32 UART backend's stdio hook, which records the handle
 * used by the _read/_write syscall stubs. Requires the CAPI UART backend to be
 * enabled.
 *
 * @param [in] handle Initialized CAPI UART handle.
 * @return 0 on success, negative error code on failure.
 */
int capi_stdio_init(struct capi_uart_handle *handle)
{
#ifdef CONFIG_CAPI_UART
	return stm32_uart_stdio_enable(handle);
#else
	(void)handle;

	return -ENOSYS;
#endif
}
