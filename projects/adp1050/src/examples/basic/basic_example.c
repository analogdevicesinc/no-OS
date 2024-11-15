/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for adp1050 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"
#include "basic_example.h"
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_pwm.h"
#include "adp1050.h"

int basic_example_main()
{
	int ret;

	struct adp1050_desc *adp1050_desc;
	uint8_t data = 120;
	uint8_t data2[2];
	uint16_t vout;

	ret = adp1050_init(&adp1050_desc, &adp1050_ip);
	if (ret)
		goto exit;

	/* Unlocking CHIP and EEPROM password. */
	ret = adp1050_unlock_pass(adp1050_desc, ADP1050_CHIP_DEFAULT_PASS,
				  ADP1050_CHIP_PASS);
	if (ret)
		goto exit;

	ret = adp1050_unlock_pass(adp1050_desc, ADP1050_EEPROM_DEFAULT_PASS,
				  ADP1050_EEPROM_PASS);
	if (ret)
		goto exit;

	ret = adp1050_unlock_pass(adp1050_desc, ADP1050_TRIM_DEFAULT_PASS,
				  ADP1050_TRIM_PASS);
	if (ret)
		goto exit;

	ret = adp1050_set_close_loop(adp1050_desc);
	if (ret)
		goto exit;

	ret = adp1050_vout_tr(adp1050_desc, ADP1050_VOUT_TR_200UV_US);
	if (ret)
		goto exit;

	ret = adp1050_vout_scale(adp1050_desc, -12, 341);
	if (ret)
		goto exit;

	ret = adp1050_vout_value(adp1050_desc, 0x3000, 0x5000);
	if (ret)
		goto exit;

	ret = adp1050_pwm_duty_cycle(adp1050_desc, 0x00DE, 0x000E, ADP1050_OUTA);
	if (ret)
		goto exit;

	ret = adp1050_set_pwm(adp1050_desc, ADP1050_OUTA, ADP1050_49KHZ);
	if (ret)
		goto exit;

	ret = adp1050_read_value(adp1050_desc, &vout, &data, ADP1050_DUTY_CYCLE);
	if (ret)
		goto exit;

	/* Checking statuses. */
	ret = adp1050_read(adp1050_desc, ADP1050_STATUS_VOUT, &data, 1);
	if (ret)
		goto exit;

	ret = adp1050_read(adp1050_desc, ADP1050_STATUS_INPUT, &data, 1);
	if (ret)
		goto exit;

	ret = adp1050_read(adp1050_desc, ADP1050_STATUS_CML, &data, 1);
	if (ret)
		goto exit;

	ret = adp1050_read(adp1050_desc, ADP1050_STATUS_WORD, data2, 2);
	if (ret)
		goto exit;

exit:
	if (ret)
		pr_info("Error!\n");
	adp1050_remove(adp1050_desc);
	return ret;
}
