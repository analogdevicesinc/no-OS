/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL I2C
 */

#ifndef _CAPI_I2C_H_
#define _CAPI_I2C_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/** Forward Declarations */

/**
 * @brief forward declaration of common hal DMA controller handle
 */
struct capi_dma_handle;

/**
 * @brief I2C bus speed mode.
 */
enum capi_i2c_speed {
	CAPI_I2C_SPEED_STANDARD,  /**< I2C Standard Speed: 100 kbps */
	CAPI_I2C_SPEED_FAST,      /**< I2C Fast Speed: 400 kbps */
	CAPI_I2C_SPEED_FAST_PLUS, /**< I2C Fast Plus Speed: 1 Mbps */
	CAPI_I2C_SPEED_HIGH,      /**< I2C High Speed: 3.4 Mbps */
	CAPI_I2C_SPEED_ULTRA      /**< I2C Fast Plus Speed: 5 Mbps */
};

/**
 * @brief async event types
 */
enum capi_i2c_async_event {
	CAPI_I2C_XFR_DONE, /**< I2C transfer has completed */
	CAPI_I2C_TXUNDER,  /**< There has been a HW Error: TX Underflow */
	CAPI_I2C_RXOVER,   /**< There has been a HW Error:  RX Overflow */
	CAPI_I2C_ALOSS,    /**< There has been an Arbitration Loss */
	CAPI_I2C_NAKD,     /**< There has been a NAK detected */
	CAPI_I2C_REPEAT,   /**< There has been a Repeated Start detected. */
	CAPI_I2C_STXREQ,   /**< There has been a Target Tx Request. */
	CAPI_I2C_SRXREQ    /**< There has been a Target Rx Request. */
};

/**
 * @brief I2C controller configuration
 */
struct capi_i2c_config {
	/** I2C controller identifier:
	 * - Base address for internal controllers.
	 * - Device address on I2C bus for external controllers
	 *   (e.g., I2C Multiplexers, I2C Repeaters).
	 */
	uint64_t identifier;
	/** Frequency of peripheral clock in Hz, if zero driver sets to default of platform */
	uint32_t clk_freq_hz;
	/** Specifies whether controller acts as I2C Initiator or I2C Target. */
	bool initiator;
	/** Device address when it acts as a target */
	uint8_t address;
	/** Device descriptor (e.g., capi_i2c_device, capi_spi_device).
	 * Populated only when this controller acts as a device for another
	 * controller (e.g., I2C Multiplexers, I2C Repeaters).
	 */
	void *device;
	/** optional - user shall pass common hal dma handle to enable dma for async transfers */
	struct capi_dma_handle *dma_handle;
	/** Optional platform specific extra configuration. */
	void *extra;
	/** Platform specific implementation of the API. (Mandatory field, but can be NULL and we'll
	 * make sure it gets auto populated when using our build system.) */
	const struct capi_i2c_ops *ops;
};

/**
 * @brief I2C controller handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_i2c_controller_handle {
	const struct capi_i2c_ops *ops; /**< set and used by capi thin layer */
	bool init_allocated;            /**< If true, the driver is owner of handle memory. */
	void *lock;                     /**< set and used by capi thin layer if mux is available */
	void *priv;                     /**< set and used by user optionally */
};

/**
 * @brief I2C device descriptor
 */
struct capi_i2c_device {
	/** I2C controller this device is connected to. */
	struct capi_i2c_controller_handle *controller;
	/** When device acts as target, I2C address of the device on the bus (as specified, not
	 * shifted). */
	uint16_t address;
	/** Specifies whether device works with 10-bit or 7-bit addressing. */
	bool b10addr;
	/** I2C speed mode supported by the device. */
	enum capi_i2c_speed speed;
	/** I2C SCLK duty cycle required by device, expressed as the numerator in the fraction
	 * duty_cycle / 256. */
	uint8_t duty_cycle;
	/* TODO: discuss clock stretching, per device? */
	/** clock stretching, 0:none, -1:infinite, [1,n]:bit times where n depends to platform
	 * support */
	int clk_stretch;
	/** Optional platform specific extra configuration. */
	void *extra;
};

/**
 * @brief  I2C transfer
 */
struct capi_i2c_transfer {
	/** Issue a repeated start condition prior to sending the sub-address. */
	bool repeated_start;
	/** Sub-address (register) buffer, input, ignored when the other device is an initiator. */
	uint8_t *sub_address;
	/** Length of sub-address buffer in bytes, ignored when the other device is an initiator. */
	uint8_t sub_address_len;
	/** Data buffer, in/out direction depending on write/read. */
	uint8_t *buf;
	/** Length of data buffer in bytes. */
	uint32_t len;
	/** Don't issue a stop condition after the data transfer. */
	bool no_stop;
};

/**
 * @brief  I2C Driver Call Back type
 * @param[in] event I2C event type
 * @param[in] arg Pointer to user specific data.
 * @param[in] event_extra optional, platform/driver specific extra information for event
 */
typedef void (*capi_i2c_callback)(enum capi_i2c_async_event event, void *arg, int event_extra);

/**
 * @brief Initialize an instance of the I2C controller.
 *
 * @param [in out] handle Points to a pointer of the context structure. If this
 *                 pointer is set to NULL, then the controller will allocate
 *	               the context structure and be the owner. If the pointer
 *	               is not NULL, the application has allocated the structure and is
 *	               the owner.
 * @param [in] config Points to the configuration for the I2C controller.
 *
 * @return int 0 for success or error code.
 */
int capi_i2c_init(struct capi_i2c_controller_handle **handle, const struct capi_i2c_config *config);

/**
 * @brief Deinitialize the I2C controller, disable, and bring to default settings.
 *
 * @param [in] handle Points to the I2C controller context.
 *
 * @return int 0 for success or error code.
 */
int capi_i2c_deinit(struct capi_i2c_controller_handle *handle);

/**
 * @brief Sync/Blocking transmit function.
 *
 * @param [in] device Points to the I2C target-device configuration/descriptor
 * @param [in] transfer Points to the I2C transfer
 *
 * @return int 0 for success or error code.
 */
int capi_i2c_transmit(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer);

/**
 * @brief Sync/Blocking receive function.
 *
 * @param [in] device Points to the I2C target-device configuration/descriptor
 * @param [in] transfer Points to the I2C transfer
 *
 * @return int 0 for success or error code.
 */
int capi_i2c_receive(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer);

/**
 * @brief Register callback to get async events
 *
 * @param [in] handle Points to the I2C controller context.
 * @param [in] callback User callback function
 * @param [in] callback_arg User callback arg
 *
 * @return int 0 for success or error code.
 */
int capi_i2c_register_callback(struct capi_i2c_controller_handle *handle,
			       capi_i2c_callback *const callback, void *const callback_arg);

/**
 * @brief Async/Non-blocking transmit function.
 *
 * @param [in] device Points to the I2C target-device configuration/descriptor
 * @param [in] transfer Points to the I2C transfer
 *
 * @return int 0 for success or error code.
 */
int capi_i2c_transmit_async(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer);

/**
 * @brief Async/Non-blocking receive function.
 *
 * @param [in] device Points to the I2C target-device configuration/descriptor
 * @param [in] transfer Points to the I2C transfer
 *
 * @return int 0 for success or error code.
 */
int capi_i2c_receive_async(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer);

/**
 * @brief I2C Driver Interrupt handler. If interrupt vectors are managed and implemented by user,
 * then user shall call this function in the relevant interrupt vector function.
 *
 * @param [in] handle Points to the I2C controller context
 */
void capi_i2c_isr(struct capi_i2c_controller_handle *handle);

/**
 * @brief Structure holding I2C function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_i2c_ops {
	/** See capi_i2c_init() */
	int (*init)(struct capi_i2c_controller_handle **handle,
		    const struct capi_i2c_config *config);
	/** See capi_i2c_deinit() */
	int (*deinit)(struct capi_i2c_controller_handle *handle);
	/** See capi_i2c_transmit() */
	int (*transmit)(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer);
	/** See capi_i2c_receive() */
	int (*receive)(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer);
	/** See capi_i2c_register_callback() */
	int (*register_callback)(struct capi_i2c_controller_handle *handle,
				 capi_i2c_callback *const callback, void *const callback_arg);
	/** See capi_i2c_transmit_async() */
	int (*transmit_async)(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer);
	/** See capi_i2c_receive_async() */
	int (*receive_async)(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer);
	/** See capi_i2c_isr() */
	void (*isr)(struct capi_i2c_controller_handle *handle);
};

#if defined(__cplusplus)
}
#endif

#endif
