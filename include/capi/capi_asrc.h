/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The ASRC(Asynchronous Sample Rate Converter) API
 *
 * The ASRC driver configures and controls the ASRC0 and ASRC1 interfaces, which are
 * independent, full duplex, real-time conversion systems. The driver enables management
 * of input & output channels, allowing connection to serial audio data ports for audio data
 * sampling & conversion. This driver supports configuration of sampling rates, input/output
 * channel activation and power management while also enabling source selection and lock status
 * checking. The driver includes low-pass filter options for control and safety mechanisms to
 * silence outputs if channel locks are lost
 */

#ifndef CAPI_ASRC_H
#define CAPI_ASRC_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief Input ASRC
 */
enum capi_asrc_input {
	CAPI_ASRCI_0,  /**< Input ASRC 0 */
	CAPI_ASRCI_1,  /**< Input ASRC 1 */
	CAPI_ASRCI_MAX /**< Max Input ASRC */
};

/**
 * @brief Output ASRC
 */
enum capi_asrc_output {
	CAPI_ASRCO_0,  /**< Output ASRC 0 */
	CAPI_ASRCO_1,  /**< Output ASRC 1 */
	CAPI_ASRCO_MAX /**< Max Output ASRC */
};

/**
 * @brief Input ASRC Channels
 */
enum capi_asrc_input_channel {
	CAPI_ASRCI_CHAN_0,  /**< Input ASRC Channel 0 */
	CAPI_ASRCI_CHAN_1,  /**< Input ASRC Channel 1 */
	CAPI_ASRCI_CHAN_2,  /**< Input ASRC Channel 2 */
	CAPI_ASRCI_CHAN_3,  /**< Input ASRC Channel 3 */
	CAPI_ASRCI_CHAN_MAX /**< Max Input ASRC Channel */
};

/**
 * @brief Output ASRC Channels
 */
enum capi_asrc_output_channel {
	CAPI_ASRCO_CHAN_0,  /**< Output ASRC Channel 0 */
	CAPI_ASRCO_CHAN_1,  /**< Output ASRC Channel 1 */
	CAPI_ASRCO_CHAN_2,  /**< Output ASRC Channel 2 */
	CAPI_ASRCO_CHAN_3,  /**< Output ASRC Channel 3 */
	CAPI_ASRCO_CHAN_MAX /**< Max Output ASRC Channel */
};

/**
 * @brief Input ASRC Output Sample Rate Selection
 *
 * The sample rate provided (ASRCIx output rate) by
 * the four input channels is linked and is set with
 * the ASRCIx_OUT_FS bits
 */
enum capi_asrc_input_out_fs {
	CAPI_ASRCI_OUT_FS_FS8K,
	CAPI_ASRCI_OUT_FS_FS12K,
	CAPI_ASRCI_OUT_FS_FS16K,
	CAPI_ASRCI_OUT_FS_FS24K,
	CAPI_ASRCI_OUT_FS_FS48K,
	CAPI_ASRCI_OUT_FS_FS96K,
	CAPI_ASRCI_OUT_FS_FS192K,
	CAPI_ASRCI_OUT_FS_MAX
};

/**
 * @brief Output ASRC Input Sample Rate Selection
 *
 * The internal sample rate accepted (ASRCOx input rate)
 * by the four output channels is linked and is set for
 * the four output channels with the ASRCOx_IN_FS bits
 */
enum capi_asrc_output_in_fs {
	CAPI_ASRCO_IN_FS_FS8K,
	CAPI_ASRCO_IN_FS_FS12K,
	CAPI_ASRCO_IN_FS_FS16K,
	CAPI_ASRCO_IN_FS_FS24K,
	CAPI_ASRCO_IN_FS_FS48K,
	CAPI_ASRCO_IN_FS_FS96K,
	CAPI_ASRCO_IN_FS_FS192K,
	CAPI_ASRCO_IN_FS_MAX
};

/**
 * @brief ASRC power modes available
 */
enum capi_asrc_pwr_mode {
	CAPI_ASRC_POWER_MODE_LOW,
	CAPI_ASRC_POWER_MODE_LOWEST,
	CAPI_ASRC_POWER_MODE_HIGH,
	CAPI_ASRC_POWER_MODE_HIGEST,
	CAPI_ASRC_POWER_MODE_MAX
};

/**
 * @brief Serial Port as Rate Source
 */
enum capi_asrc_output_sai_sel {
	CAPI_ASRCO_SAI_SEL_SAI0,
	CAPI_ASRCO_SAI_SEL_SAI1,
	CAPI_ASRCO_SAI_SEL_MAX
};

/**
 * @brief Input ASRC control configuration
 */
struct capi_asrc_input_ctrl_config {
	/** Boolean for voice-filter mode */
	bool enable_vfilt;
	/** Power modes available */
	enum capi_asrc_pwr_mode pwr_mode;
	/** Sample Rate in KHz */
	enum capi_asrc_input_out_fs out_fs;
};

/**
 * @brief Output ASRC control configuration
 */
struct capi_asrc_output_ctrl_config {
	/** Boolean for voice-filter mode*/
	bool enable_vfilt;
	/** Power modes available */
	enum capi_asrc_pwr_mode pwr_mode;
	/** Serial Port as Rate Source */
	enum capi_asrc_output_sai_sel sai_sel;
	/** Sample Rate in KHz */
	enum capi_asrc_output_in_fs in_fs;
};

/**
 * @brief ASRC configuration
 */
struct capi_asrc_config {
	/** Base address */
	uint64_t identifier;
	/** Platform specific ASRC platform ops structure */
	const struct capi_asrc_ops *ops;
	/** This is intended to store ASRC specific configurations */
	void *extra;
};

/**
 * @brief ASRC handle
 *
 * Drivers may need own handle type to handle internals
 * Driver developer shall declare this as the first field of private handle structure
 */
struct capi_asrc_handle {
	/** ASRC ID */
	uint32_t asrc_id;
	/** ASRC port count */
	uint32_t port_count;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_asrc_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Initialize ASRC peripheral with settings from configuration structure
 *
 * @param[in] handle Pointer to ASRC Driver handle pointer
 * @param[in] config Pointer to ASRC configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_asrc_init(struct capi_asrc_handle **handle, const struct capi_asrc_config *config);

/**
 * @brief Deinit ASRC peripheral
 *
 * @param[in] handle Pointer to ASRC Driver handle pointer
 *
 * @return int 0 for success or error code
 */
int capi_asrc_deinit(const struct capi_asrc_handle *handle);

/**
 * @brief Initialize input ASRC with settings from configuration structure
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Input ASRC index (CAPI_ASRCI_0 or CAPI_ASRCI_1)
 * @param[in] config Pointer to input ASRC configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_asrc_set_input_config(const struct capi_asrc_handle *handle, enum capi_asrc_input index,
			       const struct capi_asrc_input_ctrl_config *config);

/**
 * @brief Initialize output ASRC with settings from configuration structure
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Output ASRC index (CAPI_ASRCO_0 or CAPI_ASRCO_1)
 * @param[in] config Pointer to output ASRC configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_asrc_set_output_config(const struct capi_asrc_handle *handle, enum capi_asrc_output index,
				const struct capi_asrc_output_ctrl_config *config);

/**
 * @brief Initialize input ASRC power setting
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Input ASRC index (CAPI_ASRCI_0 or CAPI_ASRCI_1)
 * @param[in] mode Power mode
 *
 * @return int 0 for success or error code
 */
int capi_asrc_set_input_power_mode(const struct capi_asrc_handle *handle,
				   enum capi_asrc_input index, enum capi_asrc_pwr_mode mode);

/**
 * @brief Initialize output ASRC power setting
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Output ASRC index (CAPI_ASRCO_0 or CAPI_ASRCO_1)
 * @param[in] mode Power mode
 *
 * @return int 0 for success or error code
 */
int capi_asrc_set_output_power_mode(const struct capi_asrc_handle *handle,
				    enum capi_asrc_output index, enum capi_asrc_pwr_mode mode);

/**
 * @brief Retrieves the lock status of the input channels
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Input ASRC index (CAPI_ASRCI_0 or CAPI_ASRCI_1)
 * @param[out] locked boolean pointer to retrieve the lock status
 *
 * @return int 0 for success or error code
 */
int capi_asrc_get_input_lock_status(const struct capi_asrc_handle *handle,
				    enum capi_asrc_input index, bool *locked);

/**
 * @brief Retrieves the lock status of the output channels
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Output ASRC index (CAPI_ASRCO_0 or CAPI_ASRCO_1)
 * @param[out] locked boolean pointer to retrieve the lock status
 *
 * @return int 0 for success or error code
 */
int capi_asrc_get_output_lock_status(const struct capi_asrc_handle *handle,
				     enum capi_asrc_output index, bool *locked);

/**
 * @brief Enable input channel
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Input ASRC index (CAPI_ASRCI_0 or CAPI_ASRCI_1)
 * @param[in] chan Input channel
 *
 * @return int 0 for success or error code
 */
int capi_asrc_enable_input_channel(const struct capi_asrc_handle *handle,
				   enum capi_asrc_input index, enum capi_asrc_input_channel chan);

/**
 * @brief Enable output channel
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Output ASRC index (CAPI_ASRCO_0 or CAPI_ASRCO_1)
 * @param[in] chan Output channel
 *
 * @return int 0 for success or error code
 */
int capi_asrc_enable_output_channel(const struct capi_asrc_handle *handle,
				    enum capi_asrc_output index,
				    enum capi_asrc_output_channel chan);

/**
 * @brief Disable input channel
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Input ASRC index (CAPI_ASRCI_0 or CAPI_ASRCI_1)
 * @param[in] chan Input channel
 *
 * @return int 0 for success or error code
 */
int capi_asrc_disable_input_channel(const struct capi_asrc_handle *handle,
				    enum capi_asrc_input index, enum capi_asrc_input_channel chan);

/**
 * @brief Disable output channel
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Output ASRC index (CAPI_ASRCO_0 or CAPI_ASRCO_1)
 * @param[in] chan Output channel
 *
 * @return int 0 for success or error code
 */
int capi_asrc_disable_output_channel(const struct capi_asrc_handle *handle,
				     enum capi_asrc_output index,
				     enum capi_asrc_output_channel chan);

/**
 * @brief Get input channel status
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Input ASRC index (CAPI_ASRCI_0 or CAPI_ASRCI_1)
 * @param[in] chan Input channel
 * @param[out] status Input channel enable/disable (true/false) status
 *
 * @return int 0 for success or error code
 */
int capi_asrc_get_input_channel_status(const struct capi_asrc_handle *handle,
				       enum capi_asrc_input index,
				       enum capi_asrc_input_channel chan, bool *status);

/**
 * @brief Get output channel status
 *
 * @param[in] handle Pointer to ASRC Driver handle
 * @param[in] index Output ASRC index (CAPI_ASRCO_0 or CAPI_ASRCO_1)
 * @param[in] chan Output channel
 * @param[out] status Output channel enable/disable (true/false) status
 *
 * @return int 0 for success or error code
 */
int capi_asrc_get_output_channel_status(const struct capi_asrc_handle *handle,
					enum capi_asrc_output index,
					enum capi_asrc_output_channel chan, bool *status);

/**
 * @brief Structure holding ASRC function pointers that point to the platform
 * specific function. See API functions for relevant descriptions
 */
struct capi_asrc_ops {
	/** See capi_asrc_init() */
	int (*init)(struct capi_asrc_handle **handle, const struct capi_asrc_config *config);
	/** See capi_asrc_deinit() */
	int (*deinit)(struct capi_asrc_handle *handle);
	/** See capi_asrc_set_input_config() */
	int (*set_input_config)(const struct capi_asrc_handle *handle, enum capi_asrc_input index,
				const struct capi_asrc_input_ctrl_config *config);
	/** See capi_asrc_set_output_config() */
	int (*set_output_config)(const struct capi_asrc_handle *handle, enum capi_asrc_output index,
				 const struct capi_asrc_output_ctrl_config *config);
	/** See capi_asrc_set_input_power_mode() */
	int (*set_input_power_mode)(const struct capi_asrc_handle *handle,
				    enum capi_asrc_input index, enum capi_asrc_pwr_mode mode);
	/** See capi_asrc_set_output_power_mode() */
	int (*set_output_power_mode)(const struct capi_asrc_handle *handle,
				     enum capi_asrc_output index, enum capi_asrc_pwr_mode mode);
	/** See capi_asrc_get_input_lock_status() */
	int (*get_input_lock_status)(const struct capi_asrc_handle *handle,
				     enum capi_asrc_input index, bool *locked);
	/** See capi_asrc_get_output_lock_status() */
	int (*get_output_lock_status)(const struct capi_asrc_handle *handle,
				      enum capi_asrc_output index, bool *locked);
	/** See capi_asrc_enable_input_channel() */
	int (*enable_input_channel)(const struct capi_asrc_handle *handle,
				    enum capi_asrc_input index, enum capi_asrc_input_channel chan);
	/** See capi_asrc_enable_output_channel() */
	int (*enable_output_channel)(const struct capi_asrc_handle *handle,
				     enum capi_asrc_output index,
				     enum capi_asrc_output_channel chan);
	/** See capi_asrc_disable_input_channel() */
	int (*disable_input_channel)(const struct capi_asrc_handle *handle,
				     enum capi_asrc_input index, enum capi_asrc_input_channel chan);
	/** See capi_asrc_disable_output_channel() */
	int (*disable_output_channel)(const struct capi_asrc_handle *handle,
				      enum capi_asrc_output index,
				      enum capi_asrc_output_channel chan);
	/** See capi_asrc_get_input_channel_status() */
	int (*get_input_channel_status)(const struct capi_asrc_handle *handle,
					enum capi_asrc_input index,
					enum capi_asrc_input_channel chan, bool *status);
	/** See capi_asrc_get_output_channel_status() */
	int (*get_output_channel_status)(const struct capi_asrc_handle *handle,
					 enum capi_asrc_output index,
					 enum capi_asrc_output_channel chan, bool *status);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_ASRC_H */
