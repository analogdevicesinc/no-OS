# ECG Driver Template

Reference driver: `drivers/ecg/adas1000/`

This template covers every file needed to add a new ECG/biopotential driver to no-OS.
Replace `<devname>` with the part number (e.g., `adas1000`) and `<DEVNAME>`
with its uppercase form (e.g., `ADAS1000`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adas1000.h`, `adas1000.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_adas1000.h`, `iio_adas1000.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADAS1000_H__` |
| Init param struct | `<devname>_init_param` | `adas1000_init_param` |
| Device struct | `<devname>_dev` | `adas1000_dev` |
| IIO descriptor | `<devname>_iio_desc` | `adas1000_iio_desc` |
| Functions | `<devname>_<verb>()` | `adas1000_init()` |
| Static helpers | `_<devname>_<verb>()` | `_adas1000_build_frame()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADAS1000` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `ADAS1000_FRMCTL_FRMRATE_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADAS1000_LEAD_LA` |

---

## 2. File Checklist

```
drivers/ecg/<devname>/
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

doc/sphinx/source/drivers/ecg/<devname>.rst   # Sphinx entry
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

/* ---------------- Register Map / Frame Field Masks ----------------- */

/** Register and frame field masks — use NO_OS_BIT() for single bits,
 *  NO_OS_GENMASK(hi, lo) for multi-bit fields.
 *  ECG front-ends use 32-bit SPI frames with address/data/CRC fields. */
#define <DEVNAME>_REG_NOP		0x00
#define <DEVNAME>_REG_ECGCTL		0x01
#define <DEVNAME>_REG_FRMCTL		0x02
#define <DEVNAME>_REG_FILTCTL		0x03
#define <DEVNAME>_REG_LOFFCTL		0x04

#define <DEVNAME>_ECGCTL_LAEN_MSK	NO_OS_BIT(0)
#define <DEVNAME>_ECGCTL_LLEN_MSK	NO_OS_BIT(1)
#define <DEVNAME>_ECGCTL_RAEN_MSK	NO_OS_BIT(2)
#define <DEVNAME>_ECGCTL_V1EN_MSK	NO_OS_BIT(3)
#define <DEVNAME>_ECGCTL_V2EN_MSK	NO_OS_BIT(4)
#define <DEVNAME>_ECGCTL_PWREN_MSK	NO_OS_BIT(7)

#define <DEVNAME>_FRMCTL_FRMRATE_MSK	NO_OS_GENMASK(1, 0)
#define <DEVNAME>_FRMCTL_CRCE_MSK	NO_OS_BIT(2)

#define <DEVNAME>_FILTCTL_LPF_MSK	NO_OS_GENMASK(3, 0)

/* Product / revision ID expected values */
#define <DEVNAME>_PRODUCT_ID		0xXX

/* ---------------- Enums -------------------------------------------- */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_lead
 *  @brief Lead electrode identifiers. */
enum <devname>_lead {
	<DEVNAME>_LEAD_LA,
	<DEVNAME>_LEAD_LL,
	<DEVNAME>_LEAD_RA,
	<DEVNAME>_LEAD_V1,
	<DEVNAME>_LEAD_V2,
};

/** @enum <devname>_frame_rate
 *  @brief Data frame output rate selection. */
enum <devname>_frame_rate {
	<DEVNAME>_FRAME_RATE_2KHZ,
	<DEVNAME>_FRAME_RATE_16KHZ,
	<DEVNAME>_FRAME_RATE_128KHZ,
};

/** @enum <devname>_filter_freq
 *  @brief Low-pass filter cutoff frequency selection. */
enum <devname>_filter_freq {
	<DEVNAME>_LPF_40HZ,
	<DEVNAME>_LPF_150HZ,
	<DEVNAME>_LPF_250HZ,
	<DEVNAME>_LPF_450HZ,
};

/** @enum <devname>_power_mode
 *  @brief Device power mode selection. */
enum <devname>_power_mode {
	<DEVNAME>_POWER_LOW,
	<DEVNAME>_POWER_NORMAL,
	<DEVNAME>_POWER_HIGH_RES,
};

/* ---------------- Structures --------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Device variant. */
	enum <devname>_device_id id;
	/** SPI initialization parameters. */
	struct no_os_spi_init_param spi_init;
	/** Bitmask of enabled lead channels (LA, LL, RA, V1, V2). */
	uint8_t channel_enables;
	/** Frame output data rate. */
	enum <devname>_frame_rate frame_rate;
	/** Low-pass filter cutoff frequency. */
	enum <devname>_filter_freq filter_freq;
	/** Power mode. */
	enum <devname>_power_mode power_mode;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** Device name string. */
	const char *name;
	/** Device variant. */
	enum <devname>_device_id id;
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** Bitmask of currently enabled lead channels. */
	uint8_t channel_enables;
	/** Active frame output data rate. */
	enum <devname>_frame_rate frame_rate;
	/** Active low-pass filter cutoff frequency. */
	enum <devname>_filter_freq filter_freq;
	/** Active power mode. */
	enum <devname>_power_mode power_mode;
};

/* ---------------- Function Prototypes ------------------------------ */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Read a register (32-bit frame-based SPI). */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint32_t *val);

/** @brief Write a register (32-bit frame-based SPI). */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint32_t val);

/** @brief Read a multi-channel ECG data frame. */
int <devname>_read_data_frame(struct <devname>_dev *dev, uint32_t *data,
			      uint8_t num_channels);

/** @brief Set the frame output data rate. */
int <devname>_set_frame_rate(struct <devname>_dev *dev,
			     enum <devname>_frame_rate rate);

/** @brief Set the low-pass filter cutoff frequency. */
int <devname>_set_filter(struct <devname>_dev *dev,
			 enum <devname>_filter_freq freq);

/** @brief Enable or disable a lead electrode channel. */
int <devname>_enable_channel(struct <devname>_dev *dev,
			     enum <devname>_lead lead, bool enable);

/** @brief Perform a software reset. */
int <devname>_soft_reset(struct <devname>_dev *dev);

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

/* ---------- Static helpers ----------------------------------------- */

/**
 * @brief Read a register over SPI (32-bit frame with CRC).
 * @param dev   - Device descriptor.
 * @param addr  - Register address.
 * @param val   - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint32_t *val)
{
	uint8_t buf[4];
	int ret;

	buf[0] = addr | 0x80; /* read bit */
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;        /* CRC byte */

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	*val = ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | buf[3];

	return 0;
}

/**
 * @brief Write a register over SPI (32-bit frame with CRC).
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint32_t val)
{
	uint8_t buf[4];

	buf[0] = addr & 0x7F; /* write bit */
	buf[1] = (uint8_t)(val >> 16);
	buf[2] = (uint8_t)(val >> 8);
	buf[3] = (uint8_t)(val & 0xFF);

	return no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
}

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Read a multi-channel ECG data frame.
 * @param dev          - Device descriptor.
 * @param data         - Buffer to store per-channel ECG samples.
 * @param num_channels - Number of channels to read.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read_data_frame(struct <devname>_dev *dev, uint32_t *data,
			      uint8_t num_channels)
{
	uint8_t i;
	int ret;

	for (i = 0; i < num_channels; i++) {
		ret = <devname>_reg_read(dev, <DEVNAME>_REG_NOP, &data[i]);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Set the frame output data rate.
 * @param dev  - Device descriptor.
 * @param rate - Desired frame rate.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_frame_rate(struct <devname>_dev *dev,
			     enum <devname>_frame_rate rate)
{
	uint32_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_FRMCTL, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~<DEVNAME>_FRMCTL_FRMRATE_MSK;
	reg_val |= no_os_field_prep(<DEVNAME>_FRMCTL_FRMRATE_MSK, rate);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_FRMCTL, reg_val);
	if (ret)
		return ret;

	dev->frame_rate = rate;

	return 0;
}

/**
 * @brief Set the low-pass filter cutoff frequency.
 * @param dev  - Device descriptor.
 * @param freq - Desired filter cutoff frequency.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_filter(struct <devname>_dev *dev,
			 enum <devname>_filter_freq freq)
{
	uint32_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_FILTCTL, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~<DEVNAME>_FILTCTL_LPF_MSK;
	reg_val |= no_os_field_prep(<DEVNAME>_FILTCTL_LPF_MSK, freq);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_FILTCTL, reg_val);
	if (ret)
		return ret;

	dev->filter_freq = freq;

	return 0;
}

/**
 * @brief Enable or disable a lead electrode channel.
 * @param dev    - Device descriptor.
 * @param lead   - Lead electrode identifier.
 * @param enable - true to enable, false to disable.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_enable_channel(struct <devname>_dev *dev,
			     enum <devname>_lead lead, bool enable)
{
	uint32_t reg_val;
	uint32_t mask;
	int ret;

	mask = NO_OS_BIT(lead);

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_ECGCTL, &reg_val);
	if (ret)
		return ret;

	if (enable)
		reg_val |= mask;
	else
		reg_val &= ~mask;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_ECGCTL, reg_val);
	if (ret)
		return ret;

	if (enable)
		dev->channel_enables |= (uint8_t)mask;
	else
		dev->channel_enables &= ~(uint8_t)mask;

	return 0;
}

/**
 * @brief Perform a software reset.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_soft_reset(struct <devname>_dev *dev)
{
	int ret;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_ECGCTL, 0x00);
	if (ret)
		return ret;

	/* Allow time for the device to complete the reset sequence. */
	no_os_mdelay(10);

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
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->id = init_param->id;
	d->name = <devname>_device_name[d->id];

	ret = no_os_spi_init(&d->spi_desc, &init_param->spi_init);
	if (ret)
		goto free_dev;

	/* Perform software reset to reach a known state */
	ret = <devname>_soft_reset(d);
	if (ret)
		goto free_spi;

	/* Configure frame rate */
	ret = <devname>_set_frame_rate(d, init_param->frame_rate);
	if (ret)
		goto free_spi;

	/* Configure low-pass filter */
	ret = <devname>_set_filter(d, init_param->filter_freq);
	if (ret)
		goto free_spi;

	/* Enable requested lead channels */
	d->channel_enables = init_param->channel_enables;
	ret = <devname>_reg_write(d, <DEVNAME>_REG_ECGCTL,
				  (uint32_t)init_param->channel_enables);
	if (ret)
		goto free_spi;

	*dev = d;

	return 0;

free_spi:
	no_os_spi_remove(d->spi_desc);
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

	ret = no_os_spi_remove(dev->spi_desc);
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
 * @brief Read raw ECG value for a lead channel.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	uint32_t data;
	int32_t val;
	int ret;

	ret = <devname>_reg_read(desc-><devname>_dev, <DEVNAME>_REG_NOP,
				 &data);
	if (ret)
		return ret;

	val = (int32_t)data;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor (mV per LSB).
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	/*
	 * ECG front-end scale depends on gain and reference.
	 * Example: 1.0 mV full-scale, 16-bit resolution
	 * scale = 1000000 nV / 65536 = 15.259 nV per LSB
	 * Expressed as IIO_VAL_INT_PLUS_MICRO: 0 + 15259
	 */
	val[0] = 0;
	val[1] = 15259;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/**
 * @brief Read the current sampling frequency.
 */
static int <devname>_iio_read_samp_freq(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int32_t val;

	switch (desc-><devname>_dev->frame_rate) {
	case <DEVNAME>_FRAME_RATE_2KHZ:
		val = 2000;
		break;
	case <DEVNAME>_FRAME_RATE_16KHZ:
		val = 16000;
		break;
	case <DEVNAME>_FRAME_RATE_128KHZ:
		val = 128000;
		break;
	default:
		val = 0;
		break;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_ch_attrs[] = {
	{ .name = "raw",                .show = <devname>_iio_read_raw },
	{ .name = "scale",              .show = <devname>_iio_read_scale },
	{ .name = "sampling_frequency", .show = <devname>_iio_read_samp_freq },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_scan_type = {
	.sign = 'u',          /* unsigned */
	.realbits = 16,       /* ECG sample resolution */
	.storagebits = 32,    /* 32-bit frame container */
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * ECG channels: IIO_VOLTAGE, ch_out = false (input), one per lead.
 */
#define <DEVNAME>_IIO_CHANNEL(idx, lead_name) \
	{ \
		.name = lead_name, \
		.ch_type = IIO_VOLTAGE, \
		.channel = (idx), \
		.scan_type = &<devname>_scan_type, \
		.scan_index = (idx), \
		.attributes = <devname>_iio_ch_attrs, \
		.ch_out = false, \
		.indexed = true, \
	}

static struct iio_channel <devname>_iio_channels[] = {
	<DEVNAME>_IIO_CHANNEL(0, "LA"),
	<DEVNAME>_IIO_CHANNEL(1, "LL"),
	<DEVNAME>_IIO_CHANNEL(2, "RA"),
	<DEVNAME>_IIO_CHANNEL(3, "V1"),
	<DEVNAME>_IIO_CHANNEL(4, "V2"),
	END_ATTRIBUTES_ARRAY
};

/* ---------- Buffered read callback --------------------------------- */

/**
 * @brief Read ECG data frame samples into an IIO buffer.
 */
static int <devname>_iio_read_samples(void *dev, int16_t *buf,
				      uint32_t samples)
{
	struct <devname>_iio_desc *desc = dev;

	return <devname>_read_data_frame(desc-><devname>_dev,
					 (uint32_t *)buf, samples);
}

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels) - 1,
	.debug_reg_read = (int32_t (*)())<devname>_reg_read,
	.debug_reg_write = (int32_t (*)())<devname>_reg_write,
	.read_dev = (int32_t (*)())<devname>_iio_read_samples,
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
			     init_param-><devname>_init_param);
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
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_util.c  \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/ecg/<devname>/<devname>.h
SRCS += $(DRIVERS)/ecg/<devname>/<devname>.c
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
INCS += $(DRIVERS)/ecg/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/ecg/<devname>/iio_<devname>.c

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
extern const struct no_os_spi_init_param <devname>_spi_ip;
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

const struct no_os_spi_init_param <devname>_spi_ip = {
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
	.id = ID_<DEVNAME>,
	.spi_init = <devname>_spi_ip,
	.channel_enables = (1 << <DEVNAME>_LEAD_LA) |
			   (1 << <DEVNAME>_LEAD_LL) |
			   (1 << <DEVNAME>_LEAD_RA),
	.frame_rate = <DEVNAME>_FRAME_RATE_2KHZ,
	.filter_freq = <DEVNAME>_LPF_150HZ,
	.power_mode = <DEVNAME>_POWER_NORMAL,
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

#define ECG_NUM_LEADS	5

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct <devname>_dev *dev;
	uint32_t ecg_frame[ECG_NUM_LEADS];
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	/* Enable leads LA, LL, RA for standard Lead I/II/III */
	ret = <devname>_enable_channel(dev, <DEVNAME>_LEAD_LA, true);
	if (ret)
		goto free_dev;

	ret = <devname>_enable_channel(dev, <DEVNAME>_LEAD_LL, true);
	if (ret)
		goto free_dev;

	ret = <devname>_enable_channel(dev, <DEVNAME>_LEAD_RA, true);
	if (ret)
		goto free_dev;

	while (1) {
		ret = <devname>_read_data_frame(dev, ecg_frame, ECG_NUM_LEADS);
		if (ret)
			goto free_dev;

		pr_info("LA: %lu  LL: %lu  RA: %lu  V1: %lu  V2: %lu\r\n",
			(unsigned long)ecg_frame[0],
			(unsigned long)ecg_frame[1],
			(unsigned long)ecg_frame[2],
			(unsigned long)ecg_frame[3],
			(unsigned long)ecg_frame[4]);
		no_os_mdelay(500);
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

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;

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
```

### 7.15 `projects/<project_name>/src/platform/maxim/platform_src.mk`

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

The <DEVNAME> is a low-power, multi-channel ECG/biopotential analog front-end
with an SPI-compatible serial interface using 32-bit frame-based communication
with CRC. It supports configurable data rates (2 kHz, 16 kHz, 128 kHz),
selectable low-pass filters, and multiple lead electrode inputs (LA, LL, RA,
V1, V2) for standard 3-lead and 5-lead ECG acquisition. <Add 2-3 more
sentences from the datasheet covering key specs and features.>

Applications
------------

* Electrocardiography (ECG/EKG)
* Patient monitoring systems
* Wearable health devices
* Biopotential measurement

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device descriptor,
initialises the SPI interface, performs a software reset, and applies the
initial frame rate, filter, and lead channel configuration. The function
uses goto-based cleanup to release resources on failure.

Lead Configuration
~~~~~~~~~~~~~~~~~~

The ``<devname>_enable_channel`` function enables or disables individual
lead electrode channels (LA, LL, RA, V1, V2) by modifying the ECG control
register. Multiple leads can be enabled independently to support 3-lead
or 5-lead ECG configurations.

Data Frame Reading
~~~~~~~~~~~~~~~~~~

The ``<devname>_read_data_frame`` function acquires a multi-channel ECG
data frame by issuing successive 32-bit SPI reads. Each frame contains
one sample per enabled lead channel, packed into 32-bit words with
optional CRC verification.

Filter Configuration
~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_filter`` function programs the low-pass filter cutoff
frequency (40 Hz, 150 Hz, 250 Hz, or 450 Hz) to control the signal
bandwidth. The ``<devname>_set_frame_rate`` function configures the
output data rate (2 kHz, 16 kHz, or 128 kHz).

Power Management
~~~~~~~~~~~~~~~~

The device supports multiple power modes (low power, normal, high
resolution) selected during initialization. The ``<devname>_soft_reset``
function restores the device to its default state for reconfiguration.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .id = ID_<DEVNAME>,
           .spi_init = { /* platform-specific SPI params */ },
           .channel_enables = (1 << <DEVNAME>_LEAD_LA) |
                              (1 << <DEVNAME>_LEAD_LL) |
                              (1 << <DEVNAME>_LEAD_RA),
           .frame_rate = <DEVNAME>_FRAME_RATE_2KHZ,
           .filter_freq = <DEVNAME>_LPF_150HZ,
           .power_mode = <DEVNAME>_POWER_NORMAL,
       };

       struct <devname>_dev *dev;
       uint32_t ecg_frame[5];
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_read_data_frame(dev, ecg_frame, 5);
       if (ret)
           goto cleanup;

       /* ecg_frame contains per-lead raw ECG values */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each lead electrode as an
``IIO_VOLTAGE`` channel with ``raw``, ``scale``, and
``sampling_frequency`` attributes. The IIO application can be used
for real-time ECG data acquisition and streaming.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/ecg/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/ecg/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`).
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

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> multi-channel ECG/biopotential analog
front-end with SPI interface and 32-bit frame-based protocol.

Signed-off-by: Your Name <your.name@analog.com>
```
