/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL Ethernet PHY API
 */

#ifndef _CAPI_ETH_PHY_H_
#define _CAPI_ETH_PHY_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Ethernet link state
 */
enum capi_eth_link_state {
	CAPI_ETH_LINK_DOWN, /**< Link is down */
	CAPI_ETH_LINK_UP    /**< Link is up */
};

/**
 * @brief Ethernet PHY link speed
 */
enum capi_eth_phy_speed {
	CAPI_ETH_PHY_SPEED_UNKNOWN, /**< Speed not yet determined (link down or autoneg in progress) */
	CAPI_ETH_PHY_SPEED_10M,    	/**< 10 Mbps */
	CAPI_ETH_PHY_SPEED_100M,   	/**< 100 Mbps */
	CAPI_ETH_PHY_SPEED_1G,     	/**< 1 Gbps */
};

/**
 * @brief Ethernet PHY duplex mode
 */
enum capi_eth_phy_duplex {
	CAPI_ETH_PHY_DUPLEX_UNKNOWN, /**< Duplex not yet determined (link down or autoneg in progress) */
	CAPI_ETH_PHY_DUPLEX_HALF,    /**< Half duplex */
	CAPI_ETH_PHY_DUPLEX_FULL,    /**< Full duplex */
};

/**
 * @brief Ethernet MDI/MDIX crossover mode
 */
enum capi_eth_mdix {
	CAPI_ETH_MDIX_AUTO, /**< Auto-MDIX: PHY detects and corrects cable crossover automatically */
	CAPI_ETH_MDIX_MDI,  /**< Force MDI (straight-through cable) */
	CAPI_ETH_MDIX_MDIX, /**< Force MDIX (crossover cable) */
};

/**
 * @brief Ethernet link information
 */
struct capi_eth_link_info {
	enum capi_eth_link_state state;  /**< Link state (up/down) */
	enum capi_eth_phy_speed speed;   /**< Negotiated link speed */
	enum capi_eth_phy_duplex duplex; /**< Negotiated duplex mode */
	enum capi_eth_mdix
	mdix;         /**< Resolved MDI/MDIX state (AUTO if not yet determined) */
};

/**
 * @brief Ethernet PHY media interface type
 *
 * Passed to capi_eth_phy_set_interface() to specify the MAC-PHY interconnect.
 */
enum capi_eth_interface {
	CAPI_ETH_INTERFACE_MII,   /**< Media Independent Interface (MII) */
	CAPI_ETH_INTERFACE_RMII,  /**< Reduced Media Independent Interface (RMII) */
	CAPI_ETH_INTERFACE_SMII,  /**< Serial Media Independent Interface (SMII) */
	CAPI_ETH_INTERFACE_RGMII, /**< Reduced Gigabit Media Independent Interface (RGMII) */
	CAPI_ETH_INTERFACE_SGMII, /**< Serial Gigabit Media Independent Interface (SGMII) */
};

/**
 * @brief Ethernet PHY operation mode configuration for capi_eth_phy_set_mode()
 */
struct capi_eth_phy_mode_config {
	enum capi_eth_phy_speed  speed;   /**< Desired link speed */
	enum capi_eth_phy_duplex duplex;  /**< Desired duplex mode */
	bool auto_negotiate;              /**< true = auto-negotiate transmission parameters */
	bool loopback;                    /**< true = loopback test mode */
	bool isolate;                     /**< true = isolate PHY from MAC interface */
	enum capi_eth_mdix
	mdix;          /**< MDI/MDIX crossover mode (AUTO, MDI, or MDIX) */
};

/**
 * @brief Ethernet PHY events
 */
enum capi_eth_phy_event {
	CAPI_ETH_PHY_EVENT_LINK_UP   = (1U << 0), /**< Link has come up */
	CAPI_ETH_PHY_EVENT_LINK_DOWN = (1U << 1), /**< Link has gone down */
};

/**
 * @brief Ethernet PHY power state
 */
enum capi_eth_phy_power_state {
	CAPI_ETH_PHY_POWER_OFF,  /**< Power off */
	CAPI_ETH_PHY_POWER_LOW,  /**< Low power mode */
	CAPI_ETH_PHY_POWER_FULL, /**< Full power mode */
};

/**
 * @brief Ethernet PHY Event Callback type
 * @param[in] event Ethernet PHY event flags (see capi_eth_phy_event)
 * @param[in] arg   Pointer to user specific data
 */
typedef void (*capi_eth_phy_event_callback)(uint32_t event, void *arg);

/**
 * @brief Function pointer type for reading a PHY register.
 *
 * Provided by the MAC driver and passed to capi_eth_phy_init() so that the
 * PHY driver can access PHY registers without holding a reference to the MAC
 * handle.
 *
 * @param[in]  phy_addr PHY device address (meaning is transport-specific;
 *                      for MDIO this is the 5-bit bus address, 0-31).
 * @param[in]  reg_addr Register address (meaning is transport-specific;
 *                      for MDIO this is the 5-bit register number, 0-31).
 * @param[out] data     Returns the 16-bit register value.
 *
 * @return int 0 for success or error code.
 */
typedef int (*capi_eth_phy_read_fn)(uint8_t phy_addr, uint8_t reg_addr,
				    uint16_t *data);

/**
 * @brief Function pointer type for writing a PHY register.
 *
 * See capi_eth_phy_read_fn for details on transport and NULL semantics.
 *
 * @param[in] phy_addr PHY device address (meaning is transport-specific;
 *                     for MDIO this is the 5-bit bus address, 0-31).
 * @param[in] reg_addr Register address (meaning is transport-specific;
 *                     for MDIO this is the 5-bit register number, 0-31).
 * @param[in] data     16-bit data to write.
 *
 * @return int 0 for success or error code.
 */
typedef int (*capi_eth_phy_write_fn)(uint8_t phy_addr, uint8_t reg_addr,
				     uint16_t data);

/**
 * @brief Ethernet PHY initialization configuration
 */
struct capi_eth_phy_init_config {
	/** PHY device address on MDIO bus (0-31) */
	uint8_t phy_addr;
	/** Register read function provided by the MAC driver (see capi_eth_phy_read_fn) */
	capi_eth_phy_read_fn fn_read;
	/** Register write function provided by the MAC driver (see capi_eth_phy_write_fn) */
	capi_eth_phy_write_fn fn_write;
	/** Optional platform specific extra configuration */
	void *extra;
	/** Platform specific implementation of the API (Mandatory field) */
	const struct capi_eth_phy_ops *ops;
};

/**
 * @brief Ethernet PHY handle
 *
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_eth_phy_handle {
	const struct capi_eth_phy_ops *ops; /**< Set and used by capi thin layer */
	bool init_allocated;                /**< If true, the driver is owner of handle memory */
	void *lock;                         /**< Set and used by capi thin layer if locking is available */
	void *priv;                         /**< Set and used by user optionally */
};

/**
 * @brief Initialize an instance of the Ethernet PHY driver.
 *
 * @param[in,out] handle Points to a pointer of the context structure. If this
 *                       pointer is set to NULL, then the driver will allocate
 *                       the context structure and be the owner.
 * @param[in] config     Points to the initialization configuration for the PHY.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_phy_init(struct capi_eth_phy_handle **handle,
		      const struct capi_eth_phy_init_config *config);

/**
 * @brief Deinitialize the Ethernet PHY driver.
 *
 * @param[in] handle Points to the Ethernet PHY context.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_phy_deinit(struct capi_eth_phy_handle *handle);

/**
 * @brief Control Ethernet PHY Device Power.
 *
 * @param[in] handle Points to the Ethernet PHY context.
 * @param[in] state  Power state to set.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_phy_power_control(struct capi_eth_phy_handle *handle,
			       enum capi_eth_phy_power_state state);

/**
 * @brief Set Ethernet Media Interface between MAC and PHY.
 *
 * Should be called after init using the interface type reported by
 * capi_eth_mac_get_capabilities().media_interface.
 *
 * @param[in] handle    Points to the Ethernet PHY context.
 * @param[in] interface Media interface type (see capi_eth_interface).
 *
 * @return int 0 for success or error code.
 */
int capi_eth_phy_set_interface(struct capi_eth_phy_handle *handle,
			       enum capi_eth_interface interface);

/**
 * @brief Set Ethernet PHY operation mode.
 *
 * When config->auto_negotiate is true, config->speed and config->duplex
 * are used as the advertised capability; the PHY will still auto-negotiate
 * and may settle on a lower setting.
 *
 * @param[in] handle Points to the Ethernet PHY context.
 * @param[in] config Desired PHY mode (see capi_eth_phy_mode_config).
 *
 * @return int 0 for success or error code.
 */
int capi_eth_phy_set_mode(struct capi_eth_phy_handle *handle,
			  const struct capi_eth_phy_mode_config *config);

/**
 * @brief Get Ethernet PHY link state.
 *
 * @param[in]  handle Points to the Ethernet PHY context.
 * @param[out] state  Returns the current link state.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_phy_get_link_state(struct capi_eth_phy_handle *handle,
				enum capi_eth_link_state *state);

/**
 * @brief Get Ethernet PHY link information (state, speed, duplex).
 *
 * @param[in]  handle Points to the Ethernet PHY context.
 * @param[out] info   Returns the current link information.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_phy_get_link_info(struct capi_eth_phy_handle *handle,
			       struct capi_eth_link_info *info);

/**
 * @brief Register callback for Ethernet PHY events.
 *
 * @param[in] handle        Points to the Ethernet PHY context.
 * @param[in] callback      User callback function.
 * @param[in] callback_arg  User callback argument.
 *
 * @return int 0 for success or error code.
 */
int capi_eth_phy_register_callback(struct capi_eth_phy_handle *handle,
				   capi_eth_phy_event_callback const callback,
				   void *const callback_arg);

/**
 * @brief Ethernet PHY Driver Interrupt handler.
 *
 * If interrupt vectors are managed by the user, call this function from the
 * relevant interrupt vector.
 *
 * @param[in] handle Points to the Ethernet PHY context.
 */
void capi_eth_phy_isr(void *handle);

/**
 * @brief Structure holding Ethernet PHY function pointers.
 * See API functions for relevant descriptions.
 */
struct capi_eth_phy_ops {
	/** See capi_eth_phy_init() */
	int (*init)(struct capi_eth_phy_handle **handle,
		    const struct capi_eth_phy_init_config *config);
	/** See capi_eth_phy_deinit() */
	int (*deinit)(struct capi_eth_phy_handle *handle);
	/** See capi_eth_phy_power_control() */
	int (*power_control)(struct capi_eth_phy_handle *handle,
			     enum capi_eth_phy_power_state state);
	/** See capi_eth_phy_set_interface() */
	int (*set_interface)(struct capi_eth_phy_handle *handle,
			     enum capi_eth_interface interface);
	/** See capi_eth_phy_set_mode() */
	int (*set_mode)(struct capi_eth_phy_handle *handle,
			const struct capi_eth_phy_mode_config *config);
	/** See capi_eth_phy_get_link_state() */
	int (*get_link_state)(struct capi_eth_phy_handle *handle,
			      enum capi_eth_link_state *state);
	/** See capi_eth_phy_get_link_info() */
	int (*get_link_info)(struct capi_eth_phy_handle *handle,
			     struct capi_eth_link_info *info);
	/** See capi_eth_phy_register_callback() */
	int (*register_callback)(struct capi_eth_phy_handle *handle,
				 capi_eth_phy_event_callback const callback,
				 void *const callback_arg);
	/** See capi_eth_phy_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CAPI_ETH_PHY_H_ */