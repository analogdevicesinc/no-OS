# Resolver Driver Template

Reference driver: `drivers/resolver/ad2s1210/`

This template covers every file needed to add a new resolver-to-digital
converter (RDC) driver to no-OS. Replace `<devname>` with the part number
(e.g., `ad2s1210`) and `<DEVNAME>` with its uppercase form (e.g.,
`AD2S1210`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ad2s1210.h`, `ad2s1210.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ad2s1210.h`, `iio_ad2s1210.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __AD2S1210_H__` |
| Init param struct | `<devname>_init_param` | `ad2s1210_init_param` |
| Device struct | `<devname>_dev` | `ad2s1210_dev` |
| IIO descriptor | `<devname>_iio_desc` | `ad2s1210_iio_desc` |
| Functions | `<devname>_<verb>()` | `ad2s1210_init()` |
| Static helpers | `_<devname>_<verb>()` | `_ad2s1210_set_mode()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_AD2S1210` |
| Register addresses | `<DEVNAME>_REG_<NAME>` | `AD2S1210_REG_POSITION` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `AD2S1210_CONTROL_RES_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `AD2S1210_RES_16BIT` |

---

## 2. File Checklist

```
drivers/resolver/<devname>/
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

doc/sphinx/source/drivers/resolver/<devname>.rst   # Sphinx entry
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

/* ---------------- Register Map ----------------------------------------- */

/** Position and velocity data registers. */
#define <DEVNAME>_REG_POSITION		0x80
#define <DEVNAME>_REG_VELOCITY		0x82

/** Fault / loss-of-signal threshold registers. */
#define <DEVNAME>_REG_LOS_THRD		0x88
#define <DEVNAME>_REG_DOS_OVR_THRD	0x89
#define <DEVNAME>_REG_DOS_MIS_THRD	0x8A
#define <DEVNAME>_REG_DOS_RST_MAX_THRD	0x8B
#define <DEVNAME>_REG_DOS_RST_MIN_THRD	0x8C
#define <DEVNAME>_REG_LOT_HIGH_THRD	0x8D
#define <DEVNAME>_REG_LOT_LOW_THRD	0x8E

/** Excitation frequency and control registers. */
#define <DEVNAME>_REG_EXCIT_FREQ	0x91
#define <DEVNAME>_REG_CONTROL		0x92
#define <DEVNAME>_REG_SOFT_RESET	0xF0
#define <DEVNAME>_REG_FAULT		0xFF

/* ---------------- Control Register Bit Masks --------------------------- */

/** Resolution select — use NO_OS_BIT() for single bits,
 *  NO_OS_GENMASK(hi, lo) for multi-bit fields. */
#define <DEVNAME>_CTRL_RES_MSK		NO_OS_GENMASK(1, 0)
#define <DEVNAME>_CTRL_RES0_MSK		NO_OS_BIT(0)
#define <DEVNAME>_CTRL_RES1_MSK		NO_OS_BIT(1)
#define <DEVNAME>_CTRL_HYST_EN_MSK	NO_OS_BIT(4)

/* Minimum valid register address (for write validation). */
#define <DEVNAME>_REG_MIN		<DEVNAME>_REG_POSITION

/* ---------------- Excitation / Clock Limits ---------------------------- */

#define <DEVNAME>_MIN_CLKIN		6144000
#define <DEVNAME>_MAX_CLKIN		10240000
#define <DEVNAME>_MIN_EXCIT		2000
#define <DEVNAME>_MAX_EXCIT		20000
#define <DEVNAME>_STEP_EXCIT		250
#define <DEVNAME>_MIN_FCW		0x04
#define <DEVNAME>_MAX_FCW		0x50

/* ---------------- Channel Masks ---------------------------------------- */

#define <DEVNAME>_POS_MASK		NO_OS_BIT(0)
#define <DEVNAME>_VEL_MASK		NO_OS_BIT(1)

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_mode
 *  @brief Device operating modes, selected via A0/A1 GPIO pins. */
enum <devname>_mode {
	<DEVNAME>_MODE_POS,
	<DEVNAME>_MODE_RESERVED,
	<DEVNAME>_MODE_VEL,
	<DEVNAME>_MODE_CONFIG,
};

/** @enum <devname>_resolution
 *  @brief Converter resolution selection. */
enum <devname>_resolution {
	<DEVNAME>_RES_10BIT,
	<DEVNAME>_RES_12BIT,
	<DEVNAME>_RES_14BIT,
	<DEVNAME>_RES_16BIT,
};

/** @enum <devname>_channel
 *  @brief Measurement channel type. */
enum <devname>_channel {
	<DEVNAME>_CH_POS,
	<DEVNAME>_CH_VEL,
};

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** SPI initialization parameters. */
	struct no_os_spi_init_param spi_init;
	/** GPIO for mode select pin A0 (active mode control). */
	struct no_os_gpio_init_param gpio_a0;
	/** GPIO for mode select pin A1 (active mode control). */
	struct no_os_gpio_init_param gpio_a1;
	/** GPIO for resolution select pin RES0. */
	struct no_os_gpio_init_param gpio_res0;
	/** GPIO for resolution select pin RES1. */
	struct no_os_gpio_init_param gpio_res1;
	/** GPIO for SAMPLE latch pin. */
	struct no_os_gpio_init_param gpio_sample;
	/** Initial converter resolution (10/12/14/16 bits). */
	enum <devname>_resolution resolution;
	/** Clock input frequency in Hz. */
	uint32_t clkin_hz;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** Device name string. */
	const char *name;
	/** True if mode pins A0/A1 are connected. */
	bool have_mode_pins;
	/** True if resolution pins RES0/RES1 are connected. */
	bool have_resolution_pins;
	/** Current operating mode. */
	enum <devname>_mode mode;
	/** Active converter resolution. */
	enum <devname>_resolution resolution;
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** GPIO descriptors for mode and resolution pins. */
	struct no_os_gpio_desc *gpio_a0;
	struct no_os_gpio_desc *gpio_a1;
	struct no_os_gpio_desc *gpio_res0;
	struct no_os_gpio_desc *gpio_res1;
	/** GPIO descriptor for SAMPLE latch pin. */
	struct no_os_gpio_desc *gpio_sample;
	/** Clock input frequency in Hz. */
	uint32_t clkin_hz;
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Write a register. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t val);

/** @brief Read a register. */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *val);

/** @brief Perform a single conversion (position and/or velocity). */
int <devname>_single_conversion(struct <devname>_dev *dev,
				uint32_t active_mask,
				void *data, uint32_t size);

/** @brief Check if hysteresis is enabled. */
int <devname>_hysteresis_is_enabled(struct <devname>_dev *dev);

/** @brief Enable or disable hysteresis. */
int <devname>_set_hysteresis(struct <devname>_dev *dev, bool enable);

/** @brief Set excitation frequency and reinitialise. */
int <devname>_set_excitation_freq(struct <devname>_dev *dev,
				  uint16_t fexcit);

/** @brief Get current excitation frequency. */
int <devname>_get_excitation_freq(struct <devname>_dev *dev,
				  uint16_t *fexcit);

/** @brief Read the fault register. */
int <devname>_get_fault(struct <devname>_dev *dev, uint8_t *fault);

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

/* Forward declaration of static helpers. */
static int _<devname>_set_mode_pins(struct <devname>_dev *dev,
				    enum <devname>_mode mode);
static int _<devname>_set_resolution(struct <devname>_dev *dev,
				     enum <devname>_resolution resolution);

/* Device name lookup table (indexed by enum <devname>_device_id). */
static const char * const <devname>_device_name[] = {
	[ID_<DEVNAME>] = "<DEVNAME>",
};

/* ---------- Register Access ---------------------------------------- */

/**
 * @brief Write a register over SPI.
 * @param dev  - Device descriptor.
 * @param addr - Register address (must be >= <DEVNAME>_REG_MIN).
 * @param val  - Value to write.
 * @return 0 on success, negative error code otherwise.
 *
 * The resolver must be in CONFIG mode to access registers. This
 * function sets mode pins automatically before issuing the SPI
 * transfer. The protocol is: send address byte, then data byte.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t val)
{
	uint8_t buf;
	int ret;

	if (addr < <DEVNAME>_REG_MIN)
		return -EINVAL;

	ret = _<devname>_set_mode_pins(dev, <DEVNAME>_MODE_CONFIG);
	if (ret)
		return ret;

	buf = addr;
	ret = no_os_spi_write_and_read(dev->spi_desc, &buf, 1);
	if (ret)
		return ret;

	buf = val;
	return no_os_spi_write_and_read(dev->spi_desc, &buf, 1);
}

/**
 * @brief Read a register over SPI.
 * @param dev  - Device descriptor.
 * @param addr - Register address (must be >= <DEVNAME>_REG_MIN).
 * @param val  - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 *
 * Resolver SPI reads require two transfers: the first sends the
 * address, the second clocks out the data (which corresponds to
 * the previous cycle). A valid address is resent on SDI during
 * the read cycle to avoid undefined behaviour.
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *val)
{
	uint8_t buf;
	int ret;

	if (addr < <DEVNAME>_REG_MIN)
		return -EINVAL;

	ret = _<devname>_set_mode_pins(dev, <DEVNAME>_MODE_CONFIG);
	if (ret)
		return ret;

	buf = addr;
	ret = no_os_spi_write_and_read(dev->spi_desc, &buf, 1);
	if (ret)
		return ret;

	/* Resend address while reading result of previous cycle. */
	buf = addr;
	ret = no_os_spi_write_and_read(dev->spi_desc, &buf, 1);
	if (ret)
		return ret;

	*val = buf;

	return 0;
}

/* ---------- Static Helpers ----------------------------------------- */

/**
 * @brief Set mode select pins (A0, A1) for the desired operating mode.
 * @param dev  - Device descriptor.
 * @param mode - Target mode (POS, VEL, CONFIG).
 * @return 0 on success, negative error code otherwise.
 *
 * If mode pins are not connected the device stays in software-
 * controlled mode and this function returns immediately when the
 * requested mode matches the cached value.
 */
static int _<devname>_set_mode_pins(struct <devname>_dev *dev,
				    enum <devname>_mode mode)
{
	int ret;

	if (dev->mode == mode)
		return 0;

	ret = no_os_gpio_set_value(dev->gpio_a0, mode & NO_OS_BIT(0));
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_a1, !!(mode & NO_OS_BIT(1)));
	if (ret)
		return ret;

	dev->mode = mode;

	return 0;
}

/**
 * @brief Set device resolution (10/12/14/16 bits).
 * @param dev        - Device descriptor.
 * @param resolution - Desired resolution enum value.
 * @return 0 on success, negative error code otherwise.
 *
 * Updates both the control register and (if present) the external
 * resolution-select GPIO pins.
 */
static int _<devname>_set_resolution(struct <devname>_dev *dev,
				     enum <devname>_resolution resolution)
{
	uint8_t control;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_CONTROL, &control);
	if (ret)
		return ret;

	control &= ~<DEVNAME>_CTRL_RES_MSK;

	switch (resolution) {
	case <DEVNAME>_RES_10BIT:
		break;
	case <DEVNAME>_RES_12BIT:
		control |= <DEVNAME>_CTRL_RES0_MSK;
		break;
	case <DEVNAME>_RES_14BIT:
		control |= <DEVNAME>_CTRL_RES1_MSK;
		break;
	case <DEVNAME>_RES_16BIT:
		control |= <DEVNAME>_CTRL_RES1_MSK | <DEVNAME>_CTRL_RES0_MSK;
		break;
	default:
		pr_err("Invalid resolution: %d\n", resolution);
		return -EINVAL;
	}

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL, control);
	if (ret)
		return ret;

	/* Mirror resolution on GPIO pins if connected. */
	if (!dev->have_resolution_pins)
		return 0;

	ret = no_os_gpio_set_value(dev->gpio_res0,
				   !!(control & NO_OS_BIT(0)));
	if (ret)
		return ret;

	return no_os_gpio_set_value(dev->gpio_res1,
				    !!(control & NO_OS_BIT(1)));
}

/**
 * @brief Read position or velocity data from a single channel.
 * @param dev  - Device descriptor.
 * @param chn  - Channel enum (POS or VEL).
 * @param data - Pointer to store 16-bit result.
 * @return 0 on success, negative error code otherwise.
 *
 * When mode pins are available, the mode is set first and a single
 * 16-bit SPI transfer reads the data. Without mode pins, the
 * register address is used directly.
 */
static int _<devname>_get_channel_data(struct <devname>_dev *dev,
				       enum <devname>_channel chn,
				       uint16_t *data)
{
	uint8_t addr = <DEVNAME>_REG_POSITION;
	enum <devname>_mode mode = <DEVNAME>_MODE_POS;
	int ret;

	if (dev->have_mode_pins) {
		if (chn == <DEVNAME>_CH_VEL)
			mode = <DEVNAME>_MODE_VEL;

		ret = _<devname>_set_mode_pins(dev, mode);
		if (ret)
			return ret;

		return no_os_spi_write_and_read(dev->spi_desc,
						(uint8_t *)data, 2);
	}

	if (chn == <DEVNAME>_CH_VEL)
		addr = <DEVNAME>_REG_VELOCITY;

	ret = <devname>_reg_read(dev, addr, (uint8_t *)data);
	if (ret)
		return ret;

	return <devname>_reg_read(dev, addr + 1, (uint8_t *)data + 1);
}

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Perform a single conversion reading position and/or velocity.
 * @param dev         - Device descriptor.
 * @param active_mask - Bitmask of channels to read
 *                      (POS_MASK | VEL_MASK).
 * @param data        - Buffer to store 16-bit results.
 * @param size        - Size of data buffer in bytes.
 * @return 0 on success, negative error code otherwise.
 *
 * Pulses the SAMPLE GPIO to latch new converter data, then reads
 * the requested channels. When both position and velocity are
 * requested, the buffer must be at least 4 bytes.
 */
int <devname>_single_conversion(struct <devname>_dev *dev,
				uint32_t active_mask,
				void *data, uint32_t size)
{
	uint16_t *data_p = (uint16_t *)data;
	int ret;

	if (size < 2)
		return -EINVAL;

	if ((size < 4) && (active_mask & <DEVNAME>_POS_MASK)
	    && (active_mask & <DEVNAME>_VEL_MASK))
		return -EINVAL;

	/* Pulse SAMPLE pin to latch conversion result. */
	ret = no_os_gpio_set_value(dev->gpio_sample, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_sample, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	if (active_mask & <DEVNAME>_POS_MASK) {
		ret = _<devname>_get_channel_data(dev, <DEVNAME>_CH_POS,
						  data_p++);
		if (ret)
			return ret;
	}

	if (active_mask & <DEVNAME>_VEL_MASK) {
		ret = _<devname>_get_channel_data(dev, <DEVNAME>_CH_VEL,
						  data_p);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Check if hysteresis is enabled.
 * @param dev - Device descriptor.
 * @return 1 if enabled, 0 if disabled, negative error code on failure.
 */
int <devname>_hysteresis_is_enabled(struct <devname>_dev *dev)
{
	uint8_t control;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_CONTROL, &control);
	if (ret)
		return ret;

	if (control & <DEVNAME>_CTRL_HYST_EN_MSK)
		return 1;

	return 0;
}

/**
 * @brief Enable or disable hysteresis.
 * @param dev    - Device descriptor.
 * @param enable - true to enable, false to disable.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_hysteresis(struct <devname>_dev *dev, bool enable)
{
	uint8_t control;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_CONTROL, &control);
	if (ret)
		return ret;

	control &= ~<DEVNAME>_CTRL_HYST_EN_MSK;
	if (enable)
		control |= <DEVNAME>_CTRL_HYST_EN_MSK;

	return <devname>_reg_write(dev, <DEVNAME>_REG_CONTROL, control);
}

/**
 * @brief Set excitation frequency and perform a soft reset to
 *        reinitialise the excitation output.
 * @param dev    - Device descriptor.
 * @param fexcit - Desired excitation frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_excitation_freq(struct <devname>_dev *dev,
				  uint16_t fexcit)
{
	int8_t fcw;
	int ret;

	fcw = fexcit * (1 << 15) / dev->clkin_hz;
	if (fcw < <DEVNAME>_MIN_FCW || fcw > <DEVNAME>_MAX_FCW)
		return -ERANGE;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_EXCIT_FREQ, fcw);
	if (ret)
		return ret;

	/* Soft reset reinitialises the excitation output;
	 * it does not reset configuration registers. */
	return <devname>_reg_write(dev, <DEVNAME>_REG_SOFT_RESET, 0);
}

/**
 * @brief Get the current excitation frequency.
 * @param dev    - Device descriptor.
 * @param fexcit - Pointer to store frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_excitation_freq(struct <devname>_dev *dev,
				  uint16_t *fexcit)
{
	uint8_t val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_EXCIT_FREQ, &val);
	if (ret)
		return ret;

	*fexcit = val * dev->clkin_hz / (1 << 15);

	return 0;
}

/**
 * @brief Read the fault register.
 * @param dev   - Device descriptor.
 * @param fault - Pointer to store fault flags.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_fault(struct <devname>_dev *dev, uint8_t *fault)
{
	return <devname>_reg_read(dev, <DEVNAME>_REG_FAULT, fault);
}

/**
 * @brief Initialise the <DEVNAME> resolver-to-digital converter.
 * @param dev        - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 *
 * Allocates the device descriptor, initialises GPIO pins (mode,
 * resolution, sample), opens the SPI interface, validates the
 * clock frequency, and sets the initial resolution. Uses goto-based
 * cleanup to release resources in reverse order on failure.
 */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *d;
	int ret;

	if (init_param->clkin_hz < <DEVNAME>_MIN_CLKIN
	    || init_param->clkin_hz > <DEVNAME>_MAX_CLKIN) {
		pr_err("Clock frequency out of range: %lu\n",
		       (unsigned long)init_param->clkin_hz);
		return -EINVAL;
	}

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->name = <devname>_device_name[0];
	d->mode = <DEVNAME>_MODE_CONFIG;
	d->resolution = init_param->resolution;
	d->clkin_hz = init_param->clkin_hz;

	ret = no_os_gpio_get(&d->gpio_sample, &init_param->gpio_sample);
	if (ret)
		goto free_dev;

	ret = no_os_gpio_get_optional(&d->gpio_a0, &init_param->gpio_a0);
	if (ret)
		goto free_sample;

	ret = no_os_gpio_get_optional(&d->gpio_a1, &init_param->gpio_a1);
	if (ret)
		goto free_a0;

	ret = no_os_gpio_get_optional(&d->gpio_res0, &init_param->gpio_res0);
	if (ret)
		goto free_a1;

	ret = no_os_gpio_get_optional(&d->gpio_res1, &init_param->gpio_res1);
	if (ret)
		goto free_res0;

	/* Configure all GPIOs as outputs, default high. */
	ret = no_os_gpio_direction_output(d->gpio_sample, NO_OS_GPIO_HIGH);
	if (ret)
		goto free_res1;

	if (d->gpio_a0) {
		ret = no_os_gpio_direction_output(d->gpio_a0,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto free_res1;
	}

	if (d->gpio_a1) {
		ret = no_os_gpio_direction_output(d->gpio_a1,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto free_res1;
	}

	if (d->gpio_res0) {
		ret = no_os_gpio_direction_output(d->gpio_res0,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto free_res1;
	}

	if (d->gpio_res1) {
		ret = no_os_gpio_direction_output(d->gpio_res1,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto free_res1;
	}

	ret = no_os_spi_init(&d->spi_desc, &init_param->spi_init);
	if (ret)
		goto free_res1;

	if (d->gpio_a0 && d->gpio_a1)
		d->have_mode_pins = true;

	if (d->gpio_res0 && d->gpio_res1)
		d->have_resolution_pins = true;

	ret = _<devname>_set_resolution(d, d->resolution);
	if (ret)
		goto free_spi;

	*dev = d;

	return 0;

free_spi:
	no_os_spi_remove(d->spi_desc);
free_res1:
	no_os_gpio_remove(d->gpio_res1);
free_res0:
	no_os_gpio_remove(d->gpio_res0);
free_a1:
	no_os_gpio_remove(d->gpio_a1);
free_a0:
	no_os_gpio_remove(d->gpio_a0);
free_sample:
	no_os_gpio_remove(d->gpio_sample);
free_dev:
	no_os_free(d);

	return ret;
}

/**
 * @brief Free resources allocated by <devname>_init().
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 *
 * Frees resources in reverse order of allocation: SPI, then
 * GPIOs (res1, res0, a1, a0, sample), then the descriptor itself.
 */
int <devname>_remove(struct <devname>_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	if (dev->gpio_res1) {
		ret = no_os_gpio_remove(dev->gpio_res1);
		if (ret)
			return ret;
	}

	if (dev->gpio_res0) {
		ret = no_os_gpio_remove(dev->gpio_res0);
		if (ret)
			return ret;
	}

	if (dev->gpio_a1) {
		ret = no_os_gpio_remove(dev->gpio_a1);
		if (ret)
			return ret;
	}

	if (dev->gpio_a0) {
		ret = no_os_gpio_remove(dev->gpio_a0);
		if (ret)
			return ret;
	}

	if (dev->gpio_sample) {
		ret = no_os_gpio_remove(dev->gpio_sample);
		if (ret)
			return ret;
	}

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
 * Copyright YYYY(c) Analog Devices, Inc.
 *
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
 * Copyright YYYY(c) Analog Devices, Inc.
 *
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
 * @brief Read raw angular position for the angle channel.
 *
 * Returns the 16-bit (or configured resolution) position value
 * as an unsigned integer. The raw value represents the shaft
 * angle where the full range maps to 0-360 degrees.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	uint16_t data[2];
	uint32_t mask;
	int32_t val;
	int ret;

	if (channel->ch_num == 0)
		mask = <DEVNAME>_POS_MASK;
	else
		mask = <DEVNAME>_VEL_MASK;

	ret = <devname>_single_conversion(desc-><devname>_dev, mask,
					  data, sizeof(data));
	if (ret)
		return ret;

	val = (int32_t)data[0];

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor for the angle channel.
 *
 * For a 16-bit resolver: scale = 360 / 65536 = 0.005493 degrees
 * per LSB. Expressed as IIO_VAL_INT_PLUS_MICRO: 0 + 5493.
 *
 * For velocity: scale depends on clock and resolution.
 * Adjust calculation per the device datasheet.
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	if (channel->ch_num == 0) {
		/* Position: 360 deg / 65536 = 0.005493 deg/LSB */
		val[0] = 0;
		val[1] = 5493;
	} else {
		/* Velocity: adjust per device datasheet. */
		val[0] = 0;
		val[1] = 1;
	}

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/**
 * @brief Read the excitation frequency attribute.
 */
static int <devname>_iio_read_excit_freq(void *dev, char *buf,
					 uint32_t len,
					 const struct iio_ch_info *channel,
					 intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	uint16_t fexcit;
	int32_t val;
	int ret;

	ret = <devname>_get_excitation_freq(desc-><devname>_dev, &fexcit);
	if (ret)
		return ret;

	val = (int32_t)fexcit;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write the excitation frequency attribute.
 */
static int <devname>_iio_write_excit_freq(void *dev, char *buf,
					  uint32_t len,
					  const struct iio_ch_info *channel,
					  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	uint16_t fexcit;

	fexcit = no_os_str_to_uint32(buf);

	return <devname>_set_excitation_freq(desc-><devname>_dev, fexcit);
}

/**
 * @brief Read the hysteresis enable attribute.
 */
static int <devname>_iio_read_hysteresis(void *dev, char *buf,
					 uint32_t len,
					 const struct iio_ch_info *channel,
					 intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int32_t val;
	int ret;

	ret = <devname>_hysteresis_is_enabled(desc-><devname>_dev);
	if (ret < 0)
		return ret;

	val = ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/* ---------- Attribute tables --------------------------------------- */

/** Channel attributes for position (angle) channel. */
static struct iio_attribute <devname>_iio_angle_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_raw },
	{ .name = "scale", .show = <devname>_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

/** Channel attributes for velocity channel. */
static struct iio_attribute <devname>_iio_vel_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_raw },
	{ .name = "scale", .show = <devname>_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

/** Device-level attributes. */
static struct iio_attribute <devname>_iio_dev_attrs[] = {
	{
		.name = "excitation_frequency",
		.show = <devname>_iio_read_excit_freq,
		.store = <devname>_iio_write_excit_freq,
	},
	{
		.name = "hysteresis_enable",
		.show = <devname>_iio_read_hysteresis,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan types --------------------------------------------- */

static struct scan_type <devname>_pos_scan_type = {
	.sign = 'u',          /* unsigned position */
	.realbits = 16,       /* resolver resolution (adjust per config) */
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = true,
};

static struct scan_type <devname>_vel_scan_type = {
	.sign = 's',          /* signed velocity (direction-dependent) */
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = true,
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * Resolver channels:
 *   Channel 0: IIO_ANGL   — angular position (degrees)
 *   Channel 1: IIO_ANGL_VEL — angular velocity (deg/s)
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "angl0",
		.ch_type = IIO_ANGL,
		.channel = 0,
		.scan_type = &<devname>_pos_scan_type,
		.scan_index = 0,
		.attributes = <devname>_iio_angle_attrs,
		.ch_out = false,
		.indexed = true,
	},
	{
		.name = "anglvel0",
		.ch_type = IIO_ANGL_VEL,
		.channel = 1,
		.scan_type = &<devname>_vel_scan_type,
		.scan_index = 1,
		.attributes = <devname>_iio_vel_attrs,
		.ch_out = false,
		.indexed = true,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- Buffered read callback --------------------------------- */

/**
 * @brief Read resolver samples into an IIO buffer.
 *
 * Reads position and velocity based on active channel mask.
 */
static int <devname>_iio_read_samples(void *dev, int16_t *buf,
				      uint32_t samples)
{
	struct <devname>_iio_desc *desc = dev;
	uint32_t i;
	int ret;

	for (i = 0; i < samples; i++) {
		ret = <devname>_single_conversion(
			desc-><devname>_dev,
			<DEVNAME>_POS_MASK | <DEVNAME>_VEL_MASK,
			&buf[i * 2], 4);
		if (ret)
			return ret;
	}

	return samples;
}

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels) - 1,
	.attributes = <devname>_iio_dev_attrs,
	.debug_reg_read = (int32_t (*)())<devname>_reg_read,
	.debug_reg_write = (int32_t (*)())<devname>_reg_write,
	.read_dev = (int32_t (*)())<devname>_iio_read_samples,
};

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 * @param desc       - Pointer to IIO descriptor pointer (allocated here).
 * @param init_param - IIO initialization parameters.
 * @return 0 on success, negative error code otherwise.
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
 * @param desc - IIO descriptor.
 * @return 0 on success, negative error code otherwise.
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
  "maxim": {
    "basic_example_max32665": {
      "flags" : "BASIC_EXAMPLE=y IIO_EXAMPLE=n TARGET=max32665"
    },
    "iio_example_max32665": {
      "flags" : "BASIC_EXAMPLE=n IIO_EXAMPLE=y TARGET=max32665"
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

INCS += $(DRIVERS)/resolver/<devname>/<devname>.h
SRCS += $(DRIVERS)/resolver/<devname>/<devname>.c
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
INCS += $(DRIVERS)/resolver/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/resolver/<devname>/iio_<devname>.c

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
extern const struct no_os_gpio_init_param <devname>_gpio_a0;
extern const struct no_os_gpio_init_param <devname>_gpio_a1;
extern const struct no_os_gpio_init_param <devname>_gpio_res0;
extern const struct no_os_gpio_init_param <devname>_gpio_res1;
extern const struct no_os_gpio_init_param <devname>_gpio_sample;
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

const struct no_os_gpio_init_param <devname>_gpio_a0 = {
	.port = GPIO_A0_PORT_NUM,
	.number = GPIO_A0_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param <devname>_gpio_a1 = {
	.port = GPIO_A1_PORT_NUM,
	.number = GPIO_A1_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param <devname>_gpio_res0 = {
	.port = GPIO_RES0_PORT_NUM,
	.number = GPIO_RES0_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param <devname>_gpio_res1 = {
	.port = GPIO_RES1_PORT_NUM,
	.number = GPIO_RES1_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param <devname>_gpio_sample = {
	.port = GPIO_SAMPLE_PORT_NUM,
	.number = GPIO_SAMPLE_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct <devname>_init_param <devname>_ip = {
	.spi_init = <devname>_spi_ip,
	.gpio_a0 = <devname>_gpio_a0,
	.gpio_a1 = <devname>_gpio_a1,
	.gpio_res0 = <devname>_gpio_res0,
	.gpio_res1 = <devname>_gpio_res1,
	.gpio_sample = <devname>_gpio_sample,
	.resolution = <DEVNAME>_RES_16BIT,
	.clkin_hz = 8192000,
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
	uint16_t data[2];
	uint8_t fault;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	while (1) {
		/* Read both position and velocity. */
		ret = <devname>_single_conversion(dev,
						  <DEVNAME>_POS_MASK |
						  <DEVNAME>_VEL_MASK,
						  data, sizeof(data));
		if (ret)
			goto free_dev;

		pr_info("Position: %u  Velocity: %d\r\n",
			data[0], (int16_t)data[1]);

		/* Check for faults. */
		ret = <devname>_get_fault(dev, &fault);
		if (ret)
			goto free_dev;

		if (fault)
			pr_info("Fault register: 0x%02X\r\n", fault);

		no_os_mdelay(100);
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

/* Mode select pins */
#define GPIO_A0_PORT_NUM	0
#define GPIO_A0_PIN_NUM		19
#define GPIO_A1_PORT_NUM	0
#define GPIO_A1_PIN_NUM		20

/* Resolution select pins */
#define GPIO_RES0_PORT_NUM	0
#define GPIO_RES0_PIN_NUM	21
#define GPIO_RES1_PORT_NUM	0
#define GPIO_RES1_PIN_NUM	22

/* SAMPLE latch pin */
#define GPIO_SAMPLE_PORT_NUM	0
#define GPIO_SAMPLE_PIN_NUM	23

#define GPIO_OPS	&max_gpio_ops
#define GPIO_EXTRA	&max_gpio_extra

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;
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

struct max_gpio_init_param max_gpio_extra = {
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

The <DEVNAME> is a <resolution>-bit resolver-to-digital converter with
an SPI-compatible serial interface. It converts the analog signals from
a resolver sensor into digital position and velocity data, supporting
configurable resolutions of 10, 12, 14, or 16 bits. The device features
a programmable excitation frequency, fault detection for loss-of-signal
(LOS), degradation-of-signal (DOS), and loss-of-tracking (LOT)
conditions, and GPIO-based mode and resolution selection.

Applications
------------

* Motor control and servo systems
* Robotics and CNC machinery
* Angular position and velocity sensing
* Industrial automation and motion control

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises GPIO pins for mode selection (A0/A1),
resolution selection (RES0/RES1), and sample latching (SAMPLE),
opens the SPI interface, validates the clock frequency, and sets
the initial converter resolution. The function uses goto-based
cleanup to release resources in reverse order on failure.

Mode and Resolution Selection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The device mode (position, velocity, or configuration) is controlled
via the A0 and A1 GPIO pins. The ``_<devname>_set_mode_pins`` helper
sets these pins automatically before register access or data reads.
Resolution can be configured from 10 to 16 bits using the control
register and optional RES0/RES1 GPIO pins.

Position and Velocity Measurement
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_single_conversion`` function pulses the SAMPLE GPIO
pin to latch new conversion data, then reads position and/or velocity
based on the active channel mask. Position is returned as an unsigned
value representing 0-360 degrees. Velocity is returned as a signed
value proportional to angular speed.

Fault Detection
~~~~~~~~~~~~~~~

The ``<devname>_get_fault`` function reads the fault register, which
reports loss-of-signal, degradation-of-signal, and loss-of-tracking
conditions. Threshold registers allow configuring fault sensitivity.

Excitation Frequency
~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_excitation_freq`` function programs the excitation
frequency control word and performs a soft reset to reinitialise the
excitation output. The ``<devname>_get_excitation_freq`` function reads
back the current excitation frequency.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .spi_init = { /* platform-specific SPI params */ },
           .gpio_a0 = { /* A0 mode pin params */ },
           .gpio_a1 = { /* A1 mode pin params */ },
           .gpio_res0 = { /* RES0 pin params */ },
           .gpio_res1 = { /* RES1 pin params */ },
           .gpio_sample = { /* SAMPLE pin params */ },
           .resolution = <DEVNAME>_RES_16BIT,
           .clkin_hz = 8192000,
       };

       struct <devname>_dev *dev;
       uint16_t data[2];
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_single_conversion(dev,
                                         <DEVNAME>_POS_MASK |
                                         <DEVNAME>_VEL_MASK,
                                         data, sizeof(data));
       if (ret)
           goto cleanup;

       /* data[0] = position, data[1] = velocity */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes two channels: an ``IIO_ANGL``
channel for angular position (``angl0``) and an ``IIO_ANGL_VEL``
channel for angular velocity (``anglvel0``). Each channel provides
``raw`` and ``scale`` attributes. Device-level attributes expose
the excitation frequency and hysteresis configuration. The IIO
application can be used for real-time position and velocity
monitoring.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/resolver/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/resolver/<devname>/README.rst
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
11. **GPIO mode pins** -- resolver drivers typically use GPIO pins for
    mode selection (A0/A1) and resolution selection (RES0/RES1).
    Use `no_os_gpio_get_optional()` for pins that may not be wired.
12. **SAMPLE latch** -- pulse the SAMPLE GPIO (low then high) before
    reading conversion data to latch fresh results.
13. **IIO channel types** -- use `IIO_ANGL` for angular position and
    `IIO_ANGL_VEL` for angular velocity channels.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> <resolution>-bit resolver-to-digital
converter with SPI interface, configurable resolution, excitation
frequency control, and fault detection.

Signed-off-by: Your Name <your.name@analog.com>
```
