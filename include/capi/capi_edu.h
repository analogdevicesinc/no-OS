/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common API for Error Detection Unit (EDU)
 */

#ifndef _CAPI_EDU_H_
#define _CAPI_EDU_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief EDU core subsystem identifier type.
 *
 * Core subsystem values are defined by the specific driver implementation
 * This allows each platform to define its own set of cores.
 */
typedef uint32_t capi_edu_core_ss;

/**
 * @brief EDU interrupt priority level configuration.
 */
enum capi_edu_priority_level {
	CAPI_EDU_2_PRIORITY_LEVELS = 0,   /**< 2 priority levels */
	CAPI_EDU_4_PRIORITY_LEVELS = 1,   /**< 4 priority levels */
	CAPI_EDU_8_PRIORITY_LEVELS = 2,   /**< 8 priority levels */
	CAPI_EDU_16_PRIORITY_LEVELS = 3,  /**< 16 priority levels */
	CAPI_EDU_32_PRIORITY_LEVELS = 4,  /**< 32 priority levels */
	CAPI_EDU_64_PRIORITY_LEVELS = 5,  /**< 64 priority levels */
	CAPI_EDU_128_PRIORITY_LEVELS = 6, /**< 128 priority levels */
	CAPI_EDU_256_PRIORITY_LEVELS = 7  /**< 256 priority levels */
};

/**
 * @brief EDU fault trigger event types.
 */
enum capi_edu_fault_trig_event {
	CAPI_EDU_TRIG_FAULT_ACTIVE = 0, /**< Fault Active Mode */
	CAPI_EDU_TRIG_FAULT_PENDING = 1 /**< Fault Pending Mode */
};

/**
 * @brief EDU status query types for capi_edu_get_status().
 */
enum capi_edu_status_type {
	CAPI_EDU_STATUS_GLOBAL,                       /**< Global EDU status */
	CAPI_EDU_STATUS_FAULT_INTERFACE,              /**< Fault interface status */
	CAPI_EDU_STATUS_CORE_INTERFACE,               /**< System core interface status */
	CAPI_EDU_STATUS_FAULT_DELAY_CURRENT,          /**< Current fault delay countdown */
	CAPI_EDU_STATUS_FAULT_SYS_RESET_DELAY_CURRENT /**< Current fault system reset delay */
};

/**
 * @brief EDU async event types reported via error callbacks.
 */
enum capi_edu_async_event {
	CAPI_EDU_EVENT_NONE,    /**< No event */
	CAPI_EDU_EVENT_ERR_SFI, /**< Error in System Fault Interface */
	CAPI_EDU_EVENT_ERR_SCI, /**< Error in System Core Interface */
	CAPI_EDU_EVENT_ERR_SSI  /**< Error in System Source Interface */
};

/**
 * @brief EDU source event callback type.
 *
 * Invoked when an EDU source interrupt or fault event occurs.
 *
 * @param[in] sid       Source Interrupt ID that triggered the event.
 * @param[in] user_data Pointer to user-specific data.
 */
typedef void (*capi_edu_source_callback_t)(uint8_t sid, void *user_data);

/**
 * @brief EDU error callback type.
 *
 * Invoked when an EDU error condition is detected.
 *
 * @param[in] event     The async event type indicating the error category.
 * @param[in] user_data Pointer to user-specific data.
 */
typedef void (*capi_edu_error_callback_t)(enum capi_edu_async_event event, void *user_data);

/**
 * @brief EDU source configuration.
 */
struct capi_edu_source_config {
	uint8_t sid;           /**< Source Interrupt ID */
	uint8_t intr_priority; /**< Interrupt priority (applicable when fault_en is false) */
	uint8_t intr_group;    /**< Interrupt group (applicable when fault_en is false) */
	uint8_t core_target;   /**< Core target to which the interrupt is mapped */
	bool source_en;        /**< true: enable source, false: disable source */
	bool fault_en;         /**< true: forward as fault (NMI), false: forward as interrupt */
	bool edge_trig_en;     /**< true: edge-triggered, false: level-triggered */
	bool error_en;         /**< true: enable error detection, false: disable */
};

/**
 * @brief EDU fault configuration.
 *
 * Configures the fault management interface including enable, lock, delays,
 * trigger event, trigger output, and system reset behaviour.
 */
struct capi_edu_fault_config {
	uint32_t fault_delay;                      /**< Fault delay in clock periods */
	uint32_t fault_sys_reset_delay;            /**< Fault system reset delay in clock periods */
	enum capi_edu_fault_trig_event trig_event; /**< Trigger event select */
	bool fault_interface_en;                   /**< Fault interface enable */
	bool trig_output_en;                       /**< Trigger output enable */
	bool sys_reset_en;                         /**< System reset enable */
	bool lock;                                 /**< Lock fault interface after configuration */
};

/**
 * @brief EDU interrupt masking configuration.
 *
 * Consolidates priority mask, group mask, ungrouped interrupt masking,
 * and priority level settings into a single configuration structure.
 */
struct capi_edu_interrupt_config {
	uint8_t priority_mask;                     /**< Priority mask value (0-255) */
	uint8_t group_mask;                        /**< Group mask value */
	enum capi_edu_priority_level priority_lvl; /**< Number of priority levels */
	bool mask_ungrouped;                       /**< true: mask ungrouped interrupts */
};

/**
 * @brief EDU controller configuration.
 */
struct capi_edu_config {
	/** EDU peripheral identifier (typically base address). */
	uint64_t identifier;
	/** Core subsystem this EDU instance is associated with. */
	capi_edu_core_ss core_ss;
	/** Optional platform-specific extra configuration. */
	void *extra;
	/** Platform-specific implementation of the API. (Mandatory field, but can be NULL and
	 * auto-populated by the build system.) */
	const struct capi_edu_ops *ops;
};

/**
 * @brief EDU controller handle.
 *
 * Drivers may extend this handle via the priv member.
 * Driver developer shall declare this as the first field of the private handle structure.
 */
struct capi_edu_handle {
	const struct capi_edu_ops *ops; /**< Set and used by CAPI thin layer */
	bool init_allocated;            /**< If true, the driver owns handle memory */
	void *lock; /**< Set and used by CAPI thin layer if mutex is available */
	void *priv; /**< Set and used by user/driver optionally */
};

/**
 * @brief Initialize an instance of the EDU controller.
 *
 * Resets the global, core, and fault interfaces and enables the EDU block.
 *
 * @param[in,out] handle Points to a pointer of the context structure. If this
 *                       pointer is set to NULL, then the controller will allocate
 *                       the context structure and be the owner. If the pointer
 *                       is not NULL, the application has allocated the structure
 *                       and is the owner.
 * @param[in] config     Points to the configuration for the EDU controller.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_init(struct capi_edu_handle **handle, const struct capi_edu_config *config);

/**
 * @brief Deinitialize the EDU controller, disable, and restore default settings.
 *
 * Resets all source configurations, disables the fault and core interfaces,
 * and disables the EDU globally.
 *
 * @param[in] handle Points to the EDU controller context.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_deinit(struct capi_edu_handle *handle);

/**
 * @brief Configure a specific interrupt/fault source.
 *
 * Configures the source as a system interrupt or fault source and sets
 * priority, group, trigger mode, and error detection.
 *
 * @param[in] handle   Points to the EDU controller context.
 * @param[in] src_cfg  Points to the source configuration.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_configure_source(struct capi_edu_handle *handle,
			      const struct capi_edu_source_config *src_cfg);

/**
 * @brief Configure the fault management interface for the core subsystem.
 *
 * Sets fault interface enable, trigger event, output enables, delays,
 * and optionally locks the interface.
 *
 * @param[in] handle Points to the EDU controller context.
 * @param[in] cfg    Points to the fault configuration structure.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_set_fault_config(struct capi_edu_handle *handle,
			      const struct capi_edu_fault_config *cfg);

/**
 * @brief Configure interrupt masking for the core subsystem.
 *
 * Sets the priority mask, group mask, ungrouped interrupt masking,
 * and the number of priority levels in a single call.
 *
 * @param[in] handle Points to the EDU controller context.
 * @param[in] cfg    Points to the interrupt configuration structure.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_set_interrupt_config(struct capi_edu_handle *handle,
				  const struct capi_edu_interrupt_config *cfg);

/**
 * @brief Register a callback for source interrupt/fault events.
 *
 * @param[in] handle    Points to the EDU controller context.
 * @param[in] callback  User callback function.
 * @param[in] user_data User callback argument.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_register_source_callback(struct capi_edu_handle *handle,
				      capi_edu_source_callback_t callback, void *user_data);

/**
 * @brief Register a callback for EDU error events.
 *
 * @param[in] handle    Points to the EDU controller context.
 * @param[in] callback  User callback function.
 * @param[in] user_data User callback argument.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_register_error_callback(struct capi_edu_handle *handle,
				     capi_edu_error_callback_t callback, void *user_data);

/**
 * @brief Enable or disable the EDU block globally.
 *
 * When enabled, the EDU monitors and processes error events from configured sources.
 *
 * @param[in] handle Points to the EDU controller context.
 * @param[in] enable true to enable, false to disable.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_enable(struct capi_edu_handle *handle, bool enable);

/**
 * @brief Perform a global reset of the EDU block.
 *
 * Resets all EDU interfaces (global, core, and fault) to their default state.
 *
 * @param[in] handle Points to the EDU controller context.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_reset(struct capi_edu_handle *handle);

/**
 * @brief Lock or unlock the EDU configuration.
 *
 * When locked, all configuration changes (global, core interface,
 * and fault interface) are prevented until unlocked.
 *
 * @param[in] handle Points to the EDU controller context.
 * @param[in] lock   true to lock, false to unlock.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_lock(struct capi_edu_handle *handle, bool lock);

/**
 * @brief Manually raise an interrupt for a specific source ID.
 *
 * Useful for testing and debugging purposes.
 *
 * @param[in] handle Points to the EDU controller context.
 * @param[in] sid    Source Interrupt ID to raise.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_raise(struct capi_edu_handle *handle, uint8_t sid);

/**
 * @brief Get EDU status information.
 *
 * Retrieves one of several status values depending on the type parameter:
 * global status, fault interface status, core interface status, or
 * current fault delay countdown values.
 *
 * @param[in]  handle Points to the EDU controller context.
 * @param[in]  type   Status type to query.
 * @param[out] status Pointer to store the status value.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_get_status(struct capi_edu_handle *handle, enum capi_edu_status_type type,
			uint32_t *status);

/**
 * @brief Get the status of a specific source.
 *
 * @param[in]  handle Points to the EDU controller context.
 * @param[in]  sid    Source Interrupt ID to query.
 * @param[out] status Pointer to store the source status value.
 *
 * @return 0 for success or negative error code.
 */
int capi_edu_get_source_status(struct capi_edu_handle *handle, uint8_t sid, uint32_t *status);

/**
 * @brief EDU event interrupt service routine.
 *
 * Handles regular system interrupt events from the EDU block.
 * If interrupt vectors are managed by the user, call this function
 * from the relevant interrupt vector handler.
 *
 * @param[in] handle Points to the EDU controller context.
 */
void capi_edu_event_isr(void *handle);

/**
 * @brief EDU fault interrupt service routine.
 *
 * Handles Non-Maskable Interrupt (NMI) and fault-related events
 * from the EDU block. If interrupt vectors are managed by the user,
 * call this function from the relevant NMI/fault handler.
 *
 * @param[in] handle Points to the EDU controller context.
 */
void capi_edu_fault_isr(void *handle);

/**
 * @brief EDU error handler.
 *
 * Processes error conditions detected in the EDU block, identifies
 * the error source (SFI, SCI, or SSI), and invokes the registered
 * error callback.
 *
 * @param[in] handle Points to the EDU controller context.
 */
void capi_edu_error_handler(void *handle);

/**
 * @brief Structure holding EDU function pointers that point to the platform-
 * specific implementation. See API functions for relevant descriptions.
 */
struct capi_edu_ops {
	/** See capi_edu_init() */
	int (*init)(struct capi_edu_handle **handle, const struct capi_edu_config *config);
	/** See capi_edu_deinit() */
	int (*deinit)(struct capi_edu_handle *handle);
	/** See capi_edu_configure_source() */
	int (*configure_source)(struct capi_edu_handle *handle,
				const struct capi_edu_source_config *src_cfg);
	/** See capi_edu_set_fault_config() */
	int (*set_fault_config)(struct capi_edu_handle *handle,
				const struct capi_edu_fault_config *cfg);
	/** See capi_edu_set_interrupt_config() */
	int (*set_interrupt_config)(struct capi_edu_handle *handle,
				    const struct capi_edu_interrupt_config *cfg);
	/** See capi_edu_register_source_callback() */
	int (*register_source_callback)(struct capi_edu_handle *handle,
					capi_edu_source_callback_t callback, void *user_data);
	/** See capi_edu_register_error_callback() */
	int (*register_error_callback)(struct capi_edu_handle *handle,
				       capi_edu_error_callback_t callback, void *user_data);
	/** See capi_edu_enable() */
	int (*enable)(struct capi_edu_handle *handle, bool enable);
	/** See capi_edu_reset() */
	int (*reset)(struct capi_edu_handle *handle);
	/** See capi_edu_lock() */
	int (*lock)(struct capi_edu_handle *handle, bool lock);
	/** See capi_edu_raise() */
	int (*raise)(struct capi_edu_handle *handle, uint8_t sid);
	/** See capi_edu_get_status() */
	int (*get_status)(struct capi_edu_handle *handle, enum capi_edu_status_type type,
			  uint32_t *status);
	/** See capi_edu_get_source_status() */
	int (*get_source_status)(struct capi_edu_handle *handle, uint8_t sid, uint32_t *status);
	/** See capi_edu_event_isr() */
	void (*event_isr)(void *handle);
	/** See capi_edu_fault_isr() */
	void (*fault_isr)(void *handle);
	/** See capi_edu_error_handler() */
	void (*error_handler)(void *handle);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_EDU_H_ */
