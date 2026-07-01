/***************************************************************************//**
 *   @file   iio_app.h
 *   @brief  Header file of iio_app
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

#ifndef IIO_APP
#define IIO_APP

#include <stdbool.h>
#include "iio.h"
#include "no_os_irq.h"
#include "no_os_uart.h"
#include "no_os_error.h"
#include "no_os_delay.h"

#include "no_os_net.h"

#define IIO_APP_DEVICE(_name, _dev, _dev_descriptor, _read_buff, _write_buff, _default_trigger_id) {\
	.name = _name,\
	.dev = _dev,\
	.dev_descriptor = _dev_descriptor,\
	.read_buff = _read_buff,\
	.write_buff = _write_buff,\
	.default_trigger_id = _default_trigger_id\
}

#define IIO_APP_TRIGGER(_name, _trig, _trig_descriptor) {\
	.name = _name,\
	.trig = _trig,\
	.descriptor = _trig_descriptor,\
}

struct iio_data_buffer {
	uint32_t	size;
	void		*buff;
};

struct iio_app_device {
	char *name;
	void *dev;
	struct iio_device *dev_descriptor;
	struct iio_data_buffer *read_buff;
	struct iio_data_buffer *write_buff;
	char *default_trigger_id;
};

/**
 * @struct iio_app_desc
 * @brief IIO application descriptor.
 */
struct iio_app_desc {
	/** UART descriptor to be used */
	struct no_os_uart_desc *uart_desc;
	/** IRQ descriptor to be used */
	void *irq_desc;
	/**  IIO descriptor to be returned */
	struct iio_desc *iio_desc;
	/** Function to be called each step */
	int (*post_step_callback)(void *arg);
	/** Function parameteres */
	void *arg;

	/** Network interface used by the IIO server (NULL if UART only) */
	struct no_os_net_desc *net_desc;
	/** Set when iio_app brought up net_desc itself (and must free it) */
	bool net_owned;
};

/**
 * @struct iio_app_init_param
 * @brief IIO application descriptor initialization parameters.
 */
struct iio_app_init_param {
	/** Array of context attribute name/value pairs */
	struct iio_ctx_attr *ctx_attrs;
	/** Number of context attributes in the array above */
	uint32_t nb_ctx_attr;
	/** Array of IIO devices */
	struct iio_app_device *devices;
	/** Number of devices */
	uint32_t nb_devices;
	/** IIO triggers to be used */
	struct iio_trigger_init *trigs;
	/** Number of triggers to be used */
	int32_t nb_trigs;
	/** UART init params */
	struct no_os_uart_init_param uart_init_params;
	/** IRQ descriptor to be used */
	void *irq_desc;
	/** Function to be called each step */
	int (*post_step_callback)(void *arg);
	/** Function parameteres */
	void *arg;

	/*
	 * Networking (optional). Two ways to attach a network interface to the
	 * IIO server, mirroring the no_os_net bus/device split:
	 *
	 * - net_desc:  a network interface the caller already brought up with
	 *   no_os_net_init() and continues to own (e.g. lwIP, W5500). iio_app
	 *   forwards it and does NOT free it.
	 *
	 * - net_param: parameters for an interface iio_app should bring up and
	 *   own itself (e.g. Linux sockets, or the ESP8266 wifi backend, whose
	 *   "extra" is a struct wifi_net_param). Freed on iio_app_remove().
	 *
	 * Leave both NULL for a UART-only server. If both are set, net_desc wins.
	 */
	struct no_os_net_desc *net_desc;
	struct no_os_net_init_param *net_param;
};

/** Register devices for an IIO application */
int iio_app_init(struct iio_app_desc **app,
		 struct iio_app_init_param app_init_param);

/** Start an IIO application */
int iio_app_run(struct iio_app_desc *app);

/** Remove resources allocated by the IIO application */
int iio_app_remove(struct iio_app_desc *app);

#endif
