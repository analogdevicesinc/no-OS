/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief AXI CLKGEN register map and private handle for fpga_capi_clkgen.
 *
 * Core registers (RESETN, STATUS, DRP_CNTRL/DRP_STATUS, FPGA_INFO,
 * FPGA_VOLTAGE) and the MMCM DRP registers (CLKOUT0, CLK_FB, CLK_DIV, LOCK,
 * FILTER), as defined by hdl/library/axi_clkgen and the Xilinx MMCM DRP map,
 * all prefixed FPGA_CLKGEN_.
 */

#ifndef FPGA_CAPI_CLKGEN_PRIV_H_
#define FPGA_CAPI_CLKGEN_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include "fpga_capi_clkgen.h"

/* Register byte offsets (up_clkgen.v word address in parens). */
#define FPGA_CLKGEN_REG_VERSION			0x0000U /* PCORE_VERSION */
/* PCORE_VERSION[23:16] */
#define FPGA_CLKGEN_VERSION_MAJOR(x)		(((x) >> 16) & 0xFFU)

#define FPGA_CLKGEN_REG_FPGA_INFO		0x001CU /* (0x07) RO */
#define FPGA_CLKGEN_REG_FPGA_VOLTAGE		0x0140U /* (0x50) mV */

/* [31:24] FPGA_TECHNOLOGY */
#define FPGA_CLKGEN_INFO_FPGA_TECH(info)	((info) >> 24)
/* [23:16] FPGA_FAMILY */
#define FPGA_CLKGEN_INFO_FPGA_FAMILY(info)	(((info) >> 16) & 0xFFU)
/* [15:8] SPEED_GRADE */
#define FPGA_CLKGEN_INFO_FPGA_SPEED_GRADE(info)	(((info) >> 8) & 0xFFU)
/* [15:0] mV */
#define FPGA_CLKGEN_INFO_FPGA_VOLTAGE(val)	((val) & 0xFFFFU)

#define FPGA_CLKGEN_REG_RESETN			0x40U /* (0x10) */
#define FPGA_CLKGEN_MMCM_RESETN			(1U << 1) /* up_mmcm_resetn */
#define FPGA_CLKGEN_RESETN			(1U << 0) /* up_resetn */

#define FPGA_CLKGEN_REG_STATUS			0x5CU /* (0x17) */
#define FPGA_CLKGEN_STATUS			(1U << 0) /* up_drp_locked */

/* (0x1c): [28] rwn [27:16] addr [15:0] data */
#define FPGA_CLKGEN_REG_DRP_CNTRL		0x70U
#define FPGA_CLKGEN_DRP_CNTRL_SEL		(1U << 29) /* ignored by HDL */
#define FPGA_CLKGEN_DRP_CNTRL_READ		(1U << 28) /* up_drp_rwn */

/* (0x1d): [17] locked [15:0] rdata */
#define FPGA_CLKGEN_REG_DRP_STATUS		0x74U
#define FPGA_CLKGEN_DRP_STATUS_BUSY		(1U << 16) /* up_drp_status */

/* Xilinx MMCM DRP addresses (as used by clk_axi_clkgen.c). */
#define FPGA_CLKGEN_MMCM_REG_CLKOUT0_1		0x08U /* CLKOUT0 reg 1 */
#define FPGA_CLKGEN_MMCM_REG_CLKOUT0_2		0x09U /* CLKOUT0 reg 2 */
#define FPGA_CLKGEN_MMCM_REG_CLKOUT1_1		0x0AU /* CLKOUT1 reg 1 */
#define FPGA_CLKGEN_MMCM_REG_CLKOUT1_2		0x0BU /* CLKOUT1 reg 2 */
#define FPGA_CLKGEN_MMCM_REG_CLK_FB1		0x14U /* CLKFBOUT reg 1 */
#define FPGA_CLKGEN_MMCM_REG_CLK_FB2		0x15U /* CLKFBOUT reg 2 */
#define FPGA_CLKGEN_MMCM_REG_CLK_DIV		0x16U /* DIVCLK */
#define FPGA_CLKGEN_MMCM_REG_LOCK1		0x18U /* lock reg 1 */
#define FPGA_CLKGEN_MMCM_REG_LOCK2		0x19U /* lock reg 2 */
#define FPGA_CLKGEN_MMCM_REG_LOCK3		0x1AU /* lock reg 3 */
#define FPGA_CLKGEN_MMCM_REG_FILTER1		0x4EU /* filter reg 1 */
#define FPGA_CLKGEN_MMCM_REG_FILTER2		0x4FU /* filter reg 2 */

/* FPGA_INFO decode values (technology / family / speed grade). */
#define FPGA_CLKGEN_TECH_ULTRASCALE_PLUS	3U /* ultrascale+ */
#define FPGA_CLKGEN_FAMILY_ARTIX		1U /* artix */
#define FPGA_CLKGEN_FAMILY_KINTEX		2U /* kintex */
#define FPGA_CLKGEN_SPEED_1			10U /* -1 */
#define FPGA_CLKGEN_SPEED_1LV			14U /* -1LV */
#define FPGA_CLKGEN_SPEED_2			20U /* -2 */
#define FPGA_CLKGEN_SPEED_2LV			22U /* -2LV */
#define FPGA_CLKGEN_SPEED_3			30U /* -3 */

/* DRP busy-wait bound and MMCM lock poll (1 ms steps). */
#define FPGA_CLKGEN_DRP_TIMEOUT			1000000U /* busy polls */
#define FPGA_CLKGEN_LOCK_TIMEOUT_MS		100U /* ms */

/**
 * @brief Per-controller private handle. Lives in capi_clk_handle.priv.
 */
struct fpga_clkgen_priv {
	/** CLKGEN register-map base address. */
	uint32_t base;
	/** Reference (parent) clock in Hz. */
	uint32_t parent_rate_hz;
};

/**
 * @brief Static handle initializer for apps that avoid malloc.
 */
#define CAPI_CLK_HANDLE_FPGA_INIT()			\
	(&(struct capi_clk_handle) {		\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct fpga_clkgen_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* FPGA_CAPI_CLKGEN_PRIV_H_ */
