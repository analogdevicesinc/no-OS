/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief CAPI clock backend for the ADI AXI CLKGEN core (axi_clkgen).
 *
 * Controls the MMCM inside an ADI AXI CLKGEN instance: releases/holds its
 * reset, reads the output rate back and programs a new rate. Uses only CAPI
 * services; registers are accessed through CAPI_PLATFORM_IO_*.
 *
 * The axi_clkgen core wraps a Xilinx MMCM and is packaged for Vivado only, so
 * this backend applies to Xilinx fabric. Where a core's clock comes from a
 * fixed PLL instead (e.g. the SPI Engine SCLK on Intel fabric), no clock
 * handle is needed.
 *
 * Configuration (struct capi_clk_config):
 *   - ops      : &fpga_capi_clk_ops
 *   - priv     : struct fpga_capi_clk_config * (core base, parent clock rate)
 *   - name, chan_num : unused
 *
 * Operations:
 *   - init / deinit : store / free the handle; no register access.
 *   - enable        : release the core and MMCM resets, wait for lock
 *                     (100 ms max).
 *   - disable       : hold the MMCM in reset; the register interface stays up.
 *   - get_rate      : parent * M / (D * DOUT), read from the MMCM over DRP.
 *   - round_rate    : best D/M/DOUT within the device VCO/PFD limits
 *                     (selected from FPGA_INFO / FPGA_VOLTAGE).
 *   - set_rate      : program those dividers plus the filter and lock values
 *                     over DRP, then wait for lock.
 */

#ifndef _FPGA_CAPI_CLK_H_
#define _FPGA_CAPI_CLK_H_

#include <stdint.h>
#include <capi_clk.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct fpga_capi_clk_config
 * @brief CLKGEN configuration, passed via capi_clk_config.priv.
 */
struct fpga_capi_clk_config {
	/** ADI AXI CLKGEN register-map base address. */
	uint32_t base;
	/** Reference (parent) clock feeding the MMCM, in Hz. */
	uint32_t parent_rate_hz;
};

/**
 * @brief CLKGEN operations table. Select via config->ops.
 */
extern const struct capi_clk_ops fpga_capi_clk_ops;

#ifdef __cplusplus
}
#endif

#endif /* _FPGA_CAPI_CLK_H_ */
