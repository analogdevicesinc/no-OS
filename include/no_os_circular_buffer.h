/***************************************************************************//**
 *   @file   no_os_circular_buffer.h
 *   @brief  Circular buffer library header
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef _NO_OS_CIRCULAR_BUFFER_H_
#define _NO_OS_CIRCULAR_BUFFER_H_

#include <stdint.h>

/**
 * @struct no_os_cb_ptr
 * @brief Circular buffer pointer
 */
struct no_os_cb_ptr {
	/** Index of data in the buffer */
	uint32_t	idx;
	/** Counts the number of times idx exceeds the liniar buffer */
	uint32_t	spin_count;
	/** Set if async transaction is active */
	bool		async_started;
	/** Number of bytes to update after an async transaction is finished */
	uint32_t	async_size;
};

/**
 * @struct no_os_circular_buffer
 * @brief Circular buffer descriptor
 */
struct no_os_circular_buffer {
	/** Size of the buffer in bytes */
	uint32_t	size;
	/** Address of the buffer */
	int8_t		*buff;
	/** Write pointer */
	struct no_os_cb_ptr	write;
	/** Read pointer */
	struct no_os_cb_ptr	read;
};

int32_t no_os_cb_init(struct no_os_circular_buffer **desc, uint32_t size);
/* Configure cb structure with given parameters without memory allocation */
int32_t no_os_cb_cfg(struct no_os_circular_buffer *desc, int8_t *buf,
		     uint32_t size);
int32_t no_os_cb_remove(struct no_os_circular_buffer *desc);
int32_t no_os_cb_size(struct no_os_circular_buffer *desc, uint32_t *size);

int32_t no_os_cb_write(struct no_os_circular_buffer *desc, const void *data,
		       uint32_t nb_elements);
int32_t no_os_cb_read(struct no_os_circular_buffer *desc, void *data,
		      uint32_t nb_elements);

int32_t no_os_cb_prepare_async_write(struct no_os_circular_buffer *desc,
				     uint32_t raw_size_to_write,
				     void **write_buff,
				     uint32_t *raw_size_avilable);
int32_t no_os_cb_end_async_write(struct no_os_circular_buffer *desc);

int32_t no_os_cb_prepare_async_read(struct no_os_circular_buffer *desc,
				    uint32_t raw_size_to_read,
				    void **read_buff,
				    uint32_t *raw_size_avilable);
int32_t no_os_cb_end_async_read(struct no_os_circular_buffer *desc);

#endif //_NO_OS_CIRCULAR_BUFFER_H_
