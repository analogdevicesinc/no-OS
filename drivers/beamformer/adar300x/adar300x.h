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
/* Beamstate and control registers below all live on the configuration page */
#define ADAR300X_PAGE_CONFIG			0x0

/* Beam control registers, configuration page */
#define ADAR300X_REG_BEAMSTATE_MODE		0x015
#define ADAR300X_REG_PIN_OR_SPI_CTL		0x030
#define ADAR300X_REG_BEAMWISE_UPDATE_CODE	0x032
#define ADAR300X_REG_BEAMWISE_UPDATE		0x033

/*
 * Three banks hold one delay and one attenuation byte per element, laid out
 * identically: bank base + beam * 8 + element * 2, attenuation one above its
 * delay. The reset and mute banks hold the beamstate applied by a RESET or
 * MUTE command, the direct bank the one applied by an UPDATE in direct mode.
 */
#define ADAR300X_REG_RESET_BEAMSTATE(x)		(0x080 + (x))
#define ADAR300X_REG_MUTE_BEAMSTATE(x)		(0x0A0 + (x))
#define ADAR300X_REG_DIRECT_CTRL(x)		(0x100 + (x))

#define ADAR300X_ELEMENTS_PER_BEAM		4
#define ADAR300X_VALUES_PER_BEAM		8

/* Delay and attenuation are 6-bit: 54.5 ps / 0.865 ps and 31.5 dB / 0.5 dB */
#define ADAR300X_RAW_MSK			NO_OS_GENMASK(5, 0)
#define ADAR300X_RAW_MAX			0x3F

/* ADAR300X_REG_BEAMSTATE_MODE holds two bits per beam */
#define ADAR300X_BEAM_MODE_MSK(beam)		(0x03 << ((beam) * 2))

/* ADAR300X_REG_BEAMWISE_UPDATE, one update strobe bit per beam */
#define ADAR300X_BEAM_UPDATE_MSK(beam)		NO_OS_BIT(beam)

/* ADAR300X_REG_PIN_OR_SPI_CTL, 0 selects the pins, 1 selects SPI */
#define ADAR300X_UPDATE_SPI_CTL			NO_OS_BIT(0)

/*
 * Amplifier bias and enable, one register per amplifier per state. The beam
 * bank carries four states and the element bank three, in different orders, so
 * the two take separate state enumerations rather than a shared one.
 */
#define ADAR300X_REG_BEAM_AMP(state, beam)	(0x0C0 + (state) * 4 + (beam))
#define ADAR300X_REG_ELEMENT_AMP(state, el)	(0x0D0 + (state) * 4 + (el))

#define ADAR300X_AMP_BIAS_MSK			NO_OS_GENMASK(2, 0)
#define ADAR300X_AMP_EN_MSK			NO_OS_BIT(3)
#define ADAR300X_AMP_BIAS_MAX			0x7

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
 * @enum adar300x_beam_mode
 * @brief Source a beam takes its next beamstate from on an update.
 */
enum adar300x_beam_mode {
	/** Beamstate held in the direct control registers */
	ADAR300X_BEAM_MODE_DIRECT = 0,
	/** Sequencer walks the beamstate RAM */
	ADAR300X_BEAM_MODE_MEMORY = 1,
	/** Sequencer pops the FIFO */
	ADAR300X_BEAM_MODE_FIFO = 2,
	/** Direct control registers applied as they are written, without UPDATE */
	ADAR300X_BEAM_MODE_INST_DIRECT = 3,
};

/**
 * @enum adar300x_beamstate
 * @brief Which of the three beamstate register banks to address.
 */
enum adar300x_beamstate {
	/** Applied by an update while the beam is in direct mode */
	ADAR300X_BEAMSTATE_DIRECT,
	/** Applied by a reset command */
	ADAR300X_BEAMSTATE_RESET,
	/** Applied by a mute command */
	ADAR300X_BEAMSTATE_MUTE,
};

/**
 * @enum adar300x_beam_amp_state
 * @brief Beam amplifier states, in register order from 0x0C0.
 */
enum adar300x_beam_amp_state {
	ADAR300X_BEAM_AMP_RESET = 0,
	ADAR300X_BEAM_AMP_OPERATIONAL = 1,
	ADAR300X_BEAM_AMP_MUTE = 2,
	ADAR300X_BEAM_AMP_SLEEP = 3,
};

/**
 * @enum adar300x_element_amp_state
 * @brief Element amplifier states, in register order from 0x0D0. The element
 *	  bank has no reset state and orders the rest differently to the beam
 *	  bank, so the two enumerations are deliberately not interchangeable.
 *
 *	  The register map also lists MUTE_EL0 to MUTE_EL3 at 0x0D8 to 0x0DB,
 *	  but they are not implemented: on an ADAR3000 they power up as 0x00
 *	  rather than the documented 0x0B and discard writes. Hence no mute
 *	  state here.
 */
enum adar300x_element_amp_state {
	ADAR300X_ELEMENT_AMP_OPERATIONAL = 0,
	ADAR300X_ELEMENT_AMP_SLEEP = 1,
};

/**
 * @enum adar300x_element_param
 * @brief The two values held per element.
 */
enum adar300x_element_param {
	/** Time delay unit setting, 0.865 ps per step */
	ADAR300X_DELAY,
	/** Digital step attenuator setting, 0.5 dB per step */
	ADAR300X_ATTENUATION,
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

/** Write one element value in one of the beamstate banks. */
int adar300x_set_element(struct adar300x_dev *dev,
			 enum adar300x_beamstate beamstate, uint8_t beam,
			 uint8_t element, enum adar300x_element_param param,
			 uint8_t val);

/** Read one element value back from one of the beamstate banks. */
int adar300x_get_element(struct adar300x_dev *dev,
			 enum adar300x_beamstate beamstate, uint8_t beam,
			 uint8_t element, enum adar300x_element_param param,
			 uint8_t *val);

/** Set the bias and enable of one beam amplifier in one state. */
int adar300x_set_beam_amp(struct adar300x_dev *dev,
			  enum adar300x_beam_amp_state state, uint8_t beam,
			  uint8_t bias, bool enable);

/** Read back the bias and enable of one beam amplifier. */
int adar300x_get_beam_amp(struct adar300x_dev *dev,
			  enum adar300x_beam_amp_state state, uint8_t beam,
			  uint8_t *bias, bool *enable);

/** Set the bias and enable of one element amplifier in one state. */
int adar300x_set_element_amp(struct adar300x_dev *dev,
			     enum adar300x_element_amp_state state,
			     uint8_t element, uint8_t bias, bool enable);

/** Read back the bias and enable of one element amplifier. */
int adar300x_get_element_amp(struct adar300x_dev *dev,
			     enum adar300x_element_amp_state state,
			     uint8_t element, uint8_t *bias, bool *enable);

/** Select where a beam takes its next beamstate from. */
int adar300x_set_beam_mode(struct adar300x_dev *dev, uint8_t beam,
			   enum adar300x_beam_mode mode);

/** Read back the mode of a beam. */
int adar300x_get_beam_mode(struct adar300x_dev *dev, uint8_t beam,
			   enum adar300x_beam_mode *mode);

/** Strobe an update on every beam set in the mask. */
int adar300x_update(struct adar300x_dev *dev, uint8_t beam_mask);

/** Choose whether update, mute and reset come from the pins or from SPI. */
int adar300x_set_update_source_spi(struct adar300x_dev *dev, bool spi);

/** Read back the update source selection. */
int adar300x_get_update_source_spi(struct adar300x_dev *dev, bool *spi);

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
