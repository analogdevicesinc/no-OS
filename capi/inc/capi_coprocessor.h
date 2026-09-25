/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL coprocessor lifecycle API.
 */

#ifndef _CAPI_COPROCESSOR_H_
#define _CAPI_COPROCESSOR_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Maximum number of boot argument slots carried by
 *        @ref capi_coprocessor_boot_config.
 */
#define CAPI_COPROCESSOR_BOOT_ARG_MAX 4

/**
 * @brief How the hardware interprets
 *        @ref capi_coprocessor_boot_config.vector_base.
 */
enum capi_coprocessor_vector_mode {
	/** vector_base points at a vector table; the entry address is read from it. */
	CAPI_COPROCESSOR_VECTOR_TABLE,
	/** vector_base is itself the address the coprocessor starts executing at. */
	CAPI_COPROCESSOR_VECTOR_ENTRY
};

/**
 * @brief Boot configuration fields a driver is able to honour.
 *
 * Retrieved with capi_coprocessor_get_boot_caps(). Attempting to program a
 * field that is not advertised here fails with -ENOTSUP rather than being
 * silently discarded, so callers can negotiate instead of guessing.
 */
enum capi_coprocessor_boot_cap {
	/** @ref capi_coprocessor_boot_config.vector_base is supported. */
	CAPI_COPROCESSOR_BOOT_CAP_VECTOR_BASE = (1U << 0),
	/** @ref capi_coprocessor_boot_config.entry_pc is supported. */
	CAPI_COPROCESSOR_BOOT_CAP_ENTRY_PC = (1U << 1),
	/** @ref capi_coprocessor_boot_config.stack_pointer is supported. */
	CAPI_COPROCESSOR_BOOT_CAP_STACK_POINTER = (1U << 2),
	/** @ref capi_coprocessor_boot_config.args is supported. */
	CAPI_COPROCESSOR_BOOT_CAP_ARGS = (1U << 3),
	/** @ref capi_coprocessor_boot_config.vector_mode is selectable. */
	CAPI_COPROCESSOR_BOOT_CAP_VECTOR_MODE = (1U << 4)
};

/**
 * @brief Coprocessor boot arguments.
 *
 * Describes the architectural state the coprocessor comes out of reset with.
 * Which fields are meaningful depends on the hardware; query
 * capi_coprocessor_get_boot_caps() first.
 */
struct capi_coprocessor_boot_config {
	/** Vector table base or reset vector address, per @ref vector_mode. */
	uint64_t vector_base;
	/** Initial program counter. 0 means derive it from @ref vector_base. */
	uint64_t entry_pc;
	/** Initial stack pointer. 0 means use the hardware/driver default. */
	uint64_t stack_pointer;
	/** Values placed in the coprocessor's boot argument registers. */
	uint64_t args[CAPI_COPROCESSOR_BOOT_ARG_MAX];
	/** Bitmask of @ref args entries that carry a meaningful value. */
	uint32_t args_valid;
	/** Interpretation of @ref vector_base. */
	enum capi_coprocessor_vector_mode vector_mode;
	/** Driver-specific boot flags. */
	uint32_t flags;
};

/**
 * @brief Coprocessor activity and power state.
 *
 * These states are scoped to the coprocessor alone. The names deliberately
 * resemble @ref capi_pmu_aon_power_mode in capi_pmu_aon.h, but the two are not
 * interchangeable: capi_pmu_aon.h governs the whole SoC, whereas setting a mode
 * here never changes the state of the host core or of any other power domain.
 */
enum capi_coprocessor_mode {
	/** Clocked, out of reset, and executing. */
	CAPI_COPROCESSOR_MODE_RUN,
	/** Clock gated with state retained; not held in reset. */
	CAPI_COPROCESSOR_MODE_PARKED,
	/** Held in reset but powered, allowing a fast restart. */
	CAPI_COPROCESSOR_MODE_STANDBY,
	/** Reduced-power operation; the coprocessor may still execute. */
	CAPI_COPROCESSOR_MODE_LOW_POWER,
	/** Clock and power removed; state is lost. */
	CAPI_COPROCESSOR_MODE_OFF
};

/**
 * @brief Class of coprocessor sub-block that can be individually enabled.
 *
 * Each type is indexed independently, from 0 to the count reported by
 * capi_coprocessor_resource_get_count() minus one.
 */
enum capi_coprocessor_resource_type {
	/** Data lane / channel feeding or draining the coprocessor. */
	CAPI_COPROCESSOR_RESOURCE_DATA_LANE,
	/** Instruction or data cache. */
	CAPI_COPROCESSOR_RESOURCE_CACHE,
	/** Attached accelerator block. */
	CAPI_COPROCESSOR_RESOURCE_ACCELERATOR,
	/** Separately gated power domain within the coprocessor. */
	CAPI_COPROCESSOR_RESOURCE_POWER_DOMAIN
};

/**
 * @brief Coprocessor configuration.
 */
struct capi_coprocessor_config {
	/** Coprocessor identifier: instance number, MMIO base, or driver-defined id. */
	uint32_t identifier;
	/** Optional driver/platform-specific configuration. */
	void *extra;
	/** Platform-specific ops table. */
	const struct capi_coprocessor_ops *ops;
};

/**
 * @brief Coprocessor handle.
 *
 * Drivers may embed this as the first field of their private handle structure,
 * or attach private state through @ref priv.
 */
struct capi_coprocessor_handle {
	/** Dispatch table copied from config->ops by the driver. */
	const struct capi_coprocessor_ops *ops;
	/** True if the driver allocated this handle. */
	bool init_allocated;
	/** Optional serialization lock used by a thin layer or driver. */
	void *lock;
	/** Driver-private state. */
	void *priv;
};

/**
 * @brief Initialize a coprocessor instance.
 *
 * @param[in,out] handle If *handle is NULL, the driver may allocate a handle;
 *                       otherwise the caller owns the supplied memory.
 * @param[in] config Coprocessor configuration.
 *
 * @return 0 or negative errno.
 */
int capi_coprocessor_init(struct capi_coprocessor_handle **handle,
			  const struct capi_coprocessor_config *config);

/**
 * @brief Deinitialize a coprocessor instance and release driver resources.
 *
 * @return 0 or negative errno.
 */
int capi_coprocessor_deinit(struct capi_coprocessor_handle *handle);

/**
 * @brief Boot or release the coprocessor from reset.
 *
 * @return 0 or negative errno.
 */
int capi_coprocessor_boot(struct capi_coprocessor_handle *handle);

/**
 * @brief Halt or reset the coprocessor.
 *
 * @return 0 or negative errno.
 */
int capi_coprocessor_halt(struct capi_coprocessor_handle *handle);

/**
 * @brief Get the set of boot configuration fields this driver honours.
 *
 * @param[in] handle Coprocessor handle.
 * @param[out] caps Bitmask of @ref capi_coprocessor_boot_cap values.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if the driver does not
 *         expose boot configuration at all.
 */
int capi_coprocessor_get_boot_caps(struct capi_coprocessor_handle *handle,
				   uint32_t *caps);

/**
 * @brief Program the coprocessor's boot arguments.
 *
 * All fields are applied together, so a caller can move the vector, the stack
 * pointer and the argument registers without the coprocessor observing a
 * partially updated configuration.
 *
 * Fields not advertised by capi_coprocessor_get_boot_caps() must be left at 0;
 * setting one fails with -ENOTSUP.
 *
 * @param[in] handle Coprocessor handle.
 * @param[in] config Boot configuration to apply.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if unsupported.
 */
int capi_coprocessor_set_boot_config(struct capi_coprocessor_handle *handle,
				     const struct capi_coprocessor_boot_config *config);

/**
 * @brief Read back the coprocessor's current boot arguments.
 *
 * @param[in] handle Coprocessor handle.
 * @param[out] config Receives the current boot configuration.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if unsupported.
 */
int capi_coprocessor_get_boot_config(struct capi_coprocessor_handle *handle,
				     struct capi_coprocessor_boot_config *config);

/**
 * @brief Set only the vector base address, leaving other boot fields alone.
 *
 * @param[in] handle Coprocessor handle.
 * @param[in] vector_base Vector table base or reset vector address.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if unsupported.
 */
int capi_coprocessor_set_vector_base(struct capi_coprocessor_handle *handle,
				     uint64_t vector_base);

/**
 * @brief Get the currently programmed vector base address.
 *
 * @param[in] handle Coprocessor handle.
 * @param[out] vector_base Receives the vector base address.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if unsupported.
 */
int capi_coprocessor_get_vector_base(struct capi_coprocessor_handle *handle,
				     uint64_t *vector_base);

/**
 * @brief Set only the initial program counter, leaving other boot fields alone.
 *
 * @param[in] handle Coprocessor handle.
 * @param[in] entry_pc Address the coprocessor starts executing at.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if unsupported.
 */
int capi_coprocessor_set_entry_pc(struct capi_coprocessor_handle *handle,
				  uint64_t entry_pc);

/**
 * @brief Get the currently programmed initial program counter.
 *
 * @param[in] handle Coprocessor handle.
 * @param[out] entry_pc Receives the entry address.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if unsupported.
 */
int capi_coprocessor_get_entry_pc(struct capi_coprocessor_handle *handle,
				  uint64_t *entry_pc);

/**
 * @brief Move the coprocessor into the requested mode.
 *
 * Only the coprocessor is affected; see @ref capi_coprocessor_mode for how this
 * differs from the SoC-wide modes in capi_pmu_aon.h.
 *
 * @param[in] handle Coprocessor handle.
 * @param[in] mode Requested mode.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if the driver or the
 *         hardware does not implement the requested mode.
 */
int capi_coprocessor_set_mode(struct capi_coprocessor_handle *handle,
			      enum capi_coprocessor_mode mode);

/**
 * @brief Get the coprocessor's current mode.
 *
 * @param[in] handle Coprocessor handle.
 * @param[out] mode Receives the current mode.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if unsupported.
 */
int capi_coprocessor_get_mode(struct capi_coprocessor_handle *handle,
			      enum capi_coprocessor_mode *mode);

/**
 * @brief Get how many resources of a given type this coprocessor has.
 *
 * A count of 0 means the coprocessor has no resource of that type; this is not
 * an error.
 *
 * @param[in] handle Coprocessor handle.
 * @param[in] type Resource class to query.
 * @param[out] count Receives the number of resources of @p type.
 *
 * @return 0, -EINVAL for invalid arguments, or -ENOTSUP if unsupported.
 */
int capi_coprocessor_resource_get_count(struct capi_coprocessor_handle *handle,
					enum capi_coprocessor_resource_type type, uint32_t *count);

/**
 * @brief Enable or disable one coprocessor resource.
 *
 * Used for things like bringing a data lane up or gating an attached
 * accelerator off without disturbing the coprocessor's mode.
 *
 * @param[in] handle Coprocessor handle.
 * @param[in] type Resource class.
 * @param[in] index Resource index, below the count reported by
 *                  capi_coprocessor_resource_get_count().
 * @param[in] enable True to enable, false to disable.
 *
 * @return 0, -EINVAL for invalid arguments or an out of range index, or
 *         -ENOTSUP if unsupported.
 */
int capi_coprocessor_resource_set_enabled(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_resource_type type, uint32_t index,
		bool enable);

/**
 * @brief Query whether one coprocessor resource is enabled.
 *
 * @param[in] handle Coprocessor handle.
 * @param[in] type Resource class.
 * @param[in] index Resource index.
 * @param[out] enabled Receives the current enable state.
 *
 * @return 0, -EINVAL for invalid arguments or an out of range index, or
 *         -ENOTSUP if unsupported.
 */
int capi_coprocessor_resource_get_enabled(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_resource_type type, uint32_t index,
		bool *enabled);

/**
 * @brief Structure holding coprocessor driver function pointers.
 *
 * @ref init, @ref deinit, @ref boot and @ref halt are mandatory; a call made
 * through a NULL entry among those returns -EINVAL. Every other entry is
 * optional and returns -ENOTSUP when left NULL, which lets a caller tell a bad
 * argument apart from a capability the hardware simply does not have.
 */
struct capi_coprocessor_ops {
	/** See capi_coprocessor_init(). */
	int (*init)(struct capi_coprocessor_handle **handle,
		    const struct capi_coprocessor_config *config);
	/** See capi_coprocessor_deinit(). */
	int (*deinit)(struct capi_coprocessor_handle *handle);
	/** See capi_coprocessor_boot(). */
	int (*boot)(struct capi_coprocessor_handle *handle);
	/** See capi_coprocessor_halt(). */
	int (*halt)(struct capi_coprocessor_handle *handle);
	/** Optional. See capi_coprocessor_get_boot_caps(). */
	int (*get_boot_caps)(struct capi_coprocessor_handle *handle, uint32_t *caps);
	/** Optional. See capi_coprocessor_set_boot_config(). */
	int (*set_boot_config)(struct capi_coprocessor_handle *handle,
			       const struct capi_coprocessor_boot_config *config);
	/** Optional. See capi_coprocessor_get_boot_config(). */
	int (*get_boot_config)(struct capi_coprocessor_handle *handle,
			       struct capi_coprocessor_boot_config *config);
	/** Optional. See capi_coprocessor_set_mode(). */
	int (*set_mode)(struct capi_coprocessor_handle *handle,
			enum capi_coprocessor_mode mode);
	/** Optional. See capi_coprocessor_get_mode(). */
	int (*get_mode)(struct capi_coprocessor_handle *handle,
			enum capi_coprocessor_mode *mode);
	/** Optional. See capi_coprocessor_resource_get_count(). */
	int (*resource_get_count)(struct capi_coprocessor_handle *handle,
				  enum capi_coprocessor_resource_type type, uint32_t *count);
	/** Optional. See capi_coprocessor_resource_set_enabled(). */
	int (*resource_set_enabled)(struct capi_coprocessor_handle *handle,
				    enum capi_coprocessor_resource_type type, uint32_t index,
				    bool enable);
	/** Optional. See capi_coprocessor_resource_get_enabled(). */
	int (*resource_get_enabled)(struct capi_coprocessor_handle *handle,
				    enum capi_coprocessor_resource_type type, uint32_t index,
				    bool *enabled);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_COPROCESSOR_H_ */
