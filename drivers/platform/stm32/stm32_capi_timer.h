#ifndef STM32_CAPI_TIMER_H_
#define STM32_CAPI_TIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi/capi_timer.h"

#ifdef HAL_TIM_MODULE_ENABLED

/** Maximum number of timer channels */
#define STM32_TIMER_MAX_CHANNELS	4

/**
 * @struct stm32_capi_timer_extra_config
 * @brief STM32-specific timer configuration for CAPI interface
 */
struct stm32_capi_timer_extra_config {
	/** Pre-initialized TIM handle (optional - if NULL, driver will init) */
	TIM_HandleTypeDef *htim;
	/** Get input clock frequency function (optional) */
	uint32_t (*get_input_clock)(void);
	/** IRQ number for the timer (0 to disable IRQ) */
	uint32_t irq_num;
};

/**
 * @struct stm32_capi_timer_channel_state
 * @brief Per-channel state for STM32 timer
 */
struct stm32_capi_timer_channel_state {
	/** Channel mode */
	uint32_t mode;
	/** Channel enabled flag */
	bool enabled;
	/** Channel-specific callback */
	capi_timer_channel_callback callback;
	/** Callback argument */
	void *callback_arg;
};

/**
 * @struct stm32_capi_timer_priv
 * @brief STM32 platform specific timer private data
 */
struct stm32_capi_timer_priv {
	/** STM32 HAL timer handle */
	TIM_HandleTypeDef htim;
	/** Timer input clock frequency in Hz */
	uint64_t input_clock_hz;
	/** Timer output/counting frequency in Hz */
	uint64_t output_freq_hz;
	/** Counter direction */
	uint32_t direction;
	/** Counter minimum value */
	uint32_t counter_min;
	/** Counter maximum value */
	uint32_t counter_max;
	/** Rollover enabled */
	bool rollover;
	/** IRQ number */
	uint32_t irq_num;
	/** Global event callback */
	capi_timer_event_callback event_callback;
	/** Global event callback argument */
	void *event_callback_arg;
	/** Channel states */
	struct stm32_capi_timer_channel_state channels[STM32_TIMER_MAX_CHANNELS];
};

/**
 * @brief STM32 platform specific timer operations for CAPI
 */
extern const struct capi_timer_ops stm32_capi_timer_ops;

/**
 * @brief Initialize a timer
 * @param handle - Pointer to handle pointer
 * @param config - Timer configuration
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_init(struct capi_timer_handle **handle,
			  const struct capi_timer_config *config);

/**
 * @brief Deinitialize a timer
 * @param handle - Timer handle
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_deinit(struct capi_timer_handle *handle);

/**
 * @brief Start the timer
 * @param handle - Timer handle
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_start(struct capi_timer_handle *handle);

/**
 * @brief Stop the timer
 * @param handle - Timer handle
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_stop(struct capi_timer_handle *handle);

/**
 * @brief Configure the timer counter
 * @param handle - Timer handle
 * @param config - Counter configuration
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_counter_config(struct capi_timer_handle *handle,
				    const struct capi_timer_counter_config *config);

/**
 * @brief Get the current counter value
 * @param handle - Timer handle
 * @param counter - Pointer to store counter value
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_counter_get(struct capi_timer_handle *handle,
				 uint32_t *counter);

/**
 * @brief Enable timer event IRQ
 * @param handle - Timer handle
 * @param event - Event type
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_event_irq_enable(struct capi_timer_handle *handle,
				      uint32_t event);

/**
 * @brief Disable timer event IRQ
 * @param handle - Timer handle
 * @param event - Event type
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_event_irq_disable(struct capi_timer_handle *handle,
				       uint32_t event);

/**
 * @brief Register event callback
 * @param handle - Timer handle
 * @param callback - Callback function
 * @param callback_arg - Callback argument
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_register_event_callback(struct capi_timer_handle *handle,
					     capi_timer_event_callback callback,
					     void *callback_arg);

/**
 * @brief Initialize a timer channel
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_init(struct capi_timer_handle *handle,
				  uint32_t chan);

/**
 * @brief Deinitialize a timer channel
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_deinit(struct capi_timer_handle *handle,
				    uint32_t chan);

/**
 * @brief Configure a timer channel
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @param ch_config - Channel configuration
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_config(struct capi_timer_handle *handle,
				    uint32_t chan,
				    const struct capi_timer_channel_config *ch_config);

/**
 * @brief Enable a timer channel
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_enable(struct capi_timer_handle *handle,
				    uint32_t chan);

/**
 * @brief Disable a timer channel
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_disable(struct capi_timer_handle *handle,
				     uint32_t chan);

/**
 * @brief Set compare value for a channel
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @param compare - Compare value
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_compare_set(struct capi_timer_handle *handle,
					 uint32_t chan, uint32_t compare);

/**
 * @brief Get compare value from a channel
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @param compare - Pointer to store compare value
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_compare_get(struct capi_timer_handle *handle,
					 uint32_t chan, uint32_t *compare);

/**
 * @brief Get capture value from a channel
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @param capture - Pointer to store capture value
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_capture_get(struct capi_timer_handle *handle,
					 uint32_t chan, uint32_t *capture);

/**
 * @brief Enable channel IRQ
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @param event - Event type
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_irq_enable(struct capi_timer_handle *handle,
					uint32_t chan, uint32_t event);

/**
 * @brief Disable channel IRQ
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @param event - Event type
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_irq_disable(struct capi_timer_handle *handle,
					 uint32_t chan, uint32_t event);

/**
 * @brief Register channel callback
 * @param handle - Timer handle
 * @param chan - Channel number (0-3)
 * @param callback - Callback function
 * @param callback_arg - Callback argument
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_channel_register_callback(struct capi_timer_handle *handle,
					       uint32_t chan,
					       capi_timer_channel_callback callback,
					       void *callback_arg);

/**
 * @brief Check if IRQ is pending
 * @param handle - Timer handle
 * @param pending - Pointer to store pending status
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_is_irq_pending(struct capi_timer_handle *handle,
				    bool *pending);

/**
 * @brief Timer ISR handler
 * @param handle - Timer handle
 */
void stm32_capi_timer_isr(struct capi_timer_handle *handle);

/**
 * @brief Convert nanoseconds to timer ticks
 * @param handle - Timer handle
 * @param duration_ns - Duration in nanoseconds
 * @param ticks - Pointer to store tick count
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_nsec_to_ticks(const struct capi_timer_handle *handle,
				   uint64_t duration_ns, uint32_t *ticks);

/**
 * @brief Convert timer ticks to nanoseconds
 * @param handle - Timer handle
 * @param ticks - Tick count
 * @param duration_ns - Pointer to store duration in nanoseconds
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_timer_ticks_to_nsec(const struct capi_timer_handle *handle,
				   uint64_t ticks, uint32_t *duration_ns);

#endif /* HAL_TIM_MODULE_ENABLED */

#endif /* STM32_CAPI_TIMER_H_ */
