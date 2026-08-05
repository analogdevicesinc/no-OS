/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADMT4000 examples.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "admt4000.h"
#include "24xx32a.h"

#ifdef TMC
#include "tmc5240.h"
#endif

#ifdef IIO_SUPPORT
#include "iio_admt4000.h"
#include "iio_admt_evb.h"
#ifdef TMC
#include "iio_tmc5240.h"
#endif
#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger.h"
#endif
#endif

extern struct no_os_eeprom_init_param eeprom_ip;
extern struct no_os_uart_init_param admt4000_uart_ip;
extern struct admt4000_init_param admt4000_ip;
extern struct no_os_gpio_init_param gpio_busy_ip;
extern struct no_os_gpio_init_param gpio_cnv_ip;
extern struct no_os_gpio_init_param gpio_acalc_ip;
extern struct no_os_gpio_init_param gpio_fault_ip;
extern struct no_os_gpio_init_param gpio_bootloader_ip;
extern struct admt_evb_iio_init_param admt_evb_ip;
extern struct no_os_gpio_init_param gpio_v_en_ip;
extern struct no_os_gpio_init_param gpio_coil_rs_ip;
extern struct no_os_gpio_init_param gpio_shdn_n_ip;

#if (TMC)
extern struct tmc5240_init_param tmc5240_ip;
#endif

#endif /* __COMMON_DATA_H__ */
