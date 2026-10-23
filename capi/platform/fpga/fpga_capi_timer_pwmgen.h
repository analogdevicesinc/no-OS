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
 *   - extra          : NULL, or struct fpga_pwmgen_config * to program and
 *                      enable one channel at init (axi_pwm_init)
 *
 * Channels support CAPI_TIMER_PWM_MODE only, active-high: period, active time
 * and offset (phase) in ns. A channel is programmed by channel_config (or the
 * single-value setters) and starts or stops on channel_enable /
 * channel_disable. As axi_pwm: a duty above the period is clamped, period 0
 * and inverted polarity are accepted (the core cannot invert), new values of
 * a running channel apply at its next enable, and enable / disable write
 * LOAD_CONFIG alone, which also clears RESET (so a channel enable after stop
 * restarts the core). Both register layouts are handled: v1 (4 interleaved
 * channels) and v2 (16 banked channels), selected from CORE_VERSION major
 * (1 = v1, anything else v2, as AXI_PWMGEN_MAX_CHANNELS).
 */

#ifndef FPGA_CAPI_TIMER_PWMGEN_H_
#define FPGA_CAPI_TIMER_PWMGEN_H_

#include <capi_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct fpga_pwmgen_config
 * @brief Optional capi_timer_config.extra: the axi_pwm_init parameters.
 */
struct fpga_pwmgen_config {
	/** Channel programmed and enabled at init. */
	uint32_t channel;
	/** Its period / active time / offset (ns); polarity is ignored. */
	struct capi_timer_pwm_config pwm;
};

/**
 * @brief AXI PWM generator timer operations table. Select via config->ops.
 */
extern const struct capi_timer_ops fpga_capi_timer_pwmgen_ops;

/**
 * @brief Set the period of a channel.
 * @note axi_pwm_set_period: PERIOD = ticks when enabled, else 0 (enable
 *       writes it); no LOAD_CONFIG.
 * @param handle - Timer handle.
 * @param chan - Channel index (channel_init done).
 * @param period_ns - Period in ns (0 accepted).
 * @return 0 on success, -EINVAL on bad arguments or a tick count above 32 bit.
 */
int fpga_pwmgen_set_period(struct capi_timer_handle *handle, uint32_t chan,
			   uint64_t period_ns);

/**
 * @brief Set the active (duty) time of a channel, clamped to the period.
 * @note axi_pwm_set_duty_cycle; no LOAD_CONFIG.
 * @param handle - Timer handle.
 * @param chan - Channel index (channel_init done).
 * @param duty_ns - Active time in ns.
 * @return 0 on success, -EINVAL on bad arguments or a tick overflow.
 */
int fpga_pwmgen_set_duty(struct capi_timer_handle *handle, uint32_t chan,
			 uint64_t duty_ns);

/**
 * @brief Set the offset (phase) of a channel.
 * @note axi_pwm_set_phase; no LOAD_CONFIG.
 * @param handle - Timer handle.
 * @param chan - Channel index (channel_init done).
 * @param phase_ns - Offset in ns.
 * @return 0 on success, -EINVAL on bad arguments or a tick overflow.
 */
int fpga_pwmgen_set_phase(struct capi_timer_handle *handle, uint32_t chan,
			  uint64_t phase_ns);

/**
 * @brief Period of a channel as last configured.
 * @note axi_pwm_get_period(): returns the cached ns value, no register read.
 * @param handle - Timer handle.
 * @param chan - Channel index (channel_init done).
 * @param period_ns - Receives the period in ns (0 before channel_config).
 * @return 0 on success, -EINVAL on bad arguments.
 */
int fpga_pwmgen_get_period(struct capi_timer_handle *handle, uint32_t chan,
			   uint64_t *period_ns);

/**
 * @brief Active (duty) time of a channel as last configured.
 * @note axi_pwm_get_duty_cycle(): cached ns value.
 * @param handle - Timer handle.
 * @param chan - Channel index (channel_init done).
 * @param duty_ns - Receives the active time in ns.
 * @return 0 on success, -EINVAL on bad arguments.
 */
int fpga_pwmgen_get_duty(struct capi_timer_handle *handle, uint32_t chan,
			 uint64_t *duty_ns);

/**
 * @brief Offset (phase) of a channel as last configured.
 * @note axi_pwm_get_phase(): cached ns value.
 * @param handle - Timer handle.
 * @param chan - Channel index (channel_init done).
 * @param phase_ns - Receives the offset in ns.
 * @return 0 on success, -EINVAL on bad arguments.
 */
int fpga_pwmgen_get_phase(struct capi_timer_handle *handle, uint32_t chan,
			  uint64_t *phase_ns);

#ifdef __cplusplus
}
#endif

#endif /* FPGA_CAPI_TIMER_PWMGEN_H_ */
