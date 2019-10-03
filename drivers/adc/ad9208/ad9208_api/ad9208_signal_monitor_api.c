// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad9208_signal_monitor_api.c
 *
 * \brief Contains AD9208 APIs for Signal Monitoring Features
 *
 * Release 1.0.x
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#include "ad9208_api.h"
#include "ad9208_reg.h"
#include "api_errors.h"

#define FD_THRESHOLD_MAG_DBFS_MAX  0x1FFF
#define FD_DWELL_CLK_CYCLES_MAX  0xFFFF

int ad9208_adc_get_overange_status(ad9208_handle_t *h, uint8_t *status)
{
	int err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (status == NULL)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_OP_OVERANGE_STAT_REG, status);
	if (err != API_ERROR_OK)
		return err;

	err = ad9208_register_write(h, AD9208_OP_OVERANGE_CLR_REG, *status);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}

int ad9208_adc_set_fd_thresholds(ad9208_handle_t *h,
				 uint16_t upper_dbfs, uint16_t lower_dbfs,
				 uint16_t dwell_time)
{
	int err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if ((upper_dbfs > FD_THRESHOLD_MAG_DBFS_MAX) ||
	    (lower_dbfs > FD_THRESHOLD_MAG_DBFS_MAX) ||
	    (dwell_time > FD_DWELL_CLK_CYCLES_MAX))
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_write(h,
				    AD9208_FD_DWELL_LSB_REG,
				    AD9208_FD_DWELL_LSB(dwell_time));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h,
				    AD9208_FD_DWELL_MSB_REG,
				    AD9208_FD_DWELL_MSB(dwell_time));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h,
				    AD9208_FD_UT_LSB_REG,
				    AD9208_FD_UT_LSB(upper_dbfs));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h,
				    AD9208_FD_UT_MSB_REG,
				    AD9208_FD_UT_MSB(upper_dbfs));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h,
				    AD9208_FD_UT_LSB_REG,
				    AD9208_FD_LT_LSB(lower_dbfs));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h,
				    AD9208_FD_LT_MSB_REG,
				    AD9208_FD_LT_MSB(lower_dbfs));
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}
