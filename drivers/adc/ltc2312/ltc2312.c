/*
 * Copyright (c) 2013, 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*! @defgroup LTC2315 LTC2315: 12/14-Bit 1Msps ADC */

/*! @file
 *  @ingroup LTC2315
 *  Library for LTC2315: 12/14-Bit 1Msps ADC
 *  */

#include <stdlib.h>
#include <math.h>
#include "ltc2312.h"
#include "no_os_alloc.h"

/**
 * Initializes the ltc2312 device handler.
 *
 * @param [out] device     - The device structure.
 * @param [in]  init_param - The structure that contains the device initial
 * 		                     parameters.
 *
 * @return 0 for success, or negative error code otherwise.
 */
int32_t ltc2312_setup(struct ltc2312_dev **device,
		      struct ltc2312_init_param *init_param)
{
	struct ltc2312_dev *dev;
	int32_t ret;

	dev = no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret != 0)
		goto error;

	dev->type = init_param->type;

	*device = dev;

	return ret;
error:
	no_os_free(dev);

	return ret;
}

/**
 * Free the resources allocated by ltc2312_setup().
 *
 * @param [in] dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ltc2312_remove(struct ltc2312_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -1;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret != 0)
		return ret;

	no_os_free(dev);

	return ret;
}

/**
 * Get ADC code right shift.
 *
 * The LTC2312 returns a 16-bit data, but only the 12 or 14 most significant
 * bytes are actual data, depending on the part. The rest of the bits are 0, so
 * the data must be shifted to the right by 2 for LTC2312-14 and by 4 for
 * LTC2312-12.
 *
 * @param [in]  dev 		 - The device structure.
 * @param [out] ptr_adc_code - Read data.
 *
 */
static uint8_t ltc2312_get_shift(struct ltc2312_dev *dev)
{
	switch (dev->type) {
	case LTC2312_12:
		return 4;
		break;
	case LTC2312_14:
		return 2;
		break;
	default:
		return 4;
	}
}

/**
 * Reads the LTC2312 and returns 16-bit data in offset binary format.
 *
 * @param [in]  dev 		 - The device structure.
 * @param [out] ptr_adc_code - Read data.
 *
 * @return 0 for success, or negative error code otherwise.
 */
int32_t ltc2312_read(struct ltc2312_dev *dev, uint16_t *ptr_adc_code)
{
	int32_t ret = 0;
	uint8_t bytes_no;
	uint8_t adc_array[] = {0x00, 0x00};
	uint8_t shift;
	uint8_t i;
	uint32_t sum = 0;
	uint16_t value_array[LTC2312_READ_VALUES_NUMBER];

	/* Select number of bytes to read */
	bytes_no = LTC2312_READ_BYTES_NUMBER;

	/* Dummy read to update  */
	ret = no_os_spi_write_and_read(dev->spi_desc, adc_array, bytes_no);
	if (ret != 0)
		return ret;

	/* Gather values for averaging */
	for (i = 0; i < LTC2312_READ_VALUES_NUMBER; ++i) {
		no_os_mdelay(1);

		/* Read the value of the ADC */
		ret = no_os_spi_write_and_read(dev->spi_desc, adc_array, bytes_no);
		if (ret != 0) {
			return ret;
		}

		value_array[i] = (((uint16_t)adc_array[0] << 8) | adc_array[1]);
		shift = ltc2312_get_shift(dev);
		value_array[i] >>= shift;
	}

	/* Calculate the sum of the values */
	for (i = 0; i < LTC2312_READ_VALUES_NUMBER; ++i) {
		sum += value_array[i];
	}

	/* Return mean value of the code read from ADC */
	*ptr_adc_code = (uint16_t)(sum / (uint32_t)LTC2312_READ_VALUES_NUMBER);

	return ret;
}

/**
 * Calculates the LTC2312 input voltage given the binary data and LSB weight.
 *
 * @param [in]  dev 		 - The device structure.
 * @param [in]  adc_code - Raw ADC code.
 * @param [in]  vref	 - The reference voltage of the ADC.
 * @param [out] voltage	 - Voltage value calculated.
 *
 */
void ltc2312_code_to_voltage(struct ltc2312_dev *dev, uint16_t adc_code,
			     float vref, float *voltage)
{
	uint8_t shift;

	*voltage = (float)adc_code;
	/* 2) This calculates the input as a fraction of the reference voltage
	*  (dimensionless)
	*  */
	shift = ltc2312_get_shift(dev);
	*voltage = *voltage / (pow(2, (16 - shift)) - 1);
	/* 3) Multiply fraction by Vref to get the actual voltage at the input
	*  (in volts)
	*  */
	*voltage = *voltage * vref;
}
