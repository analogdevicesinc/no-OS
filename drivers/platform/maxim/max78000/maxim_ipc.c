/***************************************************************************//**
 *   @file   maxim_ipc.c
 *   @brief  Maxim MAX78000 CAPI mailbox driver.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Host-side implementation of the CAPI mailbox API over the Maxim SEMA
 * peripheral (doorbells + single-word mailboxes). The register-level protocol
 * is shared with the freestanding coprocessor via the inline helpers in
 * maxim_ipc.h.
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
#include "no_os_alloc.h"
#include "mxc_sys.h"
#include "sema.h"
#include <errno.h>

struct max_mailbox_state {
	capi_mailbox_id_t id;
	capi_mailbox_callback callback;
	void *callback_arg;
};

static bool max_mailbox_valid_id(capi_mailbox_id_t id)
{
	return id == MAX_IPC_HOST_ID || id == MAX_IPC_COPROC_ID;
}

static bool max_mailbox_pending(capi_mailbox_id_t id)
{
	switch (id) {
	case MAX_IPC_HOST_ID:
		return maxim_ipc_raw_pending_host();
	case MAX_IPC_COPROC_ID:
		return maxim_ipc_raw_pending_coproc();
	default:
		return false;
	}
}

static int max_mailbox_ack_id(capi_mailbox_id_t id)
{
	switch (id) {
	case MAX_IPC_HOST_ID:
		maxim_ipc_raw_ack_host();
		return 0;
	case MAX_IPC_COPROC_ID:
		maxim_ipc_raw_ack_coproc();
		return 0;
	default:
		return -EINVAL;
	}
}

static int max_mailbox_ring_id(capi_mailbox_id_t id)
{
	switch (id) {
	case MAX_IPC_HOST_ID:
		maxim_ipc_raw_ring_host();
		return 0;
	case MAX_IPC_COPROC_ID:
		maxim_ipc_raw_ring_coproc();
		return 0;
	default:
		return -EINVAL;
	}
}

static int max_mailbox_write_id(capi_mailbox_id_t id, uint32_t value)
{
	switch (id) {
	case MAX_IPC_HOST_ID:
		maxim_ipc_raw_mbox_to_host(value);
		return 0;
	case MAX_IPC_COPROC_ID:
		maxim_ipc_raw_mbox_to_coproc(value);
		return 0;
	default:
		return -EINVAL;
	}
}

static int max_mailbox_read_id(capi_mailbox_id_t id, uint32_t *value)
{
	if (!value)
		return -EINVAL;

	switch (id) {
	case MAX_IPC_HOST_ID:
		*value = maxim_ipc_raw_mbox_from_coproc();
		return 0;
	case MAX_IPC_COPROC_ID:
		*value = maxim_ipc_raw_mbox_from_host();
		return 0;
	default:
		return -EINVAL;
	}
}

static uint32_t max_mailbox_pack_word(const uint8_t *buf, uint32_t len)
{
	uint32_t value = 0;

	for (uint32_t i = 0; i < len; i++)
		value |= (uint32_t)buf[i] << (i * 8u);

	return value;
}

static void max_mailbox_unpack_word(uint8_t *buf, uint32_t len, uint32_t value)
{
	for (uint32_t i = 0; i < len; i++)
		buf[i] = (uint8_t)(value >> (i * 8u));
}

static int max_mailbox_validate_tx(const struct capi_mailbox_transaction *msg)
{
	if (!msg || !msg->buf || !msg->msg_len || msg->msg_len > sizeof(uint32_t))
		return -EINVAL;

	if (!max_mailbox_valid_id(msg->dest_id))
		return -EINVAL;

	return 0;
}

static int max_mailbox_init(struct capi_mailbox_handle **handle,
			    const struct capi_mailbox_config *config)
{
	struct capi_mailbox_handle *mailbox;
	struct max_mailbox_state *state;
	bool allocated = false;
	int ret;

	if (!handle || !config || !config->ops ||
	    !max_mailbox_valid_id(config->identifier))
		return -EINVAL;

	if (!*handle) {
		mailbox = no_os_calloc(1, sizeof(*mailbox));
		if (!mailbox)
			return -ENOMEM;
		allocated = true;
	} else {
		mailbox = *handle;
	}

	state = no_os_calloc(1, sizeof(*state));
	if (!state) {
		if (allocated)
			no_os_free(mailbox);
		return -ENOMEM;
	}

	state->id = config->identifier;
	mailbox->ops = config->ops;
	mailbox->init_allocated = allocated;
	mailbox->priv = state;

	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_SMPHR);
	ret = MXC_SEMA_Init();
	if (ret && ret != E_NONE_AVAIL) {
		no_os_free(state);
		if (allocated)
			no_os_free(mailbox);
		return ret;
	}

	*handle = mailbox;

	return 0;
}

static int max_mailbox_deinit(struct capi_mailbox_handle *handle)
{
	if (!handle)
		return -EINVAL;

	no_os_free(handle->priv);
	if (handle->init_allocated)
		no_os_free(handle);

	return 0;
}

static int max_mailbox_transmit_async(struct capi_mailbox_handle *handle,
				      struct capi_mailbox_transaction *msg)
{
	int ret;
	uint32_t value;

	(void)handle;

	ret = max_mailbox_validate_tx(msg);
	if (ret)
		return ret;

	value = max_mailbox_pack_word(msg->buf, msg->msg_len);
	ret = max_mailbox_write_id(msg->dest_id, value);
	if (ret)
		return ret;

	return max_mailbox_ring_id(msg->dest_id);
}

static int max_mailbox_transmit_sync(struct capi_mailbox_handle *handle,
				     struct capi_mailbox_transaction *msg)
{
	int ret;

	ret = max_mailbox_transmit_async(handle, msg);
	if (ret)
		return ret;

	while (max_mailbox_pending(msg->dest_id))
		;

	return 0;
}

static int max_mailbox_receive_sync(struct capi_mailbox_handle *handle,
				    struct capi_mailbox_transaction *msg)
{
	struct max_mailbox_state *state;
	uint32_t value;
	uint32_t len;
	int ret;

	if (!handle || !handle->priv || !msg || !msg->buf || !msg->size)
		return -EINVAL;

	state = handle->priv;
	while (!max_mailbox_pending(state->id))
		;

	ret = max_mailbox_read_id(state->id, &value);
	if (ret)
		return ret;

	len = msg->size < sizeof(value) ? msg->size : sizeof(value);
	max_mailbox_unpack_word(msg->buf, len, value);
	msg->msg_len = len;
	msg->dest_id = state->id;
	msg->src_id = state->id == MAX_IPC_HOST_ID ? MAX_IPC_COPROC_ID :
		      MAX_IPC_HOST_ID;

	return 0;
}

static int max_mailbox_receive_async(struct capi_mailbox_handle *handle,
				     struct capi_mailbox_transaction *msg)
{
	struct max_mailbox_state *state;

	if (!handle || !handle->priv)
		return -EINVAL;

	state = handle->priv;
	if (!max_mailbox_pending(state->id))
		return -EAGAIN;

	return max_mailbox_receive_sync(handle, msg);
}

static int max_mailbox_register_callback(struct capi_mailbox_handle *handle,
		capi_mailbox_callback const callback, void *const callback_arg)
{
	struct max_mailbox_state *state;

	if (!handle || !handle->priv || !callback)
		return -EINVAL;

	state = handle->priv;
	state->callback = callback;
	state->callback_arg = callback_arg;

	return 0;
}

static int max_mailbox_acknowledge(struct capi_mailbox_handle *handle,
				   capi_mailbox_id_t dest_id)
{
	(void)handle;

	return max_mailbox_ack_id(dest_id);
}

static int max_mailbox_ring_doorbell(struct capi_mailbox_handle *handle,
				     capi_mailbox_id_t dest_id)
{
	(void)handle;

	return max_mailbox_ring_id(dest_id);
}

static int max_mailbox_flush(struct capi_mailbox_handle *handle,
			     capi_mailbox_id_t id)
{
	int ret;

	(void)handle;

	ret = max_mailbox_write_id(id, 0);
	if (ret)
		return ret;

	return max_mailbox_ack_id(id);
}

static void max_mailbox_isr(void *handle)
{
	struct capi_mailbox_handle *mailbox = handle;
	struct max_mailbox_state *state;
	uint32_t value;
	struct capi_mailbox_transaction msg;

	if (!mailbox || !mailbox->priv)
		return;

	state = mailbox->priv;
	if (!state->callback || !max_mailbox_pending(state->id))
		return;

	if (max_mailbox_read_id(state->id, &value))
		return;

	msg.src_id = state->id == MAX_IPC_HOST_ID ? MAX_IPC_COPROC_ID :
		     MAX_IPC_HOST_ID;
	msg.dest_id = state->id;
	msg.buf = (uint8_t *)&value;
	msg.size = sizeof(value);
	msg.msg_len = sizeof(value);

	state->callback(CAPI_MAILBOX_EVENT_MSG_RECIEVED, state->callback_arg, &msg);
}

const struct capi_mailbox_ops max_mailbox_ops = {
	.init = &max_mailbox_init,
	.deinit = &max_mailbox_deinit,
	.transmit_sync = &max_mailbox_transmit_sync,
	.receive_sync = &max_mailbox_receive_sync,
	.register_callback = &max_mailbox_register_callback,
	.transmit_async = &max_mailbox_transmit_async,
	.receive_async = &max_mailbox_receive_async,
	.acknowledge = &max_mailbox_acknowledge,
	.ring_doorbell = &max_mailbox_ring_doorbell,
	.flush = &max_mailbox_flush,
	.isr = &max_mailbox_isr,
};
