/***************************************************************************//**
 *   @file   adiol100_ilink_pl.h
 *   @brief  i-link IO-Link stack port layer for the ADIOL100.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2019 rt-labs AB, Sweden.
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-only
*******************************************************************************/

#ifndef ADIOL100_ILINK_PL_H
#define ADIOL100_ILINK_PL_H

#include <stdint.h>
#include <stdbool.h>
#include "osal.h"
#include "iolink_pl_hw_drv.h"
#include "adiol100.h"

/* Port layer driver struct. The first member must be iolink_hw_drv_t so the
 * i-link stack can cast between the generic and private types. */
typedef struct iolink_adiol100_drv {
	iolink_hw_drv_t drv;

	struct adiol100_dev *dev;
	os_mutex_t *exclusive;
	os_event_t *dl_event[ADIOL100_NUM_CHANNELS];
	uint32_t pl_flag;
	bool wurq_request[ADIOL100_NUM_CHANNELS];
	bool is_iolink[ADIOL100_NUM_CHANNELS];
} iolink_adiol100_drv_t;

/* Port layer configuration. Passed to iolink_adiol100_init(). */
typedef struct iolink_adiol100_cfg {
	/* ADIOL100 driver init parameters (SPI, chip address, clock). */
	struct adiol100_init_param *adiol100_ip;
	/* Per-channel IRQ callback slots (array of ADIOL100_NUM_CHANNELS).
	 * Owned by the caller; the PL layer writes its ISR + context here. */
	void (**irq_cb)(void *);
	void **irq_cb_ctx;
} iolink_adiol100_cfg_t;

/** Initialize the ADIOL100 port layer for the i-link stack. */
iolink_hw_drv_t *iolink_adiol100_init(const iolink_adiol100_cfg_t *cfg);

#endif /* ADIOL100_ILINK_PL_H */
