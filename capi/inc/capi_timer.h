/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common API TIMER
 */

#ifndef _CAPI_TIMER_H_
#define _CAPI_TIMER_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief TIMER counter direction
 */
enum capi_timer_counter_direction {
	/** Timer counts up from min to max. */
	CAPI_TIMER_COUNT_UP,
	/** Timer counts down from max to min. */
	CAPI_TIMER_COUNT_DOWN,
	/** Can be used as a starting point to augment implementation specific directions */
	CAPI_TIMER_COUNTER_DIRECTION_LIMIT
};

/**
 * @brief TIMER counter configuration
 * @note Structure used to pass counter configuration parameters to Driver
 */
struct capi_timer_counter_config {
	/** Counter direction: up, down. See: enum capi_timer_counter_direction */
	uint32_t direction;
	/** Minimum value for the counter. */
	uint32_t min;
	/** Maximum value for the counter. */
	uint32_t max;
	/** If true, rollover to min after reaching max (or to max after reaching min if counting
	 * down). */
	bool rollover;
	/** Optional platform specific extra configuration. */
	const void *extra;
};

/**
 * @brief TIMER channel operation modes
 */
enum capi_timer_channel_mode {
	CAPI_TIMER_COMPARE_MODE,
	CAPI_TIMER_CAPTURE_MODE,
	CAPI_TIMER_PWM_MODE,
	/** Can be used as a starting point to augment implementation specific modes */
	CAPI_TIMER_CHANNEL_MODE_LIMIT
};

/**
 * @brief TIMER output compare polarity on match
 */
enum capi_timer_on_compare_polarity {
	/** On match, keep the output compare as is. */
	CAPI_TIMER_ON_COMPARE_KEEP,
	/** On match, set the output compare pin as active. */
	CAPI_TIMER_ON_COMPARE_ACTIVE,
	/** On match, set the output compare pin as inactive. */
	CAPI_TIMER_ON_COMPARE_INACTIVE,
	/** On match, toggle the output compare pin. */
	CAPI_TIMER_ON_COMPARE_TOGGLE,
	/** On match, generate a 1-cycle pulse. */
	CAPI_TIMER_ON_COMPARE_PULSE,
	/** Can be used as a starting point to augment implementation specific polarities */
	CAPI_TIMER_ON_COMPARE_POLARITY_LIMIT
};

/**
 * @brief Compare channel mode parameters
 * @note Structure used to pass compare mode parameters to Driver
 */
struct capi_timer_compare_config {
	/** If true, generates a platform specific pulse (duration, polarity, etc.) on output
	 * compare match using the output port defined by output_identifier. */
	bool generate_pulse_on_match;
	/** Platform specific output compare port or pin identifier used for pulse generation. */
	uint64_t output_identifier;
	/** Output compare polarity on match. See: enum capi_timer_on_compare_polarity */
	uint32_t polarity;
	/** Stop timer in case of a match in this channel */
	bool stop_enabled;
	/** Match value for the compare channel */
	uint32_t match_value;
	/** Optional platform specific extra configuration. */
	const void *extra;
};

/**
 * @brief TIMER input capture edge sensitivity
 */
enum capi_timer_capture_edge {
	/** Capture on both rising and falling edges of the input capture pin. */
	CAPI_TIMER_CAPTURE_BOTH,
	/** Capture only the rising edge of the input capture pin. */
	CAPI_TIMER_CAPTURE_RISING,
	/** Capture only the falling edge of the input capture pin. */
	CAPI_TIMER_CAPTURE_FALLING,
	/** Can be used as a starting point to augment implementation specific edges */
	CAPI_TIMER_CAPTURE_EDGE_LIMIT
};

/**
 * @brief Capture channel mode parameters
 * @note Structure used to pass capture mode parameters to Driver
 */
struct capi_timer_capture_config {
	/** Platform specific input capture port or pin identifier used to trigger capture. */
	uint64_t input_identifier;
	/** Specify which input capture pin edge to capture on. See: enum capi_timer_capture_edge */
	uint8_t edge;
	/** Optional platform specific extra configuration. */
	const void *extra;
};

/**
 * @brief PWM mode parameters
 * @note Structure used to pass PWM(Pulse-Width modulation) mode parameters to Driver
 */
struct capi_timer_pwm_config {
	/** Platform specific PWM output port or pin identifier used to generate PWM output. */
	uint64_t output_identifier;
	/** PWM Polarity, normal or inverted. */
	bool inverted_polarity;
	/** Period of the PWM signal in nanoseconds. */
	uint64_t period_ns;
	/** Pulse (active part of the period) duration in nanoseconds, always smaller than
	 * period_ns. */
	uint64_t active_ns;
	/** Offset of the active part of the period from the period start. */
	uint64_t offset_ns;
	/** Optional platform specific extra configuration. */
	const void *extra;
};

/**
 * @brief TIMER channel configuration
 */
struct capi_timer_channel_config {
	/** The timer channel mode. See: enum capi_timer_channel_mode */
	uint32_t mode;
	union {
		struct capi_timer_compare_config compare;
		struct capi_timer_capture_config capture;
		struct capi_timer_pwm_config pwm;
	} config;
	const void *extra; /**< Optional platform specific extra configuration. */
};

/**
 * @brief TIMER configuration
 */
struct capi_timer_config {
	/** platform ops - optional. if NULL, API selects driver from static mapping based on
	 * identifier. */
	const struct capi_timer_ops *ops;
	/** Platform specific identifier of device instance (base address, index, etc.). */
	uint64_t identifier;
	/** Platform specific identifier for the input clock source (e.g., address, index, etc.). */
	uint64_t input_clock_identifier;
	/** Input source frequency in Hz. */
	uint64_t input_clock_hz;
	/** Desired operation frequency in Hz, implementation will try to find optimal prescaler
	 * values to obtain it. */
	uint64_t output_freq_hz;
	/** Optional platform specific extra configuration. */
	const void *extra;
};

/**
 * @brief TIMER handle type
 */
struct capi_timer_handle {
	const struct capi_timer_ops *ops; /**< Function pointers that implement the Timer CAPI */
	bool init_allocated;              /**< If true, the driver is owner of handle memory. */
	void *priv;                       /**< Pointer to driver-specific handle extension. */
};

/** @brief TIMER global (non-channel) events */
enum capi_timer_global_event {
	/** Timer counter overflowed event (TIMER event, not a TIMER channel event). */
	CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW,
	/** Can be used as a starting point to augment implementation specific event types */
	CAPI_TIMER_GLOBAL_EVENT_LIMIT
};

/** @brief TIMER channel events */
enum capi_timer_channel_event {
	/** Compare match event. */
	CAPI_TIMER_CHANNEL_EVENT_COMPARE,
	/** Input capture event. */
	CAPI_TIMER_CHANNEL_EVENT_CAPTURE,
	/** Can be used as a starting point to augment implementation specific event types */
	CAPI_TIMER_CHANNEL_EVENT_LIMIT
};

/**
 * @brief TIMER event callback type (non-channel/global)
 * @param[in] event TIMER async event type. See: enum capi_timer_global_event
 * @param[in] arg Pointer to user specific data.
 * @param[in] event_extra optional, platform/driver specific extra information for event
 */
typedef void (*capi_timer_event_callback)(uint32_t event, void *arg, int event_extra);

/**
 * @brief TIMER channel callback type
 * @param[in] event TIMER async event type. See: enum capi_timer_channel_event
 * @param[in] chan TIMER channel.
 * @param[in] arg Pointer to user specific data.
 * @param[in] event_extra optional, platform/driver specific extra information for event
 */
typedef void (*capi_timer_channel_callback)(uint32_t event, uint32_t chan, void *arg,
					    int event_extra);

/**
 * @struct capi_timer_ops
 * @brief Structure that contains the timer operations function pointers.
 */
struct capi_timer_ops {
	/** See capi_timer_init() */
	int (*init)(struct capi_timer_handle **handle, const struct capi_timer_config *config);
	/** See capi_timer_deinit() */
	int (*deinit)(struct capi_timer_handle *handle);
	/** See capi_timer_start() */
	int (*start)(struct capi_timer_handle *handle);
	/** See capi_timer_stop() */
	int (*stop)(struct capi_timer_handle *handle);
	/** See capi_timer_counter_config() */
	int (*counter_config)(struct capi_timer_handle *handle,
			      const struct capi_timer_counter_config *config);
	/** See capi_timer_counter_get() */
	int (*counter_get)(struct capi_timer_handle *handle, uint32_t *counter);
	/** See capi_timer_event_irq_enable() */
	int (*event_irq_enable)(struct capi_timer_handle *handle, uint32_t event);
	/** See capi_timer_event_irq_disable() */
	int (*event_irq_disable)(struct capi_timer_handle *handle, uint32_t event);
	/** See capi_timer_register_event_callback() */
	int (*register_event_callback)(struct capi_timer_handle *handle,
				       capi_timer_event_callback callback, void *callback_arg);

	/** See capi_timer_channel_init() */
	int (*channel_init)(struct capi_timer_handle *handle, uint32_t chan);
	/** See capi_timer_channel_deinit() */
	int (*channel_deinit)(struct capi_timer_handle *handle, uint32_t chan);
	/** See capi_timer_channel_config() */
	int (*channel_config)(struct capi_timer_handle *handle, uint32_t chan,
			      const struct capi_timer_channel_config *ch_config);
	/** See capi_timer_channel_enable() */
	int (*channel_enable)(struct capi_timer_handle *handle, uint32_t chan);
	/** See capi_timer_channel_disable() */
	int (*channel_disable)(struct capi_timer_handle *handle, uint32_t chan);
	/** See capi_timer_channel_compare_set() */
	int (*channel_compare_set)(struct capi_timer_handle *handle, uint32_t chan,
				   uint32_t compare);
	/** See capi_timer_channel_compare_get() */
	int (*channel_compare_get)(struct capi_timer_handle *handle, uint32_t chan,
				   uint32_t *compare);
	/** See capi_timer_channel_capture_get() */
	int (*channel_capture_get)(struct capi_timer_handle *handle, uint32_t chan,
				   uint32_t *capture);
	/** See capi_timer_channel_irq_enable() */
	int (*channel_irq_enable)(struct capi_timer_handle *handle, uint32_t chan, uint32_t event);
	/** See capi_timer_channel_irq_disable() */
	int (*channel_irq_disable)(struct capi_timer_handle *handle, uint32_t chan, uint32_t event);
	/** See capi_timer_channel_register_callback() */
	int (*channel_register_callback)(struct capi_timer_handle *handle, uint32_t chan,
					 capi_timer_channel_callback callback, void *callback_arg);
	/** See capi_timer_is_irq_pending() */
	int (*is_irq_pending)(struct capi_timer_handle *handle, bool *pending);
	/** See capi_timer_isr() */
	void (*isr)(struct capi_timer_handle *handle);

	/** See capi_timer_nsec_to_ticks() */
	int (*nsec_to_ticks)(const struct capi_timer_handle *handle, uint64_t duration_ns,
			     uint32_t *ticks);
	/** See capi_timer_ticks_to_nsec() */
	int (*ticks_to_nsec)(const struct capi_timer_handle *handle, uint64_t ticks,
			     uint32_t *duration_ns);
};

/**
 * @brief Timer init function, allocates the handle and configures the timer.
 * @param[in] handle Double pointer to a timer handle structure. Allocated by implementation when
 * *handle is NULL.
 * @param[in] config Pointer to a timer configuration structure.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_init(struct capi_timer_handle **handle, const struct capi_timer_config *config);

/**
 * @brief Deallocate resources allocated by capi_timer_init and put the timer in an idle state.
 * @param[in] handle Pointer to a timer handle structure.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_deinit(struct capi_timer_handle *handle);

/**
 * @brief Start counting using the timer counter.
 * @param[in] handle Pointer to a timer handle structure.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_start(struct capi_timer_handle *handle);

/**
 * @brief Stop the timer counter from counting.
 * @param[in] handle Pointer to a timer handle structure.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_stop(struct capi_timer_handle *handle);

/**
 * @brief Configure the timer counter.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] config Pointer to the counter configuration structure.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_counter_config(struct capi_timer_handle *handle,
			      const struct capi_timer_counter_config *config);

/**
 * @brief Get the timer counter value.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[out] counter The value the counter is at.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_counter_get(struct capi_timer_handle *handle, uint32_t *counter);

/**
 * @brief Enable interrupt for TIMER event (non-channel) specific events.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] event The event identifier.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_event_irq_enable(struct capi_timer_handle *handle, uint32_t event);

/**
 * @brief Disable interrupt for TIMER event (non-channel) specific events.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] event The event identifier.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_event_irq_disable(struct capi_timer_handle *handle, uint32_t event);

/**
 * @brief Register a user provided callback for TIMER event (non-channel) specific events.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] callback Callback function pointer.
 * @param[in] callback_arg Arguments that the interrupt passes to the callback function.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_register_event_callback(struct capi_timer_handle *handle,
				       capi_timer_event_callback callback, void *callback_arg);

/**
 * @brief Initialize a timer channel by allocating the resources required for it.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_init(struct capi_timer_handle *handle, uint32_t chan);

/**
 * @brief Deinitialize a timer channel by deallocating the resources it required and by putting the
 * channel in an idle state.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_deinit(struct capi_timer_handle *handle, uint32_t chan);

/**
 * @brief Configure the timer channel without allocating/deallocating resources.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] ch_config Pointer to the channel configuration structure (void *, because there are
 * multiple channel types).
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_config(struct capi_timer_handle *handle, uint32_t chan,
			      const struct capi_timer_channel_config *ch_config);

/**
 * @brief Enable the specified channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_enable(struct capi_timer_handle *handle, uint32_t chan);

/**
 * @brief Disable the specified channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_disable(struct capi_timer_handle *handle, uint32_t chan);

/**
 * @brief Set the compare value for an output compare channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] compare The compare value.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_compare_set(struct capi_timer_handle *handle, uint32_t chan,
				   uint32_t compare);

/**
 * @brief Get the compare value for an output compare channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[out] compare The compare value.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_compare_get(struct capi_timer_handle *handle, uint32_t chan,
				   uint32_t *compare);
/**
 * @brief Get the capture value of an input capture channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[out] capture The input capture value.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_capture_get(struct capi_timer_handle *handle, uint32_t chan,
				   uint32_t *capture);

/**
 * @brief Enable interrupt when the specified event occurs on the specified channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] event The event identifier.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_irq_enable(struct capi_timer_handle *handle, uint32_t chan, uint32_t event);

/**
 * @brief Disable interrupt for the specified event on the specified channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] event The event identifier.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_irq_disable(struct capi_timer_handle *handle, uint32_t chan, uint32_t event);

/**
 * @brief Register a user provided callback on TIMER channel specific events.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] callback Callback function pointer.
 * @param[in] callback_arg Arguments that the interrupt passes to the callback function.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_register_callback(struct capi_timer_handle *handle, uint32_t chan,
					 capi_timer_channel_callback callback, void *callback_arg);

/**
 * @brief Check if the specified timer has any pending interrupts.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[out] pending Boolean indicating if there are any pending interrupts.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_is_irq_pending(struct capi_timer_handle *handle, bool *pending);

/**
 * @brief Convert nanoseconds to timer ticks (convenience function).
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] duration_ns Duration in nanoseconds.
 * @param[out] ticks Timer tick count for given duration_ns.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_nsec_to_ticks(const struct capi_timer_handle *handle, uint64_t duration_ns,
			     uint32_t *ticks);

/**
 * @brief Convert timer ticks to nanoseconds (convenience function).
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] ticks Timer tick count.
 * @param[out] duration_ns Duration in nanoseconds for given ticks.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_ticks_to_nsec(const struct capi_timer_handle *handle, uint64_t ticks,
			     uint32_t *duration_ns);

/**
 * @brief Timer interrupt service routine (ISR)
 *
 * This function should be called from the actual hardware interrupt handler.
 * It handles timer events and calls registered callbacks as needed.
 *
 * @param [in] handle Timer controller handle
 */
void capi_timer_isr(struct capi_timer_handle *handle);

#if defined(__cplusplus)
}
#endif

#endif
