/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx IRQ driver declarations - shared header for GIC and INTC backends.
 *
 * Single source of truth for the Xilinx IRQ id encoding, the cascade
 * sub-controller interface and the capi_irq_init() extra block. The thin
 * alias capi_irq_xilinx_driver.h just includes this file.
 *
 * Three topologies, two object files (gic.c, intc.c), no weak symbols and no
 * third dispatcher. "Owns the public API" = defines capi_irq_init() & friends;
 * exactly one file may, or you get a duplicate-symbol link error (intentional:
 * one system, one root controller). Which file owns it is picked at compile
 * time from XPAR_XSCUGIC_NUM_INSTANCES (set by the BSP's xparameters.h):
 *
 *   1. GIC-only (Zynq/ZynqMP/Versal):  link gic.c.            extra = NULL
 *   2. INTC-only root (MicroBlaze):    link intc.c.           extra = NULL
 *   3. Cascade (PS GIC + PL AXI INTC): link gic.c + intc.c.   extra = &xtra
 *
 * In case 3 gic.c owns the API as root and intc.c builds as a sub-controller
 * (exports only xilinx_capi_irq_intc_subctrl); point config->extra at a
 * struct capi_irq_xilinx_extra to hand the root that sub-controller.
 *
 * config->irq_ctrl_id (platform_ids.h wraps both as IRQ_GIC_ID / IRQ_INTC_ID):
 *   GIC  - SDT: XPAR_XSCUGIC_0_BASEADDR;  Legacy: XPAR_SCUGIC_SINGLE_DEVICE_ID
 *   INTC - SDT: XPAR_AXI_INTC_0_BASEADDR; Legacy: XPAR_AXI_INTC_0_DEVICE_ID
 */

#ifndef _XILINX_CAPI_IRQ_H_
#define _XILINX_CAPI_IRQ_H_

#include <stdint.h>
#include <capi_irq.h>

#if defined(__has_include)
#if __has_include("xparameters.h")
#include "xparameters.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------
 * IRQ id encoding
 *
 * One uint32_t carries both *which* controller and *which* input on it:
 *
 *   bits [31:16] controller selector (CAPI_IRQ_XILINX_CTRL_*)
 *   bits [15:0]  controller-local interrupt id
 *
 * Example: CAPI_IRQ_XILINX_INTC(5) == 0x0001_0005
 *          ctrl=1 (INTC), local id=5  ->  INTC input 5.
 *
 * Building (CAPI_IRQ_XILINX_GIC/INTC) and decoding (capi_irq_xilinx_ctrl/
 * _local_id) are exact inverses. A bare id like 5 has a zero high half, so it
 * decodes to controller 0 = GIC; on a standalone INTC root that selector is
 * accepted too, which is why plain INTC input numbers work unwrapped there.
 * -------------------------------------------------------------------------- */
#define CAPI_IRQ_XILINX_CTRL_SHIFT	16U		/* selector lives in bits [31:16] */
#define CAPI_IRQ_XILINX_CTRL_MASK	0xFFFF0000U	/* isolates the selector half */
#define CAPI_IRQ_XILINX_ID_MASK		0x0000FFFFU	/* isolates the local-id half */

#define CAPI_IRQ_XILINX_CTRL_GIC	0U		/* selector value for the GIC  */
#define CAPI_IRQ_XILINX_CTRL_INTC	1U		/* selector value for the INTC */

/** Encode a GIC input number into a capi_irq id (selector=GIC, low half=id). */
#define CAPI_IRQ_XILINX_GIC(id) \
	(((uint32_t)CAPI_IRQ_XILINX_CTRL_GIC << CAPI_IRQ_XILINX_CTRL_SHIFT) | \
	 ((uint32_t)(id) & CAPI_IRQ_XILINX_ID_MASK))

/** Encode an AXI INTC input number into a capi_irq id (selector=INTC). */
#define CAPI_IRQ_XILINX_INTC(id) \
	(((uint32_t)CAPI_IRQ_XILINX_CTRL_INTC << CAPI_IRQ_XILINX_CTRL_SHIFT) | \
	 ((uint32_t)(id) & CAPI_IRQ_XILINX_ID_MASK))

/** Decode the controller selector (CAPI_IRQ_XILINX_CTRL_*) from a capi_irq id. */
static inline uint32_t capi_irq_xilinx_ctrl(uint32_t irq)
{
	/* keep the high half, then slide it down to bits [15:0] */
	return (irq & CAPI_IRQ_XILINX_CTRL_MASK) >> CAPI_IRQ_XILINX_CTRL_SHIFT;
}

/** Decode the controller-local input number from a capi_irq id. */
static inline uint32_t capi_irq_xilinx_local_id(uint32_t irq)
{
	/* just the low half; the selector bits are masked away */
	return irq & CAPI_IRQ_XILINX_ID_MASK;
}

/* --------------------------------------------------------------------------
 * Sub-controller (cascade) interface
 *
 * In cascade mode the AXI INTC's aggregated output is a single GIC SPI. The
 * split of labor keeps each backend touching only its own controller:
 *
 *   sub-controller (intc.c): brings up / tears down its own XIntc hardware and
 *     reports, via struct capi_irq_xilinx_cascade, what the root must wire.
 *     It never calls any XScuGic_* function - it is pure XIntc.
 *   root (gic.c): takes that descriptor, decodes the GIC SPI, and does all the
 *     XScuGic_* wiring (connect dispatch handler, set priority, enable).
 *
 * Per-input connect/enable/... are then forwarded to the sub-controller; the
 * ids passed are controller-local (selector already stripped by the root).
 * -------------------------------------------------------------------------- */

/**
 * @brief "Figure out the cascade line yourself" sentinel for cascade_gic_irq.
 *
 * Put this in capi_irq_xilinx_extra.cascade_gic_irq and the root derives the
 * GIC SPI from the descriptor the sub-controller reports (its IntrParent /
 * IntrId), instead of you hard-coding it. The value is all-ones because no
 * real interrupt id ever reaches 0xFFFFFFFF, so it can't collide with one.
 */
#define CAPI_IRQ_XILINX_CASCADE_AUTO	0xFFFFFFFFU

/**
 * @struct capi_irq_xilinx_cascade
 * @brief How the root should wire a sub-controller's cascade line.
 *
 * Filled in by the sub-controller's init() and consumed by the root. Carries
 * only plain integers and a generic handler so the root needs no knowledge of
 * the sub-controller's BSP driver (no XIntc types cross this boundary).
 */
struct capi_irq_xilinx_cascade {
	/** Raw XIntc_Config.IntrParent: parent controller id + low type-tag bit. */
	uint32_t intr_parent;
	/** Raw XIntc_Config.IntrId: encodes the parent's interrupt id + offset. */
	uint32_t intr_id;
	/** Dispatch handler the root connects on the cascade line. */
	capi_isr_callback_t handler;
	/** Argument passed to @ref handler (the sub-controller's base address). */
	void *handler_arg;
};

/**
 * @struct capi_irq_xilinx_subctrl_ops
 * @brief Operations a cascaded sub-controller exposes to the root.
 *
 * All ids passed to connect/enable/disable/clear_pending/get_status are
 * controller-local (already stripped of the controller selector by the root).
 */
struct capi_irq_xilinx_subctrl_ops {
	/**
	 * @brief Bring up the sub-controller's own hardware and report its cascade.
	 * @param ctrl_id Sub-controller id (irq_ctrl_id form).
	 * @param out     Filled with the cascade wiring info for the root.
	 * @return 0 on success, negative errno on failure.
	 * @note Touches only the sub-controller; the root does the GIC wiring.
	 */
	int (*init)(uint32_t ctrl_id, struct capi_irq_xilinx_cascade *out);
	/**
	 * @brief Stop the sub-controller's own hardware.
	 * @return 0 on success, negative errno on failure.
	 * @note Touches only the sub-controller; the root unwires the GIC line.
	 */
	int (*deinit)(void);
	/** @brief Register an ISR on a controller-local input. */
	int (*connect)(uint32_t irq, capi_isr_callback_t isr, void *arg);
	/** @brief Enable a controller-local input. */
	int (*enable)(uint32_t irq);
	/** @brief Disable a controller-local input. */
	int (*disable)(uint32_t irq);
	/** @brief Acknowledge/clear a controller-local input. */
	int (*clear_pending)(uint32_t irq);
	/** @brief Read pending status of a controller-local input. */
	int (*get_status)(uint32_t irq, uint32_t *pactive);
};

/**
 * @struct capi_irq_xilinx_extra
 * @brief config->extra payload selecting a cascaded sub-controller.
 *
 * Set config->extra to point at one of these on a cascade build. Leave
 * config->extra NULL for a GIC-only or INTC-only root build.
 */
struct capi_irq_xilinx_extra {
	/** Sub-controller ops, e.g. &xilinx_capi_irq_intc_subctrl. */
	const struct capi_irq_xilinx_subctrl_ops *subctrl;
	/** Sub-controller controller id (irq_ctrl_id form). */
	uint32_t subctrl_ctrl_id;
	/** Root IRQ the cascade drives, or CAPI_IRQ_XILINX_CASCADE_AUTO. */
	uint32_t cascade_gic_irq;
};

/**
 * @brief Sub-controller ops exported by xilinx_capi_irq_intc.c in cascade mode.
 *
 * Only defined when the INTC file is compiled as a sub-controller
 * (XPAR_XSCUGIC_NUM_INSTANCES > 0). The GIC root references this to dispatch
 * INTC-local IRQs.
 */
extern const struct capi_irq_xilinx_subctrl_ops xilinx_capi_irq_intc_subctrl;

/* --------------------------------------------------------------------------
 * GIC backend
 * -------------------------------------------------------------------------- */
#ifdef XPAR_XSCUGIC_NUM_INSTANCES
#include "xscugic.h"

/**
 * @brief Return the active XScuGic instance.
 * @return XScuGic pointer, or NULL if capi_irq_init() has not run.
 */
XScuGic *capi_irq_get_scugic_instance(void);
#endif

/* --------------------------------------------------------------------------
 * INTC backend
 * -------------------------------------------------------------------------- */
#ifdef XPAR_XINTC_NUM_INSTANCES
#include "xintc.h"

/**
 * @brief Return the active XIntc instance.
 * @return XIntc pointer, or NULL if capi_irq_init() has not run.
 */
XIntc *capi_irq_get_intc_instance(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_IRQ_H_ */
