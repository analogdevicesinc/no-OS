/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL Ethernet MAC API
 */

#ifndef _CAPI_ETH_MAC_H_
#define _CAPI_ETH_MAC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Ethernet MAC address type (6 bytes)
 */
typedef uint8_t capi_eth_mac_addr[6];

/**
 * @brief Ethernet MAC PTP time structure
 */
struct capi_eth_mac_time {
	uint32_t ns;  /**< Nanoseconds */
	uint32_t sec; /**< Seconds */
};

/**
 * @brief Ethernet MAC capabilities
 */
struct capi_eth_mac_capabilities {
	/** IPv4 header checksum verified on receive */
	uint32_t checksum_offload_rx_ip4 : 1;
	/** IPv6 checksum verification supported on receive */
	uint32_t checksum_offload_rx_ip6 : 1;
	/** UDP payload checksum verified on receive */
	uint32_t checksum_offload_rx_udp : 1;
	/** TCP payload checksum verified on receive */
	uint32_t checksum_offload_rx_tcp : 1;
	/** ICMP payload checksum verified on receive */
	uint32_t checksum_offload_rx_icmp : 1;
	/** IPv4 header checksum generated on transmit */
	uint32_t checksum_offload_tx_ip4 : 1;
	/** IPv6 checksum generation supported on transmit */
	uint32_t checksum_offload_tx_ip6 : 1;
	/** UDP payload checksum generated on transmit */
	uint32_t checksum_offload_tx_udp : 1;
	/** TCP payload checksum generated on transmit */
	uint32_t checksum_offload_tx_tcp : 1;
	/** ICMP payload checksum generated on transmit */
	uint32_t checksum_offload_tx_icmp : 1;
	/** Ethernet Media Interface type (see capi_eth_interface) */
	uint32_t media_interface : 3;
	/** Driver provides initial valid MAC address */
	uint32_t mac_address : 1;
	/** Callback event RX_FRAME generated */
	uint32_t event_rx_frame : 1;
	/** Callback event TX_FRAME generated */
	uint32_t event_tx_frame : 1;
	/** Wakeup event generated */
	uint32_t event_wakeup : 1;
	/**
	 * Hardware provides PTP primitives: free-running timer, RX/TX frame
	 * timestamping, alarm, and frequency adjustment. The IEEE 1588 protocol
	 * state machine is the responsibility of the application.
	 */
	uint32_t ptp : 1;
	/**
	 * Number of hardware TX priority queues (0 = single queue, no priority
	 * support; 1-7 = number of queues available).
	 */
	uint32_t tx_queues : 3;
	/** Reserved (must be zero) */
	uint32_t reserved : 11;
};

/**
 * @brief Ethernet MAC events
 */
enum capi_eth_mac_event {
	CAPI_ETH_MAC_EVENT_RX_FRAME    = (1U << 0), /**< Frame received, can be read */
	CAPI_ETH_MAC_EVENT_TX_FRAME    = (1U << 1), /**< Frame transmission complete */
	CAPI_ETH_MAC_EVENT_WAKEUP      = (1U << 2), /**< Magic Packet received (in Sleep mode) */
	CAPI_ETH_MAC_EVENT_PTP_ALARM   = (1U << 3), /**< PTP alarm occurred */
};

/**
 * @brief Ethernet MAC link speed
 */
enum capi_eth_mac_speed {
	CAPI_ETH_MAC_SPEED_UNKNOWN, /**< Speed not yet determined */
	CAPI_ETH_MAC_SPEED_10M,    /**< 10 Mbps */
	CAPI_ETH_MAC_SPEED_100M,   /**< 100 Mbps */
	CAPI_ETH_MAC_SPEED_1G,     /**< 1 Gbps */
};

/**
 * @brief Ethernet MAC duplex mode
 */
enum capi_eth_mac_duplex {
	CAPI_ETH_MAC_DUPLEX_UNKNOWN, /**< Duplex not yet determined */
	CAPI_ETH_MAC_DUPLEX_HALF,    /**< Half duplex */
	CAPI_ETH_MAC_DUPLEX_FULL,    /**< Full duplex */
};

/**
 * @brief Ethernet MAC runtime configuration structure for capi_eth_mac_configure()
 *
 * Using a struct (rather than a flat bitmask) allows new options to be added
 * without breaking existing callers.
 */
struct capi_eth_mac_config {
	/** Link speed */
	enum capi_eth_mac_speed speed;
	/** Duplex mode */
	enum capi_eth_mac_duplex duplex;
	/** true = enable loopback test mode */
	bool loopback;
	/** true = enable receiver checksum offload */
	bool checksum_offload_rx;
	/** true = enable transmitter checksum offload */
	bool checksum_offload_tx;
	/** true = accept frames with Broadcast destination address */
	bool address_broadcast;
	/** true = accept frames with any Multicast destination address */
	bool address_multicast;
	/** true = accept all frames regardless of destination address (Promiscuous Mode) */
	bool address_all;
};

/**
 * @brief Ethernet VLAN filter configuration for capi_eth_mac_set_vlan_filter()
 */
struct capi_eth_vlan_filter {
	uint16_t tag;  /**< VLAN tag value (bits 0-11: VID, bits 12-15: PCP+DEI) */
	bool vid_only; /**< true = match 12-bit VID only, false = match full 16-bit tag */
	bool enabled;  /**< true = enable VLAN filter, false = disable */
};

/**
 * @brief Ethernet MAC frame transmit configuration
 */
struct capi_eth_mac_tx_frame_config {
	bool fragment;     /**< true = frame fragment, more fragments to follow */
	bool event;        /**< true = generate TX_FRAME event on completion */
	bool timestamp;    /**< true = capture transmission timestamp */
	uint8_t priority;  /**< TX queue priority (0 = default/best-effort, higher = higher priority).
	                        Valid range is 0 to (tx_queues - 1) from capabilities.
	                        Ignored if tx_queues capability is 0. */
};

/**
 * @brief Ethernet MAC power state
 */
enum capi_eth_mac_power_state {
	CAPI_ETH_MAC_POWER_OFF,  /**< Power off */
	CAPI_ETH_MAC_POWER_LOW,  /**< Low power mode */
	CAPI_ETH_MAC_POWER_FULL, /**< Full power mode */
};

/**
 * @brief Ethernet MAC initialization configuration
 */
struct capi_eth_mac_init_config {
	/** Ethernet MAC controller identifier (base address) */
	uintptr_t identifier;
	/** MAC address (6 bytes). If NULL, driver default is used */
	capi_eth_mac_addr *mac_address;
	/** Optional platform specific extra configuration */
	void *extra;
	/** Platform specific implementation of the API (Mandatory field) */
	const struct capi_eth_mac_ops *ops;
};

/**
 * @brief Ethernet MAC handle
 *
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_eth_mac_handle {
	const struct capi_eth_mac_ops *ops; /**< Set and used by capi thin layer */
	bool init_allocated;                /**< If true, the driver is owner of handle memory */
	void *lock;                         /**< Set and used by capi thin layer if locking is available */
	void *priv;                         /**< Set and used by user optionally */
};

/**
 * @brief Ethernet MAC statistics
 *
 * Counters are cumulative since the last reset or driver initialization.
 * Unsupported counters shall be set to zero by the driver.
 */
struct capi_eth_mac_stats {
	uint64_t rx_frames;          /**< Total frames received (good and bad) */
	uint64_t rx_octets;          /**< Total octets received (good frames only) */
	uint64_t rx_unicast;         /**< Unicast frames received */
	uint64_t rx_multicast;       /**< Multicast frames received */
	uint64_t rx_broadcast;       /**< Broadcast frames received */
	uint64_t rx_crc_errors;      /**< Frames discarded due to CRC error */
	uint64_t rx_align_errors;    /**< Frames discarded due to alignment error */
	uint64_t rx_overrun;         /**< Frames lost due to receive buffer overflow */
	uint64_t tx_frames;          /**< Total frames transmitted */
	uint64_t tx_octets;          /**< Total octets transmitted */
	uint64_t tx_unicast;         /**< Unicast frames transmitted */
	uint64_t tx_multicast;       /**< Multicast frames transmitted */
	uint64_t tx_broadcast;       /**< Broadcast frames transmitted */
	uint64_t tx_errors;          /**< Frames not transmitted due to error */
	uint64_t tx_collisions;      /**< Total collisions (half-duplex only) */
};

/**
 * @brief Ethernet MAC Event Callback type
 * @param[in] event Ethernet MAC event flags (see capi_eth_mac_event)
 * @param[in] arg Pointer to user specific data
 */
typedef void (*capi_eth_mac_event_callback)(uint32_t event, void *arg);

/**
 * @brief Get Ethernet MAC statistics.
 *
 * Retrieves cumulative statistics counters. Counters are not reset by this
 * call. To reset counters, reinitialize the driver.
 *
 * @param[in]  handle Points to the Ethernet MAC context.
 * @param[out] stats  Returns the current statistics counters.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_get_stats(struct capi_eth_mac_handle *handle,
			   struct capi_eth_mac_stats *stats);

/**
 * @brief Initialize an instance of the Ethernet MAC controller.
 *
 * @param[in,out] handle Points to a pointer of the context structure. If this
 *                       pointer is set to NULL, then the controller will allocate
 *                       the context structure and be the owner. If the pointer
 *                       is not NULL, the application has allocated the structure and is
 *                       the owner.
 * @param[in] config Points to the initialization configuration for the Ethernet MAC.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_init(struct capi_eth_mac_handle **handle,
		      const struct capi_eth_mac_init_config *config);

/**
 * @brief Deinitialize the Ethernet MAC controller.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_deinit(struct capi_eth_mac_handle *handle);

/**
 * @brief Control Ethernet MAC Device Power.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] state Power state to set.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_power_control(struct capi_eth_mac_handle *handle,
			       enum capi_eth_mac_power_state state);

/**
 * @brief Get Ethernet MAC capabilities.
 *
 * @param[in]  handle       Points to the Ethernet MAC context.
 * @param[out] capabilities Returns the MAC capabilities.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_get_capabilities(struct capi_eth_mac_handle *handle,
				  struct capi_eth_mac_capabilities *capabilities);

/**
 * @brief Get Ethernet MAC Address.
 *
 * @param[in]  handle Points to the Ethernet MAC context.
 * @param[out] addr   Returns the MAC address (6 bytes).
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_get_address(struct capi_eth_mac_handle *handle,
			     capi_eth_mac_addr addr);

/**
 * @brief Set Ethernet MAC Address.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] addr   MAC address to set (6 bytes).
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_set_address(struct capi_eth_mac_handle *handle,
			     const capi_eth_mac_addr addr);

/**
 * @brief Configure Address Filter (multicast/additional addresses).
 *
 * @param[in] handle    Points to the Ethernet MAC context.
 * @param[in] addr_list Pointer to array of MAC addresses to accept.
 * @param[in] num_addr  Number of addresses in the list (0 to disable filtering).
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_set_address_filter(struct capi_eth_mac_handle *handle,
				    const capi_eth_mac_addr *addr_list, uint32_t num_addr);

/**
 * @brief Send Ethernet frame.
 *
 * The frame buffer must contain DA + SA + EtherType/Length + payload.
 * The hardware generates and appends the FCS (CRC-32); it must not be
 * included in @p frame or counted in @p len.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] frame  Pointer to frame buffer (starts with MAC destination address).
 * @param[in] len    Frame length in bytes, excluding FCS.
 * @param[in] config Transmit configuration (see capi_eth_mac_tx_frame_config).
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_send_frame(struct capi_eth_mac_handle *handle,
			    const uint8_t *frame, uint32_t len,
			    const struct capi_eth_mac_tx_frame_config *config);

/**
 * @brief Get size of the next received Ethernet frame.
 *
 * Call this before capi_eth_mac_read_frame() to determine the required buffer
 * size. Returns 0 in @p size when no frame is available.
 *
 * @param[in]  handle Points to the Ethernet MAC context.
 * @param[out] size   Size of the pending frame in bytes (0 if none available).
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_get_rx_frame_size(struct capi_eth_mac_handle *handle,
				   uint32_t *size);

/**
 * @brief Read the next received Ethernet frame.
 *
 * Calling with frame=NULL and len=0 discards the frame without copying data.
 * If @p timestamp is non-NULL and the hardware supports PTP timestamping, the
 * receive timestamp is written atomically with the frame data. Pass NULL if
 * the timestamp is not needed.
 *
 * @param[in]  handle    Points to the Ethernet MAC context.
 * @param[out] frame     Pointer to buffer for received frame data.
 * @param[in]  len       Maximum frame length to read (buffer size).
 * @param[out] timestamp Returns the receive timestamp, or NULL to ignore.
 *
 * @return int Number of bytes read (>=0) or error code (<0).
 */
int capi_eth_mac_read_frame(struct capi_eth_mac_handle *handle, uint8_t *frame,
			    uint32_t len,
			    struct capi_eth_mac_time *timestamp);

/**
 * @brief Get timestamp of the most recently transmitted Ethernet frame.
 *
 * Valid only after a send with capi_eth_mac_tx_frame_config.timestamp set to
 * true and the corresponding CAPI_ETH_MAC_EVENT_TX_FRAME event.
 *
 * @param[in]  handle Points to the Ethernet MAC context.
 * @param[out] time   Returns the transmit timestamp.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_get_tx_frame_time(struct capi_eth_mac_handle *handle,
				   struct capi_eth_mac_time *time);

/**
 * @brief Configure Ethernet MAC runtime parameters.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] config Runtime configuration (speed, duplex, offload, address modes).
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_configure(struct capi_eth_mac_handle *handle,
			   const struct capi_eth_mac_config *config);

/**
 * @brief Enable or disable Ethernet MAC transmitter.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] enable true = enable transmitter, false = disable.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_set_tx_enabled(struct capi_eth_mac_handle *handle,
				bool enable);

/**
 * @brief Enable or disable Ethernet MAC receiver.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] enable true = enable receiver, false = disable.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_set_rx_enabled(struct capi_eth_mac_handle *handle,
				bool enable);

/**
 * @brief Flush Ethernet MAC receive buffer.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_flush_rx_buffer(struct capi_eth_mac_handle *handle);

/**
 * @brief Flush Ethernet MAC transmit buffer.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_flush_tx_buffer(struct capi_eth_mac_handle *handle);

/**
 * @brief Control Ethernet MAC sleep mode.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] enable true = enter sleep and wait for Magic Packet, false = exit sleep.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_sleep(struct capi_eth_mac_handle *handle, bool enable);

/**
 * @brief Configure Ethernet MAC VLAN filter.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] filter VLAN filter configuration (see capi_eth_vlan_filter).
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_set_vlan_filter(struct capi_eth_mac_handle *handle,
				 const struct capi_eth_vlan_filter *filter);

/**
 * @brief Get current PTP clock time.
 *
 * @param[in]  handle Points to the Ethernet MAC context.
 * @param[out] time   Returns the current PTP clock time.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_ptp_get_time(struct capi_eth_mac_handle *handle,
			      struct capi_eth_mac_time *time);

/**
 * @brief Set PTP clock time to an absolute value.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] time   PTP clock time to set.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_ptp_set_time(struct capi_eth_mac_handle *handle,
			      const struct capi_eth_mac_time *time);

/**
 * @brief Increment PTP clock time by a given offset.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] time   Time offset to add.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_ptp_inc_time(struct capi_eth_mac_handle *handle,
			      const struct capi_eth_mac_time *time);

/**
 * @brief Decrement PTP clock time by a given offset.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] time   Time offset to subtract.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_ptp_dec_time(struct capi_eth_mac_handle *handle,
			      const struct capi_eth_mac_time *time);

/**
 * @brief Set PTP alarm time.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 * @param[in] time   Alarm target time.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_ptp_set_alarm(struct capi_eth_mac_handle *handle,
			       const struct capi_eth_mac_time *time);

/**
 * @brief Adjust PTP clock frequency for drift correction.
 *
 * @param[in] handle          Points to the Ethernet MAC context.
 * @param[in] correction_q31  Frequency correction factor in Q31 fixed-point format.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_ptp_adjust_freq(struct capi_eth_mac_handle *handle,
				 int32_t correction_q31);

/**
 * @brief Register callback for Ethernet MAC events.
 *
 * @param[in] handle        Points to the Ethernet MAC context.
 * @param[in] callback      User callback function.
 * @param[in] callback_arg  User callback argument.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_mac_register_callback(struct capi_eth_mac_handle *handle,
				   capi_eth_mac_event_callback const callback,
				   void *const callback_arg);

/**
 * @brief Ethernet MAC Driver Interrupt handler.
 *
 * If interrupt vectors are managed by the user, call this function from the
 * relevant interrupt vector.
 *
 * @param[in] handle Points to the Ethernet MAC context.
 */
void capi_eth_mac_isr(void *handle);

/**
 * @brief Structure holding Ethernet MAC function pointers.
 * See API functions for relevant descriptions.
 */
struct capi_eth_mac_ops {
	/** See capi_eth_mac_init() */
	int (*init)(struct capi_eth_mac_handle **handle,
		    const struct capi_eth_mac_init_config *config);
	/** See capi_eth_mac_deinit() */
	int (*deinit)(struct capi_eth_mac_handle *handle);
	/** See capi_eth_mac_power_control() */
	int (*power_control)(struct capi_eth_mac_handle *handle,
			     enum capi_eth_mac_power_state state);
	/** See capi_eth_mac_get_capabilities() */
	int (*get_capabilities)(struct capi_eth_mac_handle *handle,
				struct capi_eth_mac_capabilities *capabilities);
	/** See capi_eth_mac_get_address() */
	int (*get_address)(struct capi_eth_mac_handle *handle, capi_eth_mac_addr addr);
	/** See capi_eth_mac_set_address() */
	int (*set_address)(struct capi_eth_mac_handle *handle,
			   const capi_eth_mac_addr addr);
	/** See capi_eth_mac_set_address_filter() */
	int (*set_address_filter)(struct capi_eth_mac_handle *handle,
				  const capi_eth_mac_addr *addr_list, uint32_t num_addr);
	/** See capi_eth_mac_send_frame() */
	int (*send_frame)(struct capi_eth_mac_handle *handle, const uint8_t *frame,
			  uint32_t len,
			  const struct capi_eth_mac_tx_frame_config *config);
	/** See capi_eth_mac_get_rx_frame_size() */
	int (*get_rx_frame_size)(struct capi_eth_mac_handle *handle, uint32_t *size);
	/** See capi_eth_mac_read_frame() */
	int (*read_frame)(struct capi_eth_mac_handle *handle, uint8_t *frame,
			  uint32_t len,
			  struct capi_eth_mac_time *timestamp);
	/** See capi_eth_mac_get_tx_frame_time() */
	int (*get_tx_frame_time)(struct capi_eth_mac_handle *handle,
				 struct capi_eth_mac_time *time);
	/** See capi_eth_mac_configure() */
	int (*configure)(struct capi_eth_mac_handle *handle,
			 const struct capi_eth_mac_config *config);
	/** See capi_eth_mac_set_tx_enabled() */
	int (*set_tx_enabled)(struct capi_eth_mac_handle *handle, bool enable);
	/** See capi_eth_mac_set_rx_enabled() */
	int (*set_rx_enabled)(struct capi_eth_mac_handle *handle, bool enable);
	/** See capi_eth_mac_flush_rx_buffer() */
	int (*flush_rx_buffer)(struct capi_eth_mac_handle *handle);
	/** See capi_eth_mac_flush_tx_buffer() */
	int (*flush_tx_buffer)(struct capi_eth_mac_handle *handle);
	/** See capi_eth_mac_sleep() */
	int (*sleep)(struct capi_eth_mac_handle *handle, bool enable);
	/** See capi_eth_mac_set_vlan_filter() */
	int (*set_vlan_filter)(struct capi_eth_mac_handle *handle,
			       const struct capi_eth_vlan_filter *filter);
	/** See capi_eth_mac_ptp_get_time() */
	int (*ptp_get_time)(struct capi_eth_mac_handle *handle,
			    struct capi_eth_mac_time *time);
	/** See capi_eth_mac_ptp_set_time() */
	int (*ptp_set_time)(struct capi_eth_mac_handle *handle,
			    const struct capi_eth_mac_time *time);
	/** See capi_eth_mac_ptp_inc_time() */
	int (*ptp_inc_time)(struct capi_eth_mac_handle *handle,
			    const struct capi_eth_mac_time *time);
	/** See capi_eth_mac_ptp_dec_time() */
	int (*ptp_dec_time)(struct capi_eth_mac_handle *handle,
			    const struct capi_eth_mac_time *time);
	/** See capi_eth_mac_ptp_set_alarm() */
	int (*ptp_set_alarm)(struct capi_eth_mac_handle *handle,
			     const struct capi_eth_mac_time *time);
	/** See capi_eth_mac_ptp_adjust_freq() */
	int (*ptp_adjust_freq)(struct capi_eth_mac_handle *handle,
			       int32_t correction_q31);
	/** See capi_eth_mac_get_stats() */
	int (*get_stats)(struct capi_eth_mac_handle *handle,
			 struct capi_eth_mac_stats *stats);
	/** See capi_eth_mac_register_callback() */
	int (*register_callback)(struct capi_eth_mac_handle *handle,
				 capi_eth_mac_event_callback const callback,
				 void *const callback_arg);
	/** See capi_eth_mac_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CAPI_ETH_MAC_H_ */