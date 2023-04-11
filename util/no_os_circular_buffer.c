/***************************************************************************//**
 *   @file   no_os_circular_buffer.c
 *   @brief  Circular buffer implementation
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "no_os_circular_buffer.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

int32_t no_os_cb_cfg(struct no_os_circular_buffer *desc, int8_t *buff,
		     uint32_t size)
{
	if (!desc)
		return -EINVAL;

	memset(desc, 0, sizeof(*desc));
	desc->size = size;
	desc->buff = buff;

	return 0;
}

/**
 * @brief Create circular buffer structure.
 *
 * @note Circular buffer implementation is thread safe for one write
 * and one reader.
 * If multiple writer or multiple readers access the circular buffer then
 * function that updates the structure should be called inside a critical
 * critical section.
 *
 * @param desc - Where to store the circular buffer reference
 * @param buff_size - Buffer size
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t no_os_cb_init(struct no_os_circular_buffer **desc, uint32_t buff_size)
{
	struct no_os_circular_buffer	*ldesc;

	if (!desc || !buff_size)
		return -EINVAL;

	ldesc = (struct no_os_circular_buffer*)no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	*desc = ldesc;

	ldesc->size = buff_size;
	ldesc->buff = no_os_calloc(1, buff_size);
	if (!ldesc->buff) {
		no_os_free(ldesc);
		return -ENOMEM;
	}

	return 0;
}

/**
 * @brief Free the resources allocated for the circular buffer structure.
 * @param desc - Circular buffer reference
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t no_os_cb_remove(struct no_os_circular_buffer *desc)
{
	if (!desc)
		return -1;

	if (desc->buff)
		no_os_free(desc->buff);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Get the number of elements in the buffer.
 * @param desc - Circular buffer reference
 * @param size - Where to store size of data available to read
 * @return
 *  - 0   - No errors
 *  - -EINVAL   - Wrong parameters used
 *  - -NO_OS_EOVERRUN - A buffer overrun occurred
 */
int32_t no_os_cb_size(struct no_os_circular_buffer *desc, uint32_t *size)
{
	uint32_t nb_spins;

	if (!desc || !size)
		return -EINVAL;

	if (desc->write.spin_count > desc->read.spin_count)
		nb_spins = desc->write.spin_count - desc->read.spin_count;
	else
		/* Integer overflow on desc->write.spin_count */
		nb_spins = UINT32_MAX - desc->read.spin_count +
			   desc->write.spin_count + 1;

	if (nb_spins > 0)
		*size = desc->size + desc->write.idx - desc->read.idx;
	else
		*size = desc->write.idx - desc->read.idx;

	if (*size > desc->size) {
		*size = desc->size;
		return -NO_OS_EOVERRUN;
	}

	return 0;
}

/*
 * Functionality described at no_os_cb_prepare_async_write/read having the is_read
 * parameter to specifiy if it is a read or write operation.
 */
static int32_t no_os_cb_prepare_async_operation(struct no_os_circular_buffer
		*desc,
		uint32_t requested_size,
		void **buff,
		uint32_t *raw_size_available,
		bool is_read)
{
	struct no_os_cb_ptr	*ptr;
	uint32_t	available_size;
	int32_t		ret;

	if (!desc || !buff || !raw_size_available)
		return -EINVAL;

	ret = 0;
	/* Select if read or write index will be updated */
	ptr = is_read ? &desc->read : &desc->write;

	/* Only one transaction type possible at a single time */
	if (ptr->async_started)
		return -EBUSY;

	if (is_read) {
		ret = no_os_cb_size(desc, &available_size);
		if (ret == -NO_OS_EOVERRUN) {
			/* Update read index */
			desc->read.spin_count = desc->write.spin_count - 1;
#ifndef IIO_IGNORE_BUFF_OVERRUN_ERR
			desc->read.idx = desc->write.idx;
#endif
		}
		if (!available_size)
			/* No data to read */
			return 0;

		/* We can only read available data */
		requested_size = no_os_min(requested_size, available_size);
		if (!requested_size)
			return -EAGAIN;
	}

	/* Size to end of buffer */
	ptr->async_size = no_os_min(requested_size, desc->size - ptr->idx);

	*raw_size_available = ptr->async_size;

	/* Convert index to address in the buffer */
	*buff = (void *)(desc->buff + ptr->idx);

	ptr->async_started = true;

	return ret;
}

/*
 * Functionality described at no_os_cb_end_async_write/read having the is_read
 * parameter to specifiy if it is a read or write operation.
 */
static int32_t no_os_cb_end_async_operation(struct no_os_circular_buffer *desc,
		bool is_read)
{
	struct no_os_cb_ptr	*ptr;
	uint32_t	new_val;


	if (!desc)
		return -EINVAL;

	/* Select if read or write index will be updated */
	ptr = is_read ? &desc->read : &desc->write;

	/* Transaction not started */
	if (!ptr->async_started)
		return -1;

	/* Update pointer value */
	new_val = ptr->idx + ptr->async_size;
	if (new_val >= desc->size) {
		ptr->spin_count++;
		new_val %= desc->size;
	}
	ptr->idx = new_val;
	ptr->async_size = 0;
	ptr->async_started = false;

	return 0;
}

/*
 * Functionality described at cb_write/read having the is_read
 * parameter to specifiy if it is a read or write operation.
 */
static int32_t no_os_cb_operation(struct no_os_circular_buffer *desc,
				  void *data, uint32_t size,
				  bool is_read)
{
	uint8_t		*buff;
	uint32_t	available_size = 0;
	int32_t		ret;
	uint32_t	i;
	bool		sticky_overrun;

	if (!desc || !data || !size)
		return -EINVAL;

	sticky_overrun = 0;
	i = 0;
	while (i < size) {
		do {
			ret = no_os_cb_prepare_async_operation(desc, size - i,
							       (void **)&buff,
							       &available_size,
							       is_read);
		} while (ret == -EBUSY || ret == -EAGAIN);
		if (ret == -NO_OS_EOVERRUN)
			sticky_overrun = true;

		/* If no data is available return error */
		if(!available_size)
			return -1;

		if (is_read)
			memcpy((uint8_t *)data + i, buff, available_size);
		else
			memcpy(buff, (uint8_t *)data + i, available_size);

		no_os_cb_end_async_operation(desc, is_read);

		i += available_size;
	}

	if (sticky_overrun)
		return -NO_OS_EOVERRUN;

	return 0;
}

/**
 * @brief Prepare asynchronous write.
 *
 * Get the inside raw buffer to be used in DMA transactions.
 *
 * @param desc - Circular buffer reference
 * @param size_to_write - Number of bytes needed to write to the buffer.
 * @param write_buff - Address where to store the buffer where to write to.
 * @param size_avilable - no_os_min(size_to_write, size until end of allocated buffer)
 * @return
 *  - 0   - No errors
 *  - -EINVAL   - Wrong parameters used
 *  - -EBUSY    - Asynchronous transaction already started
 */
int32_t no_os_cb_prepare_async_write(struct no_os_circular_buffer *desc,
				     uint32_t size_to_write,
				     void **write_buff,
				     uint32_t *size_avilable)
{
	return no_os_cb_prepare_async_operation(desc, size_to_write, write_buff,
						size_avilable, 0);
}

/**
 * @brief Prepare asynchronous read.
 *
 * Get the inside raw buffer to be used in DMA transactions.
 *
 * @param desc - Circular buffer reference
 * @param size_to_read - Number of bytes needed to write to the buffer.
 * @param read_buff - Address where to store the buffer where data will be read.
 * @param size_avilable - no_os_min(size_to_read, size until end of allocated buffer)
 * @return
 *  - 0   - No errors
 *  - -EAGAIN   - No data available at this moment
 *  - -EINVAL   - Wrong parameters used
 *  - -EBUSY    - Asynchronous transaction already started
 *  - -NO_OS_EOVERRUN - An overrun occurred and some data have been overwritten
 */
int32_t no_os_cb_prepare_async_read(struct no_os_circular_buffer *desc,
				    uint32_t size_to_read,
				    void **read_buff,
				    uint32_t *size_avilable)
{
	return no_os_cb_prepare_async_operation(desc, size_to_read, read_buff,
						size_avilable, 1);
}

/**
 * \defgroup end_async_group End Ashyncronous functions
 * @brief End asynchronous transaction.
 *
 * @param desc - Circular buffer reference
 * @return
 *  - 0   - No errors
 *  - -1   - Asynchronous transaction not started
 *  - -EINVAL        - Wrong parameters used
 * @{
 */
int32_t no_os_cb_end_async_write(struct no_os_circular_buffer *desc)
{
	return no_os_cb_end_async_operation(desc, 0);
}

int32_t no_os_cb_end_async_read(struct no_os_circular_buffer *desc)
{

	return no_os_cb_end_async_operation(desc, 1);
}
/** @} */

/**
 * @brief Write data to the buffer (Blocking).
 * @param desc - Circular buffer reference
 * @param data - Buffer from where data is copied to the circular buffer
 * @param size - Size to write
 * @return
 *  - 0 - No errors
 *  - -EINVAL      - Wrong parameters used
 */
int32_t no_os_cb_write(struct no_os_circular_buffer *desc, const void *data,
		       uint32_t size)
{
	return no_os_cb_operation(desc, (void *)data, size, 0);
}

/**
 * @brief Read data from the buffer (Blocking).
 * @param desc - Circular buffer reference
 * @param data - Buffer where to data is copied from the circular buffer
 * @param size - Size to read
 * @return
 *  - 0   - No errors
 *  - -EINVAL   - Wrong parameters used
 *  - -NO_OS_EOVERRUN - An overrun occurred and some data have been overwritten
 */
int32_t no_os_cb_read(struct no_os_circular_buffer *desc, void *data,
		      uint32_t size)
{
	return no_os_cb_operation(desc, data, size, 1);
}
