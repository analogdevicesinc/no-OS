# LED Driver Template

Reference driver: `drivers/led/ltc3208/`

This template covers every file needed to add a new LED driver to no-OS.
Replace `<devname>` with the part number (e.g., `ltc3208`) and `<DEVNAME>`
with its uppercase form (e.g., `LTC3208`) throughout.

---

## 1. Naming Conventions

LED drivers use `_dev` for the device descriptor struct.

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ltc3208.h`, `ltc3208.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ltc3208.h`, `iio_ltc3208.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __LTC3208_H__` |
| Init param struct | `<devname>_init_param` | `ltc3208_init_param` |
| Device struct | `<devname>_dev` | `ltc3208_dev` |
| IIO descriptor | `<devname>_iio_desc` | `ltc3208_iio_desc` |
| Functions | `<devname>_<verb>()` | `ltc3208_init()` |
| Register defines | `<DEVNAME>_REG_<NAME>` | `LTC3208_REG_MAIN` |
| Field masks | `<DEVNAME>_<FIELD>_MASK` | `LTC3208_4_BIT_HIGH_MASK` |
| Bit defines | `<DEVNAME>_<NAME>_BIT` | `LTC3208_CPO_2X_BIT` |
| Enumerations | `<devname>_<category>` | `ltc3208_dac_options` |

---

## 2. File Checklist

```
drivers/led/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
    iio_<devname>.h          # IIO header (not typical for LED drivers)
    iio_<devname>.c          # IIO implementation (not typical for LED drivers)
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
            examples_src.mk
        platform/
            platform_includes.h
            <platform>/
                main.c
                parameters.h
                parameters.c
                platform_src.mk

doc/sphinx/source/drivers/led/<devname>.rst   # Sphinx entry
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __<DEVNAME>_H__
#define __<DEVNAME>_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/* ---- Register Addresses ------------------------------------------- */

#define <DEVNAME>_REG_LED_CTRL		0x01
#define <DEVNAME>_REG_BRIGHTNESS	0x02
#define <DEVNAME>_REG_CONFIG		0x03

/* ---- Field Masks -------------------------------------------------- */

#define <DEVNAME>_CURRENT_MASK		NO_OS_GENMASK(7, 0)
#define <DEVNAME>_BRIGHT_HIGH_MASK	NO_OS_GENMASK(7, 4)
#define <DEVNAME>_BRIGHT_LOW_MASK	NO_OS_GENMASK(3, 0)

/* ---- Bit Defines -------------------------------------------------- */

#define <DEVNAME>_ENABLE_BIT		NO_OS_BIT(0)
#define <DEVNAME>_SHUTDOWN_BIT		NO_OS_BIT(7)

/* ---- Constants ---------------------------------------------------- */

/** Number of LED channels. */
#define <DEVNAME>_NUM_CHANNELS		4

/** Maximum current level (e.g., 256 steps for 8-bit, 16 steps for 4-bit). */
#define <DEVNAME>_MAX_CURRENT_LEVEL	255

/* ---- Enumerations ------------------------------------------------- */

/**
 * @enum <devname>_channel
 * @brief LED channel selection.
 */
enum <devname>_channel {
	<DEVNAME>_CH_0 = 0,
	<DEVNAME>_CH_1,
	<DEVNAME>_CH_2,
	<DEVNAME>_CH_3,
};

/* ---- Configuration Structure -------------------------------------- */

/**
 * @struct <devname>_config
 * @brief  Device configuration state (cached in device descriptor).
 */
struct <devname>_config {
	/** Per-channel enable state. */
	bool ch_enabled[<DEVNAME>_NUM_CHANNELS];
	/** Per-channel current level. */
	uint8_t ch_current[<DEVNAME>_NUM_CHANNELS];
};

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters for <devname>_init().
 */
struct <devname>_init_param {
	/** I2C initialization parameters. */
	struct no_os_i2c_init_param i2c_init_param;
	/** GPIO initialization parameters (e.g., enable pin). */
	struct no_os_gpio_init_param gpio_init_param;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** I2C descriptor. */
	struct no_os_i2c_desc *i2c_desc;
	/** GPIO descriptor (e.g., enable pin). */
	struct no_os_gpio_desc *gpio_en_desc;
	/** Cached device configuration. */
	struct <devname>_config config;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Write a register over I2C. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t reg_addr,
			uint8_t reg_data);

/** @brief Reset all registers to defaults. */
int <devname>_reset(struct <devname>_dev *dev);

/** @brief Set LED current level for a channel. */
int <devname>_set_current(struct <devname>_dev *dev,
			  enum <devname>_channel ch,
			  uint8_t current_level);

/** @brief Enable or disable a specific LED channel. */
int <devname>_set_channel_enable(struct <devname>_dev *dev,
				 enum <devname>_channel ch,
				 bool enable);

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

#endif /* __<DEVNAME>_H__ */

#ifdef __cplusplus
}
#endif
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
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_error.h"

/**
 * @brief Write a register over I2C.
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param reg_data - Data to write (8-bit).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t reg_addr,
			uint8_t reg_data)
{
	uint8_t buf[2];

	buf[0] = reg_addr;
	buf[1] = reg_data;

	return no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
}

/**
 * @brief Reset all registers to their default values.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reset(struct <devname>_dev *dev)
{
	int ret;
	uint8_t i;

	for (i = 0; i < <DEVNAME>_NUM_CHANNELS; i++) {
		ret = <devname>_set_current(dev, i, 0);
		if (ret)
			return ret;

		dev->config.ch_enabled[i] = false;
		dev->config.ch_current[i] = 0;
	}

	return 0;
}

/**
 * @brief Set LED current level for a specific channel.
 * @param dev           - Device descriptor.
 * @param ch            - Channel to configure.
 * @param current_level - Current level (0 to <DEVNAME>_MAX_CURRENT_LEVEL).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_current(struct <devname>_dev *dev,
			  enum <devname>_channel ch,
			  uint8_t current_level)
{
	uint8_t data;
	int ret;

	if (ch >= <DEVNAME>_NUM_CHANNELS)
		return -EINVAL;

	data = no_os_field_prep(<DEVNAME>_CURRENT_MASK, current_level);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_LED_CTRL + ch, data);
	if (ret)
		return ret;

	dev->config.ch_current[ch] = current_level;

	return 0;
}

/**
 * @brief Enable or disable a specific LED channel.
 * @param dev    - Device descriptor.
 * @param ch     - Channel to enable/disable.
 * @param enable - true to enable, false to disable.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_channel_enable(struct <devname>_dev *dev,
				 enum <devname>_channel ch,
				 bool enable)
{
	int ret;

	if (ch >= <DEVNAME>_NUM_CHANNELS)
		return -EINVAL;

	/* Setting current to 0 effectively disables the channel. */
	if (enable)
		ret = <devname>_set_current(dev, ch,
					    dev->config.ch_current[ch]);
	else
		ret = <devname>_set_current(dev, ch, 0);
	if (ret)
		return ret;

	dev->config.ch_enabled[ch] = enable;

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
	struct <devname>_dev *descriptor;
	int ret;

	descriptor = (struct <devname>_dev *)no_os_calloc(1,
							  sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_i2c_init(&descriptor->i2c_desc,
			     &init_param->i2c_init_param);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_get(&descriptor->gpio_en_desc,
			     &init_param->gpio_init_param);
	if (ret)
		goto free_i2c;

	ret = no_os_gpio_direction_output(descriptor->gpio_en_desc,
					  NO_OS_GPIO_HIGH);
	if (ret)
		goto free_gpio;

	*dev = descriptor;

	return 0;

free_gpio:
	no_os_gpio_remove(descriptor->gpio_en_desc);
free_i2c:
	no_os_i2c_remove(descriptor->i2c_desc);
free_desc:
	no_os_free(descriptor);

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

	if (dev->i2c_desc) {
		ret = no_os_i2c_remove(dev->i2c_desc);
		if (ret)
			return ret;
	}

	if (dev->gpio_en_desc) {
		ret = no_os_gpio_remove(dev->gpio_en_desc);
		if (ret)
			return ret;
	}

	no_os_free(dev);

	return 0;
}
```

---

## 5. IIO Header (`iio_<devname>.h`)

> **Note:** IIO support is not typical for LED drivers.  Most LED drivers
> only provide a basic example (no IIO channels).  If your device benefits
> from IIO integration (e.g., for current monitoring or brightness
> control via libiio), the minimal skeleton below can be used.

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

> **Note:** LED drivers typically do not require IIO support.  None of the
> existing LED drivers (`ltc3208`, `ltc3220`, `max25603`) implement IIO.
> If IIO is needed, follow the pattern from other driver categories
> (e.g., `drivers/dac/` or `drivers/temperature/`) and adapt the channel
> type to `IIO_CURRENT` with `ch_out = 1` (output) for LED current
> control.

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
 * @brief Read raw current level for a channel.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int32_t val;

	val = desc-><devname>_dev->config.ch_current[channel->ch_num];

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write raw current level for a channel.
 */
static int <devname>_iio_write_raw(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return <devname>_set_current(desc-><devname>_dev,
				     channel->ch_num, (uint8_t)val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_attrs[] = {
	{ .name = "raw", .show = <devname>_iio_read_raw,
	  .store = <devname>_iio_write_raw },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel definitions ------------------------------------ */

static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "current0",
		.ch_type = IIO_CURRENT,
		.ch_out = 1,
		.indexed = true,
		.channel = 0,
		.attributes = <devname>_iio_attrs,
	},
	/* Add more channels as needed. */
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

	ret = <devname>_init(&descriptor-><devname>_dev,
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
EXAMPLE ?= basic

PLATFORM ?= maxim

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
```

### 7.2 `projects/<project_name>/builds.json`

```json
{
  "<platform>": {
    "basic_example_<target>": {
      "flags" : "EXAMPLE=basic TARGET=<target>"
    }
  }
}
```

### 7.3 `projects/<project_name>/src.mk`

```makefile
INCS += $(INCLUDE)/no_os_error.h	\
	$(INCLUDE)/no_os_gpio.h		\
	$(INCLUDE)/no_os_irq.h		\
	$(INCLUDE)/no_os_list.h		\
	$(INCLUDE)/no_os_uart.h		\
	$(INCLUDE)/no_os_lf256fifo.h	\
	$(INCLUDE)/no_os_dma.h		\
	$(INCLUDE)/no_os_init.h		\
	$(INCLUDE)/no_os_util.h		\
	$(INCLUDE)/no_os_i2c.h		\
	$(INCLUDE)/no_os_mutex.h	\
	$(INCLUDE)/no_os_print_log.h	\
	$(INCLUDE)/no_os_alloc.h	\
	$(INCLUDE)/no_os_delay.h

SRCS += $(DRIVERS)/api/no_os_gpio.c	\
	$(DRIVERS)/api/no_os_irq.c	\
	$(NO-OS)/util/no_os_list.c	\
	$(DRIVERS)/api/no_os_uart.c	\
	$(NO-OS)/util/no_os_lf256fifo.c	\
	$(DRIVERS)/api/no_os_dma.c	\
	$(DRIVERS)/api/no_os_i2c.c	\
	$(NO-OS)/util/no_os_util.c	\
	$(NO-OS)/util/no_os_mutex.c	\
	$(NO-OS)/util/no_os_alloc.c

INCS += $(DRIVERS)/led/<devname>/<devname>.h
SRCS += $(DRIVERS)/led/<devname>/<devname>.c
```

### 7.4 `projects/<project_name>/src/examples/examples_src.mk`

```makefile
ifeq (basic,$(strip $(EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif
```

### 7.5 `projects/<project_name>/src/common/common_data.h`

```c
/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by <DEVNAME> examples.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "<devname>.h"

extern const struct no_os_i2c_init_param iip;
extern const struct no_os_gpio_init_param gip;
extern struct <devname>_init_param <devname>_user_init;

#endif /* __COMMON_DATA_H__ */
```

### 7.6 `projects/<project_name>/src/common/common_data.c`

```c
/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by <DEVNAME> examples.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "common_data.h"

const struct no_os_i2c_init_param iip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 400000,
	.slave_address = DEV_I2C_ADDRESS,
	.platform_ops = I2C_OPS,
	.extra = &<devname>_i2c_extra,
};

const struct no_os_gpio_init_param gip = {
	.port = GPIO_EN_PIN_PORT,
	.number = GPIO_EN_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = &<devname>_gpio_extra,
};

struct <devname>_init_param <devname>_user_init = {
	.i2c_init_param = iip,
	.gpio_init_param = gip,
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
 *   @brief  Basic example code for <devname> project
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "common_data.h"
#include "basic_example.h"
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
	int ret;

	ret = <devname>_init(&dev, &<devname>_user_init);
	if (ret)
		goto error;

	while (1) {
		ret = <devname>_set_current(dev, <DEVNAME>_CH_0, 128);
		if (ret)
			goto free_dev;

		no_os_mdelay(1000);

		ret = <devname>_set_current(dev, <DEVNAME>_CH_0, 0);
		if (ret)
			goto free_dev;

		no_os_mdelay(1000);
	}

free_dev:
	<devname>_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}
```

### 7.9 `projects/<project_name>/src/platform/platform_includes.h`

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

#endif /* __PLATFORM_INCLUDES_H__ */
```

### 7.10 `projects/<project_name>/src/platform/maxim/main.c`

```c
/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "parameters.h"
#include "common_data.h"

int example_main();

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
 *******************************************************************************/
int main()
{
	return example_main();
}
```

### 7.11 `projects/<project_name>/src/platform/maxim/parameters.h`

```c
/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_i2c.h"
#include "maxim_gpio.h"

#define DEV_I2C_ADDRESS		0x1B

#if (TARGET_NUM == 32650) || (TARGET_NUM == 78000)
#define I2C_DEVICE_ID		1
#elif (TARGET_NUM == 32655)
#define I2C_DEVICE_ID		2
#elif (TARGET_NUM == 32665) || (TARGET_NUM == 32660) || (TARGET_NUM == 32690)
#define I2C_DEVICE_ID		0
#endif

#define I2C_OPS			&max_i2c_ops

#define GPIO_OPS		&max_gpio_ops

#define GPIO_EN_PIN_NUM		14
#define GPIO_EN_PIN_PORT	0

extern struct max_gpio_init_param <devname>_gpio_extra;
extern struct max_i2c_init_param <devname>_i2c_extra;

#endif /* __PARAMETERS_H__ */
```

### 7.12 `projects/<project_name>/src/platform/maxim/parameters.c`

```c
/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#include "parameters.h"

struct max_i2c_init_param <devname>_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param <devname>_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
```

### 7.13 `projects/<project_name>/src/platform/maxim/platform_src.mk`

```makefile
INCS += $(PLATFORM_DRIVERS)/maxim_gpio.h	\
	$(PLATFORM_DRIVERS)/maxim_i2c.h		\
	$(PLATFORM_DRIVERS)/maxim_irq.h		\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h	\
	$(PLATFORM_DRIVERS)/maxim_uart.h	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c	\
	$(PLATFORM_DRIVERS)/maxim_gpio.c	\
	$(PLATFORM_DRIVERS)/maxim_i2c.c		\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.c	\
	$(PLATFORM_DRIVERS)/maxim_irq.c		\
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
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

The <DEVNAME> is a multi-channel LED driver with programmable current
sinks controlled via I2C.  It supports <N> independently adjustable LED
channels with <resolution>-bit current control.  <Add 2-3 more sentences
from the datasheet covering key specs: supply voltage range, max current
per channel, charge pump options, dimming features.>

Applications
------------

* LED backlighting for mobile devices
* LED status indicators
* RGB LED color mixing
* Keypad and display backlighting
* General-purpose LED dimming

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the I2C communication interface, and configures
the GPIO enable pin.  Error handling releases any allocated resources
on failure.

Current Control
~~~~~~~~~~~~~~~

The ``<devname>_set_current`` function writes the desired current
level to the appropriate LED channel register.  The current DAC
resolution depends on the specific device (e.g., 4-bit for 16 levels,
8-bit for 256 levels).

Channel Enable/Disable
~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_channel_enable`` function enables or disables a
specific LED channel.  Disabling sets the current to zero while
retaining the configured current level for re-enabling.

Reset
~~~~~

The ``<devname>_reset`` function writes zero to all channel registers
and clears the cached configuration state.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_i2c.h"
   #include "no_os_gpio.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .i2c_init_param = {
               .device_id     = 1,
               .max_speed_hz  = 400000,
               .slave_address = 0x1B,
               .platform_ops  = &max_i2c_ops,
               .extra         = &i2c_extra,
           },
           .gpio_init_param = {
               .port         = 0,
               .number       = 14,
               .pull         = NO_OS_PULL_NONE,
               .platform_ops = &max_gpio_ops,
               .extra        = &gpio_extra,
           },
       };

       struct <devname>_dev *dev;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       /* Set channel 0 to half brightness */
       ret = <devname>_set_current(dev, <DEVNAME>_CH_0, 128);
       if (ret)
           goto cleanup;

       /* ... */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/led/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/led/<devname>/README.rst
```

---

## 9. Key Conventions

1. **`_dev` not `_desc`** -- LED drivers use `<devname>_dev` for the
   device descriptor struct (consistent with `ltc3208_dev`,
   `ltc3220_dev`).
2. **I2C communication** -- write-only devices (no read-back):
   - Write register: `no_os_i2c_write(dev->i2c_desc, buf, 2, 1)`
     where `buf[0]` = register address, `buf[1]` = data.
   - The stop-bit parameter is `1` (generate I2C stop condition).
3. **GPIO enable pin** -- LED drivers typically have an enable/control
   GPIO (e.g., `ENRGBS` on LTC3208, `RST` on LTC3220).  Initialise
   with `no_os_gpio_get()` + `no_os_gpio_direction_output()`.
4. **Field helpers** -- use `no_os_field_prep()` to pack bit fields
   and `no_os_field_get()` to extract them.  Masks use
   `NO_OS_GENMASK()` and single bits use `NO_OS_BIT()`.
5. **Configuration caching** -- cache register state in the device
   descriptor struct since many LED controllers are write-only (no
   read-back capability).
6. **No IIO by default** -- existing LED drivers do not implement IIO.
   Only add IIO support when the device exposes measurable attributes
   (e.g., current readback, fault status).
7. **`init_param` by value vs pointer** -- `ltc3208` passes
   `init_param` by value; prefer passing by pointer
   (`struct <devname>_init_param *init_param`) for consistency with
   the broader no-OS convention.
8. **Goto-based cleanup** -- `init()` uses goto labels for error
   cleanup, unwinding resources in reverse order of allocation.
9. All other conventions (C++ compat via `extern "C"`, tabs,
   Doxygen, `no_os_calloc`/`no_os_free`, negative error codes, etc.)
   match the general no-OS driver guidelines.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> multi-channel LED driver with I2C
interface and programmable current sinks.

Signed-off-by: Your Name <your.name@analog.com>
```
