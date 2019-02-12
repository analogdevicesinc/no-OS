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
#include <inttypes.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct tinyiiod {
	int32_t instance_id;
	const char *xml;
	const struct tinyiiod_ops *ops;
};

/***************************************************************************//**
 * @brief tinyiiod_create
*******************************************************************************/
struct tinyiiod * tinyiiod_create(const char *xml,
				  const struct tinyiiod_ops *ops)
{
	struct tinyiiod *iiod = malloc(sizeof(*iiod));

	if (!iiod)
		return NULL;

	iiod->xml = xml;
	iiod->ops = ops;

	return iiod;
}

/***************************************************************************//**
 * @brief tinyiiod_destroy
*******************************************************************************/
void tinyiiod_destroy(struct tinyiiod *iiod)
{
	free(iiod);
}

/***************************************************************************//**
 * @brief tinyiiod_read_command
*******************************************************************************/
int32_t tinyiiod_read_command(struct tinyiiod *iiod)
{
	char buf[128] = {0};
	int32_t ret;
	ret = tinyiiod_read_line(iiod, buf);
	if(ret == 0)
		return ret;

	if (ret < 0)
		tinyiiod_write_value(iiod, ret);

	ret = tinyiiod_parse_string(iiod, buf);
	if (ret < 0)
		tinyiiod_write_value(iiod, ret);

	return ret;
}

/***************************************************************************//**
 * @brief tinyiiod_read_char
*******************************************************************************/
char tinyiiod_read_char(struct tinyiiod *iiod)
{
	char c;

	iiod->ops->read(&iiod->instance_id, &c, 1);

	return c;
}

/***************************************************************************//**
 * @brief tinyiiod_read
*******************************************************************************/
int32_t tinyiiod_read(struct tinyiiod *iiod, char *buf, size_t len)
{
	return iiod->ops->read(&iiod->instance_id, buf, len);
}

/***************************************************************************//**
 * @brief tinyiiod_read_line
*******************************************************************************/
int32_t tinyiiod_read_line(struct tinyiiod *iiod, char *buf)
{
	return iiod->ops->read_line(&iiod->instance_id, buf);
}

/***************************************************************************//**
 * @brief tinyiiod_read_nonblocking
*******************************************************************************/
int32_t tinyiiod_read_nonblocking(struct tinyiiod *iiod, char *buf, size_t len)
{
	return iiod->ops->read_nonbloking(&iiod->instance_id, buf, len);
}

/***************************************************************************//**
 * @brief tinyiiod_read_wait
*******************************************************************************/
int32_t tinyiiod_read_wait(struct tinyiiod *iiod, size_t len)
{
	return iiod->ops->read_wait(&iiod->instance_id, len);
}

/***************************************************************************//**
 * @brief tinyiiod_write_char
*******************************************************************************/
void tinyiiod_write_char(struct tinyiiod *iiod, char c)
{
	iiod->ops->write(iiod->instance_id, &c, 1);
}

/***************************************************************************//**
 * @brief tinyiiod_write
*******************************************************************************/
void tinyiiod_write(struct tinyiiod *iiod, const char *data, size_t len)
{
	iiod->ops->write(iiod->instance_id, data, len);
}

/***************************************************************************//**
 * @brief tinyiiod_write_string
*******************************************************************************/
void tinyiiod_write_string(struct tinyiiod *iiod, const char *str)
{
	tinyiiod_write(iiod, str, strlen(str));
}

/***************************************************************************//**
 * @brief tinyiiod_write_value
*******************************************************************************/
void tinyiiod_write_value(struct tinyiiod *iiod, int32_t value)
{
	char buf[16];

	snprintf(buf, sizeof(buf), "%"PRIi32"\n", value);
	tinyiiod_write_string(iiod, buf);
}

/***************************************************************************//**
 * @brief tinyiiod_write_xml
*******************************************************************************/
void tinyiiod_write_xml(struct tinyiiod *iiod)
{
	size_t len = strlen(iiod->xml);
	tinyiiod_write_value(iiod, len);
	tinyiiod_write(iiod, iiod->xml, len);
	tinyiiod_write_char(iiod, '\n');
}

/***************************************************************************//**
 * @brief tinyiiod_do_read_attr
*******************************************************************************/
void tinyiiod_do_read_attr(struct tinyiiod *iiod, const char *device,
			   const char *channel, bool ch_out, const char *attr, bool debug)
{
	char buf[0x1000];
	ssize_t ret = -ENODEV;
	if(debug == 0) {
		if (channel)
			ret = iiod->ops->ch_read_attr(device, channel,
						      ch_out, attr, buf, sizeof(buf));
		else
			ret = iiod->ops->read_attr(device, attr,
						   buf, sizeof(buf), debug);
	}
	tinyiiod_write_value(iiod, (int) ret);
	if (ret > 0) {
		buf[ret] = '\n';
		tinyiiod_write(iiod, buf, (size_t) ret + 1);
	}
}

/***************************************************************************//**
 * @brief tinyiiod_do_write_attr
*******************************************************************************/
void tinyiiod_do_write_attr(struct tinyiiod *iiod, const char *device,
			    const char *channel, bool ch_out, const char *attr,
			    size_t bytes, bool debug)
{
	char buf[2048] = {0};
	ssize_t ret;

	if (bytes > sizeof(buf))
		bytes = sizeof(buf);
	tinyiiod_read(iiod, buf, bytes);

	buf[bytes - 1] = '\0';

	if (channel)
		ret = iiod->ops->ch_write_attr(device, channel, ch_out,
					       attr, buf, bytes);
	else
		ret = iiod->ops->write_attr(device, attr, buf, bytes, debug);

	tinyiiod_write_value(iiod, (int) ret);
}

/***************************************************************************//**
 * @brief tinyiiod_do_open
*******************************************************************************/
void tinyiiod_do_open(struct tinyiiod *iiod, const char *device,
		      size_t sample_size, uint32_t mask)
{
	int32_t ret = iiod->ops->open(device, sample_size, mask);
	tinyiiod_write_value(iiod, ret);
}

/***************************************************************************//**
 * @brief tinyiiod_do_close
*******************************************************************************/
void tinyiiod_do_close(struct tinyiiod *iiod, const char *device)
{
	int32_t ret = iiod->ops->close(device);
	tinyiiod_write_value(iiod, ret);
}

/***************************************************************************//**
 * @brief tinyiiod_do_exit
*******************************************************************************/
int32_t tinyiiod_do_exit(struct tinyiiod *iiod)
{
	return iiod->ops->exit(iiod->instance_id);
}

/***************************************************************************//**
 * @brief tinyiiod_do_writebuf
*******************************************************************************/
int32_t tinyiiod_do_writebuf(struct tinyiiod *iiod,
			     const char *device, size_t bytes_count)
{
	char *pbuffer = (char*)malloc(bytes_count);
	if(pbuffer == NULL) {
		return -ENOMEM;
	}
	memset(pbuffer, 0, bytes_count);
	tinyiiod_read_nonblocking(iiod, pbuffer, bytes_count);
	tinyiiod_write_value(iiod, (int) bytes_count);
	size_t bytes_received = tinyiiod_read_wait(iiod, bytes_count);
	if(bytes_count != bytes_received) {
		return -EPIPE;
	}

	iiod->ops->write_device(device, pbuffer, bytes_count);
	tinyiiod_write_value(iiod, (int) bytes_count);
	free(pbuffer);

	return 0;
}

/***************************************************************************//**
 * @brief tinyiiod_do_readbuf
*******************************************************************************/
void tinyiiod_do_readbuf(struct tinyiiod *iiod,
			 const char *device, size_t bytes_count)
{
	int32_t ret;
	char *pbuf; // pointer to memory, where DMA writes, memory free not necessary
	uint32_t mask;

	ret = iiod->ops->get_mask(device, &mask);
	if (ret < 0) {
		tinyiiod_write_value(iiod, ret);
		return;
	}
	// read device
	ret = (int) iiod->ops->read_device(device, &pbuf, bytes_count);
	tinyiiod_write_value(iiod, ret);
	if (ret < 0)
		return;

	char buf_mask[10];
	snprintf(buf_mask, sizeof(buf_mask), "%08"PRIx32"\n", mask);
	tinyiiod_write_string(iiod, buf_mask);
	tinyiiod_write(iiod, pbuf, (size_t) ret);
}
