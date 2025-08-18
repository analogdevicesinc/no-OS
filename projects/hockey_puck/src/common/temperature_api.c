/****************************************************************************//*
 *   @file   hockey_puck_api.c
 *   @brief  Source file for Hockey Puck application program interface (API).
 *   @author Kent Libetario (Kent.Libetario@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <stdio.h>
#include <stdint.h>
#include "temperature_api.h"

/**
 * @brief Linearization of RTD resistance using the callendar van dusen method.
 * @param rtd_value - RTD resistance value to be linearized.
 * @return Converted temperature value result.
 */
static float temp_callendar_van_dusen(float rtd_value)
{
	float result;

	if(rtd_value < TEMP_RTD_RESISTANCE)
	{
	/* 
		Linearization from -200 degC to 0 degC:
		t(C) = -242.02 + (2.2228 * RTD) + ((2.5859 * 10^-3) * RTD^2) 
               - ((4.8260 * 10^-6) * RTD^3) - ((2.8183 * 10^-8) * RTD^4)
               + ((1.5243 * 10^-10) * RTD^5)
	*/
		result = -242.02f + (2.2228f * rtd_value) 
                 + (0.0025859f * powf(rtd_value, 2.0f))
                 - (0.0000048260f * powf(rtd_value, 3.0f)) 
                 - (0.000000028183f * powf(rtd_value, 4.0f))
                 + (0.00000000015243f * powf(rtd_value, 5.0f));
	}
	else
	{
	/*
		Linearization from 0 degC to 661 degC:
		t(C) = (-(3.9083 * 10^-3) + sqrt((3.9083 * 10^-3)^2
             - (4 * -5.775 * 10^-7) * (1 - (RTD / 100)))) / (2 * -5.775 * 10^-7)
	*/
		result = (-0.0039083f
                  + sqrtf((0.00001527480889f - (-0.00000231f
                  * (1 - (rtd_value / 100.0f)))))) / (-0.000001155f);
	}


    return result;
}

/**
 * @brief Linearization of RTD resistance using the approximation method.
 * @param rtd_value - RTD resistance value to be linearized.
 * @return Converted temperature value result.
 */
static float temp_approximation(float rtd_value)
{
    return ((rtd_value - 100.0f) / 0.385f);
}

/**
 * @brief Linearization of RTD resistance using the lookup table method.
 * @param table - The RTD resistance-temperature lookup table.
 * @param table_size - The size of the reference lookup table.
 * @param rtd_value - RTD resistance value to be linearized.
 * @return Converted temperature value result.
 */
static float temp_lookup_table(const temp_table_t *table, int32_t table_size,
                               float rtd_value)
{
    static uint32_t index;
    float m_slope, rtd_temp;

	/* Search and look for the two points in the lookup table. */
	index = (uint32_t)((table_size - 1)
                       * (1 - ((table[table_size - 1].x - rtd_value)
                               / (table[table_size - 1].x - table[0].x))));

    while((index < table_size) && (rtd_value > table[index].x))
    {
		index++;
    }

	/*
	The point-slope formula to extrapolate the temperature value:
			  y0 - y1
		m = ----------- ,  y = m * (x - x1) + y1
			  x0 - x1
	where:	x0 and x1 = Resistance up and down values, respectively
			y0 and y1 = Temperature up and down values, respectively
			x = RTD resistance value (rtd_value)
			y = RTD temperature value (rtd_temp)
	*/
    if (index <= 0)
    {
        rtd_temp = table[index].y;
    }
    else if (index >= table_size)
    {
        rtd_temp = table[index - 1].y;
    }
	else
	{
		m_slope = (table[index].y - table[index - 1].y)
                / (table[index].x - table[index-1].x);
		rtd_temp = (m_slope * (rtd_value - table[index].x)) + table[index].y;
	}

    return rtd_temp;
}


/****************************** ADC LSB Conversion ****************************/


/**
 * @brief Convert LSB value to resistance.
 * @param lsb_value - ADC LSB raw value.
 * @param gain_value - Gain applied in LSB.
 * @return Temperature resistance result.
 */
static float temp_lsb_raw_to_resistance(uint32_t lsb_value, int32_t gain_value)
{
	/*
		Rrtd = LSB * Rref / 2^n / gain,
            where: Rrtd = Vrtd / Iexc
                   Rref = Vref / Iexc
	*/
	return lsb_value * TEMP_REF_RESISTANCE / (TEMP_ADC_RESOLUTION * gain_value);
}

/**
 * @brief Convert LSB value to kelvin.
 * @param lsb_value - ADC LSB raw value.
 * @param gain_value - Gain applied in LSB.
 * @return Temperature kelvin result.
 */
static float temp_lsb_raw_to_kelvin(uint32_t lsb_value, int32_t gain_value)
{
	/*
		K = ((RTD - 100) / 0.385) + 274.15,
            where: RTD = LSB * Rref / 2^n / gain
	*/
	return ((temp_lsb_raw_to_resistance(lsb_value, gain_value) - 100.0f)
            / 0.385f) + 274.15f;
}

/**
 * @brief Convert LSB value to celsius.
 * @param lsb_value - ADC LSB raw value.
 * @param gain_value - Gain applied in LSB.
 * @return Temperature kelvin result.
 * @return Temperature celsius result.
 */
static float temp_lsb_raw_to_celsius(uint32_t lsb_value, int32_t gain_value)
{
	/*
		C = ((RTD - 100) / 0.385),
            where: RTD = LSB * Rref / 2^n / gain
	*/
	return (temp_lsb_raw_to_resistance(lsb_value, gain_value) - 100.0f) 
            / 0.385f;
}

/**
 * @brief Convert LSB value to fahrenheit.
 * @param lsb_value - ADC LSB raw value.
 * @param gain_value - Gain applied in LSB.
 * @return Temperature fahrenheit result.
 */
static float temp_lsb_raw_to_fahrenheit(uint32_t lsb_value, int32_t gain_value)
{
	/*
		F = ((1.8 * (RTD - 100)) / 0.385) + 32, 
            where: RTD = LSB * Rref / 2^n / gain
	*/
	return ((1.8f * (temp_lsb_raw_to_resistance(lsb_value, gain_value)- 100.0f))
            / 0.385f) + 32.0f;
}

/**
 * @brief Convert LSB value to other units.
 * @param unit - Temperature unit to convert.
 * @param lsb_value - ADC LSB raw value.
 * @param gain_value - Gain applied in LSB.
 * @param result - Converted value result.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
static error_t temp_convert_lsb_raw(temp_convert_t *unit, uint32_t lsb_value,
                                    int32_t gain_value, float *result)
{
	error_t err_sts = ERROR_NONE;

	switch(unit->output)
	{
		case TEMP_UNIT_LSB_RAW:
			*result = lsb_value;
			break;

        case TEMP_UNIT_RESISTANCE:
			*result = temp_lsb_raw_to_resistance(lsb_value, gain_value);
			break;

		case TEMP_UNIT_KELVIN:
			*result = temp_lsb_raw_to_kelvin(lsb_value, gain_value);
			break;

		case TEMP_UNIT_CELSIUS:
			*result = temp_lsb_raw_to_celsius(lsb_value, gain_value);
			break;

		case TEMP_UNIT_FAHRENHEIT:
			*result = temp_lsb_raw_to_fahrenheit(lsb_value, gain_value);
			break;

		default:
			err_sts = ERROR_INVALID_VALUE;
	}

	return err_sts;
}


/**************************** Resistance Conversion ***************************/


/**
 * @brief Convert resistance to LSB raw value.
 * @param rtd_value - Resistance value to be converted.
 * @param gain_value - Gain applied in LSB.
 * @return Temperature lsb raw result.
 */
static float temp_resistance_to_lsb_raw(float rtd_value, int32_t gain_value)
{
	/*
		LSB = Rrtd * gain * 2^n / Rref
            where: Rrtd = Vrtd / Iexc
                   Rref = Vref / Iexc
	*/
	return rtd_value * gain_value * TEMP_ADC_RESOLUTION / TEMP_REF_RESISTANCE;
}

/**
 * @brief Convert resistance to kelvin.
 * @param rtd_value - Resistance value to be converted.
 * @return Temperature kelvin result.
 */
static float temp_resistance_to_kelvin(float rtd_value)
{
	/*
		K = ((RTD - 100) / 0.385) + 274.15
	*/
	return ((rtd_value - 100.0f) / 0.385f) + 274.15f;
}

/**
 * @brief Convert resistance to celsius.
 * @param rtd_value - Resistance value to be converted.
 * @return Temperature celsius result.
 */
static float temp_resistance_to_celsius(float rtd_value)
{
	/*
		C = ((RTD - 100) / 0.385)
	*/
	return (rtd_value - 100.0f) / 0.385f;
}

/**
 * @brief Convert resistance to fahrenheit.
 * @param rtd_value - Resistance value to be converted.
 * @return Temperature fahrenheit result.
 */
static float temp_resistance_to_fahrenheit(float rtd_value)
{
	/*
		F = ((1.8 * (RTD - 100)) / 0.385) + 32
	*/
	return ((1.8f * (rtd_value - 100.0f)) / 0.385f) + 32.0f;
}

/**
 * @brief Convert resistance to other units.
 * @param unit - Temperature unit to convert.
 * @param rtd_value - Resistance value to convert.
 * @param gain_value - Gain applied in LSB.
 * @param result - Converted value result.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
static error_t temp_convert_resistance(temp_convert_t *unit, float rtd_value,
                                       int32_t gain_value, float *result)
{
	error_t err_sts = ERROR_NONE;

	switch(unit->output)
	{
		case TEMP_UNIT_LSB_RAW:
			*result = temp_resistance_to_lsb_raw(rtd_value, gain_value);
			break;

        case TEMP_UNIT_RESISTANCE:
			*result = rtd_value;
			break;

		case TEMP_UNIT_KELVIN:
			*result = temp_resistance_to_kelvin(rtd_value);
			break;

		case TEMP_UNIT_CELSIUS:
			*result = temp_resistance_to_celsius(rtd_value);
			break;

		case TEMP_UNIT_FAHRENHEIT:
			*result = temp_resistance_to_fahrenheit(rtd_value);
			break;

		default:
			err_sts = ERROR_INVALID_TYPE;
	}

	return err_sts;
}


/******************************* Kelvin Conversion ****************************/


/**
 * @brief Convert kelvin to resistance.
 * @param temp_value - Kelvin value to be converted.
 * @return Temperature resistance result.
 */
static float temp_kelvin_to_resistance(float temp_value)
{
	/*
		RTD = ((K - 274.15) * 0.385) + 100
	*/
	return ((temp_value - 274.15f) * 0.385f) + 100.0f;
}

/**
 * @brief Convert kelvin to lsb raw value.
 * @param temp_value - Kelvin value to be converted.
 * @param gain_value - Gain applied in LSB.
 * @return Temperature lsb raw result.
 */
static float temp_kelvin_to_lsb_raw(float temp_value, int32_t gain_value)
{
	/*
		LSB = RTD * gain * 2^n / Rref,
            where: RTD = ((K - 274.15) * 0.385) + 100
                   Rref = Vref / Iexc
	*/
	return temp_kelvin_to_resistance(temp_value) * gain_value 
            * TEMP_ADC_RESOLUTION / TEMP_REF_RESISTANCE;
}

/**
 * @brief Convert kelvin to celsius.
 * @param temp_value - Kelvin value to be converted.
 * @return Temperature celsius result.
 */
static float temp_kelvin_to_celsius(float temp_value)
{
	/*
		C = K - 273.15
	*/
	return temp_value - 273.15f;
}

/**
 * @brief Convert kelvin to fahrenheit.
 * @param temp_value - Kelvin value to be converted.
 * @return Temperature fahrenheit result.
 */
static float temp_kelvin_to_fahrenheit(float temp_value)
{
	/*
		F = ((K - 273.15) * 1.8) + 32
	*/
	return ((temp_value - 273.15f) * 1.8f) + 32.0f;
}

/**
 * @brief Convert kelvin to other units.
 * @param unit - Temperature unit to convert.
 * @param temp_value - Temperature value to convert.
 * @param gain_value - Gain applied in LSB.
 * @param result - Converted value result.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
static error_t temp_convert_kelvin(temp_convert_t *unit, float temp_value,
                                   int32_t gain_value, float *result)
{
	error_t err_sts = ERROR_NONE;

	switch(unit->output)
	{
		case TEMP_UNIT_LSB_RAW:
			*result = temp_kelvin_to_lsb_raw(temp_value, gain_value);
			break;

		case TEMP_UNIT_RESISTANCE:
			*result = temp_kelvin_to_resistance(temp_value);
			break;

		case TEMP_UNIT_KELVIN:
			*result = temp_value;
			break;

		case TEMP_UNIT_CELSIUS:
			*result = temp_kelvin_to_celsius(temp_value);
			break;

		case TEMP_UNIT_FAHRENHEIT:
			*result = temp_kelvin_to_fahrenheit(temp_value);
			break;

		default:
			err_sts = ERROR_INVALID_TYPE;
	}

	return err_sts;
}


/****************************** Celsius Conversion ****************************/


/**
 * @brief Convert celsius to resistance.
 * @param temp_value - Celsius value to be converted.
 * @return Temperature resistance result.
 */
static float temp_celsius_to_resistance(float temp_value)
{
	/*
		RTD = (C * 0.385) + 100
	*/
	return (temp_value * 0.385f) + 100.0f;
}

/**
 * @brief Convert celsius to lsb raw value.
 * @param temp_value - Celsius value to be converted.
 * @param gain_value - Gain applied in LSB.
 * @return Temperature lsb raw result.
 */
static float temp_celsius_to_lsb_raw(float temp_value, int32_t gain_value)
{
	/*
		LSB = RTD * gain * 2^n / Rref,
            where: RTD = (C * 0.385) + 100
                   Rref = Vref / Iexc
	*/
	return temp_celsius_to_resistance(temp_value) * gain_value
            * TEMP_ADC_RESOLUTION / TEMP_REF_RESISTANCE;
}

/**
 * @brief Convert celsius to kelvin.
 * @param temp_value - Celsius value to be converted.
 * @return Temperature kelvin result.
 */
static float temp_celsius_to_kelvin(float temp_value)
{
	/*
		K = C + 273.15
	*/
	return temp_value + 273.15f;
}

/**
 * @brief Convert celsius to fahrenheit.
 * @param temp_value - Celsius value to be converted.
 * @return Temperature fahrenheit result.
 */
static float temp_celsius_to_fahrenheit(float temp_value)
{
	/*
		F = (C * 1.8) + 32
	*/
	return (temp_value * 1.8f) + 32.0f;
}

/**
 * @brief Convert celsius to other units.
 * @param unit - Temperature unit to convert.
 * @param temp_value - Temperature value to convert.
 * @param gain_value - Gain applied in LSB.
 * @param result - Converted value result.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
static error_t temp_convert_celsius(temp_convert_t *unit, float temp_value,
                                    int32_t gain_value, float *result)
{
	error_t err_sts = ERROR_NONE;

	switch(unit->output)
	{
		case TEMP_UNIT_LSB_RAW:
			*result = temp_celsius_to_lsb_raw(temp_value, gain_value);
			break;

		case TEMP_UNIT_RESISTANCE:
			*result = temp_celsius_to_resistance(temp_value);
			break;

		case TEMP_UNIT_KELVIN:
			*result = temp_celsius_to_kelvin(temp_value);
			break;

		case TEMP_UNIT_CELSIUS:
			*result = temp_value;
			break;

		case TEMP_UNIT_FAHRENHEIT:
			*result = temp_celsius_to_fahrenheit(temp_value);
			break;

		default:
			err_sts = ERROR_INVALID_TYPE;
	}

	return err_sts;
}


/**************************** Fahrenheit Conversion ***************************/


/**
 * @brief Convert fahrenheit to resistance.
 * @param temp_value - Fahrenheit value to be converted.
 * @return Temperature resistance result.
 */
static float temp_fahrenheit_to_resistance(float temp_value)
{
	/*
		RTD = ((F - 32) * (0.385 / 1.8)) + 100
	*/
	return ((temp_value - 32.0f) * (0.385f / 1.8f)) + 100.0f;
}

/**
 * @brief Convert fahrenheit to lsb raw value.
 * @param temp_value - Fahrenheit value to be converted.
 * @param gain_value - Gain applied in LSB.
 * @return Temperature lsb raw result.
 */
static float temp_fahrenheit_to_lsb_raw(float temp_value, int32_t gain_value)
{
	/*
		LSB = RTD * gain * 2^n / Rref,
            where: RTD = ((F - 32) * (0.385 / 1.8)) + 100
                   Rref = Vref / Iexc
	*/
	return temp_fahrenheit_to_resistance(temp_value) * gain_value
            * TEMP_ADC_RESOLUTION / TEMP_REF_RESISTANCE;
}

/**
 * @brief Convert fahrenheit to kelvin.
 * @param temp_value - Fahrenheit value to be converted.
 * @return Temperature kelvin result.
 */
static float temp_fahrenheit_to_kelvin(float temp_value)
{
	/*
		K = ((F - 32) / 1.8) + 273.15
	*/
	return ((temp_value - 32.0f) / 1.8f) + 273.15f;
}

/**
 * @brief Convert fahrenheit to celsius.
 * @param temp_value - Fahrenheit value to be converted.
 * @return Temperature celsius result.
 */
static float temp_fahrenheit_to_celsius(float temp_value)
{
	/*
		C = (F - 32) / 1.8
	*/
	return (temp_value - 32.0f) / 1.8f;
}

/**
 * @brief Convert fahrenheit to other units.
 * @param unit - Temperature unit to convert.
 * @param temp_value - Temperature value to convert.
 * @param gain_value - Gain applied in LSB.
 * @param result - Converted value result.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
static error_t temp_convert_fahrenheit(temp_convert_t *unit, float temp_value, 
                                       int32_t gain_value, float *result)
{
	error_t err_sts = ERROR_NONE;

	switch(unit->output)
	{
		case TEMP_UNIT_LSB_RAW:
			*result = temp_fahrenheit_to_lsb_raw(temp_value, gain_value);
			break;

		case TEMP_UNIT_RESISTANCE:
			*result = temp_fahrenheit_to_resistance(temp_value); 
			break;

		case TEMP_UNIT_KELVIN:
			*result = temp_fahrenheit_to_kelvin(temp_value);
			break;

		case TEMP_UNIT_CELSIUS:
			*result = temp_fahrenheit_to_celsius(temp_value);
			break;

		case TEMP_UNIT_FAHRENHEIT:
			*result = temp_value;
			break;

		default:
			err_sts = ERROR_INVALID_TYPE;
	}

	return err_sts;
}


/******************************************************************************/
/*                           API INTERFACE FUNCTIONS                          */
/******************************************************************************/


/**
 * @brief Set the calibration values to apply.
 * @param h - Temperature handler.
 * @param offset - Offset applied in LSB.
 * @param gain - Gain applied in LSB.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t set_calibration(temp_api_handle_t *h, int16_t offset, int16_t gain)
{
	static int16_t input_gain = CALIB_GAIN_DEFAULT_VALUE;
	static int16_t input_offset = CALIB_OFFSET_DEFAULTVALUE;
	error_t err_sts = ERROR_NONE;

	if(h == NULL)
	{
		err_sts = ERROR_UNDEFINED_VALUE;
	}
	else
	{
        if((offset - 1) >= CALIB_OFFSET_MAX_SIZE ||
           (gain - 1) >= CALIB_GAIN_MAX_SIZE ||
           (offset + 1) <= CALIB_OFFSET_MIN_SIZE ||
           (gain + 1) <= CALIB_GAIN_MIN_SIZE)
		{
			err_sts = ERROR_OUT_OF_RANGE;
		}
		else
		{
			input_gain = gain;
			input_offset = offset;
		}

		h->calib_value.gain = input_gain;
		h->calib_value.offset = input_offset;
	}

	return err_sts;
}

/**
 * @brief Get the applied calibration values.
 * @param h - Temperature handler.
 * @param result - Calibration return value that is stored.
 *               - returns the ff.:
 *                 > default - if input parameter is invalid.
 *                 > output - valid calibration gain and offset values.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t get_calibration(temp_api_handle_t *h, temp_calib_t *result)
{
	error_t err_sts = ERROR_NONE;

    if(h == NULL)
	{
        result->gain = CALIB_GAIN_DEFAULT_VALUE;    /* Default Gain = 1     */
        result->offset = CALIB_OFFSET_DEFAULTVALUE; /* Default Offset = 0   */
		err_sts = ERROR_UNDEFINED_VALUE;
	}
	else
	{
		*result = h->calib_value;
	}

	return err_sts;
}

/**
 * @brief Apply calibration to the LSB values.
 * @param h - Temperature handler.
 * @param value - Value to be calibrated.
 * @param result - Calibrated result return value.
 *               - returns the ff.:
 *                 > 0 - if all input parameters are invalid.
 *                 > value - if value is valid but others are invalid.
 *                 > output - valid computed calibration output result.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t calibrate(temp_api_handle_t *h, int32_t value, int32_t *result)
{
	error_t err_sts = ERROR_NONE;
	int32_t output = 0;

	if((value - 1) >= CALIB_VALUE_MAX_SIZE ||
       (value + 1) <= CALIB_VALUE_MIN_SIZE)
	{
		err_sts = ERROR_OUT_OF_RANGE;
	}
	else
	{
        output = value;	/*  Default: result = (value * 1) + 0	*/

        if(h == NULL)
        {
                err_sts = ERROR_UNDEFINED_VALUE;
        }
		else
		{
			output =  (value * h->calib_value.gain);

			if((output - 1) >= CALIB_RESULT_MAX_SIZE ||
               (output + 1) <= CALIB_RESULT_MIN_SIZE ||
               (output + h->calib_value.offset - 1) >=  CALIB_RESULT_MAX_SIZE ||
               (output + h->calib_value.offset + 1) <= CALIB_RESULT_MIN_SIZE)
			{
				err_sts = ERROR_OUT_OF_RANGE;
			}
			else
			{
				output = output + h->calib_value.offset;
			}
		}
	}

    *result = output;

	return err_sts;
}

/**
 * @brief Set the linearization type to apply.
 * @param h - Temperature handler.
 * @param type - Type of linearization to be applied.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t set_linearization(temp_api_handle_t *h, temp_linear_t type)
{
	error_t err_sts = ERROR_NONE;

	if(h == NULL)
	{
		err_sts = ERROR_UNDEFINED_VALUE;
	}
	else
	{
		if(type >= TEMP_LINEAR_MAX)
		{
			err_sts = ERROR_INVALID_TYPE;
		}
		else
		{
			h->linear_type = type;
		}
	}

	return err_sts;
}

/**
 * @brief Get the type of linearization applied.
 * @param h - Temperature handler.
 * @param result - The linearization type that is stored.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t get_linearization(temp_api_handle_t *h, temp_linear_t *result)
{
	error_t err_sts = ERROR_NONE;

	*result = TEMP_LINEAR_NONE;

	if(h == NULL)
	{
		err_sts = ERROR_UNDEFINED_VALUE;
	}
	else
	{
		*result = h->linear_type;
	}

	return err_sts;
}

/**
 * @brief Apply linearization to the resistor.
 * @param h - Temperature handler.
 * @param rtd_value - The resistor value to be linearize.
 * @param result - Linearization result return value.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t linearize(temp_api_handle_t *h, float rtd_value, float *result)
{
	error_t err_sts = ERROR_NONE;

	if(h == NULL)
	{
		err_sts = ERROR_UNDEFINED_VALUE;
	}
	else
	{
		switch(h->linear_type)
		{
			case TEMP_LINEAR_CALLENDAR_VAN_DUSEN:
				*result = temp_callendar_van_dusen(rtd_value);
				break;

			case TEMP_LINEAR_APPROXIMATION:
				*result = temp_approximation(rtd_value);
				break;

			case TEMP_LINEAR_LOOKUP_TABLE:
                if(h->temp_table_lut != NULL)
				{
					*result = temp_lookup_table(h->temp_table_lut,
                                                h->temp_table_size, rtd_value);
				}
				else
				{
					err_sts = ERROR_UNDEFINED_VALUE;
				}	
				break;

			case TEMP_LINEAR_NONE:
				/* Linearization not applied. */
				*result = rtd_value;
				break;

			default:
				err_sts = ERROR_INVALID_TYPE;
		}
	}

	return err_sts;
}

/**
 * @brief Apply conversion of temperature units.
 * @param h - Temperature handler.
 * @param type - Temperature input and output type unit.
 * @param value - Temperature value to be converted.
 * @param result - Temperature conversion result.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t conversion(temp_api_handle_t *h, temp_convert_t *type, void *value,
                   void *result)
{
	error_t err_sts = ERROR_NONE;

	if((h == NULL) && (type == NULL))
	{
		err_sts = ERROR_UNDEFINED_VALUE;
	}
	else
	{
		switch(type->input)
		{
			case TEMP_UNIT_LSB_RAW:
				err_sts = temp_convert_lsb_raw(type, *(uint32_t*)value,
                                               h->calib_value.gain, result);
				break;

			case TEMP_UNIT_RESISTANCE:
				err_sts = temp_convert_resistance(type, *(float*)value,
                                                  h->calib_value.gain, result);
				break;

			case TEMP_UNIT_KELVIN:
				err_sts = temp_convert_kelvin(type, *(float*)value,
                                              h->calib_value.gain, result);
				break;

			case TEMP_UNIT_CELSIUS:
				err_sts = temp_convert_celsius(type, *(float*)value,
                                               h->calib_value.gain, result);
				break;

			case TEMP_UNIT_FAHRENHEIT:
				err_sts = temp_convert_fahrenheit(type, *(float*)value,
                                                  h->calib_value.gain, result);
				break;

			default:
				err_sts = ERROR_INVALID_TYPE;
		}
	}

	return err_sts;
}
