/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL Mailbox API
 */

#ifndef _CAPI_MAILBOX_H_
#define _CAPI_MAILBOX_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef uint32_t capi_mailbox_id_t;

enum capi_mailbox_async_event {
	CAPI_MAILBOX_EVENT_MSG_RECIEVED,
	CAPI_MAILBOX_EVENT_MSG_DOORBELL,
	CAPI_MAILBOX_EVENT_MSG_ACKNOWLEDGED,
	CAPI_MAILBOX_EVENT_MSG_ACK_TIMEOUT
};

struct capi_mailbox_config {
	uint32_t identifier;
	const struct capi_mailbox_ops *ops;
};

struct capi_mailbox_handle {
	const struct capi_mailbox_ops *ops;
	bool init_allocated;
	void *lock;
	void *priv;
};

struct capi_mailbox_transaction {
	capi_mailbox_id_t src_id;
	capi_mailbox_id_t dest_id;
	uint8_t *buf;
	uint32_t size;
	uint32_t msg_len;
};

typedef void (*capi_mailbox_callback)(enum capi_mailbox_async_event event,
				      void *arg,
				      struct capi_mailbox_transaction *msg);

int capi_mailbox_init(struct capi_mailbox_handle **handle,
		      const struct capi_mailbox_config *config);
int capi_mailbox_deinit(struct capi_mailbox_handle *handle);
int capi_mailbox_transmit_sync(struct capi_mailbox_handle *handle,
			       struct capi_mailbox_transaction *msg);
int capi_mailbox_receive_sync(struct capi_mailbox_handle *handle,
			      struct capi_mailbox_transaction *msg);
int capi_mailbox_register_callback(struct capi_mailbox_handle *handle,
				   capi_mailbox_callback const callback, void *const callback_arg);
int capi_mailbox_transmit_async(struct capi_mailbox_handle *handle,
				struct capi_mailbox_transaction *msg);
int capi_mailbox_receive_async(struct capi_mailbox_handle *handle,
			       struct capi_mailbox_transaction *msg);
int capi_mailbox_acknowledge(struct capi_mailbox_handle *handle,
			     capi_mailbox_id_t dest_id);
void capi_mailbox_isr(void *handle);
int capi_mailbox_ring_doorbell(struct capi_mailbox_handle *handle,
			       capi_mailbox_id_t dest_id);
int capi_mailbox_flush(struct capi_mailbox_handle *handle,
		       capi_mailbox_id_t id);

struct capi_mailbox_ops {
	int (*init)(struct capi_mailbox_handle **handle,
		    const struct capi_mailbox_config *config);
	int (*deinit)(struct capi_mailbox_handle *handle);
	int (*transmit_sync)(struct capi_mailbox_handle *handle,
			     struct capi_mailbox_transaction *msg);
	int (*receive_sync)(struct capi_mailbox_handle *handle,
			    struct capi_mailbox_transaction *msg);
	int (*register_callback)(struct capi_mailbox_handle *handle,
				 capi_mailbox_callback const callback, void *const callback_arg);
	int (*transmit_async)(struct capi_mailbox_handle *handle,
			      struct capi_mailbox_transaction *msg);
	int (*receive_async)(struct capi_mailbox_handle *handle,
			     struct capi_mailbox_transaction *msg);
	int (*acknowledge)(struct capi_mailbox_handle *handle,
			   capi_mailbox_id_t dest_id);
	int (*ring_doorbell)(struct capi_mailbox_handle *handle,
			     capi_mailbox_id_t dest_id);
	int (*flush)(struct capi_mailbox_handle *handle, capi_mailbox_id_t id);
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_MAILBOX_H_ */
