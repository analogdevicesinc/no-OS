/***************************************************************************//**
 *   @file   maxim_ipc.h
 *   @brief  MAX32690 CAPI mailbox interface for either core.
 *   @author Christopher de Guzman (christopher.deguzman@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * Firmware using only the register transport can include maxim_ipc_raw.h.
 */

#ifndef MAXIM_IPC_H_
#define MAXIM_IPC_H_

#include "capi_mailbox.h"
#include "maxim_ipc_raw.h"

/** @brief CAPI mailbox operations over the SEMA peripheral. */
extern const struct capi_mailbox_ops max_mailbox_ops;

#endif /* MAXIM_IPC_H_ */
