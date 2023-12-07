/***************************************************************************//**
 *   @file   stm32/stm32_i3c.c
 *   @brief  Implementation of stm32 i3c driver.
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
#include <stdlib.h>
#include <errno.h>
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_i3c.h"
#include "stm32_i3c.h"

// TODO callabacks to implement
// HAL_I3C_CtrlTxCpltCallback()
// function when the transmission of private data or Tx CCC transfer is completed.
//
// HAL_I3C_CtrlRxCpltCallback()
// function when the reception of private data or Rx CCC transfer is completed.
//
// HAL_I3C_CtrlMultipleXferCpltCallback()
// function when the multiple transfer of CCC, I3C private or I2C transfer is completed.
//
// HAL_I3C_CtrlDAACpltCallback() [Done]
// function when Dynamic Address Assignment procedure is completed.
//
// HAL_I3C_TgtReqDynamicAddrCallback() [Done]
// function in the controller application when a target sent its payload to the controller
// during Dynamic Address Assignment procedure.
//
// HAL_I3C_NotifyCallback()
// function when the device receives an asynchronous event like IBI, a Hot-join, CCC command for target...
//
// HAL_I3C_AbortCpltCallback()
// function when an abort process is completed.
//
// HAL_I3C_ErrorCallback()
// function when an error is occurred.

/**
  * @brief I3C target request a dynamic address callback.
  * When called, assigns a dynamic address to the target.
  * @param hi3c - Pointer to an I3C_HandleTypeDef structure that contains the
  * configuration information for the specified I3C.
  * @param targetPayload - .
  * @return None
  */
void HAL_I3C_TgtReqDynamicAddrCallback(I3C_HandleTypeDef *hi3c, uint64_t targetPayload)
{
	uint8_t addr;
	struct stm32_i3c_desc *sdesc;
	// Search for peripheral with pid
	struct no_os_i3c_slave_desc **it = i3c_devs_desc;
	for (; it < i3c_devs_stop_addr; it++) {
		if (!*it)
			continue;
		sdesc = (*it)->controller->extra;
		if (&sdesc->hi3c == hi3c && targetPayload >> 16 == (*it)->pid)
			break;
	}
	// If slave is unknown, assign other dynamic address.
	if (it == i3c_devs_stop_addr) {
		addr = no_os_i3c_addr_get_free((*--it)->controller, 8);
		no_os_i3c_addr_set_status((*it)->controller, addr, I3C_ADDR_SLOT_I3C_DEV);
		HAL_I3C_Ctrl_SetDynAddr(hi3c, addr);
		return;
	}

	/* Store BCR and DCR */
	(*it)->bcr_dcr = targetPayload;
	/* Send associated dynamic address */
	addr = (*it)->addr;
	(*it)->is_attached = 1;
	no_os_i3c_addr_set_status((*it)->controller, addr, I3C_ADDR_SLOT_I3C_DEV);
	HAL_I3C_Ctrl_SetDynAddr(hi3c, addr);
}

/**
  * @brief Controller dynamic address assignment Complete callback.
  * @param hi3c - Pointer to an I3C_HandleTypeDef structure that contains the
  * configuration information for the specified I3C.
  * @return None
  */
void HAL_I3C_CtrlDAACpltCallback(I3C_HandleTypeDef *hi3c)
{
	// Do something, like blink a LED
}

/**
 * @brief Initialize the I3C master and slaves.
 * @param desc - The I3C controller descriptor.
 * @param param - The structure that contains the I3C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_i3c_init(struct no_os_i3c_master_desc **desc, const struct no_os_i3c_platform_ops *ops, uint8_t device_id)
{
	int32_t ret;
	struct no_os_i3c_master_desc *master_desc;
	struct no_os_i3c_slave_desc *slave_desc;
	struct stm32_i3c_desc *xdesc;
	I3C_TypeDef *base = NULL;

	if (!desc)
		return -EINVAL;

	master_desc = (struct no_os_i3c_master_desc *)no_os_calloc(1,
		sizeof(struct no_os_i3c_master_desc));
	if (!master_desc)
		return -ENOMEM;

	xdesc = (struct stm32_i3c_desc *)no_os_calloc(1,
			sizeof(struct stm32_i3c_desc));
	if (!xdesc) {
		ret = -ENOMEM;
		goto error_1;
	}

	switch (device_id) {
#if defined(I3C1)
	case 1:
		base = I3C1;
		break;
#endif
#if defined(I3C2)
	case 2:
		base = I3C2;
		break;
#endif
#if defined(I3C3)
	case 3:
		base = I3C3;
		break;
#endif
	default:
		ret = -EINVAL;
		goto error_2;
	};
	xdesc->hi3c.Instance = base;
	// TODO add I3C_InitTypeDef conf xdesc->hi3c.Init.<> = <>;

	ret = HAL_I3C_Init(&xdesc->hi3c);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error_2;
	}
	master_desc->extra = xdesc;
	master_desc->device_id = device_id;

	/*
	 * Allocate the slaves descriptors not yet allocated attached to
	 * this controller.
	 */
	const struct no_os_i3c_init_param **itp = i3c_devs_param;
	struct no_os_i3c_slave_desc **itd = i3c_devs_desc;
	for (; itd < i3c_devs_stop_addr; itd++, itp++) {
		if (*itd || !*itp)
			continue;

		if ((*itp)->device_id != device_id)
			continue;

		slave_desc = (struct no_os_i3c_slave_desc *)no_os_calloc(1,
			sizeof(struct no_os_i3c_slave_desc));
		if (!slave_desc)
			goto error_3;

		// Point to controller
		slave_desc->controller = master_desc;
		*itd = slave_desc;

		// Copy from param to desc
		(*itd)->device_id =	(*itp)->device_id;
		(*itd)->pid =		(*itp)->pid;
		(*itd)->is_static = (*itp)->static_addr != 0;
		if ((*itd)->is_static){
			(*itd)->addr = (*itp)->static_addr;
		} else {
			(*itd)->addr = (*itp)->dynamic_addr;
		}
		(*itd)->is_i3c =	(*itp)->is_i3c;
		(*itd)->platform_ops =	ops;
	}

	*desc = master_desc;
	return 0;
error_3:
	itd = i3c_devs_desc;
	for (; itd < i3c_devs_stop_addr; itd++) {
		if (!*itd)
			continue;

		if ((*itd)->device_id == device_id){
			no_os_free(*itd);
		}
	}
error_2:
	no_os_free(xdesc);
error_1:
	no_os_free(master_desc);
	return ret;
}

/**
 * @brief Do I3C DAA to assign the dynamic addresses.
 * @param desc - The I3C controller descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t stm32_i3c_do_daa(struct no_os_i3c_master_desc *desc)
{
	struct stm32_i3c_desc *xdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	xdesc = desc->extra;

	if (HAL_I3C_Ctrl_DynAddrAssign_IT(&xdesc->hi3c, I3C_ONLY_ENTDAA != HAL_OK))
		return -EIO;

	// Wait DAA to finish.
	while (HAL_I3C_GetState(&xdesc->hi3c) != HAL_I3C_STATE_READY) {}

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_i3c_init().
 * @param desc - The I3C  master descriptor.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int32_t stm32_i3c_remove(struct no_os_i3c_master_desc *desc)
{
	struct stm32_i3c_desc *sdesc;

	if (!desc)
		return -EINVAL;

	struct no_os_i3c_slave_desc **it = i3c_devs_desc;
	for (; it < i3c_devs_stop_addr; it++, it++) {
		if (*it && (*it)->device_id == desc->device_id) {
			no_os_free(*it);
		 }
	}

	if (desc->extra) {
		sdesc = desc->extra;
		HAL_I3C_DeInit(&sdesc->hi3c);
		no_os_free(desc->extra);
	}

	no_os_free(desc);
	return 0;
}

/**
 * @brief I3C send CCC as master.
 * @param desc - The I3C descriptor.
 * @param data - The buffer with the data to transmit.
 * @param bytes_number - Number of bytes in the buffer.
 * @return 0 in case of success, -EINVAL or -EIO otherwise.
 */
int32_t stm32_i3c_send_ccc(struct no_os_i3c_slave_desc *desc,
			uint8_t *data,
			uint8_t bytes_number)
{
	// TODO send CCC
	return -EIO;
}


/**
 * @brief I3C private write transaction as master.
 * @param desc - The I3C descriptor.
 * @param data - The buffer with the data to transmit.
 * @param bytes_number - Number of bytes in the buffer.
 * @return 0 in case of success, -EINVAL or -EIO otherwise.
 */
int32_t stm32_i3c_priv_write(struct no_os_i3c_slave_desc *desc,
			uint8_t *data,
			uint8_t bytes_number)
{
	struct stm32_i3c_desc *xdesc;
	I3C_ControlTypeDef ctrl_buffer;
	I3C_PrivateTypeDef private_descriptor = {desc->addr, {data, bytes_number}, {NULL, 0}, HAL_I3C_DIRECTION_WRITE};

	if (!desc || !desc->controller || !desc->controller->extra || !data)
		return -EINVAL;

	xdesc->xfer.CtrlBuf.pBuffer = (uint32_t *)&ctrl_buffer;
	xdesc->xfer.CtrlBuf.Size = 1;
	xdesc->xfer.TxBuf.pBuffer = data;
	xdesc->xfer.TxBuf.Size = bytes_number;

	if (HAL_I3C_AddDescToFrame(&xdesc->hi3c,
				NULL,
				&private_descriptor,
				&xdesc->xfer,
				xdesc->xfer.CtrlBuf.Size,
				desc->is_i3c ? I3C_PRIVATE_WITH_ARB_RESTART : I2C_PRIVATE_WITH_ARB_RESTART
				) != HAL_OK)
		return -EIO;

	if (HAL_I3C_Ctrl_Transmit_IT(&xdesc->hi3c, &xdesc->xfer) != HAL_OK)
		return -EIO;

	while (HAL_I3C_GetState(&xdesc->hi3c) != HAL_I3C_STATE_READY) {}

	return 0;
}

/**
 * @brief I3C private read transaction as master.
 * @param desc - The I3C descriptor.
 * @param data - The buffer where received data is to be stored.
 * @param bytes_number - Number of bytes to receive.
 * @return 0 in case of success, -EINVAL or -EIO otherwise.
 */

int32_t stm32_i3c_priv_read(struct no_os_i3c_slave_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number)
{
	struct stm32_i3c_desc *xdesc;
	I3C_ControlTypeDef ctrl_buffer;
	I3C_PrivateTypeDef private_descriptor = {desc->addr, {NULL, 0}, {data, bytes_number}, HAL_I3C_DIRECTION_READ};

	if (!desc || !desc->controller || !desc->controller->extra || !data)
		return -EINVAL;

	xdesc = desc->controller->extra;
	xdesc->xfer.CtrlBuf.pBuffer = (uint32_t *)&ctrl_buffer;
	xdesc->xfer.CtrlBuf.Size = 1;
	xdesc->xfer.RxBuf.pBuffer = data;
	xdesc->xfer.RxBuf.Size = bytes_number;

	if (HAL_I3C_AddDescToFrame(&xdesc->hi3c,
				NULL,
				&private_descriptor,
				&xdesc->xfer,
				xdesc->xfer.CtrlBuf.Size,
				desc->is_i3c ? I3C_PRIVATE_WITH_ARB_RESTART : I2C_PRIVATE_WITH_ARB_RESTART
				) != HAL_OK)
		return -EIO;

	if (HAL_I3C_Ctrl_Receive_IT(&xdesc->hi3c, &xdesc->xfer) != HAL_OK)
		return -EIO;

	while (HAL_I3C_GetState(&xdesc->hi3c) != HAL_I3C_STATE_READY) {}

	return 0;
}

/**
 * @brief stm32 platform specific I3C platform ops structure
 */
const struct no_os_i3c_platform_ops stm32_i3c_ops = {
	.i3c_ops_init = &stm32_i3c_init,
	.i3c_ops_do_daa = &stm32_i3c_do_daa,
	.i3c_ops_send_ccc = &stm32_i3c_send_ccc,
	.i3c_ops_priv_write = &stm32_i3c_priv_write,
	.i3c_ops_priv_read = &stm32_i3c_priv_read,
	.i3c_ops_remove = &stm32_i3c_remove
};
