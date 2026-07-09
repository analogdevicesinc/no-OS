/***************************************************************************//**
 *   @file   no_os_ipc.c
 *   @brief  Implementation of the inter-processor communication (IPC) API.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Generic dispatch layer over platform IPC ops. See include/no_os_ipc.h.
 *
 ******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "no_os_ipc.h"
#include <errno.h>

/**
 * @brief Initialize the inter-processor communication interface.
 * @param desc  Output: allocated IPC descriptor.
 * @param param Input: initialization parameters.
 * @return 0 on success, -EINVAL if param/platform_ops is NULL,
 *         -ENOSYS if init op is not implemented, or platform-specific error.
 */
int no_os_ipc_init(struct no_os_ipc_desc **desc,
		   const struct no_os_ipc_init_param *param)
{
	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->init)
		return -ENOSYS;

	return param->platform_ops->init(desc, param);
}

/**
 * @brief Notify the peer on a channel (ring its doorbell).
 * @param desc IPC descriptor.
 * @param chan Channel to notify.
 * @return 0 on success, -EINVAL/-ENOSYS or platform-specific error.
 */
int no_os_ipc_notify(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->notify)
		return -ENOSYS;

	return desc->platform_ops->notify(desc, chan);
}

/**
 * @brief Test whether a doorbell is pending on a channel (non-blocking).
 * @param desc IPC descriptor.
 * @param chan Channel to test.
 * @return true if pending, false otherwise (including invalid args).
 */
bool no_os_ipc_poll(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan)
{
	if (!desc || !desc->platform_ops || !desc->platform_ops->pending)
		return false;

	return desc->platform_ops->pending(desc, chan);
}

/**
 * @brief Busy-wait until a doorbell is pending on a channel.
 * @param desc IPC descriptor.
 * @param chan Channel to wait on.
 * @return 0 once pending, -EINVAL/-ENOSYS on failure.
 */
int no_os_ipc_wait(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pending)
		return -ENOSYS;

	while (!desc->platform_ops->pending(desc, chan))
		;

	return 0;
}

/**
 * @brief Acknowledge (clear) a pending doorbell on a channel.
 * @param desc IPC descriptor.
 * @param chan Channel to acknowledge.
 * @return 0 on success, -EINVAL/-ENOSYS or platform-specific error.
 */
int no_os_ipc_ack(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->ack)
		return -ENOSYS;

	return desc->platform_ops->ack(desc, chan);
}

/**
 * @brief Send a scalar word through a channel's mailbox.
 * @param desc  IPC descriptor.
 * @param chan  Channel whose mailbox to write.
 * @param value Word to send.
 * @return 0 on success, -EINVAL/-ENOSYS or platform-specific error.
 */
int no_os_ipc_mbox_send(struct no_os_ipc_desc *desc,
			enum no_os_ipc_channel chan, uint32_t value)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->mbox_send)
		return -ENOSYS;

	return desc->platform_ops->mbox_send(desc, chan, value);
}

/**
 * @brief Receive a scalar word from a channel's mailbox.
 * @param desc  IPC descriptor.
 * @param chan  Channel whose mailbox to read.
 * @param value Output: word read.
 * @return 0 on success, -EINVAL/-ENOSYS or platform-specific error.
 */
int no_os_ipc_mbox_recv(struct no_os_ipc_desc *desc,
			enum no_os_ipc_channel chan, uint32_t *value)
{
	if (!desc || !desc->platform_ops || !value)
		return -EINVAL;

	if (!desc->platform_ops->mbox_recv)
		return -ENOSYS;

	return desc->platform_ops->mbox_recv(desc, chan, value);
}

/**
 * @brief Free IPC resources and remove the descriptor.
 * @param desc IPC descriptor.
 * @return 0 on success, -EINVAL/-ENOSYS or platform-specific error.
 */
int no_os_ipc_remove(struct no_os_ipc_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	return desc->platform_ops->remove(desc);
}
