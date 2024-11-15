/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of the swiot1l project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef SWIOT1L_MQTT_EXAMPLE

#include "swiot1l_mqtt.h"

#elif SWIOT1L_DEFAULT_FW

#include "swiot_fw.h"

#endif

/***************************************************************************//**
 * @brief Main function
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

#ifdef SWIOT1L_MQTT_EXAMPLE
	return swiot1l_mqtt();
#elif SWIOT1L_DEFAULT_FW
	return swiot_firmware();
#elif SWIOT1L_DEFAULT_FW + SWIOT1L_MQTT_EXAMPLE != 1
#error Invalid example selection. Only one example may be selected.
#endif
}
