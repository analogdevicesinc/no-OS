/***************************************************************************//**
 *   @file   circular_buffer.h
 *   @brief  Circular buffer library header
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Reference type for circular buffer
 *
 * Abstract type of the circular buffer, used as reference for the functions.
 */
struct circular_buffer;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t cb_init(struct circular_buffer **desc, uint32_t size);
int32_t cb_remove(struct circular_buffer *desc);
int32_t cb_size(struct circular_buffer *desc, uint32_t *size);

int32_t cb_write(struct circular_buffer *desc, const void *data,
		 uint32_t nb_elements);
int32_t cb_read(struct circular_buffer *desc, void *data, uint32_t nb_elements);

int32_t cb_prepare_async_write(struct circular_buffer *desc,
			       uint32_t raw_size_to_write,
			       void **write_buff,
			       uint32_t *raw_size_avilable);
int32_t cb_end_async_write(struct circular_buffer *desc);

int32_t cb_prepare_async_read(struct circular_buffer *desc,
			      uint32_t raw_size_to_read,
			      void **read_buff,
			      uint32_t *raw_size_avilable);
int32_t cb_end_async_read(struct circular_buffer *desc);

#endif
