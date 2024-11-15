/***************************************************************************//**
 *   @file   iio.h
 *   @brief  Header file of iio
 *   @author Cristian Pop (cristian.pop@analog.com)
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_H_
#define IIO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_types.h"
#include "no_os_uart.h"
#if defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING)
#include "tcp_socket.h"
#endif

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum physical_link_type {
	USE_UART,
	USE_LOCAL_BACKEND,
	USE_NETWORK
};

struct iio_desc;

struct iio_device_init {
	char *name;
	void *dev;
	struct iio_device *dev_descriptor;
	/*
	 * IIO buffer implementation can use a user provided buffer in raw_buf.
	 * If raw_buf is NULL and iio_device has buffer callback function set,
	 * it will allocate memory for it when needed.
	 */
	int8_t *raw_buf;
	/* Length of raw_buf */
	uint32_t raw_buf_len;
	/* If set, trigger will be linked to this device */
	char *trigger_id;
};

struct iio_trigger_init {
	char *name;
	void *trig;
	struct iio_trigger *descriptor;
};

/**
 * @struct iio_ctx_attr
 * @brief Structure holding the context attribute members
 */
struct iio_ctx_attr {
	/** Attribute name */
	const char *name;
	/** Attribute value */
	const char *value;
};

/**
 * @struct iio_local_backend
 * @brief Structure holding the local backend init parameters
 */
struct iio_local_backend {
	int(*local_backend_event_read)(void *conn, uint8_t *buf, uint32_t len);
	int(*local_backend_event_write)(void *conn, uint8_t *buf, uint32_t len);
	char *local_backend_buff;
	uint32_t local_backend_buff_len;
};

struct iio_init_param {
	enum physical_link_type	phy_type;
	union {
		struct no_os_uart_desc *uart_desc;
#if defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING)
		struct tcp_socket_init_param *tcp_socket_init_param;
#endif
	};
	struct iio_local_backend *local_backend;
	struct iio_ctx_attr *ctx_attrs;
	uint32_t nb_ctx_attr;
	struct iio_device_init *devs;
	uint32_t nb_devs;
	struct iio_trigger_init *trigs;
	uint32_t nb_trigs;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Set communication ops and read/write ops. */
int iio_init(struct iio_desc **desc, struct iio_init_param *init_param);
/* Free the resources allocated by iio_init(). */
int iio_remove(struct iio_desc *desc);
/* Execut an iio step. */
int iio_step(struct iio_desc *desc);
/* Signal iio that a trigger has been triggered.
 * This will be called in interrupt context. An application callback will be
   called in interrupt context if trigger is synchronous with the interrupt
   (is_synchronous = true) or will be called from iio_step if trigger is
   asynchronous (is_synchronous = false) */
int iio_process_trigger_type(struct iio_desc *desc, char *trigger_name);

int32_t iio_parse_value(char *buf, enum iio_val fmt,
			int32_t *val, int32_t *val2);
int iio_format_value(char *buf, uint32_t len, enum iio_val fmt,
		     int32_t size, int32_t *vals);

/* DMA buffer functions. */
/* Get buffer addr where to write iio_buffer.size bytes */
int iio_buffer_get_block(struct iio_buffer *buffer, void **addr);
/* To be called to mark last iio_buffer_read as done */
int iio_buffer_block_done(struct iio_buffer *buffer);

/* Trigger buffer functions. */
/* Write to buffer iio_buffer.bytes_per_scan bytes from data */
int iio_buffer_push_scan(struct iio_buffer *buffer, void *data);
/* Read from buffer iio_buffer.bytes_per_scan bytes into data */
int iio_buffer_pop_scan(struct iio_buffer *buffer, void *data);

#endif /* IIO_H_ */
