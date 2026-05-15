# Position Sensor Driver Template

Reference driver: `drivers/position/admt4000/`

This template covers every file needed to add a new position sensor driver to
no-OS. Replace `<devname>` with the part number (e.g., `admt4000`) and
`<DEVNAME>` with its uppercase form (e.g., `ADMT4000`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `admt4000.h`, `admt4000.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_admt4000.h`, `iio_admt4000.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADMT4000_H__` |
| Init param struct | `<devname>_init_param` | `admt4000_init_param` |
| Device struct | `<devname>_dev` | `admt4000_dev` |
| IIO descriptor | `<devname>_iio_dev` | `admt4000_iio_dev` |
| Functions | `<devname>_<verb>()` | `admt4000_init()` |
| Static helpers | `_<devname>_<verb>()` | `_admt4000_compute_crc()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADMT4000` |
| Register addresses | `<DEVNAME>_REG_<NAME>` | `ADMT4000_REG_RST` |
| Page-scoped regs | `<DEVNAME>_<PAGE>_REG_<NAME>` | `ADMT4000_00_REG_SINE` |
| Register masks | `<DEVNAME>_<FIELD>_MASK` | `ADMT4000_ANGLE_MASK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADMT4000_CONTINUOUS` |

---

## 2. File Checklist

```
drivers/position/<devname>/
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

doc/sphinx/source/drivers/position/<devname>.rst   # Sphinx entry
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/* ---------- Page-Agnostic Registers -------------------------------- */

#define <DEVNAME>_REG_RST		0x00
#define <DEVNAME>_AGP_REG_CNVPAGE	0x01
#define <DEVNAME>_AGP_REG_ABSANGLE	0x03
#define <DEVNAME>_AGP_REG_ANGLE		0x05
#define <DEVNAME>_AGP_REG_FAULT		0x06

/* ---------- Page 0 Registers (Measurement Data) ------------------- */

#define <DEVNAME>_00_REG_SINE		0x10
#define <DEVNAME>_00_REG_COSINE		0x11
#define <DEVNAME>_00_REG_RADIUS		0x18
#define <DEVNAME>_00_REG_TMP		0x20

/* ---------- Page 2 Registers (Configuration) ---------------------- */

#define <DEVNAME>_02_REG_GENERAL	0x10
#define <DEVNAME>_02_REG_ANGLECK	0x13

/* ---------- Field Masks ------------------------------------------- */
/* Use NO_OS_BIT() for single bits, NO_OS_GENMASK(hi, lo) for fields. */

#define <DEVNAME>_WR_EN			NO_OS_BIT(6)
#define <DEVNAME>_RW_MASK		NO_OS_GENMASK(5, 0)
#define <DEVNAME>_FAULT_MASK		NO_OS_BIT(7)

/* Page select */
#define <DEVNAME>_CNV_EDGE_MASK		NO_OS_GENMASK(15, 14)
#define <DEVNAME>_PAGE_MASK		NO_OS_GENMASK(4, 0)

/* Angle fields */
#define <DEVNAME>_ABS_ANGLE_MASK	NO_OS_GENMASK(15, 0)
#define <DEVNAME>_ANGLE_MASK		NO_OS_GENMASK(15, 4)
#define <DEVNAME>_TURN_CNT_MASK		NO_OS_GENMASK(15, 10)

/* Raw sine/cosine fields */
#define <DEVNAME>_RAW_SINE_MASK		NO_OS_GENMASK(15, 2)
#define <DEVNAME>_RAW_COSINE_MASK	NO_OS_GENMASK(15, 2)

/* Radius field */
#define <DEVNAME>_RADIUS_MASK		NO_OS_GENMASK(15, 1)

/* Temperature field */
#define <DEVNAME>_TEMP_MASK		NO_OS_GENMASK(15, 4)

/* Configuration fields */
#define <DEVNAME>_CONV_SYNC_MODE_MASK	NO_OS_GENMASK(14, 13)
#define <DEVNAME>_ANGL_FILT_MASK	NO_OS_BIT(12)
#define <DEVNAME>_CNV_MODE_MASK		NO_OS_BIT(0)

/* ---------- Constants --------------------------------------------- */

#define <DEVNAME>_MIN_PAGE		0x00
#define <DEVNAME>_MAX_PAGE		0x02

/* Angle to degree conversion */
#define <DEVNAME>_FULL_TURN_DEGREES	360
#define <DEVNAME>_ANGLE_RES		4096

/* Temperature conversion */
#define <DEVNAME>_TEMP_OFFSET		-1168
#define <DEVNAME>_TEMP_SCALE_MILLIS	15660

/* Timing */
#define <DEVNAME>_STARTUP_TIME_MS	10
#define <DEVNAME>_CONVERSION_DELAY_MS	20

/* ---------- Enums ------------------------------------------------- */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_conv_sync_mode
 *  @brief Conversion synchronization mode. */
enum <devname>_conv_sync_mode {
	<DEVNAME>_SEQ_CTRL = 0,
	<DEVNAME>_START_EDGE = 3,
};

/** @enum <devname>_conversion_mode
 *  @brief Continuous vs one-shot conversion. */
enum <devname>_conversion_mode {
	<DEVNAME>_CONTINUOUS = 0,
	<DEVNAME>_ONE_SHOT = 1,
};

/* ---------- Structures -------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** SPI communication parameters. */
	struct no_os_spi_init_param spi_init_param;
	/** Reset GPIO pin (active low, optional). */
	struct no_os_gpio_init_param gpio_reset_ip;
	/** CNV GPIO pin (conversion trigger, optional). */
	struct no_os_gpio_init_param gpio_cnv_ip;
	/* Add more GPIO init params as needed (ACALC, FAULT, etc.) */
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** SPI communication descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** Reset GPIO descriptor. */
	struct no_os_gpio_desc *gpio_reset_desc;
	/** CNV GPIO descriptor. */
	struct no_os_gpio_desc *gpio_cnv_desc;
	/** Current register page (cached to reduce SPI traffic). */
	uint8_t current_page;
	/** True when current_page matches the hardware state. */
	bool page_cache_valid;
	/** True for one-shot mode, false for continuous. */
	bool is_one_shot;
};

/* ---------- Function Prototypes ----------------------------------- */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *device);

/** @brief Read a register (with optional verification byte). */
int <devname>_reg_read(struct <devname>_dev *device, uint8_t reg_addr,
		       uint16_t *reg_data, uint8_t *verif);

/** @brief Write a register. */
int <devname>_reg_write(struct <devname>_dev *device, uint8_t reg_addr,
			uint16_t reg_data);

/** @brief Read-modify-write a register field. */
int <devname>_reg_update(struct <devname>_dev *device, uint8_t reg_addr,
			 uint16_t update_mask, uint16_t update_val);

/** @brief Toggle the CNV pin to trigger a conversion. */
int <devname>_toggle_cnv(struct <devname>_dev *device);

/** @brief Read raw angle data. */
int <devname>_raw_angle_read(struct <devname>_dev *device,
			     uint16_t *angle_data);

/** @brief Read turn count and angle in one operation. */
int <devname>_get_raw_turns_and_angle(struct <devname>_dev *device,
				      uint8_t *turns, uint16_t *angle);

/** @brief Get raw temperature value. */
int <devname>_get_temp(struct <devname>_dev *device, uint16_t *temp);

/** @brief Get sine component. */
int <devname>_get_sin(struct <devname>_dev *device, int16_t *val,
		      bool *is_new_data);

/** @brief Get cosine component. */
int <devname>_get_cos(struct <devname>_dev *device, int16_t *val,
		      bool *is_new_data);

/** @brief Get radius (signal amplitude). */
int <devname>_get_radius(struct <devname>_dev *device, uint16_t *radius,
			 bool *is_new_data);

/** @brief Get fault register. */
int <devname>_get_faults(struct <devname>_dev *device, uint16_t *faults);

/** @brief Clear all faults. */
int <devname>_clear_all_faults(struct <devname>_dev *device);

/** @brief Perform hardware reset. */
int <devname>_hard_reset(struct <devname>_dev *device);

#ifdef __cplusplus
}
#endif

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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "<devname>.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* ---------- Static helpers ----------------------------------------- */

/**
 * @brief Set the active register page.
 * @param device - Device descriptor.
 * @param page   - Target page number.
 * @return 0 on success, negative error code otherwise.
 *
 * Position sensors with page-based register maps require switching
 * pages before accessing page-specific registers.  The driver caches
 * the current page to avoid redundant SPI writes.
 */
static int <devname>_set_page(struct <devname>_dev *device, uint8_t page)
{
	uint16_t reg_val;
	int ret;

	if (page > <DEVNAME>_MAX_PAGE)
		return -EINVAL;

	/* Skip if the page is already selected and cache is valid */
	if (device->page_cache_valid && device->current_page == page)
		return 0;

	reg_val = no_os_field_prep(<DEVNAME>_PAGE_MASK, page);

	ret = <devname>_reg_write(device, <DEVNAME>_AGP_REG_CNVPAGE, reg_val);
	if (ret)
		return ret;

	device->current_page = page;
	device->page_cache_valid = true;

	return 0;
}

/* ---------- Register Access ---------------------------------------- */

/**
 * @brief Read a register over SPI.
 * @param device   - Device descriptor.
 * @param reg_addr - Register address (6-bit).
 * @param reg_data - Pointer to store the 16-bit read value.
 * @param verif    - Optional pointer for verification byte (can be NULL).
 * @return 0 on success, negative error code otherwise.
 *
 * SPI frame format for position sensors typically includes:
 * TX: [addr_byte] [0x00] [0x00] [0x00]
 * RX: [status]    [data_hi] [data_lo] [crc/verif]
 */
int <devname>_reg_read(struct <devname>_dev *device, uint8_t reg_addr,
		       uint16_t *reg_data, uint8_t *verif)
{
	uint8_t buf[4] = { 0 };
	int ret;

	if (!device || !device->spi_desc || !reg_data)
		return -EINVAL;

	buf[0] = no_os_field_prep(<DEVNAME>_RW_MASK, reg_addr);

	ret = no_os_spi_write_and_read(device->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	*reg_data = no_os_get_unaligned_be16(buf + 1);

	if (verif)
		*verif = buf[3];

	return 0;
}

/**
 * @brief Write a register over SPI.
 * @param device   - Device descriptor.
 * @param reg_addr - Register address (6-bit).
 * @param reg_data - 16-bit value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *device, uint8_t reg_addr,
			uint16_t reg_data)
{
	uint8_t buf[4] = { 0 };

	if (!device || !device->spi_desc)
		return -EINVAL;

	buf[0] = <DEVNAME>_WR_EN |
		 no_os_field_prep(<DEVNAME>_RW_MASK, reg_addr);
	buf[1] = (uint8_t)(reg_data >> 8);
	buf[2] = (uint8_t)(reg_data & 0xFF);
	/* buf[3] = CRC if required by the device */

	return no_os_spi_write_and_read(device->spi_desc, buf, sizeof(buf));
}

/**
 * @brief Read-modify-write a register field.
 * @param device      - Device descriptor.
 * @param reg_addr    - Register address.
 * @param update_mask - Bitmask of the field to update.
 * @param update_val  - New value (pre-shifted with no_os_field_prep).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_update(struct <devname>_dev *device, uint8_t reg_addr,
			 uint16_t update_mask, uint16_t update_val)
{
	uint16_t reg_val;
	int ret;

	ret = <devname>_reg_read(device, reg_addr, &reg_val, NULL);
	if (ret)
		return ret;

	reg_val &= ~update_mask;
	reg_val |= (update_val & update_mask);

	return <devname>_reg_write(device, reg_addr, reg_val);
}

/* ---------- Conversion Trigger ------------------------------------- */

/**
 * @brief Toggle the CNV pin to start a new conversion.
 * @param device - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_toggle_cnv(struct <devname>_dev *device)
{
	uint16_t temp;
	int ret;

	if (!device || !device->spi_desc)
		return -EINVAL;

	if (device->gpio_cnv_desc) {
		/* Hardware CNV pin toggle */
		ret = no_os_gpio_set_value(device->gpio_cnv_desc,
					   NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		no_os_udelay(1);

		ret = no_os_gpio_set_value(device->gpio_cnv_desc,
					   NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	} else {
		/* Software-triggered conversion via register */
		temp = no_os_field_prep(<DEVNAME>_CNV_EDGE_MASK, 0x3);
		ret = <devname>_reg_update(device,
					   <DEVNAME>_AGP_REG_CNVPAGE,
					   <DEVNAME>_CNV_EDGE_MASK, temp);
		if (ret)
			return ret;

		temp = no_os_field_prep(<DEVNAME>_CNV_EDGE_MASK, 0x0);
		ret = <devname>_reg_update(device,
					   <DEVNAME>_AGP_REG_CNVPAGE,
					   <DEVNAME>_CNV_EDGE_MASK, temp);
		if (ret)
			return ret;
	}

	return 0;
}

/* ---------- Angle & Turn Count ------------------------------------- */

/**
 * @brief Read raw angle data from ABSANGLE and ANGLE registers.
 * @param device     - Device descriptor.
 * @param angle_data - Array of at least 2 uint16_t to receive
 *                     [0] = ABSANGLE, [1] = ANGLE.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_raw_angle_read(struct <devname>_dev *device,
			     uint16_t *angle_data)
{
	int ret;

	if (!device || !device->spi_desc || !angle_data)
		return -EINVAL;

	ret = <devname>_reg_read(device, <DEVNAME>_AGP_REG_ABSANGLE,
				 &angle_data[0], NULL);
	if (ret)
		return ret;

	ret = <devname>_reg_read(device, <DEVNAME>_AGP_REG_ANGLE,
				 &angle_data[1], NULL);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get raw turn count and angle data.
 * @param device - Device descriptor.
 * @param turns  - Pointer to store raw turn count.
 * @param angle  - Array of at least 2 uint16_t [ABSANGLE, ANGLE].
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_raw_turns_and_angle(struct <devname>_dev *device,
				      uint8_t *turns, uint16_t *angle)
{
	int ret;

	if (!device || !device->spi_desc || !turns || !angle)
		return -EINVAL;

	ret = <devname>_raw_angle_read(device, angle);
	if (ret)
		return ret;

	*turns = no_os_field_get(<DEVNAME>_TURN_CNT_MASK, angle[0]);

	angle[0] = no_os_field_get(<DEVNAME>_ABS_ANGLE_MASK, angle[0]);
	angle[1] = no_os_field_get(<DEVNAME>_ANGLE_MASK, angle[1]);

	return 0;
}

/* ---------- Sensor Readback ---------------------------------------- */

/**
 * @brief Read raw temperature value.
 * @param device - Device descriptor.
 * @param temp   - Pointer to store 12-bit raw temperature.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_temp(struct <devname>_dev *device, uint16_t *temp)
{
	uint16_t raw_val;
	int ret;

	if (!device || !device->spi_desc || !temp)
		return -EINVAL;

	ret = <devname>_set_page(device, 0);
	if (ret)
		return ret;

	ret = <devname>_reg_read(device, <DEVNAME>_00_REG_TMP,
				 &raw_val, NULL);
	if (ret)
		return ret;

	*temp = no_os_field_get(<DEVNAME>_TEMP_MASK, raw_val);

	return 0;
}

/**
 * @brief Read sine component from the AMR sensor.
 * @param device      - Device descriptor.
 * @param val         - Pointer to store signed sine value.
 * @param is_new_data - Optional new-data flag (can be NULL).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_sin(struct <devname>_dev *device, int16_t *val,
		      bool *is_new_data)
{
	uint16_t raw_val;
	int ret;

	if (!device || !device->spi_desc || !val)
		return -EINVAL;

	ret = <devname>_set_page(device, 0);
	if (ret)
		return ret;

	ret = <devname>_reg_read(device, <DEVNAME>_00_REG_SINE,
				 &raw_val, NULL);
	if (ret)
		return ret;

	*val = no_os_sign_extend16(
		no_os_field_get(<DEVNAME>_RAW_SINE_MASK, raw_val), 13);

	if (is_new_data)
		*is_new_data = (bool)(raw_val & NO_OS_BIT(0));

	return 0;
}

/**
 * @brief Read cosine component from the AMR sensor.
 * @param device      - Device descriptor.
 * @param val         - Pointer to store signed cosine value.
 * @param is_new_data - Optional new-data flag (can be NULL).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_cos(struct <devname>_dev *device, int16_t *val,
		      bool *is_new_data)
{
	uint16_t raw_val;
	int ret;

	if (!device || !device->spi_desc || !val)
		return -EINVAL;

	ret = <devname>_set_page(device, 0);
	if (ret)
		return ret;

	ret = <devname>_reg_read(device, <DEVNAME>_00_REG_COSINE,
				 &raw_val, NULL);
	if (ret)
		return ret;

	*val = no_os_sign_extend16(
		no_os_field_get(<DEVNAME>_RAW_COSINE_MASK, raw_val), 13);

	if (is_new_data)
		*is_new_data = (bool)(raw_val & NO_OS_BIT(0));

	return 0;
}

/**
 * @brief Read radius (signal amplitude) value.
 * @param device      - Device descriptor.
 * @param radius      - Pointer to store raw radius.
 * @param is_new_data - Optional new-data flag (can be NULL).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_radius(struct <devname>_dev *device, uint16_t *radius,
			 bool *is_new_data)
{
	uint16_t raw_val;
	int ret;

	if (!device || !device->spi_desc || !radius)
		return -EINVAL;

	ret = <devname>_set_page(device, 0);
	if (ret)
		return ret;

	ret = <devname>_reg_read(device, <DEVNAME>_00_REG_RADIUS,
				 &raw_val, NULL);
	if (ret)
		return ret;

	*radius = no_os_field_get(<DEVNAME>_RADIUS_MASK, raw_val);

	if (is_new_data)
		*is_new_data = (bool)(raw_val & NO_OS_BIT(0));

	return 0;
}

/* ---------- Fault Handling ----------------------------------------- */

/**
 * @brief Read the fault register.
 * @param device - Device descriptor.
 * @param faults - Pointer to store 16-bit fault flags.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_faults(struct <devname>_dev *device, uint16_t *faults)
{
	if (!device || !device->spi_desc || !faults)
		return -EINVAL;

	return <devname>_reg_read(device, <DEVNAME>_AGP_REG_FAULT,
				  faults, NULL);
}

/**
 * @brief Clear all fault flags by writing 0xFFFF to the fault register.
 * @param device - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_clear_all_faults(struct <devname>_dev *device)
{
	if (!device || !device->spi_desc)
		return -EINVAL;

	return <devname>_reg_write(device, <DEVNAME>_AGP_REG_FAULT, 0xFFFF);
}

/* ---------- Reset -------------------------------------------------- */

/**
 * @brief Perform a hardware reset via the reset GPIO pin.
 * @param device - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_hard_reset(struct <devname>_dev *device)
{
	int ret;

	if (!device)
		return -EINVAL;

	if (!device->gpio_reset_desc)
		return -ENODEV;

	ret = no_os_gpio_set_value(device->gpio_reset_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(1);

	ret = no_os_gpio_set_value(device->gpio_reset_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(<DEVNAME>_STARTUP_TIME_MS);

	device->page_cache_valid = false;

	return 0;
}

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the <DEVNAME> device.
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 *
 * Resource acquisition order:
 *   1. calloc device struct
 *   2. SPI init
 *   3. Reset GPIO (optional)
 *   4. CNV GPIO (optional)
 *   5. Device configuration
 * On error, resources are freed in reverse order via goto labels.
 */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->page_cache_valid = false;
	dev->current_page = 0;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init_param);
	if (ret)
		goto free_dev;

	/* Optional reset GPIO */
	no_os_gpio_get_optional(&dev->gpio_reset_desc,
				&init_param->gpio_reset_ip);

	if (dev->gpio_reset_desc) {
		ret = no_os_gpio_direction_output(dev->gpio_reset_desc,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto free_gpio_reset;
	}

	/* Optional CNV GPIO */
	no_os_gpio_get_optional(&dev->gpio_cnv_desc,
				&init_param->gpio_cnv_ip);

	if (dev->gpio_cnv_desc) {
		ret = no_os_gpio_direction_output(dev->gpio_cnv_desc,
						  NO_OS_GPIO_LOW);
		if (ret)
			goto free_gpio_cnv;
	}

	/* Device-specific configuration goes here:
	 *   - Verify product/chip ID
	 *   - Set conversion mode
	 *   - Configure angle filter, harmonics, etc.
	 */

	*device = dev;

	return 0;

free_gpio_cnv:
	if (dev->gpio_cnv_desc)
		no_os_gpio_remove(dev->gpio_cnv_desc);

free_gpio_reset:
	if (dev->gpio_reset_desc)
		no_os_gpio_remove(dev->gpio_reset_desc);

	no_os_spi_remove(dev->spi_desc);

free_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free resources allocated by <devname>_init().
 * @param device - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 *
 * Resources are freed in reverse order of acquisition.
 */
int <devname>_remove(struct <devname>_dev *device)
{
	if (!device)
		return -ENODEV;

	if (device->gpio_cnv_desc)
		no_os_gpio_remove(device->gpio_cnv_desc);

	if (device->gpio_reset_desc)
		no_os_gpio_remove(device->gpio_reset_desc);

	if (device->spi_desc)
		no_os_spi_remove(device->spi_desc);

	no_os_free(device);

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
#ifndef IIO_<DEVNAME>_H
#define IIO_<DEVNAME>_H

#include "iio.h"
#include "<devname>.h"

/** @enum <devname>_iio_chan_type
 *  @brief IIO channel indices for the position sensor. */
enum <devname>_iio_chan_type {
	<DEVNAME>_CH_TURNS,
	<DEVNAME>_CH_ANGLE,
	<DEVNAME>_CH_TEMP,
	<DEVNAME>_CH_COSINE,
	<DEVNAME>_CH_SINE,
	<DEVNAME>_CH_RADIUS,
	<DEVNAME>_NUM_CHAN,
};

/**
 * @struct <devname>_iio_dev
 * @brief  IIO-specific device descriptor.
 */
struct <devname>_iio_dev {
	/** Core driver descriptor. */
	struct <devname>_dev *<devname>_desc;
	/** IIO device structure. */
	struct iio_device *iio_dev;
	/** Data buffer for triggered reads (one entry per channel). */
	int16_t data[<DEVNAME>_NUM_CHAN];
};

/**
 * @struct <devname>_iio_dev_init_param
 * @brief  IIO initialization parameters.
 */
struct <devname>_iio_dev_init_param {
	/** Core driver init params. */
	struct <devname>_init_param *<devname>_init_param;
};

/** @brief Initialise the IIO descriptor. */
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_dev_init_param *init_param);

/** @brief Free IIO descriptor resources. */
int <devname>_iio_remove(struct <devname>_iio_dev *desc);

/** @brief IIO trigger descriptor (for hardware-triggered acquisition). */
extern struct iio_trigger <devname>_iio_trig_desc;

#endif /* IIO_<DEVNAME>_H */
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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "iio_<devname>.h"
#include "<devname>.h"
#include "iio.h"
#include "iio_trigger.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_util.h"

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

static int <devname>_iio_reg_read(struct <devname>_iio_dev *dev,
				  uint32_t reg, uint32_t *readval);

static int <devname>_iio_reg_write(struct <devname>_iio_dev *dev,
				   uint32_t reg, uint32_t writeval);

static int <devname>_iio_trigger_handler(struct iio_device_data *dev_data);

static int <devname>_iio_submit_buffer(struct iio_device_data *dev_data);

/* ---------- Channel attribute tables ------------------------------- */

/** Attributes for the turn-count channel (IIO_ROT). */
static struct iio_attribute <devname>_turn_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_raw },
	{ .name = "scale", .show = <devname>_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

/** Attributes for the angle channel (IIO_ANGL). */
static struct iio_attribute <devname>_angle_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_raw },
	{ .name = "scale", .show = <devname>_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

/** Attributes for the temperature channel (IIO_TEMP). */
static struct iio_attribute <devname>_temp_attrs[] = {
	{ .name = "raw",    .show = <devname>_iio_read_raw },
	{ .name = "scale",  .show = <devname>_iio_read_scale },
	{ .name = "offset", .show = <devname>_iio_read_offset },
	END_ATTRIBUTES_ARRAY
};

/** Attributes for sine/cosine channels (IIO_COUNT). */
static struct iio_attribute <devname>_sincos_attrs[] = {
	{ .name = "raw", .show = <devname>_iio_read_raw },
	END_ATTRIBUTES_ARRAY
};

/** Attributes for the radius channel (IIO_VOLTAGE). */
static struct iio_attribute <devname>_radius_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_raw },
	{ .name = "scale", .show = <devname>_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan types --------------------------------------------- */

static struct scan_type <devname>_iio_angle_scan_type = {
	.sign = 'u',
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static struct scan_type <devname>_iio_temp_scan_type = {
	.sign = 'u',
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static struct scan_type <devname>_iio_turns_scan_type = {
	.sign = 's',
	.realbits = 8,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static struct scan_type <devname>_iio_sincos_scan_type = {
	.sign = 's',
	.realbits = 14,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static struct scan_type <devname>_iio_radius_scan_type = {
	.sign = 'u',
	.realbits = 15,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * Position sensor channels:
 *   - turns  (IIO_ROT)    : rotation count
 *   - angle  (IIO_ANGL)   : angular position
 *   - temp   (IIO_TEMP)   : die temperature
 *   - cosine (IIO_COUNT)  : quadrature cosine component
 *   - sine   (IIO_COUNT)  : quadrature sine component
 *   - radius (IIO_VOLTAGE): signal amplitude
 */
static struct iio_channel <devname>_channels[] = {
	{
		.name = "turns",
		.ch_type = IIO_ROT,
		.channel = 0,
		.address = <DEVNAME>_CH_TURNS,
		.scan_index = <DEVNAME>_CH_TURNS,
		.attributes = <devname>_turn_attrs,
		.scan_type = &<devname>_iio_turns_scan_type,
		.ch_out = false,
	},
	{
		.name = "angle",
		.ch_type = IIO_ANGL,
		.address = <DEVNAME>_CH_ANGLE,
		.scan_index = <DEVNAME>_CH_ANGLE,
		.attributes = <devname>_angle_attrs,
		.scan_type = &<devname>_iio_angle_scan_type,
		.ch_out = false,
	},
	{
		.name = "temp",
		.ch_type = IIO_TEMP,
		.address = <DEVNAME>_CH_TEMP,
		.scan_index = <DEVNAME>_CH_TEMP,
		.attributes = <devname>_temp_attrs,
		.scan_type = &<devname>_iio_temp_scan_type,
		.ch_out = false,
	},
	{
		.name = "cosine",
		.ch_type = IIO_COUNT,
		.channel = 1,
		.address = <DEVNAME>_CH_COSINE,
		.scan_index = <DEVNAME>_CH_COSINE,
		.attributes = <devname>_sincos_attrs,
		.scan_type = &<devname>_iio_sincos_scan_type,
		.indexed = 1,
		.ch_out = false,
	},
	{
		.name = "sine",
		.ch_type = IIO_COUNT,
		.channel = 2,
		.address = <DEVNAME>_CH_SINE,
		.scan_index = <DEVNAME>_CH_SINE,
		.attributes = <devname>_sincos_attrs,
		.scan_type = &<devname>_iio_sincos_scan_type,
		.indexed = 1,
		.ch_out = false,
	},
	{
		.name = "radius",
		.ch_type = IIO_VOLTAGE,
		.address = <DEVNAME>_CH_RADIUS,
		.scan_index = <DEVNAME>_CH_RADIUS,
		.attributes = <devname>_radius_attrs,
		.scan_type = &<devname>_iio_radius_scan_type,
		.ch_out = false,
	},
};

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_device = {
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_channels),
	.channels = <devname>_channels,
	.debug_reg_read = (int32_t (*)())<devname>_iio_reg_read,
	.debug_reg_write = (int32_t (*)())<devname>_iio_reg_write,
	.trigger_handler = (int32_t (*)())<devname>_iio_trigger_handler,
	.submit = (int32_t (*)())<devname>_iio_submit_buffer,
};

/* ---------- Attribute callbacks ------------------------------------ */

/**
 * @brief Read raw value for a channel.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_dev *iio_desc = dev;
	struct <devname>_dev *desc = iio_desc-><devname>_desc;
	int ret;
	uint16_t angle[2];
	uint8_t raw_turns;
	int16_t sincos_val;
	uint16_t temp;

	if (!dev)
		return -EINVAL;

	switch (channel->type) {
	case IIO_ANGL:
	case IIO_ROT:
	case IIO_COUNT:
		/* Trigger conversion if in one-shot mode */
		if (desc->is_one_shot) {
			ret = <devname>_toggle_cnv(desc);
			if (ret)
				return ret;

			no_os_mdelay(<DEVNAME>_CONVERSION_DELAY_MS);
		}

		ret = <devname>_get_raw_turns_and_angle(desc, &raw_turns,
							angle);
		if (ret)
			return ret;

		switch (channel->type) {
		case IIO_ANGL:
			ret = (int32_t)angle[1];
			break;
		case IIO_ROT:
			ret = (int32_t)raw_turns;
			break;
		case IIO_COUNT:
			if (channel->ch_num == 1) {
				ret = <devname>_get_cos(desc, &sincos_val,
							NULL);
				if (ret)
					return ret;
				ret = (int32_t)sincos_val;
			} else if (channel->ch_num == 2) {
				ret = <devname>_get_sin(desc, &sincos_val,
							NULL);
				if (ret)
					return ret;
				ret = (int32_t)sincos_val;
			} else {
				return -EINVAL;
			}
			break;
		default:
			return -EINVAL;
		}

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);

	case IIO_TEMP:
		ret = <devname>_get_temp(desc, &temp);
		if (ret)
			return ret;

		ret = (int32_t)temp;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);

	case IIO_VOLTAGE:
		if (channel->address == <DEVNAME>_CH_RADIUS) {
			uint16_t radius_val;

			ret = <devname>_get_radius(desc, &radius_val, NULL);
			if (ret)
				return ret;

			ret = (int32_t)radius_val;

			return iio_format_value(buf, len, IIO_VAL_INT, 1,
						&ret);
		}

		return -EINVAL;

	default:
		return -EINVAL;
	}
}

/**
 * @brief Read scale for a channel.
 *
 * Position sensors express scale as:
 *   - Angle:  360 / resolution  (degrees per LSB)
 *   - Turns:  fractional (1/4 for quarter-turn count)
 *   - Temp:   raw * scale + offset = millidegrees C
 *   - Radius: raw * scale = mV/V
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t vals[2];

	switch (channel->type) {
	case IIO_ROT:
		/* 1 turn = 4 quarter turns */
		vals[0] = 1;
		vals[1] = 4;

		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2,
					vals);
	case IIO_ANGL:
		vals[0] = <DEVNAME>_FULL_TURN_DEGREES;
		vals[1] = <DEVNAME>_ANGLE_RES;

		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2,
					vals);
	case IIO_TEMP:
		/* scale = 1000000 / 15660 => millidegrees per LSB */
		vals[0] = 1000000;
		vals[1] = <DEVNAME>_TEMP_SCALE_MILLIS;

		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2,
					vals);
	case IIO_VOLTAGE:
		if (channel->address == <DEVNAME>_CH_RADIUS) {
			/* radius scale for mV/V output */
			vals[0] = 924;
			vals[1] = 1000000;

			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL,
						2, vals);
		}

		return -EINVAL;

	default:
		return -EINVAL;
	}
}

/**
 * @brief Read offset for a channel (only temperature has an offset).
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val;

	switch (channel->type) {
	case IIO_TEMP:
		val = -<DEVNAME>_TEMP_OFFSET;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EINVAL;
	}
}

/* ---------- Debug register access ---------------------------------- */

/**
 * @brief Register read wrapper for IIO debug interface.
 */
static int <devname>_iio_reg_read(struct <devname>_iio_dev *dev,
				  uint32_t reg, uint32_t *readval)
{
	int ret;
	uint16_t temp;

	ret = <devname>_reg_read(dev-><devname>_desc, (uint8_t)reg,
				 &temp, NULL);
	*readval = temp;

	return ret;
}

/**
 * @brief Register write wrapper for IIO debug interface.
 */
static int <devname>_iio_reg_write(struct <devname>_iio_dev *dev,
				   uint32_t reg, uint32_t writeval)
{
	return <devname>_reg_write(dev-><devname>_desc, (uint8_t)reg,
				   (uint16_t)writeval);
}

/* ---------- Triggered / buffered read ------------------------------ */

/**
 * @brief Trigger handler: reads one data set and pushes it to the buffer.
 * @param dev_data - IIO device data structure.
 * @return 0 on success, negative error code otherwise.
 *
 * Reads each enabled channel in scan-index order, packing values
 * into the data[] array, then pushes a single scan to the IIO buffer.
 */
static int <devname>_iio_trigger_handler(struct iio_device_data *dev_data)
{
	struct <devname>_iio_dev *iio_desc;
	struct <devname>_dev *desc;
	int i = 0;
	int ret;
	uint16_t angles[2];
	uint8_t turns;

	if (!dev_data)
		return -EINVAL;

	iio_desc = (struct <devname>_iio_dev *)dev_data->dev;
	desc = iio_desc-><devname>_desc;

	ret = <devname>_get_raw_turns_and_angle(desc, &turns, angles);
	if (ret)
		return ret;

	if (dev_data->buffer->active_mask & NO_OS_BIT(<DEVNAME>_CH_TURNS))
		iio_desc->data[i++] = (int16_t)turns;

	if (dev_data->buffer->active_mask & NO_OS_BIT(<DEVNAME>_CH_ANGLE))
		iio_desc->data[i++] = (int16_t)angles[1];

	if (dev_data->buffer->active_mask & NO_OS_BIT(<DEVNAME>_CH_TEMP)) {
		uint16_t temp;

		ret = <devname>_get_temp(desc, &temp);
		if (ret)
			return ret;

		iio_desc->data[i++] = (int16_t)temp;
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(<DEVNAME>_CH_COSINE)) {
		ret = <devname>_get_cos(desc, &iio_desc->data[i], NULL);
		if (ret)
			return ret;

		i++;
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(<DEVNAME>_CH_SINE)) {
		ret = <devname>_get_sin(desc, &iio_desc->data[i], NULL);
		if (ret)
			return ret;

		i++;
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(<DEVNAME>_CH_RADIUS)) {
		uint16_t radius;

		ret = <devname>_get_radius(desc, &radius, NULL);
		if (ret)
			return ret;

		iio_desc->data[i++] = (int16_t)radius;
	}

	return iio_buffer_push_scan(dev_data->buffer, &iio_desc->data[0]);
}

/**
 * @brief Submit buffer handler for software-triggered conversion.
 * @param dev_data - IIO device data structure.
 * @return 0 on success, negative error code otherwise.
 */
static int <devname>_iio_submit_buffer(struct iio_device_data *dev_data)
{
	struct <devname>_iio_dev *iio_desc;
	struct <devname>_dev *desc;
	int ret;

	if (!dev_data)
		return -EINVAL;

	iio_desc = (struct <devname>_iio_dev *)dev_data->dev;
	desc = iio_desc-><devname>_desc;

	/* Trigger conversion manually */
	ret = <devname>_toggle_cnv(desc);
	if (ret)
		return ret;

	no_os_mdelay(<DEVNAME>_CONVERSION_DELAY_MS);

	return <devname>_iio_trigger_handler(dev_data);
}

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 */
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_dev_init_param *init_param)
{
	struct <devname>_iio_dev *descriptor;
	int ret;

	if (!init_param)
		return -EINVAL;

	descriptor = (struct <devname>_iio_dev *)no_os_calloc(1,
							      sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = <devname>_init(&descriptor-><devname>_desc,
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
	if (!desc)
		return -EINVAL;

	<devname>_remove(desc-><devname>_desc);
	no_os_free(desc);

	return 0;
}
```

---

## 7. Project Example

### 7.1 `projects/<project_name>/Makefile`

```makefile
EXAMPLE ?= basic
FIRMWARE_VERSION ?= v1.0.0-a.0

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
```

### 7.2 `projects/<project_name>/builds.json`

```json
{
  "<platform>": {
    "basic": {
      "flags" : "EXAMPLE=basic"
    },
    "iio_example": {
      "flags" : "EXAMPLE=iio_example"
    }
  }
}
```

### 7.3 `projects/<project_name>/src.mk`

```makefile
INCS += $(INCLUDE)/no_os_delay.h	\
	$(INCLUDE)/no_os_error.h	\
	$(INCLUDE)/no_os_gpio.h		\
	$(INCLUDE)/no_os_print_log.h	\
	$(INCLUDE)/no_os_spi.h		\
	$(INCLUDE)/no_os_alloc.h	\
	$(INCLUDE)/no_os_irq.h		\
	$(INCLUDE)/no_os_list.h		\
	$(INCLUDE)/no_os_uart.h		\
	$(INCLUDE)/no_os_lf256fifo.h	\
	$(INCLUDE)/no_os_util.h		\
	$(INCLUDE)/no_os_units.h	\
	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c	\
	$(NO-OS)/util/no_os_lf256fifo.c	\
	$(DRIVERS)/api/no_os_irq.c	\
	$(DRIVERS)/api/no_os_spi.c	\
	$(DRIVERS)/api/no_os_uart.c	\
	$(NO-OS)/util/no_os_list.c	\
	$(NO-OS)/util/no_os_util.c	\
	$(NO-OS)/util/no_os_alloc.c	\
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/position/<devname>/<devname>.h
SRCS += $(DRIVERS)/position/<devname>/<devname>.c
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
INCS += $(DRIVERS)/position/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/position/<devname>/iio_<devname>.c

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

#include "parameters.h"
#include "<devname>.h"

#ifdef IIO_SUPPORT
#include "iio_<devname>.h"
#endif

extern struct no_os_uart_init_param <devname>_uart_ip;
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
#include "no_os_gpio.h"

struct no_os_uart_init_param <devname>_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

const struct no_os_spi_init_param <devname>_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
};

const struct no_os_gpio_init_param gpio_reset_ip = {
	.port = GPIO_RESET_PORT,
	.number = GPIO_RESET,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct <devname>_init_param <devname>_ip = {
	.spi_init_param = <devname>_spi_ip,
	.gpio_reset_ip = gpio_reset_ip,
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

int example_main();

#endif /* __BASIC_EXAMPLE_H__ */
```

### 7.8 `projects/<project_name>/src/examples/basic/basic_example.c`

```c
/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for <devname> project
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
int example_main()
{
	struct <devname>_dev *dev;
	uint8_t raw_turns;
	uint16_t raw_angle[2];
	uint16_t raw_temp;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret) {
		pr_info("Failed to initialize <DEVNAME>: %d\r\n", ret);
		return ret;
	}

	while (1) {
		/* Read turns and angle */
		ret = <devname>_get_raw_turns_and_angle(dev, &raw_turns,
							raw_angle);
		if (ret)
			goto free_dev;

		pr_info("Angle: %u (raw), Turns: %u\r\n",
			raw_angle[1], raw_turns);

		/* Read temperature */
		ret = <devname>_get_temp(dev, &raw_temp);
		if (ret)
			goto free_dev;

		pr_info("Temperature raw: %u\r\n", raw_temp);

		no_os_mdelay(500);
	}

free_dev:
	<devname>_remove(dev);

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

int example_main();

#endif /* __IIO_EXAMPLE_H__ */
```

### 7.10 `projects/<project_name>/src/examples/iio_example/iio_example.c`

```c
/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for <devname> project
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
int example_main()
{
	int ret;
	struct <devname>_iio_dev *<devname>_iio_desc;
	struct <devname>_iio_dev_init_param <devname>_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	<devname>_iio_ip.<devname>_init_param = &<devname>_ip;

	ret = <devname>_iio_init(&<devname>_iio_desc, &<devname>_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "<devname>",
			.dev = <devname>_iio_desc,
			.dev_descriptor = <devname>_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = <devname>_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error;

	ret = iio_app_run(app);
	if (ret)
		pr_info("Error: iio_app_run: %d\r\n", ret);

	iio_app_remove(app);
error:
	<devname>_iio_remove(<devname>_iio_desc);

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

#ifdef STM32_PLATFORM
#include "stm32/parameters.h"
#endif

#ifdef MAXIM_PLATFORM
#include "maxim/parameters.h"
#endif

#ifdef IIO_SUPPORT
#include "iio_app.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
```

### 7.12 `projects/<project_name>/src/platform/<platform>/main.c`

```c
/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for <platform> platform of <devname> project.
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

	ret = no_os_uart_init(&uart, &<devname>_uart_ip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart);
	ret = example_main();
	if (ret)
		goto error;
#endif

#ifdef IIO_EXAMPLE
	return example_main();
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

### 7.13 `projects/<project_name>/src/platform/<platform>/parameters.h`

```c
/***************************************************************************//**
 *   @brief  Definitions specific to <platform> used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/* Include platform-specific headers (e.g., stm32_spi.h, maxim_spi.h) */
#include "<platform>_uart.h"
#include "<platform>_uart_stdio.h"
#include "<platform>_spi.h"
#include "<platform>_gpio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID	0
#endif

#define UART_IRQ_ID	/* platform-specific IRQ */
#define UART_DEVICE_ID	1
#define UART_BAUDRATE	115200
#define UART_OPS	&<platform>_uart_ops
#define UART_EXTRA	&<platform>_uart_extra

#define SPI_DEVICE_ID	1
#define SPI_CS		0
#define SPI_BAUDRATE	1000000
#define SPI_OPS		&<platform>_spi_ops
#define SPI_EXTRA	&<platform>_spi_extra

#define GPIO_OPS	&<platform>_gpio_ops
#define GPIO_EXTRA	&<platform>_gpio_extra

#define GPIO_RESET_PORT	0
#define GPIO_RESET	/* pin number */

extern struct <platform>_uart_init_param <platform>_uart_extra;
extern struct <platform>_spi_init_param <platform>_spi_extra;
extern struct <platform>_gpio_init_param <platform>_gpio_extra;

#endif /* __PARAMETERS_H__ */
```

### 7.14 `projects/<project_name>/src/platform/<platform>/parameters.c`

```c
/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of <platform> platform data used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#include "parameters.h"

struct <platform>_uart_init_param <platform>_uart_extra = {
	/* Platform-specific UART configuration */
};

struct <platform>_spi_init_param <platform>_spi_extra = {
	/* Platform-specific SPI configuration */
};

struct <platform>_gpio_init_param <platform>_gpio_extra = {
	/* Platform-specific GPIO configuration */
};
```

### 7.15 `projects/<project_name>/src/platform/<platform>/platform_src.mk`

```makefile
INCS +=	$(PLATFORM_DRIVERS)/<platform>_gpio.h      \
	$(PLATFORM_DRIVERS)/<platform>_spi.h       \
	$(PLATFORM_DRIVERS)/<platform>_irq.h       \
	$(PLATFORM_DRIVERS)/<platform>_uart.h      \
	$(PLATFORM_DRIVERS)/<platform>_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/<platform>_delay.c     \
	$(PLATFORM_DRIVERS)/<platform>_gpio.c      \
	$(PLATFORM_DRIVERS)/<platform>_spi.c       \
	$(PLATFORM_DRIVERS)/<platform>_irq.c       \
	$(PLATFORM_DRIVERS)/<platform>_uart.c      \
	$(PLATFORM_DRIVERS)/<platform>_uart_stdio.c
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

The <DEVNAME> is a high-precision magnetic position sensor with an
SPI-compatible serial interface. It provides angular position measurement
with <resolution>-bit resolution, multi-turn tracking, and integrated
temperature sensing. The device uses page-based register access, with
measurement data on page 0 and configuration on page 2.
<Add 2-3 more sentences from the datasheet covering key specs and
features such as harmonic correction, conversion modes, etc.>

Applications
------------

* Motor position feedback
* Robotics joint tracking
* Industrial automation
* Rotary encoder replacement

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface, configures optional
GPIO pins (reset, CNV), and applies the initial device
configuration. The function uses goto-based cleanup to release
resources on failure.

Angle Measurement
~~~~~~~~~~~~~~~~~

The ``<devname>_get_raw_turns_and_angle`` function reads the ABSANGLE
and ANGLE registers in a single operation, extracting the turn count
and angular position. The ``<devname>_raw_angle_read`` function
provides direct access to both angle registers.

Temperature Readback
~~~~~~~~~~~~~~~~~~~~

The ``<devname>_get_temp`` function reads the on-chip temperature
sensor by switching to page 0 and reading the temperature register.
Raw values can be converted to degrees Celsius using the device-specific
scale and offset constants.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .spi_init_param = { /* platform-specific SPI params */ },
           .gpio_reset_ip = { /* optional reset GPIO params */ },
       };

       struct <devname>_dev *dev;
       uint8_t turns;
       uint16_t angle[2];
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_get_raw_turns_and_angle(dev, &turns, angle);
       if (ret)
           goto cleanup;

       /* angle[1] contains the raw angular position */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes the following channels:

* ``IIO_ROT`` (turns) - rotation count with ``raw`` and ``scale``
* ``IIO_ANGL`` (angle) - angular position with ``raw`` and ``scale``
* ``IIO_TEMP`` (temp) - temperature with ``raw``, ``scale``, ``offset``
* ``IIO_COUNT`` (cosine/sine) - quadrature components with ``raw``
* ``IIO_VOLTAGE`` (radius) - signal amplitude with ``raw`` and ``scale``

The driver supports both software-triggered and hardware-triggered
buffered data acquisition.

IIO Initialization Example
---------------------------

.. code-block:: C

   int iio_example_main()
   {
       int ret;
       struct <devname>_iio_dev *iio_desc;
       struct <devname>_iio_dev_init_param iio_ip;
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
       app_init_param.uart_init_params = <devname>_uart_ip;

       ret = iio_app_init(&app, app_init_param);
       if (ret)
           return ret;

       return iio_app_run(app);
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/position/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/position/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- wrap header declarations in
   `extern "C" { }` guards; explicit casts from `void *`
   (e.g., `(struct <devname>_dev *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs for indentation, 80-char line limit.
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
11. **Page-based register access** -- cache the current page in the
    device struct to avoid redundant SPI writes; invalidate after reset.
12. **Optional GPIOs** -- use `no_os_gpio_get_optional()` for pins that
    may not be connected; guard all usage with NULL checks.
13. **Position-specific IIO channels** -- use `IIO_ANGL` for angular
    position, `IIO_ROT` for turn count, `IIO_TEMP` for die temperature,
    `IIO_COUNT` for quadrature signals, `IIO_VOLTAGE` for radius/amplitude.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> high-precision magnetic position sensor
with SPI interface, page-based register access, angular measurement,
multi-turn tracking, and integrated temperature sensing.

Signed-off-by: Your Name <your.name@analog.com>
```
