/***************************************************************************//**
 *   @file   iio_types.h
 *   @brief  Header file for iio_types
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef IIO_TYPES_H_
#define IIO_TYPES_H_

#include <stdbool.h>
#include <stdint.h>
#include "no_os_circular_buffer.h"

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

/**
 * @struct iio_chan_type
 * @brief IIO channel types
 */
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
	IIO_WEIGHT,
};

/**
 * @struct iio_modifier
 * @brief IIO channel modifier
 */
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

/**
 * @enum iio_event_type
 * @brief IIO event type
 *
 * Some IIO devices can deliver events. The type of the event can be specified
 * by one of the iio_event_type values.
 */
enum iio_event_type {
	IIO_EV_TYPE_THRESH,
	IIO_EV_TYPE_MAG,
	IIO_EV_TYPE_ROC,
	IIO_EV_TYPE_THRESH_ADAPTIVE,
	IIO_EV_TYPE_MAG_ADAPTIVE,
	IIO_EV_TYPE_CHANGE,
	IIO_EV_TYPE_MAG_REFERENCED,
	IIO_EV_TYPE_GESTURE,
	IIO_EV_TYPE_FAULT,
};

/**
 * @enum iio_event_direction
 * @brief IIO event direction
 *
 * When applicable, this enum specifies the direction of the iio_event_type.
 */
enum iio_event_direction {
	IIO_EV_DIR_EITHER,
	IIO_EV_DIR_RISING,
	IIO_EV_DIR_FALLING,
	IIO_EV_DIR_NONE,
	IIO_EV_DIR_SINGLETAP,
	IIO_EV_DIR_DOUBLETAP,
	IIO_EV_DIR_FAULT_OPENWIRE,
};

/**
 * @struct iio_ch_info
 * @brief Structure holding channel attributess.
 */
struct iio_ch_info {
	/** Channel number. TODO: refactor out the ch_ prefix. */
	int16_t ch_num;
	/** Channel direction: input/output. TODO: refactor out the ch_ prefix. */
	bool ch_out;
	/** Channel type */
	enum iio_chan_type type;
	/** Differential channel indicator */
	bool differential;
	/** Driver specific identifier. */
	uint32_t address;
};

#define END_ATTRIBUTES_ARRAY {.name = NULL}

enum iio_attribute_shared {
	IIO_SEPARATE,
	IIO_SHARED_BY_TYPE,
	IIO_SHARED_BY_DIR,
	IIO_SHARED_BY_ALL,
};

/**
 * @struct iio_attribute
 * @brief Structure holding pointers to show and store functions.
 */
struct iio_attribute {
	/** Attribute name */
	const char *name;
	/** Attribute id */
	intptr_t priv;
	/** Whether this attribute is shared by all channels of the same type, or direction
	 * or simply by all channels. If left uninitialized, the sharedness defaults to
	 * separate.
	*/
	enum iio_attribute_shared shared;
	/** Show function pointer */
	int (*show)(void *device, char *buf, uint32_t len,
		    const struct iio_ch_info *channel, intptr_t priv);
	/** Store function pointer */
	int (*store)(void *device, char *buf, uint32_t len,
		     const struct iio_ch_info *channel, intptr_t priv);
};

/**
 * @struct iio_channel
 * @brief Struct describing the scan type
 */
struct scan_type {
	/** 's' or 'u' to specify signed or unsigned */
	char			sign;
	/** Number of valid bits of data */
	uint8_t 		realbits;
	/** Realbits + padding */
	uint8_t			storagebits;
	/** Shift right by this before masking out realbits. */
	uint8_t			shift;
	/** True if big endian, false if little endian */
	bool			is_big_endian;
};

/**
 * @struct iio_channel
 * @brief Structure holding attributes of a channel.
 */
struct iio_channel {
	/** Channel name */
	const char		*name;
	/** Chanel type */
	enum iio_chan_type	ch_type;
	/** Channel number when the same channel type */
	int 			channel;
	/** If modified is set, this provides the modifier. E.g. IIO_MOD_X
	 *  for angular rate when applied to channel2 will make make the
	 *  IIO_ANGL_VEL have anglvel_x which corresponds to the x-axis. */
	int 			channel2;
	/** Driver specific identifier. */
	unsigned long		address;
	/** Index to give ordering in scans when read  from a buffer. */
	int			scan_index;
	/** */
	struct scan_type	*scan_type;
	/** Array of attributes. Last one should have its name set to NULL */
	struct iio_attribute	*attributes;
	/** if true, the channel is an output channel */
	bool			ch_out;
	/** Set if channel has a modifier. Use channel2 property to
	 *  select the modifier to use.*/
	bool			modified;
	/** Specify if channel has a numerical index. If not set, channel
	 *  number will be suppressed. */
	bool			indexed;
	/* Set if the channel is differential. */
	bool			diferential;
};

enum iio_buffer_direction {
	IIO_DIRECTION_INPUT,
	IIO_DIRECTION_OUTPUT
};

struct iio_cyclic_buffer_info {
	bool is_cyclic;
	uint32_t buff_index;
};

struct iio_buffer {
	/* Mask with active channels */
	uint32_t active_mask;
	/* Size in bytes */
	uint32_t size;
	/* Number of bytes per sample * number of active channels */
	uint32_t bytes_per_scan;
	/* Number of requested samples */
	uint32_t samples;
	/* Buffer direction */
	enum iio_buffer_direction dir;
	/* Buffer where data is stored */
	struct no_os_circular_buffer *buf;
	/* Stores cyclic buffer specific information */
	struct iio_cyclic_buffer_info cyclic_info;
};

struct iio_block {
	struct iio_buffer *buffer;
	uint8_t *data;
	uint32_t size;
	uint16_t cl_id;

	uint32_t bytes_used;
};

struct iio_stream {
	struct iio_buffer *buffer;
	struct iio_block **blocks;
	uint32_t nb_blocks;
	uint8_t curr;
	bool started, buf_enabled, all_enqueued;
};

struct __attribute__((packed)) iio_event {
	uint64_t channel_id: 16;
	uint64_t diff_channel_id: 16;
	uint64_t channel_type: 8;
	uint64_t modifier: 8;
	uint64_t event_dir: 7;
	uint64_t is_differential: 1;
	uint64_t event_type: 8;
	int64_t timestamp: 64;
};

struct iio_device_data {
	void *dev;
	struct iio_buffer *buffer;
};

struct iio_trigger {
	/** If true the trigger handler will be called in interrupt context
	 *  If false the handler will be called from iio_step */
	bool is_synchronous;
	/** Array of attributes. Last one should have its name set to NULL */
	struct iio_attribute *attributes;
	/** Called when needs to be enabled */
	int (*enable)(void *trig);
	/** Called when needs to be disabled */
	int (*disable)(void *trig);
};

/**
 * @struct iio_device
 * @brief Structure holding channels and attributes of a device.
 */
struct iio_device {
	/** Structure for existing initialized irq controllers. Has to be
	 * set to NULL if there isn't any irq controller initialized. */
	struct no_os_irq_ctrl_desc *irq_desc;
	/** Device number of channels */
	uint16_t num_ch;
	/** List of channels */
	struct iio_channel *channels;
	/** Array of attributes. Last one should have its name set to NULL */
	struct iio_attribute *attributes;
	/** Array of attributes. Last one should have its name set to NULL */
	struct iio_attribute *debug_attributes;
	/** Array of attributes. Last one should have its name set to NULL */
	struct iio_attribute *buffer_attributes;
	/* Numbers of bytes will be:
	 * samples * (storage_size_of_first_active_ch / 8) * nb_active_channels
	 * DEPRECATED.
	 */
	int32_t	(*read_dev)(void *dev, void *buff, uint32_t nb_samples);
	/* Numbers of bytes will be:
	 * samples * (storage_size_of_first_active_ch / 8) * nb_active_channels
	 * DEPRECATED.
	 */
	int32_t	(*write_dev)(void *dev, void *buff, uint32_t nb_samples);

	/* Bufer callbacks */
	/** Called before enabling buffer */
	int32_t (*pre_enable)(void *dev, uint32_t mask, uint16_t *block_ids);
	/** Called after disabling buffer */
	int32_t (*post_disable)(void *dev);
	/** Called when buffer ready to transfer. Write/read to/from dev */
	int32_t	(*submit)(struct iio_device_data *dev);
	/** Called after a trigger signal has been received by iio */
	int32_t (*trigger_handler)(struct iio_device_data *dev);

	/* Read device register */
	int32_t (*debug_reg_read)(void *dev, uint32_t reg, uint32_t *readval);
	/* Write device register */
	int32_t (*debug_reg_write)(void *dev, uint32_t reg, uint32_t writeval);

	int32_t (*create_block)(struct iio_device_data *dev, struct iio_block *block, uint32_t block_size_bytes);
	int32_t	(*transfer_block)(struct iio_device_data *dev, uint8_t block_id);

};

#endif /* IIO_TYPES_H_ */
