/***************************************************************************//**
 *   @file   dummy_example.c
 *   @brief  DUMMY example code for adt7420-pmdz project
 *   @author RNechita (ramona.nechita@analog.com)
 *   @author Jamila Macagba (Jamila.Macagba@analog.com)
 *   @author Rene Arthur Necesito (Renearthur.Necesito@analog.com)
********************************************************************************
 * Copyright 2022, 2025(c) Analog Devices, Inc.
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

#include "common_data.h"
#include "adt7420.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

#ifdef LINUX_PLATFORM
#include <time.h>
#include <sys/time.h>
struct timeval tv;
struct tm *timeinfo;
char time_string[9];  // hh:mm:ss\0
#endif  // LINUX_PLATFORM

#define ADT7320_L8		NO_OS_BIT(8)
#define ADT7320_L16		NO_OS_BIT(16)

#define ADT7320_LEN_MSK		NO_OS_GENMASK(16, 8)
#define ADT7320_ADDR_MSK	NO_OS_GENMASK(15,0)

#define REG_THAT_IS_16		(ADT7320_L16 | ADT7320_REG_T_HIGH)
#define REG_OTH_IS_16		(ADT7320_L16 | ADT7320_REG_T_LOW)
#define REG_THAT_IS_8		ADT7320_REG_ID
#define REG_OTH_IS_8		ADT7320_REG_ID

/***************************************************************************//**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct adt7420_dev *adt7420;
	uint16_t temp_max = 0, temp_min = 0;
	uint16_t temp_msb_l = 0, temp_lsb_l = 0;
	float temp_c_max, temp_c_min;
	float temp_now;
	int ret;

#ifndef LINUX_PLATFORM
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart);
#endif // LINUX_PLATFORM

	ret = adt7420_init(&adt7420, adt7420_user_init);
	if (ret)
		goto error;
	ret = adt7420_reset(adt7420);
	if (ret)
		goto error_adt7420;

	/* Datasheet specified delay between conversions. */
	no_os_mdelay(240);

	while (1) {
		ret = adt7420_reg_read(adt7420, ADT7420_REG_T_HIGH_MSB, &temp_msb_l);
		if (ret)
			goto error_adt7420;
		ret = adt7420_reg_read(adt7420, ADT7420_REG_T_HIGH_LSB, &temp_lsb_l);
		if (ret)
			goto error_adt7420;
		temp_max = (((uint8_t)temp_msb_l) << 8) | ((uint8_t)temp_lsb_l);

		ret = adt7420_reg_read(adt7420, ADT7420_REG_T_LOW_MSB, &temp_msb_l);
		if (ret)
			goto error_adt7420;
		ret = adt7420_reg_read(adt7420, ADT7420_REG_T_LOW_LSB, &temp_lsb_l);
		if (ret)
			goto error_adt7420;
		temp_min = (temp_msb_l << 8) | temp_lsb_l;

		if (adt7420->resolution_setting) {
			if (temp_max & ADT7420_16BIT_SIGN) {
				/*! Negative temperature */
				temp_c_max = (float)((int32_t)temp_max - ADT7420_16BIT_NEG) / ADT7420_16BIT_DIV;
				temp_c_min = (float)((int32_t)temp_min - ADT7420_16BIT_NEG) / ADT7420_16BIT_DIV;
			} else {
				/*! Positive temperature */
				temp_c_max = (float)temp_max / ADT7420_16BIT_DIV;
				temp_c_min = (float)temp_min / ADT7420_16BIT_DIV;
			}
		} else {
			temp_max >>= 3;
			temp_min >>= 3;
			if (temp_max & ADT7420_13BIT_SIGN) {
				/*! Negative temperature */
				temp_c_max = (float)((int32_t)temp_max - ADT7420_13BIT_NEG) / ADT7420_13BIT_DIV;
				temp_c_min = (float)((int32_t)temp_min - ADT7420_13BIT_NEG) / ADT7420_13BIT_DIV;
			} else {
				/*! Positive temperature */
				temp_c_max = (float)temp_max / ADT7420_13BIT_DIV;
				temp_c_min = (float)temp_min / ADT7420_13BIT_DIV;
			}
		}

		temp_now = adt7420_get_temperature(adt7420);
		uint32_t hyst = 5;
		ret = adt7420_reg_write(adt7420, ADT7420_REG_HIST, hyst);
		if (ret)
			goto error_adt7420;
		uint32_t readval = 0;
		ret = adt7420_reg_read(adt7420, ADT7420_REG_HIST, &readval);
		if (ret)
			goto error_adt7420;
#ifdef LINUX_PLATFORM
		gettimeofday(&tv, NULL);
		timeinfo = localtime(&tv.tv_sec);
		strftime(time_string, sizeof(time_string), "%H:%M:%S", timeinfo);
		printf("[%lu.%lu - %s]\n", tv.tv_sec, tv.tv_usec, time_string);
#endif  // LINUX_PLATFORM
		printf("The value read from the hist register is %d\n", readval);
		pr_info("Temp read is %lf\r\n", temp_now);
		pr_info("Current temp high setpoint is %d\r\n", (int) temp_c_max);
		pr_info("Current temp low setpoint is %d\r\n", (int) temp_c_min);
		no_os_mdelay(3000);
	}
error_adt7420:
	adt7420_remove(adt7420);
error:
	pr_info("Error!\r\n");
	return 0;
}
