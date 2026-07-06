/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_mdio.h
 * @brief Common HAL MDIO bus API
 *
 * Provides a transport-independent abstraction over the Management Data
 * Input/Output (MDIO) bus (IEEE 802.3 Clause 22 and Clause 45).
 */

#ifndef _CAPI_MDIO_H_
#define _CAPI_MDIO_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* IEEE 802.3 Clause 22 standard register addresses (0–31) */
#define CAPI_MDIO_REG_BMCR      0x00 /**< Basic Mode Control Register */
#define CAPI_MDIO_REG_BMSR      0x01 /**< Basic Mode Status Register */
#define CAPI_MDIO_REG_PHYSID1   0x02 /**< PHY Identifier 1 (OUI bits 3–18) */
#define CAPI_MDIO_REG_PHYSID2   0x03 /**< PHY Identifier 2 (OUI + model + rev) */
#define CAPI_MDIO_REG_ADVERTISE 0x04 /**< Auto-Negotiation Advertisement */
#define CAPI_MDIO_REG_LPA       0x05 /**< Link Partner Ability */
#define CAPI_MDIO_REG_EXPANSION 0x06 /**< Auto-Negotiation Expansion */
#define CAPI_MDIO_REG_NEXTPAGE  0x07 /**< Next Page Transmit */
#define CAPI_MDIO_REG_LPN       0x08 /**< Link Partner Next Page */
#define CAPI_MDIO_REG_CTRL1000  0x09 /**< 1000BASE-T Control */
#define CAPI_MDIO_REG_STAT1000  0x0A /**< 1000BASE-T Status */
#define CAPI_MDIO_REG_MMD_CTRL  0x0D /**< MMD Access Control (Clause 45 via C22) */
#define CAPI_MDIO_REG_MMD_DATA  0x0E /**< MMD Access Register Data */
#define CAPI_MDIO_REG_ESTATUS   0x0F /**< Extended Status */

/* BMCR (register 0) bit definitions */
#define CAPI_MDIO_BMCR_SPEED1000 (1u << 6)  /**< Speed select MSB: 1 = 1000 Mbps */
#define CAPI_MDIO_BMCR_CTST      (1u << 7)  /**< Collision test enable */
#define CAPI_MDIO_BMCR_FULLDPLX  (1u << 8)  /**< Full-duplex mode */
#define CAPI_MDIO_BMCR_ANRESTART (1u << 9)  /**< Restart auto-negotiation */
#define CAPI_MDIO_BMCR_ISOLATE   (1u << 10) /**< Isolate PHY from MII/GMII */
#define CAPI_MDIO_BMCR_PDOWN     (1u << 11) /**< Power down */
#define CAPI_MDIO_BMCR_ANENABLE  (1u << 12) /**< Auto-negotiation enable */
#define CAPI_MDIO_BMCR_SPEED100  (1u << 13) /**< Speed select LSB: 1 = 100 Mbps */
#define CAPI_MDIO_BMCR_LOOPBACK  (1u << 14) /**< Loopback mode */
#define CAPI_MDIO_BMCR_RESET     (1u << 15) /**< Software reset (self-clearing) */

/* BMSR (register 1) bit definitions */
#define CAPI_MDIO_BMSR_ERCAP        (1u << 0)  /**< Extended register capabilities */
#define CAPI_MDIO_BMSR_JCD          (1u << 1)  /**< Jabber condition detected */
#define CAPI_MDIO_BMSR_LSTATUS      (1u << 2)  /**< Link status (latching low) */
#define CAPI_MDIO_BMSR_ANEGCAPABLE  (1u << 3)  /**< Auto-negotiation capable */
#define CAPI_MDIO_BMSR_RFAULT       (1u << 4)  /**< Remote fault */
#define CAPI_MDIO_BMSR_ANEGCOMPLETE (1u << 5)  /**< Auto-negotiation complete */
#define CAPI_MDIO_BMSR_ESTATEN      (1u << 8)  /**< Extended status register present */
#define CAPI_MDIO_BMSR_100HALF2     (1u << 9)  /**< 100BASE-T2 half-duplex capable */
#define CAPI_MDIO_BMSR_100FULL2     (1u << 10) /**< 100BASE-T2 full-duplex capable */
#define CAPI_MDIO_BMSR_10HALF       (1u << 11) /**< 10BASE-T half-duplex capable */
#define CAPI_MDIO_BMSR_10FULL       (1u << 12) /**< 10BASE-T full-duplex capable */
#define CAPI_MDIO_BMSR_100HALF      (1u << 13) /**< 100BASE-TX half-duplex capable */
#define CAPI_MDIO_BMSR_100FULL      (1u << 14) /**< 100BASE-TX full-duplex capable */
#define CAPI_MDIO_BMSR_100BASE4     (1u << 15) /**< 100BASE-T4 capable */

/* ADVERTISE register 4 bit definitions */
#define CAPI_MDIO_ADVERTISE_CSMA       (1u << 0)  /**< Selector: IEEE 802.3 CSMA/CD */
#define CAPI_MDIO_ADVERTISE_10HALF     (1u << 5)  /**< Advertise 10BASE-T half-duplex */
#define CAPI_MDIO_ADVERTISE_10FULL     (1u << 6)  /**< Advertise 10BASE-T full-duplex */
#define CAPI_MDIO_ADVERTISE_100HALF    (1u << 7)  /**< Advertise 100BASE-TX half-duplex */
#define CAPI_MDIO_ADVERTISE_100FULL    (1u << 8)  /**< Advertise 100BASE-TX full-duplex */
#define CAPI_MDIO_ADVERTISE_100BASE4   (1u << 9)  /**< Advertise 100BASE-T4 */
#define CAPI_MDIO_ADVERTISE_PAUSE_CAP  (1u << 10) /**< Advertise symmetric pause */
#define CAPI_MDIO_ADVERTISE_PAUSE_ASYM (1u << 11) /**< Advertise asymmetric pause */
#define CAPI_MDIO_ADVERTISE_RFAULT     (1u << 13) /**< Remote fault */
#define CAPI_MDIO_ADVERTISE_NPAGE      (1u << 15) /**< Next page supported */

/* LPA register 5 bit definitions */
#define CAPI_MDIO_LPA_10HALF     CAPI_MDIO_ADVERTISE_10HALF
#define CAPI_MDIO_LPA_10FULL     CAPI_MDIO_ADVERTISE_10FULL
#define CAPI_MDIO_LPA_100HALF    CAPI_MDIO_ADVERTISE_100HALF
#define CAPI_MDIO_LPA_100FULL    CAPI_MDIO_ADVERTISE_100FULL
#define CAPI_MDIO_LPA_100BASE4   CAPI_MDIO_ADVERTISE_100BASE4
#define CAPI_MDIO_LPA_PAUSE_CAP  CAPI_MDIO_ADVERTISE_PAUSE_CAP
#define CAPI_MDIO_LPA_PAUSE_ASYM CAPI_MDIO_ADVERTISE_PAUSE_ASYM
#define CAPI_MDIO_LPA_RFAULT     CAPI_MDIO_ADVERTISE_RFAULT
#define CAPI_MDIO_LPA_LPACK      (1u << 14) /**< Link partner acknowledged */
#define CAPI_MDIO_LPA_NPAGE      CAPI_MDIO_ADVERTISE_NPAGE

/* CTRL1000 register bit definitions */
#define CAPI_MDIO_ADVERTISE_1000HALF    (1u << 8)  /**< Advertise 1000BASE-T half-duplex */
#define CAPI_MDIO_ADVERTISE_1000FULL    (1u << 9)  /**< Advertise 1000BASE-T full-duplex */
#define CAPI_MDIO_CTRL1000_MASTER_SLAVE (1u << 10) /**< Prefer master role */
#define CAPI_MDIO_CTRL1000_MANUAL_MS    (1u << 11) /**< Manual master/slave select */

/* STAT1000 register bit definitions */
#define CAPI_MDIO_STAT1000_HALF      (1u << 10) /**< Partner 1000BASE-T half-duplex */
#define CAPI_MDIO_STAT1000_FULL      (1u << 11) /**< Partner 1000BASE-T full-duplex */
#define CAPI_MDIO_STAT1000_REMRXOK   (1u << 12) /**< Remote receiver OK */
#define CAPI_MDIO_STAT1000_LOCALRXOK (1u << 13) /**< Local receiver OK */
#define CAPI_MDIO_STAT1000_MASTER    (1u << 14) /**< Local PHY is master */

/* MMD_CTRL (register 13) bit definitions (Clause 45 access via Clause 22) */
#define CAPI_MDIO_MMD_CTRL_DEVAD_MASK 0x1Fu      /**< MMD device address (bits 4:0) */
#define CAPI_MDIO_MMD_CTRL_ADDR       (0u << 14) /**< Function: address */
#define CAPI_MDIO_MMD_CTRL_NOINCR     (1u << 14) /**< Function: data, no post-increment */
#define CAPI_MDIO_MMD_CTRL_INCR_RDWR  (2u << 14) /**< Function: data, post-increment on R/W */
#define CAPI_MDIO_MMD_CTRL_INCR_WR    (3u << 14) /**< Function: data, post-increment on write */

/* ESTATUS (register 15) bit definitions */
#define CAPI_MDIO_ESTATUS_1000XHALF (1u << 12) /**< 1000BASE-X half-duplex capable */
#define CAPI_MDIO_ESTATUS_1000XFULL (1u << 13) /**< 1000BASE-X full-duplex capable */
#define CAPI_MDIO_ESTATUS_1000THALF (1u << 14) /**< 1000BASE-T half-duplex capable */
#define CAPI_MDIO_ESTATUS_1000TFULL (1u << 15) /**< 1000BASE-T full-duplex capable */

/* IEEE 802.3 Clause 45 MMD common registers (present in all standard MMDs 1–7) */
#define CAPI_MDIO_MMD_REG_CTRL1        0x0000 /**< Control 1 */
#define CAPI_MDIO_MMD_REG_STAT1        0x0001 /**< Status 1 */
#define CAPI_MDIO_MMD_REG_DEVID1       0x0002 /**< Device Identifier 1 (OUI bits 3–18) */
#define CAPI_MDIO_MMD_REG_DEVID2       0x0003 /**< Device Identifier 2 (OUI + model + rev) */
#define CAPI_MDIO_MMD_REG_SPEED        0x0004 /**< Speed Ability */
#define CAPI_MDIO_MMD_REG_DEVS_IN_PKG1 0x0005 /**< Devices in Package 1 */
#define CAPI_MDIO_MMD_REG_DEVS_IN_PKG2 0x0006 /**< Devices in Package 2 */
#define CAPI_MDIO_MMD_REG_CTRL2        0x0007 /**< Control 2 */
#define CAPI_MDIO_MMD_REG_STAT2        0x0008 /**< Status 2 */
#define CAPI_MDIO_MMD_REG_PKGID1       0x000E /**< Package Identifier 1 */
#define CAPI_MDIO_MMD_REG_PKGID2       0x000F /**< Package Identifier 2 */

/* IEEE 802.3 Clause 45 MMD device addresses (Table 45-1) */
#define CAPI_MDIO_MMD_PMAPMD 1  /**< Physical Medium Attachment / Physical Medium Dependent */
#define CAPI_MDIO_MMD_WIS    2  /**< WAN Interface Sublayer */
#define CAPI_MDIO_MMD_PCS    3  /**< Physical Coding Sublayer */
#define CAPI_MDIO_MMD_PHYXS  4  /**< PHY eXtra Sublayer */
#define CAPI_MDIO_MMD_DTEXS  5  /**< DTE eXtra Sublayer */
#define CAPI_MDIO_MMD_TC     6  /**< Transmission Convergence */
#define CAPI_MDIO_MMD_AN     7  /**< Auto-Negotiation */
#define CAPI_MDIO_MMD_VEND1  30 /**< Vendor Specific 1 */
#define CAPI_MDIO_MMD_VEND2  31 /**< Vendor Specific 2 */

struct capi_mdio_ops;

/**
 * @brief MDIO bus handle.
 *
 * Driver developers: declare this as the first field of the private MDIO
 * descriptor so the handle pointer can be cast freely.
 */
struct capi_mdio_handle {
	const struct capi_mdio_ops *ops;
	bool init_allocated;
	void *lock;
	void *priv;
};

/**
 * @brief MDIO bus initialization configuration
 */
struct capi_mdio_init_config {
	void *extra;
	const struct capi_mdio_ops *ops;
};

/**
 * @brief Initialize a MDIO bus instance.
 *
 * @param[in,out] handle  Points to a pointer of the handle. If *handle is
 *                        NULL the driver allocates the handle and is its owner.
 * @param[in]     config  Initialization configuration.
 *
 * @return int 0 for success or error code.
 */
int capi_mdio_init(struct capi_mdio_handle **handle,
		   const struct capi_mdio_init_config *config);

/**
 * @brief Deinitialize a MDIO bus instance.
 *
 * @param[in] handle  MDIO bus handle.
 *
 * @return int 0 for success or error code.
 */
int capi_mdio_deinit(struct capi_mdio_handle *handle);

/**
 * @brief Read a PHY register using Clause 22 access.
 *
 * @param[in]  handle   MDIO bus handle.
 * @param[in]  phy_addr PHY address on the bus (0-31).
 * @param[in]  reg      Register address (0-31).
 * @param[out] data     Returns the 16-bit register value.
 *
 * @return int 0 for success or error code.
 */
int capi_mdio_read(struct capi_mdio_handle *handle, uint8_t phy_addr,
		   uint8_t reg, uint16_t *data);

/**
 * @brief Write a PHY register using Clause 22 access.
 *
 * @param[in] handle   MDIO bus handle.
 * @param[in] phy_addr PHY address on the bus (0-31).
 * @param[in] reg      Register address (0-31).
 * @param[in] data     16-bit value to write.
 *
 * @return int 0 for success or error code.
 */
int capi_mdio_write(struct capi_mdio_handle *handle, uint8_t phy_addr,
		    uint8_t reg, uint16_t data);

/**
 * @brief Read a PHY register using Clause 45 access.
 *
 * @param[in]  handle   MDIO bus handle.
 * @param[in]  phy_addr PHY address on the bus (0-31).
 * @param[in]  devnum   MMD device number (0-31).
 * @param[in]  reg      16-bit register address within the MMD.
 * @param[out] data     Returns the 16-bit register value.
 *
 * @return int 0 for success or error code.
 */
int capi_mdio_read_c45(struct capi_mdio_handle *handle, uint8_t phy_addr,
		       uint8_t devnum, uint16_t reg, uint16_t *data);

/**
 * @brief Write a PHY register using Clause 45 access.
 *
 * @param[in] handle   MDIO bus handle.
 * @param[in] phy_addr PHY address on the bus (0-31).
 * @param[in] devnum   MMD device number (0-31).
 * @param[in] reg      16-bit register address within the MMD.
 * @param[in] data     16-bit value to write.
 *
 * @return int 0 for success or error code.
 */
int capi_mdio_write_c45(struct capi_mdio_handle *handle, uint8_t phy_addr,
			uint8_t devnum, uint16_t reg, uint16_t data);

/**
 * @brief Scan the bus for a responding PHY.
 *
 * Tries Clause 22 addresses 1..31 then 0, returns the first address whose
 * PHY ID registers (0x02, 0x03) both respond with a non-zero non-0xFFFF value.
 *
 * @param[in]  handle   MDIO bus handle.
 * @param[out] phy_addr Returns the discovered PHY address.
 *
 * @return int 0 for success, -ENODEV if no PHY found, or error code.
 */
int capi_mdio_scan(struct capi_mdio_handle *handle, uint8_t *phy_addr);

/**
 * @brief MDIO bus driver operations table
 */
struct capi_mdio_ops {
	/** See capi_mdio_init() */
	int (*init)(struct capi_mdio_handle **handle,
		    const struct capi_mdio_init_config *config);
	/** See capi_mdio_deinit() */
	int (*deinit)(struct capi_mdio_handle *handle);
	/** See capi_mdio_read() */
	int (*read_c22)(struct capi_mdio_handle *handle, uint8_t phy_addr,
			uint8_t reg, uint16_t *data);
	/** See capi_mdio_write() */
	int (*write_c22)(struct capi_mdio_handle *handle, uint8_t phy_addr,
			 uint8_t reg, uint16_t data);
	/**
	 * See capi_mdio_read_c45(). Optional: if NULL the thin layer
	 * falls back to Clause 22 indirect access.
	 */
	int (*read_c45)(struct capi_mdio_handle *handle, uint8_t phy_addr,
			uint8_t devnum, uint16_t reg, uint16_t *data);
	/**
	 * See capi_mdio_write_c45(). Optional: if NULL falls back to
	 * Clause 22 indirect access.
	 */
	int (*write_c45)(struct capi_mdio_handle *handle, uint8_t phy_addr,
			 uint8_t devnum, uint16_t reg, uint16_t data);
};

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CAPI_MDIO_H_ */
