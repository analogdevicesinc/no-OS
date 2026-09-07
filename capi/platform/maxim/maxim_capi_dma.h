/***************************************************************************//**
 *   @file   maxim_capi_dma.h
 *   @brief  Header file for DMA functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_DMA_H_
#define MAXIM_CAPI_DMA_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*
 * mxc_device.h selects this part's max<n>.h (TARGET_NUM and the MXC_DMA1_S /
 * DMAn_IRQn macros the family probe below keys on) and dma.h carries the MSDK
 * DMA request-select enum (mxc_dma_reqsel_t) that the transfer extra is built
 * from.
 */
#include "mxc_device.h"
#include "dma.h"
#include "capi_dma.h"

/*
 * One CAPI DMA backend for every MSDK part. Three controller families ship, and
 * they differ ONLY in the MSDK call shape and the per-channel IRQ names -- the
 * transfer logic is identical. The split is isolated to signature shims in
 * maxim_capi_dma.c and to the weak DMA vectors in maxim_capi_irq.c; nothing in
 * the algorithms branches on the part.
 *
 *   Single-controller (MAX32655/MAX32690/most): one MXC_DMA. Init/DeInit/
 *     AcquireChannel/EnableInt/Handler take no controller argument;
 *     MXC_DMA_CH_GET_IRQ(ch); per-channel lines are DMAn_IRQn.
 *   Multi-controller, non-secure (MAX32665): MXC_DMA0 + MXC_DMA1. Init/DeInit/
 *     AcquireChannel/Handler take an mxc_dma_regs_t*, but EnableInt and
 *     MXC_DMA_CH_GET_IRQ(ch) do NOT -- the channel index is global across both
 *     controllers. The backend drives MXC_DMA0.
 *   Multi-controller, secure (MAX32657): MXC_DMA0_NS + MXC_DMA1_S. Every one of
 *     Init/DeInit/AcquireChannel/EnableInt/Handler takes the instance, and
 *     MXC_DMA_CH_GET_IRQ(dma, ch) is two-arg. Only DMA1 is reachable from the
 *     TrustZone-secure world, so the backend drives MXC_DMA1_S.
 *
 * "Takes a controller argument" and "the controller is TrustZone-secure" are
 * INDEPENDENT facts -- MAX32665 is the first without the second -- so they are
 * two traits, each probed on a die fact from max<n>.h, never on TARGET_NUM.
 * EnableInt/CH_GET_IRQ get a third trait because the MSDK is not internally
 * consistent about which calls carry the instance.
 */

/*
 * Init/DeInit/AcquireChannel/Handler take an mxc_dma_regs_t* (MAX32657,
 * MAX32665). Probe on MXC_DMA1 -- "a second controller exists" -- not on
 * MXC_DMA0: MAX32657 only spells MXC_DMA0 in its non-secure branch, so a secure
 * build of that part would fall through to the no-argument shape and fail.
 */
#if defined(MXC_DMA1)
#define MAX_CAPI_DMA_INST_ARG		1
#else
#define MAX_CAPI_DMA_INST_ARG		0
#endif

/** The driven controller is the TrustZone-secure DMA1_S (MAX32657 only). */
#if defined(MXC_DMA1_S)
#define MAX_CAPI_DMA_SECURE_DUAL	1
#else
#define MAX_CAPI_DMA_SECURE_DUAL	0
#endif

/**
 * EnableInt() and MXC_DMA_CH_GET_IRQ() also carry the instance. Only the secure
 * family does; MAX32665 keeps both single-argument with a global channel index.
 */
#define MAX_CAPI_DMA_INT_INST_ARG	MAX_CAPI_DMA_SECURE_DUAL

/*
 * The controller this backend drives. All three choices are mxc_dma_regs_t*, so
 * priv->dma holds any of them. Declared here rather than in maxim_capi_dma.c
 * because maxim_capi_irq.c's weak DMA vectors need the same instance to call
 * MXC_DMA_Handler() with.
 */
#if MAX_CAPI_DMA_SECURE_DUAL
#define MAX_CAPI_DMA_REGS		MXC_DMA1_S
#elif MAX_CAPI_DMA_INST_ARG
#define MAX_CAPI_DMA_REGS		MXC_DMA0
#else
#define MAX_CAPI_DMA_REGS		MXC_DMA
#endif


/**
 * @struct max_capi_dma_xfer_extra
 * @brief Maxim platform-specific per-transfer DMA configuration.
 *
 * Carries the MSDK request-select line that ties a channel to a peripheral's
 * DMA handshake. Attach it to capi_dma_transfer::extra; config_xfer reads it
 * and defaults to a memory-to-memory transfer when it is absent.
 */
struct max_capi_dma_xfer_extra {
	/** MSDK DMA request select (peripheral handshake, or mem-to-mem). */
	mxc_dma_reqsel_t reqsel;
};

/** UART receive DMA request select. */
#define MAX_CAPI_DMA_REQUEST_UART_RX	MXC_DMA_REQUEST_UARTRX
/** UART transmit DMA request select. */
#define MAX_CAPI_DMA_REQUEST_UART_TX	MXC_DMA_REQUEST_UARTTX

extern const struct capi_dma_ops max_capi_dma_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_DMA_H_ */
