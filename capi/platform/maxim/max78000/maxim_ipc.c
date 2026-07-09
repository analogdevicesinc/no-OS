/* SPDX-License-Identifier: BSD-3-Clause */

/**
 *   @file   maxim_ipc.c
 *   @brief  Maxim MAX78000 CAPI mailbox driver.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Host-side implementation of the CAPI mailbox API over the Maxim SEMA
 * peripheral (doorbells + single-word mailboxes). The register-level protocol
 * is shared with the freestanding coprocessor via the inline helpers in
 * maxim_ipc.h.
 */

#include "maxim_ipc.h"
#include "capi_alloc.h"
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

/**
 * @brief Initialize a SEMA mailbox endpoint and enable the peripheral clock.
 * @param handle Receives a new handle if *handle is NULL; otherwise reuses it.
 * @param config Endpoint identifier and mailbox operations.
 * @return 0 on success, or an error code on invalid configuration, allocation
 *         failure, or SEMA initialization failure.
 */
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
		mailbox = capi_calloc(1, sizeof(*mailbox));
		if (!mailbox)
			return -ENOMEM;
		allocated = true;
	} else {
		mailbox = *handle;
	}

	state = capi_calloc(1, sizeof(*state));
	if (!state) {
		if (allocated)
			capi_free(mailbox);
		return -ENOMEM;
	}

	state->id = config->identifier;
	mailbox->ops = config->ops;
	mailbox->init_allocated = allocated;
	mailbox->priv = state;

	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_SMPHR);
	ret = MXC_SEMA_Init();
	if (ret && ret != E_NONE_AVAIL) {
		capi_free(state);
		if (allocated)
			capi_free(mailbox);
		return ret;
	}

	*handle = mailbox;

	return 0;
}

/**
 * @brief Release endpoint state and a handle allocated by initialization.
 * @param handle Initialized mailbox handle.
 * @return 0 on success, or -EINVAL if handle is NULL.
 */
static int max_mailbox_deinit(struct capi_mailbox_handle *handle)
{
	if (!handle)
		return -EINVAL;

	capi_free(handle->priv);
	if (handle->init_allocated)
		capi_free(handle);

	return 0;
}

/**
 * @brief Write a mailbox word and ring the destination doorbell without waiting.
 * @param handle Mailbox handle; the destination is selected by msg->dest_id.
 * @param msg Destination and one to four payload bytes, packed least byte first.
 * @return 0 on success, or -EINVAL for an invalid destination or payload.
 * @note The caller must wait for the previous transfer to be acknowledged before
 *       reusing the destination mailbox register.
 */
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

/**
 * @brief Send a mailbox word and wait for the peer to acknowledge its doorbell.
 * @param handle Mailbox handle passed to the asynchronous transmit operation.
 * @param msg Destination and one to four payload bytes.
 * @return 0 after acknowledgement, or -EINVAL for an invalid transaction.
 * @note This operation has no timeout and requires a responding peer.
 */
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

/**
 * @brief Wait for the endpoint doorbell and copy the received mailbox word.
 * @param handle Initialized receiving endpoint.
 * @param msg Output transaction with a buffer and nonzero buffer size. Receives
 *            up to four bytes, the copied length, and source/destination IDs.
 * @return 0 on success, or -EINVAL for an invalid handle or output buffer.
 * @note No payload length is sent by the hardware. The caller must acknowledge
 *       the doorbell after consuming the word. This wait has no timeout.
 */
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

/**
 * @brief Read the endpoint mailbox if its doorbell is pending.
 * @param handle Initialized receiving endpoint.
 * @param msg Output transaction with a buffer and nonzero buffer size.
 * @return 0 on success, -EAGAIN if no doorbell is pending, or -EINVAL for an
 *         invalid handle or output buffer.
 * @note The caller must acknowledge the doorbell after consuming the word.
 */
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

/**
 * @brief Register the callback invoked by the mailbox ISR for received words.
 * @param handle Initialized receiving endpoint.
 * @param callback Non-NULL callback, called in interrupt context.
 * @param callback_arg User context passed unchanged to the callback.
 * @return 0 on success, or -EINVAL for an invalid handle or NULL callback.
 */
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

/**
 * @brief Clear the pending doorbell for an endpoint.
 * @param handle Mailbox handle; the endpoint is selected by dest_id.
 * @param dest_id Endpoint whose pending notification is to be acknowledged.
 * @return 0 on success, or -EINVAL for an invalid endpoint ID.
 */
static int max_mailbox_acknowledge(struct capi_mailbox_handle *handle,
				   capi_mailbox_id_t dest_id)
{
	(void)handle;

	return max_mailbox_ack_id(dest_id);
}

/**
 * @brief Notify an endpoint without changing its mailbox word.
 * @param handle Mailbox handle; the endpoint is selected by dest_id.
 * @param dest_id Destination endpoint.
 * @return 0 on success, or -EINVAL for an invalid endpoint ID.
 */
static int max_mailbox_ring_doorbell(struct capi_mailbox_handle *handle,
				     capi_mailbox_id_t dest_id)
{
	(void)handle;

	return max_mailbox_ring_id(dest_id);
}

/**
 * @brief Zero an endpoint's mailbox word and clear its pending doorbell.
 * @param handle Mailbox handle; the endpoint is selected by id.
 * @param id Endpoint to flush.
 * @return 0 on success, or -EINVAL for an invalid endpoint ID.
 */
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

/**
 * @brief Deliver a pending mailbox word to the registered receive callback.
 * @param handle Initialized receiving endpoint, passed as the ISR context.
 * @note The transaction and its four-byte payload are valid only during the
 *       callback. The callback must consume or copy the payload and acknowledge
 *       the endpoint doorbell. Invalid handles and endpoints without a callback
 *       or pending notification are ignored.
 */
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
