/********************************************************************************
 *   @file   rtc_max31343.h
 *   @brief  MAX31343 RTC driver header file
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

#ifndef ZEPHYR_DRIVERS_RTC_RTC_MAX31343_H
#define ZEPHYR_DRIVERS_RTC_RTC_MAX31343_H

#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>

/* Core registers */
#define MAX31343_STATUS          0x00u
#define MAX31343_INT_EN          0x01u
#define MAX31343_RTC_RESET       0x02u
#define MAX31343_CFG1            0x03u
#define MAX31343_CFG2            0x04u
#define MAX31343_TIMER_CONFIG    0x05u

/* Time registers (BCD encoded) */
#define MAX31343_SECONDS         0x06u
#define MAX31343_MINUTES         0x07u
#define MAX31343_HOURS           0x08u
#define MAX31343_DAY             0x09u
#define MAX31343_DATE            0x0Au
#define MAX31343_MONTH           0x0Bu
#define MAX31343_YEAR            0x0Cu

/* RTC RESET register bits */
#define MAX31343_SWRST           BIT(0)

/* CFG1 register bits */
#define MAX31343_CFG1_ENOSC      BIT(1)

/* INT_EN register bits */
#define MAX31343_INT_EN_A1IE     BIT(0)
#define MAX31343_INT_EN_A2IE     BIT(1)
#define MAX31343_INT_EN_TIE      BIT(2)

/* BCD field masks (same encoding as MAX31331) */
#define MAX31343_SECONDS_MASK    (GENMASK(6, 4) | GENMASK(3, 0))  /* 0x7F */
#define MAX31343_MINUTES_MASK    (GENMASK(6, 4) | GENMASK(3, 0))  /* 0x7F */
#define MAX31343_HOURS_MASK      (BIT(5) | BIT(4) | GENMASK(3, 0)) /* 0x3F */
#define MAX31343_DAY_MASK        GENMASK(2, 0)                     /* 0x07 */
#define MAX31343_DATE_MASK       (GENMASK(5, 4) | GENMASK(3, 0))  /* 0x3F */
#define MAX31343_MONTH_MASK      (BIT(4) | GENMASK(3, 0))         /* 0x1F */
#define MAX31343_YEAR_MASK       (GENMASK(7, 4) | GENMASK(3, 0))  /* 0xFF */

/* Century bit in MONTH register */
#define MAX31343_CENTURY_MASK    BIT(7)

/* Day register: MAX31343 stores 1-7, Zephyr rtc_time tm_wday is 0-6 */
#define MAX31343_DAY_OFFSET      (-1)

/* Century boundary: tm_year >= 100 means year >= 2000, set century bit.
 * Same logic as MAX31331: century=1 for 2000-2099, century=0 for 1900-1999 */
#define MAX31343_YEAR_2000       (100)

/* CFG2 register bits (square wave / clock output) */
#define MAX31343_CFG2_SQW_HZ_MASK  GENMASK(2, 0)  /* SQW freq on INT/SQW pin */
#define MAX31343_CFG2_ENCLKO       BIT(7)          /* Enable CLKO output */
/* SQW_HZ values: 000=1Hz, 001=4.096kHz, 010=8.192kHz, 011=32.768kHz */

/* Supported time fields for validation */
#define MAX31343_RTC_TIME_MASK                                               \
	(RTC_ALARM_TIME_MASK_SECOND | RTC_ALARM_TIME_MASK_MINUTE |           \
	 RTC_ALARM_TIME_MASK_HOUR | RTC_ALARM_TIME_MASK_MONTH |             \
	 RTC_ALARM_TIME_MASK_MONTHDAY | RTC_ALARM_TIME_MASK_YEAR |          \
	 RTC_ALARM_TIME_MASK_WEEKDAY)

/**
 * @brief Prepare RTC for phase-aligned sync.
 *
 * Stops oscillator, resets prescaler/divider chain via SWRST,
 * re-configures essentials (IRQ off, SQW 1Hz), and writes the
 * target time. Oscillator remains stopped — call
 * rtc_max31343_start_oscillator() at the precise moment.
 */
int rtc_max31343_prepare_phase_sync(const struct device *dev,
				    const struct rtc_time *timeptr);

/**
 * @brief Start RTC oscillator.
 *
 * Enables ENOSC via direct register write (no read-modify-write).
 */
int rtc_max31343_start_oscillator(const struct device *dev);

/**
 * @brief Execute phase-aligned sync at the PTP second boundary.
 *
 * MUST be called at the exact PTP second boundary (after busy-wait).
 * Does SWRST to zero dividers, immediately re-enables oscillator
 * (crystal was running → off for only ~500µs → instant restart),
 * then writes config and time registers (non-critical).
 *
 * Replaces the old prepare_phase_sync() + busy-wait + start_oscillator()
 * pattern which killed the crystal for 1+ second causing 30ms phase error.
 */
int rtc_max31343_trigger_phase_sync(const struct device *dev,
				    const struct rtc_time *timeptr);

#endif /* ZEPHYR_DRIVERS_RTC_RTC_MAX31343_H */
