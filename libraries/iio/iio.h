/***************************************************************************//**
 *   @file   iio.h
 *   @brief  Header file of iio
 *   @author Cristian Pop (cristian.pop@analog.com)
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

#ifndef IIO_H_
#define IIO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_types.h"
#include "uart.h"
#ifdef ENABLE_IIO_NETWORK
#include "tcp_socket.h"
#endif

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum pysical_link_type {
	USE_UART,
#ifdef ENABLE_IIO_NETWORK
	USE_NETWORK
#endif
};

struct iio_desc;

struct iio_init_param {
	enum pysical_link_type	phy_type;
	union {
		struct uart_desc *uart_desc;
#ifdef ENABLE_IIO_NETWORK
		struct tcp_socket_init_param *tcp_socket_init_param;
#endif
	};
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Set communication ops and read/write ops that will be called from
 * "libtinyiiod". */
ssize_t iio_init(struct iio_desc **desc, struct iio_init_param *init_param);
/* Free the resources allocated by iio_init(). */
ssize_t iio_remove(struct iio_desc *desc);
/* Execut an iio step. */
ssize_t iio_step(struct iio_desc *desc);
/* Register interface. */
ssize_t iio_register(struct iio_desc *desc, struct iio_device *dev_descriptor,
		     char *name, void *dev_instance,
		     struct iio_data_buffer *read_buff,
		     struct iio_data_buffer *write_buff);
/* Unregister interface. */
ssize_t iio_unregister(struct iio_desc *desc, char *name);
int32_t iio_parse_value(char *buf, enum iio_val fmt,
			int32_t *val, int32_t *val2);
ssize_t iio_format_value(char *buf, size_t len, enum iio_val fmt,
			 int32_t size, int32_t *vals);

#endif /* IIO_H_ */
