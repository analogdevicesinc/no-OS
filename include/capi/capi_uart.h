/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL UART API
 */

#ifndef _CAPI_UART_H_
#define _CAPI_UART_H_

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
 * @brief uart line status flags
 */
enum capi_uart_line_status_flags {
	CAPI_UART_LINE_STAT_BREAK_IND = 1U,     /**< Break indicator (data line was held low for
						 * longer than a full character transmission time) */
	CAPI_UART_LINE_STAT_FRAMING_ERROR = 2U, /**< Framing error (invalid stop bit is found) */
	CAPI_UART_LINE_STAT_PARITY_ERROR = 4U,  /**< Parity error */
	CAPI_UART_LINE_STAT_OVERRUN_ERROR = 8U, /**< The receive buffer was full, and a new byte
						 * arrived before the previous one was read */
};

/**
 * @brief uart interrupt reasons
 */
enum capi_uart_interrupt_reason {
	CAPI_UART_INTR_MODEM_STATUS,    /**< Modem status interrupt */
	CAPI_UART_INTR_TX_BUFFER_EMPTY, /**< Transmit buffer empty interrupt */
	CAPI_UART_INTR_RX_BUFFER_FULL,  /**< Receive buffer full interrupt */
	CAPI_UART_INTR_RX_LINE_STATUS,  /**< Receive line status interrupt,
					 * Details can be get by calling capi_uart_get_line_status()
					 */
};

/**
 * @brief uart async events
 */
enum capi_uart_async_event {
	CAPI_UART_EVENT_TX_DONE,    /**< Transfer completed */
	CAPI_UART_EVENT_TX_ABORTED, /**< Transfer aborted */
	CAPI_UART_EVENT_RX_DONE,    /**< Rx completed */
	CAPI_UART_EVENT_RX_TIMEOUT, /**< Rx timeout */
	CAPI_UART_EVENT_INTERRUPT,  /**< UART interrupt,
				     * reason can be get by calling capi_uart_get_interrupt_reason()
				     */
};

/**
 * @brief number of data bits
 */
enum capi_uart_data_bits {
	CAPI_UART_DATA_BITS_8, /**< 8 data bits */
	CAPI_UART_DATA_BITS_7, /**< 7 data bits */
	CAPI_UART_DATA_BITS_6, /**< 6 data bits */
	CAPI_UART_DATA_BITS_5, /**< 5 data bits */
};

/**
 * @brief uart parity options
 */
enum capi_uart_parity {
	CAPI_UART_PARITY_NONE,  /**< no parity */
	CAPI_UART_PARITY_MARK,  /**< mark parity */
	CAPI_UART_PARITY_SPACE, /**< space parity */
	CAPI_UART_PARITY_ODD,   /**< odd parity */
	CAPI_UART_PARITY_EVEN   /**< even parity */
};

/**
 * @brief uart number of stop bits options
 */
enum capi_uart_stop_bits {
	CAPI_UART_STOP_1_BIT, /**< one stop bit */
	CAPI_UART_STOP_2_BIT  /**< two stop bits */
};

/**
 * @brief uart flow control options
 */
enum capi_uart_flow_control {
	CAPI_UART_FLOW_CONTROL_NONE,  /**< no flow control */
	CAPI_UART_FLOW_CONTROL_RTS,   /**< RTS flow control */
	CAPI_UART_FLOW_CONTROL_CTS,   /**< CTS flow control */
	CAPI_UART_FLOW_CONTROL_RTSCTS /**< RTS/CTS flow control */
};

/**
 * @brief uart address mode for multi-drop communication
 */
enum capi_uart_address_mode {
	CAPI_UART_ADDRESS_MODE_DISABLED, /**< normal 8-bit mode */
	CAPI_UART_ADDRESS_MODE_9BIT,     /**< 9-bit address mode */
	CAPI_UART_ADDRESS_MODE_MULTIDROP /**< multi-drop mode with address detection */
};

/**
 * @brief UART line configuration
 */
struct capi_uart_line_config {
	/** UART baudrate in bps. Valid range: 300 - 4000000 */
	uint32_t baudrate;
	/** UART number of data bits. */
	enum capi_uart_data_bits size;
	/** UART parity */
	enum capi_uart_parity parity;
	/** UART number of stop bits */
	enum capi_uart_stop_bits stop_bits;
	/** Flow control configuration */
	enum capi_uart_flow_control flow_control;
	/** Address mode for multi-drop communication */
	enum capi_uart_address_mode address_mode;
	/** Device address for multi-drop mode (0-255) */
	uint8_t device_address;
	/** Sticky parity enable/disable */
	bool sticky_parity;
	/** Loopback mode enable/disable */
	bool loopback;
};

/**
 * @brief UART controller configuration
 */
struct capi_uart_config {
	/** UART controller identifier:
	 * - Base address for internal controllers.
	 * - Device address on uart bus for external controllers
	 *   (e.g.,  UART Mux, UART to I2C Bridge).
	 */
	uint32_t identifier;
	/** optional - user shall pass common hal dma handle to enable dma for async transfers */
	struct capi_dma_handle *dma_handle;
	/** Frequency of peripheral clock in Hz, if zero, driver sets to default of platform */
	uint32_t clk_freq_hz;
	/** Pointer to UART line configuration. If NULL, HW defaults will be active */
	struct capi_uart_line_config *line_config;
	/** Optional platform specific extra configuration. */
	void *extra;
	/** Platform specific implementation of the API. (Mandatory field, but can be NULL and we'll
	 * make sure it gets auto populated when using our build system.) */
	const struct capi_uart_ops *ops;
};

/**
 * @brief UART handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_uart_handle {
	const struct capi_uart_ops *ops; /**< set and used by capi thin layer */
	bool init_allocated;             /**< If true, the driver is owner of handle memory. */
	void *lock; /**< set and used by capi thin layer if locking is available */
	void *priv; /**< set and used by user optionally */
};

/**
 * @brief  UART Driver Call Back type
 * @param[in] event UART async event type
 * @param[in] arg Pointer to user specific data.
 * @param[in] event_extra optional, platform/driver specific extra information for event
 */
typedef void (*capi_uart_callback)(enum capi_uart_async_event event, void *arg, int event_extra);

/**
 * @brief Initialize an instance of the UART controller.
 *
 * @param [in out] handle Points to a pointer of the context structure. If this
 *                 pointer is set to NULL, then the controller will allocate
 *	               the context structure and be the owner. If the pointer
 *	               is not NULL, the application has allocated the structure and is
 *	               the owner.
 * @param [in] config Points to the configuration for the UART controller.
 *
 * @return int 0 for success or error code.
 */
int capi_uart_init(struct capi_uart_handle **handle, const struct capi_uart_config *config);

/**
 * @brief Deinitialize the UART controller, disable, and bring to default settings.
 *
 * @param [in] handle Points to the UART controller context.
 *
 * @return int 0 for success or error code.
 */
int capi_uart_deinit(struct capi_uart_handle *handle);

/**
 * @brief Get line config of UART.
 * If not set before, this function shall return startup/default config.
 * User may need to get defaults from HW and change individual settings.
 *
 * @param [in] handle Points to the UART controller context.
 * @param [out] line_config returns the line config.
 *
 * @return int 0 for success or error code.
 */
int capi_uart_get_line_config(struct capi_uart_handle *handle,
			      struct capi_uart_line_config *line_config);

/**
 * @brief Set line config of UART.
 *
 * @param [in] handle Points to the UART controller context.
 * @param [in] line_config Points to the UART line config.
 *
 * @return int 0 for success or error code.
 */
int capi_uart_set_line_config(struct capi_uart_handle *handle,
			      struct capi_uart_line_config *line_config);

/* HW fifo related functions */

/**
 * @brief Enable/Disable HW fifo of UART controller
 *
 * @param [in] handle Points to the UART controller context.
 * @param [in] enable enabled state of UART FIFO
 *
 * @return int 0 for success or error code.
 */
int capi_uart_enable_fifo(struct capi_uart_handle *handle, bool enable);

/**
 * @brief Flush Tx fifo of UART controller
 *
 * @param [in] handle Points to the UART controller context.
 *
 * @return int 0 for success or error code.
 */
int capi_uart_flush_tx_fifo(struct capi_uart_handle *handle);

/**
 * @brief Flush Rx fifo of UART controller
 *
 * @param [in] handle Points to the UART controller context.
 *
 * @return int 0 for success or error code.
 */
int capi_uart_flush_rx_fifo(struct capi_uart_handle *handle);

/**
 * @brief Get available bytes on Rx fifo of UART controller
 *
 * @param [in] handle Points to the UART controller context.
 * @param [out] count returns number of bytes in UART Rx FIFO
 *
 * @return int 0 for success or error code.
 */
int capi_uart_get_rx_fifo_count(struct capi_uart_handle *handle, uint16_t *count);

/**
 * @brief Get available bytes on Tx fifo of UART controller
 *
 * @param [in] handle Points to the UART controller context.
 * @param [out] count returns number of bytes in UART Tx FIFO
 *
 * @return int 0 for success or error code.
 */
int capi_uart_get_tx_fifo_count(struct capi_uart_handle *handle, uint16_t *count);

/**
 * @brief Sync/Blocking transmit function.
 *
 * @param [in] handle Points to the UART controller context.
 * @param [in] buf Points to the user buffer holding data
 * @param [in] len number of bytes to be transmitted
 *
 * @return int 0 for success or error code.
 */
int capi_uart_transmit(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len);

/**
 * @brief Sync/Blocking receive function.
 *
 * @param [in] handle Points to the UART controller context.
 * @param [out] buf Points to the user buffer for incoming data
 * @param [in] len number of bytes to be received
 *
 * @return int 0 for success or error code.
 */
int capi_uart_receive(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len);

/**
 * @brief Register callback to get async events
 *
 * @param [in] handle Points to the UART controller context.
 * @param [in] callback User callback function
 * @param [in] callback_arg User callback arg
 *
 * @return int 0 for success or error code.
 */
int capi_uart_register_callback(struct capi_uart_handle *handle, capi_uart_callback const callback,
				void *const callback_arg);

/**
 * @brief Async/Non-Blocking transmit function.
 *
 * @param [in] handle Points to the UART controller context.
 * @param [in] buf Points to the user buffer holding data
 * @param [in] len number of bytes to be transmitted
 *
 * @return int 0 for success or error code.
 */
int capi_uart_transmit_async(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len);

/**
 * @brief Async/Non-Blocking receive function.
 *
 * @param [in] handle Points to the UART controller context.
 * @param [out] buf Points to the user buffer for incoming data
 * @param [in] len number of bytes to be received
 *
 * @return int 0 for success or error code.
 */
int capi_uart_receive_async(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len);

/**
 * @brief Get UART interrupt reason.
 * if CAPI_UART_EVENT_INTERRUPT event occurs, user can get the reason with this api
 *
 * @param [in] handle Points to the UART controller context.
 * @param [out] reason returns interrupt reason
 *
 * @return int 0 for success or error code.
 */
int capi_uart_get_interrupt_reason(struct capi_uart_handle *handle,
				   enum capi_uart_interrupt_reason *reason);

/**
 * @brief Get UART line status flags
 * if the interrupt reason is CAPI_UART_INTR_RX_LINE_STATUS,
 * user can get the line status flags with this api
 *
 * @param [in] handle Points to the UART controller context.
 * @param [out] status_flags returns status flags. Check enum capi_uart_line_status_flags
 *
 * @return int 0 for success or error code.
 */
int capi_uart_get_line_status(struct capi_uart_handle *handle, uint32_t *status_flags);

/**
 * @brief Send 9-bit data for multi-drop communication
 *
 * @param [in] handle Points to the UART controller context.
 * @param [in] data 9-bit data to transmit (only lower 9 bits used)
 * @param [in] is_address true if this is an address byte, false for data
 *
 * @return int 0 for success or error code.
 */
int capi_uart_transmit_9bit(struct capi_uart_handle *handle, uint16_t data, bool is_address);

/**
 * @brief Receive 9-bit data for multi-drop communication
 *
 * @param [in] handle Points to the UART controller context.
 * @param [out] data received 9-bit data (only lower 9 bits valid)
 * @param [out] is_address true if this is an address byte, false for data
 *
 * @return int 0 for success or error code.
 */
int capi_uart_receive_9bit(struct capi_uart_handle *handle, uint16_t *data, bool *is_address);

/**
 * @brief Set flow control state manually
 *
 * @param [in] handle Points to the UART controller context.
 * @param [in] rts_state RTS signal state (true = asserted, false = deasserted)
 * @param [in] cts_state CTS signal state (true = asserted, false = deasserted)
 *
 * @return int 0 for success or error code.
 */
int capi_uart_set_flow_control_state(struct capi_uart_handle *handle, bool rts_state,
				     bool cts_state);

/**
 * @brief Get flow control state
 *
 * @param [in] handle Points to the UART controller context.
 * @param [out] rts_state RTS signal state (true = asserted, false = deasserted)
 * @param [out] cts_state CTS signal state (true = asserted, false = deasserted)
 *
 * @return int 0 for success or error code.
 */
int capi_uart_get_flow_control_state(struct capi_uart_handle *handle, bool *rts_state,
				     bool *cts_state);

/**
 * @brief UART Driver Interrupt handler. If interrupt vectors are managed and implemented by user,
 * then user shall call this function in the relevant interrupt vector function.
 *
 * @param [in] handle Points to the UART controller context
 */
void capi_uart_isr(void *handle);

/**
 * @brief Structure holding UART function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_uart_ops {
	/** See capi_uart_init() */
	int (*init)(struct capi_uart_handle **handle, const struct capi_uart_config *config);
	/** See capi_uart_deinit() */
	int (*deinit)(struct capi_uart_handle *handle);
	/** See capi_uart_get_line_config() */
	int (*get_line_config)(struct capi_uart_handle *handle,
			       struct capi_uart_line_config *line_config);
	/** See capi_uart_set_line_config() */
	int (*set_line_config)(struct capi_uart_handle *handle,
			       struct capi_uart_line_config *line_config);
	/** See capi_uart_enable_fifo() */
	int (*enable_fifo)(struct capi_uart_handle *handle, bool enable);
	/** See capi_uart_flush_tx_fifo() */
	int (*flush_tx_fifo)(struct capi_uart_handle *handle);
	/** See capi_uart_flush_rx_fifo() */
	int (*flush_rx_fifo)(struct capi_uart_handle *handle);
	/** See capi_uart_get_rx_fifo_count() */
	int (*get_rx_fifo_count)(struct capi_uart_handle *handle, uint16_t *count);
	/** See capi_uart_get_tx_fifo_count() */
	int (*get_tx_fifo_count)(struct capi_uart_handle *handle, uint16_t *count);
	/** See capi_uart_transmit() */
	int (*transmit)(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len);
	/** See capi_uart_receive() */
	int (*receive)(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len);
	/** See capi_uart_register_callback() */
	int (*register_callback)(struct capi_uart_handle *handle, capi_uart_callback const callback,
				 void *const callback_arg);
	/** See capi_uart_transmit_async() */
	int (*transmit_async)(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len);
	/** See capi_uart_receive_async() */
	int (*receive_async)(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len);
	/** See capi_uart_get_interrupt_reason() */
	int (*get_interrupt_reason)(struct capi_uart_handle *handle,
				    enum capi_uart_interrupt_reason *reason);
	/** See capi_uart_get_line_status() */
	int (*get_line_status)(struct capi_uart_handle *handle, uint32_t *status_flags);
	/** See capi_uart_transmit_9bit() */
	int (*transmit_9bit)(struct capi_uart_handle *handle, uint16_t data, bool is_address);
	/** See capi_uart_receive_9bit() */
	int (*receive_9bit)(struct capi_uart_handle *handle, uint16_t *data, bool *is_address);
	/** See capi_uart_set_flow_control_state() */
	int (*set_flow_control_state)(struct capi_uart_handle *handle, bool rts_state,
				      bool cts_state);
	/** See capi_uart_get_flow_control_state() */
	int (*get_flow_control_state)(struct capi_uart_handle *handle, bool *rts_state,
				      bool *cts_state);
	/** See capi_uart_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif

#endif
