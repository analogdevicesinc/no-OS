# Frequency / Clock Generator Driver Template

Reference drivers: `drivers/frequency/adf4368/`, `drivers/frequency/ad9508/`

This template covers every file needed to add a new frequency / clock
generator driver to no-OS.  Replace `<devname>` with the part number
(e.g., `adf4368`) and `<DEVNAME>` with its uppercase form (e.g.,
`ADF4368`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adf4368.h`, `adf4368.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_adf4368.h`, `iio_adf4368.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADF4368_H__` |
| Init param struct | `<devname>_init_param` | `adf4368_init_param` |
| Device struct | `<devname>_dev` | `adf4368_dev` |
| IIO descriptor | `<devname>_iio_dev` | `adf4368_iio_dev` |
| Functions | `<devname>_<verb>()` | `adf4368_init()` |
| Static helpers | `_<devname>_<verb>()` | `_adf4368_calc_pfd()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADF4368` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `ADF4368_REG00_SOFT_RESET_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADF4368_CP_CURRENT_1MA` |

---

## 2. File Checklist

```
drivers/frequency/<devname>/
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

doc/sphinx/source/drivers/frequency/<devname>.rst   # Sphinx entry
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
#include "no_os_util.h"

/* ---------------- Register Addresses --------------------------------- */

#define <DEVNAME>_REG_SPI_CFG		0x0000
#define <DEVNAME>_REG_PROD_ID		0x0003
#define <DEVNAME>_REG_SCRATCH		0x000A
#define <DEVNAME>_REG_PLL_CFG		0x0010
#define <DEVNAME>_REG_N_INT_LSB		0x0011
#define <DEVNAME>_REG_N_INT_MSB		0x0012
#define <DEVNAME>_REG_FRAC_LSB		0x0013
#define <DEVNAME>_REG_FRAC_MSB		0x0014
#define <DEVNAME>_REG_MOD_LSB		0x0015
#define <DEVNAME>_REG_MOD_MSB		0x0016
#define <DEVNAME>_REG_R_DIV		0x0017
#define <DEVNAME>_REG_CP_CFG		0x0020
#define <DEVNAME>_REG_VCO_CFG		0x0025
#define <DEVNAME>_REG_OUT_CFG		0x0030
#define <DEVNAME>_REG_OUT_DIV		0x0031
#define <DEVNAME>_REG_OUT_PWR		0x0032
#define <DEVNAME>_REG_STATUS		0x0058

/* ---------------- Register Field Masks ------------------------------- */

/** Use NO_OS_BIT() for single bits, NO_OS_GENMASK(hi, lo) for fields. */
#define <DEVNAME>_SOFT_RESET_MSK	NO_OS_BIT(0)
#define <DEVNAME>_SDO_ACTIVE_MSK	NO_OS_BIT(3)
#define <DEVNAME>_N_INT_MSK		NO_OS_GENMASK(15, 0)
#define <DEVNAME>_FRAC_MSK		NO_OS_GENMASK(23, 0)
#define <DEVNAME>_MOD_MSK		NO_OS_GENMASK(23, 0)
#define <DEVNAME>_R_DIV_MSK		NO_OS_GENMASK(5, 0)
#define <DEVNAME>_CP_CURRENT_MSK	NO_OS_GENMASK(3, 0)
#define <DEVNAME>_VCO_BAND_MSK		NO_OS_GENMASK(7, 0)
#define <DEVNAME>_RF_OUT_EN_MSK		NO_OS_BIT(5)
#define <DEVNAME>_OUT_DIV_MSK		NO_OS_GENMASK(2, 0)
#define <DEVNAME>_OUT_PWR_MSK		NO_OS_GENMASK(3, 0)
#define <DEVNAME>_LOCKED_MSK		NO_OS_BIT(0)
#define <DEVNAME>_REF_OK_MSK		NO_OS_BIT(3)

/* Product / revision ID expected values */
#define <DEVNAME>_PRODUCT_ID		0xXX
#define <DEVNAME>_SCRATCH_TEST_VAL	0x5A

/* Specifications */
#define <DEVNAME>_VCO_FREQ_MIN		2200000000ULL	/* Hz */
#define <DEVNAME>_VCO_FREQ_MAX		4400000000ULL	/* Hz */
#define <DEVNAME>_RFOUT_MIN		34375000ULL	/* Hz */
#define <DEVNAME>_RFOUT_MAX		4400000000ULL	/* Hz */
#define <DEVNAME>_REFIN_MAX		250000000ULL	/* Hz */
#define <DEVNAME>_PFD_FREQ_MAX		32000000ULL	/* Hz */
#define <DEVNAME>_R_DIV_MAX		63
#define <DEVNAME>_OUT_PWR_MAX		15

/* SPI framing */
#define <DEVNAME>_SPI_WRITE_CMD		0x00
#define <DEVNAME>_SPI_READ_CMD		0x80
#define <DEVNAME>_SPI_BUFF_SIZE		3

/* ---------------- Enums ---------------------------------------------- */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_cp_current
 *  @brief Charge pump current settings. */
enum <devname>_cp_current {
	<DEVNAME>_CP_CURRENT_0_3MA,
	<DEVNAME>_CP_CURRENT_0_6MA,
	<DEVNAME>_CP_CURRENT_0_9MA,
	<DEVNAME>_CP_CURRENT_1_2MA,
	/* Add more values as needed */
};

/** @enum <devname>_out_power
 *  @brief Output power level settings. */
enum <devname>_out_power {
	<DEVNAME>_OUT_PWR_M4DBM,
	<DEVNAME>_OUT_PWR_M1DBM,
	<DEVNAME>_OUT_PWR_2DBM,
	<DEVNAME>_OUT_PWR_5DBM,
};

/* Add more enums as needed: muxout modes, lock detect, etc. */

/* ---------------- Structures ----------------------------------------- */

/**
 * @struct <devname>_output_cfg
 * @brief  Per-output channel configuration.
 */
struct <devname>_output_cfg {
	/** Output divider ratio. */
	uint32_t divider;
	/** Output power level. */
	enum <devname>_out_power power;
	/** Output enable. */
	bool enable;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Device variant. */
	enum <devname>_device_id id;
	/** SPI initialization parameters. */
	struct no_os_spi_init_param *spi_init;
	/** Reference clock frequency in Hz. */
	uint64_t ref_freq_hz;
	/** Desired output frequency in Hz. */
	uint64_t freq;
	/** Reference divider (R counter). */
	uint8_t ref_div;
	/** Enable reference doubler. */
	bool ref_doubler_en;
	/** Charge pump current setting. */
	uint8_t cp_i;
	/** Number of output channels to configure. */
	uint8_t num_outputs;
	/** Per-output configuration array. */
	struct <devname>_output_cfg *outputs;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** Device name string. */
	const char *name;
	/** Device variant. */
	enum <devname>_device_id id;
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** Reference clock frequency in Hz. */
	uint64_t ref_freq_hz;
	/** Current output frequency in Hz. */
	uint64_t freq;
	/** Phase-frequency detector frequency in Hz. */
	uint64_t pfd_freq;
	/** PLL integer divider value (N). */
	uint16_t n_int;
	/** PLL fractional value. */
	uint32_t frac;
	/** PLL modulus value. */
	uint32_t mod;
	/** Reference divider (R counter). */
	uint8_t ref_div;
	/** Reference doubler enabled. */
	bool ref_doubler_en;
	/** Charge pump current setting. */
	uint8_t cp_i;
	/** VCO band. */
	uint8_t vco_band;
	/** Number of output channels. */
	uint8_t num_outputs;
	/** Per-output configuration. */
	struct <devname>_output_cfg *outputs;
};

/* ---------------- Function Prototypes -------------------------------- */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Read a register via SPI. */
int <devname>_spi_read(struct <devname>_dev *dev, uint16_t reg_addr,
		       uint8_t *data);

/** @brief Write a register via SPI. */
int <devname>_spi_write(struct <devname>_dev *dev, uint16_t reg_addr,
			uint8_t data);

/** @brief Update specific bits in a register. */
int <devname>_spi_update_bits(struct <devname>_dev *dev, uint16_t reg_addr,
			      uint8_t mask, uint8_t data);

/** @brief Set the output frequency. */
int <devname>_set_freq(struct <devname>_dev *dev, uint64_t freq);

/** @brief Get the current output frequency. */
int <devname>_get_freq(struct <devname>_dev *dev, uint64_t *freq);

/** @brief Set the reference clock frequency. */
int <devname>_set_ref_clk(struct <devname>_dev *dev, uint64_t ref_freq_hz);

/** @brief Get the reference clock frequency. */
int <devname>_get_ref_clk(struct <devname>_dev *dev, uint64_t *ref_freq_hz);

/** @brief Set the reference divider (R counter). */
int <devname>_set_ref_div(struct <devname>_dev *dev, int32_t div);

/** @brief Get the reference divider (R counter). */
int <devname>_get_ref_div(struct <devname>_dev *dev, int32_t *div);

/** @brief Enable or disable the reference doubler. */
int <devname>_set_en_ref_doubler(struct <devname>_dev *dev, bool en);

/** @brief Get the reference doubler state. */
int <devname>_get_en_ref_doubler(struct <devname>_dev *dev, bool *en);

/** @brief Set the charge pump current. */
int <devname>_set_cp_i(struct <devname>_dev *dev, int32_t reg_val);

/** @brief Get the charge pump current setting. */
int <devname>_get_cp_i(struct <devname>_dev *dev, int32_t *reg_val);

/** @brief Set the output power for a channel. */
int <devname>_set_out_power(struct <devname>_dev *dev, uint8_t ch,
			    int32_t pwr);

/** @brief Get the output power for a channel. */
int <devname>_get_out_power(struct <devname>_dev *dev, uint8_t ch,
			    int32_t *pwr);

/** @brief Enable or disable an output channel. */
int <devname>_set_en_chan(struct <devname>_dev *dev, uint8_t ch, bool en);

/** @brief Get the enable state of an output channel. */
int <devname>_get_en_chan(struct <devname>_dev *dev, uint8_t ch, bool *en);

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

/* ---------- SPI helpers ---------------------------------------------- */

/**
 * @brief Read a register over SPI.
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param data     - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_spi_read(struct <devname>_dev *dev, uint16_t reg_addr,
		       uint8_t *data)
{
	uint8_t buf[<DEVNAME>_SPI_BUFF_SIZE];
	int ret;

	buf[0] = <DEVNAME>_SPI_READ_CMD | (uint8_t)(reg_addr >> 8);
	buf[1] = (uint8_t)(reg_addr & 0xFF);
	buf[2] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	*data = buf[2];

	return 0;
}

/**
 * @brief Write a register over SPI.
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param data     - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_spi_write(struct <devname>_dev *dev, uint16_t reg_addr,
			uint8_t data)
{
	uint8_t buf[<DEVNAME>_SPI_BUFF_SIZE];

	buf[0] = <DEVNAME>_SPI_WRITE_CMD | (uint8_t)(reg_addr >> 8);
	buf[1] = (uint8_t)(reg_addr & 0xFF);
	buf[2] = data;

	return no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
}

/**
 * @brief Update specific bits in a register (read-modify-write).
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param mask     - Bitmask of fields to update.
 * @param data     - New value for the masked bits.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_spi_update_bits(struct <devname>_dev *dev, uint16_t reg_addr,
			      uint8_t mask, uint8_t data)
{
	uint8_t reg_val;
	int ret;

	ret = <devname>_spi_read(dev, reg_addr, &reg_val);
	if (ret)
		return ret;

	reg_val = (reg_val & ~mask) | (data & mask);

	return <devname>_spi_write(dev, reg_addr, reg_val);
}

/* ---------- PLL computation helpers ---------------------------------- */

/**
 * @brief Compute PLL parameters (N, FRAC, MOD) from the desired frequency.
 * @param dev  - Device descriptor (pfd_freq must be set).
 * @param freq - Desired output frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 *
 * This is a skeleton -- adapt the math to the specific device's
 * PLL architecture (integer-N, fractional-N, etc.).
 */
static int _<devname>_calc_pll(struct <devname>_dev *dev, uint64_t freq)
{
	uint64_t vco_freq;
	uint32_t div;

	/* Determine output divider to place VCO in range */
	for (div = 1; div <= 64; div <<= 1) {
		vco_freq = freq * div;
		if (vco_freq >= <DEVNAME>_VCO_FREQ_MIN &&
		    vco_freq <= <DEVNAME>_VCO_FREQ_MAX)
			break;
	}

	if (vco_freq < <DEVNAME>_VCO_FREQ_MIN ||
	    vco_freq > <DEVNAME>_VCO_FREQ_MAX)
		return -EINVAL;

	/* Integer-N: N = VCO_freq / PFD_freq */
	dev->n_int = (uint16_t)(vco_freq / dev->pfd_freq);
	/* Fractional-N remainder (if applicable) */
	dev->frac = 0;
	dev->mod = 1;
	dev->freq = freq;

	return 0;
}

/* ---------- Public API ----------------------------------------------- */

/**
 * @brief Set the output frequency.
 * @param dev  - Device descriptor.
 * @param freq - Desired output frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_freq(struct <devname>_dev *dev, uint64_t freq)
{
	int ret;

	if (freq < <DEVNAME>_RFOUT_MIN || freq > <DEVNAME>_RFOUT_MAX)
		return -EINVAL;

	ret = _<devname>_calc_pll(dev, freq);
	if (ret)
		return ret;

	/* Program N integer */
	ret = <devname>_spi_write(dev, <DEVNAME>_REG_N_INT_LSB,
				  (uint8_t)(dev->n_int & 0xFF));
	if (ret)
		return ret;

	ret = <devname>_spi_write(dev, <DEVNAME>_REG_N_INT_MSB,
				  (uint8_t)(dev->n_int >> 8));
	if (ret)
		return ret;

	/* Program fractional value (if fractional-N device) */
	ret = <devname>_spi_write(dev, <DEVNAME>_REG_FRAC_LSB,
				  (uint8_t)(dev->frac & 0xFF));
	if (ret)
		return ret;

	ret = <devname>_spi_write(dev, <DEVNAME>_REG_FRAC_MSB,
				  (uint8_t)(dev->frac >> 8));
	if (ret)
		return ret;

	dev->freq = freq;

	return 0;
}

/**
 * @brief Get the current output frequency.
 * @param dev  - Device descriptor.
 * @param freq - Pointer to store the frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_freq(struct <devname>_dev *dev, uint64_t *freq)
{
	*freq = dev->freq;

	return 0;
}

/**
 * @brief Set the reference clock frequency.
 * @param dev         - Device descriptor.
 * @param ref_freq_hz - Reference frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_ref_clk(struct <devname>_dev *dev, uint64_t ref_freq_hz)
{
	if (ref_freq_hz > <DEVNAME>_REFIN_MAX)
		return -EINVAL;

	dev->ref_freq_hz = ref_freq_hz;

	/* Recompute PFD frequency: PFD = (REF * (1 + doubler)) / R */
	dev->pfd_freq = dev->ref_freq_hz *
			(dev->ref_doubler_en ? 2 : 1) / dev->ref_div;

	return 0;
}

/**
 * @brief Get the reference clock frequency.
 * @param dev         - Device descriptor.
 * @param ref_freq_hz - Pointer to store the reference frequency.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_ref_clk(struct <devname>_dev *dev, uint64_t *ref_freq_hz)
{
	*ref_freq_hz = dev->ref_freq_hz;

	return 0;
}

/**
 * @brief Set the reference divider (R counter).
 * @param dev - Device descriptor.
 * @param div - Divider value (1 to <DEVNAME>_R_DIV_MAX).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_ref_div(struct <devname>_dev *dev, int32_t div)
{
	int ret;

	if (div < 1 || div > <DEVNAME>_R_DIV_MAX)
		return -EINVAL;

	ret = <devname>_spi_update_bits(dev, <DEVNAME>_REG_R_DIV,
					<DEVNAME>_R_DIV_MSK,
					no_os_field_prep(<DEVNAME>_R_DIV_MSK,
							 div));
	if (ret)
		return ret;

	dev->ref_div = (uint8_t)div;
	dev->pfd_freq = dev->ref_freq_hz *
			(dev->ref_doubler_en ? 2 : 1) / dev->ref_div;

	return 0;
}

/**
 * @brief Get the reference divider (R counter).
 * @param dev - Device descriptor.
 * @param div - Pointer to store the divider value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_ref_div(struct <devname>_dev *dev, int32_t *div)
{
	*div = (int32_t)dev->ref_div;

	return 0;
}

/**
 * @brief Enable or disable the reference doubler.
 * @param dev - Device descriptor.
 * @param en  - true to enable, false to disable.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_en_ref_doubler(struct <devname>_dev *dev, bool en)
{
	dev->ref_doubler_en = en;
	dev->pfd_freq = dev->ref_freq_hz *
			(dev->ref_doubler_en ? 2 : 1) / dev->ref_div;

	return 0;
}

/**
 * @brief Get the reference doubler state.
 * @param dev - Device descriptor.
 * @param en  - Pointer to store the state.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_en_ref_doubler(struct <devname>_dev *dev, bool *en)
{
	*en = dev->ref_doubler_en;

	return 0;
}

/**
 * @brief Set the charge pump current.
 * @param dev     - Device descriptor.
 * @param reg_val - Charge pump register value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_cp_i(struct <devname>_dev *dev, int32_t reg_val)
{
	int ret;

	ret = <devname>_spi_update_bits(dev, <DEVNAME>_REG_CP_CFG,
					<DEVNAME>_CP_CURRENT_MSK,
					no_os_field_prep(
						<DEVNAME>_CP_CURRENT_MSK,
						reg_val));
	if (ret)
		return ret;

	dev->cp_i = (uint8_t)reg_val;

	return 0;
}

/**
 * @brief Get the charge pump current setting.
 * @param dev     - Device descriptor.
 * @param reg_val - Pointer to store the register value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_cp_i(struct <devname>_dev *dev, int32_t *reg_val)
{
	*reg_val = (int32_t)dev->cp_i;

	return 0;
}

/**
 * @brief Set the output power for a channel.
 * @param dev - Device descriptor.
 * @param ch  - Channel index.
 * @param pwr - Power level register value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_out_power(struct <devname>_dev *dev, uint8_t ch,
			    int32_t pwr)
{
	if (ch >= dev->num_outputs)
		return -EINVAL;

	if (pwr > <DEVNAME>_OUT_PWR_MAX)
		return -EINVAL;

	dev->outputs[ch].power = pwr;

	return <devname>_spi_update_bits(dev, <DEVNAME>_REG_OUT_PWR,
					 <DEVNAME>_OUT_PWR_MSK,
					 no_os_field_prep(
						<DEVNAME>_OUT_PWR_MSK,
						pwr));
}

/**
 * @brief Get the output power for a channel.
 * @param dev - Device descriptor.
 * @param ch  - Channel index.
 * @param pwr - Pointer to store the power level.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_out_power(struct <devname>_dev *dev, uint8_t ch,
			    int32_t *pwr)
{
	if (ch >= dev->num_outputs)
		return -EINVAL;

	*pwr = (int32_t)dev->outputs[ch].power;

	return 0;
}

/**
 * @brief Enable or disable an output channel.
 * @param dev - Device descriptor.
 * @param ch  - Channel index.
 * @param en  - true to enable, false to disable.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_en_chan(struct <devname>_dev *dev, uint8_t ch, bool en)
{
	if (ch >= dev->num_outputs)
		return -EINVAL;

	dev->outputs[ch].enable = en;

	return <devname>_spi_update_bits(dev, <DEVNAME>_REG_OUT_CFG,
					 <DEVNAME>_RF_OUT_EN_MSK,
					 no_os_field_prep(
						<DEVNAME>_RF_OUT_EN_MSK,
						en));
}

/**
 * @brief Get the enable state of an output channel.
 * @param dev - Device descriptor.
 * @param ch  - Channel index.
 * @param en  - Pointer to store the enable state.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_en_chan(struct <devname>_dev *dev, uint8_t ch, bool *en)
{
	if (ch >= dev->num_outputs)
		return -EINVAL;

	*en = dev->outputs[ch].enable;

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
	uint8_t product_id;
	uint8_t scratch;
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->id = init_param->id;
	d->name = <devname>_device_name[d->id];

	ret = no_os_spi_init(&d->spi_desc, init_param->spi_init);
	if (ret)
		goto free_dev;

	/* Software reset */
	ret = <devname>_spi_write(d, <DEVNAME>_REG_SPI_CFG,
				  <DEVNAME>_SOFT_RESET_MSK);
	if (ret)
		goto free_spi;

	no_os_mdelay(10);

	/* SPI scratchpad test */
	ret = <devname>_spi_write(d, <DEVNAME>_REG_SCRATCH,
				  <DEVNAME>_SCRATCH_TEST_VAL);
	if (ret)
		goto free_spi;

	ret = <devname>_spi_read(d, <DEVNAME>_REG_SCRATCH, &scratch);
	if (ret)
		goto free_spi;

	if (scratch != <DEVNAME>_SCRATCH_TEST_VAL) {
		pr_err("SPI scratchpad mismatch: 0x%02X\n", scratch);
		ret = -EIO;
		goto free_spi;
	}

	/* Verify product ID */
	ret = <devname>_spi_read(d, <DEVNAME>_REG_PROD_ID, &product_id);
	if (ret)
		goto free_spi;

	if (product_id != <DEVNAME>_PRODUCT_ID) {
		pr_err("Unexpected product ID: 0x%02X\n", product_id);
		ret = -ENODEV;
		goto free_spi;
	}

	/* Store configuration from init_param */
	d->ref_freq_hz = init_param->ref_freq_hz;
	d->ref_div = init_param->ref_div;
	d->ref_doubler_en = init_param->ref_doubler_en;
	d->cp_i = init_param->cp_i;
	d->num_outputs = init_param->num_outputs;
	d->outputs = init_param->outputs;

	/* Compute PFD frequency */
	d->pfd_freq = d->ref_freq_hz *
		      (d->ref_doubler_en ? 2 : 1) / d->ref_div;

	/* Program reference divider */
	ret = <devname>_set_ref_div(d, d->ref_div);
	if (ret)
		goto free_spi;

	/* Program charge pump current */
	ret = <devname>_set_cp_i(d, d->cp_i);
	if (ret)
		goto free_spi;

	/* Set output frequency */
	if (init_param->freq) {
		ret = <devname>_set_freq(d, init_param->freq);
		if (ret)
			goto free_spi;
	}

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
 * @struct <devname>_iio_dev_init_param
 * @brief  IIO initialization parameters.
 */
struct <devname>_iio_dev_init_param {
	/** Core driver init params. */
	struct <devname>_init_param *<devname>_dev_init;
};

/** @brief IIO channel attribute IDs. */
enum <devname>_iio_ch_attr_id {
	<DEVNAME>_IIO_CH_ATTR_ENABLE,
	<DEVNAME>_IIO_CH_ATTR_FREQ,
	<DEVNAME>_IIO_CH_ATTR_OPWR,
	<DEVNAME>_IIO_CH_ATTR_PHASE,
};

/** @brief IIO device attribute IDs. */
enum <devname>_iio_dev_attr_id {
	<DEVNAME>_IIO_DEV_ATTR_CP,
	<DEVNAME>_IIO_DEV_ATTR_REF_DIV,
	<DEVNAME>_IIO_DEV_ATTR_REF_DB,
	<DEVNAME>_IIO_DEV_ATTR_REF_FREQ,
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
#include <inttypes.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_<devname>.h"
#include "<devname>.h"

/* ---------- Register debug wrappers ---------------------------------- */

/**
 * @brief Wrapper for reading a register (used by IIO debug_reg_read).
 */
static int <devname>_iio_read_reg(struct <devname>_iio_dev *dev,
				  uint32_t reg, uint32_t *readval)
{
	uint8_t tmp;
	int ret;

	ret = <devname>_spi_read(dev-><devname>_dev, (uint16_t)reg, &tmp);
	if (ret)
		return ret;

	*readval = tmp;

	return 0;
}

/**
 * @brief Wrapper for writing a register (used by IIO debug_reg_write).
 */
static int <devname>_iio_write_reg(struct <devname>_iio_dev *dev,
				   uint32_t reg, uint32_t writeval)
{
	return <devname>_spi_write(dev-><devname>_dev, (uint16_t)reg,
				   (uint8_t)writeval);
}

/* ---------- Channel attribute callbacks ------------------------------ */

/**
 * @brief Read a channel attribute.
 */
static int <devname>_iio_read_ch_attr(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct <devname>_iio_dev *iio_dev = (struct <devname>_iio_dev *)dev;
	struct <devname>_dev *d = iio_dev-><devname>_dev;
	int64_t val = 0;
	uint64_t freq;
	int32_t pwr;
	bool en;
	int ret;

	switch (priv) {
	case <DEVNAME>_IIO_CH_ATTR_ENABLE:
		ret = <devname>_get_en_chan(d, channel->ch_num, &en);
		if (ret)
			return ret;
		val = en;
		break;

	case <DEVNAME>_IIO_CH_ATTR_FREQ:
		ret = <devname>_get_freq(d, &freq);
		if (ret)
			return ret;
		val = (int64_t)freq;
		break;

	case <DEVNAME>_IIO_CH_ATTR_OPWR:
		ret = <devname>_get_out_power(d, channel->ch_num, &pwr);
		if (ret)
			return ret;
		val = pwr;
		break;

	case <DEVNAME>_IIO_CH_ATTR_PHASE:
		/* Placeholder: implement if device supports phase adjust */
		val = 0;
		break;

	default:
		return -EINVAL;
	}

	return snprintf(buf, len, "%"PRIi64, val);
}

/**
 * @brief Write a channel attribute.
 */
static int <devname>_iio_write_ch_attr(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct <devname>_iio_dev *iio_dev = (struct <devname>_iio_dev *)dev;
	struct <devname>_dev *d = iio_dev-><devname>_dev;
	int64_t val;

	sscanf(buf, "%"PRIi64, &val);

	switch (priv) {
	case <DEVNAME>_IIO_CH_ATTR_ENABLE:
		return <devname>_set_en_chan(d, channel->ch_num, (bool)val);

	case <DEVNAME>_IIO_CH_ATTR_FREQ:
		return <devname>_set_freq(d, (uint64_t)val);

	case <DEVNAME>_IIO_CH_ATTR_OPWR:
		return <devname>_set_out_power(d, channel->ch_num,
					      (int32_t)val);

	case <DEVNAME>_IIO_CH_ATTR_PHASE:
		/* Placeholder: implement if device supports phase adjust */
		return 0;

	default:
		return -EINVAL;
	}
}

/* ---------- Device attribute callbacks ------------------------------- */

/**
 * @brief Read a device attribute.
 */
static int <devname>_iio_read_dev_attr(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct <devname>_iio_dev *iio_dev = (struct <devname>_iio_dev *)dev;
	struct <devname>_dev *d = iio_dev-><devname>_dev;
	int32_t val;
	uint64_t val_64;
	bool en;
	int ret;

	switch (priv) {
	case <DEVNAME>_IIO_DEV_ATTR_CP:
		ret = <devname>_get_cp_i(d, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case <DEVNAME>_IIO_DEV_ATTR_REF_DIV:
		ret = <devname>_get_ref_div(d, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case <DEVNAME>_IIO_DEV_ATTR_REF_DB:
		ret = <devname>_get_en_ref_doubler(d, &en);
		if (ret)
			return ret;
		val = en;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case <DEVNAME>_IIO_DEV_ATTR_REF_FREQ:
		ret = <devname>_get_ref_clk(d, &val_64);
		if (ret)
			return ret;
		return snprintf(buf, len, "%"PRIu64, val_64);

	default:
		return -EINVAL;
	}
}

/**
 * @brief Write a device attribute.
 */
static int <devname>_iio_write_dev_attr(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct <devname>_iio_dev *iio_dev = (struct <devname>_iio_dev *)dev;
	struct <devname>_dev *d = iio_dev-><devname>_dev;
	int32_t val;
	uint64_t val_64;
	int ret;

	switch (priv) {
	case <DEVNAME>_IIO_DEV_ATTR_CP:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;
		return <devname>_set_cp_i(d, val);

	case <DEVNAME>_IIO_DEV_ATTR_REF_DIV:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;
		return <devname>_set_ref_div(d, val);

	case <DEVNAME>_IIO_DEV_ATTR_REF_DB:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;
		return <devname>_set_en_ref_doubler(d, (bool)val);

	case <DEVNAME>_IIO_DEV_ATTR_REF_FREQ:
		sscanf(buf, "%"PRIu64, &val_64);
		return <devname>_set_ref_clk(d, val_64);

	default:
		return -EINVAL;
	}
}

/* ---------- Attribute tables ----------------------------------------- */

/** Device-level attributes (shared across all channels). */
static struct iio_attribute <devname>_iio_attrs[] = {
	{
		.name = "charge_pump_current",
		.shared = IIO_SHARED_BY_ALL,
		.priv = <DEVNAME>_IIO_DEV_ATTR_CP,
		.show = <devname>_iio_read_dev_attr,
		.store = <devname>_iio_write_dev_attr,
	},
	{
		.name = "reference_divider",
		.shared = IIO_SHARED_BY_ALL,
		.priv = <DEVNAME>_IIO_DEV_ATTR_REF_DIV,
		.show = <devname>_iio_read_dev_attr,
		.store = <devname>_iio_write_dev_attr,
	},
	{
		.name = "reference_doubler_en",
		.shared = IIO_SHARED_BY_ALL,
		.priv = <DEVNAME>_IIO_DEV_ATTR_REF_DB,
		.show = <devname>_iio_read_dev_attr,
		.store = <devname>_iio_write_dev_attr,
	},
	{
		.name = "reference_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.priv = <DEVNAME>_IIO_DEV_ATTR_REF_FREQ,
		.show = <devname>_iio_read_dev_attr,
		.store = <devname>_iio_write_dev_attr,
	},
	END_ATTRIBUTES_ARRAY
};

/** Per-channel attributes. */
static struct iio_attribute <devname>_iio_ch_attrs[] = {
	{
		.name = "en",
		.shared = IIO_SEPARATE,
		.priv = <DEVNAME>_IIO_CH_ATTR_ENABLE,
		.show = <devname>_iio_read_ch_attr,
		.store = <devname>_iio_write_ch_attr,
	},
	{
		.name = "frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.priv = <DEVNAME>_IIO_CH_ATTR_FREQ,
		.show = <devname>_iio_read_ch_attr,
		.store = <devname>_iio_write_ch_attr,
	},
	{
		.name = "output_power",
		.shared = IIO_SEPARATE,
		.priv = <DEVNAME>_IIO_CH_ATTR_OPWR,
		.show = <devname>_iio_read_ch_attr,
		.store = <devname>_iio_write_ch_attr,
	},
	{
		.name = "phase",
		.shared = IIO_SHARED_BY_TYPE,
		.priv = <DEVNAME>_IIO_CH_ATTR_PHASE,
		.show = <devname>_iio_read_ch_attr,
		.store = <devname>_iio_write_ch_attr,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel definitions -------------------------------------- */

/**
 * Frequency / clock channels: IIO_ALTVOLTAGE, ch_out = true, indexed.
 * Frequency synthesisers use altvoltage channels (not voltage).
 */
#define <DEVNAME>_IIO_CHANNEL(idx) \
	{ \
		.ch_type = IIO_ALTVOLTAGE, \
		.channel = (idx), \
		.ch_out = true, \
		.indexed = true, \
		.attributes = <devname>_iio_ch_attrs, \
		.scan_type = NULL, \
	}

static struct iio_channel <devname>_iio_channels[] = {
	<DEVNAME>_IIO_CHANNEL(0),
	<DEVNAME>_IIO_CHANNEL(1),
	/* Add more channels as needed */
};

/* ---------- IIO device structure ------------------------------------- */

static struct iio_device <devname>_iio_device = {
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels),
	.channels = <devname>_iio_channels,
	.attributes = <devname>_iio_attrs,
	.debug_reg_read = (int32_t (*)())<devname>_iio_read_reg,
	.debug_reg_write = (int32_t (*)())<devname>_iio_write_reg,
};

/* ---------- Init / Remove -------------------------------------------- */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 * @param iio_dev    - Pointer to IIO descriptor pointer (allocated here).
 * @param init_param - IIO initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_dev_init_param *init_param)
{
	struct <devname>_iio_dev *dev;
	int ret;

	dev = (struct <devname>_iio_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->iio_dev = &<devname>_iio_device;

	ret = <devname>_init(&dev-><devname>_dev,
			     init_param-><devname>_dev_init);
	if (ret)
		goto free_desc;

	*iio_dev = dev;

	return 0;

free_desc:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free IIO descriptor resources.
 * @param desc - IIO device descriptor.
 * @return 0 on success, negative error code otherwise.
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

INCS += $(DRIVERS)/frequency/<devname>/<devname>.h
SRCS += $(DRIVERS)/frequency/<devname>/<devname>.c
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
INCS += $(DRIVERS)/frequency/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/frequency/<devname>/iio_<devname>.c

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

static struct <devname>_output_cfg <devname>_outputs[] = {
	{ .divider = 1, .power = <DEVNAME>_OUT_PWR_5DBM, .enable = true },
	{ .divider = 1, .power = <DEVNAME>_OUT_PWR_5DBM, .enable = false },
};

struct <devname>_init_param <devname>_ip = {
	.id = ID_<DEVNAME>,
	.spi_init = (struct no_os_spi_init_param *)&<devname>_spi_ip,
	.ref_freq_hz = 125000000,	/* 125 MHz reference */
	.freq = 1000000000,		/* 1 GHz output */
	.ref_div = 1,
	.ref_doubler_en = false,
	.cp_i = 7,
	.num_outputs = NO_OS_ARRAY_SIZE(<devname>_outputs),
	.outputs = <devname>_outputs,
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
	uint64_t freq;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	ret = <devname>_get_freq(dev, &freq);
	if (ret)
		goto free_dev;

	pr_info("Output frequency: %llu Hz\r\n", freq);

	while (1) {
		/* Main application loop -- e.g., lock monitor */
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

The <DEVNAME> is a wideband frequency synthesiser / clock generator
with an integrated PLL and VCO, controlled via an SPI-compatible
serial interface. It provides <N> independently configurable clock
outputs with programmable dividers and output power levels. The
device supports output frequencies from <min> MHz to <max> GHz.
<Add 2-3 more sentences from the datasheet covering key specs
and features.>

Applications
------------

* Clock generation and distribution
* Wireless infrastructure (LO generation)
* Test and measurement equipment
* High-speed data converters clocking

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface, performs a software reset,
verifies the product ID via a scratchpad test, and programs the PLL
reference path and output frequency. The function uses goto-based
cleanup to release resources on failure.

Reference Path Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_ref_clk``, ``<devname>_set_ref_div``, and
``<devname>_set_en_ref_doubler`` functions configure the reference
input path. The phase-frequency detector (PFD) frequency is
computed as:

    PFD = REF_CLK x (1 + DOUBLER) / R_DIV

Frequency Programming
~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_freq`` function computes the N, FRAC, and MOD
values for the PLL, determines the output divider to keep the VCO
within its operating range, and programs the synthesiser registers.

Output Configuration
~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_out_power`` and ``<devname>_set_en_chan`` functions
control the output power level and enable state for each clock output.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct no_os_spi_init_param spi_ip = {
           /* platform-specific SPI params */
       };

       struct <devname>_output_cfg outputs[] = {
           { .divider = 1, .power = <DEVNAME>_OUT_PWR_5DBM, .enable = true },
       };

       struct <devname>_init_param ip = {
           .id = ID_<DEVNAME>,
           .spi_init = &spi_ip,
           .ref_freq_hz = 125000000,
           .freq = 1000000000,
           .ref_div = 1,
           .ref_doubler_en = false,
           .cp_i = 7,
           .num_outputs = 1,
           .outputs = outputs,
       };

       struct <devname>_dev *dev;
       uint64_t freq;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_get_freq(dev, &freq);
       if (ret)
           goto cleanup;

       /* freq contains the programmed output frequency */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each clock output as an
``IIO_ALTVOLTAGE`` channel with ``en``, ``frequency``,
``output_power``, and ``phase`` attributes. Device-level attributes
include ``charge_pump_current``, ``reference_divider``,
``reference_doubler_en``, and ``reference_frequency``.

IIO Initialization Example
----------------------------

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/frequency/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/frequency/<devname>/README.rst
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
11. **IIO channel type** -- frequency / clock drivers use
    `IIO_ALTVOLTAGE` (not `IIO_VOLTAGE`) with `ch_out = true`.
12. **SPI framing** -- frequency devices typically use 16-bit addressing
    (2 address bytes + 1 data byte); read bit is `0x80` in byte 0.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> wideband frequency synthesiser / clock
generator with SPI interface.

Signed-off-by: Your Name <your.name@analog.com>
```
