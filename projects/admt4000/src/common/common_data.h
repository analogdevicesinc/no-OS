/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADMT4000 examples.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "parameters.h"
#include "admt4000.h"
#include "iio_admt_evb.h"
#include "24xx32a.h"

#ifdef TMC
#include "tmc5240.h"
#endif

#ifdef IIO_SUPPORT
#include "iio_admt4000.h"
#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger.h"
#endif
#endif

extern struct no_os_eeprom_init_param eeprom_ip;
extern struct no_os_uart_init_param admt4000_uart_ip;
extern struct admt4000_init_param admt4000_ip;
extern struct no_os_spi_init_param spi_tmc_spi_ip;
extern struct no_os_gpio_init_param gpio_shdn_n_ip;
extern struct no_os_gpio_init_param gpio_v_en_ip;
extern struct no_os_gpio_init_param gpio_rstb_ip;
extern struct no_os_gpio_init_param gpio_coil_rs_ip;

#if (TMC)
extern struct tmc5240_init_param tmc5240_ip;
#endif

#endif /* __COMMON_DATA_H__ */
