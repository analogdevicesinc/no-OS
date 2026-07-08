/* SPDX-License-Identifier: LicenseRef-ADI-BSD */

/**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max78000 examples.
 *   @author Victor Pascu (victor.pascu@analog.com)
 */

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

/* Force linker to keep UART platform ops by declaring extern reference */
extern const struct no_os_uart_platform_ops max_uart_ops;

extern struct no_os_uart_init_param uart_ip;

#endif /* __COMMON_DATA_H__ */
