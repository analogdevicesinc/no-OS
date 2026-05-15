# Photo-Electronic / Optical Sensing Driver Template

Reference driver: `drivers/photo-electronic/adpd188/`

This template covers every file needed to add a new photo-electronic (optical
sensing) driver to no-OS. Replace `<devname>` with the part number (e.g.,
`adpd188`) and `<DEVNAME>` with its uppercase form (e.g., `ADPD188`)
throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adpd188.h`, `adpd188.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_adpd188.h`, `iio_adpd188.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADPD188_H__` |
| Init param struct | `<devname>_init_param` | `adpd188_init_param` |
| Device struct | `<devname>_dev` | `adpd188_dev` |
| IIO descriptor | `<devname>_iio_desc` | `adpd188_iio_desc` |
| Functions | `<devname>_<verb>()` | `adpd188_init()` |
| Static helpers | `_<devname>_<verb>()` | `_adpd188_spi_read()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADPD188BI` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `ADPD188_MODE_MODE_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADPD188_SLOTA` |

---

## 2. File Checklist

```
drivers/photo-electronic/<devname>/
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

doc/sphinx/source/drivers/photo-electronic/<devname>.rst   # Sphinx entry
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
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"

/* ---------------- Register Map ----------------------------------------- */

#define <DEVNAME>_REG_STATUS		0x00
#define <DEVNAME>_REG_INT_MASK		0x01
#define <DEVNAME>_REG_DEVID		0x08
#define <DEVNAME>_REG_MODE		0x10
#define <DEVNAME>_REG_SLOT_EN		0x11
#define <DEVNAME>_REG_FSAMPLE		0x12
#define <DEVNAME>_REG_PD_LED_SELECT	0x14
#define <DEVNAME>_REG_SLOTA_LED_PULSE	0x30
#define <DEVNAME>_REG_SLOTA_NUMPULSES	0x31
#define <DEVNAME>_REG_SLOTB_LED_PULSE	0x35
#define <DEVNAME>_REG_SLOTB_NUMPULSES	0x36
#define <DEVNAME>_REG_FIFO_ACCESS	0x60
#define <DEVNAME>_REG_DATA		0x64

/* ---------------- Field Masks ------------------------------------------ */

/** Use NO_OS_BIT() for single-bit fields,
 *  NO_OS_GENMASK(hi, lo) for multi-bit fields. */
#define <DEVNAME>_MODE_MSK		NO_OS_GENMASK(1, 0)
#define <DEVNAME>_SLOT_EN_SLOTA_MSK	NO_OS_BIT(0)
#define <DEVNAME>_SLOT_EN_SLOTB_MSK	NO_OS_BIT(5)
#define <DEVNAME>_SLOTA_FIFO_MODE_MSK	NO_OS_GENMASK(4, 2)
#define <DEVNAME>_SLOTB_FIFO_MODE_MSK	NO_OS_GENMASK(8, 6)
#define <DEVNAME>_FIFO_SAMPLES_MSK	NO_OS_GENMASK(15, 8)

/* Product / revision ID expected values */
#define <DEVNAME>_PRODUCT_ID		0xXX

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_phy_opt
 *  @brief Communication physical protocol. */
enum <devname>_phy_opt {
	/** SPI communication. */
	<DEVNAME>_SPI,
	/** I2C communication. */
	<DEVNAME>_I2C,
};

/** @enum <devname>_mode
 *  @brief Device operating modes. */
enum <devname>_mode {
	/** Standby — low power, registers accessible. */
	<DEVNAME>_STANDBY,
	/** Program — configuration mode. */
	<DEVNAME>_PROGRAM,
	/** Normal — active sampling. */
	<DEVNAME>_NORMAL,
};

/** @enum <devname>_slots
 *  @brief Time slot identifiers for multiplexed sampling. */
enum <devname>_slots {
	/** Time slot A. */
	<DEVNAME>_SLOTA,
	/** Time slot B. */
	<DEVNAME>_SLOTB,
};

/** @enum <devname>_slot_fifo_mode
 *  @brief How a time slot stores data into the FIFO. */
enum <devname>_slot_fifo_mode {
	/** No data written to FIFO. */
	<DEVNAME>_NO_FIFO,
	/** 16-bit sum of all channels. */
	<DEVNAME>_16BIT_SUM,
	/** 32-bit sum of all channels. */
	<DEVNAME>_32BIT_SUM,
	/** Four channels of 16-bit sample data. */
	<DEVNAME>_16BIT_4CHAN = 0x4,
	/** Four channels of 32-bit sample data. */
	<DEVNAME>_32BIT_4CHAN = 0x6,
};

/* Add more enums as needed: LED selection, TIA gain, etc. */

/* ---------------- Structures ------------------------------------------- */

/**
 * @union <devname>_phy_init
 * @brief  Communication physical protocol initialization.
 *         Can be I2C or SPI.
 */
union <devname>_phy_init {
	/** I2C initialization structure. */
	struct no_os_i2c_init_param i2c_phy;
	/** SPI initialization structure. */
	struct no_os_spi_init_param spi_phy;
};

/**
 * @struct <devname>_slot_config
 * @brief  Slot configuration — enable, FIFO mode.
 */
struct <devname>_slot_config {
	/** Time slot ID. */
	enum <devname>_slots slot_id;
	/** Enable time slot. */
	bool slot_en;
	/** Time slot FIFO data mode. */
	enum <devname>_slot_fifo_mode fifo_mode;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Device variant. */
	enum <devname>_device_id id;
	/** Communication physical type (SPI or I2C). */
	enum <devname>_phy_opt phy_opt;
	/** Communication physical initialization structure. */
	union <devname>_phy_init phy_init;
	/** GPIO 0 initialization structure (interrupt / data ready). */
	struct no_os_gpio_init_param gpio0_init;
	/** GPIO 1 initialization structure. */
	struct no_os_gpio_init_param gpio1_init;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** Device variant. */
	enum <devname>_device_id id;
	/** Communication physical type. */
	enum <devname>_phy_opt phy_opt;
	/** Communication physical descriptor (SPI or I2C). */
	void *phy_desc;
	/** GPIO 0 descriptor. */
	struct no_os_gpio_desc *gpio0;
	/** GPIO 1 descriptor. */
	struct no_os_gpio_desc *gpio1;
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Read a 16-bit register. */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint16_t *val);

/** @brief Write a 16-bit register. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint16_t val);

/** @brief Get the current operating mode. */
int <devname>_mode_get(struct <devname>_dev *dev,
		       enum <devname>_mode *mode);

/** @brief Set the operating mode. */
int <devname>_mode_set(struct <devname>_dev *dev,
		       enum <devname>_mode new_mode);

/** @brief Configure a time slot (enable, FIFO mode). */
int <devname>_slot_setup(struct <devname>_dev *dev,
			 struct <devname>_slot_config *config);

/** @brief Set the ADC sampling frequency. */
int <devname>_adc_fsample_set(struct <devname>_dev *dev,
			      uint16_t freq_hz);

/** @brief Get the ADC sampling frequency. */
int <devname>_adc_fsample_get(struct <devname>_dev *dev,
			      uint16_t *freq_hz);

/** @brief Perform a software reset. */
int <devname>_sw_reset(struct <devname>_dev *dev);

/** @brief Read raw photodiode data from a slot. */
int <devname>_read_data(struct <devname>_dev *dev,
			enum <devname>_slots slot,
			uint32_t *data, uint8_t nb_channels);

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
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* Device name lookup table (indexed by enum <devname>_device_id). */
static const char * const <devname>_device_name[] = {
	[ID_<DEVNAME>] = "<DEVNAME>",
};

/* ---------- Register access (dual I2C + SPI) --------------------------- */

/**
 * @brief Read a 16-bit register.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 *
 * SPI protocol: first byte = (addr << 1) & 0xFE (read), then 2 data bytes.
 * I2C protocol: write address byte, then read 2 data bytes.
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint16_t *val)
{
	uint8_t buf[3] = {0, 0, 0};
	int ret;

	if (dev->phy_opt == <DEVNAME>_SPI) {
		buf[0] = (addr << 1) & 0xFE;
		ret = no_os_spi_write_and_read(
			(struct no_os_spi_desc *)dev->phy_desc,
			buf, sizeof(buf));
	} else {
		buf[0] = addr;
		ret = no_os_i2c_write(
			(struct no_os_i2c_desc *)dev->phy_desc,
			&buf[0], 1, 0);
		if (ret)
			return ret;
		ret = no_os_i2c_read(
			(struct no_os_i2c_desc *)dev->phy_desc,
			&buf[1], 2, 1);
	}
	if (ret)
		return ret;

	*val = ((uint16_t)buf[1] << 8) | buf[2];

	return 0;
}

/**
 * @brief Write a 16-bit register.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint16_t val)
{
	uint8_t buf[3];
	int ret;

	buf[1] = (uint8_t)(val >> 8);
	buf[2] = (uint8_t)(val & 0xFF);

	if (dev->phy_opt == <DEVNAME>_SPI) {
		buf[0] = (addr << 1) | 0x01;
		ret = no_os_spi_write_and_read(
			(struct no_os_spi_desc *)dev->phy_desc,
			buf, sizeof(buf));
	} else {
		buf[0] = addr;
		ret = no_os_i2c_write(
			(struct no_os_i2c_desc *)dev->phy_desc,
			buf, sizeof(buf), 1);
	}

	return ret;
}

/* ---------- Public API ------------------------------------------------- */

/**
 * @brief Get the current operating mode.
 * @param dev  - Device descriptor.
 * @param mode - Pointer to store mode value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_mode_get(struct <devname>_dev *dev,
		       enum <devname>_mode *mode)
{
	uint16_t data;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_MODE, &data);
	if (ret)
		return ret;

	*mode = no_os_field_get(<DEVNAME>_MODE_MSK, data);

	return 0;
}

/**
 * @brief Set the operating mode (standby, program, or normal).
 * @param dev      - Device descriptor.
 * @param new_mode - New operating mode.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_mode_set(struct <devname>_dev *dev,
		       enum <devname>_mode new_mode)
{
	uint16_t data;

	data = no_os_field_prep(<DEVNAME>_MODE_MSK, new_mode);

	return <devname>_reg_write(dev, <DEVNAME>_REG_MODE, data);
}

/**
 * @brief Perform a software reset.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_sw_reset(struct <devname>_dev *dev)
{
	return <devname>_reg_write(dev, <DEVNAME>_REG_SW_RESET, 0x0001);
}

/**
 * @brief Configure a time slot — enable it and set its FIFO mode.
 * @param dev    - Device descriptor.
 * @param config - Pointer to slot configuration.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_slot_setup(struct <devname>_dev *dev,
			 struct <devname>_slot_config *config)
{
	uint16_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_SLOT_EN, &reg_val);
	if (ret)
		return ret;

	if (config->slot_id == <DEVNAME>_SLOTA) {
		reg_val &= ~<DEVNAME>_SLOT_EN_SLOTA_MSK;
		reg_val |= no_os_field_prep(<DEVNAME>_SLOT_EN_SLOTA_MSK,
					    config->slot_en);
		reg_val &= ~<DEVNAME>_SLOTA_FIFO_MODE_MSK;
		reg_val |= no_os_field_prep(<DEVNAME>_SLOTA_FIFO_MODE_MSK,
					    config->fifo_mode);
	} else {
		reg_val &= ~<DEVNAME>_SLOT_EN_SLOTB_MSK;
		reg_val |= no_os_field_prep(<DEVNAME>_SLOT_EN_SLOTB_MSK,
					    config->slot_en);
		reg_val &= ~<DEVNAME>_SLOTB_FIFO_MODE_MSK;
		reg_val |= no_os_field_prep(<DEVNAME>_SLOTB_FIFO_MODE_MSK,
					    config->fifo_mode);
	}

	return <devname>_reg_write(dev, <DEVNAME>_REG_SLOT_EN, reg_val);
}

/**
 * @brief Set the ADC sampling frequency.
 * @param dev     - Device descriptor.
 * @param freq_hz - Desired frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_adc_fsample_set(struct <devname>_dev *dev,
			      uint16_t freq_hz)
{
	uint16_t reg_val;

	if (!freq_hz)
		return -EINVAL;

	reg_val = 32000 / (freq_hz * 4);

	return <devname>_reg_write(dev, <DEVNAME>_REG_FSAMPLE, reg_val);
}

/**
 * @brief Get the ADC sampling frequency.
 * @param dev     - Device descriptor.
 * @param freq_hz - Pointer to store frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_adc_fsample_get(struct <devname>_dev *dev,
			      uint16_t *freq_hz)
{
	uint16_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_FSAMPLE, &reg_val);
	if (ret)
		return ret;

	*freq_hz = 32000 / (reg_val * 4);

	return 0;
}

/**
 * @brief Read raw photodiode data from a time slot via FIFO.
 * @param dev         - Device descriptor.
 * @param slot        - Time slot to read (SLOTA or SLOTB).
 * @param data        - Buffer to store channel samples.
 * @param nb_channels - Number of photodiode channels to read.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read_data(struct <devname>_dev *dev,
			enum <devname>_slots slot,
			uint32_t *data, uint8_t nb_channels)
{
	uint16_t lo, hi;
	uint8_t i;
	int ret;

	for (i = 0; i < nb_channels; i++) {
		ret = <devname>_reg_read(dev, <DEVNAME>_REG_FIFO_ACCESS,
					&lo);
		if (ret)
			return ret;
		ret = <devname>_reg_read(dev, <DEVNAME>_REG_FIFO_ACCESS,
					&hi);
		if (ret)
			return ret;

		data[i] = ((uint32_t)hi << 16) | lo;
	}

	return 0;
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param dev        - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *d;
	uint16_t devid;
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->id = init_param->id;
	d->phy_opt = init_param->phy_opt;

	/* Initialise communication interface (SPI or I2C) */
	if (d->phy_opt == <DEVNAME>_SPI)
		ret = no_os_spi_init(
			(struct no_os_spi_desc **)&d->phy_desc,
			&init_param->phy_init.spi_phy);
	else
		ret = no_os_i2c_init(
			(struct no_os_i2c_desc **)&d->phy_desc,
			&init_param->phy_init.i2c_phy);
	if (ret)
		goto free_dev;

	/* Verify device ID */
	ret = <devname>_reg_read(d, <DEVNAME>_REG_DEVID, &devid);
	if (ret)
		goto free_phy;

	if (devid != <DEVNAME>_PRODUCT_ID) {
		pr_err("Unexpected device ID: 0x%04X\n", devid);
		ret = -ENODEV;
		goto free_phy;
	}

	/* Software reset */
	ret = <devname>_sw_reset(d);
	if (ret)
		goto free_phy;

	/* Initialise GPIOs (interrupt / data ready) */
	ret = no_os_gpio_get(&d->gpio0, &init_param->gpio0_init);
	if (ret)
		goto free_phy;

	ret = no_os_gpio_get(&d->gpio1, &init_param->gpio1_init);
	if (ret)
		goto free_gpio0;

	ret = no_os_gpio_direction_input(d->gpio0);
	if (ret)
		goto free_gpio1;

	ret = no_os_gpio_direction_input(d->gpio1);
	if (ret)
		goto free_gpio1;

	*dev = d;

	return 0;

free_gpio1:
	no_os_gpio_remove(d->gpio1);
free_gpio0:
	no_os_gpio_remove(d->gpio0);
free_phy:
	if (d->phy_opt == <DEVNAME>_SPI)
		no_os_spi_remove((struct no_os_spi_desc *)d->phy_desc);
	else
		no_os_i2c_remove((struct no_os_i2c_desc *)d->phy_desc);
free_dev:
	no_os_free(d);

	return ret;
}

/**
 * @brief Free resources allocated by <devname>_init().
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_remove(struct <devname>_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_remove(dev->gpio1);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio0);
	if (ret)
		return ret;

	if (dev->phy_opt == <DEVNAME>_SPI)
		ret = no_os_spi_remove(
			(struct no_os_spi_desc *)dev->phy_desc);
	else
		ret = no_os_i2c_remove(
			(struct no_os_i2c_desc *)dev->phy_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
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

/** Number of photodiode channels (4 per slot x 2 slots). */
#define <DEVNAME>_CHANNEL_NO		8
/** Bits per FIFO word. */
#define <DEVNAME>_WORD_BIT_SIZE		16
/** Bits per 32-bit combined sample. */
#define <DEVNAME>_BITS_PER_SAMPLE	32
/** Number of 16-bit words per 32-bit sample. */
#define <DEVNAME>_WORDS_PER_SAMPLE	\
	(<DEVNAME>_BITS_PER_SAMPLE / <DEVNAME>_WORD_BIT_SIZE)

/**
 * @struct <devname>_iio_desc
 * @brief  IIO-specific device descriptor.
 */
struct <devname>_iio_desc {
	/** Core driver descriptor. */
	struct <devname>_dev *drv_dev;
	/** IIO device structure. */
	struct iio_device *iio_dev;
	/** Bitmask of active channels for buffered reads. */
	uint32_t active_channels;
};

/**
 * @struct <devname>_iio_init_param
 * @brief  IIO initialization parameters.
 */
struct <devname>_iio_init_param {
	/** Core driver init params. */
	struct <devname>_init_param drv_init_param;
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
#include <inttypes.h>
#include "iio_<devname>.h"
#include "iio_types.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/* ---------- Attribute callbacks ---------------------------------------- */

/**
 * @brief Read raw photodiode value for a channel.
 *
 * Enters normal mode, waits for FIFO data, reads all channels
 * from one complete sample set, and returns the requested channel.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc =
		(struct <devname>_iio_desc *)dev;
	uint16_t data[<DEVNAME>_WORDS_PER_SAMPLE * <DEVNAME>_CHANNEL_NO];
	uint32_t sample;
	uint8_t i;
	int ret;

	ret = <devname>_mode_set(desc->drv_dev, <DEVNAME>_PROGRAM);
	if (ret)
		return ret;
	ret = <devname>_mode_set(desc->drv_dev, <DEVNAME>_NORMAL);
	if (ret)
		return ret;

	/* Wait for FIFO to fill (poll status register). */
	/* ... device-specific wait logic ... */

	for (i = 0;
	     i < (<DEVNAME>_WORDS_PER_SAMPLE * <DEVNAME>_CHANNEL_NO);
	     i++) {
		ret = <devname>_reg_read(desc->drv_dev,
					<DEVNAME>_REG_FIFO_ACCESS,
					&data[i]);
		if (ret)
			return ret;
	}

	ret = <devname>_mode_set(desc->drv_dev, <DEVNAME>_PROGRAM);
	if (ret)
		return ret;
	ret = <devname>_mode_set(desc->drv_dev, <DEVNAME>_STANDBY);
	if (ret)
		return ret;

	sample = data[2 * channel->ch_num] |
		 ((uint32_t)data[2 * channel->ch_num + 1] << 16);

	return snprintf(buf, len, "%"PRIX32"", sample);
}

/**
 * @brief Read offset for a photodiode channel.
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read sampling frequency (device-level attribute).
 */
static int <devname>_iio_read_odr(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc =
		(struct <devname>_iio_desc *)dev;
	uint16_t freq_hz;
	int ret;

	ret = <devname>_adc_fsample_get(desc->drv_dev, &freq_hz);
	if (ret)
		return ret;

	return snprintf(buf, len, "%"PRId16"", freq_hz);
}

/**
 * @brief Write sampling frequency (device-level attribute).
 */
static int <devname>_iio_write_odr(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct <devname>_iio_desc *desc =
		(struct <devname>_iio_desc *)dev;
	uint16_t freq_hz;
	int ret;

	sscanf(buf, "%hu", &freq_hz);

	ret = <devname>_mode_set(desc->drv_dev, <DEVNAME>_PROGRAM);
	if (ret)
		return ret;

	ret = <devname>_adc_fsample_set(desc->drv_dev, freq_hz);
	if (ret)
		return ret;

	ret = <devname>_mode_set(desc->drv_dev, <DEVNAME>_STANDBY);
	if (ret)
		return ret;

	return len;
}

/* ---------- Attribute tables ------------------------------------------- */

static struct iio_attribute <devname>_iio_ch_attrs[] = {
	{ .name = "raw",    .show = <devname>_iio_read_raw },
	{ .name = "offset", .show = <devname>_iio_read_offset },
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute <devname>_iio_dev_attrs[] = {
	{
		.name = "sampling_frequency",
		.show = <devname>_iio_read_odr,
		.store = <devname>_iio_write_odr,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type -------------------------------------------------- */

/**
 * Photodiode channels produce 32-bit unsigned samples (27 real bits
 * from a 32-bit container, stored big-endian from the FIFO).
 */
static struct scan_type <devname>_scan_type = {
	.sign = 'u',
	.realbits = 27,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = true,
};

/* ---------- Channel definitions ---------------------------------------- */

/**
 * Photodiode channels: IIO_INTENSITY, ch_out = false (input), indexed.
 * 4 PD channels per time slot, 2 slots = 8 channels total.
 */
#define <DEVNAME>_IIO_CHANNEL(nm, idx) \
	{ \
		.name = nm, \
		.ch_type = IIO_INTENSITY, \
		.channel = (idx), \
		.scan_type = &<devname>_scan_type, \
		.scan_index = (idx), \
		.attributes = <devname>_iio_ch_attrs, \
		.ch_out = false, \
		.indexed = true, \
	}

static struct iio_channel <devname>_iio_channels[] = {
	<DEVNAME>_IIO_CHANNEL("pd0", 0),
	<DEVNAME>_IIO_CHANNEL("pd1", 1),
	<DEVNAME>_IIO_CHANNEL("pd2", 2),
	<DEVNAME>_IIO_CHANNEL("pd3", 3),
	<DEVNAME>_IIO_CHANNEL("pd4", 4),
	<DEVNAME>_IIO_CHANNEL("pd5", 5),
	<DEVNAME>_IIO_CHANNEL("pd6", 6),
	<DEVNAME>_IIO_CHANNEL("pd7", 7),
};

/* ---------- Buffered read callbacks ------------------------------------ */

/**
 * @brief Enable channels and enter normal sampling mode.
 */
static int <devname>_iio_pre_enable(void *dev, uint32_t mask)
{
	struct <devname>_iio_desc *desc =
		(struct <devname>_iio_desc *)dev;

	desc->active_channels = mask;

	return <devname>_mode_set(desc->drv_dev, <DEVNAME>_NORMAL);
}

/**
 * @brief Disable sampling and return to standby.
 */
static int <devname>_iio_post_disable(void *dev)
{
	struct <devname>_iio_desc *desc =
		(struct <devname>_iio_desc *)dev;

	desc->active_channels = 0;

	return <devname>_mode_set(desc->drv_dev, <DEVNAME>_STANDBY);
}

/**
 * @brief Read photodiode samples into an IIO buffer.
 *
 * Reads complete FIFO sample sets and packs only the enabled
 * channels into the output buffer.
 */
static int <devname>_iio_read_samples(void *dev, int32_t *buf,
				      uint32_t nb_samples)
{
	struct <devname>_iio_desc *desc =
		(struct <devname>_iio_desc *)dev;
	uint16_t data[<DEVNAME>_WORDS_PER_SAMPLE * <DEVNAME>_CHANNEL_NO];
	uint32_t i, ch, buf_idx = 0;
	int ret;

	for (i = 0; i < nb_samples; i++) {
		/* Wait for FIFO data ready ... */

		for (ch = 0;
		     ch < (<DEVNAME>_WORDS_PER_SAMPLE * <DEVNAME>_CHANNEL_NO);
		     ch++) {
			ret = <devname>_reg_read(desc->drv_dev,
						<DEVNAME>_REG_FIFO_ACCESS,
						&data[ch]);
			if (ret)
				return ret;
		}

		for (ch = 0; ch < <DEVNAME>_CHANNEL_NO; ch++) {
			if (!(desc->active_channels & NO_OS_BIT(ch)))
				continue;
			buf[buf_idx] = data[ch * 2] |
				       ((uint32_t)data[ch * 2 + 1] << 16);
			buf_idx++;
		}
	}

	return 0;
}

/* ---------- IIO device structure --------------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels),
	.attributes = <devname>_iio_dev_attrs,
	.pre_enable = <devname>_iio_pre_enable,
	.post_disable = <devname>_iio_post_disable,
	.read_dev = (int32_t (*)())<devname>_iio_read_samples,
	.debug_reg_read = (int32_t (*)())<devname>_reg_read,
	.debug_reg_write = (int32_t (*)())<devname>_reg_write,
};

/* ---------- Init / Remove ---------------------------------------------- */

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

	ret = <devname>_init(&d->drv_dev, &init_param->drv_init_param);
	if (ret)
		goto free_desc;

	/* Configure both slots for 32-bit, 4-channel FIFO mode */
	struct <devname>_slot_config slota = {
		.slot_id = <DEVNAME>_SLOTA,
		.slot_en = true,
		.fifo_mode = <DEVNAME>_32BIT_4CHAN,
	};
	struct <devname>_slot_config slotb = {
		.slot_id = <DEVNAME>_SLOTB,
		.slot_en = true,
		.fifo_mode = <DEVNAME>_32BIT_4CHAN,
	};

	ret = <devname>_mode_set(d->drv_dev, <DEVNAME>_PROGRAM);
	if (ret)
		goto free_drv;

	ret = <devname>_slot_setup(d->drv_dev, &slota);
	if (ret)
		goto free_drv;

	ret = <devname>_slot_setup(d->drv_dev, &slotb);
	if (ret)
		goto free_drv;

	d->iio_dev = &<devname>_iio_dev;

	*desc = d;

	return 0;

free_drv:
	<devname>_remove(d->drv_dev);
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

	ret = <devname>_remove(desc->drv_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
```

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
	$(INCLUDE)/no_os_i2c.h       \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_spi.h       \
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
	$(DRIVERS)/api/no_os_i2c.c  \
	$(DRIVERS)/api/no_os_spi.c  \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_util.c  \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/photo-electronic/<devname>/<devname>.h
SRCS += $(DRIVERS)/photo-electronic/<devname>/<devname>.c
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
INCS += $(DRIVERS)/photo-electronic/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/photo-electronic/<devname>/iio_<devname>.c

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
	.id = ID_<DEVNAME>,
	.phy_opt = <DEVNAME>_SPI,
	.phy_init = {
		.spi_phy = {
			.device_id = SPI_DEVICE_ID,
			.max_speed_hz = SPI_MAX_SPEED,
			.chip_select = SPI_CS,
			.mode = NO_OS_SPI_MODE_0,
			.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
			.platform_ops = SPI_OPS,
			.extra = SPI_EXTRA,
		},
	},
	.gpio0_init = {
		.number = GPIO0_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio1_init = {
		.number = GPIO1_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
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
	uint32_t pd_data[4];
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	/* Enter program mode to configure slot A */
	ret = <devname>_mode_set(dev, <DEVNAME>_PROGRAM);
	if (ret)
		goto free_dev;

	struct <devname>_slot_config slota = {
		.slot_id = <DEVNAME>_SLOTA,
		.slot_en = true,
		.fifo_mode = <DEVNAME>_32BIT_4CHAN,
	};
	ret = <devname>_slot_setup(dev, &slota);
	if (ret)
		goto free_dev;

	while (1) {
		/* Enter normal mode to sample */
		ret = <devname>_mode_set(dev, <DEVNAME>_NORMAL);
		if (ret)
			goto free_dev;

		no_os_mdelay(100);

		ret = <devname>_read_data(dev, <DEVNAME>_SLOTA, pd_data, 4);
		if (ret)
			goto free_dev;

		/* Return to standby */
		ret = <devname>_mode_set(dev, <DEVNAME>_STANDBY);
		if (ret)
			goto free_dev;

		pr_info("PD0: %lu  PD1: %lu  PD2: %lu  PD3: %lu\r\n",
			(unsigned long)pd_data[0],
			(unsigned long)pd_data[1],
			(unsigned long)pd_data[2],
			(unsigned long)pd_data[3]);

		no_os_mdelay(1000);
	}

free_dev:
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

	<devname>_iio_ip.drv_init_param = <devname>_ip;
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

### 7.11 `projects/<project_name>/src/platform/platform_includes.h`

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

#ifdef IIO_SUPPORT
#include "iio_app.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
```

### 7.12 `projects/<project_name>/src/platform/maxim/main.c`

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

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

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

#ifdef IIO_EXAMPLE
	return iio_example_main();
#endif

#if (IIO_EXAMPLE + BASIC_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

#ifdef BASIC_EXAMPLE
error:
	no_os_uart_remove(uart);
#endif
	return 0;
}
```

### 7.13 `projects/<project_name>/src/platform/maxim/parameters.h`

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
#include "maxim_i2c.h"
#include "maxim_gpio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID	0
#endif

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

#define I2C_DEVICE_ID	0
#define I2C_OPS		&max_i2c_ops
#define I2C_EXTRA	&max_i2c_extra

#define GPIO0_PIN	0
#define GPIO1_PIN	1
#define GPIO_OPS	&max_gpio_ops
#define GPIO_EXTRA	&max_gpio_extra

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;
extern struct max_i2c_init_param max_i2c_extra;
extern struct max_gpio_init_param max_gpio_extra;

#endif /* __PARAMETERS_H__ */
```

### 7.14 `projects/<project_name>/src/platform/maxim/parameters.c`

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

struct max_i2c_init_param max_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param max_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
```

### 7.15 `projects/<project_name>/src/platform/maxim/platform_src.mk`

```makefile
INCS +=	$(PLATFORM_DRIVERS)/maxim_gpio.h      \
	$(PLATFORM_DRIVERS)/maxim_spi.h       \
	$(PLATFORM_DRIVERS)/maxim_i2c.h       \
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h       \
	$(PLATFORM_DRIVERS)/maxim_irq.h       \
	$(PLATFORM_DRIVERS)/maxim_uart.h      \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
	$(PLATFORM_DRIVERS)/maxim_gpio.c      \
	$(PLATFORM_DRIVERS)/maxim_spi.c       \
	$(PLATFORM_DRIVERS)/maxim_i2c.c       \
	$(PLATFORM_DRIVERS)/../common/maxim_dma.c       \
	$(PLATFORM_DRIVERS)/maxim_irq.c       \
	$(PLATFORM_DRIVERS)/maxim_uart.c      \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c
```

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

The <DEVNAME> is a multimodal optical sensor front-end with integrated
photodiodes and LED drivers. It supports dual time-slot operation with
up to 4 photodiode channels per slot, configurable LED drive current,
and a deep FIFO for autonomous data collection. Communication is
available via SPI or I2C. <Add 2-3 more sentences from the datasheet
covering key specs and features.>

Applications
------------

* Smoke / particle detection
* Proximity sensing
* Optical heart rate monitoring
* Gesture recognition

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device descriptor,
initialises the communication interface (SPI or I2C based on the
``phy_opt`` parameter), verifies the device ID, performs a software reset,
and configures the GPIO pins as inputs for interrupt / data ready
signalling. The function uses goto-based cleanup to release resources
on failure.

Time Slot Configuration
~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_slot_setup`` function enables or disables a time slot
(A or B) and configures how it writes data to the FIFO (16-bit sum,
32-bit sum, 4-channel 16-bit, or 4-channel 32-bit). Time slots allow
multiplexed measurements with different LED and photodiode combinations.

Operating Modes
~~~~~~~~~~~~~~~

The ``<devname>_mode_set`` function transitions the device between
standby (low power), program (register configuration), and normal
(active sampling) modes. Configuration register changes must be made
in program mode.

Sampling / Data Readout
~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_read_data`` function reads raw photodiode sample data
from the FIFO. Each 32-bit sample represents the integrated photocurrent
from one photodiode channel during a time slot. Multiple channels can
be read in a single call.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .id = ID_<DEVNAME>,
           .phy_opt = <DEVNAME>_SPI,
           .phy_init = {
               .spi_phy = { /* platform-specific SPI params */ },
           },
           .gpio0_init = { /* interrupt GPIO params */ },
           .gpio1_init = { /* data ready GPIO params */ },
       };

       struct <devname>_dev *dev;
       uint32_t pd_data[4];
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_mode_set(dev, <DEVNAME>_NORMAL);
       if (ret)
           goto cleanup;

       /* Wait for data, then read 4 channels from slot A */
       ret = <devname>_read_data(dev, <DEVNAME>_SLOTA, pd_data, 4);
       if (ret)
           goto cleanup;

       /* pd_data[] contains raw photodiode values */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each photodiode channel as an
``IIO_INTENSITY`` channel with ``raw`` and ``offset`` attributes.
A device-level ``sampling_frequency`` attribute controls the ADC
sample rate. The IIO application can be used for real-time buffered
data acquisition across all 8 photodiode channels.

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

       iio_ip.drv_init_param = <devname>_ip;
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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/photo-electronic/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/photo-electronic/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`,
   `(struct no_os_spi_desc *)dev->phy_desc`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int`, negative = error.
5. **Init signature** -- `int <devname>_init(struct <devname>_dev **dev,
   struct <devname>_init_param *init_param)`.
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
11. **Dual comm interface** -- use a `union` for I2C/SPI init params
    and a `void *phy_desc` with explicit casts for the active descriptor.
12. **Timeslot-based sampling** -- configure slots independently for
    LED source, photodiode selection, and FIFO data format before
    entering normal mode.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> multimodal optical sensor front-end
with dual time-slot photodiode sampling, configurable LED drive,
and SPI/I2C communication.

Signed-off-by: Your Name <your.name@analog.com>
```
