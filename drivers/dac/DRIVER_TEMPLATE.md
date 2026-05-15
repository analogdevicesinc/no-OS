# DAC Driver Template

Reference driver: `drivers/dac/ad5686/`

This template covers every file needed to add a new DAC driver to no-OS.
Replace `<devname>` with the part number (e.g., `ad5686`) and `<DEVNAME>`
with its uppercase form (e.g., `AD5686`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ad5686.h`, `ad5686.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ad5686.h`, `iio_ad5686.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __AD5686_H__` |
| Init param struct | `<devname>_init_param` | `ad5686_init_param` |
| Device struct | `<devname>_dev` | `ad5686_dev` |
| IIO descriptor | `<devname>_iio_desc` | `ad5686_iio_desc` |
| Functions | `<devname>_<verb>()` | `ad5686_init()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_AD5686` |
| Control defines | `<DEVNAME>_CTRL_<NAME>` | `AD5686_CTRL_WRITE` |
| DAC channels | `<DEVNAME>_CH_<N>` | `AD5686_CH_0` |

---

## 2. File Checklist

```
drivers/dac/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
    iio_<devname>.h          # IIO header (optional)
    iio_<devname>.c          # IIO implementation (optional)
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
            iio_example/
                iio_example.c
                iio_example.h
            examples_src.mk
        platform/
            platform_includes.h
            <platform>/
                main.c
                parameters.h
                parameters.c
                platform_src.mk

doc/sphinx/source/drivers/dac/<devname>.rst   # Sphinx entry
```

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
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"

/* ---- Command / control register codes ----------------------------- */

#define <DEVNAME>_CTRL_NOP		0
#define <DEVNAME>_CTRL_WRITE		1
#define <DEVNAME>_CTRL_UPDATE		2
#define <DEVNAME>_CTRL_WRITEUPDATE	3
#define <DEVNAME>_CTRL_PWR		4
#define <DEVNAME>_CTRL_LDAC_MASK	5
#define <DEVNAME>_CTRL_SWRESET		6
#define <DEVNAME>_CTRL_IREF_REG		7
#define <DEVNAME>_CTRL_RB_REG		9

/* ---- Power-down modes --------------------------------------------- */

#define <DEVNAME>_PWRM_NORMAL		0
#define <DEVNAME>_PWRM_1K		1
#define <DEVNAME>_PWRM_100K		2
#define <DEVNAME>_PWRM_THREESTATE	3
#define <DEVNAME>_PWRM_MASK		3

/* ---- Enable / disable flags --------------------------------------- */

#define <DEVNAME>_INTREF_EN		1
#define <DEVNAME>_INTREF_DIS		0

/* ---- Data formatting ---------------------------------------------- */

#define MAX_RESOLUTION			16
#define PKT_LENGTH			3

/* ---- Register map type -------------------------------------------- */

/*
 * Some DAC families have two register map formats (e.g., AD5683 vs
 * AD5686).  Use a define or enum to distinguish them if needed.
 */
#define <DEVNAME>_REG_MAP		1

/* ---- Enums -------------------------------------------------------- */

/** @enum <devname>_type
 *  @brief Supported device IDs. */
enum <devname>_type {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/**
 * @enum comm_type
 * @brief Communication interface selection.
 *
 * DACs often support both SPI and I2C.  The chip_info table stores the
 * correct interface for each device variant.
 */
enum comm_type {
	SPI,
	I2C,
};

/** @enum <devname>_dac_channels
 *  @brief DAC channel identifiers. */
enum <devname>_dac_channels {
	<DEVNAME>_CH_0,
	<DEVNAME>_CH_1,
	<DEVNAME>_CH_2,
	<DEVNAME>_CH_3,
	/* Add more channels as needed */
};

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_chip_info
 * @brief  Per-variant chip information (resolution, register map, comm).
 */
struct <devname>_chip_info {
	/** DAC resolution in bits. */
	uint8_t resolution;
	/** Register map type. */
	uint8_t register_map;
	/** Communication type (SPI or I2C). */
	enum comm_type communication;
	/** Channel address lookup table. */
	const uint32_t *channel_addr;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** I2C descriptor (NULL if SPI). */
	struct no_os_i2c_desc *i2c_desc;
	/** SPI descriptor (NULL if I2C). */
	struct no_os_spi_desc *spi_desc;
	/** Reset GPIO descriptor. */
	struct no_os_gpio_desc *gpio_reset;
	/** LDAC GPIO descriptor. */
	struct no_os_gpio_desc *gpio_ldac;
	/** Gain GPIO descriptor. */
	struct no_os_gpio_desc *gpio_gain;
	/** Active device type. */
	enum <devname>_type act_device;
	/** Per-channel power-down state mask. */
	uint32_t power_down_mask;
	/** LDAC mask register state. */
	uint32_t ldac_mask;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** I2C init parameters. */
	struct no_os_i2c_init_param i2c_init;
	/** SPI init parameters. */
	struct no_os_spi_init_param spi_init;
	/** Reset GPIO init. */
	struct no_os_gpio_init_param gpio_reset;
	/** LDAC GPIO init. */
	struct no_os_gpio_init_param gpio_ldac;
	/** Gain GPIO init. */
	struct no_os_gpio_init_param gpio_gain;
	/** Device type. */
	enum <devname>_type act_device;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Initialise the device. */
int32_t <devname>_init(struct <devname>_dev **device,
		       struct <devname>_init_param init_param);

/** @brief Free resources allocated by <devname>_init(). */
int32_t <devname>_remove(struct <devname>_dev *dev);

/** @brief Write to the shift register (core register access). */
uint16_t <devname>_set_shift_reg(struct <devname>_dev *dev,
				 uint8_t command,
				 uint8_t address,
				 uint16_t data);

/** @brief Write a value to a DAC input register. */
void <devname>_write_register(struct <devname>_dev *dev,
			      enum <devname>_dac_channels channel,
			      uint16_t data);

/** @brief Update DAC register from input register. */
void <devname>_update_register(struct <devname>_dev *dev,
			       enum <devname>_dac_channels channel);

/** @brief Write and update a DAC channel atomically. */
void <devname>_write_update_register(struct <devname>_dev *dev,
				     enum <devname>_dac_channels channel,
				     uint16_t data);

/** @brief Read back the DAC input register. */
uint16_t <devname>_read_back_register(struct <devname>_dev *dev,
				      enum <devname>_dac_channels channel);

/** @brief Set channel power-down mode. */
void <devname>_power_mode(struct <devname>_dev *dev,
			  enum <devname>_dac_channels channel,
			  uint8_t mode);

/** @brief Set LDAC mask for a channel. */
void <devname>_ldac_mask(struct <devname>_dev *dev,
			 enum <devname>_dac_channels channel,
			 uint8_t enable);

/** @brief Perform a software reset. */
void <devname>_software_reset(struct <devname>_dev *dev);

/** @brief Enable or disable the internal reference. */
void <devname>_internal_reference(struct <devname>_dev *dev,
				  uint8_t value);

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
#include <errno.h>
#include "<devname>.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* ---- Channel address lookup tables -------------------------------- */

static const uint32_t <devname>_channel_addr[] = {
	[<DEVNAME>_CH_0] = 1,
	[<DEVNAME>_CH_1] = 2,
	[<DEVNAME>_CH_2] = 4,
	[<DEVNAME>_CH_3] = 8,
};

/* ---- Chip info table ---------------------------------------------- */

/**
 * Per-variant chip information indexed by enum <devname>_type.
 */
static const struct <devname>_chip_info chip_info[] = {
	[ID_<DEVNAME>] = {
		.resolution = 16,
		.register_map = <DEVNAME>_REG_MAP,
		.communication = SPI,
		.channel_addr = <devname>_channel_addr,
	},
};

/* ---- Core register access ----------------------------------------- */

/**
 * @brief Write to the input shift register via SPI or I2C.
 * @param dev     - Device descriptor.
 * @param command - Command nibble (e.g., <DEVNAME>_CTRL_WRITE).
 * @param address - Channel address bits.
 * @param data    - 16-bit data word.
 * @return Read-back data (SPI only; 0 for I2C).
 *
 * Packet format (3 bytes):
 *   Byte 0: [CMD(4) | ADDR(4)]
 *   Byte 1: [Data MSB]
 *   Byte 2: [Data LSB]
 */
uint16_t <devname>_set_shift_reg(struct <devname>_dev *dev,
				 uint8_t command,
				 uint8_t address,
				 uint16_t data)
{
	uint8_t buf[PKT_LENGTH];
	uint16_t read_back = 0;

	buf[0] = ((command & 0x0F) << 4) | (address & 0x0F);
	buf[1] = (uint8_t)((data >> 8) & 0xFF);
	buf[2] = (uint8_t)(data & 0xFF);

	if (chip_info[dev->act_device].communication == SPI) {
		no_os_spi_write_and_read(dev->spi_desc, buf, PKT_LENGTH);
		read_back = ((uint16_t)buf[1] << 8) | buf[2];
	} else {
		no_os_i2c_write(dev->i2c_desc, buf, PKT_LENGTH, 1);
	}

	return read_back;
}

/* ---- Channel operations ------------------------------------------- */

/**
 * @brief Write a value to a DAC input register (dependent on LDAC).
 * @param dev     - Device descriptor.
 * @param channel - DAC channel.
 * @param data    - Value to write (MSB-aligned internally).
 */
void <devname>_write_register(struct <devname>_dev *dev,
			      enum <devname>_dac_channels channel,
			      uint16_t data)
{
	uint8_t offset = MAX_RESOLUTION -
			 chip_info[dev->act_device].resolution;
	uint8_t addr = chip_info[dev->act_device].channel_addr[channel];

	<devname>_set_shift_reg(dev, <DEVNAME>_CTRL_WRITE, addr,
				data << offset);
}

/**
 * @brief Update DAC register from the input register.
 * @param dev     - Device descriptor.
 * @param channel - DAC channel.
 */
void <devname>_update_register(struct <devname>_dev *dev,
			       enum <devname>_dac_channels channel)
{
	uint8_t addr = chip_info[dev->act_device].channel_addr[channel];

	<devname>_set_shift_reg(dev, <DEVNAME>_CTRL_UPDATE, addr, 0);
}

/**
 * @brief Write and update a DAC channel atomically.
 * @param dev     - Device descriptor.
 * @param channel - DAC channel.
 * @param data    - Value to write.
 */
void <devname>_write_update_register(struct <devname>_dev *dev,
				     enum <devname>_dac_channels channel,
				     uint16_t data)
{
	uint8_t offset = MAX_RESOLUTION -
			 chip_info[dev->act_device].resolution;
	uint8_t addr = chip_info[dev->act_device].channel_addr[channel];

	<devname>_set_shift_reg(dev, <DEVNAME>_CTRL_WRITEUPDATE, addr,
				data << offset);
}

/**
 * @brief Read back the DAC input register.
 * @param dev     - Device descriptor.
 * @param channel - DAC channel.
 * @return Read-back value.
 */
uint16_t <devname>_read_back_register(struct <devname>_dev *dev,
				      enum <devname>_dac_channels channel)
{
	uint8_t offset = MAX_RESOLUTION -
			 chip_info[dev->act_device].resolution;
	uint8_t addr = chip_info[dev->act_device].channel_addr[channel];
	uint16_t read_back;

	if (chip_info[dev->act_device].communication == SPI) {
		<devname>_set_shift_reg(dev, <DEVNAME>_CTRL_RB_REG, addr, 0);
		read_back = <devname>_set_shift_reg(dev, <DEVNAME>_CTRL_NOP,
						    0, 0);
	} else {
		uint8_t cmd = addr;
		uint8_t buf[2] = {0};

		no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
		no_os_i2c_read(dev->i2c_desc, buf, 2, 1);
		read_back = ((uint16_t)buf[0] << 8) | buf[1];
	}

	return read_back >> offset;
}

/* ---- Power / control ---------------------------------------------- */

/**
 * @brief Set channel power-down mode.
 * @param dev     - Device descriptor.
 * @param channel - DAC channel.
 * @param mode    - Power-down mode (NORMAL, 1K, 100K, THREESTATE).
 */
void <devname>_power_mode(struct <devname>_dev *dev,
			  enum <devname>_dac_channels channel,
			  uint8_t mode)
{
	uint8_t addr = chip_info[dev->act_device].channel_addr[channel];

	dev->power_down_mask &= ~(0x3 << (channel * 2));
	dev->power_down_mask |= (mode & 0x3) << (channel * 2);

	<devname>_set_shift_reg(dev, <DEVNAME>_CTRL_PWR, addr,
				dev->power_down_mask);
}

/**
 * @brief Set LDAC mask for a channel.
 * @param dev     - Device descriptor.
 * @param channel - DAC channel.
 * @param enable  - 1 = mask (ignore LDAC), 0 = unmask.
 */
void <devname>_ldac_mask(struct <devname>_dev *dev,
			 enum <devname>_dac_channels channel,
			 uint8_t enable)
{
	uint8_t addr = chip_info[dev->act_device].channel_addr[channel];

	if (enable)
		dev->ldac_mask |= addr;
	else
		dev->ldac_mask &= ~addr;

	<devname>_set_shift_reg(dev, <DEVNAME>_CTRL_LDAC_MASK, 0,
				dev->ldac_mask);
}

/**
 * @brief Perform a software reset.
 * @param dev - Device descriptor.
 */
void <devname>_software_reset(struct <devname>_dev *dev)
{
	<devname>_set_shift_reg(dev, <DEVNAME>_CTRL_SWRESET, 0, 0);
}

/**
 * @brief Enable or disable the internal reference.
 * @param dev   - Device descriptor.
 * @param value - 1 = enable, 0 = disable.
 */
void <devname>_internal_reference(struct <devname>_dev *dev,
				  uint8_t value)
{
	<devname>_set_shift_reg(dev, <DEVNAME>_CTRL_IREF_REG, 0, value);
}

/* ---- Init / Remove ------------------------------------------------ */

/**
 * @brief Initialise the <DEVNAME> device.
 * @param device     - Pointer to device descriptor pointer.
 * @param init_param - Initialization parameters (passed by value).
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_init(struct <devname>_dev **device,
		       struct <devname>_init_param init_param)
{
	struct <devname>_dev *dev;
	int32_t ret;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->act_device = init_param.act_device;
	dev->power_down_mask = 0;
	dev->ldac_mask = 0;

	/* Initialise communication interface (SPI or I2C). */
	if (chip_info[dev->act_device].communication == SPI) {
		ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
		if (ret)
			goto free_dev;
	} else {
		ret = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
		if (ret)
			goto free_dev;
	}

	/* Initialise GPIOs. */
	ret = no_os_gpio_get_optional(&dev->gpio_ldac, &init_param.gpio_ldac);
	if (ret)
		goto free_comm;

	if (dev->gpio_ldac) {
		ret = no_os_gpio_direction_output(dev->gpio_ldac, 0);
		if (ret)
			goto free_ldac;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_reset, &init_param.gpio_reset);
	if (ret)
		goto free_ldac;

	if (dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset, 1);
		if (ret)
			goto free_reset;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_gain, &init_param.gpio_gain);
	if (ret)
		goto free_reset;

	if (dev->gpio_gain) {
		ret = no_os_gpio_direction_output(dev->gpio_gain, 0);
		if (ret)
			goto free_gain;
	}

	*device = dev;

	return 0;

free_gain:
	no_os_gpio_remove(dev->gpio_gain);
free_reset:
	no_os_gpio_remove(dev->gpio_reset);
free_ldac:
	no_os_gpio_remove(dev->gpio_ldac);
free_comm:
	if (chip_info[dev->act_device].communication == SPI)
		no_os_spi_remove(dev->spi_desc);
	else
		no_os_i2c_remove(dev->i2c_desc);
free_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free resources allocated by <devname>_init().
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_remove(struct <devname>_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -EINVAL;

	if (chip_info[dev->act_device].communication == SPI)
		ret = no_os_spi_remove(dev->spi_desc);
	else
		ret = no_os_i2c_remove(dev->i2c_desc);

	no_os_gpio_remove(dev->gpio_ldac);
	no_os_gpio_remove(dev->gpio_reset);
	no_os_gpio_remove(dev->gpio_gain);
	no_os_free(dev);

	return ret;
}
```

---

## 5. IIO Header (`iio_<devname>.h`)

```c
/***************************************************************************//**
 *   @file   iio_<devname>.h
 *   @brief  Header file for <DEVNAME> IIO Driver
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __IIO_<DEVNAME>_H__
#define __IIO_<DEVNAME>_H__

#include "iio.h"
#include "<devname>.h"

/**
 * @struct <devname>_iio_desc
 * @brief  IIO-specific device descriptor.
 */
struct <devname>_iio_desc {
	/** Core driver descriptor. */
	struct <devname>_dev *<devname>_dev;
	/** IIO device structure. */
	struct iio_device *iio_dev;
	/** Bitmask of active channels. */
	uint32_t active_channels;
};

/**
 * @struct <devname>_iio_init_param
 * @brief  IIO initialization parameters.
 */
struct <devname>_iio_init_param {
	/** Core driver init params. */
	struct <devname>_init_param *<devname>_init_param;
};

/** @brief Initialise the IIO descriptor. */
int <devname>_iio_init(struct <devname>_iio_desc **desc,
		       struct <devname>_iio_init_param *init_param);

/** @brief Free IIO descriptor resources. */
int <devname>_iio_remove(struct <devname>_iio_desc *desc);

#endif /* __IIO_<DEVNAME>_H__ */
```

---

## 6. IIO Source (`iio_<devname>.c`)

```c
/***************************************************************************//**
 *   @file   iio_<devname>.c
 *   @brief  Implementation of <DEVNAME> IIO Driver
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iio_<devname>.h"
#include "iio_types.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/* ---------- Attribute callbacks ------------------------------------ */

/**
 * @brief Read raw DAC value for a channel.
 *
 * For DAC outputs, "raw" is typically readable (read back from the
 * input register) and writable (set a new output code).
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int32_t val;

	val = (int32_t)<devname>_read_back_register(
		desc-><devname>_dev,
		(enum <devname>_dac_channels)channel->ch_num);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write raw DAC value for a channel.
 */
static int <devname>_iio_write_raw(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	uint32_t val;

	if (no_os_str_to_uint32(buf, &val))
		return -EINVAL;

	<devname>_write_update_register(
		desc-><devname>_dev,
		(enum <devname>_dac_channels)channel->ch_num,
		(uint16_t)val);

	return len;
}

/**
 * @brief Read scale factor (mV per LSB = Vref / 2^bits).
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	/*
	 * Example: 2.5V reference, 16-bit DAC
	 * scale = 2500 mV / 65536 = 0.038147 mV per LSB
	 */
	val[0] = 0;
	val[1] = 38147;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_ch_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_raw,
			   .store = <devname>_iio_write_raw },
	{ .name = "scale", .show = <devname>_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_scan_type = {
	.sign = 'u',
	.realbits = 16,       /* DAC resolution */
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * DAC channels: IIO_VOLTAGE, ch_out = true (output), indexed.
 */
#define <DEVNAME>_IIO_CHANNEL(idx) \
	{ \
		.name = "voltage" #idx, \
		.ch_type = IIO_VOLTAGE, \
		.channel = (idx), \
		.scan_type = &<devname>_scan_type, \
		.scan_index = (idx), \
		.attributes = <devname>_iio_ch_attrs, \
		.ch_out = true, \
		.indexed = true, \
	}

static struct iio_channel <devname>_iio_channels[] = {
	<DEVNAME>_IIO_CHANNEL(0),
	<DEVNAME>_IIO_CHANNEL(1),
	<DEVNAME>_IIO_CHANNEL(2),
	<DEVNAME>_IIO_CHANNEL(3),
	END_ATTRIBUTES_ARRAY
};

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels) - 1,
};

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 */
int <devname>_iio_init(struct <devname>_iio_desc **desc,
		       struct <devname>_iio_init_param *init_param)
{
	struct <devname>_iio_desc *d;
	int ret;

	d = (struct <devname>_iio_desc *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = <devname>_init(&d-><devname>_dev,
			     *init_param-><devname>_init_param);
	if (ret)
		goto free_desc;

	d->iio_dev = &<devname>_iio_dev;

	*desc = d;

	return 0;

free_desc:
	no_os_free(d);

	return ret;
}

/**
 * @brief Free IIO descriptor resources.
 */
int <devname>_iio_remove(struct <devname>_iio_desc *desc)
{
	int ret;

	ret = <devname>_remove(desc-><devname>_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
```

**Note:** The DAC `_init()` in the reference driver (ad5686) takes `init_param`
**by value** rather than by pointer. This is reflected in the IIO init call:
`<devname>_init(&d-><devname>_dev, *init_param-><devname>_init_param)`.
If your driver uses pass-by-pointer, adjust accordingly.

---

## 7. Project Example

### 7.1 `projects/<project_name>/Makefile`

```makefile
# Select the example you want to enable by choosing y for enabling and n for disabling
BASIC_EXAMPLE = y
IIO_EXAMPLE = n

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
```

### 7.2 `projects/<project_name>/builds.json`

```json
{
  "<platform>": {
    "basic_example_<target>": {
      "flags" : "BASIC_EXAMPLE=y IIO_EXAMPLE=n TARGET=<target>"
    },
    "iio_example_<target>": {
      "flags" : "BASIC_EXAMPLE=n IIO_EXAMPLE=y TARGET=<target>"
    }
  }
}
```

### 7.3 `projects/<project_name>/src.mk`

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
	$(INCLUDE)/no_os_i2c.h       \
	$(INCLUDE)/no_os_alloc.h     \
	$(INCLUDE)/no_os_irq.h       \
	$(INCLUDE)/no_os_list.h      \
	$(INCLUDE)/no_os_uart.h      \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_util.h      \
	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_irq.c  \
	$(DRIVERS)/api/no_os_spi.c  \
	$(DRIVERS)/api/no_os_i2c.c  \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_util.c  \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/dac/<devname>/<devname>.h
SRCS += $(DRIVERS)/dac/<devname>/<devname>.c
```

### 7.4 `projects/<project_name>/src/examples/examples_src.mk`

```makefile
ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app
INCS += $(DRIVERS)/dac/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/dac/<devname>/iio_<devname>.c

INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
endif
```

### 7.5 `projects/<project_name>/src/common/common_data.h`

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
#ifdef IIO_SUPPORT
#include "iio_<devname>.h"
#endif

extern struct no_os_uart_init_param uip;
extern struct <devname>_init_param <devname>_ip;

#endif /* __COMMON_DATA_H__ */
```

### 7.6 `projects/<project_name>/src/common/common_data.c`

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

struct <devname>_init_param <devname>_ip = {
	.act_device = ID_<DEVNAME>,
	.spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_1,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	},
	/* GPIO init params — set port/pin to 0 if not used */
	.gpio_reset = { .port = 0, .number = 0, .platform_ops = GPIO_OPS },
	.gpio_ldac  = { .port = 0, .number = 0, .platform_ops = GPIO_OPS },
	.gpio_gain  = { .port = 0, .number = 0, .platform_ops = GPIO_OPS },
};
```

### 7.7 `projects/<project_name>/src/examples/basic/basic_example.h`

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

### 7.8 `projects/<project_name>/src/examples/basic/basic_example.c`

```c
/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example code for <devname> project
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

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
	struct <devname>_dev *dev;
	uint16_t readback;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, <devname>_ip);
	if (ret)
		goto error;

	/* Enable internal reference */
	<devname>_internal_reference(dev, <DEVNAME>_INTREF_EN);

	while (1) {
		/* Write mid-scale to channel 0 */
		<devname>_write_update_register(dev, <DEVNAME>_CH_0, 0x8000);

		/* Read back channel 0 */
		readback = <devname>_read_back_register(dev, <DEVNAME>_CH_0);
		pr_info("DAC CH0 readback: %u\r\n", readback);

		no_os_mdelay(1000);
	}

	<devname>_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}
```

### 7.9 `projects/<project_name>/src/examples/iio_example/iio_example.h`

```c
/***************************************************************************//**
 *   @file   iio_example.h
 *   @brief  IIO example header for <devname> project
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __IIO_EXAMPLE_H__
#define __IIO_EXAMPLE_H__

int iio_example_main();

#endif /* __IIO_EXAMPLE_H__ */
```

### 7.10 `projects/<project_name>/src/examples/iio_example/iio_example.c`

```c
/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example code for <devname> project
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "iio_example.h"
#include "common_data.h"
#include "no_os_print_log.h"

/*******************************************************************************
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 *******************************************************************************/
int iio_example_main()
{
	int ret;
	struct <devname>_iio_desc *<devname>_iio_dev;
	struct <devname>_iio_init_param <devname>_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	<devname>_iio_ip.<devname>_init_param = &<devname>_ip;
	ret = <devname>_iio_init(&<devname>_iio_dev, &<devname>_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "<devname>",
			.dev = <devname>_iio_dev,
			.dev_descriptor = <devname>_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error;

	ret = iio_app_run(app);
	if (ret)
		pr_info("Error: iio_app_run: %d\r\n", ret);

	iio_app_remove(app);
error:
	<devname>_iio_remove(<devname>_iio_dev);
	return ret;
}
```

### 7.11 Platform files

Same structure as ADC template — see ADC `DRIVER_TEMPLATE.md` sections
7.11 through 7.15.  The only differences:

- `parameters.h` may need both SPI and I2C platform includes
  (`maxim_spi.h`, `maxim_i2c.h`) and GPIO defines for reset/ldac/gain.
- `platform_src.mk` must include both SPI and I2C platform drivers.
- `src.mk` must include both `no_os_spi.h`/`no_os_i2c.h` and their
  API sources.

---

## 8. Documentation

### 8.1 Driver `README.rst`

```rst
<DEVNAME> no-OS Driver
======================

.. no-os-doxygen::

Supported Devices
-----------------

* :adi:`<DEVNAME>`

Overview
--------

The <DEVNAME> is a <resolution>-bit, <channels>-channel voltage output
digital-to-analog converter with <SPI/I2C> interface. It features
<key specs: internal reference, power-down modes, LDAC support, etc.>.
<Add 2-3 more sentences from the datasheet.>

Applications
------------

* Industrial automation
* Portable instrumentation
* Digital gain and offset adjustment

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI (or I2C) interface and GPIO pins
(reset, LDAC, gain), and prepares the device for operation.

DAC Channel Write / Update
~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_write_register`` function loads a value into the
DAC input register.  The ``<devname>_update_register`` function
transfers the input register to the DAC output.
``<devname>_write_update_register`` combines both operations
atomically.

Reference Configuration
~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_internal_reference`` function enables or disables
the on-chip voltage reference.

Power Management
~~~~~~~~~~~~~~~~

The ``<devname>_power_mode`` function selects per-channel
power-down modes: normal, 1 kΩ to GND, 100 kΩ to GND, or
three-state.

LDAC Control
~~~~~~~~~~~~

The ``<devname>_ldac_mask`` function controls which channels
respond to the LDAC pin.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .act_device = ID_<DEVNAME>,
           .spi_init = { /* platform-specific SPI params */ },
       };

       struct <devname>_dev *dev;
       int ret;

       ret = <devname>_init(&dev, ip);
       if (ret)
           return ret;

       <devname>_internal_reference(dev, <DEVNAME>_INTREF_EN);
       <devname>_write_update_register(dev, <DEVNAME>_CH_0, 0x8000);

       <devname>_remove(dev);
       return 0;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each DAC output as an
``IIO_VOLTAGE`` channel (output) with ``raw`` (read+write) and
``scale`` attributes.

IIO Initialization Example
---------------------------

.. code-block:: C

   int iio_example_main()
   {
       int ret;
       struct <devname>_iio_desc *iio_desc;
       struct <devname>_iio_init_param iio_ip;
       struct iio_app_desc *app;
       struct iio_app_init_param app_init_param = {0};

       iio_ip.<devname>_init_param = &<devname>_ip;
       ret = <devname>_iio_init(&iio_desc, &iio_ip);
       if (ret)
           return ret;

       struct iio_app_device iio_devices[] = {
           {
               .name = "<devname>",
               .dev = iio_desc,
               .dev_descriptor = iio_desc->iio_dev,
           },
       };

       app_init_param.devices = iio_devices;
       app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
       app_init_param.uart_init_params = uip;

       ret = iio_app_init(&app, app_init_param);
       if (ret)
           return ret;

       return iio_app_run(app);
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/dac/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/dac/<devname>/README.rst
```

---

## 9. Key Conventions

1. **Dual SPI/I2C** — the `chip_info` table stores the communication
   type for each variant; `_init()` initialises the correct interface.
2. **GPIO pins** — DACs commonly use reset, LDAC, and gain GPIOs.
   Use `no_os_gpio_get_optional()` so pins can be omitted.
3. **Resolution alignment** — data is MSB-aligned in the shift
   register.  Compute `offset = MAX_RESOLUTION - resolution` and
   shift accordingly.
4. **By-value init_param** — the ad5686 reference passes `init_param`
   by value (not pointer).  This is an older convention; newer drivers
   may pass by pointer.  Match the convention in use for your family.
5. **IIO ch_out** — DAC channels use `ch_out = true` (output).
6. **IIO raw read+write** — DAC `raw` attribute must have both
   `.show` and `.store` callbacks.
7. All other conventions (C++ compat, tabs, Doxygen, `no_os_calloc`,
   goto cleanup, negative error codes, etc.) match the ADC template.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> <resolution>-bit, <channels>-channel
voltage output DAC with <SPI/I2C> interface.

Signed-off-by: Your Name <your.name@analog.com>
```
