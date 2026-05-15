# Network/Ethernet Driver Template

Reference driver: `drivers/net/adin1110/`

This template covers every file needed to add a new network/ethernet driver
to no-OS. Replace `<devname>` with the part number (e.g., `adin1110`) and
`<DEVNAME>` with its uppercase form (e.g., `ADIN1110`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adin1110.h`, `adin1110.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADIN1110_H__` |
| Init param struct | `<devname>_init_param` | `adin1110_init_param` |
| Device struct | `<devname>_desc` | `adin1110_desc` |
| Frame buffer struct | `<devname>_eth_buff` | `adin1110_eth_buff` |
| Functions | `<devname>_<verb>()` | `adin1110_init()` |
| Static helpers | `_<devname>_<verb>()` | `_adin1110_setup_mac()` |
| Device ID enum | `<DEVNAME>` | `ADIN1110` |
| Register addresses | `<DEVNAME>_<REG>_REG` | `ADIN1110_STATUS0_REG` |
| Register masks | `<DEVNAME>_<FIELD>_MSK` | `ADIN1110_LINK_STATE_MASK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADIN1110_MDIO_OP_RD` |

---

## 2. File Checklist

```
drivers/net/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
    README.rst               # Driver documentation

projects/<project_name>/
    Makefile
    builds.json
    src.mk
    src/
        common/
            common_data.c
            common_data.h
        examples/
            basic/
                basic_example.c
                basic_example.h
            examples_src.mk
        platform/
            platform_includes.h
            <platform>/
                main.c
                parameters.h
                parameters.c
                platform_src.mk

doc/sphinx/source/drivers/net/<devname>.rst   # Sphinx entry
```

Note: IIO is not typical for network devices. If IIO support is needed
(e.g., PHY statistics via IIO), add `iio_<devname>.h` and
`iio_<devname>.c` following the IIO patterns from the ADC template.

---

## 3. Driver Header (`<devname>.h`)

```c
/***************************************************************************//**
 *   @file   <devname>.h
 *   @brief  Header file of <DEVNAME> Driver
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef __<DEVNAME>_H__
#define __<DEVNAME>_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/* ---------------- Ethernet Constants ------------------------------- */

#define <DEVNAME>_BUFF_LEN			1530
#define <DEVNAME>_ETH_ALEN			6
#define <DEVNAME>_ETHERTYPE_LEN			2
#define <DEVNAME>_ETH_HDR_LEN			14
#define <DEVNAME>_FCS_LEN			4
#define <DEVNAME>_MAC_LEN			6

/* ---------------- Register Map ------------------------------------- */

#define <DEVNAME>_REG_STATUS			0x00
#define <DEVNAME>_REG_CONFIG			0x01
#define <DEVNAME>_REG_IMASK			0x02
#define <DEVNAME>_REG_TX_FSIZE			0x03
#define <DEVNAME>_REG_TX			0x04
#define <DEVNAME>_REG_TX_SPACE			0x05
#define <DEVNAME>_REG_RX_FSIZE			0x06
#define <DEVNAME>_REG_RX			0x07
#define <DEVNAME>_REG_SOFT_RST			0x08
#define <DEVNAME>_REG_PHY_ID			0x09

/* ---------------- Register Field Masks ----------------------------- */

/** Configuration field masks -- use NO_OS_BIT() for single bits,
 *  NO_OS_GENMASK(hi, lo) for multi-bit fields. */
#define <DEVNAME>_ADDR_MASK			NO_OS_GENMASK(12, 0)
#define <DEVNAME>_RW_MASK			NO_OS_BIT(13)
#define <DEVNAME>_CD_MASK			NO_OS_BIT(15)

#define <DEVNAME>_LINK_STATE_MASK		NO_OS_BIT(0)
#define <DEVNAME>_RESETC_MASK			NO_OS_BIT(6)
#define <DEVNAME>_RX_RDY_MASK			NO_OS_BIT(4)
#define <DEVNAME>_TX_RDY_MASK			NO_OS_BIT(3)
#define <DEVNAME>_CRC_APPEND_MASK		NO_OS_BIT(5)
#define <DEVNAME>_FWD_UNK2HOST_MASK		NO_OS_BIT(2)

/* MDIO access field masks */
#define <DEVNAME>_MDIO_TRDONE			NO_OS_BIT(31)
#define <DEVNAME>_MDIO_ST			NO_OS_GENMASK(29, 28)
#define <DEVNAME>_MDIO_OP			NO_OS_GENMASK(27, 26)
#define <DEVNAME>_MDIO_PRTAD			NO_OS_GENMASK(25, 21)
#define <DEVNAME>_MDIO_DEVAD			NO_OS_GENMASK(20, 16)
#define <DEVNAME>_MDIO_DATA			NO_OS_GENMASK(15, 0)

/* Product / PHY ID expected values */
#define <DEVNAME>_PHY_ID			0xXXXXXXXX

/* MDIO operation codes */
#define <DEVNAME>_MDIO_OP_WR			0x1
#define <DEVNAME>_MDIO_OP_RD			0x3

/* ---------------- Enums -------------------------------------------- */

/** @enum <devname>_chip_id
 *  @brief Supported device IDs. */
enum <devname>_chip_id {
	<DEVNAME>,
	/* <DEVNAME_VARIANT>, */
};

/* Add more enums as needed: link speed, duplex mode, etc. */

/* ---------------- Structures --------------------------------------- */

/**
 * @struct <devname>_eth_buff
 * @brief  Buffer structure used for frame RX and TX transactions.
 */
struct <devname>_eth_buff {
	/** Frame length in bytes. */
	uint32_t len;
	/** Destination MAC address. */
	uint8_t mac_dest[<DEVNAME>_ETH_ALEN];
	/** Source MAC address. */
	uint8_t mac_source[<DEVNAME>_ETH_ALEN];
	/** Ethertype field. */
	uint8_t ethertype[2];
	/** Pointer to payload data. */
	uint8_t *payload;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Device variant. */
	enum <devname>_chip_id chip_type;
	/** SPI initialization parameters. */
	struct no_os_spi_init_param comm_param;
	/** Reset GPIO initialization parameters. */
	struct no_os_gpio_init_param reset_param;
	/** MAC address (6 bytes). */
	uint8_t mac_address[<DEVNAME>_ETH_ALEN];
	/** Enable CRC appending to SPI frames. */
	bool append_crc;
};

/**
 * @struct <devname>_desc
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_desc {
	/** Device variant. */
	enum <devname>_chip_id chip_type;
	/** SPI descriptor. */
	struct no_os_spi_desc *comm_desc;
	/** MAC address. */
	uint8_t mac_address[<DEVNAME>_ETH_ALEN];
	/** Internal data buffer for SPI transactions. */
	uint8_t *data;
	/** Reset GPIO descriptor. */
	struct no_os_gpio_desc *reset_gpio;
	/** CRC append mode flag. */
	bool append_crc;
};

/* ---------------- Function Prototypes ------------------------------ */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_desc **desc,
		   struct <devname>_init_param *param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_desc *desc);

/** @brief Read a register. */
int <devname>_reg_read(struct <devname>_desc *desc, uint16_t addr,
		       uint32_t *val);

/** @brief Write a register. */
int <devname>_reg_write(struct <devname>_desc *desc, uint16_t addr,
			uint32_t val);

/** @brief Update a register field (read-modify-write). */
int <devname>_reg_update(struct <devname>_desc *desc, uint16_t addr,
			 uint32_t mask, uint32_t val);

/** @brief Write a frame to the TX FIFO. */
int <devname>_write_fifo(struct <devname>_desc *desc, uint32_t port,
			 struct <devname>_eth_buff *eth_buff);

/** @brief Read a frame from the RX FIFO. */
int <devname>_read_fifo(struct <devname>_desc *desc, uint32_t port,
			struct <devname>_eth_buff *eth_buff);

/** @brief Write a PHY register via MDIO. */
int <devname>_mdio_write(struct <devname>_desc *desc, uint32_t phy_id,
			 uint32_t reg, uint16_t data);

/** @brief Read a PHY register via MDIO. */
int <devname>_mdio_read(struct <devname>_desc *desc, uint32_t phy_id,
			uint32_t reg, uint16_t *data);

/** @brief Get the ethernet link state. */
int <devname>_link_state(struct <devname>_desc *desc, uint32_t *state);

/** @brief Set a MAC address filter. */
int <devname>_set_mac_addr(struct <devname>_desc *desc,
			   uint8_t mac_address[<DEVNAME>_ETH_ALEN]);

/** @brief Enable/disable promiscuous mode on a port. */
int <devname>_set_promisc(struct <devname>_desc *desc, uint32_t port,
			  bool promisc);

/** @brief Enable/disable broadcast frame forwarding. */
int <devname>_broadcast_filter(struct <devname>_desc *desc, bool enabled);

/** @brief Perform a software reset (MAC and PHY). */
int <devname>_sw_reset(struct <devname>_desc *desc);

#endif /* __<DEVNAME>_H__ */
```

---

## 4. Driver Source (`<devname>.c`)

```c
/***************************************************************************//**
 *   @file   <devname>.c
 *   @brief  Implementation of <DEVNAME> Driver
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * ... (same BSD-3-Clause block as header) ...
 *******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* ---------- Static helpers ----------------------------------------- */

/**
 * @brief Read a register over SPI.
 * @param desc - Device descriptor.
 * @param addr - Register address.
 * @param val  - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_read(struct <devname>_desc *desc, uint16_t addr,
		       uint32_t *val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->data,
		.rx_buff = desc->data,
		.bytes_number = 7,
		.cs_change = 1,
	};
	int ret;

	no_os_put_unaligned_be16(addr | <DEVNAME>_CD_MASK, desc->data);
	desc->data[2] = 0x00;

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be32(&desc->data[3]);

	return 0;
}

/**
 * @brief Write a register over SPI.
 * @param desc - Device descriptor.
 * @param addr - Register address.
 * @param val  - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_desc *desc, uint16_t addr,
			uint32_t val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->data,
		.rx_buff = desc->data,
		.bytes_number = 6,
		.cs_change = 1,
	};

	addr &= <DEVNAME>_ADDR_MASK;
	addr |= <DEVNAME>_CD_MASK | <DEVNAME>_RW_MASK;
	no_os_put_unaligned_be16(addr, desc->data);
	no_os_put_unaligned_be32(val, &desc->data[2]);

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

/**
 * @brief Update a register field (read-modify-write).
 * @param desc - Device descriptor.
 * @param addr - Register address.
 * @param mask - Bitmask of the field to modify.
 * @param val  - New value (only bits within mask are applied).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_update(struct <devname>_desc *desc, uint16_t addr,
			 uint32_t mask, uint32_t val)
{
	uint32_t reg_val;
	int ret;

	ret = <devname>_reg_read(desc, addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= mask & val;

	return <devname>_reg_write(desc, addr, reg_val);
}

/* ---------- MDIO access -------------------------------------------- */

/**
 * @brief Write a PHY register via MDIO (clause 22).
 * @param desc   - Device descriptor.
 * @param phy_id - PHY device address.
 * @param reg    - Register address.
 * @param data   - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_mdio_write(struct <devname>_desc *desc, uint32_t phy_id,
			 uint32_t reg, uint16_t data)
{
	uint32_t mdio_done = 0;
	uint32_t mdio_val;
	uint32_t val;
	int ret;

	val = no_os_field_prep(<DEVNAME>_MDIO_ST, 0x1) |
	      no_os_field_prep(<DEVNAME>_MDIO_OP, <DEVNAME>_MDIO_OP_WR) |
	      no_os_field_prep(<DEVNAME>_MDIO_PRTAD, phy_id) |
	      no_os_field_prep(<DEVNAME>_MDIO_DEVAD, reg) |
	      no_os_field_prep(<DEVNAME>_MDIO_DATA, data);

	ret = <devname>_reg_write(desc, <DEVNAME>_REG_MDIO_ACC, val);
	if (ret)
		return ret;

	while (!mdio_done) {
		ret = <devname>_reg_read(desc, <DEVNAME>_REG_MDIO_ACC,
					&mdio_val);
		if (ret)
			return ret;

		mdio_done = no_os_field_get(<DEVNAME>_MDIO_TRDONE,
					    mdio_val);
	}

	return 0;
}

/**
 * @brief Read a PHY register via MDIO (clause 22).
 * @param desc   - Device descriptor.
 * @param phy_id - PHY device address.
 * @param reg    - Register address.
 * @param data   - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_mdio_read(struct <devname>_desc *desc, uint32_t phy_id,
			uint32_t reg, uint16_t *data)
{
	uint32_t mdio_done = 0;
	uint32_t mdio_val;
	uint32_t val;
	int ret;

	val = no_os_field_prep(<DEVNAME>_MDIO_ST, 0x1) |
	      no_os_field_prep(<DEVNAME>_MDIO_OP, <DEVNAME>_MDIO_OP_RD) |
	      no_os_field_prep(<DEVNAME>_MDIO_PRTAD, phy_id) |
	      no_os_field_prep(<DEVNAME>_MDIO_DEVAD, reg);

	ret = <devname>_reg_write(desc, <DEVNAME>_REG_MDIO_ACC, val);
	if (ret)
		return ret;

	while (!mdio_done) {
		ret = <devname>_reg_read(desc, <DEVNAME>_REG_MDIO_ACC,
					&mdio_val);
		if (ret)
			return ret;

		mdio_done = no_os_field_get(<DEVNAME>_MDIO_TRDONE,
					    mdio_val);
	}

	*data = no_os_field_get(<DEVNAME>_MDIO_DATA, mdio_val);

	return 0;
}

/* ---------- Frame TX / RX ------------------------------------------ */

/**
 * @brief Write a frame to the TX FIFO.
 * @param desc     - Device descriptor.
 * @param port     - Port number for transmission.
 * @param eth_buff - Frame to transmit.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_write_fifo(struct <devname>_desc *desc, uint32_t port,
			 struct <devname>_eth_buff *eth_buff)
{
	uint32_t frame_offset;
	uint32_t padded_len;
	uint32_t padding = 0;
	uint32_t tx_space;
	struct no_os_spi_msg xfer = {0};
	int ret;

	/* Minimum ethernet frame length is 64 bytes (including FCS). */
	if (eth_buff->len + <DEVNAME>_FCS_LEN < 64)
		padding = 64 - (eth_buff->len + <DEVNAME>_FCS_LEN);

	padded_len = eth_buff->len + padding + 2; /* +2 for frame header */

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_TX_SPACE, &tx_space);
	if (ret)
		return ret;

	if (padded_len > 2 * tx_space)
		return -EAGAIN;

	ret = <devname>_reg_write(desc, <DEVNAME>_REG_TX_FSIZE, padded_len);
	if (ret)
		return ret;

	xfer.tx_buff = desc->data;
	xfer.rx_buff = desc->data;
	xfer.cs_change = 1;

	/* Build SPI header for TX register write. */
	no_os_put_unaligned_be16(<DEVNAME>_REG_TX, &desc->data[0]);
	desc->data[0] |= 0x80 | 0x20; /* CD + RW bits */
	frame_offset = 2;

	/* Port selection header. */
	no_os_put_unaligned_be16(port, &desc->data[frame_offset]);
	frame_offset += 2;

	/* Copy ethernet header (dest MAC + source MAC + ethertype). */
	memcpy(&desc->data[frame_offset], (void *)&eth_buff->mac_dest[0],
	       <DEVNAME>_ETH_HDR_LEN);
	frame_offset += <DEVNAME>_ETH_HDR_LEN;

	/* Copy payload. */
	memcpy(&desc->data[frame_offset], eth_buff->payload,
	       eth_buff->len - <DEVNAME>_ETH_HDR_LEN);

	xfer.bytes_number = no_os_align(padded_len + 2, 4);

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

/**
 * @brief Read a frame from the RX FIFO.
 * @param desc     - Device descriptor.
 * @param port     - Port number for reception.
 * @param eth_buff - Buffer to store the received frame.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read_fifo(struct <devname>_desc *desc, uint32_t port,
			struct <devname>_eth_buff *eth_buff)
{
	uint32_t field_offset = 3; /* SPI read header length */
	uint32_t payload_length;
	uint32_t rounded_len;
	uint32_t frame_size;
	struct no_os_spi_msg xfer = {0};
	int ret;

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_RX_FSIZE, &frame_size);
	if (ret)
		return ret;

	if (frame_size < 2 + <DEVNAME>_FCS_LEN)
		return 0;

	memset(desc->data, 0, <DEVNAME>_BUFF_LEN);
	no_os_put_unaligned_be16(<DEVNAME>_REG_RX, &desc->data[0]);
	desc->data[0] |= 0x80; /* CD bit */
	desc->data[2] = 0x00;

	rounded_len = no_os_align(frame_size, 4);
	xfer.tx_buff = desc->data;
	xfer.rx_buff = desc->data;
	xfer.cs_change = 1;
	xfer.bytes_number = rounded_len + field_offset;

	/* Skip port header. */
	field_offset += 2;

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	/* Extract ethernet header. */
	memcpy((void *)&eth_buff->mac_dest[0], &desc->data[field_offset],
	       <DEVNAME>_ETH_HDR_LEN);
	field_offset += <DEVNAME>_ETH_HDR_LEN;

	/* Extract payload. */
	payload_length = frame_size - 2 - <DEVNAME>_ETH_HDR_LEN;
	memcpy(eth_buff->payload, &desc->data[field_offset], payload_length);
	eth_buff->len = frame_size - 2;

	return 0;
}

/* ---------- Link management ---------------------------------------- */

/**
 * @brief Get the ethernet link state.
 * @param desc  - Device descriptor.
 * @param state - Pointer to store link state (1 = up, 0 = down).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_link_state(struct <devname>_desc *desc, uint32_t *state)
{
	int ret;

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_STATUS, state);
	if (ret)
		return ret;

	*state = no_os_field_get(<DEVNAME>_LINK_STATE_MASK, *state);

	return 0;
}

/* ---------- MAC filtering ------------------------------------------ */

/**
 * @brief Set a MAC address destination filter.
 * @param desc        - Device descriptor.
 * @param mac_address - The 6-byte MAC address to filter on.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_mac_addr(struct <devname>_desc *desc,
			   uint8_t mac_address[<DEVNAME>_ETH_ALEN])
{
	/*
	 * Write the MAC address to the appropriate filter registers.
	 * Implementation depends on the device's filter slot mechanism.
	 */
	return 0;
}

/**
 * @brief Enable/disable broadcast frame forwarding.
 * @param desc    - Device descriptor.
 * @param enabled - true to forward broadcast frames, false to drop.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_broadcast_filter(struct <devname>_desc *desc, bool enabled)
{
	uint8_t broadcast_addr[<DEVNAME>_ETH_ALEN];

	memset(broadcast_addr, 0xFF, <DEVNAME>_ETH_ALEN);

	if (enabled)
		return <devname>_set_mac_addr(desc, broadcast_addr);

	/* Clear the broadcast filter entry. */
	return 0;
}

/**
 * @brief Enable/disable promiscuous mode on a port.
 * @param desc    - Device descriptor.
 * @param port    - Port number.
 * @param promisc - true to enable, false to disable.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_promisc(struct <devname>_desc *desc, uint32_t port,
			  bool promisc)
{
	return <devname>_reg_update(desc, <DEVNAME>_REG_CONFIG,
				    <DEVNAME>_FWD_UNK2HOST_MASK,
				    promisc ? <DEVNAME>_FWD_UNK2HOST_MASK : 0);
}

/* ---------- Reset -------------------------------------------------- */

/**
 * @brief Perform a software reset (MAC and PHY).
 * @param desc - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_sw_reset(struct <devname>_desc *desc)
{
	return <devname>_reg_write(desc, <DEVNAME>_REG_SOFT_RST, 0x1);
}

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the <DEVNAME> device.
 * @param desc  - Pointer to device descriptor pointer (allocated here).
 * @param param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_init(struct <devname>_desc **desc,
		   struct <devname>_init_param *param)
{
	struct <devname>_desc *d;
	uint32_t phy_id;
	int ret;

	d = (struct <devname>_desc *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->chip_type = param->chip_type;
	d->append_crc = param->append_crc;
	memcpy(d->mac_address, param->mac_address, <DEVNAME>_ETH_ALEN);

	ret = no_os_gpio_get_optional(&d->reset_gpio, &param->reset_param);
	if (ret)
		goto free_dev;

	if (d->reset_gpio) {
		ret = no_os_gpio_direction_output(d->reset_gpio,
						  NO_OS_GPIO_LOW);
		if (ret)
			goto free_gpio;

		no_os_mdelay(10);

		ret = no_os_gpio_set_value(d->reset_gpio, NO_OS_GPIO_HIGH);
		if (ret)
			goto free_gpio;

		/* Wait for MAC/PHY to initialise after reset. */
		no_os_mdelay(90);
	}

	ret = no_os_spi_init(&d->comm_desc, &param->comm_param);
	if (ret)
		goto free_gpio;

	d->data = (uint8_t *)no_os_calloc(<DEVNAME>_BUFF_LEN,
					   sizeof(*d->data));
	if (!d->data) {
		ret = -ENOMEM;
		goto free_spi;
	}

	if (!d->reset_gpio) {
		ret = <devname>_sw_reset(d);
		if (ret)
			goto free_data;

		no_os_mdelay(90);
	}

	/* Verify PHY ID. */
	ret = <devname>_reg_read(d, <DEVNAME>_REG_PHY_ID, &phy_id);
	if (ret)
		goto free_data;

	if (phy_id != <DEVNAME>_PHY_ID) {
		pr_err("Unexpected PHY ID: 0x%08X\n", phy_id);
		ret = -ENODEV;
		goto free_data;
	}

	/* Set up MAC filters and initial configuration. */
	ret = <devname>_set_mac_addr(d, d->mac_address);
	if (ret)
		goto free_data;

	*desc = d;

	return 0;

free_data:
	no_os_free(d->data);
free_spi:
	no_os_spi_remove(d->comm_desc);
free_gpio:
	no_os_gpio_remove(d->reset_gpio);
free_dev:
	no_os_free(d);

	return ret;
}

/**
 * @brief Free resources allocated by <devname>_init().
 * @param desc - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_remove(struct <devname>_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_spi_remove(desc->comm_desc);
	if (ret)
		return ret;

	if (desc->reset_gpio) {
		ret = no_os_gpio_remove(desc->reset_gpio);
		if (ret)
			return ret;
	}

	no_os_free(desc->data);
	no_os_free(desc);

	return 0;
}
```

---

## 5. IIO Support

IIO is not typical for network/ethernet devices. Network drivers deal with
frame TX/RX, MAC filtering, and link management rather than analog
measurements.

If IIO is required (e.g., to expose PHY-level statistics such as frame
counters or error rates), follow the IIO header and source patterns from
the ADC driver template, adapting channel types and attributes to the
network domain.

---

## 6. Project Example

### 6.1 `projects/<project_name>/Makefile`

```makefile
# Select the example you want to enable by choosing y for enabling and n for disabling
BASIC_EXAMPLE = y

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
```

### 6.2 `projects/<project_name>/builds.json`

```json
{
  "<platform>": {
    "basic_example_<target>": {
      "flags" : "BASIC_EXAMPLE=y TARGET=<target>"
    }
  }
}
```

### 6.3 `projects/<project_name>/src.mk`

```makefile
include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

INCS += $(INCLUDE)/no_os_delay.h     \
	$(INCLUDE)/no_os_error.h     \
	$(INCLUDE)/no_os_gpio.h      \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_spi.h       \
	$(INCLUDE)/no_os_alloc.h     \
	$(INCLUDE)/no_os_irq.h       \
	$(INCLUDE)/no_os_list.h      \
	$(INCLUDE)/no_os_dma.h       \
	$(INCLUDE)/no_os_crc8.h      \
	$(INCLUDE)/no_os_uart.h      \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_util.h      \
	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_irq.c  \
	$(DRIVERS)/api/no_os_spi.c  \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_dma.c  \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_crc8.c  \
	$(NO-OS)/util/no_os_util.c  \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/net/<devname>/<devname>.h
SRCS += $(DRIVERS)/net/<devname>/<devname>.c
```

### 6.4 `projects/<project_name>/src/examples/examples_src.mk`

```makefile
ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif
```

### 6.5 `projects/<project_name>/src/common/common_data.h`

```c
/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by <devname> examples.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "<devname>.h"

extern struct no_os_uart_init_param uip;
extern struct no_os_spi_init_param <devname>_spi_ip;
extern struct no_os_gpio_init_param <devname>_reset_gpio_ip;

#endif /* __COMMON_DATA_H__ */
```

### 6.6 `projects/<project_name>/src/common/common_data.c`

```c
/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by <devname> examples.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param uip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

struct no_os_spi_init_param <devname>_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_MAX_SPEED,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
};

struct no_os_gpio_init_param <devname>_reset_gpio_ip = {
	.port = RST_GPIO_PORT,
	.number = RST_GPIO_NUM,
	.pull = NO_OS_PULL_UP,
	.platform_ops = GPIO_OPS,
	.extra = RST_GPIO_EXTRA,
};
```

### 6.7 `projects/<project_name>/src/examples/basic/basic_example.h`

```c
/***************************************************************************//**
 *   @file   basic_example.h
 *   @brief  Basic example header for <devname> project
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __BASIC_EXAMPLE_H__
#define __BASIC_EXAMPLE_H__

int basic_example_main();

#endif /* __BASIC_EXAMPLE_H__ */
```

### 6.8 `projects/<project_name>/src/examples/basic/basic_example.c`

```c
/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example code for <devname> project
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include <string.h>
#include "basic_example.h"
#include "common_data.h"
#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct <devname>_desc *dev;
	struct <devname>_init_param ip;
	uint32_t link_state;
	uint8_t mac_addr[<DEVNAME>_ETH_ALEN] = {
		0xCA, 0x2F, 0xB7, 0x10, 0x23, 0x63
	};
	uint8_t rx_payload[<DEVNAME>_BUFF_LEN];
	struct <devname>_eth_buff rx_buff = {
		.payload = rx_payload,
	};
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ip.chip_type = <DEVNAME>;
	ip.comm_param = <devname>_spi_ip;
	ip.reset_param = <devname>_reset_gpio_ip;
	ip.append_crc = false;
	memcpy(ip.mac_address, mac_addr, <DEVNAME>_ETH_ALEN);

	ret = <devname>_init(&dev, &ip);
	if (ret)
		goto error;

	/* Enable promiscuous mode to receive all frames. */
	ret = <devname>_set_promisc(dev, 0, true);
	if (ret)
		goto free_dev;

	while (1) {
		ret = <devname>_link_state(dev, &link_state);
		if (ret)
			goto free_dev;

		pr_info("Link state: %s\r\n",
			link_state ? "UP" : "DOWN");

		if (link_state) {
			rx_buff.len = 0;
			ret = <devname>_read_fifo(dev, 0, &rx_buff);
			if (ret)
				goto free_dev;

			if (rx_buff.len) {
				pr_info("Received frame (%u bytes)\r\n",
					rx_buff.len);
				pr_info("Dest MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
					rx_buff.mac_dest[0],
					rx_buff.mac_dest[1],
					rx_buff.mac_dest[2],
					rx_buff.mac_dest[3],
					rx_buff.mac_dest[4],
					rx_buff.mac_dest[5]);
			}
		}

		no_os_mdelay(1000);
	}

free_dev:
	<devname>_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}
```

### 6.9 `projects/<project_name>/src/platform/platform_includes.h`

```c
/***************************************************************************//**
 *   @file   platform_includes.h
 *   @brief  Platform includes for <devname> project
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __PLATFORM_INCLUDES_H__
#define __PLATFORM_INCLUDES_H__

#ifdef MAXIM_PLATFORM
#include "maxim/parameters.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
```

### 6.10 `projects/<project_name>/src/platform/maxim/main.c`

```c
/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "platform_includes.h"
#include "common_data.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

int main()
{
#ifdef BASIC_EXAMPLE
	int ret;
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart);
	ret = basic_example_main();
	if (ret)
		goto error;
#endif

#ifdef BASIC_EXAMPLE
error:
	no_os_uart_remove(uart);
#endif
	return 0;
}
```

### 6.11 `projects/<project_name>/src/platform/maxim/parameters.h`

```c
/***************************************************************************//**
 *   @brief  Definitions specific to Maxim platform used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"

#define UART_IRQ_ID	UART1_IRQn
#define UART_DEVICE_ID	1
#define UART_BAUDRATE	115200
#define UART_OPS	&max_uart_ops
#define UART_EXTRA	&max_uart_extra

#define SPI_DEVICE_ID	1
#define SPI_CS		0
#define SPI_MAX_SPEED	1000000
#define SPI_OPS		&max_spi_ops
#define SPI_EXTRA	&max_spi_extra

#define GPIO_OPS	&max_gpio_ops
#define RST_GPIO_PORT	0
#define RST_GPIO_NUM	19
#define RST_GPIO_EXTRA	NULL

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;

#endif /* __PARAMETERS_H__ */
```

### 6.12 `projects/<project_name>/src/platform/maxim/parameters.c`

```c
/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of maxim platform data used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#include "parameters.h"

struct max_uart_init_param max_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_spi_init_param max_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
```

### 6.13 `projects/<project_name>/src/platform/maxim/platform_src.mk`

```makefile
INCS +=	$(PLATFORM_DRIVERS)/maxim_gpio.h      \
	$(PLATFORM_DRIVERS)/maxim_spi.h       \
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h       \
	$(PLATFORM_DRIVERS)/maxim_irq.h       \
	$(PLATFORM_DRIVERS)/maxim_uart.h      \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
	$(PLATFORM_DRIVERS)/maxim_gpio.c      \
	$(PLATFORM_DRIVERS)/maxim_spi.c       \
	$(PLATFORM_DRIVERS)/../common/maxim_dma.c       \
	$(PLATFORM_DRIVERS)/maxim_irq.c       \
	$(PLATFORM_DRIVERS)/maxim_uart.c      \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c
```

---

## 7. Documentation

### 7.1 Driver `README.rst`

```rst
<DEVNAME> no-OS Driver
======================

.. no-os-doxygen::

Supported Devices
-----------------

* :adi:`<DEVNAME>`

Overview
--------

The <DEVNAME> is a low-power, single-port 10BASE-T1L ethernet MAC-PHY
with an SPI interface, designed for industrial ethernet connectivity.
It integrates a MAC and PHY to enable direct connection to a 10 Mbit/s
single-pair ethernet network. <Add 2-3 more sentences from the
datasheet covering key features: frame buffering, MAC filtering, MDIO
management, etc.>

Applications
------------

* Industrial ethernet / field-level connectivity
* Building automation
* Process control instrumentation

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, configures the reset GPIO (if available), initialises the
SPI interface, verifies the PHY ID, and sets up MAC address filtering.
The function uses goto-based cleanup to release resources on failure.

MAC Address Filtering
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_mac_addr`` function programs a destination MAC
filter so that only frames matching the configured address are
forwarded to the host. The ``<devname>_broadcast_filter`` function
enables or disables forwarding of broadcast frames.

Frame Transmit / Receive
~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_write_fifo`` function writes an ethernet frame to the
device TX FIFO for transmission on a specified port. The
``<devname>_read_fifo`` function reads a received frame from the RX
FIFO, extracting the ethernet header and payload.

Link Management
~~~~~~~~~~~~~~~

The ``<devname>_link_state`` function reads the device status register
to determine whether the ethernet link is up or down. The
``<devname>_set_promisc`` function enables or disables promiscuous mode,
allowing the host to receive all frames regardless of MAC filtering.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"
   #include "no_os_gpio.h"

   int main()
   {
       uint8_t mac[<DEVNAME>_ETH_ALEN] = {
           0xCA, 0x2F, 0xB7, 0x10, 0x23, 0x63
       };
       struct <devname>_init_param ip = {
           .chip_type = <DEVNAME>,
           .comm_param = { /* platform-specific SPI params */ },
           .reset_param = { /* platform-specific GPIO params */ },
           .append_crc = false,
       };

       memcpy(ip.mac_address, mac, <DEVNAME>_ETH_ALEN);

       struct <devname>_desc *dev;
       uint32_t link;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_link_state(dev, &link);
       if (ret)
           goto cleanup;

       /* link == 1 means connected */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }
```

### 7.2 Sphinx Entry (`doc/sphinx/source/drivers/net/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/net/<devname>/README.rst
```

---

## 8. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_desc *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int`, negative = error.
5. **Init signature** -- `int <devname>_init(struct <devname>_desc **desc,
   struct <devname>_init_param *param)`.
6. **Allocation** -- use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
7. **Goto cleanup** -- in `_init()`, use goto labels to free in reverse
   allocation order on error.
8. **Remove order** -- `_remove()` frees resources in reverse order of
   `_init()`.
9. **No globals** -- all state lives in the device descriptor struct.
10. **Register macros** -- `NO_OS_BIT()` for single bits,
    `NO_OS_GENMASK(hi, lo)` for fields; `no_os_field_prep()` /
    `no_os_field_get()` for field access.

---

## 9. Network Driver Specifics

1. **SPI protocol** -- network MAC-PHY devices use SPI for both register
   access and frame TX/RX. Register reads/writes use a control byte with
   address, CD (control/data), and RW bits. Frame data is burst-transferred
   through FIFO registers.
2. **MAC address** -- the device descriptor stores the local MAC address.
   The init function programs it as a filter so the device forwards
   matching frames to the host.
3. **Ethernet frame buffer** -- use a dedicated struct (e.g.,
   `<devname>_eth_buff`) to hold destination MAC, source MAC, ethertype,
   payload pointer, and total length.
4. **Reset GPIO** -- network devices typically require a hardware reset
   via GPIO (hold low for 10 ms, then wait 90 ms after release). If no
   GPIO is available, fall back to software reset.
5. **MDIO** -- PHY registers are accessed via MDIO (clause 22 or clause
   45), tunnelled through the MAC's SPI register interface.
6. **Link state** -- poll a status register to determine link up/down.
   The link state check is typically part of the main loop in examples.
7. **Frame padding** -- ethernet frames must be at least 64 bytes
   (including FCS). The driver must pad shorter frames before
   transmission.
8. **No IIO** -- network drivers typically do not implement IIO. Frame
   TX/RX and link management are the primary operations, not analog
   measurements.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> single-port 10BASE-T1L ethernet MAC-PHY
with SPI interface.

Signed-off-by: Your Name <your.name@analog.com>
```
