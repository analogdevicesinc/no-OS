/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_adc.h
 * @brief Common HAL ADC. This is the generic portion of an ADC which
 *        will be covered by all ADC drivers. Other specific features will be
 *        extend the interface based on the HW design.
 */

#ifndef _CAPI_ADC_H_
#define _CAPI_ADC_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Enumeration of the ADC event that caused a call to the
 *        asynchronous callback function.
 */
enum capi_adc_event {
	CAPI_ADC_EVENT_DATA_RDY, /**< Data read is complete. */
	CAPI_ADC_EVENT_TIMEOUT,  /**< A timeout event occured on the interface. */
	CAPI_ADC_EVENT_MAX       /**< Max value. */
};

/**
 * @brief Enumeration of the ADC Clock which can be either
 *         from an internal source or an external source
 */
enum capi_adc_clock {
	CAPI_ADC_INTERNAL_CLOCK, /**< Internal clock source. */
	CAPI_ADC_EXTERNAL_CLOCK, /**< External clock source. */
	CAPI_ADC_CLOCK_MAX       /**< Max value. */
};

/**
 * @brief Enumeration of the clock polarity of the input clock.
 */
enum capi_adc_clock_polarity {
	CAPI_ADC_CLOCK_POLARITY_NEG, /**< Negative polarity. */
	CAPI_ADC_CLOCK_POLARITY_POS, /**< Posifitve poloarity. */
	CAPI_ADC_CLOCK_POLARITY_MAX  /**< Max value. */
};

/**
 * @brief Enumeration of the clock polarity of the input clock.
 */
enum capi_adc_vref_src {
	CAPI_ADC_VREF_SOURCE_EXTERNAL, /**< VREF comes from external source. */
	CAPI_ADC_VREF_SOURCE_INTERNAL, /**< VREF generated internally. */
	CAPI_ADC_VREF_SOURCE_MAX       /**< Max value. */
};

/**
 * @brief ADC Callback.
 *
 * @param [in] event Specifies the event that prompted the callback.
 * @param [in] arg Points to application defined argument.
 * @param [in] event_extra Optional information from the driver.
 */
typedef void (*capi_adc_callback_t)(enum capi_adc_event event, void *arg, int event_extra);

/**
 * @struct capi_adc_config
 * @brief Structure that holds the ADC configuration.
 */
struct capi_adc_config {
	/** Identifies the instance. */
	uint32_t identifier;
	/** Interrupt request ID. */
	uint32_t adc_ctrl_id;
	/** Input clock */
	enum capi_adc_clock input_clock;
	/** Clock Polarity */
	enum capi_adc_clock_polarity clock_polarity;
	/** Platform specific ADC platform ops structure. */
	const struct capi_adc_ops *adc_ops;
	/**
	 * This is intended to store ADC specific configurations,
	 * it should not be a reference to any peripheral descriptor.
	 */
	void *extra;
};

/**
 * @struct capi_adc_handle
 * @brief Structure for ADC context.
 */
struct capi_adc_handle {
	/** ADC ID. */
	uint32_t adc_id;
	/** If true, the driver is owner of handle memory. */
	bool init_allocated;
	/** Lock */
	void *lock;
	/** Platform specific ADC platform ops structure. */
	const struct capi_adc_ops *adc_ops;
	/** This is intended to store ADC specific configurations. */
	void *extra;
};

/**
 * @struct capi_adc_chan_calibration
 * @brief Structure that defines channel calibration information.
 */
struct capi_adc_chan_calibration {
	/** To be added to the read value. */
	uint32_t offset;
	/** Gain to multiplied to read value. */
	uint32_t gain;
	/** Phase to be used for offset into measurement time. */
	uint32_t phase;
};

/**
 * @struct capi_adc_chan_config
 * @brief Structure that holds the ADC Channel configuration.
 */
struct capi_adc_chan_config {
	/** ADC. */
	struct capi_adc_handle *adc_handle;
	/** If true, the driver is the owner of the handle memory. */
	bool init_allocated;
	/** Sample Rate in hertz*/
	uint32_t samples_hz;
	/** Identifies the channel. */
	uint32_t chan_id;
	/** Resolution in bits */
	uint8_t resolution;
	/** Adds offset to value read. */
	uint32_t offset;
	/** Gain value for value read. */
	uint32_t gain;
	/** Oversampling. */
	uint8_t osr;
	/** Channel Calibration data. */
	struct capi_adc_chan_calibration calibration;
	/** This is intended to store ADC channel specific configurations. */
	void *extra;
};

/**
 * @struct capi_adc_chan_handle
 * @brief Structure that contains the context of an ADC channel.
 */
struct capi_adc_chan_handle {
	/** ADC handle */
	struct capi_adc_handle *adc_handle;
	/** Channel identifier. */
	uint32_t chan_id;
	/** Data pointer. */
	uint32_t *data;
	/** Callback pointer. */
	capi_adc_callback_t callback;
	/** Callback argument. */
	void *callback_arg;
	/** Extra information if needed */
	void *extra;
};

/**
 * @struct capi_adc_ops
 * @brief Operation jump table for the ADC.
 */
struct capi_adc_ops {
	/** See capi_adc_init() */
	int (*init)(struct capi_adc_handle **handle, const struct capi_adc_config *config);
	/** See capi_adc_deinit() */
	int (*deinit)(struct capi_adc_handle *handle);
	/** See capi_adc_power_on() */
	int (*power_on)(struct capi_adc_handle *handle);
	/** See capi_adc_power_off() */
	int (*power_off)(struct capi_adc_handle *handle);
	/** See capi_adc_is_power_on() */
	bool (*is_power_on)(struct capi_adc_handle *handle);
	/** See capi_adc_set_vref_source() */
	int (*set_vref_source)(struct capi_adc_handle *handle, enum capi_adc_vref_src src);
	/** See capi_adc_get_vref_source() */
	int (*get_vref_source)(struct capi_adc_handle *handle, enum capi_adc_vref_src *src);
	/** See capi_adc_set_calibration() */
	int (*set_calibration)(struct capi_adc_handle *handle, uint8_t *cal_data);
	/** See capi_adc_chan_init() */
	int (*chan_init)(struct capi_adc_chan_handle **handle,
			 const struct capi_adc_chan_config *config);
	/** See capi_adc_chan_deinit() */
	int (*chan_deinit)(struct capi_adc_chan_handle *handle);
	/** See capi_adc_chan_set_offset() */
	int (*chan_set_offset)(struct capi_adc_chan_handle *handle, uint32_t offset);
	/** See capi_adc_chan_get_offset() */
	int (*chan_get_offset)(struct capi_adc_chan_handle *handle, uint32_t *offset);
	/** See capi_adc_chan_set_gain() */
	int (*chan_set_gain)(struct capi_adc_chan_handle *handle, uint32_t gain);
	/** See capi_adc_chan_get_gain() */
	int (*chan_get_gain)(struct capi_adc_chan_handle *handle, uint32_t *gain);
	/** See capi_adc_chan_read() */
	int (*chan_read)(struct capi_adc_chan_handle *handle, void *data);
	/** See capi_adc_chan_read_async() */
	int (*chan_read_async)(struct capi_adc_chan_handle *handle, capi_adc_callback_t callback,
			       void *data);
	/** See capi_adc_chan_stream_enable() */
	int (*chan_stream_enable)(struct capi_adc_chan_handle *handle);
	/** See capi_adc_chan_stream_disable() */
	int (*chan_stream_disable)(struct capi_adc_chan_handle *handle);
	/** See capi_adc_chan_set_calibration() */
	int (*chan_set_calibration)(const struct capi_adc_chan_handle *handle,
				    struct capi_adc_chan_calibration *cal);
	/** See capi_adc_chan_get_calibration() */
	int (*chan_get_calibration)(const struct capi_adc_chan_handle *handle,
				    struct capi_adc_chan_calibration *cal);
};

/**
 * @brief Initialize an instance of the ADC.
 *
 * @param [in out] handle Points to a pointer of the context structure. If this
 *                 pointer is set to NULL, then the ADC will allocate
 *	               the context structure and be the owner. If the pointer
 *	               is not NULL, the application has allocated the structure and is
 *	               the owner.
 * @param [in] config Points to the configuration for the ADC.
 * @return int 0 for success or error code.
 */
int capi_adc_init(struct capi_adc_handle **handle, struct capi_adc_config *config);

/**
 * @brief Deinitialize the ADC, disable, and bring to default settings.
 *
 * @param [in] handle Points to the ADC context.
 * @return int 0 for success or error code.
 */
int capi_adc_deinit(struct capi_adc_handle *handle);

/**
 * @brief Power up the ADC.
 *
 * @param [in] handle Points to the ADC context.
 * @return int 0 for success or error code.
 */
int capi_adc_power_on(struct capi_adc_handle *handle);

/**
 * @brief Power off the ADC.
 *
 * @param [in] handle Points to the ADC context.
 * @return int 0 for success or error code.
 */
int capi_adc_power_off(struct capi_adc_handle *handle);

/**
 * @brief Return the power state of the ADC.
 *
 * @param [in out] handle Points to the ADC context.
 * @return true if power on or false if power off.
 */
bool capi_adc_is_power_on(struct capi_adc_handle *handle);

/**
 * @brief Set the voltage reference source to internal or external sources.
 *
 * @param [in] handle Points to the ADC context.
 * @param [in] src The source of the Voltage reference.
 * @return int 0 for success or error code.
 */
int capi_adc_set_vref_source(struct capi_adc_handle *handle, enum capi_adc_vref_src src);

/**
 * @brief Get the current voltage reference source.
 *
 * @param [in] handle Points to the ADC context.
 * @param [out] src Points to where the source is stored.
 * @return int 0 for success or error code.
 */
int capi_adc_get_vref_source(struct capi_adc_handle *handle, enum capi_adc_vref_src *src);

/**
 * @brief Get the current voltage reference source.
 *
 * @param [in] handle Points to the ADC context.
 * @param [in] cal_data Points to the calibration data.
 * @return int 0 for success or error code.
 */
int capi_adc_set_calibration(struct capi_adc_handle *handle, uint8_t *cal_data);

/**
 * @brief Initialize an ADC channel.
 *
 * @param [in out] handle Points to the ADC channel context.
 * @param [in] config Points to the channel configuration parameters.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_init(struct capi_adc_chan_handle **handle,
		       const struct capi_adc_chan_config *config);

/**
 * @brief Disable the ADC channel and
 *
 * @param [in] handle Points to the ADC channel context.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_deinit(struct capi_adc_chan_handle *handle);

/**
 * @brief Disable the ADC channel and
 *
 * @param [in] handle Points to the ADC channel context.
 * @param [in] offset Value added to the ADC value.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_set_offset(struct capi_adc_chan_handle *handle, uint32_t offset);

/**
 * @brief Disable the ADC channel and
 *
 * @param [in] handle Points to the ADC channel context.
 * @param [out] offset Current value added to the ADC value.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_get_offset(struct capi_adc_chan_handle *handle, uint32_t *offset);

/**
 * @brief Disable the ADC channel and
 *
 * @param [in] handle Points to the ADC channel context.
 * @param [in] gain Value multiplied to the ADC value.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_set_gain(struct capi_adc_chan_handle *handle, uint32_t gain);

/**
 * @brief Disable the ADC channel and
 *
 * @param [in] handle Points to the ADC channel context.
 * @param [in] gain Current value multiplied to the ADC value.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_get_gain(struct capi_adc_chan_handle *handle, uint32_t *gain);

/**
 * @brief Reads a sample from the ADC channel.
 *
 * @param [in] handle Points to the ADC channel context.
 * @param [out] data Points to location where ADC data will be stored.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_read(struct capi_adc_chan_handle *handle, void *data);

/**
 * @brief Starts an non-blocking asynchronous read of data from the ADC channel. When
 *        the read is complete, the callback will be called.
 *
 * @param [in] handle Points to the ADC channel context.
 * @param [in] callback Defines the function to call when asynchronous data read is complete.
 * @param [in] data POints to the location where ADC data will be stored.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_read_async(struct capi_adc_chan_handle *handle, capi_adc_callback_t callback,
			     void *data);

/**
 * @brief Some ADCs have different data interfaces that continuously stream data. This function
 *        starts the sample stream.
 *
 * @param [in] handle Points to the ADC channel context.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_stream_enable(struct capi_adc_chan_handle *handle);

/**
 * @brief Stops the ADC sample stream.
 *
 * @param [in] handle Points to the ADC channel context.
 * @return int 0 for success or error code.
 */
int capi_adc_chan_stream_disable(struct capi_adc_chan_handle *handle);

/**
 * @brief Sets the calibration data for the specified channel.
 *
 * @param [in] handle Points to the ADC channel context.
 * @param [in] cal Holds the calibration data for the channel.
 * @return int 0 for success or error code.
 */
int chan_set_calibration(const struct capi_adc_chan_handle *handle,
			 struct capi_adc_chan_calibration *cal);

/**
 * @brief Gets the calibration data for the specified channel.
 *
 * @param [in] handle Points to the ADC channel context.
 * @param [out] cal Holds the calibration data for the channel.
 * @return int 0 for success or error code.
 */
int chan_get_calibration(const struct capi_adc_chan_handle *handle,
			 struct capi_adc_chan_calibration *cal);

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_ADC_H_ */