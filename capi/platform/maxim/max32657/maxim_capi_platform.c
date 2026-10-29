/***************************************************************************//**
 *   @file   maxim_capi_platform.c
 *   @brief  Implementation of the CAPI platform contract for MAX32657
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <stdint.h>
#include "capi_platform.h"
#include "mxc_sys.h"
#include "maxim_capi_time.h"

#ifdef CONFIG_CAPI_UART
#include "maxim_capi_uart.h"
#endif

/** Function implementations **************************************************/

/**
 * @brief One-call MAX32657 platform initialization
 *
 * Nothing left to do: the MSDK SystemInit() has already configured the system
 * clock and called Board_Init() (maxim_capi_init.c), which starts the 1 ms
 * SysTick, before main() runs.
 *
 * @return 0 on success
 */
int capi_platform_init(void)
{
	return 0;
}

/**
 * @brief Get the MAX32657 CPU clock frequency in Hz
 * @param[out] clock_hz CPU clock frequency in Hz
 * @return 0 on success, -EINVAL on a NULL argument
 */
int capi_platform_get_clock_hz(uint64_t *clock_hz)
{
	if (!clock_hz)
		return -EINVAL;

	*clock_hz = (uint64_t)SystemCoreClock;

	return 0;
}

/**
 * @brief MAX32657 platform tick handler
 *
 * For middleware that owns SysTick_Handler (e.g. an RTOS calling this from
 * its tick hook). Expects a 1 kHz tick so capi_uptime() stays accurate.
 */
void capi_platform_tick(void)
{
	max_capi_time_tick();
}

/**
 * @brief Redirect stdio (printf/scanf) to the given CAPI UART handle
 * @param[in] handle Initialized CAPI UART handle
 * @return 0 on success, negative error code otherwise
 */
int capi_stdio_init(struct capi_uart_handle *handle)
{
#ifdef CONFIG_CAPI_UART
	return max_capi_uart_stdio_enable(handle);
#else
	(void)handle;

	return -ENOSYS;
#endif
}
