/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for LTM3360B project
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_print_log.h"
#include "no_os_uart.h"
#include "no_os_error.h"
#include "common_data.h"
#include "ltm3360b.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int example_main()
{
	struct ltm3360b_dev *ltm3360b_dev;
	struct no_os_uart_desc *uart_desc;
	uint8_t adc_readback;
	int ret;

	ret = no_os_uart_init(&uart_desc, &ltm3360b_uart_ip);
	if (ret) {
		return ret;
	}

	pr_info("***** LTM3360B Basic Example *****\n");

	ret = ltm3360b_init(&ltm3360b_dev, &ltm3360b_ip);
	if (ret) {
		pr_err("Failed to initialize LTM3360B: %d\n", ret);
		goto error_uart;
	}

	ret = ltm3360b_adc_enable(ltm3360b_dev, true);
	if (ret) {
		pr_err("Failed to enable ADC: %d\n", ret);
		goto error_device;
	}

	ret = ltm3360b_adc_read_raw(ltm3360b_dev, LTM3360B_ADC_VOUT_P, &adc_readback);
	if (ret) {
		pr_err("Failed to read VOUT_P ADC: %d\n", ret);
		goto error_device;
	}
	pr_info("VOUT_P ADC Raw Reading: %d\n", adc_readback);

	ret = ltm3360b_adc_read_raw(ltm3360b_dev, LTM3360B_ADC_VOUT_N, &adc_readback);
	if (ret) {
		pr_err("Failed to read VOUT_N ADC: %d\n", ret);
		goto error_device;
	}
	pr_info("VOUT_N ADC Raw Reading: %d\n", adc_readback);

	ret = ltm3360b_adc_read_raw(ltm3360b_dev, LTM3360B_ADC_VIN, &adc_readback);
	if (ret) {
		pr_err("Failed to read VIN ADC: %d\n", ret);
		goto error_device;
	}
	pr_info("VIN ADC Raw Reading: %d\n", adc_readback);

	ltm3360b_remove(ltm3360b_dev);
	no_os_uart_remove(uart_desc);

	return 0;

error_device:
	ltm3360b_remove(ltm3360b_dev);

error_uart:
	pr_err("LTM3360B example failed with error: %d\n", ret);
	no_os_uart_remove(uart_desc);
	return ret;
}
