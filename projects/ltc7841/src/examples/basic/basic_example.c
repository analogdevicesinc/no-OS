/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for ltc7841 project.
 *   @author Marvin Neil Cabuenas (marvinneil.cabuenas@analog.com)
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
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "ltc7841.h"

int example_main(void)
{
	uint8_t value[2];
	int error;
	struct ltc7841_desc *ltc7841_desc;
	struct no_os_uart_desc *uart_desc;
	int ret = -EINVAL;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	error = ltc7841_init(&ltc7841_desc, NULL);
	if (error != E_NO_ERROR) {
		return error;
	}
	while (1) {
		error = ltc7841_mfr_clear_peaks(ltc7841_desc);
		if (error)
			return error;
		error = ltc7841_clear_status_word_bits(ltc7841_desc);
		if (error)
			return error;
		/* read every readable register for checking*/
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_OPERATION, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_VOUT_MODE, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_STATUS_WORD, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_READ_VIN, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_READ_IIN, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_READ_VOUT, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_READ_IOUT, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_READ_TEMPERATURE_1, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_PMBUS_REVISION, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_IOUT_PEAK, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_VOUT_PEAK, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_VIN_PEAK, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_TEMEPRATURE1_PEAK, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_IIN_PEAK, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_VOUT_MARGIN_HIGH, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_SPECIAL_ID, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_VOUT_COMMAND, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_CONFIG, value);
		if (error)
			return error;
		error = ltc7841_reg_read(ltc7841_desc, LTC7841_MFR_VOUT_MARGIN_LOW, value);
		if (error)
			goto remove_ltc7841;
		no_os_mdelay(500);
	}
remove_ltc7841:
	ltc7841_remove(ltc7841_desc);

	return E_NO_ERROR;
}
