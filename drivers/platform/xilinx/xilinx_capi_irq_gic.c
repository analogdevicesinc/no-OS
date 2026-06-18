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
#include "capi_irq.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_io.h"

#include "xinterrupt_wrap.h"

static bool g_initialized = false;
static XScuGic g_scugic;

/**
 * @brief Initialize the CAPI backend instance.
 * @note PS: XScuGic_LookupConfig()/XScuGic_CfgInitialize().
 *
 * @return 0 on success, negative errno on failure.
 */
int capi_irq_init(struct capi_irq_config *config)
{
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

	for (uint32_t i = 0; i < XSCUGIC_MAX_NUM_INTR_INPUTS; i++) {
		XScuGic_Disable(&g_scugic, i);
		XScuGic_Disconnect(&g_scugic, i);
	}

	Xil_ExceptionRemoveHandler(XIL_EXCEPTION_ID_INT);
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
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	if (isr == NULL)
		return -EINVAL;

	int status = XScuGic_Connect(&g_scugic, irq, (Xil_InterruptHandler)isr, arg);
	return (status == XST_SUCCESS) ? 0 : -EIO;
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
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;

	XScuGic_Enable(&g_scugic, irq);
	return 0;
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
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;

	XScuGic_Disable(&g_scugic, irq);
	return 0;
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
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;

	uint32_t reg = XSCUGIC_PENDING_CLR_OFFSET + (irq / 32U) * 4U;
	uint32_t bit = 1U << (irq % 32U);
	Xil_Out32(g_scugic.Config->DistBaseAddress + reg, bit);
	return 0;
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
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;

	uint32_t reg = XSCUGIC_PENDING_SET_OFFSET + (irq / 32U) * 4U;
	uint32_t bit = 1U << (irq % 32U);
	uint32_t val = Xil_In32(g_scugic.Config->DistBaseAddress + reg);
	*pactive = (val & bit) ? 1U : 0U;
	return 0;
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
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;

	/* XScuGic_SetPriorityTriggerType() asserts above the BSP max. */
	if (priority > XSCUGIC_MAX_INTR_PRIO_VAL)
		return -EINVAL;

	uint32_t id = irq;
	uint8_t cur_prio, cur_trig;
	XScuGic_GetPriorityTriggerType(&g_scugic, id, &cur_prio, &cur_trig);
	XScuGic_SetPriorityTriggerType(&g_scugic, id, (uint8_t)priority, cur_trig);
	return 0;
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
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;
	if (priority == NULL)
		return -EINVAL;

	uint32_t id = irq;
	uint8_t prio, trig;
	XScuGic_GetPriorityTriggerType(&g_scugic, id, &prio, &trig);
	*priority = prio;
	return 0;
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
	if (irq >= XSCUGIC_MAX_NUM_INTR_INPUTS)
		return -EINVAL;

	uint32_t id = irq;
	uint8_t prio, trig;
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

	XScuGic_GetPriorityTriggerType(&g_scugic, id, &prio, &trig);
	XScuGic_SetPriorityTriggerType(&g_scugic, id, prio, new_trig);
	return 0;
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
