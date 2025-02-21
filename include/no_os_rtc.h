/***************************************************************************//**
 *   @file   no_os_rtc.h
 *   @brief  Header of the RTC interface
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef _NO_OS_RTC_H_
#define _NO_OS_RTC_H_

#include <stdint.h>

/**
 * @struct no_os_rtc_desc
 * @brief Structure holding RTC descriptor.
 */
struct no_os_rtc_desc {
	/** ID of the real time clock core. */
	uint8_t id;
	/** Frequency of the RTC */
	uint32_t freq;
	/** Load value of the RTC */
	uint32_t load;
	/** Device specific RTC parameters. */
	void *extra;
};

/**
 * @struct no_os_rtc_init_param
 * @brief Structure holding RTC initialization parameters.
 */
struct no_os_rtc_init_param {
	/** ID of the real time clock core. */
	uint8_t id;
	/** Frequency of the RTC */
	uint32_t freq;
	/** Load value of the RTC */
	uint32_t load;
	/** Device specific RTC parameters. */
	void *extra;
};

/** Initialize the RTC peripheral. */
int32_t no_os_rtc_init(struct no_os_rtc_desc **device,
		       struct no_os_rtc_init_param *init_param);

/** Free the resources allocated by no_os_rtc_init(). */
int32_t no_os_rtc_remove(struct no_os_rtc_desc *dev);

/** Start the real time clock. */
int32_t no_os_rtc_start(struct no_os_rtc_desc *dev);

/** Stop the real time clock. */
int32_t no_os_rtc_stop(struct no_os_rtc_desc *dev);

/** Get the current count for the real time clock. */
int32_t no_os_rtc_get_cnt(struct no_os_rtc_desc *dev, uint32_t *tmr_cnt);

/** Set the current count for the real time clock. */
int32_t no_os_rtc_set_cnt(struct no_os_rtc_desc *dev, uint32_t tmr_cnt);

/** Set the time at which an interrupt will occur */
int32_t no_os_rtc_set_irq_time(struct no_os_rtc_desc *dev, uint32_t irq_time);

#endif // _NO_OS_RTC_H_
