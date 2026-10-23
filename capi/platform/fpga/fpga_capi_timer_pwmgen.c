/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Implementation of the AXI PWM generator CAPI timer backend.
 *
 * The axi_pwm_gen core has one RESET bit for the whole generator and, per
 * channel, period, duty and offset registers, all counted in ticks of the
 * core's ext_clk. Writes to these registers are staged: the channels switch
 * to the new values together when LOAD_CONFIG is written, so a running
 * output never shows a half-updated period/duty pair. Two register layouts
 * exist, selected from CORE_VERSION: v1 (4 interleaved channels) and v2
 * (16 banked channels).
 *
 * CAPI timer operations map to:
 *   - init: confirm an axi_pwm_gen sits at the base address: the CORE_MAGIC
 *     ID register must read 0x601A3471 ("PLSG", fixed in the HDL), and a
 *     value written to the SCRATCHPAD register (free read/write test
 *     register) must read back. Then read CORE_VERSION and the channel
 *     count, and clear RESET.
 *   - deinit: set RESET.
 *   - start / stop: clear / set RESET (all channels).
 *   - channel_init: check the channel exists.
 *   - channel_config: write period, duty and offset, converted from ns to
 *     ext_clk ticks (rounded up).
 *   - channel_enable: write the period, then LOAD_CONFIG.
 *   - channel_disable: write period 0, then LOAD_CONFIG.
 *
 * See fpga_capi_timer_pwmgen.h for configuration.
 */

#include "fpga_capi_timer_pwmgen_priv.h"
#include "capi_alloc.h"
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include "capi_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Write a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @param val - Value to write.
 */
static inline void fpga_pwmgen_wr(uint32_t base, uint32_t reg, uint32_t val)
{
	CAPI_PLATFORM_IO_WRITE32(base + reg, val);
}

/**
 * @brief Read a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @return Register value.
 */
static inline uint32_t fpga_pwmgen_rd(uint32_t base, uint32_t reg)
{
	return CAPI_PLATFORM_IO_READ32(base + reg);
}

/**
 * @brief Read-modify-write a core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @param mask - Bits to update.
 * @param data - New value of the masked bits.
 */
static void fpga_pwmgen_write_mask(uint32_t base, uint32_t reg, uint32_t mask,
				   uint32_t data)
{
	uint32_t temp = fpga_pwmgen_rd(base, reg);

	fpga_pwmgen_wr(base, reg, (temp & ~mask) | (data & mask));
}

/**
 * @brief Convert nanoseconds to ext_clk ticks, rounded up.
 * @param priv - Pointer to the backend private handle.
 * @param ns - Duration in nanoseconds.
 * @param ticks - Output: ext_clk ticks.
 * @return 0 on success, -EINVAL if the result does not fit a 32-bit register.
 */
static int fpga_pwmgen_ns_to_ticks(const struct fpga_timer_pwmgen_priv_handle
				   *priv, uint64_t ns, uint32_t *ticks)
{
	uint64_t khz = priv->input_clock_hz / FPGA_PWMGEN_NSEC_PER_USEC;
	uint64_t tmp;

	/* Downscale by 1000 in order to avoid overflow when multiplying */
	if (khz && ns > UINT64_MAX / khz)
		return -EINVAL;
	tmp = khz * ns;
	tmp = (tmp + FPGA_PWMGEN_USEC_PER_SEC - 1ULL) / FPGA_PWMGEN_USEC_PER_SEC;
	if (tmp > UINT32_MAX)
		return -EINVAL;

	*ticks = (uint32_t)tmp;

	return 0;
}

/**
 * @brief Validate handle and channel and return the private handle.
 * @param handle - Pointer to the CAPI handle.
 * @param chan - Channel index.
 * @return Private handle, or NULL if handle/channel are invalid or the channel
 *         was not claimed with channel_init().
 */
static struct fpga_timer_pwmgen_priv_handle *
fpga_pwmgen_chan_priv(struct capi_timer_handle *handle, uint32_t chan)
{
	struct fpga_timer_pwmgen_priv_handle *priv;

	if (!handle || !handle->priv)
		return NULL;

	priv = handle->priv;
	if (chan >= priv->num_channels || !priv->chan[chan].initialized)
		return NULL;

	return priv;
}

/* --- static prototypes ---------------------------------------------------- */

static int fpga_capi_timer_pwmgen_init(struct capi_timer_handle **handle,
				       const struct capi_timer_config *config);
static int fpga_capi_timer_pwmgen_deinit(struct capi_timer_handle *handle);
static int fpga_capi_timer_pwmgen_start(struct capi_timer_handle *handle);
static int fpga_capi_timer_pwmgen_stop(struct capi_timer_handle *handle);
static int fpga_capi_timer_pwmgen_channel_init(struct capi_timer_handle *handle,
		uint32_t chan);
static int fpga_capi_timer_pwmgen_channel_deinit(struct capi_timer_handle
		*handle, uint32_t chan);
static int fpga_capi_timer_pwmgen_channel_config(struct capi_timer_handle
		*handle, uint32_t chan,
		const struct capi_timer_channel_config *ch_config);
static int fpga_capi_timer_pwmgen_channel_enable(struct capi_timer_handle
		*handle, uint32_t chan);
static int fpga_capi_timer_pwmgen_channel_disable(struct capi_timer_handle
		*handle, uint32_t chan);
static int fpga_capi_timer_pwmgen_nsec_to_ticks(const struct capi_timer_handle
		*handle, uint64_t duration_ns, uint32_t *ticks);
static int fpga_capi_timer_pwmgen_ticks_to_nsec(const struct capi_timer_handle
		*handle, uint64_t ticks, uint32_t *duration_ns);

const struct capi_timer_ops fpga_capi_timer_pwmgen_ops = {
	.init = fpga_capi_timer_pwmgen_init,
	.deinit = fpga_capi_timer_pwmgen_deinit,
	.start = fpga_capi_timer_pwmgen_start,
	.stop = fpga_capi_timer_pwmgen_stop,
	.channel_init = fpga_capi_timer_pwmgen_channel_init,
	.channel_deinit = fpga_capi_timer_pwmgen_channel_deinit,
	.channel_config = fpga_capi_timer_pwmgen_channel_config,
	.channel_enable = fpga_capi_timer_pwmgen_channel_enable,
	.channel_disable = fpga_capi_timer_pwmgen_channel_disable,
	.nsec_to_ticks = fpga_capi_timer_pwmgen_nsec_to_ticks,
	.ticks_to_nsec = fpga_capi_timer_pwmgen_ticks_to_nsec,
	/* No free-running counter, compare/capture or interrupt on this core. */
};

/* --- ops ------------------------------------------------------------------ */

/**
 * @brief Initialize the CAPI backend instance.
 * @note PWMGEN: axi_pwm_init core part -- reads NPWM, clears RESET (RMW),
 *       writes SCRATCHPAD, reads CORE_VERSION, reads SCRATCHPAD back. Also
 *       checks CORE_MAGIC first. Channels are set up by channel_init/config.
 * @param handle - Pointer to the CAPI handle; allocated if *handle is NULL.
 * @param config - Pointer to the CAPI configuration (identifier = base,
 *                 input_clock_hz = ext_clk rate, at least 1 kHz).
 * @return 0 on success, or:
 *         -ENODEV   bad magic, no channels or dead scratchpad
 *         -ENOTSUP  major version without a known channel layout
 *         -ENOMEM   allocation failed
 *         -EINVAL   bad arguments
 */
static int fpga_capi_timer_pwmgen_init(struct capi_timer_handle **handle,
				       const struct capi_timer_config *config)
{
	struct fpga_timer_pwmgen_priv_handle *priv;
	struct capi_timer_handle *h;
	uint32_t base, data;
	bool alloc;
	int ret;

	/* input_clock_hz is the generator's ext_clk: ns <-> ticks needs it. */
	if (!handle || !config || config->identifier == 0U ||
	    config->identifier > UINT32_MAX ||
	    config->input_clock_hz < FPGA_PWMGEN_NSEC_PER_USEC)
		return -EINVAL;

	base = (uint32_t)config->identifier;

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
		priv = h->priv;
		*priv = (struct fpga_timer_pwmgen_priv_handle) {
			0
		};
	}

	priv->base = base;
	priv->input_clock_hz = config->input_clock_hz;

	ret = -ENODEV;
	if (fpga_pwmgen_rd(base, FPGA_PWMGEN_REG_CORE_MAGIC) !=
	    FPGA_PWMGEN_CORE_MAGIC)
		goto error;

	data = fpga_pwmgen_rd(base, FPGA_PWMGEN_REG_NPWM);
	if (data == 0U)
		goto error;
	priv->num_channels = data;

	/* Enable the core */
	fpga_pwmgen_write_mask(base, FPGA_PWMGEN_REG_CONFIG, FPGA_PWMGEN_RESET, 0);

	fpga_pwmgen_wr(base, FPGA_PWMGEN_REG_SCRATCHPAD, FPGA_PWMGEN_TEST_DATA);

	/* Get Hardware version */
	data = fpga_pwmgen_rd(base, FPGA_PWMGEN_REG_CORE_VERSION);
	priv->hw_major_ver = (uint8_t)FPGA_PWMGEN_VERSION_MAJOR(data);
	if (priv->hw_major_ver != 1U && priv->hw_major_ver != 2U) {
		ret = -ENOTSUP;
		goto error;
	}

	if (priv->num_channels > FPGA_PWMGEN_MAX_CHANNELS(priv))
		priv->num_channels = FPGA_PWMGEN_MAX_CHANNELS(priv);

	if (fpga_pwmgen_rd(base, FPGA_PWMGEN_REG_SCRATCHPAD) !=
	    FPGA_PWMGEN_TEST_DATA)
		goto error;

	h->init_allocated = alloc;
	h->ops = config->ops ? config->ops : &fpga_capi_timer_pwmgen_ops;
	*handle = h;

	return 0;

error:
	if (alloc) {
		capi_free(priv);
		capi_free(h);
	}

	return ret;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note PWMGEN: axi_pwm_remove -- writes CONFIG = RESET, stopping all channels.
 * @param handle - Pointer to the CAPI handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_timer_pwmgen_deinit(struct capi_timer_handle *handle)
{
	struct fpga_timer_pwmgen_priv_handle *priv;

	if (!handle)
		return -EINVAL;

	priv = handle->priv;
	if (priv && priv->base)
		fpga_pwmgen_wr(priv->base, FPGA_PWMGEN_REG_CONFIG, FPGA_PWMGEN_RESET);

	if (handle->init_allocated) {
		capi_free(priv);
		capi_free(handle);
	}

	return 0;
}

/**
 * @brief Start the generator.
 * @note PWMGEN: clears the core RESET bit (RMW), as axi_pwm_init does.
 * @param handle - Pointer to the CAPI handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_timer_pwmgen_start(struct capi_timer_handle *handle)
{
	struct fpga_timer_pwmgen_priv_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	fpga_pwmgen_write_mask(priv->base, FPGA_PWMGEN_REG_CONFIG,
			       FPGA_PWMGEN_RESET, 0);

	return 0;
}

/**
 * @brief Stop the generator.
 * @note PWMGEN: sets the core RESET bit (RMW), halting every channel.
 * @param handle - Pointer to the CAPI handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_timer_pwmgen_stop(struct capi_timer_handle *handle)
{
	struct fpga_timer_pwmgen_priv_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	fpga_pwmgen_write_mask(priv->base, FPGA_PWMGEN_REG_CONFIG,
			       FPGA_PWMGEN_RESET, FPGA_PWMGEN_RESET);

	return 0;
}

/**
 * @brief Claim a generator channel.
 * @note PWMGEN: the axi_pwm_init range checks (NPWM and the layout maximum).
 * @param handle - Pointer to the CAPI handle.
 * @param chan - Channel index.
 * @return 0 on success, -EINVAL if the channel does not exist.
 */
static int fpga_capi_timer_pwmgen_channel_init(struct capi_timer_handle *handle,
		uint32_t chan)
{
	struct fpga_timer_pwmgen_priv_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	if (chan >= priv->num_channels)
		return -EINVAL;

	priv->chan[chan] = (struct fpga_pwmgen_chan) {
		.initialized = true
	};

	return 0;
}

/**
 * @brief Release a generator channel.
 * @note PWMGEN: disables the channel output, then forgets it.
 * @param handle - Pointer to the CAPI handle.
 * @param chan - Channel index.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_timer_pwmgen_channel_deinit(struct capi_timer_handle
		*handle, uint32_t chan)
{
	struct fpga_timer_pwmgen_priv_handle *priv;
	int ret;

	ret = fpga_capi_timer_pwmgen_channel_disable(handle, chan);
	if (ret)
		return ret;

	priv = handle->priv;
	priv->chan[chan].initialized = false;

	return 0;
}

/**
 * @brief Configure a channel in PWM mode.
 * @note PWMGEN: axi_pwm_set_period / set_duty_cycle / set_phase -- writes
 *       CHX_PERIOD (0 while disabled), CHX_DUTY and CHX_PHASE; LOAD_CONFIG
 *       latches them at once on a running channel, else at channel_enable.
 * @param handle - Pointer to the CAPI handle.
 * @param chan - Channel index.
 * @param ch_config - Channel configuration; mode must be CAPI_TIMER_PWM_MODE.
 * @return 0 on success, or:
 *         -ENOTSUP  compare/capture mode or inverted polarity
 *         -EINVAL   bad channel, zero period, active longer than period, or a
 *                   value that does not fit the 32-bit tick registers
 */
static int fpga_capi_timer_pwmgen_channel_config(struct capi_timer_handle
		*handle, uint32_t chan,
		const struct capi_timer_channel_config *ch_config)
{
	struct fpga_timer_pwmgen_priv_handle *priv;
	const struct capi_timer_pwm_config *pwm;
	uint32_t period, duty, phase;

	priv = fpga_pwmgen_chan_priv(handle, chan);
	if (!priv || !ch_config)
		return -EINVAL;

	if (ch_config->mode != CAPI_TIMER_PWM_MODE)
		return -ENOTSUP;

	pwm = &ch_config->config.pwm;
	/* The generator emits an active-high pulse; it cannot invert. */
	if (pwm->inverted_polarity)
		return -ENOTSUP;
	if (pwm->period_ns == 0U || pwm->active_ns > pwm->period_ns)
		return -EINVAL;

	if (fpga_pwmgen_ns_to_ticks(priv, pwm->period_ns, &period) ||
	    fpga_pwmgen_ns_to_ticks(priv, pwm->active_ns, &duty) ||
	    fpga_pwmgen_ns_to_ticks(priv, pwm->offset_ns, &phase) ||
	    period == 0U)
		return -EINVAL;

	priv->chan[chan].period_ticks = period;
	fpga_pwmgen_wr(priv->base, FPGA_PWMGEN_CHX_PERIOD(priv, chan),
		       priv->chan[chan].enabled ? period : 0U);
	fpga_pwmgen_wr(priv->base, FPGA_PWMGEN_CHX_DUTY(priv, chan), duty);
	fpga_pwmgen_wr(priv->base, FPGA_PWMGEN_CHX_PHASE(priv, chan), phase);
	/* A running channel takes the new values now; otherwise at enable. */
	if (priv->chan[chan].enabled)
		fpga_pwmgen_wr(priv->base, FPGA_PWMGEN_REG_CONFIG,
			       FPGA_PWMGEN_LOAD_CONFIG);

	return 0;
}

/**
 * @brief Start the channel output.
 * @note PWMGEN: axi_pwm_enable -- writes CHX_PERIOD, then LOAD_CONFIG.
 * @param handle - Pointer to the CAPI handle.
 * @param chan - Channel index.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_timer_pwmgen_channel_enable(struct capi_timer_handle
		*handle, uint32_t chan)
{
	struct fpga_timer_pwmgen_priv_handle *priv;

	priv = fpga_pwmgen_chan_priv(handle, chan);
	if (!priv)
		return -EINVAL;

	fpga_pwmgen_wr(priv->base, FPGA_PWMGEN_CHX_PERIOD(priv, chan),
		       priv->chan[chan].period_ticks);
	fpga_pwmgen_wr(priv->base, FPGA_PWMGEN_REG_CONFIG,
		       FPGA_PWMGEN_LOAD_CONFIG);
	priv->chan[chan].enabled = true;

	return 0;
}

/**
 * @brief Stop the channel output.
 * @note PWMGEN: axi_pwm_disable -- writes CHX_PERIOD 0, then LOAD_CONFIG.
 * @param handle - Pointer to the CAPI handle.
 * @param chan - Channel index.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_timer_pwmgen_channel_disable(struct capi_timer_handle
		*handle, uint32_t chan)
{
	struct fpga_timer_pwmgen_priv_handle *priv;

	priv = fpga_pwmgen_chan_priv(handle, chan);
	if (!priv)
		return -EINVAL;

	fpga_pwmgen_wr(priv->base, FPGA_PWMGEN_CHX_PERIOD(priv, chan),
		       FPGA_PWMGEN_CHANNEL_DISABLE);
	fpga_pwmgen_wr(priv->base, FPGA_PWMGEN_REG_CONFIG,
		       FPGA_PWMGEN_LOAD_CONFIG);
	priv->chan[chan].enabled = false;

	return 0;
}

/**
 * @brief Convert a duration to generator ticks.
 * @note PWMGEN: ns -> ext_clk ticks, rounded up.
 * @param handle - Pointer to the CAPI handle.
 * @param duration_ns - Duration in nanoseconds.
 * @param ticks - Output: ext_clk ticks.
 * @return 0 on success, -EINVAL on bad arguments or a result above 32 bit.
 */
static int fpga_capi_timer_pwmgen_nsec_to_ticks(const struct capi_timer_handle
		*handle, uint64_t duration_ns, uint32_t *ticks)
{
	if (!handle || !handle->priv || !ticks)
		return -EINVAL;

	return fpga_pwmgen_ns_to_ticks(handle->priv, duration_ns, ticks);
}

/**
 * @brief Convert generator ticks to a duration.
 * @note PWMGEN: ticks * 1e9 / input_clock_hz.
 * @param handle - Pointer to the CAPI handle.
 * @param ticks - ext_clk ticks.
 * @param duration_ns - Output: duration in nanoseconds.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_timer_pwmgen_ticks_to_nsec(const struct capi_timer_handle
		*handle, uint64_t ticks, uint32_t *duration_ns)
{
	const struct fpga_timer_pwmgen_priv_handle *priv;
	uint64_t ns;

	if (!handle || !handle->priv || !duration_ns)
		return -EINVAL;

	priv = handle->priv;
	if (ticks > UINT64_MAX / (FPGA_PWMGEN_NSEC_PER_USEC *
				  FPGA_PWMGEN_USEC_PER_SEC))
		return -EINVAL;
	ns = ticks * FPGA_PWMGEN_NSEC_PER_USEC * FPGA_PWMGEN_USEC_PER_SEC /
	     priv->input_clock_hz;
	if (ns > UINT32_MAX)
		return -EINVAL;
	*duration_ns = (uint32_t)ns;

	return 0;
}

#ifdef __cplusplus
}
#endif
