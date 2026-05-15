# Real-Time Clock Driver Template

Reference drivers: `drivers/rtc/max31343/`, `drivers/rtc/pcf85263/`

This template covers every file needed to add a new RTC driver to no-OS.
Replace `<devname>` with the part number (e.g., `max31343`) and
`<DEVNAME>` with its uppercase form (e.g., `MAX31343`) throughout.

---

## 1. Naming Conventions

RTC drivers use `_desc` instead of `_dev` for the device descriptor
struct.

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `max31343.h`, `max31343.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_max31343.h`, `iio_max31343.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __MAX31343_H__` |
| Init param struct | `<devname>_init_param` | `max31343_init_param` |
| Device struct | `<devname>_desc` (**not** `_dev`) | `max31343_desc` |
| Time stamp struct | `<devname>_time_stamp` | `max31343_time_stamp` |
| Functions | `<devname>_<verb>()` | `max31343_init()` |
| Register defines | `<DEVNAME>_REG_<NAME>` | `MAX31343_REG_SECONDS` |
| Field masks | `<DEVNAME>_<FIELD>_MASK` | `MAX31343_ENOSC_MASK` |
| Status bits | `<DEVNAME>_<FLAG>` | `MAX31343_STATUS_OSF` |

---

## 2. File Checklist

```
drivers/rtc/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
    iio_<devname>.h          # IIO header (optional, uncommon for RTCs)
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

doc/sphinx/source/drivers/rtc/<devname>.rst   # Sphinx entry
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
 * I2C address.  Most RTCs have a fixed address.
 * Example: MAX31343 = 0x68, PCF85263 = 0x51.
 */
#define <DEVNAME>_I2C_ADDRESS		0x68

/* ---- Register Addresses: Time/Date -------------------------------- */

#define <DEVNAME>_REG_SECONDS		0x06
#define <DEVNAME>_REG_MINUTES		0x07
#define <DEVNAME>_REG_HOURS		0x08
#define <DEVNAME>_REG_DAY		0x09
#define <DEVNAME>_REG_DATE		0x0A
#define <DEVNAME>_REG_MONTH		0x0B
#define <DEVNAME>_REG_YEAR		0x0C

/* ---- Register Addresses: Alarm 1 ---------------------------------- */

#define <DEVNAME>_REG_ALM1_SEC		0x0D
#define <DEVNAME>_REG_ALM1_MIN		0x0E
#define <DEVNAME>_REG_ALM1_HRS		0x0F
#define <DEVNAME>_REG_ALM1_DAY_DATE	0x10
#define <DEVNAME>_REG_ALM1_MON		0x11
#define <DEVNAME>_REG_ALM1_YEAR	0x12

/* ---- Register Addresses: Alarm 2 ---------------------------------- */

#define <DEVNAME>_REG_ALM2_MIN		0x13
#define <DEVNAME>_REG_ALM2_HRS		0x14
#define <DEVNAME>_REG_ALM2_DAY_DATE	0x15

/* ---- Register Addresses: Control/Status --------------------------- */

#define <DEVNAME>_REG_STATUS		0x00
#define <DEVNAME>_REG_INT_EN		0x01
#define <DEVNAME>_REG_RTC_RESET	0x02
#define <DEVNAME>_REG_CFG1		0x03
#define <DEVNAME>_REG_CFG2		0x04
#define <DEVNAME>_REG_TIMER_CFG	0x05
#define <DEVNAME>_REG_TIMER_COUNT	0x16
#define <DEVNAME>_REG_TIMER_INIT	0x17

/* ---- Register Addresses: Power / Trickle Charger ------------------ */

#define <DEVNAME>_REG_PWR_MGMT		0x18
#define <DEVNAME>_REG_TRICKLE		0x19

/* ---- Register Addresses: Temperature ------------------------------ */

#define <DEVNAME>_REG_TEMP_MSB		0x1A
#define <DEVNAME>_REG_TEMP_LSB		0x1B
#define <DEVNAME>_REG_TS_CFG		0x1C

/* ---- Status Register Bits ----------------------------------------- */

#define <DEVNAME>_STATUS_A1F		NO_OS_BIT(0)
#define <DEVNAME>_STATUS_A2F		NO_OS_BIT(1)
#define <DEVNAME>_STATUS_TIF		NO_OS_BIT(2)
#define <DEVNAME>_STATUS_TSF		NO_OS_BIT(3)
#define <DEVNAME>_STATUS_PFAIL		NO_OS_BIT(5)
#define <DEVNAME>_STATUS_OSF		NO_OS_BIT(6)

/* ---- Interrupt Enable Bits ---------------------------------------- */

#define <DEVNAME>_INT_A1IE		NO_OS_BIT(0)
#define <DEVNAME>_INT_A2IE		NO_OS_BIT(1)
#define <DEVNAME>_INT_TIE		NO_OS_BIT(2)
#define <DEVNAME>_INT_TSIE		NO_OS_BIT(3)

/* ---- Config 1 Bits ------------------------------------------------ */

#define <DEVNAME>_CFG1_ENOSC_MASK	NO_OS_BIT(1)
#define <DEVNAME>_CFG1_I2C_TIMEOUT_MASK	NO_OS_BIT(3)
#define <DEVNAME>_CFG1_DATA_RET_MASK	NO_OS_BIT(4)

/* ---- Config 2 Bits ------------------------------------------------ */

#define <DEVNAME>_CFG2_SQW_HZ_MASK	NO_OS_GENMASK(2, 0)
#define <DEVNAME>_CFG2_CLKO_HZ_MASK	NO_OS_GENMASK(5, 3)
#define <DEVNAME>_CFG2_ENCLKO_MASK	NO_OS_BIT(7)

/* ---- Reset Register ----------------------------------------------- */

#define <DEVNAME>_SWRST_MASK		NO_OS_BIT(0)

/* ---- Power Management Bits ---------------------------------------- */

#define <DEVNAME>_PWR_DMAN_SEL_MASK	NO_OS_BIT(2)
#define <DEVNAME>_PWR_D_VBACK_MASK	NO_OS_BIT(3)
#define <DEVNAME>_PWR_PFVT_MASK		NO_OS_GENMASK(5, 4)

/* ---- Trickle Charger Bits ----------------------------------------- */

#define <DEVNAME>_TRICKLE_D_MASK	NO_OS_GENMASK(3, 0)
#define <DEVNAME>_TRICKLE_TCHE_MASK	NO_OS_GENMASK(5, 4)

/* ---- Timer Config Bits -------------------------------------------- */

#define <DEVNAME>_TIMER_TFS_MASK	NO_OS_GENMASK(1, 0)
#define <DEVNAME>_TIMER_TRPT_MASK	NO_OS_BIT(2)
#define <DEVNAME>_TIMER_TPAUSE_MASK	NO_OS_BIT(3)
#define <DEVNAME>_TIMER_TE_MASK		NO_OS_BIT(4)

/* ---- BCD Masks for Time Registers --------------------------------- */

#define <DEVNAME>_SEC_MASK		NO_OS_GENMASK(6, 0)
#define <DEVNAME>_MIN_MASK		NO_OS_GENMASK(6, 0)
#define <DEVNAME>_HR_MASK		NO_OS_GENMASK(5, 0)
#define <DEVNAME>_DATE_MASK		NO_OS_GENMASK(5, 0)
#define <DEVNAME>_MON_MASK		NO_OS_GENMASK(4, 0)
#define <DEVNAME>_CENTURY_MASK		NO_OS_BIT(7)

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_time_stamp
 * @brief  Structure holding time and date parameters.
 *
 * All time fields are stored in binary (not BCD).
 * BCD conversion happens inside the driver during I2C read/write.
 */
struct <devname>_time_stamp {
	/** Seconds [0-59]. */
	uint8_t		sec;
	/** Minutes [0-59]. */
	uint8_t		min;
	/** Hours [0-23] (24h mode). */
	uint8_t		hr;
	/** Day of month [1-31]. */
	uint8_t		day;
	/** Month [1-12]. */
	uint8_t		mon;
	/** Year [2000-2199]. */
	uint16_t	year;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters for <devname>_init().
 */
struct <devname>_init_param {
	/** I2C initialization parameters (pointer). */
	struct no_os_i2c_init_param	*i2c_init;
	/** Enable backup battery switching. */
	uint8_t				battery_en;
};

/**
 * @struct <devname>_desc
 * @brief  Device descriptor -- allocated by <devname>_init().
 *
 * Note: RTC drivers use _desc (not _dev).
 */
struct <devname>_desc {
	/** I2C descriptor. */
	struct no_os_i2c_desc		*i2c_desc;
	/** Battery switching enabled. */
	uint8_t				battery_en;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Read a single register. */
int <devname>_reg_read(struct <devname>_desc *desc, uint8_t reg_addr,
		       uint8_t *reg_data);

/** @brief Write a single register. */
int <devname>_reg_write(struct <devname>_desc *desc, uint8_t reg_addr,
			uint8_t reg_data);

/** @brief Update specific register bits (read-modify-write). */
int <devname>_update_bits(struct <devname>_desc *desc, uint8_t reg_addr,
			  uint8_t mask, uint8_t reg_data);

/** @brief Set the current time and date. */
int <devname>_set_time_stamp(struct <devname>_desc *desc,
			     struct <devname>_time_stamp ts);

/** @brief Read the current time and date. */
int <devname>_read_time_stamp(struct <devname>_desc *desc,
			      struct <devname>_time_stamp *ts);

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_desc **desc,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_desc *desc);

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

#include <errno.h>
#include "<devname>.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"

/**
 * @brief Read a single device register over I2C.
 * @param desc     - Device descriptor.
 * @param reg_addr - Register address.
 * @param reg_data - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 *
 * RTC registers are single-byte.  The register address is written
 * without a stop condition, then one byte is read with a stop.
 */
int <devname>_reg_read(struct <devname>_desc *desc, uint8_t reg_addr,
		       uint8_t *reg_data)
{
	int ret;

	ret = no_os_i2c_write(desc->i2c_desc, &reg_addr, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(desc->i2c_desc, reg_data, 1, 1);
}

/**
 * @brief Write a single device register over I2C.
 * @param desc     - Device descriptor.
 * @param reg_addr - Register address.
 * @param reg_data - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_desc *desc, uint8_t reg_addr,
			uint8_t reg_data)
{
	uint8_t buf[2];

	buf[0] = reg_addr;
	buf[1] = reg_data;

	return no_os_i2c_write(desc->i2c_desc, buf, 2, 1);
}

/**
 * @brief Update specific bits in a register (read-modify-write).
 * @param desc     - Device descriptor.
 * @param reg_addr - Register address.
 * @param mask     - Bitmask of field to update.
 * @param reg_data - New value (pre-shifted into position).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_update_bits(struct <devname>_desc *desc, uint8_t reg_addr,
			  uint8_t mask, uint8_t reg_data)
{
	uint8_t data;
	int ret;

	ret = <devname>_reg_read(desc, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data;

	return <devname>_reg_write(desc, reg_addr, data);
}

/**
 * @brief Set the current time and date.
 * @param desc - Device descriptor.
 * @param ts   - Time stamp to write (binary values, not BCD).
 * @return 0 on success, negative error code otherwise.
 *
 * Time fields are converted to BCD before writing to registers.
 * For devices with a century bit (year >= 2100), bit 7 of the
 * month register is set.
 */
int <devname>_set_time_stamp(struct <devname>_desc *desc,
			     struct <devname>_time_stamp ts)
{
	uint8_t data;
	uint8_t century;
	int ret;

	century = (ts.year >= 2100) ? 1 : 0;

	data = no_os_bin2bcd(ts.sec);
	ret = <devname>_reg_write(desc, <DEVNAME>_REG_SECONDS, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.min);
	ret = <devname>_reg_write(desc, <DEVNAME>_REG_MINUTES, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.hr);
	ret = <devname>_reg_write(desc, <DEVNAME>_REG_HOURS, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.day);
	ret = <devname>_reg_write(desc, <DEVNAME>_REG_DATE, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.mon);
	if (century)
		data |= <DEVNAME>_CENTURY_MASK;
	ret = <devname>_reg_write(desc, <DEVNAME>_REG_MONTH, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.year - (century ? 2100 : 2000));
	ret = <devname>_reg_write(desc, <DEVNAME>_REG_YEAR, data);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read the current time and date.
 * @param desc - Device descriptor.
 * @param ts   - Pointer to store time stamp (binary values).
 * @return 0 on success, negative error code otherwise.
 *
 * BCD values from registers are converted to binary.  The century
 * bit in the month register determines whether year base is 2000
 * or 2100.
 */
int <devname>_read_time_stamp(struct <devname>_desc *desc,
			      struct <devname>_time_stamp *ts)
{
	uint8_t data;
	uint8_t century;
	int ret;

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_SECONDS, &data);
	if (ret)
		return ret;
	ts->sec = no_os_bcd2bin(data & <DEVNAME>_SEC_MASK);

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_MINUTES, &data);
	if (ret)
		return ret;
	ts->min = no_os_bcd2bin(data & <DEVNAME>_MIN_MASK);

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_HOURS, &data);
	if (ret)
		return ret;
	ts->hr = no_os_bcd2bin(data & <DEVNAME>_HR_MASK);

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_DATE, &data);
	if (ret)
		return ret;
	ts->day = no_os_bcd2bin(data & <DEVNAME>_DATE_MASK);

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_MONTH, &data);
	if (ret)
		return ret;
	century = data & <DEVNAME>_CENTURY_MASK;
	ts->mon = no_os_bcd2bin(data & <DEVNAME>_MON_MASK);

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_YEAR, &data);
	if (ret)
		return ret;
	ts->year = no_os_bcd2bin(data) + (century ? 2100 : 2000);

	return 0;
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param desc       - Pointer to descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 *
 * Performs:
 *   1. Allocate descriptor with no_os_calloc().
 *   2. Initialise I2C communication.
 *   3. Deassert software reset.
 *   4. Enable the oscillator.
 *   5. Disable all interrupt sources.
 */
int <devname>_init(struct <devname>_desc **desc,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_desc *descriptor;
	int ret;

	descriptor = (struct <devname>_desc *)no_os_calloc(1,
							   sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_i2c_init(&descriptor->i2c_desc, init_param->i2c_init);
	if (ret)
		goto free_desc;

	descriptor->battery_en = init_param->battery_en;

	/* Deassert software reset. */
	ret = <devname>_update_bits(descriptor, <DEVNAME>_REG_RTC_RESET,
				    <DEVNAME>_SWRST_MASK, 0);
	if (ret)
		goto free_i2c;

	/* Enable oscillator. */
	ret = <devname>_update_bits(descriptor, <DEVNAME>_REG_CFG1,
				    <DEVNAME>_CFG1_ENOSC_MASK,
				    no_os_field_prep(<DEVNAME>_CFG1_ENOSC_MASK,
						    1));
	if (ret)
		goto free_i2c;

	/* Disable all interrupts. */
	ret = <devname>_reg_write(descriptor, <DEVNAME>_REG_INT_EN, 0);
	if (ret)
		goto free_i2c;

	*desc = descriptor;

	return 0;

free_i2c:
	no_os_i2c_remove(descriptor->i2c_desc);
free_desc:
	no_os_free(descriptor);

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

	ret = no_os_i2c_remove(desc->i2c_desc);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
```

---

## 5. IIO Header (`iio_<devname>.h`)

> **Note:** IIO support is uncommon for RTC drivers.  Neither the
> max31343 nor pcf85263 reference drivers include IIO files.  If your
> device also exposes an on-chip temperature sensor, IIO can be used
> for that channel.  Otherwise this section can be skipped.

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
	struct <devname>_desc *<devname>_desc;
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

> **Note:** This skeleton exposes the on-chip temperature sensor (if
> present) as an `IIO_TEMP` channel.  If the device has no temperature
> sensor, IIO files should not be created.

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
 * @brief Read raw temperature from on-chip sensor.
 *
 * Reads the 10-bit temperature value from TEMP_MSB/TEMP_LSB registers.
 * Resolution: 0.25 degrees C per LSB.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = (struct <devname>_iio_desc *)dev;
	uint8_t msb, lsb;
	int32_t val;
	int ret;

	ret = <devname>_reg_read(desc-><devname>_desc,
				 <DEVNAME>_REG_TEMP_MSB, &msb);
	if (ret)
		return ret;

	ret = <devname>_reg_read(desc-><devname>_desc,
				 <DEVNAME>_REG_TEMP_LSB, &lsb);
	if (ret)
		return ret;

	val = (int8_t)msb * 4 + (lsb >> 6);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor (milliCelsius per LSB = 250 m°C).
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val = 250;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_raw },
	{ .name = "scale", .show = <devname>_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel definition ------------------------------------- */

static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "temp",
		.ch_type = IIO_TEMP,
		.ch_out = 0,
		.attributes = <devname>_iio_attrs,
	},
};

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels),
};

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 */
int <devname>_iio_init(struct <devname>_iio_desc **desc,
		       struct <devname>_iio_init_param *init_param)
{
	struct <devname>_iio_desc *descriptor;
	int ret;

	descriptor = (struct <devname>_iio_desc *)no_os_calloc(1,
			sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = <devname>_init(&descriptor-><devname>_desc,
			     init_param-><devname>_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &<devname>_iio_dev;

	*desc = descriptor;

	return 0;

free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free IIO descriptor resources.
 */
int <devname>_iio_remove(struct <devname>_iio_desc *desc)
{
	int ret;

	ret = <devname>_remove(desc-><devname>_desc);
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

INCS += $(DRIVERS)/rtc/<devname>/<devname>.h
SRCS += $(DRIVERS)/rtc/<devname>/<devname>.c
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
INCS += $(DRIVERS)/rtc/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/rtc/<devname>/iio_<devname>.c

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
	.i2c_init = &(struct no_os_i2c_init_param) {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 400000,
		.slave_address = <DEVNAME>_I2C_ADDRESS,
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA,
	},
	.battery_en = 1,
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
	struct <devname>_desc *desc;
	struct <devname>_time_stamp ts;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&desc, &<devname>_ip);
	if (ret)
		goto error;

	/* Optionally set an initial time. */
	ts.sec  = 0;
	ts.min  = 30;
	ts.hr   = 12;
	ts.day  = 15;
	ts.mon  = 6;
	ts.year = 2025;
	ret = <devname>_set_time_stamp(desc, ts);
	if (ret)
		goto free_dev;

	while (1) {
		ret = <devname>_read_time_stamp(desc, &ts);
		if (ret)
			goto free_dev;

		pr_info("Time: %02d:%02d:%02d  Date: %04d-%02d-%02d\r\n",
			ts.hr, ts.min, ts.sec,
			ts.year, ts.mon, ts.day);
		no_os_mdelay(1000);
	}

free_dev:
	<devname>_remove(desc);
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

The <DEVNAME> is a low-power real-time clock with I2C interface.  It
maintains seconds, minutes, hours, day, date, month, and year with
automatic leap year correction.  <Add 2-3 more sentences from the
datasheet covering key features: alarm outputs, square-wave output,
trickle charger, backup battery switching, temperature compensation,
power-fail detection.>

Applications
------------

* Battery-backed timekeeping
* Industrial data loggers
* Utility meters
* Building automation
* Medical instruments

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the I2C communication interface, deasserts the
software reset, enables the oscillator, and disables all interrupts.
Error handling uses goto-based cleanup to release resources on failure.

Time/Date
~~~~~~~~~

The ``<devname>_set_time_stamp`` function converts binary time values
to BCD and writes them to the time registers (seconds through year).
The century bit in the month register handles years 2100-2199.

The ``<devname>_read_time_stamp`` function reads BCD values from the
time registers and converts them to binary, masking off unused bits
(e.g., oscillator-stop flag in the seconds register).

Alarm Configuration
~~~~~~~~~~~~~~~~~~~

Alarm registers (ALM1, ALM2) can be programmed via
``<devname>_reg_write`` with BCD-encoded match values.  The interrupt
enable register controls which alarm flags trigger the interrupt output.

Trickle Charger
~~~~~~~~~~~~~~~

The trickle charge register configures charging of a backup
supercapacitor or rechargeable battery via a selectable diode and
resistor path.  Use ``<devname>_update_bits`` to set the trickle
register fields.

Power Management
~~~~~~~~~~~~~~~~

The power management register controls backup battery switching mode
(automatic or manual) and power-fail voltage threshold.  The
``battery_en`` field in ``<devname>_init_param`` enables or disables
battery switchover during initialisation.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_i2c.h"

   int main()
   {
       struct no_os_i2c_init_param i2c_ip = {
           .device_id     = 1,
           .max_speed_hz  = 400000,
           .slave_address = <DEVNAME>_I2C_ADDRESS,
           .platform_ops  = &max_i2c_ops,
           .extra         = &i2c_extra,
       };

       struct <devname>_init_param ip = {
           .i2c_init   = &i2c_ip,
           .battery_en = 1,
       };

       struct <devname>_desc *desc;
       struct <devname>_time_stamp ts;
       int ret;

       ret = <devname>_init(&desc, &ip);
       if (ret)
           return ret;

       ret = <devname>_read_time_stamp(desc, &ts);
       if (ret)
           goto cleanup;

       /* ts now holds the current time/date in binary */

   cleanup:
       <devname>_remove(desc);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/rtc/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/rtc/<devname>/README.rst
```

---

## 9. Key Conventions

1. **`_desc` not `_dev`** -- RTC drivers use `<devname>_desc` for the
   device descriptor struct.
2. **I2C communication** -- use `no_os_i2c_write()` and
   `no_os_i2c_read()` with the stop-bit parameter:
   - Write register address: `no_os_i2c_write(..., 1, 0)` (no stop)
   - Read data: `no_os_i2c_read(..., 1, 1)` (with stop)
   - Write data: `no_os_i2c_write(..., 2, 1)` (with stop)
3. **BCD encoding** -- time registers store values in BCD format.
   Use `no_os_bin2bcd()` when writing and `no_os_bcd2bin()` when
   reading.  These functions are declared in `no_os_util.h`.
4. **Bit masking on read** -- mask off reserved/flag bits before BCD
   conversion (e.g., `data & 0x7F` for seconds to clear bit 7).
5. **Century handling** -- bit 7 of the month register indicates
   years >= 2100.  Year is stored as a 2-digit BCD offset from the
   base (2000 or 2100).
6. **Fixed I2C address** -- most RTCs have a fixed address (no
   configurable address pins), so use a simple `#define` rather than
   an `_ADDR(x)` macro.
7. **Read-modify-write** -- provide an `_update_bits()` helper for
   setting individual fields in control/config registers.
8. **Pointer-based init_param** -- `init_param` is passed as a
   pointer (`*init_param`), and `i2c_init` within it is also a
   pointer.  This matches the recommended pattern.
9. **Goto cleanup in init** -- use labeled goto for multi-stage error
   cleanup: `free_i2c` then `free_desc`.
10. All other conventions (C++ compat with explicit `void*` casts,
    tabs for indentation, Doxygen comments, `no_os_calloc` /
    `no_os_free`, `NO_OS_BIT()` / `NO_OS_GENMASK()`, negative error
    codes) match the general no-OS driver guide.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> real-time clock with I2C interface,
BCD time encoding, alarm configuration, and trickle charger control.

Signed-off-by: Your Name <your.name@analog.com>
```
