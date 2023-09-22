/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad7091r8-sdz example.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
