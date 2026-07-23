/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx PS GIC IRQ driver (XScuGic)
 *
 * Singleton: one GIC per system, initialized once via capi_irq_init().
 * Mirrors no-OS IRQ_PS path: XScuGic_CfgInitialize + exception handler.
 */

#include <stdbool.h>
#include <errno.h>
#include "xilinx_capi_irq.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_io.h"

#include "xinterrupt_wrap.h"

static bool g_initialized = false;
static XScuGic g_scugic;
static const struct capi_irq_xilinx_subctrl_ops *g_subctrl;
/* GIC SPI the sub-controller cascade drives, valid while g_subctrl != NULL. */
static uint32_t g_cascade_gic_irq;

/*
 * Trigger types as XScuGic_SetPriorityTriggerType() wants them: the raw 2-bit
 * GIC ICFGR field, where bit1 selects edge vs level and bit0 must be set.
 * embeddedsw ships no named constants for these, so we define the two we use.
 *   0b01 = active-high level   0b11 = rising edge
 */
#define CAPI_GIC_TRIGGER_LEVEL_HIGH	0x01U
#define CAPI_GIC_TRIGGER_EDGE_RISING	0x03U
/* Priority for the cascade SPI: mid level; lower value = higher priority. */
#define CAPI_GIC_CASCADE_PRIORITY	0xA0U
/* GIC pending registers pack 32 inputs per 32-bit word: word = id/32, bit = id%32. */
#define CAPI_GIC_PENDING_IRQS_PER_REG	32U
#define CAPI_GIC_PENDING_REG_STRIDE	4U

static int capi_irq_gic_validate(uint32_t irq)
{
	return (irq < XSCUGIC_MAX_NUM_INTR_INPUTS) ? 0 : -EINVAL;
}

static int capi_irq_gic_connect(uint32_t irq, capi_isr_callback_t isr,
				void *arg)
{
	if (capi_irq_gic_validate(irq))
		return -EINVAL;
	if (isr == NULL)
		return -EINVAL;

	int status = XScuGic_Connect(&g_scugic, irq,
				     (Xil_InterruptHandler)isr, arg);
	return (status == XST_SUCCESS) ? 0 : -EIO;
}

static int capi_irq_gic_enable(uint32_t irq)
{
	if (capi_irq_gic_validate(irq))
		return -EINVAL;

	XScuGic_Enable(&g_scugic, irq);
	return 0;
}

static int capi_irq_gic_disable(uint32_t irq)
{
	if (capi_irq_gic_validate(irq))
		return -EINVAL;

	XScuGic_Disable(&g_scugic, irq);
	return 0;
}

static int capi_irq_gic_clear_pending(uint32_t irq)
{
	if (capi_irq_gic_validate(irq))
		return -EINVAL;

	/* GIC pending registers pack 32 interrupt IDs per 32-bit word. */
	uint32_t reg = XSCUGIC_PENDING_CLR_OFFSET +
		       (irq / CAPI_GIC_PENDING_IRQS_PER_REG) *
		       CAPI_GIC_PENDING_REG_STRIDE;
	uint32_t bit = 1U << (irq % CAPI_GIC_PENDING_IRQS_PER_REG);
	Xil_Out32(g_scugic.Config->DistBaseAddress + reg, bit);
	return 0;
}

static int capi_irq_gic_get_status(uint32_t irq, uint32_t *pactive)
{
	if (pactive == NULL)
		return -EINVAL;
	if (capi_irq_gic_validate(irq))
		return -EINVAL;

	/* GIC pending registers pack 32 interrupt IDs per 32-bit word. */
	uint32_t reg = XSCUGIC_PENDING_SET_OFFSET +
		       (irq / CAPI_GIC_PENDING_IRQS_PER_REG) *
		       CAPI_GIC_PENDING_REG_STRIDE;
	uint32_t bit = 1U << (irq % CAPI_GIC_PENDING_IRQS_PER_REG);
	uint32_t val = Xil_In32(g_scugic.Config->DistBaseAddress + reg);
	*pactive = (val & bit) ? 1U : 0U;
	return 0;
}

static int capi_irq_gic_set_priority(uint32_t irq, uint32_t priority)
{
	if (capi_irq_gic_validate(irq))
		return -EINVAL;

	/* XScuGic_SetPriorityTriggerType() asserts above the BSP max. */
	if (priority > XSCUGIC_MAX_INTR_PRIO_VAL)
		return -EINVAL;

	uint8_t cur_prio, cur_trig;
	XScuGic_GetPriorityTriggerType(&g_scugic, irq, &cur_prio, &cur_trig);
	XScuGic_SetPriorityTriggerType(&g_scugic, irq, (uint8_t)priority,
				       cur_trig);
	return 0;
}

static int capi_irq_gic_get_priority(uint32_t irq, uint32_t *priority)
{
	if (priority == NULL)
		return -EINVAL;
	if (capi_irq_gic_validate(irq))
		return -EINVAL;

	uint8_t prio, trig;
	XScuGic_GetPriorityTriggerType(&g_scugic, irq, &prio, &trig);
	*priority = prio;
	return 0;
}

static int capi_irq_gic_set_level_edge_trigger(uint32_t irq,
		enum capi_irq_trig_level trigger)
{
	if (capi_irq_gic_validate(irq))
		return -EINVAL;

	uint8_t prio, trig;
	uint8_t new_trig;

	switch (trigger) {
	case CAPI_IRQ_LEVEL_HIGH:
		new_trig = CAPI_GIC_TRIGGER_LEVEL_HIGH;
		break;
	case CAPI_IRQ_EDGE_RISING:
		new_trig = CAPI_GIC_TRIGGER_EDGE_RISING;
		break;
	default:
		return -ENOTSUP;
	}

	XScuGic_GetPriorityTriggerType(&g_scugic, irq, &prio, &trig);
	XScuGic_SetPriorityTriggerType(&g_scugic, irq, prio, new_trig);
	return 0;
}

/*
 * Error to report for an INTC-targeted call that the GIC root can't service
 * itself (priority / trigger config, which the sub-controller ops don't
 * expose). Distinguish the two failure modes:
 *   sub-controller present -> -ENOTSUP (INTC inputs lack this feature)
 *   no sub-controller       -> -ENODEV  (nothing is wired to the INTC selector)
 */
static int capi_irq_subctrl_missing(void)
{
	return g_subctrl ? -ENOTSUP : -ENODEV;
}

/*
 * Resolve the GIC SPI a sub-controller's cascade drives.
 *
 * Use @p requested when the caller named one (anything but 0 or the AUTO
 * sentinel). Otherwise auto-detect from the descriptor the sub-controller
 * reported: intr_parent must be a GIC, not another INTC (a chained INTC has
 * no single GIC line); XGet_IntrId strips intr_id's type tag and
 * XGet_IntrOffset adds the SPI base (e.g. +32), giving the real GIC id.
 */
static int capi_irq_resolve_cascade(const struct capi_irq_xilinx_cascade *casc,
				    uint32_t requested, uint32_t *gic_irq)
{
	if (requested != CAPI_IRQ_XILINX_CASCADE_AUTO && requested != 0U) {
		*gic_irq = requested;
		return 0;
	}

	if (casc->intr_parent == 0U ||
	    XGet_IntcType(casc->intr_parent) == XINTC_TYPE_IS_INTC)
		return -ENODEV;

	*gic_irq = XGet_IntrId(casc->intr_id) + XGet_IntrOffset(casc->intr_id);
	return 0;
}

/*
 * Wire a sub-controller's already-running cascade onto the GIC: route its
 * dispatch handler to the SPI, clear any stale pending bit, set the trigger
 * and priority, and enable the line. All the GIC-side work the sub-controller
 * deliberately left to us.
 */
static int capi_irq_wire_cascade(const struct capi_irq_xilinx_cascade *casc,
				 uint32_t gic_irq)
{
	int status = XScuGic_Connect(&g_scugic, gic_irq,
				     (Xil_InterruptHandler)casc->handler,
				     casc->handler_arg);
	if (status != XST_SUCCESS)
		return -EIO;

	XScuGic_DistWriteReg(&g_scugic,
			     XSCUGIC_PENDING_CLR_OFFSET +
			     (gic_irq / CAPI_GIC_PENDING_IRQS_PER_REG) *
			     CAPI_GIC_PENDING_REG_STRIDE,
			     1U << (gic_irq % CAPI_GIC_PENDING_IRQS_PER_REG));

	XScuGic_SetPriorityTriggerType(&g_scugic, gic_irq,
				       CAPI_GIC_CASCADE_PRIORITY,
				       CAPI_GIC_TRIGGER_LEVEL_HIGH);
	XScuGic_Enable(&g_scugic, gic_irq);
	return 0;
}

/**
 * @brief Initialize the CAPI backend instance.
 * @note PS: XScuGic_LookupConfig()/XScuGic_CfgInitialize().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_init(struct capi_irq_config *config)
{
	int ret;

	if (config == NULL)
		return -EINVAL;

	if (g_initialized)
		return -EBUSY;

	XScuGic_Config *cfg = XScuGic_LookupConfig((UINTPTR)config->irq_ctrl_id);
	if (cfg == NULL)
		return -ENODEV;

	int status = XScuGic_CfgInitialize(&g_scugic, cfg, cfg->CpuBaseAddress);
	if (status != XST_SUCCESS)
		return -EIO;

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				     (Xil_ExceptionHandler)XScuGic_InterruptHandler, &g_scugic);

	g_subctrl = NULL;
	struct capi_irq_xilinx_extra *extra = config->extra;
	if (extra != NULL && extra->subctrl != NULL) {
		struct capi_irq_xilinx_cascade casc;

		/* Sub-controller brings up its own hardware and tells us... */
		ret = extra->subctrl->init(extra->subctrl_ctrl_id, &casc);
		if (ret)
			return ret;

		/* ...which GIC line to wire, then we do the GIC-side work. */
		ret = capi_irq_resolve_cascade(&casc, extra->cascade_gic_irq,
					       &g_cascade_gic_irq);
		if (ret == 0)
			ret = capi_irq_wire_cascade(&casc, g_cascade_gic_irq);
		if (ret) {
			extra->subctrl->deinit();
			return ret;
		}

		g_subctrl = extra->subctrl;
	}

	g_initialized = true;
	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note PS: XScuGic_Disable()/XScuGic_Disconnect().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_deinit(void)
{
	if (!g_initialized)
		return -EINVAL;

	Xil_ExceptionDisable();

	if (g_subctrl != NULL) {
		/* Stop the sub-controller, then tear down its GIC cascade line. */
		if (g_subctrl->deinit != NULL) {
			int ret = g_subctrl->deinit();
			if (ret)
				return ret;
		}
		XScuGic_Disable(&g_scugic, g_cascade_gic_irq);
		XScuGic_Disconnect(&g_scugic, g_cascade_gic_irq);
		g_cascade_gic_irq = 0U;
	}

	for (uint32_t i = 0; i < XSCUGIC_MAX_NUM_INTR_INPUTS; i++) {
		XScuGic_Disable(&g_scugic, i);
		XScuGic_Disconnect(&g_scugic, i);
	}

	Xil_ExceptionRemoveHandler(XIL_EXCEPTION_ID_INT);
	g_subctrl = NULL;
	g_initialized = false;
	return 0;
}

/**
 * @brief Enable global interrupt delivery.
 * @note PS: Xil_ExceptionEnable().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_global_enable(void)
{
	if (!g_initialized)
		return -EINVAL;

	Xil_ExceptionEnable();
	return 0;
}

/**
 * @brief Disable global interrupt delivery.
 * @note PS: Xil_ExceptionDisable().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_global_disable(void)
{
	if (!g_initialized)
		return -EINVAL;

	Xil_ExceptionDisable();
	return 0;
}

/**
 * @brief Connect an ISR to an interrupt line.
 * @note PS: XScuGic_Connect().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	if (!g_initialized)
		return -EINVAL;
	if (isr == NULL)
		return -EINVAL;

	switch (capi_irq_xilinx_ctrl(irq)) {
	case CAPI_IRQ_XILINX_CTRL_GIC:
		return capi_irq_gic_connect(capi_irq_xilinx_local_id(irq), isr, arg);
	case CAPI_IRQ_XILINX_CTRL_INTC:
		if (g_subctrl == NULL)
			return -ENODEV;
		if (g_subctrl->connect == NULL)
			return -ENOTSUP;
		return g_subctrl->connect(capi_irq_xilinx_local_id(irq), isr, arg);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Enable an interrupt line.
 * @note PS: XScuGic_Enable().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_enable(uint32_t irq)
{
	if (!g_initialized)
		return -EINVAL;

	switch (capi_irq_xilinx_ctrl(irq)) {
	case CAPI_IRQ_XILINX_CTRL_GIC:
		return capi_irq_gic_enable(capi_irq_xilinx_local_id(irq));
	case CAPI_IRQ_XILINX_CTRL_INTC:
		if (g_subctrl == NULL)
			return -ENODEV;
		if (g_subctrl->enable == NULL)
			return -ENOTSUP;
		return g_subctrl->enable(capi_irq_xilinx_local_id(irq));
	default:
		return -EINVAL;
	}
}

/**
 * @brief Disable an interrupt line.
 * @note PS: XScuGic_Disable().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_disable(uint32_t irq)
{
	if (!g_initialized)
		return -EINVAL;

	switch (capi_irq_xilinx_ctrl(irq)) {
	case CAPI_IRQ_XILINX_CTRL_GIC:
		return capi_irq_gic_disable(capi_irq_xilinx_local_id(irq));
	case CAPI_IRQ_XILINX_CTRL_INTC:
		if (g_subctrl == NULL)
			return -ENODEV;
		if (g_subctrl->disable == NULL)
			return -ENOTSUP;
		return g_subctrl->disable(capi_irq_xilinx_local_id(irq));
	default:
		return -EINVAL;
	}
}

/**
 * @brief Clear an interrupt pending bit.
 * @note PS: Xil_Out32() to GIC pending-clear register.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_clear_pending(uint32_t irq)
{
	if (!g_initialized)
		return -EINVAL;

	switch (capi_irq_xilinx_ctrl(irq)) {
	case CAPI_IRQ_XILINX_CTRL_GIC:
		return capi_irq_gic_clear_pending(capi_irq_xilinx_local_id(irq));
	case CAPI_IRQ_XILINX_CTRL_INTC:
		if (g_subctrl == NULL)
			return -ENODEV;
		if (g_subctrl->clear_pending == NULL)
			return -ENOTSUP;
		return g_subctrl->clear_pending(capi_irq_xilinx_local_id(irq));
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read interrupt pending status.
 * @note PS: Xil_In32() from GIC pending-set register.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	if (!g_initialized || pactive == NULL)
		return -EINVAL;

	switch (capi_irq_xilinx_ctrl(irq)) {
	case CAPI_IRQ_XILINX_CTRL_GIC:
		return capi_irq_gic_get_status(capi_irq_xilinx_local_id(irq),
					       pactive);
	case CAPI_IRQ_XILINX_CTRL_INTC:
		if (g_subctrl == NULL)
			return -ENODEV;
		if (g_subctrl->get_status == NULL)
			return -ENOTSUP;
		return g_subctrl->get_status(capi_irq_xilinx_local_id(irq),
					     pactive);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Set interrupt priority.
 * @note PS: XScuGic_Get/SetPriorityTriggerType().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	if (!g_initialized)
		return -EINVAL;

	switch (capi_irq_xilinx_ctrl(irq)) {
	case CAPI_IRQ_XILINX_CTRL_GIC:
		return capi_irq_gic_set_priority(capi_irq_xilinx_local_id(irq),
						 priority);
	case CAPI_IRQ_XILINX_CTRL_INTC:
		return capi_irq_subctrl_missing();
	default:
		return -EINVAL;
	}
}

/**
 * @brief Get interrupt priority.
 * @note PS: XScuGic_GetPriorityTriggerType().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	if (!g_initialized)
		return -EINVAL;
	if (priority == NULL)
		return -EINVAL;

	switch (capi_irq_xilinx_ctrl(irq)) {
	case CAPI_IRQ_XILINX_CTRL_GIC:
		return capi_irq_gic_get_priority(capi_irq_xilinx_local_id(irq),
						 priority);
	case CAPI_IRQ_XILINX_CTRL_INTC:
		return capi_irq_subctrl_missing();
	default:
		return -EINVAL;
	}
}

/**
 * @brief Configure interrupt trigger type.
 * @note PS: XScuGic_SetPriorityTriggerType().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_set_level_edge_trigger(uint32_t irq,
				    enum capi_irq_trig_level trigger)
{
	if (!g_initialized)
		return -EINVAL;

	switch (capi_irq_xilinx_ctrl(irq)) {
	case CAPI_IRQ_XILINX_CTRL_GIC:
		return capi_irq_gic_set_level_edge_trigger(
			       capi_irq_xilinx_local_id(irq), trigger);
	case CAPI_IRQ_XILINX_CTRL_INTC:
		return capi_irq_subctrl_missing();
	default:
		return -EINVAL;
	}
}

/**
 * @brief Return the active XScuGic instance.
 * @note PS: Returns active XScuGic instance.
 *
 * @return XScuGic instance pointer, or NULL if not initialized.
 */
XScuGic *capi_irq_get_scugic_instance(void)
{
	return g_initialized ? &g_scugic : NULL;
}
