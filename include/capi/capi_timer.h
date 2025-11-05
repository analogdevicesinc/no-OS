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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TIMER channel operation modes
 */
enum capi_timer_channel_mode {
	CAPI_TIMER_COMPARE_MODE = 0u,
	CAPI_TIMER_CAPTURE_MODE,
	CAPI_TIMER_PWM_MODE,
};

/**
 * @brief TIMER output compare polarity on match
 */
enum capi_timer_on_compare_polarity {
	/** On match, keep the output compare as is. */
	CAPI_TIMER_ON_COMPARE_KEEP = 0u,
	/** On match, set the output compare pin as active. */
	CAPI_TIMER_ON_COMPARE_ACTIVE,
	/** On match, set the output compare pin as inactive. */
	CAPI_TIMER_ON_COMPARE_INACTIVE,
	/** On match, toggle the output compare pin. */
	CAPI_TIMER_ON_COMPARE_TOGGLE
};

/**
 * @brief TIMER input capture edge sensitivity
 */
enum capi_timer_capture_edge {
	/** Capture on both rising and falling edges of the input capture pin. */
	CAPI_TIMER_CAPTURE_BOTH = 0u,
	/** Capture only the rising edge of the input capture pin. */
	CAPI_TIMER_CAPTURE_RISING,
	/** Capture only the falling edge of the input capture pin. */
	CAPI_TIMER_CAPTURE_FALLING,
};

/**
 * @brief PWM timer capture channel mode parameters
 * @note Structure used to pass PWM(Pulse-Width modulation) mode parameters to Driver
 */
struct capi_timer_compare_config {
	/** Specify whether to use the output compare pin or not. */
	bool use_pin;
	/** Output compare port number. */
	uint32_t port;
	/** Output compare pin number. */
	uint32_t pin;
	/** Output compare polarity on match. */
	enum capi_timer_on_compare_polarity polarity;
};

/**
 * @brief PWM timer capture channel mode parameters
 * @note Structure used to pass PWM(Pulse-Width modulation) mode parameters to Driver
 */
struct capi_timer_capture_config {
	/** Specify whether to use the input capture pin or not. */
	bool use_pin;
	/** Input capture port number. */
	uint32_t port;
	/** Input capture pin number. */
	uint32_t pin;
	/** Specify which input capture pin edge to capture on. */
	enum capi_timer_capture_edge edge;
	/** Enable digital filter or debouncing filter. */
	bool filter;
};

/**
 * @brief PWM mode parameters
 * @note Structure used to pass PWM(Pulse-Width modulation) mode parameters to Driver
 */
struct capi_timer_pwm_config {
	/** PWM output port number. */
	uint32_t port;
	/** PWM output pin number. */
	uint32_t pin;
	/** PWM Polarity, normal or inverted. */
	bool inverted_polarity;
	/** Period of the PWM signal in nanoseconds. */
	uint64_t period_ns;
	/** Pulse (active part of the period) duration in nanoseconds, always smaller than
	 * period_ns. */
	uint64_t active_ns;
	/** Offset of the active part of the period from the period start. */
	uint64_t offset_ns;
};

/**
 * @brief TIMER channel configuration
 */
struct capi_timer_channel_config {
	/** The timer channel mode. */
	enum capi_timer_channel_mode mode;
	union {
		struct capi_timer_compare_config compare;
		struct capi_timer_capture_config capture;
		struct capi_timer_pwm_config pwm;
	};
};

/**
 * @brief TIMER configuration
 */
struct capi_timer_config {
	/** platform ops - optional. if NULL, API selects driver from static mapping based on
	 * identifier. */
	const struct capi_timer_ops *ops;
	/** TIMER controller identifier (base address). */
	uint64_t identifier;
	/** Specifies whether the input clock of the timer is internal or external. */
	bool input_clock_external;
	/** Input source frequency in Hz. */
	uint32_t input_clock_hz;
	/** Desired operation frequency, implementation will try to find optimal prescaler values to
	 * obtain it. */
	uint32_t output_freq_hz;
	/** Optional platform specific extra configuration. */
	void *extra;
};

/**
 * @brief TIMER handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_timer_handle {
	/** Function pointers that implement the Timer CAPI. */
	const struct capi_timer_ops *ops;
	/** If true, the driver is owner of handle memory. */
	bool init_allocated;
	/** Used by CAPI thin-layer to provide exclusive access to the peripheral. */
	void *lock;
	/** Implementation specifics. */
	void *priv;
};

/** @brief TIMER events */
enum capi_timer_event {
	/** Timer counter overflowed event (TIMER event, not a TIMER channel event). */
	CAPI_TIMER_EVENT_COUNTER_OVERFLOW,
	/** Compare match event. */
	CAPI_TIMER_EVENT_COMPARE,
	/** Input capture event. */
	CAPI_TIMER_EVENT_CAPTURE,
};

/**
 * @brief  TIMER callback type
 * @param[in] event TIMER async event type.
 * @param[in] chan TIMER channel.
 * @param[in] arg Pointer to user specific data.
 * @param[in] event_extra optional, platform/driver specific extra information for event
 */
typedef void (*capi_timer_callback)(enum capi_timer_event event, uint32_t chan, void *arg,
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
	/** See capi_timer_counter_set() */
	int (*counter_set)(struct capi_timer_handle *handle, uint32_t counter);
	/** See capi_timer_counter_get() */
	int (*counter_get)(struct capi_timer_handle *handle, uint32_t *counter);

	/** See capi_timer_channel_init() */
	int (*channel_init)(struct capi_timer_handle *handle, uint32_t chan);
	/** See capi_timer_channel_config() */
	int (*channel_config)(struct capi_timer_handle *handle, uint32_t chan, void *ch_config);
	/** See capi_timer_channel_deinit() */
	int (*channel_deinit)(struct capi_timer_handle *handle, uint32_t chan);
	/** See capi_timer_channel_enable() */
	int (*channel_enable)(struct capi_timer_handle *handle, uint32_t chan);
	/** See capi_timer_channel_disable() */
	int (*channel_disable)(struct capi_timer_handle *handle, uint32_t chan);
	/** See capi_timer_irq_enable() */
	int (*irq_enable)(struct capi_timer_handle *handle, uint32_t chan,
			  enum capi_timer_event type);
	/** See capi_timer_irq_disable() */
	int (*irq_disable)(struct capi_timer_handle *handle, uint32_t chan,
			   enum capi_timer_event type);
	/** See capi_timer_channel_register_callback() */
	int (*channel_register_callback)(struct capi_timer_handle *handle, uint32_t chan,
					 capi_timer_callback *const callback,
					 void *const callback_arg);
	/** See capi_timer_compare_set() */
	int (*compare_set)(struct capi_timer_handle *handle, uint32_t chan, uint32_t compare);
	/** See capi_timer_compare_get() */
	int (*compare_get)(struct capi_timer_handle *handle, uint32_t chan, uint32_t *compare);
	/** See capi_timer_capture_get() */
	int (*capture_get)(struct capi_timer_handle *handle, uint32_t chan, uint32_t *capture);
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
 * @brief Set the timer counter value.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] counter The value the counter will be set to.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_counter_set(struct capi_timer_handle *handle, uint32_t counter);

/**
 * @brief Get the timer counter value.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[out] counter The value the counter is at.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_counter_get(struct capi_timer_handle *handle, uint32_t *counter);

/**
 * @brief Initialize a timer channel by allocating the resources required for it.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_init(struct capi_timer_handle *handle, uint32_t chan);

/**
 * @brief Configure the timer channel without allocating/deallocating resources.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] ch_config Pointer to the channel configuration structure (void *, because there are
 * multiple channel types).
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_config(struct capi_timer_handle *handle, uint32_t chan, void *ch_config);

/**
 * @brief Deinitialize a timer channel by deallocating the resources it required and by putting the
 * channel in an idle state.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_deinit(struct capi_timer_handle *handle, uint32_t chan);

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
 * @brief Enable interrupt when the specified event occurs on the specified channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] event The event identifier.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_irq_enable(struct capi_timer_handle *handle, uint32_t chan,
				  enum capi_timer_event event);

/**
 * @brief Disable interrupt when the specified event occurs on the specified channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] event The event identifier.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_irq_disable(struct capi_timer_handle *handle, uint32_t chan,
				   enum capi_timer_event event);

/**
 * @brief Register a user provided callback on TIMER channel specific events.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] callback Callback function pointer.
 * @param[in] callback_arg Arguments that the interrupt passes to the callback function.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_channel_register_callback(struct capi_timer_handle *handle, uint32_t chan,
					 capi_timer_callback *const callback,
					 void *const callback_arg);

/**
 * @brief Set the compare value for an output compare channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[in] compare The compare value.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_compare_set(struct capi_timer_handle *handle, uint32_t chan, uint32_t compare);
/**
 * @brief Get the compare value from an output compare channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[out] compare The compare value.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_compare_get(struct capi_timer_handle *handle, uint32_t chan, uint32_t *compare);

/**
 * @brief Get the capture value of an input capture channel.
 * @param[in] handle Pointer to a timer handle structure.
 * @param[in] chan The channel number.
 * @param[out] capture The input capture value.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_timer_capture_get(struct capi_timer_handle *handle, uint32_t chan, uint32_t *capture);

#ifdef __cplusplus
}
#endif

#endif
