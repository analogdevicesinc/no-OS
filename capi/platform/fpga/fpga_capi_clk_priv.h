/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief AXI CLKGEN register map and private handle for fpga_capi_clk.
 *
 * Core registers (RESETN, STATUS, DRP_CNTRL/DRP_STATUS, FPGA_INFO,
 * FPGA_VOLTAGE) and the MMCM DRP registers (CLKOUT0, CLK_FB, CLK_DIV, LOCK,
 * FILTER), as defined by hdl/library/axi_clkgen and the Xilinx MMCM DRP map,
 * all prefixed FPGA_CLKGEN_.
 */

#ifndef _FPGA_CAPI_CLK_PRIV_H_
#define _FPGA_CAPI_CLK_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include "fpga_capi_clk.h"

#define FPGA_CLKGEN_REG_VERSION			0x0000U
#define FPGA_CLKGEN_VERSION_MAJOR(x)		(((x) >> 16) & 0xFFU)

#define FPGA_CLKGEN_REG_FPGA_INFO		0x001CU
#define FPGA_CLKGEN_REG_FPGA_VOLTAGE		0x0140U

#define FPGA_CLKGEN_INFO_FPGA_TECH(info)	((info) >> 24)
#define FPGA_CLKGEN_INFO_FPGA_FAMILY(info)	(((info) >> 16) & 0xFFU)
#define FPGA_CLKGEN_INFO_FPGA_SPEED_GRADE(info)	(((info) >> 8) & 0xFFU)
#define FPGA_CLKGEN_INFO_FPGA_VOLTAGE(val)	((val) & 0xFFFFU)

#define FPGA_CLKGEN_REG_RESETN			0x40U
#define FPGA_CLKGEN_MMCM_RESETN			(1U << 1)
#define FPGA_CLKGEN_RESETN			(1U << 0)

#define FPGA_CLKGEN_REG_STATUS			0x5CU
#define FPGA_CLKGEN_STATUS			(1U << 0)

#define FPGA_CLKGEN_REG_DRP_CNTRL		0x70U
#define FPGA_CLKGEN_DRP_CNTRL_SEL		(1U << 29)
#define FPGA_CLKGEN_DRP_CNTRL_READ		(1U << 28)

#define FPGA_CLKGEN_REG_DRP_STATUS		0x74U
#define FPGA_CLKGEN_DRP_STATUS_BUSY		(1U << 16)

#define FPGA_CLKGEN_MMCM_REG_CLKOUT0_1		0x08U
#define FPGA_CLKGEN_MMCM_REG_CLKOUT0_2		0x09U
#define FPGA_CLKGEN_MMCM_REG_CLKOUT1_1		0x0AU
#define FPGA_CLKGEN_MMCM_REG_CLKOUT1_2		0x0BU
#define FPGA_CLKGEN_MMCM_REG_CLK_FB1		0x14U
#define FPGA_CLKGEN_MMCM_REG_CLK_FB2		0x15U
#define FPGA_CLKGEN_MMCM_REG_CLK_DIV		0x16U
#define FPGA_CLKGEN_MMCM_REG_LOCK1		0x18U
#define FPGA_CLKGEN_MMCM_REG_LOCK2		0x19U
#define FPGA_CLKGEN_MMCM_REG_LOCK3		0x1AU
#define FPGA_CLKGEN_MMCM_REG_FILTER1		0x4EU
#define FPGA_CLKGEN_MMCM_REG_FILTER2		0x4FU

/* FPGA_INFO decode values (technology / family / speed grade). */
#define FPGA_CLKGEN_TECH_ULTRASCALE_PLUS	3U
#define FPGA_CLKGEN_FAMILY_ARTIX		1U
#define FPGA_CLKGEN_FAMILY_KINTEX		2U
#define FPGA_CLKGEN_SPEED_1			10U
#define FPGA_CLKGEN_SPEED_1LV			14U
#define FPGA_CLKGEN_SPEED_2			20U
#define FPGA_CLKGEN_SPEED_2LV			22U
#define FPGA_CLKGEN_SPEED_3			30U

/* DRP busy-wait bound and MMCM lock poll (1 ms steps). */
#define FPGA_CLKGEN_DRP_TIMEOUT			1000000U
#define FPGA_CLKGEN_LOCK_TIMEOUT_MS		100U

/**
 * @brief Per-controller private handle. Lives in capi_clk_handle.priv.
 */
struct fpga_clk_priv_handle {
	/** CLKGEN register-map base address. */
	uint32_t base;
	/** Reference (parent) clock in Hz. */
	uint32_t parent_rate_hz;
};

/**
 * @brief Stack allocation helper for apps that avoid malloc.
 */
#define FPGA_CLK_HANDLE_DEFINE(name)                                    \
	struct capi_clk_handle name = {                                 \
		.ops = NULL,                                            \
		.init_allocated = false,                                \
		.priv = &(struct fpga_clk_priv_handle){0}               \
	}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _FPGA_CAPI_CLK_PRIV_H_ */
