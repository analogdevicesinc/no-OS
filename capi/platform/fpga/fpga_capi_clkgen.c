/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Implementation of the AXI CLKGEN CAPI clock backend.
 *
 * The axi_clkgen core wraps a Xilinx MMCM, the FPGA's clock synthesizer:
 *   - PFD (phase-detector input) = parent clock / D, D = input divider
 *   - VCO = PFD x M, M = feedback multiplier
 *   - output = VCO / DOUT, DOUT = output divider
 *
 * e.g. parent 100 MHz, D = 1, M = 10, DOUT = 5 -> VCO 1 GHz, output 200 MHz.
 *
 * The core's own registers control the resets and report lock. The MMCM
 * dividers are not mapped directly: they sit behind the MMCM's DRP (Dynamic
 * Reconfiguration Port), which the core exposes as an indirect address/data
 * window, DRP_CNTRL / DRP_STATUS. Each DRP access waits, bounded, for the
 * busy flag to clear.
 *
 * Reading the rate (get_rate): rate = parent_rate_hz x M / (D x DOUT), with
 * D, M and DOUT read back from the CLK_DIV, CLK_FB and CLKOUT0 DRP
 * registers.
 *
 * Setting a rate (set_rate; round_rate stops after step 2):
 *   1. Select the allowed VCO and PFD frequency ranges. They depend on the
 *      FPGA family, speed grade and core voltage, which the core reports in
 *      FPGA_INFO and FPGA_VOLTAGE.
 *   2. Search D, M and DOUT for the closest achievable rate.
 *   3. Look up the loop filter and lock-detector settings that Xilinx
 *      specifies for that M (fixed tables).
 *   4. Hold the MMCM in reset and write dividers, filter and lock over DRP.
 *   5. Release the reset and poll STATUS for lock (1 ms steps, 100 ms max).
 *
 * enable / disable only release / hold the MMCM reset; the register
 * interface stays accessible while the MMCM is in reset.
 *
 * See fpga_capi_clkgen.h for configuration.
 */

#include "fpga_capi_clkgen_priv.h"
#include "capi_alloc.h"
#include "capi_time.h"
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include "capi_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MMCM loop filter settings per feedback multiplier, indexed by M - 1
 * (Xilinx XAPP888 "optimized" bandwidth). The filter (charge-pump current,
 * resistor, capacitor) sets the loop bandwidth, which must follow M because
 * the loop gain scales with it. Each entry packs two DRP words:
 * bits 31:16 -> FILTER1, bits 15:0 -> FILTER2, both written with mask 0x9900.
 * M > 47 uses 0x08008090.
 */
static const uint32_t fpga_clkgen_filter_table[] = {
	0x01001990, 0x01001190, 0x01009890, 0x01001890,
	0x01008890, 0x01009090, 0x01009090, 0x01009090,
	0x01009090, 0x01000890, 0x01000890, 0x01000890,
	0x08009090, 0x01001090, 0x01001090, 0x01001090,
	0x01001090, 0x01001090, 0x01001090, 0x01001090,
	0x01001090, 0x01001090, 0x01001090, 0x01008090,
	0x01008090, 0x01008090, 0x01008090, 0x01008090,
	0x01008090, 0x01008090, 0x01008090, 0x01008090,
	0x01008090, 0x01008090, 0x01008090, 0x01008090,
	0x01008090, 0x08001090, 0x08001090, 0x08001090,
	0x08001090, 0x08001090, 0x08001090, 0x08001090,
	0x08001090, 0x08001090, 0x08001090
};

/*
 * MMCM lock-detector settings per feedback multiplier, indexed by M - 1
 * (Xilinx XAPP888). The lock detector raises LOCKED once the output is
 * stable. Each entry packs:
 *   - bits 9:0   -> LOCK1, lock count: reference cycles that must match
 *     before LOCKED (fewer at high M, where the VCO settles faster)
 *   - bits 20:16 -> LOCK2[14:10], feedback-path delay
 *   - bits 28:24 -> LOCK3[14:10], reference-path delay
 * LOCK2[9:0] (unlock count, 1) and LOCK3[9:0] (lock saturation, 1001) are
 * fixed. M > 36 uses 0x1f1f00fa.
 */
static const uint32_t fpga_clkgen_lock_table[] = {
	0x060603e8, 0x060603e8, 0x080803e8, 0x0b0b03e8,
	0x0e0e03e8, 0x111103e8, 0x131303e8, 0x161603e8,
	0x191903e8, 0x1c1c03e8, 0x1f1f0384, 0x1f1f0339,
	0x1f1f02ee, 0x1f1f02bc, 0x1f1f028a, 0x1f1f0271,
	0x1f1f023f, 0x1f1f0226, 0x1f1f020d, 0x1f1f01f4,
	0x1f1f01db, 0x1f1f01c2, 0x1f1f01a9, 0x1f1f0190,
	0x1f1f0190, 0x1f1f0177, 0x1f1f015e, 0x1f1f015e,
	0x1f1f0145, 0x1f1f0145, 0x1f1f012c, 0x1f1f012c,
	0x1f1f012c, 0x1f1f0113, 0x1f1f0113, 0x1f1f0113
};

/**
 * @brief Write a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @param val - Value to write.
 */
static inline void fpga_clkgen_wr(uint32_t base, uint32_t reg, uint32_t val)
{
	CAPI_PLATFORM_IO_WRITE32(base + reg, val);
}

/**
 * @brief Read a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @return Register value.
 */
static inline uint32_t fpga_clkgen_rd(uint32_t base, uint32_t reg)
{
	return CAPI_PLATFORM_IO_READ32(base + reg);
}

/* --- MMCM / DRP helpers (clk_axi_clkgen.c) ------------------------------- */

/**
 * @brief Wait until the DRP port is idle.
 * @param base - CLKGEN base address.
 * @param val - Output: last DRP_STATUS value read.
 * @return 0 when idle, -ETIMEDOUT if still busy after the bound.
 */
static int fpga_clkgen_drp_wait(uint32_t base, uint32_t *val)
{
	uint32_t timeout = FPGA_CLKGEN_DRP_TIMEOUT;

	do {
		*val = fpga_clkgen_rd(base, FPGA_CLKGEN_REG_DRP_STATUS);
	} while ((*val & FPGA_CLKGEN_DRP_STATUS_BUSY) && --timeout);

	return timeout ? 0 : -ETIMEDOUT;
}

/**
 * @brief Read an MMCM register over DRP.
 * @param base - CLKGEN base address.
 * @param reg - MMCM register address.
 * @param val - Output: 16-bit register value.
 * @return 0 on success, -ETIMEDOUT if the DRP port stays busy.
 */
static int fpga_clkgen_mmcm_read(uint32_t base, uint32_t reg, uint32_t *val)
{
	uint32_t reg_val;
	int ret;

	ret = fpga_clkgen_drp_wait(base, &reg_val);
	if (ret)
		return ret;

	fpga_clkgen_wr(base, FPGA_CLKGEN_REG_DRP_CNTRL, FPGA_CLKGEN_DRP_CNTRL_SEL |
		       FPGA_CLKGEN_DRP_CNTRL_READ | (reg << 16));

	ret = fpga_clkgen_drp_wait(base, val);
	if (ret)
		return ret;

	*val &= 0xffff;

	return 0;
}

/**
 * @brief Read-modify-write an MMCM register over DRP.
 * @param base - CLKGEN base address.
 * @param reg - MMCM register address.
 * @param val - Value to write (masked).
 * @param mask - Bits of reg to update; 0xffff writes the full register.
 * @return 0 on success, -ETIMEDOUT if the DRP port stays busy.
 */
static int fpga_clkgen_mmcm_write(uint32_t base, uint32_t reg, uint32_t val,
				  uint32_t mask)
{
	uint32_t reg_val;
	int ret;

	ret = fpga_clkgen_drp_wait(base, &reg_val);
	if (ret)
		return ret;

	if (mask != 0xffff) {
		ret = fpga_clkgen_mmcm_read(base, reg, &reg_val);
		if (ret)
			return ret;
		reg_val &= ~mask;
	} else {
		reg_val = 0;
	}

	reg_val |= FPGA_CLKGEN_DRP_CNTRL_SEL | (reg << 16) | (val & mask);
	fpga_clkgen_wr(base, FPGA_CLKGEN_REG_DRP_CNTRL, reg_val);

	return 0;
}

/**
 * @brief Look up the MMCM loop-filter setting for a feedback multiplier.
 * @param m - Feedback multiplier minus one.
 * @return Filter word.
 */
static uint32_t fpga_clkgen_lookup_filter(uint32_t m)
{
	if (m < 47)
		return fpga_clkgen_filter_table[m];

	return 0x08008090;
}

/**
 * @brief Look up the MMCM lock-detector setting for a feedback multiplier.
 * @param m - Feedback multiplier minus one.
 * @return Lock word.
 */
static uint32_t fpga_clkgen_lookup_lock(uint32_t m)
{
	if (m < 36)
		return fpga_clkgen_lock_table[m];

	return 0x1f1f00fa;
}

/**
 * @brief Narrow the PFD/VCO limits from the FPGA_INFO/VOLTAGE registers.
 * @param base - CLKGEN base address.
 * @param fpfd_max - In/out: PFD maximum in kHz.
 * @param fvco_min - In/out: VCO minimum in kHz.
 * @param fvco_max - In/out: VCO maximum in kHz.
 */
static void fpga_clkgen_setup_ranges(uint32_t base, uint32_t *fpfd_max,
				     uint32_t *fvco_min, uint32_t *fvco_max)
{
	uint32_t reg_value, tech, family, speed_grade, voltage;

	reg_value = fpga_clkgen_rd(base, FPGA_CLKGEN_REG_FPGA_INFO);
	tech = FPGA_CLKGEN_INFO_FPGA_TECH(reg_value);
	family = FPGA_CLKGEN_INFO_FPGA_FAMILY(reg_value);
	speed_grade = FPGA_CLKGEN_INFO_FPGA_SPEED_GRADE(reg_value);

	reg_value = fpga_clkgen_rd(base, FPGA_CLKGEN_REG_FPGA_VOLTAGE);
	voltage = FPGA_CLKGEN_INFO_FPGA_VOLTAGE(reg_value);

	if (speed_grade >= FPGA_CLKGEN_SPEED_1 &&
	    speed_grade <= FPGA_CLKGEN_SPEED_1LV) {
		*fvco_max = 1200000;
		*fpfd_max = 450000;
	} else if (speed_grade >= FPGA_CLKGEN_SPEED_2 &&
		   speed_grade <= FPGA_CLKGEN_SPEED_2LV) {
		*fvco_max = 1440000;
		*fpfd_max = 500000;
		if ((family == FPGA_CLKGEN_FAMILY_KINTEX ||
		     family == FPGA_CLKGEN_FAMILY_ARTIX) && voltage < 950) {
			*fvco_max = 1200000;
			*fpfd_max = 450000;
		}
	} else if (speed_grade == FPGA_CLKGEN_SPEED_3) {
		*fvco_max = 1600000;
		*fpfd_max = 550000;
	}

	if (tech == FPGA_CLKGEN_TECH_ULTRASCALE_PLUS) {
		*fvco_max = 1600000;
		*fvco_min = 800000;
	}
}

/**
 * @brief Search the input/feedback/output dividers closest to a rate.
 * @param base - CLKGEN base address.
 * @param fin - Parent rate in Hz.
 * @param fout - Requested rate in Hz.
 * @param best_d - Output: input divider (0 if none found).
 * @param best_m - Output: feedback multiplier.
 * @param best_dout - Output: CLK0 output divider.
 */
static void fpga_clkgen_calc_params(uint32_t base, uint32_t fin, uint32_t fout,
				    uint32_t *best_d, uint32_t *best_m,
				    uint32_t *best_dout)
{
	uint32_t fpfd_min = 10000;
	uint32_t fpfd_max = 300000;
	uint32_t fvco_min = 600000;
	uint32_t fvco_max = 1200000;
	uint32_t d, d_min, d_max, _d_min, _d_max;
	uint32_t m, m_min, m_max;
	uint32_t dout, fvco;
	int32_t f, best_f;

	if (FPGA_CLKGEN_VERSION_MAJOR(fpga_clkgen_rd(base,
				      FPGA_CLKGEN_REG_VERSION)) > 0x04)
		fpga_clkgen_setup_ranges(base, &fpfd_max, &fvco_min, &fvco_max);

	fin /= 1000;
	fout /= 1000;
	/* Below 1 kHz the kHz-scaled maths divides by zero: no solution. */
	*best_d = 0;
	if (!fin || !fout)
		return;

	best_f = 0x7fffffff;
	*best_d = 0;
	*best_m = 0;
	*best_dout = 0;

	d_min = (fin + fpfd_max - 1) / fpfd_max;
	if (d_min < 1)
		d_min = 1;
	d_max = fin / fpfd_min;
	if (d_max > 80)
		d_max = 80;

	m_min = ((fvco_min + fin - 1) / fin) * d_min;
	if (m_min < 1)
		m_min = 1;
	m_max = fvco_max * d_max / fin;
	if (m_max > 64)
		m_max = 64;

	for (m = m_min; m <= m_max; m++) {
		_d_min = (fin * m + fvco_max - 1) / fvco_max;
		if (_d_min < d_min)
			_d_min = d_min;
		_d_max = fin * m / fvco_min;
		if (_d_max > d_max)
			_d_max = d_max;

		for (d = _d_min; d <= _d_max; d++) {
			fvco = fin * m / d;
			dout = (fvco + fout / 2) / fout;
			if (dout < 1)
				dout = 1;
			if (dout > 128)
				dout = 128;
			f = fvco / dout;
			if (abs(f - (int32_t)fout) < abs(best_f - (int32_t)fout)) {
				best_f = f;
				*best_d = d;
				*best_m = m;
				*best_dout = dout;
				if (best_f == (int32_t)fout)
					return;
			}
		}
	}
}

/**
 * @brief Split a divider into MMCM high/low/edge/nocount fields.
 * @param divider - Divider value.
 * @param low - Output: low time.
 * @param high - Output: high time.
 * @param edge - Output: edge bit.
 * @param nocount - Output: nocount bit.
 */
static void fpga_clkgen_calc_clk_params(uint32_t divider, uint32_t *low,
					uint32_t *high, uint32_t *edge,
					uint32_t *nocount)
{
	*nocount = (divider == 1) ? 1 : 0;
	*high = divider / 2;
	*edge = divider % 2;
	*low = divider - *high;
}

/**
 * @brief Hold or release the MMCM reset; core RESETN stays released.
 * @param base - CLKGEN base address.
 * @param enable - true releases the MMCM.
 */
static void fpga_clkgen_mmcm_enable(uint32_t base, bool enable)
{
	uint32_t val = FPGA_CLKGEN_RESETN;

	if (enable)
		val |= FPGA_CLKGEN_MMCM_RESETN;

	fpga_clkgen_wr(base, FPGA_CLKGEN_REG_RESETN, val);
}

/**
 * @brief Poll STATUS for MMCM lock in 1 ms steps.
 * @param base - CLKGEN base address.
 * @return 0 when locked, -ETIMEDOUT otherwise.
 */
static int fpga_clkgen_wait_lock(uint32_t base)
{
	uint32_t timeout = FPGA_CLKGEN_LOCK_TIMEOUT_MS;
	uint32_t reg_val;

	do {
		capi_wait_ms(1);
		reg_val = fpga_clkgen_rd(base, FPGA_CLKGEN_REG_STATUS);
	} while (!(reg_val & FPGA_CLKGEN_STATUS) && --timeout);

	return (reg_val & FPGA_CLKGEN_STATUS) ? 0 : -ETIMEDOUT;
}

/* --- static prototypes ---------------------------------------------------- */

static int fpga_capi_clkgen_init(struct capi_clk_handle **handle,
				 const struct capi_clk_config *config);
static int fpga_capi_clkgen_deinit(struct capi_clk_handle *handle);
static int fpga_capi_clkgen_enable(struct capi_clk_handle *handle);
static int fpga_capi_clkgen_disable(struct capi_clk_handle *handle);
static int fpga_capi_clkgen_get_rate(struct capi_clk_handle *handle,
				     uint64_t *rate);
static int fpga_capi_clkgen_round_rate(struct capi_clk_handle *handle,
				       uint64_t rate, uint64_t *rounded_rate);
static int fpga_capi_clkgen_set_rate(struct capi_clk_handle *handle,
				     uint64_t rate);

const struct capi_clk_ops fpga_capi_clkgen_ops = {
	.init = fpga_capi_clkgen_init,
	.deinit = fpga_capi_clkgen_deinit,
	.enable = fpga_capi_clkgen_enable,
	.disable = fpga_capi_clkgen_disable,
	.get_rate = fpga_capi_clkgen_get_rate,
	.round_rate = fpga_capi_clkgen_round_rate,
	.set_rate = fpga_capi_clkgen_set_rate,
};

/* --- ops ------------------------------------------------------------------ */

/**
 * @brief Initialize the CAPI backend instance.
 * @note CLKGEN: stores base and parent rate from config->priv; no register
 *       access.
 * @param handle - Pointer to the CAPI handle; allocated if *handle is NULL.
 * @param config - Pointer to the CAPI configuration.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_clkgen_init(struct capi_clk_handle **handle,
				 const struct capi_clk_config *config)
{
	struct capi_clk_handle *h;
	struct fpga_clkgen_priv *priv;
	const struct fpga_clkgen_config *extra;
	bool alloc;

	if (handle == NULL || config == NULL)
		return -EINVAL;

	extra = config->priv;
	if (extra == NULL || extra->base == 0U)
		return -EINVAL;

	alloc = (*handle == NULL);
	if (alloc) {
		h = capi_calloc(1, sizeof(*h));
		if (!h)
			return -ENOMEM;
		priv = capi_calloc(1, sizeof(*priv));
		if (!priv) {
			capi_free(h);
			return -ENOMEM;
		}
		h->priv = priv;
	} else {
		h = *handle;
		if (!h->priv)
			return -EINVAL;
		/* Re-init of a live handle is a caller bug, not a reset. */
		if (h->ops)
			return -EBUSY;
		priv = h->priv;
	}

	h->init_allocated = alloc;
	priv->base = extra->base;
	priv->parent_rate_hz = extra->parent_rate_hz;
	h->name = config->name;

	h->ops = config->ops ? config->ops : &fpga_capi_clkgen_ops;

	*handle = h;

	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note CLKGEN: frees the handle only; the clock keeps running.
 * @param handle - Pointer to the CAPI handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_clkgen_deinit(struct capi_clk_handle *handle)
{
	if (!handle)
		return -EINVAL;

	if (handle->init_allocated) {
		capi_free(handle->priv);
		capi_free(handle);
	} else {
		/* Static handle: later calls (and a second deinit) get -EINVAL. */
		handle->ops = NULL;
	}

	return 0;
}

/**
 * @brief Enable the clock output.
 * @note CLKGEN: releases the MMCM reset and polls STATUS for lock in 1 ms steps
 *       (100 ms max).
 * @param handle - Pointer to the CAPI handle.
 * @return 0 on success, -ETIMEDOUT if the MMCM never locks, negative errno
 *         otherwise.
 */
static int fpga_capi_clkgen_enable(struct capi_clk_handle *handle)
{
	struct fpga_clkgen_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	fpga_clkgen_mmcm_enable(priv->base, true);

	return fpga_clkgen_wait_lock(priv->base);
}

/**
 * @brief Disable the clock output.
 * @note CLKGEN: holds the MMCM in reset, core RESETN stays released.
 * @param handle - Pointer to the CAPI handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_clkgen_disable(struct capi_clk_handle *handle)
{
	struct fpga_clkgen_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	fpga_clkgen_mmcm_enable(priv->base, false);

	return 0;
}

/**
 * @brief Get the output clock rate.
 * @note CLKGEN: reads CLKOUT0/CLK_DIV/CLK_FB1 back over DRP and computes
 *       parent / D * M / DOUT.
 * @param handle - Pointer to the CAPI handle.
 * @param rate - Output: clock rate in Hz (0 if the dividers read back zero).
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_clkgen_get_rate(struct capi_clk_handle *handle,
				     uint64_t *rate)
{
	struct fpga_clkgen_priv *priv;
	uint32_t d, m, dout, reg;
	int ret;

	if (!handle || !handle->priv || !rate)
		return -EINVAL;

	priv = handle->priv;

	/*
	 * A divider with its NO_COUNT bit set is bypassed (divide by 1) whatever
	 * HIGH/LOW hold; the XSA default D=1 is encoded that way. Same decode as
	 * Linux clk-axi-clkgen (the legacy no-OS driver ignores NO_COUNT).
	 */
	ret = fpga_clkgen_mmcm_read(priv->base, FPGA_CLKGEN_MMCM_REG_CLKOUT0_1, &reg);
	if (ret)
		return ret;
	dout = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	ret = fpga_clkgen_mmcm_read(priv->base, FPGA_CLKGEN_MMCM_REG_CLKOUT0_2, &reg);
	if (ret)
		return ret;
	if (reg & (1U << 6))
		dout = 1;
	ret = fpga_clkgen_mmcm_read(priv->base, FPGA_CLKGEN_MMCM_REG_CLK_DIV, &reg);
	if (ret)
		return ret;
	d = (reg & (1U << 12)) ? 1 : (reg & 0x3f) + ((reg >> 6) & 0x3f);
	ret = fpga_clkgen_mmcm_read(priv->base, FPGA_CLKGEN_MMCM_REG_CLK_FB1, &reg);
	if (ret)
		return ret;
	m = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	ret = fpga_clkgen_mmcm_read(priv->base, FPGA_CLKGEN_MMCM_REG_CLK_FB2, &reg);
	if (ret)
		return ret;
	if (reg & (1U << 6))
		m = 1;

	if (d == 0 || dout == 0) {
		*rate = 0;
		return 0;
	}

	*rate = (uint64_t)(priv->parent_rate_hz / d) * m / dout;

	return 0;
}

/**
 * @brief Round a requested rate to the closest achievable one.
 * @note CLKGEN: runs the D/M/DOUT divider search; no register
 *       writes.
 * @param handle - Pointer to the CAPI handle.
 * @param rate - Requested rate in Hz.
 * @param rounded_rate - Output: achievable rate in Hz.
 * @return 0 on success, -EINVAL if no divider set fits.
 */
static int fpga_capi_clkgen_round_rate(struct capi_clk_handle *handle,
				       uint64_t rate, uint64_t *rounded_rate)
{
	struct fpga_clkgen_priv *priv;
	uint32_t d, m, dout;

	if (!handle || !handle->priv || !rounded_rate || rate == 0 ||
	    rate > UINT32_MAX)
		return -EINVAL;

	priv = handle->priv;
	if (priv->parent_rate_hz == 0)
		return -EINVAL;

	fpga_clkgen_calc_params(priv->base, priv->parent_rate_hz, (uint32_t)rate,
				&d, &m, &dout);
	if (d == 0 || m == 0 || dout == 0)
		return -EINVAL;

	*rounded_rate = (uint64_t)(priv->parent_rate_hz / d) * m / dout;

	return 0;
}

/**
 * @brief Set the output clock rate.
 * @note CLKGEN: divider search, MMCM held in reset,
 *       CLKOUT0/CLKOUT1/DIV/FB/LOCK/FILTER programmed over DRP, MMCM released,
 *       lock polled up to 100 ms.
 * @param handle - Pointer to the CAPI handle.
 * @param rate - Requested rate in Hz.
 * @note axi_clkgen_set_rate: rate 0, parent 0 or no divider set returns 0
 *       untouched; a DRP access that times out is skipped (the driver
 *       returns from mmcm_read/write silently) and the lock decides.
 * @return 0 on success (or nothing to do), -EINVAL on a bad handle or a rate
 *         above 32 bit, -ETIMEDOUT if the MMCM never locks.
 */
static int fpga_capi_clkgen_set_rate(struct capi_clk_handle *handle,
				     uint64_t rate)
{
	struct fpga_clkgen_priv *priv;
	uint32_t d, m, dout, nocount, high, edge, low, filter, lock, base;

	if (!handle || !handle->priv || rate > UINT32_MAX)
		return -EINVAL;

	priv = handle->priv;
	base = priv->base;
	if (priv->parent_rate_hz == 0 || rate == 0)
		return 0;

	fpga_clkgen_calc_params(base, priv->parent_rate_hz, (uint32_t)rate,
				&d, &m, &dout);
	if (d == 0 || dout == 0 || m == 0)
		return 0;

	filter = fpga_clkgen_lookup_filter(m - 1);
	lock = fpga_clkgen_lookup_lock(m - 1);

	fpga_clkgen_mmcm_enable(base, false);

	/* A timed-out DRP write is skipped, as axi_clkgen_mmcm_write. */
	fpga_clkgen_calc_clk_params(dout, &low, &high, &edge, &nocount);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_CLKOUT0_1,
				     (high << 6) | low, 0xefff);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_CLKOUT0_2,
				     (edge << 7) | (nocount << 6), 0x03ff);

	dout *= 4;
	fpga_clkgen_calc_clk_params(dout, &low, &high, &edge, &nocount);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_CLKOUT1_1,
				     (high << 6) | low, 0xefff);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_CLKOUT1_2,
				     (edge << 7) | (nocount << 6), 0x03ff);

	fpga_clkgen_calc_clk_params(d, &low, &high, &edge, &nocount);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_CLK_DIV,
				     (edge << 13) | (nocount << 12) | (high << 6) | low,
				     0x3fff);

	fpga_clkgen_calc_clk_params(m, &low, &high, &edge, &nocount);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_CLK_FB1,
				     (high << 6) | low, 0xefff);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_CLK_FB2,
				     (edge << 7) | (nocount << 6), 0x03ff);

	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_LOCK1,
				     lock & 0x3ff, 0x3ff);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_LOCK2,
				     (((lock >> 16) & 0x1f) << 10) | 0x1, 0x7fff);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_LOCK3,
				     (((lock >> 24) & 0x1f) << 10) | 0x3e9, 0x7fff);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_FILTER1,
				     filter >> 16, 0x9900);
	(void)fpga_clkgen_mmcm_write(base, FPGA_CLKGEN_MMCM_REG_FILTER2,
				     filter, 0x9900);

	fpga_clkgen_mmcm_enable(base, true);

	return fpga_clkgen_wait_lock(base);
}

#ifdef __cplusplus
}
#endif
