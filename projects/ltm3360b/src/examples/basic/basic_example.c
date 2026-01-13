/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for LTM3360B project
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
