/* SPDX-License-Identifier: BSD-3-Clause */

/** @file maxim_ipc.h
 *  @brief MAX78000 CAPI mailbox interface for either core.
 *
 * Firmware using only the register transport can include maxim_ipc_raw.h.
 */
#ifndef _MAXIM_IPC_H_
#define _MAXIM_IPC_H_

#include "capi_mailbox.h"
#include "maxim_ipc_raw.h"

/** @brief CAPI mailbox operations over the SEMA peripheral. */
extern const struct capi_mailbox_ops max_mailbox_ops;

#endif /* _MAXIM_IPC_H_ */
