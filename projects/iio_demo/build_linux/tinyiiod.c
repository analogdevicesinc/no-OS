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
	if (!iiod->buf) {
		free(iiod);
		return NULL;
	}
	iiod->ops = ops;

	return iiod;
}

void tinyiiod_destroy(struct tinyiiod *iiod)
{
	free(iiod->buf);
	free(iiod);
}

int32_t tinyiiod_read_command(struct tinyiiod *iiod)
{
	char buf[128];
	int32_t ret;

	ret = tinyiiod_read_line(iiod, buf, sizeof(buf));
	printf("ret dupa read line %d\n",ret);
	if (ret < 0)
		return ret;

	ret = tinyiiod_parse_string(iiod, buf);
	printf("ret dupa parse string %d \n",ret);
	if (ret < 0)
		tinyiiod_write_value(iiod, ret);

	return ret;
}

ssize_t tinyiiod_read(struct tinyiiod *iiod, char *buf, size_t len)
{
	return iiod->ops->read(buf, len);
}

ssize_t tinyiiod_read_line(struct tinyiiod *iiod, char *buf, size_t len)
{
	char ch;
	uint32_t i;
	bool found = false;
	int32_t ret;
	
	if (iiod->ops->read_line)
		return iiod->ops->read_line(buf, len);

	for (i = 0; i < len - 1; i++) { 
		ret = iiod->ops->read(&ch, 1);
		if (ret <= 0)
			return -EIO;
		buf[i] = ch;
		if (buf[i] != '\n')
			found = true;
		else if (found)
			break;
	}

	if (!found || i == len - 1) {
		/* No \n found -> garbage data */
		printf("is this the case????\n");
		return -EIO;
	}

	buf[i - 1] = '\0';

	return i;
}

ssize_t tinyiiod_write_char(struct tinyiiod *iiod, char c)
{
	return iiod->ops->write(&c, 1);
}

ssize_t tinyiiod_write(struct tinyiiod *iiod, const char *data, size_t len)
{
	return iiod->ops->write(data, len);
}

ssize_t tinyiiod_write_string(struct tinyiiod *iiod, const char *str)
{
	return tinyiiod_write(iiod, str, strlen(str));
}

ssize_t tinyiiod_write_value(struct tinyiiod *iiod, int32_t value)
{
	char buf[16];

	snprintf(buf, sizeof(buf), "%"PRIi32"\n", value);
	return tinyiiod_write_string(iiod, buf);
}

void tinyiiod_write_xml(struct tinyiiod *iiod)
{
	char *xml;
	iiod->ops->get_xml(&xml);
	size_t len = strlen(xml);

	tinyiiod_write_value(iiod, len);
	tinyiiod_write(iiod, xml, len);
	tinyiiod_write_char(iiod, '\n');
}

void tinyiiod_do_read_attr(struct tinyiiod *iiod, const char *device,
			   const char *channel, bool ch_out, const char *attr, enum iio_attr_type type)
{
	ssize_t ret;

	if (channel)
		ret = iiod->ops->ch_read_attr(device, channel,
					      ch_out, attr, iiod->buf, IIOD_BUFFER_SIZE);
	else
		ret = iiod->ops->read_attr(device, attr,
					   iiod->buf, IIOD_BUFFER_SIZE, type);

	tinyiiod_write_value(iiod, (int32_t) ret);
	if (ret > 0) {
		iiod->buf[ret] = '\n';
		tinyiiod_write(iiod, iiod->buf, (size_t) ret + 1);
	}
}

void tinyiiod_do_write_attr(struct tinyiiod *iiod, const char *device,
			    const char *channel, bool ch_out, const char *attr,
			    size_t bytes, enum iio_attr_type type)
{
	ssize_t ret;

	if (bytes > IIOD_BUFFER_SIZE - 1)
		bytes = IIOD_BUFFER_SIZE - 1;

	tinyiiod_read(iiod, iiod->buf, bytes);
	iiod->buf[bytes] = '\0';

	if (channel)
		ret = iiod->ops->ch_write_attr(device, channel, ch_out,
					       attr, iiod->buf, bytes);
	else
		ret = iiod->ops->write_attr(device, attr, iiod->buf, bytes, type);

	tinyiiod_write_value(iiod, (int32_t) ret);
}

void tinyiiod_do_open(struct tinyiiod *iiod, const char *device,
		      size_t sample_size, uint32_t mask)
{
	int32_t ret = iiod->ops->open(device, sample_size, mask);
	tinyiiod_write_value(iiod, ret);
}

void tinyiiod_do_close(struct tinyiiod *iiod, const char *device)
{
	int32_t ret = iiod->ops->close(device);
	tinyiiod_write_value(iiod, ret);
}

int32_t tinyiiod_do_open_instance(struct tinyiiod *iiod)
{
	if (iiod->ops->open_instance)
		return iiod->ops->open_instance();

	return 0;
}

int32_t tinyiiod_do_close_instance(struct tinyiiod *iiod)
{
	if (iiod->ops->close_instance)
		return iiod->ops->close_instance();

	return 0;
}

int32_t tinyiiod_do_writebuf(struct tinyiiod *iiod,
			     const char *device, size_t bytes_count)
{
	size_t bytes, offset = 0, total_bytes = bytes_count;
	char buf[256];
	int32_t ret = 0;

	tinyiiod_write_value(iiod, bytes_count);
	while (bytes_count) {
		bytes = bytes_count > sizeof(buf) ? sizeof(buf) : bytes_count;
		ret = tinyiiod_read(iiod, buf, bytes);
		if (ret > 0) {
			ret = iiod->ops->write_data(device, buf, offset, ret);
			offset += ret;
			if (ret < 0)
				return ret;
			bytes_count -= ret;
		} else
			return ret;
	}
	if (iiod->ops->transfer_mem_to_dev) {
		ret = iiod->ops->transfer_mem_to_dev(device, total_bytes);
		if (ret < 0)
			return ret;
	}
	tinyiiod_write_value(iiod, (int) total_bytes);

	return ret;
}

int32_t tinyiiod_do_readbuf(struct tinyiiod *iiod,
			    const char *device, size_t bytes_count)
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
	if (iiod->ops->transfer_dev_to_mem) {
		ret = iiod->ops->transfer_dev_to_mem(device, bytes_count);
		if (ret < 0)
			return ret;
	}
	while (bytes_count) {
		size_t bytes = bytes_count > sizeof(buf) ? sizeof(buf) : bytes_count;

		ret = (int) iiod->ops->read_data(device, buf, offset, bytes);
		offset += bytes;
		tinyiiod_write_value(iiod, ret);
		if (ret < 0)
			return ret;

		if (print_mask) {
			char buf_mask[10];

			snprintf(buf_mask, sizeof(buf_mask), "%08"PRIx32"\n", mask);
			tinyiiod_write_string(iiod, buf_mask);
			print_mask = false;
		}

		tinyiiod_write(iiod, buf, (size_t) ret);
		bytes_count -= (size_t) ret;
	}

	return ret;
}

int32_t tinyiiod_set_timeout(struct tinyiiod *iiod, uint32_t timeout)
{
	int32_t ret = 0;

	if (iiod->ops->set_timeout)
		ret = iiod->ops->set_timeout(timeout);
	tinyiiod_write_value(iiod, ret);

	return ret;
}
