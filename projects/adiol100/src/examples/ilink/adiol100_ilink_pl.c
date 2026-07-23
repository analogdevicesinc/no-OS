/***************************************************************************//**
 *   @file   adiol100_ilink_pl.c
 *   @brief  i-link IO-Link stack port layer for the ADIOL100.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <string.h>
#include "adiol100_ilink_pl.h"
#include "iolink_dl.h"
#include "osal_log.h"
#include "no_os_alloc.h"
#include "no_os_irq.h"
#include "no_os_gpio.h"

static void adiol100_pl_isr(void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)arg;
	uint8_t ch;

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

	os_mutex_lock(drv->exclusive);

	switch (mode) {
	case iolink_mode_SDCI:
		adiol100_reset_tx_fifo(drv->dev, ch);
		adiol100_reset_rx_fifo(drv->dev, ch);

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
		drv->is_iolink[ch] = false;
		drv->wurq_request[ch] = false;
		break;
	}

	os_mutex_unlock(drv->exclusive);

	return true;
}

static void adiol100_pl_enable_cycle_timer(
	iolink_hw_drv_t *iolink_hw,
	void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

	adiol100_enable_cycle_timer(drv->dev, ch);
}

static void adiol100_pl_disable_cycle_timer(
	iolink_hw_drv_t *iolink_hw,
	void *arg)
{
	iolink_adiol100_drv_t *drv = (iolink_adiol100_drv_t *)iolink_hw;
	enum adiol100_channel ch = (enum adiol100_channel)(uintptr_t)arg;

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

	os_mutex_lock(drv->exclusive);
	adiol100_get_fifo_status(drv->dev, ch, &rxstat);
	os_mutex_unlock(drv->exclusive);

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
	uint8_t rxlen = 0;
	int ret;

	os_mutex_lock(drv->exclusive);
	ret = adiol100_read_msg(drv->dev, ch, rxdata, &rxlen);
	os_mutex_unlock(drv->exclusive);

	if (ret)
		return false;

	return (rxlen > 0);
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

	if (ch == ADIOL100_CH_A)
		fc1_reg = ADIOL100_REG_FRAMCTRL1_A;
	else
		fc1_reg = ADIOL100_REG_FRAMCTRL1_B;

	os_mutex_lock(drv->exclusive);

	if (drv->wurq_request[ch]) {
		os_mutex_unlock(drv->exclusive);
		return false;
	}

	adiol100_update(drv->dev, fc1_reg, ADIOL100_CQSEND, 0);
	adiol100_disable_cycle_timer(drv->dev, ch);
	adiol100_reset_tx_fifo(drv->dev, ch);
	adiol100_reset_rx_fifo(drv->dev, ch);

	adiol100_update(drv->dev, fc1_reg, ADIOL100_ESTCOM, ADIOL100_ESTCOM);

	drv->wurq_request[ch] = true;

	os_mutex_unlock(drv->exclusive);

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

	if (!drv->is_iolink[ch])
		return;

	os_mutex_lock(drv->exclusive);

	adiol100_get_channel_irq(drv->dev, ch, &irq);

	if (irq & ADIOL100_TXERRINT) {
		if (drv->wurq_request[ch]) {
			adiol100_get_fifo_status(drv->dev, ch, &irq);
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

	os_mutex_unlock(drv->exclusive);
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
	drv->drv.mtx = drv->exclusive;

	for (ch = 0; ch < ADIOL100_NUM_CHANNELS; ch++) {
		drv->wurq_request[ch] = false;
		drv->is_iolink[ch] = false;
	}

	if (cfg->irq_gpio_a) {
		ret = no_os_gpio_get(&drv->irq_gpio_a_desc, cfg->irq_gpio_a);
		if (ret)
			goto err_dev;

		ret = no_os_gpio_direction_input(drv->irq_gpio_a_desc);
		if (ret)
			goto err_gpio_a;

		ret = no_os_irq_ctrl_init(&drv->irq_ctrl, cfg->irq_ip);
		if (ret)
			goto err_gpio_a;

		struct no_os_callback_desc irq_cb = {
			.callback = adiol100_pl_isr,
			.ctx = drv,
			.event = NO_OS_EVT_GPIO,
			.peripheral = NO_OS_GPIO_IRQ,
		};

		ret = no_os_irq_register_callback(drv->irq_ctrl,
						   drv->irq_gpio_a_desc->number,
						   &irq_cb);
		if (ret)
			goto err_irq_ctrl;

		ret = no_os_irq_trigger_level_set(drv->irq_ctrl,
						  drv->irq_gpio_a_desc->number,
						  NO_OS_IRQ_EDGE_FALLING);
		if (ret)
			goto err_irq_ctrl;

		ret = no_os_irq_enable(drv->irq_ctrl,
				       drv->irq_gpio_a_desc->number);
		if (ret)
			goto err_irq_ctrl;
	}

	if (cfg->irq_gpio_b) {
		ret = no_os_gpio_get(&drv->irq_gpio_b_desc, cfg->irq_gpio_b);
		if (ret)
			goto err_irq_ctrl;

		ret = no_os_gpio_direction_input(drv->irq_gpio_b_desc);
		if (ret)
			goto err_gpio_b;

		struct no_os_callback_desc irq_cb_b = {
			.callback = adiol100_pl_isr,
			.ctx = drv,
			.event = NO_OS_EVT_GPIO,
			.peripheral = NO_OS_GPIO_IRQ,
		};

		ret = no_os_irq_register_callback(drv->irq_ctrl,
						   drv->irq_gpio_b_desc->number,
						   &irq_cb_b);
		if (ret)
			goto err_gpio_b;

		ret = no_os_irq_trigger_level_set(drv->irq_ctrl,
						  drv->irq_gpio_b_desc->number,
						  NO_OS_IRQ_EDGE_FALLING);
		if (ret)
			goto err_gpio_b;

		ret = no_os_irq_enable(drv->irq_ctrl,
				       drv->irq_gpio_b_desc->number);
		if (ret)
			goto err_gpio_b;
	}

	return &drv->drv;

err_gpio_b:
	no_os_gpio_remove(drv->irq_gpio_b_desc);
err_irq_ctrl:
	if (drv->irq_ctrl)
		no_os_irq_ctrl_remove(drv->irq_ctrl);
err_gpio_a:
	if (drv->irq_gpio_a_desc)
		no_os_gpio_remove(drv->irq_gpio_a_desc);
err_dev:
	adiol100_remove(drv->dev);
err_free:
	no_os_free(drv);
	return NULL;
}
