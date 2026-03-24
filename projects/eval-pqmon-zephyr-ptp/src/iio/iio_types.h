/********************************************************************************
 *   @file   iio_types.h
 *   @brief  IIO type definitions adapted from no-OS
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

#ifndef IIO_TYPES_H_
#define IIO_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

/* Forward declarations to replace no-OS dependencies */
struct no_os_circular_buffer;
struct no_os_irq_ctrl_desc;

enum iio_val {
	IIO_VAL_INT = 1,
	IIO_VAL_INT_PLUS_MICRO,
	IIO_VAL_INT_PLUS_NANO,
	IIO_VAL_INT_PLUS_MICRO_DB,
	IIO_VAL_INT_MULTIPLE,
	IIO_VAL_FRACTIONAL = 10,
	IIO_VAL_FRACTIONAL_LOG2,
	IIO_VAL_CHAR
};

enum iio_chan_type {
	IIO_VOLTAGE,
	IIO_CURRENT,
	IIO_ALTVOLTAGE,
	IIO_ANGL_VEL,
	IIO_TEMP,
	IIO_CAPACITANCE,
	IIO_ACCEL,
	IIO_RESISTANCE,
	IIO_MAGN,
	IIO_INCLI,
	IIO_VELOCITY,
	IIO_ANGL,
	IIO_ROT,
	IIO_COUNT,
	IIO_DELTA_ANGL,
	IIO_DELTA_VELOCITY,
};

enum iio_modifier {
	IIO_NO_MOD,
	IIO_MOD_X,
	IIO_MOD_Y,
	IIO_MOD_Z,
	IIO_MOD_TEMP_AMBIENT,
	IIO_MOD_PITCH,
	IIO_MOD_YAW,
	IIO_MOD_ROLL,
};

struct iio_ch_info {
	int16_t ch_num;
	bool ch_out;
	enum iio_chan_type type;
	bool differential;
	uint32_t address;
};

#define END_ATTRIBUTES_ARRAY {.name = NULL}

enum iio_attribute_shared {
	IIO_SEPARATE,
	IIO_SHARED_BY_TYPE,
	IIO_SHARED_BY_DIR,
	IIO_SHARED_BY_ALL,
};

struct iio_attribute {
	const char *name;
	intptr_t priv;
	enum iio_attribute_shared shared;
	int (*show)(void *device, char *buf, uint32_t len,
		    const struct iio_ch_info *channel, intptr_t priv);
	int (*store)(void *device, char *buf, uint32_t len,
		     const struct iio_ch_info *channel, intptr_t priv);
};

struct scan_type {
	char sign;
	uint8_t realbits;
	uint8_t storagebits;
	uint8_t shift;
	bool is_big_endian;
};

struct iio_channel {
	const char *name;
	enum iio_chan_type ch_type;
	int channel;
	int channel2;
	unsigned long address;
	int scan_index;
	struct scan_type *scan_type;
	struct iio_attribute *attributes;
	bool ch_out;
	bool modified;
	bool indexed;
	bool diferential;
};

enum iio_buffer_direction {
	IIO_DIRECTION_INPUT,
	IIO_DIRECTION_OUTPUT
};

struct iio_buffer {
	uint32_t active_mask;
	uint32_t size;
	uint32_t bytes_per_scan;
	uint32_t samples;
	enum iio_buffer_direction dir;
	struct no_os_circular_buffer *buf;
};

struct iio_device_data {
	void *dev;
	struct iio_buffer *buffer;
};

struct iio_device {
	struct no_os_irq_ctrl_desc *irq_desc;
	uint16_t num_ch;
	struct iio_channel *channels;
	struct iio_attribute *attributes;
	struct iio_attribute *debug_attributes;
	struct iio_attribute *buffer_attributes;
	int32_t (*read_dev)(void *dev, void *buff, uint32_t nb_samples);
	int32_t (*write_dev)(void *dev, void *buff, uint32_t nb_samples);
	int32_t (*pre_enable)(void *dev, uint32_t mask);
	int32_t (*post_disable)(void *dev);
	int32_t (*submit)(struct iio_device_data *dev);
	int32_t (*trigger_handler)(struct iio_device_data *dev);
	int32_t (*debug_reg_read)(void *dev, uint32_t reg, uint32_t *readval);
	int32_t (*debug_reg_write)(void *dev, uint32_t reg, uint32_t writeval);
};

#endif /* IIO_TYPES_H_ */
