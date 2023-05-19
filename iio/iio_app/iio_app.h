/***************************************************************************//**
 *   @file   iio_app.h
 *   @brief  Header file of iio_app
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

#ifndef IIO_APP
#define IIO_APP

#include "iio.h"
#include "no_os_irq.h"
#include "no_os_uart.h"
#include "no_os_error.h"
#include "no_os_delay.h"

#if defined(NO_OS_LWIP_NETWORKING)
#include "lwip_socket.h"
#endif

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

#ifdef NO_OS_LWIP_NETWORKING
	struct lwip_network_desc *lwip_desc;
#endif
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

#ifdef NO_OS_LWIP_NETWORKING
	struct lwip_network_param lwip_param;
#endif
};

/** Register devices for an IIO application */
int iio_app_init(struct iio_app_desc **app,
		 struct iio_app_init_param app_init_param);

/** Start an IIO application */
int iio_app_run(struct iio_app_desc *app);

/** Remove resources allocated by the IIO application */
int iio_app_remove(struct iio_app_desc *app);

#endif
