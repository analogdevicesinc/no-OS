/***************************************************************************//**
 *   @file   iio_ad4080.c
 *   @brief  Implementation of iio_ad4080.c.
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "no_os_alloc.h"

#include "iio_ad4080.h"

enum {
	RAW_ATTR_ID,
	SCALE_ATTR_ID,
	OFFSET_ATTR_ID,
	GP0_IO_GLOB_ATTR_ID,
	GP0_FUNC_GLOB_ATTR_ID,
	GP1_IO_GLOB_ATTR_ID,
	GP1_FUNC_GLOB_ATTR_ID,
	GP2_IO_GLOB_ATTR_ID,
	GP2_FUNC_GLOB_ATTR_ID,
	GP3_IO_GLOB_ATTR_ID,
	GP3_FUNC_GLOB_ATTR_ID,
	FIFO_MODE_GLOB_ATTR_ID,
	FIFO_FULL_GLOB_ATTR_ID,
	FIFO_READ_DONE_GLOB_ATTR_ID,
	FIFO_WATERMARK_GLOB_ATTR_ID,
	THRESHOLD_EVENT_DETECTED_ATTR_ID,
	EVT_DETECTION_HYSTERESIS_GLOB_ATTR_ID,
	EVT_DETECTION_HI_GLOB_ATTR_ID,
	EVT_DETECTION_LO_GLOB_ATTR_ID,
	FILTER_SEL_GLOB_ATTR_ID,
	FILTER_SINC_DEC_RATE_GLOB_ATTR_ID,
	DEVICE_MODE_GLOB_ATTR_ID,
	AFE_CTRL_GLOB_ATTR_ID,
	SELECT_SAMPLING_FREQ_GLOB_ATTR_ID,
	MAX_ATTR_ID,
};

typedef int (*attr_fn)(struct iio_ad4080_desc *iio_ad4080,
		       char *buf,
		       uint32_t len,
		       const struct iio_ch_info *ch_info,
		       bool show);

static const char *fifo_mode[] = {
	"disabled",
	"immediate_trigger_mode",
	"read_latest_watermark_mode",
	"read_all_FIFO_mode",
};

static const char *fifo_status[] = {
	"false",
	"true"
};

static const char *threshold_event_status[] = {
	"None",
	"lo",
	"hi",
	"lo_and_hi",
};

static const char *io_str[] = {
	"input",
	"output",
};

static const char *func_str[] = {
	"adi_nspi_sdo_data",   	       /* 0 */
	"gpio_fifo_full", 	       /* 1 */
	"gpio_fifo_read_done",         /* 2 */
	"gpio_filter_result_ready",    /* 3 */
	"gpio_ht_detect",              /* 4 */
	"gpio_lt_detect",              /* 5 */
	"gpio_status_alert", 	       /* 6 */
	"gpio_gpo_data", 	       /* 7 */
	"gpio_filter_synch_input",     /* 8 */
	"gpio_ext_event_trigger_fifo", /* 9 */
	"gpio_cnv_inhibit_input",      /* 10 */
};

static const char *filter_select[] = {
	"none",
	"sinc1",
	"sinc5",
	"sinc5+pf1",
};

static const char *decimation_factor[] = {
	"2",
	"4",
	"8",
	"16",
	"32",
	"64",
	"128",
	"256",
	"512",
	"1024",
};

static const char *operating_modes[] = {
	"Normal", 	/* 0 */
	"Standby", 	/* 2 */
	"Sleep", 	/* 3 */
};

static const char *afe_stat[] = {
	"disable",
	"enable",
};

/* only have one option for sampling frequency (hardcoded) */
static const unsigned long sampling_frequency[] = {
	40000000, /* 40 Mhz */
};

/**
 * @brief Formats the raw data read from the AD4080 FIFO into a more readable format.
 * @param data - The formatted data to be filled.
 * @param raw_data - The raw data read from the FIFO.
 * @param count - The number of samples to format.
 */
static void ad4080_format_raw_data(uint32_t *data, uint8_t *raw_data,
				   size_t count)
{
	size_t i;
	for (i = 0; i < count; i++) {
		uint8_t *p = &raw_data[3 * i + 1];
		data[i] = no_os_get_unaligned_be24(p);
	}
	return;
}

/**
 * @brief Reads the raw data from the AD4080 FIFO.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @return ret - The result of the read operation.
 *
 */
static int iio_ad4080_read_data(struct iio_ad4080_desc *iio_ad4080)
{
	struct ad4080_dev *dev = iio_ad4080->ad4080;
	struct iio_ad4080_fifo_struct *fifo;
	int err;

	/* first read raw fifo data */
	fifo = &iio_ad4080->fifo;
	err = ad4080_read_data(dev, fifo->raw_fifo, fifo->bufsize);
	if (err)
		return err;

	/* then condition the raw data to make it readable */
	ad4080_format_raw_data(fifo->formatted_fifo, fifo->raw_fifo,
			       fifo->watermark);

	return err;
}

/**
 * @brief Triggers the AD4080 FIFO to read data immediately.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 *
 * This function sets the FIFO mode to immediate trigger and waits for the FIFO to be filled.
 * It uses a completion structure to wait for the FIFO to be filled before returning.
 */
static int iio_ad4080_immediate_trigger(struct iio_ad4080_desc *iio_ad4080)
{
	int err;
	uint8_t value = NO_OS_GPIO_LOW;
	size_t retry_count = AD4080_FIFO_TIMEOUT;

	/* Arm the AD4080 FIFO and wait for done */
	ad4080_set_fifo_mode(iio_ad4080->ad4080, AD4080_IMMEDIATE_TRIGGER);

	/* wait FIFO done */
	while (retry_count > 0) {
		no_os_gpio_get_value(iio_ad4080->fifo.ff_full, &value);
		if (value == NO_OS_GPIO_HIGH)
			break;
		retry_count -= 1;
	};

	if (retry_count == 0) {
		/* AD4080 is not responsive anymore. we timeout the system. */
		err = -ETIMEDOUT;
		goto err_out;
	}

	err = iio_ad4080_read_data(iio_ad4080);

err_out:
	/* un-arm the AD4080 FIFO and wait for done */
	ad4080_set_fifo_mode(iio_ad4080->ad4080, AD4080_FIFO_DISABLE);

	return err;
}

/**
 * @brief Reads a register from the AD4080 device.
 * @param dev - The AD4080 device structure.
 * @param reg - The register address to read from.
 * @param readval - Pointer to store the read value.
 * @return
 */
static int32_t ad4080_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	if (reg > AD4080_LAST_REG_ADDR)
		return -EINVAL;
	return ad4080_read(dev, reg, (uint8_t *)readval);
}

/**
 * @brief Writes a value to a register in the AD4080 device.
 * @param dev - The AD4080 device structure.
 * @param reg - The register address to write to.
 * @param writeval - The value to write to the register.
 * @return ret - The result of the write operation.
 */
static int32_t ad4080_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	if (reg > AD4080_LAST_REG_ADDR)
		return -EINVAL;
	return ad4080_write(dev, reg, writeval);
}

/**
 * @brief Handles the raw attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the formatted data.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the data or not.
 */
static int raw_attr_handler(struct iio_ad4080_desc *iio_ad4080,
			    char *buf,
			    uint32_t len,
			    const struct iio_ch_info *ch_info,
			    bool show)
{
	int err = -1;
	struct iio_ad4080_fifo_struct *fifo;

	fifo = &iio_ad4080->fifo;
	if (show) {
		err = iio_ad4080_immediate_trigger(iio_ad4080);
		if (err)
			return err;
		err = sprintf(buf, "%d", fifo->formatted_fifo[0]);
	}
	return err;
}

/**
 * @brief Handles the scale attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the scale value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the scale value or not.
 * @return The formatted scale value as a string.
 */
static int scale_attr_handler(struct iio_ad4080_desc *iio_ad4080,
			      char *buf,
			      uint32_t len,
			      const struct iio_ch_info *ch_info,
			      bool show)
{
	const double ad4080_scale = AD4080_DEFAULT_SCALE;
	return sprintf(buf, "%10f", ad4080_scale);
}

/**
 * @brief Reads a 16-bit register from the AD4080 device.
 * @param ad4080 - The AD4080 device structure.
 * @param reg - The register address to read from.
 * @return The 16-bit value read from the register.
 */
static uint16_t ad4080_read16(struct ad4080_dev *dev, uint16_t reg)
{
	uint16_t val = 0;
	if (dev) {
		uint8_t tmp;
		ad4080_read(dev, reg + 1, &tmp);
		val = tmp;

		ad4080_read(dev, reg, &tmp);
		val = (val << 8) | tmp;
	}
	return val;
}

/**
 * @brief Writes a 16-bit value to a register in the AD4080 device.
 * @param ad4080 - The AD4080 device structure.
 * @param reg - The register address to write to.
 * @param reg_val - The 16-bit value to write to the register.
 */
static void ad4080_write16(struct ad4080_dev *dev, uint16_t reg,
			   uint16_t reg_val)
{
	if (dev) {
		uint8_t tmp;

		tmp = (reg_val >> 8) & 0xF;
		ad4080_write(dev, reg + 1, tmp);


		tmp = (uint8_t)(reg_val & 0xFF);
		ad4080_write(dev, reg, tmp);
	}
	return;
}

/**
 * @brief Reads the offset register from the AD4080 device.
 * @param ad4080 - The AD4080 device structure.
 * @return The offset value read from the register.
 */
static int16_t ad4080_read_offset(struct ad4080_dev *dev)
{
	int16_t reg_val = 0;
	if (dev) {
		reg_val = ad4080_read16(dev, AD4080_REG_OFFSET);
	}
	return reg_val & OFFSET_CORRECTION_NEGATIVE_LIMIT ? -reg_val : reg_val;
}

/**
 * @brief Writes an offset value to the AD4080 device.
 * @param ad4080 - The AD4080 device structure.
 * @param offset - The offset value to write to the register.
 */
static void ad4080_write_offset(struct ad4080_dev *dev, uint16_t offset)
{
	if (dev) {
		ad4080_write16(dev, AD4080_REG_OFFSET, offset);
	}
	return;
}

/**
 * @brief Handles the offset attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the offset value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the offset value or not.
 * @return The formatted offset value as a string or an error code.
 */
static int offset_attr_handler(struct iio_ad4080_desc *iio_ad4080,
			       char *buf,
			       uint32_t len,
			       const struct iio_ch_info *ch_info,
			       bool show)
{
	const double lsb = 0.00572;
	struct ad4080_dev *dev;
	double offset_correction_coefficient;
	const double max_offset_correction = 2047 * lsb;
	const double least_offset_correction = -2048 * lsb;
	uint16_t offset;
	dev = iio_ad4080->ad4080;

	if (show) {
		offset_correction_coefficient = (double)ad4080_read_offset(dev);
		offset_correction_coefficient *= lsb;
		return sprintf(buf, "%10f", offset_correction_coefficient);
	}

	offset_correction_coefficient = strtof(buf, NULL);

	if (offset_correction_coefficient > max_offset_correction)
		offset_correction_coefficient = max_offset_correction;
	if (offset_correction_coefficient < least_offset_correction)
		offset_correction_coefficient = least_offset_correction;

	offset = abs((int16_t)(offset_correction_coefficient / lsb));
	ad4080_write_offset(dev, offset);

	return 0;
}

/**
 * @brief Handles the GPIO input/output attribute for the AD4080 device.
 * @param ad4080 - The AD4080 device structure.
 * @param buf - The buffer to store the GPIO configuration.
 * @param len - The length of the buffer.
 * @param show - Whether to show the GPIO configuration or not.
 * @param gpio - The GPIO pin to configure (0-3).
 * @return The formatted GPIO configuration as a string or an error code.
 */
static int gpx_glob_io_attr_handler(struct ad4080_dev *dev,
				    char *buf,
				    uint32_t len,
				    bool show,
				    enum ad4080_gpio gpio)
{

	uint8_t config_a;
	size_t index;
	uint8_t mask;

	mask = 1 << gpio;
	ad4080_read(dev, AD4080_REG_GPIO_CONFIG_A, &config_a);
	if (show) {
		index = ((config_a & mask) == mask) ? 1 : 0;
		return sprintf(buf, "%s", *(io_str + index));
	}

	if (strcmp(buf, io_str[0]) == 0) {
		config_a &= ~mask;
	} else if (strcmp(buf, io_str[1]) == 0) {
		config_a |= mask;
	} else {
		return -1;
	}

	ad4080_write(dev, AD4080_REG_GPIO_CONFIG_A, config_a);

	return len;
}

/**
 * @brief Handles the GPIO input/output attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the GPIO configuration.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the GPIO configuration or not.
 * @return The formatted GPIO configuration as a string or an error code.
 */
static int gp0_io_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				    char *buf,
				    uint32_t len,
				    const struct iio_ch_info *ch_info,
				    bool show)
{
	return gpx_glob_io_attr_handler(iio_ad4080->ad4080, buf, len, show,
					AD4080_GPIO_0);
}

/**
 * @brief Handles the GPIO input/output attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the GPIO configuration.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the GPIO configuration or not.
 * @return The formatted GPIO configuration as a string or an error code.
 */
static int gp1_io_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				    char *buf,
				    uint32_t len,
				    const struct iio_ch_info *ch_info,
				    bool show)
{
	return gpx_glob_io_attr_handler(iio_ad4080->ad4080, buf, len, show,
					AD4080_GPIO_1);
}

/**
 * @brief Handles the GPIO input/output attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the GPIO configuration.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the GPIO configuration or not.
 * @return The formatted GPIO configuration as a string or an error code.
 */
static int gp2_io_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				    char *buf,
				    uint32_t len,
				    const struct iio_ch_info *ch_info,
				    bool show)
{
	return gpx_glob_io_attr_handler(iio_ad4080->ad4080, buf, len, show,
					AD4080_GPIO_2);
}

/**
 * @brief Handles the GPIO input/output attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the GPIO configuration.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the GPIO configuration or not.
 * @return The formatted GPIO configuration as a string or an error code.
 */
static int gp3_io_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				    char *buf,
				    uint32_t len,
				    const struct iio_ch_info *ch_info,
				    bool show)
{
	return gpx_glob_io_attr_handler(iio_ad4080->ad4080, buf, len, show,
					AD4080_GPIO_3);
}

/**
 * @brief Handles the function attribute for the AD4080 GPIO pins.
 * @param ad4080 - The AD4080 device structure.
 * @param buf - The buffer to store the function configuration.
 * @param len - The length of the buffer.
 * @param show - Whether to show the function configuration or not.
 * @param gpio - The GPIO pin to configure (0-3).
 * @return The formatted function configuration as a string or an error code.
 */
static int gpx_glob_func_attr_handler(struct ad4080_dev *dev,
				      char *buf,
				      uint32_t len,
				      bool show,
				      enum ad4080_gpio gpio)
{
	uint8_t config;
	uint16_t reg = AD4080_REG_GPIO_CONFIG_B;
	uint16_t shift = 0;
	uint16_t mask = 0x0F;
	size_t i;
	size_t count;
	int err;

	if (gpio > AD4080_GPIO_1)
		reg = AD4080_REG_GPIO_CONFIG_C;

	if ((gpio == AD4080_GPIO_1) || (gpio == AD4080_GPIO_3)) {
		shift = 4;
		mask = 0xF0;
	}

	ad4080_read(dev, reg, &config);
	if (show) {
		config = (config >> shift) & 0xF;
		return snprintf(buf, len, "%s", func_str[config]);
	}

	count = sizeof func_str / sizeof func_str[0];
	for (i = 0; i < count; i++) {
		if (strcmp(buf, func_str[i]) == 0) {
			config &= ~mask;
			config |= (i << shift);
			err = ad4080_write(dev, reg, config);
			if (err)
				return err;
			break;
		}
	}

	return len;
}

/**
 * @brief Handles the function attribute for the AD4080 GPIO pins.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the function configuration.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the function configuration or not.
 * @return The formatted function configuration as a string or an error code.
 */
static int gp0_func_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				      char *buf,
				      uint32_t len,
				      const struct iio_ch_info *ch_info,
				      bool show)
{
	return gpx_glob_func_attr_handler(iio_ad4080->ad4080, buf, len, show,
					  AD4080_GPIO_0);
}

/**
 * @brief Handles the function attribute for the AD4080 GPIO pins.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the function configuration.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the function configuration or not.
 * @return The formatted function configuration as a string or an error code.
 */
static int gp1_func_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				      char *buf,
				      uint32_t len,
				      const struct iio_ch_info *ch_info,
				      bool show)
{
	return gpx_glob_func_attr_handler(iio_ad4080->ad4080, buf, len, show,
					  AD4080_GPIO_1);
}

/**
 * @brief Handles the function attribute for the AD4080 GPIO pins.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the function configuration.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the function configuration or not.
 * @return The formatted function configuration as a string or an error code.
 */
static int gp2_func_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				      char *buf,
				      uint32_t len,
				      const struct iio_ch_info *ch_info,
				      bool show)
{
	return gpx_glob_func_attr_handler(iio_ad4080->ad4080, buf, len, show,
					  AD4080_GPIO_2);
}

/**
 * @brief Handles the function attribute for the AD4080 GPIO pins.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the function configuration.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the function configuration or not.
 * @return The formatted function configuration as a string or an error code.
 */
static int gp3_func_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				      char *buf,
				      uint32_t len,
				      const struct iio_ch_info *ch_info,
				      bool show)
{
	return gpx_glob_func_attr_handler(iio_ad4080->ad4080, buf, len, show,
					  AD4080_GPIO_3);
}

/**
 * @brief Handles the FIFO mode attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the FIFO mode.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the FIFO mode or not.
 * @return The formatted FIFO mode as a string or an error code.
 */
static int fifo_mode_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				       char *buf,
				       uint32_t len,
				       const struct iio_ch_info *ch_info,
				       bool show)
{
	enum ad4080_fifo_mode mode;
	unsigned long val;

	ad4080_get_fifo_mode(iio_ad4080->ad4080, &mode);
	if (show) {
		return sprintf(buf, "%s", *(fifo_mode + mode));
	}

	for (val = 0; val <= AD4080_EVENT_TRIGGER; val++) {
		if (!strcmp(buf, fifo_mode[val])) {
			break;
		}
	}

	if (val > AD4080_EVENT_TRIGGER)
		return -EINVAL;

	/* insert new fifo mode */
	return ad4080_set_fifo_mode(iio_ad4080->ad4080, val);
}

/**
 * @brief Handles the FIFO full attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the FIFO mode.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the FIFO mode or not.
 * @return The formatted FIFO mode as a string or an error code.
 */
static int fifo_full_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				       char *buf,
				       uint32_t len,
				       const struct iio_ch_info *ch_info,
				       bool show)
{
	int err;
	uint8_t val;

	err = ad4080_read(iio_ad4080->ad4080, AD4080_REG_DEVICE_STATUS, &val);
	if (err)
		return err;
	val = !!(val & (1 << 7));
	return sprintf(buf, "%s", fifo_status[val]);
}

/**
 * @brief Handles the FIFO full attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the FIFO mode.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the FIFO mode or not.
 * @return The formatted FIFO mode as a string or an error code.
 */
static int fifo_read_done_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
		char *buf,
		uint32_t len,
		const struct iio_ch_info *ch_info,
		bool show)
{
	int err;
	uint8_t val;

	err = ad4080_read(iio_ad4080->ad4080, AD4080_REG_DEVICE_STATUS, &val);
	if (err)
		return err;
	val = !!(val & (1 << 7));
	return sprintf(buf, "%s", fifo_status[val]);
}


/**
 * @brief Handles the FIFO watermark attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the FIFO watermark.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the FIFO watermark or not.
 * @return The formatted FIFO watermark as a string or an error code.
 */
static int fifo_watermark_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
		char *buf,
		uint32_t len,
		const struct iio_ch_info *ch_info,
		bool show)
{
	uint16_t watermark;
	unsigned long val;
	char *endptr;
	int base = 10;
	size_t input_len;

	if (show) {
		ad4080_get_fifo_watermark(iio_ad4080->ad4080, &watermark);
		return sprintf(buf, "%hd", watermark);
	}

	/* max range is 1 - AD4080_FIFO_DEPT */
	input_len = strlen(buf);
	if ((input_len > 2) && (buf[0] == '0') && (buf[1] == 'x')) {
		base = 16;
	}
	val = strtoul(buf, &endptr, base);
	/* invalid characters detected */
	if (*endptr != '\0')
		return -1;

	if ((val < 1 || val > AD4080_FIFO_DEPTH))
		return -1;

	return ad4080_set_fifo_watermark(iio_ad4080->ad4080, (uint16_t)val);
}

static int threshold_event_detected_glob_attr_handler(struct iio_ad4080_desc
		*iio_ad4080,
		char *buf,
		uint32_t len,
		const struct iio_ch_info *ch_info,
		bool show)
{
	int err;
	uint8_t val;
	err = ad4080_read(iio_ad4080->ad4080, AD4080_REG_DEVICE_STATUS, &val);
	if (err)
		return err;

	val = (val >> 4) & 0x3;
	return snprintf(buf, len, "%s", threshold_event_status[val]);
}

/**
 * @brief Reads the hysteresis value from the AD4080 device.
 * @param ad4080 - The AD4080 device structure.
 * @return The hysteresis value read from the register.
 */
static uint16_t ad4080_read_hysteresis(struct ad4080_dev *dev)
{
	uint16_t hyst = 0;
	uint8_t tmp;
	if (dev) {
		ad4080_read(dev, AD4080_REG_EVENT_HYSTERESIS + 1, &tmp);
		hyst = tmp << 8;

		ad4080_read(dev, AD4080_REG_EVENT_HYSTERESIS, &tmp);
		hyst = hyst | (tmp & 0xFF);
	}
	return hyst & 0x3FF;
}

/**
 * @brief Writes a hysteresis value to the AD4080 device.
 * @param ad4080 - The AD4080 device structure.
 * @param val - The hysteresis value to write to the register.
 *
 * This function writes a 10-bit hysteresis value to the AD4080 device, ensuring that
 * the upper 5 bits are zeroed out before writing.
 */
static void ad4080_write_hysteresis(struct ad4080_dev *dev, uint16_t val)
{
	uint8_t tmp;
	val = val & 0x3FF; /* zero out the upper 5 bits */
	if (dev) {
		tmp = val >> 8;
		ad4080_write(dev, AD4080_REG_EVENT_HYSTERESIS + 1, tmp);

		tmp = val & 0xFF;
		ad4080_write(dev, AD4080_REG_EVENT_HYSTERESIS, tmp);
	}
	return;
}

/**
 * @brief Handles the hysteresis attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the hysteresis value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the hysteresis value or not.
 * @return The formatted hysteresis value as a string or an error code.
 */
static int evt_detection_hysteresis_glob_attr_handler(struct iio_ad4080_desc
		*iio_ad4080,
		char *buf,
		uint32_t len,
		const struct iio_ch_info *ch_info,
		bool show)
{
	double hyst;
	const double lsb = 1.46484;
	const double max_hyst = 0x3FF * lsb;
	uint16_t val;

	if (show) {
		hyst = (double)ad4080_read_hysteresis(iio_ad4080->ad4080);
		hyst = hyst * lsb;
		return sprintf(buf, "%10f", hyst);
	}

	/* hysteresis in mV, clip value to maximum possible mV only */
	hyst = strtof(buf, NULL);
	if (hyst > max_hyst)
		hyst = max_hyst;
	val = (uint16_t)(hyst / lsb);
	ad4080_write_hysteresis(iio_ad4080->ad4080, val);
	return len;
}

/**
 * @brief Reads the event detection value from the AD4080 device.
 * @param ad4080 - The AD4080 device structure.
 * @param hi - Whether to read the high or low event detection value.
 * @return The event detection value read from the register, sign-extended if necessary.
 */
static int16_t ad4080_read_evt_detection(struct ad4080_dev *dev, bool hi)
{
	uint16_t evt_detection_reg = AD4080_REG_EVENT_DETECTION_HI;
	int16_t reg_val;

	if (!dev)
		return 0;

	if (!hi)
		evt_detection_reg = AD4080_REG_EVENT_DETECTION_LO;

	reg_val = ad4080_read16(dev, evt_detection_reg);

	return reg_val & OFFSET_CORRECTION_NEGATIVE_LIMIT ? -reg_val : reg_val;
}

/**
 * @brief Writes an event detection value to the AD4080 device.
 * @param ad4080 - The AD4080 device structure.
 * @param reg_val - The event detection value to write to the register.
 * @param hi - Whether to write to the high or low event detection register.
 *
 * This function writes a 16-bit event detection value to the AD4080 device, either
 * to the high or low event detection register based on the 'hi' parameter.
 */
static void ad4080_write_evt_detection(struct ad4080_dev *dev, uint16_t reg_val,
				       bool hi)
{
	uint16_t evt_detection_reg = AD4080_REG_EVENT_DETECTION_HI;
	if (!dev)
		return;
	if (!hi)
		evt_detection_reg = AD4080_REG_EVENT_DETECTION_LO;
	ad4080_write16(dev, evt_detection_reg, reg_val);
	return ;
}

/**
 * @brief Handles the event detection high attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the event detection high value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the event detection high value or not.
 * @return The formatted event detection high value as a string or an error code.
 */
static int evt_detection_hi_glob_attr_handler(struct iio_ad4080_desc
		*iio_ad4080,
		char *buf,
		uint32_t len,
		const struct iio_ch_info *ch_info,
		bool show)
{
	int16_t reg_val;
	const double lsb = 1.46484;
	const double least_value = -2048.0l * lsb;
	const double max_value = 2047.0l * lsb;
	double evt_detection_hi;
	if (show) {
		reg_val = ad4080_read_evt_detection(iio_ad4080->ad4080, true);
		evt_detection_hi = reg_val * lsb;
		return sprintf(buf, "%10f", evt_detection_hi);
	}

	evt_detection_hi = strtof(buf, NULL);
	if (evt_detection_hi > max_value)
		evt_detection_hi = max_value;
	else if (evt_detection_hi < least_value)
		evt_detection_hi = least_value;

	reg_val = abs((int16_t)(evt_detection_hi / lsb));
	ad4080_write_evt_detection(iio_ad4080->ad4080, reg_val, true);
	return len;
}

/**
 * @brief Handles the event detection low attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the event detection low value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the event detection low value or not.
 * @return The formatted event detection low value as a string or an error code.
 */
static int evt_detection_lo_glob_attr_handler(struct iio_ad4080_desc
		*iio_ad4080,
		char *buf,
		uint32_t len,
		const struct iio_ch_info *ch_info,
		bool show)
{
	int16_t reg_val;
	const double lsb = 1.46484;
	const double least_value = -2048.0l * lsb;
	const double max_value = 2047.0l * lsb;
	double evt_detection_lo;
	if (show) {
		reg_val = ad4080_read_evt_detection(iio_ad4080->ad4080, false);
		evt_detection_lo = reg_val * lsb;
		return sprintf(buf, "%10f", evt_detection_lo);
	}

	evt_detection_lo = strtof(buf, NULL);
	if (evt_detection_lo > max_value)
		evt_detection_lo = max_value;
	else if (evt_detection_lo < least_value)
		evt_detection_lo = least_value;

	reg_val = abs((int16_t)(evt_detection_lo / lsb));
	ad4080_write_evt_detection(iio_ad4080->ad4080, reg_val, false);
	return len;
}

/**
 * @brief Handles the filter selection attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the filter selection.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the filter selection or not.
 * @return The formatted filter selection as a string or an error code.
 */
static int filter_sel_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
					char *buf,
					uint32_t len,
					const struct iio_ch_info *ch_info,
					bool show)
{
	int err;
	uint8_t reg_val;
	size_t i;
	size_t count;
	uint8_t decimation;

	ad4080_read(iio_ad4080->ad4080, AD4080_REG_FILTER_CONFIG, &reg_val);
	if (show) {
		reg_val &= AD4080_FILTER_SEL_MSK;
		return snprintf(buf, len, "%s", filter_select[reg_val]);
	}

	decimation = no_os_field_get(AD4080_SINC_DECIMATION_MSK, reg_val);
	count = sizeof filter_select / sizeof filter_select[0];
	for (i = 0; i < count; i++) {
		if (strcmp(buf, filter_select[i]) == 0) {
			reg_val &= ~AD4080_FILTER_SEL_MSK;
			reg_val |= no_os_field_prep(AD4080_FILTER_SEL_MSK, i);

			/* for selected filter, setting invalid values outside
			 * of those supported will set the filter at maximum
			 * value. 512 and 1024 are only valid when filter
			 * is sinc1. */
			if ((i != AD4080_FILTER_SINC1) && (decimation > AD4080_SINC_DEC_256)) {
				reg_val &= ~AD4080_SINC_DECIMATION_MSK;
				reg_val |= no_os_field_prep(AD4080_SINC_DECIMATION_MSK, AD4080_SINC_DEC_256);
			}

			err = ad4080_write(iio_ad4080->ad4080,
					   AD4080_REG_FILTER_CONFIG,
					   reg_val);
			if (err)
				return err;
			break;
		}
	}

	return len;
}

/**
 * @brief Handles the sinc decimation attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the sinc decimation value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the sinc decimation value or not.
 * @return The formatted sinc decimation value as a string or an error code.
 */
static int filter_sinc_dec_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
		char *buf,
		uint32_t len,
		const struct iio_ch_info *ch_info,
		bool show)
{
	int err;
	uint8_t reg_val;
	uint8_t shift = 3;
	size_t i;
	size_t count;
	uint8_t filter_sel;

	ad4080_read(iio_ad4080->ad4080, AD4080_REG_FILTER_CONFIG, &reg_val);
	if (show) {
		reg_val = (reg_val & AD4080_SINC_DECIMATION_MSK) >> shift;
		return sprintf(buf, "%s", decimation_factor[reg_val]);
	}

	filter_sel = no_os_field_get(AD4080_FILTER_SEL_MSK, reg_val);
	count = sizeof decimation_factor / sizeof decimation_factor[0];
	for (i = 0; i < count; i++) {
		if (strcmp(buf, decimation_factor[i]) == 0) {

			/* for selected filter, setting invalid values outside
			 * of those supported will set the filter at maximum
			 * value. 512 and 1024 are only valid when filter
			 * is sinc1. */
			if ((i > AD4080_SINC_DEC_256) && (filter_sel != AD4080_FILTER_SINC1))
				i = AD4080_SINC_DEC_256;

			reg_val = (reg_val & ~AD4080_SINC_DECIMATION_MSK) | (i << shift);
			err = ad4080_write(iio_ad4080->ad4080,
					   AD4080_REG_FILTER_CONFIG,
					   reg_val);
			if (err)
				return err;

			break;
		}
	}
	return len;
}

/**
 * @brief Handles the device mode attribute for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param buf - The buffer to store the device mode.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param show - Whether to show the device mode or not.
 * @return The formatted device mode as a string or an error code.
 */
static int device_mode_glob_attr_handler(struct iio_ad4080_desc *iio_ad4080,
		char *buf,
		uint32_t len,
		const struct iio_ch_info *ch_info,
		bool show)
{
	int err;
	uint8_t reg_val;
	size_t i;
	size_t count;

	ad4080_read(iio_ad4080->ad4080, AD4080_REG_DEVICE_CONFIG, &reg_val);
	if (show) {
		uint8_t mode = reg_val & AD4080_OP_MODE_MSK;
		switch (mode) {
		case 0:
			return sprintf(buf, "%s", operating_modes[0]);
		case 2:
			return sprintf(buf, "%s", operating_modes[1]);
		case 3:
			return sprintf(buf, "%s", operating_modes[2]);
		default:
			return -1;
		}
	}

	count = sizeof operating_modes / sizeof operating_modes[0];
	for (i = 0; i < count; i++) {
		if (strcmp(buf, operating_modes[i]) == 0) {
			switch (i) {
			case 0:
			case 2:
			case 3:
				reg_val = (reg_val & ~AD4080_OP_MODE_MSK) | i;
				break;
			default:
				return -1;
			}

			err = ad4080_write(iio_ad4080->ad4080,
					   AD4080_REG_DEVICE_CONFIG,
					   reg_val);
			if (err)
				return err;

			break;
		}
	}

	return len;
}

/**
 * @brief handle AFE control attributes
 * @param device - The IIO device structure.
 * @param buf - The buffer to store the attribute value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param priv - The private data associated with the attribute.
 * @return The result of the attribute operation (success or error code).
 */
static int afe_ctrl_attr_handler(struct iio_ad4080_desc *iio_ad4080,
				 char *buf,
				 uint32_t len,
				 const struct iio_ch_info *ch_info,
				 bool show)
{
	int err = -1;
	uint8_t val;

	if (show) {
		no_os_gpio_get_value(iio_ad4080->afe_ctrl, &val);
		return sprintf(buf, "%s", afe_stat[val]);
	}

	if (strcmp(buf, afe_stat[0]) == 0)
		err = no_os_gpio_set_value(iio_ad4080->afe_ctrl, 0);
	else if (strcmp(buf, afe_stat[1]) == 0)
		err = no_os_gpio_set_value(iio_ad4080->afe_ctrl, 1);

	if (err)
		return err;

	return len;
}

/**
 * @brief sampling frequency control attributes
 * @param device - The IIO device structure.
 * @param buf - The buffer to store the attribute value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param priv - The private data associated with the attribute.
 * @return The result of the attribute operation (success or error code).
 */
static int select_sampling_freq_attr_handler(struct iio_ad4080_desc *iio_ad4080,
		char *buf,
		uint32_t len,
		const struct iio_ch_info *ch_info,
		bool show)
{
	if (show) {
		return sprintf(buf, "%lu",
			       sampling_frequency[iio_ad4080->sampling_frequency_idx]);
	}

	/* cannot set sampling frequency. hardcoded to 40Mhz */
	return len;
}

static attr_fn attr_handlers[] = {
	raw_attr_handler,
	scale_attr_handler,
	offset_attr_handler,
	gp0_io_glob_attr_handler,
	gp0_func_glob_attr_handler,
	gp1_io_glob_attr_handler,
	gp1_func_glob_attr_handler,
	gp2_io_glob_attr_handler,
	gp2_func_glob_attr_handler,
	gp3_io_glob_attr_handler,
	gp3_func_glob_attr_handler,
	fifo_mode_glob_attr_handler,
	fifo_full_glob_attr_handler,
	fifo_read_done_glob_attr_handler,
	fifo_watermark_glob_attr_handler,
	threshold_event_detected_glob_attr_handler,
	evt_detection_hysteresis_glob_attr_handler,
	evt_detection_hi_glob_attr_handler,
	evt_detection_lo_glob_attr_handler,
	filter_sel_glob_attr_handler,
	filter_sinc_dec_glob_attr_handler,
	device_mode_glob_attr_handler,
	afe_ctrl_attr_handler,
	select_sampling_freq_attr_handler,
};

/**
 * @brief Stores or shows the attribute value for the AD4080 device.
 * @param device - The IIO device structure.
 * @param buf - The buffer to store the attribute value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param priv - The private data associated with the attribute.
 * @return The result of the attribute operation (success or error code).
 */
static int ad4080_attr_store(void *device,
			     char *buf,
			     uint32_t len,
			     const struct iio_ch_info *ch_info,
			     intptr_t priv)
{
	int err = -1;
	struct iio_ad4080_desc *iio_ad4080;
	iio_ad4080 = ad4080_privdata(device);
	if (priv < MAX_ATTR_ID)
		err = attr_handlers[priv](iio_ad4080, buf, len, ch_info, false);
	return err;
}

/**
 * @brief Shows the attribute value for the AD4080 device.
 * @param device - The IIO device structure.
 * @param buf - The buffer to store the attribute value.
 * @param len - The length of the buffer.
 * @param ch_info - The channel information.
 * @param priv - The private data associated with the attribute.
 * @return The result of the attribute operation (success or error code).
 */
static int ad4080_attr_show(void *device,
			    char *buf,
			    uint32_t len,
			    const struct iio_ch_info *ch_info,
			    intptr_t priv)
{
	int err = -1;
	struct iio_ad4080_desc *iio_ad4080;
	iio_ad4080 = ad4080_privdata(device);
	if (priv < MAX_ATTR_ID)
		err = attr_handlers[priv](iio_ad4080, buf, len, ch_info, true);
	return err;
}

static int ad4080_attr_avail_show(void *device,
				  char *buf,
				  uint32_t len,
				  const struct iio_ch_info *ch_info,
				  intptr_t priv)
{
	switch (priv) {
	case FIFO_MODE_GLOB_ATTR_ID:
		return snprintf(buf, len, "%s %s %s %s",
				fifo_mode[0],
				fifo_mode[1],
				fifo_mode[2],
				fifo_mode[3]);

	case FIFO_FULL_GLOB_ATTR_ID:
		return snprintf(buf, len, "%s %s",
				fifo_status[0],
				fifo_status[1]);

	case FIFO_READ_DONE_GLOB_ATTR_ID:
		return snprintf(buf, len, "%s %s",
				fifo_status[0],
				fifo_status[1]);

	case THRESHOLD_EVENT_DETECTED_ATTR_ID:
		return snprintf(buf, len, "%s %s %s %s",
				threshold_event_status[0],
				threshold_event_status[1],
				threshold_event_status[2],
				threshold_event_status[3]);
	case GP0_IO_GLOB_ATTR_ID:
	case GP1_IO_GLOB_ATTR_ID:
	case GP2_IO_GLOB_ATTR_ID:
	case GP3_IO_GLOB_ATTR_ID:
		return snprintf(buf, len, "%s %s",
				io_str[0],
				io_str[1]);

	case GP0_FUNC_GLOB_ATTR_ID:
	case GP1_FUNC_GLOB_ATTR_ID:
	case GP2_FUNC_GLOB_ATTR_ID:
	case GP3_FUNC_GLOB_ATTR_ID:
		return snprintf(buf, len, "%s %s %s %s %s %s %s %s %s %s",
				func_str[0],
				func_str[1],
				func_str[2],
				func_str[3],
				func_str[4],
				func_str[5],
				func_str[6],
				func_str[7],
				func_str[8],
				func_str[9]);

	case FILTER_SEL_GLOB_ATTR_ID:
		return snprintf(buf, len, "%s %s %s %s",
				filter_select[0],
				filter_select[1],
				filter_select[2],
				filter_select[3]);

	case FILTER_SINC_DEC_RATE_GLOB_ATTR_ID:
		return snprintf(buf, len, "%s %s %s %s %s %s %s %s %s %s",
				decimation_factor[0],
				decimation_factor[1],
				decimation_factor[2],
				decimation_factor[3],
				decimation_factor[4],
				decimation_factor[5],
				decimation_factor[6],
				decimation_factor[7],
				decimation_factor[8],
				decimation_factor[9]);

	case DEVICE_MODE_GLOB_ATTR_ID:
		return snprintf(buf, len, "%s %s %s",
				operating_modes[0],
				operating_modes[1],
				operating_modes[2]);

	case AFE_CTRL_GLOB_ATTR_ID:
		return snprintf(buf, len, "%s %s",
				afe_stat[0],
				afe_stat[1]);

	case SELECT_SAMPLING_FREQ_GLOB_ATTR_ID:
		return snprintf(buf, len, "%lu",
				sampling_frequency[0]);
	}

	return len;
}

static int32_t iio_ad4080_prepare_transfer(void *dev, uint32_t mask)
{
	return 0;
}

static int32_t iio_ad4080_end_transfer(void *dev)
{
	return 0;
}

/* some clarifying points here.
 * 1) 1 channel (zero possibility to for any interleave to happen)
 * 2) 20-bit is extended to 32-bits (4 bytes)
 * 3) Raw fifo data needs to be formatted before it's usable.
 *    3.1) as simple as to just remove the 0xAA synchro starting byte
 * 4) buffer is passed by the upper layer as a circular buffer.
 *    4.1) cannot be memcpy'd directly.
 *    4.2) need to use no os circular buffer facilities to transfer data
 */

/**
 * @brief Unset the watermark configuration and data structure fors ad4080 driver.
 * @param fifo - Pointer to the IIO AD4080 FIFO to unset.
 */
void iio_ad4080_fifo_unset_watermark(struct iio_ad4080_fifo_struct *fifo)
{
	/* turn off the fifo */
	ad4080_set_fifo_mode(fifo->ad4080,
			     AD4080_FIFO_DISABLE);

	if (fifo->formatted_fifo) {
		no_os_free(fifo->formatted_fifo);
		fifo->formatted_fifo = NULL;
		fifo->formatted_bufsize = 0;
	}

	if (fifo->raw_fifo) {
		no_os_free(fifo->raw_fifo);
		fifo->raw_fifo = NULL;
		fifo->bufsize = 0;
	}

	return;
}

/**
 * @brief Sets the FIFO watermark for the AD4080 device.
 * @param fifo - Pointer to the IIO AD4080 FIFO structure.
 * @param watermark - The desired watermark value (number of samples).
 * @return 0 on success, or an error code on failure.
 */
int iio_ad4080_fifo_set_watermark(struct iio_ad4080_fifo_struct *fifo,
				  const size_t watermark)
{
	int err;
	size_t fifo_size;
	size_t formatted_bufsize;
	uint8_t *raw_fifo;
	uint32_t *formatted_fifo;

	if (fifo == NULL)
		return -EINVAL;

	if ((watermark < 1) || (watermark > 16384)) {
		return -EINVAL;
	}

	iio_ad4080_fifo_unset_watermark(fifo);

	fifo_size = NO_OS_DIV_ROUND_UP(AD4080_ADC_GRANULARITY, 8);
	fifo_size = (fifo_size * watermark);
	fifo_size = fifo_size + 1; /* account for the 0xAA synchro byte */

	raw_fifo = no_os_malloc(fifo_size);
	if (!raw_fifo) {
		return -ENOMEM;
	}
	fifo->raw_fifo = raw_fifo;
	fifo->watermark = watermark;
	fifo->bufsize = fifo_size;

	formatted_bufsize = watermark * sizeof(uint32_t);
	formatted_fifo = no_os_malloc(formatted_bufsize);
	if (!formatted_fifo)
		goto err_malloc_formatted_fifo;
	fifo->formatted_fifo = formatted_fifo;
	fifo->formatted_bufsize = formatted_bufsize;

	err = ad4080_set_fifo_watermark(fifo->ad4080, fifo->watermark);
	if (err) {
		goto err_set_fifo_watermark;
	}

	return 0;
err_set_fifo_watermark:
	no_os_free(formatted_fifo);
err_malloc_formatted_fifo:
	no_os_free(raw_fifo);
	return err;
}

/**
 * @brief Submits the IIO device data for the AD4080 device.
 * @param iio_device_data - The IIO device data structure containing the device and buffer information.
 * @return 0 on success, or an error code on failure.
 */
static int32_t ad4080_submit(struct iio_device_data *iio_device_data)
{
	struct ad4080_dev *dev = iio_device_data->dev;
	struct iio_ad4080_desc *iio_ad4080 = ad4080_privdata(dev);
	struct iio_ad4080_fifo_struct *fifo = &iio_ad4080->fifo;
	uint32_t samples;
	int err;

	samples = iio_device_data->buffer->size /
		  iio_device_data->buffer->bytes_per_scan;

	/* see if we need to update our watermark based on sample size
	 * request from host */
	if (fifo->watermark != samples) {
		iio_ad4080_fifo_unset_watermark(fifo);
		err = iio_ad4080_fifo_set_watermark(fifo, samples);
		if (err)
			return err;
	}

	err = iio_ad4080_immediate_trigger(iio_ad4080);
	if (err)
		return err;

	err = no_os_cb_write(iio_device_data->buffer->buf,
			     fifo->formatted_fifo,
			     iio_device_data->buffer->size);
	if (err)
		return err;

	return 0;
}

#define IIO_AD4080_CH_ATTR(_name, _priv) 	\
	{ 					\
		.name = _name, 			\
		.priv = _priv, 			\
		.show = ad4080_attr_show, 	\
		.store = ad4080_attr_store, 	\
	}

#define IIO_AD4080_GLOB_ATTR(_name, _priv) 	\
	{ 					\
		.name = _name, 			\
		.priv = _priv, 			\
		.show = ad4080_attr_show, 	\
		.store = ad4080_attr_store, 	\
	}

#define IIO_AD4080_GLOB_ATTR_AVAIL(_name, _priv) \
	{ 					\
		.name = _name, 			\
		.priv = _priv, 			\
		.show = ad4080_attr_avail_show, \
		.store = NULL, \
	}

static struct iio_attribute ad4080_ch_attr[] = {
	IIO_AD4080_CH_ATTR("raw", RAW_ATTR_ID),
	IIO_AD4080_CH_ATTR("scale", SCALE_ATTR_ID),
	IIO_AD4080_CH_ATTR("offset", OFFSET_ATTR_ID),
	{0},
};

static struct iio_attribute ad4080_global_attr[] = {
	IIO_AD4080_GLOB_ATTR("gpio0_output_enable", GP0_IO_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("gpio0_output_enable_available", GP0_IO_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("gpio0_output_func_sel", GP0_FUNC_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("gpio0_output_func_sel_available", GP0_FUNC_GLOB_ATTR_ID),

	IIO_AD4080_GLOB_ATTR("gpio1_output_enable", GP1_IO_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("gpio1_output_enable_available", GP1_IO_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("gpio1_output_func_sel", GP1_FUNC_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("gpio1_output_func_sel_available", GP1_FUNC_GLOB_ATTR_ID),

	IIO_AD4080_GLOB_ATTR("gpio2_output_enable", GP2_IO_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("gpio2_output_enable_available", GP2_IO_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("gpio2_output_func_sel", GP2_FUNC_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("gpio2_output_func_sel_available", GP2_FUNC_GLOB_ATTR_ID),

	IIO_AD4080_GLOB_ATTR("gpio3_output_enable", GP3_IO_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("gpio3_output_enable_available", GP3_IO_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("gpio3_output_func_sel", GP3_FUNC_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("gpio3_output_func_sel_available", GP3_FUNC_GLOB_ATTR_ID),

	IIO_AD4080_GLOB_ATTR("fifo_mode", FIFO_MODE_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("fifo_mode_available", FIFO_MODE_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("fifo_full", FIFO_FULL_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("fifo_full_available", FIFO_FULL_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("fifo_read_done", FIFO_READ_DONE_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("fifo_read_done_available", FIFO_READ_DONE_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("fifo_watermark", FIFO_WATERMARK_GLOB_ATTR_ID),

	IIO_AD4080_GLOB_ATTR("threshold_event_detected", THRESHOLD_EVENT_DETECTED_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("threshold_event_detected_available", THRESHOLD_EVENT_DETECTED_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("hysteresis_mv", EVT_DETECTION_HYSTERESIS_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("hi_threshold_mv", EVT_DETECTION_HI_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("lo_threshold_mv", EVT_DETECTION_LO_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("filter_type", FILTER_SEL_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("filter_type_available", FILTER_SEL_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR("oversampling_ratio", FILTER_SINC_DEC_RATE_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("oversampling_ratio_available", FILTER_SINC_DEC_RATE_GLOB_ATTR_ID),

	IIO_AD4080_GLOB_ATTR("operating_mode", DEVICE_MODE_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("operating_mode_available", DEVICE_MODE_GLOB_ATTR_ID),

	IIO_AD4080_GLOB_ATTR("afe_ctrl", AFE_CTRL_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("afe_ctrl_available", AFE_CTRL_GLOB_ATTR_ID),

	IIO_AD4080_GLOB_ATTR("sampling_frequency", SELECT_SAMPLING_FREQ_GLOB_ATTR_ID),
	IIO_AD4080_GLOB_ATTR_AVAIL("sampling_frequency_available", SELECT_SAMPLING_FREQ_GLOB_ATTR_ID),

	{0},
};

static struct scan_type ad4080_scan_type = {
	.sign = 's',
	.realbits = AD4080_ADC_GRANULARITY,
	.storagebits = 32, /* round up to 4 bytes */
	.shift = 0,
	.is_big_endian = false
};

static struct iio_channel ad4080_ch = {
	.name = "voltage",
	.ch_type = IIO_VOLTAGE,
	.channel = 0,
	.scan_index = 0,
	.indexed = true,
	.scan_type = &ad4080_scan_type,
	.attributes = ad4080_ch_attr,
	.ch_out = false,
};

/**
 * @brief Gets the AD4080 device from the IIO AD4080 descriptor.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param ad4080 - Pointer to store the AD4080 device pointer.
 * @return 0 on success, or an error code on failure.
 */
int ad4080_device(struct iio_ad4080_desc *iio_ad4080,
		  struct ad4080_dev **ad4080)
{
	if (!iio_ad4080 || !ad4080)
		return -EINVAL;
	*ad4080 = iio_ad4080->ad4080;
	return 0;
}

/**
 * @brief Initializes the IIO device structure for the AD4080 device.
 * @param iio_ad4080 - The IIO AD4080 descriptor.
 * @param iio_device - The IIO device structure to be initialized.
 * @return 0 on success, or an error code on failure.
 */
int ad4080_iio_device(struct iio_ad4080_desc *iio_ad4080,
		      struct iio_device *iio_device)
{
	if (!iio_ad4080 || !iio_device)
		return -EINVAL;

	iio_device->num_ch = 1;
	iio_device->channels = &ad4080_ch;
	iio_device->attributes = ad4080_global_attr;
	iio_device->debug_attributes = NULL;
	iio_device->buffer_attributes = NULL;
	iio_device->pre_enable = iio_ad4080_prepare_transfer;
	iio_device->post_disable = iio_ad4080_end_transfer;
	iio_device->submit = ad4080_submit;
	iio_device->debug_reg_read = ad4080_reg_read;
	iio_device->debug_reg_write = ad4080_reg_write;

	return 0;
}

/**
 * @brief Initializes the IIO AD4080 FIFO structure.
 * @return 0 on success, or an error code on failure.
 */
static int iio_ad4080_fifo_init(struct iio_ad4080_desc *ad4080_iio,
				struct ad4080_dev *dev,
				struct iio_ad4080_init_param *ip)
{
	int err;
	struct iio_ad4080_fifo_struct *fifo;

	fifo  = &ad4080_iio->fifo;
	fifo->ad4080 = dev;
	fifo->raw_fifo = NULL;
	fifo->bufsize = 0;
	fifo->formatted_fifo = NULL;
	fifo->formatted_bufsize = 0;
	fifo->i_gp = ip->i_gp;

	/* configure the GPIO FIFO full line */
	err = no_os_gpio_get(&fifo->ff_full, ip->ff_full_init_param);
	if (err)
		return err;

	err = no_os_gpio_direction_input(fifo->ff_full);
	if (err)
		goto err_gpio_input;

	/* now we can configure the FIFO full on the AD4080 side */
	err = ad4080_set_gpio_output_enable(dev, ip->i_gp, AD4080_GPIO_OUTPUT);
	if (err)
		goto err_gpio_input;

	err = ad4080_set_gpio_output_func(dev, ip->i_gp, AD4080_GPIO_FIFO_FULL);
	if (err)
		goto err_gpio_input;

	return 0;

err_gpio_input:
	no_os_gpio_remove(fifo->ff_full);
	return err;
}

/**
 * @brief De-initializes the IIO AD4080 FIFO structure.
 * @param fifo - Pointer to the IIO AD4080 FIFO structure to deinitialize.
 * @return none .
 */
static void iio_ad4080_fifo_fini(struct iio_ad4080_fifo_struct *fifo)
{
	assert(fifo != NULL);
	no_os_gpio_remove(fifo->ff_full);
	return;
}

/**
 * @brief Initializes the IIO AD4080 descriptor.
 * @param iio_ad4080 - Pointer to store the initialized IIO AD4080 descriptor.
 * @param iio_ad4080_init_param - Initialization parameters for the IIO AD4080 descriptor.
 * @return 0 on success, or an error code on failure.
 */
int iio_ad4080_init(struct iio_ad4080_desc **iio_ad4080,
		    struct iio_ad4080_init_param *iio_ad4080_init_param)
{
	int err;
	struct ad4080_dev *dev;
	struct ad4080_init_param *ad4080_init_param;
	struct iio_ad4080_desc *ad4080_iio;

	if (!iio_ad4080)
		return -EINVAL;

	if (!iio_ad4080_init_param)
		return -EINVAL;

	ad4080_init_param = iio_ad4080_init_param->ad4080_init_param;

	ad4080_init_param->privdata_len = sizeof(struct iio_ad4080_desc);
	err = ad4080_init(&dev, *ad4080_init_param);
	if (err)
		return err;

	ad4080_iio = ad4080_privdata(dev);
	ad4080_iio->ad4080 = dev;
	err = iio_ad4080_fifo_init(ad4080_iio, dev, iio_ad4080_init_param);
	if (err)
		goto err_iio_ad4080_fifo_init;

	err = iio_ad4080_fifo_set_watermark(&ad4080_iio->fifo,
					    iio_ad4080_init_param->watermark);
	if (err)
		goto err_set_watermark;

	/* initialize the AFE control - default disabled */
	if (!iio_ad4080_init_param->afe_ctrl_init_param)
		goto err_set_watermark;

	err = no_os_gpio_get(&ad4080_iio->afe_ctrl,
			     iio_ad4080_init_param->afe_ctrl_init_param);
	if (err)
		goto err_afe_ctrl;
	no_os_gpio_direction_output(ad4080_iio->afe_ctrl, NO_OS_GPIO_HIGH);

	/* lastly, hardcoded sampling frequency of 0 = 40Mhz */
	ad4080_iio->sampling_frequency_idx = 0;

	*iio_ad4080 = ad4080_iio;

	return 0;

err_afe_ctrl:
	iio_ad4080_fifo_unset_watermark(&ad4080_iio->fifo);
err_set_watermark:
	iio_ad4080_fifo_fini(&ad4080_iio->fifo);
err_iio_ad4080_fifo_init:
	ad4080_remove(dev);
	return err;
}

void iio_ad4080_fini(struct iio_ad4080_desc *iio_ad4080)
{
	struct ad4080_dev *dev;
	struct iio_ad4080_fifo_struct *fifo;

	if (!iio_ad4080)
		return;

	dev = iio_ad4080->ad4080;
	fifo = &iio_ad4080->fifo;

	iio_ad4080_fifo_unset_watermark(fifo);
	iio_ad4080_fifo_fini(fifo);
	ad4080_remove(dev);
	return;
}
