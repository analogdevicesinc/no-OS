/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common API for Pin Interrupt (PINT) Controller
 */

#ifndef _CAPI_PINT_H_
#define _CAPI_PINT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief PINT trigger types
 */
enum capi_pint_trigger {
	CAPI_PINT_TRIGGER_EDGE_RISING = 0x00U,  /**< Rising edge trigger */
	CAPI_PINT_TRIGGER_EDGE_FALLING = 0x01U, /**< Falling edge trigger */
	CAPI_PINT_TRIGGER_LEVEL_LOW = 0x02U,    /**< Level low trigger */
	CAPI_PINT_TRIGGER_LEVEL_HIGH = 0x03U    /**< Level high trigger */
};

/**
 * @brief PINT callback type
 *
 * @param [in] pin_index Pin index that triggered the interrupt
 * @param [in] user_data Pointer to user-specific data
 */
typedef void (*capi_pint_callback_t)(uint8_t pin_index, void *user_data);

/**
 * @brief PINT pin configuration structure
 */
struct capi_pint_pin_config {
	enum capi_pint_trigger trigger_type; /**< Trigger type for the pin */
	bool enabled;                        /**< Enable/disable flag */
};

/**
 * @brief PINT port handle type
 */
struct capi_pint_port_handle {
	const struct capi_pint_ops *ops; /**< Set and used by CAPI thin layer */
	bool init_allocated;             /**< If true, the driver owns handle memory */
	void *lock; /**< Set and used by CAPI thin layer if mutex is available */
	void *priv; /**< Set and used by driver implementation */
};

/**
 * @brief PINT port configuration
 */
struct capi_pint_port_config {
	/** PINT port specific operations */
	const struct capi_pint_ops *ops;
	/** PINT port identifier (typically base address) */
	uint64_t identifier;
	/** Number of PINT pins */
	uint8_t num_pins;
	/** Reference to PINT port private information */
	void *priv;
};

/**
 * @brief Initialize the PINT port
 *
 * @param [in,out] handle If NULL, the function must allocate the required memory.
 *                        Otherwise, init() will use the preallocated structure.
 * @param [in] config The PINT port configuration.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_port_init(struct capi_pint_port_handle **handle,
			const struct capi_pint_port_config *config);

/**
 * @brief Deinitialize the PINT port
 *
 * @param [in] handle The PINT port handle. If allocated by init(),
 *                    deinit() must free the memory.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_port_deinit(struct capi_pint_port_handle **handle);

/**
 * @brief Configure a specific PINT pin
 *
 * @param [in] handle The PINT port handle.
 * @param [in] pin_index Pin index to configure.
 * @param [in] config Pin configuration (trigger type, enable/disable).
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_configure_pin(struct capi_pint_port_handle *handle,
			    uint8_t pin_index,
			    const struct capi_pint_pin_config *config);

/**
 * @brief Enable a specific PINT pin
 *
 * @param [in] handle The PINT port handle.
 * @param [in] pin_index Pin index to enable.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_enable_pin(struct capi_pint_port_handle *handle,
			 uint8_t pin_index);

/**
 * @brief Disable a specific PINT pin
 *
 * @param [in] handle The PINT port handle.
 * @param [in] pin_index Pin index to disable.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_disable_pin(struct capi_pint_port_handle *handle,
			  uint8_t pin_index);

/**
 * @brief Register a callback for a specific PINT pin
 *
 * @param [in] handle The PINT port handle.
 * @param [in] pin_index Pin index for callback registration.
 * @param [in] callback Callback function to be invoked on interrupt.
 * @param [in] user_data User-specific data passed to the callback.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_register_callback(struct capi_pint_port_handle *handle,
				uint8_t pin_index,
				capi_pint_callback_t callback, void *user_data);

/**
 * @brief Unregister a callback for a specific PINT pin
 *
 * @param [in] handle The PINT port handle.
 * @param [in] pin_index Pin index for callback unregistration.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_unregister_callback(struct capi_pint_port_handle *handle,
				  uint8_t pin_index);

/**
 * @brief Clear pending interrupt for a specific PINT pin
 *
 * @param [in] handle The PINT port handle.
 * @param [in] pin_index Pin index to clear pending interrupt.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_clear_pending(struct capi_pint_port_handle *handle,
			    uint8_t pin_index);

/**
 * @brief Get pending interrupt status for a specific PINT pin
 *
 * @param [in] handle The PINT port handle.
 * @param [out] pending Pointer to store pending status (bitmask of pending pins).
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_get_pending(struct capi_pint_port_handle *handle,
			  uint32_t *pending);

/**
 * @brief Handle interrupt for the PINT port (should be called from ISR)
 *
 * @param [in] handle The PINT port handle.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pint_handle_interrupt(struct capi_pint_port_handle *handle);

/**
 * @brief Container for PINT specific operations
 */
struct capi_pint_ops {
	/** See capi_pint_port_init() */
	int (*port_init)(struct capi_pint_port_handle **handle,
			 const struct capi_pint_port_config *config);
	/** See capi_pint_port_deinit() */
	int (*port_deinit)(struct capi_pint_port_handle **handle);
	/** See capi_pint_configure_pin() */
	int (*configure_pin)(struct capi_pint_port_handle *handle, uint8_t pin_index,
			     const struct capi_pint_pin_config *config);
	/** See capi_pint_enable_pin() */
	int (*enable_pin)(struct capi_pint_port_handle *handle, uint8_t pin_index);
	/** See capi_pint_disable_pin() */
	int (*disable_pin)(struct capi_pint_port_handle *handle, uint8_t pin_index);
	/** See capi_pint_register_callback() */
	int (*register_callback)(struct capi_pint_port_handle *handle,
				 uint8_t pin_index,
				 capi_pint_callback_t callback, void *user_data);
	/** See capi_pint_unregister_callback() */
	int (*unregister_callback)(struct capi_pint_port_handle *handle,
				   uint8_t pin_index);
	/** See capi_pint_clear_pending() */
	int (*clear_pending)(struct capi_pint_port_handle *handle, uint8_t pin_index);
	/** See capi_pint_get_pending() */
	int (*get_pending)(struct capi_pint_port_handle *handle, uint32_t *pending);
	/** See capi_pint_handle_interrupt() */
	int (*handle_interrupt)(struct capi_pint_port_handle *handle);
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CAPI_PINT_H_ */
