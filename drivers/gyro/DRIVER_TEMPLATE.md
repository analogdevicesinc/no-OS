# Gyroscope Driver Template

Reference driver: `drivers/gyro/adxrs290/`

This template covers every file needed to add a new gyroscope driver to no-OS.
Replace `<devname>` with the part number (e.g., `adxrs290`) and `<DEVNAME>`
with its uppercase form (e.g., `ADXRS290`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adxrs290.h`, `adxrs290.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_adxrs290.h`, `iio_adxrs290.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADXRS290_H__` |
| Init param struct | `<devname>_init_param` | `adxrs290_init_param` |
| Device struct | `<devname>_dev` | `adxrs290_dev` |
| IIO descriptor | `<devname>_iio_desc` | `adxrs290_iio_desc` |
| Functions | `<devname>_<verb>()` | `adxrs290_init()` |
| Static helpers | `_<devname>_<verb>()` | `_adxrs290_read_burst()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADXRS290` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `ADXRS290_FILTER_LPF_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADXRS290_LPF_480HZ` |

---

## 2. File Checklist

```
drivers/gyro/<devname>/
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

doc/sphinx/source/drivers/gyro/<devname>.rst   # Sphinx entry
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
#include "no_os_gpio.h"
#include "no_os_util.h"

/* ---------------- Register Map / Bit Masks ------------------------- */

/** Register addresses — adjust to match the device datasheet. */
#define <DEVNAME>_REG_ADI_ID		0x00
#define <DEVNAME>_REG_DEV_ID		0x02
#define <DEVNAME>_REG_DATAX0		0x08
#define <DEVNAME>_REG_DATAX1		0x09
#define <DEVNAME>_REG_DATAY0		0x0A
#define <DEVNAME>_REG_DATAY1		0x0B
#define <DEVNAME>_REG_TEMP0		0x0C
#define <DEVNAME>_REG_TEMP1		0x0D
#define <DEVNAME>_REG_POWER_CTL		0x10
#define <DEVNAME>_REG_FILTER		0x11
#define <DEVNAME>_REG_DATA_READY	0x12

/** Bit masks — use NO_OS_BIT() for single bits,
 *  NO_OS_GENMASK(hi, lo) for multi-bit fields. */
#define <DEVNAME>_READ			NO_OS_BIT(7)
#define <DEVNAME>_TSM			NO_OS_BIT(0)
#define <DEVNAME>_MEASUREMENT		NO_OS_BIT(1)
#define <DEVNAME>_DATA_RDY_OUT		NO_OS_BIT(0)
#define <DEVNAME>_LPF_MASK		NO_OS_GENMASK(2, 0)
#define <DEVNAME>_HPF_MASK		NO_OS_GENMASK(7, 4)

/* Product / device ID expected values */
#define <DEVNAME>_DEV_ID_VAL		0xXX

#define <DEVNAME>_CHANNEL_COUNT		3
#define <DEVNAME>_CHANNEL_MASK		0x07

/* ---------------- Enums -------------------------------------------- */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_mode
 *  @brief Operating mode selection. */
enum <devname>_mode {
	/** Standby mode — low power, no data output. */
	<DEVNAME>_MODE_STANDBY,
	/** Measurement mode — active data acquisition. */
	<DEVNAME>_MODE_MEASUREMENT,
};

/** @enum <devname>_channel
 *  @brief Data channel selection. */
enum <devname>_channel {
	/** X-axis angular velocity. */
	<DEVNAME>_CHANNEL_X,
	/** Y-axis angular velocity. */
	<DEVNAME>_CHANNEL_Y,
	/** On-die temperature sensor. */
	<DEVNAME>_CHANNEL_TEMP,
};

/** @enum <devname>_lpf
 *  @brief Low-pass filter pole location. */
enum <devname>_lpf {
	<DEVNAME>_LPF_480HZ,
	<DEVNAME>_LPF_320HZ,
	<DEVNAME>_LPF_160HZ,
	<DEVNAME>_LPF_80HZ,
	<DEVNAME>_LPF_56HZ6,
	<DEVNAME>_LPF_40HZ,
	<DEVNAME>_LPF_28HZ3,
	<DEVNAME>_LPF_20HZ,
};

/** @enum <devname>_hpf
 *  @brief High-pass filter pole location. */
enum <devname>_hpf {
	<DEVNAME>_HPF_ALL_PASS,
	<DEVNAME>_HPF_0HZ011,
	<DEVNAME>_HPF_0HZ022,
	<DEVNAME>_HPF_0HZ044,
	<DEVNAME>_HPF_0HZ087,
	<DEVNAME>_HPF_0HZ175,
	<DEVNAME>_HPF_0HZ350,
	<DEVNAME>_HPF_0HZ700,
	<DEVNAME>_HPF_1HZ400,
	<DEVNAME>_HPF_2HZ800,
	<DEVNAME>_HPF_11HZ30,
};

/* Add more enums as needed: bandwidth, sync modes, etc. */

/* ---------------- Structures --------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** SPI initialization parameters. */
	struct no_os_spi_init_param	spi_init;
	/** Optional GPIO for data-ready / sync signal. */
	struct no_os_gpio_init_param	*gpio_sync;
	/** Initial operating mode. */
	enum <devname>_mode		mode;
	/** Initial low-pass filter setting. */
	enum <devname>_lpf		lpf;
	/** Initial high-pass filter setting. */
	enum <devname>_hpf		hpf;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** SPI descriptor. */
	struct no_os_spi_desc		*spi_desc;
	/** GPIO descriptor for data-ready / sync signal. */
	struct no_os_gpio_desc		*gpio_sync;
	/** Bitmask of active channels. */
	uint8_t				ch_mask;
};

/* ---------------- Function Prototypes ------------------------------ */

/** @brief Read a register. */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *val);

/** @brief Write a register. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t val);

/** @brief Set the operating mode (standby / measurement). */
int <devname>_set_op_mode(struct <devname>_dev *dev,
			  enum <devname>_mode mode);

/** @brief Get the low-pass filter pole location. */
int <devname>_get_lpf(struct <devname>_dev *dev, enum <devname>_lpf *lpf);

/** @brief Set the low-pass filter pole location. */
int <devname>_set_lpf(struct <devname>_dev *dev, enum <devname>_lpf lpf);

/** @brief Get the high-pass filter pole location. */
int <devname>_get_hpf(struct <devname>_dev *dev, enum <devname>_hpf *hpf);

/** @brief Set the high-pass filter pole location. */
int <devname>_set_hpf(struct <devname>_dev *dev, enum <devname>_hpf hpf);

/** @brief Read angular velocity data for a given axis (X or Y). */
int <devname>_get_rate_data(struct <devname>_dev *dev,
			    enum <devname>_channel ch, int16_t *rate);

/** @brief Read temperature sensor data. */
int <devname>_get_temp_data(struct <devname>_dev *dev, int16_t *temp);

/** @brief Read burst data for all active channels. */
int <devname>_get_burst_data(struct <devname>_dev *dev,
			     int16_t *burst_data, uint8_t *ch_cnt);

/** @brief Set the active channel mask. */
int <devname>_set_active_channels(struct <devname>_dev *dev,
				  uint32_t mask);

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

/* ---------- Register access ---------------------------------------- */

/**
 * @brief Read a register over SPI.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *val)
{
	uint8_t buf[2];
	int ret;

	buf[0] = addr | <DEVNAME>_READ;
	buf[1] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	*val = buf[1];

	return 0;
}

/**
 * @brief Write a register over SPI.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t val)
{
	uint8_t buf[2];

	buf[0] = addr & 0x7F; /* write bit */
	buf[1] = val;

	return no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
}

/* ---------- Mode control ------------------------------------------- */

/**
 * @brief Set the operating mode (standby or measurement).
 * @param dev  - Device descriptor.
 * @param mode - Target operating mode.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_op_mode(struct <devname>_dev *dev,
			  enum <devname>_mode mode)
{
	uint8_t val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_POWER_CTL, &val);
	if (ret)
		return ret;

	switch (mode) {
	case <DEVNAME>_MODE_STANDBY:
		val &= ~<DEVNAME>_MEASUREMENT;
		break;
	case <DEVNAME>_MODE_MEASUREMENT:
		val |= <DEVNAME>_MEASUREMENT;
		break;
	default:
		return -EINVAL;
	}

	return <devname>_reg_write(dev, <DEVNAME>_REG_POWER_CTL, val);
}

/* ---------- Filter configuration ----------------------------------- */

/**
 * @brief Get the low-pass filter pole location.
 * @param dev - Device descriptor.
 * @param lpf - Pointer to store current LPF setting.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_lpf(struct <devname>_dev *dev, enum <devname>_lpf *lpf)
{
	uint8_t data;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_FILTER, &data);
	if (ret)
		return ret;

	*lpf = no_os_field_get(<DEVNAME>_LPF_MASK, data);

	return 0;
}

/**
 * @brief Set the low-pass filter pole location.
 * @param dev - Device descriptor.
 * @param lpf - Desired LPF pole location.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_lpf(struct <devname>_dev *dev, enum <devname>_lpf lpf)
{
	uint8_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_FILTER, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~<DEVNAME>_LPF_MASK;
	reg_val |= no_os_field_prep(<DEVNAME>_LPF_MASK, lpf);

	return <devname>_reg_write(dev, <DEVNAME>_REG_FILTER, reg_val);
}

/**
 * @brief Get the high-pass filter pole location.
 * @param dev - Device descriptor.
 * @param hpf - Pointer to store current HPF setting.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_hpf(struct <devname>_dev *dev, enum <devname>_hpf *hpf)
{
	uint8_t data;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_FILTER, &data);
	if (ret)
		return ret;

	*hpf = no_os_field_get(<DEVNAME>_HPF_MASK, data);

	return 0;
}

/**
 * @brief Set the high-pass filter pole location.
 * @param dev - Device descriptor.
 * @param hpf - Desired HPF pole location.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_hpf(struct <devname>_dev *dev, enum <devname>_hpf hpf)
{
	uint8_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_FILTER, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~<DEVNAME>_HPF_MASK;
	reg_val |= no_os_field_prep(<DEVNAME>_HPF_MASK, hpf);

	return <devname>_reg_write(dev, <DEVNAME>_REG_FILTER, reg_val);
}

/* ---------- Data readout ------------------------------------------- */

/**
 * @brief Read angular velocity data for a given axis.
 * @param dev  - Device descriptor.
 * @param ch   - Channel to read (X or Y).
 * @param rate - Pointer to store angular velocity value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_rate_data(struct <devname>_dev *dev,
			    enum <devname>_channel ch, int16_t *rate)
{
	uint8_t data[3];
	int ret;

	data[0] = <DEVNAME>_READ | (<DEVNAME>_REG_DATAX0 + ch * 2);
	data[1] = <DEVNAME>_READ | (<DEVNAME>_REG_DATAX0 + ch * 2 + 1);
	data[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, data, sizeof(data));
	if (ret)
		return ret;

	*rate = (((int16_t)data[2]) << 8) | data[1];

	return 0;
}

/**
 * @brief Read the on-die temperature sensor.
 * @param dev  - Device descriptor.
 * @param temp - Pointer to store temperature value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_temp_data(struct <devname>_dev *dev, int16_t *temp)
{
	uint8_t data[3];
	int ret;

	data[0] = <DEVNAME>_READ | <DEVNAME>_REG_TEMP0;
	data[1] = <DEVNAME>_READ | <DEVNAME>_REG_TEMP1;
	data[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, data, sizeof(data));
	if (ret)
		return ret;

	*temp = ((((int16_t)data[2]) << 8) | data[1]) & 0x0FFF;
	*temp = (*temp << 4) >> 4; /* sign-extend 12-bit value */

	return 0;
}

/**
 * @brief Read burst data for all active channels.
 * @param dev        - Device descriptor.
 * @param burst_data - Buffer to store channel data.
 * @param ch_cnt     - Pointer to store number of channels returned.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_burst_data(struct <devname>_dev *dev,
			     int16_t *burst_data, uint8_t *ch_cnt)
{
	uint8_t data_bytes = <DEVNAME>_CHANNEL_COUNT * 2;
	uint8_t data[data_bytes + 1];
	int16_t result;
	uint8_t ch_idx;
	uint8_t i;
	int ret;

	for (i = 0; i < data_bytes; i++)
		data[i] = <DEVNAME>_READ | (<DEVNAME>_REG_DATAX0 + i);

	data[data_bytes] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, data,
				       data_bytes + 1);
	if (ret)
		return ret;

	i = 1;
	*ch_cnt = 0;
	for (ch_idx = 0; ch_idx < <DEVNAME>_CHANNEL_COUNT; ch_idx++) {
		result = (((int16_t)data[i + 1]) << 8) | data[i];
		i += 2;

		if (ch_idx == <DEVNAME>_CHANNEL_TEMP)
			result = (result << 4) >> 4;

		if ((1 << ch_idx) & dev->ch_mask)
			burst_data[(*ch_cnt)++] = result;
	}

	return 0;
}

/**
 * @brief Set the active channel mask.
 * @param dev  - Device descriptor.
 * @param mask - Bitmask of channels to enable.
 * @return 0 on success.
 */
int <devname>_set_active_channels(struct <devname>_dev *dev,
				  uint32_t mask)
{
	dev->ch_mask = mask & <DEVNAME>_CHANNEL_MASK;

	return 0;
}

/* ---------- Init / Remove ------------------------------------------ */

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
	uint8_t dev_id;
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = no_os_spi_init(&d->spi_desc, &init_param->spi_init);
	if (ret)
		goto free_dev;

	/* Verify device ID */
	ret = <devname>_reg_read(d, <DEVNAME>_REG_DEV_ID, &dev_id);
	if (ret)
		goto free_spi;

	if (dev_id != <DEVNAME>_DEV_ID_VAL) {
		pr_err("Unexpected device ID: 0x%02X\n", dev_id);
		ret = -ENODEV;
		goto free_spi;
	}

	/* Set initial operating mode */
	if (init_param->mode == <DEVNAME>_MODE_MEASUREMENT) {
		ret = <devname>_set_op_mode(d, <DEVNAME>_MODE_MEASUREMENT);
		if (ret)
			goto free_spi;
	}

	/* Apply initial filter settings */
	ret = <devname>_set_lpf(d, init_param->lpf);
	if (ret)
		goto free_spi;

	ret = <devname>_set_hpf(d, init_param->hpf);
	if (ret)
		goto free_spi;

	/* Set up optional data-ready / sync GPIO */
	ret = no_os_gpio_get_optional(&d->gpio_sync, init_param->gpio_sync);
	if (ret)
		goto free_spi;

	if (d->gpio_sync) {
		ret = no_os_gpio_direction_input(d->gpio_sync);
		if (ret)
			goto free_gpio;
	}

	/* Enable all channels by default */
	d->ch_mask = <DEVNAME>_CHANNEL_MASK;

	*dev = d;

	return 0;

free_gpio:
	no_os_gpio_remove(d->gpio_sync);
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
	if (!dev)
		return -EINVAL;

	no_os_gpio_remove(dev->gpio_sync);
	no_os_spi_remove(dev->spi_desc);
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

/* ---------- Filter frequency tables -------------------------------- */

/**
 * Available cut-off frequencies of the low-pass filter in Hz.
 * The integer part and fractional part are represented separately.
 */
static const int <devname>_lpf_3db_freq_hz_table[][2] = {
	[0] = {480, 0},
	[1] = {320, 0},
	[2] = {160, 0},
	[3] = {80, 0},
	[4] = {56, 600000},
	[5] = {40, 0},
	[6] = {28, 300000},
	[7] = {20, 0},
};

/**
 * Available cut-off frequencies of the high-pass filter in Hz.
 * The integer part and fractional part are represented separately.
 */
static const int <devname>_hpf_3db_freq_hz_table[][2] = {
	[0] = {0, 0},
	[1] = {0, 11000},
	[2] = {0, 22000},
	[3] = {0, 44000},
	[4] = {0, 87000},
	[5] = {0, 175000},
	[6] = {0, 350000},
	[7] = {0, 700000},
	[8] = {1, 400000},
	[9] = {2, 800000},
	[10] = {11, 300000},
};

/* ---------- Attribute callbacks ------------------------------------ */

/**
 * @brief Read raw angular velocity or temperature value for a channel.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int16_t data;
	int32_t val;
	int ret;

	if (channel->ch_num == <DEVNAME>_CHANNEL_TEMP) {
		ret = <devname>_get_temp_data(desc-><devname>_dev, &data);
	} else {
		ret = <devname>_get_rate_data(desc-><devname>_dev,
					      channel->ch_num, &data);
	}
	if (ret)
		return ret;

	val = (int32_t)data;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor.
 *
 * For angular velocity channels: scale converts LSB to rad/s.
 * For temperature channel: scale converts LSB to millidegrees Celsius.
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	if (channel->ch_num == <DEVNAME>_CHANNEL_TEMP) {
		/*
		 * Temperature: 1 LSB = 0.1 deg C = 100 mdeg C.
		 * Expressed as IIO_VAL_INT: 100
		 */
		val[0] = 100;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, val);
	}

	/*
	 * Angular velocity: 1 LSB = 0.005 deg/s = 0.000087266 rad/s.
	 * Expressed as IIO_VAL_INT_PLUS_MICRO: 0 + 87266
	 */
	val[0] = 0;
	val[1] = 87266;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/**
 * @brief Read offset (typically 0 for gyroscope channels).
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the high-pass filter 3 dB frequency.
 */
static int <devname>_iio_read_hpf(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	enum <devname>_hpf hpf;
	int ret;

	ret = <devname>_get_hpf(desc-><devname>_dev, &hpf);
	if (ret)
		return ret;

	if (hpf > <DEVNAME>_HPF_11HZ30)
		hpf = <DEVNAME>_HPF_11HZ30;

	return snprintf(buf, len, "%d.%06d",
			<devname>_hpf_3db_freq_hz_table[hpf][0],
			<devname>_hpf_3db_freq_hz_table[hpf][1]);
}

/**
 * @brief Write the high-pass filter 3 dB frequency.
 */
static int <devname>_iio_write_hpf(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	float hpf = strtof(buf, NULL);
	int32_t val = (int32_t)hpf;
	int32_t val2 = (int32_t)(hpf * 1000000) % 1000000;
	uint8_t i;
	uint8_t n = NO_OS_ARRAY_SIZE(<devname>_hpf_3db_freq_hz_table);

	for (i = 0; i < n; i++)
		if (<devname>_hpf_3db_freq_hz_table[i][0] == val &&
		    <devname>_hpf_3db_freq_hz_table[i][1] == val2) {
			<devname>_set_hpf(desc-><devname>_dev,
					  (enum <devname>_hpf)i);
			return len;
		}

	return -EINVAL;
}

/**
 * @brief Read the low-pass filter 3 dB frequency.
 */
static int <devname>_iio_read_lpf(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	enum <devname>_lpf lpf;
	int ret;

	ret = <devname>_get_lpf(desc-><devname>_dev, &lpf);
	if (ret)
		return ret;

	if (lpf > <DEVNAME>_LPF_20HZ)
		lpf = <DEVNAME>_LPF_20HZ;

	return snprintf(buf, len, "%d.%06d",
			<devname>_lpf_3db_freq_hz_table[lpf][0],
			<devname>_lpf_3db_freq_hz_table[lpf][1]);
}

/**
 * @brief Write the low-pass filter 3 dB frequency.
 */
static int <devname>_iio_write_lpf(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	float lpf = strtof(buf, NULL);
	int32_t val = (int32_t)lpf;
	int32_t val2 = (int32_t)(lpf * 1000000) % 1000000;
	uint8_t i;
	uint8_t n = NO_OS_ARRAY_SIZE(<devname>_lpf_3db_freq_hz_table);

	for (i = 0; i < n; i++)
		if (<devname>_lpf_3db_freq_hz_table[i][0] == val &&
		    <devname>_lpf_3db_freq_hz_table[i][1] == val2) {
			<devname>_set_lpf(desc-><devname>_dev,
					  (enum <devname>_lpf)i);
			return len;
		}

	return -EINVAL;
}

/* ---------- Attribute tables --------------------------------------- */

/**
 * Angular velocity channel attributes: raw, scale, offset, and
 * filter settings (HPF / LPF 3 dB frequencies).
 */
static struct iio_attribute <devname>_iio_vel_attrs[] = {
	{
		.name = "raw",
		.show = <devname>_iio_read_raw,
	},
	{
		.name = "scale",
		.show = <devname>_iio_read_scale,
	},
	{
		.name = "offset",
		.show = <devname>_iio_read_offset,
	},
	{
		.name = "filter_high_pass_3db_frequency",
		.show = <devname>_iio_read_hpf,
		.store = <devname>_iio_write_hpf,
	},
	{
		.name = "filter_low_pass_3db_frequency",
		.show = <devname>_iio_read_lpf,
		.store = <devname>_iio_write_lpf,
	},
	END_ATTRIBUTES_ARRAY
};

/**
 * Temperature channel attributes: raw and scale only.
 */
static struct iio_attribute <devname>_iio_temp_attrs[] = {
	{
		.name = "raw",
		.show = <devname>_iio_read_raw,
	},
	{
		.name = "scale",
		.show = <devname>_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan types --------------------------------------------- */

static struct scan_type <devname>_scan_type_gyro = {
	.sign = 's',          /* signed */
	.realbits = 16,       /* gyroscope resolution */
	.storagebits = 16,    /* container size */
	.shift = 0,
	.is_big_endian = false,
};

static struct scan_type <devname>_scan_type_temp = {
	.sign = 's',          /* signed */
	.realbits = 12,       /* temperature resolution */
	.storagebits = 16,    /* container size */
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * Gyroscope channels: IIO_ANGL_VEL, ch_out = false (input).
 * Two angular velocity channels (X, Y) plus one optional temperature
 * channel (IIO_TEMP).
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.ch_type = IIO_ANGL_VEL,
		.channel = 0,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.scan_index = 0,
		.scan_type = &<devname>_scan_type_gyro,
		.attributes = <devname>_iio_vel_attrs,
		.ch_out = false,
	},
	{
		.ch_type = IIO_ANGL_VEL,
		.channel = 1,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.scan_index = 1,
		.scan_type = &<devname>_scan_type_gyro,
		.attributes = <devname>_iio_vel_attrs,
		.ch_out = false,
	},
	{
		.ch_type = IIO_TEMP,
		.channel = 2,
		.scan_index = 2,
		.scan_type = &<devname>_scan_type_temp,
		.attributes = <devname>_iio_temp_attrs,
		.ch_out = false,
	},
};

/* ---------- Buffered read / trigger callbacks ---------------------- */

/**
 * @brief Update active channel mask before buffered capture.
 */
static int32_t <devname>_iio_update_channels(void *dev, uint32_t mask)
{
	struct <devname>_iio_desc *desc = dev;

	<devname>_set_active_channels(desc-><devname>_dev, mask);

	return 0;
}

/**
 * @brief Read gyroscope samples into an IIO buffer.
 */
static int32_t <devname>_iio_read_samples(void *dev, uint16_t *buf,
					   uint32_t samples)
{
	struct <devname>_iio_desc *desc = dev;
	int16_t data[<DEVNAME>_CHANNEL_COUNT];
	uint8_t ch_cnt;
	uint32_t i;
	uint32_t offset = 0;
	int ret;

	for (i = 0; i < samples; i++) {
		ret = <devname>_get_burst_data(desc-><devname>_dev,
					       data, &ch_cnt);
		if (ret)
			return ret;

		memcpy(&buf[offset], data, ch_cnt * sizeof(int16_t));
		offset += ch_cnt;
	}

	return samples;
}

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels),
	.pre_enable = <devname>_iio_update_channels,
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

INCS += $(DRIVERS)/gyro/<devname>/<devname>.h
SRCS += $(DRIVERS)/gyro/<devname>/<devname>.c
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
INCS += $(DRIVERS)/gyro/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/gyro/<devname>/iio_<devname>.c

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
	.mode = NO_OS_SPI_MODE_3,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.parent = NULL,
};

struct <devname>_init_param <devname>_ip = {
	.spi_init = <devname>_spi_ip,
	.gpio_sync = NULL,
	.mode = <DEVNAME>_MODE_MEASUREMENT,
	.lpf = <DEVNAME>_LPF_480HZ,
	.hpf = <DEVNAME>_HPF_ALL_PASS,
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
	int16_t rate_x, rate_y;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	while (1) {
		ret = <devname>_get_rate_data(dev, <DEVNAME>_CHANNEL_X,
					      &rate_x);
		if (ret)
			goto free_dev;

		ret = <devname>_get_rate_data(dev, <DEVNAME>_CHANNEL_Y,
					      &rate_y);
		if (ret)
			goto free_dev;

		pr_info("Angular velocity  X: %d  Y: %d\r\n",
			rate_x, rate_y);
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

The <DEVNAME> is a high-performance, dual-axis MEMS gyroscope with an
SPI-compatible serial interface. It provides angular velocity measurements
on the X and Y axes, with an integrated temperature sensor for compensation.
The device features configurable low-pass and high-pass digital filters
for optimizing noise performance. <Add 2-3 more sentences from the
datasheet covering key specs and features.>

Applications
------------

* Inertial navigation systems
* Platform stabilization
* Vibration monitoring
* Motion tracking

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface, verifies the device ID,
configures the initial operating mode, and applies the low-pass and
high-pass filter settings. An optional GPIO can be configured for
data-ready / sync signalling. The function uses goto-based cleanup
to release resources on failure.

Angular Velocity Reading
~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_get_rate_data`` function reads a 16-bit signed angular
velocity value from the selected axis (X or Y) by issuing two consecutive
register reads over SPI. The ``<devname>_get_burst_data`` function reads
all active channels (X, Y, and temperature) in a single SPI transaction,
returning only the data for channels enabled via the active channel mask.

Filter Configuration
~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_lpf`` and ``<devname>_set_hpf`` functions program
the low-pass and high-pass filter pole locations respectively by
modifying the filter configuration register. The corresponding
``<devname>_get_lpf`` and ``<devname>_get_hpf`` functions read back
the current filter settings.

Power Modes
~~~~~~~~~~~

The ``<devname>_set_op_mode`` function switches the device between
standby mode (low power, no data output) and measurement mode (active
data acquisition). The device should be placed in measurement mode
before reading angular velocity or temperature data.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .spi_init = { /* platform-specific SPI params */ },
           .gpio_sync = NULL,
           .mode = <DEVNAME>_MODE_MEASUREMENT,
           .lpf = <DEVNAME>_LPF_480HZ,
           .hpf = <DEVNAME>_HPF_ALL_PASS,
       };

       struct <devname>_dev *dev;
       int16_t rate_x, rate_y;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_get_rate_data(dev, <DEVNAME>_CHANNEL_X, &rate_x);
       if (ret)
           goto cleanup;

       ret = <devname>_get_rate_data(dev, <DEVNAME>_CHANNEL_Y, &rate_y);
       if (ret)
           goto cleanup;

       /* rate_x and rate_y contain the raw angular velocity values */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each gyroscope axis as an
``IIO_ANGL_VEL`` channel with ``raw``, ``scale``, ``offset``,
``filter_high_pass_3db_frequency``, and
``filter_low_pass_3db_frequency`` attributes. An additional
``IIO_TEMP`` channel provides on-die temperature data with ``raw``
and ``scale`` attributes. The IIO application can be used for
real-time angular velocity data acquisition.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/gyro/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/gyro/<devname>/README.rst
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

Add support for the <DEVNAME> dual-axis MEMS gyroscope with SPI
interface. The driver provides angular velocity measurement on X
and Y axes, on-die temperature readout, and configurable digital
low-pass and high-pass filters.

Signed-off-by: Your Name <your.name@analog.com>
```
