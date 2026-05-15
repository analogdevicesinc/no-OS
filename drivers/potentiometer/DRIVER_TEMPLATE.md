# Digital Potentiometer Driver Template

Reference driver: `drivers/potentiometer/ad5110/`

This template covers every file needed to add a new digital potentiometer
driver to no-OS.  Replace `<devname>` with the part number (e.g.,
`ad5110`) and `<DEVNAME>` with its uppercase form (e.g., `AD5110`)
throughout.

---

## 1. Naming Conventions

Digital potentiometer drivers use `_dev` for the device descriptor
struct.

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ad5110.h`, `ad5110.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ad5110.h`, `iio_ad5110.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __AD5110_H__` |
| Init param struct | `<devname>_init_param` | `ad5110_init_param` |
| Device struct | `<devname>_dev` | `ad5110_dev` |
| IIO descriptor | `<devname>_iio_desc` | `ad5110_iio_desc` |
| Functions | `<devname>_<verb>()` | `ad5110_init()` |
| Address macro | `<DEVNAME>_ADDR(x)` | `AD5110_ADDR(x)` |
| Command defines | `<DEVNAME>_CMD_<NAME>` | `AD5110_CMD_WR_RDAC` |
| Field masks | `<DEVNAME>_<FIELD>_MASK` | `AD5110_WIPER_MASK` |

---

## 2. File Checklist

```
drivers/potentiometer/<devname>/
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

doc/sphinx/source/drivers/potentiometer/<devname>.rst   # Sphinx entry
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
#include "no_os_util.h"

/* ---- I2C Address -------------------------------------------------- */

/**
 * I2C base address.  x = A0 pin state (0-1).
 * Example: AD5110 base = 0x2F.
 */
#define <DEVNAME>_ADDR(x)		(0x2F + (x))

/* ---- Constants ---------------------------------------------------- */

/** I2C frame size (command + data byte). */
#define <DEVNAME>_FRAME_SIZE		2

/** Wiper resolution in bits. */
#define <DEVNAME>_RESOLUTION		7

/** Maximum wiper position (2^resolution - 1). */
#define <DEVNAME>_MAX_POS		NO_OS_GENMASK(<DEVNAME>_RESOLUTION - 1, 0)

/* ---- Command Codes ------------------------------------------------ */

#define <DEVNAME>_CMD_NOP		0
#define <DEVNAME>_CMD_WR_RDAC_EEPROM	1
#define <DEVNAME>_CMD_WR_RDAC		2
#define <DEVNAME>_CMD_SHUTDOWN		3
#define <DEVNAME>_CMD_RESET		4
#define <DEVNAME>_CMD_RD_RDAC		5
#define <DEVNAME>_CMD_RD_EEPROM		6

/** Command shift position within the first transmitted byte. */
#define <DEVNAME>_CMD_SHIFT		8

/* ---- EEPROM Read Sub-addresses ------------------------------------ */

#define <DEVNAME>_EEPROM_WIPER		0
#define <DEVNAME>_EEPROM_TOLERANCE	1

/* ---- Register Masks ----------------------------------------------- */

#define <DEVNAME>_WIPER_MASK		NO_OS_GENMASK(7, 0)
#define <DEVNAME>_SHUTDOWN_MASK		NO_OS_BIT(0)

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters for <devname>_init().
 */
struct <devname>_init_param {
	/** I2C initialization parameters. */
	struct no_os_i2c_init_param i2c_init;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** I2C descriptor. */
	struct no_os_i2c_desc *i2c_desc;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Write a wiper position to the RDAC register. */
int <devname>_write_rdac(struct <devname>_dev *dev, uint8_t rdac_val);

/** @brief Read the current wiper position from the RDAC register. */
int <devname>_read_rdac(struct <devname>_dev *dev, uint8_t *rdac_val);

/** @brief Store the current RDAC value to EEPROM. */
int <devname>_store_eeprom(struct <devname>_dev *dev);

/** @brief Read the wiper position stored in EEPROM. */
int <devname>_read_eeprom_wiper(struct <devname>_dev *dev, uint8_t *val);

/** @brief Read the resistor tolerance from EEPROM. */
int <devname>_read_tolerance(struct <devname>_dev *dev, uint8_t *val);

/** @brief Software reset (reload RDAC from EEPROM). */
int <devname>_reset(struct <devname>_dev *dev);

/** @brief Enter or exit shutdown mode. */
int <devname>_shutdown(struct <devname>_dev *dev, uint8_t enable);

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

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
#include "<devname>.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/**
 * @brief Send a two-byte command frame over I2C.
 * @param dev     - Device descriptor.
 * @param cmd     - Command code (upper byte).
 * @param data    - Data byte (lower byte).
 * @return 0 on success, negative error code otherwise.
 */
static int <devname>_send_cmd(struct <devname>_dev *dev, uint8_t cmd,
			      uint8_t data)
{
	uint8_t buf[<DEVNAME>_FRAME_SIZE];

	buf[0] = cmd;
	buf[1] = data;

	return no_os_i2c_write(dev->i2c_desc, buf, <DEVNAME>_FRAME_SIZE, 1);
}

/**
 * @brief Write a wiper position to the RDAC register.
 * @param dev      - Device descriptor.
 * @param rdac_val - Wiper value to write (0 to <DEVNAME>_MAX_POS).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_write_rdac(struct <devname>_dev *dev, uint8_t rdac_val)
{
	if (rdac_val > <DEVNAME>_MAX_POS)
		return -EINVAL;

	return <devname>_send_cmd(dev, <DEVNAME>_CMD_WR_RDAC, rdac_val);
}

/**
 * @brief Read the current wiper position from the RDAC register.
 * @param dev      - Device descriptor.
 * @param rdac_val - Pointer to store the wiper position.
 * @return 0 on success, negative error code otherwise.
 *
 * The read is a two-step I2C transaction:
 *   1. Write CMD_RD_RDAC to initiate the read.
 *   2. Read one byte containing the wiper position.
 */
int <devname>_read_rdac(struct <devname>_dev *dev, uint8_t *rdac_val)
{
	uint8_t buf[<DEVNAME>_FRAME_SIZE];
	int ret;

	ret = <devname>_send_cmd(dev, <DEVNAME>_CMD_RD_RDAC, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, buf, 1, 1);
	if (ret)
		return ret;

	*rdac_val = buf[0];

	return 0;
}

/**
 * @brief Store the current RDAC value to EEPROM (non-volatile).
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_store_eeprom(struct <devname>_dev *dev)
{
	return <devname>_send_cmd(dev, <DEVNAME>_CMD_WR_RDAC_EEPROM, 0);
}

/**
 * @brief Read the wiper position stored in EEPROM.
 * @param dev - Device descriptor.
 * @param val - Pointer to store the EEPROM wiper value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read_eeprom_wiper(struct <devname>_dev *dev, uint8_t *val)
{
	uint8_t buf[<DEVNAME>_FRAME_SIZE];
	int ret;

	ret = <devname>_send_cmd(dev, <DEVNAME>_CMD_RD_EEPROM,
				 <DEVNAME>_EEPROM_WIPER);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, buf, 1, 1);
	if (ret)
		return ret;

	*val = buf[0];

	return 0;
}

/**
 * @brief Read the resistor tolerance from EEPROM.
 * @param dev - Device descriptor.
 * @param val - Pointer to store tolerance value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read_tolerance(struct <devname>_dev *dev, uint8_t *val)
{
	uint8_t buf[<DEVNAME>_FRAME_SIZE];
	int ret;

	ret = <devname>_send_cmd(dev, <DEVNAME>_CMD_RD_EEPROM,
				 <DEVNAME>_EEPROM_TOLERANCE);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, buf, 1, 1);
	if (ret)
		return ret;

	*val = buf[0];

	return 0;
}

/**
 * @brief Software reset; refreshes RDAC from EEPROM.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reset(struct <devname>_dev *dev)
{
	return <devname>_send_cmd(dev, <DEVNAME>_CMD_RESET, 0);
}

/**
 * @brief Enter or exit shutdown mode.
 * @param dev    - Device descriptor.
 * @param enable - 1 to shut down, 0 to resume normal operation.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_shutdown(struct <devname>_dev *dev, uint8_t enable)
{
	return <devname>_send_cmd(dev, <DEVNAME>_CMD_SHUTDOWN,
				  enable & <DEVNAME>_SHUTDOWN_MASK);
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param dev        - Pointer to descriptor pointer (allocated here).
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

	ret = no_os_i2c_init(&descriptor->i2c_desc, &init_param->i2c_init);
	if (ret)
		goto free_desc;

	*dev = descriptor;

	return 0;

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
 * @brief Read raw wiper position from the RDAC register.
 *
 * Returns the wiper position as an integer (0 to <DEVNAME>_MAX_POS).
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = (struct <devname>_iio_desc *)dev;
	uint8_t rdac_val;
	int32_t val;
	int ret;

	ret = <devname>_read_rdac(desc-><devname>_dev, &rdac_val);
	if (ret)
		return ret;

	val = rdac_val;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write raw wiper position to the RDAC register.
 */
static int <devname>_iio_write_raw(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct <devname>_iio_desc *desc = (struct <devname>_iio_desc *)dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	if (val < 0 || val > <DEVNAME>_MAX_POS)
		return -EINVAL;

	return <devname>_write_rdac(desc-><devname>_dev, (uint8_t)val);
}

/**
 * @brief Read scale factor.
 *
 * For a digital potentiometer, scale converts the raw wiper position
 * to an equivalent resistance.  If full-scale resistance is R_AB and
 * resolution is N bits:
 *   scale = R_AB / (2^N - 1)
 *
 * For example, with R_AB = 10000 ohms and 7-bit resolution (128 steps):
 *   scale = 10000 / 127 = 78.740157 ohms/step
 *   Expressed as IIO_VAL_INT_PLUS_MICRO: 78 + 740157
 *
 * Adjust these values for the target device.
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	val[0] = 78;
	val[1] = 740157;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_attrs[] = {
	{ .name = "raw",   .show = <devname>_iio_read_raw,
			   .store = <devname>_iio_write_raw },
	{ .name = "scale", .show = <devname>_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_scan = {
	.sign = 'u',          /* unsigned wiper position */
	.realbits = 8,        /* wiper resolution */
	.storagebits = 8,     /* stored in 8-bit container */
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Channel definition ------------------------------------- */

/**
 * Resistance channel: IIO_RESISTANCE, ch_out = true (output), single
 * channel.
 *
 * The wiper position represents a controllable resistance, so ch_out
 * is set to true (it is an output channel that can be written).
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "resistance",
		.ch_type = IIO_RESISTANCE,
		.ch_out = 1,
		.indexed = true,
		.channel = 0,
		.attributes = <devname>_iio_attrs,
		.scan_type = &<devname>_scan,
	},
};

/* ---------- Buffered read callback --------------------------------- */

/**
 * @brief Read wiper position samples into an IIO buffer.
 */
static int <devname>_iio_read_samples(void *dev, int16_t *buf,
				      uint32_t samples)
{
	struct <devname>_iio_desc *desc = (struct <devname>_iio_desc *)dev;
	uint8_t rdac_val;
	uint32_t i;
	int ret;

	for (i = 0; i < samples; i++) {
		ret = <devname>_read_rdac(desc-><devname>_dev, &rdac_val);
		if (ret)
			return ret;

		buf[i] = (int16_t)rdac_val;
	}

	return samples;
}

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = 1,
	.read_dev = (int32_t (*)())<devname>_iio_read_samples,
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

INCS += $(DRIVERS)/potentiometer/<devname>/<devname>.h
SRCS += $(DRIVERS)/potentiometer/<devname>/<devname>.c
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
INCS += $(DRIVERS)/potentiometer/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/potentiometer/<devname>/iio_<devname>.c

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
		.slave_address = <DEVNAME>_ADDR(0),
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
	uint8_t rdac_val;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	while (1) {
		ret = <devname>_read_rdac(dev, &rdac_val);
		if (ret)
			goto free_dev;

		pr_info("Wiper position: %d\r\n", rdac_val);
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

The <DEVNAME> is a non-volatile digital potentiometer with a
<resolution>-bit RDAC register and I2C interface.  It features
<max_positions> wiper positions and an end-to-end resistance of
<R_AB> ohms.  The EEPROM allows the wiper position to be stored
and automatically restored at power-on.  <Add 2-3 more sentences
from the datasheet covering key specs: supply voltage, tolerance,
number of program cycles, operating temperature range.>

Applications
------------

* Mechanical potentiometer replacement
* Sensor calibration
* Audio volume control
* Power supply adjustment
* Programmable gain and offset

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor and initialises the I2C communication interface.  Error
handling releases any allocated resources on failure.

Wiper Position Control
~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_write_rdac`` function writes a new wiper position
to the volatile RDAC register.  Valid values range from 0 to
``<DEVNAME>_MAX_POS``.  The ``<devname>_read_rdac`` function reads
the current wiper position back.

EEPROM Storage
~~~~~~~~~~~~~~

The ``<devname>_store_eeprom`` function copies the current RDAC
value to non-volatile EEPROM.  On power-up or software reset, the
stored value is automatically restored.  ``<devname>_read_eeprom_wiper``
reads the EEPROM-stored wiper position.

Tolerance Readback
~~~~~~~~~~~~~~~~~~

The ``<devname>_read_tolerance`` function reads the factory-programmed
resistor tolerance value from EEPROM.

Power Management
~~~~~~~~~~~~~~~~

The ``<devname>_shutdown`` function places the device in a low-power
shutdown mode.  In shutdown, the wiper is connected to terminal B and
terminal A is open-circuited.

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
               .slave_address = <DEVNAME>_ADDR(0),
               .platform_ops  = &max_i2c_ops,
               .extra         = &i2c_extra,
           },
       };

       struct <devname>_dev *dev;
       uint8_t wiper_pos;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       /* Set wiper to midscale */
       ret = <devname>_write_rdac(dev, <DEVNAME>_MAX_POS / 2);
       if (ret)
           goto cleanup;

       /* Read back current position */
       ret = <devname>_read_rdac(dev, &wiper_pos);
       if (ret)
           goto cleanup;

       /* Store to EEPROM for power-on restore */
       ret = <devname>_store_eeprom(dev);

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes a single ``IIO_RESISTANCE`` output
channel with ``raw`` (read/write wiper position), and ``scale``
(ohms per step) attributes.  The raw attribute is writable to allow
setting the wiper position from IIO clients.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/potentiometer/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/potentiometer/<devname>/README.rst
```

---

## 9. Key Conventions

1. **`_dev` not `_desc`** -- digital potentiometer drivers use
   `<devname>_dev` for the device descriptor struct.
2. **I2C command protocol** -- most potentiometer commands are
   two-byte frames: `[command, data]`.  Use
   `no_os_i2c_write(..., 2, 1)` with the stop bit.  For reads,
   write the read command first, then `no_os_i2c_read(..., 1, 1)`.
3. **Wiper position** -- the RDAC register holds the volatile
   wiper position.  Values range from 0 to `<DEVNAME>_MAX_POS`.
   Use `NO_OS_GENMASK()` to define the maximum.
4. **EEPROM store/restore** -- non-volatile storage of the wiper
   position.  `CMD_WR_RDAC_EEPROM` stores, `CMD_RESET` restores.
   `CMD_RD_EEPROM` with sub-address reads wiper or tolerance.
5. **Address macro** -- `<DEVNAME>_ADDR(x)` where `x` is the pin
   state for configurable I2C address.
6. **Command defines** -- use `<DEVNAME>_CMD_<NAME>` naming for
   command codes (e.g., `<DEVNAME>_CMD_WR_RDAC`).
7. **IIO channel type** -- `IIO_RESISTANCE` with `ch_out = 1`
   (output) since the wiper position is a controllable output.
   The `raw` attribute must be both readable and writable.
8. **IIO scale** -- expressed as `IIO_VAL_INT_PLUS_MICRO`
   representing ohms per wiper step.  Calculate as
   `R_AB / (2^N - 1)` for the target device.
9. All other conventions (C++ compat, tabs, Doxygen,
   `no_os_calloc`, goto cleanup, negative error codes,
   `NO_OS_BIT()`/`NO_OS_GENMASK()` macros) match the ADC
   template.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> digital potentiometer with <resolution>-bit
RDAC, EEPROM wiper storage, and I2C interface.

Signed-off-by: Your Name <your.name@analog.com>
```
