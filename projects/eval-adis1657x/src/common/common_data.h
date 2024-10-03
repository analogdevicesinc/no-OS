/***************************************************************************//**
 *   @file   eval-adis1657x/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adis1657x examples.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_includes.h"
#include "adis1657x.h"
#ifdef IIO_SUPPORT
#include "iio_adis1657x.h"
#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger.h"
#endif
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

extern struct no_os_uart_init_param adis1657x_uart_ip;
extern struct no_os_spi_init_param adis1657x_spi_ip;
extern struct no_os_gpio_init_param adis1657x_gpio_reset_ip;
extern struct adis_init_param adis1657x_ip;

#ifdef IIO_TRIGGER_EXAMPLE
#define ADIS1657X_GPIO_TRIG_NAME "adis16577-3-dev0"

extern struct iio_hw_trig_init_param adis1657x_gpio_trig_ip;
extern struct no_os_irq_init_param adis1657x_gpio_irq_ip;
#endif

#endif /* __COMMON_DATA_H__ */
