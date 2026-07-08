/* SPDX-License-Identifier: LicenseRef-ADI-BSD */

/**
 *   @file   parameters.h
 *   @brief  Definitions used by the max78000 project.
 *   @author Victor Pascu (victor.pascu@analog.com)
 */

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_uart.h"

/* The MAX78000FTHR routes UART0 to the on-board USB-serial bridge (CN1). */
#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200

extern struct max_uart_init_param uart_extra_ip;

#endif /* __PARAMETERS_H__ */
