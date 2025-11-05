/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common API SPI
 */

#ifndef _CAPI_SPI_MIO_H_
#define _CAPI_SPI_MIO_H_

#include <stdbool.h>
#include <stdint.h>

#include <capi_spi.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/** Forward Declarations */

/**
 * @brief forward declaration of common hal GPIO pin desc
 */
struct capi_gpio_pin;

/**
 * @brief forward declaration of common hal DMA controller handle
 */
struct capi_dma_handle;

/**
 * @brief MIO SPI controller configuration
 */
struct capi_spi_mio_config {
	/** ops - optional. if NULL, API selects driver from static mapping based on
	 * identifier */
	const struct capi_spi_mio_ops *ops;
	/** MIO SPI controller identifier:
	 * - Base address for internal controllers.
	 * - Device specific address for external controllers (e.g., SPI
	 *   expanders, SPI bridges).
	 */
	uint64_t identifier;
	/** optional - user shall pass common hal dma handle to enable dma for async transfers */
	struct capi_dma_handle *dma_handle;
	/** Loopback enable, for CAPI_SPI_MIO_NONE connects MISO to MOSI during configuration.
	 * This is mostly for test purposes.
	 */
	bool loopback;
	/** Frequency of peripheral clock in Hz, if zero, driver sets to default of platform */
	uint32_t clk_freq_hz;
	/** Optional platform specific extra configuration */
	void *extra;
};

/**
 * @brief MIO SPI controller handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_spi_mio_controller_handle {
	const struct capi_spi_mio_ops *ops; /**< set and used by capi thin layer */
	bool init_allocated;                /**< If true, the driver is owner of handle memory. */
	void *lock; /**< set and used by capi thin layer if mux is available */
	void *priv; /**< set and used by user optionally */
};

/** @brief Multiple I/O Modes */
enum capi_spi_mio_mode {
	CAPI_SPI_MIO_NONE,
	CAPI_SPI_MIO_DUAL,
	CAPI_SPI_MIO_QUAD,
	CAPI_SPI_MIO_OCTAL,
};

/** @brief Word Transfer Size */
enum capi_spi_mio_word_size {
	CAPI_SPI_MIO_8BIT_WORD,
	CAPI_SPI_MIO_16BIT_WORD,
	CAPI_SPI_MIO_32BIT_WORD,
};

/**
 * @struct capi_spi_mio_mem_map_cfg
 * @brief Structure that contains the memory mapped configuration for SPI MIO.
 */
struct capi_spi_mio_mem_map_cfg {
	bool enable;       /**< Enable Memory mapped mode */
	uint8_t opcode;    /**< Read Opcode */
	uint8_t adr_size;  /**< Bytes of Read Address */
	uint32_t top_addr; /**< SPI Memory Top Address */
};

/**
 * @brief MIO SPI target-device configuration/descriptor
 */
struct capi_spi_mio_device {
	/** MIO SPI controller this device is connected to. */
	struct capi_spi_mio_controller_handle *controller;
	/** Maximum transfer speed supported by the device. */
	uint32_t max_speed_hz;
	/** Multiple I/O mode */
	enum capi_spi_mio_mode miom;
	/** SPI mode (e.g., phase, polarity). */
	enum capi_spi_mode mode;
	/** Native chip-selects bitmask (multiple CSs selection is allowed). */
	uint16_t native_cs;
	/** GPIO chip-selects descriptors (multiple GPIO CSs are allowed).
	 *  user shall include "capi_gpio.h" to reach gpio api */
	struct capi_gpio_pin *cs_gpio;
	/** Number of GPIO chip-selects. */
	uint8_t cs_gpio_num;
	/** flow control parameters */
	struct capi_spi_flow_ctl_params flow_ctl_param;
	/** LSB First Transfer; true: LSB first, false: MSB first */
	bool lsb_first;
	/** Memory map mode cfg */
	struct capi_spi_mio_mem_map_cfg mmap_cfg;
	/** Transfer Word Size */
	enum capi_spi_mio_word_size word_size;
	/** Enable Open Drain Mode */
	bool odm_enable;
	/** Optional platform specific extra configuration */
	void *extra;
};

/**
 * @brief Initialize an instance of the MIO SPI controller.
 *
 * @param [in out] handle Points to a pointer of the context structure. If this
 *                 pointer is set to NULL, then the controller will allocate
 *	               the context structure and be the owner. If the pointer
 *	               is not NULL, the application has allocated the structure and is
 *	               the owner.
 * @param [in] config Points to the configuration for the MIO SPI controller.
 *
 * @return int 0 for success or error code.
 */
int capi_spi_mio_init(struct capi_spi_mio_controller_handle **handle,
		      const struct capi_spi_mio_config *config);

/**
 * @brief Deinitialize the MIO SPI controller, disable, and bring to default settings.
 *
 * @param [in] handle Points to the MIO SPI controller context.
 *
 * @return int 0 for success or error code.
 */
int capi_spi_mio_deinit(struct capi_spi_mio_controller_handle *handle);

/**
 * @brief Sync/Blocking transceive function.
 *
 * @param [in] device Points to the MIO SPI target-device configuration/descriptor
 * @param [in] transfer Points to the SPI transfer
 *
 * @return int 0 for success or error code.
 */
int capi_spi_mio_transceive(struct capi_spi_mio_device *device, struct capi_spi_transfer *transfer);

/**
 * @brief Async/Non-Blocking transceive function.
 *
 * @param [in] device Points to the MIO SPI target-device configuration/descriptor
 * @param [in] transfer Points to the SPI transfer
 *
 * @return int 0 for success or error code.
 */
int capi_spi_mio_transceive_async(struct capi_spi_mio_device *device,
				  struct capi_spi_transfer *transfer);

/**
 * @brief Abort recent async transfer operation
 *
 * @param [in] device Points to the MIO SPI target-device configuration/descriptor
 *
 * @return int 0 for success or error code.
 */
int capi_spi_mio_abort_async(struct capi_spi_mio_device *device);

/**
 * @brief Register callback to get async events
 *
 * @param [in] handle Points to the MIO SPI controller context.
 * @param [in] callback User callback function
 * @param [in] callback_arg User callback arg
 *
 * @return int 0 for success or error code.
 */
int capi_spi_mio_register_callback(struct capi_spi_mio_controller_handle *handle,
				   capi_spi_callback_t const callback, void *callback_arg);

/**
 * @brief Set chip select mode/state.
 *
 * @param [in] device Points to the MIO SPI target-device configuration/descriptor
 * @param [in] cs_control chip select mode/level
 *
 * @return int 0 for success or error code.
 */
int capi_spi_mio_set_cs(struct capi_spi_mio_device *device, enum capi_spi_cs_control cs_control);

/**
 * @brief Set memory mapped configuration
 *
 * @param[in] device Pointer to MIO SPI Device
 * @param[in] cfg Memory mapped mode configuration
 *
 * @return int 0 for success or error code.
 */
int capi_spi_mio_set_memory_mapped_mode(const struct capi_spi_mio_device *device,
					struct capi_spi_mio_mem_map_cfg *cfg);

/**
 * @brief SPI Driver Interrupt handler. If interrupt vectors are managed and implemented by user,
 * then user shall call this function in the relevant interrupt vector function.
 *
 * @param [in] handle Points to the MIO SPI controller context
 */
void capi_spi_mio_isr(struct capi_spi_mio_controller_handle *handle);

/**
 * @brief Structure holding MIO SPI function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_spi_mio_ops {
	/** See capi_spi_mio_init() */
	int (*init)(struct capi_spi_mio_controller_handle **handle,
		    const struct capi_spi_mio_config *config);
	/** See capi_spi_mio_deinit() */
	int (*deinit)(struct capi_spi_mio_controller_handle *handle);
	/** See capi_spi_mio_transceive() */
	int (*transceive)(struct capi_spi_mio_device *device, struct capi_spi_transfer *transfer);
	/** See capi_spi_mio_transceive_async() */
	int (*transceive_async)(struct capi_spi_mio_device *device,
				struct capi_spi_transfer *transfer, int timeout);
	/** See capi_spi_mio_abort_async() */
	int (*abort_async)(struct capi_spi_mio_device *device);
	/** See capi_spi_mio_register_callback() */
	int (*register_callback)(struct capi_spi_mio_controller_handle *handle,
				 capi_spi_callback_t const callback, void *callback_arg);
	/** See capi_spi_mio_set_cs() */
	int (*set_cs)(struct capi_spi_mio_device *device, enum capi_spi_cs_control cs);
	/** See capi_spi_mio_set_memory_mapped_mode() */
	int (*set_memory_mapped_mode)(const struct capi_spi_mio_device *device,
				      struct capi_spi_mio_mem_map_cfg *cfg);
	/** See capi_spi_mio_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif

#endif
