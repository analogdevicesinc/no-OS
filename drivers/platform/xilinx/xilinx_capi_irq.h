/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx IRQ driver declarations - shared header for GIC and INTC backends.
 *
 * Two mutually exclusive backends exist:
 *   xilinx_capi_irq_gic.c  - XScuGic (PS GIC, Zynq/ZynqMP/Versal)
 *   xilinx_capi_irq_intc.c - XIntc   (AXI INTC, cascade or standalone)
 *
 * Linking both in the same build produces a duplicate-symbol linker error.
 * That is intentional - one system, one interrupt controller.
 *
 * config->irq_ctrl_id:
 *   GIC  - SDT: XPAR_XSCUGIC_0_BASEADDR;  Legacy: XPAR_SCUGIC_SINGLE_DEVICE_ID
 *   INTC - SDT: XPAR_AXI_INTC_0_BASEADDR; Legacy: XPAR_AXI_INTC_0_DEVICE_ID
 *   (platform_ids.h provides IRQ_GIC_ID and IRQ_INTC_ID for both BSP styles)
 *
 * config->extra:
 *   GIC  - NULL
 *   INTC - NULL (cascade auto-detected from XIntc_Config IntrId/IntrParent)
 */

#ifndef _XILINX_CAPI_IRQ_H_
#define _XILINX_CAPI_IRQ_H_

#include <stdint.h>
#include <capi_irq.h>

#ifdef __cplusplus
extern "C" {
#endif

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
