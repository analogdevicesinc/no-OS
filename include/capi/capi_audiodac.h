/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Audio DAC Common API
 *
 * The Audio DAC block is an multi-channel digital-to-analog converter.
 * A programmable capability of the digital filters allows balancing filtering
 * and latency responses, depending on the targeted application
 *
 * DRC(Dynamic Range Compression) :
 *  If supported, Soft Clipping DRC feature is used to reduce occurence of
 *  possible hard clipping events when input signal reaches fullscale
 */

#ifndef CAPI_AUDIODAC_H
#define CAPI_AUDIODAC_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief Audio DAC Main clock frequency
 */
enum capi_audiodac_mclk_freq {
	CAPI_AUDIODAC_MCLK_FREQ_4096K,  /**< Main clock  4.0960MHz */
	CAPI_AUDIODAC_MCLK_FREQ_5644K,  /**< Main clock  5.6448MHz */
	CAPI_AUDIODAC_MCLK_FREQ_6144K,  /**< Main clock  6.1440MHz */
	CAPI_AUDIODAC_MCLK_FREQ_8192K,  /**< Main clock  8.1920MHz */
	CAPI_AUDIODAC_MCLK_FREQ_11289K, /**< Main clock 11.2896MHz */
	CAPI_AUDIODAC_MCLK_FREQ_12288K, /**< Main clock 12.2880MHz */
	CAPI_AUDIODAC_MCLK_FREQ_16384K, /**< Main clock 16.3840MHz */
	CAPI_AUDIODAC_MCLK_FREQ_16934K, /**< Main clock 16.9344MHz */
	CAPI_AUDIODAC_MCLK_FREQ_18432K, /**< Main clock 18.4320MHz */
	CAPI_AUDIODAC_MCLK_FREQ_22579K, /**< Main clock 22.5792MHz */
	CAPI_AUDIODAC_MCLK_FREQ_24576K, /**< Main clock 24.5760MHz */
	CAPI_AUDIODAC_MCLK_FREQ_33868K, /**< Main clock 33.8688MHz */
	CAPI_AUDIODAC_MCLK_FREQ_MAX     /**< Max main clock */
};

/**
 * @brief Audio DAC H/W blocks domain type
 */
enum capi_audiodac_hw_domain {
	CAPI_AUDIODAC_HW_DOMAIN_ANALOG,  /**< Analog hw blocks only */
	CAPI_AUDIODAC_HW_DOMAIN_DIGITAL, /**< Digital hw blocks only */
	CAPI_AUDIODAC_HW_DOMAIN_BOTH,    /**< Analog and digital hw blocks */
	CAPI_AUDIODAC_HW_DOMAIN_MAX      /**< Max domain type */
};

/**
 * @brief Audio DAC filter latency
 */
enum capi_audiodac_filter_latency {
	CAPI_AUDIODAC_FILTER_LATENCY_HIGH,        /**< High latency filtering */
	CAPI_AUDIODAC_FILTER_LATENCY_BALANCED,    /**< Balanced latency filtering */
	CAPI_AUDIODAC_FILTER_LATENCY_LOW,         /**< Low latency filtering */
	CAPI_AUDIODAC_FILTER_LATENCY_ULTRA_LOW,   /**< Ultra low latency filtering */
	CAPI_AUDIODAC_FILTER_LATENCY_EXTREME_LOW, /**< Extreme low latency filtering */
	CAPI_AUDIODAC_FILTER_LATENCY_MAX          /**< Max Latency */
};

/**
 * @brief Audio DAC operating power modes
 */
enum capi_audiodac_op_power_mode {
	CAPI_AUDIODAC_OP_POWER_MODE_NORMAL,   /**< Operating power mode normal */
	CAPI_AUDIODAC_OP_POWER_MODE_LOW,      /**< Operating power mode low */
	CAPI_AUDIODAC_OP_POWER_MODE_ULTRALOW, /**< Operating power mode ultra low */
	CAPI_AUDIODAC_OP_POWER_MODE_MAX       /**< Max operating mode */
};

/**
 * @brief Audio DAC data word size type
 */
enum capi_audiodac_data_word_size {
	CAPI_AUDIODAC_DATA_WORD_SIZE_8bit,  /**<  8bit */
	CAPI_AUDIODAC_DATA_WORD_SIZE_10bit, /**< 10bit */
	CAPI_AUDIODAC_DATA_WORD_SIZE_12bit, /**< 12bit */
	CAPI_AUDIODAC_DATA_WORD_SIZE_16bit, /**< 16bit */
	CAPI_AUDIODAC_DATA_WORD_SIZE_18bit, /**< 18bit */
	CAPI_AUDIODAC_DATA_WORD_SIZE_20bit, /**< 20bit */
	CAPI_AUDIODAC_DATA_WORD_SIZE_24bit, /**< 24bit */
	CAPI_AUDIODAC_DATA_WORD_SIZE_32bit, /**< 32bit */
	CAPI_AUDIODAC_DATA_WORD_SIZE_MAX    /**< Max Data word Size */
};

/**
 * @brief Audio DAC channels
 */
enum capi_audiodac_chan {
	CAPI_AUDIODAC_CHAN_0,  /**< Channel 0 (in Channel Pair 0) */
	CAPI_AUDIODAC_CHAN_1,  /**< Channel 1 (in Channel Pair 0) */
	CAPI_AUDIODAC_CHAN_2,  /**< Channel 2 (in Channel Pair 1) */
	CAPI_AUDIODAC_CHAN_3,  /**< Channel 3 (in Channel Pair 1) */
	CAPI_AUDIODAC_CHAN_4,  /**< Channel 4 (in Channel Pair 2) */
	CAPI_AUDIODAC_CHAN_5,  /**< Channel 5 (in Channel Pair 2) */
	CAPI_AUDIODAC_CHAN_6,  /**< Channel 6 (in Channel Pair 3) */
	CAPI_AUDIODAC_CHAN_7,  /**< Channel 7 (in Channel Pair 3) */
	CAPI_AUDIODAC_CHAN_MAX /**< Max Channel maximum */
};

/**
 * @brief Audio DAC channel pairs
 */
enum capi_audiodac_chan_pair {
	CAPI_AUDIODAC_CHAN_PAIR_0,  /**< Channel pair 0 */
	CAPI_AUDIODAC_CHAN_PAIR_1,  /**< Channel pair 1 */
	CAPI_AUDIODAC_CHAN_PAIR_2,  /**< Channel pair 2 */
	CAPI_AUDIODAC_CHAN_PAIR_3,  /**< Channel pair 3 */
	CAPI_AUDIODAC_CHAN_PAIR_MAX /**< Max channel pair */
};

/**
 * @brief Audio DAC I2S paring
 */
enum capi_audiodac_i2s_pairing {
	CAPI_AUDIODAC_I2S_PAIRING_RIGHT_FIRST, /**< Right first */
	CAPI_AUDIODAC_I2S_PAIRING_LEFT_FIRST,  /**< Left first */
	CAPI_AUDIODAC_I2S_PAIRING_MAX          /**< Max I2S pairing */
};

/**
 * @brief Audio DAC data interface protocol
 */
enum capi_audiodac_data_interface_protocol {
	CAPI_AUDIODAC_DATA_INTERFACE_PROTOCOL_I2S,      /**< I2S */
	CAPI_AUDIODAC_DATA_INTERFACE_PROTOCOL_PARALLEL, /**< Parallel */
	CAPI_AUDIODAC_DATA_INTERFACE_PROTOCOL_MAX       /**< Max data interface protocol */
};

/**
 * @brief Audio DAC channel pair input sample rate
 */
enum capi_audiodac_sample_rate {
	CAPI_AUDIODAC_SAMPLE_RATE_FS8000,   /**< Sampling Rate :   8.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS11025,  /**< Sampling Rate :  11.025KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS12000,  /**< Sampling Rate :  12.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS16000,  /**< Sampling Rate :  16.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS22050,  /**< Sampling Rate :  22.050KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS24000,  /**< Sampling Rate :  24.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS32000,  /**< Sampling Rate :  32.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS44100,  /**< Sampling Rate :  44.100KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS48000,  /**< Sampling Rate :  48.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS96000,  /**< Sampling Rate :  96.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS192000, /**< Sampling Rate : 192.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS384000, /**< Sampling Rate : 384.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_FS768000, /**< Sampling Rate : 768.000KHz */
	CAPI_AUDIODAC_SAMPLE_RATE_MAX       /**< Max Sampling Rate */
};

/**
 * @brief Audio DAC digital gain value
 */
enum capi_audiodac_digital_gain {
	CAPI_AUDIODAC_DIGITAL_GAIN_M64DB, /**< -64dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M63DB, /**< -63dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M62DB, /**< -62dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M61DB, /**< -61dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M60DB, /**< -60dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M59DB, /**< -59dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M58DB, /**< -58dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M57DB, /**< -57dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M56DB, /**< -56dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M55DB, /**< -55dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M54DB, /**< -54dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M53DB, /**< -53dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M52DB, /**< -52dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M51DB, /**< -51dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M50DB, /**< -50dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M49DB, /**< -49dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M48DB, /**< -48dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M47DB, /**< -47dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M46DB, /**< -46dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M45DB, /**< -45dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M44DB, /**< -44dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M43DB, /**< -43dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M42DB, /**< -42dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M41DB, /**< -41dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M40DB, /**< -40dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M39DB, /**< -39dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M38DB, /**< -38dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M37DB, /**< -37dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M36DB, /**< -36dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M35DB, /**< -35dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M34DB, /**< -34dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M33DB, /**< -33dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M32DB, /**< -32dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M31DB, /**< -31dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M30DB, /**< -30dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M29DB, /**< -29dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M28DB, /**< -28dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M27DB, /**< -27dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M26DB, /**< -26dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M25DB, /**< -25dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M24DB, /**< -24dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M23DB, /**< -23dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M22DB, /**< -22dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M21DB, /**< -21dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M20DB, /**< -20dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M19DB, /**< -19dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M18DB, /**< -18dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M17DB, /**< -17dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M16DB, /**< -16dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M15DB, /**< -15dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M14DB, /**< -14dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M13DB, /**< -13dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M12DB, /**< -12dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M11DB, /**< -11dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M10DB, /**< -10dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M9DB,  /**<  -9dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M8DB,  /**<  -8dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M7DB,  /**<  -7dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M6DB,  /**<  -6dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M5DB,  /**<  -5dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M4DB,  /**<  -4dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M3DB,  /**<  -3dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M2DB,  /**<  -2dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_M1DB,  /**<  -1dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_0DB,   /**<  -0dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_1DB,   /**<   1dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_2DB,   /**<   2dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_3DB,   /**<   3dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_4DB,   /**<   4dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_5DB,   /**<   5dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_6DB,   /**<   6dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_7DB,   /**<   7dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_8DB,   /**<   8dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_9DB,   /**<   9dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_10DB,  /**<  10dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_11DB,  /**<  11dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_12DB,  /**<  12dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_13DB,  /**<  13dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_14DB,  /**<  14dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_15DB,  /**<  15dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_16DB,  /**<  16dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_17DB,  /**<  17dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_18DB,  /**<  18dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_19DB,  /**<  19dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_20DB,  /**<  20dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_21DB,  /**<  21dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_22DB,  /**<  22dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_23DB,  /**<  23dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_24DB,  /**<  24dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_25DB,  /**<  25dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_26DB,  /**<  26dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_27DB,  /**<  27dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_28DB,  /**<  28dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_29DB,  /**<  29dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_30DB,  /**<  30dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_31DB,  /**<  31dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_32DB,  /**<  32dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_33DB,  /**<  33dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_34DB,  /**<  34dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_35DB,  /**<  35dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_36DB,  /**<  36dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_37DB,  /**<  37dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_38DB,  /**<  38dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_39DB,  /**<  39dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_40DB,  /**<  40dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_41DB,  /**<  41dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_42DB,  /**<  42dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_43DB,  /**<  43dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_44DB,  /**<  44dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_45DB,  /**<  45dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_46DB,  /**<  46dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_47DB,  /**<  47dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_48DB,  /**<  48dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_49DB,  /**<  49dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_50DB,  /**<  50dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_51DB,  /**<  51dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_52DB,  /**<  52dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_53DB,  /**<  53dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_54DB,  /**<  54dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_55DB,  /**<  55dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_56DB,  /**<  56dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_57DB,  /**<  57dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_58DB,  /**<  58dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_59DB,  /**<  59dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_60DB,  /**<  60dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_61DB,  /**<  61dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_62DB,  /**<  62dB */
	CAPI_AUDIODAC_DIGITAL_GAIN_63DB,  /**<  63dB */
};

/**
 * @brief Channel pair configuration structure
 */
struct capi_audiodac_chan_pair_cfg {
	/** Data interface master or slave */
	bool data_interface_master;
	/** Sampling rate */
	enum capi_audiodac_sample_rate sample_rate;
	/** Data word size */
	enum capi_audiodac_data_word_size data_word_size;
	/** Data interface protocol */
	enum capi_audiodac_data_interface_protocol data_interface_protocol;
	/** I2S pairing */
	enum capi_audiodac_i2s_pairing i2s_pairing;
	/** Operating power mode */
	enum capi_audiodac_op_power_mode operating_power_mode;
	/** Filtering Latency/Quality balance */
	enum capi_audiodac_filter_latency filter_latency;
};

/**
 * @brief Audio DAC configuration
 */
struct capi_audiodac_config {
	/** Base address */
	uint64_t identifier;
	/** Platform specific Audio DAC platform ops structure */
	const struct capi_audiodac_ops *ops;
	/** This is intended to store Audio DAC specific configurations */
	void *extra;
};

/**
 * @brief Audio DAC handle
 *
 * Drivers may need own handle type to handle internals
 * Driver developer shall declare this as the first field of private handle structure
 */
struct capi_audiodac_handle {
	/** Audio DAC ID */
	uint32_t audiodac_id;
	/** Audio DAC channel count */
	uint32_t chan_count;
	/** Audio DAC channel pair count */
	uint32_t chan_pair_count;
	/** Audio DAC DRC(Soft Clipping Dynamic Range Compression) support */
	bool drc_supported;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_audiodac_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Initialize handle base address
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] config Pointer to Audio DAC configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_init(struct capi_audiodac_handle **handle,
		       const struct capi_audiodac_config *config);

/**
 * @brief Deinitialize handle
 *
 * @param[in] handle Pointer to the Audio DAC handle
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_deinit(const struct capi_audiodac_handle *handle);

/**
 * @brief Configure main clock frequency
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] mclk_freq Clock freq
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_clk_config(const struct capi_audiodac_handle *handle,
			     enum capi_audiodac_mclk_freq mclk_freq);

/**
 * @brief Internal hw domain enable
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] domain Internal domain(Analog or Digital)
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_hw_domain_enable(const struct capi_audiodac_handle *handle,
				   enum capi_audiodac_hw_domain domain);

/**
 * @brief Internal hw domain disable
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] domain Internal domain(Analog or Digital)
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_hw_domain_disable(const struct capi_audiodac_handle *handle,
				    enum capi_audiodac_hw_domain domain);

/**
 * @brief Configure channel pair
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_pair_id Required channel pair
 * @param[in] chan_pair_cfg Pointer to config structure for the channel pair
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_pair_config(const struct capi_audiodac_handle *handle,
				   enum capi_audiodac_chan_pair chan_pair_id,
				   const struct capi_audiodac_chan_pair_cfg *chan_pair_cfg);

/**
 * @brief Activate channel pair data_interface
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_pair_id Required channel pair
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_pair_data_interface_activate(const struct capi_audiodac_handle *handle,
						    enum capi_audiodac_chan_pair chan_pair_id);

/**
 * @brief Deactivate channel pair data_interface
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_pair_id Required channel pair
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_pair_data_interface_deactivate(const struct capi_audiodac_handle *handle,
						      enum capi_audiodac_chan_pair chan_pair_id);

/**
 * @brief Set operational power mode
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_pair_id Required channel pair
 * @param[in] power_mode Required power mode
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_pair_set_operational_power_mode(const struct capi_audiodac_handle *handle,
						       enum capi_audiodac_chan_pair chan_pair_id,
						       enum capi_audiodac_op_power_mode power_mode);

/**
 * @brief Get operational power mode
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_pair_id Required channel pair
 * @param[out] power_mode Pointer to power mode
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_pair_get_operational_power_mode(
	const struct capi_audiodac_handle *handle, enum capi_audiodac_chan_pair chan_pair_id,
	enum capi_audiodac_op_power_mode *power_mode);

/**
 * @brief Activate channel conversion
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_conversion_activate(const struct capi_audiodac_handle *handle,
					   enum capi_audiodac_chan chan_id);

/**
 * @brief Deactivate channel conversion
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_conversion_deactivate(const struct capi_audiodac_handle *handle,
					     enum capi_audiodac_chan chan_id);

/**
 * @brief Soft mute channel
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_soft_mute(const struct capi_audiodac_handle *handle,
				 enum capi_audiodac_chan chan_id);

/**
 * @brief Soft unmute channel
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_soft_unmute(const struct capi_audiodac_handle *handle,
				   enum capi_audiodac_chan chan_id);

/**
 * @brief Activate amplifier
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_activate_amplifier(const struct capi_audiodac_handle *handle,
					  enum capi_audiodac_chan chan_id);

/**
 * @brief Deactivate amplifier
 *
 * @param[in] handle Pointer to the Audio DAC handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_audiodac_chan_deactivate_amplifier(const struct capi_audiodac_handle *handle,
					    enum capi_audiodac_chan chan_id);

/**
 * @brief Structure holding Audio DAC function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_audiodac_ops {
	/** See capi_audiodac_init() */
	int (*init)(struct capi_audiodac_handle **handle,
		    const struct capi_audiodac_config *config);
	/** See capi_audiodac_deinit() */
	int (*deinit)(const struct capi_audiodac_handle *handle);
	/** See capi_audiodac_clk_config() */
	int (*clk_config)(const struct capi_audiodac_handle *handle,
			  enum capi_audiodac_mclk_freq mclk_freq);
	/** See capi_audiodac_hw_domain_enable() */
	int (*hw_domain_enable)(const struct capi_audiodac_handle *handle,
				enum capi_audiodac_hw_domain domain);
	/** See capi_audiodac_hw_domain_disable() */
	int (*hw_domain_disable)(const struct capi_audiodac_handle *handle,
				 enum capi_audiodac_hw_domain domain);
	/** See capi_audiodac_chan_pair_config() */
	int (*chan_pair_config)(const struct capi_audiodac_handle *handle,
				enum capi_audiodac_chan_pair chan_pair_id,
				const struct capi_audiodac_chan_pair_cfg *chan_pair_cfg);
	/** See capi_audiodac_chan_pair_data_interface_activate() */
	int (*chan_pair_data_interface_activate)(const struct capi_audiodac_handle *handle,
						 enum capi_audiodac_chan_pair chan_pair_id);
	/** See capi_audiodac_chan_pair_data_interface_deactivate() */
	int (*chan_pair_data_interface_deactivate)(const struct capi_audiodac_handle *handle,
						   enum capi_audiodac_chan_pair chan_pair_id);
	/** See capi_audiodac_chan_pair_set_operational_power_mode() */
	int (*chan_pair_set_operational_power_mode)(const struct capi_audiodac_handle *handle,
						    enum capi_audiodac_chan_pair chan_pair_id,
						    enum capi_audiodac_op_power_mode power_mode);
	/** See capi_audiodac_chan_pair_get_operational_power_mode() */
	int (*chan_pair_get_operational_power_mode)(const struct capi_audiodac_handle *handle,
						    enum capi_audiodac_chan_pair chan_pair_id,
						    enum capi_audiodac_op_power_mode *power_mode);
	/** See capi_audiodac_chan_conversion_activate() */
	int (*chan_conversion_activate)(const struct capi_audiodac_handle *handle,
					enum capi_audiodac_chan chan_id);
	/** See capi_audiodac_chan_conversion_deactivate() */
	int (*chan_conversion_deactivate)(const struct capi_audiodac_handle *handle,
					  enum capi_audiodac_chan chan_id);
	/** See capi_audiodac_chan_soft_mute() */
	int (*chan_soft_mute)(const struct capi_audiodac_handle *handle,
			      enum capi_audiodac_chan chan_id);
	/** See capi_audiodac_chan_soft_unmute() */
	int (*chan_soft_unmute)(const struct capi_audiodac_handle *handle,
				enum capi_audiodac_chan chan_id);
	/** See capi_audiodac_chan_activate_amplifier() */
	int (*chan_activate_amplifier)(const struct capi_audiodac_handle *handle,
				       enum capi_audiodac_chan chan_id);
	/** See capi_audiodac_chan_deactivate_amplifier() */
	int (*chan_deactivate_amplifier)(const struct capi_audiodac_handle *handle,
					 enum capi_audiodac_chan chan_id);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_AUDIODAC_H */
