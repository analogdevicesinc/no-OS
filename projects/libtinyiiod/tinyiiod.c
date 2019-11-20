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
#define IIOD_BUFFER_SIZE 1024
struct tinyiiod {
	struct tinyiiod_ops *ops;
	char *buf;
};

struct tinyiiod * tinyiiod_create(struct tinyiiod_ops *ops)
{
	struct tinyiiod *iiod = malloc(sizeof(*iiod));

	if (!iiod)
		return NULL;

	iiod->buf = malloc(IIOD_BUFFER_SIZE);
	iiod->ops = ops;

	return iiod;
}

void tinyiiod_destroy(struct tinyiiod *iiod)
{
	free(iiod->ops);
	free(iiod->buf);
	free(iiod);
}

int32_t tinyiiod_read_command(struct tinyiiod *iiod)
{
	uint32_t instance_id = 0xFFFFAAAA;
	char buf[128];
	int32_t ret;

	ret = tinyiiod_read_line(iiod, buf, sizeof(buf), &instance_id);
	if (ret < 0)
		return ret;

	ret = tinyiiod_parse_string(iiod, buf, instance_id);
	if (ret < 0)
		tinyiiod_write_value(iiod, ret, instance_id);

	return ret;
}

char tinyiiod_read_char(struct tinyiiod *iiod, uint32_t *instance_id)
{
	char c;

	iiod->ops->read(&c, 1, instance_id);
	return c;
}

ssize_t tinyiiod_read(struct tinyiiod *iiod, char *buf, size_t len, uint32_t *instance_id)
{
	return iiod->ops->read(buf, len, instance_id);
}

ssize_t tinyiiod_read_line(struct tinyiiod *iiod, char *buf, size_t len, uint32_t *instance_id)
{
	int32_t i;
	bool found = false;

	if (iiod->ops->read_line)
		return iiod->ops->read_line(buf, len, instance_id);

	for (i = 0; i < len - 1; i++) {
		buf[i] = tinyiiod_read_char(iiod, instance_id);

		if (buf[i] != '\n')
			found = true;
		else if (found)
			break;
	}

	if (!found || i == len - 1) {
		/* No \n found -> garbage data */
		return -EIO;
	}

	buf[i - 1] = '\0';

	return i;
}

ssize_t tinyiiod_write_char(struct tinyiiod *iiod, char c, uint32_t instance_id)
{
	return iiod->ops->write(&c, 1, instance_id);
}

ssize_t tinyiiod_write(struct tinyiiod *iiod, const char *data, size_t len, uint32_t instance_id)
{
	return iiod->ops->write(data, len, instance_id);
}

ssize_t tinyiiod_write_string(struct tinyiiod *iiod, const char *str, uint32_t instance_id)
{
	return tinyiiod_write(iiod, str, strlen(str), instance_id);
}

ssize_t tinyiiod_write_value(struct tinyiiod *iiod, int32_t value, uint32_t instance_id)
{
	char buf[16];

	snprintf(buf, sizeof(buf), "%"PRIi32"\n", value);
	return tinyiiod_write_string(iiod, buf, instance_id);
}

void tinyiiod_write_xml(struct tinyiiod *iiod, uint32_t instance_id)
{
	char *xml;
	iiod->ops->get_xml(&xml);
	size_t len = strlen(xml);

	tinyiiod_write_value(iiod, len, instance_id);
	tinyiiod_write(iiod, xml, len, instance_id);
	tinyiiod_write_char(iiod, '\n', instance_id);
	free(xml);
}

void tinyiiod_do_read_attr(struct tinyiiod *iiod, const char *device,
			   const char *channel, bool ch_out, const char *attr, bool debug, uint32_t instance_id)
{
	ssize_t ret;

	if (channel)
		ret = iiod->ops->ch_read_attr(device, channel,
					      ch_out, attr, iiod->buf, IIOD_BUFFER_SIZE);
	else
		ret = iiod->ops->read_attr(device, attr,
					   iiod->buf, IIOD_BUFFER_SIZE, debug);

	tinyiiod_write_value(iiod, (int32_t) ret, instance_id);
	if (ret > 0) {
		iiod->buf[ret] = '\n';
		tinyiiod_write(iiod, iiod->buf, (size_t) ret + 1, instance_id);
	}
}

void tinyiiod_do_write_attr(struct tinyiiod *iiod, const char *device,
			    const char *channel, bool ch_out, const char *attr,
			    size_t bytes, bool debug, uint32_t instance_id)
{
	ssize_t ret;

	if (bytes > IIOD_BUFFER_SIZE - 1)
		bytes = IIOD_BUFFER_SIZE - 1;

	tinyiiod_read(iiod, iiod->buf, bytes, &instance_id);
	iiod->buf[bytes] = '\0';

	if (channel)
		ret = iiod->ops->ch_write_attr(device, channel, ch_out,
					       attr, iiod->buf, bytes);
	else
		ret = iiod->ops->write_attr(device, attr, iiod->buf, bytes, debug);

	tinyiiod_write_value(iiod, (int32_t) ret, instance_id);
}

void tinyiiod_do_open(struct tinyiiod *iiod, const char *device,
		      size_t sample_size, uint32_t mask, uint32_t instance_id)
{
	int32_t ret = iiod->ops->open(device, sample_size, mask);
	tinyiiod_write_value(iiod, ret, instance_id);
}

void tinyiiod_do_close(struct tinyiiod *iiod, const char *device, uint32_t instance_id)
{
	int32_t ret = iiod->ops->close(device);
	tinyiiod_write_value(iiod, ret, instance_id);
}

int32_t tinyiiod_do_open_instance(struct tinyiiod *iiod)
{
	return iiod->ops->open_instance();
}

int32_t tinyiiod_do_close_instance(struct tinyiiod *iiod, uint32_t instance_id)
{
	return iiod->ops->close_instance(instance_id);
}

int32_t tinyiiod_do_writebuf(struct tinyiiod *iiod,
			     const char *device, size_t bytes_count, uint32_t instance_id)
{
	size_t bytes, offset = 0, total_bytes = bytes_count;
	char buf[256];
	int32_t ret;

	tinyiiod_write_value(iiod, bytes_count, instance_id);
	while (bytes_count) {
		bytes = bytes_count > sizeof(buf) ? sizeof(buf) : bytes_count;
		ret = tinyiiod_read(iiod, buf, bytes, &instance_id);
		if (ret > 0) {
			ret = iiod->ops->write_data(device, buf, offset, ret);
			offset += ret;
			if (ret < 0)
				return ret;
			bytes_count -= ret;
		} else
			return ret;
	}
	if (iiod->ops->transfer_mem_to_dev)
		ret = iiod->ops->transfer_mem_to_dev(device, total_bytes);
	tinyiiod_write_value(iiod, (int) total_bytes, instance_id);

	return ret;
}

int32_t tinyiiod_do_readbuf(struct tinyiiod *iiod,
			    const char *device, size_t bytes_count, uint32_t instance_id)
{
	int32_t ret;
	char buf[256];
	uint32_t mask;
	bool print_mask = true;
	size_t offset = 0;

	ret = iiod->ops->get_mask(device, &mask);
	if (ret < 0) {
		return ret;
	}
	if (iiod->ops->transfer_dev_to_mem)
		ret = iiod->ops->transfer_dev_to_mem(device, bytes_count);
	while (bytes_count) {
		size_t bytes = bytes_count > sizeof(buf) ? sizeof(buf) : bytes_count;

		ret = (int) iiod->ops->read_data(device, buf, offset, bytes);
		offset += bytes;
		tinyiiod_write_value(iiod, ret, instance_id);
		if (ret < 0)
			return ret;

		if (print_mask) {
			char buf_mask[10];

			snprintf(buf_mask, sizeof(buf_mask), "%08"PRIx32"\n", mask);
			tinyiiod_write_string(iiod, buf_mask, instance_id);
			print_mask = false;
		}

		tinyiiod_write(iiod, buf, (size_t) ret, instance_id);
		bytes_count -= (size_t) ret;
	}

	return ret;
}

int32_t tinyiiod_set_timeout(struct tinyiiod *iiod, uint32_t timeout, uint32_t instance_id)
{
	int32_t ret = 0;

	if (iiod->ops->set_timeout)
		ret = iiod->ops->set_timeout(timeout);
	tinyiiod_write_value(iiod, ret, instance_id);

	return ret;
}
