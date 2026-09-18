/***************************************************************************//**
 *   @file   adar300x.h
 *   @brief  Header file of ADAR3000/ADAR3001 Driver.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef ADAR300X_H_
#define ADAR300X_H_

#include <stdbool.h>
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

/* Configuration registers, page independent */
#define ADAR300X_REG_SPI_CONFIG			0x000
#define ADAR300X_REG_CHIP_RESET			0x001
#define ADAR300X_REG_CHIPTYPE			0x003
#define ADAR300X_REG_PRODUCT_ID_L		0x004
#define ADAR300X_REG_PRODUCT_ID_H		0x005
#define ADAR300X_REG_ADDRESS_PAGE		0x008
#define ADAR300X_REG_SCRATCHPAD			0x00A
#define ADAR300X_REG_SPI_REV			0x00B

/*
 * ADAR300X_REG_SPI_CONFIG is bit mirrored (bit7<->bit0, bit5<->bit2,
 * bit4<->bit3) so a write lands correctly whether the part latched the byte
 * MSB or LSB first. Always set both halves of a field.
 */
#define ADAR300X_SPI_CONFIG_RESET_		NO_OS_BIT(7)
#define ADAR300X_SPI_CONFIG_BIG_ENDIAN_		NO_OS_BIT(5)
#define ADAR300X_SPI_CONFIG_SDOACTIVE_		NO_OS_BIT(4)
#define ADAR300X_SPI_CONFIG_SDOACTIVE		NO_OS_BIT(3)
#define ADAR300X_SPI_CONFIG_BIG_ENDIAN		NO_OS_BIT(2)
#define ADAR300X_SPI_CONFIG_RESET		NO_OS_BIT(0)

/* Datasheet recommended soft reset command, and the value 0x000 reads after */
#define ADAR300X_SOFT_RESET_CMD			0xBD
#define ADAR300X_SPI_CONFIG_DEFAULT		0x3C

/*
 * ADAR300X_REG_CHIP_RESET resets only the chip whose ADDR3..0 straps match the
 * header. A soft reset through ADAR300X_REG_SPI_CONFIG is bus wide and is only
 * honoured when issued to chip 0; chips 1 to 15 ignore it entirely.
 */
#define ADAR300X_CHIP_RESET_MSK			NO_OS_GENMASK(2, 1)
#define ADAR300X_CHIP_RESET_VAL			0x1
/*
 * The datasheet asks for two, preferably three, back to back soft resets when
 * RSTB could not be held low across the supply ramp.
 */
#define ADAR300X_SOFT_RESET_COUNT		3

/* ADAR300X_REG_ADDRESS_PAGE */
#define ADAR300X_ADDRESS_PAGE_MSK		NO_OS_GENMASK(3, 0)
/* Not a real page value, forces the next paged access to write the register */
#define ADAR300X_PAGE_INVALID			0xFF

/*
 * 16-bit address header: A15 = R/W, A14 = 0 for normal transactions,
 * A13:A10 = chip ID, A9:A0 = register address.
 */
#define ADAR300X_SPI_READ_CMD			NO_OS_BIT(15)
#define ADAR300X_REG_ADDR_MSK			NO_OS_GENMASK(9, 0)
#define ADAR300X_DEV_ADDR_MSK			NO_OS_GENMASK(13, 10)
/*
 * The chip ID is transmitted as-is. The datasheet describes A13:A10 as being
 * XNOR'ed with the ADDR3..0 strap pins, which only means the part answers when
 * the transmitted nibble equals its straps, it is not a complement. See the
 * EVALZ user guide Table 1: chip 0 -> 0x000A, chip 1 -> 0x040A.
 */
#define ADAR300X_DEV_ADDR(x) \
	no_os_field_prep(ADAR300X_DEV_ADDR_MSK, (x) & 0xF)

#define ADAR300X_BUFF_SIZE_BYTES		3
#define ADAR300X_SPI_DUMMY_DATA			0x00

/* Expected ADAR300X_REG_CHIPTYPE readback for the whole family */
#define ADAR300X_CHIPTYPE_VAL			0x01

/* Scratchpad patterns used to prove the bus works in both directions */
#define ADAR300X_SCRATCHPAD_PATTERN_1		0xAD
#define ADAR300X_SCRATCHPAD_PATTERN_2		0xEA

/**
 * @enum adar300x_type
 * @brief Supported devices.
 */
enum adar300x_type {
	ADAR3000,
	ADAR3001,
	ADAR3002,
	ADAR3003,
};

/**
 * @struct adar300x_chip_info
 * @brief Per part constants.
 */
struct adar300x_chip_info {
	/** Part name, used in log messages */
	const char	*name;
	/** Expected ADAR300X_REG_PRODUCT_ID_L, informational only */
	uint8_t		product_id;
	/** Number of beams */
	uint8_t		num_beams;
	/** Number of elements per beam */
	uint8_t		num_elements;
};

/**
 * @struct adar300x_dev
 * @brief ADAR300X device descriptor. One instance is one physical chip, and
 *        several chips may share one SPI descriptor, see spi_desc_shared.
 */
struct adar300x_dev {
	/** SPI descriptor */
	struct no_os_spi_desc			*spi_desc;
	/** False when spi_desc was adopted and must not be freed */
	bool					spi_desc_is_owned;
	/** Per part constants */
	const struct adar300x_chip_info		*chip_info;
	/** Part variant */
	enum adar300x_type			type;
	/** Chip ID, 0 to 15, must match the ADDR3..0 straps */
	uint8_t					dev_addr;
	/** Cached ADDRESS_PAGE, ADAR300X_PAGE_INVALID when unknown */
	uint8_t					cur_page;
	/** Hard reset, active low (RSTB) */
	struct no_os_gpio_desc			*gpio_rstb;
	/** SPI source select on the eval board mux, must be driven low */
	struct no_os_gpio_desc			*gpio_mux_sel;
	/** Beam state latch strobe, rising edge */
	struct no_os_gpio_desc			*gpio_update;
	/** Beam reset qualifier, active high */
	struct no_os_gpio_desc			*gpio_reset;
	/** Beam mute qualifier, active high */
	struct no_os_gpio_desc			*gpio_mute;
};

/**
 * @struct adar300x_init_param
 * @brief ADAR300X initialization parameters.
 */
struct adar300x_init_param {
	/** SPI initialization parameters, ignored when spi_desc_shared is set */
	struct no_os_spi_init_param		spi_init;
	/** Adopt this descriptor instead of initializing SPI, NULL to own the bus */
	struct no_os_spi_desc			*spi_desc_shared;
	/** Part variant */
	enum adar300x_type			type;
	/** Chip ID, 0 to 15, must match the ADDR3..0 straps */
	uint8_t					dev_addr;
	/** Hard reset GPIO, optional */
	struct no_os_gpio_init_param		*gpio_rstb;
	/** Eval board SPI mux select GPIO, optional */
	struct no_os_gpio_init_param		*gpio_mux_sel;
	/** Beam state latch strobe GPIO, optional */
	struct no_os_gpio_init_param		*gpio_update;
	/** Beam reset qualifier GPIO, optional */
	struct no_os_gpio_init_param		*gpio_reset;
	/** Beam mute qualifier GPIO, optional */
	struct no_os_gpio_init_param		*gpio_mute;
};

/** Write one register. */
int adar300x_reg_write(struct adar300x_dev *dev, uint16_t reg_addr,
		       uint8_t data);

/** Read one register. */
int adar300x_reg_read(struct adar300x_dev *dev, uint16_t reg_addr,
		      uint8_t *data);

/** Read modify write one register. */
int adar300x_reg_update(struct adar300x_dev *dev, uint16_t reg_addr,
			uint8_t mask, uint8_t data);

/** Select the address page, skipping the write when already selected. */
int adar300x_set_page(struct adar300x_dev *dev, uint8_t page);

/** Write one register on a given address page. */
int adar300x_page_reg_write(struct adar300x_dev *dev, uint8_t page,
			    uint16_t reg_addr, uint8_t data);

/** Read one register from a given address page. */
int adar300x_page_reg_read(struct adar300x_dev *dev, uint8_t page,
			   uint16_t reg_addr, uint8_t *data);

/** Issue the SPI soft reset sequence. */
int adar300x_soft_reset(struct adar300x_dev *dev);

/** Pulse the RSTB pin, no-op when no reset GPIO is wired. */
int adar300x_hard_reset(struct adar300x_dev *dev);

/** Initialize the device. */
int adar300x_init(struct adar300x_dev **dev,
		  struct adar300x_init_param *init_param);

/** Free resources allocated by adar300x_init(). */
int adar300x_remove(struct adar300x_dev *dev);

#endif /* ADAR300X_H_ */
