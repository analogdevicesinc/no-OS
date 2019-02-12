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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#if !defined(__ssize_t_defined) && !defined(_SSIZE_T_DEFINED)
#define __ssize_t_defined
#define _SSIZE_T_DEFINED
#endif

struct tinyiiod;

struct tinyiiod_ops {
	int (*read)(char *buf, size_t len);
	/* Read from the input stream */
	int (*read_line)(char *buf);
	int (*read_nonbloking)(char *buf, size_t len);
	int (*read_wait)(size_t len);

	/* Write to the output stream */
	void (*write)(const char *buf, size_t len);

	ssize_t (*read_attr)(const char *device, const char *attr,
			     char *buf, size_t len, bool debug);
	ssize_t (*write_attr)(const char *device, const char *attr,
			      const char *buf, size_t len, bool debug);

	ssize_t (*ch_read_attr)(const char *device, const char *channel,
				bool ch_out, const char *attr, char *buf, size_t len);
	ssize_t (*ch_write_attr)(const char *device, const char *channel,
				 bool ch_out, const char *attr,
				 const char *buf, size_t len);

	int (*open)(const char *device, size_t sample_size, uint32_t mask);
	int (*close)(const char *device);

	ssize_t (*read_device)(const char *device, char **pbuf, size_t bytes_count);
	ssize_t (*write_device)(const char *device, const char *buf,
				size_t bytes_count);
	int (*read_reg)(unsigned int addr, int32_t *value);
	int (*write_reg)(unsigned int addr, uint32_t value);

	int (*get_mask)(const char *device, uint32_t *mask);
};

struct tinyiiod * tinyiiod_create(const char *xml,
				  const struct tinyiiod_ops *ops);
void tinyiiod_destroy(struct tinyiiod *iiod);
int tinyiiod_read_command(struct tinyiiod *iiod);
int tinyiiod_do_writebuf(struct tinyiiod *iiod, const char *device,
			 size_t bytes_count);

#endif /* TINYIIOD_H */

