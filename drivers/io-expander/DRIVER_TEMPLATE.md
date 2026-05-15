# I/O Expander Driver Template

Reference driver: `drivers/io-expander/adp5589/`

This template covers every file needed to add a new I/O expander
driver to no-OS.  Replace `<devname>` with the part number (e.g.,
`adp5589`) and `<DEVNAME>` with its uppercase form (e.g., `ADP5589`)
throughout.

---

## 1. Naming Conventions

I/O expander drivers use `_dev` for the device descriptor struct
(the standard no-OS convention).

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adp5589.h`, `adp5589.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_adp5589.h`, `iio_adp5589.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADP5589_H__` |
| Init param struct | `<devname>_init_param` | `adp5589_init_param` |
| Device struct | `<devname>_dev` | `adp5589_dev` |
| IIO descriptor | `<devname>_iio_dev` | `adp5589_iio_dev` |
| Functions | `<devname>_<verb>()` | `adp5589_init()` |
| Address macro | `<DEVNAME>_I2C_ADDR` | `ADP5589_I2C_ADDR` |
| Register defines | `<DEVNAME>_REG_<NAME>` | `ADP5589_REG_ID` |
| Field masks | `<DEVNAME>_<FIELD>_MASK` | `ADP5589_MAN_ID_MASK` |

---

## 2. File Checklist

```
drivers/io-expander/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
    iio_<devname>.h          # IIO header (optional, uncommon for I/O expanders)
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

doc/sphinx/source/drivers/io-expander/<devname>.rst   # Sphinx entry
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
#include "no_os_i2c.h"
#include "no_os_util.h"

/* ---- I2C Address -------------------------------------------------- */

/**
 * I2C device address.
 * Example: ADP5589 fixed address = 0x34.
 */
#define <DEVNAME>_I2C_ADDR		0x34

/**
 * Manufacturer / device ID for part verification.
 * Example: ADP5589_ID = 0x10.
 */
#define <DEVNAME>_DEVICE_ID		0x10

/* ---- Constants ---------------------------------------------------- */

/** Total number of GPIO pins on the device. */
#define <DEVNAME>_NUM_GPIOS		19

/** Maximum number of keyboard matrix rows. */
#define <DEVNAME>_MAX_ROWS		8

/** Maximum number of keyboard matrix columns. */
#define <DEVNAME>_MAX_COLS		11

/** Number of FIFO event registers. */
#define <DEVNAME>_FIFO_SIZE		16

/* ---- Register Addresses ------------------------------------------- */

#define <DEVNAME>_REG_ID		0x00
#define <DEVNAME>_REG_INT_STATUS	0x01
#define <DEVNAME>_REG_STATUS		0x02
#define <DEVNAME>_REG_FIFO1		0x03

#define <DEVNAME>_REG_GPI_INT_STAT_A	0x13
#define <DEVNAME>_REG_GPI_INT_STAT_B	0x14
#define <DEVNAME>_REG_GPI_INT_STAT_C	0x15
#define <DEVNAME>_REG_GPI_STATUS_A	0x16
#define <DEVNAME>_REG_GPI_STATUS_B	0x17
#define <DEVNAME>_REG_GPI_STATUS_C	0x18

#define <DEVNAME>_REG_RPULL_CFG_A	0x19
#define <DEVNAME>_REG_RPULL_CFG_B	0x1A
#define <DEVNAME>_REG_RPULL_CFG_C	0x1B
#define <DEVNAME>_REG_RPULL_CFG_D	0x1C
#define <DEVNAME>_REG_RPULL_CFG_E	0x1D

#define <DEVNAME>_REG_GPI_INT_LVL_A	0x1E
#define <DEVNAME>_REG_GPI_INT_LVL_B	0x1F
#define <DEVNAME>_REG_GPI_INT_LVL_C	0x20

#define <DEVNAME>_REG_GPI_EVT_EN_A	0x21
#define <DEVNAME>_REG_GPI_EVT_EN_B	0x22
#define <DEVNAME>_REG_GPI_EVT_EN_C	0x23

#define <DEVNAME>_REG_GPI_INT_EN_A	0x24
#define <DEVNAME>_REG_GPI_INT_EN_B	0x25
#define <DEVNAME>_REG_GPI_INT_EN_C	0x26

#define <DEVNAME>_REG_DEBOUNCE_A	0x27
#define <DEVNAME>_REG_DEBOUNCE_B	0x28
#define <DEVNAME>_REG_DEBOUNCE_C	0x29

#define <DEVNAME>_REG_GPO_DATA_A	0x2A
#define <DEVNAME>_REG_GPO_DATA_B	0x2B
#define <DEVNAME>_REG_GPO_DATA_C	0x2C

#define <DEVNAME>_REG_GPO_OUT_MODE_A	0x2D
#define <DEVNAME>_REG_GPO_OUT_MODE_B	0x2E
#define <DEVNAME>_REG_GPO_OUT_MODE_C	0x2F

#define <DEVNAME>_REG_GPIO_DIR_A	0x30
#define <DEVNAME>_REG_GPIO_DIR_B	0x31
#define <DEVNAME>_REG_GPIO_DIR_C	0x32

#define <DEVNAME>_REG_UNLOCK1		0x33
#define <DEVNAME>_REG_UNLOCK2		0x34
#define <DEVNAME>_REG_EXT_LOCK_EVT	0x35
#define <DEVNAME>_REG_UNLOCK_TIMERS	0x36
#define <DEVNAME>_REG_LOCK_CFG		0x37

#define <DEVNAME>_REG_RESET1_EVT_A	0x38
#define <DEVNAME>_REG_RESET1_EVT_B	0x39
#define <DEVNAME>_REG_RESET1_EVT_C	0x3A
#define <DEVNAME>_REG_RESET2_EVT_A	0x3B
#define <DEVNAME>_REG_RESET2_EVT_B	0x3C
#define <DEVNAME>_REG_RESET_CFG		0x3D

#define <DEVNAME>_REG_PWM_OFFT_LOW	0x3E
#define <DEVNAME>_REG_PWM_OFFT_HIGH	0x3F
#define <DEVNAME>_REG_PWM_ONT_LOW	0x40
#define <DEVNAME>_REG_PWM_ONT_HIGH	0x41
#define <DEVNAME>_REG_PWM_CFG		0x42
#define <DEVNAME>_REG_CLK_DIV_CFG	0x43

#define <DEVNAME>_REG_LOGIC1_CFG	0x44
#define <DEVNAME>_REG_LOGIC2_CFG	0x45
#define <DEVNAME>_REG_LOGIC_FF_CFG	0x46
#define <DEVNAME>_REG_LOGIC_INT_EVT	0x47
#define <DEVNAME>_REG_POLL_TIME_CFG	0x48

#define <DEVNAME>_REG_PIN_CFG_A		0x49
#define <DEVNAME>_REG_PIN_CFG_B		0x4A
#define <DEVNAME>_REG_PIN_CFG_C		0x4B
#define <DEVNAME>_REG_PIN_CFG_D		0x4C
#define <DEVNAME>_REG_GENERAL_CFG	0x4D
#define <DEVNAME>_REG_INT_EN		0x4E

/* ---- Register Field Masks ----------------------------------------- */

/* ID Register (0x00) */
#define <DEVNAME>_MAN_ID_MASK		NO_OS_GENMASK(7, 4)
#define <DEVNAME>_REV_ID_MASK		NO_OS_GENMASK(3, 0)

/* INT_STATUS Register (0x01) */
#define <DEVNAME>_EVENT_INT_MASK	NO_OS_BIT(0)
#define <DEVNAME>_GPI_INT_MASK		NO_OS_BIT(1)
#define <DEVNAME>_OVERFLOW_INT_MASK	NO_OS_BIT(2)
#define <DEVNAME>_LOCK_INT_MASK		NO_OS_BIT(3)
#define <DEVNAME>_LOGIC1_INT_MASK	NO_OS_BIT(4)
#define <DEVNAME>_LOGIC2_INT_MASK	NO_OS_BIT(5)

/* STATUS Register (0x02) */
#define <DEVNAME>_EC_MASK		NO_OS_GENMASK(4, 0)
#define <DEVNAME>_LOCK_STAT_MASK	NO_OS_BIT(5)
#define <DEVNAME>_LOGIC1_STAT_MASK	NO_OS_BIT(6)
#define <DEVNAME>_LOGIC2_STAT_MASK	NO_OS_BIT(7)

/* GENERAL_CFG Register (0x4D) */
#define <DEVNAME>_RST_CFG_MASK		NO_OS_BIT(0)
#define <DEVNAME>_INT_CFG_MASK		NO_OS_BIT(1)
#define <DEVNAME>_LCK_TRK_GPI_MASK	NO_OS_BIT(3)
#define <DEVNAME>_LCK_TRK_LOGIC_MASK	NO_OS_BIT(4)
#define <DEVNAME>_CORE_FREQ_MASK	NO_OS_GENMASK(6, 5)
#define <DEVNAME>_OSC_EN_MASK		NO_OS_BIT(7)

/* PWM_CFG Register (0x42) */
#define <DEVNAME>_PWM_EN_MASK		NO_OS_BIT(0)
#define <DEVNAME>_PWM_MODE_MASK		NO_OS_BIT(1)
#define <DEVNAME>_PWM_IN_AND_MASK	NO_OS_BIT(2)

/* PIN_CONFIG_D Register (0x4C) */
#define <DEVNAME>_R0_EXTEND_MASK	NO_OS_BIT(0)
#define <DEVNAME>_C9_EXTEND_MASK	NO_OS_BIT(1)
#define <DEVNAME>_R3_EXTEND_MASK	NO_OS_GENMASK(3, 2)
#define <DEVNAME>_C6_EXTEND_MASK	NO_OS_BIT(4)
#define <DEVNAME>_R4_EXTEND_MASK	NO_OS_BIT(5)
#define <DEVNAME>_C4_EXTEND_MASK	NO_OS_BIT(6)
#define <DEVNAME>_PULL_SELECT_MASK	NO_OS_BIT(7)

/* INT_EN Register (0x4E) */
#define <DEVNAME>_EVENT_IEN_MASK	NO_OS_BIT(0)
#define <DEVNAME>_GPI_IEN_MASK		NO_OS_BIT(1)
#define <DEVNAME>_OVERFLOW_IEN_MASK	NO_OS_BIT(2)
#define <DEVNAME>_LOCK_IEN_MASK		NO_OS_BIT(3)
#define <DEVNAME>_LOGIC1_IEN_MASK	NO_OS_BIT(4)
#define <DEVNAME>_LOGIC2_IEN_MASK	NO_OS_BIT(5)

/* ---- Enumerations ------------------------------------------------- */

/**
 * @enum <devname>_gpio_dir
 * @brief GPIO direction setting.
 */
enum <devname>_gpio_dir {
	<DEVNAME>_GPIO_INPUT = 0,
	<DEVNAME>_GPIO_OUTPUT,
};

/**
 * @enum <devname>_out_mode
 * @brief GPO output drive mode.
 */
enum <devname>_out_mode {
	<DEVNAME>_OUT_OPEN_DRAIN = 0,
	<DEVNAME>_OUT_PUSH_PULL,
};

/**
 * @enum <devname>_core_freq
 * @brief Internal oscillator core frequency.
 */
enum <devname>_core_freq {
	<DEVNAME>_CORE_FREQ_100KHZ = 0,
	<DEVNAME>_CORE_FREQ_200KHZ,
	<DEVNAME>_CORE_FREQ_300KHZ,
	<DEVNAME>_CORE_FREQ_500KHZ,
};

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters for <devname>_init().
 */
struct <devname>_init_param {
	/** I2C initialization parameters. */
	struct no_os_i2c_init_param	i2c_init;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** I2C descriptor. */
	struct no_os_i2c_desc	*i2c_desc;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Initialise the device and verify the device ID. */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Read a single register. */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *val);

/** @brief Write a single register. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t val);

/** @brief Update selected bits of a register. */
int <devname>_reg_update(struct <devname>_dev *dev, uint8_t addr,
			 uint8_t mask, uint8_t val);

/** @brief Set the direction of a GPIO pin. */
int <devname>_gpio_set_dir(struct <devname>_dev *dev, uint8_t pin,
			   enum <devname>_gpio_dir dir);

/** @brief Read the state of a GPIO input pin. */
int <devname>_gpio_get(struct <devname>_dev *dev, uint8_t pin,
		       uint8_t *val);

/** @brief Set the output state of a GPIO pin. */
int <devname>_gpio_set(struct <devname>_dev *dev, uint8_t pin,
		       uint8_t val);

/** @brief Initialise the PWM generator. */
int <devname>_pwm_init(struct <devname>_dev *dev);

/** @brief Set PWM on and off times. */
int <devname>_pwm_set(struct <devname>_dev *dev, uint16_t off_time,
		      uint16_t on_time);

/** @brief Configure keyboard matrix scanning. */
int <devname>_keypad_init(struct <devname>_dev *dev, uint8_t rows,
			  uint8_t cols);

/** @brief Read a key event from the FIFO. */
int <devname>_keypad_read(struct <devname>_dev *dev, uint8_t *key,
			  bool *pressed);

/** @brief Read the interrupt status register and clear it. */
int <devname>_get_int_status(struct <devname>_dev *dev, uint8_t *status);

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
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

/**
 * @brief Read a single register over I2C.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 *
 * I/O expander registers are 8-bit.  The I2C sequence is:
 *   1. Write register address (no stop).
 *   2. Read 1 byte (with stop).
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint8_t *val)
{
	int ret;

	ret = no_os_i2c_write(dev->i2c_desc, &addr, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, val, 1, 1);
}

/**
 * @brief Write a single register over I2C.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint8_t val)
{
	uint8_t buf[2];

	buf[0] = addr;
	buf[1] = val;

	return no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
}

/**
 * @brief Update selected bits of a register (read-modify-write).
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param mask - Bitmask of bits to modify.
 * @param val  - New value for the masked bits.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_update(struct <devname>_dev *dev, uint8_t addr,
			 uint8_t mask, uint8_t val)
{
	uint8_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= no_os_field_prep(mask, val);

	return <devname>_reg_write(dev, addr, reg_val);
}

/**
 * @brief Set the direction (input/output) of a GPIO pin.
 * @param dev - Device descriptor.
 * @param pin - Pin number (0 to <DEVNAME>_NUM_GPIOS - 1).
 * @param dir - Direction enum (input or output).
 * @return 0 on success, negative error code otherwise.
 *
 * GPIO direction is spread across three 8-bit registers
 * (GPIO_DIR_A, GPIO_DIR_B, GPIO_DIR_C).  Each bit corresponds
 * to one pin: 1 = output, 0 = input.
 */
int <devname>_gpio_set_dir(struct <devname>_dev *dev, uint8_t pin,
			   enum <devname>_gpio_dir dir)
{
	uint8_t reg;
	uint8_t bit_mask;

	if (pin >= <DEVNAME>_NUM_GPIOS)
		return -EINVAL;

	reg = <DEVNAME>_REG_GPIO_DIR_A + (pin / 8);
	bit_mask = NO_OS_BIT(pin % 8);

	return <devname>_reg_update(dev, reg, bit_mask,
				    dir == <DEVNAME>_GPIO_OUTPUT ? 1 : 0);
}

/**
 * @brief Read the state of a GPIO input pin.
 * @param dev - Device descriptor.
 * @param pin - Pin number (0 to <DEVNAME>_NUM_GPIOS - 1).
 * @param val - Pointer to store pin state (0 or 1).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_gpio_get(struct <devname>_dev *dev, uint8_t pin,
		       uint8_t *val)
{
	uint8_t reg;
	uint8_t reg_val;
	int ret;

	if (pin >= <DEVNAME>_NUM_GPIOS)
		return -EINVAL;

	reg = <DEVNAME>_REG_GPI_STATUS_A + (pin / 8);

	ret = <devname>_reg_read(dev, reg, &reg_val);
	if (ret)
		return ret;

	*val = !!(reg_val & NO_OS_BIT(pin % 8));

	return 0;
}

/**
 * @brief Set the output state of a GPIO pin.
 * @param dev - Device descriptor.
 * @param pin - Pin number (0 to <DEVNAME>_NUM_GPIOS - 1).
 * @param val - Output value (0 or 1).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_gpio_set(struct <devname>_dev *dev, uint8_t pin,
		       uint8_t val)
{
	uint8_t reg;
	uint8_t bit_mask;

	if (pin >= <DEVNAME>_NUM_GPIOS)
		return -EINVAL;

	reg = <DEVNAME>_REG_GPO_DATA_A + (pin / 8);
	bit_mask = NO_OS_BIT(pin % 8);

	return <devname>_reg_update(dev, reg, bit_mask, val ? 1 : 0);
}

/**
 * @brief Initialise the PWM generator in continuous mode.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 *
 * Enables the PWM output on the designated pin (R3 extended)
 * and sets the PWM to continuous mode.
 */
int <devname>_pwm_init(struct <devname>_dev *dev)
{
	int ret;

	/* Configure R3 pin for PWM output. */
	ret = <devname>_reg_update(dev, <DEVNAME>_REG_PIN_CFG_D,
				   <DEVNAME>_R3_EXTEND_MASK, 0x02);
	if (ret)
		return ret;

	/* Enable PWM in continuous mode. */
	return <devname>_reg_write(dev, <DEVNAME>_REG_PWM_CFG,
				   <DEVNAME>_PWM_EN_MASK);
}

/**
 * @brief Set PWM on-time and off-time.
 * @param dev      - Device descriptor.
 * @param off_time - Off-time in microseconds.
 * @param on_time  - On-time in microseconds.
 * @return 0 on success, negative error code otherwise.
 *
 * PWM times are stored as 16-bit values split across two 8-bit
 * registers each (LOW and HIGH bytes).
 */
int <devname>_pwm_set(struct <devname>_dev *dev, uint16_t off_time,
		      uint16_t on_time)
{
	int ret;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_PWM_OFFT_HIGH,
				  (uint8_t)(off_time >> 8));
	if (ret)
		return ret;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_PWM_OFFT_LOW,
				  (uint8_t)(off_time & 0xFF));
	if (ret)
		return ret;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_PWM_ONT_HIGH,
				  (uint8_t)(on_time >> 8));
	if (ret)
		return ret;

	return <devname>_reg_write(dev, <DEVNAME>_REG_PWM_ONT_LOW,
				   (uint8_t)(on_time & 0xFF));
}

/**
 * @brief Configure keyboard matrix scanning.
 * @param dev  - Device descriptor.
 * @param rows - Bitmask of rows to enable in PIN_CONFIG_A.
 * @param cols - Bitmask of columns to enable in PIN_CONFIG_B.
 * @return 0 on success, negative error code otherwise.
 *
 * Rows and columns that are not used for the keypad matrix
 * can be configured as GPIOs instead.
 */
int <devname>_keypad_init(struct <devname>_dev *dev, uint8_t rows,
			  uint8_t cols)
{
	int ret;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_PIN_CFG_A, rows);
	if (ret)
		return ret;

	return <devname>_reg_write(dev, <DEVNAME>_REG_PIN_CFG_B, cols);
}

/**
 * @brief Read a key event from the FIFO.
 * @param dev     - Device descriptor.
 * @param key     - Pointer to store key event identifier.
 * @param pressed - Pointer to store press/release state.
 * @return 0 on success, negative error code otherwise.
 *
 * The FIFO holds up to 16 key events.  Bit 7 of the FIFO
 * register indicates pressed (1) or released (0).  Bits [6:0]
 * encode the key identifier.
 */
int <devname>_keypad_read(struct <devname>_dev *dev, uint8_t *key,
			  bool *pressed)
{
	uint8_t fifo_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_FIFO1, &fifo_val);
	if (ret)
		return ret;

	*key = fifo_val & 0x7F;
	*pressed = !!(fifo_val & NO_OS_BIT(7));

	return 0;
}

/**
 * @brief Read and clear the interrupt status register.
 * @param dev    - Device descriptor.
 * @param status - Pointer to store interrupt status bits.
 * @return 0 on success, negative error code otherwise.
 *
 * Reading INT_STATUS clears the latched interrupt bits.
 */
int <devname>_get_int_status(struct <devname>_dev *dev, uint8_t *status)
{
	return <devname>_reg_read(dev, <DEVNAME>_REG_INT_STATUS, status);
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param device     - Pointer to descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 *
 * Allocates the device descriptor, initialises I2C, verifies the
 * manufacturer ID, and enables the internal oscillator.
 */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *dev;
	uint8_t id;
	int ret;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (ret)
		goto free_dev;

	/* Verify device ID. */
	ret = <devname>_reg_read(dev, <DEVNAME>_REG_ID, &id);
	if (ret)
		goto free_i2c;

	if (no_os_field_get(<DEVNAME>_MAN_ID_MASK, id) !=
	    <DEVNAME>_DEVICE_ID) {
		ret = -ENODEV;
		goto free_i2c;
	}

	/* Enable internal oscillator at 500 kHz. */
	ret = <devname>_reg_write(dev, <DEVNAME>_REG_GENERAL_CFG,
				  <DEVNAME>_OSC_EN_MASK |
				  no_os_field_prep(<DEVNAME>_CORE_FREQ_MASK,
						   <DEVNAME>_CORE_FREQ_500KHZ));
	if (ret)
		goto free_i2c;

	*device = dev;

	return 0;

free_i2c:
	no_os_i2c_remove(dev->i2c_desc);
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

> **Note:** IIO support is not typical for I/O expander drivers since they
> do not produce continuous sampled data.  If your device has ADC inputs or
> sensor channels that benefit from IIO, provide this file.  Otherwise, this
> section may be omitted.

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
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_init_param *init_param);

/** @brief Free IIO descriptor resources. */
int <devname>_iio_remove(struct <devname>_iio_dev *iio_dev);

#endif /* __IIO_<DEVNAME>_H__ */
```

---

## 6. IIO Source (`iio_<devname>.c`)

> **Note:** Since I/O expanders are not data-acquisition devices, IIO
> support is minimal.  The skeleton below exposes GPIO pin states as
> IIO voltage channels.  Omit this file entirely if IIO is not needed.

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
 * @brief Read raw GPIO pin state (0 or 1).
 */
static int <devname>_iio_read_raw(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_dev *iio_dev = device;
	uint8_t val;
	int32_t ival;
	int ret;

	ret = <devname>_gpio_get(iio_dev->dev, channel->ch_num, &val);
	if (ret)
		return ret;

	ival = val;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &ival);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_attrs[] = {
	{ .name = "raw", .show = <devname>_iio_read_raw },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel definition ------------------------------------- */

/**
 * GPIO channels: IIO_VOLTAGE, ch_out = false (input), one per pin.
 * Dynamically populate based on number of GPIOs if needed.
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "gpio0",
		.ch_type = IIO_VOLTAGE,
		.ch_out = 0,
		.ch_num = 0,
		.attributes = <devname>_iio_attrs,
	},
};

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_device = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels),
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

	ret = <devname>_init(&descriptor->dev, init_param->init_param);
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
int <devname>_iio_remove(struct <devname>_iio_dev *iio_dev)
{
	int ret;

	ret = <devname>_remove(iio_dev->dev);
	if (ret)
		return ret;

	no_os_free(iio_dev);

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

INCS += $(DRIVERS)/io-expander/<devname>/<devname>.h
SRCS += $(DRIVERS)/io-expander/<devname>/<devname>.c
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
INCS += $(DRIVERS)/io-expander/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/io-expander/<devname>/iio_<devname>.c

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
		.slave_address = <DEVNAME>_I2C_ADDR,
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA,
	},
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
	uint8_t pin_state;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	/* Configure pin 0 as output and pin 1 as input. */
	ret = <devname>_gpio_set_dir(dev, 0, <DEVNAME>_GPIO_OUTPUT);
	if (ret)
		goto free_dev;

	ret = <devname>_gpio_set_dir(dev, 1, <DEVNAME>_GPIO_INPUT);
	if (ret)
		goto free_dev;

	while (1) {
		/* Toggle output pin. */
		ret = <devname>_gpio_set(dev, 0, 1);
		if (ret)
			goto free_dev;
		no_os_mdelay(500);

		ret = <devname>_gpio_set(dev, 0, 0);
		if (ret)
			goto free_dev;
		no_os_mdelay(500);

		/* Read input pin. */
		ret = <devname>_gpio_get(dev, 1, &pin_state);
		if (ret)
			goto free_dev;

		pr_info("Pin 1 state: %d\r\n", pin_state);
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

The <DEVNAME> is a configurable I/O expander with up to <n> GPIO pins,
a keyboard matrix scanner, PWM generator, and programmable logic
blocks.  It communicates via I2C and supports interrupt-driven event
detection.  <Add 2-3 more sentences from the datasheet covering key
specs: number of GPIOs, supply voltage range, key matrix size, built-in
pull-up/pull-down resistors, debounce circuitry.>

Applications
------------

* Industrial I/O expansion
* Keyboard / keypad scanning
* LED dimming (via PWM)
* System reset generation
* Remote GPIO control

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the I2C communication interface, verifies the
manufacturer ID, and enables the internal oscillator.  Error handling
releases any allocated resources on failure (goto-based cleanup).

GPIO Configuration
~~~~~~~~~~~~~~~~~~

Each pin can be configured as input or output using
``<devname>_gpio_set_dir``.  GPIO direction is spread across three
8-bit registers (GPIO_DIR_A/B/C).  Outputs can be set or cleared
with ``<devname>_gpio_set``, and inputs read with
``<devname>_gpio_get``.

PWM Output
~~~~~~~~~~

The ``<devname>_pwm_init`` function enables the PWM generator on the
designated pin.  ``<devname>_pwm_set`` configures on-time and off-time
as 16-bit microsecond values.

Keyboard Matrix
~~~~~~~~~~~~~~~

``<devname>_keypad_init`` configures selected rows and columns for
matrix scanning.  Key events are stored in a 16-entry FIFO and read
with ``<devname>_keypad_read``, which returns both the key identifier
and press/release state.

Interrupt Handling
~~~~~~~~~~~~~~~~~~

``<devname>_get_int_status`` reads and clears the interrupt status
register, returning a bitmask of event, GPI, overflow, lock, and
logic interrupt flags.

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
               .slave_address = <DEVNAME>_I2C_ADDR,
               .platform_ops  = &max_i2c_ops,
               .extra         = &i2c_extra,
           },
       };

       struct <devname>_dev *dev;
       uint8_t pin_val;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_gpio_set_dir(dev, 0, <DEVNAME>_GPIO_OUTPUT);
       if (ret)
           goto cleanup;

       ret = <devname>_gpio_set(dev, 0, 1);
       if (ret)
           goto cleanup;

       ret = <devname>_gpio_get(dev, 1, &pin_val);
       if (ret)
           goto cleanup;

       /* pin_val holds the GPIO input state */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/io-expander/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/io-expander/<devname>/README.rst
```

---

## 9. Key Conventions

1. **`_dev` not `_desc`** -- I/O expander drivers use `<devname>_dev`
   for the device descriptor struct (the standard no-OS convention).
2. **I2C communication** -- 8-bit registers with single-byte
   read/write.  Use `no_os_i2c_write()` and `no_os_i2c_read()` with
   the stop-bit parameter:
   - Write register address: `no_os_i2c_write(..., 1, 0)` (no stop)
   - Read data: `no_os_i2c_read(..., 1, 1)` (with stop)
   - Write data: `no_os_i2c_write(..., 2, 1)` (with stop)
3. **Read-modify-write** -- use a `reg_update()` helper for masked
   register writes: read the register, clear the mask bits, OR in
   the new value via `no_os_field_prep()`.
4. **Bit manipulation** -- use `NO_OS_BIT()` for single-bit masks
   and `NO_OS_GENMASK()` for multi-bit fields.  Use
   `no_os_field_get()` and `no_os_field_prep()` for field access.
5. **GPIO register banks** -- GPIO pins are typically spread across
   three 8-bit registers (A, B, C).  Compute the register and bit
   position from the pin number: `reg = base + (pin / 8)`,
   `bit = NO_OS_BIT(pin % 8)`.
6. **C++ compatibility** -- all `void *` allocations must use
   explicit casts: `(struct <devname>_dev *)no_os_calloc(...)`.
7. **Memory allocation** -- use `no_os_calloc()` (not `malloc`)
   for descriptor allocation and `no_os_free()` for deallocation.
8. **Init pattern** -- `init()` takes `**device` + `*init_param`,
   allocates via `no_os_calloc`, uses goto-based cleanup on error,
   and assigns `*device = dev` only on success.
9. **IIO is optional** -- I/O expanders are not data-acquisition
   devices.  IIO support is provided only when GPIO states or other
   readings benefit from the IIO framework.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> I/O expander with <n>-pin GPIO,
keyboard matrix scanner, and PWM output over I2C interface.

Signed-off-by: Your Name <your.name@analog.com>
```
