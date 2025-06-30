/***************************************************************************//**
 *   @file   maxim_rtc.h
 *   @brief  Header file of RTC driver.
 *   @author Francis Roi Manabat (francisroi.manabat@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following discl aimer.
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

#ifndef MAXIM_RTC_H_
#define MAXIM_RTC_H_

extern const struct no_os_rtc_platform_ops max_rtc_ops;

int32_t max_rtc_init(struct no_os_rtc_desc **device,
		     struct no_os_rtc_init_param *init_param);

int32_t max_rtc_remove(struct no_os_rtc_desc *dev);

int32_t max_rtc_start(struct no_os_rtc_desc *dev);

int32_t max_rtc_stop(struct no_os_rtc_desc *dev);

int32_t max_rtc_get_cnt(struct no_os_rtc_desc *dev, uint32_t *tmr_cnt);

int32_t max_rtc_set_cnt(struct no_os_rtc_desc *dev, uint32_t tmr_cnt);

int32_t max_rtc_set_irq_time(struct no_os_rtc_desc *dev, uint32_t irq_time);

#endif
