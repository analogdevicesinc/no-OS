/***************************************************************************//**
 *   @file   dummy_example.c
 *   @brief  DUMMY example code for adt7420-pmdz project
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "dummy_example.h"
#include "common_data.h"
#include "adt7420.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADT7320_L8		NO_OS_BIT(8)
#define ADT7320_L16		NO_OS_BIT(16)

#define ADT7320_LEN_MSK		NO_OS_GENMASK(16, 8)
#define ADT7320_ADDR_MSK	NO_OS_GENMASK(15,0)

#define REG_THAT_IS_16		(ADT7320_L16 | ADT7320_REG_T_HIGH)
#define REG_OTH_IS_16		(ADT7320_L16 | ADT7320_REG_T_LOW)
#define REG_THAT_IS_8		ADT7320_REG_ID
#define REG_OTH_IS_8		ADT7320_REG_ID

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int dummy_example_main()
{
	struct adt7420_dev *adt7420;
	uint16_t temp_max = 0, temp_min = 0;
	uint16_t temp_msb_l = 0, temp_lsb_l = 0;
	float temp_c_max, temp_c_min;
	float temp_now;
	int ret;

	ret = adt7420_init(&adt7420, adt7420_user_init);
	if (ret)
		goto error;
	ret = adt7420_reset(adt7420);
	if (ret)
		goto error_adt7420;

	/* Datasheet specified delay between conversions. */
	no_os_mdelay(240);

	while(1) {
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

		if(adt7420->resolution_setting) {
			if(temp_max & ADT7420_16BIT_SIGN) {
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
			temp_min >>=3;
			if(temp_max & ADT7420_13BIT_SIGN) {
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
