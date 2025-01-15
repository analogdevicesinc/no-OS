/***************************************************************************//**
 *   @file   stm32/stm32_i3c.c
 *   @brief  Implementation of stm32 I3C driver.
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
#if (USE_HAL_I3C_MODULE == 1)
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_i3c.h"
#include "stm32_i3c.h"

static int32_t stm32_i3c_config_extra(struct no_os_i3c_desc *desc,
				      struct stm32_i3c_device_desc *xdesc);
static inline void stm32_i3c_find_i3c_table(I3C_HandleTypeDef *hi3c,
		struct no_os_i3c_bus_desc **iter);
static inline int32_t stm32_i3c_wait_ready(I3C_HandleTypeDef *hi3c);

/**
 * @brief I3C target request a dynamic address callback.
 * When called, assigns a dynamic address to the target.
 * @param hi3c - Pointer to an I3C_HandleTypeDef structure that contains the
 * configuration information for the specified I3C.
 * @param targetPayload - The {BCR, DCR, PID} yielded by the device during
 * the DAA.
 */
void HAL_I3C_TgtReqDynamicAddrCallback(I3C_HandleTypeDef *hi3c,
				       uint64_t targetPayload)
{
	uint8_t addr;
	uint64_t pid;
	struct no_os_i3c_bus_desc *it;
	uint8_t i, j;

	no_os_memswap64(&targetPayload, sizeof(targetPayload), sizeof(targetPayload));
	pid = STM32_I3C_TP_PID(targetPayload);

	stm32_i3c_find_i3c_table(hi3c, &it);
	if (!it)
		return;

	/* Search for device with pid */
	for (i = 0 ; i < NO_OS_I3C_MAX_DEV_NUMBER; i++) {
		if (pid == it->daa_candidates[i].pid)
			break;
	}

	if (i == NO_OS_I3C_MAX_DEV_NUMBER) {
		/* Device is unknown */
		/* No error handling, #free_addr >> #devs_on_bus */
		addr = no_os_i3c_addr_get_free(it, 8);
		if (HAL_I3C_Ctrl_SetDynAddr(hi3c, addr) != HAL_OK)
			return;
		j = (it)->num_devs_unknown;
		(it)->daa_unknown[j].pid = pid;
		(it)->daa_unknown[j].addr = addr;
		(it)->daa_unknown[j].is_attached = 1;
		if (j < NO_OS_I3C_MAX_DEV_NUMBER - 1)
			(it)->num_devs_unknown++;
	} else {
		/* Device is known */
		addr = (it)->daa_candidates[i].addr;
		if (HAL_I3C_Ctrl_SetDynAddr(hi3c, addr) != HAL_OK)
			return;
		(it)->daa_candidates[i].is_attached = 1;
	}

	/* Send associated dynamic address */
	no_os_i3c_addr_set_status(it, addr, NO_OS_I3C_ADDR_SLOT_I3C_DEV);

	/* Don't store BCR and DCR, device_desc may not be initialized */
}

/**
 * @brief Controller dynamic address assignment Complete callback.
 * @param hi3c - Pointer to an I3C_HandleTypeDef structure that contains the
 * configuration information for the specified I3C.
 */
void HAL_I3C_CtrlDAACpltCallback(I3C_HandleTypeDef *hi3c) {}
/**
 * @brief Controller Transmit Complete callback.
 * @param hi3c - Pointer to an I3C_HandleTypeDef structure that contains the
 * configuration information for the specified I3C.
 */
void HAL_I3C_CtrlTxCpltCallback(I3C_HandleTypeDef *hi3c) {}

/**
 * @brief Controller Reception Complete callback.
 * @param hi3c - Pointer to an I3C_HandleTypeDef structure that contains the
 * configuration information for the specified I3C.
 */
void HAL_I3C_CtrlRxCpltCallback(I3C_HandleTypeDef *hi3c) {}

/*
 * @brief Error callback.
 * @param hi3c - Pointer to an I3C_HandleTypeDef structure that contains the
 * configuration information for the specified I3C.
 */
void HAL_I3C_ErrorCallback(I3C_HandleTypeDef *hi3c) {}

/*
 * @brief When the multiple transfer of CCC, I3C private or I2C transfer is
 * completed callback.
 * @param hi3c - Pointer to an I3C_HandleTypeDef structure that contains the
 * configuration information for the specified I3C.
 */
void HAL_I3C_CtrlMultipleXferCpltCallback(I3C_HandleTypeDef *hi3c) {}

/*
 * @brief Callback for asynchronous events, like IBI and Hot-join.
 * @param hi3c - Pointer to an I3C_HandleTypeDef structure that contains the
 * configuration information for the specified I3C.
 * @param eventId - Event identifier.
 */
void HAL_I3C_NotifyCallback(I3C_HandleTypeDef *hi3c, uint32_t eventId)
{
	struct no_os_i3c_bus_desc *it;

	stm32_i3c_find_i3c_table(hi3c, &it);
	if (!it)
		return;

	if ((eventId & EVENT_ID_IBI) == EVENT_ID_IBI)
		it->irq_events |= NO_OS_I3C_IRQ_IBI;
	if ((eventId & EVENT_ID_HJ) == EVENT_ID_HJ)
		it->irq_events |= NO_OS_I3C_IRQ_HJ;
	if ((eventId & EVENT_ID_CR) == EVENT_ID_CR)
		it->irq_events |= NO_OS_I3C_IRQ_CR;

	if (it->async_irq)
		no_os_i3c_call_irq(it);

	return;
}

/**
 * @brief Initialize the I3C device.
 * @param desc - The I3C device descriptor.
 * @param param - The structure that contains the I3C parameters (unused).
 * @return 0 in case of success, error code otherwise.
 */
int stm32_i3c_init(struct no_os_i3c_desc *desc,
		   const struct no_os_i3c_init_param *param)
{
	struct stm32_i3c_device_desc *xdesc;

	if (!desc)
		return -EINVAL;

	xdesc = (struct stm32_i3c_device_desc *)no_os_calloc(1,
			sizeof(struct stm32_i3c_device_desc));
	if (!xdesc)
		return -ENOMEM;

	if (stm32_i3c_config_extra(desc, xdesc)) {
		no_os_free(xdesc);
		return -EIO;
	}

	desc->extra = xdesc;

	return 0;
}

/**
 * @brief Initialize the I3C bus.
 * @param desc - The I3C bus descriptor.
 * @param param - The structure that contains the I3C parameters.
 * @return 0 in case of success, error code otherwise.
 */
int stm32_i3c_init_bus(struct no_os_i3c_bus_desc *desc,
		       const struct no_os_i3c_bus_init_param *param)
{
	struct stm32_i3c_init_param *bus_init;
	struct stm32_i3c_bus_desc *xdesc;

	if (!desc || !param)
		return -EINVAL;

	switch (param->device_id) {
#if defined(I3C1)
	case 1:
		break;
#endif
#if defined(I3C2)
	case 2:
		break;
#endif
#if defined(I3C3)
	case 3:
		break;
#endif
	default:
		return -EINVAL;
	};

	xdesc = (struct stm32_i3c_bus_desc *)no_os_calloc(1,
			sizeof(struct stm32_i3c_bus_desc));
	if (!xdesc)
		return -ENOMEM;

	bus_init = param->extra;
	xdesc->hi3c = bus_init->hi3c;
	desc->extra = xdesc;

	return 0;
}

/**
 * @brief Do I3C DAA to assign the dynamic addresses.
 * @param desc - The I3C bus descriptor.
 * @param rstdaa - Unassigned the DA of devices already assigned.
 * @return 0 in case of success, error code otherwise.
 */
int stm32_i3c_do_daa(struct no_os_i3c_bus_desc *desc, bool rstdaa)
{
	struct stm32_i3c_bus_desc *xdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	xdesc = desc->extra;
	if (HAL_I3C_Ctrl_DynAddrAssign_IT(xdesc->hi3c,
					  rstdaa ? I3C_RSTDAA_THEN_ENTDAA : I3C_ONLY_ENTDAA) != HAL_OK)
		return -EIO;

	/*
	 * Wait DAA to finish.
	 */
	while (HAL_I3C_GetState(xdesc->hi3c) != HAL_I3C_STATE_READY) {}

	return 0;
}

/**
 * @brief Free the resources allocated by stm32_i3c_init().
 * @param desc - The I3C  device descriptor.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int stm32_i3c_remove(struct no_os_i3c_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (desc->extra) {
		no_os_free(desc->extra);
		desc->extra = NULL;
	}

	return 0;
}

/**
 * @brief Deinit I3C bus and free resources allocated by stm32_i3c_init_bus().
 * @param desc - The I3C  bus descriptor.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int stm32_i3c_remove_bus(struct no_os_i3c_bus_desc *desc)
{
	struct stm32_i3c_bus_desc *xdesc;

	if (!desc)
		return -EINVAL;

	if (desc->extra) {
		xdesc = desc->extra;
		HAL_I3C_DeInit(xdesc->hi3c);
		no_os_free(desc->extra);
		desc->extra = NULL;
	}

	return 0;
}

/**
 * @brief Send CCC as bus.
 * Retrieve data is only valid for unicast commands.
 * Multi frames are not supported such as:
 * * Read CCCs with a defining bytes.
 * @param desc - The I3C descriptor.
 * @param addr - Address to send to.
 * @param id - CCC ID.
 * @param rnw - True if the CCC should retrieve data from the device.
 * @param defining - Contain defining byte.
 * @param data - The buffer with the data to transmit.
 * @param size - Number of bytes in the buffer.
 * @return 0 in case of success, error code otherwise.
 */
int stm32_i3c_send_ccc(struct no_os_i3c_bus_desc *desc,
		       uint8_t addr,
		       uint8_t id,
		       bool rnw,
		       bool defining,
		       uint8_t *data,
		       uint8_t size)
{
	int ret;
	struct stm32_i3c_bus_desc *xdesc;
	bool bcast;
	uint32_t option;
	/* Buffers are filled by the HAL. */
	/* CtrlBuf size requirements:
	 * Direct CCC: CtrlBuf.Size >= 2*#frames
	 * Broadcast CCC: CtrlBuf.Size >= #frames
	 */
	uint32_t ctrl_buffer[2];
	uint8_t tx_buffer[0xF];
	uint8_t rx_buffer[0xF];

	if (size > sizeof(rx_buffer) / sizeof(uint8_t))
		return -ENOMEM;
	/* Dual frames not implemented */
	if (rnw & defining)
		return -EOPNOTSUPP;

	bcast = addr == NO_OS_I3C_BCAST_ADDR;

	I3C_CCCTypeDef ccc_descriptor = {
		/* TargetAddr, CCC Value */
		(bcast ? 0 : addr), id,
		/* {CCC data + defbyte pointer, CCC size + defining} */
		{rnw ? NULL : data, size + defining},
		/* Direction */
		(rnw ? HAL_I3C_DIRECTION_READ : HAL_I3C_DIRECTION_WRITE)
	};

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	xdesc = desc->extra;
	xdesc->xfer.CtrlBuf.pBuffer = ctrl_buffer;
	xdesc->xfer.CtrlBuf.Size = sizeof(ctrl_buffer) / sizeof(uint32_t);
	/*
	 * Rx/TxBuf.Size is the sum of all I3C_CCCTypeDef {CCC size + byte}.
	 */
	xdesc->xfer.RxBuf.pBuffer = rx_buffer;
	xdesc->xfer.TxBuf.pBuffer = tx_buffer;
	if (rnw) {
		xdesc->xfer.TxBuf.Size = 0;
		xdesc->xfer.RxBuf.Size = size;
	} else {
		xdesc->xfer.TxBuf.Size = size + defining;
		xdesc->xfer.RxBuf.Size = 0;
	}

	if (defining) {
		option = (bcast ? I3C_BROADCAST_WITH_DEFBYTE_STOP :
			  I3C_DIRECT_WITH_DEFBYTE_STOP);
	} else {
		option = (bcast ? I3C_BROADCAST_WITHOUT_DEFBYTE_STOP :
			  I3C_DIRECT_WITHOUT_DEFBYTE_STOP);
	}
	if (HAL_I3C_AddDescToFrame(xdesc->hi3c,
				   &ccc_descriptor,
				   NULL,
				   &xdesc->xfer,
				   1,
				   option
				  ) != HAL_OK)
		return -EIO;

	if (rnw) {
		while (HAL_I3C_Ctrl_ReceiveCCC_IT(xdesc->hi3c, &xdesc->xfer) != HAL_OK) {}
	} else {
		while (HAL_I3C_Ctrl_TransmitCCC_IT(xdesc->hi3c, &xdesc->xfer) != HAL_OK) {}
	}

	ret = stm32_i3c_wait_ready(xdesc->hi3c);
	if (ret)
		return ret;

	/* If receive, copy read value */
	if (rnw) memcpy(data, rx_buffer, size);

	return 0;
}

/**
 * @brief I3C private write transaction as bus.
 * @param desc - The I3C descriptor.
 * @param data - The buffer with the data to transmit.
 * @param data_len - Number of bytes in the buffer.
 * @return 0 in case of success, -EINVAL or -EIO otherwise.
 */
int stm32_i3c_write(struct no_os_i3c_desc *desc,
		    uint8_t *data,
		    uint8_t data_len)
{
	struct stm32_i3c_bus_desc *xdesc;
	I3C_ControlTypeDef ctrl_buffer;
	I3C_PrivateTypeDef private_descriptor = {
		desc->addr, {data, data_len}, {NULL, 0}, HAL_I3C_DIRECTION_WRITE
	};

	if (!desc || !desc->bus || !desc->bus->extra || !data)
		return -EINVAL;

	xdesc = desc->bus->extra;
	xdesc->xfer.CtrlBuf.pBuffer = (uint32_t *)&ctrl_buffer;
	xdesc->xfer.CtrlBuf.Size = 1;
	xdesc->xfer.TxBuf.pBuffer = data;
	xdesc->xfer.TxBuf.Size = data_len;

	if (HAL_I3C_AddDescToFrame(xdesc->hi3c,
				   NULL,
				   &private_descriptor,
				   &xdesc->xfer,
				   xdesc->xfer.CtrlBuf.Size,
				   desc->is_i3c ?
				   I3C_PRIVATE_WITHOUT_ARB_RESTART : I2C_PRIVATE_WITHOUT_ARB_RESTART
				  ) != HAL_OK)
		return -EIO;

	if (HAL_I3C_Ctrl_Transmit_IT(xdesc->hi3c, &xdesc->xfer) != HAL_OK)
		return -EIO;

	return stm32_i3c_wait_ready(xdesc->hi3c);
}

/**
 * @brief I3C private read transaction as bus.
 * @param desc - The I3C descriptor.
 * @param data - The buffer where received data is to be stored.
 * @param data_len - Number of bytes to receive.
 * @return 0 in case of success, -EINVAL or -EIO otherwise.
 */
int stm32_i3c_read(struct no_os_i3c_desc *desc,
		   uint8_t *data,
		   uint8_t data_len)
{
	struct stm32_i3c_bus_desc *xdesc;
	I3C_ControlTypeDef ctrl_buffer;
	I3C_PrivateTypeDef private_descriptor = {desc->addr, {NULL, 0}, {data, data_len}, HAL_I3C_DIRECTION_READ};

	if (!desc || !desc->bus || !desc->bus->extra || !data)
		return -EINVAL;

	xdesc = desc->bus->extra;
	xdesc->xfer.CtrlBuf.pBuffer = (uint32_t *)&ctrl_buffer;
	xdesc->xfer.CtrlBuf.Size = 1;
	xdesc->xfer.RxBuf.pBuffer = data;
	xdesc->xfer.RxBuf.Size = data_len;

	if (HAL_I3C_AddDescToFrame(xdesc->hi3c,
				   NULL,
				   &private_descriptor,
				   &xdesc->xfer,
				   xdesc->xfer.CtrlBuf.Size,
				   desc->is_i3c ?
				   I3C_PRIVATE_WITHOUT_ARB_RESTART : I2C_PRIVATE_WITHOUT_ARB_RESTART
				  ) != HAL_OK)
		return -EIO;

	if (HAL_I3C_Ctrl_Receive_IT(xdesc->hi3c, &xdesc->xfer) != HAL_OK)
		return -EIO;

	return stm32_i3c_wait_ready(xdesc->hi3c);
}

/**
 * @brief I3C private write and read transaction as bus.
 * @param desc - The I3C descriptor.
 * @param tx_data - The buffer with the transmitted data.
 * @param tx_data_len - Number of bytes to write.
 * @param rx_data - The buffer with the received data.
 * @param rx_data_len - Number of bytes to read.
 * @return 0 in case of success, -EINVAL or -EIO otherwise.
 */
int stm32_i3c_write_and_read(struct no_os_i3c_desc *desc,
			     uint8_t *tx_data,
			     uint8_t tx_data_len,
			     uint8_t *rx_data,
			     uint8_t rx_data_len)
{
	struct stm32_i3c_bus_desc *xdesc;
	I3C_ControlTypeDef ctrl_buffer;
	I3C_PrivateTypeDef private_descriptor = {desc->addr, {tx_data, tx_data_len}, {rx_data, rx_data_len}, HAL_I3C_DIRECTION_BOTH};

	if (!desc || !desc->bus || !desc->bus->extra || !tx_data || !rx_data)
		return -EINVAL;

	xdesc = desc->bus->extra;
	xdesc->xfer.CtrlBuf.pBuffer = (uint32_t *)&ctrl_buffer;
	xdesc->xfer.CtrlBuf.Size = 1;
	xdesc->xfer.TxBuf.pBuffer = tx_data;
	xdesc->xfer.TxBuf.Size = tx_data_len;
	xdesc->xfer.RxBuf.pBuffer = rx_data;
	xdesc->xfer.RxBuf.Size = rx_data_len;

	if (HAL_I3C_AddDescToFrame(xdesc->hi3c,
				   NULL,
				   &private_descriptor,
				   &xdesc->xfer,
				   xdesc->xfer.CtrlBuf.Size,
				   desc->is_i3c ?
				   I3C_PRIVATE_WITHOUT_ARB_STOP : I2C_PRIVATE_WITHOUT_ARB_STOP
				  ) != HAL_OK)
		return -EIO;

	if (HAL_I3C_Ctrl_Receive_IT(xdesc->hi3c, &xdesc->xfer) != HAL_OK)
		return -EIO;

	return stm32_i3c_wait_ready(xdesc->hi3c);
}

/**
 * @brief Check if I3C device is ready.
 * @param desc - The I3C descriptor.
 * @return 0 in case of success, -EIO in case of error.
 */
int stm32_i3c_is_dev_ready(struct no_os_i3c_desc* desc)
{
	struct stm32_i3c_bus_desc *xdesc;

	if (!desc || !desc->bus || !desc->bus->extra)
		return -EINVAL;

	xdesc = desc->bus->extra;
	if (HAL_I3C_Ctrl_IsDeviceI3C_Ready(xdesc->hi3c, desc->addr, 300,
					   1000) != HAL_OK)
		return -EIO;
	return 0;
}

/**
 * @brief Configure I3C interrupts.
 * @param desc - The I3C descriptor.
 * @param irq - The interrupt type to enable/disable.
 * @param en - 1 to enable, 0 to disable irq type.
 * @return 0 in case of success, error code otherwise.
 */
int stm32_i3c_conf_irq(struct no_os_i3c_bus_desc* desc,
		       uint8_t irq, bool en)
{
	struct stm32_i3c_bus_desc *xdesc;
	uint32_t value;

	if (!desc || !desc->extra)
		return -EINVAL;

	switch (irq) {
	case NO_OS_I3C_IRQ_IBI:
		value = HAL_I3C_IT_IBIIE;
		break;
	case NO_OS_I3C_IRQ_HJ:
		value = HAL_I3C_IT_HJIE;
		break;
	case NO_OS_I3C_IRQ_CR:
		value = HAL_I3C_IT_CRIE;
		break;
	default:
		return -EINVAL;
	}

	xdesc = desc->extra;
	if (en) {
		if (HAL_I3C_ActivateNotification(xdesc->hi3c, NULL, value) != HAL_OK)
			return -EIO;
	} else {
		if (HAL_I3C_DeactivateNotification(xdesc->hi3c, value) != HAL_OK)
			return -EIO;
	}

	return 0;
}

/**
 * @brief Update I3C CCC info.
 * @param desc - The I3C descriptor.
 * @param irq - The interrupt type to update the field.
 * @return 0 in case of success, error code otherwise.
 */
int stm32_i3c_get_ccc_info(struct no_os_i3c_bus_desc* desc,
			   uint8_t irq)
{
	struct stm32_i3c_bus_desc *xdesc;
	uint32_t event;

	switch (irq) {
	case NO_OS_I3C_IRQ_IBI:
		event = EVENT_ID_IBI;
		break;
	case NO_OS_I3C_IRQ_HJ:
		event = EVENT_ID_CR;
		break;
	case NO_OS_I3C_IRQ_CR:
		event = EVENT_ID_HJ;
		break;
	default:
		return -EINVAL;
	}

	xdesc = desc->extra;
	if (HAL_I3C_GetCCCInfo(xdesc->hi3c, event, &xdesc->ccc_info) != HAL_OK)
		return -EIO;

	desc->ccc_info.ibi_cr_addr = xdesc->ccc_info.IBICRTgtAddr;
	desc->ccc_info.ibi_payload_len = xdesc->ccc_info.IBITgtNbPayload;
	desc->ccc_info.ibi_payload = xdesc->ccc_info.IBITgtPayload;

	desc->irq_events &= ~irq;

	return 0;
}

/**
 * @brief Configure the bus according the new device.
 * @param desc - The I3C device descriptor.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t stm32_i3c_config_extra(struct no_os_i3c_desc *desc,
				      struct stm32_i3c_device_desc *xdesc)
{
	int ret = 0;
	I3C_DeviceConfTypeDef *dev_conf;
	struct stm32_i3c_bus_desc *mxdesc;
	struct no_os_i3c_desc **it;
	uint8_t dev_index = 0;
	uint8_t bcr;

	if (!desc->is_i3c)
		return 0;

	/* Get BCR info */
	ret = no_os_i3c_send_direct_ccc(desc, NO_OS_I3C_CCC_GETBCR, &bcr);
	if (ret)
		return ret;

	/* DeviceIndex is array position + 1, since 0 is the controller */
	it = desc->bus->devs;
	for (; it < desc->bus->devs + NO_OS_I3C_MAX_DEV_NUMBER; it++, dev_index++) {
		if (!*it)
			continue;
		if (*it == desc)
			break;
	};
	dev_conf = &(xdesc->dev_conf);
	dev_conf->DeviceIndex        = dev_index + 1;
	dev_conf->TargetDynamicAddr  = desc->addr;
	dev_conf->IBIAck             = NO_OS_I3C_BCR_IBI_REQUEST_CAPABLE(bcr);
	dev_conf->IBIPayload         = NO_OS_I3C_BCR_IBI_PAYLOAD(bcr);
	dev_conf->CtrlRoleReqAck     = NO_OS_I3C_BCR_DEVICE_ROLE(bcr);
	dev_conf->CtrlStopTransfer   = DISABLE;

	mxdesc = desc->bus->extra;
	if (HAL_I3C_Ctrl_ConfigBusDevices(mxdesc->hi3c, dev_conf, 1U) != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Search for no_os_i3c_bus_desc by matching I3C_HandleTypeDef.
 * @param hi3c - The STM32 I3C bus descriptor.
 * @param iter - Iterator to store the I3C bus descriptor, NULL if not found.
 */
static inline void stm32_i3c_find_i3c_table(I3C_HandleTypeDef *hi3c,
		struct no_os_i3c_bus_desc **iter)
{
	struct stm32_i3c_bus_desc *sdesc;
	struct no_os_i3c_bus_desc **it;
	/*
	 * Find bus that triggered
	 */
	for (it = i3c_table; it < i3c_table + NO_OS_I3C_MAX_BUS_NUMBER; it++) {
		if (!*it)
			continue;

		sdesc = (*it)->extra;
		if (sdesc->hi3c == hi3c) {
			*iter = *it;
			return;
		}
	}
	*iter = NULL;
};

/**
 * @brief Halt execution until the I3C controller go to ready or error state.
 * @param hi3c - The STM32 I3C bus descriptor.
 * @return 0 on success and -EIO in case of error.
 */
static inline int32_t stm32_i3c_wait_ready(I3C_HandleTypeDef *hi3c)
{
	uint8_t state;

	state = HAL_I3C_GetState(hi3c);
	while (state != HAL_I3C_STATE_READY &&
	       state != HAL_I3C_STATE_LISTEN &&
	       state != HAL_I3C_STATE_ERROR) {
		state = HAL_I3C_GetState(hi3c);
	}

	return state == HAL_I3C_STATE_ERROR ? -EIO : 0;
};

/**
 * @brief stm32 platform specific I3C platform ops structure.
 */
const struct no_os_i3c_platform_ops stm32_i3c_ops = {
	.i3c_ops_init_bus = &stm32_i3c_init_bus,
	.i3c_ops_init = &stm32_i3c_init,
	.i3c_ops_do_daa = &stm32_i3c_do_daa,
	.i3c_ops_send_ccc = &stm32_i3c_send_ccc,
	.i3c_ops_write = &stm32_i3c_write,
	.i3c_ops_read = &stm32_i3c_read,
	.i3c_ops_write_and_read = &stm32_i3c_write_and_read,
	.i3c_ops_remove_bus = &stm32_i3c_remove_bus,
	.i3c_ops_remove = &stm32_i3c_remove,
	.i3c_ops_is_dev_ready = &stm32_i3c_is_dev_ready,
	.i3c_ops_conf_irq = &stm32_i3c_conf_irq,
	.i3c_ops_get_ccc_info = &stm32_i3c_get_ccc_info,
};

#endif