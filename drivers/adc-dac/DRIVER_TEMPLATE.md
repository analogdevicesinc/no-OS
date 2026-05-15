# ADC-DAC Driver Template

Reference driver: `drivers/adc-dac/ad5592r/`

This template covers every file needed to add a new combined ADC/DAC driver
to no-OS. These devices feature configurable pins that can individually
operate as ADC inputs, DAC outputs, or GPIOs. The reference uses a
dual-interface pattern (SPI + I2C) with a shared base layer and
per-interface transport files.

Replace `<devname>` with the part number (e.g., `ad5592r`) and `<DEVNAME>`
with its uppercase form (e.g., `AD5592R`) throughout. When two variants
exist (e.g., SPI = `ad5592r`, I2C = `ad5593r`), use `<devname_base>` /
`<DEVNAME_BASE>` for the shared base layer (e.g., `ad5592r-base`).

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Base files | `<devname>-base.h`, `<devname>-base.c` | `ad5592r-base.h`, `ad5592r-base.c` |
| SPI variant files | `<devname_spi>.h`, `<devname_spi>.c` | `ad5592r.h`, `ad5592r.c` |
| I2C variant files | `<devname_i2c>.h`, `<devname_i2c>.c` | `ad5593r.h`, `ad5593r.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ad559xr.h`, `iio_ad559xr.c` |
| Include guard | `#ifndef <DEVNAME_BASE>_H_` | `#ifndef AD5592R_BASE_H_` |
| Init param struct | `<devname>_init_param` | `ad5592r_init_param` |
| Device struct | `<devname>_dev` | `ad5592r_dev` |
| IIO descriptor | `iio_<devname>_desc` | `iio_ad559xr_desc` |
| Functions | `<devname>_<verb>()` | `ad5592r_read_adc()` |
| Channel mode defines | `CH_MODE_<MODE>` | `CH_MODE_ADC`, `CH_MODE_DAC` |
| Register enum | `<devname>_registers` | `ad5592r_registers` |
| Register masks | `<DEVNAME>_REG_<NAME>` | `AD5592R_REG_CTRL_ADC_RANGE` |

---

## 2. File Checklist

```
drivers/adc-dac/<devname>/
    <devname>-base.h          # Shared base header (structs, enums, base API)
    <devname>-base.c          # Shared base implementation
    <devname_spi>.h           # SPI variant header
    <devname_spi>.c           # SPI variant implementation
    <devname_i2c>.h           # I2C variant header
    <devname_i2c>.c           # I2C variant implementation
    iio_<devname>.h           # IIO header (optional)
    iio_<devname>.c           # IIO implementation (optional)
    README.rst                # Driver documentation

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

doc/sphinx/source/drivers/adc-dac/<devname>.rst   # Sphinx entry
```

---

## 3. Driver Base Header (`<devname>-base.h`)

```c
/***************************************************************************//**
 *   @file   <devname>-base.h
 *   @brief  Header file of <DEVNAME> Base Driver
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
#ifndef __<DEVNAME>_BASE_H__
#define __<DEVNAME>_BASE_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/* ---------------- Channel Mode Defines --------------------------------- */

#define CH_MODE_UNUSED			0
#define CH_MODE_ADC			1
#define CH_MODE_DAC			2
#define CH_MODE_DAC_AND_ADC		3
#define CH_MODE_GPI			4
#define CH_MODE_GPO			5

/* ---------------- Register Map ----------------------------------------- */

/** Register addresses — use an enum for multi-register maps. */
enum <devname>_registers {
	<DEVNAME>_REG_NOOP		= 0x0,
	<DEVNAME>_REG_DAC_READBACK	= 0x1,
	<DEVNAME>_REG_ADC_SEQ		= 0x2,
	<DEVNAME>_REG_CTRL		= 0x3,
	<DEVNAME>_REG_ADC_EN		= 0x4,
	<DEVNAME>_REG_DAC_EN		= 0x5,
	<DEVNAME>_REG_GPIO_OUT_EN	= 0x8,
	<DEVNAME>_REG_GPIO_SET		= 0x9,
	<DEVNAME>_REG_GPIO_IN_EN	= 0xA,
	<DEVNAME>_REG_PD		= 0xB,
	<DEVNAME>_REG_RESET		= 0xF,
};

/* Control register field masks — use NO_OS_BIT() / NO_OS_GENMASK() */
#define <DEVNAME>_REG_CTRL_ADC_RANGE	NO_OS_BIT(5)
#define <DEVNAME>_REG_CTRL_DAC_RANGE	NO_OS_BIT(4)

#define <DEVNAME>_REG_PD_EN_REF		NO_OS_BIT(9)

#define NUM_OF_CHANNELS			8

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_range
 *  @brief ADC/DAC voltage range selection. */
enum <devname>_range {
	ZERO_TO_VREF,
	ZERO_TO_2VREF,
};

/* ---------------- Structures ------------------------------------------- */

struct <devname>_dev;

/**
 * @struct <devname>_rw_ops
 * @brief  Transport-layer function pointers (SPI or I2C).
 *
 * Each interface variant (SPI, I2C) populates this struct so the base
 * layer can call read/write operations without knowing the transport.
 */
struct <devname>_rw_ops {
	int32_t (*write_dac)(struct <devname>_dev *dev, uint8_t chan,
			     uint16_t value);
	int32_t (*read_adc)(struct <devname>_dev *dev, uint8_t chan,
			    uint16_t *value);
	int32_t (*multi_read_adc)(struct <devname>_dev *dev,
				  uint16_t chans, uint16_t *value);
	int32_t (*reg_write)(struct <devname>_dev *dev, uint8_t reg,
			     uint16_t value);
	int32_t (*reg_read)(struct <devname>_dev *dev, uint8_t reg,
			    uint16_t *value);
	int32_t (*gpio_read)(struct <devname>_dev *dev, uint8_t *value);
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters — shared by SPI and I2C variants.
 *
 * Only one of spi_init / i2c_init should be set, depending on the variant.
 */
struct <devname>_init_param {
	/** Use internal reference if true. */
	bool int_ref;
	/** External reference voltage in mV (used when int_ref is false). */
	uint32_t external_vref;
	/** SPI init params (set for SPI variant, NULL otherwise). */
	struct no_os_spi_init_param *spi_init;
	/** I2C init params (set for I2C variant, NULL otherwise). */
	struct no_os_i2c_init_param *i2c_init;
	/** Per-channel mode: CH_MODE_ADC, CH_MODE_DAC, CH_MODE_GPI, etc. */
	uint8_t channel_modes[NUM_OF_CHANNELS];
	/** ADC voltage range selection. */
	enum <devname>_range adc_range;
	/** DAC voltage range selection. */
	enum <devname>_range dac_range;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 *
 * Contains both SPI and I2C descriptors; only one is active
 * depending on which variant was initialised.
 */
struct <devname>_dev {
	/** Transport-layer operations (set during init). */
	const struct <devname>_rw_ops *ops;
	/** I2C descriptor (NULL when using SPI variant). */
	struct no_os_i2c_desc *i2c;
	/** SPI descriptor (NULL when using I2C variant). */
	struct no_os_spi_desc *spi;
	/** Number of configurable channels. */
	uint8_t num_channels;
	/** Cached DAC values per channel. */
	uint16_t cached_dac[NUM_OF_CHANNELS];
	/** Per-channel mode configuration. */
	uint8_t channel_modes[NUM_OF_CHANNELS];
	/** ADC voltage range. */
	enum <devname>_range adc_range;
	/** DAC voltage range. */
	enum <devname>_range dac_range;
	/** Internal reference enabled. */
	bool int_ref;
	/** External reference voltage in mV. */
	uint32_t external_vref;
};

/* ---------------- Base Function Prototypes ----------------------------- */

/** @brief Write a register via transport ops. */
int32_t <devname>_base_reg_write(struct <devname>_dev *dev, uint8_t reg,
				 uint16_t value);

/** @brief Read a register via transport ops. */
int32_t <devname>_base_reg_read(struct <devname>_dev *dev, uint8_t reg,
				uint16_t *value);

/** @brief Software-reset the device. */
int32_t <devname>_software_reset(struct <devname>_dev *dev);

/** @brief Configure channel modes (ADC/DAC/GPIO). */
int32_t <devname>_set_channel_modes(struct <devname>_dev *dev);

/** @brief Set ADC voltage range. */
int32_t <devname>_set_adc_range(struct <devname>_dev *dev,
				enum <devname>_range adc_range);

/** @brief Set DAC voltage range. */
int32_t <devname>_set_dac_range(struct <devname>_dev *dev,
				enum <devname>_range dac_range);

/** @brief Enable/disable internal reference. */
int32_t <devname>_set_int_ref(struct <devname>_dev *dev, bool enable);

/** @brief Set external reference voltage. */
int32_t <devname>_set_ext_ref(struct <devname>_dev *dev, uint32_t vref_mv);

/** @brief Get the active reference voltage in mV. */
int32_t <devname>_get_ref(struct <devname>_dev *dev, uint32_t *vref_mv);

#endif /* __<DEVNAME>_BASE_H__ */
```

---

## 4. SPI Variant Source (`<devname_spi>.c`)

```c
/***************************************************************************//**
 *   @file   <devname_spi>.c
 *   @brief  Implementation of <DEVNAME_SPI> Driver (SPI variant)
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * ... (same BSD-3-Clause block as header) ...
 *******************************************************************************/

#include <errno.h>
#include "no_os_error.h"
#include "<devname>-base.h"
#include "<devname_spi>.h"

const struct <devname>_rw_ops <devname_spi>_rw_ops = {
	.write_dac = <devname_spi>_write_dac,
	.read_adc = <devname_spi>_read_adc,
	.multi_read_adc = <devname_spi>_multi_read_adc,
	.reg_write = <devname_spi>_reg_write,
	.reg_read = <devname_spi>_reg_read,
	.gpio_read = <devname_spi>_gpio_read,
};

/**
 * @brief Write a DAC channel value over SPI.
 * @param dev  - Device descriptor.
 * @param chan - Channel index.
 * @param value - DAC code to write.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname_spi>_write_dac(struct <devname>_dev *dev, uint8_t chan,
				uint16_t value)
{
	/* Build SPI frame: DAC write bit | channel | data */
	/* ... transport-specific implementation ... */
	dev->cached_dac[chan] = value;

	return 0;
}

/**
 * @brief Read an ADC channel value over SPI.
 * @param dev   - Device descriptor.
 * @param chan  - Channel index.
 * @param value - Pointer to store ADC result.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname_spi>_read_adc(struct <devname>_dev *dev, uint8_t chan,
			       uint16_t *value)
{
	/* Issue ADC sequence command, then NOP-read the result */
	/* ... transport-specific implementation ... */

	return 0;
}

/**
 * @brief Read multiple ADC channels over SPI.
 * @param dev    - Device descriptor.
 * @param chans  - Bitmask of channels to read.
 * @param values - Array to store ADC results.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname_spi>_multi_read_adc(struct <devname>_dev *dev,
				     uint16_t chans, uint16_t *values)
{
	/* ... transport-specific implementation ... */

	return 0;
}

/**
 * @brief Write a register over SPI.
 * @param dev  - Device descriptor.
 * @param reg  - Register address.
 * @param value - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname_spi>_reg_write(struct <devname>_dev *dev, uint8_t reg,
				uint16_t value)
{
	if (!dev)
		return -ENODEV;

	/* SPI frame: (reg << 11) | value, byte-swapped */
	/* ... transport-specific implementation ... */

	return no_os_spi_write_and_read(dev->spi, /* buf */, /* len */);
}

/**
 * @brief Read a register over SPI.
 * @param dev  - Device descriptor.
 * @param reg  - Register address.
 * @param value - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname_spi>_reg_read(struct <devname>_dev *dev, uint8_t reg,
			       uint16_t *value)
{
	int32_t ret;

	if (!dev)
		return -ENODEV;

	/* Write readback command, then NOP-read */
	/* ... transport-specific implementation ... */

	return 0;
}

/**
 * @brief Read GPIO values over SPI.
 * @param dev   - Device descriptor.
 * @param value - Pointer to store GPIO state.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname_spi>_gpio_read(struct <devname>_dev *dev, uint8_t *value)
{
	/* ... transport-specific implementation ... */

	return 0;
}

/**
 * @brief Initialise the device (SPI variant).
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname_spi>_init(struct <devname>_dev **device,
			   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *dev;
	uint8_t i;
	int32_t ret;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi, init_param->spi_init);
	if (ret)
		goto err_free_dev;

	dev->ops = &<devname_spi>_rw_ops;
	dev->num_channels = NUM_OF_CHANNELS;

	ret = <devname>_software_reset(dev);
	if (ret)
		goto err_remove_spi;

	for (i = 0; i < NUM_OF_CHANNELS; i++)
		dev->channel_modes[i] = init_param->channel_modes[i];

	ret = <devname>_set_adc_range(dev, init_param->adc_range);
	if (ret)
		goto err_remove_spi;

	ret = <devname>_set_dac_range(dev, init_param->dac_range);
	if (ret)
		goto err_remove_spi;

	ret = <devname>_set_channel_modes(dev);
	if (ret)
		goto err_remove_spi;

	if (init_param->int_ref)
		ret = <devname>_set_int_ref(dev, true);
	else
		ret = <devname>_set_ext_ref(dev, init_param->external_vref);
	if (ret)
		goto err_remove_spi;

	*device = dev;

	return 0;

err_remove_spi:
	no_os_spi_remove(dev->spi);
err_free_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free resources allocated by <devname_spi>_init().
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname_spi>_remove(struct <devname>_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = <devname>_software_reset(dev);
	if (ret)
		return ret;

	ret = no_os_spi_remove(dev->spi);
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

#include "iio_types.h"
#include "<devname_spi>.h"
#include "<devname_i2c>.h"

#define <DEVNAME>_MAX_AIO_PORT		NUM_OF_CHANNELS
#define <DEVNAME>_ADC_RESOLUTION	12
#define <DEVNAME>_STORAGE_BITS		16
#define <DEVNAME>_STORAGE_BYTES		(<DEVNAME>_STORAGE_BITS / 8)

/**
 * @struct <devname>_aio_port
 * @brief  Per-channel IIO port descriptor.
 *
 * Tracks the channel number, mode (ADC/DAC/GPIO), and parent device
 * so IIO callbacks can dispatch to the correct transport ops.
 */
struct <devname>_aio_port {
	/** Channel name string. */
	char name[24];
	/** Parent device descriptor. */
	struct <devname>_dev *dev;
	/** Physical channel number on the device. */
	uint8_t number;
	/** Channel mode: CH_MODE_ADC, CH_MODE_DAC, etc. */
	uint8_t mode;
	/** Back-pointer to the IIO channel (for scan_index). */
	struct iio_channel *chptr;
};

/**
 * @struct iio_<devname>_desc
 * @brief  IIO-specific device descriptor.
 */
struct iio_<devname>_desc {
	/** List of configured AIO ports. */
	struct no_os_list_desc *aio_ports;
	/** Iterator for the port list. */
	struct no_os_iterator *aio_ports_iter;
	/** Number of configured ports. */
	size_t port_count;
	/** IIO device structure. */
	struct iio_device iiodev;
	/** Dynamically allocated IIO channel array. */
	struct iio_channel *channels;
};

/** @brief Initialise AIO (ADC + DAC) IIO channels for one device. */
int iio_<devname>_aio_init(struct iio_<devname>_desc **desc,
			   struct <devname>_dev *dev);

/** @brief Free IIO descriptor resources. */
int iio_<devname>_remove(struct iio_<devname>_desc *desc);

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

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "no_os_alloc.h"
#include "no_os_list.h"
#include "no_os_util.h"
#include "iio_<devname>.h"

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_aio_scan_type = {
	.sign = 'u',
	.realbits = <DEVNAME>_ADC_RESOLUTION,
	.storagebits = <DEVNAME>_STORAGE_BITS,
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Attribute callbacks ------------------------------------ */

/**
 * @brief Read raw value for an ADC or DAC channel.
 *
 * For ADC channels, triggers a conversion and returns the result.
 * For DAC channels, returns the cached output value.
 */
static int <devname>_iio_show(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	struct <devname>_aio_port *port;
	struct <devname>_dev *dev;
	uint16_t value;
	int ret;

	port = (struct <devname>_aio_port *)channel->address;
	dev = port->dev;

	switch (priv) {
	case 0: /* raw */
		if (port->mode == CH_MODE_DAC) {
			value = dev->cached_dac[port->number];
		} else {
			ret = dev->ops->read_adc(dev, port->number,
						 &value);
			if (ret)
				return ret;
		}

		return sprintf(buf, "%d", value);

	case 1: /* scale — Vref / 2^resolution in mV */
		/* Compute from dev->adc_range or dev->dac_range */
		return sprintf(buf, "%d", 0);

	case 2: /* offset */
		return sprintf(buf, "%d", 0);
	}

	return 0;
}

/**
 * @brief Write raw value for a DAC channel.
 */
static int <devname>_iio_store(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct <devname>_aio_port *port;
	uint16_t dac_value;
	int ret;

	port = (struct <devname>_aio_port *)channel->address;

	switch (priv) {
	case 0: /* raw */
		if (port->mode == CH_MODE_DAC) {
			dac_value = no_os_str_to_uint32(buf);
			ret = port->dev->ops->write_dac(port->dev,
							port->number,
							dac_value);
			if (ret)
				return ret;
		}
		break;
	}

	return len;
}

/* ---------- Attribute tables --------------------------------------- */

/**
 * ADC channel attributes: raw (read-only), scale, offset.
 */
static struct iio_attribute <devname>_iio_adc_attrs[] = {
	{ .name = "raw",    .priv = 0, .show = <devname>_iio_show },
	{ .name = "scale",  .priv = 1, .show = <devname>_iio_show },
	{ .name = "offset", .priv = 2, .show = <devname>_iio_show },
	END_ATTRIBUTES_ARRAY
};

/**
 * DAC channel attributes: raw (read/write), scale, offset.
 */
static struct iio_attribute <devname>_iio_dac_attrs[] = {
	{ .name = "raw",    .priv = 0, .show = <devname>_iio_show,
					.store = <devname>_iio_store },
	{ .name = "scale",  .priv = 1, .show = <devname>_iio_show },
	{ .name = "offset", .priv = 2, .show = <devname>_iio_show },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel population ------------------------------------- */

/**
 * @brief Populate IIO channel array from the configured port list.
 *
 * ADC channels are created with ch_out = false (input).
 * DAC channels are created with ch_out = true (output).
 * Both use IIO_VOLTAGE channel type.
 */
static int <devname>_populate_channels(struct iio_<devname>_desc *desc)
{
	uint32_t port_count;
	struct iio_channel *ch;
	struct <devname>_aio_port *port;
	size_t i;
	int ret;

	ret = no_os_list_get_size(desc->aio_ports, &port_count);
	if (ret)
		return ret;

	ch = (struct iio_channel *)no_os_calloc(port_count,
						sizeof(struct iio_channel));
	if (!ch)
		return -ENOMEM;

	desc->channels = ch;

	for (i = 0; i < port_count; i++) {
		no_os_list_read_idx(desc->aio_ports, (void **)&port, i);

		ch->ch_type = IIO_VOLTAGE;
		ch->channel = port->number;
		ch->address = (unsigned long)port;
		ch->scan_index = i;
		ch->indexed = true;
		ch->scan_type = &<devname>_aio_scan_type;
		port->chptr = ch;

		if (port->mode == CH_MODE_ADC) {
			ch->ch_out = false;
			ch->attributes = <devname>_iio_adc_attrs;
		} else if (port->mode == CH_MODE_DAC) {
			ch->ch_out = true;
			ch->attributes = <devname>_iio_dac_attrs;
		}

		ch++;
	}

	return (int)i;
}

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise IIO descriptor for ADC + DAC channels.
 *
 * Iterates over the device channel_modes array and creates an IIO
 * channel for each pin configured as ADC, DAC, or DAC_AND_ADC.
 * Pins in DAC_AND_ADC mode get both an input (ADC) and output (DAC)
 * IIO channel.
 *
 * @param desc - Pointer to IIO descriptor pointer (allocated here).
 * @param dev  - Already-initialised device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int iio_<devname>_aio_init(struct iio_<devname>_desc **desc,
			   struct <devname>_dev *dev)
{
	struct iio_<devname>_desc *d;
	struct <devname>_aio_port *port;
	int ret;
	size_t i;

	if (!desc || !dev)
		return -EINVAL;

	d = (struct iio_<devname>_desc *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = no_os_list_init(&d->aio_ports, NO_OS_LIST_DEFAULT, NULL);
	if (ret)
		goto err_free_desc;

	ret = no_os_iterator_init(&d->aio_ports_iter, d->aio_ports, 0);
	if (ret)
		goto err_free_list;

	/* Create port entries per channel mode */
	for (i = 0; i < NUM_OF_CHANNELS; i++) {
		if (dev->channel_modes[i] == CH_MODE_ADC ||
		    dev->channel_modes[i] == CH_MODE_DAC_AND_ADC) {
			port = (struct <devname>_aio_port *)no_os_calloc(
				1, sizeof(*port));
			if (!port) {
				ret = -ENOMEM;
				goto err_free_ports;
			}
			snprintf(port->name, sizeof(port->name),
				 "adc%zu", i);
			port->mode = CH_MODE_ADC;
			port->number = i;
			port->dev = dev;
			ret = no_os_list_add_last(d->aio_ports, port);
			if (ret) {
				no_os_free(port);
				goto err_free_ports;
			}
		}

		if (dev->channel_modes[i] == CH_MODE_DAC ||
		    dev->channel_modes[i] == CH_MODE_DAC_AND_ADC) {
			port = (struct <devname>_aio_port *)no_os_calloc(
				1, sizeof(*port));
			if (!port) {
				ret = -ENOMEM;
				goto err_free_ports;
			}
			snprintf(port->name, sizeof(port->name),
				 "dac%zu", i);
			port->mode = CH_MODE_DAC;
			port->number = i;
			port->dev = dev;
			ret = no_os_list_add_last(d->aio_ports, port);
			if (ret) {
				no_os_free(port);
				goto err_free_ports;
			}
		}
	}

	ret = <devname>_populate_channels(d);
	if (ret < 1)
		goto err_free_ports;

	d->port_count = ret;
	d->iiodev.num_ch = d->port_count;
	d->iiodev.channels = d->channels;

	*desc = d;

	return 0;

err_free_ports:
	/* Port entries freed when list is removed */
err_free_list:
	no_os_list_remove(d->aio_ports);
	no_os_iterator_remove(d->aio_ports_iter);
err_free_desc:
	no_os_free(d);

	return ret;
}

/**
 * @brief Free IIO descriptor resources.
 * @param desc - IIO descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int iio_<devname>_remove(struct iio_<devname>_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (desc->channels)
		no_os_free(desc->channels);

	no_os_list_remove(desc->aio_ports);
	no_os_iterator_remove(desc->aio_ports_iter);
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

INCS += $(DRIVERS)/adc-dac/<devname>/<devname>-base.h \
	$(DRIVERS)/adc-dac/<devname>/<devname_spi>.h
SRCS += $(DRIVERS)/adc-dac/<devname>/<devname>-base.c \
	$(DRIVERS)/adc-dac/<devname>/<devname_spi>.c
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
INCS += $(DRIVERS)/adc-dac/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/adc-dac/<devname>/iio_<devname>.c

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
#include "<devname>-base.h"
#include "<devname_spi>.h"
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

static const struct no_os_spi_init_param <devname>_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_MAX_SPEED,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.parent = NULL,
};

struct <devname>_init_param <devname>_ip = {
	.int_ref = true,
	.spi_init = &<devname>_spi_ip,
	.i2c_init = NULL,
	.channel_modes = {
		CH_MODE_ADC,    /* Channel 0: ADC input */
		CH_MODE_ADC,    /* Channel 1: ADC input */
		CH_MODE_DAC,    /* Channel 2: DAC output */
		CH_MODE_DAC,    /* Channel 3: DAC output */
		CH_MODE_UNUSED, /* Channel 4: unused */
		CH_MODE_UNUSED, /* Channel 5: unused */
		CH_MODE_UNUSED, /* Channel 6: unused */
		CH_MODE_UNUSED, /* Channel 7: unused */
	},
	.adc_range = ZERO_TO_VREF,
	.dac_range = ZERO_TO_VREF,
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
#include "<devname>-base.h"
#include "<devname_spi>.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * Initialises the device, reads ADC channels configured as inputs, and
 * writes a test value to DAC channels configured as outputs.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct <devname>_dev *dev;
	uint16_t adc_data;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname_spi>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	while (1) {
		/* Read ADC channel 0 */
		ret = dev->ops->read_adc(dev, 0, &adc_data);
		if (ret)
			goto free_dev;

		pr_info("ADC ch0 raw: %u\r\n", adc_data);

		/* Write DAC channel 2 */
		ret = dev->ops->write_dac(dev, 2, 2048);
		if (ret)
			goto free_dev;

		no_os_mdelay(1000);
	}

free_dev:
	<devname_spi>_remove(dev);
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
#include "iio_<devname>.h"
#include "<devname_spi>.h"
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
	struct <devname>_dev *dev;
	struct iio_<devname>_desc *iio_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	/* Initialise the core driver (SPI variant) */
	ret = <devname_spi>_init(&dev, &<devname>_ip);
	if (ret)
		return ret;

	/* Initialise IIO ADC+DAC channels from the device config */
	ret = iio_<devname>_aio_init(&iio_desc, dev);
	if (ret)
		goto free_dev;

	struct iio_app_device iio_devices[] = {
		{
			.name = "<devname>",
			.dev = iio_desc,
			.dev_descriptor = &iio_desc->iiodev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto free_iio;

	ret = iio_app_run(app);
	if (ret)
		pr_info("Error: iio_app_run: %d\r\n", ret);

	iio_app_remove(app);
free_iio:
	iio_<devname>_remove(iio_desc);
free_dev:
	<devname_spi>_remove(dev);

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
#define I2C_MAX_SPEED	100000
#define I2C_OPS		&max_i2c_ops
#define I2C_EXTRA	&max_i2c_extra

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;
extern struct max_i2c_init_param max_i2c_extra;

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

* :adi:`<DEVNAME_SPI>`
* :adi:`<DEVNAME_I2C>`

Overview
--------

The <DEVNAME> is a <resolution>-bit, <channels>-channel configurable
ADC/DAC with an SPI-compatible serial interface (<DEVNAME_SPI>) or
I2C interface (<DEVNAME_I2C>). Each of the <channels> I/O pins can
be independently configured as an ADC input, DAC output, digital
GPIO, or left unused. The device supports both internal and external
voltage references. <Add 2-3 more sentences from the datasheet
covering key specs and features.>

Applications
------------

* Industrial process control
* Programmable logic controllers
* Multi-function data acquisition
* Building automation

Device Configuration
--------------------

Driver Architecture
~~~~~~~~~~~~~~~~~~~

The driver uses a split architecture with a shared base layer
(``<devname>-base.c``) and per-interface transport files
(``<devname_spi>.c`` for SPI, ``<devname_i2c>.c`` for I2C). The
base layer implements common register operations, channel mode
configuration, and reference management. Transport-specific
read/write operations are provided through a function pointer
struct (``<devname>_rw_ops``).

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname_spi>_init`` (or ``<devname_i2c>_init``) function
allocates memory for the device descriptor, initialises the
communication interface, performs a software reset, configures
channel modes, sets ADC/DAC voltage ranges, and selects the
reference source. The function uses goto-based cleanup to release
resources on failure.

Channel Configuration
~~~~~~~~~~~~~~~~~~~~~

Each channel can be independently configured via the
``channel_modes`` array in the init params structure. Supported
modes include ADC input, DAC output, combined DAC+ADC, GPI, GPO,
and unused. The ``<devname>_set_channel_modes`` function programs
all channel mode registers in a single call.

ADC / DAC Operations
~~~~~~~~~~~~~~~~~~~~

ADC readings are performed via ``ops->read_adc()`` which triggers
a conversion on the specified channel and returns the raw digital
code. Multi-channel reads are supported via ``ops->multi_read_adc()``.

DAC writes are performed via ``ops->write_dac()`` which loads a new
output code into the specified channel. Written values are cached
in the device descriptor for readback.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>-base.h"
   #include "<devname_spi>.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .int_ref = true,
           .spi_init = &spi_ip,    /* platform-specific SPI params */
           .i2c_init = NULL,
           .channel_modes = {
               CH_MODE_ADC,    /* ch0: ADC */
               CH_MODE_DAC,    /* ch1: DAC */
               CH_MODE_UNUSED, /* ch2-7: unused */
           },
           .adc_range = ZERO_TO_VREF,
           .dac_range = ZERO_TO_VREF,
       };

       struct <devname>_dev *dev;
       uint16_t adc_val;
       int ret;

       ret = <devname_spi>_init(&dev, &ip);
       if (ret)
           return ret;

       /* Read ADC channel 0 */
       ret = dev->ops->read_adc(dev, 0, &adc_val);
       if (ret)
           goto cleanup;

       /* Write DAC channel 1 */
       ret = dev->ops->write_dac(dev, 1, 2048);
       if (ret)
           goto cleanup;

   cleanup:
       <devname_spi>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each configured pin as an
``IIO_VOLTAGE`` channel. ADC channels appear as input channels
(``ch_out = false``) with ``raw``, ``scale``, and ``offset``
attributes. DAC channels appear as output channels
(``ch_out = true``) with the same attributes plus a writable
``raw`` attribute. Pins configured in ``CH_MODE_DAC_AND_ADC``
mode produce both an input and an output IIO channel for the
same physical pin.

IIO Initialization Example
---------------------------

.. code-block:: C

   int iio_example_main()
   {
       int ret;
       struct <devname>_dev *dev;
       struct iio_<devname>_desc *iio_desc;
       struct iio_app_desc *app;
       struct iio_app_init_param app_init_param = {0};

       ret = <devname_spi>_init(&dev, &<devname>_ip);
       if (ret)
           return ret;

       ret = iio_<devname>_aio_init(&iio_desc, dev);
       if (ret)
           goto free_dev;

       struct iio_app_device iio_devices[] = {
           {
               .name = "<devname>",
               .dev = iio_desc,
               .dev_descriptor = &iio_desc->iiodev,
           },
       };

       app_init_param.devices = iio_devices;
       app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
       app_init_param.uart_init_params = uip;

       ret = iio_app_init(&app, app_init_param);
       if (ret)
           goto free_iio;

       ret = iio_app_run(app);

       iio_app_remove(app);
   free_iio:
       iio_<devname>_remove(iio_desc);
   free_dev:
       <devname_spi>_remove(dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/adc-dac/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/adc-dac/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int` (or `int32_t`),
   negative = error.
5. **Dual-interface pattern** -- shared base header defines structs,
   enums, and base API; per-variant files implement transport ops and
   init/remove.
6. **Transport ops** -- use a `<devname>_rw_ops` function pointer
   struct so the base layer and IIO layer are transport-agnostic.
7. **Init signature** -- `int <devname_spi>_init(struct <devname>_dev
   **dev, struct <devname>_init_param *init_param)`.
8. **Allocation** -- use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
9. **Goto cleanup** -- in `_init()`, use goto labels to free in reverse
   allocation order on error.
10. **Register macros** -- `NO_OS_BIT()` for single bits,
    `NO_OS_GENMASK(hi, lo)` for fields; `no_os_field_prep()` /
    `no_os_field_get()` for field access.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME_SPI> (SPI) and <DEVNAME_I2C> (I2C)
<resolution>-bit, <channels>-channel configurable ADC/DAC with
per-pin mode selection (ADC, DAC, GPIO).

Signed-off-by: Your Name <your.name@analog.com>
```
