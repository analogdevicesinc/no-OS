/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief CAPI timer (PWM mode) backend for the ADI AXI PWM generator core
 *        (axi_pwm_gen).
 *
 * Drives the PWM channels of an axi_pwm_gen instance, e.g. as the periodic
 * trigger of a SPI Engine offload, using only CAPI services; registers are
 * accessed through CAPI_PLATFORM_IO_*. The core is packaged for both Xilinx
 * and Intel fabric.
 *
 * Configuration (struct capi_timer_config):
 *   - ops            : &fpga_capi_timer_pwmgen_ops
 *   - identifier     : axi_pwm_gen base address
 *   - input_clock_hz : the generator's ext_clk in Hz (at least 1 kHz); the
 *                      ns values of a channel config are converted with it
 *   - extra          : unused
 *
 * Channels support CAPI_TIMER_PWM_MODE only, active-high: period, active time
 * and offset (phase) in ns. A channel is programmed by channel_config and
 * starts or stops on channel_enable / channel_disable. Both register layouts
 * are handled: v1 (4 interleaved channels) and v2 (16 banked channels),
 * selected from CORE_VERSION at init.
 */

#ifndef _FPGA_CAPI_TIMER_PWMGEN_H_
#define _FPGA_CAPI_TIMER_PWMGEN_H_

#include <capi_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AXI PWM generator timer operations table. Select via config->ops.
 */
extern const struct capi_timer_ops fpga_capi_timer_pwmgen_ops;

#ifdef __cplusplus
}
#endif

#endif /* _FPGA_CAPI_TIMER_PWMGEN_H_ */
