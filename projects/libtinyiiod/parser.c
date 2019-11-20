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

#include "tinyiiod-private.h"

#include "compat.h"
#define TINYIIOD_VERSION_MAJOR 1
#define TINYIIOD_VERSION_MINOR 2
#define TINYIIOD_VERSION_GIT "as"
static int32_t parse_rw_string(struct tinyiiod *iiod, char *str, bool write, uint32_t instance_id)
{
	char *device, *channel, *attr, *ptr;
	bool is_channel = false, output = false, debug = false;
	long bytes;

	ptr = strchr(str, ' ');
	if (!ptr)
		return -EINVAL;

	*ptr = '\0';
	device = str;
	str = ptr + 1;

	if (!strncmp(str, "INPUT ", sizeof("INPUT ") - 1)) {
		is_channel = true;
		str += sizeof("INPUT ") - 1;
	} else if (!strncmp(str, "OUTPUT ", sizeof("OUTPUT ") - 1)) {
		is_channel = true;
		output = true;
		str += sizeof("OUTPUT ") - 1;
	} else if (!strncmp(str, "DEBUG ", sizeof("DEBUG ") - 1)) {
		debug = true;
		str += sizeof("DEBUG ") - 1;
	}

	if (is_channel) {
		ptr = strchr(str, ' ');
		if (!ptr)
			return -EINVAL;

		*ptr = '\0';
		channel = str;
		str = ptr + 1;
	} else {
		channel = NULL;
	}

	ptr = strchr(str, ' ');
	if ((!!ptr) ^ write)
		return -EINVAL;

	attr = str;

	if (write) {
		*ptr = '\0';
		str = ptr + 1;
	} else {
		tinyiiod_do_read_attr(iiod, device, channel,
				      output, attr, debug, instance_id);
		return 0;
	}

	bytes = strtol(str, &ptr, 10);
	if (str == ptr || bytes < 0)
		return -EINVAL;

	tinyiiod_do_write_attr(iiod, device, channel,
			       output, attr, (size_t) bytes, debug, instance_id);

	return 0;
}

static int32_t parse_open_string(struct tinyiiod *iiod, char *str, uint32_t instance_id)
{
	char *device, *ptr;
	long samples_count;
	uint32_t mask = 0;

	ptr = strchr(str, ' ');
	if (!ptr)
		return -EINVAL;

	*ptr = '\0';
	device = str;
	str = ptr + 1;

	samples_count = strtol(str, &ptr, 10);
	if (str == ptr || *ptr != ' ' || samples_count < 0)
		return -EINVAL;

	str = ptr + 1;

	mask = strtoul(str, NULL, 16);

	tinyiiod_do_open(iiod, device, (size_t) samples_count, mask, instance_id);

	return 0;
}

static int32_t parse_timeout_string(struct tinyiiod *iiod, char *str, uint32_t instance_id)
{
	uint32_t timeout = strtoul(str, NULL, 10);

	return tinyiiod_set_timeout(iiod, timeout, instance_id);
}

static int32_t parse_writebuf_string(struct tinyiiod *iiod, char *str, uint32_t instance_id)
{
	char *device, *ptr;
	long bytes_count;

	ptr = strchr(str, ' ');
	if (!ptr)
		return -EINVAL;

	*ptr = '\0';
	device = str;
	str = ptr + 1;

	bytes_count = strtol(str, &ptr, 10);
	if (str == ptr || *ptr != '\0' || bytes_count < 0)
		return -EINVAL;

	return tinyiiod_do_writebuf(iiod, device, (size_t) bytes_count, instance_id);
}

static int32_t parse_readbuf_string(struct tinyiiod *iiod, char *str, uint32_t instance_id)
{
	char *device, *ptr;
	long bytes_count;

	ptr = strchr(str, ' ');
	if (!ptr)
		return -EINVAL;

	*ptr = '\0';
	device = str;
	str = ptr + 1;

	bytes_count = strtol(str, &ptr, 10);
	if (str == ptr || *ptr != '\0' || bytes_count < 0)
		return -EINVAL;

	return tinyiiod_do_readbuf(iiod, device, (size_t) bytes_count, instance_id);
}

int32_t tinyiiod_parse_string(struct tinyiiod *iiod, char *str, uint32_t instance_id)
{
	while (*str == '\n' || *str == '\r')
		str++;

	if (str[0] == '\0')
		return 0;

	if (!strncmp(str, "VERSION", sizeof("VERSION"))) {
		char buf[32];

		snprintf(buf, sizeof(buf), "%"PRIu16".%"PRIu16".%-7.7s\n",
			 TINYIIOD_VERSION_MAJOR,
			 TINYIIOD_VERSION_MINOR,
			 TINYIIOD_VERSION_GIT);
		tinyiiod_write_string(iiod, buf, instance_id);
		return 0;
	}

	if (!strncmp(str, "PRINT", sizeof("PRINT"))) {
		tinyiiod_write_xml(iiod, instance_id);
		return 0;
	}

	if (!strncmp(str, "READ ", sizeof("READ ") - 1))
		return parse_rw_string(iiod, str + sizeof("READ ") - 1, false, instance_id);

	if (!strncmp(str, "WRITE ", sizeof("WRITE ") -1))
		return parse_rw_string(iiod, str + sizeof("WRITE ") - 1, true, instance_id);

	if (!strncmp(str, "OPEN ", sizeof("OPEN ") -1))
		return parse_open_string(iiod, str + sizeof("OPEN ") - 1, instance_id);

	if (!strncmp(str, "CLOSE ", sizeof("CLOSE ") -1)) {
		tinyiiod_do_close(iiod, str + sizeof("CLOSE ") - 1, instance_id);
		return 0;
	}

	if (!strncmp(str, "READBUF ", sizeof("READBUF ") -1))
		return parse_readbuf_string(iiod, str + sizeof("READBUF ") - 1, instance_id);

	if (!strncmp(str, "TIMEOUT ", sizeof("TIMEOUT ") - 1))
		return parse_timeout_string(iiod, str + sizeof("TIMEOUT ") - 1, instance_id);

	if (!strncmp(str, "WRITEBUF ", sizeof("WRITEBUF ") -1))
		return parse_writebuf_string(iiod, str + sizeof("WRITEBUF ") - 1, instance_id);

	if (!strncmp(str, "EXIT", sizeof("EXIT") - 1))
		return tinyiiod_do_close_instance(iiod, instance_id);

	if (!strncmp(str, "GETTRIG", sizeof("GETTRIG") - 1))
		tinyiiod_write_value(iiod, -ENODEV, instance_id);

	return -EINVAL;
}
