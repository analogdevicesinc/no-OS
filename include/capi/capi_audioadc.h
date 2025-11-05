/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The Audio ADC API
 * *
 * @brief Audio ADC block is a multi-channel analog-to-digital converter composed
 * of analog and digital hardware domains working together. ADCs have programmable
 * digital filters which allow balancing filtering quality and latency responses,
 * depending on the targeted application
 *
 * ALC(Automatic Level Control):
 *  If supported, ALC enables a self-adaptive sound level control
 *
 * WNF(Wind Noise Filter) :
 *  If supported, WNF is programmed to reduce wind noise for windy environments
 */

#ifndef CAPI_AUDIOADC_H
#define CAPI_AUDIOADC_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief Audio ADC Main clock frequency
 */
enum capi_audioadc_mclk_freq {
	CAPI_AUDIOADC_MCLK_FREQ_4096K,  /**< Main clock  4.0960MHz */
	CAPI_AUDIOADC_MCLK_FREQ_5644K,  /**< Main clock  5.6448MHz */
	CAPI_AUDIOADC_MCLK_FREQ_6144K,  /**< Main clock  6.1440MHz */
	CAPI_AUDIOADC_MCLK_FREQ_8192K,  /**< Main clock  8.1920MHz */
	CAPI_AUDIOADC_MCLK_FREQ_11289K, /**< Main clock 11.2896MHz */
	CAPI_AUDIOADC_MCLK_FREQ_12288K, /**< Main clock 12.2880MHz */
	CAPI_AUDIOADC_MCLK_FREQ_16384K, /**< Main clock 16.3840MHz */
	CAPI_AUDIOADC_MCLK_FREQ_16934K, /**< Main clock 16.9344MHz */
	CAPI_AUDIOADC_MCLK_FREQ_18432K, /**< Main clock 18.4320MHz */
	CAPI_AUDIOADC_MCLK_FREQ_22579K, /**< Main clock 22.5792MHz */
	CAPI_AUDIOADC_MCLK_FREQ_24576K, /**< Main clock 24.5760MHz */
	CAPI_AUDIOADC_MCLK_FREQ_33868K, /**< Main clock 33.8688MHz */
	CAPI_AUDIOADC_MCLK_FREQ_MAX     /**< Max main clock */
};

/**
 * @brief Audio ADC sample rate
 */
enum capi_audioadc_sample_rate {
	CAPI_AUDIOADC_SAMPLE_RATE_FS8000,   /**< Sampling Rate :   8.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS11025,  /**< Sampling Rate :  11.025KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS12000,  /**< Sampling Rate :  12.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS16000,  /**< Sampling Rate :  16.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS22050,  /**< Sampling Rate :  22.050KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS24000,  /**< Sampling Rate :  24.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS32000,  /**< Sampling Rate :  32.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS44100,  /**< Sampling Rate :  44.100KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS48000,  /**< Sampling Rate :  48.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS96000,  /**< Sampling Rate :  96.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS192000, /**< Sampling Rate : 192.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS384000, /**< Sampling Rate : 384.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_FS768000, /**< Sampling Rate : 768.000KHz */
	CAPI_AUDIOADC_SAMPLE_RATE_MAX       /**< Max Sampling Rate */
};

/**
 * @brief Audio ADC data word size type
 */
enum capi_audioadc_data_word_size {
	CAPI_AUDIOADC_DATA_WORD_SIZE_8bit,  /**<  8bit */
	CAPI_AUDIOADC_DATA_WORD_SIZE_10bit, /**< 10bit */
	CAPI_AUDIOADC_DATA_WORD_SIZE_12bit, /**< 12bit */
	CAPI_AUDIOADC_DATA_WORD_SIZE_16bit, /**< 16bit */
	CAPI_AUDIOADC_DATA_WORD_SIZE_18bit, /**< 18bit */
	CAPI_AUDIOADC_DATA_WORD_SIZE_20bit, /**< 20bit */
	CAPI_AUDIOADC_DATA_WORD_SIZE_24bit, /**< 24bit */
	CAPI_AUDIOADC_DATA_WORD_SIZE_32bit, /**< 32bit */
	CAPI_AUDIOADC_DATA_WORD_SIZE_MAX    /**< Max Data word Size */
};

/**
 * @brief Audio ADC channels
 */
enum capi_audioadc_chan {
	CAPI_AUDIOADC_CHAN_0,  /**< Channel 0 (in Channel Pair 0) */
	CAPI_AUDIOADC_CHAN_1,  /**< Channel 1 (in Channel Pair 0) */
	CAPI_AUDIOADC_CHAN_2,  /**< Channel 2 (in Channel Pair 1) */
	CAPI_AUDIOADC_CHAN_3,  /**< Channel 3 (in Channel Pair 1) */
	CAPI_AUDIOADC_CHAN_4,  /**< Channel 4 (in Channel Pair 2) */
	CAPI_AUDIOADC_CHAN_5,  /**< Channel 5 (in Channel Pair 2) */
	CAPI_AUDIOADC_CHAN_6,  /**< Channel 6 (in Channel Pair 3) */
	CAPI_AUDIOADC_CHAN_7,  /**< Channel 7 (in Channel Pair 3) */
	CAPI_AUDIOADC_CHAN_MAX /**< Max Channel maximum */
};

/**
 * @brief Audio ADC channel pairs
 */
enum capi_audioadc_chan_pair {
	CAPI_AUDIOADC_CHAN_PAIR_0,  /**< Channel pair 0 */
	CAPI_AUDIOADC_CHAN_PAIR_1,  /**< Channel pair 1 */
	CAPI_AUDIOADC_CHAN_PAIR_2,  /**< Channel pair 2 */
	CAPI_AUDIOADC_CHAN_PAIR_3,  /**< Channel pair 3 */
	CAPI_AUDIOADC_CHAN_PAIR_MAX /**< Max channel pair */
};

/**
 * @brief Audio ADC data interface protocol
 */
enum capi_audioadc_data_interface_protocol {
	CAPI_AUDIOADC_DATA_INTERFACE_PROTOCOL_I2S,      /**< I2S */
	CAPI_AUDIOADC_DATA_INTERFACE_PROTOCOL_PARALLEL, /**< Parallel */
	CAPI_AUDIOADC_DATA_INTERFACE_PROTOCOL_MAX       /**< Max data interface protocol */
};

/**
 * @brief Audio ADC I2S pairing
 */
enum capi_audioadc_i2s_pairing {
	CAPI_AUDIOADC_I2S_PAIRING_RIGHT_FIRST, /**< Right first */
	CAPI_AUDIOADC_I2S_PAIRING_LEFT_FIRST,  /**< Left first */
	CAPI_AUDIOADC_I2S_PAIRING_MAX          /**< Max I2S pairing */
};

/**
 * @brief Audio ADC operating power modes
 */
enum capi_audioadc_op_power_mode {
	CAPI_AUDIOADC_OP_POWER_MODE_NORMAL,   /**< Operating power mode normal */
	CAPI_AUDIOADC_OP_POWER_MODE_LOW,      /**< Operating power mode low */
	CAPI_AUDIOADC_OP_POWER_MODE_ULTRALOW, /**< Operating power mode ultra low */
	CAPI_AUDIOADC_OP_POWER_MODE_MAX       /**< Max operating mode */
};

/**
 * @brief Audio ADC filter latency
 */
enum capi_audioadc_filter_latency {
	CAPI_AUDIOADC_FILTER_LATENCY_HIGH,        /**< High latency filtering */
	CAPI_AUDIOADC_FILTER_LATENCY_BALANCED,    /**< Balanced latency filtering */
	CAPI_AUDIOADC_FILTER_LATENCY_LOW,         /**< Low latency filtering */
	CAPI_AUDIOADC_FILTER_LATENCY_ULTRA_LOW,   /**< Ultra low latency filtering */
	CAPI_AUDIOADC_FILTER_LATENCY_EXTREME_LOW, /**< Extreme low latency filtering */
	CAPI_AUDIOADC_FILTER_LATENCY_MAX          /**< Max Latency */
};

/**
 * @brief Audio ADC H/W blocks domain type
 */
enum capi_audioadc_hw_domain {
	CAPI_AUDIOADC_HW_DOMAIN_ANALOG,  /**< Analog hw blocks only */
	CAPI_AUDIOADC_HW_DOMAIN_DIGITAL, /**< Digital hw blocks only */
	CAPI_AUDIOADC_HW_DOMAIN_BOTH,    /**< Analog and digital hw blocks */
	CAPI_AUDIOADC_HW_DOMAIN_MAX      /**< Max domain type */
};

/**
 * @brief Audio ADC hw blocks bypass modes
 */
enum capi_audioadc_bypass_mode {
	CAPI_AUDIOADC_BYPASS_MODE_NO_BYPASS,              /**< Bypass mode no bypass */
	CAPI_AUDIOADC_BYPASS_MODE_DIGITAL_FILTERS_BYPASS, /**< Bypass mode digital filters bypass */
	CAPI_AUDIOADC_BYPASS_MODE_ANALOG_PART_BYPASS,     /**< Bypass mode analog part bypass */
	CAPI_AUDIOADC_BYPASS_MODE_MAX                     /**< Max bypass mode */
};

/**
 * @brief Audio ADC Digital Filters gain value
 */
enum capi_audioadc_digital_filters_gain {
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M64DB, /**< -64dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M63DB, /**< -63dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M62DB, /**< -62dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M61DB, /**< -61dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M60DB, /**< -60dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M59DB, /**< -59dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M58DB, /**< -58dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M57DB, /**< -57dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M56DB, /**< -56dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M55DB, /**< -55dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M54DB, /**< -54dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M53DB, /**< -53dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M52DB, /**< -52dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M51DB, /**< -51dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M50DB, /**< -50dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M49DB, /**< -49dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M48DB, /**< -48dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M47DB, /**< -47dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M46DB, /**< -46dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M45DB, /**< -45dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M44DB, /**< -44dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M43DB, /**< -43dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M42DB, /**< -42dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M41DB, /**< -41dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M40DB, /**< -40dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M39DB, /**< -39dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M38DB, /**< -38dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M37DB, /**< -37dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M36DB, /**< -36dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M35DB, /**< -35dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M34DB, /**< -34dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M33DB, /**< -33dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M32DB, /**< -32dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M31DB, /**< -31dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M30DB, /**< -30dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M29DB, /**< -29dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M28DB, /**< -28dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M27DB, /**< -27dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M26DB, /**< -26dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M25DB, /**< -25dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M24DB, /**< -24dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M23DB, /**< -23dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M22DB, /**< -22dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M21DB, /**< -21dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M20DB, /**< -20dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M19DB, /**< -19dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M18DB, /**< -18dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M17DB, /**< -17dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M16DB, /**< -16dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M15DB, /**< -15dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M14DB, /**< -14dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M13DB, /**< -13dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M12DB, /**< -12dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M11DB, /**< -11dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M10DB, /**< -10dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M9DB,  /**<  -9dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M8DB,  /**<  -8dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M7DB,  /**<  -7dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M6DB,  /**<  -6dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M5DB,  /**<  -5dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M4DB,  /**<  -4dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M3DB,  /**<  -3dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M2DB,  /**<  -2dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_M1DB,  /**<  -1dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_0DB,   /**<   0dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_1DB,   /**<   1dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_2DB,   /**<   2dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_3DB,   /**<   3dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_4DB,   /**<   4dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_5DB,   /**<   5dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_6DB,   /**<   6dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_7DB,   /**<   7dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_8DB,   /**<   8dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_9DB,   /**<   9dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_10DB,  /**<  10dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_11DB,  /**<  11dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_12DB,  /**<  12dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_13DB,  /**<  13dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_14DB,  /**<  14dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_15DB,  /**<  15dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_16DB,  /**<  16dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_17DB,  /**<  17dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_18DB,  /**<  18dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_19DB,  /**<  19dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_20DB,  /**<  20dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_21DB,  /**<  21dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_22DB,  /**<  22dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_23DB,  /**<  23dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_24DB,  /**<  24dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_25DB,  /**<  25dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_26DB,  /**<  26dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_27DB,  /**<  27dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_28DB,  /**<  28dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_29DB,  /**<  29dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_30DB,  /**<  30dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_31DB,  /**<  31dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_32DB,  /**<  32dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_33DB,  /**<  33dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_34DB,  /**<  34dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_35DB,  /**<  35dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_36DB,  /**<  36dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_37DB,  /**<  37dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_38DB,  /**<  38dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_39DB,  /**<  39dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_40DB,  /**<  40dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_41DB,  /**<  41dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_42DB,  /**<  42dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_43DB,  /**<  43dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_44DB,  /**<  44dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_45DB,  /**<  45dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_46DB,  /**<  46dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_47DB,  /**<  47dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_48DB,  /**<  48dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_49DB,  /**<  49dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_50DB,  /**<  50dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_51DB,  /**<  51dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_52DB,  /**<  52dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_53DB,  /**<  53dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_54DB,  /**<  54dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_55DB,  /**<  55dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_56DB,  /**<  56dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_57DB,  /**<  57dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_58DB,  /**<  58dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_59DB,  /**<  59dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_60DB,  /**<  60dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_61DB,  /**<  61dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_62DB,  /**<  62dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_63DB,  /**<  63dB */
	CAPI_AUDIOADC_DIGITAL_FILTERS_GAIN_MAX    /**<  Max digital filters gain value */
};

/**
 * @brief Audio ADC channel pair configuration Structure
 */
struct capi_audioadc_chan_pair_cfg {
	/** Data interface master or slave */
	bool data_interface_master;
	/** Sampling rate */
	enum capi_audioadc_sample_rate sample_rate;
	/** Data word size */
	enum capi_audioadc_data_word_size data_word_size;
	/** Data interface protocol */
	enum capi_audioadc_data_interface_protocol data_interface_protocol;
	/** I2S pairing */
	enum capi_audioadc_i2s_pairing i2s_pairing;
	/** Operating power mode */
	enum capi_audioadc_op_power_mode operating_power_mode;
	/** Filtering Latency/Quality balance */
	enum capi_audioadc_filter_latency filter_latency;
};

/**
 * @brief Audio ADC configuration
 */
struct capi_audioadc_config {
	/** Base address */
	uint64_t identifier;
	/** Platform specific Audio ADC platform ops structure */
	const struct capi_audioadc_ops *ops;
	/** This is intended to store Audio ADC specific configurations */
	void *extra;
};

/**
 * @brief Audio ADC handle
 *
 * Drivers may need own handle type to handle internals
 * Driver developer shall declare this as the first field of private handle structure
 */
struct capi_audioadc_handle {
	/** Audio ADC ID */
	uint32_t audioadc_id;
	/** Audio ADC channel count */
	uint32_t chan_count;
	/** Audio ADC channel pair count */
	uint32_t chan_pair_count;
	/** Audio ADC ALC(Automatic Level Control) support */
	bool alc_supported;
	/** Audio ADC WNF(Wind Noise Filter) support */
	bool wnf_supported;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_audioadc_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Initialize Audio ADC
 *
 * @param[in] handle Pointer to the Audio ADC handle pointer
 * @param[in] config Pointer to Audio ADC configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_init(struct capi_audioadc_handle **handle,
		       const struct capi_audioadc_config *config);

/**
 * @brief Deinitialize Audio ADC
 *
 * @param[in] handle Pointer to the Audio ADC handle
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_deinit(const struct capi_audioadc_handle *handle);

/**
 * @brief Configure main clock frequency
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] mclk_freq Clock freq.
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_clk_config(const struct capi_audioadc_handle *handle,
			     enum capi_audioadc_mclk_freq mclk_freq);

/**
 * @brief Internal H/W domain enable
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] domain Internal domain(Analog or Digital)
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_hw_domain_enable(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_hw_domain domain);

/**
 * @brief Internal H/W domain disable
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] domain Internal domain(Analog or Digital)
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_hw_domain_disable(const struct capi_audioadc_handle *handle,
				    enum capi_audioadc_hw_domain domain);

/**
 * @brief Internal blocks bypass mode
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] bypass_mode Internal blocks
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_set_bypass_mode(const struct capi_audioadc_handle *handle,
				  enum capi_audioadc_bypass_mode bypass_mode);

/**
 * @brief Get the status of power on acknowledge
 *
 * This function checks if the Audio ADC device is ready to operate by verifying the power on
 * acknowledge status
 *
 * @param[in] handle Pointer to the Audio ADC handle.
 * @param[out] pon_ack Pointer to a boolean that will be set to true if the device is ready to
 * operate, false otherwise
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_get_power_on_ack(const struct capi_audioadc_handle *handle, bool *pon_ack);

/**
 * @brief Resync the Audio ADC
 *
 * This function performs ADC stream synchronization, allowing phase-aligned operation for different
 * ADC streams with the same Fs/filtering configurations. It ensures that multiple ADC streams
 * operate in sync
 *
 * @param[in] handle Pointer to the Audio ADC handle.
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_resync(const struct capi_audioadc_handle *handle);

/**
 * @brief Configure channel pair
 *
 * @param[in] handle Pointer to the Audio ADC handle.
 * @param[in] chan_pair_id Required channel pair
 * @param[in] chan_pair_cfg Pointer to config structure for the channel pair
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_pair_config(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_chan_pair chan_pair_id,
				   const struct capi_audioadc_chan_pair_cfg *chan_pair_cfg);

/**
 * @brief Activate channel pair
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_pair_id Required channel pair
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_pair_activate(const struct capi_audioadc_handle *handle,
				     enum capi_audioadc_chan_pair chan_pair_id);

/**
 * @brief Deactivate channel pair
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_pair_id Required channel pair
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_pair_deactivate(const struct capi_audioadc_handle *handle,
				       enum capi_audioadc_chan_pair chan_pair_id);

/**
 * @brief Set operational power mode
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_pair_id Required channel pair
 * @param[in] power_mode Required power mode
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_pair_set_operational_power_mode(const struct capi_audioadc_handle *handle,
						       enum capi_audioadc_chan_pair chan_pair_id,
						       enum capi_audioadc_op_power_mode power_mode);

/**
 * @brief Get operational power mode
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_pair_id Required channel pair
 * @param[out] power_mode Pointer to power mode
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_pair_get_operational_power_mode(
	const struct capi_audioadc_handle *handle, enum capi_audioadc_chan_pair chan_pair_id,
	enum capi_audioadc_op_power_mode *power_mode);

/**
 * @brief Activate channel
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_activate(const struct capi_audioadc_handle *handle,
				enum capi_audioadc_chan chan_id);

/**
 * @brief Deactivate channel
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_deactivate(const struct capi_audioadc_handle *handle,
				  enum capi_audioadc_chan chan_id);

/**
 * @brief Deactivate all channels
 *
 * @param[in] handle Pointer to the Audio ADC handle
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_deactivate_all(const struct capi_audioadc_handle *handle);

/**
 * @brief Activate mic bias
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_activate_mic_bias(const struct capi_audioadc_handle *handle,
					 enum capi_audioadc_chan chan_id);

/**
 * @brief Deactivate mic bias
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_deactivate_mic_bias(const struct capi_audioadc_handle *handle,
					   enum capi_audioadc_chan chan_id);

/**
 * @brief Soft mute channel
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_soft_mute(const struct capi_audioadc_handle *handle,
				 enum capi_audioadc_chan chan_id);

/**
 * @brief Soft unmute channel
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_soft_unmute(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_chan chan_id);

/**
 * @brief Hard mute channel
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_hard_mute(const struct capi_audioadc_handle *handle,
				 enum capi_audioadc_chan chan_id);

/**
 * @brief Hard unmute channel
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_hard_unmute(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_chan chan_id);

/**
 * @brief Set channel digital gain value
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 * @param[in] gain_value Digital gain value for the channel
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_set_digital_gain(const struct capi_audioadc_handle *handle,
					enum capi_audioadc_chan chan_id,
					enum capi_audioadc_digital_filters_gain gain_value);

/**
 * @brief Activate HPF(High Pass Filter) for channel
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_enable_hpf(const struct capi_audioadc_handle *handle,
				  enum capi_audioadc_chan chan_id);

/**
 * @brief Disable HPF(High Pass Filter) for channel
 *
 * @param[in] handle Pointer to the Audio ADC handle
 * @param[in] chan_id Required channel id.
 *
 * @return int 0 for success or error code
 */
int capi_audioadc_chan_disable_hpf(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_chan chan_id);

/**
 * @brief Structure holding Audio ADC function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_audioadc_ops {
	/** See capi_audioadc_init() */
	int (*init)(struct capi_audioadc_handle **handle,
		    const struct capi_audioadc_config *config);
	/** See capi_audioadc_deinit() */
	int (*deinit)(const struct capi_audioadc_handle *handle);
	/** See capi_audioadc_clk_config() */
	int (*clk_config)(const struct capi_audioadc_handle *handle,
			  enum capi_audioadc_mclk_freq mclk_freq);
	/** See capi_audioadc_hw_domain_enable() */
	int (*hw_domain_enable)(const struct capi_audioadc_handle *handle,
				enum capi_audioadc_hw_domain domain);
	/** See capi_audioadc_hw_domain_disable() */
	int (*hw_domain_disable)(const struct capi_audioadc_handle *handle,
				 enum capi_audioadc_hw_domain domain);
	/** See capi_audioadc_set_bypass_mode() */
	int (*set_bypass_mode)(const struct capi_audioadc_handle *handle,
			       enum capi_audioadc_bypass_mode bypass_mode);
	/** See capi_audioadc_get_power_on_ack() */
	int (*get_power_on_ack)(const struct capi_audioadc_handle *handle, bool *pon_ack);
	/** See capi_audioadc_resync() */
	int (*resync)(const struct capi_audioadc_handle *handle);
	/** See capi_audioadc_chan_pair_config() */
	int (*chan_pair_config)(const struct capi_audioadc_handle *handle,
				enum capi_audioadc_chan_pair chan_pair_id,
				const struct capi_audioadc_chan_pair_cfg *chan_pair_cfg);
	/** See capi_audioadc_chan_pair_activate() */
	int (*chan_pair_activate)(const struct capi_audioadc_handle *handle,
				  enum capi_audioadc_chan_pair chan_pair_id);
	/** See capi_audioadc_chan_pair_deactivate() */
	int (*chan_pair_deactivate)(const struct capi_audioadc_handle *handle,
				    enum capi_audioadc_chan_pair chan_pair_id);
	/** See capi_audioadc_chan_pair_set_operational_power_mode() */
	int (*chan_pair_set_operational_power_mode)(const struct capi_audioadc_handle *handle,
						    enum capi_audioadc_chan_pair chan_pair_id,
						    enum capi_audioadc_op_power_mode power_mode);
	/** See capi_audioadc_chan_pair_get_operational_power_mode() */
	int (*chan_pair_get_operational_power_mode)(const struct capi_audioadc_handle *handle,
						    enum capi_audioadc_chan_pair chan_pair_id,
						    enum capi_audioadc_op_power_mode *power_mode);
	/** See capi_audioadc_chan_activate() */
	int (*chan_activate)(const struct capi_audioadc_handle *handle,
			     enum capi_audioadc_chan chan_id);
	/** See capi_audioadc_chan_deactivate() */
	int (*chan_deactivate)(const struct capi_audioadc_handle *handle,
			       enum capi_audioadc_chan chan_id);
	/** See capi_audioadc_chan_deactivate_all() */
	int (*chan_deactivate_all)(const struct capi_audioadc_handle *handle);
	/** See capi_audioadc_chan_activate_mic_bias() */
	int (*chan_activate_mic_bias)(const struct capi_audioadc_handle *handle,
				      enum capi_audioadc_chan chan_id);
	/** See capi_audioadc_chan_deactivate_mic_bias() */
	int (*chan_deactivate_mic_bias)(const struct capi_audioadc_handle *handle,
					enum capi_audioadc_chan chan_id);
	/** See capi_audioadc_chan_soft_mute() */
	int (*chan_soft_mute)(const struct capi_audioadc_handle *handle,
			      enum capi_audioadc_chan chan_id);
	/** See capi_audioadc_chan_soft_unmute() */
	int (*chan_soft_unmute)(const struct capi_audioadc_handle *handle,
				enum capi_audioadc_chan chan_id);
	/** See capi_audioadc_chan_hard_mute() */
	int (*chan_hard_mute)(const struct capi_audioadc_handle *handle,
			      enum capi_audioadc_chan chan_id);
	/** See capi_audioadc_chan_hard_unmute() */
	int (*chan_hard_unmute)(const struct capi_audioadc_handle *handle,
				enum capi_audioadc_chan chan_id);
	/** See capi_audioadc_chan_set_digital_gain() */
	int (*chan_set_digital_gain)(const struct capi_audioadc_handle *handle,
				     enum capi_audioadc_chan chan_id,
				     enum capi_audioadc_digital_filters_gain gain_value);
	/** See capi_audioadc_chan_enable_hpf() */
	int (*chan_enable_hpf)(const struct capi_audioadc_handle *handle,
			       enum capi_audioadc_chan chan_id);
	/** See capi_audioadc_chan_disable_hpf() */
	int (*chan_disable_hpf)(const struct capi_audioadc_handle *handle,
				enum capi_audioadc_chan chan_id);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_AUDIOADC_H */
