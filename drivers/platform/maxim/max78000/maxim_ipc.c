/***************************************************************************//**
 *   @file   maxim_ipc.c
 *   @brief  Maxim MAX78000 inter-processor communication (IPC) driver.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Host-side implementation of the generic no_os_ipc API over the Maxim SEMA
 * peripheral (doorbells + mailboxes). The register-level protocol is shared
 * with the freestanding coprocessor via the inline helpers in maxim_ipc.h.
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

#include "maxim_ipc.h"
#include "no_os_ipc.h"
#include "no_os_alloc.h"
#include "mxc_sys.h"
#include "sema.h"
#include <errno.h>

/**
 * @brief Initialize the SEMA-based IPC and allocate the descriptor.
 * Enables the SEMA peripheral clock and initializes the hardware. A return of
 * E_NONE_AVAIL (-14) from MXC_SEMA_Init() means no mailbox was reserved, which
 * is fine -- the doorbell/mailbox registers are still usable.
 * @param desc  Output: allocated IPC descriptor.
 * @param param Input: initialization parameters.
 * @return 0 on success, -EINVAL if param is NULL, -ENOMEM on alloc failure.
 */
static int max_ipc_init(struct no_os_ipc_desc **desc,
			const struct no_os_ipc_init_param *param)
{
	struct no_os_ipc_desc *descriptor;
	int ret;

	if (!param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->id = param->id;
	descriptor->platform_ops = param->platform_ops;
	descriptor->extra = param->extra;

	/* Enable SEMA peripheral clock and initialize the hardware */
	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_SMPHR);
	ret = MXC_SEMA_Init();
	if (ret && ret != E_NONE_AVAIL) {
		no_os_free(descriptor);
		return ret;
	}

	*desc = descriptor;

	return 0;
}

/**
 * @brief Ring the doorbell on a channel (notify the peer).
 * @param desc IPC descriptor (unused; state is in the SEMA peripheral).
 * @param chan Channel to notify.
 * @return 0 on success, -EINVAL on unknown channel.
 */
static int max_ipc_notify(struct no_os_ipc_desc *desc,
			  enum no_os_ipc_channel chan)
{
	(void)desc;

	switch (chan) {
	case NO_OS_IPC_CHAN_HOST:
		maxim_ipc_raw_ring_host();
		return 0;
	case NO_OS_IPC_CHAN_COPROC:
		maxim_ipc_raw_ring_coproc();
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Test whether a doorbell is pending on a channel.
 * @param desc IPC descriptor (unused).
 * @param chan Channel to test.
 * @return true if pending, false otherwise (including unknown channel).
 */
static bool max_ipc_pending(struct no_os_ipc_desc *desc,
			    enum no_os_ipc_channel chan)
{
	(void)desc;

	switch (chan) {
	case NO_OS_IPC_CHAN_HOST:
		return maxim_ipc_raw_pending_host();
	case NO_OS_IPC_CHAN_COPROC:
		return maxim_ipc_raw_pending_coproc();
	default:
		return false;
	}
}

/**
 * @brief Acknowledge (clear) a pending doorbell on a channel.
 * @param desc IPC descriptor (unused).
 * @param chan Channel to acknowledge.
 * @return 0 on success, -EINVAL on unknown channel.
 */
static int max_ipc_ack(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan)
{
	(void)desc;

	switch (chan) {
	case NO_OS_IPC_CHAN_HOST:
		maxim_ipc_raw_ack_host();
		return 0;
	case NO_OS_IPC_CHAN_COPROC:
		maxim_ipc_raw_ack_coproc();
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Write a scalar word to a channel's mailbox.
 * @param desc  IPC descriptor (unused).
 * @param chan  Channel whose mailbox to write.
 * @param value Word to send.
 * @return 0 on success, -EINVAL on unknown channel.
 */
static int max_ipc_mbox_send(struct no_os_ipc_desc *desc,
			     enum no_os_ipc_channel chan, uint32_t value)
{
	(void)desc;

	switch (chan) {
	case NO_OS_IPC_CHAN_HOST:
		maxim_ipc_raw_mbox_to_host(value);
		return 0;
	case NO_OS_IPC_CHAN_COPROC:
		maxim_ipc_raw_mbox_to_coproc(value);
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read a scalar word from a channel's mailbox.
 * @param desc  IPC descriptor (unused).
 * @param chan  Channel whose mailbox to read.
 * @param value Output: word read.
 * @return 0 on success, -EINVAL on unknown channel.
 */
static int max_ipc_mbox_recv(struct no_os_ipc_desc *desc,
			     enum no_os_ipc_channel chan, uint32_t *value)
{
	(void)desc;

	switch (chan) {
	case NO_OS_IPC_CHAN_HOST:
		*value = maxim_ipc_raw_mbox_from_coproc();
		return 0;
	case NO_OS_IPC_CHAN_COPROC:
		*value = maxim_ipc_raw_mbox_from_host();
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Free the IPC descriptor.
 * @param desc IPC descriptor.
 * @return 0 on success, -EINVAL if desc is NULL.
 */
static int max_ipc_remove(struct no_os_ipc_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc);

	return 0;
}

/** Platform ops table implementing no_os_ipc over the Maxim SEMA peripheral. */
const struct no_os_ipc_platform_ops max_ipc_ops = {
	.init = &max_ipc_init,
	.notify = &max_ipc_notify,
	.pending = &max_ipc_pending,
	.ack = &max_ipc_ack,
	.mbox_send = &max_ipc_mbox_send,
	.mbox_recv = &max_ipc_mbox_recv,
	.remove = &max_ipc_remove,
};
