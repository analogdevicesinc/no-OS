# Amplifier Driver Template

Reference driver: `drivers/amplifiers/ada4250/`

This template covers every file needed to add a new amplifier driver to no-OS.
Replace `<devname>` with the part number (e.g., `ada4250`) and `<DEVNAME>`
with its uppercase form (e.g., `ADA4250`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ada4250.h`, `ada4250.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ada4250.h`, `iio_ada4250.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADA4250_H__` |
| Init param struct | `<devname>_init_param` | `ada4250_init_param` |
| Device struct | `<devname>_dev` | `ada4250_dev` |
| IIO descriptor | `<devname>_iio_desc` | `ada4250_iio_desc` |
| Functions | `<devname>_<verb>()` | `ada4250_init()` |
| Static helpers | `_<devname>_<verb>()` | `_ada4250_spi_xfer()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADA4250` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `ADA4250_GAIN_MUX_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADA4250_GAIN_1` |

---

## 2. File Checklist

```
drivers/amplifiers/<devname>/
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

doc/sphinx/source/drivers/amplifiers/<devname>.rst   # Sphinx entry
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

/* ---------------- Register Map ----------------------------------------- */

#define <DEVNAME>_REG_GAIN_MUX		0x00
#define <DEVNAME>_REG_REFBUF_EN		0x01
#define <DEVNAME>_REG_RESET		0x02
#define <DEVNAME>_REG_OFFSET_CAL_VAL	0x04
#define <DEVNAME>_REG_OFFSET_CAL_CNFG	0x05
#define <DEVNAME>_REG_CHIP_ID1		0x19
#define <DEVNAME>_REG_CHIP_ID2		0x1A

/* ---------------- Field Masks ------------------------------------------ */

/** Gain mux field — use NO_OS_GENMASK() for multi-bit fields. */
#define <DEVNAME>_GAIN_MUX_MSK		NO_OS_GENMASK(2, 0)
#define <DEVNAME>_GAIN_MUX(x)		no_os_field_prep(<DEVNAME>_GAIN_MUX_MSK, x)

/** Reference buffer enable — use NO_OS_BIT() for single-bit fields. */
#define <DEVNAME>_REFBUF_EN_MSK		NO_OS_BIT(0)
#define <DEVNAME>_REFBUF_EN(x)		no_os_field_prep(<DEVNAME>_REFBUF_EN_MSK, x)

/** Software reset. */
#define <DEVNAME>_RESET_MSK		NO_OS_BIT(0)
#define <DEVNAME>_RESET(x)		no_os_field_prep(<DEVNAME>_RESET_MSK, x)

/** Bias current source selection. */
#define <DEVNAME>_BIAS_SET_MSK		NO_OS_GENMASK(3, 2)
#define <DEVNAME>_BIAS_SET(x)		no_os_field_prep(<DEVNAME>_BIAS_SET_MSK, x)

/** Offset calibration range selection. */
#define <DEVNAME>_RANGE_SET_MSK		NO_OS_GENMASK(1, 0)
#define <DEVNAME>_RANGE_SET(x)		no_os_field_prep(<DEVNAME>_RANGE_SET_MSK, x)

/** Offset calibration value (8-bit). */
#define <DEVNAME>_CAL_VAL_MSK		NO_OS_GENMASK(7, 0)
#define <DEVNAME>_CAL_VAL(x)		no_os_field_prep(<DEVNAME>_CAL_VAL_MSK, x)

/* ---------------- SPI Protocol ----------------------------------------- */

#define <DEVNAME>_SPI_READ_CMD		NO_OS_BIT(7)
#define <DEVNAME>_SPI_WRITE_CMD		0x00
#define <DEVNAME>_BUFF_SIZE_BYTES	2
#define <DEVNAME>_SPI_DUMMY_DATA	0x00

/* ---------------- Product / Chip ID ------------------------------------ */

#define <DEVNAME>_CHIP_ID		0xXXXX

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_id
 *  @brief Supported device IDs. */
enum <devname>_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_gain
 *  @brief Programmable gain settings. */
enum <devname>_gain {
	<DEVNAME>_GAIN_1,
	<DEVNAME>_GAIN_2,
	<DEVNAME>_GAIN_4,
	<DEVNAME>_GAIN_8,
	<DEVNAME>_GAIN_16,
	<DEVNAME>_GAIN_32,
	<DEVNAME>_GAIN_64,
	<DEVNAME>_GAIN_128,
};

/** @enum <devname>_bias
 *  @brief Current bias source selection. */
enum <devname>_bias {
	<DEVNAME>_BIAS_DISABLE,
	<DEVNAME>_BIAS_BANDGAP_REF,
	<DEVNAME>_BIAS_AVDD,
};

/** @enum <devname>_offset_range
 *  @brief Sensor offset trim range. */
enum <devname>_offset_range {
	<DEVNAME>_RANGE1,
	<DEVNAME>_RANGE2,
	<DEVNAME>_RANGE3,
	<DEVNAME>_RANGE4,
};

/** @enum <devname>_bandwidth
 *  @brief Bandwidth mode selection. */
enum <devname>_bandwidth {
	<DEVNAME>_BANDWIDTH_LOW,
	<DEVNAME>_BANDWIDTH_HIGH,
};

/** @enum <devname>_power_mode
 *  @brief Power mode selection. */
enum <devname>_power_mode {
	<DEVNAME>_POWER_NORMAL,
	<DEVNAME>_POWER_SLEEP,
	<DEVNAME>_POWER_SHUTDOWN,
};

/* Add more enums as needed: input modes, channel selection, etc. */

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Device variant. */
	enum <devname>_id device_id;
	/** SPI initialization parameters. */
	struct no_os_spi_init_param *spi_init;
	/** GPIO initialization parameters for bandwidth control. */
	struct no_os_gpio_init_param *gpio_bw_param;
	/** GPIO initialization parameters for sleep pin. */
	struct no_os_gpio_init_param *gpio_slp;
	/** GPIO initialization parameters for shutdown pin. */
	struct no_os_gpio_init_param *gpio_shtdwn;
	/** AVDD supply voltage in millivolts. */
	int32_t avdd_v;
	/** Reference buffer enable. */
	bool refbuf_en;
	/** Initial gain setting. */
	enum <devname>_gain gain;
	/** Bias current source selection. */
	enum <devname>_bias bias;
	/** Bandwidth mode. */
	enum <devname>_bandwidth bandwidth;
	/** Offset calibration value in nanovolts. */
	int32_t offset_nv;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** Device variant. */
	enum <devname>_id device_id;
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** GPIO descriptor for bandwidth control. */
	struct no_os_gpio_desc *gpio_bw;
	/** GPIO descriptor for sleep pin. */
	struct no_os_gpio_desc *gpio_slp;
	/** GPIO descriptor for shutdown pin. */
	struct no_os_gpio_desc *gpio_shtdwn;
	/** AVDD supply voltage in millivolts. */
	int32_t avdd_v;
	/** Reference buffer enabled. */
	bool refbuf_en;
	/** Current gain setting. */
	enum <devname>_gain gain;
	/** Offset trim range. */
	enum <devname>_offset_range offset_range;
	/** Bias current source. */
	enum <devname>_bias bias;
	/** Bandwidth mode. */
	enum <devname>_bandwidth bandwidth;
	/** Power mode. */
	enum <devname>_power_mode power_mode;
	/** Offset calibration value in nanovolts. */
	int32_t offset_nv;
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Write a register over SPI. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t data);

/** @brief Read a register over SPI. */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *data);

/** @brief Read-modify-write a register field. */
int <devname>_reg_update(struct <devname>_dev *dev, uint8_t addr,
			 uint8_t mask, uint8_t data);

/** @brief Perform a software reset. */
int <devname>_soft_reset(struct <devname>_dev *dev);

/** @brief Enable/disable the reference buffer. */
int <devname>_en_refbuf(struct <devname>_dev *dev, bool refbuf);

/** @brief Set the gain. */
int <devname>_set_gain(struct <devname>_dev *dev,
		       enum <devname>_gain gain);

/** @brief Set the bias current source. */
int <devname>_set_bias(struct <devname>_dev *dev,
		       enum <devname>_bias bias);

/** @brief Set the offset calibration value. */
int <devname>_set_offset(struct <devname>_dev *dev, int64_t offset);

/** @brief Set the bandwidth mode. */
int <devname>_set_bandwidth(struct <devname>_dev *dev,
			    enum <devname>_bandwidth bw);

/** @brief Enter sleep or shutdown mode. */
int <devname>_set_slp_shtdwn_mode(struct <devname>_dev *dev,
				  enum <devname>_power_mode pwrmode);

/** @brief Return to normal mode. */
int <devname>_set_normal_mode(struct <devname>_dev *dev, bool reconfig);

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

#include <stdlib.h>
#include <errno.h>
#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

/* Device name lookup table (indexed by enum <devname>_id). */
static const char * const <devname>_device_name[] = {
	[ID_<DEVNAME>] = "<DEVNAME>",
};

/* ---------- Static helpers ----------------------------------------- */

/**
 * @brief Write a register over SPI.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param data - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t data)
{
	uint8_t buf[<DEVNAME>_BUFF_SIZE_BYTES];

	buf[0] = <DEVNAME>_SPI_WRITE_CMD | addr;
	buf[1] = data;

	return no_os_spi_write_and_read(dev->spi_desc, buf,
					<DEVNAME>_BUFF_SIZE_BYTES);
}

/**
 * @brief Read a register over SPI.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param data - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *data)
{
	uint8_t buf[<DEVNAME>_BUFF_SIZE_BYTES];
	int ret;

	buf[0] = <DEVNAME>_SPI_READ_CMD | addr;
	buf[1] = <DEVNAME>_SPI_DUMMY_DATA;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf,
				       <DEVNAME>_BUFF_SIZE_BYTES);
	if (ret)
		return ret;

	*data = buf[1];

	return 0;
}

/**
 * @brief Read-modify-write a register field.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param mask - Bit mask for the field to update.
 * @param data - New field value (pre-shifted using field_prep).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_update(struct <devname>_dev *dev, uint8_t addr,
			 uint8_t mask, uint8_t data)
{
	uint8_t read_val;
	int ret;

	ret = <devname>_reg_read(dev, addr, &read_val);
	if (ret)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	return <devname>_reg_write(dev, addr, read_val);
}

/**
 * @brief Perform a software reset.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_soft_reset(struct <devname>_dev *dev)
{
	int ret;
	uint16_t timeout = 0xFFFF;
	uint8_t data;

	ret = <devname>_reg_update(dev, <DEVNAME>_REG_RESET,
				   <DEVNAME>_RESET_MSK,
				   <DEVNAME>_RESET(1));
	if (ret)
		return ret;

	while (timeout--) {
		ret = <devname>_reg_read(dev, <DEVNAME>_REG_RESET, &data);
		if (ret)
			return ret;

		if (!(data & <DEVNAME>_RESET_MSK))
			return 0;
	}

	return -ETIME;
}

/**
 * @brief Enable/disable the reference buffer.
 * @param dev    - Device descriptor.
 * @param refbuf - true to enable, false to disable.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_en_refbuf(struct <devname>_dev *dev, bool refbuf)
{
	int ret;

	ret = <devname>_reg_update(dev, <DEVNAME>_REG_REFBUF_EN,
				   <DEVNAME>_REFBUF_EN_MSK,
				   <DEVNAME>_REFBUF_EN(refbuf));
	if (ret)
		return ret;

	dev->refbuf_en = refbuf;

	return 0;
}

/**
 * @brief Set the gain.
 * @param dev  - Device descriptor.
 * @param gain - Gain value from enum <devname>_gain.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_gain(struct <devname>_dev *dev,
		       enum <devname>_gain gain)
{
	int ret;

	ret = <devname>_reg_update(dev, <DEVNAME>_REG_GAIN_MUX,
				   <DEVNAME>_GAIN_MUX_MSK,
				   <DEVNAME>_GAIN_MUX(gain));
	if (ret)
		return ret;

	dev->gain = gain;

	return 0;
}

/**
 * @brief Set the bias current source.
 * @param dev  - Device descriptor.
 * @param bias - Bias selection from enum <devname>_bias.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_bias(struct <devname>_dev *dev,
		       enum <devname>_bias bias)
{
	int ret;

	ret = <devname>_reg_update(dev, <DEVNAME>_REG_OFFSET_CAL_CNFG,
				   <DEVNAME>_BIAS_SET_MSK,
				   <DEVNAME>_BIAS_SET(bias));
	if (ret)
		return ret;

	dev->bias = bias;

	return 0;
}

/**
 * @brief Set the offset calibration value.
 * @param dev    - Device descriptor.
 * @param offset - Offset in nanovolts.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_offset(struct <devname>_dev *dev, int64_t offset)
{
	int ret;

	/*
	 * Compute the raw DAC code and range from the requested offset
	 * in nanovolts. The calculation depends on the current gain and
	 * bias source — refer to the device datasheet for the formula.
	 */

	/* ... device-specific offset calculation ... */

	ret = <devname>_reg_update(dev, <DEVNAME>_REG_OFFSET_CAL_CNFG,
				   <DEVNAME>_RANGE_SET_MSK,
				   <DEVNAME>_RANGE_SET(dev->offset_range));
	if (ret)
		return ret;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_OFFSET_CAL_VAL,
				  /* computed raw code */0);
	if (ret)
		return ret;

	dev->offset_nv = (int32_t)offset;

	return 0;
}

/**
 * @brief Set the bandwidth mode via GPIO.
 * @param dev - Device descriptor.
 * @param bw  - Bandwidth mode from enum <devname>_bandwidth.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_bandwidth(struct <devname>_dev *dev,
			    enum <devname>_bandwidth bw)
{
	int ret;
	uint8_t val;

	switch (bw) {
	case <DEVNAME>_BANDWIDTH_LOW:
		val = NO_OS_GPIO_HIGH;
		break;
	case <DEVNAME>_BANDWIDTH_HIGH:
		val = NO_OS_GPIO_LOW;
		break;
	default:
		return -EINVAL;
	}

	ret = no_os_gpio_set_value(dev->gpio_bw, val);
	if (ret)
		return ret;

	dev->bandwidth = bw;

	return 0;
}

/**
 * @brief Enter sleep or shutdown mode via GPIO.
 * @param dev     - Device descriptor.
 * @param pwrmode - Power mode to enter.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_slp_shtdwn_mode(struct <devname>_dev *dev,
				  enum <devname>_power_mode pwrmode)
{
	int ret;

	switch (pwrmode) {
	case <DEVNAME>_POWER_SLEEP:
		ret = no_os_gpio_set_value(dev->gpio_slp, NO_OS_GPIO_LOW);
		break;
	case <DEVNAME>_POWER_SHUTDOWN:
		ret = no_os_gpio_set_value(dev->gpio_shtdwn, NO_OS_GPIO_LOW);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	dev->power_mode = pwrmode;

	return 0;
}

/**
 * @brief Return to normal mode from sleep or shutdown.
 * @param dev      - Device descriptor.
 * @param reconfig - true to re-apply configuration after shutdown.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_normal_mode(struct <devname>_dev *dev, bool reconfig)
{
	int ret;

	switch (dev->power_mode) {
	case <DEVNAME>_POWER_SHUTDOWN:
		ret = no_os_gpio_set_value(dev->gpio_shtdwn,
					   NO_OS_GPIO_HIGH);
		break;
	case <DEVNAME>_POWER_SLEEP:
		ret = no_os_gpio_set_value(dev->gpio_slp,
					   NO_OS_GPIO_HIGH);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	/* Wait for the device to wake up */
	no_os_udelay(dev->power_mode == <DEVNAME>_POWER_SLEEP ? 200 : 400);

	dev->power_mode = <DEVNAME>_POWER_NORMAL;

	return 0;
}

/* ---------- Public API --------------------------------------------- */

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
	uint8_t data;
	uint16_t chip_id;
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->device_id = init_param->device_id;
	d->gain = init_param->gain;
	d->bias = init_param->bias;
	d->bandwidth = init_param->bandwidth;
	d->refbuf_en = init_param->refbuf_en;
	d->offset_nv = init_param->offset_nv;
	d->avdd_v = init_param->avdd_v;

	/* Initialize shutdown GPIO (active-low) — pull HIGH for normal */
	ret = no_os_gpio_get_optional(&d->gpio_shtdwn,
				      init_param->gpio_shtdwn);
	if (ret)
		goto free_dev;

	ret = no_os_gpio_direction_output(d->gpio_shtdwn, NO_OS_GPIO_HIGH);
	if (ret)
		goto free_shtdwn;

	/* Initialize sleep GPIO (active-low) — pull HIGH for normal */
	ret = no_os_gpio_get_optional(&d->gpio_slp, init_param->gpio_slp);
	if (ret)
		goto free_shtdwn;

	ret = no_os_gpio_direction_output(d->gpio_slp, NO_OS_GPIO_HIGH);
	if (ret)
		goto free_slp;

	/* Wait for the device to power up */
	no_os_mdelay(1);
	d->power_mode = <DEVNAME>_POWER_NORMAL;

	/* Initialize SPI */
	ret = no_os_spi_init(&d->spi_desc, init_param->spi_init);
	if (ret)
		goto free_slp;

	/* Verify chip ID */
	ret = <devname>_reg_read(d, <DEVNAME>_REG_CHIP_ID2, &data);
	if (ret)
		goto free_spi;

	chip_id = data;

	ret = <devname>_reg_read(d, <DEVNAME>_REG_CHIP_ID1, &data);
	if (ret)
		goto free_spi;

	chip_id = (chip_id << 8) | data;

	if (chip_id != <DEVNAME>_CHIP_ID) {
		pr_err("Unexpected chip ID: 0x%04X\n", chip_id);
		ret = -ENODEV;
		goto free_spi;
	}

	/* Software reset */
	ret = <devname>_soft_reset(d);
	if (ret)
		goto free_spi;

	/* Initialize bandwidth GPIO */
	ret = no_os_gpio_get_optional(&d->gpio_bw,
				      init_param->gpio_bw_param);
	if (ret)
		goto free_spi;

	ret = no_os_gpio_direction_output(d->gpio_bw, NO_OS_GPIO_HIGH);
	if (ret)
		goto free_bw;

	/* Apply initial configuration */
	ret = <devname>_set_bandwidth(d, d->bandwidth);
	if (ret)
		goto free_bw;

	ret = <devname>_set_gain(d, d->gain);
	if (ret)
		goto free_bw;

	ret = <devname>_en_refbuf(d, d->refbuf_en);
	if (ret)
		goto free_bw;

	ret = <devname>_set_bias(d, d->bias);
	if (ret)
		goto free_bw;

	*dev = d;

	return 0;

free_bw:
	no_os_gpio_remove(d->gpio_bw);
free_spi:
	no_os_spi_remove(d->spi_desc);
free_slp:
	no_os_gpio_remove(d->gpio_slp);
free_shtdwn:
	no_os_gpio_remove(d->gpio_shtdwn);
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

	ret = no_os_gpio_remove(dev->gpio_bw);
	if (ret)
		return ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_slp);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_shtdwn);
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
 * @brief Read the current gain as a hardware value.
 */
static int <devname>_iio_read_hardwaregain(void *dev, char *buf,
					   uint32_t len,
					   const struct iio_ch_info *channel,
					   intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int32_t val;

	/* Convert gain enum to linear multiplier: 1 << gain_enum */
	val = 1 << desc-><devname>_dev->gain;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write a new gain value.
 */
static int <devname>_iio_write_hardwaregain(void *dev, char *buf,
					    uint32_t len,
					    const struct iio_ch_info *channel,
					    intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int32_t gain_val;
	enum <devname>_gain gain_enum;

	iio_parse_value(buf, IIO_VAL_INT, &gain_val, NULL);

	/*
	 * Map integer gain back to enum.
	 * E.g., 1->GAIN_1, 2->GAIN_2, 4->GAIN_4, etc.
	 */
	switch (gain_val) {
	case 1:
		gain_enum = <DEVNAME>_GAIN_1;
		break;
	case 2:
		gain_enum = <DEVNAME>_GAIN_2;
		break;
	case 4:
		gain_enum = <DEVNAME>_GAIN_4;
		break;
	case 8:
		gain_enum = <DEVNAME>_GAIN_8;
		break;
	case 16:
		gain_enum = <DEVNAME>_GAIN_16;
		break;
	case 32:
		gain_enum = <DEVNAME>_GAIN_32;
		break;
	case 64:
		gain_enum = <DEVNAME>_GAIN_64;
		break;
	case 128:
		gain_enum = <DEVNAME>_GAIN_128;
		break;
	default:
		return -EINVAL;
	}

	return <devname>_set_gain(desc-><devname>_dev, gain_enum);
}

/**
 * @brief Read the offset calibration value.
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int32_t val;

	val = desc-><devname>_dev->offset_nv;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write a new offset calibration value.
 */
static int <devname>_iio_write_offset(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return <devname>_set_offset(desc-><devname>_dev, (int64_t)val);
}

/**
 * @brief Read scale factor (always 1.0 for an amplifier pass-through).
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	/*
	 * For an amplifier, scale represents the mV-per-LSB factor
	 * of any internal ADC, or 1.0 if the device is purely analog.
	 * Adjust per your device's datasheet.
	 */
	val[0] = 1;
	val[1] = 0;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_ch_attrs[] = {
	{ .name = "hardwaregain",
	  .show = <devname>_iio_read_hardwaregain,
	  .store = <devname>_iio_write_hardwaregain },
	{ .name = "offset",
	  .show = <devname>_iio_read_offset,
	  .store = <devname>_iio_write_offset },
	{ .name = "scale",
	  .show = <devname>_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * Amplifier channels: IIO_VOLTAGE, ch_out = false (input side),
 * indexed by channel number.
 */
#define <DEVNAME>_IIO_CHANNEL(idx) \
	{ \
		.name = "voltage" #idx, \
		.ch_type = IIO_VOLTAGE, \
		.channel = (idx), \
		.attributes = <devname>_iio_ch_attrs, \
		.ch_out = false, \
		.indexed = true, \
	}

static struct iio_channel <devname>_iio_channels[] = {
	<DEVNAME>_IIO_CHANNEL(0),
	/* Add more channels as needed */
	END_ATTRIBUTES_ARRAY
};

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels) - 1,
	.debug_reg_read = (int32_t (*)())<devname>_reg_read,
	.debug_reg_write = (int32_t (*)())<devname>_reg_write,
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

INCS += $(DRIVERS)/amplifiers/<devname>/<devname>.h
SRCS += $(DRIVERS)/amplifiers/<devname>/<devname>.c
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
INCS += $(DRIVERS)/amplifiers/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/amplifiers/<devname>/iio_<devname>.c

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
	.device_id = ID_<DEVNAME>,
	.spi_init = (struct no_os_spi_init_param *)&<devname>_spi_ip,
	.avdd_v = 5000,
	.refbuf_en = true,
	.gain = <DEVNAME>_GAIN_1,
	.bias = <DEVNAME>_BIAS_DISABLE,
	.bandwidth = <DEVNAME>_BANDWIDTH_HIGH,
	.offset_nv = 0,
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
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	/* Set gain to 8x */
	ret = <devname>_set_gain(dev, <DEVNAME>_GAIN_8);
	if (ret)
		goto free_dev;

	pr_info("Gain set to 8x\r\n");

	/* Set bandwidth to low */
	ret = <devname>_set_bandwidth(dev, <DEVNAME>_BANDWIDTH_LOW);
	if (ret)
		goto free_dev;

	pr_info("Bandwidth set to low\r\n");

	while (1) {
		/* Application-specific processing */
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

The <DEVNAME> is a programmable gain instrumentation amplifier with an
SPI-compatible serial interface. It supports gain settings from 1x to
128x, sensor offset calibration, configurable bandwidth modes, and
low-power sleep/shutdown operation. <Add 2-3 more sentences from the
datasheet covering key specs and features.>

Applications
------------

* Sensor signal conditioning
* Precision instrumentation
* Data acquisition front-ends
* Weigh scales

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface and GPIO pins, verifies the
chip ID, performs a software reset, and applies the initial amplifier
configuration (gain, bias, bandwidth, reference buffer). The function
uses goto-based cleanup to release resources on failure.

Gain Configuration
~~~~~~~~~~~~~~~~~~

The ``<devname>_set_gain`` function programs the gain multiplexer
register to select a gain from 1x to 128x. Gain values are encoded
as a 3-bit field in the gain mux register.

Bias / Offset Calibration
~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_bias`` function selects the bias current source
(disabled, bandgap reference, or AVDD). The ``<devname>_set_offset``
function computes the raw calibration code and range from a requested
offset in nanovolts, then programs the offset calibration registers.

Bandwidth Configuration
~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_bandwidth`` function controls the bandwidth mode
via a dedicated GPIO pin. Low bandwidth is selected by driving the
pin HIGH; high bandwidth by driving it LOW.

Power Management
~~~~~~~~~~~~~~~~

The ``<devname>_set_slp_shtdwn_mode`` and ``<devname>_set_normal_mode``
functions control sleep and shutdown modes via GPIO pins.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .device_id = ID_<DEVNAME>,
           .spi_init = &spi_ip,
           .avdd_v = 5000,
           .refbuf_en = true,
           .gain = <DEVNAME>_GAIN_1,
           .bias = <DEVNAME>_BIAS_DISABLE,
           .bandwidth = <DEVNAME>_BANDWIDTH_HIGH,
           .offset_nv = 0,
       };

       struct <devname>_dev *dev;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_set_gain(dev, <DEVNAME>_GAIN_8);
       if (ret)
           goto cleanup;

       /* Amplifier is now configured at 8x gain */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each amplifier input as an
``IIO_VOLTAGE`` channel with ``hardwaregain``, ``offset``, and
``scale`` attributes. The IIO application can be used for real-time
gain and offset control.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/amplifiers/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/amplifiers/<devname>/README.rst
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
11. **GPIO patterns** -- use `no_os_gpio_get_optional()` for pins that
    may not be wired; `no_os_gpio_get()` for mandatory pins.
12. **Amplifier-specific** -- gain is the primary configuration; offset
    calibration, bias source, bandwidth, and power modes are common
    secondary controls.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> programmable gain instrumentation
amplifier with SPI interface, configurable gain (1x-128x), offset
calibration, bandwidth control, and sleep/shutdown power modes.

Signed-off-by: Your Name <your.name@analog.com>
```
