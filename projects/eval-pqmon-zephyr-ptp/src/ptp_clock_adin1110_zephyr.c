/********************************************************************************
 *   @file   ptp_clock_adin1110_zephyr.c
 *   @brief  ADIN1110 hardware PTP clock driver for Zephyr
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

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/ptp_clock.h>
#include <zephyr/drivers/ethernet/eth_adin2111.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/net/ptp_time.h>

#include <time.h>
#include <zephyr/drivers/rtc.h>

#include "ptp_clock_adin1110_hw.h"
#include "rtc_max31343.h"
#include "afe_sync.h"

#define DT_DRV_COMPAT adi_adin1110_ptp_clock

LOG_MODULE_REGISTER(ptp_clock_adin1110, LOG_LEVEL_INF);

/* ADIN1110 PTP register definitions - validated from working implementation */
#define ADIN1110_TS_ADDEND      0x80    /* Frequency control register */
#define ADIN1110_TS_1SEC_CMP    0x81    /* 1 second compare value */
#define ADIN1110_TS_SEC_CNT     0x82    /* Second counter (WRITE-ONLY) */
#define ADIN1110_TS_NS_CNT      0x83    /* Nanosecond counter (WRITE-ONLY) */
#define ADIN1110_TS_CFG         0x84    /* Timestamp configuration */
#define ADIN1110_CONFIG0        0x04    /* Frame timestamp config */
#define ADIN1110_TTSCAH         0x10    /* TX Timestamp Capture A High */
#define ADIN1110_TTSCAL         0x11    /* TX Timestamp Capture A Low */

/* TS_TIMER registers - output signal generation driven by syntonized counter */
#define ADIN1110_TS_TIMER_HI      0x85  /* High period (must be multiple of 16) */
#define ADIN1110_TS_TIMER_LO      0x86  /* Low period (must be multiple of 16) */
#define ADIN1110_TS_TIMER_QE_CORR 0x87  /* Quantization error correction (0-15) */
#define ADIN1110_TS_TIMER_START   0x88  /* Start time (ns compare, one-shot) */

/* TS_CAPT registers - syntonized counter capture on external pin edge */
#define ADIN1110_TS_CAPT_PIN_0    0x89  /* Captured syntonized counter bits 31:0 (nanoseconds) */
#define ADIN1110_TS_CAPT_PIN_1    0x8A  /* Captured syntonized counter bits 63:32 (seconds) */

/* TS_CAPT GPIO - P2.30 on MAX32650, triggers capture on rising edge */
#define TS_CAPT_NODE DT_NODELABEL(adin_ts_capt)

/* Configuration constants - from validated working implementation */
#define TS_ADDEND_DEFAULT       0x85555555  /* Default 1:1 frequency ratio */
#define TS_1SEC_CMP_VALUE       1000000000  /* 1 billion nanoseconds */
#define TS_CFG_TS_EN            BIT(0)      /* Enable timestamp counter */
#define TS_CFG_TS_CLR           BIT(1)      /* Clear timestamp counters */
#define TS_CFG_TS_TIMER_DEF     BIT(2)      /* TS_TIMER default output level */
#define TS_CFG_TS_TIMER_STOP    BIT(3)      /* Stop TS_TIMER output */
#define CONFIG0_FTSE            BIT(7)      /* Frame Timestamp Enable */
#define CONFIG0_FTSS            BIT(6)      /* Frame Timestamp Size (1=8B) */
#define CONFIG0_SYNC            BIT(15)     /* Sync configuration */

/* ADIN1110 device reference - get parent device */
#define PTP_CLOCK_NODE DT_DRV_INST(0)
#define ADIN_NODE DT_PARENT(PTP_CLOCK_NODE)
#define ADIN_DEV DEVICE_DT_GET(ADIN_NODE)

/*
 * Driver data structure.
 * Time is tracked via hardware RX/TX frame timestamps only - no k_uptime.
 */
struct adin1110_ptp_data {
	const struct device *adin_dev;      /* ADIN1110 ethernet device */
	struct k_mutex ptp_mutex;           /* Thread-safe access */
	uint32_t current_ts_addend;         /* Current TS_ADDEND value */
	uint64_t last_hw_sec;              /* Last known HW time - seconds */
	uint32_t last_hw_nsec;             /* Last known HW time - nanoseconds */
	bool hw_ts_valid;                   /* Whether we have a valid HW timestamp */
	bool initialized;                   /* Initialization state */
	struct gpio_dt_spec ts_capt_gpio;   /* TS_CAPT trigger pin (P2.30) */
	bool ts_capt_ready;                 /* TS_CAPT GPIO configured */
};

/* Driver configuration */
struct adin1110_ptp_config {
	uint32_t base_ts_addend;            /* Default TS_ADDEND value */
};

/* External ADIN1110 register access functions */
extern int eth_adin2111_reg_read(const struct device *dev, const uint16_t reg, uint32_t *val);
extern int eth_adin2111_reg_write(const struct device *dev, const uint16_t reg, uint32_t val);
extern int eth_adin2111_lock(const struct device *dev, k_timeout_t timeout);
extern int eth_adin2111_unlock(const struct device *dev);

/* Forward declaration — defined after adin1110_ptp_init() */
static struct adin1110_ptp_data adin1110_ptp_data_0;

/* Set true when the PTP servo first adjusts the clock (set or rate_adjust).
 * This proves sync messages are being processed — safe to check convergence. */
static volatile bool ptp_servo_active = false;

/*
 * Update stored hardware time from frame timestamp.
 * Called by the ethernet driver on every RX/TX timestamp capture.
 */
void adin1110_ptp_update_hw_time(uint32_t sec, uint32_t nsec)
{
	struct adin1110_ptp_data *data = &adin1110_ptp_data_0;

	k_mutex_lock(&data->ptp_mutex, K_FOREVER);
	data->last_hw_sec = (uint64_t)sec;
	data->last_hw_nsec = nsec;
	data->hw_ts_valid = true;
	k_mutex_unlock(&data->ptp_mutex);
}

/*
 * Get current PTP time from last known hardware timestamp.
 *
 * Returns the most recent timestamp captured from an RX or TX frame.
 * During PTP message processing (when this is typically called),
 * the timestamp is only microseconds old - essentially "now".
 */
static int adin1110_ptp_clock_get(const struct device *dev, struct net_ptp_time *tm)
{
	struct adin1110_ptp_data *data = dev->data;

	if (!data->initialized || !data->hw_ts_valid) {
		return -ENODEV;
	}

	k_mutex_lock(&data->ptp_mutex, K_FOREVER);
	tm->second = data->last_hw_sec;
	tm->nanosecond = data->last_hw_nsec;
	k_mutex_unlock(&data->ptp_mutex);

	return 0;
}

/*
 * Set PTP clock time (step adjustment).
 * Writes new time to TS_SEC_CNT/TS_NS_CNT and updates stored value.
 */
static int adin1110_ptp_clock_set(const struct device *dev, struct net_ptp_time *tm)
{
	struct adin1110_ptp_data *data = dev->data;
	int ret;

	if (!data->initialized) {
		return -ENODEV;
	}

	k_mutex_lock(&data->ptp_mutex, K_FOREVER);

	ret = eth_adin2111_lock(data->adin_dev, K_MSEC(100));
	if (ret < 0) {
		k_mutex_unlock(&data->ptp_mutex);
		return ret;
	}

	/* Write new time to hardware syntonized counter */
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_SEC_CNT, (uint32_t)tm->second);
	if (ret == 0) {
		ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_NS_CNT, tm->nanosecond);
	}

	eth_adin2111_unlock(data->adin_dev);

	if (ret == 0) {
		/* Update stored time to match what we just wrote to hardware */
		data->last_hw_sec = tm->second;
		data->last_hw_nsec = tm->nanosecond;
		data->hw_ts_valid = true;
	}

	k_mutex_unlock(&data->ptp_mutex);

	if (ret < 0) {
		LOG_ERR("Failed to set PTP time: %d", ret);
		return ret;
	}

	/* NOTE: do NOT set ptp_servo_active here. clock_set() is a step
	 * (large offset correction). Right after stepping, offset_from_tt
	 * is transiently 0, which would falsely pass convergence checks.
	 * Only rate_adjust() (PI servo tuning) proves real convergence. */
	LOG_INF("PTP time set to %u.%09u", (uint32_t)tm->second, tm->nanosecond);
	return 0;
}

/*
 * Fine adjustment of PTP clock (nanosecond increment).
 * Adjusts the stored time value. The next frame timestamp will
 * naturally reflect the actual hardware counter state.
 */
static int adin1110_ptp_clock_adjust(const struct device *dev, int increment)
{
	struct adin1110_ptp_data *data = dev->data;

	if (!data->initialized) {
		return -ENODEV;
	}

	k_mutex_lock(&data->ptp_mutex, K_FOREVER);

	int64_t ns = (int64_t)data->last_hw_nsec + increment;
	while (ns >= NSEC_PER_SEC) {
		data->last_hw_sec++;
		ns -= NSEC_PER_SEC;
	}
	while (ns < 0) {
		data->last_hw_sec--;
		ns += NSEC_PER_SEC;
	}
	data->last_hw_nsec = (uint32_t)ns;

	k_mutex_unlock(&data->ptp_mutex);

	return 0;
}

/*
 * Rate adjustment using TS_ADDEND frequency control.
 * This is the core hardware servo - uses the validated TS_ADDEND control.
 */
static int adin1110_ptp_clock_rate_adjust(const struct device *dev, double ratio)
{
	struct adin1110_ptp_data *data = dev->data;
	const struct adin1110_ptp_config *cfg = dev->config;
	uint32_t new_ts_addend;
	int ret;

	LOG_DBG("rate_adjust(ratio=%f)", ratio);

	if (!data->initialized) {
		return -ENODEV;
	}

	/* Calculate new TS_ADDEND value from frequency ratio */
	new_ts_addend = (uint32_t)(cfg->base_ts_addend * ratio);

	/* Bounds checking - allow +/-10% adjustment range */
	if (ratio < 0.9 || ratio > 1.1) {
		LOG_WRN("TS_ADDEND ratio out of safe range: %f (must be 0.9-1.1)", ratio);
		return -EINVAL;
	}

	k_mutex_lock(&data->ptp_mutex, K_FOREVER);

	ret = eth_adin2111_lock(data->adin_dev, K_MSEC(100));
	if (ret < 0) {
		k_mutex_unlock(&data->ptp_mutex);
		return ret;
	}

	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_ADDEND, new_ts_addend);

	eth_adin2111_unlock(data->adin_dev);

	if (ret == 0) {
		ptp_servo_active = true;
		data->current_ts_addend = new_ts_addend;
		LOG_DBG("TS_ADDEND updated: 0x%08X (ratio: %f)", new_ts_addend, ratio);
	} else {
		LOG_ERR("Failed to write TS_ADDEND: %d", ret);
	}

	k_mutex_unlock(&data->ptp_mutex);
	return ret;
}

/* Zephyr PTP Clock Driver API */
static const struct ptp_clock_driver_api adin1110_ptp_api = {
	.set = adin1110_ptp_clock_set,
	.get = adin1110_ptp_clock_get,
	.adjust = adin1110_ptp_clock_adjust,
	.rate_adjust = adin1110_ptp_clock_rate_adjust,
};

/*
 * Initialize ADIN1110 PTP hardware.
 * Sets up syntonized counter with validated configuration.
 */
static int adin1110_ptp_init(const struct device *dev)
{
	struct adin1110_ptp_data *data = dev->data;
	const struct adin1110_ptp_config *cfg = dev->config;
	uint32_t config0_val;
	int ret;

	LOG_DBG("ADIN1110 PTP clock init starting...");

	/* Get ADIN1110 ethernet device */
	data->adin_dev = ADIN_DEV;
	if (!data->adin_dev) {
		LOG_ERR("ADIN1110 device not found in device tree");
		return -ENODEV;
	}
	if (!device_is_ready(data->adin_dev)) {
		LOG_ERR("ADIN1110 device not ready (check init priorities)");
		return -ENODEV;
	}
	LOG_DBG("ADIN1110 ethernet device ready");

	/* Initialize mutex */
	k_mutex_init(&data->ptp_mutex);

	ret = eth_adin2111_lock(data->adin_dev, K_FOREVER);
	if (ret < 0) {
		LOG_ERR("Failed to lock ADIN1110 device: %d", ret);
		return ret;
	}

	/* Step 1: Clear syntonized counter */
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_CFG, TS_CFG_TS_CLR);
	if (ret < 0) {
		LOG_ERR("Failed to clear syntonized counter: %d", ret);
		goto unlock_exit;
	}

	/* Wait for counter clear */
	eth_adin2111_unlock(data->adin_dev);
	k_msleep(10);
	ret = eth_adin2111_lock(data->adin_dev, K_FOREVER);
	if (ret < 0) {
		return ret;
	}

	/* Step 2: Configure TS_ADDEND for frequency control */
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_ADDEND, cfg->base_ts_addend);
	if (ret < 0) {
		LOG_ERR("Failed to set TS_ADDEND: %d", ret);
		goto unlock_exit;
	}
	LOG_DBG("TS_ADDEND initialized: 0x%08X", cfg->base_ts_addend);

	/* Step 3: Set TS_1SEC_CMP for nanosecond rollover */
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_1SEC_CMP, TS_1SEC_CMP_VALUE);
	if (ret < 0) {
		LOG_ERR("Failed to set TS_1SEC_CMP: %d", ret);
		goto unlock_exit;
	}

	/* Step 4: Enable hardware packet timestamping (FTSE + FTSS) */
	ret = eth_adin2111_reg_read(data->adin_dev, ADIN1110_CONFIG0, &config0_val);
	if (ret < 0) {
		LOG_ERR("Failed to read CONFIG0: %d", ret);
		goto unlock_exit;
	}

	config0_val |= CONFIG0_FTSE | CONFIG0_FTSS;
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_CONFIG0, config0_val);
	if (ret < 0) {
		LOG_ERR("Failed to enable packet timestamping: %d", ret);
		goto unlock_exit;
	}
	LOG_DBG("Packet timestamping enabled (FTSE|FTSS): 0x%08X", config0_val);

	/* Step 5: Seed counter with initial time.
	 * This is a placeholder (~Jan 1 2024) so the counter starts at a
	 * reasonable epoch rather than zero. PTP sync messages will correct
	 * it once convergence is reached (clock_set / rate_adjust). */
	uint32_t init_seconds = 1704067000;
	uint32_t init_nanoseconds = 0;

	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_SEC_CNT, init_seconds);
	if (ret < 0) {
		LOG_ERR("Failed to seed TS_SEC_CNT: %d", ret);
		goto unlock_exit;
	}

	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_NS_CNT, init_nanoseconds);
	if (ret < 0) {
		LOG_ERR("Failed to seed TS_NS_CNT: %d", ret);
		goto unlock_exit;
	}
	LOG_DBG("Counter seeded: %u.%09u", init_seconds, init_nanoseconds);

	/* Step 6: Enable the counter */
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_CFG, TS_CFG_TS_EN);
	if (ret < 0) {
		LOG_ERR("Failed to enable syntonized counter: %d", ret);
		goto unlock_exit;
	}

	/* Step 7: Configure frame timestamping with SYNC */
	ret = eth_adin2111_reg_read(data->adin_dev, ADIN1110_CONFIG0, &config0_val);
	if (ret < 0) {
		LOG_ERR("Failed to read CONFIG0: %d", ret);
		goto unlock_exit;
	}

	config0_val |= (CONFIG0_FTSE | CONFIG0_FTSS | CONFIG0_SYNC);
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_CONFIG0, config0_val);
	if (ret < 0) {
		LOG_ERR("Failed to configure frame timestamping: %d", ret);
		goto unlock_exit;
	}

	eth_adin2111_unlock(data->adin_dev);

	/* Initialize stored time with seed values */
	data->current_ts_addend = cfg->base_ts_addend;
	data->last_hw_sec = (uint64_t)init_seconds;
	data->last_hw_nsec = init_nanoseconds;
	data->hw_ts_valid = true;
	data->initialized = true;

	LOG_DBG("ADIN1110 PTP clock initialized (HW timestamp mode)");
	return 0;

unlock_exit:
	eth_adin2111_unlock(data->adin_dev);
	return ret;
}

/* Driver instance data */
static struct adin1110_ptp_data adin1110_ptp_data_0;

/* Driver configuration */
static const struct adin1110_ptp_config adin1110_ptp_config_0 = {
	.base_ts_addend = TS_ADDEND_DEFAULT,
};

/* Register PTP clock device with Zephyr */
DEVICE_DT_INST_DEFINE(0, adin1110_ptp_init, NULL,
					  &adin1110_ptp_data_0, &adin1110_ptp_config_0,
					  POST_KERNEL, 85,  /* Init after ethernet (CONFIG_ETH_INIT_PRIORITY=80) */
					  &adin1110_ptp_api);

/*
 * Configure TS_TIMER output signal.
 *
 * Generates a repeating waveform on the TS_TIMER pin driven by the
 * syntonized counter. HI/LO periods are rounded down to multiples of 16,
 * and the remainder is compensated via TS_TIMER_QE_CORR.
 *
 * The timer starts when the nanosecond portion of the syntonized counter
 * matches start_ns (one-shot trigger). After PTP convergence, both boards
 * hit the same ns value at the same wall-clock instant, so the outputs
 * are phase-aligned.
 */
int adin1110_ptp_configure_ts_timer(uint32_t high_ns, uint32_t low_ns,
									uint32_t start_ns)
{
	struct adin1110_ptp_data *data = &adin1110_ptp_data_0;
	uint32_t ts_cfg_val;
	int ret;

	if (!data->initialized) {
		return -ENODEV;
	}

	/* Round HI/LO to multiples of 16, compute QE_CORR from remainder */
	uint32_t hi_reg = high_ns & ~0xFU;  /* Round down to multiple of 16 */
	uint32_t lo_reg = low_ns & ~0xFU;
	uint8_t qe_corr = (high_ns & 0xF) + (low_ns & 0xF);
	if (qe_corr > 15) {
		qe_corr = 15;
	}

	LOG_DBG("TS_TIMER config: HI=%u LO=%u QE_CORR=%u START=%u",
			hi_reg, lo_reg, qe_corr, start_ns);

	ret = eth_adin2111_lock(data->adin_dev, K_MSEC(100));
	if (ret < 0) {
		return ret;
	}

	/* Step 1: Stop any running timer (preserve TS_EN) */
	ret = eth_adin2111_reg_read(data->adin_dev, ADIN1110_TS_CFG, &ts_cfg_val);
	if (ret < 0) {
		goto unlock;
	}
	ts_cfg_val |= TS_CFG_TS_TIMER_STOP;
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_CFG, ts_cfg_val);
	if (ret < 0) {
		goto unlock;
	}

	/* Step 2: Write high and low periods */
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_TIMER_HI, hi_reg);
	if (ret < 0) {
		goto unlock;
	}
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_TIMER_LO, lo_reg);
	if (ret < 0) {
		goto unlock;
	}

	/* Step 3: Write quantization error correction */
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_TIMER_QE_CORR, qe_corr);
	if (ret < 0) {
		goto unlock;
	}

	/* Step 4: Clear TS_TIMER_STOP to allow output (preserve TS_EN) */
	ts_cfg_val &= ~TS_CFG_TS_TIMER_STOP;
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_CFG, ts_cfg_val);
	if (ret < 0) {
		goto unlock;
	}

	/* Step 5: Write start time - timer begins when ns counter matches */
	ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_TIMER_START, start_ns);
	if (ret < 0) {
		goto unlock;
	}

	LOG_DBG("TS_TIMER started (1PPS on P2.9)");

unlock:
	eth_adin2111_unlock(data->adin_dev);
	if (ret < 0) {
		LOG_ERR("TS_TIMER configuration failed: %d", ret);
	}
	return ret;
}

/*
 * Stop TS_TIMER output.
 * Output returns to the default level specified by TS_TIMER_DEF.
 */
int adin1110_ptp_stop_ts_timer(void)
{
	struct adin1110_ptp_data *data = &adin1110_ptp_data_0;
	uint32_t ts_cfg_val;
	int ret;

	if (!data->initialized) {
		return -ENODEV;
	}

	ret = eth_adin2111_lock(data->adin_dev, K_MSEC(100));
	if (ret < 0) {
		return ret;
	}

	ret = eth_adin2111_reg_read(data->adin_dev, ADIN1110_TS_CFG, &ts_cfg_val);
	if (ret == 0) {
		ts_cfg_val |= TS_CFG_TS_TIMER_STOP;
		ret = eth_adin2111_reg_write(data->adin_dev, ADIN1110_TS_CFG, ts_cfg_val);
	}

	eth_adin2111_unlock(data->adin_dev);

	if (ret == 0) {
		LOG_INF("TS_TIMER stopped");
	} else {
		LOG_ERR("Failed to stop TS_TIMER: %d", ret);
	}
	return ret;
}

/*
 * Helper function for timestamp extraction.
 * Converts raw ADIN1110 64-bit timestamps to Zephyr net_ptp_time format.
 * Currently unused but kept for future use (e.g. direct raw timestamp parsing).
 */
int adin1110_extract_ptp_timestamp(uint32_t ts_high, uint32_t ts_low, struct net_ptp_time *tm)
{
	uint64_t raw_64bit = ((uint64_t)ts_high << 32) | ts_low;

	/* IEEE 1588 PTP format parsing (validated approach) */
	uint32_t seconds = (uint32_t)((raw_64bit >> 32) & 0xFFFFFFFF);
	uint32_t nanoseconds = (uint32_t)(raw_64bit & 0x3FFFFFFF);

	tm->second = (uint64_t)seconds;
	tm->nanosecond = nanoseconds;

	return 0;
}

/*
 * Initialize TS_CAPT GPIO pin.
 * Configures P2.30 as output, driven low (idle state).
 * Rising edge on this pin triggers the ADIN1110 to latch the
 * syntonized counter into TS_CAPT_PIN_0/PIN_1.
 */
int adin1110_ptp_ts_capt_init(void)
{
	struct adin1110_ptp_data *data = &adin1110_ptp_data_0;
	int ret;

	if (!data->initialized) {
		return -ENODEV;
	}

	if (data->ts_capt_ready) {
		return 0;  /* Already initialized */
	}

	data->ts_capt_gpio = (struct gpio_dt_spec)GPIO_DT_SPEC_GET(TS_CAPT_NODE, gpios);

	if (!gpio_is_ready_dt(&data->ts_capt_gpio)) {
		LOG_ERR("TS_CAPT GPIO device not ready");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&data->ts_capt_gpio, GPIO_OUTPUT_LOW);
	if (ret < 0) {
		LOG_ERR("Failed to configure TS_CAPT GPIO: %d", ret);
		return ret;
	}

	data->ts_capt_ready = true;
	LOG_DBG("TS_CAPT GPIO initialized (P2.30 output, idle low)");
	return 0;
}

/*
 * Capture the current syntonized counter value via TS_CAPT pin.
 *
 * Pulses P2.30 high (rising edge triggers capture in ADIN1110),
 * then reads TS_CAPT_PIN_1 (seconds) and TS_CAPT_PIN_0 (nanoseconds).
 * Returns the captured timestamp in net_ptp_time format.
 *
 * This provides on-demand reads of the syntonized counter,
 * independent of PTP frame traffic.
 */
int adin1110_ptp_capture_timestamp(struct net_ptp_time *tm)
{
	struct adin1110_ptp_data *data = &adin1110_ptp_data_0;
	uint32_t pin1_val, pin0_val;
	int ret;

	if (!data->initialized || !data->ts_capt_ready) {
		return -ENODEV;
	}

	/* Pulse TS_CAPT: rising edge triggers capture */
	gpio_pin_set_dt(&data->ts_capt_gpio, 1);
	/* Brief delay — the ADIN1110 latches on the edge, but we need the
	 * SPI bus anyway so the register read provides sufficient delay */
	gpio_pin_set_dt(&data->ts_capt_gpio, 0);

	/* Read captured syntonized counter value */
	ret = eth_adin2111_lock(data->adin_dev, K_MSEC(100));
	if (ret < 0) {
		return ret;
	}

	ret = eth_adin2111_reg_read(data->adin_dev, ADIN1110_TS_CAPT_PIN_1, &pin1_val);
	if (ret < 0) {
		goto unlock;
	}

	ret = eth_adin2111_reg_read(data->adin_dev, ADIN1110_TS_CAPT_PIN_0, &pin0_val);
	if (ret < 0) {
		goto unlock;
	}

unlock:
	eth_adin2111_unlock(data->adin_dev);

	if (ret < 0) {
		LOG_ERR("TS_CAPT register read failed: %d", ret);
		return ret;
	}

	/* Parse: PIN_1 = seconds (bits 63:32), PIN_0 = nanoseconds (bits 31:0, 30-bit) */
	tm->second = (uint64_t)pin1_val;
	tm->nanosecond = pin0_val & 0x3FFFFFFF;

	return 0;
}

/* ==================================================================
 * TS_TIMER Interrupt-Driven RTC Phase Sync
 *
 * Uses the hardware 1PPS signal on P2.9 (TS_TIMER) to trigger
 * deterministic RTC synchronization. Both boards receive the
 * TS_TIMER edge within ~100ns of each other, so the RTC update
 * sequence executes at the same wall-clock instant on both boards.
 *
 * Flow:
 *   1. TS_TIMER rising edge on P2.9 → GPIO ISR (~1-3µs latency)
 *   2. ISR: Pulse P2.30 HIGH→LOW (hardware-latches syntonized counter)
 *   3. ISR: k_sem_give() → wakes cooperative thread
 *   4. Thread (K_PRIO_COOP(0), scheduler locked):
 *      a. SPI: Read TS_CAPT_PIN_1/PIN_0 (seconds/nanoseconds)
 *      b. Convert epoch → rtc_time
 *      c. I2C: SWRST → ENOSC → write time (rtc_max31343_trigger_phase_sync)
 *   5. Thread: disarm, log result
 *
 * The entire thread sequence runs with k_sched_lock() so no other
 * thread can preempt between the SPI read and I2C write. Both boards
 * execute this identical code path, so the I2C timing is symmetric
 * and the inter-board phase error is dominated only by TS_TIMER
 * jitter (~100ns) + ISR latency jitter (~1-5µs).
 * ================================================================== */

/* TS_TIMER input pin (P2.9) and TS_CAPT output pin (P2.30) — both on GPIO port 2 */
#define TS_TIMER_IRQ_PIN  9
#define TS_CAPT_IRQ_PIN   30
#define GPIO_VDDIOH       0x200

/* RTC sync cooperative thread */
#define RTC_SYNC_STACK_SIZE 2048
K_THREAD_STACK_DEFINE(rtc_sync_stack, RTC_SYNC_STACK_SIZE);
static struct k_thread rtc_sync_thread_data;

/* Synchronization */
static struct k_sem rtc_sync_sem;
static volatile bool rtc_sync_armed;
static volatile bool rtc_sync_complete;
static volatile bool pqlib_sync_enabled;
static volatile bool pqlib_print_ready;
static volatile bool afe_sync_pending;   /* AFE sync armed, need 1PPS wakeup */

/* PQLib time variables — set by ISR thread, consumed by SyncLibTime() in main loop */
extern volatile int64_t ptp_time_ms;
extern volatile uint8_t newSyncTimeAvailable;

/* Device references for the sync module */
static const struct device *rtc_sync_dev;       /* MAX31343 RTC */
static const struct device *ts_timer_gpio_dev;   /* GPIO port 2 */
static struct gpio_callback ts_timer_cb_data;

/*
 * TS_TIMER GPIO ISR — called on each 1PPS rising edge.
 *
 * Runs in hardware interrupt context. Only does two things:
 * 1. Pulse P2.30 to trigger TS_CAPT hardware latch (~100ns per GPIO write)
 * 2. Wake the cooperative sync thread via semaphore
 *
 * If not armed, returns immediately (ignores the pulse).
 */
static void ts_timer_isr_cb(const struct device *dev, struct gpio_callback *cb,
							 uint32_t pins)
{
	if (!rtc_sync_armed && !pqlib_sync_enabled && !afe_sync_pending) {
		return;
	}

	/* Pulse TS_CAPT: rising edge makes ADIN1110 latch the syntonized counter.
	 * P2.30 is on the same GPIO port 2 as P2.9 (TS_TIMER). */
	gpio_pin_set(dev, TS_CAPT_IRQ_PIN, 1);
	gpio_pin_set(dev, TS_CAPT_IRQ_PIN, 0);

	k_sem_give(&rtc_sync_sem);
}

/*
 * Cooperative RTC sync thread — K_PRIO_COOP(0), highest cooperative priority.
 *
 * Waits on the semaphore from the ISR, then runs with scheduler locked:
 *   1. SPI read TS_CAPT registers (counter was latched in ISR)
 *   2. Convert PTP epoch → struct rtc_time
 *   3. I2C: SWRST + ENOSC + write time (rtc_max31343_trigger_phase_sync)
 *
 * Scheduler lock prevents any other thread from running between the SPI
 * read and I2C write, ensuring both boards execute the exact same code
 * path with the same timing.
 */
static void rtc_sync_thread_fn(void *p1, void *p2, void *p3)
{
	struct adin1110_ptp_data *data = &adin1110_ptp_data_0;
	uint32_t pin1_val, pin0_val;
	int ret;

	while (1) {
		k_sem_take(&rtc_sync_sem, K_FOREVER);

		bool do_rtc = rtc_sync_armed;

		/* Step 1: Read TS_CAPT registers via SPI.
		 * The counter was hardware-latched in the ISR (P2.30 pulse).
		 * We're just retrieving the frozen value now. */
		ret = eth_adin2111_lock(data->adin_dev, K_MSEC(10));
		if (ret < 0) {
			LOG_ERR("1PPS sync: ADIN1110 SPI lock failed: %d", ret);
			rtc_sync_armed = false;
			continue;
		}

		ret = eth_adin2111_reg_read(data->adin_dev,
									 ADIN1110_TS_CAPT_PIN_1, &pin1_val);
		if (ret == 0) {
			ret = eth_adin2111_reg_read(data->adin_dev,
										 ADIN1110_TS_CAPT_PIN_0, &pin0_val);
		}

		eth_adin2111_unlock(data->adin_dev);

		if (ret < 0) {
			LOG_ERR("1PPS sync: TS_CAPT read failed: %d", ret);
			rtc_sync_armed = false;
			continue;
		}

		/* Parse TS_CAPT: PIN_1 = seconds, PIN_0 = nanoseconds (30-bit) */
		uint32_t ptp_sec = pin1_val;
		uint32_t ptp_nsec = pin0_val & 0x3FFFFFFF;

		/* PQLib time sync — update on every 1PPS edge.
		 * Time is captured at the exact second boundary by hardware.
		 * SyncLibTime() in the main loop will pick this up and call
		 * adi_pqlib_SetTime() with near-zero uncertainty. */
		if (pqlib_sync_enabled) {
			ptp_time_ms = (int64_t)ptp_sec * 1000LL +
						  ptp_nsec / 1000000;
			newSyncTimeAvailable = 1;
			pqlib_print_ready = true;
		}

		/* AFE sync — one-shot, triggered by UDP protocol.
		 * When armed, check if this is the target second.
		 * If so, restart AFE and PQLib for aligned cycle boundaries. */
		if (afe_sync_pending) {
			if (afe_sync_check_1pps(ptp_sec)) {
				afe_sync_pending = false;
				LOG_DBG("AFE sync restart at PTP %u", ptp_sec);
			}
		}

		/* RTC sync — one-shot, with scheduler lock for deterministic
		 * timing between SPI read and I2C write. */
		if (do_rtc) {
			k_sched_lock();

			time_t epoch = (time_t)ptp_sec;
			struct tm tm_val;

			if (gmtime_r(&epoch, &tm_val) == NULL) {
				LOG_ERR("RTC sync: gmtime_r failed for epoch %u", ptp_sec);
				k_sched_unlock();
				rtc_sync_armed = false;
				continue;
			}

			struct rtc_time rt = {
				.tm_sec = tm_val.tm_sec,
				.tm_min = tm_val.tm_min,
				.tm_hour = tm_val.tm_hour,
				.tm_mday = tm_val.tm_mday,
				.tm_mon = tm_val.tm_mon,
				.tm_year = tm_val.tm_year,
				.tm_wday = tm_val.tm_wday,
				.tm_yday = tm_val.tm_yday,
				.tm_isdst = -1,
				.tm_nsec = 0,
			};

			ret = rtc_max31343_trigger_phase_sync(rtc_sync_dev, &rt);

			k_sched_unlock();

			rtc_sync_armed = false;

			if (ret == 0) {
				rtc_sync_complete = true;
				LOG_INF("RTC synced via TS_TIMER ISR: "
						"%04d-%02d-%02d %02d:%02d:%02d "
						"(PTP %u.%09u, ns_into_sec=%u)",
						rt.tm_year + 1900, rt.tm_mon + 1, rt.tm_mday,
						rt.tm_hour, rt.tm_min, rt.tm_sec,
						ptp_sec, ptp_nsec, ptp_nsec);
			} else {
				LOG_ERR("RTC sync: trigger_phase_sync failed: %d", ret);
			}
		}
	}
}

/*
 * Initialize TS_TIMER interrupt-driven RTC sync.
 *
 * Reconfigures P2.9 from MSSEL output (set during early_init for ADIN1110
 * bootstrap) to GPIO input with rising edge interrupt. The ADIN1110's
 * TS_TIMER drives P2.9 externally with the 1PPS signal.
 *
 * Creates a cooperative thread at K_PRIO_COOP(0) for the actual RTC update.
 */
int adin1110_ptp_ts_timer_irq_init(const struct device *rtc_dev)
{
	struct adin1110_ptp_data *data = &adin1110_ptp_data_0;
	int ret;

	if (!data->initialized) {
		LOG_ERR("TS_TIMER IRQ: PTP clock not initialized");
		return -ENODEV;
	}

	if (!data->ts_capt_ready) {
		LOG_ERR("TS_TIMER IRQ: TS_CAPT not initialized (call ts_capt_init first)");
		return -ENODEV;
	}

	if (!device_is_ready(rtc_dev)) {
		LOG_ERR("TS_TIMER IRQ: RTC device not ready");
		return -ENODEV;
	}

	rtc_sync_dev = rtc_dev;

	/* Get GPIO port 2 device (P2.9 and P2.30 are both on port 2) */
	ts_timer_gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio2));
	if (!device_is_ready(ts_timer_gpio_dev)) {
		LOG_ERR("TS_TIMER IRQ: GPIO2 device not ready");
		return -ENODEV;
	}

	/* Initialize sync primitives */
	k_sem_init(&rtc_sync_sem, 0, 1);
	rtc_sync_armed = false;
	rtc_sync_complete = false;
	pqlib_sync_enabled = false;
	pqlib_print_ready = false;

	/* Reconfigure P2.9 from output (MSSEL bootstrap) to input.
	 * The ADIN1110 now drives P2.9 externally with TS_TIMER 1PPS. */
	ret = gpio_pin_configure(ts_timer_gpio_dev, TS_TIMER_IRQ_PIN,
							 GPIO_INPUT | GPIO_VDDIOH);
	if (ret < 0) {
		LOG_ERR("Failed to configure P2.9 as input: %d", ret);
		return ret;
	}

	/* Configure rising edge interrupt */
	ret = gpio_pin_interrupt_configure(ts_timer_gpio_dev, TS_TIMER_IRQ_PIN,
									   GPIO_INT_EDGE_RISING);
	if (ret < 0) {
		LOG_ERR("Failed to configure P2.9 interrupt: %d", ret);
		return ret;
	}

	/* Register ISR callback */
	gpio_init_callback(&ts_timer_cb_data, ts_timer_isr_cb,
						BIT(TS_TIMER_IRQ_PIN));
	ret = gpio_add_callback(ts_timer_gpio_dev, &ts_timer_cb_data);
	if (ret < 0) {
		LOG_ERR("Failed to add P2.9 ISR callback: %d", ret);
		return ret;
	}

	/* Create cooperative sync thread at highest cooperative priority */
	k_thread_create(&rtc_sync_thread_data, rtc_sync_stack,
					K_THREAD_STACK_SIZEOF(rtc_sync_stack),
					rtc_sync_thread_fn, NULL, NULL, NULL,
					K_PRIO_COOP(0), 0, K_NO_WAIT);
	k_thread_name_set(&rtc_sync_thread_data, "rtc_sync");

	LOG_DBG("TS_TIMER IRQ-driven RTC sync initialized "
			"(P2.9 input, thread K_PRIO_COOP(0))");
	return 0;
}

void adin1110_ptp_arm_rtc_sync(void)
{
	rtc_sync_complete = false;
	rtc_sync_armed = true;
	LOG_INF("RTC sync armed — will trigger on next TS_TIMER 1PPS edge");
}

bool adin1110_ptp_rtc_sync_done(void)
{
	return rtc_sync_complete;
}

/*
 * Enable continuous PQLib time sync from TS_TIMER 1PPS.
 *
 * When enabled, every 1PPS rising edge triggers:
 *   ISR → TS_CAPT latch → thread reads SPI → sets ptp_time_ms
 *
 * SyncLibTime() in the main loop picks up the new time and calls
 * adi_pqlib_SetTime() with the precise PTP second boundary time.
 * Multiple boards receive the same 1PPS edge within ~100ns,
 * so their PQLib clocks are synchronized to the same wall-clock instant.
 */
void adin1110_ptp_enable_pqlib_sync(void)
{
	pqlib_sync_enabled = true;
	LOG_INF("PQLib 1PPS time sync enabled");
}

bool adin1110_ptp_servo_active(void)
{
	return ptp_servo_active;
}

void adin1110_ptp_disable_pqlib_sync(void)
{
	pqlib_sync_enabled = false;
	LOG_INF("PQLib 1PPS time sync disabled");
}

bool adin1110_ptp_pqlib_print_ready(void)
{
	if (pqlib_print_ready) {
		pqlib_print_ready = false;
		return true;
	}
	return false;
}

void adin1110_ptp_arm_afe_sync(void)
{
	afe_sync_pending = true;
	LOG_DBG("AFE sync: 1PPS handler armed");
}
