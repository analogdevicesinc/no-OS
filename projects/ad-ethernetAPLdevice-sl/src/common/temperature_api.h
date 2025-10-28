/****************************************************************************//*
 *   @file   hockey_puck_api.h
 *   @brief  Definitions for Hockey Puck application program interface (API).
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

#ifndef __HOCKEY_PUCK_API_H__
#define __HOCKEY_PUCK_API_H__

#include <math.h>

#define SHORT_INT_MAX_SIZE 			(32767)
#define SHORT_INT_MIN_SIZE 			(-32768)
#define LONG_INT_MAX_SIZE 			(2147483647)
#define LONG_INT_MIN_SIZE 			(-2147483648)
#define LONG_UINT_MAX_SIZE 			(4294967296)
#define LONG_UINT_MIN_SIZE 			(0)

#define CALIB_GAIN_MAX_SIZE			SHORT_INT_MAX_SIZE
#define CALIB_GAIN_MIN_SIZE			SHORT_INT_MIN_SIZE
#define CALIB_OFFSET_MAX_SIZE		SHORT_INT_MAX_SIZE
#define CALIB_OFFSET_MIN_SIZE		SHORT_INT_MIN_SIZE
#define CALIB_VALUE_MAX_SIZE		LONG_INT_MAX_SIZE
#define CALIB_VALUE_MIN_SIZE		LONG_INT_MIN_SIZE
#define CALIB_RESULT_MAX_SIZE		LONG_INT_MAX_SIZE
#define CALIB_RESULT_MIN_SIZE		LONG_INT_MIN_SIZE
#define CALIB_GAIN_DEFAULT_VALUE	(1u)
#define CALIB_OFFSET_DEFAULTVALUE	(0u)

/* ADC channel resolution of 24-bits        */
#define TEMP_ADC_BIT_NUMBER			(24u)
/* ADC codes resolution at pow(2, 24)       */
#define TEMP_ADC_RESOLUTION			(16777216u)
/* I_exc = 0.00025 Amp in 4-wire config     */
#define TEMP_IEXC_REF_VALUE			(0.00025f)
/* Vref = I_exc * Rref in 4-wire config     */
#define TEMP_VOLT_REF_VALUE			(0.975f)
/* Rref  = 3900 Ohms in 4-wire config       */
#define TEMP_REF_RESISTANCE			(3900.0f)
/* PT100 RTD resistance of R0 at 0 degC     */
#define TEMP_RTD_RESISTANCE			(100.0f)


typedef enum
{
	ERROR_NONE,
	ERROR_OUT_OF_RANGE = 0xFFF0,
	ERROR_INVALID_TYPE = 0xFFF1,
	ERROR_INVALID_VALUE = 0xFFF2,
	ERROR_UNDEFINED_VALUE = 0xFFF3,
	ERROR_UNKNOWN = 0xFFFF,
} error_t;

typedef enum
{
	TEMP_LINEAR_NONE,
	TEMP_LINEAR_CALLENDAR_VAN_DUSEN,
	TEMP_LINEAR_APPROXIMATION,
	TEMP_LINEAR_LOOKUP_TABLE,
	TEMP_LINEAR_MAX
} temp_linear_t;

typedef enum
{
	TEMP_CHANNEL_0,
	TEMP_CHANNEL_1,
	TEMP_CHANNEL_MAX
} temp_channel_t;

typedef enum
{
	TEMP_UNIT_LSB_RAW,
	TEMP_UNIT_RESISTANCE,
	TEMP_UNIT_KELVIN,
	TEMP_UNIT_CELSIUS,
	TEMP_UNIT_FAHRENHEIT,
	TEMP_UNIT_MAX
} temp_unit_t;

typedef struct
{
	temp_unit_t input;
	temp_unit_t output;
} temp_convert_t;

typedef struct
{
	int16_t offset;
	int16_t gain;
} temp_calib_t;

typedef struct
{
	float x;			/* Resistance coordinates value.	*/
	float y;			/* Temperature coordinates value.	*/
} temp_table_t;

typedef struct
{
	/* Temperature Calibration */
	temp_calib_t calib_value;

	/* Temperature Conversion */
	temp_convert_t convert_type;
    int32_t convert_lsb_value;
    float convert_rtd_value;
    float convert_temp_value;

	/* Temperature Linearization */
	temp_linear_t linear_type;
    float linear_temp_value;
	const temp_table_t *temp_table_lut;
	uint32_t temp_table_size;
} temp_api_handle_t;


/* 
	Calibration interface functions
*/

/**
 * @brief Set the calibration values to apply.
 * @param h - Temperature handler.
 * @param offset - Offset applied in LSB.
 * @param gain - Gain applied in LSB.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t set_calibration(temp_api_handle_t *h, int16_t offset, int16_t gain);

/**
 * @brief Get the applied calibration values.
 * @param h - Temperature handler.
 * @param result - Calibration return value that is stored.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t get_calibration(temp_api_handle_t *h, temp_calib_t *result);

/**
 * @brief Apply calibration to the LSB values.
 * @param h - Temperature handler.
 * @param value - Value to be calibrated.
 * @param result - Calibrated result return value.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t calibrate(temp_api_handle_t *h, int32_t value, int32_t *result);


/* 
	Linearization interface functions
 */

/**
 * @brief Set the linearization type to apply.
 * @param h - Temperature handler.
 * @param channel_no - ADC channel to apply linearization.
 * @param type - Type of linearization to be applied.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t set_linearization(temp_api_handle_t *h, temp_linear_t type);

/**
 * @brief Get the type of linearization applied.
 * @param h - Temperature handler.
 * @param channel_no - ADC channel to apply linearization.
 * @param result - The linearization type that is stored.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t get_linearization(temp_api_handle_t *h, temp_linear_t *result);

/**
 * @brief Apply linearization to the resistor.
 * @param h - Temperature handler.
 * @param rtd_value - The resistor value to be linearize.
 * @param result - Linearization result return value.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t linearize(temp_api_handle_t *h, float rtd_value, float *result);


/* 
	Conversion interface function
 */

/**
 * @brief Apply conversion of temperature units.
 * @param h - Temperature handler.
 * @param type - Temperature input and output type unit.
 * @param value - Temperature value to be converted.
 * @param result - Temperature conversion result.
 * @return ERROR_NONE if success, error_t error codes otherwise.
 */
error_t conversion(temp_api_handle_t *h, temp_convert_t *type, void *value,
                   void *result);


/* 
	Temperature-resistance lookup table
 */
static const temp_table_t temp_api_table_pt100_rtd[] =
{	/*      x       y	*/
	{	18.52,	-200	},
	{	18.96,	-199	},
	{	19.39,	-198	},
	{	19.82,	-197	},
	{	20.25,	-196	},
	{	20.68,	-195	},
	{	21.11,	-194	},
	{	21.54,	-193	},
	{	21.97,	-192	},
	{	22.4,	-191	},
	{	22.83,	-190	},
	{	23.26,	-189	},
	{	23.69,	-188	},
	{	24.12,	-187	},
	{	24.55,	-186	},
	{	24.97,	-185	},
	{	25.39,	-184	},
	{	25.82,	-183	},
	{	26.25,	-182	},
	{	26.67,	-181	},
	{	27.1,	-180	},
	{	27.52,	-179	},
	{	27.95,	-178	},
	{	28.37,	-177	},
	{	28.8,	-176	},
	{	29.22,	-175	},
	{	29.65,	-174	},
	{	30.07,	-173	},
	{	30.49,	-172	},
	{	30.92,	-171	},
	{	31.34,	-170	},
	{	31.76,	-169	},
	{	32.18,	-168	},
	{	32.61,	-167	},
	{	33.03,	-166	},
	{	33.45,	-165	},
	{	33.86,	-164	},
	{	34.28,	-163	},
	{	34.7,	-162	},
	{	35.12,	-161	},
	{	35.54,	-160	},
	{	35.96,	-159	},
	{	36.38,	-158	},
	{	36.8,	-157	},
	{	37.22,	-156	},
	{	37.63,	-155	},
	{	38.05,	-154	},
	{	38.47,	-153	},
	{	38.89,	-152	},
	{	39.31,	-151	},
	{	39.72,	-150	},
	{	40.14,	-149	},
	{	40.56,	-148	},
	{	40.97,	-147	},
	{	41.39,	-146	},
	{	41.8,	-145	},
	{	42.22,	-144	},
	{	42.64,	-143	},
	{	43.05,	-142	},
	{	43.46,	-141	},
	{	43.88,	-140	},
	{	44.29,	-139	},
	{	44.71,	-138	},
	{	45.12,	-137	},
	{	45.53,	-136	},
	{	45.95,	-135	},
	{	46.35,	-134	},
	{	46.76,	-133	},
	{	47.18,	-132	},
	{	47.59,	-131	},
	{	48.0,	-130	},
	{	48.41,	-129	},
	{	48.82,	-128	},
	{	49.23,	-127	},
	{	49.64,	-126	},
	{	50.06,	-125	},
	{	50.47,	-124	},
	{	50.88,	-123	},
	{	51.29,	-122	},
	{	51.7,	-121	},
	{	52.11,	-120	},
	{	52.52,	-119	},
	{	52.92,	-118	},
	{	53.33,	-117	},
	{	53.74,	-116	},
	{	54.15,	-115	},
	{	54.56,	-114	},
	{	54.97,	-113	},
	{	55.38,	-112	},
	{	55.78,	-111	},
	{	56.19,	-110	},
	{	56.6,	-109	},
	{	57.0,	-108	},
	{	57.41,	-107	},
	{	57.82,	-106	},
	{	58.22,	-105	},
	{	58.63,	-104	},
	{	59.04,	-103	},
	{	59.44,	-102	},
	{	59.85,	-101	},
	{	60.26,	-100	},
	{	60.67,	-99		},
	{	61.07,	-98		},
	{	61.48,	-97		},
	{	61.87,	-96		},
	{	62.29,	-95		},
	{	62.69,	-94		},
	{	63.1,	-93		},
	{	63.5,	-92		},
	{	63.91,	-91		},
	{	64.3,	-90		},
	{	64.7,	-89		},
	{	65.11,	-88		},
	{	65.51,	-87		},
	{	65.91,	-86		},
	{	66.31,	-85		},
	{	66.72,	-84		},
	{	67.12,	-83		},
	{	67.52,	-82		},
	{	67.92,	-81		},
	{	68.33,	-80		},
	{	68.73,	-79		},
	{	69.13,	-78		},
	{	69.53,	-77		},
	{	69.93,	-76		},
	{	70.33,	-75		},
	{	70.73,	-74		},
	{	71.13,	-73		},
	{	71.53,	-72		},
	{	71.93,	-71		},
	{	72.33,	-70		},
	{	72.73,	-69		},
	{	73.13,	-68		},
	{	73.53,	-67		},
	{	73.93,	-66		},
	{	74.33,	-65		},
	{	74.73,	-64		},
	{	75.13,	-63		},
	{	75.53,	-62		},
	{	75.93,	-61		},
	{	76.33,	-60		},
	{	76.73,	-59		},
	{	77.13,	-58		},
	{	77.52,	-57		},
	{	77.92,	-56		},
	{	78.32,	-55		},
	{	78.72,	-54		},
	{	79.11,	-53		},
	{	79.51,	-52		},
	{	79.91,	-51		},
	{	80.31,	-50		},
	{	80.7,	-49		},
	{	81.1,	-48		},
	{	81.5,	-47		},
	{	81.89,	-46		},
	{	82.29,	-45		},
	{	82.69,	-44		},
	{	83.08,	-43		},
	{	83.48,	-42		},
	{	83.88,	-41		},
	{	84.27,	-40		},
	{	84.67,	-39		},
	{	85.06,	-38		},
	{	85.46,	-37		},
	{	85.85,	-36		},
	{	86.25,	-35		},
	{	86.64,	-34		},
	{	87.04,	-33		},
	{	87.43,	-32		},
	{	87.83,	-31		},
	{	88.22,	-30		},
	{	88.62,	-29		},
	{	89.01,	-28		},
	{	89.4,	-27		},
	{	89.8,	-26		},
	{	90.19,	-25		},
	{	90.59,	-24		},
	{	90.98,	-23		},
	{	91.37,	-22		},
	{	91.77,	-21		},
	{	92.16,	-20		},
	{	92.55,	-19		},
	{	92.95,	-18		},
	{	93.34,	-17		},
	{	93.73,	-16		},
	{	94.12,	-15		},
	{	94.52,	-14		},
	{	94.91,	-13		},
	{	95.3,	-12		},
	{	95.69,	-11		},
	{	96.09,	-10		},
	{	96.48,	-9		},
	{	96.87,	-8		},
	{	97.26,	-7		},
	{	97.65,	-6		},
	{	98.04,	-5		},
	{	98.44,	-4		},
	{	98.83,	-3		},
	{	99.22,	-2		},
	{	99.61,	-1		},
	{	100,	0		},
	{	100.39,	1		},
	{	100.78,	2		},
	{	101.17,	3		},
	{	101.56,	4		},
	{	101.95,	5		},
	{	102.34,	6		},
	{	102.73,	7		},
	{	103.12,	8		},
	{	103.51,	9		},
	{	103.9,	10		},
	{	104.29,	11		},
	{	104.68,	12		},
	{	105.07,	13		},
	{	105.46,	14		},
	{	105.85,	15		},
	{	106.24,	16		},
	{	106.63,	17		},
	{	107.02,	18		},
	{	107.4,	19		},
	{	107.79,	20		},
	{	108.18,	21		},
	{	108.57,	22		},
	{	108.96,	23		},
	{	109.35,	24		},
	{	109.73,	25		},
	{	110.12,	26		},
	{	110.51,	27		},
	{	110.9,	28		},
	{	111.28,	29		},
	{	111.67,	30		},
	{	112.06,	31		},
	{	112.45,	32		},
	{	112.83,	33		},
	{	113.22,	34		},
	{	113.61,	35		},
	{	113.99,	36		},
	{	114.38,	37		},
	{	114.77,	38		},
	{	115.15,	39		},
	{	115.54,	40		},
	{	115.93,	41		},
	{	116.31,	42		},
	{	116.7,	43		},
	{	117.08,	44		},
	{	117.47,	45		},
	{	117.85,	46		},
	{	118.24,	47		},
	{	118.62,	48		},
	{	119.01,	49		},
	{	119.4,	50		},
	{	119.78,	51		},
	{	120.16,	52		},
	{	120.55,	53		},
	{	120.93,	54		},
	{	121.32,	55		},
	{	121.7,	56		},
	{	122.09,	57		},
	{	122.47,	58		},
	{	122.86,	59		},
	{	123.24,	60		},
	{	123.62,	61		},
	{	124.01,	62		},
	{	124.39,	63		},
	{	124.77,	64		},
	{	125.17,	65		},
	{	125.55,	66		},
	{	125.93,	67		},
	{	126.32,	68		},
	{	126.7,	69		},
	{	127.08,	70		},
	{	127.46,	71		},
	{	127.85,	72		},
	{	128.23,	73		},
	{	128.61,	74		},
	{	128.99,	75		},
	{	129.38,	76		},
	{	129.76,	77		},
	{	130.14,	78		},
	{	130.52,	79		},
	{	130.9,	80		},
	{	131.28,	81		},
	{	131.67,	82		},
	{	132.05,	83		},
	{	132.43,	84		},
	{	132.81,	85		},
	{	133.19,	86		},
	{	133.57,	87		},
	{	133.95,	88		},
	{	134.33,	89		},
	{	134.71,	90		},
	{	135.09,	91		},
	{	135.47,	92		},
	{	135.85,	93		},
	{	136.23,	94		},
	{	136.61,	95		},
	{	136.99,	96		},
	{	137.37,	97		},
	{	137.75,	98		},
	{	138.13,	99		},
	{	138.51,	100		},
	{	138.89,	101		},
	{	139.27,	102		},
	{	139.65,	103		},
	{	140.03,	104		},
	{	140.39,	105		},
	{	140.77,	106		},
	{	141.15,	107		},
	{	141.53,	108		},
	{	141.91,	109		},
	{	142.29,	110		},
	{	142.66,	111		},
	{	143.04,	112		},
	{	143.42,	113		},
	{	143.8,	114		},
	{	144.18,	115		},
	{	144.56,	116		},
	{	144.94,	117		},
	{	145.32,	118		},
	{	145.69,	119		},
	{	146.07,	120		},
	{	146.45,	121		},
	{	146.82,	122		},
	{	147.2,	123		},
	{	147.58,	124		},
	{	147.95,	125		},
	{	148.33,	126		},
	{	148.71,	127		},
	{	149.08,	128		},
	{	149.46,	129		},
	{	149.83,	130		},
	{	150.21,	131		},
	{	150.58,	132		},
	{	150.96,	133		},
	{	151.34,	134		},
	{	151.71,	135		},
	{	152.09,	136		},
	{	152.46,	137		},
	{	152.84,	138		},
	{	153.21,	139		},
	{	153.58,	140		},
	{	153.95,	141		},
	{	154.32,	142		},
	{	154.71,	143		},
	{	155.08,	144		},
	{	155.46,	145		},
	{	155.83,	146		},
	{	156.21,	147		},
	{	156.58,	148		},
	{	156.96,	149		},
	{	157.33,	150		},
	{	157.71,	151		},
	{	158.08,	152		},
	{	158.45,	153		},
	{	158.83,	154		},
	{	159.2,	155		},
	{	159.56,	156		},
	{	159.94,	157		},
	{	160.31,	158		},
	{	160.68,	159		},
	{	161.05,	160		},
	{	161.43,	161		},
	{	161.8,	162		},
	{	162.17,	163		},
	{	162.54,	164		},
	{	162.91,	165		},
	{	163.28,	166		},
	{	163.66,	167		},
	{	164.03,	168		},
	{	164.4,	169		},
	{	164.77,	170		},
	{	165.14,	171		},
	{	165.51,	172		},
	{	165.88,	173		},
	{	166.25,	174		},
	{	166.62,	175		},
	{	167,	176		},
	{	167.37,	177		},
	{	167.74,	178		},
	{	168.11,	179		},
	{	168.48,	180		},
	{	168.85,	181		},
	{	169.22,	182		},
	{	169.59,	183		},
	{	169.96,	184		},
	{	170.33,	185		},
	{	170.69,	186		},
	{	171.06,	187		},
	{	171.43,	188		},
	{	171.8,	189		},
	{	172.17,	190		},
	{	172.54,	191		},
	{	172.91,	192		},
	{	173.27,	193		},
	{	173.64,	194		},
	{	174.01,	195		},
	{	174.39,	196		},
	{	174.75,	197		},
	{	175.12,	198		},
	{	175.49,	199		},
	{	175.86,	200		},
	{	176.23,	201		},
	{	176.59,	202		},
	{	176.96,	203		},
	{	177.33,	204		},
	{	177.7,	205		},
	{	178.06,	206		},
	{	178.43,	207		},
	{	178.8,	208		},
	{	179.16,	209		},
	{	179.53,	210		},
	{	179.9,	211		},
	{	180.26,	212		},
	{	180.63,	213		},
	{	180.99,	214		},
	{	181.36,	215		},
	{	181.73,	216		},
	{	182.09,	217		},
	{	182.46,	218		},
	{	182.82,	219		},
	{	183.19,	220		},
	{	183.55,	221		},
	{	183.92,	222		},
	{	184.28,	223		},
	{	184.65,	224		},
	{	185.01,	225		},
	{	185.38,	226		},
	{	185.74,	227		},
	{	186.11,	228		},
	{	186.47,	229		},
	{	186.84,	230		},
	{	187.2,	231		},
	{	187.56,	232		},
	{	187.93,	233		},
	{	188.29,	234		},
	{	188.65,	235		},
	{	189.02,	236		},
	{	189.38,	237		},
	{	189.74,	238		},
	{	190.11,	239		},
	{	190.47,	240		},
	{	190.83,	241		},
	{	191.2,	242		},
	{	191.56,	243		},
	{	191.92,	244		},
	{	192.28,	245		},
	{	192.66,	246		},
	{	193.02,	247		},
	{	193.38,	248		},
	{	193.74,	249		},
	{	194.1,	250		},
	{	194.47,	251		},
	{	194.83,	252		},
	{	195.19,	253		},
	{	195.55,	254		},
	{	195.9,	255		},
	{	196.26,	256		},
	{	196.62,	257		},
	{	196.98,	258		},
	{	197.35,	259		},
	{	197.71,	260		},
	{	198.07,	261		},
	{	198.43,	262		},
	{	198.79,	263		},
	{	199.15,	264		},
	{	199.51,	265		},
	{	199.87,	266		},
	{	200.23,	267		},
	{	200.59,	268		},
	{	200.95,	269		},
	{	201.31,	270		},
	{	201.67,	271		},
	{	202.03,	272		},
	{	202.38,	273		},
	{	202.74,	274		},
	{	203.1,	275		},
	{	203.46,	276		},
	{	203.82,	277		},
	{	204.18,	278		},
	{	204.54,	279		},
	{	204.9,	280		},
	{	205.25,	281		},
	{	205.61,	282		},
	{	205.97,	283		},
	{	206.33,	284		},
	{	206.7,	285		},
	{	207.05,	286		},
	{	207.41,	287		},
	{	207.77,	288		},
	{	208.13,	289		},
	{	208.48,	290		},
	{	208.84,	291		},
	{	209.2,	292		},
	{	209.55,	293		},
	{	209.91,	294		},
	{	210.27,	295		},
	{	210.62,	296		},
	{	210.98,	297		},
	{	211.34,	298		},
	{	211.69,	299		},
	{	212.05,	300		},
	{	212.4,	301		},
	{	212.76,	302		},
	{	213.12,	303		},
	{	213.47,	304		},
	{	213.83,	305		},
	{	214.19,	306		},
	{	214.55,	307		},
	{	214.9,	308		},
	{	215.26,	309		},
	{	215.61,	310		},
	{	215.97,	311		},
	{	216.32,	312		},
	{	216.68,	313		},
	{	217.03,	314		},
	{	217.39,	315		},
	{	217.73,	316		},
	{	218.08,	317		},
	{	218.44,	318		},
	{	218.79,	319		},
	{	219.15,	320		},
	{	219.5,	321		},
	{	219.85,	322		},
	{	220.21,	323		},
	{	220.56,	324		},
	{	220.91,	325		},
	{	221.27,	326		},
	{	221.62,	327		},
	{	221.97,	328		},
	{	222.32,	329		},
	{	222.68,	330		},
	{	223.03,	331		},
	{	223.38,	332		},
	{	223.73,	333		},
	{	224.09,	334		},
	{	224.45,	335		},
	{	224.8,	336		},
	{	225.15,	337		},
	{	225.5,	338		},
	{	225.85,	339		},
	{	226.21,	340		},
	{	226.56,	341		},
	{	226.91,	342		},
	{	227.26,	343		},
	{	227.61,	344		},
	{	227.96,	345		},
	{	228.31,	346		},
	{	228.66,	347		},
	{	229.01,	348		},
	{	229.36,	349		},
	{	229.72,	350		},
	{	230.07,	351		},
	{	230.42,	352		},
	{	230.77,	353		},
	{	231.12,	354		},
	{	231.47,	355		},
	{	231.81,	356		},
	{	232.16,	357		},
	{	232.51,	358		},
	{	232.86,	359		},
	{	233.21,	360		},
	{	233.56,	361		},
	{	233.91,	362		},
	{	234.26,	363		},
	{	234.6,	364		},
	{	234.95,	365		},
	{	235.3,	366		},
	{	235.65,	367		},
	{	236,	368		},
	{	236.35,	369		},
	{	236.7,	370		},
	{	237.05,	371		},
	{	237.4,	372		},
	{	237.75,	373		},
	{	238.09,	374		},
	{	238.44,	375		},
	{	238.79,	376		},
	{	239.14,	377		},
	{	239.48,	378		},
	{	239.83,	379		},
	{	240.18,	380		},
	{	240.52,	381		},
	{	240.87,	382		},
	{	241.22,	383		},
	{	241.56,	384		},
	{	241.91,	385		},
	{	242.25,	386		},
	{	242.6,	387		},
	{	242.95,	388		},
	{	243.29,	389		},
	{	243.64,	390		},
	{	243.98,	391		},
	{	244.33,	392		},
	{	244.67,	393		},
	{	245.02,	394		},
	{	245.36,	395		},
	{	245.71,	396		},
	{	246.05,	397		},
	{	246.4,	398		},
	{	246.74,	399		},
	{	247.09,	400		},
	{	247.43,	401		},
	{	247.78,	402		},
	{	248.12,	403		},
	{	248.46,	404		},
	{	248.81,	405		},
	{	249.15,	406		},
	{	249.5,	407		},
	{	249.84,	408		},
	{	250.18,	409		},
	{	250.53,	410		},
	{	250.89,	411		},
	{	251.21,	412		},
	{	251.55,	413		},
	{	251.9,	414		},
	{	252.24,	415		},
	{	252.59,	416		},
	{	252.94,	417		},
	{	253.28,	418		},
	{	253.62,	419		},
	{	253.96,	420		},
	{	254.3,	421		},
	{	254.65,	422		},
	{	254.99,	423		},
	{	255.33,	424		},
	{	255.67,	425		},
	{	256.01,	426		},
	{	256.35,	427		},
	{	256.7,	428		},
	{	257.04,	429		},
	{	257.38,	430		},
	{	257.72,	431		},
	{	258.06,	432		},
	{	258.4,	433		},
	{	258.74,	434		},
	{	259.08,	435		},
	{	259.42,	436		},
	{	259.76,	437		},
	{	260.1,	438		},
	{	260.44,	439		},
	{	260.78,	440		},
	{	261.12,	441		},
	{	261.46,	442		},
	{	261.8,	443		},
	{	262.14,	444		},
	{	262.48,	445		},
	{	262.83,	446		},
	{	263.17,	447		},
	{	263.5,	448		},
	{	263.84,	449		},
	{	264.18,	450		},
	{	264.52,	451		},
	{	264.86,	452		},
	{	265.2,	453		},
	{	265.54,	454		},
	{	265.87,	455		},
	{	266.21,	456		},
	{	266.55,	457		},
	{	266.89,	458		},
	{	267.22,	459		},
	{	267.56,	460		},
	{	267.9,	461		},
	{	268.24,	462		},
	{	268.57,	463		},
	{	268.91,	464		},
	{	269.25,	465		},
	{	269.58,	466		},
	{	269.92,	467		},
	{	270.26,	468		},
	{	270.59,	469		},
	{	270.93,	470		},
	{	271.27,	471		},
	{	271.6,	472		},
	{	271.94,	473		},
	{	272.27,	474		},
	{	272.61,	475		},
	{	272.95,	476		},
	{	273.28,	477		},
	{	273.62,	478		},
	{	273.95,	479		},
};
#define TEMP_API_TABLE_PT100_RTD_SIZE (sizeof(temp_api_table_pt100_rtd) \
                                        / sizeof(temp_api_table_pt100_rtd[0]))


#endif /* __HOCKEY_PUCK_API_H__ */