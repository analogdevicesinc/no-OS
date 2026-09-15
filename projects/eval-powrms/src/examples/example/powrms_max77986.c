/***************************************************************************//**
 *   @file   powrms_max77986.c
 *   @brief  MAX77985/MAX77986 charger setup for EVAL-POWRMS project.
 *   @author Robert Budai (robert.budai@analog.com)
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

#include "powrms_max77986.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/* Settling delay inserted after each register write during bring-up. */
#define MAX77986_WRITE_DELAY_MS		50

/**
 * @brief Write a single 8-bit value to a MAX77986 register.
 *
 * Standard register write: START -> addr(W) -> reg -> data -> STOP. A 50 ms
 * settling delay is applied after a successful write to give the part time to
 * act on the new configuration before the next register is touched.
 *
 * @param desc - I2C descriptor for the MAX77986.
 * @param reg  - Register address.
 * @param val  - Value to write.
 *
 * @return 0 on success, negative error code on failure.
 */
static int max77986_reg_write(struct no_os_i2c_desc *desc, uint8_t reg,
			      uint8_t val)
{
	uint8_t buf[2] = { reg, val };
	int ret;

	pr_debug("MAX77986 write reg 0x%02X = 0x%02X\n", reg, val);

	ret = no_os_i2c_write(desc, buf, sizeof(buf), 1);
	if (ret) {
		pr_err("MAX77986 write reg 0x%02X failed: %d\n", reg, ret);
		return ret;
	}

	no_os_mdelay(MAX77986_WRITE_DELAY_MS);

	return 0;
}

/**
 * @brief Read a single 8-bit value from a MAX77986 register.
 *
 * @param desc - I2C descriptor for the MAX77986.
 * @param reg  - Register address.
 * @param val  - Will be set to the register content on success.
 *
 * @return 0 on success, negative error code on failure.
 */
static int max77986_reg_read(struct no_os_i2c_desc *desc, uint8_t reg,
			     uint8_t *val)
{
	int ret;

	ret = no_os_i2c_write(desc, &reg, 1, 0);
	if (ret) {
		pr_err("MAX77986 addr reg 0x%02X failed: %d\n", reg, ret);
		return ret;
	}

	ret = no_os_i2c_read(desc, val, 1, 1);
	if (ret) {
		pr_err("MAX77986 read reg 0x%02X failed: %d\n", reg, ret);
		return ret;
	}

	return 0;
}

int max77986_init(struct no_os_i2c_desc **desc,
		  const struct no_os_i2c_init_param *param)
{
	struct no_os_i2c_desc *i2c_desc;
	int ret;

	if (!desc || !param)
		return -EINVAL;

	ret = no_os_i2c_init(&i2c_desc, param);
	if (ret)
		return ret;

	/*
	 * MODE 5 (charger on, high-voltage buck on) bring-up sequence. The
	 * charger config registers are protected, so CHGPROT must be unlocked
	 * before any other CHG_CNFG_* write takes effect. MODE = 0x5 is
	 * enabled last so charging only starts once the current and voltage
	 * limits are in place.
	 */

	/* Step 1: unlock charger registers (CHGPROT = 0b11). */
	ret = max77986_reg_write(i2c_desc, MAX77986_REG_CHG_CNFG_06,
				 MAX77986_CHG_CNFG_06_UNLOCK);
	if (ret)
		goto error;

	/* Step 2: trickle charge, no watchdog, restart threshold, safety timer. */
	ret = max77986_reg_write(i2c_desc, MAX77986_REG_CHG_CNFG_01,
				 MAX77986_CHG_CNFG_01_VAL);
	if (ret)
		goto error;

	/* Step 3: CHG_CC -> fast-charge current. */
	ret = max77986_reg_write(i2c_desc, MAX77986_REG_CHG_CNFG_02,
				 MAX77986_CHG_CNFG_02_VAL);
	if (ret)
		goto error;

	/* Step 4: top-off current threshold and timer. */
	ret = max77986_reg_write(i2c_desc, MAX77986_REG_CHG_CNFG_03,
				 MAX77986_CHG_CNFG_03_VAL);
	if (ret)
		goto error;

	/* Step 5: SYS tracking enabled, MINSYS and 4.20 V termination. */
	ret = max77986_reg_write(i2c_desc, MAX77986_REG_CHG_CNFG_04,
				 MAX77986_CHG_CNFG_04_VAL);
	if (ret)
		goto error;

	/* Step 6: DISIBS = 0 -> BATT-to-SYS FET follows the power path. */
	ret = max77986_reg_write(i2c_desc, MAX77986_REG_CHG_CNFG_07,
				 MAX77986_CHG_CNFG_07_VAL);
	if (ret)
		goto error;

	/* Step 7: BCKSYS -> fixed buck SYS voltage (5.0 V, maximum). */
	ret = max77986_reg_write(i2c_desc, MAX77986_REG_CHG_CNFG_08,
				 MAX77986_CHG_CNFG_08_VAL);
	if (ret)
		goto error;

	/* Step 8: CHGIN_ILIM -> adapter input current limit. */
	ret = max77986_reg_write(i2c_desc, MAX77986_REG_CHG_CNFG_09,
				 MAX77986_CHG_CNFG_09_VAL);
	if (ret)
		goto error;

	/* Step 9: BATRMV_MSK = 1, MODE = 0x5 -> enable charger + buck last. */
	ret = max77986_reg_write(i2c_desc, MAX77986_REG_CHG_CNFG_00,
				 MAX77986_CHG_CNFG_00_VAL);
	if (ret)
		goto error;

	*desc = i2c_desc;

	return 0;

error:
	no_os_i2c_remove(i2c_desc);
	return ret;
}

int max77986_get_details(struct no_os_i2c_desc *desc, uint8_t *details_00,
			 uint8_t *details_01)
{
	int ret;

	if (!desc || !details_00 || !details_01)
		return -EINVAL;

	ret = max77986_reg_read(desc, MAX77986_REG_CHG_DETAILS_00, details_00);
	if (ret)
		return ret;

	return max77986_reg_read(desc, MAX77986_REG_CHG_DETAILS_01, details_01);
}

int max77986_log_status(struct no_os_i2c_desc *desc)
{
	uint8_t details_00, details_01;
	int ret;

	ret = max77986_get_details(desc, &details_00, &details_01);
	if (ret)
		return ret;

	/*
	 * CHG_DTLS: 0x0 prequal, 0x1 fast-charge CC, 0x2 fast-charge CV,
	 * 0x3 top-off, 0x4 done, 0x6 timer fault, 0x7 QBATT disabled
	 * (DISQBAT/DISIBS), 0x8 charger off or input invalid, 0xA overtemp,
	 * 0xB watchdog expired, 0xC JEITA suspend, 0xD battery removal
	 * detected on THM, 0xE SUSPEND pin high.
	 * BAT_DTLS: 0x0 battery removed, 0x3 battery okay, 0x4 battery low.
	 */
	pr_info("MAX77986 CHGIN_DTLS=0x%X BAT_DTLS=0x%X CHG_DTLS=0x%X\n",
		MAX77986_CHGIN_DTLS(details_00), MAX77986_BAT_DTLS(details_01),
		MAX77986_CHG_DTLS(details_01));

	return 0;
}
