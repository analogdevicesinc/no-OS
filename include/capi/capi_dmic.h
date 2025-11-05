/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The DMIC API
 *
 * @brief The Digital Microphone (DMIC) block performs the function
 * of down-sampling and filtering the DMIC data using multi-stage filter
 * blocks to meet the desired audio performance. PDM Demodulator block is
 * used as an interface to the DMIC.
 */

#ifndef CAPI_DMIC_H
#define CAPI_DMIC_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief DMIC channel
 */
enum capi_dmic_ch {
	CAPI_DMIC_CHAN_0,   /**< DMIC Channel 0 */
	CAPI_DMIC_CHAN_1,   /**< DMIC Channel 1 */
	CAPI_DMIC_CHAN_2,   /**< DMIC Channel 2 */
	CAPI_DMIC_CHAN_3,   /**< DMIC Channel 3 */
	CAPI_DMIC_CHAN_MAX, /**< Max Channel */
};

/**
 * @brief DMIC channel pair
 */
enum capi_dmic_chan_pair {
	CAPI_DMIC_CHAN_PAIR_0,   /**< DMIC Channel0 and channel1 */
	CAPI_DMIC_CHAN_PAIR_1,   /**< DMIC Channel2 and channel3 */
	CAPI_DMIC_CHAN_PAIR_MAX, /**< Max channel pair */
};

/**
 * @brief DMIC Clock Rates
 */
enum capi_dmic_clk_rate {
	CAPI_DMIC_CLK_RATE_C384K, /**<  384 kHz Clock Rate (12/24/48/96/192/384/768kHz FS only).  */
	CAPI_DMIC_CLK_RATE_C768K, /**<  768 kHz Clock Rate (12/24/48/96/192/384/768kHz FS only).  */
	CAPI_DMIC_CLK_RATE_C1536K, /**< 1536 kHz Clock Rate (12/24/48/96/192/384/768kHz FS only). */
	CAPI_DMIC_CLK_RATE_C3072K, /**< 3072 kHz Clock Rate (12/24/48/96/192/384/768kHz FS only). */
	CAPI_DMIC_CLK_RATE_C6144K, /**< 6144 kHz Clock Rate (12/24/48/96/192/384/768kHz FS only). */
	CAPI_DMIC_CLK_RATE_C256K, /**<  256 kHz Clock Rate (8/16kHz FS only).                     */
	CAPI_DMIC_CLK_RATE_C512K, /**<  512 kHz Clock Rate (8/16kHz FS only).                     */
	CAPI_DMIC_CLK_RATE_C1024K, /**< 1024 kHz Clock Rate (8/16kHz FS only). */
	CAPI_DMIC_CLK_RATE_C2048K, /**< 2048 kHz Clock Rate (8/16kHz FS only). */
	CAPI_DMIC_CLK_RATE_C4096K, /**< 4096 kHz Clock Rate (8/16kHz FS only). */
	CAPI_DMIC_CLK_RATE_MAX     /**< Max Clock Rate. */
};

#define CAPI_DMIC_CLK_DIS (false) /**< DMIC Channel pair clock disable. */
#define CAPI_DMIC_CLK_EN  (true)  /**< DMIC Channel pair clock enable. */

/**
 * @brief DMIC channel pair output sample rate
 */
enum capi_dmic_sample_rate {
	CAPI_DMIC_SAMPLE_RATE_FS8K,   /**< 8 kHz Sample Rate.  */
	CAPI_DMIC_SAMPLE_RATE_FS12K,  /**< 12 kHz Sample Rate. */
	CAPI_DMIC_SAMPLE_RATE_FS16K,  /**< 16 kHz Sample Rate. */
	CAPI_DMIC_SAMPLE_RATE_FS24K,  /**< 24 kHz Sample Rate. */
	CAPI_DMIC_SAMPLE_RATE_FS48K,  /**< 48 kHz Sample Rate. */
	CAPI_DMIC_SAMPLE_RATE_FS96K,  /**< 96 kHz Sample Rate. */
	CAPI_DMIC_SAMPLE_RATE_FS192K, /**< 192 kHz Sample Rate. */
	CAPI_DMIC_SAMPLE_RATE_FS384K, /**< 384 kHz Sample Rate. */
	CAPI_DMIC_SAMPLE_RATE_FS768K, /**< 768 kHz Sample Rate. */
	CAPI_DMIC_SAMPLE_RATE_MAX     /**< Max Sample rate. */
};

#define CAPI_DMIC_HPF_OFF (false) /**< High-Pass Filter OFF */
#define CAPI_DMIC_HPF_ON  (true)  /**< High-Pass Filter ON */

#define CAPI_DMIC_DEC_FILTER_FOURTH (false) /**< Decimation Filter - 4th order */
#define CAPI_DMIC_DEC_FILTER_FIFTH  (true)  /**< Decimation Filter - 5th order */

#define CAPI_DMIC_FCOMP_OFF (false) /**< Frequency response is not compensated. */
#define CAPI_DMIC_FCOMP_ON  (true)  /**< Frequency response is compensated. */

/**
 * @brief High-Pass Filter Cut-off Frequency
 */
enum capi_dmic_hpf_cf {
	CAPI_DMIC_HPF_CF_MIN = 4,   /**< Invalid HPF */
	CAPI_DMIC_HPF_CF_FC241 = 5, /**< 241 Hz.  */
	CAPI_DMIC_HPF_CF_FC120,     /**< 120 Hz.  */
	CAPI_DMIC_HPF_CF_FC60,      /**< 60 Hz.   */
	CAPI_DMIC_HPF_CF_FC30,      /**< 30 Hz.   */
	CAPI_DMIC_HPF_CF_FC15,      /**< 15 Hz.   */
	CAPI_DMIC_HPF_CF_FC7,       /**< 7 Hz.    */
	CAPI_DMIC_HPF_CF_FC4,       /**< 4 Hz.    */
	CAPI_DMIC_HPF_CF_FC2,       /**< 2 Hz.    */
	CAPI_DMIC_HPF_CF_FC1,       /**< 1 Hz.    */
	CAPI_DMIC_HPF_CF_FCP5,      /**< 0.5 Hz.  */
	CAPI_DMIC_HPF_CF_FCP25,     /**< 0.25 Hz. */
	CAPI_DMIC_HPF_CF_MAX        /**< Max HPF */
};

#define CAPI_DMIC_CHAN_UNMUTE (false) /**< Unmute DMIC Channel */
#define CAPI_DMIC_CHAN_MUTE   (true)  /**< Mute DMIC Channel */

#define CAPI_DMIC_CHAN_OFF (false) /**< DMIC Channel OFF. */
#define CAPI_DMIC_CHAN_ON  (true)  /**< DMIC Channel ON. */

/**
 * @brief DMIC Volume control parameters
 */
enum capi_dmic_vol_param {
	CAPI_DMIC_VOL_PARAM_HARD, /**< Hard volume feature */
	CAPI_DMIC_VOL_PARAM_LINK, /**< Volume link feature */
	CAPI_DMIC_VOL_PARAM_ZC,   /**< Zero cross feature */
	CAPI_DMIC_VOL_PARAM_MAX   /**< Max Parameter */
};

/**
 * @brief DMIC configuration
 */
struct capi_dmic_config {
	/**< Base address */
	uint64_t identifier;
	/** Platform specific I2S platform ops structure */
	const struct capi_dmic_ops *ops;
	/** This is intended to store I2S specific configurations. */
	void *extra;
};

/**
 * @brief DMIC handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_dmic_handle {
	/** DMIC ID */
	uint32_t dmic_id;
	/** Channel pair count */
	uint32_t channel_pair_count;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_dmic_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief DMIC Channel pair configuration
 */
struct capi_dmic_chan_pair_config {
	bool ch_map;                            /**< Clock Selection */
	bool clk_edge;                          /**< Clock Edge Selection */
	bool en_fcomp;                          /**< Enable or Disable Frequency compensation */
	bool dec_order;                         /**< Decimation filter order */
	bool en_hpf;                            /**< Enable or Disable High-Pass Filter */
	enum capi_dmic_sample_rate sample_rate; /**< Sample Rate */
	enum capi_dmic_hpf_cf hpf_cut_off;      /**< High-Pass Cut off Frequency */
};

/**
 * @brief Initialize DMIC base address.
 *
 * @param[in] handle Pointer to DMIC handle pointer.
 * @param[in] config Pointer to DMIC configuration structure.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_init(struct capi_dmic_handle **handle, const struct capi_dmic_config *config);

/**
 * @brief Deinitialize DMIC.
 *
 * @param[in] handle Pointer to the DMIC handle.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_deinit(const struct capi_dmic_handle *handle);

/**
 * @brief Configure one pair of DMIC channel pair.
 *
 * @param[in] handle Pointer to the DMIC handle.
 * @param[in] ch_pair Required channel pair to be configured.
 * @param[in] ch_pair_config Pointer to config structure for channel pair.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_configure_chan_pair(const struct capi_dmic_handle *handle,
				  enum capi_dmic_chan_pair ch_pair,
				  const struct capi_dmic_chan_pair_config *ch_pair_config);

/**
 * @brief Configure DMIC channel pairs.
 *
 * @param[in] handle Pointer to the DMIC handle.
 * @param[in] ch_pair_mask Required channel pairs; LSB for Channel 0 pair.
 * @param[in] ch_pair_config Pointer to array of config structure for channel pair.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_configure_chan_pairs(const struct capi_dmic_handle *handle, uint8_t ch_pair_mask,
				   const struct capi_dmic_chan_pair_config *ch_pair_config);

/**
 * @brief Enable and configure the high-pass filter (HPF) of DMIC channel pair.
 *
 * @param[in] handle Pointer to the DMIC handle.
 * @param[in] ch_pair DMIC Channel pair to be configured.
 * @param[in] en Enable or disable HPF.
 * @param[in] cut_off_freq HPF cut-off frequency.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_enable_hpf(const struct capi_dmic_handle *handle, enum capi_dmic_chan_pair ch_pair,
			 bool en, enum capi_dmic_hpf_cf cut_off_freq);

/**
 * @brief Enable or Disable Volume control parameter - Link, zero-cross, Hard volume.
 *
 * @param[in] handle Pointer to the DMIC handle.
 * @param[in] vol_param Volume parameter control to be configured.
 * @param[in] en true: To enable the feature.
 *              false: To disable the feature.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_configure_vol_param(const struct capi_dmic_handle *handle,
				  enum capi_dmic_vol_param vol_param, bool en);

/**
 * @brief Configure DMIC channel volume.
 *
 * @param[in] handle Pointer to the DMIC handle.
 * @param[in] ch DMIC Channel for which volume to be set.
 * @param[in] vol Volume level value in dB and is equal to 24 - (vol * 0.375).
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_set_chan_volume(const struct capi_dmic_handle *handle, enum capi_dmic_ch ch,
			      uint8_t vol);

/**
 * @brief Enable DMIC channels.
 *
 * @param[in] handle Pointer to the DMIC handle.
 * @param[in] ch_mask Required channels to be enabled.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_enable_channels(const struct capi_dmic_handle *handle, uint8_t ch_mask);

/**
 * @brief Disable DMIC channels.
 *
 * @param[in] handle Pointer to the DMIC handle.
 * @param[in] ch_mask Required channels to be disabled.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_disable_channels(const struct capi_dmic_handle *handle, uint8_t ch_mask);

/**
 * @brief Enable and configure DMIC clock.
 *
 * @param[in] handle Pointer to the DMIC handle.
 * @param[in] clk_map Clock to enable and configure.
 * @param[in] en true: Enable the clock.
 *              false: Disable the clock.
 * @param[in] clk_rate Clock frequency.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_configure_clk(const struct capi_dmic_handle *handle, bool clk_map, bool en,
			    enum capi_dmic_clk_rate clk_rate);

/**
 * @brief Mute DMIC channels.
 *
 * @param[in] handle Pointer to DMIC driver handle.
 * @param[in] ch_mask Requried channels to be muted.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_mute_channels(const struct capi_dmic_handle *handle, uint8_t ch_mask);

/**
 * @brief Unmute DMIC channels.
 *
 * @param[in] handle Pointer to DMIC driver handle.
 * @param[in] ch_mask Requried channels to be unmuted.
 *
 * @return int 0 for success or error code.
 */
int capi_dmic_unmute_channels(const struct capi_dmic_handle *handle, uint8_t ch_mask);

/**
 * @brief Structure holding DMIC function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_dmic_ops {
	/** See capi_dmic_init() */
	int32_t (*init)(struct capi_dmic_handle **handle, const struct capi_dmic_config *config);
	/** See capi_dmic_deinit() */
	int32_t (*deinit)(const struct capi_dmic_handle *handle);
	/** See capi_dmic_configure_chan_pair() */
	int32_t (*configure_chan_pair)(const struct capi_dmic_handle *handle,
				       enum capi_dmic_chan_pair ch_pair,
				       const struct capi_dmic_chan_pair_config *ch_pair_config);
	/** See capi_dmic_configure_chan_pairs() */
	int32_t (*configure_chan_pairs)(const struct capi_dmic_handle *handle, uint8_t ch_pair_mask,
					const struct capi_dmic_chan_pair_config *ch_pair_config);
	/** See capi_dmic_enable_hpf() */
	int32_t (*enable_hpf)(const struct capi_dmic_handle *handle,
			      enum capi_dmic_chan_pair ch_pair, bool en,
			      enum capi_dmic_hpf_cf cut_off_freq);
	/** See capi_dmic_configure_vol_param() */
	int32_t (*configure_vol_param)(const struct capi_dmic_handle *handle,
				       enum capi_dmic_vol_param vol_param, bool en);
	/** See capi_dmic_set_chan_volume() */
	int32_t (*set_chan_volume)(const struct capi_dmic_handle *handle, enum capi_dmic_ch ch,
				   uint8_t vol);
	/** See capi_dmic_enable_channels() */
	int32_t (*enable_channels)(const struct capi_dmic_handle *handle, uint8_t ch_mask);
	/** See capi_dmic_disable_channels() */
	int32_t (*disable_channels)(const struct capi_dmic_handle *handle, uint8_t ch_mask);
	/** See capi_dmic_configure_clk() */
	int32_t (*configure_clk)(const struct capi_dmic_handle *handle, bool clk_map, bool en,
				 enum capi_dmic_clk_rate clk_rate);
	/** See capi_dmic_mute_channels() */
	int32_t (*mute_channels)(const struct capi_dmic_handle *handle, uint8_t ch_mask);
	/** See capi_dmic_unmute_channels() */
	int32_t (*unmute_channels)(const struct capi_dmic_handle *handle, uint8_t ch_mask);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_DMIC_H */
