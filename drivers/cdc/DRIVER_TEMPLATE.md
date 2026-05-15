# Capacitance-to-Digital Converter (CDC) Driver Template

Reference driver: `drivers/cdc/ad7156/` (simple), `drivers/cdc/ad7746/` (advanced)

This template covers every file needed to add a new CDC driver to no-OS.
Replace `<devname>` with the part number (e.g., `ad7156`) and `<DEVNAME>`
with its uppercase form (e.g., `AD7156`) throughout.

---

## 1. Naming Conventions

CDC drivers use `_dev` for the device descriptor struct (standard
convention).

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ad7156.h`, `ad7156.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ad7156.h`, `iio_ad7156.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __AD7156_H__` |
| Init param struct | `<devname>_init_param` | `ad7156_init_param` |
| Device struct | `<devname>_dev` | `ad7156_dev` |
| IIO descriptor | `<devname>_iio_dev` | `ad7156_iio_dev` |
| Functions | `<devname>_<verb>()` | `ad7156_init()` |
| Address macro | `<DEVNAME>_ADDR(x)` | `AD7156_ADDR(x)` |
| Register defines | `<DEVNAME>_REG_<NAME>` | `AD7156_REG_STATUS` |
| Field masks | `<DEVNAME>_<FIELD>_MSK` | `AD7156_STATUS_RDY1_MSK` |
| Range constants | `<DEVNAME>_RANGE_<VAL>` | `AD7156_RANGE_2_PF` |

---

## 2. File Checklist

```
drivers/cdc/<devname>/
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

doc/sphinx/source/drivers/cdc/<devname>.rst   # Sphinx entry
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
#include "no_os_i2c.h"
#include "no_os_util.h"

/* ---- I2C Address -------------------------------------------------- */

/**
 * I2C base address.  x = A0 pin state (0-1).
 * Example: AD7156 base = 0x48.
 */
#define <DEVNAME>_ADDR(x)		(0x48 + (x))

/* ---- Timing / Constants ------------------------------------------- */

/** Max I2C frame size (register address + 2 data bytes). */
#define <DEVNAME>_FRAME_SIZE		3

/** Number of capacitance input channels. */
#define <DEVNAME>_NUM_CHANNELS		2

/** Chip ID expected from the chip ID register. */
#define <DEVNAME>_DEFAULT_ID		0x88

/** Reset command byte. */
#define <DEVNAME>_RESET_CMD		0xBF

/* ---- Register Addresses ------------------------------------------- */

#define <DEVNAME>_REG_STATUS		0x00
#define <DEVNAME>_REG_CH1_DATA_H	0x01
#define <DEVNAME>_REG_CH1_DATA_L	0x02
#define <DEVNAME>_REG_CH2_DATA_H	0x03
#define <DEVNAME>_REG_CH2_DATA_L	0x04
#define <DEVNAME>_REG_CH1_AVG_H		0x05
#define <DEVNAME>_REG_CH1_AVG_L		0x06
#define <DEVNAME>_REG_CH2_AVG_H		0x07
#define <DEVNAME>_REG_CH2_AVG_L		0x08
#define <DEVNAME>_REG_CH1_THRSH_H	0x09
#define <DEVNAME>_REG_CH1_THRSH_L	0x0A
#define <DEVNAME>_REG_CH1_SETUP		0x0B
#define <DEVNAME>_REG_CH2_THRSH_H	0x0C
#define <DEVNAME>_REG_CH2_THRSH_L	0x0D
#define <DEVNAME>_REG_CH2_SETUP		0x0E
#define <DEVNAME>_REG_CONFIG		0x0F
#define <DEVNAME>_REG_PWR_DWN_TMR	0x10
#define <DEVNAME>_REG_CH1_CAPDAC	0x11
#define <DEVNAME>_REG_CH2_CAPDAC	0x12
#define <DEVNAME>_REG_CHIP_ID		0x17

/* ---- Status Register Masks ---------------------------------------- */

#define <DEVNAME>_STATUS_PWR_DWN_MSK	NO_OS_BIT(7)
#define <DEVNAME>_STATUS_DAC_STEP2_MSK	NO_OS_BIT(6)
#define <DEVNAME>_STATUS_OUT2_MSK	NO_OS_BIT(5)
#define <DEVNAME>_STATUS_DAC_STEP1_MSK	NO_OS_BIT(4)
#define <DEVNAME>_STATUS_OUT1_MSK	NO_OS_BIT(3)
#define <DEVNAME>_STATUS_C1_C2_MSK	NO_OS_BIT(2)
#define <DEVNAME>_STATUS_RDY2_MSK	NO_OS_BIT(1)
#define <DEVNAME>_STATUS_RDY1_MSK	NO_OS_BIT(0)

/* ---- Channel Setup Register Masks --------------------------------- */

#define <DEVNAME>_SETUP_RANGE_MSK	NO_OS_GENMASK(7, 6)
#define <DEVNAME>_SETUP_HYST_MSK	NO_OS_BIT(4)
#define <DEVNAME>_SETUP_THR_MSK		NO_OS_GENMASK(3, 0)

/* ---- Configuration Register Masks --------------------------------- */

#define <DEVNAME>_CONFIG_THR_FIXED_MSK	NO_OS_BIT(7)
#define <DEVNAME>_CONFIG_THR_MD_MSK	NO_OS_GENMASK(6, 5)
#define <DEVNAME>_CONFIG_EN_CH1_MSK	NO_OS_BIT(4)
#define <DEVNAME>_CONFIG_EN_CH2_MSK	NO_OS_BIT(3)
#define <DEVNAME>_CONFIG_MD_MSK		NO_OS_GENMASK(2, 0)

/* ---- CAPDAC Register Masks ---------------------------------------- */

#define <DEVNAME>_CAPDAC_EN_MSK		NO_OS_BIT(7)
#define <DEVNAME>_CAPDAC_AUTO_MSK	NO_OS_BIT(6)
#define <DEVNAME>_CAPDAC_VAL_MSK	NO_OS_GENMASK(5, 0)

/* ---- Capacitive Input Range Options ------------------------------- */

#define <DEVNAME>_RANGE_2_PF		0
#define <DEVNAME>_RANGE_0_5_PF		1
#define <DEVNAME>_RANGE_1_PF		2
#define <DEVNAME>_RANGE_4_PF		3

/* ---- Threshold Mode Options --------------------------------------- */

#define <DEVNAME>_THR_MODE_NEGATIVE	0
#define <DEVNAME>_THR_MODE_POSITIVE	1
#define <DEVNAME>_THR_MODE_IN_WINDOW	2
#define <DEVNAME>_THR_MODE_OUT_WINDOW	3

/* ---- Conversion Mode Options -------------------------------------- */

#define <DEVNAME>_CONV_MODE_IDLE	0
#define <DEVNAME>_CONV_MODE_CONT	1
#define <DEVNAME>_CONV_MODE_SINGLE	2
#define <DEVNAME>_CONV_MODE_PWR_DWN	3

/* ---- Channel Identifiers ------------------------------------------ */

#define <DEVNAME>_CHANNEL1		1
#define <DEVNAME>_CHANNEL2		2

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters for <devname>_init().
 */
struct <devname>_init_param {
	/** I2C initialization parameters. */
	struct no_os_i2c_init_param i2c_init;
	/** Channel 1 capacitive input range (use <DEVNAME>_RANGE_*). */
	uint8_t ch1_range;
	/** Channel 2 capacitive input range (use <DEVNAME>_RANGE_*). */
	uint8_t ch2_range;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** I2C descriptor. */
	struct no_os_i2c_desc *i2c_desc;
	/** Channel 1 range in picofarads (cached). */
	float ch1_range;
	/** Channel 2 range in picofarads (cached). */
	float ch2_range;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Read one or more registers via I2C. */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t reg,
		       uint8_t *data, uint8_t len);

/** @brief Write one or more registers via I2C. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t reg,
			uint8_t *data, uint8_t len);

/** @brief Read a 16-bit capacitance sample from the selected channel. */
int <devname>_read_channel_data(struct <devname>_dev *dev, uint8_t channel,
				uint16_t *data);

/** @brief Read a capacitance sample and convert to picofarads. */
int <devname>_read_channel_capacitance(struct <devname>_dev *dev,
				       uint8_t channel, float *pf);

/** @brief Set the capacitive input range for a channel. */
int <devname>_set_range(struct <devname>_dev *dev, uint8_t channel,
			uint8_t range);

/** @brief Set the converter operating mode. */
int <devname>_set_power_mode(struct <devname>_dev *dev, uint8_t mode);

/** @brief Set the threshold mode and fixed/adaptive selection. */
int <devname>_set_threshold_mode(struct <devname>_dev *dev,
				 uint8_t thr_mode, uint8_t thr_fixed);

/** @brief Reset the device. */
int <devname>_reset(struct <devname>_dev *dev);

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
#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/**
 * @brief Read one or more device registers over I2C.
 * @param dev - Device descriptor.
 * @param reg - Starting register address.
 * @param data - Pointer to store read data.
 * @param len - Number of bytes to read.
 * @return 0 on success, negative error code otherwise.
 *
 * Performs a write of the register address (no stop bit) followed
 * by a read of the requested number of bytes (with stop bit).
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t reg,
		       uint8_t *data, uint8_t len)
{
	int ret;

	ret = no_os_i2c_write(dev->i2c_desc, &reg, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, data, len, 1);
}

/**
 * @brief Write one or more device registers over I2C.
 * @param dev - Device descriptor.
 * @param reg - Starting register address.
 * @param data - Pointer to data to write.
 * @param len - Number of data bytes to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t reg,
			uint8_t *data, uint8_t len)
{
	uint8_t buf[<DEVNAME>_FRAME_SIZE];
	uint8_t i;

	if (len > (<DEVNAME>_FRAME_SIZE - 1))
		return -EINVAL;

	buf[0] = reg;
	for (i = 0; i < len; i++)
		buf[i + 1] = data[i];

	return no_os_i2c_write(dev->i2c_desc, buf, len + 1, 1);
}

/**
 * @brief Read a 16-bit raw capacitance sample from the selected channel.
 * @param dev - Device descriptor.
 * @param channel - Channel number (<DEVNAME>_CHANNEL1 or <DEVNAME>_CHANNEL2).
 * @param data - Pointer to store the 16-bit result.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read_channel_data(struct <devname>_dev *dev, uint8_t channel,
				uint16_t *data)
{
	uint8_t reg_data[2];
	uint8_t reg_addr;
	int ret;

	reg_addr = (channel == <DEVNAME>_CHANNEL1) ?
		   <DEVNAME>_REG_CH1_DATA_H : <DEVNAME>_REG_CH2_DATA_H;

	ret = <devname>_reg_read(dev, reg_addr, reg_data, 2);
	if (ret)
		return ret;

	*data = ((uint16_t)reg_data[0] << 8) | reg_data[1];

	return 0;
}

/**
 * @brief Read a capacitance sample and convert to picofarads.
 * @param dev - Device descriptor.
 * @param channel - Channel number (<DEVNAME>_CHANNEL1 or <DEVNAME>_CHANNEL2).
 * @param pf - Pointer to store the capacitance value in picofarads.
 * @return 0 on success, negative error code otherwise.
 *
 * Conversion:
 *   The CDC output code ranges from 0x3000 (zero-scale) to 0xD000
 *   (full-scale).  Capacitance = ((code - 0x3000) * range_pF) / 0xA000.
 */
int <devname>_read_channel_capacitance(struct <devname>_dev *dev,
				       uint8_t channel, float *pf)
{
	uint16_t raw;
	float range;
	int ret;

	range = (channel == <DEVNAME>_CHANNEL1) ?
		dev->ch1_range : dev->ch2_range;

	ret = <devname>_read_channel_data(dev, channel, &raw);
	if (ret)
		return ret;

	if (raw < 0x3000)
		raw = 0x3000;
	else if (raw > 0xD000)
		raw = 0xD000;

	*pf = ((float)(raw - 0x3000) * range) / 0xA000;

	return 0;
}

/**
 * @brief Set the capacitive input range for a channel.
 * @param dev - Device descriptor.
 * @param channel - Channel number.
 * @param range - Range option (<DEVNAME>_RANGE_*).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_range(struct <devname>_dev *dev, uint8_t channel,
			uint8_t range)
{
	uint8_t reg_addr;
	uint8_t setup_reg;
	int ret;

	reg_addr = (channel == <DEVNAME>_CHANNEL1) ?
		   <DEVNAME>_REG_CH1_SETUP : <DEVNAME>_REG_CH2_SETUP;

	ret = <devname>_reg_read(dev, reg_addr, &setup_reg, 1);
	if (ret)
		return ret;

	setup_reg &= ~<DEVNAME>_SETUP_RANGE_MSK;
	setup_reg |= no_os_field_prep(<DEVNAME>_SETUP_RANGE_MSK, range);

	ret = <devname>_reg_write(dev, reg_addr, &setup_reg, 1);
	if (ret)
		return ret;

	/* Update the cached range value. */
	/* Map range code to picofarads. */
	float range_pf;
	switch (range) {
	case <DEVNAME>_RANGE_2_PF:
		range_pf = 2.0f;
		break;
	case <DEVNAME>_RANGE_0_5_PF:
		range_pf = 0.5f;
		break;
	case <DEVNAME>_RANGE_1_PF:
		range_pf = 1.0f;
		break;
	case <DEVNAME>_RANGE_4_PF:
		range_pf = 4.0f;
		break;
	default:
		return -EINVAL;
	}

	if (channel == <DEVNAME>_CHANNEL1)
		dev->ch1_range = range_pf;
	else
		dev->ch2_range = range_pf;

	return 0;
}

/**
 * @brief Set the converter operating mode.
 * @param dev - Device descriptor.
 * @param mode - Operating mode (<DEVNAME>_CONV_MODE_*).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_power_mode(struct <devname>_dev *dev, uint8_t mode)
{
	uint8_t config;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_CONFIG, &config, 1);
	if (ret)
		return ret;

	config &= ~<DEVNAME>_CONFIG_MD_MSK;
	config |= no_os_field_prep(<DEVNAME>_CONFIG_MD_MSK, mode);

	return <devname>_reg_write(dev, <DEVNAME>_REG_CONFIG, &config, 1);
}

/**
 * @brief Set the threshold mode and fixed/adaptive selection.
 * @param dev - Device descriptor.
 * @param thr_mode - Threshold mode (<DEVNAME>_THR_MODE_*).
 * @param thr_fixed - 0 for adaptive, 1 for fixed threshold.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_threshold_mode(struct <devname>_dev *dev,
				 uint8_t thr_mode, uint8_t thr_fixed)
{
	uint8_t config;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_CONFIG, &config, 1);
	if (ret)
		return ret;

	config &= ~(<DEVNAME>_CONFIG_THR_FIXED_MSK |
		     <DEVNAME>_CONFIG_THR_MD_MSK);
	if (thr_fixed)
		config |= <DEVNAME>_CONFIG_THR_FIXED_MSK;
	config |= no_os_field_prep(<DEVNAME>_CONFIG_THR_MD_MSK, thr_mode);

	return <devname>_reg_write(dev, <DEVNAME>_REG_CONFIG, &config, 1);
}

/**
 * @brief Reset the device via the I2C general-call reset command.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reset(struct <devname>_dev *dev)
{
	uint8_t cmd = <DEVNAME>_RESET_CMD;

	return no_os_i2c_write(dev->i2c_desc, &cmd, 1, 1);
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param dev - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *descriptor;
	uint8_t chip_id;
	int ret;

	descriptor = (struct <devname>_dev *)no_os_calloc(1,
							  sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_i2c_init(&descriptor->i2c_desc, &init_param->i2c_init);
	if (ret)
		goto free_desc;

	/* Verify chip ID. */
	ret = <devname>_reg_read(descriptor, <DEVNAME>_REG_CHIP_ID,
				 &chip_id, 1);
	if (ret)
		goto free_i2c;

	if (chip_id != <DEVNAME>_DEFAULT_ID) {
		ret = -ENODEV;
		goto free_i2c;
	}

	/* Set initial channel ranges. */
	ret = <devname>_set_range(descriptor, <DEVNAME>_CHANNEL1,
				  init_param->ch1_range);
	if (ret)
		goto free_i2c;

	ret = <devname>_set_range(descriptor, <DEVNAME>_CHANNEL2,
				  init_param->ch2_range);
	if (ret)
		goto free_i2c;

	*dev = descriptor;

	return 0;

free_i2c:
	no_os_i2c_remove(descriptor->i2c_desc);
free_desc:
	no_os_free(descriptor);

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

	ret = no_os_i2c_remove(dev->i2c_desc);
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
 * @struct <devname>_iio_dev
 * @brief  IIO-specific device descriptor.
 */
struct <devname>_iio_dev {
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
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_init_param *init_param);

/** @brief Free IIO descriptor resources. */
int <devname>_iio_remove(struct <devname>_iio_dev *desc);

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

/* ---------- Debug register access ---------------------------------- */

/**
 * @brief Debug register read callback.
 */
static int <devname>_iio_reg_read(struct <devname>_iio_dev *dev,
				  uint32_t reg, uint32_t *readval)
{
	uint8_t val;
	int ret;

	ret = <devname>_reg_read(dev-><devname>_dev, (uint8_t)reg, &val, 1);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

/**
 * @brief Debug register write callback.
 */
static int <devname>_iio_reg_write(struct <devname>_iio_dev *dev,
				   uint32_t reg, uint32_t writeval)
{
	uint8_t val = (uint8_t)writeval;

	return <devname>_reg_write(dev-><devname>_dev, (uint8_t)reg, &val, 1);
}

/* ---------- Attribute callbacks ------------------------------------ */

/**
 * @brief Read raw capacitance value (16-bit unsigned).
 *
 * Returns the raw register value.  The code range is 0x3000 to 0xD000.
 * To convert to picofarads: (raw - 0x3000) * scale.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_dev *iiodev = (struct <devname>_iio_dev *)dev;
	uint16_t raw;
	int32_t val;
	int ret;

	ret = <devname>_read_channel_data(iiodev-><devname>_dev,
					  channel->ch_num + 1, &raw);
	if (ret)
		return ret;

	/* Offset so zero-scale = 0 for IIO consumers. */
	val = (int32_t)raw - 0x3000;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor (picofarads per LSB).
 *
 * Scale = range_pF / 0xA000.
 * For the 2pF range: 2 / 40960 = 0.000048828125 pF/LSB.
 * Expressed as IIO_VAL_INT_PLUS_NANO: 0 + 48828 nF/LSB.
 *
 * Adjust the values below to match the device's full-scale range.
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	/*
	 * Default for 2pF range: 2 / 0xA000 = ~48.828 nF/LSB.
	 * Expressed as IIO_VAL_INT_PLUS_NANO.
	 */
	val[0] = 0;
	val[1] = 48828;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_NANO, 2, val);
}

/**
 * @brief Read offset value.
 *
 * The raw code has an inherent offset of 0x3000 (zero-scale).
 * Since the raw callback already subtracts 0x3000, offset is 0.
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_cap_attrs[] = {
	{ .name = "raw",    .show = <devname>_iio_read_raw },
	{ .name = "scale",  .show = <devname>_iio_read_scale },
	{ .name = "offset", .show = <devname>_iio_read_offset },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_scan = {
	.sign = 'u',          /* unsigned */
	.realbits = 16,       /* capacitance resolution */
	.storagebits = 16,    /* stored in 16-bit container */
	.shift = 0,           /* no shift needed */
	.is_big_endian = false,
};

/* ---------- Channel definition ------------------------------------- */

/**
 * Capacitance channels: IIO_CAPACITANCE, ch_out = false (input).
 * Two single-ended channels corresponding to CIN1 and CIN2.
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "capacitance0",
		.ch_type = IIO_CAPACITANCE,
		.indexed = true,
		.channel = 0,
		.ch_out = 0,
		.attributes = <devname>_iio_cap_attrs,
		.scan_type = &<devname>_scan,
	},
	{
		.name = "capacitance1",
		.ch_type = IIO_CAPACITANCE,
		.indexed = true,
		.channel = 1,
		.ch_out = 0,
		.attributes = <devname>_iio_cap_attrs,
		.scan_type = &<devname>_scan,
	},
};

/* ---------- Buffered read callback --------------------------------- */

/**
 * @brief Read capacitance samples into an IIO buffer.
 */
static int <devname>_iio_read_samples(void *dev, int16_t *buf,
				      uint32_t samples)
{
	struct <devname>_iio_dev *iiodev = (struct <devname>_iio_dev *)dev;
	uint16_t val;
	uint32_t i;
	int ret;

	for (i = 0; i < samples; i++) {
		/* Alternate between channels, or read active channel. */
		ret = <devname>_read_channel_data(
			iiodev-><devname>_dev,
			(i % <DEVNAME>_NUM_CHANNELS) + 1, &val);
		if (ret)
			return ret;

		buf[i] = (int16_t)val;
	}

	return samples;
}

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_device = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels),
	.read_dev = (int32_t (*)())<devname>_iio_read_samples,
	.debug_reg_read = (int32_t (*)())<devname>_iio_reg_read,
	.debug_reg_write = (int32_t (*)())<devname>_iio_reg_write,
};

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 */
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_init_param *init_param)
{
	struct <devname>_iio_dev *descriptor;
	int ret;

	descriptor = (struct <devname>_iio_dev *)no_os_calloc(1,
			sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = <devname>_init(&descriptor-><devname>_dev,
			     init_param-><devname>_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &<devname>_iio_device;

	*iio_dev = descriptor;

	return 0;

free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free IIO descriptor resources.
 */
int <devname>_iio_remove(struct <devname>_iio_dev *desc)
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
	$(INCLUDE)/no_os_i2c.h       \
	$(INCLUDE)/no_os_alloc.h     \
	$(INCLUDE)/no_os_irq.h       \
	$(INCLUDE)/no_os_list.h      \
	$(INCLUDE)/no_os_uart.h      \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_util.h      \
	$(INCLUDE)/no_os_units.h     \
	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_irq.c  \
	$(DRIVERS)/api/no_os_i2c.c  \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_util.c  \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/cdc/<devname>/<devname>.h
SRCS += $(DRIVERS)/cdc/<devname>/<devname>.c
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
INCS += $(DRIVERS)/cdc/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/cdc/<devname>/iio_<devname>.c

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
	.i2c_init = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 400000,
		.slave_address = <DEVNAME>_ADDR(0),
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA,
	},
	.ch1_range = <DEVNAME>_RANGE_2_PF,
	.ch2_range = <DEVNAME>_RANGE_2_PF,
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
	float cap_pf;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	/* Start continuous conversion. */
	ret = <devname>_set_power_mode(dev, <DEVNAME>_CONV_MODE_CONT);
	if (ret)
		goto free_dev;

	while (1) {
		ret = <devname>_read_channel_capacitance(dev,
							 <DEVNAME>_CHANNEL1,
							 &cap_pf);
		if (ret)
			goto free_dev;

		pr_info("CH1 Capacitance: %d fF\r\n",
			(int)(cap_pf * 1000.0f));

		ret = <devname>_read_channel_capacitance(dev,
							 <DEVNAME>_CHANNEL2,
							 &cap_pf);
		if (ret)
			goto free_dev;

		pr_info("CH2 Capacitance: %d fF\r\n",
			(int)(cap_pf * 1000.0f));

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
	struct <devname>_iio_dev *<devname>_iio;
	struct <devname>_iio_init_param <devname>_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	<devname>_iio_ip.<devname>_init_param = &<devname>_ip;
	ret = <devname>_iio_init(&<devname>_iio, &<devname>_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "<devname>",
			.dev = <devname>_iio,
			.dev_descriptor = <devname>_iio->iio_dev,
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
	<devname>_iio_remove(<devname>_iio);
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

#define I2C_DEVICE_ID	1
#define I2C_OPS		&max_i2c_ops
#define I2C_EXTRA	&max_i2c_extra

extern struct max_uart_init_param max_uart_extra;
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

struct max_i2c_init_param max_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
```

### 7.15 `projects/<project_name>/src/platform/maxim/platform_src.mk`

```makefile
INCS +=	$(PLATFORM_DRIVERS)/maxim_gpio.h      \
	$(PLATFORM_DRIVERS)/maxim_i2c.h       \
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h       \
	$(PLATFORM_DRIVERS)/maxim_irq.h       \
	$(PLATFORM_DRIVERS)/maxim_uart.h      \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
	$(PLATFORM_DRIVERS)/maxim_gpio.c      \
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

The <DEVNAME> is a capacitance-to-digital converter with a <resolution>-bit
sigma-delta CDC that measures capacitance with a precision of <precision> fF.
It communicates via I2C at up to 400 kHz and features <num_channels>
capacitance input channels with configurable input ranges (0.5 pF to 4 pF).
<Add 2-3 more sentences from the datasheet covering key specs: conversion
modes, threshold/alert features, CAPDAC offset calibration, power
consumption, package options.>

Applications
------------

* Proximity sensing
* Level sensing
* Liquid flow monitoring
* Touch-screen controllers
* Button/switch replacement
* Position sensing

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the I2C communication interface, verifies the
chip ID register, and sets the initial capacitive input range for both
channels.  Error handling releases any allocated resources on failure.

Capacitance Reading
~~~~~~~~~~~~~~~~~~~

The ``<devname>_read_channel_data`` function reads the raw 16-bit
capacitance code from the selected channel.  The
``<devname>_read_channel_capacitance`` function converts this raw code
to picofarads using: ``(raw - 0x3000) * range_pF / 0xA000``.

The CDC output range is 0x3000 (zero-scale) to 0xD000 (full-scale).

Range Configuration
~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_range`` function programs the capacitive input
range for a channel.  Available ranges are 0.5 pF, 1 pF, 2 pF,
and 4 pF.

Alert / Threshold Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_threshold_mode`` function selects between
adaptive and fixed threshold modes and configures the comparator
output polarity (positive, negative, in-window, or out-of-window).

Power Management
~~~~~~~~~~~~~~~~

The ``<devname>_set_power_mode`` function sets the converter to idle,
continuous conversion, single conversion, or power-down mode.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_i2c.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .i2c_init = {
               .device_id     = 1,
               .max_speed_hz  = 400000,
               .slave_address = <DEVNAME>_ADDR(0),
               .platform_ops  = &max_i2c_ops,
               .extra         = &i2c_extra,
           },
           .ch1_range = <DEVNAME>_RANGE_2_PF,
           .ch2_range = <DEVNAME>_RANGE_2_PF,
       };

       struct <devname>_dev *dev;
       float cap_pf;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_set_power_mode(dev, <DEVNAME>_CONV_MODE_CONT);
       if (ret)
           goto cleanup;

       ret = <devname>_read_channel_capacitance(dev,
                                                <DEVNAME>_CHANNEL1,
                                                &cap_pf);
       if (ret)
           goto cleanup;

       /* cap_pf holds the capacitance in picofarads */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes ``IIO_CAPACITANCE`` channels with
``raw``, ``scale`` (picofarads per LSB), and ``offset`` attributes.
Debug register read/write is also supported.  The IIO application can
be used for continuous capacitance monitoring.

IIO Initialization Example
---------------------------

.. code-block:: C

   int iio_example_main()
   {
       int ret;
       struct <devname>_iio_dev *iio_dev;
       struct <devname>_iio_init_param iio_ip;
       struct iio_app_desc *app;
       struct iio_app_init_param app_init_param = {0};

       iio_ip.<devname>_init_param = &<devname>_ip;
       ret = <devname>_iio_init(&iio_dev, &iio_ip);
       if (ret)
           return ret;

       struct iio_app_device iio_devices[] = {
           {
               .name = "<devname>",
               .dev = iio_dev,
               .dev_descriptor = iio_dev->iio_dev,
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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/cdc/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/cdc/<devname>/README.rst
```

---

## 9. Key Conventions

1. **`_dev` not `_desc`** -- CDC drivers use `<devname>_dev` for the
   device descriptor struct (standard convention, unlike temperature
   sensor drivers which use `_desc`).
2. **I2C communication** -- use `no_os_i2c_write()` and
   `no_os_i2c_read()` with the stop-bit parameter:
   - Write register address: `no_os_i2c_write(..., 1, 0)` (no stop)
   - Read data: `no_os_i2c_read(..., len, 1)` (with stop)
   - Write data: `no_os_i2c_write(..., len + 1, 1)` (with stop)
3. **Big-endian data** -- multi-byte register data is transmitted
   MSB-first over I2C.  Reconstruct with
   `((uint16_t)buf[0] << 8) | buf[1]`.
4. **Address macro** -- `<DEVNAME>_ADDR(x)` where `x` is the pin
   state for configurable I2C address.
5. **Capacitance conversion** -- raw code range 0x3000 to 0xD000.
   Capacitance (pF) = `(raw - 0x3000) * range_pF / 0xA000`.
6. **Register masks** -- use `NO_OS_BIT()` for single-bit fields and
   `NO_OS_GENMASK()` for multi-bit fields.  Use `no_os_field_get()`
   and `no_os_field_prep()` to extract and insert field values.
7. **IIO channel type** -- `IIO_CAPACITANCE` with `ch_out = 0`
   (input).  Use `indexed = true` for multi-channel devices.
8. **IIO scale** -- expressed as `IIO_VAL_INT_PLUS_NANO` representing
   picofarads per LSB (e.g., 0 + 48828 for ~48.828 nF/LSB at 2 pF
   range).
9. **Chip ID verification** -- `init()` should read the chip ID
   register and return `-ENODEV` if the value does not match the
   expected constant.
10. All other conventions (C++ compat, tabs, Doxygen, `no_os_calloc`,
    goto cleanup, negative error codes, etc.) match the ADC template.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> capacitance-to-digital converter with
<resolution>-bit CDC and I2C interface.

Signed-off-by: Your Name <your.name@analog.com>
```
