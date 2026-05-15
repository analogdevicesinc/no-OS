# Filter Driver Template

Reference driver: `drivers/filter/admv8818/`

This template covers every file needed to add a new RF/signal filter driver
to no-OS. Replace `<devname>` with the part number (e.g., `admv8818`) and
`<DEVNAME>` with its uppercase form (e.g., `ADMV8818`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `admv8818.h`, `admv8818.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_admv8818.h`, `iio_admv8818.c` |
| Include guard | `#ifndef <DEVNAME>_H_` | `#ifndef ADMV8818_H_` |
| Init param struct | `<devname>_init_param` | `admv8818_init_param` |
| Device struct | `<devname>_dev` | `admv8818_dev` |
| IIO descriptor | `<devname>_iio_desc` | `admv8818_iio_desc` |
| Functions | `<devname>_<verb>()` | `admv8818_init()` |
| Static helpers | `_<devname>_<verb>()` | `_admv8818_compute_band()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADMV8818` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `ADMV8818_HPF_WR0_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADMV8818_AUTO` |

---

## 2. File Checklist

```
drivers/filter/<devname>/
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

doc/sphinx/source/drivers/filter/<devname>.rst   # Sphinx entry
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
#ifndef <DEVNAME>_H_
#define <DEVNAME>_H_

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_util.h"

/* ---------------- Register Map --------------------------------------------- */

#define <DEVNAME>_REG_SPI_CONFIG_A		0x00
#define <DEVNAME>_REG_SPI_CONFIG_B		0x01
#define <DEVNAME>_REG_CHIPTYPE			0x03
#define <DEVNAME>_REG_PRODUCT_ID_L		0x04
#define <DEVNAME>_REG_PRODUCT_ID_H		0x05
#define <DEVNAME>_REG_WR0_SW			0x20
#define <DEVNAME>_REG_WR0_FILTER		0x21

/* ---------------- Configuration Field Masks -------------------------------- */

/** SPI configuration masks — use NO_OS_BIT() for single bits,
 *  NO_OS_GENMASK(hi, lo) for multi-bit fields. */
#define <DEVNAME>_SOFTRESET_N_MSK		NO_OS_BIT(7)
#define <DEVNAME>_SOFTRESET_MSK			NO_OS_BIT(0)
#define <DEVNAME>_SDOACTIVE_N_MSK		NO_OS_BIT(4)
#define <DEVNAME>_SDOACTIVE_MSK			NO_OS_BIT(3)
#define <DEVNAME>_SINGLE_INSTRUCTION_MSK	NO_OS_BIT(7)

/* Switch control masks */
#define <DEVNAME>_SW_IN_SET_WR0_MSK		NO_OS_BIT(7)
#define <DEVNAME>_SW_OUT_SET_WR0_MSK		NO_OS_BIT(6)
#define <DEVNAME>_SW_IN_WR0_MSK			NO_OS_GENMASK(5, 3)
#define <DEVNAME>_SW_OUT_WR0_MSK		NO_OS_GENMASK(2, 0)

/* Filter control masks */
#define <DEVNAME>_HPF_WR0_MSK			NO_OS_GENMASK(7, 4)
#define <DEVNAME>_LPF_WR0_MSK			NO_OS_GENMASK(3, 0)

/* Specifications */
#define <DEVNAME>_BUFF_SIZE_BYTES		3
#define <DEVNAME>_CHIP_ID			0xXX
#define <DEVNAME>_SPI_READ_CMD			NO_OS_BIT(7)

/* ---------------- Enums ---------------------------------------------------- */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_filter_mode
 *  @brief Filter operating modes. */
enum <devname>_filter_mode {
	/** Automatic: HPF and LPF configured from RF input frequency. */
	<DEVNAME>_AUTO,
	/** Manual: HPF and LPF configured independently by the user. */
	<DEVNAME>_MANUAL,
};

/* ---------------- Structures ----------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** SPI initialization parameters. */
	struct no_os_spi_init_param	*spi_init;
	/** RF input frequency (Hz) — used in AUTO mode. */
	unsigned long long		rf_in;
	/** Filter operating mode (AUTO or MANUAL). */
	enum <devname>_filter_mode	mode;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** SPI descriptor. */
	struct no_os_spi_desc		*spi_desc;
	/** RF input frequency (Hz). */
	unsigned long long		rf_in;
	/** Current filter operating mode. */
	enum <devname>_filter_mode	mode;
};

/* ---------------- Function Prototypes -------------------------------------- */

/** @brief Write a register over SPI. */
int <devname>_spi_write(struct <devname>_dev *dev, uint16_t reg_addr,
			uint8_t data);

/** @brief Read a register over SPI. */
int <devname>_spi_read(struct <devname>_dev *dev, uint16_t reg_addr,
		       uint8_t *data);

/** @brief Update specific bits in a register. */
int <devname>_spi_update_bits(struct <devname>_dev *dev, uint16_t reg_addr,
			      uint8_t mask, uint8_t data);

/** @brief Set the high-pass filter cutoff frequency. */
int <devname>_hpf_select(struct <devname>_dev *dev, unsigned long long freq);

/** @brief Read the current high-pass filter cutoff frequency. */
int <devname>_read_hpf_freq(struct <devname>_dev *dev,
			    unsigned long long *freq);

/** @brief Set the low-pass filter cutoff frequency. */
int <devname>_lpf_select(struct <devname>_dev *dev, unsigned long long freq);

/** @brief Read the current low-pass filter cutoff frequency. */
int <devname>_read_lpf_freq(struct <devname>_dev *dev,
			    unsigned long long *freq);

/** @brief Select RF input band (configures HPF and LPF from rf_in). */
int <devname>_rfin_select(struct <devname>_dev *dev);

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

#endif /* <DEVNAME>_H_ */
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
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_units.h"
#include "no_os_util.h"

/* HPF frequency ranges per band [min, max] in Hz. */
static const unsigned long long freq_range_hpf[4][2] = {
	{1750000000ULL, 3550000000ULL},
	{3400000000ULL, 7250000000ULL},
	{6600000000ULL, 12000000000ULL},
	{12500000000ULL, 19900000000ULL}
};

/* LPF frequency ranges per band [min, max] in Hz. */
static const unsigned long long freq_range_lpf[4][2] = {
	{2050000000ULL, 3850000000ULL},
	{3350000000ULL, 7250000000ULL},
	{7000000000ULL, 13000000000ULL},
	{12550000000ULL, 18500000000ULL}
};

/* ---------- Register access ---------------------------------------- */

/**
 * @brief Write a register over SPI.
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param data     - Data value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_spi_write(struct <devname>_dev *dev, uint16_t reg_addr,
			uint8_t data)
{
	uint8_t buff[<DEVNAME>_BUFF_SIZE_BYTES];

	buff[0] = no_os_field_get(NO_OS_GENMASK(15, 8), reg_addr);
	buff[1] = no_os_field_get(NO_OS_GENMASK(7, 0), reg_addr);
	buff[2] = data;

	return no_os_spi_write_and_read(dev->spi_desc, buff,
					<DEVNAME>_BUFF_SIZE_BYTES);
}

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
	uint8_t buff[<DEVNAME>_BUFF_SIZE_BYTES];
	int ret;

	buff[0] = <DEVNAME>_SPI_READ_CMD |
		  no_os_field_get(NO_OS_GENMASK(15, 8), reg_addr);
	buff[1] = no_os_field_get(NO_OS_GENMASK(7, 0), reg_addr);
	buff[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff,
				       <DEVNAME>_BUFF_SIZE_BYTES);
	if (ret)
		return ret;

	*data = buff[1];

	return 0;
}

/**
 * @brief Update specific bits in a register (read-modify-write).
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param mask     - Mask for bits to be updated.
 * @param data     - Data value (requires prior bit shifting).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_spi_update_bits(struct <devname>_dev *dev, uint16_t reg_addr,
			      uint8_t mask, uint8_t data)
{
	uint8_t read_val;
	int ret;

	ret = <devname>_spi_read(dev, reg_addr, &read_val);
	if (ret)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	return <devname>_spi_write(dev, reg_addr, read_val);
}

/* ---------- Filter control ----------------------------------------- */

/**
 * @brief Set the high-pass filter cutoff frequency.
 * @param dev  - Device descriptor.
 * @param freq - HPF cutoff frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 *
 * Selects the closest HPF band and state for the given frequency.
 * Each band has 16 states (4-bit control) for fine adjustment.
 */
int <devname>_hpf_select(struct <devname>_dev *dev, unsigned long long freq)
{
	unsigned int hpf_step = 0, hpf_band = 0, i, j;
	unsigned long long freq_step;
	int ret;

	if (freq < freq_range_hpf[0][0])
		goto hpf_write;

	if (freq > freq_range_hpf[3][1]) {
		hpf_step = 15;
		hpf_band = 4;
		goto hpf_write;
	}

	for (i = 0; i < 4; i++) {
		freq_step = (freq_range_hpf[i][1] - freq_range_hpf[i][0]) / 15;

		if (freq > freq_range_hpf[i][0] &&
		    (freq < freq_range_hpf[i][1] + freq_step)) {
			hpf_band = i + 1;

			for (j = 1; j <= 16; j++) {
				if (freq < (freq_range_hpf[i][0] + (freq_step * j))) {
					hpf_step = j - 1;
					break;
				}
			}
			break;
		}
	}

hpf_write:
	ret = <devname>_spi_update_bits(dev, <DEVNAME>_REG_WR0_SW,
					<DEVNAME>_SW_IN_SET_WR0_MSK |
					<DEVNAME>_SW_IN_WR0_MSK,
					no_os_field_prep(<DEVNAME>_SW_IN_SET_WR0_MSK, 1) |
					no_os_field_prep(<DEVNAME>_SW_IN_WR0_MSK, hpf_band));
	if (ret)
		return ret;

	return <devname>_spi_update_bits(dev, <DEVNAME>_REG_WR0_FILTER,
					 <DEVNAME>_HPF_WR0_MSK,
					 no_os_field_prep(<DEVNAME>_HPF_WR0_MSK, hpf_step));
}

/**
 * @brief Read the current high-pass filter cutoff frequency.
 * @param dev  - Device descriptor.
 * @param freq - Pointer to store HPF frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read_hpf_freq(struct <devname>_dev *dev,
			    unsigned long long *freq)
{
	unsigned int hpf_band, hpf_state;
	uint8_t data;
	int ret;

	ret = <devname>_spi_read(dev, <DEVNAME>_REG_WR0_SW, &data);
	if (ret)
		return ret;

	hpf_band = no_os_field_get(<DEVNAME>_SW_IN_WR0_MSK, data);
	if (!hpf_band || hpf_band > 4) {
		*freq = 0;
		return 0;
	}

	ret = <devname>_spi_read(dev, <DEVNAME>_REG_WR0_FILTER, &data);
	if (ret)
		return ret;

	hpf_state = no_os_field_get(<DEVNAME>_HPF_WR0_MSK, data);

	*freq = (freq_range_hpf[hpf_band - 1][1] -
		 freq_range_hpf[hpf_band - 1][0]) / 15;
	*freq = freq_range_hpf[hpf_band - 1][0] + (*freq * hpf_state);

	return 0;
}

/**
 * @brief Set the low-pass filter cutoff frequency.
 * @param dev  - Device descriptor.
 * @param freq - LPF cutoff frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 *
 * Selects the closest LPF band and state for the given frequency.
 * Each band has 16 states (4-bit control) for fine adjustment.
 */
int <devname>_lpf_select(struct <devname>_dev *dev, unsigned long long freq)
{
	unsigned int lpf_step = 0, lpf_band = 0, i, j;
	unsigned long long freq_step;
	int ret;

	if (freq > freq_range_lpf[3][1])
		goto lpf_write;

	if (freq < freq_range_lpf[0][0]) {
		lpf_band = 1;
		goto lpf_write;
	}

	for (i = 0; i < 4; i++) {
		if (freq > freq_range_lpf[i][0] && freq < freq_range_lpf[i][1]) {
			lpf_band = i + 1;
			freq_step = (freq_range_lpf[i][1] -
				     freq_range_lpf[i][0]) / 15;

			for (j = 0; j <= 15; j++) {
				if (freq < (freq_range_lpf[i][0] + (freq_step * j))) {
					lpf_step = j;
					break;
				}
			}
			break;
		}
	}

lpf_write:
	ret = <devname>_spi_update_bits(dev, <DEVNAME>_REG_WR0_SW,
					<DEVNAME>_SW_OUT_SET_WR0_MSK |
					<DEVNAME>_SW_OUT_WR0_MSK,
					no_os_field_prep(<DEVNAME>_SW_OUT_SET_WR0_MSK, 1) |
					no_os_field_prep(<DEVNAME>_SW_OUT_WR0_MSK, lpf_band));
	if (ret)
		return ret;

	return <devname>_spi_update_bits(dev, <DEVNAME>_REG_WR0_FILTER,
					 <DEVNAME>_LPF_WR0_MSK,
					 no_os_field_prep(<DEVNAME>_LPF_WR0_MSK, lpf_step));
}

/**
 * @brief Read the current low-pass filter cutoff frequency.
 * @param dev  - Device descriptor.
 * @param freq - Pointer to store LPF frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read_lpf_freq(struct <devname>_dev *dev,
			    unsigned long long *freq)
{
	unsigned int lpf_band, lpf_state;
	uint8_t data;
	int ret;

	ret = <devname>_spi_read(dev, <DEVNAME>_REG_WR0_SW, &data);
	if (ret)
		return ret;

	lpf_band = no_os_field_get(<DEVNAME>_SW_OUT_WR0_MSK, data);
	if (!lpf_band || lpf_band > 4) {
		*freq = 0;
		return 0;
	}

	ret = <devname>_spi_read(dev, <DEVNAME>_REG_WR0_FILTER, &data);
	if (ret)
		return ret;

	lpf_state = no_os_field_get(<DEVNAME>_LPF_WR0_MSK, data);

	*freq = (freq_range_lpf[lpf_band - 1][1] -
		 freq_range_lpf[lpf_band - 1][0]) / 15;
	*freq = freq_range_lpf[lpf_band - 1][0] + (*freq * lpf_state);

	return 0;
}

/**
 * @brief Select RF input band (configures both HPF and LPF).
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 *
 * Uses the rf_in frequency stored in the device descriptor to
 * automatically configure both the HPF and LPF cutoff frequencies.
 */
int <devname>_rfin_select(struct <devname>_dev *dev)
{
	int ret;

	ret = <devname>_hpf_select(dev, dev->rf_in);
	if (ret)
		return ret;

	return <devname>_lpf_select(dev, dev->rf_in);
}

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the <DEVNAME> device.
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 *
 * Performs a soft reset, enables the SDO output, verifies the chip ID,
 * and enables single-instruction mode. In AUTO mode the HPF and LPF are
 * configured from the RF input frequency.
 */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param)
{
	uint8_t chip_id;
	struct <devname>_dev *dev;
	int ret;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->rf_in = init_param->rf_in;
	dev->mode = init_param->mode;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	/* Soft reset */
	ret = <devname>_spi_write(dev, <DEVNAME>_REG_SPI_CONFIG_A,
				  no_os_field_prep(<DEVNAME>_SOFTRESET_N_MSK, 1) |
				  no_os_field_prep(<DEVNAME>_SOFTRESET_MSK, 1));
	if (ret)
		goto error_spi;

	/* Enable SDO */
	ret = <devname>_spi_write(dev, <DEVNAME>_REG_SPI_CONFIG_A,
				  no_os_field_prep(<DEVNAME>_SDOACTIVE_N_MSK, 1) |
				  no_os_field_prep(<DEVNAME>_SDOACTIVE_MSK, 1));
	if (ret)
		goto error_spi;

	/* Verify chip ID */
	ret = <devname>_spi_read(dev, <DEVNAME>_REG_CHIPTYPE, &chip_id);
	if (ret)
		goto error_spi;

	if (chip_id != <DEVNAME>_CHIP_ID) {
		ret = -EINVAL;
		goto error_spi;
	}

	/* Enable single instruction mode */
	ret = <devname>_spi_update_bits(dev, <DEVNAME>_REG_SPI_CONFIG_B,
					<DEVNAME>_SINGLE_INSTRUCTION_MSK,
					no_os_field_prep(<DEVNAME>_SINGLE_INSTRUCTION_MSK, 1));
	if (ret)
		goto error_spi;

	/* In AUTO mode, configure filters from RF input frequency */
	if (dev->mode == <DEVNAME>_AUTO) {
		ret = <devname>_rfin_select(dev);
		if (ret)
			goto error_spi;
	}

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
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
#ifndef IIO_<DEVNAME>_H_
#define IIO_<DEVNAME>_H_

#include "iio.h"
#include "<devname>.h"

/**
 * @struct <devname>_iio_desc
 * @brief  IIO-specific device descriptor.
 *
 * Note: IIO support is not typical for filter devices since there is no
 * standard IIO channel type for tunable filters. If present, the driver
 * uses IIO_ALTVOLTAGE channels with custom attributes for frequency
 * settings.
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

#endif /* IIO_<DEVNAME>_H_ */
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

/*
 * Note: IIO support is not typical for filter devices. There is no
 * standard IIO channel type for tunable bandpass filters. When IIO is
 * needed, use IIO_ALTVOLTAGE channels with custom attributes to expose
 * the HPF and LPF frequency settings.
 */

/* ---------- Attribute callbacks ------------------------------------ */

/**
 * @brief Read HPF cutoff frequency attribute.
 */
static int <devname>_iio_read_hpf_freq(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	unsigned long long freq;
	int32_t val;
	int ret;

	ret = <devname>_read_hpf_freq(desc-><devname>_dev, &freq);
	if (ret)
		return ret;

	val = (int32_t)(freq / 1000000ULL); /* Report in MHz */

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write HPF cutoff frequency attribute.
 */
static int <devname>_iio_write_hpf_freq(void *dev, char *buf, uint32_t len,
					 const struct iio_ch_info *channel,
					 intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	unsigned long long freq;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	freq = (unsigned long long)val * 1000000ULL; /* MHz to Hz */

	return <devname>_hpf_select(desc-><devname>_dev, freq);
}

/**
 * @brief Read LPF cutoff frequency attribute.
 */
static int <devname>_iio_read_lpf_freq(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	unsigned long long freq;
	int32_t val;
	int ret;

	ret = <devname>_read_lpf_freq(desc-><devname>_dev, &freq);
	if (ret)
		return ret;

	val = (int32_t)(freq / 1000000ULL); /* Report in MHz */

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write LPF cutoff frequency attribute.
 */
static int <devname>_iio_write_lpf_freq(void *dev, char *buf, uint32_t len,
					 const struct iio_ch_info *channel,
					 intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	unsigned long long freq;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	freq = (unsigned long long)val * 1000000ULL; /* MHz to Hz */

	return <devname>_lpf_select(desc-><devname>_dev, freq);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_ch_attrs[] = {
	{ .name = "hpf_frequency", .show = <devname>_iio_read_hpf_freq,
	  .store = <devname>_iio_write_hpf_freq },
	{ .name = "lpf_frequency", .show = <devname>_iio_read_lpf_freq,
	  .store = <devname>_iio_write_lpf_freq },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * Filter channels: IIO_ALTVOLTAGE, indexed.
 * Used to expose HPF/LPF frequency control attributes.
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "altvoltage0",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 0,
		.attributes = <devname>_iio_ch_attrs,
		.ch_out = true,
		.indexed = true,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels) - 1,
	.debug_reg_read = (int32_t (*)())<devname>_spi_read,
	.debug_reg_write = (int32_t (*)())<devname>_spi_write,
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

INCS += $(DRIVERS)/filter/<devname>/<devname>.h
SRCS += $(DRIVERS)/filter/<devname>/<devname>.c
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
INCS += $(DRIVERS)/filter/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/filter/<devname>/iio_<devname>.c

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
extern struct no_os_spi_init_param <devname>_spi_ip;
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

struct no_os_spi_init_param <devname>_spi_ip = {
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
	.spi_init = &<devname>_spi_ip,
	.rf_in = 10000000000ULL, /* 10 GHz */
	.mode = <DEVNAME>_AUTO,
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
 *               execute the filter configuration and read-back, then return.
 *******************************************************************************/
int basic_example_main()
{
	struct <devname>_dev *dev;
	unsigned long long hpf_freq, lpf_freq;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	/* Configure HPF cutoff to 5 GHz */
	ret = <devname>_hpf_select(dev, 5000000000ULL);
	if (ret)
		goto free_dev;

	/* Configure LPF cutoff to 8 GHz */
	ret = <devname>_lpf_select(dev, 8000000000ULL);
	if (ret)
		goto free_dev;

	/* Read back HPF frequency */
	ret = <devname>_read_hpf_freq(dev, &hpf_freq);
	if (ret)
		goto free_dev;

	pr_info("HPF frequency: %llu Hz\r\n", hpf_freq);

	/* Read back LPF frequency */
	ret = <devname>_read_lpf_freq(dev, &lpf_freq);
	if (ret)
		goto free_dev;

	pr_info("LPF frequency: %llu Hz\r\n", lpf_freq);

free_dev:
	<devname>_remove(dev);
error:
	if (ret)
		pr_info("Error!\r\n");
	return ret;
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

The <DEVNAME> is a digitally tunable RF bandpass filter with SPI control.
It features independently configurable high-pass and low-pass filter
sections covering the <low> GHz to <high> GHz frequency range, with
<N>-bit state control per filter for fine cutoff frequency adjustment.
<Add 2-3 more sentences from the datasheet covering key specs such as
insertion loss, rejection, package, and target applications.>

Applications
------------

* Test and measurement equipment
* Military radar, electronic warfare, and electronic countermeasures
* Satellite communications and space
* Industrial and medical equipment

<DEVNAME> Device Configuration
------------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

The first API to be called is ``<devname>_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. During initialization,
the driver performs a soft reset, enables the SDO output, verifies the chip ID,
and configures single instruction mode. If the filter mode is set to
``<DEVNAME>_AUTO``, the driver will automatically configure the HPF and LPF
based on the provided RF input frequency.

High-Pass Filter Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The high-pass filter cutoff frequency is configured using
``<devname>_hpf_select``, which accepts a frequency value in Hz and
automatically selects the closest HPF band and state. Each band has 16
states (4-bit control) for fine frequency adjustment. The current HPF
frequency can be read back using ``<devname>_read_hpf_freq``.

Low-Pass Filter Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The low-pass filter cutoff frequency is configured using
``<devname>_lpf_select``, which accepts a frequency value in Hz and
automatically selects the closest LPF band and state. Each band has 16
states (4-bit control) for fine frequency adjustment. The current LPF
frequency can be read back using ``<devname>_read_lpf_freq``.

RF Input Selection
~~~~~~~~~~~~~~~~~~

The ``<devname>_rfin_select`` function configures both the HPF and LPF
cutoff frequencies from the RF input frequency stored in the device
descriptor. This is called automatically during initialization when the
filter mode is set to ``<DEVNAME>_AUTO``, or it can be called manually
at any time to reconfigure the filter passband.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct no_os_spi_init_param spi_init = {
           .device_id = SPI_DEVICE_ID,
           .max_speed_hz = SPI_MAX_SPEED,
           .chip_select = SPI_CS,
           .mode = NO_OS_SPI_MODE_0,
           .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
           .platform_ops = SPI_OPS,
           .extra = SPI_EXTRA,
       };

       struct <devname>_init_param ip = {
           .spi_init = &spi_init,
           .rf_in = 10000000000ULL, /* 10 GHz */
           .mode = <DEVNAME>_AUTO,
       };

       struct <devname>_dev *dev;
       unsigned long long hpf_freq, lpf_freq;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       /* Manually configure HPF to 5 GHz */
       ret = <devname>_hpf_select(dev, 5000000000ULL);
       if (ret)
           goto cleanup;

       /* Manually configure LPF to 8 GHz */
       ret = <devname>_lpf_select(dev, 8000000000ULL);
       if (ret)
           goto cleanup;

       /* Read back filter settings */
       ret = <devname>_read_hpf_freq(dev, &hpf_freq);
       if (ret)
           goto cleanup;

       ret = <devname>_read_lpf_freq(dev, &lpf_freq);
       if (ret)
           goto cleanup;

       /* hpf_freq and lpf_freq contain the configured values */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

IIO support is not typical for filter devices since there is no standard
IIO channel type for tunable bandpass filters. If present, the <DEVNAME>
IIO driver uses ``IIO_ALTVOLTAGE`` channels with custom attributes for
HPF and LPF frequency settings.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/filter/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/filter/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int`, negative = error.
5. **Init signature** -- `int <devname>_init(struct <devname>_dev **device,
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

Add support for the <DEVNAME> digitally tunable RF bandpass filter
with SPI interface, featuring independently configurable high-pass
and low-pass filter sections.

Signed-off-by: Your Name <your.name@analog.com>
```
