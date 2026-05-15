# Temperature Sensor Driver Template

Reference driver: `drivers/temperature/adt75/`

This template covers every file needed to add a new temperature sensor
driver to no-OS.  Replace `<devname>` with the part number (e.g.,
`adt75`) and `<DEVNAME>` with its uppercase form (e.g., `ADT75`)
throughout.

---

## 1. Naming Conventions

Temperature sensor drivers use `_desc` instead of `_dev` for the device
descriptor struct.

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adt75.h`, `adt75.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_adt75.h`, `iio_adt75.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADT75_H__` |
| Init param struct | `<devname>_init_param` | `adt75_init_param` |
| Device struct | `<devname>_desc` (**not** `_dev`) | `adt75_desc` |
| IIO descriptor | `<devname>_iio_desc` | `adt75_iio_desc` |
| Functions | `<devname>_<verb>()` | `adt75_init()` |
| Address macro | `<DEVNAME>_ADDR(x)` | `ADT75_ADDR(x)` |
| Register defines | `<DEVNAME>_<REG>_REG` | `ADT75_TEMP_VALUE_REG` |
| Field masks | `<DEVNAME>_<FIELD>_MASK` | `ADT75_TEMP_MASK` |

---

## 2. File Checklist

```
drivers/temperature/<devname>/
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

doc/sphinx/source/drivers/temperature/<devname>.rst   # Sphinx entry
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

/**
 * I2C base address.  x = A2:A1:A0 pin state (0-7).
 * Example: ADT75 base = 0x48.
 */
#define <DEVNAME>_ADDR(x)		(0x48 + (x))

/* ---- Timing / Constants ------------------------------------------- */

/** Conversion delay in milliseconds. */
#define <DEVNAME>_CONV_DELAY_MS		40

/** Max I2C frame size (register address + 2 data bytes). */
#define <DEVNAME>_FRAME_SIZE		3

/**
 * Temperature divider: 1 LSB = 1/<DEVNAME>_TEMP_DIV °C.
 * Example: ADT75 has 0.0625°C/LSB → divider = 16.
 */
#define <DEVNAME>_TEMP_DIV		16

/* ---- Register Addresses ------------------------------------------- */

#define <DEVNAME>_TEMP_VALUE_REG	0x0
#define <DEVNAME>_CONF_REG		0x1
#define <DEVNAME>_HYST_REG		0x2
#define <DEVNAME>_OS_REG		0x3
#define <DEVNAME>_ONE_SHOT_REG		0x4

/* ---- Register Masks ----------------------------------------------- */

#define <DEVNAME>_DATA_REG_MASK		NO_OS_GENMASK(2, 0)
#define <DEVNAME>_CONFIG_REG_MASK	NO_OS_GENMASK(7, 0)
#define <DEVNAME>_TEMP_MASK		NO_OS_GENMASK(15, 4)
#define <DEVNAME>_SIGN_BIT		11

#define <DEVNAME>_SHUTDOWN_MASK		NO_OS_BIT(0)
#define <DEVNAME>_COMP_INT_MASK		NO_OS_BIT(1)
#define <DEVNAME>_ALERT_POL_MASK	NO_OS_BIT(2)
#define <DEVNAME>_FAULT_QUEUE_MASK	NO_OS_GENMASK(4, 3)
#define <DEVNAME>_ONESHOT_MASK		NO_OS_BIT(5)

#define <DEVNAME>_HYST_MASK		NO_OS_GENMASK(15, 4)
#define <DEVNAME>_OVER_TEMP_MASK	NO_OS_GENMASK(15, 4)

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters for <devname>_init().
 */
struct <devname>_init_param {
	/** I2C initialization parameters. */
	struct no_os_i2c_init_param comm_param;
};

/**
 * @struct <devname>_desc
 * @brief  Device descriptor — allocated by <devname>_init().
 *
 * Note: temperature sensor drivers use _desc (not _dev).
 */
struct <devname>_desc {
	/** I2C descriptor. */
	struct no_os_i2c_desc *comm_desc;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Read a single temperature in milliCelsius. */
int <devname>_get_single_temp(struct <devname>_desc *desc, int32_t *val);

/** @brief Read a register. */
int <devname>_reg_read(struct <devname>_desc *desc, uint32_t addr,
		       uint16_t *val);

/** @brief Write a register. */
int <devname>_reg_write(struct <devname>_desc *desc, uint32_t addr,
			uint16_t val);

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

#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

/**
 * @brief Read a device register over I2C.
 * @param desc - Device descriptor.
 * @param addr - Register address.
 * @param val  - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 *
 * The config register is 1 byte; all other registers are 2 bytes
 * (big-endian).
 */
int <devname>_reg_read(struct <devname>_desc *desc, uint32_t addr,
		       uint16_t *val)
{
	uint8_t buf[<DEVNAME>_FRAME_SIZE];
	int reg_size;
	int ret;

	reg_size = (addr == <DEVNAME>_CONF_REG) ? 1 : 2;

	buf[0] = no_os_field_get(<DEVNAME>_DATA_REG_MASK, addr);
	ret = no_os_i2c_write(desc->comm_desc, buf, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(desc->comm_desc, buf, reg_size, 1);
	if (ret)
		return ret;

	if (reg_size == 2)
		*val = no_os_get_unaligned_be16(buf);
	else
		*val = buf[0];

	return 0;
}

/**
 * @brief Write a device register over I2C.
 * @param desc - Device descriptor.
 * @param addr - Register address.
 * @param val  - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_desc *desc, uint32_t addr,
			uint16_t val)
{
	uint8_t buf[<DEVNAME>_FRAME_SIZE];
	int reg_size;

	reg_size = (addr == <DEVNAME>_CONF_REG) ? 1 : 2;

	buf[0] = no_os_field_get(<DEVNAME>_DATA_REG_MASK, addr);
	no_os_put_unaligned_be16(val, &buf[1]);

	return no_os_i2c_write(desc->comm_desc, buf, reg_size + 1, 1);
}

/**
 * @brief Read a single temperature value.
 * @param desc - Device descriptor.
 * @param val  - Pointer to store temperature in milliCelsius.
 * @return 0 on success, negative error code otherwise.
 *
 * Conversion:
 *   1. Read 16-bit raw from temp register.
 *   2. Extract bits [15:4] (12-bit signed value).
 *   3. Sign-extend from bit 11.
 *   4. Multiply by (1000 / <DEVNAME>_TEMP_DIV) to get milliCelsius.
 *      (e.g., 1000/16 = 62.5 m°C per LSB)
 */
int <devname>_get_single_temp(struct <devname>_desc *desc, int32_t *val)
{
	uint16_t reg_val;
	int ret;

	ret = <devname>_reg_read(desc, <DEVNAME>_TEMP_VALUE_REG, &reg_val);
	if (ret)
		return ret;

	/* Wait for a new conversion to finish. */
	no_os_mdelay(<DEVNAME>_CONV_DELAY_MS);

	reg_val = no_os_field_get(<DEVNAME>_TEMP_MASK, reg_val);
	*val = no_os_sign_extend32(reg_val, <DEVNAME>_SIGN_BIT);
	*val *= MILLIDEGREE_PER_DEGREE / <DEVNAME>_TEMP_DIV;

	return 0;
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param desc       - Pointer to descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_init(struct <devname>_desc **desc,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_desc *descriptor;
	int ret;

	descriptor = (struct <devname>_desc *)no_os_calloc(1,
							   sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_i2c_init(&descriptor->comm_desc, &init_param->comm_param);
	if (ret)
		goto free_desc;

	*desc = descriptor;

	return 0;

free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by <devname>_init().
 * @param desc - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_remove(struct <devname>_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_remove(desc->comm_desc);
	if (ret)
		return ret;

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
	struct <devname>_desc *<devname>_desc;
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
 * @brief Read raw temperature value (sign-extended 12-bit).
 *
 * Returns the raw register value after masking and sign extension.
 * To convert to milliCelsius: raw * scale.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	uint16_t reg_val;
	int32_t val;
	int ret;

	ret = <devname>_reg_read(desc-><devname>_desc,
				 <DEVNAME>_TEMP_VALUE_REG, &reg_val);
	if (ret)
		return ret;

	reg_val = no_os_field_get(<DEVNAME>_TEMP_MASK, reg_val);
	val = no_os_sign_extend32(reg_val, <DEVNAME>_SIGN_BIT);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor (milliCelsius per LSB).
 *
 * For a 12-bit sensor with 0.0625°C/LSB:
 *   scale = 62.5 m°C per LSB
 *   Expressed as IIO_VAL_INT_PLUS_MICRO: 62 + 500000
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	val[0] = 62;
	val[1] = 500000;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/**
 * @brief Read offset (0 for most temperature sensors).
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_attrs[] = {
	{ .name = "raw",    .show = <devname>_iio_read_raw },
	{ .name = "scale",  .show = <devname>_iio_read_scale },
	{ .name = "offset", .show = <devname>_iio_read_offset },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_scan = {
	.sign = 's',          /* signed */
	.realbits = 12,       /* temperature resolution */
	.storagebits = 16,    /* stored in 16-bit container */
	.shift = 4,           /* raw data in bits [15:4] */
	.is_big_endian = false,
};

/* ---------- Channel definition ------------------------------------- */

/**
 * Temperature channel: IIO_TEMP, ch_out = false (input), single channel.
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "temp",
		.ch_type = IIO_TEMP,
		.ch_out = 0,
		.attributes = <devname>_iio_attrs,
		.scan_type = &<devname>_scan,
	},
};

/* ---------- Buffered read callback --------------------------------- */

/**
 * @brief Read temperature samples into an IIO buffer.
 */
static int <devname>_iio_read_samples(void *dev, int16_t *buf,
				      uint32_t samples)
{
	struct <devname>_iio_desc *desc = dev;
	uint16_t val;
	uint32_t i;
	int ret;

	for (i = 0; i < samples; i++) {
		ret = <devname>_reg_read(desc-><devname>_desc,
					 <DEVNAME>_TEMP_VALUE_REG, &val);
		if (ret)
			return ret;

		buf[i] = val;
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

INCS += $(DRIVERS)/temperature/<devname>/<devname>.h
SRCS += $(DRIVERS)/temperature/<devname>/<devname>.c
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
INCS += $(DRIVERS)/temperature/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/temperature/<devname>/iio_<devname>.c

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
	.comm_param = {
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
	struct <devname>_desc *desc;
	int32_t temp_mc;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&desc, &<devname>_ip);
	if (ret)
		goto error;

	while (1) {
		ret = <devname>_get_single_temp(desc, &temp_mc);
		if (ret)
			goto free_dev;

		pr_info("Temperature: %d mC\r\n", temp_mc);
		no_os_mdelay(1000);
	}

free_dev:
	<devname>_remove(desc);
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

The <DEVNAME> is a temperature monitoring device with a <resolution>-bit
ADC that provides temperature readings with a precision of <precision>°C.
It communicates via I2C and supports configurable address pins (A0, A1,
A2).  <Add 2-3 more sentences from the datasheet covering key specs:
voltage range, power consumption, temperature range, alert features.>

Applications
------------

* Isolated sensors
* Environmental control systems
* Computer thermal monitoring
* Thermal protection
* Industrial process control

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor and initialises the I2C communication interface.  Error
handling releases any allocated resources on failure.

Temperature Reading
~~~~~~~~~~~~~~~~~~~

The ``<devname>_get_single_temp`` function reads the temperature
register, extracts the signed value, and converts it to milliCelsius.
The conversion is: ``raw * (1000 / <DEVNAME>_TEMP_DIV)`` where each
LSB equals <precision>°C.

Alert Configuration
~~~~~~~~~~~~~~~~~~~

The hysteresis and over-temperature registers can be programmed via
``<devname>_reg_write`` to configure alert thresholds.  The
configuration register controls alert polarity, comparator vs.
interrupt mode, and fault queue depth.

Power Management
~~~~~~~~~~~~~~~~

The ``<devname>_reg_write`` function can set the shutdown bit in
the configuration register for low-power operation.  The one-shot
register triggers a single conversion in shutdown mode.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_i2c.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .comm_param = {
               .device_id     = 1,
               .max_speed_hz  = 400000,
               .slave_address = <DEVNAME>_ADDR(0),
               .platform_ops  = &max_i2c_ops,
               .extra         = &i2c_extra,
           },
       };

       struct <devname>_desc *desc;
       int32_t temp_mc;
       int ret;

       ret = <devname>_init(&desc, &ip);
       if (ret)
           return ret;

       ret = <devname>_get_single_temp(desc, &temp_mc);
       if (ret)
           goto cleanup;

       /* temp_mc holds the temperature in milliCelsius */

   cleanup:
       <devname>_remove(desc);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes a single ``IIO_TEMP`` channel with
``raw``, ``scale`` (milliCelsius per LSB), and ``offset`` attributes.
The IIO application can be used for continuous temperature monitoring.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/temperature/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/temperature/<devname>/README.rst
```

---

## 9. Key Conventions

1. **`_desc` not `_dev`** — temperature sensor drivers use
   `<devname>_desc` for the device descriptor struct.
2. **I2C communication** — use `no_os_i2c_write()` and
   `no_os_i2c_read()` with the stop-bit parameter:
   - Write register address: `no_os_i2c_write(..., 1, 0)` (no stop)
   - Read data: `no_os_i2c_read(..., reg_size, 1)` (with stop)
   - Write data: `no_os_i2c_write(..., reg_size + 1, 1)` (with stop)
3. **Big-endian registers** — use `no_os_get_unaligned_be16()` for
   reading and `no_os_put_unaligned_be16()` for writing.
4. **Address macro** — `<DEVNAME>_ADDR(x)` where `x` is the pin
   state for configurable I2C address.
5. **Temperature conversion** — always return milliCelsius:
   `raw * MILLIDEGREE_PER_DEGREE / <DEVNAME>_TEMP_DIV`.
6. **Sign extension** — use `no_os_sign_extend32(reg_val, sign_bit)`.
7. **IIO channel type** — `IIO_TEMP` with `ch_out = 0` (input).
8. **IIO scale** — expressed as `IIO_VAL_INT_PLUS_MICRO` representing
   milliCelsius per LSB (e.g., 62 + 500000 for 62.5 m°C/LSB).
9. All other conventions (C++ compat, tabs, Doxygen, `no_os_calloc`,
   goto cleanup, negative error codes, etc.) match the ADC template.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> temperature sensor with <resolution>-bit
ADC and I2C interface.

Signed-off-by: Your Name <your.name@analog.com>
```
