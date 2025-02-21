/***************************************************************************//**
 *   @file   ad7091r8-sdz/src/common/common_data.h
 *   @brief  Defines common data to be used by ad7091r8-sdz examples.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "ad7091r8.h"
#ifdef IIO_SUPPORT
#include "iio_ad7091r8.h"
#endif
#if defined(IIO_TIMER_TRIGGER_EXAMPLE)
#include "iio_trigger.h"
#endif

extern struct no_os_uart_init_param ad7091r8_uart_ip;
extern struct no_os_spi_init_param ad7091r8_spi_ip;
extern struct ad7091r8_init_param ad7091r8_ip;

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
#define AD7091R8_TIMER_TRIG_NAME "ad7091r8-timer-trig"
extern struct no_os_timer_init_param ad7091r8_timer_ip;
extern struct no_os_irq_init_param ad7091r8_timer_irq_ip;
extern struct iio_hw_trig_init_param ad7091r8_timer_trig_ip;
#endif

#endif /* __COMMON_DATA_H__ */
