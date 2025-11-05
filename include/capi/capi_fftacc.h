/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The FFTAcc(FFT Accelerator) API
 *
 * The FFT Accelerator is used to process audio signals and can
 * be configured with DFG (Data Flow Gasket) to implement FFT
 * operations. FFT operations include programmable overlap, IFFT
 * operations with WOLA(Weighted Overlap Add), windowing applied
 * to FFT input and IFFT output with programmable window coefficients
 */

#ifndef CAPI_FFTACC_H
#define CAPI_FFTACC_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief FFTACC priority
 */
enum capi_fftacc_prio {
	CAPI_FFTACC_PRIORITY_LOWEST,
	CAPI_FFTACC_PRIORITY_LOW,
	CAPI_FFTACC_PRIORITY_HIGH,
	CAPI_FFTACC_PRIORITY_HIGHEST,
	CAPI_FFTACC_PRIORITY_MAX
};

/**
 * @brief FFTACC FFT/IFFT size
 */
enum capi_fftacc_fft_size {
	CAPI_FFTACC_FFT_SIZE_128,
	CAPI_FFTACC_FFT_SIZE_256,
	CAPI_FFTACC_FFT_SIZE_512,
	CAPI_FFTACC_FFT_SIZE_1024,
	CAPI_FFTACC_FFT_SIZE_2048,
	CAPI_FFTACC_FFT_SIZE_MAX
};

/**
 * @brief FFTACC operation type - FFT or IFFT
 */
enum capi_fftacc_op_type {
	CAPI_FFTACC_OP_FFT,
	CAPI_FFTACC_OP_IFFT,
	CAPI_FFTACC_OP_MAX
};

/**
 * @brief FFTACC configuration
 */
struct capi_fftacc_config {
	/** Base address */
	uint64_t identifier;
	/** Platform specific FFTACC platform ops structure */
	const struct capi_fftacc_ops *ops;
	/** This is intended to store FFTACC specific configurations. */
	void *extra;
};

/**
 * @brief FFTACC handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_fftacc_handle {
	/** FFTACC ID */
	uint32_t fftacc_id;
	/** FFTACC channel count */
	uint32_t channel_count;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_fftacc_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief FFTACC channel configuration
 */
struct capi_fftacc_chan_config {
	/** FFTACC-DFG input stream id */
	uint8_t is_strm_id;
	/** FFTACC-DFG output stream id */
	uint8_t os_strm_id;
	/** FFTACC bin Scaler */
	int8_t fftacc_bin_scl;
	/** FFTACC priority level */
	enum capi_fftacc_prio fftacc_prio;
	/** FFTACC FFT size */
	enum capi_fftacc_fft_size fftacc_fft_size;
	/** FFTACC window enable */
	bool fftacc_wnd_en;
	/** FFTACC operation type - FFTACC or IFFTACC */
	enum capi_fftacc_op_type fftacc_op_type;
	/** FFTACC window coefficient location */
	uint8_t wnd_loc;
	/** FFTACC software start enable */
	bool sw_start;
	/** FFTACC WOLA enable */
	bool wola_en;
};

/**
 * @brief FFTACC WOLA (Windowed Overlap Add) configuration
 */
struct capi_fftacc_chan_wola_config {
	/** FFTACC WOLA feedback size */
	uint8_t fbk_sz;
	/** FFTACC WOLA input size */
	uint8_t input_sz;
	/** FFTACC WOLA output size */
	uint8_t output_sz;
	/** FFTACC WOLA buffer location */
	uint8_t buf_loc;
};

/**
 * @brief Initialize FFTACC with settings from the configuration structure
 *
 * @param [in] handle Pointer to FFTACC controller handle pointer
 * @param [in] config Pointer to FFTACC configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_fftacc_init(struct capi_fftacc_handle **handle, const struct capi_fftacc_config *config);

/**
 * @brief Deinit FFTACC
 *
 * @param[in] handle Pointer to FFTACC Driver handle
 *
 * @return int 0 for success or error code.
 */
int capi_fftacc_deinit(const struct capi_fftacc_handle *handle);

/**
 * @brief Get FFTACC status
 *
 * @param [in] handle FFTACC controller handle
 * @param [out] stat FFTACC status storing current active FFTACC ID and FFTACC busy status
 *
 * @return int 0 for success or error code
 */
int capi_fftacc_get_status(const struct capi_fftacc_handle *handle, uint32_t *stat);

/**
 * @brief FFTACC channel initialization
 *
 * @param [in] handle FFTACC controller handle
 * @param [in] idx FFTACC channel index
 * @param [in] config Configuration for a channel
 * @param [in] wola_config WOLA configuration for a channel
 *
 * @return int 0 for success or error code
 */
int capi_fftacc_chan_init(const struct capi_fftacc_handle *handle, uint8_t idx,
			  const struct capi_fftacc_chan_config *config,
			  const struct capi_fftacc_chan_wola_config *wola_config);

/**
 * @brief Start FFTACC by software
 *
 * @param [in] handle FFTACC controller handle
 * @param [in] idx FFTACC channel index
 *
 * @return int 0 for success or error code
 */
int capi_fftacc_sw_chan_start(const struct capi_fftacc_handle *handle, uint8_t idx);

/**
 * @brief Start FFTACC automatically as soon as data is available and FFTACC stream enabled
 *
 * @param [in] handle FFTACC controller handle
 * @param [in] idx FFTACC channel index
 *
 * @return int 0 for success or error code
 */
int capi_fftacc_chan_start(const struct capi_fftacc_handle *handle, uint8_t idx);

/**
 * @brief FFTACC operation done status
 *
 * @param [in] handle FFTACC controller handle
 * @param [in] idx FFTACC channel index
 * @param [out] op_done Boolean for FFTACC operation done
 *
 * @return int 0 for success or error code
 */
int capi_fftacc_get_chan_op_done_stat(const struct capi_fftacc_handle *handle, uint8_t idx,
				      bool *op_done);

/**
 * @brief Clear FFTACC stream channel operation done
 *
 * @param [in] handle FFTACC controller handle
 * @param [in] idx FFTACC channel index
 *
 * @return int 0 for success or error code
 */
int capi_fftacc_clear_chan_op_done_stat(struct capi_fftacc_handle *handle, uint8_t idx);

/**
 * @brief Deinit FFTACC stream channel
 *
 * @param [in] handle FFTACC controller handle
 * @param [in] idx FFTACC channel index
 *
 * @return int 0 for success or error code
 */
int capi_fftacc_chan_deinit(struct capi_fftacc_handle *handle, uint8_t idx);

/**
 * @brief Structure holding FFT Accelerator function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_fftacc_ops {
	/** See capi_fftacc_init() */
	int (*init)(struct capi_fftacc_handle **handle, const struct capi_fftacc_config *config);
	/** See capi_fftacc_deinit() */
	int (*deinit)(const struct capi_fftacc_handle *handle);
	/** See capi_fftacc_get_status() */
	int (*get_status)(const struct capi_fftacc_handle *handle, uint32_t *stat);
	/** See capi_fftacc_chan_init() */
	int (*chan_init)(const struct capi_fftacc_handle *handle, uint8_t idx,
			 const struct capi_fftacc_chan_config *config,
			 const struct capi_fftacc_chan_wola_config *wola_config);
	/** See capi_fftacc_sw_chan_start() */
	int (*sw_chan_start)(const struct capi_fftacc_handle *handle, uint8_t idx);
	/** See capi_fftacc_chan_start() */
	int (*chan_start)(const struct capi_fftacc_handle *handle, uint8_t idx);
	/** See capi_fftacc_get_chan_op_done_stat() */
	int (*get_chan_op_done_stat)(const struct capi_fftacc_handle *handle, uint8_t idx,
				     bool *op_done);
	/** See capi_fftacc_clear_chan_op_done_stat() */
	int (*clear_chan_op_done_stat)(struct capi_fftacc_handle *handle, uint8_t idx);
	/** See capi_fftacc_chan_deinit() */
	int (*chan_deinit)(struct capi_fftacc_handle *handle, uint8_t idx);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_FFTACC_H */
