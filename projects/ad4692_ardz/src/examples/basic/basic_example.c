/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_error.h"

int example_main()
{
	struct ad4692_desc *ad4692_desc;
	uint32_t val;
	int ret;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ad4692_uart_ip);
	if (ret)
		goto exit;

	no_os_uart_stdio(uart_desc);

	ret = ad4692_init(&ad4692_desc, &ad4692_ip);
	if (ret)
		goto ad4692_uart_remove;

	/*
	 * For modes driven by the internal oscillator (CNV Burst, Autonomous,
	 * SPI Burst), configure the OSC frequency before reading channels.
	 * ad4692_get_ch handles sequencer setup, sampling, and readback
	 * internally for all modes.
	 */
	if (ad4692_desc->mode != AD4692_MANUAL_MODE &&
	    ad4692_desc->mode != AD4692_CNV_CLOCK) {
		ret = ad4692_set_osc(ad4692_desc, AD4692_OSC_500KHZ);
		if (ret)
			goto remove_ad4692;
	}

	ret = ad4692_get_ch(ad4692_desc, 0, &val);
	if (ret)
		goto remove_ad4692;

	pr_info("Ch0 value: %x\n", val);

	ret = ad4692_get_ch(ad4692_desc, 1, &val);
	if (ret)
		goto remove_ad4692;

	pr_info("Ch1 value: %x\n", val);

remove_ad4692:
	ad4692_remove(ad4692_desc);
ad4692_uart_remove:
	no_os_uart_remove(uart_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
