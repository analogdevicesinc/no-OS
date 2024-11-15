/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad7091r8-sdz example.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct no_os_uart_init_param ad7091r8_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_2_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_spi_init_param ad7091r8_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

struct ad7091r8_init_param ad7091r8_ip = {
	.spi_init = &ad7091r8_spi_ip,
	.gpio_convst = GPIO_CONVST_INIT,
	.gpio_reset = GPIO_RESET_INIT,
#ifdef AD7091R2_DEV
	.device_id = AD7091R2,
#elif defined AD7091R4_DEV
	.device_id = AD7091R4,
#else
	.device_id = AD7091R8,
#endif
};

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
/* AD7091R-8 timer init parameter */
struct no_os_timer_init_param ad7091r8_timer_ip = {
	.id = AD7091R8_TIMER_DEVICE_ID,
	.freq_hz = AD7091R8_TIMER_FREQ_HZ,
	.ticks_count = AD7091R8_TIMER_TICKS_COUNT,
	.platform_ops = TIMER_OPS,
	.extra = AD7091R8_TIMER_EXTRA,
};

/* AD7091R-8 timer irq init parameter */
struct no_os_irq_init_param ad7091r8_timer_irq_ip = {
	.irq_ctrl_id = 0,
	.platform_ops = TIMER_IRQ_OPS,
	.extra = AD7091R8_TIMER_IRQ_EXTRA,
};

/* AD7091R-8 timer trigger callback info */
const struct iio_hw_trig_cb_info ad7091r8_timer_cb_info = {
	.event = NO_OS_EVT_TIM_ELAPSED,
	.peripheral = NO_OS_TIM_IRQ,
	.handle = AD7091R8_TIMER_CB_HANDLE,
};

/* AD7091R-8 timer trigger init parameter */
struct iio_hw_trig_init_param ad7091r8_timer_trig_ip = {
	.irq_id = AD7091R8_TIMER_TRIG_IRQ_ID,
	.cb_info = ad7091r8_timer_cb_info,
	.name = AD7091R8_TIMER_TRIG_NAME,
};
#endif
