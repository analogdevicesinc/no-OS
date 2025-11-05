

/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_dac.h
 * @brief Common HAL DAC. This is the generic portion of an DAC which
 *        will be covered by all DAC drivers. Other specific features will be
 *        extend the interface based on the HW design.
 */

#ifndef _CAPI_PLL_H_
#define _CAPI_PLL_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Enumeration of the DAC event that caused a call to the
 *        asynchronous callback function.
 */
enum capi_dac_event {
	CAPI_DAC_BUFFER_DONE_EVT,
	CAPI_DAC_BUFFER_THRESHOLD_EVT,
	CAPI_DAC_UNDERRUN_EVT,
	CAPI_DAC_ERROR_EVT,
	CAPI_DAC_CALIBRATION_DONE_EVT,
};

/**
 * @brief Enumeration of the DAC Clock which can be either
 *         from an internal source or an external source
 */
enum capi_dac_clock {
	CAPI_DAC_INTERNAL_CLOCK, /**< Internal clock source. */
	CAPI_DAC_EXTERNAL_CLOCK, /**< External clock source. */
	CAPI_DAC_CLOCK_MAX       /**< Max value. */
};

/**
 * @brief Enumeration of the clock polarity of the input clock.
 */
enum capi_dac_clock_polarity {
	CAPI_DAC_CLOCK_POLARITY_NEG, /**< Negative polarity. */
	CAPI_DAC_CLOCK_POLARITY_POS, /**< Posifitve poloarity. */
	CAPI_DAC_CLOCK_POLARITY_MAX  /**< Max value. */
};

/**
 * @brief Enumeration of voltage reference
 */
enum capi_dac_vref_src {
	CAPI_DAC_VREF_SOURCE_EXTERNAL, /**< VREF comes from external source. */
	CAPI_DAC_VREF_SOURCE_INTERNAL, /**< VREF generated internally. */
	CAPI_DAC_VREF_SOURCE_MAX       /**< Max value. */
};

/**
 * @typedef capi_dac_trigger_src
 * @brief Type definition for DAC trigger source (External, Timer, DMA).
 */
typedef uint32_t capi_dac_trigger_src;

/**
 * @brief DAC Callback.
 *
 * @param [in] event Specifies the event that prompted the callback.
 * @param [in] arg Points to application defined argument.
 * @param [in] event_extra Optional information from the driver.
 */
typedef void (*capi_dac_callback_t)(enum capi_dac_event event, void *arg, int event_extra);

/**
 * @struct capi_dac_config
 * @brief Structure that contains the configuration for a DAC controller.
 */
struct capi_dac_config {
	/** Identifies the instance. */
	uint32_t identifier;
	/** Interrupt request ID. */
	uint32_t dac_ctrl_id;
	/** Input clock */
	enum capi_dac_clock input_clock;
	/** Clock Polarity */
	enum capi_dac_clock_polarity clock_polarity;
	/** Trigger source */
	capi_dac_trigger_src trig_src;
	/** Platform specific DAC platform ops structure. */
	const struct capi_dac_ops *dac_ops;
	/**
	 * This is intended to store DAC specific configurations,
	 * it should not be a reference to any peripheral descriptor.
	 */
	void *extra;
};

/**
 * @struct capi_dac_handle
 * @brief Structure that contains the context of a DAC controller.
 */
struct capi_dac_handle {
	/** DAC ID. */
	uint32_t dac_id;
	/** If true, the driver is owner of handle memory. */
	bool init_allocated;
	/** Maximum number of channels controller drives */
	uint32_t channel_count;
	/** Lock */
	void *lock;
	/** Platform specific DAC platform ops structure. */
	const struct capi_dac_ops *dac_ops;
	/** This is intended to store DAC specific configurations. */
	void *extra;
};

/**
 * @struct capi_dac_chan_calibration
 * @brief Structure that defines channel calibration information.
 */
struct capi_dac_chan_calibration {
	/** To be added to the read value. */
	uint32_t offset;
	/** Attenuation to multiplied to read value. */
	uint32_t attenuation;
};

/**
 * @struct capi_dac_chan_config
 * @brief Structure that contains the configuration for a DAC channel.
 */
struct capi_dac_chan_config {
	/** DAC . */
	struct capi_dac_handle *dac_handle;
	/** If false, the driver is the owner of the handle memory. */
	uint32_t chan_id;
	/** Resolution in bits */
	uint8_t resolution;
	/** Adds offset to value write. */
	uint32_t offset;
	/** Attenuation for value write. */
	uint32_t attenuation;
	/** Channel Calibration data. */
	struct capi_dac_chan_calibration calibration;
	/** This is intended to store DAC channel specific configurations. */
	void *extra;
};

/**
 * @struct capi_dac_chan_handle
 * @brief Structure that contains the context of a DAC channel.
 */
struct capi_dac_chan_handle {
	/** DAC handle */
	struct capi_dac_handle *dac_handle;
	/** Channel identifier. */
	uint32_t chan_id;
	/** Data pointer. */
	uint32_t *data;
	/** Callback pointer. */
	capi_dac_callback_t callback;
	/** Callback argument. */
	void *callback_arg;
	/** Extra information if needed */
	void *extra;
};

/**
 * @brief Initialize an instance of the DAC.
 *
 * @param [in out] handle Points to a pointer of the context structure. If this
 *                 pointer is set to NULL, then the DAC will allocate
 *	               the context structure and be the owner. If the pointer
 *	               is not NULL, the application has allocated the structure and is
 *	               the owner.
 * @param [in] config Points to the configuration for the DAC.
 * @return int 0 for success or error code.
 */
int capi_dac_init(struct capi_dac_handle **handle, struct capi_dac_config *config);

/**
 * @brief Deinitialize the DAC, disable, and bring to default settings.
 *
 * @param [in] handle Points to the DAC context.
 * @return int 0 for success or error code.
 */
int capi_dac_deinit(struct capi_dac_handle *handle);

/**
 * @brief Power up the DAC.
 *
 * @param [in] handle Points to the DAC context.
 * @return int 0 for success or error code.
 */
int capi_dac_power_on(struct capi_dac_handle *handle);

/**
 * @brief Power off the DAC.
 *
 * @param [in] handle Points to the DAC context.
 * @return int 0 for success or error code.
 */
int capi_dac_power_off(struct capi_dac_handle *handle);

/**
 * @brief Set the voltage reference source to internal or external sources.
 *
 * @param [in] handle Points to the DAC context.
 * @param [in] src The source of the Voltage reference.
 * @return int 0 for success or error code.
 */
int capi_dac_set_vref_source(struct capi_dac_handle *handle, enum capi_dac_vref_src src);

/**
 * @brief Get the current voltage reference source.
 *
 * @param [in] handle Points to the DAC context.
 * @param [out] src Points to where the source is stored.
 * @return int 0 for success or error code.
 */
int capi_dac_get_vref_source(struct capi_dac_handle *handle, enum capi_dac_vref_src *src);

/**
 * @brief Get the current voltage reference source.
 *
 * @param [in] handle Points to the DAC context.
 * @param [in] cal_data Points to the calibration data.
 * @return int 0 for success or error code.
 */
int capi_dac_set_calibration(struct capi_dac_handle *handle, uint8_t *cal_data);

/**
 * @brief Initialize an DAC channel.
 *
 * @param [in out] handle Points to the DAC channel context.
 * @param [in] config Points to the channel configuration parameters.
 * @return int 0 for success or error code.
 */
int capi_dac_chan_init(struct capi_dac_chan_handle **handle,
		       const struct capi_dac_chan_config *config);

/**
 * @brief Disable the DAC channel and
 *
 * @param [in] handle Points to the DAC channel context.
 * @return int 0 for success or error code.
 */
int capi_dac_chan_deinit(struct capi_dac_chan_handle *handle);

/**
 * @brief Set Offset for DAC channel
 *
 * @param [in] handle Points to the DAC channel context.
 * @param [in] offset Value added to the DAC value.
 * @return int 0 for success or error code.
 */
int capi_dac_chan_set_offset(struct capi_dac_chan_handle *handle, uint32_t offset);

/**
 * @brief Get the currrent offset setting for DAC channel
 *
 * @param [in] handle Points to the DAC channel context.
 * @param [out] offset Current value added to the DAC value.
 * @return int 0 for success or error code.
 */
int capi_dac_chan_get_offset(struct capi_dac_chan_handle *handle, uint32_t *offset);

/**
 * @brief Set attenuation for DAC channel
 *
 * @param [in] handle Points to the DAC channel context.
 * @param [in] attenuation Value multiplied to the DAC value.
 * @return int 0 for success or error code.
 */
int capi_dac_chan_set_attenuation(struct capi_dac_chan_handle *handle, uint32_t attenuation);

/**
 * @brief Get the currrent attenuation setting for DAC channel
 *
 * @param [in] handle Points to the DAC channel context.
 * @param [in] attenuation Current value multiplied to the DAC value.
 * @return int 0 for success or error code.
 */
int capi_dac_chan_get_attenuation(struct capi_dac_chan_handle *handle, uint32_t *attenuation);

/**
 * @brief Writes a sample to a DAC channel.
 *
 * @param [in] handle Points to the DAC channel context.
 * @param [in] data Points to location where DAC data is stored.
 * @param [in] wr_cnt num data Points to be written to DAC channel.
 * @return int 0 for success or error code.
 */
int capi_dac_chan_write(struct capi_dac_chan_handle *handle, void *data, int wr_cnt);

/**
 * @brief Some DACs have different data interfaces that continuously stream data lije DMA engine.
 * This function starts the sample stream.
 *
 * @param [in] handle Points to the DAC channel context.
 * @return int 0 for success or error code.
 */
int capi_dac_chan_stream_enable(struct capi_dac_chan_handle *handle);

/**
 * @brief Stops the DAC sample stream.
 *
 * @param [in] handle Points to the ADC channel context.
 * @return int 0 for success or error code.
 */
int capi_dac_chan_stream_disable(struct capi_dac_chan_handle *handle);

/**
 * @brief attach callback to DAC
 *
 * @param [in] handle Points to the ADC channel context.
 * @param [in] cb callback function
 * @return int 0 for success or error code.
 */
int capi_dac_attach_callback(struct capi_dac_chan_handle *handle, capi_dac_callback_t cb);

/**
 * @struct capi_dac_ops
 * @brief Operation jump table for the DAC.
 */
struct capi_dac_ops {
	/** See capi_dac_init() */
	int (*init)(struct capi_dac_handle **handle, const struct capi_dac_config *config);
	/** See capi_dac_deinit() */
	int (*deinit)(struct capi_dac_handle *handle);
	/** See capi_dac_power_on() */
	int (*power_on)(struct capi_dac_handle *handle);
	/** See capi_dac_power_off() */
	int (*power_off)(struct capi_dac_handle *handle);
	/** See capi_dac_set_vref_source() */
	int (*set_vref_source)(struct capi_dac_handle *handle, enum capi_dac_vref_src src);
	/** See capi_dac_get_vref_source() */
	int (*get_vref_source)(struct capi_dac_handle *handle, enum capi_dac_vref_src *src);
	/** See capi_dac_set_calibration() */
	int (*set_calibration)(struct capi_dac_handle *handle, uint8_t *cal_data);
	/** See capi_dac_chan_init() */
	int (*chan_init)(struct capi_dac_chan_handle **handle,
			 const struct capi_dac_chan_config *config);
	/** See capi_dac_chan_deinit() */
	int (*chan_deinit)(struct capi_dac_chan_handle *handle);
	/** See capi_dac_chan_set_offset() */
	int (*chan_set_offset)(struct capi_dac_chan_handle *handle, uint32_t offset);
	/** See capi_dac_chan_get_offset() */
	int (*chan_get_offset)(struct capi_dac_chan_handle *handle, uint32_t *offset);
	/** See capi_dac_chan_set_attenuation() */
	int (*chan_set_attenuation)(struct capi_dac_chan_handle *handle, uint32_t attenuation);
	/** See capi_dac_chan_get_attenuation() */
	int (*chan_get_attenuation)(struct capi_dac_chan_handle *handle, uint32_t *attenuation);
	/** See capi_dac_chan_write() */
	int (*chan_write)(struct capi_dac_chan_handle *handle, void *data, int wr_cnt);
	/** See capi_dac_chan_stream_enable() */
	int (*chan_stream_enable)(struct capi_dac_chan_handle *handle);
	/** See capi_dac_chan_stream_disable() */
	int (*chan_stream_disable)(struct capi_dac_chan_handle *handle);
	/** See capi_dac_chan_set_calibration() */
	int (*chan_set_calibration)(const struct capi_dac_chan_handle *handle,
				    struct capi_dac_chan_calibration *cal);
	/** See capi_dac_attach_callback() */
	int (*capi_dac_attach_callback)(struct capi_dac_chan_handle *handle,
					capi_dac_callback_t cb);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_PLL_H_ */
