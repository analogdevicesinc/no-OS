/***************************************************************************//**
 *   @file   uc_settings.h
 *   @brief  Use Case Settings of AD9083 project.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
