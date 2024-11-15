/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by all examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "no_os_uart.h"
#include "no_os_util.h"

#if defined(APARD32690_ECHO_SERVER_EXAMPLE)
#include "lwip_socket.h"
#include "lwip_adin1110.h"
#endif

#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_gpio.h"
#include "maxim_spi.h"

extern struct no_os_uart_init_param uart_ip;

#if defined(APARD32690_ADIN1110_STANDALONE_EXAMPLE)
extern struct adin1110_init_param adin1110_ip;
#endif

#if defined(APARD32690_ECHO_SERVER_EXAMPLE)
extern struct lwip_network_param lwip_ip;
extern struct adin1110_init_param adin1110_ip;
#endif

#endif /* __COMMON_DATA_H__ */
