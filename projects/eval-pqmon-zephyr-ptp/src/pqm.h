/********************************************************************************
 *   @file   pqm.h
 *   @brief  PQM constants and types for IIO device definition
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

#ifndef PQM_H
#define PQM_H

#include <stdint.h>
#include <stdbool.h>
#include "adi_pqlib.h"
#include "afe_calibration.h"

#define FW_VERSION 2.2

#define TOTAL_PQM_CHANNELS 11
#define VOLTAGE_CH_NUMBER 3
#define CURRENT_CH_NUMBER 4
#define MAX_CH_ATTRS 31
#define PQM_DEVICE_ATTR_NUMBER 71
#define WAVEFORM_BUFFER_LENGTH (256 * 7)
#define MAX_EVENT_NUMBER 6

enum availavle_values_type {
	V_CONSEL_TYPE,
	FLICKER_MODEL_TYPE,
	NOMINAL_FREQUENCY_TYPE
};

struct pqm_desc {
	uint8_t reg[TOTAL_PQM_CHANNELS];
	float pqm_global_attr[PQM_DEVICE_ATTR_NUMBER];
	uint32_t pqm_ch_attr[TOTAL_PQM_CHANNELS][MAX_CH_ATTRS];
	uint32_t active_ch;
	uint32_t ext_buff_len;
	int16_t *ext_buff;
};

struct pqm_init_para {
	float dev_global_attr[PQM_DEVICE_ATTR_NUMBER];
	uint32_t dev_ch_attr[TOTAL_PQM_CHANNELS][MAX_CH_ATTRS];
	uint32_t ext_buff_len;
	int16_t *ext_buff;
};

int32_t update_pqm_channels(void *dev, uint32_t mask);
int32_t close_pqm_channels(void *dev);

#endif /* PQM_H */
