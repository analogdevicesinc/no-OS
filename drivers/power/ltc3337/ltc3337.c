/***************************************************************************//**
 *   @file   ltc3337.c
 *   @brief  Implementation file for the LTC3337 Driver
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
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include "ltc3337.h"
#include "no_os_alloc.h"


/**
 * Structure supporting the iPeak lookup table
 */
struct ipeak_entry_t {
	uint8_t  ipeak_ma;		//iPeak, in mA
	uint32_t qlsb_na_hr;	//qLsb, nA-hr component
};

/**
 * Lookup table for the iPeak values. This converts the 3-bit
 * iPeak input configuration signals, to a iPeak current and qLsb
 * values for Coulomb counter calculations
 */
static const struct ipeak_entry_t ipeak_lookup_table[LTC3337_NUM_IPEAKS] = {
	{ .ipeak_ma = 5,   .qlsb_na_hr = 745700 },   //b000 - 5ma, 745.7 uA-hr
	{ .ipeak_ma = 10,  .qlsb_na_hr = 1491000 },  //b001 - 10ma, 1.491 mA-hr
	{ .ipeak_ma = 15,  .qlsb_na_hr = 2237000 },  //b010 - 15ma, 2.237 mA-hr
	{ .ipeak_ma = 20,  .qlsb_na_hr = 2983000 },  //b011 - 20ma, 2.983 mA-hr
	{ .ipeak_ma = 25,  .qlsb_na_hr = 3728000 },  //b100 - 25ma, 3.728 mA-hr
	{ .ipeak_ma = 50,  .qlsb_na_hr = 7457000 },  //b101 - 50ma, 7.457 mA-hr
	{ .ipeak_ma = 75,  .qlsb_na_hr = 11180000 }, //b110 - 75ma, 11.18 mA-hr
	{ .ipeak_ma = 100, .qlsb_na_hr = 14910000 }, //b111 - 100ma, 14.91 mA-hr
};

/** Local Prototypes */
static int ltc3337_read_reg(struct ltc3337_dev* dev, uint8_t reg,
			    uint16_t* data);
static int ltc3337_write_reg(struct ltc3337_dev* dev, uint8_t reg,
			     uint16_t data);
static uint32_t ltc3337_vbat_to_mv(uint16_t raw_value);
static int16_t ltc3337_temp_reg_to_c(uint8_t raw_value);
static uint8_t ltc3337_temp_c_to_reg(int16_t temp_c);
static void ltc3337_charge_count_to_a(uint16_t charge_count, uint8_t prescale,
				      uint32_t lsb_na, struct charge_count_t* value);
static uint16_t ltc3337_a_to_charge_count(uint8_t prescale, uint32_t lsb_na,
		struct charge_count_t* charge_a);


/**
 * Initializes the driver instance. Assigns the I2C bus and reads the
 * IPK pins.  The prescaler is set to the user value, and the accumulated
 * charge alarm is defaulted to maximum 0xFF
 * @param device - Pointer to the device instance
 * @param init_param - Initialization parameters
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_init(struct ltc3337_dev** device,
		 struct ltc3337_init_param* init_param)
{
	int ret;
	uint16_t reg_val;
	struct ltc3337_dev *dev;

	//Input parameter NULL checks
	if((!device) || (!init_param))
		return -EINVAL;

	dev = (struct ltc3337_dev*)no_os_calloc(1, sizeof(struct ltc3337_dev));
	if(!dev)
		return  -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if(ret) //Failure creating I2C
		goto err;

	ret = ltc3337_read_reg(dev, LTC3337_REG_C, &reg_val);
	if(ret) //Failure on a I2C Read
		goto comm_err;

	//Success
	dev->ipeak_latched = no_os_field_get(LTC3337_RC_IPK_PIN_MSK, reg_val);
	reg_val = no_os_field_prep(LTC3337_RA_PRESCALE_MSK, init_param->prescale);
	reg_val |= no_os_field_prep(LTC3337_RA_ALARM_LVL_MSK,
				    LTC3337_OVERFLOW_ALARM_MAX);
	dev->latched_reg_a = reg_val;
	ret = ltc3337_write_reg(dev, LTC3337_REG_A, reg_val);
	*device = dev;

	return 0;

comm_err:
	no_os_i2c_remove(dev->i2c_desc);
err:
	no_os_free(dev);
	return ret;
}

/**
 * Uninitializes the device instance and frees up any allocated memory
 * @param dev - Device to remove
 * @returns 0 on success, non-0 on error
 */
int ltc3337_remove(struct ltc3337_dev* dev)
{
	int ret;

	//Input parameter NULL check
	if(!dev)
		return -EINVAL;

	ret = no_os_i2c_remove(dev->i2c_desc);
	no_os_free(dev);

	return ret;
}

/**
 * Gets the current value of the requested voltage monitor in mV.
 * @param dev - Device to read from
 * @param source - Which voltage source to read
 * @param value - Location to store calcualted value, in mV
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_get_voltage_mv(struct ltc3337_dev* dev,
			   enum ltc3337_voltage_src_t source,
			   uint32_t* value)
{
	uint8_t reg;
	uint16_t reg_value;
	int ret;

	//Input parameter NULL checks
	if((!dev) || (!value))
		return -EINVAL;

	switch(source) {
	case BAT_IN_IPEAK_ON:
		reg = LTC3337_REG_D;
		break;
	case BAT_IN_IPEAK_OFF:
		reg = LTC3337_REG_E;
		break;
	case BAT_OUT_IPEAK_ON:
		reg = LTC3337_REG_F;
		break;
	case BAT_OUT_IPEAK_OFF:
		reg = LTC3337_REG_G;
		break;
	default:
		//Invalid register
		return -EINVAL;
	}

	ret = ltc3337_read_reg(dev, reg, &reg_value);
	if(ret)
		return ret;

	reg_value = no_os_field_get(LTC3337_BATV_MSK, reg_value);
	*value = ltc3337_vbat_to_mv(reg_value);

	return 0;
}

/**
 * Gets the current die temperature
 * @param dev - Device to read from
 * @param value - Location to store calculated value, in Deg C
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_get_temperature_c(struct ltc3337_dev* dev, int16_t* value)
{
	uint16_t reg_value;
	int ret;

	//Input parameter NULL checks
	if((!dev) || (!value))
		return -EINVAL;

	ret = ltc3337_read_reg(dev, LTC3337_REG_C, &reg_value);
	if(ret)
		return ret;

	*value = ltc3337_temp_reg_to_c(
			 no_os_field_get(LTC3337_RC_DIE_TEMP_MSK, reg_value));

	return 0;
}

/**
 * Gets the current value of the accumulated charge register.  This function
 * can return the raw value and/or the calculated value based on the prescaler
 * and IPK settings of dev.
 * @param dev - Device to read from
 * @param value - Pointer to calculated storage location, or NULL
 * @param raw_value - Pointer to raw storage location, or NULL
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_get_accumulated_charge(struct ltc3337_dev* dev,
				   struct charge_count_t* value,
				   uint16_t* raw_value)
{
	uint16_t reg_value;
	uint8_t prescale;

	int ret;

	//Input parameter NULL check
	if(!dev)
		return -EINVAL;

	ret = ltc3337_read_reg(dev, LTC3337_REG_B, &reg_value);
	if(ret)
		return ret;

	if(raw_value)
		*raw_value = reg_value;

	if(value) {
		prescale = no_os_field_get(LTC3337_RA_PRESCALE_MSK, dev->latched_reg_a);
		ltc3337_charge_count_to_a(reg_value, prescale,
					  ipeak_lookup_table[dev->ipeak_latched].qlsb_na_hr,
					  value);
	}

	return 0;
}

/**
 * Sets the prescaler value of the device.
 * IMPORTANT: Changing the prescaler does not affect the value of the accumulated
 * charge register. If the prescaler is set sometime following the initial
 * runtime setup, after charge has been accumulated, the acccumulated charge
 * register should be set according to the current value shifted by the
 * difference in original and current prescaler. This driver does not
 * automatically perform that action.
 * IMPORTANT: Changing the prescaler does not alter the configuration of alarm.
 * That may need to be changed if configured prior to the prescaler
 * @param dev - Pointer to the device instance
 * @param prescale - Prescaler value
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_set_prescaler(struct ltc3337_dev* dev, uint8_t prescale)
{
	uint16_t reg_value;
	int ret;

	//Input parameter NULL check
	if(!dev)
		return -EINVAL;

	reg_value = dev->latched_reg_a;
	reg_value &= ~(LTC3337_RA_PRESCALE_MSK);
	reg_value |= no_os_field_prep(LTC3337_RA_PRESCALE_MSK, prescale);
	ret = ltc3337_write_reg(dev, LTC3337_REG_A, reg_value);
	if(ret)
		return ret;

	dev->latched_reg_a = reg_value;

	return 0;
}

/**
 * Set the alarm thresholds for hot and cold temperatures
 * @param dev - Device to configure
 * @param hot_alarm - Hot alarm threshold, in Degrees C
 * @param cold_alarm - Cold alarm threshold, in Degrees C
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_set_temperature_alarms_c(struct ltc3337_dev* dev, int16_t hot_alarm,
				     int16_t cold_alarm)
{
	uint16_t reg_val;
	uint8_t temp_val;

	//Input parameter NULL check
	if(!dev)
		return -EINVAL;

	if((hot_alarm < LTC3337_MIN_TEMP_C) || (hot_alarm > LTC3337_MAX_TEMP_C) ||
	    (cold_alarm < LTC3337_MIN_TEMP_C) || (cold_alarm > LTC3337_MAX_TEMP_C))
		return -EINVAL;

	temp_val = ltc3337_temp_c_to_reg(hot_alarm);
	reg_val = no_os_field_prep(LTC3337_RH_HOT_ALARM_MSK, temp_val);
	temp_val = ltc3337_temp_c_to_reg(cold_alarm);
	reg_val |= no_os_field_prep(LTC3337_RH_COLD_ALARM_MSK, temp_val);

	return ltc3337_write_reg(dev, LTC3337_REG_H, reg_val);
}

/**
 * Sets the state of the coulomb counter shutdown function
 * @param dev - Device to configure
 * @param shutdown_en - 1 - Enable shutdown, 0 - Coulomb counter running
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_set_counter_shutdown(struct ltc3337_dev* dev, uint8_t shutdown_en)
{
	uint16_t reg_value;
	int ret;

	//Input parameter NULL check
	if(!dev)
		return -EINVAL;

	reg_value = dev->latched_reg_a;

	if(shutdown_en)
		reg_value |= LTC3337_RA_SHTDN;
	else
		reg_value &= ~(LTC3337_RA_SHTDN);

	ret = ltc3337_write_reg(dev, LTC3337_REG_A, reg_value);
	if(ret)
		return ret;

	dev->latched_reg_a = reg_value;

	return 0;
}

/**
 * Sets the alarm level for the coulomb counter alarm. The input value is the
 * raw register value, and can be calculated using LTC3337_CalculateChargeRegister
 * or manually by hand.
 * IMPORTANT: Only the upper 8-bits of the value are utilized. Use the roundUp
 * flag to have the driver optionally round up to the next valid bit
 * @param dev - Device to configure
 * @param counter_reg_val - Register value, as if all 16-bits were utilized
 * @param round_up - 1 -Have the driver round up to next usable alarm bit
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_set_counter_alarm(struct ltc3337_dev* dev, uint16_t counter_reg_val,
			      uint8_t round_up)
{
	uint8_t data_to_set;
	uint16_t reg_value;
	int ret;

	//Input parameter NULL check
	if(!dev)
		return -EINVAL;

	reg_value = dev->latched_reg_a;

	//Only the upper 8 bits are utilized
	data_to_set = (counter_reg_val >> 8);

	//If rounding was requested, check the if any of the lower bits are set,
	//and if we wont overflow dataToSet beyond 8-bits, increment
	if((round_up) && (counter_reg_val & 0xFF) && (data_to_set != 0xFF))
		data_to_set++;

	reg_value &= ~(LTC3337_RA_ALARM_LVL_MSK);
	reg_value |= no_os_field_prep(LTC3337_RA_ALARM_LVL_MSK, data_to_set);

	ret = ltc3337_write_reg(dev, LTC3337_REG_A, reg_value);
	if(ret)
		return ret;

	dev->latched_reg_a = reg_value;

	return 0;
}

/**
 * Sets the level for the coulomb counter accumulated charge register.
 * The input value is the raw register value, and can be calculated using
 * LTC3337_CalculateChargeRegister or manually by hand.
 * IMPORTANT: Only the upper 8-bits of the value are utilized. Use the roundUp
 * flag to have the driver optionally round up to the next valid bit
 * @param dev - Device to configure
 * @param reg_value - Register value, as if all 16-bits were utilized
 * @param round_up - 1 -Have the driver round up to next usable alarm bit
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_set_accumulated_charge(struct ltc3337_dev* dev, uint16_t reg_value,
				   uint8_t round_up)
{
	uint16_t data_to_set;

	//Input parameter NULL check
	if(!dev)
		return -EINVAL;

	//Only upper 8-bits (MSB) are writable
	data_to_set = reg_value & 0xFF00;

	//If rounding up was requested, check if any of the lower 8-bits of the
	//original value are set, and verify dataToSet will not overflow 16-bits
	//if we increment
	//Add 1 to the MSB of the data to set to account for rounding
	//request
	if((round_up) && (reg_value & 0xFF) && (data_to_set != 0xFF00))
		data_to_set += 0x100;

	return ltc3337_write_reg(dev, LTC3337_REG_B, data_to_set);
}

/**
 * Reads and clears the interrupt status of the chip.  Since the die temp is
 * also provided in the interrupt register, temperature is optionally provided
 * as well. This allows a user to poll interrupt status and not accidentally
 * clear interrupts by reading temperature inbetween interrupt polls.
 * Use the LTC3337_RC_ macros to determine status of each interrupt in int_field
 * @param dev - Device to read from
 * @param int_field - Interrupt status bits
 * @param temp_c - Location to store temperature (or NULL)
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_get_and_clear_interrupts(struct ltc3337_dev* dev,
				     uint16_t* int_field,
				     int16_t* temp_c)
{
	uint16_t write_reg_value;
	int ret;

	//Input parameter NULL checks
	if((!dev) || (!int_field))
		return -EINVAL;

	ret = ltc3337_read_reg(dev, LTC3337_REG_C, int_field);
	if(ret)
		return ret;

	write_reg_value = dev->latched_reg_a;
	write_reg_value |= (LTC3337_RA_CLEAR_INT);
	ret = ltc3337_write_reg(dev, LTC3337_REG_A, write_reg_value);
	if(ret)
		return ret;

	if(temp_c)
		*temp_c = ltc3337_temp_reg_to_c(no_os_field_get(LTC3337_RC_DIE_TEMP_MSK,
						*int_field));

	return 0;
}

/**
 * Gets a register value equivalent of the provided charge in A/nA-hrs
 * to be used in configuring the charge alarms, and resetting the charge
 * register. This function does not access the device over the bus, but
 * utilizes the latched prescaled and IPK values to perform the calculation
 * @param dev - Device to utilize configuration from
 * @param charge_a - Pointer to the charge values in A/nA-hrs
 * @param reg_value - Pointer to store the output register value
 * @returns 0 on success, Non-0 on error
 */
int ltc3337_calculate_charge_register(struct ltc3337_dev* dev,
				      struct charge_count_t* charge_a,
				      uint16_t* reg_value)
{
	uint8_t prescale;

	//Input parameter NULL checks
	if((!dev) || (!charge_a) || (!reg_value))
		return -EINVAL;

	prescale = no_os_field_get(LTC3337_RA_PRESCALE_MSK, dev->latched_reg_a);
	*reg_value = ltc3337_a_to_charge_count(prescale,
					       ipeak_lookup_table[dev->ipeak_latched].qlsb_na_hr,
					       charge_a);

	return 0;
}

/**
 * Reads a register from the device.
 * @param dev - Device instance
 * @param reg - Register to read
 * @param data - Data storage location
 * @returns 0 on success, else on error
 */
static int ltc3337_read_reg(struct ltc3337_dev* dev, uint8_t reg,
			    uint16_t* data)
{
	uint8_t reg_buf = reg;
	uint8_t data_buf[2];
	int ret;

	//Input parameter NULL checks
	if((!dev) || (!data))
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg_buf, 1, 1);
	if(ret)
		return ret;

	//Successful write
	ret = no_os_i2c_read(dev->i2c_desc, data_buf, 2, 1);
	if(ret)
		return ret;

	//Data comes Little Endian
	*data = no_os_get_unaligned_le16(data_buf);

	return 0;
}

/**
 * Writes a register ont the device.
 * @param dev - Device instance
 * @param reg - Register to write
 * @param data - Data to write
 * @returns 0 on success, else on error
 */
static int ltc3337_write_reg(struct ltc3337_dev* dev, uint8_t reg,
			     uint16_t data)
{
	uint8_t data_buf[3];

	//Input parameter NULL check
	if(!dev)
		return -EINVAL;

	//Register followed by Little Endian 16-bit word
	data_buf[0] = reg;
	data_buf[1] = (uint8_t)(data & 0xFF);
	data_buf[2] = (uint8_t)((data >> 8) & 0xFF);

	return no_os_i2c_write(dev->i2c_desc, data_buf, 3, 1);
}

/**
 * Converts a raw register value for battery voltage to
 * millivolts
 * @param raw_value - Value to convert
 * @returns Converted value in millivolts
 */
static uint32_t ltc3337_vbat_to_mv(uint16_t raw_value)
{
	//uV to mV, rounded to nearest mV
	return NO_OS_DIV_ROUND_CLOSEST((uint32_t)raw_value * LTC3337_VLSB_UV,
				       LTC3337_UV_TO_MV_SCALE);
}

/**
 * Converts a raw register value for temperature to
 * Degrees C
 * @param raw_value - Value to convert
 * @returns Converted value in Degrees C
 */
static int16_t ltc3337_temp_reg_to_c(uint8_t raw_value)
{
	//Rounded to the nearest C
	return (int16_t)NO_OS_DIV_ROUND_CLOSEST((raw_value * LTC3337_TLSB_MC),
						LTC3337_MC_TO_C_SCALE) +
	       LTC3337_TEMP_MIN_C;
}

/**
 * Converts a temperature in Deg C to the parts register
 * equivalent
 * @param temp_c - Temperature in C
 * @returns Converted register value
*/
static uint8_t ltc3337_temp_c_to_reg(int16_t temp_c)
{
	int32_t working;

	if(temp_c > LTC3337_MAX_TEMP_C )
		return 0xFF;
	else if(temp_c < LTC3337_MIN_TEMP_C)
		return 0x00;
	else {
		working = (temp_c - LTC3337_TEMP_MIN_C) * LTC3337_MC_TO_C_SCALE;
		//Round to the nearest
		working = NO_OS_DIV_ROUND_CLOSEST(working, LTC3337_TLSB_MC);
		return (uint8_t)working;
	}
}

/**
 * Converts a charge count register to accumulated charge in
 * A/nA based on the prescaler qLSB values
 * @param charge_count - Charge count register value
 * @param prescale - Prescale value
 * @param lsb_na - LSB value in Nano-amps
 * @param ret_value - Pointer to return structure
  */
static void ltc3337_charge_count_to_a(uint16_t charge_count, uint8_t prescale,
				      uint32_t lsb_na,       struct charge_count_t* ret_value)
{
	uint32_t qlsb_m_na;
	uint32_t qlsb_m_remainder;
	uint32_t whole_count, fract_count;

	//Per the data sheet qLSB_M = (qLSB / 2*M)
	//We can get qLSB_M by doing (qLSB >> M);
	qlsb_m_na = (lsb_na >> prescale);

	//Get the fractional bits we shifted out
	qlsb_m_remainder = lsb_na & ((1 << prescale) - 1);

	//In an effort to keep all the math 32-bit integer based, avoiding floats,
	//to be portable across differnt systems:
	//Let's convert our qlsb_m_nA to whole and fractional parts
	//This is to prevent an overflow during the math.
	//Worst case is 14910000 nA with a prescaler of 0. If we had a full counter
	//(0xFFFF), multiplication works out to 0xE3_814C_7DD0, 32-bit overflow.
	//Using base-2 components is easier programming wise, but using base-10
	//helps track the units better:
	//Divisor of 10,000 (10uA per whole value, 1nA per fract Value)
	//  - 14910000 / 10000 = 1491 * 0xFFFF (full counter) = 0x5D2_FA2D
	//  - 9,999 (Wost case remainder) * 0xFFFF = 0x270E_D8F1
	whole_count = (uint32_t)charge_count * (qlsb_m_na / LTC3337_CALC_SCALE);
	fract_count = (uint32_t)charge_count * (qlsb_m_na % LTC3337_CALC_SCALE);

	ret_value->a_hr = whole_count /
			  LTC3337_CALC_TO_WHOLE; //10uA per bit,  / 100,000 to get A
	ret_value->na_hr = (whole_count % LTC3337_CALC_TO_WHOLE) *
			   LTC3337_CALC_SCALE; //*10,000 nA per bit
	ret_value->na_hr += fract_count; //Add in the other nA's

	//Add back in the remainder after shifting out the prescaler
	//Each LSB is 1/2^M nA
	fract_count = qlsb_m_remainder * charge_count;
	ret_value->na_hr += fract_count >> prescale;

	//If the nA-hr component overflowed 1A, adjust accordingly
	while(ret_value->na_hr > LTC3337_NANO_AMP) {
		ret_value->a_hr++;
		ret_value->na_hr -= LTC3337_NANO_AMP;
	}
}

/**
 * Converts a charge in A/nA-hr to an equivalent count register
 * value to be used for setting alarms and clearing the register
 * @param prescale - Prescale value
 * @param lsb_na - LSB value in Nano-amps
 * @param charge_a - Pointer to the charge value in A/nA-hrs
 * @returns Equivalent register value based on parameters
 */
static uint16_t ltc3337_a_to_charge_count(uint8_t prescale, uint32_t lsb_na,
		struct charge_count_t* charge_a)
{
	uint32_t qlsb_m_na;
	uint32_t qlsb_m_remainder;
	uint32_t bits_per_a;
	uint32_t extra_per_a;
	uint32_t working_count;
	uint32_t remainder_count;
	uint32_t fract_count;

	//Per the data sheet qLSB_M = (qLSB / 2*M)
	//We can get qLSB_M by doing (qLSB >> M);
	qlsb_m_na = (lsb_na >> prescale);

	//Get the fractional bits we shifted out
	qlsb_m_remainder = lsb_na & ((1 << prescale) - 1);

	//Calculate the number of bits for each A-hr
	bits_per_a = LTC3337_NANO_AMP / qlsb_m_na;
	extra_per_a = LTC3337_NANO_AMP % qlsb_m_na;

	//Exceeds the bounds of the configuration, and could overflow
	//the math.  Drop out now.
	if(charge_a->a_hr > (UINT16_MAX / bits_per_a))
		return UINT16_MAX;

	//Get the baseline count
	working_count = (charge_a->a_hr * bits_per_a);
	working_count += (charge_a->na_hr / qlsb_m_na);

	//Accumulate any remainders from the math, and add extra bits
	//if we overflow qlsb_m_nA
	remainder_count = (charge_a->a_hr * extra_per_a);
	remainder_count += (charge_a->na_hr % qlsb_m_na);
	working_count += (remainder_count / qlsb_m_na);

	//Incorporate the fractional piece from the prescaler
	//Since these fractional pieces would be more divising components
	//We'll subtract the result
	fract_count = working_count * qlsb_m_remainder;
	working_count -= ((fract_count /
			   lsb_na)); //divide by lsbna to negate the prescaler

	if(working_count > UINT16_MAX) //Clamp to register max
		return UINT16_MAX;
	else
		return working_count;
}
