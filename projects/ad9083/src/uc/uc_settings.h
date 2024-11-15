/***************************************************************************//**
 *   @file   uc_settings.h
 *   @brief  Use Case Settings of AD9083 project.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __UC_SETTINGS_H
#define __UC_SETTINGS_H

#include <stdint.h>
#include "adi_cms_api_common.h"

/* uc_settings */
struct uc_settings {
	/* clk_hz */
	uint64_t (*clk_hz)[3];
	/* vmax */
	uint32_t *vmax;
	/* fc */
	uint32_t *fc;
	/* rterm */
	uint8_t *rterm;
	/* en_hp */
	uint32_t *en_hp;
	/* backoff */
	uint32_t *backoff;
	/* finmax */
	uint32_t *finmax;
	/* nco_freq_hz */
	uint64_t (*nco_freq_hz)[3];
	/* decimation */
	uint8_t (*decimation)[4];
	/* nco0_datapath_mode */
	uint8_t *nco0_datapath_mode;
	/* jtx_logiclane_mapping_pe_brd */
	uint8_t *jtx_logiclane_mapping_pe_brd;
	/* jtx_param */
	adi_cms_jesd_param_t *jtx_param;
};

/* Get uc settings */
struct uc_settings* get_uc_settings();

#endif /* __UC_SETTINGS_H */
