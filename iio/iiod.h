/***************************************************************************//**
 *   @file   iiod.h
 *   @brief  Header file of iiod
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#ifndef IIOD_H
#define IIOD_H

#include <stdint.h>
#include <stdbool.h>

#include "iio.h"

/* Maximum nomber of iiod connections to allocate simultaneously */
#define IIOD_MAX_CONNECTIONS	10
#define IIOD_VERSION		"1.1.0000000"
#define IIOD_VERSION_LEN	(sizeof(IIOD_VERSION) - 1)

#define MAX_DEV_ID		64
#define MAX_TRIG_ID		64
#define MAX_CHN_ID		64
#define MAX_ATTR_NAME		256

enum iio_attr_type {
	IIO_ATTR_TYPE_DEBUG,
	IIO_ATTR_TYPE_BUFFER,
	IIO_ATTR_TYPE_CH_OUT,
	IIO_ATTR_TYPE_CH_IN,
	IIO_ATTR_TYPE_DEVICE,
};

struct iiod_attr {
	enum iio_attr_type type;
	/*
	 * Attribute name.
	 * If empty (""), all attributes of the specified iio_attr_type must
	 * be read/wrote
	 */
	const char *name;
	const char *channel;
};

struct iiod_ctx {
	/* Value specified in iiod_init_param.instance in iiod_init */
	void *instance;
	/* Value specified in iiod_conn_data.conn in iiod_conn_add */
	void *conn;
};

struct iiod_conn_data {
	/* Value to be used in iiod_ctx */
	void *conn;
	/* Buffer to store attributes or buffer data for a connection. */
	char *buf;
	/* Size of the provided buffer. It must fit the max attribute size */
	uint32_t len;
};

/* Functions should return a negative error code on failure */
struct iiod_ops {
	/*
	 * I/O operations
	 * Send and recv are used to send or receive data to/from a connection.
	 * They should send/receive the at maximum len bytes.
	 * They must return the number of bytes sent/received.
	 * They can return 0 or -EAGAIN when no data was processed.
	 * They can do not block. They will be called again if there is still
	 * data to be sent/recevied
	 */
	int (*send)(struct iiod_ctx *ctx, uint8_t *buf, uint32_t len);
	int (*recv)(struct iiod_ctx *ctx, uint8_t *buf, uint32_t len);

	/*
	 * This is the equivalent of libiio iio_device_create_buffer.
	 * Called in order to create a buffer to read or write data.
	 * read_buffer or write_buffer will follow with a maximum of samples
	 * (depending on the internal buffer).
	 * All calls with the same ctx will refer to this buffer until close is
	 * called.
	 */
	int (*open)(struct iiod_ctx *ctx, const char *device, uint32_t samples,
		    uint32_t mask, bool cyclic);
	/* Equivalent of iio_buffer_destroy */
	int (*close)(struct iiod_ctx *ctx, const char *device);

	/* Read data from opened buffer */
	int (*read_buffer)(struct iiod_ctx *ctx, const char *device, char *buf,
			   uint32_t bytes);
	/* Called to notify that buffer must be refiiled */
	int (*refill_buffer)(struct iiod_ctx *ctx, const char *device);

	/* Write data to opened buffer */
	int (*write_buffer)(struct iiod_ctx *ctx, const char *device,
			    const char *buf, uint32_t bytes);
	/* Called to notify that buffer must be pushed to hardware */
	int (*push_buffer)(struct iiod_ctx *ctx, const char *device);

	/*
	 * Attribute has to be read in buf and return the number of bytes
	 * written.
	 */
	int (*read_attr)(struct iiod_ctx *ctx, const char *device,
			 struct iiod_attr *attr, char *buf, uint32_t len);
	/* Attribute buf is filled with the attribute value. */
	int (*write_attr)(struct iiod_ctx *ctx, const char *device,
			  struct iiod_attr *attr, char *buf, uint32_t len);
	/* Simular with read_attr but trigger must be filled */
	int (*get_trigger)(struct iiod_ctx *ctx, const char *device,
			   char *trigger, uint32_t len);
	/*
	 * Simular with write_attr but trigger name is in trigger.
	 * If trigger is equals to "". Trigger must be removed.
	 */
	int (*set_trigger)(struct iiod_ctx *ctx, const char *device,
			   const char *trigger, uint32_t len);

	/* I don't know what this should be used for :) */
	int (*set_timeout)(struct iiod_ctx *ctx, uint32_t timeout);

	/* I don't know what this should be used for :) */
	int (*set_buffers_count)(struct iiod_ctx *ctx, const char *device,
				 uint32_t buffers_count);
};

/*
 * Internal structure.
 * It is created in iiod_init and must be passed to all fucntions
 */
struct iiod_desc;

/* Parameter to initialize iiod_desc */
struct iiod_init_param {
	struct iiod_ops *ops;
	/* Value to be send in each iiod_ctx from iiod_ops functions */
	void *instance;
	/*
	 * Xml description of the context and devices. It should exist until
	 * iiod_remove is called
	 */
	char *xml;
	/* Size of xml in bytes */
	uint32_t xml_len;
	/* Backend used by IIOD */
	enum physical_link_type phy_type;
};

/* Initialize desc. */
int32_t iiod_init(struct iiod_desc **desc, struct iiod_init_param *param);
/* Remove desc resources */
void iiod_remove(struct iiod_desc *desc);

/*
 * Notify iiod about a new connection in order to store context for it.
 * new_conn_id is set in order to reference the connection in iiod_conn_step
 */
int32_t iiod_conn_add(struct iiod_desc *desc, struct iiod_conn_data *data,
		      uint32_t *new_conn_id);
/* Remove conn_id from iiod. Provided data is returned in data */
int32_t iiod_conn_remove(struct iiod_desc *desc, uint32_t conn_id,
			 struct iiod_conn_data *data);
/* Advance in the state machine of a connection. Will not block */
int32_t iiod_conn_step(struct iiod_desc *desc, uint32_t conn_id);

#endif //IIOD_H
