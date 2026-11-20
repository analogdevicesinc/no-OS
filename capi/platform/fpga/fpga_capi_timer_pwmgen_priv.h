/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief AXI PWM generator register map and private handle for
 *        fpga_capi_timer_pwmgen.
 *
 * Register offsets, bits and the per-channel v1/v2 layout macros follow the
 * hdl/library/axi_pwm_gen register map, prefixed FPGA_PWMGEN_.
 */

#ifndef FPGA_CAPI_TIMER_PWMGEN_PRIV_H_
#define FPGA_CAPI_TIMER_PWMGEN_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include "fpga_capi_timer_pwmgen.h"

/* Register byte offsets (axi_pwm_gen_regmap.sv word address). */
#define FPGA_PWMGEN_REG_CORE_VERSION	0x00U /* CORE_VERSION (0x00) */
#define FPGA_PWMGEN_REG_ID		0x04U /* ID (0x01) */
#define FPGA_PWMGEN_REG_SCRATCHPAD	0x08U /* up_scratch (0x02), R/W */
#define FPGA_PWMGEN_REG_CORE_MAGIC	0x0CU /* CORE_MAGIC (0x03) */
#define FPGA_PWMGEN_REG_CONFIG		0x10U /* RESET/LOAD_CONFIG (0x04) */
#define FPGA_PWMGEN_REG_NPWM		0x14U /* N_PWMS (0x05) */
/* Channel period, PWM clock cycles (v2: up_pwm_period, 0x10 + ch) */
#define FPGA_PWMGEN_CHX_PERIOD(p, ch)	((p)->hw_major_ver == 1U ? \
					 (0x40U + 12U * (ch)) : (0x40U + 4U * (ch)))
/* Channel high time, PWM clock cycles (v2: up_pwm_width, 0x20 + ch) */
#define FPGA_PWMGEN_CHX_DUTY(p, ch)	((p)->hw_major_ver == 1U ? \
					 (0x44U + 12U * (ch)) : (0x80U + 4U * (ch)))
/* Channel offset, PWM clock cycles (v2: up_pwm_offset, 0x30 + ch) */
#define FPGA_PWMGEN_CHX_PHASE(p, ch)	((p)->hw_major_ver == 1U ? \
					 (0x48U + 12U * (ch)) : (0xC0U + 4U * (ch)))
/* Bits and helpers (axi_pwm_gen_regmap.sv / axi_pwm_gen.sv). */
#define FPGA_PWMGEN_TEST_DATA		0x5A0F0081U /* scratch pattern */
#define FPGA_PWMGEN_LOAD_CONFIG		(1U << 1) /* up_load_config */
#define FPGA_PWMGEN_RESET		(1U << 0) /* up_reset, 1 = in reset */
#define FPGA_PWMGEN_CHANNEL_DISABLE	0U /* zero period/duty */
/* v1: 4, v2: 16 */
#define FPGA_PWMGEN_MAX_CHANNELS(p)	((p)->hw_major_ver == 1U ? 4U : 16U)
#define FPGA_PWMGEN_NSEC_PER_USEC	1000ULL /* ns per us */
#define FPGA_PWMGEN_USEC_PER_SEC	1000000ULL /* us per s */
#define FPGA_PWMGEN_VERSION_MAJOR(x)	(((x) >> 16) & 0xFFU) /* [23:16] */
#define FPGA_PWMGEN_VERSION_MINOR(x)	(((x) >> 8) & 0xFFU) /* [15:8] */
#define FPGA_PWMGEN_VERSION_PATCH(x)	((x) & 0xFFU) /* [7:0] */

/* CORE_MAGIC ("PLSG", axi_pwm_gen.sv) presence check. */
#define FPGA_PWMGEN_CORE_MAGIC		0x601A3471U
/* Ceiling of the v2 banked layout; sizes the per-channel state. */
#define FPGA_PWMGEN_CHANNELS_MAX	16U

/**
 * @brief Per-channel state (axi_pwm_desc fields).
 */
struct fpga_pwmgen_chan {
	/** Channel claimed through channel_init(). */
	bool initialized;
	/** Channel output running (axi_pwm enabled). */
	bool enabled;
	/** Period in ticks written by channel_enable() (axi_desc->ch_period). */
	uint32_t period_ticks;
	/** Last channel_config period / active / offset in ns (axi_pwm
	 *  desc->period_ns / duty_cycle_ns / phase_ns). */
	uint64_t period_ns;
	uint64_t duty_ns;
	uint64_t phase_ns;
};

/**
 * @brief Per-controller private handle. Lives in capi_timer_handle.priv.
 */
struct fpga_pwmgen_priv {
	/** Generator register-map base address. */
	uint32_t base;
	/** Generator ext_clk in Hz (from capi_timer_config.input_clock_hz). */
	uint64_t input_clock_hz;
	/** Core major version from CORE_VERSION; selects the channel layout. */
	uint8_t hw_major_ver;
	/** Channel count reported by NPWM. */
	uint32_t num_channels;
	/** Per-channel state. */
	struct fpga_pwmgen_chan chan[FPGA_PWMGEN_CHANNELS_MAX];
};

/**
 * @brief Static handle initializer for apps that avoid malloc.
 */
#define CAPI_TIMER_HANDLE_FPGA_INIT()			\
	(&(struct capi_timer_handle) {		\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct fpga_pwmgen_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* FPGA_CAPI_TIMER_PWMGEN_PRIV_H_ */
