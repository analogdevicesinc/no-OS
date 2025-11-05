/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CAPI_CLK_H_
#define _CAPI_CLK_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The clock handle.
 */
struct capi_clk_handle {
	/** Set if init() has allocated the required memory. */
	bool init_allocated;
	/** Clock specific operations. */
	const struct capi_clk_ops *ops;
	/** Clock name. */
	const char *name;
	/** Hardware clock channel number. */
	int chan_num;
	/** Reference to clock private information. */
	void *priv;
};

/**
 * The clock configuration.
 */
struct capi_clk_config {
	/** Clock specific operations. */
	const struct capi_clk_ops *ops;
	/** Optional clock name. */
	const char *name;
	/** Optional hardware clock channel number. */
	int chan_num;
	/** Optional reference to clock private information. */
	void *priv;
};

/**
 * @brief Initialize the clock.
 *
 * @param [in,out] handle If NULL, the function must allocate the required
 *                        memory. Otherwise, init() will use the preallocated
 *                        structure pointed to by the handle.
 * @param [in] config The clock configuration.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_clk_init(struct capi_clk_handle **handle, const struct capi_clk_config *config);

/**
 * @brief Deinitialize the clock.
 *
 * @param [in,out] handle The clock handle. If allocated by the init()
 *                        function, deinit() must free the memory.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_clk_deinit(struct capi_clk_handle **handle);

/**
 * @brief Enable the clock.
 *
 * @param [in] handle The clock handle.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_clk_enable(struct capi_clk_handle *handle);

/**
 * @brief Disable the clock.
 *
 * @param [in] handle The clock handle.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_clk_disable(struct capi_clk_handle *handle);

/**
 * @brief Get the current clock rate (Hz).
 *
 * @param [in] handle The clock handle.
 * @param [out] rate The current clock rate (Hz).
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_clk_get_rate(struct capi_clk_handle *handle, uint64_t *rate);

/**
 * @brief Round the specified rate (Hz) to a frequency that the hardware
 *        can actually provide.
 *
 * @param [in] handle The clock handle.
 * @param [in] rate The desired clock rate (Hz).
 * @param [out] rounded_rate The rounded clock rate (Hz).
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_clk_round_rate(struct capi_clk_handle *handle, uint64_t rate, uint64_t *rounded_rate);

/**
 * @brief Set the specified clock rate (Hz).
 *
 * @param [in] handle The clock handle.
 * @param [in] rate The clock rate (Hz) to be set.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_clk_set_rate(struct capi_clk_handle *handle, uint64_t rate);

/**
 * Container for the clock specific operations.
 */
struct capi_clk_ops {
	/** Initialize the clock. */
	int (*init)(struct capi_clk_handle **handle, const struct capi_clk_config *config);
	/** Deinitialize the clock. */
	int (*deinit)(struct capi_clk_handle *handle);
	/** Enable the clock. */
	int (*enable)(struct capi_clk_handle *handle);
	/** Disable the clock. */
	int (*disable)(struct capi_clk_handle *handle);
	/** Get the current clock rate (Hz). */
	int (*get_rate)(struct capi_clk_handle *handle, uint64_t *rate);
	/** Round the specified rate (Hz) to a frequency that the hardware
	 *  can actually provide. */
	int (*round_rate)(struct capi_clk_handle *handle, uint64_t rate, uint64_t *rounded_rate);
	/** Set the specified clock rate (Hz). */
	int (*set_rate)(struct capi_clk_handle *handle, uint64_t rate);
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CAPI_CLK_H_ */
