/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common API SPI
 */

#ifndef _CAPI_SPI_H_
#define _CAPI_SPI_H_

#include <stdbool.h>
#include <stdint.h>

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
 * @brief SPI controller configuration
 */
struct capi_spi_config {
	/** ops - optional. if NULL, API selects driver from static mapping based on
	 * identifier */
	const struct capi_spi_ops *ops;
	/** SPI controller identifier:
	 * - Base address for internal controllers.
	 * - Device specific address for external controllers (e.g., SPI
	 *   expanders, SPI bridges).
	 */
	uint64_t identifier;
	/** optional - user shall pass common hal dma handle to enable dma for async transfers */
	struct capi_dma_handle *dma_handle;
	/** Enables 3 pin mode of operation */
	bool three_pin_mode;
	/** Loopback enable, connects MISO to MOSI during configuration */
	bool loopback;
	/** Frequency of peripheral clock in Hz, if zero, driver sets to default of platform */
	uint32_t clk_freq_hz;
	/** Optional platform specific extra configuration */
	void *extra;
};

/**
 * @brief SPI controller handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_spi_controller_handle {
	const struct capi_spi_ops *ops; /**< set and used by capi thin layer */
	bool init_allocated;            /**< If true, the driver is owner of handle memory. */
	void *lock;                     /**< set and used by capi thin layer if mux is available */
	void *priv;                     /**< set and used by user optionally */
};

/**
 * @name SPI modes
 * @{
 */

/** The CPHA bit selects the clock phase. */
#define CAPI_SPI_CPHA 0x01

/** The CPOL bit sets the polarity of the clock signal during the idle state. */
#define CAPI_SPI_CPOL 0x02

/**
 * @enum capi_spi_mode
 * @brief SPI communication modes defining clock polarity and phase.
 */
enum capi_spi_mode {
	/** Data sampled on rising edge and shifted out on the falling edge. */
	CAPI_SPI_MODE_0 = (0 | 0),
	/** Data sampled on the falling edge and shifted out on the rising edge. */
	CAPI_SPI_MODE_1 = (0 | CAPI_SPI_CPHA),
	/** Data sampled on the falling edge and shifted out on the rising edge. */
	CAPI_SPI_MODE_2 = (CAPI_SPI_CPOL | 0),
	/** Data sampled on the rising edge and shifted out on the falling edge. */
	CAPI_SPI_MODE_3 = (CAPI_SPI_CPOL | CAPI_SPI_CPHA)
};

/** @} */

/**
 * @brief SPI polarity of RDY/MISO line for flow control
 */
enum capi_spi_rdy_pol {
	CAPI_SPI_RDY_POL_HIGH,
	CAPI_SPI_RDY_POL_LOW,
};

/**
 * @brief SPI flow control mode
 */
enum capi_spi_flow_ctl {
	CAPI_SPI_FLOW_CTL_DISABLE, /**< No flow control */
	CAPI_SPI_FLOW_CTL_TIMER, /**< Timer based flow control. Time unit is number of clock cycles.
				  * If read burst size is less than transfer count, initiator enters
				  * wait state after transferring burst size of bytes.
				  */
	CAPI_SPI_FLOW_CTL_RDY,   /** Ready pin based flow control
				  * In this mode, the initiator waits for an active level on the RDYIN
				  * pin.   Once it detects the transition, it transfers burts size of
				  * bytes   and then goes back to wait state until another active
				  * level   is detected. */
	CAPI_SPI_FLOW_CTL_MISO,  /** In this mode, the initiator waits for an active level on the
				  * MISO pin.  Once it detects the transition, it transfers burts
				  * size  of bytes  and then goes back to wait state until another
				  * active  level is detected. */
};

/**
 * @brief Flow control configuration for data reads
 */
struct capi_spi_flow_ctl_params {
	enum capi_spi_flow_ctl mode; /**< Flow coontrol mode */
	uint16_t burts_size;         /**< if non-zero, specifies the limit of number of bytes to be
				      * transferred at once.
				      * See flow control mode descriptions for further information. */
	enum capi_spi_rdy_pol
		rdy_pol;   /**< Polarity of the Readt Pin, used when ready pin based flow ctrl */
	uint16_t wait_tmr; /**< number of clock cycles to wait, used when timer based flow ctrl.
			    * See flow control mode descriptions for further information */
};

/**
 * @brief SPI target-device configuration/descriptor
 */
struct capi_spi_device {
	/** SPI controller this device is connected to. */
	struct capi_spi_controller_handle *controller;
	/** Maximum transfer speed supported by the device. */
	uint32_t max_speed_hz;
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
	/** sets byte based CS assert/de-assert transaction */
	bool non_continuous_mode;
	/** LSB First Transfer; true: LSB first, false: MSB first */
	bool lsb_first;
	/** Optional platform specific extra configuration */
	void *extra;
};

/**
 * @brief CS control
 */
enum capi_spi_cs_control {
	CAPI_SPI_CS_AUTO,            /**< default setting for supported HW */
	CAPI_SPI_CS_MANUAL_ASSERT,   /**< User controlled CS assert state */
	CAPI_SPI_CS_MANUAL_DEASSERT, /**< User controlled CS de-assert state */
};

/**
 * @brief  SPI transfer
 */
struct capi_spi_transfer {
	const uint8_t *tx_buf; /**< transmit buffer, if NULL zeroes will be transmitted */
	uint8_t *rx_buf;       /**< receive buffer, if NULL rx bytes will be discarded */
	uint16_t tx_size;      /**< tx size */
	uint16_t rx_size;      /**< rx size, rx size can be different from tx
				*   for read command operations */
	int timeout;           /**< if supported, timeout in milliseconds. 0 or -1 means infinite */
	uint8_t xfer_delay_clk_cycles; /**< Transfer Delay time in clock cycles */
};

/** @brief spi async events */
enum capi_async_event {
	CAPI_SPI_EVENT_XFR_DONE = 1,      /**< transfer complete */
	CAPI_SPI_EVENT_XFR_TIMEOUT,       /**< Transfer timeout */
	CAPI_SPI_EVENT_TX_IRQ,            /**< required number of bytes transmitted
					   * see capi_spi_set_transfer_irq_trig_level()
					   */
	CAPI_SPI_EVENT_TX_FIFO_UNDERFLOW, /**< Tx FIFO underflow event */
	CAPI_SPI_EVENT_RX_FIFO_OVERFLOW,  /**< Rx FIFO overflow event */
	CAPI_SPI_EVENT_ERROR,             /**< Generic error event,
					   * details may be provided by driver in extra
					   */
	CAPI_SPI_EVENT_TX_COLLISION,      /**< There has been a HW Error: TX Collision */
	CAPI_SPI_EVENT_OTHER              /**< Other events,
					   * details may be provided by driver in extra
					   */
};

/**
 * @brief  SPI Driver Call Back type
 * @param[in] event SPI event type
 * @param[in] arg Pointer to user specific data.
 * @param[in] event_extra optional, platform/driver specific extra information for event
 */
typedef void (*capi_spi_callback_t)(enum capi_async_event event, void *arg, int event_extra);

/**
 * @brief Initialize an instance of the SPI controller.
 *
 * @param [in out] handle Points to a pointer of the context structure. If this
 *                 pointer is set to NULL, then the controller will allocate
 *	               the context structure and be the owner. If the pointer
 *	               is not NULL, the application has allocated the structure and is
 *	               the owner.
 * @param [in] config Points to the configuration for the SPI controller.
 *
 * @return int 0 for success or error code.
 */
int capi_spi_init(struct capi_spi_controller_handle **handle, const struct capi_spi_config *config);

/**
 * @brief Deinitialize the SPI controller, disable, and bring to default settings.
 *
 * @param [in] handle Points to the SPI controller context.
 *
 * @return int 0 for success or error code.
 */
int capi_spi_deinit(struct capi_spi_controller_handle *handle);

/**
 * @brief Sync/Blocking transceive function.
 *
 * @param [in] device Points to the SPI target-device configuration/descriptor
 * @param [in] transfer Points to the SPI transfer
 *
 * @return int 0 for success or error code.
 */
int capi_spi_transceive(struct capi_spi_device *device, struct capi_spi_transfer *transfer);

/**
 * @brief Async/Non-Blocking transceive function.
 *
 * @param [in] device Points to the SPI target-device configuration/descriptor
 * @param [in] transfer Points to the SPI transfer
 *
 * @return int 0 for success or error code.
 */
int capi_spi_transceive_async(struct capi_spi_device *device, struct capi_spi_transfer *transfer);

/**
 * @brief Sync/Blocking read command style transceive function.
 * Although the SPI transfers are usually full-duplex, in many cases, the target works on a protocol
 * which has a “Command + Address + Read/Write-data” sequence. The write command is always
 * unidirectional. However, the read command needs a Tx transfer followed by an Rx transfer in a
 * single CS frame.
 *
 * Example-1: Tx 2 bytes (instruction, address) Rx 1 byte
 *            MOSI ____<instruction><  address  >______________
 *            MISO ______________________________<  data_byte  >
 *
 * Example-2: Write-only, Tx 3 bytes (instruction, address, data), Rx size 0
 *            MOSI ____<instruction><  address  ><  data_byte  >
 *            MISO ____________________________________________
 *
 * @param [in] device Points to the SPI target-device configuration/descriptor
 * @param [in] transfer Points to the SPI transfer
 *
 * @return int 0 for success or error code.
 */
int capi_spi_read_command(struct capi_spi_device *device, struct capi_spi_transfer *transfer);

/**
 * @brief Async/Non-Blocking read command style transceive function.
 * See blocking version of this function for description
 *
 * @param [in] device Points to the SPI target-device configuration/descriptor
 * @param [in] transfer Points to the SPI transfer
 *
 * @return int 0 for success or error code.
 */
int capi_spi_read_command_async(struct capi_spi_device *device, struct capi_spi_transfer *transfer);

/**
 * @brief Abort recent async transfer operation
 *
 * @param [in] device Points to the SPI target-device configuration/descriptor
 *
 * @return int 0 for success or error code.
 */
int capi_spi_abort_async(struct capi_spi_device *device);

/**
 * @brief Register callback to get async events
 *
 * @param [in] handle Points to the SPI controller context.
 * @param [in] callback User callback function
 * @param [in] callback_arg User callback arg
 *
 * @return int 0 for success or error code.
 */
int capi_spi_register_callback(struct capi_spi_controller_handle *handle,
			       capi_spi_callback_t const callback, void *callback_arg);

/**
 * @brief Set chip select mode/state.
 *
 * @param [in] device Points to the SPI target-device configuration/descriptor
 * @param [in] cs_control chip select mode/level
 *
 * @return int 0 for success or error code.
 */
int capi_spi_set_cs(struct capi_spi_device *device, enum capi_spi_cs_control cs_control);

/**
 * @brief SPI Driver Interrupt handler. If interrupt vectors are managed and implemented by user,
 * then user shall call this function in the relevant interrupt vector function.
 *
 * @param [in] handle Points to the SPI controller context
 */
void capi_spi_isr(struct capi_spi_controller_handle *handle);

/**
 * @brief Structure holding SPI function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_spi_ops {
	/** See capi_spi_init() */
	int (*init)(struct capi_spi_controller_handle **handle,
		    const struct capi_spi_config *config);
	/** See capi_spi_deinit() */
	int (*deinit)(struct capi_spi_controller_handle *handle);
	/** See capi_spi_transceive() */
	int (*transceive)(struct capi_spi_device *device, struct capi_spi_transfer *transfer);
	/** See capi_spi_transceive_async() */
	int (*transceive_async)(struct capi_spi_device *device, struct capi_spi_transfer *transfer,
				int timeout);
	/** See capi_spi_read_command() */
	int (*read_command)(struct capi_spi_device *device, struct capi_spi_transfer *transfer);
	/** See capi_spi_read_command_async() */
	int (*read_command_async)(struct capi_spi_device *device,
				  struct capi_spi_transfer *transfer);
	/** See capi_spi_abort_async() */
	int (*abort_async)(struct capi_spi_device *device);
	/** See capi_spi_register_callback() */
	int (*register_callback)(struct capi_spi_device *device, capi_spi_callback_t const callback,
				 void *callback_arg);
	/** See capi_spi_set_cs() */
	int (*set_cs)(struct capi_spi_device *device, enum capi_spi_cs_control cs);
	/** See capi_spi_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif

#endif
