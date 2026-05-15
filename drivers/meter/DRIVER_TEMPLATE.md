# Meter Driver Template

Reference driver: `drivers/meter/ade7753/`

This template covers every file needed to add a new energy meter driver to
no-OS. Replace `<devname>` with the part number (e.g., `ade7753`) and
`<DEVNAME>` with its uppercase form (e.g., `ADE7753`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ade7753.h`, `ade7753.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ade7753.h`, `iio_ade7753.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADE7753_H__` |
| Init param struct | `<devname>_init_param` | `ade7753_init_param` |
| Device struct | `<devname>_dev` | `ade7753_dev` |
| IIO descriptor | `<devname>_iio_desc` | `ade7753_iio_desc` |
| Functions | `<devname>_<verb>()` | `ade7753_init()` |
| Static helpers | `_<devname>_<verb>()` | `_ade7753_spi_read()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADE7753` |
| Register addresses | `<DEVNAME>_REG_<NAME>` | `ADE7753_REG_IRMS` |
| Register masks | `<DEVNAME>_<FIELD>_MSK` | `ADE7753_SWRST_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADE7753_PGA_GAIN_1` |

---

## 2. File Checklist

```
drivers/meter/<devname>/
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

doc/sphinx/source/drivers/meter/<devname>.rst   # Sphinx entry
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
#include "no_os_irq.h"
#include "no_os_util.h"

/* ---------------- SPI Command Definitions ------------------------------ */

/** SPI read bit — set in the address byte for read transactions. */
#define <DEVNAME>_SPI_READ				NO_OS_BIT(7)

/* ---------------- Register Map ----------------------------------------- */

#define <DEVNAME>_REG_CONFIG				0x00
#define <DEVNAME>_REG_STATUS				0x01
#define <DEVNAME>_REG_IRQEN				0x02
#define <DEVNAME>_REG_RSTSTATUS				0x03
#define <DEVNAME>_REG_GAIN				0x04
#define <DEVNAME>_REG_PHCAL				0x05
#define <DEVNAME>_REG_AENERGY				0x06
#define <DEVNAME>_REG_RAENERGY				0x07
#define <DEVNAME>_REG_VAENERGY				0x08
#define <DEVNAME>_REG_IRMS				0x09
#define <DEVNAME>_REG_VRMS				0x0A
#define <DEVNAME>_REG_PERIOD				0x0B
#define <DEVNAME>_REG_LINECYC				0x0C
#define <DEVNAME>_REG_WAVEFORM				0x0D
#define <DEVNAME>_REG_VERSION				0x3F

/* ---------------- Config / Mode Register Bit Masks --------------------- */

/** Configuration field masks — use NO_OS_BIT() for single bits,
 *  NO_OS_GENMASK(hi, lo) for multi-bit fields. */
#define <DEVNAME>_SWRST_MSK				NO_OS_BIT(6)
#define <DEVNAME>_CYCMODE_MSK				NO_OS_BIT(7)
#define <DEVNAME>_DISHPF_MSK				NO_OS_BIT(0)
#define <DEVNAME>_DISLPF2_MSK				NO_OS_BIT(1)
#define <DEVNAME>_DISCF_MSK				NO_OS_BIT(2)
#define <DEVNAME>_WAVSEL_MSK				NO_OS_GENMASK(14, 13)
#define <DEVNAME>_DTRT_MSK				NO_OS_GENMASK(12, 11)

/* ---------------- Status / IRQ Bit Masks ------------------------------- */

#define <DEVNAME>_ZX_MSK				NO_OS_BIT(4)
#define <DEVNAME>_CYCEND_MSK				NO_OS_BIT(2)
#define <DEVNAME>_SAG_MSK				NO_OS_BIT(1)
#define <DEVNAME>_RESET_MSK				NO_OS_BIT(6)

/* ---------------- Gain Register Bit Masks ------------------------------ */

#define <DEVNAME>_PGA1_GAIN_MSK				NO_OS_GENMASK(2, 0)
#define <DEVNAME>_PGA2_GAIN_MSK				NO_OS_GENMASK(7, 5)
#define <DEVNAME>_FULL_SCALE_MSK			NO_OS_GENMASK(4, 3)

/* ---------------- Product / Revision ID -------------------------------- */

#define <DEVNAME>_VERSION				0xXX

/* Reset delay in ms */
#define <DEVNAME>_RESET_DEL				1

/* ENABLE and DISABLE */
#define ENABLE						1u
#define DISABLE						0u

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_pga_gain
 *  @brief PGA gain selection. */
enum <devname>_pga_gain {
	<DEVNAME>_PGA_GAIN_1,
	<DEVNAME>_PGA_GAIN_2,
	<DEVNAME>_PGA_GAIN_4,
	<DEVNAME>_PGA_GAIN_8,
	<DEVNAME>_PGA_GAIN_16,
};

/** @enum <devname>_waveform_src
 *  @brief Waveform register data source selection. */
enum <devname>_waveform_src {
	<DEVNAME>_WAV_ACTIVE_POWER,
	<DEVNAME>_WAV_CH1 = 2,
	<DEVNAME>_WAV_CH2,
};

/* Add more enums as needed: update rate, full-scale select, etc. */

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_energy_values
 * @brief  Accumulated energy register values.
 */
struct <devname>_energy_values {
	/** Active energy register value. */
	int32_t active_energy_reg_val;
	/** Apparent energy register value. */
	int32_t apparent_energy_reg_val;
};

/**
 * @struct <devname>_rms_values
 * @brief  RMS measurement register values.
 */
struct <devname>_rms_values {
	/** Current RMS register value. */
	uint32_t current_rms_reg_val;
	/** Voltage RMS register value. */
	uint32_t voltage_rms_reg_val;
};

/**
 * @struct <devname>_period_value
 * @brief  Period measurement register value.
 */
struct <devname>_period_value {
	/** Period register value (line period). */
	uint32_t period_reg_val;
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
	/** GPIO RESET descriptor (NULL if using SW reset). */
	struct no_os_gpio_init_param *gpio_reset;
	/** IRQ controller descriptor for interrupt handling. */
	struct no_os_irq_ctrl_desc *irq_ctrl;
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
	/** GPIO RESET descriptor. */
	struct no_os_gpio_desc *gpio_reset;
	/** IRQ controller descriptor. */
	struct no_os_irq_ctrl_desc *irq_ctrl;
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Read a register. */
int <devname>_read(struct <devname>_dev *dev, uint16_t reg_addr,
		   int32_t *reg_data);

/** @brief Write a register. */
int <devname>_write(struct <devname>_dev *dev, uint16_t reg_addr,
		    uint32_t reg_data);

/** @brief Update specific register bits. */
int <devname>_update_bits(struct <devname>_dev *dev, uint16_t reg_addr,
			  uint32_t mask, uint32_t reg_data);

/** @brief Perform a software reset. */
int <devname>_sw_reset(struct <devname>_dev *dev);

/** @brief Perform a hardware reset via GPIO. */
int <devname>_hw_reset(struct <devname>_dev *dev);

/** @brief Read the version/product ID register. */
int <devname>_version_product(struct <devname>_dev *dev,
			      uint32_t *data_read);

/** @brief Read accumulated energy values. */
int <devname>_energy_vals(struct <devname>_dev *dev,
			  struct <devname>_energy_values *data);

/** @brief Read RMS values (voltage and current). */
int <devname>_rms_vals(struct <devname>_dev *dev,
		       struct <devname>_rms_values *data);

/** @brief Read the line period value. */
int <devname>_period_val(struct <devname>_dev *dev,
			 struct <devname>_period_value *data);

/** @brief Get interrupt status for a given mask. */
int <devname>_get_int_status(struct <devname>_dev *dev, uint32_t msk,
			     uint8_t *status);

/** @brief Clear IRQ status flags by reading the reset-status register. */
int <devname>_clear_irq_status(struct <devname>_dev *dev,
			       int32_t *reg_data);

/** @brief Enable or disable an interrupt. */
int <devname>_enable_irq(struct <devname>_dev *dev, uint32_t msk,
			 uint8_t en);

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
#include <string.h>
#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* Device name lookup table (indexed by enum <devname>_device_id). */
static const char * const <devname>_device_name[] = {
	[ID_<DEVNAME>] = "<DEVNAME>",
};

/* ---------- Register Access ---------------------------------------- */

/**
 * @brief Read a device register over SPI.
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param reg_data - Pointer to store the value read.
 * @return 0 on success, negative error code otherwise.
 *
 * Note: Meter ICs typically have variable-width registers (8, 12, 16,
 * 24 bits). Determine the register width from the address and adjust
 * the SPI transaction length accordingly. See ade7753_read() for a
 * concrete example.
 */
int <devname>_read(struct <devname>_dev *dev, uint16_t reg_addr,
		   int32_t *reg_data)
{
	int ret;
	uint8_t buf[4] = { 0 };

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	/* Address byte: bits [5:0] = register address, bit 7 clear = read */
	buf[0] = (uint8_t)no_os_field_prep(NO_OS_GENMASK(5, 0), reg_addr);

	/*
	 * TODO: Determine register width from address and read the
	 * appropriate number of bytes. Example for a 16-bit register:
	 */
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);
	if (ret)
		return ret;

	*reg_data = no_os_sign_extend32(
		no_os_get_unaligned_be16(&buf[1]), 15);

	return 0;
}

/**
 * @brief Write a device register over SPI.
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param reg_data - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_write(struct <devname>_dev *dev, uint16_t reg_addr,
		    uint32_t reg_data)
{
	uint8_t buf[4] = { 0 };

	if (!dev)
		return -ENODEV;

	/* Address byte: bits [5:0] = register address, bit 7 set = write */
	buf[0] = (uint8_t)no_os_field_prep(NO_OS_GENMASK(5, 0), reg_addr);
	buf[0] |= <DEVNAME>_SPI_READ;

	/*
	 * TODO: Determine register width from address. Example for 16-bit:
	 */
	no_os_put_unaligned_be16(reg_data, &buf[1]);

	return no_os_spi_write_and_read(dev->spi_desc, buf, 3);
}

/**
 * @brief Update specific register bits (read-modify-write).
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param mask     - Bit mask for the field to update.
 * @param reg_data - New data (pre-masked or field-prepped).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_update_bits(struct <devname>_dev *dev, uint16_t reg_addr,
			  uint32_t mask, uint32_t reg_data)
{
	int ret;
	int32_t data;

	ret = <devname>_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return <devname>_write(dev, reg_addr, (uint32_t)data);
}

/* ---------- Reset -------------------------------------------------- */

/**
 * @brief Perform a software reset via the mode register.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_sw_reset(struct <devname>_dev *dev)
{
	int ret;

	ret = <devname>_update_bits(dev, <DEVNAME>_REG_CONFIG,
				    <DEVNAME>_SWRST_MSK,
				    no_os_field_prep(<DEVNAME>_SWRST_MSK,
						    ENABLE));
	if (ret)
		return ret;

	no_os_mdelay(<DEVNAME>_RESET_DEL);

	return 0;
}

/**
 * @brief Perform a hardware reset via the GPIO reset pin.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_hw_reset(struct <devname>_dev *dev)
{
	int ret;

	if (!dev || !dev->gpio_reset)
		return -ENODEV;

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(1);

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	return 0;
}

/* ---------- Identification ----------------------------------------- */

/**
 * @brief Read the version/product ID register.
 * @param dev       - Device descriptor.
 * @param data_read - Pointer to store the version value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_version_product(struct <devname>_dev *dev,
			      uint32_t *data_read)
{
	int ret;
	int32_t version;

	if (!data_read)
		return -EINVAL;

	ret = <devname>_read(dev, <DEVNAME>_REG_VERSION, &version);
	if (ret)
		return ret;

	*data_read = (uint32_t)version;

	return 0;
}

/* ---------- Measurement Readback ----------------------------------- */

/**
 * @brief Read accumulated energy values (active + apparent).
 * @param dev  - Device descriptor.
 * @param data - Pointer to energy values structure.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_energy_vals(struct <devname>_dev *dev,
			  struct <devname>_energy_values *data)
{
	int ret;
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	ret = <devname>_read(dev, <DEVNAME>_REG_AENERGY, &temp_val);
	if (ret)
		return ret;
	data->active_energy_reg_val = temp_val;

	ret = <devname>_read(dev, <DEVNAME>_REG_VAENERGY, &temp_val);
	if (ret)
		return ret;
	data->apparent_energy_reg_val = temp_val;

	return 0;
}

/**
 * @brief Read RMS values (current and voltage).
 * @param dev  - Device descriptor.
 * @param data - Pointer to RMS values structure.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_rms_vals(struct <devname>_dev *dev,
		       struct <devname>_rms_values *data)
{
	int ret;
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	ret = <devname>_read(dev, <DEVNAME>_REG_VRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val = (uint32_t)temp_val;

	ret = <devname>_read(dev, <DEVNAME>_REG_IRMS, &temp_val);
	if (ret)
		return ret;
	data->current_rms_reg_val = (uint32_t)temp_val;

	return 0;
}

/**
 * @brief Read the line period register value.
 * @param dev  - Device descriptor.
 * @param data - Pointer to period value structure.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_period_val(struct <devname>_dev *dev,
			 struct <devname>_period_value *data)
{
	int ret;
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	ret = <devname>_read(dev, <DEVNAME>_REG_PERIOD, &temp_val);
	if (ret)
		return ret;
	data->period_reg_val = (uint32_t)temp_val;

	return 0;
}

/* ---------- Interrupt Management ----------------------------------- */

/**
 * @brief Get interrupt status for a given mask bit.
 * @param dev    - Device descriptor.
 * @param msk    - Interrupt mask (single bit).
 * @param status - Pointer to store status (1 = set, 0 = clear).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_int_status(struct <devname>_dev *dev, uint32_t msk,
			     uint8_t *status)
{
	int ret;
	int32_t reg_val;

	if (!status)
		return -EINVAL;

	ret = <devname>_read(dev, <DEVNAME>_REG_STATUS, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk),
				 (uint32_t *)&reg_val);

	return 0;
}

/**
 * @brief Clear IRQ status flags by reading the reset-status register.
 * @param dev      - Device descriptor.
 * @param reg_data - Pointer to store status register value before clear.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_clear_irq_status(struct <devname>_dev *dev,
			       int32_t *reg_data)
{
	if (!reg_data)
		return -EINVAL;

	return <devname>_read(dev, <DEVNAME>_REG_RSTSTATUS, reg_data);
}

/**
 * @brief Enable or disable an interrupt via the IRQ enable register.
 * @param dev - Device descriptor.
 * @param msk - Interrupt mask.
 * @param en  - ENABLE (1) or DISABLE (0).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_enable_irq(struct <devname>_dev *dev, uint32_t msk,
			 uint8_t en)
{
	return <devname>_update_bits(dev, <DEVNAME>_REG_IRQEN, msk,
				     no_os_field_prep(msk, en));
}

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the <DEVNAME> device.
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param init_param)
{
	struct <devname>_dev *dev;
	uint32_t reg_val;
	uint16_t timeout = 1000;
	uint8_t status;
	int ret;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->id = init_param.id;
	dev->name = <devname>_device_name[dev->id];

	/* SPI initialisation */
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto free_dev;

	/* Store IRQ controller reference */
	dev->irq_ctrl = init_param.irq_ctrl;

	/* Store/init reset GPIO */
	dev->gpio_reset = init_param.gpio_reset;

	/* Reset the device */
	if (dev->gpio_reset) {
		ret = <devname>_hw_reset(dev);
		if (ret)
			goto free_spi;
	} else {
		ret = <devname>_sw_reset(dev);
		if (ret)
			goto free_spi;
	}

	/* Wait for reset acknowledgement */
	do {
		ret = <devname>_get_int_status(dev, <DEVNAME>_RESET_MSK,
					       &status);
		if (ret)
			goto free_spi;
		timeout--;
	} while ((!status) && (timeout > 0));

	if (timeout == 0) {
		ret = -ETIMEDOUT;
		goto free_spi;
	}

	/* Verify product ID */
	ret = <devname>_version_product(dev, &reg_val);
	if (ret)
		goto free_spi;

	if (reg_val != <DEVNAME>_VERSION) {
		pr_err("Unexpected product ID: 0x%02X\n", reg_val);
		ret = -ENODEV;
		goto free_spi;
	}

	*device = dev;

	return 0;

free_spi:
	no_os_spi_remove(dev->spi_desc);
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
		return -ENODEV;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_reset);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_remove(dev->irq_ctrl);
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

Energy metering IIO drivers expose multiple channel types:
- `IIO_VOLTAGE` — voltage RMS measurement
- `IIO_CURRENT` — current RMS measurement
- `IIO_POWER` — active/apparent power and energy accumulation

Each channel type can have its own attribute set.

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

/* ---------- Attribute callbacks: Voltage channel ------------------- */

/**
 * @brief Read raw voltage RMS value.
 */
static int <devname>_iio_read_vrms_raw(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	struct <devname>_rms_values rms;
	int32_t val;
	int ret;

	ret = <devname>_rms_vals(desc-><devname>_dev, &rms);
	if (ret)
		return ret;

	val = (int32_t)rms.voltage_rms_reg_val;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read voltage scale (mV per LSB).
 *
 * scale = (ADC full-scale input in mV) / (RMS full-scale code)
 * Express as IIO_VAL_INT_PLUS_MICRO.
 */
static int <devname>_iio_read_vrms_scale(void *dev, char *buf, uint32_t len,
					 const struct iio_ch_info *channel,
					 intptr_t priv)
{
	int32_t val[2];

	/*
	 * Example: 353.5 mV full-scale, 1729993 full-scale code
	 * scale = 353.5 / 1729993 = 0.000204 mV per LSB
	 * Expressed as: 0 + 204 micro
	 */
	val[0] = 0;
	val[1] = 204;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/* ---------- Attribute callbacks: Current channel ------------------- */

/**
 * @brief Read raw current RMS value.
 */
static int <devname>_iio_read_irms_raw(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	struct <devname>_rms_values rms;
	int32_t val;
	int ret;

	ret = <devname>_rms_vals(desc-><devname>_dev, &rms);
	if (ret)
		return ret;

	val = (int32_t)rms.current_rms_reg_val;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read current scale (mA per LSB).
 */
static int <devname>_iio_read_irms_scale(void *dev, char *buf, uint32_t len,
					 const struct iio_ch_info *channel,
					 intptr_t priv)
{
	int32_t val[2];

	/*
	 * Example: 353.5 mV full-scale, 1374901 full-scale code
	 * scale = 353.5 / 1374901 = 0.000257 mA per LSB
	 * Expressed as: 0 + 257 micro
	 */
	val[0] = 0;
	val[1] = 257;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/* ---------- Attribute callbacks: Power/Energy channel --------------- */

/**
 * @brief Read raw active energy accumulation value.
 */
static int <devname>_iio_read_energy_raw(void *dev, char *buf, uint32_t len,
					 const struct iio_ch_info *channel,
					 intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	struct <devname>_energy_values energy;
	int32_t val;
	int ret;

	ret = <devname>_energy_vals(desc-><devname>_dev, &energy);
	if (ret)
		return ret;

	val = energy.active_energy_reg_val;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read raw apparent energy accumulation value.
 */
static int <devname>_iio_read_apparent_raw(void *dev, char *buf, uint32_t len,
					   const struct iio_ch_info *channel,
					   intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	struct <devname>_energy_values energy;
	int32_t val;
	int ret;

	ret = <devname>_energy_vals(desc-><devname>_dev, &energy);
	if (ret)
		return ret;

	val = energy.apparent_energy_reg_val;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the line period (used to derive frequency).
 */
static int <devname>_iio_read_period_raw(void *dev, char *buf, uint32_t len,
					 const struct iio_ch_info *channel,
					 intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	struct <devname>_period_value period;
	int32_t val;
	int ret;

	ret = <devname>_period_val(desc-><devname>_dev, &period);
	if (ret)
		return ret;

	val = (int32_t)period.period_reg_val;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/* ---------- Attribute tables --------------------------------------- */

/** Voltage channel attributes (RMS voltage). */
static struct iio_attribute <devname>_iio_voltage_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_vrms_raw },
	{ .name = "scale", .show = <devname>_iio_read_vrms_scale },
	END_ATTRIBUTES_ARRAY
};

/** Current channel attributes (RMS current). */
static struct iio_attribute <devname>_iio_current_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_irms_raw },
	{ .name = "scale", .show = <devname>_iio_read_irms_scale },
	END_ATTRIBUTES_ARRAY
};

/** Power channel attributes (active energy, apparent energy, period). */
static struct iio_attribute <devname>_iio_power_attrs[] = {
	{ .name = "raw",          .show = <devname>_iio_read_energy_raw },
	{ .name = "raw_apparent", .show = <devname>_iio_read_apparent_raw },
	{ .name = "period_raw",   .show = <devname>_iio_read_period_raw },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * Meter channels: multiple types to cover the measurement domains.
 *
 * Channel 0: IIO_VOLTAGE — Voltage RMS (input)
 * Channel 1: IIO_CURRENT — Current RMS (input)
 * Channel 2: IIO_POWER   — Active/Apparent energy (input)
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "voltage0",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.scan_index = 0,
		.attributes = <devname>_iio_voltage_attrs,
		.ch_out = false,
		.indexed = true,
	},
	{
		.name = "current0",
		.ch_type = IIO_CURRENT,
		.channel = 1,
		.scan_index = 1,
		.attributes = <devname>_iio_current_attrs,
		.ch_out = false,
		.indexed = true,
	},
	{
		.name = "power0",
		.ch_type = IIO_POWER,
		.channel = 2,
		.scan_index = 2,
		.attributes = <devname>_iio_power_attrs,
		.ch_out = false,
		.indexed = true,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels) - 1,
	.debug_reg_read = (int32_t (*)())<devname>_read,
	.debug_reg_write = (int32_t (*)())<devname>_write,
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
			     *init_param-><devname>_init_param);
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
	$(INCLUDE)/no_os_units.h     \
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

INCS += $(DRIVERS)/meter/<devname>/<devname>.h
SRCS += $(DRIVERS)/meter/<devname>/<devname>.c
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
INCS += $(DRIVERS)/meter/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/meter/<devname>/iio_<devname>.c

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
extern struct no_os_gpio_init_param <devname>_gpio_reset_ip;
extern struct <devname>_init_param <devname>_ip;

/* Calibration constants — adjust per hardware design */
#define RSMALL				1000
#define RHIGH				1000000
#define V_GAIN				((RSMALL + RHIGH) / RSMALL)
#define I_GAIN				1
#define VOLTAGE_RMS_FS_CODE		1729993
#define CURRENT_RMS_FS_CODE		1374901
#define ADC_FS_RMS_IN			353.5
#define FREQ_CC				448280

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

struct no_os_gpio_init_param <devname>_gpio_reset_ip = {
	.number = GPIO_RESET_PIN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct <devname>_init_param <devname>_ip = {
	.id = ID_<DEVNAME>,
	.spi_init = &<devname>_spi_ip,
	.gpio_reset = NULL,    /* Set to &<devname>_gpio_reset_ip for HW reset */
	.irq_ctrl = NULL,
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
	struct <devname>_energy_values energy;
	struct <devname>_rms_values rms;
	struct <devname>_period_value period;
	float current_rms, voltage_rms, frequency;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, <devname>_ip);
	if (ret)
		goto error;

	while (1) {
		/* Read RMS values */
		ret = <devname>_rms_vals(dev, &rms);
		if (ret)
			goto free_dev;

		current_rms = ((float)rms.current_rms_reg_val /
			       CURRENT_RMS_FS_CODE) * ADC_FS_RMS_IN * I_GAIN;
		voltage_rms = ((float)rms.voltage_rms_reg_val /
			       VOLTAGE_RMS_FS_CODE) * ADC_FS_RMS_IN * V_GAIN;

		pr_info("I rms: %.2f mA\r\n", current_rms);
		pr_info("V rms: %.2f mV\r\n", voltage_rms);

		/* Read energy values */
		ret = <devname>_energy_vals(dev, &energy);
		if (ret)
			goto free_dev;

		pr_info("Active energy: %d\r\n",
			energy.active_energy_reg_val);
		pr_info("Apparent energy: %d\r\n",
			energy.apparent_energy_reg_val);

		/* Read line period and compute frequency */
		ret = <devname>_period_val(dev, &period);
		if (ret)
			goto free_dev;

		frequency = (float)FREQ_CC /
			    (float)(period.period_reg_val + 1);
		pr_info("Frequency: %.2f Hz\r\n", frequency);

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
#include "maxim_irq.h"

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

#define GPIO_RESET_PIN	0
#define GPIO_OPS	&max_gpio_ops
#define GPIO_EXTRA	NULL

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

The <DEVNAME> is a single-phase energy metering IC with a SPI-compatible
serial interface. It provides active and apparent energy accumulation,
voltage/current RMS measurement, and line period/frequency detection.
It features a digital integrator for use with di/dt current sensors and
programmable gain amplifiers (PGA) on both input channels. <Add 2-3 more
sentences from the datasheet covering key specs and features.>

Applications
------------

* Single-phase energy metering
* Power quality monitoring
* Smart plugs and smart outlets
* EV charging stations

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface, performs a device reset
(hardware via GPIO or software via the mode register), waits for the
reset acknowledgement, and verifies the product ID. The function uses
goto-based cleanup to release resources on failure.

Energy Measurement
~~~~~~~~~~~~~~~~~~

The ``<devname>_energy_vals`` function reads the accumulated active
energy and apparent energy registers. These registers accumulate over
time and can be reset by reading the reset-on-read variants.

RMS Measurement
~~~~~~~~~~~~~~~

The ``<devname>_rms_vals`` function reads the voltage RMS and current
RMS registers. These provide the RMS values of the voltage and current
input channels, updated every line cycle.

Period / Frequency Measurement
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_period_val`` function reads the line period register.
The line frequency can be calculated as: ``freq = FREQ_CC / (period + 1)``,
where ``FREQ_CC`` is a device-specific calibration constant.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .id = ID_<DEVNAME>,
           .spi_init = &spi_ip,
           .gpio_reset = NULL,
           .irq_ctrl = NULL,
       };

       struct <devname>_dev *dev;
       struct <devname>_rms_values rms;
       int ret;

       ret = <devname>_init(&dev, ip);
       if (ret)
           return ret;

       ret = <devname>_rms_vals(dev, &rms);
       if (ret)
           goto cleanup;

       /* rms.voltage_rms_reg_val / rms.current_rms_reg_val */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes three channel types:

- ``IIO_VOLTAGE`` channel with ``raw`` and ``scale`` attributes for
  voltage RMS measurement.
- ``IIO_CURRENT`` channel with ``raw`` and ``scale`` attributes for
  current RMS measurement.
- ``IIO_POWER`` channel with ``raw`` (active energy),
  ``raw_apparent`` (apparent energy), and ``period_raw`` (line period)
  attributes.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/meter/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/meter/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int`, negative = error.
5. **Init signature** -- `int <devname>_init(struct <devname>_dev **device,
   struct <devname>_init_param init_param)` (note: meter drivers typically
   pass init_param by value, matching the ade7753 pattern).
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
11. **Variable-width registers** -- meter ICs often have registers of
    different widths (6, 8, 12, 16, 24 bits). The read/write functions
    must determine the width from the address and adjust the SPI
    transaction accordingly.
12. **Multiple measurement domains** -- meter drivers should provide
    separate readback functions for energy accumulation, RMS values,
    and line period/frequency, each returning data in a dedicated
    structure.
13. **IIO channel types** -- use `IIO_VOLTAGE` for voltage RMS,
    `IIO_CURRENT` for current RMS, and `IIO_POWER` for energy/power
    measurements. There is no `IIO_ENERGY` type in the no-OS IIO
    framework.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> single-phase energy metering IC with
SPI interface. The driver provides energy accumulation, voltage/current
RMS measurement, and line period/frequency detection.

Signed-off-by: Your Name <your.name@analog.com>
```
