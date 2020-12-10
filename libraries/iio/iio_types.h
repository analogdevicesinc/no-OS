/***************************************************************************//**
 *   @file   iio_types.h
 *   @brief  Header file for iio_types
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef IIO_TYPES_H_
#define IIO_TYPES_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

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
	/* All new types must be added before this field */
	IIO_LAST_TYPE
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
};

/**
 * @struct iio_ch_info
 * @brief Structure holding channel attributess.
 */
struct iio_ch_info {
	/** Channel number */
	int16_t ch_num;
	/** Channel type: input/output */
	bool ch_out;
};

#define END_ATTRIBUTES_ARRAY {.name = NULL}

/**
 * @struct iio_attribute
 * @brief Structure holding pointers to show and store functions.
 */
struct iio_attribute {
	/** Attribute name */
	const char *name;
	/** Attribute id */
	intptr_t priv;
	/** Show function pointer */
	ssize_t (*show)(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel, intptr_t priv);
	/** Store function pointer */
	ssize_t (*store)(void *device, char *buf, size_t len,
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
	/** channel name */
	char			*name;
	/** Chanel type */
	enum iio_chan_type	ch_type;
	/** Channel number when the same channel type */
	int 			channel;
	/** If modified is set, this provides the modifier. E.g. IIO_MOD_X
	 *  for angular rate when applied to channel2 will make make the
	 *  IIO_ANGL_VEL have anglvel_x which corresponds to the x-axis. */
	int 			channel2;
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

struct iio_data_buffer {
	uint32_t	size;
	void		*buff;
};

/**
 * @struct iio_device
 * @brief Structure holding channels and attributes of a device.
 */
struct iio_device {
	/** Device number of channels */
	uint16_t num_ch;
	/** List of channels */
	struct iio_channel **channels;
	/** Array of attributes. Last one should have its name set to NULL */
	struct iio_attribute *attributes;
	/** Array of attributes. Last one should have its name set to NULL */
	struct iio_attribute *debug_attributes;
	/** Array of attributes. Last one should have its name set to NULL */
	struct iio_attribute *buffer_attributes;
	/** Transfer data from device into RAM */
	ssize_t (*transfer_dev_to_mem)(void *dev_instance, size_t bytes_count,
				       uint32_t ch_mask);
	/** Read data from RAM to pbuf. It should be called after "transfer_dev_to_mem" */
	ssize_t (*read_data)(void *dev_instance, char *pbuf, size_t offset,
			     size_t bytes_count, uint32_t ch_mask);
	/** Transfer data from RAM to device */
	ssize_t (*transfer_mem_to_dev)(void *dev_instance, size_t bytes_count,
				       uint32_t ch_mask);
	/** Write data to RAM. It should be called before "transfer_mem_to_dev" */
	ssize_t (*write_data)(void *dev_instance, char *pbuf, size_t offset,
			      size_t bytes_count, uint32_t ch_mask);
	/** Called before a transfer starts. The device should activate the
	 * channels from the mask */
	int32_t (*prepare_transfer)(void *dev, uint32_t mask);
	/** Called after a tranfer ends */
	int32_t (*end_transfer)(void *dev);
	/* Numbers of bytes will be:
	 * samples * (storage_size_of_first_active_ch / 8) * nb_active_channels
	 */
	int32_t	(*read_dev)(void *dev, void *buff, uint32_t nb_samples);
	/* Numbers of bytes will be:
	 * samples * (storage_size_of_first_active_ch / 8) * nb_active_channels
	 */
	int32_t	(*write_dev)(void *dev, void *buff, uint32_t nb_samples);
	/* Read device register */
	int32_t (*debug_reg_read)(void *dev, uint32_t reg, uint32_t *readval);
	/* Write device register */
	int32_t (*debug_reg_write)(void *dev, uint32_t reg, uint32_t writeval);

};

#endif /* IIO_TYPES_H_ */
