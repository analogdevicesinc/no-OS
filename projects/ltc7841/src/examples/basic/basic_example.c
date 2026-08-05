/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for ltc7841 project.
 *   @author Marvin Neil Cabuenas (marvinneil.cabuenas@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
