/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx AXI INTC IRQ driver (XIntc), cascaded to the PS GIC.
 *
 * SDT BSP only. The AXI INTC output is routed to the processor through a
 * GIC SPI line, and CAPI IRQ numbers are INTC input numbers.
 * capi_irq_init() must be called before peripheral IRQ users.
 */

#include <stdbool.h>
#include <errno.h>

#include "capi_irq.h"
#include "xilinx_capi_irq.h"
#include "xintc.h"
#include "xintc_l.h"
#include "xinterrupt_wrap.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_io.h"

static bool g_initialized = false;
static uint32_t g_cascade_gic_irq = 0;
static XIntc g_intc;
static XScuGic g_scugic;

/**
 * @brief Initialize the CAPI backend instance.
 * @note INTC: XIntc_Initialize()/XScuGic_Connect().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_init(struct capi_irq_config *config)
{
	int status;

	if (!config)
		return -EINVAL;

	if (g_initialized)
		return -EBUSY;

	Xil_ExceptionInit();

	XIntc_Config *intc_cfg = XIntc_LookupConfig((UINTPTR)config->irq_ctrl_id);
	if (!intc_cfg)
		return -ENODEV;

	/* Reject standalone XIntc topologies on PS targets. */
	if (intc_cfg->IntrParent == 0u
	    || XGet_IntcType(intc_cfg->IntrParent) == XINTC_TYPE_IS_INTC)
		return -ENODEV;

	g_cascade_gic_irq = XGet_IntrId(intc_cfg->IntrId) + XGet_IntrOffset(
				    intc_cfg->IntrId);

	XScuGic_Config *gic_cfg =
		XScuGic_LookupConfig((UINTPTR)(intc_cfg->IntrParent & ~XINTC_TYPE_MASK));
	if (!gic_cfg)
		return -ENODEV;

	status = XScuGic_CfgInitialize(&g_scugic, gic_cfg, gic_cfg->CpuBaseAddress);
	if (status != XST_SUCCESS)
		return -EIO;

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				     (Xil_ExceptionHandler)XScuGic_InterruptHandler, &g_scugic);

	status = XIntc_Initialize(&g_intc, (UINTPTR)config->irq_ctrl_id);
	if (status != XST_SUCCESS)
		return -EIO;

	status = XIntc_Start(&g_intc, XIN_REAL_MODE);
	if (status != XST_SUCCESS)
		return -EIO;

	/* XIntc_DeviceInterruptHandler takes the INTC base address in SDT. */
	void *handler_arg = (void *)(uintptr_t)g_intc.CfgPtr->BaseAddress;
	status = XScuGic_Connect(&g_scugic, g_cascade_gic_irq,
				 (Xil_InterruptHandler)XIntc_DeviceInterruptHandler, handler_arg);
	if (status != XST_SUCCESS)
		return -EIO;

	UINTPTR intc_base = g_intc.CfgPtr->BaseAddress;
	XIntc_Out32(intc_base + XIN_IAR_OFFSET, 0xFFFFFFFFu);
	XScuGic_DistWriteReg(&g_scugic,
			     XSCUGIC_PENDING_CLR_OFFSET + (g_cascade_gic_irq / 32u) * 4u,
			     1u << (g_cascade_gic_irq % 32u));

	XScuGic_SetPriorityTriggerType(&g_scugic, g_cascade_gic_irq, 0xA0u, 0x1u);
	XScuGic_Enable(&g_scugic, g_cascade_gic_irq);

	g_initialized = true;
	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note INTC: XScuGic_Disable()/XScuGic_Disconnect().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_deinit(void)
{
	if (!g_initialized)
		return -EINVAL;

	Xil_ExceptionDisable();

	XIntc_Stop(&g_intc);

	/* Tear down only the cascade line owned by this backend. */
	XScuGic_Disable(&g_scugic, g_cascade_gic_irq);
	XScuGic_Disconnect(&g_scugic, g_cascade_gic_irq);

	Xil_ExceptionRemoveHandler(XIL_EXCEPTION_ID_INT);

	g_cascade_gic_irq = 0u;
	g_initialized = false;
	return 0;
}

/**
 * @brief Enable global interrupt delivery.
 * @note INTC: Xil_ExceptionEnable().
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
 * @note INTC: Xil_ExceptionDisable().
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
 * @note INTC: irq < 32 routes to XIntc; irq >= 32 routes to GIC (PS peripherals).
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	if (!g_initialized || !isr)
		return -EINVAL;
	if (irq < XPAR_INTC_MAX_NUM_INTR_INPUTS) {
		int status = XIntc_Connect(&g_intc, (u8)irq, (XInterruptHandler)isr, arg);
		return (status == XST_SUCCESS) ? 0 : -EIO;
	}
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	int status = XScuGic_Connect(&g_scugic, irq, (Xil_InterruptHandler)isr, arg);
	return (status == XST_SUCCESS) ? 0 : -EIO;
}

/**
 * @brief Enable an interrupt line.
 * @note INTC: irq < 32 routes to XIntc; irq >= 32 routes to GIC.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_enable(uint32_t irq)
{
	if (!g_initialized)
		return -EINVAL;
	if (irq < XPAR_INTC_MAX_NUM_INTR_INPUTS) {
		XIntc_Enable(&g_intc, (u8)irq);
		return 0;
	}
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	XScuGic_Enable(&g_scugic, irq);
	return 0;
}

/**
 * @brief Disable an interrupt line.
 * @note INTC: irq < 32 routes to XIntc; irq >= 32 routes to GIC.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_disable(uint32_t irq)
{
	if (!g_initialized)
		return -EINVAL;
	if (irq < XPAR_INTC_MAX_NUM_INTR_INPUTS) {
		XIntc_Disable(&g_intc, (u8)irq);
		return 0;
	}
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	XScuGic_Disable(&g_scugic, irq);
	return 0;
}

/**
 * @brief Clear an interrupt pending bit.
 * @note INTC: irq < 32 routes to XIntc; irq >= 32 routes to GIC.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_clear_pending(uint32_t irq)
{
	if (!g_initialized)
		return -EINVAL;
	if (irq < XPAR_INTC_MAX_NUM_INTR_INPUTS) {
		XIntc_Acknowledge(&g_intc, (u8)irq);
		return 0;
	}
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	uint32_t reg = XSCUGIC_PENDING_CLR_OFFSET + (irq / 32U) * 4U;
	Xil_Out32(g_scugic.Config->DistBaseAddress + reg, 1U << (irq % 32U));
	return 0;
}

/**
 * @brief Read interrupt pending status.
 * @note INTC: irq < 32 routes to XIntc; irq >= 32 routes to GIC.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	if (!g_initialized || !pactive)
		return -EINVAL;
	if (irq < XPAR_INTC_MAX_NUM_INTR_INPUTS) {
		uint32_t isr = XIntc_GetIntrStatus(g_intc.BaseAddress);
		*pactive = (isr & (1u << irq)) ? 1u : 0u;
		return 0;
	}
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	uint32_t reg = XSCUGIC_PENDING_SET_OFFSET + (irq / 32U) * 4U;
	uint32_t val = Xil_In32(g_scugic.Config->DistBaseAddress + reg);
	*pactive = (val & (1U << (irq % 32U))) ? 1U : 0U;
	return 0;
}

/**
 * @brief Set interrupt priority.
 * @note INTC inputs do not support priority. GIC inputs do.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	if (!g_initialized)
		return -EINVAL;
	if (irq < XPAR_INTC_MAX_NUM_INTR_INPUTS)
		return -ENOTSUP;
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	if (priority > XSCUGIC_MAX_INTR_PRIO_VAL)
		return -EINVAL;
	uint8_t cur_prio, cur_trig;
	XScuGic_GetPriorityTriggerType(&g_scugic, irq, &cur_prio, &cur_trig);
	XScuGic_SetPriorityTriggerType(&g_scugic, irq, (uint8_t)priority, cur_trig);
	return 0;
}

/**
 * @brief Get interrupt priority.
 * @note INTC inputs do not support priority. GIC inputs do.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	if (!g_initialized || !priority)
		return -EINVAL;
	if (irq < XPAR_INTC_MAX_NUM_INTR_INPUTS)
		return -ENOTSUP;
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	uint8_t prio, trig;
	XScuGic_GetPriorityTriggerType(&g_scugic, irq, &prio, &trig);
	*priority = prio;
	return 0;
}

/**
 * @brief Configure interrupt trigger type.
 * @note INTC inputs do not support trigger config. GIC inputs do.
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_set_level_edge_trigger(uint32_t irq,
				    enum capi_irq_trig_level trigger)
{
	if (!g_initialized)
		return -EINVAL;
	if (irq < XPAR_INTC_MAX_NUM_INTR_INPUTS)
		return -ENOTSUP;
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	uint8_t new_trig;
	switch (trigger) {
	case CAPI_IRQ_LEVEL_HIGH:
		new_trig = 0x01;
		break;
	case CAPI_IRQ_EDGE_RISING:
		new_trig = 0x03;
		break;
	default:
		return -ENOTSUP;
	}
	uint8_t prio, trig;
	XScuGic_GetPriorityTriggerType(&g_scugic, irq, &prio, &trig);
	XScuGic_SetPriorityTriggerType(&g_scugic, irq, prio, new_trig);
	return 0;
}

/**
 * @brief Return the active XIntc instance.
 * @note INTC: Returns active XIntc instance.
 *
 * @return XIntc instance pointer, or NULL if not initialized.
 */
XIntc *capi_irq_get_intc_instance(void)
{
	return g_initialized ? &g_intc : NULL;
}
