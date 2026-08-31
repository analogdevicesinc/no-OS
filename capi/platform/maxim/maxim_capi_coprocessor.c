/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_coprocessor.c
 * @brief CAPI coprocessor driver for the MAX78000 RV32 core (CPU1).
 *
 * The RV32 core is controlled through three places in the register map:
 *
 *   - MXC_FCR->urvbootaddr        the reset vector the core fetches from
 *   - MXC_GCR->pclkdis1.CPU1      the core's only clock gate
 *   - MXC_GCR->rst1.CPU1          a self-clearing reset pulse that starts it
 *
 * MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CPU1) and clearing
 * MXC_F_GCR_PCLKDIS1_CPU1 are the same bit, so there is exactly one gate and no
 * separate held-in-reset state: rst1 is a pulse, not a level. The mode mapping
 * below is built from what the hardware actually offers, and the requested mode
 * is shadowed in the private data so that a mode the bits cannot distinguish on
 * their own still reads back faithfully.
 */

#include <errno.h>
#include <stdint.h>
#include "capi_alloc.h"
#include "maxim_capi_coprocessor_priv.h"
#include "max78000.h"
#include "mxc_sys.h"
#include "icc.h"
#include "gcr_regs.h"
#include "fcr_regs.h"
#include "icc_regs.h"

static int maxim_capi_coprocessor_init(struct capi_coprocessor_handle **handle,
				       const struct capi_coprocessor_config *config);
static int maxim_capi_coprocessor_deinit(struct capi_coprocessor_handle
		*handle);
static int maxim_capi_coprocessor_boot(struct capi_coprocessor_handle *handle);
static int maxim_capi_coprocessor_halt(struct capi_coprocessor_handle *handle);
static int maxim_capi_coprocessor_get_boot_caps(struct capi_coprocessor_handle
		*handle, uint32_t *caps);
static int maxim_capi_coprocessor_set_boot_config(struct capi_coprocessor_handle
		*handle, const struct capi_coprocessor_boot_config *config);
static int maxim_capi_coprocessor_get_boot_config(struct capi_coprocessor_handle
		*handle, struct capi_coprocessor_boot_config *config);
static int maxim_capi_coprocessor_set_mode(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_mode mode);
static int maxim_capi_coprocessor_get_mode(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_mode *mode);
static int maxim_capi_coprocessor_resource_get_count(struct
		capi_coprocessor_handle
		*handle, enum capi_coprocessor_resource_type type, uint32_t *count);
static int maxim_capi_coprocessor_resource_set_enabled(struct
		capi_coprocessor_handle
		*handle, enum capi_coprocessor_resource_type type, uint32_t index,
		bool enable);
static int maxim_capi_coprocessor_resource_get_enabled(struct
		capi_coprocessor_handle
		*handle, enum capi_coprocessor_resource_type type, uint32_t index,
		bool *enabled);

const struct capi_coprocessor_ops maxim_capi_coprocessor_ops = {
	.init = maxim_capi_coprocessor_init,
	.deinit = maxim_capi_coprocessor_deinit,
	.boot = maxim_capi_coprocessor_boot,
	.halt = maxim_capi_coprocessor_halt,
	.get_boot_caps = maxim_capi_coprocessor_get_boot_caps,
	.set_boot_config = maxim_capi_coprocessor_set_boot_config,
	.get_boot_config = maxim_capi_coprocessor_get_boot_config,
	.set_mode = maxim_capi_coprocessor_set_mode,
	.get_mode = maxim_capi_coprocessor_get_mode,
	.resource_get_count = maxim_capi_coprocessor_resource_get_count,
	.resource_set_enabled = maxim_capi_coprocessor_resource_set_enabled,
	.resource_get_enabled = maxim_capi_coprocessor_resource_get_enabled,
};

/** @brief True when the RV32 clock gate is open. */
static bool maxim_capi_coprocessor_clocked(void)
{
	return (MXC_GCR->pclkdis1 & MXC_F_GCR_PCLKDIS1_CPU1) == 0;
}

/** @brief Open or close the RV32 clock gate. */
static void maxim_capi_coprocessor_set_clocked(bool clocked)
{
	if (clocked)
		MXC_GCR->pclkdis1 &= ~MXC_F_GCR_PCLKDIS1_CPU1;
	else
		MXC_GCR->pclkdis1 |= MXC_F_GCR_PCLKDIS1_CPU1;
}

/**
 * @brief Pulse the CPU1 reset so the core restarts from urvbootaddr.
 *
 * MXC_F_GCR_RST1_CPU1 is self-clearing; writing it is what actually starts the
 * core, which is why MXC_SYS_RISCVRun() ends with this write.
 */
static void maxim_capi_coprocessor_reset_pulse(void)
{
	MXC_GCR->rst1 |= MXC_F_GCR_RST1_CPU1;
}

/** @brief Program the reset vector register from a shadowed boot config. */
static void maxim_capi_coprocessor_apply_vector(const struct
		capi_coprocessor_boot_config *config)
{
	uint32_t vector = (uint32_t)config->vector_base;

	/* entry_pc, when given, is the more specific request of the two. */
	if (config->entry_pc)
		vector = (uint32_t)config->entry_pc;

	/*
	 * Establish the MAX78000 CPU1 reset defaults instead of inheriting
	 * URVCTRL from a bootloader or a previously flashed application. This
	 * matches the boot sequence in the existing MAX78000 CAPI driver.
	 */
	MXC_FCR->urvctrl = 0;
	MXC_FCR->urvbootaddr = vector;
}

/**
 * @brief Resolve a resource class to its count and, optionally, apply or read
 *        its enable state.
 *
 * Keeping the mapping in one place is what stops the three resource ops from
 * disagreeing with each other about how many of anything exist.
 *
 * @param[in] type Resource class.
 * @param[in] index Resource index within the class.
 * @param[out] count Receives the number of resources in the class.
 * @param[in] apply When true, write @p enable; when false and @p enabled is
 *                  non-NULL, read the current state instead.
 * @param[in] enable Desired state, used only when @p apply is true.
 * @param[out] enabled Receives the current state, when non-NULL.
 *
 * @return 0 on success, -EINVAL for an unknown class or out-of-range index.
 */
static int maxim_capi_coprocessor_resource(enum capi_coprocessor_resource_type
		type,
		uint32_t index, uint32_t *count, bool apply, bool enable, bool *enabled)
{
	mxc_sys_periph_clock_t clock;

	switch (type) {
	case CAPI_COPROCESSOR_RESOURCE_DATA_LANE:
		/* The semaphore block is the host-to-RV32 data lane. */
		*count = MAXIM_CAPI_COPROCESSOR_NUM_DATA_LANES;
		clock = MXC_SYS_PERIPH_CLOCK_SMPHR;
		break;
	case CAPI_COPROCESSOR_RESOURCE_CACHE:
		/* ICC1 is the RV32's instruction cache. */
		*count = MAXIM_CAPI_COPROCESSOR_NUM_CACHES;
		if (index >= *count)
			return -EINVAL;

		if (apply) {
			if (enable)
				MXC_ICC_Enable(MXC_ICC1);
			else
				MXC_ICC_Disable(MXC_ICC1);
		}
		if (enabled)
			*enabled = (MXC_ICC1->ctrl & MXC_F_ICC_CTRL_EN) != 0;

		return 0;
	case CAPI_COPROCESSOR_RESOURCE_ACCELERATOR:
		*count = MAXIM_CAPI_COPROCESSOR_NUM_ACCELERATORS;
		clock = MXC_SYS_PERIPH_CLOCK_CNN;
		break;
	case CAPI_COPROCESSOR_RESOURCE_POWER_DOMAIN:
		*count = MAXIM_CAPI_COPROCESSOR_NUM_POWER_DOMAINS;
		clock = MXC_SYS_PERIPH_CLOCK_CPU1;
		break;
	default:
		return -EINVAL;
	}

	if (index >= *count)
		return -EINVAL;

	if (apply) {
		if (enable)
			MXC_SYS_ClockEnable(clock);
		else
			MXC_SYS_ClockDisable(clock);
	}
	if (enabled)
		*enabled = MXC_SYS_IsClockEnabled(clock) != 0;

	return 0;
}

static int maxim_capi_coprocessor_init(struct capi_coprocessor_handle **handle,
				       const struct capi_coprocessor_config *config)
{
	struct maxim_capi_coprocessor_config *extra;
	struct capi_coprocessor_handle *copro_handle;
	struct maxim_capi_coprocessor_priv *priv;

	if (!handle || !config || !config->ops)
		return -EINVAL;

	if (config->identifier != MAXIM_CAPI_COPROCESSOR_RV32)
		return -EINVAL;

	if (*handle == NULL) {
		copro_handle = capi_calloc(1, sizeof(*copro_handle));
		if (!copro_handle)
			return -ENOMEM;

		priv = capi_calloc(1, sizeof(*priv));
		if (!priv) {
			capi_free(copro_handle);
			return -ENOMEM;
		}

		copro_handle->priv = priv;
		copro_handle->init_allocated = true;
	} else {
		copro_handle = *handle;
		copro_handle->init_allocated = false;

		if (!copro_handle->priv)
			return -EINVAL;

		priv = copro_handle->priv;
	}

	copro_handle->ops = config->ops;

	priv->identifier = config->identifier;
	priv->boot_config.vector_mode = CAPI_COPROCESSOR_VECTOR_ENTRY;
	priv->boot_config.vector_base = MXC_FCR->urvbootaddr;

	if (config->extra) {
		extra = (struct maxim_capi_coprocessor_config *)config->extra;
		priv->options = *extra;
	}

	/*
	 * Leave the core held off rather than starting it behind the caller's
	 * back; capi_coprocessor_boot() is what releases it.
	 */
	maxim_capi_coprocessor_set_clocked(false);
	priv->mode = CAPI_COPROCESSOR_MODE_STANDBY;

	*handle = copro_handle;

	return 0;
}

static int maxim_capi_coprocessor_deinit(struct capi_coprocessor_handle *handle)
{
	if (!handle)
		return -EINVAL;

	maxim_capi_coprocessor_set_clocked(false);

	if (handle->init_allocated) {
		capi_free(handle->priv);
		capi_free(handle);
	}

	return 0;
}

static int maxim_capi_coprocessor_boot(struct capi_coprocessor_handle *handle)
{
	struct maxim_capi_coprocessor_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	/*
	 * Same order as MXC_SYS_RISCVRun(): gate the core, point it at its
	 * vector, ungate, then pulse reset so the fetch starts there.
	 */
	maxim_capi_coprocessor_set_clocked(false);
	maxim_capi_coprocessor_apply_vector(&priv->boot_config);
	maxim_capi_coprocessor_set_clocked(true);
	maxim_capi_coprocessor_reset_pulse();

	priv->mode = CAPI_COPROCESSOR_MODE_RUN;

	return 0;
}

static int maxim_capi_coprocessor_halt(struct capi_coprocessor_handle *handle)
{
	struct maxim_capi_coprocessor_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	maxim_capi_coprocessor_set_clocked(false);
	priv->mode = CAPI_COPROCESSOR_MODE_STANDBY;

	return 0;
}

static int maxim_capi_coprocessor_get_boot_caps(struct capi_coprocessor_handle
		*handle, uint32_t *caps)
{
	if (!handle || !caps)
		return -EINVAL;

	*caps = MAXIM_CAPI_COPROCESSOR_BOOT_CAPS;

	return 0;
}

static int maxim_capi_coprocessor_set_boot_config(struct capi_coprocessor_handle
		*handle, const struct capi_coprocessor_boot_config *config)
{
	struct maxim_capi_coprocessor_priv *priv;
	uint64_t vector;

	if (!handle || !handle->priv || !config)
		return -EINVAL;

	priv = handle->priv;

	/*
	 * Report what this core genuinely cannot do instead of accepting the
	 * request and quietly dropping the fields.
	 */
	if (config->stack_pointer || config->args_valid)
		return -ENOTSUP;

	if (config->vector_mode != CAPI_COPROCESSOR_VECTOR_ENTRY)
		return -ENOTSUP;

	vector = config->entry_pc ? config->entry_pc : config->vector_base;
	if (vector > UINT32_MAX)
		return -EINVAL;

	priv->boot_config = *config;
	maxim_capi_coprocessor_apply_vector(&priv->boot_config);

	return 0;
}

static int maxim_capi_coprocessor_get_boot_config(struct capi_coprocessor_handle
		*handle, struct capi_coprocessor_boot_config *config)
{
	struct maxim_capi_coprocessor_priv *priv;

	if (!handle || !handle->priv || !config)
		return -EINVAL;

	priv = handle->priv;

	*config = priv->boot_config;

	/* Read the vector back from the register rather than the shadow. */
	if (priv->boot_config.entry_pc)
		config->entry_pc = MXC_FCR->urvbootaddr;
	else
		config->vector_base = MXC_FCR->urvbootaddr;

	return 0;
}

static int maxim_capi_coprocessor_set_mode(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_mode mode)
{
	struct maxim_capi_coprocessor_priv *priv;
	bool restart_needed;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	switch (mode) {
	case CAPI_COPROCESSOR_MODE_RUN:
		/*
		 * Coming back from a state that lost the core's execution
		 * context means restarting it at its vector; coming back from
		 * PARKED only means reopening the clock gate.
		 */
		restart_needed = priv->mode == CAPI_COPROCESSOR_MODE_STANDBY ||
				 priv->mode == CAPI_COPROCESSOR_MODE_OFF;

		if (priv->mode == CAPI_COPROCESSOR_MODE_OFF)
			MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CPU1);

		MXC_ICC_Enable(MXC_ICC1);
		maxim_capi_coprocessor_set_clocked(true);
		if (restart_needed) {
			maxim_capi_coprocessor_apply_vector(&priv->boot_config);
			maxim_capi_coprocessor_reset_pulse();
		}
		break;
	case CAPI_COPROCESSOR_MODE_PARKED:
		/* Clock gated, no reset: the core resumes where it stopped. */
		maxim_capi_coprocessor_set_clocked(false);
		break;
	case CAPI_COPROCESSOR_MODE_STANDBY:
		/*
		 * rst1.CPU1 is a pulse, so the core cannot be held in reset.
		 * Gating the clock and remembering that the next RUN must
		 * restart from the vector is the closest equivalent.
		 */
		maxim_capi_coprocessor_set_clocked(false);
		break;
	case CAPI_COPROCESSOR_MODE_LOW_POWER:
		/*
		 * The core keeps executing with its instruction cache gated:
		 * slower, but noticeably less dynamic power.
		 */
		maxim_capi_coprocessor_set_clocked(true);
		MXC_ICC_Disable(MXC_ICC1);
		break;
	case CAPI_COPROCESSOR_MODE_OFF:
		/* Equivalent of MXC_SYS_RISCVShutdown(). */
		MXC_ICC_Disable(MXC_ICC1);
		MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CPU1);
		maxim_capi_coprocessor_set_clocked(false);
		if (!priv->options.keep_accelerator_on)
			MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);
		break;
	default:
		return -EINVAL;
	}

	priv->mode = mode;

	return 0;
}

static int maxim_capi_coprocessor_get_mode(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_mode *mode)
{
	struct maxim_capi_coprocessor_priv *priv;

	if (!handle || !handle->priv || !mode)
		return -EINVAL;

	priv = handle->priv;

	/*
	 * One clock gate cannot express five modes, so the shadow is trusted
	 * only where it agrees with the gate; otherwise the hardware wins.
	 */
	if (maxim_capi_coprocessor_clocked()) {
		if (priv->mode != CAPI_COPROCESSOR_MODE_RUN &&
		    priv->mode != CAPI_COPROCESSOR_MODE_LOW_POWER)
			priv->mode = CAPI_COPROCESSOR_MODE_RUN;
	} else {
		if (priv->mode == CAPI_COPROCESSOR_MODE_RUN ||
		    priv->mode == CAPI_COPROCESSOR_MODE_LOW_POWER)
			priv->mode = CAPI_COPROCESSOR_MODE_PARKED;
	}

	*mode = priv->mode;

	return 0;
}

static int maxim_capi_coprocessor_resource_get_count(struct
		capi_coprocessor_handle
		*handle, enum capi_coprocessor_resource_type type, uint32_t *count)
{
	if (!handle || !count)
		return -EINVAL;

	/* Index 0 always exists here, so it is safe to probe with. */
	return maxim_capi_coprocessor_resource(type, 0, count, false, false, NULL);
}

static int maxim_capi_coprocessor_resource_set_enabled(struct
		capi_coprocessor_handle
		*handle, enum capi_coprocessor_resource_type type, uint32_t index,
		bool enable)
{
	uint32_t count;

	if (!handle)
		return -EINVAL;

	return maxim_capi_coprocessor_resource(type, index, &count, true, enable, NULL);
}

static int maxim_capi_coprocessor_resource_get_enabled(struct
		capi_coprocessor_handle
		*handle, enum capi_coprocessor_resource_type type, uint32_t index,
		bool *enabled)
{
	uint32_t count;

	if (!handle || !enabled)
		return -EINVAL;

	return maxim_capi_coprocessor_resource(type, index, &count, false, false,
					       enabled);
}
