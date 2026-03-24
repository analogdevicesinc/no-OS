/********************************************************************************
 *   @file   rtc_max31343.c
 *   @brief  Minimal Zephyr RTC driver for MAX31343
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 ******************************************************************************/

#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/rtc.h>
#include <errno.h>
#include "rtc_max31343.h"

#define DT_DRV_COMPAT adi_max31343

LOG_MODULE_REGISTER(rtc_max31343, CONFIG_RTC_LOG_LEVEL);

static bool max31343_validate_time(const struct rtc_time *t)
{
	if (t->tm_sec < 0 || t->tm_sec > 59) return false;
	if (t->tm_min < 0 || t->tm_min > 59) return false;
	if (t->tm_hour < 0 || t->tm_hour > 23) return false;
	if (t->tm_mday < 1 || t->tm_mday > 31) return false;
	if (t->tm_mon < 0 || t->tm_mon > 11) return false;
	if (t->tm_wday < 0 || t->tm_wday > 6) return false;
	if (t->tm_year < 0 || t->tm_year > 199) return false;
	return true;
}

struct rtc_max31343_config {
	struct i2c_dt_spec i2c;
};

static int max31343_reg_read(const struct device *dev, uint8_t reg_addr,
			     uint8_t *val, uint8_t length)
{
	const struct rtc_max31343_config *config = dev->config;

	return i2c_burst_read_dt(&config->i2c, reg_addr, val, length);
}

static int max31343_reg_write(const struct device *dev, uint8_t reg_addr,
			      uint8_t val)
{
	const struct rtc_max31343_config *config = dev->config;

	return i2c_burst_write_dt(&config->i2c, reg_addr, &val, 1);
}

static int max31343_reg_write_multiple(const struct device *dev,
				       uint8_t reg_addr, uint8_t *val,
				       uint8_t length)
{
	const struct rtc_max31343_config *config = dev->config;

	return i2c_burst_write_dt(&config->i2c, reg_addr, val, length);
}

static int max31343_reg_update(const struct device *dev, uint8_t reg_addr,
			       uint8_t mask, uint8_t val)
{
	uint8_t reg_val = 0;
	int ret;

	ret = max31343_reg_read(dev, reg_addr, &reg_val, 1);
	if (ret < 0) {
		return ret;
	}

	reg_val &= ~mask;
	reg_val |= FIELD_PREP(mask, val);

	return max31343_reg_write(dev, reg_addr, reg_val);
}

static int rtc_max31343_get_time(const struct device *dev,
				 struct rtc_time *timeptr)
{
	int ret;
	uint8_t raw_time[7]; /* sec, min, hr, day, date, month, year */

	ret = max31343_reg_read(dev, MAX31343_SECONDS, raw_time,
				ARRAY_SIZE(raw_time));
	if (ret) {
		LOG_ERR("Unable to get time. Err: %d", ret);
		return ret;
	}

	timeptr->tm_sec = bcd2bin(raw_time[0] & MAX31343_SECONDS_MASK);
	timeptr->tm_min = bcd2bin(raw_time[1] & MAX31343_MINUTES_MASK);
	timeptr->tm_hour = bcd2bin(raw_time[2] & MAX31343_HOURS_MASK);
	timeptr->tm_wday = bcd2bin(raw_time[3] & MAX31343_DAY_MASK) +
			   MAX31343_DAY_OFFSET;
	timeptr->tm_mday = bcd2bin(raw_time[4] & MAX31343_DATE_MASK);
	timeptr->tm_mon = bcd2bin(raw_time[5] & MAX31343_MONTH_MASK) - 1;

	if (raw_time[5] & MAX31343_CENTURY_MASK) {
		timeptr->tm_year = bcd2bin(raw_time[6] & MAX31343_YEAR_MASK) +
				   MAX31343_YEAR_2000;
	} else {
		timeptr->tm_year = bcd2bin(raw_time[6] & MAX31343_YEAR_MASK);
	}

	return 0;
}

static int rtc_max31343_set_time(const struct device *dev,
				 const struct rtc_time *timeptr)
{
	int ret;
	uint8_t raw_time[7];

	if ((timeptr == NULL) || !max31343_validate_time(timeptr)) {
		LOG_ERR("invalid time");
		return -EINVAL;
	}

	raw_time[0] = bin2bcd(timeptr->tm_sec) & MAX31343_SECONDS_MASK;
	raw_time[1] = bin2bcd(timeptr->tm_min) & MAX31343_MINUTES_MASK;
	raw_time[2] = bin2bcd(timeptr->tm_hour) & MAX31343_HOURS_MASK;
	raw_time[3] = bin2bcd(timeptr->tm_wday - MAX31343_DAY_OFFSET) &
		      MAX31343_DAY_MASK;
	raw_time[4] = bin2bcd(timeptr->tm_mday) & MAX31343_DATE_MASK;
	raw_time[5] = bin2bcd(timeptr->tm_mon + 1) & MAX31343_MONTH_MASK;

	if (timeptr->tm_year >= MAX31343_YEAR_2000) {
		raw_time[5] |= MAX31343_CENTURY_MASK;
		raw_time[6] = bin2bcd(timeptr->tm_year % 100) &
			      MAX31343_YEAR_MASK;
	} else {
		raw_time[6] = bin2bcd(timeptr->tm_year % 100) &
			      MAX31343_YEAR_MASK;
	}

	ret = max31343_reg_write_multiple(dev, MAX31343_SECONDS, raw_time,
					  ARRAY_SIZE(raw_time));
	if (ret) {
		LOG_ERR("Error when setting time: %d", ret);
		return ret;
	}
	return 0;
}

int rtc_max31343_prepare_phase_sync(const struct device *dev,
				    const struct rtc_time *timeptr)
{
	/* Old approach: stopped oscillator early, crystal dead for 1+ second
	 * during busy-wait → variable crystal startup → 30ms phase error.
	 *
	 * New approach: crystal stays running (from init). This function
	 * just validates the time. The actual SWRST + ENOSC happens in
	 * rtc_max31343_trigger_phase_sync() at the precise second boundary.
	 */
	if ((timeptr == NULL) || !max31343_validate_time(timeptr)) {
		LOG_ERR("Phase sync: invalid time");
		return -EINVAL;
	}
	LOG_INF("RTC phase sync prepared (crystal still running)");
	return 0;
}

int rtc_max31343_start_oscillator(const struct device *dev)
{
	return max31343_reg_write(dev, MAX31343_CFG1, MAX31343_CFG1_ENOSC);
}

int rtc_max31343_trigger_phase_sync(const struct device *dev,
				    const struct rtc_time *timeptr)
{
	int ret;

	/*
	 * TIMING-CRITICAL SECTION
	 * Called at the exact PTP second boundary. Crystal has been running
	 * continuously since init (60+ seconds). We do SWRST to zero the
	 * divider chain and immediately re-enable the oscillator.
	 *
	 * Crystal off-time: only ~500µs (3 I2C writes at 200kHz).
	 * At Q~50000, the 32.768kHz crystal retains ~99.9% amplitude
	 * after 500µs → restarts essentially instantaneously.
	 */

	/* 1. SWRST: zeros divider chain + clears all registers (including ENOSC) */
	ret = max31343_reg_write(dev, MAX31343_RTC_RESET, MAX31343_SWRST);
	if (ret) {
		return ret;
	}
	ret = max31343_reg_write(dev, MAX31343_RTC_RESET, 0);
	if (ret) {
		return ret;
	}

	/* 2. Immediately re-enable oscillator — crystal barely stopped.
	 *    Write 0x02 directly (SWRST cleared CFG1 to 0x00) to avoid
	 *    slow read-modify-write. Dividers start counting from 0 now. */
	ret = max31343_reg_write(dev, MAX31343_CFG1, MAX31343_CFG1_ENOSC);
	if (ret) {
		return ret;
	}

	/*
	 * NON-CRITICAL SECTION
	 * Crystal is running, dividers counting from 0. Now restore the
	 * config and time registers that SWRST cleared.
	 */

	/* 3. Restore interrupt config */
	ret = max31343_reg_write(dev, MAX31343_INT_EN, 0);
	if (ret) {
		return ret;
	}

	/* 4. Restore 1Hz SQW output */
	ret = max31343_reg_write(dev, MAX31343_CFG2, 0);
	if (ret) {
		return ret;
	}

	/* 5. Write target time */
	ret = rtc_max31343_set_time(dev, timeptr);
	if (ret) {
		LOG_ERR("Phase sync: failed to write time");
		return ret;
	}

	return 0;
}

static int rtc_max31343_init(const struct device *dev)
{
	const struct rtc_max31343_config *config = dev->config;
	int ret;

	if (!i2c_is_ready_dt(&config->i2c)) {
		LOG_ERR("I2C bus not ready");
		return -ENODEV;
	}

	/* Software reset */
	ret = max31343_reg_write(dev, MAX31343_RTC_RESET, MAX31343_SWRST);
	if (ret) {
		LOG_ERR("Failed to assert SW reset");
		return ret;
	}

	/* Deassert reset */
	ret = max31343_reg_write(dev, MAX31343_RTC_RESET, 0);
	if (ret) {
		LOG_ERR("Failed to deassert SW reset");
		return ret;
	}

	/* Disable all interrupts */
	ret = max31343_reg_write(dev, MAX31343_INT_EN, 0);
	if (ret) {
		LOG_ERR("Failed to disable interrupts");
		return ret;
	}

	/* Enable oscillator (ENOSC bit in CFG1) */
	ret = max31343_reg_update(dev, MAX31343_CFG1, MAX31343_CFG1_ENOSC, 1);
	if (ret) {
		LOG_ERR("Failed to enable oscillator");
		return ret;
	}

	/* Configure 1Hz square wave on INT/SQW pin (P2.13).
	 * With INT_EN=0 (all interrupts disabled), the pin outputs SQW.
	 * SQW_HZ[2:0]=000 selects 1Hz. */
	ret = max31343_reg_update(dev, MAX31343_CFG2,
				  MAX31343_CFG2_SQW_HZ_MASK, 0);
	if (ret) {
		LOG_ERR("Failed to configure SQW output");
		return ret;
	}

	LOG_DBG("MAX31343 RTC initialized (1Hz SQW on INT/SQW pin)");
	return 0;
}

static DEVICE_API(rtc, rtc_max31343_api) = {
	.set_time = rtc_max31343_set_time,
	.get_time = rtc_max31343_get_time,
};

#define RTC_MAX31343_DEFINE(inst)                                              \
	static const struct rtc_max31343_config rtc_max31343_config_##inst = { \
		.i2c = I2C_DT_SPEC_INST_GET(inst),                            \
	};                                                                     \
	DEVICE_DT_INST_DEFINE(inst, rtc_max31343_init, NULL, NULL,             \
			      &rtc_max31343_config_##inst, POST_KERNEL,        \
			      CONFIG_RTC_INIT_PRIORITY, &rtc_max31343_api);

DT_INST_FOREACH_STATUS_OKAY(RTC_MAX31343_DEFINE)
