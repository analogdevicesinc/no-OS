/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL I3C
 */

#ifndef _CAPI_I3C_H_
#define _CAPI_I3C_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/** I3C broadcast address used for broadcast CCCs */
#define CAPI_I3C_BROADCAST_ADDR 0x7EU

/** Length of the I3C Provisional ID (PID) in bytes */
#define CAPI_I3C_PID_LEN 6U

/** Forward Declarations */

/**
 * @brief Forward declaration of common HAL DMA controller handle
 */
struct capi_dma_handle;

/**
 * @brief I3C async event types
 */
enum capi_i3c_async_event {
	CAPI_I3C_NONE,            /**< No event */
	CAPI_I3C_XFR_DONE,        /**< Transfer completed */
	CAPI_I3C_DAA_DONE,        /**< Dynamic Address Assignment completed */
	CAPI_I3C_NACK,            /**< Target NACKed */
	CAPI_I3C_BUS_ERROR,       /**< Bus error */
	CAPI_I3C_TIMEOUT,         /**< Timeout */
	CAPI_I3C_IBI_REQUEST,     /**< In-Band Interrupt request from target */
	CAPI_I3C_IBI_DONE,        /**< IBI completed */
	CAPI_I3C_HOT_JOIN,        /**< Hot-Join request from target */
	CAPI_I3C_DMA_DONE,        /**< DMA transfer completed */
	CAPI_I3C_FIFO_OVERRUN,    /**< FIFO overrun */
	CAPI_I3C_FIFO_UNDERRUN,   /**< FIFO underrun */
	CAPI_I3C_HDR_MATCHED,     /**< HDR command matched */
	CAPI_I3C_PARITY_ERROR,    /**< Parity error */
	CAPI_I3C_CRC_ERROR,       /**< CRC error */
	CAPI_I3C_DYN_ADDR_CHANGE, /**< Dynamic address changed */
};

/**
 * @brief IBI response policy for the I3C controller (initiator mode).
 *
 * Controls how the controller responds when a target issues an IBI.
 */
enum capi_i3c_ibi_response {
	CAPI_I3C_IBI_RESP_ACK,    /**< ACK all IBIs (based on IBI rules table) */
	CAPI_I3C_IBI_RESP_NACK,   /**< NACK all IBIs */
	CAPI_I3C_IBI_RESP_MANUAL, /**< Manual ACK/NACK from application */
};

/**
 * @brief Device information discovered during DAA.
 *
 * Contains the Provisional ID (PID), Bus Characteristics Register (BCR),
 * Device Characteristics Register (DCR), and the assigned dynamic address.
 */
struct capi_i3c_device_info {
	/** 48-bit Provisional ID (6 bytes, MSB first) */
	uint8_t pid[CAPI_I3C_PID_LEN];
	/** Bus Characteristics Register */
	uint8_t bcr;
	/** Device Characteristics Register */
	uint8_t dcr;
	/** Dynamic address assigned during DAA, or static address */
	uint8_t addr;
	/** true if address was assigned via DAA (dynamic), false if static */
	bool is_dynamic;
};

/**
 * @brief I3C transfer type
 */
enum capi_i3c_xfer_type {
	CAPI_I3C_XFER_I2C,     /**< I2C mode transfer */
	CAPI_I3C_XFER_I3C_SDR, /**< I3C SDR mode transfer */
	CAPI_I3C_XFER_I3C_DDR, /**< I3C HDR-DDR mode transfer */
	CAPI_I3C_XFER_CCC,     /**< Common Command Code broadcast */
};

/**
 * @brief I3C controller configuration
 */
struct capi_i3c_config {
	/** I3C controller identifier (hardware base address, lower 32 bits) */
	uint64_t identifier;
	/** Frequency of peripheral clock in Hz; if 0, driver uses platform default */
	uint32_t clk_freq_hz;
	/** true: Initiator (controller) mode, false: Target mode */
	bool initiator;
	/** Static address when operating as target (7-bit) */
	uint16_t address;
	/** Optional pointer to DMA handle for async transfers */
	struct capi_dma_handle *dma_handle;
	/** Optional platform-specific extra configuration */
	void *extra;
	/** Platform-specific implementation of the API */
	const struct capi_i3c_ops *ops;
};

/**
 * @brief I3C controller handle
 *
 * Drivers may extend via the priv pointer to store driver-specific state.
 */
struct capi_i3c_controller_handle {
	const struct capi_i3c_ops *ops; /**< Set and used by CAPI thin layer */
	bool init_allocated;            /**< If true, driver owns handle memory */
	void *lock;                     /**< Set and used by CAPI thin layer if mutex available */
	void *priv;                     /**< Driver-specific extension data */
};

/**
 * @brief I3C target device descriptor
 */
struct capi_i3c_device {
	/** I3C controller this device is connected to */
	struct capi_i3c_controller_handle *controller;
	/** Device address on the bus (7-bit, not shifted) */
	uint16_t address;
	/** true if this is a legacy I2C device, false for I3C */
	bool is_i2c;
	/** Device ID assigned by the controller during DAA */
	uint8_t dev_id;
	/** Optional platform-specific extra configuration */
	void *extra;
};

/**
 * @brief I3C transfer descriptor
 */
struct capi_i3c_transfer {
	/** Data buffer for read/write */
	uint8_t *buf;
	/** Length of data buffer in bytes */
	uint32_t len;
	/** Transfer type (I2C, I3C SDR, I3C DDR, CCC) */
	enum capi_i3c_xfer_type type;
	/** HDR-DDR command word (used when type is CAPI_I3C_XFER_I3C_DDR) */
	uint16_t hdr_ddr_cmd;
	/** true to issue STOP condition after transfer */
	bool stop;
	/** true to allow IBI from targets during transfer */
	bool allow_ibi;
};

/**
 * @brief I3C driver callback type
 * @param[in] event I3C event type
 * @param[in] arg Pointer to user-specific data
 * @param[in] event_extra Optional platform/driver-specific info for event
 */
typedef void (*capi_i3c_callback)(enum capi_i3c_async_event event, void *arg, int event_extra);

/**
 * @brief Single message within a batch (scatter-gather) transfer.
 *
 * Each message describes one bus segment: a write or read with an optional
 * STOP condition. By chaining messages with @c stop = false, the caller
 * builds atomic multi-segment transactions (e.g., write-then-read with
 * repeated START).
 */
struct capi_i3c_msg {
	/** Data buffer for read/write */
	uint8_t *buf;
	/** Length of data buffer in bytes */
	uint32_t len;
	/** Transfer type (I2C, I3C SDR, I3C DDR) */
	enum capi_i3c_xfer_type type;
	/** HDR-DDR command word (used when type is CAPI_I3C_XFER_I3C_DDR) */
	uint16_t hdr_ddr_cmd;
	/** true for a read transfer, false for a write transfer */
	bool is_read;
	/** true to issue STOP after this segment */
	bool stop;
};

/**
 * @brief Initialize an instance of the I3C controller.
 *
 * @param[in,out] handle Pointer to a pointer of the controller handle. If *handle is NULL,
 *                       the driver allocates the handle (if dynamic allocation enabled).
 * @param[in] config Pointer to the I3C controller configuration.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_init(struct capi_i3c_controller_handle **handle, const struct capi_i3c_config *config);

/**
 * @brief Deinitialize the I3C controller and release resources.
 *
 * @param[in] handle Pointer to the I3C controller handle.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_deinit(struct capi_i3c_controller_handle *handle);

/**
 * @brief Synchronous/blocking transmit.
 *
 * @param[in] device Pointer to the I3C target device descriptor.
 * @param[in] transfer Pointer to the transfer descriptor.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_transmit(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer);

/**
 * @brief Synchronous/blocking receive.
 *
 * @param[in] device Pointer to the I3C target device descriptor.
 * @param[in] transfer Pointer to the transfer descriptor.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_receive(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer);

/**
 * @brief Asynchronous/non-blocking transmit.
 *
 * @param[in] device Pointer to the I3C target device descriptor.
 * @param[in] transfer Pointer to the transfer descriptor.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_transmit_async(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer);

/**
 * @brief Asynchronous/non-blocking receive.
 *
 * @param[in] device Pointer to the I3C target device descriptor.
 * @param[in] transfer Pointer to the transfer descriptor.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_receive_async(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer);

/**
 * @brief Register callback for async event notifications.
 *
 * @param[in] handle Pointer to the I3C controller handle.
 * @param[in] callback User callback function, or NULL to deregister.
 * @param[in] callback_arg User callback argument.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_register_callback(struct capi_i3c_controller_handle *handle,
			       capi_i3c_callback callback, void *callback_arg);

/**
 * @brief Perform Dynamic Address Assignment (blocking).
 *
 * @param[in] handle Pointer to the I3C controller handle.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_do_daa(struct capi_i3c_controller_handle *handle);

/**
 * @brief Start Dynamic Address Assignment (non-blocking).
 *
 * @param[in] handle Pointer to the I3C controller handle.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_do_daa_async(struct capi_i3c_controller_handle *handle);

/**
 * @brief Add a static (I2C) device to the controller's device table.
 *
 * @param[in] handle Pointer to the I3C controller handle.
 * @param[in] addr Static device address (7-bit).
 * @param[out] dev_id Pointer to store the assigned device ID.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_add_static_device(struct capi_i3c_controller_handle *handle, uint8_t addr,
			       uint8_t *dev_id);

/**
 * @brief Get the number of target devices on the bus.
 *
 * @param[in] handle Pointer to the I3C controller handle.
 * @param[out] num_devices Pointer to store the device count.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_get_num_devices(struct capi_i3c_controller_handle *handle, uint8_t *num_devices);

/**
 * @brief Get device information for a discovered device.
 *
 * Retrieves the PID, BCR, DCR, and assigned address for a device
 * that was discovered during DAA or added as a static device.
 *
 * @param[in]  handle  Pointer to the I3C controller handle.
 * @param[in]  dev_id  Device table index (0 to num_devices-1).
 * @param[out] info    Pointer to store the device information.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_get_device_info(struct capi_i3c_controller_handle *handle, uint8_t dev_id,
			     struct capi_i3c_device_info *info);

/**
 * @brief Register controller as an I3C target device.
 *
 * Configures the controller to operate in target mode and participate in
 * Dynamic Address Assignment.
 *
 * @param[in] handle Pointer to the I3C controller handle.
 * @param[in] wait_for_daa true to block until DAA completes.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_register_target(struct capi_i3c_controller_handle *handle, bool wait_for_daa);

/**
 * @brief Enable IBI from a target device or enable IBI generation.
 *
 * In initiator mode, configures the controller to accept IBIs from targets
 * using the specified response policy.
 *
 * In target mode, enables this device to generate IBIs on the bus.
 *
 * @param[in] handle    Pointer to the I3C controller handle.
 * @param[in] response  IBI response policy (initiator mode only; ignored in target mode).
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_ibi_enable(struct capi_i3c_controller_handle *handle,
			enum capi_i3c_ibi_response response);

/**
 * @brief Disable IBI.
 *
 * In initiator mode, configures the controller to NACK all IBIs.
 *
 * In target mode, disables IBI generation from this device.
 *
 * @param[in] handle  Pointer to the I3C controller handle.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_ibi_disable(struct capi_i3c_controller_handle *handle);

/**
 * @brief Raise an IBI from target mode with a mandatory data byte.
 *
 * Requests the controller (in target mode) to issue an IBI on the bus.
 * The mandatory data byte is sent as part of the IBI.
 *
 * @param[in] handle    Pointer to the I3C controller handle.
 * @param[in] ibi_data  Mandatory data byte to include in the IBI.
 * @param[in] has_extra true if additional data bytes follow the mandatory byte.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_target_raise_ibi(struct capi_i3c_controller_handle *handle, uint8_t ibi_data,
			      bool has_extra);

/**
 * @brief Get IBI payload information after an IBI event (initiator mode).
 *
 * After receiving a CAPI_I3C_IBI_REQUEST event, the initiator can retrieve
 * the IBI source address and type.
 *
 * @param[in]  handle   Pointer to the I3C controller handle.
 * @param[out] addr     Pointer to store the 7-bit address of the IBI source.
 * @param[out] ibi_data Pointer to store the mandatory data byte (if any).
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_ibi_get_payload(struct capi_i3c_controller_handle *handle, uint8_t *addr,
			     uint8_t *ibi_data);

/**
 * @brief Send a CCC (Common Command Code) on the I3C bus.
 *
 * Provides a portable abstraction over raw CCC byte transfers. The CCC
 * command ID is an explicit parameter rather than being embedded in the
 * data buffer.
 *
 * For broadcast CCCs, set @p addr to CAPI_I3C_BROADCAST_ADDR.
 * For direct write CCCs, set @p addr to the target device address.
 *
 * @param[in] handle  Pointer to the I3C controller handle.
 * @param[in] ccc_id  CCC command code as defined by the I3C specification.
 * @param[in] addr    Target address for direct CCC, or CAPI_I3C_BROADCAST_ADDR
 *                     for broadcast CCC.
 * @param[in] buf     Optional data buffer (defining byte, sub-command, payload).
 *                     May be NULL when @p len is 0.
 * @param[in] len     Length of data buffer in bytes.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_send_ccc(struct capi_i3c_controller_handle *handle, uint8_t ccc_id, uint16_t addr,
		      const uint8_t *buf, uint32_t len);

/**
 * @brief Receive data via a direct read CCC on the I3C bus.
 *
 * Issues a direct read CCC to the target at the given address and
 * reads the response into the provided buffer.
 *
 * @param[in]  handle  Pointer to the I3C controller handle.
 * @param[in]  ccc_id  CCC command code as defined by the I3C specification.
 * @param[in]  addr    Target device address (7-bit).
 * @param[out] buf     Buffer to receive CCC response data.
 * @param[in]  len     Length of receive buffer in bytes.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_recv_ccc(struct capi_i3c_controller_handle *handle, uint8_t ccc_id, uint16_t addr,
		      uint8_t *buf, uint32_t len);

/**
 * @brief Enable Hot-Join acceptance (initiator) or request Hot-Join (target).
 *
 * In initiator mode, configures the controller to ACK Hot-Join requests
 * from new devices joining the bus. When a Hot-Join is detected, the
 * callback is invoked with CAPI_I3C_HOT_JOIN; the application should
 * then call capi_i3c_do_daa() to assign an address.
 *
 * In target mode, triggers a Hot-Join request on the bus so that the
 * initiator performs DAA for this device.
 *
 * @param[in] handle  Pointer to the I3C controller handle.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_hot_join_enable(struct capi_i3c_controller_handle *handle);

/**
 * @brief Disable Hot-Join acceptance (initiator) or cancel Hot-Join (target).
 *
 * In initiator mode, configures the controller to NACK Hot-Join requests.
 *
 * In target mode, clears the Hot-Join event request.
 *
 * @param[in] handle  Pointer to the I3C controller handle.
 *
 * @return 0 for success or negative error code.
 */
int capi_i3c_hot_join_disable(struct capi_i3c_controller_handle *handle);

/**
 * @brief Execute an atomic batch of transfers (scatter-gather).
 *
 * Processes multiple bus segments without releasing the bus between
 * segments that have @c stop = false. This guarantees no other
 * initiator or transfer can interleave.
 *
 * Typical use case: register read (write address + repeated-START read).
 *
 * @param[in] device  Pointer to the I3C target device descriptor.
 * @param[in] msgs    Array of message descriptors.
 * @param[in] count   Number of messages in the array.
 *
 * @return 0 for success or negative error code. On error, a STOP is
 *         emitted to release the bus.
 */
int capi_i3c_transfer_batch(struct capi_i3c_device *device, struct capi_i3c_msg *msgs,
			    uint32_t count);

/**
 * @brief I3C interrupt service routine.
 *
 * @param[in] handle Pointer to the I3C controller handle (void* for ISR vector compatibility).
 */
void capi_i3c_isr(void *handle);

/**
 * @brief Structure holding I3C function pointers for platform-specific implementation.
 */
struct capi_i3c_ops {
	/** See capi_i3c_init() */
	int (*init)(struct capi_i3c_controller_handle **handle,
		    const struct capi_i3c_config *config);
	/** See capi_i3c_deinit() */
	int (*deinit)(struct capi_i3c_controller_handle *handle);
	/** See capi_i3c_transmit() */
	int (*transmit)(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer);
	/** See capi_i3c_receive() */
	int (*receive)(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer);
	/** See capi_i3c_transmit_async() */
	int (*transmit_async)(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer);
	/** See capi_i3c_receive_async() */
	int (*receive_async)(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer);
	/** See capi_i3c_register_callback() */
	int (*register_callback)(struct capi_i3c_controller_handle *handle,
				 capi_i3c_callback callback, void *callback_arg);
	/** See capi_i3c_do_daa() */
	int (*do_daa)(struct capi_i3c_controller_handle *handle);
	/** See capi_i3c_do_daa_async() */
	int (*do_daa_async)(struct capi_i3c_controller_handle *handle);
	/** See capi_i3c_add_static_device() */
	int (*add_static_device)(struct capi_i3c_controller_handle *handle, uint8_t addr,
				 uint8_t *dev_id);
	/** See capi_i3c_get_num_devices() */
	int (*get_num_devices)(struct capi_i3c_controller_handle *handle, uint8_t *num_devices);
	/** See capi_i3c_get_device_info() */
	int (*get_device_info)(struct capi_i3c_controller_handle *handle, uint8_t dev_id,
			       struct capi_i3c_device_info *info);
	/** See capi_i3c_register_target() */
	int (*register_target)(struct capi_i3c_controller_handle *handle, bool wait_for_daa);
	/** See capi_i3c_ibi_enable() */
	int (*ibi_enable)(struct capi_i3c_controller_handle *handle,
			  enum capi_i3c_ibi_response response);
	/** See capi_i3c_ibi_disable() */
	int (*ibi_disable)(struct capi_i3c_controller_handle *handle);
	/** See capi_i3c_target_raise_ibi() */
	int (*target_raise_ibi)(struct capi_i3c_controller_handle *handle, uint8_t ibi_data,
				bool has_extra);
	/** See capi_i3c_ibi_get_payload() */
	int (*ibi_get_payload)(struct capi_i3c_controller_handle *handle, uint8_t *addr,
			       uint8_t *ibi_data);
	/** See capi_i3c_send_ccc() */
	int (*send_ccc)(struct capi_i3c_controller_handle *handle, uint8_t ccc_id, uint16_t addr,
			const uint8_t *buf, uint32_t len);
	/** See capi_i3c_recv_ccc() */
	int (*recv_ccc)(struct capi_i3c_controller_handle *handle, uint8_t ccc_id, uint16_t addr,
			uint8_t *buf, uint32_t len);
	/** See capi_i3c_hot_join_enable() */
	int (*hot_join_enable)(struct capi_i3c_controller_handle *handle);
	/** See capi_i3c_hot_join_disable() */
	int (*hot_join_disable)(struct capi_i3c_controller_handle *handle);
	/** See capi_i3c_transfer_batch() */
	int (*transfer_batch)(struct capi_i3c_device *device, struct capi_i3c_msg *msgs,
			      uint32_t count);
	/** See capi_i3c_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_I3C_H_ */
