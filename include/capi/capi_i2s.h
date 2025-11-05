/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The I2S API
 *
 * The Inter-IC Sound (I2S) serial audio ports are I/O ports that allow
 * for the transmit/receive of multiple channels of digital PCM audio data over
 * a 4-wire serial interface.
 *
 */

#ifndef CAPI_I2S_H
#define CAPI_I2S_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief I2S Clock Source
 */
enum capi_i2s_clk_src {
	CAPI_I2S_CLK_SRC_MASTER, /**< Internally generated master clock */
	CAPI_I2S_CLK_SRC_SLAVE,  /**< Externallly generated master clock */
	CAPI_I2S_CLK_SRC_MAX     /**< Invalid master clock source  */
};

/**
 * @brief I2S BCLK(Bit Clock) Source
 */
enum capi_i2s_bclk {
	CAPI_I2S_BCLK_3072K,
	CAPI_I2S_BCLK_6144K,
	CAPI_I2S_BCLK_12288K,
	CAPI_I2S_BCLK_24576K,
	CAPI_I2S_BCLK_MAX
};

/**
 * @brief I2S BCLK(Bit Clock) Polarity
 */
enum capi_i2s_bclk_pol {
	CAPI_I2S_BCLK_POL_NORMAL,
	CAPI_I2S_BCLK_POL_INVERTED,
	CAPI_I2S_BCLK_POL_MAX
};

/**
 * @brief  I2S LRCLK(Left-Right Clock)
 */
enum capi_i2s_lrclk {
	CAPI_I2S_LRCLK_8K,
	CAPI_I2S_LRCLK_12K,
	CAPI_I2S_LRCLK_16K,
	CAPI_I2S_LRCLK_24K,
	CAPI_I2S_LRCLK_48K,
	CAPI_I2S_LRCLK_96K,
	CAPI_I2S_LRCLK_192K,
	CAPI_I2S_LRCLK_384K,
	CAPI_I2S_LRCLK_768K,
	CAPI_I2S_LRCLK_MAX
};

/**
 * @brief I2S LRCLK Polarity
 */
enum capi_i2s_lrclk_pol {
	CAPI_I2S_LRCLK_POL_NORMAL,
	CAPI_I2S_LRCLK_POL_INVERTED,
	CAPI_I2S_LRCLK_POL_MAX
};

/**
 * @brief I2S Direction
 */
enum capi_i2s_direction {
	CAPI_I2S_DIRECTION_RX_ONLY,
	CAPI_I2S_DIRECTION_TX_ONLY,
	CAPI_I2S_DIRECTION_BOTH,
	CAPI_I2S_DIRECTION_MAX
};

/**
 * @brief I2S Slot Width
 */
enum capi_i2s_slot_width {
	CAPI_I2S_SLOT_WIDTH_8,
	CAPI_I2S_SLOT_WIDTH_16,
	CAPI_I2S_SLOT_WIDTH_24,
	CAPI_I2S_SLOT_WIDTH_32,
	CAPI_I2S_SLOT_WIDTH_MAX
};

/**
 * @brief I2S Data Format
 */
enum capi_i2s_data_format {
	CAPI_I2S_DATA_FORMAT_I2S,
	CAPI_I2S_DATA_FORMAT_LEFT_JUSTIFIED,
	CAPI_I2S_DATA_FORMAT_RIGHT_JUSTIFIED,
	CAPI_I2S_DATA_FORMAT_MAX
};

/**
 * @brief I2S Data Order
 */
enum capi_i2s_data_order {
	CAPI_I2S_DATA_ORDER_MSB_FIRST,
	CAPI_I2S_DATA_ORDER_LSB_FIRST,
	CAPI_I2S_DATA_ORDER_MAX
};

/**
 * @brief I2S configuration
 *
 */
struct capi_i2s_config {
	/**< Base address */
	uint64_t identifier;
	/** Platform specific I2S platform ops structure */
	const struct capi_i2s_ops *ops;
	/** This is intended to store I2S specific configurations. */
	void *extra;
};

/**
 * @brief I2S port configuration
 *
 */
struct capi_i2s_port_config {
	/**< I2S port */
	uint32_t port_no;
	/** I2S clocks(BCLK/LRCLK) internal or external */
	enum capi_i2s_clk_src clk_src;
	/** I2S BCLK setting */
	enum capi_i2s_bclk bclk;
	/** I2S LRCLK setting */
	enum capi_i2s_lrclk lrclk;
	/** I2S BCLK polarity */
	enum capi_i2s_bclk_pol bclk_pol;
	/** I2S LRCLK polarity */
	enum capi_i2s_lrclk_pol lrclk_pol;
	/** I2S direction */
	enum capi_i2s_direction direction;
	/** I2S slot width */
	enum capi_i2s_slot_width slot_width;
	/** I2S data format */
	enum capi_i2s_data_format format;
	/** I2S data order */
	enum capi_i2s_data_order order;
	/** Optional platform specific extra configuration */
	void *extra;
};

/**
 * @brief I2S handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_i2s_handle {
	/** I2S ID */
	uint32_t i2s_id;
	/** I2S port count */
	uint32_t port_count;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;

	/** set and used by capi thin layer */
	const struct capi_i2s_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Initialize I2S Instance.
 *
 * @param[in] handle Pointer to I2S controller handler pointer.
 * @param[in] config Pointer to configuration register.
 *
 * @return int 0 for success or error code.
 */
int capi_i2s_init(struct capi_i2s_handle **handle, const struct capi_i2s_config *config);

/**
 * @brief Deinit I2S
 *
 * @param[in] handle Pointer to I2S controller handler
 *
 * @retval 0 Deinit successful
 * @retval EINVAL when the handler is NULL
 */
int capi_i2s_deinit(const struct capi_i2s_handle *handle);

/**
 * @brief Get the configuration for an I2S instance
 *
 * @param[in] handle Pointer to I2S controller handler
 * @param[out] config Pointer to configuration register
 *
 * @return int 0 for success or error code.
 */
int capi_i2s_get_config(const struct capi_i2s_handle *handle, struct capi_i2s_config *config);

/**
 * @brief Set the configuration for an I2S port
 *
 * @param[in] handle Pointer to the I2S handle.
 * @param[in] port_config Pointer to port configuration struct
 *
 * @return int 0 for success or error code.
 */
int capi_i2s_set_port_config(const struct capi_i2s_handle *handle,
			     const struct capi_i2s_port_config *port_config);

/**
 * @brief Get the configuration for an I2S port
 *
 * @param[in] handle Pointer to the I2S handle.
 * @param[in] port_config Pointer to port configuration struct
 *
 * @return int 0 for success or error code.
 */
int capi_i2s_get_port_config(const struct capi_i2s_handle *handle,
			     struct capi_i2s_port_config *port_config);

/**
 * @brief Enable I2S port
 *
 * @param[in] handle Pointer to the I2S handle
 * @param[in] port_no I2S port no
 * @param[in] direction Rx, Tx or Both
 *
 * @return int 0 for success or error code
 */
int capi_i2s_port_enable(const struct capi_i2s_handle *handle, uint32_t port_no,
			 enum capi_i2s_direction direction);

/**
 * @brief Disable I2S port
 *
 * @param[in] handle Pointer to the I2S handle
 * @param[in] port_no I2S port no
 * @param[in] direction Rx, Tx or Both
 *
 * @return int 0 for success or error code.
 */
int capi_i2s_port_disable(const struct capi_i2s_handle *handle, uint32_t port_no,
			  enum capi_i2s_direction direction);

/**
 * @brief Structure holding I2S function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_i2s_ops {
	/** See capi_i2s_init() */
	int (*init)(struct capi_i2s_handle **handle, const struct capi_i2s_config *config);
	/** See capi_i2s_deinit() */
	int (*deinit)(const struct capi_i2s_handle *handle);
	/** See capi_i2s_get_config() */
	int (*get_config)(const struct capi_i2s_handle *handle, struct capi_i2s_config *config);
	/** See capi_i2s_set_port_config() */
	int (*set_port_config)(const struct capi_i2s_handle *handle,
			       const struct capi_i2s_port_config *port_config);
	/** See capi_i2s_get_port_config() */
	int (*get_port_config)(const struct capi_i2s_handle *handle,
			       struct capi_i2s_port_config *port_config);
	/** See capi_i2s_port_enable() */
	int (*port_enable)(const struct capi_i2s_handle *handle, uint32_t port_no,
			   enum capi_i2s_direction direction);
	/** See capi_i2s_port_disable() */
	int (*port_disable)(const struct capi_i2s_handle *handle, uint32_t port_no,
			    enum capi_i2s_direction direction);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_I2S_H */
