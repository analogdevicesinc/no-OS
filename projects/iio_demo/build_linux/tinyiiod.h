/*
 * libtinyiiod - Tiny IIO Daemon Library
 *
 * Copyright (C) 2016 Analog Devices, Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef TINYIIOD_H
#define TINYIIOD_H

#include "compat.h"

struct tinyiiod;

enum iio_attr_type {
	IIO_ATTR_TYPE_DEVICE = 0,
	IIO_ATTR_TYPE_DEBUG = 1,
	IIO_ATTR_TYPE_BUFFER = 2,
};

struct tinyiiod_ops {
	/* Read from the input stream */
	ssize_t (*read)(char *buf, size_t len);

	/* Write to the output stream */
	ssize_t (*write)(const char *buf, size_t len);
	ssize_t (*read_line)(char *buf, size_t len);

	ssize_t (*open_instance)();

	ssize_t (*close_instance)();

	ssize_t (*read_attr)(const char *device, const char *attr,
			     char *buf, size_t len, enum iio_attr_type type);
	ssize_t (*write_attr)(const char *device, const char *attr,
			      const char *buf, size_t len, enum iio_attr_type type);

	ssize_t (*ch_read_attr)(const char *device, const char *channel,
				bool ch_out, const char *attr, char *buf, size_t len);
	ssize_t (*ch_write_attr)(const char *device, const char *channel,
				 bool ch_out, const char *attr,
				 const char *buf, size_t len);

	int32_t (*open)(const char *device, size_t sample_size, uint32_t mask);
	int32_t (*close)(const char *device);

	ssize_t (*transfer_dev_to_mem)(const char *device, size_t bytes_count);
	ssize_t (*read_data)(const char *device, char *buf, size_t offset,
			     size_t bytes_count);

	ssize_t (*transfer_mem_to_dev)(const char *device, size_t bytes_count);
	ssize_t (*write_data)(const char *device, const char *buf, size_t offset,
			      size_t bytes_count);

	int32_t (*get_mask)(const char *device, uint32_t *mask);

	int32_t (*set_timeout)(uint32_t timeout);

	ssize_t (*get_xml)(char **outxml);
};

struct tinyiiod * tinyiiod_create(struct tinyiiod_ops *ops);
void tinyiiod_destroy(struct tinyiiod *iiod);
int32_t tinyiiod_read_command(struct tinyiiod *iiod);

#endif /* TINYIIOD_H */
