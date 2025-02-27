/***************************************************************************//**
 *   @file   no_os_i3c.c
 *   @brief  Implementation of the I3C Interface
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include <string.h>
#include "no_os_i3c.h"
#include "no_os_error.h"
#include "no_os_mutex.h"
#include "no_os_alloc.h"

/**
 * @brief i3c_table contains the pointers towards the I3C buses.
*/
struct no_os_i3c_bus_desc *i3c_table [NO_OS_I3C_MAX_BUS_NUMBER + 1] = {NULL};

static enum no_os_i3c_slot_status no_os_i3c_addr_get_status(
	struct no_os_i3c_bus_desc *desc, uint8_t addr);
static bool no_os_i3c_addr_is_avail(struct no_os_i3c_bus_desc *desc,
				    uint8_t addr);
static void no_os_i3c_addr_init(struct no_os_i3c_bus_desc *desc);

/**
 * @brief Initialize the I3C device.
 * If the bus of the I3C device is not initialized, it will call to init it.
 * @param desc - The I3C device descriptor.
 * @param param - The structure that contains the I3C device parameters, to match PID.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_init(struct no_os_i3c_desc **desc,
		   const struct no_os_i3c_init_param *param)
{
	int ret;
	struct no_os_i3c_desc *device_desc;
	struct no_os_i3c_bus_desc **bus_desc;
	struct no_os_i3c_bus_init_param *param_bus;
	struct no_os_i3c_desc **it;
	uint8_t i;

	if (!param->bus->device_id ||
	    param->bus->device_id > NO_OS_I3C_MAX_BUS_NUMBER)

		return -EINVAL;

	bus_desc = &i3c_table[param->bus->device_id - 1];
	if (!*bus_desc) {
		/* Initialize bus if not already */
		ret = no_os_i3c_init_bus(bus_desc, param->bus);
		if (ret)
			return ret;
	}

	/* Initialize device */
	param_bus = param->bus;

	device_desc = (struct no_os_i3c_desc *)no_os_malloc(
			      sizeof(struct no_os_i3c_desc));
	if (!device_desc)
		return -ENOMEM;

	/* Set is_attached flag */
	if (!param->is_i3c || param->is_static)
		device_desc->is_attached = 1;
	else {
		for (i = 0; i < NO_OS_I3C_MAX_DEV_NUMBER; i++) {
			if ((*bus_desc)->daa_candidates[i].pid == param->pid)
				break;
		}

		if (i == NO_OS_I3C_MAX_DEV_NUMBER) {
			ret = -EFAULT;
			goto error_device;
		}
		device_desc->is_attached = (*bus_desc)->daa_candidates[i].is_attached;
		if (!device_desc->is_attached) {
			ret = -EPERM;
			goto error_device;
		}
	}

	/* Copy from init param to dev descriptor */
	device_desc->pid = param->pid;
	device_desc->addr = param->addr;
	device_desc->is_static = param->is_static;
	device_desc->is_i3c = param->is_i3c;
	device_desc->platform_ops = param_bus->platform_ops;
	device_desc->bus = *bus_desc;
	device_desc->event_callback = NULL;

	/* Find free slot */
	it = (*bus_desc)->devs;
	while (it < (*bus_desc)->devs + NO_OS_I3C_MAX_DEV_NUMBER) {
		if (!*it)
			break;
		it++;
	};
	if (it == (*bus_desc)->devs + NO_OS_I3C_MAX_DEV_NUMBER)
		goto error_device;

	*it = device_desc;

	ret = device_desc->platform_ops->i3c_ops_init(device_desc, param);
	if (ret)
		goto error_device;

	ret = device_desc->platform_ops->i3c_ops_is_dev_ready(device_desc);
	if (ret)
		goto error_device;

	*desc = device_desc;
	return 0;

error_device:
	no_os_free(device_desc);

	return ret;
}

/**
 * @brief Initialize the I3C bus.
 * @param desc - The I3C bus descriptor.
 * @param param - The structure that contains the I3C bus parameters.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_init_bus(struct no_os_i3c_bus_desc **desc,
		       const struct no_os_i3c_bus_init_param *param)
{
	struct no_os_i3c_bus_desc *bus_desc;
	bool has_static_i3c_addr = 0;
	int ret;
	int i = 0;
	uint8_t data;
	const struct no_os_i3c_init_param **it;

	if (!param || !param->platform_ops)
		return -EINVAL;
	if (!param->platform_ops->i3c_ops_init_bus ||
	    !param->platform_ops->i3c_ops_remove_bus)
		return -ENOSYS;

	bus_desc = (struct no_os_i3c_bus_desc *)no_os_calloc(1,
			sizeof(struct no_os_i3c_bus_desc));
	if (!bus_desc)
		return -ENOMEM;

	bus_desc->device_id = param->device_id;
	bus_desc->num_devs = param->num_devs;
	bus_desc->num_devs_unknown = 0;
	bus_desc->async_irq = 0;
	bus_desc->platform_ops = param->platform_ops;

	/*
	 * Execute the platform specific init routine,
	 * which allocate the extra features.
	 */
	ret = param->platform_ops->i3c_ops_init_bus(bus_desc, param);
	if (ret)
		goto error_bus_1;

	i3c_table[param->device_id - 1] = bus_desc;

	no_os_i3c_addr_init(bus_desc);

	/*
	 * Set status for addr, and
	 * If a device supports static addr, set it with CCC STAASA.
	 * If not, add to the PID-DA LUT for the DAA procedure.
	 */
	for (it = param->devs; it < param->devs + param->num_devs; it++) {
		if (!*it)
			continue;

		if (!no_os_i3c_addr_is_avail(bus_desc, (*it)->addr)) {
			ret = -EINVAL;
			goto error_bus_2;
		}

		if ((*it)->is_i3c) {
			if ((*it)->is_static) {
				has_static_i3c_addr = 1;
				no_os_i3c_addr_set_status(bus_desc, (*it)->addr,
							  NO_OS_I3C_ADDR_SLOT_I3C_DEV);
			} else {
				bus_desc->daa_candidates[i].pid = (*it)->pid;
				bus_desc->daa_candidates[i].addr = (*it)->addr;
				i++;
			}
		} else
			no_os_i3c_addr_set_status(bus_desc, (*it)->addr,
						  NO_OS_I3C_ADDR_SLOT_I2C_DEV);
	}

	/* Ensure other candidates fields are empty */
	memset(&bus_desc->daa_candidates[i], 0,
	       sizeof(struct no_os_i3c_daa_lut) * (NO_OS_I3C_MAX_DEV_NUMBER - i));

	/* Reset fully every device on the bus */
	data = NO_OS_I3C_CCC_RSTACT_WHOLE_TARGET;
	ret = no_os_i3c_send_ccc(*desc,
				 NO_OS_I3C_BCAST_ADDR,
				 NO_OS_I3C_CCC_RSTACT_BCAST, &data);
	if (ret)
		goto error_bus_2;
	/* Set I3C devices that support static addresses to use them */
	if (has_static_i3c_addr) {
		ret = no_os_i3c_send_ccc(bus_desc, NO_OS_I3C_BCAST_ADDR, NO_OS_I3C_CCC_SETAASA,
					 NULL);
		if (ret)
			goto error_bus_2;
	}

	ret = no_os_i3c_do_daa(bus_desc, 1);
	if (ret)
		goto error_bus_2;

	no_os_mutex_init(bus_desc->mutex);
	*desc = bus_desc;

	return 0;

error_bus_2:
	ret = param->platform_ops->i3c_ops_remove_bus(bus_desc);
error_bus_1:
	no_os_free(bus_desc);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_i3c_init().
 * @param desc - The I3C device descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_remove(struct no_os_i3c_desc *desc)
{
	struct no_os_i3c_bus_desc *bus_desc;
	struct no_os_i3c_desc **it;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_remove)
		return -ENOSYS;

	bus_desc = desc->bus;
	it = bus_desc->devs;
	while (it < bus_desc->devs + NO_OS_I3C_MAX_DEV_NUMBER) {
		if (*it == desc)
			*it = NULL;
		it++;
	}

	/* Only error case is !desc */
	desc->platform_ops->i3c_ops_remove(desc);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_i3c_init_bus.
 * Must remove all devices first, if not, -EFAULT is returned.
 * @param desc - The I3C bus descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_remove_bus(struct no_os_i3c_bus_desc *desc)
{
	struct no_os_i3c_desc **it;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_remove_bus)
		return -ENOSYS;

	/* Verify if all devices are removed */
	for (it = desc->devs; it < desc->devs + NO_OS_I3C_MAX_DEV_NUMBER; it++) {
		if (*it)
			return -EFAULT;
	}

	no_os_mutex_remove(desc->mutex);

	/* Only error case is !desc */
	desc->platform_ops->i3c_ops_remove_bus(desc);

	i3c_table[desc->device_id - 1] = NULL;
	no_os_free(desc);

	return 0;
}

/**
 * @brief Do DAA to assign the dynamic addresses.
 * @param desc - The I3C descriptor.
 * @param rstdaa - Do RSTDAA CCC before the DAA.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_do_daa(struct no_os_i3c_bus_desc *desc, bool rstdaa)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_do_daa)
		return -ENOSYS;

	return desc->platform_ops->i3c_ops_do_daa(desc, rstdaa);
}

/**
 * @brief Send CCC, either to a device device or broadcast to all.
 * @param desc - The I3C descriptor.
 * @param addr - Device address or broadcast address.
 * @param ccc - CCC RnW, CCC Length, CCC ID.
 * @param data - The buffer with the transmitted/received data.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_send_ccc(struct no_os_i3c_bus_desc *desc,
		       uint8_t addr,
		       uint32_t ccc,
		       uint8_t *data)
{
	int ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_send_ccc)
		return -ENOSYS;

	if ((NO_OS_I3C_CCC_GET_RNW(ccc) && addr == NO_OS_I3C_BCAST_ADDR) ||
	    ((addr == NO_OS_I3C_BCAST_ADDR) && (ccc & NO_OS_I3C_CCC_DIRECT)) ||
	    ((addr != NO_OS_I3C_BCAST_ADDR) && !(ccc & NO_OS_I3C_CCC_DIRECT)))
		return -EINVAL;

	no_os_mutex_lock(desc->mutex);
	ret = desc->platform_ops->i3c_ops_send_ccc(desc, addr, NO_OS_I3C_CCC_ADDR(ccc),
			NO_OS_I3C_CCC_GET_RNW(ccc), NO_OS_I3C_CCC_GET_DEF(ccc),
			data, NO_OS_I3C_CCC_GET_LEN(ccc));
	no_os_mutex_unlock(desc->mutex);

	return ret;
}

/**
 * @brief Send CCC to device device.
 * @param desc - The I3C descriptor.
 * @param ccc - CCC RnW, CCC length, CCC id.
 * @param data - The buffer with the transmitted/received data.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_send_direct_ccc(struct no_os_i3c_desc *desc,
			      uint32_t ccc,
			      uint8_t *data)
{
	if (!desc || !desc->bus)
		return -EINVAL;
	if (!(ccc & NO_OS_I3C_CCC_DIRECT))
		return -EINVAL;

	return no_os_i3c_send_ccc(desc->bus, desc->addr, ccc, data);
}

/**
 * @brief Write data to device device.
 * @param desc - The I3C descriptor.
 * @param data - The buffer with the transmitted data.
 * @param size - Number of bytes to write.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_write(struct no_os_i3c_desc *desc,
		    uint8_t *data,
		    uint8_t size)
{
	int ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_write)
		return -ENOSYS;

	no_os_mutex_lock(desc->bus->mutex);
	ret = desc->bus->platform_ops->i3c_ops_write(desc,
			data, size);
	no_os_mutex_unlock(desc->bus->mutex);

	return ret;
}

/**
 * @brief Read data from device device.
 * @param desc - The I3C descriptor.
 * @param data - The buffer with the received data.
 * @param size - Number of bytes to read.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_read(struct no_os_i3c_desc *desc,
		   uint8_t *data,
		   uint8_t size)
{
	int ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_read)
		return -ENOSYS;

	no_os_mutex_lock(desc->bus->mutex);
	ret = desc->bus->platform_ops->i3c_ops_read(desc,
			data, size);
	no_os_mutex_unlock(desc->bus->mutex);

	return ret;
}

/**
 * @brief Read and write the device.
 * @param desc - The I3C descriptor.
 * @param tx_data - The buffer with the transmitted data.
 * @param tx_data_len - Number of bytes to write.
 * @param rx_data - The buffer with the received data.
 * @param rx_data_len - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_i3c_write_and_read(struct no_os_i3c_desc *desc,
			     uint8_t *tx_data,
			     uint8_t tx_data_len,
			     uint8_t *rx_data,
			     uint8_t rx_data_len)
{
	int ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_read)
		return -ENOSYS;

	no_os_mutex_lock(desc->bus->mutex);
	ret = desc->bus->platform_ops->i3c_ops_write_and_read(desc,
			tx_data, tx_data_len, rx_data, rx_data_len);
	no_os_mutex_unlock(desc->bus->mutex);

	return ret;
}

/**
 * @brief Configure I3C interrupts.
 * @param desc - The I3C descriptor.
 * @param irq - The interrupt type to enable.
 * @param en - 1 to enable, 0 to disable irq type.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_i3c_conf_irq(struct no_os_i3c_bus_desc *desc,
		       uint8_t irq, bool en)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_conf_irq)
		return -ENOSYS;

	return desc->platform_ops->i3c_ops_conf_irq(desc, irq, en);
}

/**
 * @brief Enable I3C non-blocking interrupts.
 * @param desc - The I3C descriptor.
 * @param en - 1 to enable, 0 to disable async callback.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int no_os_i3c_async_irq(struct no_os_i3c_bus_desc *desc, bool en)
{
	if (!desc)
		return -EINVAL;

	desc->async_irq = en;
	return 0;
}

/**
 * @brief Wait I3C interrupt.
 * Set async_irq on call to false to ensure single entry point.
 * @param desc - The I3C descriptor.
 * @param irq - The interrupt to wait for.
 * @return 0 in case of success, -EINVAL if in async mode or error codes.
 */
int no_os_i3c_wait_irq(struct no_os_i3c_bus_desc* desc,
		       uint8_t irq)
{
	if (!desc || desc->async_irq)
		return -EINVAL;

	while (!(desc->irq_events & irq)) {}

	return no_os_i3c_call_irq(desc);
}

/**
 * @brief Non-blocking I3C interrupt.
 * Since a payload is always retrieved with it, also update the no_os_i3c_ccc_info fields.
 * @param desc - The I3C descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_i3c_call_irq(struct no_os_i3c_bus_desc* desc)
{
	struct no_os_i3c_desc **it;
	int ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_get_ccc_info)
		return -ENOSYS;

	ret = desc->platform_ops->i3c_ops_get_ccc_info(desc, desc->irq_events);
	if (ret)
		return ret;

	it = desc->devs;
	while (it < desc->devs + NO_OS_I3C_MAX_DEV_NUMBER) {
		if (*it && (*it)->addr == desc->ccc_info.ibi_cr_addr) {
			if ((*it)->event_callback) {
				(*it)->event_callback((*it), desc->ccc_info.ibi_payload,
						      desc->ccc_info.ibi_payload_len);
			}
			break;
		}
		it++;
	};

	return ret;
}

/**
 * @brief Get a free address.
 * @param desc - The I3C bus descriptor.
 * @param start_addr - Start address to search from.
 * @return The free address or 0 if no address is available.
 */
uint8_t no_os_i3c_addr_get_free(struct no_os_i3c_bus_desc *desc,
				uint8_t start_addr)
{
	enum no_os_i3c_slot_status status;
	uint8_t addr;

	for (addr = start_addr; addr < NO_OS_I3C_MAX_ADDR; addr++) {
		status = no_os_i3c_addr_get_status(desc, addr);
		if (status == NO_OS_I3C_ADDR_SLOT_FREE)
			return addr;
	}

	return 0;
}

/**
 * @brief Set status of an address.
 * @param desc - The I3C bus descriptor.
 * @param addr - The address to set.
 * @param status - The value to write.
 */
void no_os_i3c_addr_set_status(struct no_os_i3c_bus_desc *desc,
			       uint8_t addr, enum no_os_i3c_slot_status status)
{
	unsigned int slot, bitpos;
	unsigned int *ptr;

	if (addr > NO_OS_I3C_I2C_MAX_ADDR)
		return;

	slot = NO_OS_I3C_ADDR_GET_SLOT(addr);
	bitpos = NO_OS_I3C_ADDR_GET_POS(addr);

	ptr = desc->addrslots + slot;
	// Unset old value
	*ptr &= ~(NO_OS_I3C_ADDR_SLOT_STATUS_MASK << bitpos);
	// Set new value
	*ptr |= (status & NO_OS_I3C_ADDR_SLOT_STATUS_MASK) << bitpos;
}

/**
 * @brief Get status of an address.
 * @param desc - The I3C bus descriptor.
 * @param start_addr - Start address to search from.
 * @return The free address or -ENOMEM if no available address.
 */
static enum no_os_i3c_slot_status no_os_i3c_addr_get_status(
	struct no_os_i3c_bus_desc *desc, uint8_t addr)
{
	unsigned int slot, bitpos;
	enum no_os_i3c_slot_status status;

	if (addr > NO_OS_I3C_I2C_MAX_ADDR)
		return NO_OS_I3C_ADDR_SLOT_RSVD;

	slot = NO_OS_I3C_ADDR_GET_SLOT(addr);
	bitpos = NO_OS_I3C_ADDR_GET_POS(addr);

	status = desc->addrslots[slot];
	status >>= bitpos;

	return status & NO_OS_I3C_ADDR_SLOT_STATUS_MASK;
}

/**
 * @brief Check if address is available.
 * @param desc - The I3C bus descriptor.
 * @param addr - Address to check.
 * @return 1 if available, 0 if not.
 */
static bool no_os_i3c_addr_is_avail(struct no_os_i3c_bus_desc *desc,
				    uint8_t addr)
{
	enum no_os_i3c_slot_status status;

	status = no_os_i3c_addr_get_status(desc, addr);

	return status == NO_OS_I3C_ADDR_SLOT_FREE;
}

/**
 * @brief Initialize address buffer, reserving reserved addresses.
 * @param desc - The I3C bus descriptor.
 */
static void no_os_i3c_addr_init(struct no_os_i3c_bus_desc *desc)
{
	/* Addresses 0 to 7 are reserved. */
	for (int i = 0; i < 8; i++)
		no_os_i3c_addr_set_status(desc, i, NO_OS_I3C_ADDR_SLOT_RSVD);

	/*
	 * Reserve broadcast address and all addresses that might collide
	 * with the broadcast address when facing a single bit error.
	 */
	no_os_i3c_addr_set_status(desc, NO_OS_I3C_BCAST_ADDR,
				  NO_OS_I3C_ADDR_SLOT_RSVD);
	for (int i = 0; i < 7; i++)
		no_os_i3c_addr_set_status(desc, NO_OS_I3C_BCAST_ADDR ^ NO_OS_BIT(i),
					  NO_OS_I3C_ADDR_SLOT_RSVD);
}

/**
 * @brief Attach event callback.
 * @param desc - The I3C device descriptor.
 * @param callback - Method to call on event.
 */
void no_os_i3c_attach_callback(struct no_os_i3c_desc *desc,
			       void (*callback)(struct no_os_i3c_desc*, uint32_t, uint32_t))
{
	desc->event_callback = callback;
}

/**
 * @brief Detach event callback.
 * @param desc - The I3C device descriptor.
 */
void no_os_i3c_detach_callback(struct no_os_i3c_desc *desc)
{
	no_os_i3c_attach_callback(desc, NULL);
}

/**
 * @brief Transfer a list of messages using DMA.
 * Non-blocking, invokes a callback after the last message is concluded.
 * @param desc - The I3C descriptor.
 * @param msgs - The list of messages to transfer.
 * @param len - The number of messages to transfer.
 * @param callback - The callback to invoke after the transfer is concluded.
 * @param ctx - The context to pass to the callback.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_i3c_transfer_dma_async(struct no_os_i3c_desc *desc,
				     struct no_os_i3c_msg *msgs,
				     uint32_t len,
				     void (*callback)(void *),
				     void *ctx)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_transfer_dma_async)
		return -ENOSYS;

	no_os_mutex_lock(desc->bus->mutex);
	return desc->platform_ops->i3c_ops_transfer_dma_async(
		       desc, msgs, len, callback, ctx);
}

/**
 * @brief Abort the ongoing DMA transaction
 * @param desc - The I3C descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_i3c_abort_dma(struct no_os_i3c_desc *desc)
{
	int32_t ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i3c_ops_abort_dma)
		return -ENOSYS;

	ret = desc->platform_ops->i3c_ops_abort_dma(desc);
	no_os_mutex_unlock(desc->bus->mutex);

	return ret;
}
