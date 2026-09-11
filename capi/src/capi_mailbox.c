/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <capi_mailbox.h>

int capi_mailbox_init(struct capi_mailbox_handle **handle,
		      const struct capi_mailbox_config *config)
{
	if (config && config->ops && config->ops->init)
		return config->ops->init(handle, config);

	return -EINVAL;
}

int capi_mailbox_deinit(struct capi_mailbox_handle *handle)
{
	if (handle && handle->ops && handle->ops->deinit)
		return handle->ops->deinit(handle);

	return -EINVAL;
}

int capi_mailbox_transmit_sync(struct capi_mailbox_handle *handle,
			       struct capi_mailbox_transaction *msg)
{
	if (handle && handle->ops && handle->ops->transmit_sync)
		return handle->ops->transmit_sync(handle, msg);

	return -EINVAL;
}

int capi_mailbox_receive_sync(struct capi_mailbox_handle *handle,
			      struct capi_mailbox_transaction *msg)
{
	if (handle && handle->ops && handle->ops->receive_sync)
		return handle->ops->receive_sync(handle, msg);

	return -EINVAL;
}

int capi_mailbox_register_callback(struct capi_mailbox_handle *handle,
				   capi_mailbox_callback const callback, void *const callback_arg)
{
	if (handle && handle->ops && handle->ops->register_callback)
		return handle->ops->register_callback(handle, callback, callback_arg);

	return -EINVAL;
}

int capi_mailbox_transmit_async(struct capi_mailbox_handle *handle,
				struct capi_mailbox_transaction *msg)
{
	if (handle && handle->ops && handle->ops->transmit_async)
		return handle->ops->transmit_async(handle, msg);

	return -EINVAL;
}

int capi_mailbox_receive_async(struct capi_mailbox_handle *handle,
			       struct capi_mailbox_transaction *msg)
{
	if (handle && handle->ops && handle->ops->receive_async)
		return handle->ops->receive_async(handle, msg);

	return -EINVAL;
}

int capi_mailbox_acknowledge(struct capi_mailbox_handle *handle,
			     capi_mailbox_id_t dest_id)
{
	if (handle && handle->ops && handle->ops->acknowledge)
		return handle->ops->acknowledge(handle, dest_id);

	return -EINVAL;
}

int capi_mailbox_ring_doorbell(struct capi_mailbox_handle *handle,
			       capi_mailbox_id_t dest_id)
{
	if (handle && handle->ops && handle->ops->ring_doorbell)
		return handle->ops->ring_doorbell(handle, dest_id);

	return -EINVAL;
}

int capi_mailbox_flush(struct capi_mailbox_handle *handle, capi_mailbox_id_t id)
{
	if (handle && handle->ops && handle->ops->flush)
		return handle->ops->flush(handle, id);

	return -EINVAL;
}

void capi_mailbox_isr(void *handle)
{
	struct capi_mailbox_handle *ch = (struct capi_mailbox_handle *)handle;

	if (ch && ch->ops && ch->ops->isr)
		ch->ops->isr(handle);
}
