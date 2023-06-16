/***************************************************************************//**
 *   @file   ltc3337.h
 *   @brief  Header file for the LTC3337 Driver
 *   @author Brent Kowal (brent.kowal@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef __LTC3337_H__
#define __LTC3337_H__

#include <stdint.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

#define LTC3337_I2C_ADDR			0x64 // b1100100[r/w] 0xC8, 0xC9

#define LTC3337_MAX_TEMP_C			159  //Maximum temperature for alarms
#define LTC3337_MIN_TEMP_C			-41  //Minimum temperature for alarms
#define LTC3337_MAX_PRESCALE		15   //Maximum prescaler value

#define LTC3337_OVERFLOW_ALARM_MAX	0xFF //Max value for overflow alarm

#define LTC3337_REG_A	0x01 //Prescaler Select, IRQ Clear, Shutdown, Alarm Thresh

#define LTC3337_RA_PRESCALE_MSK		NO_OS_GENMASK(3, 0)
#define LTC3337_RA_CLEAR_INT		NO_OS_BIT(4)
#define LTC3337_RA_CNT_CHK		NO_OS_BIT(5)
#define LTC3337_RA_SHTDN		NO_OS_BIT(6)
#define LTC3337_RA_ADC_CONV		NO_OS_BIT(7)
#define LTC3337_RA_ALARM_LVL_MSK	NO_OS_GENMASK(15, 8)

#define LTC3337_REG_B	0x02 //Accumulated Charge

#define LTC3337_REG_C	0x03 //Status, Temperature
//Use the following definitions for derterming interrupt status
#define LTC3337_RC_OVERFLOW			NO_OS_BIT(0)
#define LTC3337_RC_ALARM_TRIP		NO_OS_BIT(1)
#define LTC3337_RC_ALARM_MIN		NO_OS_BIT(2)
#define LTC3337_RC_ALARM_MAX		NO_OS_BIT(3)
#define LTC3337_RC_ADC_READY		NO_OS_BIT(4)
#define LTC3337_RC_IPK_PIN_MSK		NO_OS_GENMASK(7, 5)
#define LTC3337_RC_DIE_TEMP_MSK		NO_OS_GENMASK(15, 8)

#define LTC3337_REG_D	0x04 //BAT_IN V, Ipeak On
#define LTC3337_REG_E	0x05 //BAT_IN V, Ipeak Off
#define LTC3337_REG_F	0x06 //BAT_OUT V, Ipeak On
#define LTC3337_REG_G	0x07 //BAT_OUT V, Ipeak Off
#define LTC3337_BATV_MSK		NO_OS_GENMASK(11, 0)

#define LTC3337_REG_H	0x08 //Temp Alarm Config
#define LTC3337_RH_HOT_ALARM_MSK	NO_OS_GENMASK(15, 8)
#define LTC3337_RH_COLD_ALARM_MSK	NO_OS_GENMASK(7, 0)

#define LTC3337_VLSB_UV		1465 //1.465 mV = 1465 uV
#define LTC3337_TLSB_MC		784  //0.784 Deg C = 784 mC
#define LTC3337_TEMP_MIN_C	-41

#define LTC3337_NUM_IPEAKS	8 //3-bit iPeak Input

//Scale values used in integer match to calulate A/nA-hrs from counters
#define LTC3337_NANO_AMP	1000000000
#define LTC3337_CALC_SCALE	10000
#define LTC3337_CALC_TO_WHOLE	(LTC3337_NANO_AMP / LTC3337_CALC_SCALE)

//Scale value for converting uV to mV
#define LTC3337_UV_TO_MV_SCALE	1000

//Scale Value for converting mC to Deg C
#define LTC3337_MC_TO_C_SCALE	1000

/**
 * Instance structure for the LTC3337 instance. Storage should be
 * allocated in user's code. Fields should be used by the driver only,
 * and not read or manipulated by the user
 */
struct ltc3337_dev {
	struct no_os_i2c_desc *i2c_desc;	//I2C device
	uint8_t ipeak_latched;				//iPeak value read at init
	uint16_t latched_reg_a;				//Latched Register A value
};

/**
 * Initialization parameters for the device
 */
struct ltc3337_init_param {
	struct no_os_i2c_init_param i2c_init;	//I2C Configuration
	uint8_t prescale;						//Initial device prescaler value
};

/**
 * Structure for holding the accumulated charge calculation
 */
struct charge_count_t {
	uint32_t a_hr;	//A-hrs
	uint32_t na_hr;	//nA-hrs
};

/**
 * Enumeration of sources for reading voltage
 */
enum ltc3337_voltage_src_t {
	BAT_IN_IPEAK_ON,
	BAT_IN_IPEAK_OFF,
	BAT_OUT_IPEAK_ON,
	BAT_OUT_IPEAK_OFF
};

/* Initializes the device instance */
int ltc3337_init(struct ltc3337_dev** dev,
		 struct ltc3337_init_param* init_param);

/* Removes the device instance */
int ltc3337_remove( struct ltc3337_dev* dev);

/* Sets the device prescaler value */
int ltc3337_set_prescaler(struct ltc3337_dev* dev, uint8_t prescale);

/* Sets the temperature alarms, in Deg C */
int ltc3337_set_temperature_alarms_c(struct ltc3337_dev* dev, int16_t hot_alarm,
				     int16_t cold_alarm);

/* Enabled / Disables the Coulomb counter shutdown */
int ltc3337_set_counter_shutdown(struct ltc3337_dev* dev, uint8_t shutdown_en);

/* Sets the couloumb counter alarm threshold */
int ltc3337_set_counter_alarm(struct ltc3337_dev* dev, uint16_t reg_value,
			      uint8_t round_up);

/* Manually sets the accumulated charge register */
int ltc3337_set_accumulated_charge(struct ltc3337_dev* dev, uint16_t reg_value,
				   uint8_t round_up);

/* Gets the current value of the accumulated charge register */
int ltc3337_get_accumulated_charge(struct ltc3337_dev* dev,
				   struct charge_count_t* value,
				   uint16_t* raw_value);

/* Reads the specified voltage source, in millivolts */
int ltc3337_get_voltage_mv(struct ltc3337_dev* dev,
			   enum ltc3337_voltage_src_t source,
			   uint32_t* value);

/* Gets the current die temperature, in Deg C */
int ltc3337_get_temperature_c(struct ltc3337_dev* dev, int16_t* value);

/* Gets the device interrupt status, and clears interrupts */
int ltc3337_get_and_clear_interrupts(struct ltc3337_dev* dev,
				     uint16_t* int_field,
				     int16_t* temp_c);

/* Calculates a charge register value based on A-Hr units */
int ltc3337_calculate_charge_register(struct ltc3337_dev* dev,
				      struct charge_count_t* charge_a,
				      uint16_t* reg_value);

#endif
