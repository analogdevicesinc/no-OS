/***************************************************************************//**
 *   @file   adin1110_standalone_example.c
 *   @brief  Implementation of the ADIN1110 standalone example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <stdio.h>
#include "common_data.h"

#include "no_os_error.h"
#include "adin1110.h"

/***************************************************************************//**
 * @brief Configure the ADIN1110 and read the device ID.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int adin1110_standalone_example_main()
{
	struct no_os_uart_desc *uart_desc;
	struct adin1110_desc *adin1110;
	uint32_t device_id;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = adin1110_init(&adin1110, &adin1110_ip);
	if (ret) {
		printf("Error during ADIN1110 config (%d)\n", ret);
		return ret;
	}

	ret = adin1110_reg_read(adin1110, 0x1, &device_id);
	if (ret) {
		printf("Error reading the ADIN1110's device id (%d)\n", ret);
		goto out;
	}

	printf("Got device id 0x%X\n", device_id);

out:
	adin1110_remove(adin1110);

	return 0;
}
