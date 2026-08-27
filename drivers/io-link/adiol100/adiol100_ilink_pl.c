/***************************************************************************//**
 *   @file   adiol100_ilink_pl.c
 *   @brief  i-link IO-Link stack port layer for the ADIOL100.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2019 rt-labs AB, Sweden.
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-only
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "adiol100_ilink_pl.h"
#include "iolink_dl.h"
#include "osal_log.h"
#include "no_os_alloc.h"

static void adiol100_pl_isr(void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)arg;
	uint8_t ch;

	if (!drv)
		return;

	for (ch = 0; ch < ADIOL100_NUM_CHANNELS; ch++) {
		if (drv->dl_event[ch] != NULL)
			os_event_set(drv->dl_event[ch], drv->pl_flag);
	}
}

static iolink_baudrate_t adiol100_pl_get_baudrate(
	iolink_hw_drv_t *iolink_hw,
	void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;
	uint8_t comrt;

	if (!drv)
		return IOLINK_BAUDRATE_NONE;

	adiol100_get_comrt(drv->dev, ch, &comrt);

	switch (comrt) {
	case 1:
		return IOLINK_BAUDRATE_COM1;
	case 2:
		return IOLINK_BAUDRATE_COM2;
	case 3:
		return IOLINK_BAUDRATE_COM3;
	default:
		return IOLINK_BAUDRATE_NONE;
	}
}

static uint8_t adiol100_pl_get_cycletime(
	iolink_hw_drv_t *iolink_hw,
	void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;
	uint16_t val;

	if (!drv)
		return 0;

	adiol100_get_cycle_tmr(drv->dev, ch, &val);

	/* Our register is 16-bit; cycle time byte is in the low 8 bits. */
	return (uint8_t)(val & 0xFF);
}

static void adiol100_pl_set_cycletime(
	iolink_hw_drv_t *iolink_hw,
	void *arg,
	uint8_t cycbyte)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

	if (!drv)
		return;

	adiol100_set_cycle_tmr(drv->dev, ch, cycbyte);
}

static bool adiol100_pl_set_mode(
	iolink_hw_drv_t *iolink_hw,
	void *arg,
	iolink_pl_mode_t mode)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;
	uint16_t irq_mask;

	if (!drv)
		return false;

	switch (mode) {
	case iolink_mode_SDCI:
		adiol100_reset_tx_fifo(drv->dev, ch);
		adiol100_reset_rx_fifo(drv->dev, ch);

		adiol100_config_lp(drv->dev, ch, ADIOL100_LP_EN,
				   ADIOL100_LP_REV_EN);

		adiol100_config_cq(drv->dev, ch,
				   ADIOL100_CQ_PUSHPULL,
				   ADIOL100_CQ_DRV_EN,
				   ADIOL100_SINKSEL_5MA,
				   ADIOL100_CQSLEW_250NS);

		adiol100_config_framer(drv->dev, ch,
				       ADIOL100_CHKS_EN,
				       ADIOL100_FRAMER_EN);

		irq_mask = ADIOL100_RXDARDYINT | ADIOL100_TXERRINT |
			   ADIOL100_RXERRINT | ADIOL100_ESTCOMSUCINT |
			   ADIOL100_ESTCOMFAILINT;
		adiol100_enable_channel_irq(drv->dev, ch, irq_mask);

		drv->is_iolink[ch] = true;
		drv->wurq_request[ch] = false;
		break;

	case iolink_mode_DI:
	case iolink_mode_INACTIVE:
		adiol100_enable_channel_irq(drv->dev, ch, 0);
		adiol100_config_cq(drv->dev, ch,
				   ADIOL100_CQ_NPN,
				   ADIOL100_CQ_DRV_DIS,
				   ADIOL100_SINKSEL_OFF,
				   ADIOL100_CQSLEW_250NS);
		adiol100_config_lp(drv->dev, ch, ADIOL100_LP_DIS,
				   ADIOL100_LP_REV_DIS);
		drv->is_iolink[ch] = false;
		drv->wurq_request[ch] = false;
		break;

	case iolink_mode_DO:
		adiol100_enable_channel_irq(drv->dev, ch, 0);
		adiol100_config_cq(drv->dev, ch,
				   ADIOL100_CQ_PUSHPULL,
				   ADIOL100_CQ_DRV_EN,
				   ADIOL100_SINKSEL_OFF,
				   ADIOL100_CQSLEW_250NS);
		adiol100_config_lp(drv->dev, ch, ADIOL100_LP_DIS,
				   ADIOL100_LP_REV_DIS);
		drv->is_iolink[ch] = false;
		drv->wurq_request[ch] = false;
		break;
	}

	return true;
}

static void adiol100_pl_enable_cycle_timer(
	iolink_hw_drv_t *iolink_hw,
	void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

	if (!drv)
		return;

	adiol100_enable_cycle_timer(drv->dev, ch);
}

static void adiol100_pl_disable_cycle_timer(
	iolink_hw_drv_t *iolink_hw,
	void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

	if (!drv)
		return;

	adiol100_disable_cycle_timer(drv->dev, ch);
}

static void adiol100_pl_get_error(
	iolink_hw_drv_t *iolink_hw,
	void *arg,
	uint8_t *cqerr,
	uint8_t *devdly)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;
	uint16_t rxstat;

	if (!drv || !cqerr || !devdly)
		return;

	adiol100_get_fifo_status(drv->dev, ch, &rxstat);

	/* Error flags are in the upper byte of RxFIFOStat. */
	*cqerr = (uint8_t)((rxstat >> 8) & 0xFF);
	/* ADIOL100 has no DeviceDly register. */
	*devdly = 0;
}

static bool adiol100_pl_get_data(
	iolink_hw_drv_t *iolink_hw,
	void *arg,
	uint8_t *rxdata,
	uint8_t len)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

	if (!drv || !rxdata)
		return false;

	return adiol100_read_msg(drv->dev, ch, rxdata, len) == 0;
}

static void adiol100_pl_dl_msg(
	iolink_hw_drv_t *iolink_hw,
	void *arg,
	uint8_t rxbytes,
	uint8_t txbytes,
	uint8_t *data)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

	if (!drv || !data)
		return;

	adiol100_reset_tx_fifo(drv->dev, ch);
	adiol100_load_msg(drv->dev, ch, data, txbytes, rxbytes,
			  ADIOL100_KEEP_MSG);
}

static void adiol100_pl_send_msg(
	iolink_hw_drv_t *iolink_hw,
	void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

	if (!drv)
		return;

	adiol100_send_msg(drv->dev, ch);
}

static void adiol100_pl_transfer_req(
	iolink_hw_drv_t *iolink_hw,
	void *arg,
	uint8_t rxbytes,
	uint8_t txbytes,
	uint8_t *data)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

	if (!drv || !data)
		return;

	adiol100_reset_tx_fifo(drv->dev, ch);
	adiol100_load_and_send_msg(drv->dev, ch, data, txbytes, rxbytes,
				   ADIOL100_KEEP_MSG);
}

static bool adiol100_pl_init_sdci(
	iolink_hw_drv_t *iolink_hw,
	void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;
	uint16_t fc1_reg;

	if (!drv)
		return false;

	if (ch == ADIOL100_CH_A)
		fc1_reg = ADIOL100_REG_FRAMCTRL1_A;
	else
		fc1_reg = ADIOL100_REG_FRAMCTRL1_B;

	if (drv->wurq_request[ch])
		return false;

	adiol100_update(drv->dev, fc1_reg, ADIOL100_CQSEND, 0);
	adiol100_disable_cycle_timer(drv->dev, ch);
	adiol100_reset_tx_fifo(drv->dev, ch);
	adiol100_reset_rx_fifo(drv->dev, ch);

	adiol100_update(drv->dev, fc1_reg, ADIOL100_ESTCOM, ADIOL100_ESTCOM);

	drv->wurq_request[ch] = true;

	return true;
}

static void adiol100_pl_configure_event(
	iolink_hw_drv_t *iolink_hw,
	void *arg,
	os_event_t *event,
	uint32_t flag)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

	if (!drv)
		return;

	drv->dl_event[ch] = event;
	drv->pl_flag = flag;
}

static void adiol100_pl_handler(
	iolink_hw_drv_t *iolink_hw,
	void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;
	uint16_t irq;
	uint16_t rxstat;

	if (!drv)
		return;

	if (!drv->is_iolink[ch])
		return;

	adiol100_get_channel_irq(drv->dev, ch, &irq);

	if (irq & ADIOL100_TXERRINT) {
		if (drv->wurq_request[ch]) {
			adiol100_get_fifo_status(drv->dev, ch, &rxstat);
		} else {
			os_event_set(drv->dl_event[ch], IOLINK_PL_EVENT_TXERR);
		}
	}

	if (irq & ADIOL100_RXERRINT)
		os_event_set(drv->dl_event[ch], IOLINK_PL_EVENT_RXERR);

	if (irq & ADIOL100_RXDARDYINT)
		os_event_set(drv->dl_event[ch], IOLINK_PL_EVENT_RXRDY);

	if (irq & (ADIOL100_ESTCOMSUCINT | ADIOL100_ESTCOMFAILINT)) {
		drv->wurq_request[ch] = false;
		os_event_set(drv->dl_event[ch], IOLINK_PL_EVENT_WURQ);
	}
}

static const iolink_hw_ops_t adiol100_hw_ops = {
	.get_baudrate        = adiol100_pl_get_baudrate,
	.get_cycletime       = adiol100_pl_get_cycletime,
	.set_cycletime       = adiol100_pl_set_cycletime,
	.set_mode            = adiol100_pl_set_mode,
	.enable_cycle_timer  = adiol100_pl_enable_cycle_timer,
	.disable_cycle_timer = adiol100_pl_disable_cycle_timer,
	.get_error           = adiol100_pl_get_error,
	.get_data            = adiol100_pl_get_data,
	.send_msg            = adiol100_pl_send_msg,
	.dl_msg              = adiol100_pl_dl_msg,
	.transfer_req        = adiol100_pl_transfer_req,
	.init_sdci           = adiol100_pl_init_sdci,
	.configure_event     = adiol100_pl_configure_event,
	.pl_handler          = adiol100_pl_handler,
};

iolink_hw_drv_t *iolink_adiol100_init(const iolink_adiol100_cfg_t *cfg)
{
	iolink_adiol100_drv_t *drv;
	uint8_t ch;
	int ret;

	if (!cfg || !cfg->adiol100_ip)
		return NULL;

	drv = no_os_calloc(1, sizeof(iolink_adiol100_drv_t));
	if (!drv)
		return NULL;

	drv->drv.ops = &adiol100_hw_ops;

	ret = adiol100_init(&drv->dev, cfg->adiol100_ip);
	if (ret) {
		LOG_ERROR(IOLINK_PL_LOG,
			  "PL: adiol100_init failed: %d\n", ret);
		goto err_free;
	}

	drv->exclusive = os_mutex_create();
	if (!drv->exclusive)
		goto err_dev;

	drv->drv.mtx = drv->exclusive;

	for (ch = 0; ch < ADIOL100_NUM_CHANNELS; ch++) {
		drv->wurq_request[ch] = false;
		drv->is_iolink[ch] = false;
	}

	if (cfg->irq_cb && cfg->irq_cb_ctx) {
		cfg->irq_cb[ADIOL100_CH_A] = adiol100_pl_isr;
		cfg->irq_cb_ctx[ADIOL100_CH_A] = drv;
		cfg->irq_cb[ADIOL100_CH_B] = adiol100_pl_isr;
		cfg->irq_cb_ctx[ADIOL100_CH_B] = drv;
	}

	return &drv->drv;

err_dev:
	adiol100_remove(drv->dev);
err_free:
	no_os_free(drv);
	return NULL;
}
