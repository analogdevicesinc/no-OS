/*******************************************************************************
 *   @file   rtc_config.c
 *   @brief  RTC interfacing functionalities
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 *OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 *DAMAGE.
 *******************************************************************************/

#include "rtc_config.h"

int rtc_init(void)
{
	int status = 0;

	pqlibExample.startLog = false;

	pqlibExample.logStartTimeMillisec
		= convert_string_to_ms(pqlibExample.logStartTime);
	pqlibExample.logStopTimeMillisec
		= convert_string_to_ms(pqlibExample.logStopTime);

	status = max31343_init(&rtcDevice, max31343_rtc_ini_param);
	if (status)
		return status;

	rtcIntrpTimeInMilliSeconds = read_time_in_ms();

	return status;
}

int rtc_remove()
{
	int ret = 0;
	if (rtcDevice)
		ret = max31343_remove(rtcDevice);

	return ret;
}

int rtc_set_time_in_ms(uint64_t milliSeconds)
{
	int status = 0;
	time_t unixEpoch = milliSeconds / 1000;
	struct tm timeYMD = *localtime(&unixEpoch);
	struct max31343_time_stamp date;
	extern volatile uint8_t newSyncTimeAvailable;
	date.year = timeYMD.tm_year;
	date.mon = timeYMD.tm_mon;
	date.day = timeYMD.tm_mday;
	date.hr = timeYMD.tm_hour;
	date.min = timeYMD.tm_min;
	date.sec = timeYMD.tm_sec;

	if (timeYMD.tm_year < 100) {
		status = 1;
	} else {
		status = max31343_set_time_stamp(rtcDevice, date);
		if (status == 0) {
			rtcIntrpTimeInMilliSeconds = read_time_in_ms();
			newSyncTimeAvailable = 1;
		}
	}

	return status;
}

uint64_t read_time_in_ms(void)
{
	int status = 0;
	uint64_t timeInMilliSeconds = 0;
	struct max31343_time_stamp date;

	status = max31343_reg_read_time_stamp(rtcDevice, &date);
	if (!status)
		timeInMilliSeconds = convert_time_stamp_to_ms(date);

	return timeInMilliSeconds;
}

uint64_t convert_time_stamp_to_ms(struct max31343_time_stamp date)
{
	uint64_t timeInMilliSeconds;
	time_t unixEpoch = 0;
	struct tm timeYMD;
	timeYMD.tm_sec = date.sec;
	timeYMD.tm_min = date.min;
	timeYMD.tm_hour = date.hr;
	timeYMD.tm_mon = date.mon;
	timeYMD.tm_mday = date.day;
	timeYMD.tm_year = date.year;
	timeYMD.tm_isdst = -1;
	unixEpoch = mktime(&timeYMD);

	timeInMilliSeconds = (uint64_t)unixEpoch * SECONDS_TO_MILLISECONDS;
	return timeInMilliSeconds;
}

uint64_t convert_string_to_ms(char timeStamp[])
{
	uint64_t timeInMilliSeconds;
	time_t unixEpoch = 0;
	struct tm timeYMD;

	char tmp[4];
	memset (tmp, '0', 4);
	memcpy (tmp, &timeStamp[0], 4);
	timeYMD.tm_year = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &timeStamp[4], 2);
	timeYMD.tm_mon = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &timeStamp[6], 2);
	timeYMD.tm_mday = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &timeStamp[8], 2);
	timeYMD.tm_hour = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &timeStamp[10], 2);
	timeYMD.tm_min = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &timeStamp[12], 2);
	timeYMD.tm_sec = atoi (tmp);
	timeYMD.tm_isdst = -1;
	unixEpoch = mktime (&timeYMD);

	timeInMilliSeconds = (uint64_t)unixEpoch * SECONDS_TO_MILLISECONDS;
	return timeInMilliSeconds;
}

int set_rtc_time()
{
	struct max31343_time_stamp date;
	char tmp[4];
	memset (tmp, '0', 4);
	memcpy (tmp, &pqlibExample.systemTimeStr[0], 4);
	date.year = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &pqlibExample.systemTimeStr[4], 2);
	date.mon = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &pqlibExample.systemTimeStr[6], 2);
	date.day = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &pqlibExample.systemTimeStr[8], 2);
	date.hr = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &pqlibExample.systemTimeStr[10], 2);
	date.min = atoi (tmp);
	memset (tmp, '0', 4);
	memcpy (&tmp[2], &pqlibExample.systemTimeStr[12], 2);
	date.sec = atoi (tmp);

	return max31343_set_time_stamp(rtcDevice, date);
}