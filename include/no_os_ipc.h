/***************************************************************************//**
 *   @file   no_os_ipc.h
 *   @brief  Header file of the inter-processor communication (IPC) interface.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Generic inter-core IPC API for multi-core SoCs (e.g. Arm + RISC-V). Abstracts
 * the hardware doorbell (core-to-core interrupt/notification) and mailbox
 * (single-word handoff) that such devices provide -- on Maxim parts this is the
 * SEMA peripheral. Platform-agnostic; implementations must provide ops.
 *
 * A "channel" is the directed notification path toward the peer core. The local
 * core rings the peer's doorbell (no_os_ipc_notify), the peer waits on / polls
 * and acknowledges it (no_os_ipc_wait / no_os_ipc_poll / no_os_ipc_ack), and a
 * scalar payload can ride along in the mailbox (no_os_ipc_mbox_send/recv).
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

#ifndef _NO_OS_IPC_H_
#define _NO_OS_IPC_H_

#include <stdint.h>
#include <stdbool.h>

/* Forward declaration of the ops struct */
struct no_os_ipc_platform_ops;

/**
 * @enum no_os_ipc_channel
 * @brief Directed notification path toward a peer core.
 * @details Identifies which peer's doorbell an operation targets. On a dual
 * core Arm+RISC-V SoC the two channels are the Arm->RISC-V and RISC-V->Arm
 * doorbells. The local core rings the peer channel and acknowledges the channel
 * on which it receives.
 */
enum no_os_ipc_channel {
	/** Notification path toward the primary/host core (e.g. Arm CM4) */
	NO_OS_IPC_CHAN_HOST = 0,
	/** Notification path toward the coprocessor core (e.g. RISC-V RV32) */
	NO_OS_IPC_CHAN_COPROC = 1,
};

/**
 * @struct no_os_ipc_init_param
 * @brief IPC initialization parameters.
 */
struct no_os_ipc_init_param {
	/** IPC instance identifier (0-based; board-specific meaning) */
	uint32_t id;
	/** Pointer to platform-specific ops table (required) */
	const struct no_os_ipc_platform_ops *platform_ops;
	/** Platform-specific extra initialization parameters */
	void *extra;
};

/**
 * @struct no_os_ipc_desc
 * @brief IPC descriptor. Allocated by no_os_ipc_init().
 */
struct no_os_ipc_desc {
	/** IPC instance identifier (copied from init_param) */
	uint32_t id;
	/** Pointer to platform-specific ops table (copied from init_param) */
	const struct no_os_ipc_platform_ops *platform_ops;
	/** Platform-specific runtime state */
	void *extra;
};

/**
 * @struct no_os_ipc_platform_ops
 * @brief Platform-specific IPC operation vtable.
 */
struct no_os_ipc_platform_ops {
	/** Initialize IPC hardware and allocate the descriptor. */
	int (*init)(struct no_os_ipc_desc **desc,
		    const struct no_os_ipc_init_param *param);
	/** Ring the doorbell on @p chan (notify the peer). */
	int (*notify)(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan);
	/** Return true if a doorbell is pending on @p chan (non-blocking). */
	bool (*pending)(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan);
	/** Acknowledge (clear) a pending doorbell on @p chan. */
	int (*ack)(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan);
	/** Write a scalar word to the mailbox associated with @p chan. */
	int (*mbox_send)(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan,
			 uint32_t value);
	/** Read the scalar word from the mailbox associated with @p chan. */
	int (*mbox_recv)(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan,
			 uint32_t *value);
	/** Free IPC resources and the descriptor. */
	int (*remove)(struct no_os_ipc_desc *desc);
};

/**
 * @brief Initialize the inter-processor communication interface.
 * @param desc  Output: allocated IPC descriptor.
 * @param param Input: initialization parameters (must set platform_ops).
 * @return 0 on success, negative error code on failure.
 */
int no_os_ipc_init(struct no_os_ipc_desc **desc,
		   const struct no_os_ipc_init_param *param);

/**
 * @brief Notify the peer on a channel (ring its doorbell).
 * @param desc IPC descriptor.
 * @param chan Channel to notify.
 * @return 0 on success, negative error code on failure.
 */
int no_os_ipc_notify(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan);

/**
 * @brief Test whether a doorbell is pending on a channel (non-blocking).
 * @param desc IPC descriptor.
 * @param chan Channel to test.
 * @return true if a notification is pending, false otherwise (or on error).
 */
bool no_os_ipc_poll(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan);

/**
 * @brief Busy-wait until a doorbell is pending on a channel.
 * @param desc IPC descriptor.
 * @param chan Channel to wait on.
 * @return 0 once pending, negative error code on failure.
 */
int no_os_ipc_wait(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan);

/**
 * @brief Acknowledge (clear) a pending doorbell on a channel.
 * @param desc IPC descriptor.
 * @param chan Channel to acknowledge.
 * @return 0 on success, negative error code on failure.
 */
int no_os_ipc_ack(struct no_os_ipc_desc *desc, enum no_os_ipc_channel chan);

/**
 * @brief Send a scalar word through a channel's mailbox.
 * @param desc  IPC descriptor.
 * @param chan  Channel whose mailbox to write.
 * @param value Word to send.
 * @return 0 on success, negative error code on failure.
 */
int no_os_ipc_mbox_send(struct no_os_ipc_desc *desc,
			enum no_os_ipc_channel chan, uint32_t value);

/**
 * @brief Receive a scalar word from a channel's mailbox.
 * @param desc  IPC descriptor.
 * @param chan  Channel whose mailbox to read.
 * @param value Output: word read.
 * @return 0 on success, negative error code on failure.
 */
int no_os_ipc_mbox_recv(struct no_os_ipc_desc *desc,
			enum no_os_ipc_channel chan, uint32_t *value);

/**
 * @brief Free IPC resources and remove the descriptor.
 * @param desc IPC descriptor.
 * @return 0 on success, negative error code on failure.
 */
int no_os_ipc_remove(struct no_os_ipc_desc *desc);

#endif /* _NO_OS_IPC_H_ */
