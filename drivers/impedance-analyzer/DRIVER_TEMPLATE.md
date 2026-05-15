# Impedance Analyzer Driver Template

Reference driver: `drivers/impedance-analyzer/ad5933/`

This template covers every file needed to add a new impedance analyzer
driver to no-OS.  Replace `<devname>` with the part number (e.g.,
`ad5933`) and `<DEVNAME>` with its uppercase form (e.g., `AD5933`)
throughout.

---

## 1. Naming Conventions

Impedance analyzer drivers use `_dev` for the device descriptor struct
and communicate over I2C.  The driver must support frequency sweep
configuration, gain factor calibration, and impedance calculation from
real/imaginary DFT data.

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ad5933.h`, `ad5933.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ad5933.h`, `iio_ad5933.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __AD5933_H__` |
| Init param struct | `<devname>_init_param` | `ad5933_init_param` |
| Device struct | `<devname>_dev` | `ad5933_dev` |
| IIO descriptor | `<devname>_iio_dev` | `ad5933_iio_dev` |
| Functions | `<devname>_<verb>()` | `ad5933_init()` |
| Address macro | `<DEVNAME>_ADDRESS` | `AD5933_ADDRESS` |
| Register defines | `<DEVNAME>_REG_<NAME>` | `AD5933_REG_CONTROL_HB` |
| Field masks | `<DEVNAME>_<FIELD>_MASK` | `AD5933_CONTROL_RANGE_MASK` |

---

## 2. File Checklist

```
drivers/impedance-analyzer/<devname>/
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

doc/sphinx/source/drivers/impedance-analyzer/<devname>.rst   # Sphinx entry
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

/* ---- I2C Address -------------------------------------------------- */

/** Default I2C address. */
#define <DEVNAME>_ADDRESS		0x0D

/* ---- Block Commands ----------------------------------------------- */

/** I2C block write command. */
#define <DEVNAME>_BLOCK_WRITE		0xA0
/** I2C block read command. */
#define <DEVNAME>_BLOCK_READ		0xA1
/** I2C set address pointer command. */
#define <DEVNAME>_ADDR_POINTER		0xB0

/* ---- Register Addresses ------------------------------------------- */

#define <DEVNAME>_REG_CONTROL_HB	0x80
#define <DEVNAME>_REG_CONTROL_LB	0x81
#define <DEVNAME>_REG_FREQ_START	0x82
#define <DEVNAME>_REG_FREQ_INC		0x85
#define <DEVNAME>_REG_INC_NUM		0x88
#define <DEVNAME>_REG_SETTLING_CYCLES	0x8A
#define <DEVNAME>_REG_STATUS		0x8F
#define <DEVNAME>_REG_TEMP_DATA	0x92
#define <DEVNAME>_REG_REAL_DATA	0x94
#define <DEVNAME>_REG_IMAG_DATA	0x96

/* ---- Control Register HB Masks ----------------------------------- */

#define <DEVNAME>_CTRL_FUNC_MASK	NO_OS_GENMASK(7, 4)
#define <DEVNAME>_CTRL_RANGE_MASK	NO_OS_GENMASK(2, 1)
#define <DEVNAME>_CTRL_PGA_GAIN_MASK	NO_OS_BIT(0)

/* ---- Control Register LB Masks ----------------------------------- */

#define <DEVNAME>_CTRL_RESET_MASK	NO_OS_BIT(4)
#define <DEVNAME>_CTRL_CLK_SRC_MASK	NO_OS_BIT(3)

/* ---- Control Function Options ------------------------------------- */

#define <DEVNAME>_FUNC_NOP		0x0
#define <DEVNAME>_FUNC_INIT_START_FREQ	0x1
#define <DEVNAME>_FUNC_START_SWEEP	0x2
#define <DEVNAME>_FUNC_INC_FREQ	0x3
#define <DEVNAME>_FUNC_REPEAT_FREQ	0x4
#define <DEVNAME>_FUNC_MEASURE_TEMP	0x9
#define <DEVNAME>_FUNC_POWER_DOWN	0xA
#define <DEVNAME>_FUNC_STANDBY		0xB

/* ---- Output Voltage Range Options --------------------------------- */

#define <DEVNAME>_RANGE_2000mVpp	0x0
#define <DEVNAME>_RANGE_200mVpp	0x1
#define <DEVNAME>_RANGE_400mVpp	0x2
#define <DEVNAME>_RANGE_1000mVpp	0x3

/* ---- PGA Gain Options --------------------------------------------- */

#define <DEVNAME>_GAIN_X5		0
#define <DEVNAME>_GAIN_X1		1

/* ---- Settling Time Multiplier Options ----------------------------- */

#define <DEVNAME>_SETTLING_X1		0
#define <DEVNAME>_SETTLING_X2		1
#define <DEVNAME>_SETTLING_X4		3

/* ---- Status Register Bits ----------------------------------------- */

#define <DEVNAME>_STAT_TEMP_VALID_MASK	NO_OS_BIT(0)
#define <DEVNAME>_STAT_DATA_VALID_MASK	NO_OS_BIT(1)
#define <DEVNAME>_STAT_SWEEP_DONE_MASK	NO_OS_BIT(2)

/* ---- Specifications ----------------------------------------------- */

/** Internal system clock frequency in Hz. */
#define <DEVNAME>_INTERNAL_SYS_CLK	16000000ul
/** Maximum number of frequency increments. */
#define <DEVNAME>_MAX_INC_NUM		511
/** Default number of settling cycles. */
#define <DEVNAME>_DEFAULT_SETTLING	15

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** I2C descriptor. */
	struct no_os_i2c_desc *i2c_desc;
	/** Current system clock frequency in Hz. */
	uint32_t current_sys_clk;
	/** Current clock source setting. */
	uint8_t current_clock_source;
	/** Current PGA gain setting. */
	uint8_t current_gain;
	/** Current output voltage range setting. */
	uint8_t current_range;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters for <devname>_init().
 */
struct <devname>_init_param {
	/** I2C initialization parameters. */
	struct no_os_i2c_init_param i2c_init;
	/** System clock frequency in Hz. */
	uint32_t current_sys_clk;
	/** Clock source (internal or external). */
	uint8_t current_clock_source;
	/** PGA gain setting. */
	uint8_t current_gain;
	/** Output voltage range setting. */
	uint8_t current_range;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Write a register value. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t reg,
			uint32_t val, uint8_t bytes);

/** @brief Read a register value. */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t reg,
		       uint8_t bytes, uint32_t *val);

/** @brief Reset the device. */
int <devname>_reset(struct <devname>_dev *dev);

/** @brief Set the system clock source and frequency. */
int <devname>_set_system_clk(struct <devname>_dev *dev,
			     uint8_t clk_source,
			     uint32_t ext_clk_freq);

/** @brief Set the output voltage range and PGA gain. */
int <devname>_set_range_and_gain(struct <devname>_dev *dev,
				 uint8_t range, uint8_t gain);

/** @brief Read the on-chip temperature sensor (milliCelsius). */
int <devname>_get_temperature(struct <devname>_dev *dev, int32_t *temp_mc);

/** @brief Configure sweep: start freq, increment freq, number of steps. */
int <devname>_config_sweep(struct <devname>_dev *dev,
			   uint32_t start_freq,
			   uint32_t inc_freq,
			   uint16_t inc_num);

/** @brief Start the frequency sweep. */
int <devname>_start_sweep(struct <devname>_dev *dev);

/** @brief Set the settling time cycles and multiplier. */
int <devname>_set_settling_time(struct <devname>_dev *dev,
				uint8_t multiplier,
				uint16_t number_cycles);

/** @brief Read real and imaginary DFT data at a sweep point. */
int <devname>_get_data(struct <devname>_dev *dev, uint8_t freq_function,
		       int16_t *real_data, int16_t *imag_data);

/** @brief Calculate the gain factor from a known calibration impedance. */
int <devname>_calculate_gain_factor(struct <devname>_dev *dev,
				    uint32_t calibration_impedance,
				    uint8_t freq_function,
				    int64_t *gain_factor);

/** @brief Calculate impedance from real/imag data and gain factor. */
int <devname>_calculate_impedance(struct <devname>_dev *dev,
				  int64_t gain_factor,
				  uint8_t freq_function,
				  int64_t *impedance);

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

#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include <string.h>

/** 2^27, used for frequency-to-register conversion. */
static const uint32_t pow_2_27 = 134217728ul;

/**
 * @brief Write a device register over I2C.
 * @param dev   - Device descriptor.
 * @param reg   - Register address.
 * @param val   - Value to write.
 * @param bytes - Number of bytes to write (1, 2, or 3).
 * @return 0 on success, negative error code otherwise.
 *
 * Impedance analyzer registers are written one byte at a time via the
 * I2C interface: each byte is sent as a 2-byte frame [addr, data].
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t reg,
			uint32_t val, uint8_t bytes)
{
	uint8_t buf[2];
	uint8_t i;
	int ret;

	for (i = 0; i < bytes; i++) {
		buf[0] = reg + bytes - i - 1;
		buf[1] = (uint8_t)((val >> (i * 8)) & 0xFF);
		ret = no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Read a device register over I2C.
 * @param dev   - Device descriptor.
 * @param reg   - Register address.
 * @param bytes - Number of bytes to read (1, 2, or 3).
 * @param val   - Pointer to store the read value.
 * @return 0 on success, negative error code otherwise.
 *
 * Each byte is read by first setting the address pointer via I2C write,
 * then issuing a single-byte I2C read.
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t reg,
		       uint8_t bytes, uint32_t *val)
{
	uint8_t buf[2];
	uint32_t result = 0;
	uint8_t i;
	int ret;

	for (i = 0; i < bytes; i++) {
		/* Set the register pointer. */
		buf[0] = <DEVNAME>_ADDR_POINTER;
		buf[1] = reg + i;
		ret = no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
		if (ret)
			return ret;

		/* Read one byte. */
		ret = no_os_i2c_read(dev->i2c_desc, buf, 1, 1);
		if (ret)
			return ret;

		result = (result << 8) | buf[0];
	}

	*val = result;

	return 0;
}

/**
 * @brief Reset the device.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reset(struct <devname>_dev *dev)
{
	uint8_t val;

	val = no_os_field_prep(<DEVNAME>_CTRL_RESET_MASK, 1) |
	      no_os_field_prep(<DEVNAME>_CTRL_CLK_SRC_MASK,
			       dev->current_clock_source);

	return <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL_LB, val, 1);
}

/**
 * @brief Select the source of the system clock.
 * @param dev          - Device descriptor.
 * @param clk_source   - 0 for internal, 1 for external.
 * @param ext_clk_freq - External clock frequency in Hz (ignored if internal).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_system_clk(struct <devname>_dev *dev,
			     uint8_t clk_source,
			     uint32_t ext_clk_freq)
{
	dev->current_clock_source = clk_source;

	if (clk_source)
		dev->current_sys_clk = ext_clk_freq;
	else
		dev->current_sys_clk = <DEVNAME>_INTERNAL_SYS_CLK;

	return <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL_LB,
				   no_os_field_prep(<DEVNAME>_CTRL_CLK_SRC_MASK,
						    clk_source), 1);
}

/**
 * @brief Select the output voltage range and PGA gain.
 * @param dev   - Device descriptor.
 * @param range - Output voltage range (e.g. <DEVNAME>_RANGE_2000mVpp).
 * @param gain  - PGA gain (e.g. <DEVNAME>_GAIN_X1).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_range_and_gain(struct <devname>_dev *dev,
				 uint8_t range, uint8_t gain)
{
	uint8_t val;
	int ret;

	val = no_os_field_prep(<DEVNAME>_CTRL_FUNC_MASK,
			       <DEVNAME>_FUNC_NOP) |
	      no_os_field_prep(<DEVNAME>_CTRL_RANGE_MASK, range) |
	      no_os_field_prep(<DEVNAME>_CTRL_PGA_GAIN_MASK, gain);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL_HB, val, 1);
	if (ret)
		return ret;

	dev->current_range = range;
	dev->current_gain = gain;

	return 0;
}

/**
 * @brief Read the on-chip temperature sensor.
 * @param dev     - Device descriptor.
 * @param temp_mc - Pointer to store temperature in milliCelsius.
 * @return 0 on success, negative error code otherwise.
 *
 * Temperature conversion:
 *   - 14-bit signed result, 1 LSB = 1/32 degree C.
 *   - Values < 8192: temp = raw / 32.
 *   - Values >= 8192: temp = (raw - 16384) / 32.
 *   - Returned as milliCelsius: raw * 1000 / 32.
 */
int <devname>_get_temperature(struct <devname>_dev *dev, int32_t *temp_mc)
{
	uint32_t status = 0;
	uint32_t raw;
	uint8_t ctrl_val;
	int ret;

	ctrl_val = no_os_field_prep(<DEVNAME>_CTRL_FUNC_MASK,
				    <DEVNAME>_FUNC_MEASURE_TEMP) |
		   no_os_field_prep(<DEVNAME>_CTRL_RANGE_MASK,
				    dev->current_range) |
		   no_os_field_prep(<DEVNAME>_CTRL_PGA_GAIN_MASK,
				    dev->current_gain);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL_HB, ctrl_val, 1);
	if (ret)
		return ret;

	/* Poll until temperature data is valid. */
	while (!(status & <DEVNAME>_STAT_TEMP_VALID_MASK)) {
		ret = <devname>_reg_read(dev, <DEVNAME>_REG_STATUS,
					 1, &status);
		if (ret)
			return ret;
	}

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_TEMP_DATA, 2, &raw);
	if (ret)
		return ret;

	if (raw < 8192)
		*temp_mc = (int32_t)raw * 1000 / 32;
	else
		*temp_mc = ((int32_t)raw - 16384) * 1000 / 32;

	return 0;
}

/**
 * @brief Configure the frequency sweep parameters.
 * @param dev        - Device descriptor.
 * @param start_freq - Start frequency in Hz.
 * @param inc_freq   - Frequency increment in Hz.
 * @param inc_num    - Number of frequency increments (max 511).
 * @return 0 on success, negative error code otherwise.
 *
 * Frequency register conversion:
 *   freq_code = (freq_hz * 4 / sys_clk) * 2^27
 */
int <devname>_config_sweep(struct <devname>_dev *dev,
			   uint32_t start_freq,
			   uint32_t inc_freq,
			   uint16_t inc_num)
{
	uint32_t start_code;
	uint32_t inc_code;
	uint16_t inc_num_clamped;
	int ret;

	if (inc_num > <DEVNAME>_MAX_INC_NUM)
		inc_num_clamped = <DEVNAME>_MAX_INC_NUM;
	else
		inc_num_clamped = inc_num;

	/* Convert frequency to register code. */
	start_code = (uint32_t)((uint64_t)start_freq * 4 *
				pow_2_27 / dev->current_sys_clk);
	inc_code = (uint32_t)((uint64_t)inc_freq * 4 *
			      pow_2_27 / dev->current_sys_clk);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_FREQ_START,
				  start_code, 3);
	if (ret)
		return ret;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_FREQ_INC,
				  inc_code, 3);
	if (ret)
		return ret;

	return <devname>_reg_write(dev, <DEVNAME>_REG_INC_NUM,
				   inc_num_clamped, 2);
}

/**
 * @brief Start the frequency sweep.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 *
 * Sequence: standby -> reset -> init start freq -> start sweep.
 * Blocks until the first data point is valid.
 */
int <devname>_start_sweep(struct <devname>_dev *dev)
{
	uint32_t status = 0;
	uint8_t ctrl_val;
	int ret;

	/* Enter standby. */
	ctrl_val = no_os_field_prep(<DEVNAME>_CTRL_FUNC_MASK,
				    <DEVNAME>_FUNC_STANDBY) |
		   no_os_field_prep(<DEVNAME>_CTRL_RANGE_MASK,
				    dev->current_range) |
		   no_os_field_prep(<DEVNAME>_CTRL_PGA_GAIN_MASK,
				    dev->current_gain);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL_HB, ctrl_val, 1);
	if (ret)
		return ret;

	ret = <devname>_reset(dev);
	if (ret)
		return ret;

	/* Initialise start frequency. */
	ctrl_val = no_os_field_prep(<DEVNAME>_CTRL_FUNC_MASK,
				    <DEVNAME>_FUNC_INIT_START_FREQ) |
		   no_os_field_prep(<DEVNAME>_CTRL_RANGE_MASK,
				    dev->current_range) |
		   no_os_field_prep(<DEVNAME>_CTRL_PGA_GAIN_MASK,
				    dev->current_gain);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL_HB, ctrl_val, 1);
	if (ret)
		return ret;

	/* Start sweep. */
	ctrl_val = no_os_field_prep(<DEVNAME>_CTRL_FUNC_MASK,
				    <DEVNAME>_FUNC_START_SWEEP) |
		   no_os_field_prep(<DEVNAME>_CTRL_RANGE_MASK,
				    dev->current_range) |
		   no_os_field_prep(<DEVNAME>_CTRL_PGA_GAIN_MASK,
				    dev->current_gain);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL_HB, ctrl_val, 1);
	if (ret)
		return ret;

	/* Wait for valid data. */
	while (!(status & <DEVNAME>_STAT_DATA_VALID_MASK)) {
		ret = <devname>_reg_read(dev, <DEVNAME>_REG_STATUS,
					 1, &status);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Set the number of settling cycles.
 * @param dev           - Device descriptor.
 * @param multiplier    - Settling multiplier (X1, X2, or X4).
 * @param number_cycles - 9-bit number of settling cycles.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_settling_time(struct <devname>_dev *dev,
				uint8_t multiplier,
				uint16_t number_cycles)
{
	uint16_t reg_val;

	if (multiplier != <DEVNAME>_SETTLING_X2 &&
	    multiplier != <DEVNAME>_SETTLING_X4)
		multiplier = <DEVNAME>_SETTLING_X1;

	reg_val = number_cycles | ((uint16_t)multiplier << 9);

	return <devname>_reg_write(dev, <DEVNAME>_REG_SETTLING_CYCLES,
				   reg_val, 2);
}

/**
 * @brief Read the real and imaginary DFT data at a sweep point.
 * @param dev           - Device descriptor.
 * @param freq_function - Frequency function (INC_FREQ or REPEAT_FREQ).
 * @param real_data     - Pointer to store real component.
 * @param imag_data     - Pointer to store imaginary component.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_data(struct <devname>_dev *dev, uint8_t freq_function,
		       int16_t *real_data, int16_t *imag_data)
{
	uint32_t status = 0;
	uint32_t raw;
	uint8_t ctrl_val;
	int ret;

	ctrl_val = no_os_field_prep(<DEVNAME>_CTRL_FUNC_MASK,
				    freq_function) |
		   no_os_field_prep(<DEVNAME>_CTRL_RANGE_MASK,
				    dev->current_range) |
		   no_os_field_prep(<DEVNAME>_CTRL_PGA_GAIN_MASK,
				    dev->current_gain);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL_HB, ctrl_val, 1);
	if (ret)
		return ret;

	/* Wait for valid data. */
	while (!(status & <DEVNAME>_STAT_DATA_VALID_MASK)) {
		ret = <devname>_reg_read(dev, <DEVNAME>_REG_STATUS,
					 1, &status);
		if (ret)
			return ret;
	}

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_REAL_DATA, 2, &raw);
	if (ret)
		return ret;
	*real_data = (int16_t)raw;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_IMAG_DATA, 2, &raw);
	if (ret)
		return ret;
	*imag_data = (int16_t)raw;

	return 0;
}

/**
 * @brief Calculate the gain factor using a known calibration impedance.
 * @param dev                    - Device descriptor.
 * @param calibration_impedance  - Known impedance in Ohms.
 * @param freq_function          - Frequency function (INC_FREQ or REPEAT_FREQ).
 * @param gain_factor            - Pointer to store the gain factor (scaled).
 * @return 0 on success, negative error code otherwise.
 *
 * Gain factor = 1 / (magnitude * calibration_impedance).
 * The result is scaled by 10^9 to preserve precision in integer math.
 */
int <devname>_calculate_gain_factor(struct <devname>_dev *dev,
				    uint32_t calibration_impedance,
				    uint8_t freq_function,
				    int64_t *gain_factor)
{
	int16_t real_data, imag_data;
	uint64_t magnitude_sq;
	int ret;

	ret = <devname>_get_data(dev, freq_function, &real_data, &imag_data);
	if (ret)
		return ret;

	magnitude_sq = (int64_t)real_data * real_data +
		       (int64_t)imag_data * imag_data;

	if (magnitude_sq == 0 || calibration_impedance == 0)
		return -EINVAL;

	/*
	 * gain_factor = 10^9 / (sqrt(magnitude_sq) * cal_impedance)
	 *
	 * Implemented as integer-only using magnitude squared to avoid
	 * floating point.  Actual impedance calculation uses the same
	 * scaling convention.
	 */
	*gain_factor = (int64_t)(1000000000ULL /
				 (calibration_impedance * magnitude_sq));

	return 0;
}

/**
 * @brief Calculate impedance from real/imag data and a gain factor.
 * @param dev           - Device descriptor.
 * @param gain_factor   - Gain factor from calibration (scaled by 10^9).
 * @param freq_function - Frequency function (INC_FREQ or REPEAT_FREQ).
 * @param impedance     - Pointer to store impedance in milliOhms.
 * @return 0 on success, negative error code otherwise.
 *
 * impedance = 1 / (gain_factor * magnitude).
 */
int <devname>_calculate_impedance(struct <devname>_dev *dev,
				  int64_t gain_factor,
				  uint8_t freq_function,
				  int64_t *impedance)
{
	int16_t real_data, imag_data;
	uint64_t magnitude_sq;
	int ret;

	ret = <devname>_get_data(dev, freq_function, &real_data, &imag_data);
	if (ret)
		return ret;

	magnitude_sq = (int64_t)real_data * real_data +
		       (int64_t)imag_data * imag_data;

	if (magnitude_sq == 0 || gain_factor == 0)
		return -EINVAL;

	*impedance = (int64_t)(1000000000ULL /
			       (gain_factor * magnitude_sq));

	return 0;
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *dev;
	int ret;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (ret)
		goto free_dev;

	dev->current_sys_clk = init_param->current_sys_clk;
	dev->current_clock_source = init_param->current_clock_source;
	dev->current_gain = init_param->current_gain;
	dev->current_range = init_param->current_range;

	*device = dev;

	return 0;

free_dev:
	no_os_free(dev);

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
	struct <devname>_dev *dev;
	/** IIO device structure. */
	struct iio_device *iio_dev;
	/** Bitmask of active channels. */
	uint32_t active_channels;
	/** Cached gain factor from calibration (scaled). */
	int64_t gain_factor;
	/** Calibration impedance in Ohms. */
	uint32_t calibration_impedance;
};

/**
 * @struct <devname>_iio_init_param
 * @brief  IIO initialization parameters.
 */
struct <devname>_iio_init_param {
	/** Core driver init params. */
	struct <devname>_init_param *init_param;
};

/** @brief Initialise the IIO descriptor. */
int <devname>_iio_init(struct <devname>_iio_dev **desc,
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

/* ---------- Attribute callbacks ------------------------------------ */

/**
 * @brief Read the raw real-component DFT value.
 *
 * IIO channel 0 (voltage0) returns the real register data.
 * IIO channel 1 (voltage1) returns the imaginary register data.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_dev *desc = (struct <devname>_iio_dev *)dev;
	int16_t real_data, imag_data;
	int32_t val;
	int ret;

	ret = <devname>_get_data(desc->dev,
				 <DEVNAME>_FUNC_REPEAT_FREQ,
				 &real_data, &imag_data);
	if (ret)
		return ret;

	if (channel->ch_num == 0)
		val = real_data;
	else
		val = imag_data;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor.
 *
 * Returns a unity scale (1.0) since the raw values are the DFT
 * output codes.  Impedance is computed externally from real + imag.
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	val[0] = 1;
	val[1] = 0;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/**
 * @brief Read the frequency attribute (current sweep start frequency).
 *
 * Reads the 24-bit start frequency register and converts back to Hz.
 */
static int <devname>_iio_read_frequency(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct <devname>_iio_dev *desc = (struct <devname>_iio_dev *)dev;
	uint32_t freq_code;
	int32_t freq_hz;
	int ret;

	ret = <devname>_reg_read(desc->dev, <DEVNAME>_REG_FREQ_START,
				 3, &freq_code);
	if (ret)
		return ret;

	freq_hz = (int32_t)((uint64_t)freq_code * desc->dev->current_sys_clk /
			    (4ULL * 134217728ULL));

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &freq_hz);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_attrs[] = {
	{ .name = "raw",       .show = <devname>_iio_read_raw },
	{ .name = "scale",     .show = <devname>_iio_read_scale },
	{ .name = "frequency", .show = <devname>_iio_read_frequency },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_scan = {
	.sign = 's',          /* signed */
	.realbits = 16,       /* DFT output resolution */
	.storagebits = 16,    /* stored in 16-bit container */
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * Two voltage channels:
 *   - voltage0: real component of DFT output
 *   - voltage1: imaginary component of DFT output
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "real",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.ch_out = 0,
		.attributes = <devname>_iio_attrs,
		.scan_type = &<devname>_scan,
	},
	{
		.name = "imaginary",
		.ch_type = IIO_VOLTAGE,
		.channel = 1,
		.ch_out = 0,
		.attributes = <devname>_iio_attrs,
		.scan_type = &<devname>_scan,
	},
};

/* ---------- Buffered read callback --------------------------------- */

/**
 * @brief Read DFT samples into an IIO buffer.
 *
 * Each sample consists of a real + imaginary pair.
 */
static int <devname>_iio_read_samples(void *dev, int16_t *buf,
				      uint32_t samples)
{
	struct <devname>_iio_dev *desc = (struct <devname>_iio_dev *)dev;
	int16_t real_data, imag_data;
	uint32_t i;
	int ret;

	for (i = 0; i < samples; i++) {
		ret = <devname>_get_data(desc->dev,
					 <DEVNAME>_FUNC_INC_FREQ,
					 &real_data, &imag_data);
		if (ret)
			return ret;

		buf[i * 2] = real_data;
		buf[i * 2 + 1] = imag_data;
	}

	return samples;
}

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_device = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels),
	.read_dev = (int32_t (*)())<devname>_iio_read_samples,
};

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 */
int <devname>_iio_init(struct <devname>_iio_dev **desc,
		       struct <devname>_iio_init_param *init_param)
{
	struct <devname>_iio_dev *descriptor;
	int ret;

	descriptor = (struct <devname>_iio_dev *)no_os_calloc(1,
			sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = <devname>_init(&descriptor->dev, init_param->init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &<devname>_iio_device;

	*desc = descriptor;

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

	ret = <devname>_remove(desc->dev);
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

INCS += $(DRIVERS)/impedance-analyzer/<devname>/<devname>.h
SRCS += $(DRIVERS)/impedance-analyzer/<devname>/<devname>.c
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
INCS += $(DRIVERS)/impedance-analyzer/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/impedance-analyzer/<devname>/iio_<devname>.c

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
		.slave_address = <DEVNAME>_ADDRESS,
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA,
	},
	.current_sys_clk = <DEVNAME>_INTERNAL_SYS_CLK,
	.current_clock_source = 0,
	.current_gain = <DEVNAME>_GAIN_X1,
	.current_range = <DEVNAME>_RANGE_2000mVpp,
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
	int16_t real_data, imag_data;
	int32_t temp_mc;
	int64_t gain_factor;
	int64_t impedance;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	/* Read and display the on-chip temperature. */
	ret = <devname>_get_temperature(dev, &temp_mc);
	if (ret)
		goto free_dev;
	pr_info("Temperature: %d mC\r\n", temp_mc);

	/* Configure a frequency sweep. */
	ret = <devname>_set_range_and_gain(dev, <DEVNAME>_RANGE_2000mVpp,
					   <DEVNAME>_GAIN_X1);
	if (ret)
		goto free_dev;

	ret = <devname>_set_settling_time(dev, <DEVNAME>_SETTLING_X1,
					  <DEVNAME>_DEFAULT_SETTLING);
	if (ret)
		goto free_dev;

	ret = <devname>_config_sweep(dev, 30000, 10, 100);
	if (ret)
		goto free_dev;

	ret = <devname>_start_sweep(dev);
	if (ret)
		goto free_dev;

	/*
	 * Calibrate with a known impedance (e.g. 200 kOhm resistor).
	 * Replace the calibration value with your actual reference.
	 */
	ret = <devname>_calculate_gain_factor(dev, 200000,
					      <DEVNAME>_FUNC_REPEAT_FREQ,
					      &gain_factor);
	if (ret)
		goto free_dev;

	/* Continuously measure impedance. */
	while (1) {
		ret = <devname>_get_data(dev, <DEVNAME>_FUNC_INC_FREQ,
					&real_data, &imag_data);
		if (ret)
			goto free_dev;

		pr_info("Real: %d  Imag: %d\r\n", real_data, imag_data);
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
	struct <devname>_iio_dev *<devname>_iio;
	struct <devname>_iio_init_param <devname>_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	<devname>_iio_ip.init_param = &<devname>_ip;
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

The <DEVNAME> is a high-precision impedance converter system that
combines an on-board frequency generator with a 12-bit, 1 MSPS ADC.
It communicates over I2C and measures impedance magnitude and phase
at programmable frequencies.  The DFT engine returns real and
imaginary components for each frequency point, enabling full complex
impedance analysis.  <Add 2-3 more sentences from the datasheet:
frequency range, excitation voltage, internal system clock,
temperature sensor, typical applications.>

Applications
------------

* Electrochemical analysis
* Bioimpedance measurement
* Corrosion monitoring
* Material characterisation
* Sensor interface (capacitive / inductive)
* Industrial process monitoring

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the I2C communication interface, and stores
the initial system clock, gain, and range settings.  Error handling
releases any allocated resources on failure.

Frequency Sweep
~~~~~~~~~~~~~~~

The ``<devname>_config_sweep`` function programmes the start frequency,
frequency increment, and number of increments into the device
registers.  Frequencies are converted from Hz to 24-bit register
codes using the formula: ``code = freq_hz * 4 / sys_clk * 2^27``.

The ``<devname>_start_sweep`` function initiates the sweep sequence
(standby -> reset -> init start frequency -> start sweep) and blocks
until the first data point is valid.

Impedance Measurement
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_get_data`` function reads the 16-bit signed real and
imaginary DFT output from the device at the current frequency point.

The ``<devname>_calculate_gain_factor`` function uses a known
calibration impedance to compute a gain factor for subsequent
measurements.

The ``<devname>_calculate_impedance`` function computes the unknown
impedance from the gain factor and measured magnitude.

Range and Gain
~~~~~~~~~~~~~~

The ``<devname>_set_range_and_gain`` function configures the output
excitation voltage range (200 mVpp to 2 Vpp) and PGA gain (x1 or x5).

Temperature Sensor
~~~~~~~~~~~~~~~~~~

The ``<devname>_get_temperature`` function reads the on-chip 14-bit
temperature sensor.  The result is returned in milliCelsius.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_i2c.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .i2c_init = {
               .device_id     = 1,
               .max_speed_hz  = 400000,
               .slave_address = <DEVNAME>_ADDRESS,
               .platform_ops  = &max_i2c_ops,
               .extra         = &i2c_extra,
           },
           .current_sys_clk     = <DEVNAME>_INTERNAL_SYS_CLK,
           .current_clock_source = 0,
           .current_gain        = <DEVNAME>_GAIN_X1,
           .current_range       = <DEVNAME>_RANGE_2000mVpp,
       };

       struct <devname>_dev *dev;
       int16_t real_data, imag_data;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_config_sweep(dev, 30000, 10, 100);
       if (ret)
           goto cleanup;

       ret = <devname>_start_sweep(dev);
       if (ret)
           goto cleanup;

       ret = <devname>_get_data(dev, <DEVNAME>_FUNC_REPEAT_FREQ,
                                &real_data, &imag_data);
       if (ret)
           goto cleanup;

       /* real_data + imag_data hold the DFT output */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes two ``IIO_VOLTAGE`` input channels:
``voltage0`` (real DFT component) and ``voltage1`` (imaginary DFT
component).  Each channel provides ``raw``, ``scale``, and
``frequency`` attributes.  The IIO application can be used for
continuous impedance data acquisition during a frequency sweep.

IIO Initialization Example
---------------------------

.. code-block:: C

   int iio_example_main()
   {
       int ret;
       struct <devname>_iio_dev *iio_desc;
       struct <devname>_iio_init_param iio_ip;
       struct iio_app_desc *app;
       struct iio_app_init_param app_init_param = {0};

       iio_ip.init_param = &<devname>_ip;
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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/impedance-analyzer/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/impedance-analyzer/<devname>/README.rst
```

---

## 9. Key Conventions

1. **`_dev` not `_desc`** -- impedance analyzer drivers use
   `<devname>_dev` for the device descriptor struct.
2. **I2C communication** -- uses the address pointer protocol:
   - Set register pointer: `[ADDR_POINTER, reg_addr]` via
     `no_os_i2c_write()`.
   - Read byte: `no_os_i2c_read(..., 1, 1)`.
   - Write register: `[reg_addr, data_byte]` via
     `no_os_i2c_write(..., 2, 1)`.
   - Multi-byte registers are accessed one byte at a time.
3. **Frequency register encoding** -- 24-bit codes computed as:
   `code = freq_hz * 4 / sys_clk * 2^27`.  Use `uint64_t`
   intermediate math to avoid overflow.
4. **Fixed I2C address** -- `<DEVNAME>_ADDRESS` (no configurable
   pins, unlike temperature sensors).
5. **Sweep sequence** -- standby -> reset -> init start freq ->
   start sweep -> poll status for data valid.
6. **DFT output** -- two 16-bit signed registers (real + imaginary).
   Impedance magnitude = `1 / (gain_factor * sqrt(R^2 + I^2))`.
7. **Gain factor calibration** -- computed from a known reference
   impedance: `gain_factor = 1 / (magnitude * cal_impedance)`.
   Use integer scaling (e.g. 10^9) to avoid floating point.
8. **IIO channel type** -- `IIO_VOLTAGE` with `ch_out = 0` (input),
   two channels: `voltage0` (real) and `voltage1` (imaginary).
9. **Control register** -- split into HB (function, range, gain) and
   LB (reset, clock source).  Use `no_os_field_prep()` with masks.
10. All other conventions (C++ compat, tabs, Doxygen, `no_os_calloc`,
    goto cleanup, negative error codes, etc.) match the ADC template.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> impedance converter system with
I2C interface, programmable frequency sweep, and DFT-based
impedance measurement.

Signed-off-by: Your Name <your.name@analog.com>
```
