# Digital Power Controller Driver Template

Reference driver: `drivers/power/adp1050/`

This template covers every file needed to add a new digital power controller
driver to no-OS.  Replace `<devname>` with the part number (e.g.,
`adp1050`) and `<DEVNAME>` with its uppercase form (e.g., `ADP1050`)
throughout.

---

## 1. Naming Conventions

Power controller drivers use `_desc` for the device descriptor struct.

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adp1050.h`, `adp1050.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_adp1050.h`, `iio_adp1050.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADP_1050_H__` |
| Init param struct | `<devname>_init_param` | `adp1050_init_param` |
| Device struct | `<devname>_desc` | `adp1050_desc` |
| IIO descriptor | `<devname>_iio_desc` | `adp1050_iio_desc` |
| Functions | `<devname>_<verb>()` | `adp1050_init()` |
| PMBus address | `<DEVNAME>_PMBUS_<RESISTOR>_ADDRESS` | `ADP1050_PMBUS_10KOHM_ADDRESS` |
| PMBus commands | `<DEVNAME>_<COMMAND>` | `ADP1050_OPERATION` |
| Field masks | `<DEVNAME>_<FIELD>_MASK` | `ADP1050_MANT_MASK` |

---

## 2. File Checklist

```
drivers/power/<devname>/
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

doc/sphinx/source/drivers/power/<devname>.rst   # Sphinx entry
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
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_util.h"

/* ---- PMBus Extended Command Prefix -------------------------------- */

/** Extended command byte for manufacturer-specific commands > 0xFF. */
#define <DEVNAME>_EXTENDED_COMMAND		0xFF

/* ---- Byte/Field Masks --------------------------------------------- */

#define <DEVNAME>_LSB_MASK			NO_OS_GENMASK(7, 0)
#define <DEVNAME>_MSB_MASK			NO_OS_GENMASK(15, 8)

/** PMBus Linear11 mantissa (bits [10:0]). */
#define <DEVNAME>_MANT_MASK			NO_OS_GENMASK(10, 0)

/** PMBus Linear11 exponent (bits [15:11]). */
#define <DEVNAME>_EXP_MASK			NO_OS_GENMASK(15, 11)

#define <DEVNAME>_MANT_MAX			0x7FF
#define <DEVNAME>_EXP_MAX			0x1F

/* ---- ON/OFF Configuration ----------------------------------------- */

#define <DEVNAME>_ON_OFF_DEFAULT_CFG		0x00
#define <DEVNAME>_CTRL_PIN_ENABLE		NO_OS_BIT(4)

#define <DEVNAME>_OPERATION_ON			0x80
#define <DEVNAME>_OPERATION_OFF			0x00
#define <DEVNAME>_OPERATION_SOFT_OFF		0x40

/* ---- PMBus Addresses ---------------------------------------------- */

/**
 * PMBus address set by external resistor on ADDR pin.
 * Adjust base and offsets for the specific device.
 */
#define <DEVNAME>_PMBUS_BASE_ADDRESS		0x70

/* ---- PMBus Standard Command Set ----------------------------------- */

#define <DEVNAME>_OPERATION			0x01
#define <DEVNAME>_ON_OFF_CONFIG			0x02
#define <DEVNAME>_CLEAR_FAULTS			0x03
#define <DEVNAME>_WRITE_PROTECT			0x10
#define <DEVNAME>_STORE_USER_ALL		0x15
#define <DEVNAME>_RESTORE_USER_ALL		0x16
#define <DEVNAME>_VOUT_MODE			0x20
#define <DEVNAME>_VOUT_COMMAND			0x21
#define <DEVNAME>_VOUT_MAX			0x24
#define <DEVNAME>_VOUT_MARGIN_HIGH		0x25
#define <DEVNAME>_VOUT_MARGIN_LOW		0x26
#define <DEVNAME>_FREQUENCY_SWITCH		0x33
#define <DEVNAME>_VIN_ON			0x35
#define <DEVNAME>_VIN_OFF			0x36
#define <DEVNAME>_VOUT_OV_FAULT_LIMIT		0x40
#define <DEVNAME>_VOUT_OV_FAULT_RESPONSE	0x41
#define <DEVNAME>_VOUT_UV_FAULT_LIMIT		0x44
#define <DEVNAME>_VOUT_UV_FAULT_RESPONSE	0x45
#define <DEVNAME>_OT_FAULT_LIMIT		0x4F
#define <DEVNAME>_OT_FAULT_RESPONSE		0x50
#define <DEVNAME>_TON_DELAY			0x60
#define <DEVNAME>_TON_RISE			0x61
#define <DEVNAME>_TOFF_DELAY			0x64
#define <DEVNAME>_STATUS_BYTE			0x78
#define <DEVNAME>_STATUS_WORD			0x79
#define <DEVNAME>_STATUS_VOUT			0x7A
#define <DEVNAME>_STATUS_IOUT			0x7B
#define <DEVNAME>_STATUS_INPUT			0x7C
#define <DEVNAME>_STATUS_TEMPERATURE		0x7D
#define <DEVNAME>_STATUS_CML			0x7E
#define <DEVNAME>_READ_VIN			0x88
#define <DEVNAME>_READ_IIN			0x89
#define <DEVNAME>_READ_VOUT			0x8B
#define <DEVNAME>_READ_IOUT			0x8C
#define <DEVNAME>_READ_TEMPERATURE		0x8D
#define <DEVNAME>_READ_DUTY_CYCLE		0x94
#define <DEVNAME>_READ_FREQUENCY		0x95
#define <DEVNAME>_MFR_ID			0x99
#define <DEVNAME>_MFR_MODEL			0x9A
#define <DEVNAME>_IC_DEVICE_ID			0xAD

/* ---- Manufacturer-Specific Extended Commands ---------------------- */

/**
 * Extended commands use the 0xFF prefix byte followed by a 16-bit
 * command address.  Add device-specific extended registers here.
 *
 * Example:
 * #define <DEVNAME>_PWM_OUTPUT_DISABLE	0xFE53
 * #define <DEVNAME>_GO_COMMANDS		0xFE61
 */

/* ---- PWM Output Masks --------------------------------------------- */

#define <DEVNAME>_PWM_OUTPUT_DISABLE_MASK	0x3F
#define <DEVNAME>_ALL_PWM_OFF			0x3F

/* ---- Fault Response Masks ----------------------------------------- */

#define <DEVNAME>_FAULT_RESPONSE_MASK		NO_OS_GENMASK(3, 2)
#define <DEVNAME>_AFTER_FAULT_MASK		NO_OS_GENMASK(1, 0)

/* ---- Enumerations ------------------------------------------------- */

/**
 * @enum <devname>_loop
 * @brief Control loop operating mode.
 */
enum <devname>_loop {
	<DEVNAME>_CLOSE_LOOP,
	<DEVNAME>_OPEN_LOOP
};

/**
 * @enum <devname>_channel
 * @brief PWM output channel selection.
 */
enum <devname>_channel {
	<DEVNAME>_OUTA,
	<DEVNAME>_OUTB,
	<DEVNAME>_SR1,
	<DEVNAME>_SR2,
	<DEVNAME>_DISABLE_ALL
};

/**
 * @enum <devname>_value_type
 * @brief Telemetry reading type selector.
 */
enum <devname>_value_type {
	<DEVNAME>_VIN = <DEVNAME>_READ_VIN,
	<DEVNAME>_IIN = <DEVNAME>_READ_IIN,
	<DEVNAME>_IOUT = <DEVNAME>_READ_IOUT,
	<DEVNAME>_TEMP = <DEVNAME>_READ_TEMPERATURE,
	<DEVNAME>_DUTY_CYCLE = <DEVNAME>_READ_DUTY_CYCLE,
	<DEVNAME>_FREQUENCY = <DEVNAME>_READ_FREQUENCY,
};

/**
 * @enum <devname>_status_type
 * @brief Status register type selector.
 */
enum <devname>_status_type {
	<DEVNAME>_STATUS_VOUT_TYPE = <DEVNAME>_STATUS_VOUT,
	<DEVNAME>_STATUS_IOUT_TYPE = <DEVNAME>_STATUS_IOUT,
	<DEVNAME>_STATUS_INPUT_TYPE = <DEVNAME>_STATUS_INPUT,
	<DEVNAME>_STATUS_WORD_TYPE = <DEVNAME>_STATUS_WORD,
	<DEVNAME>_STATUS_TEMPERATURE_TYPE = <DEVNAME>_STATUS_TEMPERATURE,
	<DEVNAME>_STATUS_CML_TYPE = <DEVNAME>_STATUS_CML,
};

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters for <devname>_init().
 */
struct <devname>_init_param {
	/** I2C/PMBus initialization parameters. */
	struct no_os_i2c_init_param *i2c_param;
	/** Power-good / alert GPIO (optional). */
	struct no_os_gpio_init_param *pg_alt_param;
	/** CTRL pin GPIO for hardware on/off (optional). */
	struct no_os_gpio_init_param *ctrl_param;
	/** On/off configuration byte. */
	uint8_t on_off_config;
};

/**
 * @struct <devname>_desc
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_desc {
	/** I2C/PMBus descriptor. */
	struct no_os_i2c_desc *i2c_desc;
	/** Power-good / alert GPIO descriptor. */
	struct no_os_gpio_desc *pg_alt_desc;
	/** CTRL pin GPIO descriptor. */
	struct no_os_gpio_desc *ctrl_desc;
	/** Current control loop mode. */
	enum <devname>_loop loop;
	/** Current switching frequency. */
	uint16_t freq;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Send a command byte (no data) over PMBus. */
int <devname>_send_command(struct <devname>_desc *desc, uint16_t command);

/** @brief Read data from a PMBus command register. */
int <devname>_read(struct <devname>_desc *desc, uint16_t command,
		   uint8_t *data, uint8_t bytes_number);

/** @brief Write data to a PMBus command register. */
int <devname>_write(struct <devname>_desc *desc, uint16_t command,
		    uint16_t data, uint8_t bytes_number);

/** @brief Read a status register. */
int <devname>_read_status(struct <devname>_desc *desc,
			  enum <devname>_status_type status,
			  uint16_t *status_val);

/** @brief Read output voltage (VOUT) raw value. */
int <devname>_read_vsense(struct <devname>_desc *desc, uint16_t *vsense);

/** @brief Read a telemetry value (VIN/IIN/IOUT/TEMP) as Linear11. */
int <devname>_read_value(struct <devname>_desc *desc, uint16_t *mant,
			 uint8_t *exp, enum <devname>_value_type val_type);

/** @brief Set VOUT_COMMAND and VOUT_MAX. */
int <devname>_vout_value(struct <devname>_desc *desc, uint16_t vout_command,
			 uint16_t vout_max);

/** @brief Set PWM output channel and switching frequency. */
int <devname>_set_pwm(struct <devname>_desc *desc,
		      enum <devname>_channel chan, uint16_t freq);

/** @brief Set close-loop operation. */
int <devname>_set_close_loop(struct <devname>_desc *desc);

/** @brief Software-reset the device. */
int <devname>_software_reset(struct <devname>_desc *desc);

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

#include <stdlib.h>
#include <string.h>
#include "<devname>.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"

/**
 * @brief Send a command byte/word to the device over PMBus.
 * @param desc    - Device descriptor.
 * @param command - PMBus command code.
 * @return 0 on success, negative error code otherwise.
 *
 * Standard commands (<=0xFF) send a single byte.
 * Extended commands (>0xFF) send two bytes: 0xFF prefix + LSB.
 */
int <devname>_send_command(struct <devname>_desc *desc, uint16_t command)
{
	uint8_t data[2];
	uint8_t command_val;

	if (!desc)
		return -EINVAL;

	if (command > <DEVNAME>_EXTENDED_COMMAND) {
		data[0] = no_os_field_get(<DEVNAME>_LSB_MASK, command);
		data[1] = no_os_field_get(<DEVNAME>_MSB_MASK, command);

		return no_os_i2c_write(desc->i2c_desc, data, 2, 1);
	}

	command_val = no_os_field_get(<DEVNAME>_LSB_MASK, command);

	return no_os_i2c_write(desc->i2c_desc, &command_val, 1, 1);
}

/**
 * @brief Read data from a PMBus command register.
 * @param desc         - Device descriptor.
 * @param command      - PMBus command code.
 * @param data         - Buffer to store read data.
 * @param bytes_number - Number of data bytes to read (1 or 2).
 * @return 0 on success, negative error code otherwise.
 *
 * For extended commands (>0xFF), two address bytes are sent.
 * Then bytes_number bytes are read via I2C.
 */
int <devname>_read(struct <devname>_desc *desc, uint16_t command,
		   uint8_t *data, uint8_t bytes_number)
{
	int ret;
	uint8_t command_val[2] = {0, 0};
	uint8_t write_bytes;

	if (!desc)
		return -EINVAL;

	if (command > <DEVNAME>_EXTENDED_COMMAND) {
		command_val[1] = no_os_field_get(<DEVNAME>_LSB_MASK, command);
		command_val[0] = no_os_field_get(<DEVNAME>_MSB_MASK, command);
		write_bytes = 2;
	} else {
		command_val[0] = no_os_field_get(<DEVNAME>_LSB_MASK, command);
		write_bytes = 1;
	}
	ret = no_os_i2c_write(desc->i2c_desc, command_val, write_bytes, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(desc->i2c_desc, data, bytes_number, 1);
}

/**
 * @brief Write data to a PMBus command register.
 * @param desc         - Device descriptor.
 * @param command      - PMBus command code.
 * @param data         - Data word to write (byte or word).
 * @param bytes_number - Number of data bytes (1 or 2).
 * @return 0 on success, negative error code otherwise.
 *
 * PMBus data is sent in little-endian byte order.
 */
int <devname>_write(struct <devname>_desc *desc, uint16_t command,
		    uint16_t data, uint8_t bytes_number)
{
	uint8_t val[4] = {0, 0, 0, 0};

	if (!desc)
		return -EINVAL;

	if (command > <DEVNAME>_EXTENDED_COMMAND) {
		val[0] = no_os_field_get(<DEVNAME>_MSB_MASK, command);
		val[1] = no_os_field_get(<DEVNAME>_LSB_MASK, command);
		if (bytes_number > 1) {
			val[2] = no_os_field_get(<DEVNAME>_LSB_MASK, data);
			val[3] = no_os_field_get(<DEVNAME>_MSB_MASK, data);
		} else
			val[2] = no_os_field_get(<DEVNAME>_LSB_MASK, data);

		return no_os_i2c_write(desc->i2c_desc, val,
				       bytes_number + 2, 1);
	} else {
		val[0] = no_os_field_get(<DEVNAME>_LSB_MASK, command);
		if (bytes_number > 1) {
			val[1] = no_os_field_get(<DEVNAME>_LSB_MASK, data);
			val[2] = no_os_field_get(<DEVNAME>_MSB_MASK, data);
		} else
			val[1] = no_os_field_get(<DEVNAME>_LSB_MASK, data);

		return no_os_i2c_write(desc->i2c_desc, val,
				       bytes_number + 1, 1);
	}
}

/**
 * @brief Read a status register.
 * @param desc       - Device descriptor.
 * @param status     - Status type to read.
 * @param status_val - Pointer to store status value.
 * @return 0 on success, negative error code otherwise.
 *
 * Single-byte statuses: VOUT, IOUT, INPUT, TEMPERATURE, CML.
 * Two-byte status: STATUS_WORD.
 */
int <devname>_read_status(struct <devname>_desc *desc,
			  enum <devname>_status_type status,
			  uint16_t *status_val)
{
	uint8_t read_byte;
	uint8_t data[2];
	int ret;

	switch (status) {
	case <DEVNAME>_STATUS_VOUT_TYPE:
	case <DEVNAME>_STATUS_IOUT_TYPE:
	case <DEVNAME>_STATUS_INPUT_TYPE:
	case <DEVNAME>_STATUS_TEMPERATURE_TYPE:
	case <DEVNAME>_STATUS_CML_TYPE:
		ret = <devname>_read(desc, (uint16_t)status, &read_byte, 1);
		if (ret)
			return ret;

		*status_val = read_byte;

		return 0;
	case <DEVNAME>_STATUS_WORD_TYPE:
		ret = <devname>_read(desc, (uint16_t)status, data, 2);
		if (ret)
			return ret;

		*status_val = no_os_get_unaligned_le16(data);

		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read the output voltage (VS/VOUT) raw value.
 * @param desc   - Device descriptor.
 * @param vsense - Pointer to store VOUT raw value.
 * @return 0 on success, negative error code otherwise.
 *
 * VOUT uses ULINEAR16 format (not Linear11), so the raw 16-bit
 * value is returned directly.
 */
int <devname>_read_vsense(struct <devname>_desc *desc, uint16_t *vsense)
{
	int ret;
	uint8_t data[2];

	ret = <devname>_read(desc, <DEVNAME>_READ_VOUT, data, 2);
	if (ret)
		return ret;

	*vsense = no_os_get_unaligned_le16(data);

	return 0;
}

/**
 * @brief Read a telemetry value in PMBus Linear11 format.
 * @param desc     - Device descriptor.
 * @param mant     - Pointer to store mantissa (11-bit).
 * @param exp      - Pointer to store exponent (5-bit).
 * @param val_type - Telemetry type (VIN, IIN, IOUT, TEMP, etc.).
 * @return 0 on success, negative error code otherwise.
 *
 * Linear11 format: value = mantissa * 2^exponent.
 */
int <devname>_read_value(struct <devname>_desc *desc, uint16_t *mant,
			 uint8_t *exp, enum <devname>_value_type val_type)
{
	int ret;
	uint8_t data[2];

	ret = <devname>_read(desc, val_type, data, 2);
	if (ret)
		return ret;

	*mant = no_os_field_get(<DEVNAME>_MANT_MASK,
				no_os_get_unaligned_le16(data));
	*exp = no_os_field_get(<DEVNAME>_EXP_MASK,
			       no_os_get_unaligned_le16(data));

	return 0;
}

/**
 * @brief Set VOUT_COMMAND and VOUT_MAX values.
 * @param desc         - Device descriptor.
 * @param vout_command - VOUT command value (ULINEAR16).
 * @param vout_max     - VOUT maximum value (ULINEAR16).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_vout_value(struct <devname>_desc *desc, uint16_t vout_command,
			 uint16_t vout_max)
{
	int ret;

	ret = <devname>_write(desc, <DEVNAME>_VOUT_COMMAND, vout_command, 2);
	if (ret)
		return ret;

	return <devname>_write(desc, <DEVNAME>_VOUT_MAX, vout_max, 2);
}

/**
 * @brief Set the PWM output channel and switching frequency.
 * @param desc - Device descriptor.
 * @param chan - Channel to enable (or DISABLE_ALL).
 * @param freq - Switching frequency (device-specific encoding).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_pwm(struct <devname>_desc *desc,
		      enum <devname>_channel chan, uint16_t freq)
{
	int ret;

	ret = <devname>_write(desc, <DEVNAME>_FREQUENCY_SWITCH, freq, 2);
	if (ret)
		return ret;

	desc->freq = freq;

	/* Enable/disable PWM outputs as needed per channel. */
	/* ... device-specific PWM output enable logic ... */

	return 0;
}

/**
 * @brief Set close-loop operation mode.
 * @param desc - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_close_loop(struct <devname>_desc *desc)
{
	/* Disable all PWM outputs, reconfigure for closed loop. */
	/* ... device-specific closed-loop configuration ... */

	desc->loop = <DEVNAME>_CLOSE_LOOP;

	return 0;
}

/**
 * @brief Software-reset the device.
 * @param desc - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_software_reset(struct <devname>_desc *desc)
{
	/* Write to the device-specific software reset register. */
	/* ... device-specific reset sequence ... */

	return 0;
}

/**
 * @brief Initialise the device.
 * @param desc       - Pointer to descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 *
 * Allocation order:
 *   1. Allocate descriptor with no_os_calloc.
 *   2. Initialise I2C/PMBus interface.
 *   3. Configure GPIO pins (power-good, ctrl).
 *   4. Set on/off config and enable operation.
 *   5. Clear faults and set defaults.
 *
 * On error, goto-based cleanup releases resources in reverse order.
 */
int <devname>_init(struct <devname>_desc **desc,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_desc *descriptor;
	int ret;

	descriptor = (struct <devname>_desc *)no_os_calloc(sizeof(*descriptor),
							    1);
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_i2c_init(&descriptor->i2c_desc, init_param->i2c_param);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_get_optional(&descriptor->pg_alt_desc,
				      init_param->pg_alt_param);
	if (ret)
		goto free_i2c;

	if (descriptor->pg_alt_desc) {
		ret = no_os_gpio_direction_input(descriptor->pg_alt_desc);
		if (ret)
			goto free_pg;
	}

	ret = no_os_gpio_get_optional(&descriptor->ctrl_desc,
				      init_param->ctrl_param);
	if (ret)
		goto free_pg;

	/* Configure on/off mode. */
	ret = <devname>_write(descriptor, <DEVNAME>_ON_OFF_CONFIG,
			      (uint16_t)init_param->on_off_config, 1);
	if (ret)
		goto free_ctrl;

	/* Turn on operation. */
	ret = <devname>_write(descriptor, <DEVNAME>_OPERATION,
			      <DEVNAME>_OPERATION_ON, 1);
	if (ret)
		goto free_ctrl;

	/* Wait for power-up. */
	no_os_mdelay(52);

	/* Clear latched faults. */
	ret = <devname>_send_command(descriptor, <DEVNAME>_CLEAR_FAULTS);
	if (ret)
		goto free_ctrl;

	descriptor->loop = <DEVNAME>_CLOSE_LOOP;
	descriptor->freq = 0;

	*desc = descriptor;

	return 0;

free_ctrl:
	no_os_gpio_remove(descriptor->ctrl_desc);
free_pg:
	no_os_gpio_remove(descriptor->pg_alt_desc);
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
 *
 * Turns off the device before releasing I2C and GPIO resources.
 */
int <devname>_remove(struct <devname>_desc *desc)
{
	int ret;

	if (!desc)
		return -ENODEV;

	if (desc->i2c_desc) {
		ret = <devname>_write(desc, <DEVNAME>_OPERATION,
				      <DEVNAME>_OPERATION_OFF, 1);
		if (ret)
			return ret;
	}

	no_os_gpio_remove(desc->ctrl_desc);
	no_os_gpio_remove(desc->pg_alt_desc);
	no_os_i2c_remove(desc->i2c_desc);
	no_os_free(desc);

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

#include <stdbool.h>
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
 * @struct <devname>_iio_desc_init_param
 * @brief  IIO initialization parameters.
 */
struct <devname>_iio_desc_init_param {
	/** Core driver init params. */
	struct <devname>_init_param *<devname>_init_param;
	/** VOUT scale monitor value for IIO scaling. */
	uint16_t vout_scale_monitor;
	/** VIN scale monitor value for IIO scaling. */
	uint16_t vin_scale_monitor;
	/** IIN scale monitor value for IIO scaling. */
	uint16_t iin_scale_monitor;
};

/** @brief Initialise the IIO descriptor. */
int <devname>_iio_init(struct <devname>_iio_desc **desc,
		       struct <devname>_iio_desc_init_param *init_param);

/** @brief Free IIO descriptor resources. */
int <devname>_iio_remove(struct <devname>_iio_desc *desc);

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "<devname>.h"
#include "iio_<devname>.h"

/* ---------- Internal enumerations ---------------------------------- */

enum <devname>_iio_input_chan_type {
	<DEVNAME>_IIO_VIN_CHAN,
	<DEVNAME>_IIO_IIN_CHAN,
	<DEVNAME>_IIO_VOUT_CHAN,
	<DEVNAME>_IIO_TEMP_CHAN
};

enum <devname>_iio_output_chan_type {
	<DEVNAME>_IIO_OUTA_CHAN = 4,
	<DEVNAME>_IIO_OUTB_CHAN = 5,
	<DEVNAME>_IIO_SR1_CHAN = 6,
	<DEVNAME>_IIO_SR2_CHAN = 7
};

/* ---------- Attribute callbacks ------------------------------------ */

/**
 * @brief Read raw telemetry value for an input channel.
 *
 * Returns the PMBus Linear11 raw word for VIN, IIN, TEMP,
 * or the ULINEAR16 raw value for VOUT.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int ret;
	uint16_t mant, val;
	uint8_t exp;
	struct <devname>_iio_desc *iio_desc = dev;
	struct <devname>_desc *<devname> = iio_desc-><devname>_desc;

	switch (channel->address) {
	case <DEVNAME>_IIO_VIN_CHAN:
		ret = <devname>_read_value(<devname>, &mant, &exp,
					   <DEVNAME>_VIN);
		break;
	case <DEVNAME>_IIO_IIN_CHAN:
		ret = <devname>_read_value(<devname>, &mant, &exp,
					   <DEVNAME>_IIN);
		break;
	case <DEVNAME>_IIO_VOUT_CHAN:
		ret = <devname>_read_vsense(<devname>, &mant);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1,
					(int32_t *)&mant);
	case <DEVNAME>_IIO_TEMP_CHAN:
		ret = <devname>_read_value(<devname>, &mant, &exp,
					   <DEVNAME>_TEMP);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	val = no_os_field_get(<DEVNAME>_EXP_MASK, exp) |
	      no_os_field_get(<DEVNAME>_MANT_MASK, mant);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Read scale factor for an input channel.
 *
 * Returns the scale monitor register value to convert raw readings
 * to physical units.
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	/* Return the appropriate scale for the channel type.
	 * For Linear11: value = mantissa * 2^exponent.
	 * For ULINEAR16: value = raw * 2^(VOUT_MODE exponent). */
	int32_t val = 1;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read a status register (debug attribute).
 */
static int <devname>_iio_read_status(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int ret;
	uint16_t status_val;
	struct <devname>_iio_desc *iio_desc = dev;

	ret = <devname>_read_status(iio_desc-><devname>_desc,
				    (enum <devname>_status_type)priv,
				    &status_val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1,
				(int32_t *)&status_val);
}

/* ---------- Attribute tables --------------------------------------- */

static struct iio_attribute <devname>_input_attrs[] = {
	{
		.name = "raw",
		.show = <devname>_iio_read_raw,
	},
	{
		.name = "scale",
		.show = <devname>_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute <devname>_debug_attrs[] = {
	{
		.name = "status_vout",
		.show = <devname>_iio_read_status,
		.priv = <DEVNAME>_STATUS_VOUT_TYPE
	},
	{
		.name = "status_iout",
		.show = <devname>_iio_read_status,
		.priv = <DEVNAME>_STATUS_IOUT_TYPE,
	},
	{
		.name = "status_input",
		.show = <devname>_iio_read_status,
		.priv = <DEVNAME>_STATUS_INPUT_TYPE,
	},
	{
		.name = "status_temperature",
		.show = <devname>_iio_read_status,
		.priv = <DEVNAME>_STATUS_TEMPERATURE_TYPE,
	},
	{
		.name = "status_cml",
		.show = <devname>_iio_read_status,
		.priv = <DEVNAME>_STATUS_CML_TYPE
	},
	{
		.name = "status_word",
		.show = <devname>_iio_read_status,
		.priv = <DEVNAME>_STATUS_WORD_TYPE
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * Input channels: VIN (IIO_VOLTAGE), IIN (IIO_CURRENT),
 *                 VOUT (IIO_VOLTAGE), TEMP (IIO_TEMP).
 *
 * Output channels: PWM outputs (IIO_VOLTAGE, ch_out = true).
 * Output channels expose enable, frequency, duty_cycle attributes
 * (add as needed for the specific device).
 */
static struct iio_channel <devname>_channels[] = {
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = <DEVNAME>_IIO_VIN_CHAN,
		.address = <DEVNAME>_IIO_VIN_CHAN,
		.attributes = <devname>_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = <DEVNAME>_IIO_IIN_CHAN,
		.address = <DEVNAME>_IIO_IIN_CHAN,
		.attributes = <devname>_input_attrs,
		.ch_out = false,
	},
	{
		.name = "vout",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = <DEVNAME>_IIO_VOUT_CHAN,
		.address = <DEVNAME>_IIO_VOUT_CHAN,
		.attributes = <devname>_input_attrs,
		.ch_out = false,
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = <DEVNAME>_IIO_TEMP_CHAN,
		.address = <DEVNAME>_IIO_TEMP_CHAN,
		.attributes = <devname>_input_attrs,
		.ch_out = false,
	},
	{
		.name = "outa",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = <DEVNAME>_IIO_OUTA_CHAN,
		.address = <DEVNAME>_IIO_OUTA_CHAN,
		.ch_out = true,
	},
	{
		.name = "outb",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = <DEVNAME>_IIO_OUTB_CHAN,
		.address = <DEVNAME>_IIO_OUTB_CHAN,
		.ch_out = true,
	},
};

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_channels),
	.channels = <devname>_channels,
	.debug_attributes = <devname>_debug_attrs,
};

/* ---------- Init / Remove ------------------------------------------ */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 * @param desc       - Pointer to IIO descriptor pointer.
 * @param init_param - IIO initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_iio_init(struct <devname>_iio_desc **desc,
		       struct <devname>_iio_desc_init_param *init_param)
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
 * @param desc - IIO descriptor.
 * @return 0 on success, negative error code otherwise.
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

INCS += $(DRIVERS)/power/<devname>/<devname>.h
SRCS += $(DRIVERS)/power/<devname>/<devname>.c
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
INCS += $(DRIVERS)/power/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/power/<devname>/iio_<devname>.c

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
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "<devname>.h"

extern struct no_os_uart_init_param <devname>_uart_ip;
extern struct no_os_i2c_init_param <devname>_i2c_ip;
extern struct no_os_gpio_init_param <devname>_pg_alt_ip;
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

struct no_os_uart_init_param <devname>_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

struct no_os_i2c_init_param <devname>_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 100000,
	.platform_ops = I2C_OPS,
	.slave_address = <DEVNAME>_PMBUS_BASE_ADDRESS,
	.extra = I2C_EXTRA,
};

struct no_os_gpio_init_param <devname>_pg_alt_ip = {
	.port = GPIO_PG_ALT_PORT,
	.number = GPIO_PG_ALT_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct <devname>_init_param <devname>_ip = {
	.i2c_param = &<devname>_i2c_ip,
	.pg_alt_param = &<devname>_pg_alt_ip,
	.ctrl_param = NULL,
	.on_off_config = <DEVNAME>_ON_OFF_DEFAULT_CFG,
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

#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "<devname>.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
 *******************************************************************************/
int basic_example_main()
{
	struct <devname>_desc *desc;
	uint16_t vout, mant;
	uint8_t exp;
	int ret;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &<devname>_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = <devname>_init(&desc, &<devname>_ip);
	if (ret)
		goto exit;

	/* Set closed-loop operation. */
	ret = <devname>_set_close_loop(desc);
	if (ret)
		goto free_dev;

	/* Set output voltage. */
	ret = <devname>_vout_value(desc, 0x3000, 0x5000);
	if (ret)
		goto free_dev;

	/* Read input voltage. */
	ret = <devname>_read_value(desc, &mant, &exp, <DEVNAME>_VIN);
	if (ret)
		goto free_dev;

	pr_info("VIN mantissa: %d, exponent: %d\r\n", mant, exp);

	/* Read output voltage. */
	ret = <devname>_read_vsense(desc, &vout);
	if (ret)
		goto free_dev;

	pr_info("VOUT raw: %d\r\n", vout);

	/* Read input current. */
	ret = <devname>_read_value(desc, &mant, &exp, <DEVNAME>_IIN);
	if (ret)
		goto free_dev;

	pr_info("IIN mantissa: %d, exponent: %d\r\n", mant, exp);

	/* Check statuses. */
	uint16_t status;

	ret = <devname>_read_status(desc, <DEVNAME>_STATUS_VOUT_TYPE, &status);
	if (ret)
		goto free_dev;

	pr_info("STATUS_VOUT: 0x%02X\r\n", status);

	ret = <devname>_read_status(desc, <DEVNAME>_STATUS_INPUT_TYPE, &status);
	if (ret)
		goto free_dev;

	pr_info("STATUS_INPUT: 0x%02X\r\n", status);

free_dev:
	<devname>_remove(desc);
exit:
	if (ret)
		pr_info("Error!\r\n");
	no_os_uart_remove(uart_desc);
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
#include "iio_<devname>.h"

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
	struct <devname>_iio_desc_init_param <devname>_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	<devname>_iio_ip.<devname>_init_param = &<devname>_ip;
	<devname>_iio_ip.vout_scale_monitor = 0;
	<devname>_iio_ip.vin_scale_monitor = 0;
	<devname>_iio_ip.iin_scale_monitor = 0;

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
	app_init_param.uart_init_params = <devname>_uart_ip;

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
	return basic_example_main();
#endif

#ifdef IIO_EXAMPLE
	return iio_example_main();
#endif

#if (IIO_EXAMPLE + BASIC_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
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

#define GPIO_OPS	&max_gpio_ops
#define GPIO_EXTRA	NULL
#define GPIO_PG_ALT_PORT	0
#define GPIO_PG_ALT_PIN	0

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

The <DEVNAME> is a digital power controller with an integrated PMBus
interface for configuring and monitoring a DC-to-DC converter.  It
provides voltage, current, and temperature telemetry via PMBus standard
commands.  <Add 2-3 more sentences from the datasheet covering key
specs: input/output voltage range, switching frequency range, number
of PWM outputs, protection features.>

Applications
------------

* Isolated DC-DC converters
* Telecom and server power supplies
* Industrial power systems
* High-efficiency digital power management
* Active-clamp forward and full-bridge topologies

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor and initialises the I2C/PMBus communication interface,
GPIO pins (power-good, CTRL), and sets the on/off configuration.
After power-up, latched faults are cleared.  Error handling releases
any allocated resources on failure using goto-based cleanup.

Voltage Output Control
~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_vout_value`` function sets VOUT_COMMAND and VOUT_MAX
registers.  Output voltage uses ULINEAR16 format where the exponent
is defined by VOUT_MODE.

Telemetry Reading
~~~~~~~~~~~~~~~~~

The ``<devname>_read_value`` function reads VIN, IIN, IOUT,
temperature, duty cycle, and frequency registers in PMBus Linear11
format.  The ``<devname>_read_vsense`` function reads the output
voltage in ULINEAR16 format.

Status Monitoring
~~~~~~~~~~~~~~~~~

The ``<devname>_read_status`` function reads fault and warning status
registers (STATUS_VOUT, STATUS_IOUT, STATUS_INPUT, STATUS_TEMPERATURE,
STATUS_CML, STATUS_WORD) for real-time fault diagnosis.

PWM Control
~~~~~~~~~~~

The ``<devname>_set_pwm`` function configures the switching frequency
and enables/disables individual PWM output channels.  The device
supports both open-loop and closed-loop operation modes.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_i2c.h"
   #include "no_os_gpio.h"

   int main()
   {
       struct no_os_i2c_init_param i2c_ip = {
           .device_id     = 1,
           .max_speed_hz  = 100000,
           .slave_address = <DEVNAME>_PMBUS_BASE_ADDRESS,
           .platform_ops  = &max_i2c_ops,
           .extra         = &i2c_extra,
       };

       struct <devname>_init_param ip = {
           .i2c_param     = &i2c_ip,
           .pg_alt_param  = NULL,
           .ctrl_param    = NULL,
           .on_off_config = <DEVNAME>_ON_OFF_DEFAULT_CFG,
       };

       struct <devname>_desc *desc;
       uint16_t mant, vout;
       uint8_t exp;
       int ret;

       ret = <devname>_init(&desc, &ip);
       if (ret)
           return ret;

       /* Read input voltage (Linear11 format). */
       ret = <devname>_read_value(desc, &mant, &exp, <DEVNAME>_VIN);
       if (ret)
           goto cleanup;

       /* Read output voltage (ULINEAR16 format). */
       ret = <devname>_read_vsense(desc, &vout);
       if (ret)
           goto cleanup;

   cleanup:
       <devname>_remove(desc);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes input channels for VIN
(``IIO_VOLTAGE``), IIN (``IIO_CURRENT``), VOUT (``IIO_VOLTAGE``), and
temperature (``IIO_TEMP``) with ``raw`` and ``scale`` attributes.
PWM output channels (``IIO_VOLTAGE``, ``ch_out = true``) expose
enable, frequency, and duty cycle attributes.  Debug attributes
provide access to fault status registers.

IIO Initialization Example
---------------------------

.. code-block:: C

   int iio_example_main()
   {
       int ret;
       struct <devname>_iio_desc *iio_desc;
       struct <devname>_iio_desc_init_param iio_ip;
       struct iio_app_desc *app;
       struct iio_app_init_param app_init_param = {0};

       iio_ip.<devname>_init_param = &<devname>_ip;
       iio_ip.vout_scale_monitor = 0;
       iio_ip.vin_scale_monitor = 0;
       iio_ip.iin_scale_monitor = 0;
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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/power/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/power/<devname>/README.rst
```

---

## 9. Key Conventions

1. **`_desc` naming** -- power controller drivers use
   `<devname>_desc` for the device descriptor struct.
2. **PMBus/I2C communication** -- use `no_os_i2c_write()` and
   `no_os_i2c_read()` with the stop-bit parameter:
   - Write command address: `no_os_i2c_write(..., bytes, 0)` (no stop)
   - Read data: `no_os_i2c_read(..., bytes, 1)` (with stop)
   - Write data: `no_os_i2c_write(..., bytes, 1)` (with stop)
3. **Extended commands** -- commands > 0xFF use the 0xFF prefix byte
   (two-byte address).  Standard PMBus commands use a single byte.
4. **Little-endian data** -- PMBus data is little-endian; use
   `no_os_get_unaligned_le16()` for reading and encode LSB-first
   for writing.
5. **PMBus Linear11 format** -- telemetry values (VIN, IIN, IOUT,
   TEMP) use the 5-bit exponent + 11-bit mantissa format:
   `value = mantissa * 2^exponent`.
6. **ULINEAR16 format** -- VOUT uses a 16-bit unsigned value with
   the exponent defined in VOUT_MODE:
   `value = raw * 2^(VOUT_MODE exponent)`.
7. **IIO input channels** -- `IIO_VOLTAGE` for VIN/VOUT,
   `IIO_CURRENT` for IIN/IOUT, `IIO_TEMP` for temperature, all
   with `ch_out = false`.
8. **IIO output channels** -- PWM outputs use `IIO_VOLTAGE` with
   `ch_out = true` and expose enable, frequency, duty_cycle
   attributes.
9. **IIO debug attributes** -- fault status registers (STATUS_VOUT,
   STATUS_IOUT, STATUS_INPUT, STATUS_TEMPERATURE, STATUS_CML,
   STATUS_WORD) are exposed as debug attributes.
10. **Init/remove pattern** -- `init()` allocates with
    `no_os_calloc`, initializes I2C then GPIOs, configures on/off,
    clears faults.  `remove()` turns off the device before
    releasing resources in reverse order.  Goto-based cleanup on
    error.
11. All other conventions (C++ compat, tabs, Doxygen, `no_os_calloc`,
    `no_os_free`, negative error codes, `NO_OS_BIT()`,
    `NO_OS_GENMASK()`, `no_os_field_get()`, `no_os_field_prep()`)
    match the ADC/temperature templates.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> digital power controller with PMBus
interface, voltage/current/temperature monitoring, and configurable
PWM outputs.

Signed-off-by: Your Name <your.name@analog.com>
```
