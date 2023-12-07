/***************************************************************************//**
 *   @file   no_os_i3c.c
 *   @brief  Implementation of the I3C Interface
 *   @author @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include <inttypes.h>
#include <stdlib.h>
#include "no_os_i3c.h"
#include "no_os_error.h"
#include "no_os_mutex.h"
#include "no_os_alloc.h"

static enum i3c_addr_slot_status
no_os_i3c_addr_get_status(struct no_os_i3c_master_desc *desc, uint8_t addr)
{
	int status, bitpos = addr * 2;

	if (addr > I2C_MAX_ADDR)
		return I3C_ADDR_SLOT_RSVD;

	status = desc->addrslots[bitpos / NO_OS_BITS_PER_LONG];
	status >>= bitpos % NO_OS_BITS_PER_LONG;

	return status & I3C_ADDR_SLOT_STATUS_MASK;
}

void no_os_i3c_addr_set_status(struct no_os_i3c_master_desc *desc,
		       uint8_t addr, enum i3c_addr_slot_status status)
{
	int bitpos = addr * 2;
	uint32_t *ptr;

	if (addr > I2C_MAX_ADDR)
		return;

	ptr = desc->addrslots + (bitpos / NO_OS_BITS_PER_LONG);
	*ptr &= ~((unsigned long)I3C_ADDR_SLOT_STATUS_MASK <<
						(bitpos % NO_OS_BITS_PER_LONG));
	*ptr |= (unsigned long)status << (bitpos % NO_OS_BITS_PER_LONG);
}

// TODO Are we even going to check for addr collision?
static bool no_os_i3c_addr_is_avail(struct no_os_i3c_master_desc *desc,
		       uint8_t addr)
{
	enum i3c_addr_slot_status status;

	status = no_os_i3c_addr_get_status(desc, addr);

	return status == I3C_ADDR_SLOT_FREE;
}

int32_t no_os_i3c_addr_get_free(struct no_os_i3c_master_desc *desc,
		       uint8_t start_addr)
{
	enum i3c_addr_slot_status status;
	uint8_t addr;

	for (addr = start_addr; addr < I3C_MAX_ADDR; addr++) {
		status = no_os_i3c_addr_get_status(desc, addr);
		if (status == I3C_ADDR_SLOT_FREE)
			return addr;
	}

	return -ENOMEM;
}

static void no_os_i3c_addr_init(struct no_os_i3c_master_desc *desc)
{
	int i;

	/* Addresses 0 to 7 are reserved. */
	for (i = 0; i < 8; i++)
		no_os_i3c_addr_set_status(desc, i, I3C_ADDR_SLOT_RSVD);

	/*
	 * Reserve broadcast address and all addresses that might collide
	 * with the broadcast address when facing a single bit error.
	 */
	no_os_i3c_addr_set_status(desc, I3C_BROADCAST_ADDR,
				     I3C_ADDR_SLOT_RSVD);
	for (i = 0; i < 7; i++)
		no_os_i3c_addr_set_status(desc, I3C_BROADCAST_ADDR ^ NO_OS_BIT(i),
					     I3C_ADDR_SLOT_RSVD);
}

/**
 * @brief Initialize the I3C communication peripheral.
 * @param desc - The I3C descriptor.
 * @param param - The structure that contains the I3C parameters.
 * @param param - The device id of the I3C controller to initialize.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_i3c_init(struct no_os_i3c_master_desc **desc,
			const struct no_os_i3c_platform_ops *ops,
			uint8_t device_id)
{
	int32_t ret;

	if (!ops)
		return -EINVAL;

	if (!ops->i3c_ops_init)
		return -ENOSYS;

	/*
	 * Execute the platform specific init routine, which
	 * might configure its internal logic to match the bus limitations.
	 */
	ret = ops->i3c_ops_init(desc, ops, device_id);
	if (ret)
		goto err_init;

	no_os_i3c_addr_init(*desc);

	/*
	 * Attach all devices with static address, and set status for addr.
	 */
	struct no_os_i3c_slave_desc **it = i3c_devs_desc;
	for (; it < i3c_devs_stop_addr; it++) {
		// TODO Nullyfy i3c_devs_desc first
		if (!*it)
			continue;

		if ((*it)->device_id == device_id) {
			(*it)->is_attached = 1;
			if ((*it)->is_i3c) {
				if ((*it)->is_static)
					no_os_i3c_addr_set_status(*desc, (*it)->addr, I3C_ADDR_SLOT_I3C_DEV);
			} else
				no_os_i3c_addr_set_status(*desc, (*it)->addr, I3C_ADDR_SLOT_I2C_DEV);
		}
	}
	return 0;


err_init:
	no_os_i3c_remove(*desc);

	return ret;
}

/**
 * @brief Attach I3C descriptor to driver pointer.
 * @param desc - The I3C descriptor.
 * @param param - The structure that contains the I3C parameters, to match pid.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_i3c_init_driver(struct no_os_i3c_slave_desc **desc,
		       const struct no_os_i3c_init_param *param)
{
	struct no_os_i3c_slave_desc **it = i3c_devs_desc;
	for (; it < i3c_devs_stop_addr; it++) {
		if (!*it)
			continue;

		if ((*it)->pid == param->pid)
			break;
	}
	if (it == i3c_devs_stop_addr)
		return -ENXIO;

	if (!(*it)->is_attached)
		return -EPERM;

	*desc = *it;
	return 0;
}

/**
 * @brief Do I3C DAA to assign the dynamic addresses.
 * @param devs- The structure that contains peripherals I3C parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_i3c_do_daa(struct no_os_i3c_master_desc *desc)
{
	int32_t ret;
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_do_daa)
		return -ENOSYS;

	/*
	 * Reset all dynamic address that may have been assigned before
	 * (assigned by the bootloader for example).
	 */
	// TODO RSTDAA, DISEC
	//
	// Do DAA to assign the DA to the I3C peripheral
	ret = desc->platform_ops->i3c_ops_do_daa(desc);

	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_i3c_init().
 * @param desc - The I3C descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_i3c_remove(struct no_os_i3c_master_desc *desc)
{
	no_os_mutex_remove(desc->mutex);

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_remove)
		return -ENOSYS;

	return desc->platform_ops->i3c_ops_remove(desc);
}

/**
 * @brief Detach I3C descriptor to driver pointer.
 * @param desc - The I3C descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_i3c_remove_driver(struct no_os_i3c_slave_desc **desc)
{
	if (!desc)
		return -EINVAL;

	*desc = NULL;
	return 0;
}

/**
 * @brief Send I3C CCC to slave device.
 * @param desc - The I3C descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_i3c_send_ccc(struct no_os_i3c_slave_desc *desc,
			uint8_t *data,
			uint8_t bytes_number)
{
	int32_t ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_send_ccc)
		return -ENOSYS;

	no_os_mutex_lock(desc->controller->mutex);
	ret = desc->controller->platform_ops->i3c_ops_send_ccc(desc, data, bytes_number);
	no_os_mutex_unlock(desc->controller->mutex);

	return ret;
}


/**
 * @brief I3C Write data to slave device.
 * @param desc - The I3C descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_i3c_priv_write(struct no_os_i3c_slave_desc *desc,
			uint8_t *data,
			uint8_t bytes_number)
{
	int32_t ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_priv_write)
		return -ENOSYS;

	no_os_mutex_lock(desc->controller->mutex);
	ret = desc->controller->platform_ops->i3c_ops_priv_write(desc, data, bytes_number);
	no_os_mutex_unlock(desc->controller->mutex);

	return ret;
}

/**
 * @brief I3C Read data from slave device.
 * @param desc - The i3c descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_i3c_priv_read(struct no_os_i3c_slave_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number)
{
	int32_t ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_priv_read)
		return -ENOSYS;

	no_os_mutex_lock(desc->controller->mutex);
	ret = desc->controller->platform_ops->i3c_ops_priv_read(desc, data, bytes_number);
	no_os_mutex_unlock(desc->controller->mutex);

	return ret;
}
