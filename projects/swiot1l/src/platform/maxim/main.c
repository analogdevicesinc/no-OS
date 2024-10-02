/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of the swiot1l project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
	printf("\n");

	printf("\e[H\e[2J\e[93mStarting MQTT Example Program:\e[0m\r\n\r\n");
	ret = swiot1l_mqtt();
	printf("\r\n\r\n\e[93mExit with code\e[0m -> %d\r\n", ret);


	return ret;
#ifdef SWIOT1L_MQTT_EXAMPLE
	return swiot1l_mqtt();
#elif SWIOT1L_DEFAULT_FW
	return swiot_firmware();
#elif SWIOT1L_DEFAULT_FW + SWIOT1L_MQTT_EXAMPLE != 1
#error Invalid example selection. Only one example may be selected.
#endif
}
