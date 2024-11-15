/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "basic_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"
#include "no_os_util.h"
#include "no_os_units.h"

/**
 * @brief Basic example main execution
 * @return ret - Result of the example execution. If working correctly, will
 * 		 measure active and reactive energy and RMS for channel A and
 * 		 for voltage stopping whenever an interrupt occurs and resulting
 * 		 in a stoppage of the measurement.
*/
int basic_example_main()
{
	struct ade7816_desc *desc;
	bool interrupt = false;
	uint32_t rms, scaled;
	int32_t val;
	int ret;

	/** GPIO Pin Interrupt Controller */
	struct no_os_irq_ctrl_desc *gpio_irq_desc;
	struct no_os_irq_init_param gpio_irq_desc_param = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA
	};

	ret = no_os_irq_ctrl_init(&gpio_irq_desc, &gpio_irq_desc_param);
	if (ret)
		goto exit;

	ade7816_ip.irq_ctrl = gpio_irq_desc;

	ret = ade7816_init(&desc, &ade7816_ip);
	if (ret)
		goto remove_irq;

	ret = ade7816_zx_detect(desc, ADE7816_CHANNEL_A);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_set_phase(desc, ADE7816_CHANNEL_A, ADE7816_PCF_50HZ);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_set_phase(desc, ADE7816_CHANNEL_B, ADE7816_PCF_50HZ);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_set_phase(desc, ADE7816_CHANNEL_C, ADE7816_PCF_50HZ);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_set_phase(desc, ADE7816_CHANNEL_D, ADE7816_PCF_50HZ);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_set_phase(desc, ADE7816_CHANNEL_E, ADE7816_PCF_50HZ);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_set_phase(desc, ADE7816_CHANNEL_F, ADE7816_PCF_50HZ);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_set_active_thr(desc, 8000);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_set_reactive_thr(desc, 8000);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_reg_update(desc, ADE7816_MASK0_REG, NO_OS_BIT(10),
				 NO_OS_BIT(10));
	if (ret)
		goto remove_ade7816;

	ret = ade7816_write_reg(desc, ADE7816_DICOEFF_REG, 0xFFF8000);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_reg_update(desc, ADE7816_LCYCMODE_REG,
				 ADE7816_RSTREAD_MASK,
				 no_os_field_prep(ADE7816_RSTREAD_MASK, 0));
	if (ret)
		goto remove_ade7816;

	desc->lcycle_mode = false;

	ret = ade7816_write_reg(desc, ADE7816_STATUS0_REG, 0);
	if (ret)
		goto remove_ade7816;

	ret = ade7816_read_reg(desc, ADE7816_MASK0_REG, &rms);
	if (ret)
		goto remove_ade7816;

	ret = no_os_irq_enable(gpio_irq_desc, 21);
	if (ret)
		goto remove_ade7816;

	while (!interrupt) {
		pr_info("Channel A:\n");

		ret = ade7816_read_active_energy(desc, ADE7816_CHANNEL_A, &val);
		if (ret)
			goto remove_ade7816;

		pr_info("Accumulated Active Energy RAW: %d\n", val);

		ret = ade7816_read_reactive_energy(desc, ADE7816_CHANNEL_A,
						   &val);
		if (ret)
			goto remove_ade7816;

		pr_info("Accumulated Reactive Energy RAW: %d\n", val);

		ret = ade7816_read_rms(desc, ADE7816_CHANNEL_A, &rms);
		if (ret)
			goto remove_ade7816;

		ret = ade7816_rms_to_micro(desc, ADE7816_CHANNEL_A, rms,
					   &scaled);
		if (ret)
			goto remove_ade7816;

		pr_info("IA_RMS: %d uA\n", scaled);

		ret = ade7816_read_rms(desc, ADE7816_CHANNEL_VOLTAGE, &rms);
		if (ret)
			goto remove_ade7816;

		ret = ade7816_rms_to_micro(desc, ADE7816_CHANNEL_VOLTAGE, rms,
					   &scaled);
		if (ret)
			goto remove_ade7816;

		pr_info("V_RMS: %d uV\n", scaled);

		if (desc->status0) {
			pr_info("DETECTED IRQ0 FALLING EDGE, STATUS RETURN : 0x%x\n", desc->status0);
			interrupt = true;
		}

		no_os_mdelay(500);
	}

remove_ade7816:
	ade7816_remove(desc);
remove_irq:
	no_os_irq_ctrl_remove(gpio_irq_desc);
exit:
	return ret;
}
