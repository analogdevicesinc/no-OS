/***************************************************************************//**
 *   @file   win_uart.c
 *   @brief  Implementation of Windows platform UART Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "win_uart.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

/**
 * @struct win_uart_desc
 * @brief Windows platform specific UART descriptor
 */
struct win_uart_desc {
	/** COM device handler. */
	HANDLE serialHandler;
};

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t win_uart_init(struct no_os_uart_desc **desc,
			     struct no_os_uart_init_param *param)
{
	struct win_uart_init_param *win_init;
	struct win_uart_desc *win_desc;
	struct no_os_uart_desc *descriptor;
	DCB dcbSerialParameters = { 0 };
	DWORD err;
	int ret;

	descriptor = no_os_malloc(sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	win_desc = (struct win_uart_desc*) no_os_malloc(sizeof(struct win_uart_desc));
	if (!win_desc) {
		ret = -ENOMEM;
		goto free_desc;
	}

	descriptor->extra = win_desc;
	win_init = param->extra;

	win_desc->serialHandler = CreateFile(win_init->device_id,
					     GENERIC_READ | GENERIC_WRITE,
					     0, 0, OPEN_EXISTING,
					     FILE_ATTRIBUTE_NORMAL, 0);
	if (win_desc->serialHandler == INVALID_HANDLE_VALUE) {
		ret = -EIO;
		goto free_win_desc;
	}

	switch (param->baud_rate) {
	case 110:
		dcbSerialParameters.BaudRate = CBR_110;
		break;
	case 300:
		dcbSerialParameters.BaudRate = CBR_300;
		break;
	case 600:
		dcbSerialParameters.BaudRate = CBR_600;
		break;
	case 1200:
		dcbSerialParameters.BaudRate = CBR_1200;
		break;
	case 2400:
		dcbSerialParameters.BaudRate = CBR_2400;
		break;
	case 4800:
		dcbSerialParameters.BaudRate = CBR_4800;
		break;
	case 9600:
		dcbSerialParameters.BaudRate = CBR_9600;
		break;
	case 14400:
		dcbSerialParameters.BaudRate = CBR_14400;
		break;
	case 19200:
		dcbSerialParameters.BaudRate = CBR_19200;
		break;
	case 38400:
		dcbSerialParameters.BaudRate = CBR_38400;
		break;
	case 57600:
		dcbSerialParameters.BaudRate = CBR_57600;
		break;
	case 115200:
		dcbSerialParameters.BaudRate = CBR_115200;
		break;
	case 128000:
		dcbSerialParameters.BaudRate = CBR_128000;
		break;
	case 256000:
		dcbSerialParameters.BaudRate = CBR_256000;
		break;
	default:
		ret = -EINVAL;
		goto free;
	}

	switch (param->size) {
	case NO_OS_UART_CS_5:
		dcbSerialParameters.ByteSize = 5;
		break;
	case NO_OS_UART_CS_6:
		dcbSerialParameters.ByteSize = 6;
		break;
	case NO_OS_UART_CS_7:
		dcbSerialParameters.ByteSize = 7;
		break;
	case NO_OS_UART_CS_8:
		dcbSerialParameters.ByteSize = 8;
		break;
	case NO_OS_UART_CS_9:
		dcbSerialParameters.ByteSize = 9;
		break;
	default:
		ret = -EINVAL;
		goto free;
	}

	switch (param->parity) {
	case NO_OS_UART_PAR_NO:
		dcbSerialParameters.Parity = NOPARITY;
		break;
	case NO_OS_UART_PAR_MARK:
		dcbSerialParameters.Parity = MARKPARITY;
		break;
	case NO_OS_UART_PAR_SPACE:
		dcbSerialParameters.Parity = SPACEPARITY;
		break;
	case NO_OS_UART_PAR_ODD:
		dcbSerialParameters.Parity = ODDPARITY;
		break;
	case NO_OS_UART_PAR_EVEN:
		dcbSerialParameters.Parity = EVENPARITY;
		break;
	default:
		ret = -EINVAL;
		goto free;
	}

	if (param->stop == NO_OS_UART_STOP_1_BIT)
		dcbSerialParameters.StopBits = ONESTOPBIT;
	else
		dcbSerialParameters.StopBits = TWOSTOPBITS;

	if (!SetCommState(win_desc->serialHandler, &dcbSerialParameters)) {
		ret = -EIO;
		goto free;
	}

	*desc = descriptor;

	return 0;

free:
	CloseHandle(win_desc->serialHandler);
free_win_desc:
	no_os_free(win_desc);
free_desc:
	no_os_free(descriptor);

	return ret;
};

/**
 * @brief Free the resources allocated by win_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t win_uart_remove(struct no_os_uart_desc *desc)
{
	struct win_uart_desc *win_desc;
	int32_t ret;

	win_desc = desc->extra;

	ret = CloseHandle(win_desc->serialHandler);
	if (!ret)
		return -EIO;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
};

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t win_uart_write(struct no_os_uart_desc *desc,
			      const uint8_t *data,
			      uint32_t bytes_number)
{
	struct win_uart_desc *win_desc;
	DWORD dwBytesWritten = 0;
	uint32_t count = 0;
	int32_t ret;

	win_desc = desc->extra;

	while (count < bytes_number) {
		ret = WriteFile(win_desc->serialHandler, data, bytes_number, &dwBytesWritten,
				NULL);
		if (!ret) {
			count += dwBytesWritten;
		}
	}

	return 0;
};

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t win_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			     uint32_t bytes_number)
{
	struct win_uart_desc *win_desc;
	DWORD dwBytesRead = 0;
	uint32_t count = 0;
	int ret;

	win_desc = desc->extra;

	while (count < bytes_number) {
		ret = ReadFile(win_desc->serialHandler, data, bytes_number, &dwBytesRead, NULL);
		if (!ret)
			count += dwBytesRead;
	}

	return 0;
};

/**
 * @brief Windows platform specific UART platform ops structure
 */
const struct no_os_uart_platform_ops win_uart_ops = {
	.init = &win_uart_init,
	.read = &win_uart_read,
	.write = &win_uart_write,
	.remove = &win_uart_remove
};
