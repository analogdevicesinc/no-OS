/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx AXI INTC IRQ driver (XIntc).
 *
 * This file supports two link modes:
 *   - linked alone on MicroBlaze: XIntc owns the public capi_irq_* API;
 *   - linked with xilinx_capi_irq_gic.c on Zynq/ZynqMP: XIntc exports only
 *     xilinx_capi_irq_intc_subctrl and the GIC owns the public capi_irq_* API.
 */

#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#if defined(__has_include)
#if __has_include("xparameters.h")
#include "xparameters.h"
#endif
#endif

#include "xilinx_capi_irq.h"
#include "xintc.h"
#include "xintc_l.h"
#include "xil_exception.h"

/*
 * Pick this file's role at compile time. A GIC in the BSP means we are the PL
 * sub-controller cascaded into it (case 3); no GIC means we are the standalone
 * MicroBlaze root (case 2). The two are mutually exclusive (SUBCTRL == !ROOT),
 * so the public capi_irq_* API below is compiled in exactly one build.
 *
 * Either way this file is pure XIntc: it includes no xscugic.h and calls no
 * XScuGic_* function. In cascade mode the root (gic.c) owns all GIC wiring;
 * we only report what to wire (see struct capi_irq_xilinx_cascade).
 *
 * Note: an undefined XPAR_XSCUGIC_NUM_INSTANCES evaluates to 0 in #if, so the
 * MicroBlaze case takes the #else cleanly without needing the macro defined.
 */
#if XPAR_XSCUGIC_NUM_INSTANCES > 0
#define CAPI_IRQ_XILINX_INTC_SUBCTRL	1
#define CAPI_IRQ_XILINX_INTC_ROOT	0
#else
#define CAPI_IRQ_XILINX_INTC_SUBCTRL	0
#define CAPI_IRQ_XILINX_INTC_ROOT	1
#endif

/* Write-1-to-clear every input in the INTC's Acknowledge register at once. */
#define CAPI_INTC_ACK_ALL_PENDING	0xFFFFFFFFU

static bool g_initialized = false;
static XIntc g_intc;

#if CAPI_IRQ_XILINX_INTC_SUBCTRL
static bool g_cascaded = false;
#endif

static int capi_irq_intc_validate(uint32_t irq)
{
	/*
	 * Bound against the input count the BSP reports for this instance
	 * rather than a compile-time macro: XIntc carries it at runtime and
	 * the spelling of the XPAR_*_NUM_INTR_INPUTS macro varies by BSP era.
	 * Only ever called after init, so CfgPtr is valid.
	 */
	return (irq < (uint32_t)g_intc.CfgPtr->NumberofIntrs) ? 0 : -EINVAL;
}

#if CAPI_IRQ_XILINX_INTC_ROOT
/*
 * Map a public capi_irq id to a bare INTC input number for the root path.
 *
 * On a standalone INTC root there is only one controller, so both selectors
 * mean the same thing: GIC accepts bare ids (high half 0), INTC accepts ids
 * built with CAPI_IRQ_XILINX_INTC(). Either way the input is the low half.
 * Range is left to the leaf op, the single point both modes funnel through.
 */
static int capi_irq_intc_decode_root_irq(uint32_t irq, uint32_t *local_irq)
{
	if (local_irq == NULL)
		return -EINVAL;

	switch (capi_irq_xilinx_ctrl(irq)) {
	case CAPI_IRQ_XILINX_CTRL_GIC:
	case CAPI_IRQ_XILINX_CTRL_INTC:
		*local_irq = capi_irq_xilinx_local_id(irq);
		return 0;
	default:
		return -EINVAL;
	}
}
#endif

static int capi_irq_intc_initialize(uint32_t ctrl_id)
{
	int status;

	status = XIntc_Initialize(&g_intc, (UINTPTR)ctrl_id);
	if (status != XST_SUCCESS)
		return -ENODEV;

	status = XIntc_Start(&g_intc, XIN_REAL_MODE);
	return (status == XST_SUCCESS) ? 0 : -EIO;
}

/**
 * @brief Connect an ISR to an AXI INTC input.
 * @note INTC: XIntc_Connect().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_irq_intc_connect(uint32_t irq, capi_isr_callback_t isr,
				 void *arg)
{
	if (capi_irq_intc_validate(irq))
		return -EINVAL;
	if (isr == NULL)
		return -EINVAL;

	int status = XIntc_Connect(&g_intc, (u8)irq, (XInterruptHandler)isr, arg);
	return (status == XST_SUCCESS) ? 0 : -EIO;
}

/**
 * @brief Enable an AXI INTC input.
 * @note INTC: XIntc_Enable().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_irq_intc_enable(uint32_t irq)
{
	if (capi_irq_intc_validate(irq))
		return -EINVAL;

	XIntc_Enable(&g_intc, (u8)irq);
	return 0;
}

/**
 * @brief Disable an AXI INTC input.
 * @note INTC: XIntc_Disable().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_irq_intc_disable(uint32_t irq)
{
	if (capi_irq_intc_validate(irq))
		return -EINVAL;

	XIntc_Disable(&g_intc, (u8)irq);
	return 0;
}

/**
 * @brief Clear a pending AXI INTC input.
 * @note INTC: XIntc_Acknowledge().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_irq_intc_clear_pending(uint32_t irq)
{
	if (capi_irq_intc_validate(irq))
		return -EINVAL;

	XIntc_Acknowledge(&g_intc, (u8)irq);
	return 0;
}

/**
 * @brief Read whether an AXI INTC input is pending.
 * @note INTC: XIntc_GetIntrStatus().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_irq_intc_get_status(uint32_t irq, uint32_t *pactive)
{
	if (pactive == NULL)
		return -EINVAL;
	if (capi_irq_intc_validate(irq))
		return -EINVAL;

	uint32_t isr = XIntc_GetIntrStatus(g_intc.BaseAddress);
	*pactive = (isr & (1U << irq)) ? 1U : 0U;
	return 0;
}

#if CAPI_IRQ_XILINX_INTC_SUBCTRL
/**
 * @brief Initialize AXI INTC as a cascaded sub-controller.
 * @note INTC: XIntc_Initialize()/XIntc_Start().
 *
 * We stop at the INTC boundary: start the controller, ack any stale inputs,
 * and hand back the raw IntrParent/IntrId from the SDT config plus the
 * device interrupt handler and its arg (our base address). The root (gic.c)
 * decodes the GIC SPI from IntrParent/IntrId and does the XScuGic_* wiring.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_irq_intc_subctrl_init(uint32_t ctrl_id,
				      struct capi_irq_xilinx_cascade *out)
{
	int ret;

	if (out == NULL)
		return -EINVAL;
	if (g_initialized)
		return -EBUSY;

	ret = capi_irq_intc_initialize(ctrl_id);
	if (ret)
		return ret;

	/* Acknowledge any stale inputs so the cascade starts quiet. */
	XIntc_Out32(g_intc.CfgPtr->BaseAddress + XIN_IAR_OFFSET,
		    CAPI_INTC_ACK_ALL_PENDING);

	out->intr_parent = g_intc.CfgPtr->IntrParent;
	out->intr_id = g_intc.CfgPtr->IntrId;
	out->handler = (capi_isr_callback_t)XIntc_DeviceInterruptHandler;
	out->handler_arg = (void *)(uintptr_t)g_intc.CfgPtr->BaseAddress;

	g_initialized = true;
	g_cascaded = true;
	return 0;
}

/**
 * @brief Deinitialize the cascaded AXI INTC sub-controller.
 * @note INTC: XIntc_Stop().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_irq_intc_subctrl_deinit(void)
{
	if (!g_initialized || !g_cascaded)
		return -EINVAL;

	XIntc_Stop(&g_intc);

	g_cascaded = false;
	g_initialized = false;
	return 0;
}

const struct capi_irq_xilinx_subctrl_ops xilinx_capi_irq_intc_subctrl = {
	.init = capi_irq_intc_subctrl_init,
	.deinit = capi_irq_intc_subctrl_deinit,
	.connect = capi_irq_intc_connect,
	.enable = capi_irq_intc_enable,
	.disable = capi_irq_intc_disable,
	.clear_pending = capi_irq_intc_clear_pending,
	.get_status = capi_irq_intc_get_status,
};
#endif /* CAPI_IRQ_XILINX_INTC_SUBCTRL */

#if CAPI_IRQ_XILINX_INTC_ROOT

/**
 * @brief Initialize the CAPI INTC backend as a root controller.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_init(struct capi_irq_config *config)
{
	int ret;

	if (!config)
		return -EINVAL;
	if (g_initialized)
		return -EBUSY;

	Xil_ExceptionInit();

	ret = capi_irq_intc_initialize(config->irq_ctrl_id);
	if (ret)
		return ret;

	/* Route the INTC's aggregated output to the MicroBlaze IRQ exception. */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				     (Xil_ExceptionHandler)XIntc_InterruptHandler,
				     &g_intc);

	g_initialized = true;
	return 0;
}

/**
 * @brief Deinitialize the CAPI INTC root backend.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_deinit(void)
{
	if (!g_initialized)
		return -EINVAL;

	Xil_ExceptionDisable();
	XIntc_Stop(&g_intc);
	Xil_ExceptionRemoveHandler(XIL_EXCEPTION_ID_INT);

	g_initialized = false;
	return 0;
}

int capi_irq_global_enable(void)
{
	if (!g_initialized)
		return -EINVAL;

	Xil_ExceptionEnable();
	return 0;
}

int capi_irq_global_disable(void)
{
	if (!g_initialized)
		return -EINVAL;

	Xil_ExceptionDisable();
	return 0;
}

int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	uint32_t local_irq;
	int ret;

	if (!g_initialized)
		return -EINVAL;

	ret = capi_irq_intc_decode_root_irq(irq, &local_irq);
	if (ret)
		return ret;

	return capi_irq_intc_connect(local_irq, isr, arg);
}

int capi_irq_enable(uint32_t irq)
{
	uint32_t local_irq;
	int ret;

	if (!g_initialized)
		return -EINVAL;

	ret = capi_irq_intc_decode_root_irq(irq, &local_irq);
	if (ret)
		return ret;

	return capi_irq_intc_enable(local_irq);
}

int capi_irq_disable(uint32_t irq)
{
	uint32_t local_irq;
	int ret;

	if (!g_initialized)
		return -EINVAL;

	ret = capi_irq_intc_decode_root_irq(irq, &local_irq);
	if (ret)
		return ret;

	return capi_irq_intc_disable(local_irq);
}

int capi_irq_clear_pending(uint32_t irq)
{
	uint32_t local_irq;
	int ret;

	if (!g_initialized)
		return -EINVAL;

	ret = capi_irq_intc_decode_root_irq(irq, &local_irq);
	if (ret)
		return ret;

	return capi_irq_intc_clear_pending(local_irq);
}

int capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	uint32_t local_irq;
	int ret;

	if (!g_initialized || pactive == NULL)
		return -EINVAL;

	ret = capi_irq_intc_decode_root_irq(irq, &local_irq);
	if (ret)
		return ret;

	return capi_irq_intc_get_status(local_irq, pactive);
}

int capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	uint32_t local_irq;
	int ret;

	(void)priority;

	if (!g_initialized)
		return -EINVAL;

	ret = capi_irq_intc_decode_root_irq(irq, &local_irq);
	if (ret)
		return ret;

	return -ENOTSUP;
}

int capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	uint32_t local_irq;
	int ret;

	if (!g_initialized)
		return -EINVAL;
	if (priority == NULL)
		return -EINVAL;

	ret = capi_irq_intc_decode_root_irq(irq, &local_irq);
	if (ret)
		return ret;

	return -ENOTSUP;
}

int capi_irq_set_level_edge_trigger(uint32_t irq,
				    enum capi_irq_trig_level trigger)
{
	uint32_t local_irq;
	int ret;

	(void)trigger;

	if (!g_initialized)
		return -EINVAL;

	ret = capi_irq_intc_decode_root_irq(irq, &local_irq);
	if (ret)
		return ret;

	return -ENOTSUP;
}

#endif /* CAPI_IRQ_XILINX_INTC_ROOT */

/**
 * @brief Return the active XIntc instance.
 *
 * @return XIntc instance pointer, or NULL if not initialized.
 */
XIntc *capi_irq_get_intc_instance(void)
{
	return g_initialized ? &g_intc : NULL;
}
