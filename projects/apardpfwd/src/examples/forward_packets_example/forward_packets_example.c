/***************************************************************************//**
 *   @file   forward_packets_example.c
 *   @brief  Implementation of the forward packets example.
 *   @author Aron Kis (kis.aron@analog.com)
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

#include <stdio.h>
#include "common_data.h"

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "adin1110.h"

/***************************************************************************//**
 * @brief Configure the output port of the AD-APARDPFWD-SL then ping the
 *        downstream device.
 * @return ret - Result of the example execution.
*******************************************************************************/

int example_main()
{
	struct no_os_uart_desc *uart_desc;
	struct adin1110_desc *adin1110;

	uint32_t device_id;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret) {
		pr_err("UART initialization failed (%d)\n", ret);
		return ret;
	}

	no_os_uart_stdio(uart_desc);

	ret = spi_cfg_0(adin1110_spi_cfg_0);
	if (ret) {
		pr_err("ADIN2111 SPI configuration failed (%d)\n", ret);
		goto remove_uart;
	}

	pr_info("AD-APARDPFWD FORWARD PACKETS EXAMPLE.\n");

	ret = port2_cfg(port2_cfg_0, NO_OS_GPIO_LOW);
	if (ret) {
		pr_err("AD-APARDPFWD output port configuration failed (%d)\n", ret);
		goto remove_uart;
	}

	ret = adin1110_init(&adin1110, &adin1110_ip);
	if (ret) {
		pr_err("Error during ADIN1110 config (%d)\n", ret);
		goto remove_uart;
	}

	ret = adin1110_reg_read(adin1110, ADIN1110_PHY_ID_REG, &device_id);
	if (ret) {
		pr_err("Error reading the ADIN1110's device id (%d)\n", ret);
		goto remove_adin;
	}

	pr_info("Got device id 0x%X\n", device_id);

remove_adin:
	adin1110_remove(adin1110);

remove_uart:
	no_os_uart_remove(uart_desc);

	return 0;
}
