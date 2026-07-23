/*******************************************************************************
 *   @file   maxim_capi_rtc.h
 *   @brief  Header file for RTC functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#ifndef MAXIM_CAPI_RTC_H_
#define MAXIM_CAPI_RTC_H_

#include "capi_irq.h"
#include "capi_rtc.h"

struct max_capi_rtc_extra {
	/** Whether to measure the crystal frequency on startup and during trim */
	bool measure_freq;
};

struct max_capi_rtc_priv {
	/** Frequency in Hz (measured, typically 32768) */
	uint32_t freq;
	/** Whether to measure the crystal frequency on startup and during trim */
	bool measure_freq;
	/** Event callback function */
	capi_rtc_event_callback_t callback;
	/** Context pointer for callback */
	void *event_ctx;
	/** Enabled events mask */
	uint32_t events_enabled;
	/** RTC running state */
	bool is_running;
};

extern const struct capi_rtc_ops max_capi_rtc_ops;

#endif /* MAXIM_CAPI_RTC_H_ */
