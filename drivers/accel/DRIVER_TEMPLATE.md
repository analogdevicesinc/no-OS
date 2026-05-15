# Accelerometer Driver Template

Reference driver: `drivers/accel/adxl313/`

This template covers every file needed to add a new accelerometer driver to
no-OS. Replace `<devname>` with the part number (e.g., `adxl313`) and
`<DEVNAME>` with its uppercase form (e.g., `ADXL313`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adxl313.h`, `adxl313.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_adxl313.h`, `iio_adxl313.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADXL313_H__` |
| Init param struct | `<devname>_init_param` | `adxl313_init_param` |
| Device struct | `<devname>_dev` | `adxl313_dev` |
| IIO descriptor | `<devname>_iio_dev` | `adxl313_iio_dev` |
| Functions | `<devname>_<verb>()` | `adxl313_init()` |
| Static helpers | `_<devname>_<verb>()` | `_adxl313_read_reg()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADXL313` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `ADXL313_DATA_FORMAT_RANGE` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADXL313_ODR_100HZ` |

---

## 2. File Checklist

```
drivers/accel/<devname>/
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

doc/sphinx/source/drivers/accel/<devname>.rst   # Sphinx entry
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
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"

/* SPI command bits */
#define <DEVNAME>_SPI_READ			(0x80)
#define <DEVNAME>_SPI_WRITE			(0x00)
#define <DEVNAME>_SPI_MULTIBIT			NO_OS_BIT(6)

/* ---------------- Register Map ----------------------------------------- */

#define <DEVNAME>_REG_DEVID			0x00
#define <DEVNAME>_REG_SOFT_RESET		0x18
#define <DEVNAME>_REG_OFS_AXIS(index)		(0x1E + (index))
#define <DEVNAME>_REG_BW_RATE			0x2C
#define <DEVNAME>_REG_POWER_CTL			0x2D
#define <DEVNAME>_REG_INT_EN			0x2E
#define <DEVNAME>_REG_INT_MAP			0x2F
#define <DEVNAME>_REG_INT_SRC			0x30
#define <DEVNAME>_REG_DATA_FORMAT		0x31
#define <DEVNAME>_REG_DATA_AXIS(index)		(0x32 + ((index) * 2))
#define <DEVNAME>_REG_FIFO_CTL			0x38
#define <DEVNAME>_REG_FIFO_STATUS		0x39

/* Product / revision ID expected values */
#define <DEVNAME>_DEVID				0xXX

/* Reset key */
#define <DEVNAME>_RESET_KEY			0x52

/* ---------------- Field Masks ------------------------------------------ */

/** Configuration field masks -- use NO_OS_BIT() for single bits,
 *  NO_OS_GENMASK(hi, lo) for multi-bit fields. */
#define <DEVNAME>_RATE_MSK			NO_OS_GENMASK(3, 0)
#define <DEVNAME>_LOW_POWER_OP			NO_OS_BIT(4)

#define <DEVNAME>_POWER_CTL_MEASURE		NO_OS_BIT(3)

#define <DEVNAME>_DATA_FORMAT_RANGE		NO_OS_GENMASK(1, 0)
#define <DEVNAME>_DATA_FORMAT_FULL_RES		NO_OS_BIT(3)

#define <DEVNAME>_FIFO_CTL_MODE_MSK		NO_OS_GENMASK(7, 6)
#define <DEVNAME>_FIFO_CTL_SAMPLES_MSK		NO_OS_GENMASK(4, 0)

/* Number of registers read for all axes (X_L, X_H, Y_L, Y_H, Z_L, Z_H) */
#define <DEVNAME>_REGS_PER_ENTRY		6

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_comm_type
 *  @brief Communication interface type. */
enum <devname>_comm_type {
	<DEVNAME>_SPI_COMM = 0,
	<DEVNAME>_I2C_COMM = 1
};

/** @enum <devname>_op_mode
 *  @brief Operating mode selection. */
enum <devname>_op_mode {
	<DEVNAME>_STDBY = 0,
	<DEVNAME>_MEAS  = 1
};

/** @enum <devname>_range
 *  @brief Measurement range (g) selection. */
enum <devname>_range {
	<DEVNAME>_RANGE_0 = 0,
	<DEVNAME>_RANGE_1 = 1,
	<DEVNAME>_RANGE_2 = 2,
	<DEVNAME>_RANGE_3 = 3,
};

/** @enum <devname>_odr
 *  @brief Output data rate selection. */
enum <devname>_odr {
	<DEVNAME>_ODR_6_25HZ  = 0,
	<DEVNAME>_ODR_12_5HZ  = 1,
	<DEVNAME>_ODR_25HZ    = 2,
	<DEVNAME>_ODR_50HZ    = 3,
	<DEVNAME>_ODR_100HZ   = 4,
	<DEVNAME>_ODR_200HZ   = 5,
	<DEVNAME>_ODR_400HZ   = 6,
	<DEVNAME>_ODR_800HZ   = 7,
	<DEVNAME>_ODR_1600HZ  = 8,
	<DEVNAME>_ODR_3200HZ  = 9,
};

/** @enum <devname>_axis
 *  @brief Accelerometer axis selection. */
enum <devname>_axis {
	<DEVNAME>_X_AXIS = 0,
	<DEVNAME>_Y_AXIS = 1,
	<DEVNAME>_Z_AXIS = 2
};

/** @enum <devname>_fifo_mode
 *  @brief FIFO operating mode selection. */
enum <devname>_fifo_mode {
	<DEVNAME>_BYPASS_MODE    = 0,
	<DEVNAME>_FIFO_MODE      = 1,
	<DEVNAME>_STREAM_MODE    = 2,
	<DEVNAME>_TRIGGERED_MODE = 3
};

/* Add more enums as needed: low-power modes, interrupt polarity, etc. */

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_frac_repr
 * @brief  Data format in which acceleration values are converted.
 */
struct <devname>_frac_repr {
	int64_t integer;
	int32_t fractional;
};

/**
 * @union <devname>_comm_init_param
 * @brief  Communication init parameters (SPI or I2C).
 */
union <devname>_comm_init_param {
	/** I2C Initialization structure. */
	struct no_os_i2c_init_param i2c_init;
	/** SPI Initialization structure. */
	struct no_os_spi_init_param spi_init;
};

/**
 * @union <devname>_comm_desc
 * @brief  Communication descriptor (SPI or I2C).
 */
union <devname>_comm_desc {
	/** I2C descriptor. */
	struct no_os_i2c_desc *i2c_desc;
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Device communication init: either SPI or I2C. */
	union <devname>_comm_init_param comm_init;
	/** Communication type: <DEVNAME>_SPI_COMM or <DEVNAME>_I2C_COMM. */
	enum <devname>_comm_type comm_type;
	/** Device variant. */
	enum <devname>_device_id dev_type;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** Device communication descriptor. */
	union <devname>_comm_desc com_desc;
	/** Communication type. */
	enum <devname>_comm_type comm_type;
	/** Device variant. */
	enum <devname>_device_id dev_type;
	/** Measurement range. */
	enum <devname>_range range;
	/** Operating mode: standby or measurement. */
	enum <devname>_op_mode op_mode;
	/** Output data rate. */
	enum <devname>_odr odr;
	/** FIFO mode. */
	enum <devname>_fifo_mode fifo_mode;
	/** Communication buffer. */
	uint8_t comm_buff[24];
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Read a register. */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *val);

/** @brief Write a register. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t val);

/** @brief Read the 3-axis raw data from the accelerometer. */
int <devname>_get_raw_xyz(struct <devname>_dev *dev,
			  int16_t *x_raw, int16_t *y_raw,
			  int16_t *z_raw);

/** @brief Read the 3-axis data and convert to m/s^2. */
int <devname>_get_xyz(struct <devname>_dev *dev,
		      struct <devname>_frac_repr *x_m_s2,
		      struct <devname>_frac_repr *y_m_s2,
		      struct <devname>_frac_repr *z_m_s2);

/** @brief Set measurement range. */
int <devname>_set_range(struct <devname>_dev *dev,
			enum <devname>_range range);

/** @brief Set output data rate. */
int <devname>_set_odr(struct <devname>_dev *dev,
		      enum <devname>_odr odr);

/** @brief Set operating mode (standby / measurement). */
int <devname>_set_op_mode(struct <devname>_dev *dev,
			  enum <devname>_op_mode op_mode);

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
 * @brief Read a register via SPI or I2C.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *val)
{
	int ret;

	if (dev->comm_type == <DEVNAME>_SPI_COMM) {
		dev->comm_buff[0] = addr | <DEVNAME>_SPI_READ;
		dev->comm_buff[1] = 0x00;

		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc,
					       dev->comm_buff, 2);
		if (ret)
			return ret;

		*val = dev->comm_buff[1];
	} else {
		ret = no_os_i2c_write(dev->com_desc.i2c_desc, &addr, 1, 0);
		if (ret)
			return ret;

		ret = no_os_i2c_read(dev->com_desc.i2c_desc, val, 1, 1);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Write a register via SPI or I2C.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t val)
{
	if (dev->comm_type == <DEVNAME>_SPI_COMM) {
		dev->comm_buff[0] = addr & ~<DEVNAME>_SPI_READ;
		dev->comm_buff[1] = val;

		return no_os_spi_write_and_read(dev->com_desc.spi_desc,
						dev->comm_buff, 2);
	} else {
		dev->comm_buff[0] = addr;
		dev->comm_buff[1] = val;

		return no_os_i2c_write(dev->com_desc.i2c_desc,
				       dev->comm_buff, 2, 1);
	}
}

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Read the 3-axis raw data from the accelerometer.
 * @param dev   - Device descriptor.
 * @param x_raw - Pointer to store raw X-axis value.
 * @param y_raw - Pointer to store raw Y-axis value.
 * @param z_raw - Pointer to store raw Z-axis value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_raw_xyz(struct <devname>_dev *dev,
			  int16_t *x_raw, int16_t *y_raw,
			  int16_t *z_raw)
{
	uint8_t data[<DEVNAME>_REGS_PER_ENTRY];
	uint8_t reg;
	int ret;
	int i;

	reg = <DEVNAME>_REG_DATA_AXIS(0);

	/* Read all 6 data registers (X_L, X_H, Y_L, Y_H, Z_L, Z_H). */
	for (i = 0; i < <DEVNAME>_REGS_PER_ENTRY; i++) {
		ret = <devname>_reg_read(dev, reg + i, &data[i]);
		if (ret)
			return ret;
	}

	*x_raw = (int16_t)((data[1] << 8) | data[0]);
	*y_raw = (int16_t)((data[3] << 8) | data[2]);
	*z_raw = (int16_t)((data[5] << 8) | data[4]);

	return 0;
}

/**
 * @brief Read the 3-axis data and convert to m/s^2.
 * @param dev    - Device descriptor.
 * @param x_m_s2 - Pointer to store X acceleration in m/s^2.
 * @param y_m_s2 - Pointer to store Y acceleration in m/s^2.
 * @param z_m_s2 - Pointer to store Z acceleration in m/s^2.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_xyz(struct <devname>_dev *dev,
		      struct <devname>_frac_repr *x_m_s2,
		      struct <devname>_frac_repr *y_m_s2,
		      struct <devname>_frac_repr *z_m_s2)
{
	int16_t x_raw, y_raw, z_raw;
	int ret;

	ret = <devname>_get_raw_xyz(dev, &x_raw, &y_raw, &z_raw);
	if (ret)
		return ret;

	/*
	 * Convert raw values to m/s^2 using the device scale factor.
	 * The actual conversion depends on the configured range and
	 * resolution. See the datasheet for the LSB-to-mg mapping.
	 */
	/* TODO: implement device-specific conversion. */

	return 0;
}

/**
 * @brief Set the measurement range.
 * @param dev   - Device descriptor.
 * @param range - Range enum value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_range(struct <devname>_dev *dev,
			enum <devname>_range range)
{
	uint8_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_DATA_FORMAT, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~<DEVNAME>_DATA_FORMAT_RANGE;
	reg_val |= no_os_field_prep(<DEVNAME>_DATA_FORMAT_RANGE, range);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_DATA_FORMAT, reg_val);
	if (ret)
		return ret;

	dev->range = range;

	return 0;
}

/**
 * @brief Set the output data rate.
 * @param dev - Device descriptor.
 * @param odr - ODR enum value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_odr(struct <devname>_dev *dev,
		      enum <devname>_odr odr)
{
	uint8_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_BW_RATE, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~<DEVNAME>_RATE_MSK;
	reg_val |= no_os_field_prep(<DEVNAME>_RATE_MSK, odr);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_BW_RATE, reg_val);
	if (ret)
		return ret;

	dev->odr = odr;

	return 0;
}

/**
 * @brief Set the operating mode (standby or measurement).
 * @param dev     - Device descriptor.
 * @param op_mode - Operating mode enum value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_op_mode(struct <devname>_dev *dev,
			  enum <devname>_op_mode op_mode)
{
	uint8_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_POWER_CTL, &reg_val);
	if (ret)
		return ret;

	if (op_mode == <DEVNAME>_MEAS)
		reg_val |= <DEVNAME>_POWER_CTL_MEASURE;
	else
		reg_val &= ~<DEVNAME>_POWER_CTL_MEASURE;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_POWER_CTL, reg_val);
	if (ret)
		return ret;

	dev->op_mode = op_mode;

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

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_SOFT_RESET,
				  <DEVNAME>_RESET_KEY);
	if (ret)
		return ret;

	/* Wait for the device to reset. */
	no_os_mdelay(100);

	return 0;
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param dev        - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param init_param)
{
	struct <devname>_dev *d;
	uint8_t dev_id;
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->comm_type = init_param.comm_type;
	d->dev_type = init_param.dev_type;

	/* Initialise the communication interface. */
	if (d->comm_type == <DEVNAME>_SPI_COMM) {
		ret = no_os_spi_init(&d->com_desc.spi_desc,
				     &init_param.comm_init.spi_init);
	} else {
		ret = no_os_i2c_init(&d->com_desc.i2c_desc,
				     &init_param.comm_init.i2c_init);
	}
	if (ret)
		goto free_dev;

	/* Verify product ID. */
	ret = <devname>_reg_read(d, <DEVNAME>_REG_DEVID, &dev_id);
	if (ret)
		goto free_comm;

	if (dev_id != <DEVNAME>_DEVID) {
		pr_err("Unexpected device ID: 0x%02X\n", dev_id);
		ret = -ENODEV;
		goto free_comm;
	}

	/* Place device in standby for safe configuration. */
	ret = <devname>_set_op_mode(d, <DEVNAME>_STDBY);
	if (ret)
		goto free_comm;

	*dev = d;

	return 0;

free_comm:
	if (d->comm_type == <DEVNAME>_SPI_COMM)
		no_os_spi_remove(d->com_desc.spi_desc);
	else
		no_os_i2c_remove(d->com_desc.i2c_desc);
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

	if (dev->comm_type == <DEVNAME>_SPI_COMM)
		ret = no_os_spi_remove(dev->com_desc.spi_desc);
	else
		ret = no_os_i2c_remove(dev->com_desc.i2c_desc);

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
	/** Bitmask of active channels. */
	uint32_t active_channels;
	/** Number of active channels. */
	uint8_t no_of_active_channels;
};

/**
 * @struct <devname>_iio_dev_init_param
 * @brief  IIO initialization parameters.
 */
struct <devname>_iio_dev_init_param {
	/** Core driver init params. */
	struct <devname>_init_param *<devname>_dev_init;
};

/** @brief Initialise the IIO descriptor. */
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_dev_init_param *init_param);

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
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_<devname>.h"
#include "<devname>.h"

/* ---------- Forward declarations ----------------------------------- */

static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);
static int <devname>_iio_read_sampling_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel,
		intptr_t priv);
static int <devname>_iio_write_sampling_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel,
		intptr_t priv);
static int <devname>_iio_read_samples(void *dev, int16_t *buff,
				      uint32_t samples);
static int <devname>_iio_update_channels(void *dev, uint32_t mask);

/* ---------- Attribute callbacks ------------------------------------ */

/**
 * @brief Read raw accelerometer value for a channel.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_dev *desc = dev;
	int16_t x_raw, y_raw, z_raw;
	int32_t val;
	int ret;

	ret = <devname>_get_raw_xyz(desc-><devname>_dev,
				    &x_raw, &y_raw, &z_raw);
	if (ret)
		return ret;

	switch (channel->address) {
	case 0:
		val = (int32_t)x_raw;
		break;
	case 1:
		val = (int32_t)y_raw;
		break;
	case 2:
		val = (int32_t)z_raw;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor (m/s^2 per LSB).
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	/*
	 * Scale factor depends on device and range. Express as
	 * IIO_VAL_INT_PLUS_NANO: integer part + nano part.
	 * Example: 0.009576806 m/s^2 per LSB => val[0]=0, val[1]=9576806
	 */
	val[0] = 0;
	val[1] = 9576806; /* Adjust per device/range */

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_NANO, 2, val);
}

/**
 * @brief Read offset (typically 0 for accelerometers).
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read current sampling frequency.
 */
static int <devname>_iio_read_sampling_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel,
		intptr_t priv)
{
	/* TODO: Read current ODR and return as integer + fractional Hz. */
	int32_t val[2] = {100, 0};

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/**
 * @brief Write sampling frequency.
 */
static int <devname>_iio_write_sampling_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel,
		intptr_t priv)
{
	/* TODO: Parse value and call <devname>_set_odr(). */
	return 0;
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_accel_attrs[] = {
	{
		.name = "raw",
		.show = <devname>_iio_read_raw,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = <devname>_iio_read_scale,
	},
	{
		.name = "offset",
		.show = <devname>_iio_read_offset,
	},
	{
		.name   = "sampling_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = <devname>_iio_read_sampling_freq,
		.store  = <devname>_iio_write_sampling_freq,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_iio_accel_scan_type = {
	.sign = 's',          /* signed */
	.realbits = 13,       /* accelerometer resolution */
	.storagebits = 16,    /* container size */
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * Accelerometer channels: IIO_ACCEL, ch_out = false (input), 3 axes.
 * Uses modified channel scheme with IIO_MOD_X/Y/Z.
 */
#define <DEVNAME>_ACCEL_CHANNEL(index, reg, axis) { \
	.ch_type = IIO_ACCEL,                       \
	.channel = (index),                         \
	.address = (reg),                           \
	.modified = true,                           \
	.channel2 = IIO_MOD_##axis,                 \
	.scan_type = &<devname>_iio_accel_scan_type,\
	.scan_index = (index),                      \
	.attributes = <devname>_iio_accel_attrs,    \
	.ch_out = false,                            \
}

static struct iio_channel <devname>_iio_channels[] = {
	<DEVNAME>_ACCEL_CHANNEL(0, 0, X),
	<DEVNAME>_ACCEL_CHANNEL(1, 1, Y),
	<DEVNAME>_ACCEL_CHANNEL(2, 2, Z),
};

/* ---------- Buffered read / channel update callbacks --------------- */

/**
 * @brief Update active channels mask.
 */
static int <devname>_iio_update_channels(void *dev, uint32_t mask)
{
	struct <devname>_iio_dev *desc = dev;

	desc->active_channels = mask;
	desc->no_of_active_channels = no_os_hweight32(mask);

	return 0;
}

/**
 * @brief Read accelerometer samples into an IIO buffer.
 */
static int <devname>_iio_read_samples(void *dev, int16_t *buff,
				      uint32_t samples)
{
	struct <devname>_iio_dev *desc = dev;
	int16_t x_raw, y_raw, z_raw;
	uint32_t i;
	int ret;

	for (i = 0; i < samples; i++) {
		ret = <devname>_get_raw_xyz(desc-><devname>_dev,
					    &x_raw, &y_raw, &z_raw);
		if (ret)
			return ret;

		if (desc->active_channels & NO_OS_BIT(0))
			*buff++ = x_raw;
		if (desc->active_channels & NO_OS_BIT(1))
			*buff++ = y_raw;
		if (desc->active_channels & NO_OS_BIT(2))
			*buff++ = z_raw;
	}

	return samples;
}

/* ---------- Debug register access ---------------------------------- */

static int <devname>_iio_read_reg(struct <devname>_iio_dev *dev,
				  uint32_t reg, uint32_t *readval)
{
	uint8_t val;
	int ret;

	ret = <devname>_reg_read(dev-><devname>_dev, (uint8_t)reg, &val);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

static int <devname>_iio_write_reg(struct <devname>_iio_dev *dev,
				   uint32_t reg, uint32_t writeval)
{
	return <devname>_reg_write(dev-><devname>_dev, (uint8_t)reg,
				  (uint8_t)writeval);
}

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels),
	.channels = <devname>_iio_channels,
	.pre_enable = (int32_t (*)())<devname>_iio_update_channels,
	.read_dev = (int32_t (*)())<devname>_iio_read_samples,
	.debug_reg_read = (int32_t (*)())<devname>_iio_read_reg,
	.debug_reg_write = (int32_t (*)())<devname>_iio_write_reg,
};

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 */
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_dev_init_param *init_param)
{
	struct <devname>_iio_dev *d;
	int ret;

	d = (struct <devname>_iio_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = <devname>_init(&d-><devname>_dev,
			     *init_param-><devname>_dev_init);
	if (ret)
		goto free_desc;

	d->iio_dev = &<devname>_iio_dev;

	*iio_dev = d;

	return 0;

free_desc:
	no_os_free(d);

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

INCS += $(DRIVERS)/accel/<devname>/<devname>.h
SRCS += $(DRIVERS)/accel/<devname>/<devname>.c
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
INCS += $(DRIVERS)/accel/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/accel/<devname>/iio_<devname>.c

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
	.comm_type = <DEVNAME>_SPI_COMM,
	.comm_init = {
		.spi_init = {
			.device_id = SPI_DEVICE_ID,
			.max_speed_hz = SPI_MAX_SPEED,
			.chip_select = SPI_CS,
			.mode = NO_OS_SPI_MODE_3,
			.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
			.platform_ops = SPI_OPS,
			.extra = SPI_EXTRA,
			.parent = NULL,
		},
	},
	.dev_type = ID_<DEVNAME>,
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
	int16_t x_raw, y_raw, z_raw;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, <devname>_ip);
	if (ret)
		goto error;

	/* Enter measurement mode. */
	ret = <devname>_set_op_mode(dev, <DEVNAME>_MEAS);
	if (ret)
		goto free_dev;

	while (1) {
		ret = <devname>_get_raw_xyz(dev, &x_raw, &y_raw, &z_raw);
		if (ret)
			goto free_dev;

		pr_info("X: %6d  Y: %6d  Z: %6d\r\n",
			x_raw, y_raw, z_raw);
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
	struct <devname>_iio_dev_init_param <devname>_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	<devname>_iio_ip.<devname>_dev_init = &<devname>_ip;
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
#define I2C_MAX_SPEED	400000
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

* :adi:`<DEVNAME>`

Overview
--------

The <DEVNAME> is a low-power, <resolution>-bit, 3-axis digital
accelerometer with selectable measurement ranges of <ranges>. It
provides an SPI and I2C digital interface, and supports output data
rates from <min_rate> Hz to <max_rate> Hz. <Add 2-3 more sentences
from the datasheet covering key specs, FIFO depth, and features.>

Applications
------------

* Motion and tilt sensing
* Vibration monitoring
* IoT and wearable devices
* Platform stabilisation

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI or I2C interface (selected by the
``comm_type`` field in the init parameters), verifies the device ID,
and places the accelerometer in standby mode for safe configuration.
The function uses goto-based cleanup to release resources on failure.

Axis Data Reading
~~~~~~~~~~~~~~~~~

The ``<devname>_get_raw_xyz`` function reads all six data registers
(X_L, X_H, Y_L, Y_H, Z_L, Z_H) and returns signed 16-bit raw
values for each axis. The ``<devname>_get_xyz`` function additionally
converts these raw values to physical units (m/s^2) using the
device's current scale factor.

Range Configuration
~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_range`` function programs the measurement range
bits in the DATA_FORMAT register. Changing the range affects the
sensitivity (LSB per g) and the scale factor used for conversion.

Output Data Rate
~~~~~~~~~~~~~~~~

The ``<devname>_set_odr`` function configures the output data rate
by writing to the BW_RATE register. Higher data rates enable faster
sampling but may increase power consumption.

Power Modes
~~~~~~~~~~~

The ``<devname>_set_op_mode`` function switches between standby mode
(low power, no measurements) and measurement mode (active sensing).
Configuration changes should be made in standby mode.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .comm_type = <DEVNAME>_SPI_COMM,
           .comm_init = {
               .spi_init = { /* platform-specific SPI params */ },
           },
           .dev_type = ID_<DEVNAME>,
       };

       struct <devname>_dev *dev;
       int16_t x_raw, y_raw, z_raw;
       int ret;

       ret = <devname>_init(&dev, ip);
       if (ret)
           return ret;

       ret = <devname>_set_op_mode(dev, <DEVNAME>_MEAS);
       if (ret)
           goto cleanup;

       ret = <devname>_get_raw_xyz(dev, &x_raw, &y_raw, &z_raw);
       if (ret)
           goto cleanup;

       /* x_raw, y_raw, z_raw contain signed acceleration data */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each accelerometer axis as an
``IIO_ACCEL`` channel with ``raw``, ``scale``, ``offset``, and
``sampling_frequency`` attributes. The IIO application can be
used for real-time 3-axis data acquisition.

IIO Initialization Example
---------------------------

.. code-block:: C

   int iio_example_main()
   {
       int ret;
       struct <devname>_iio_dev *iio_dev;
       struct <devname>_iio_dev_init_param iio_ip;
       struct iio_app_desc *app;
       struct iio_app_init_param app_init_param = {0};

       iio_ip.<devname>_dev_init = &<devname>_ip;
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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/accel/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/accel/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int`, negative = error.
5. **Init signature** -- `int <devname>_init(struct <devname>_dev **dev,
   struct <devname>_init_param init_param)` (note: init_param passed
   by value for dual-interface drivers using unions).
6. **Dual-interface pattern** -- use a `union` for `comm_init_param`
   (SPI/I2C init) and `comm_desc` (SPI/I2C descriptor); select with
   a `comm_type` enum.
7. **Allocation** -- use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
8. **Goto cleanup** -- in `_init()`, use goto labels to free in reverse
   allocation order on error; handle both SPI and I2C cleanup paths.
9. **Remove order** -- `_remove()` frees resources in reverse order of
   `_init()`, checking `comm_type` to call the correct remove function.
10. **No globals** -- all state lives in the device descriptor struct.
11. **Register macros** -- `NO_OS_BIT()` for single bits,
    `NO_OS_GENMASK(hi, lo)` for fields; `no_os_field_prep()` /
    `no_os_field_get()` for field access.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> low-power, 3-axis digital accelerometer
with SPI and I2C interface.

Signed-off-by: Your Name <your.name@analog.com>
```
