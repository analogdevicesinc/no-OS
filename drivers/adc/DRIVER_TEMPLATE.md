# ADC Driver Template

Reference driver: `drivers/adc/ad7689/`

This template covers every file needed to add a new ADC driver to no-OS.
Replace `<devname>` with the part number (e.g., `ad7689`) and `<DEVNAME>`
with its uppercase form (e.g., `AD7689`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ad7689.h`, `ad7689.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ad7689.h`, `iio_ad7689.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __AD7689_H__` |
| Init param struct | `<devname>_init_param` | `ad7689_init_param` |
| Device struct | `<devname>_dev` | `ad7689_dev` |
| IIO descriptor | `<devname>_iio_desc` | `ad7689_iio_desc` |
| Functions | `<devname>_<verb>()` | `ad7689_init()` |
| Static helpers | `_<devname>_<verb>()` | `_ad7689_rac()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_AD7689` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `AD7689_CFG_REF_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `AD7689_REF_INTERNAL_2p5V` |

---

## 2. File Checklist

```
drivers/adc/<devname>/
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

doc/sphinx/source/drivers/adc/<devname>.rst   # Sphinx entry
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

/* ---------------- Register Map / Config Field Masks ---------------- */

/** Configuration field masks — use NO_OS_BIT() for single bits,
 *  NO_OS_GENMASK(hi, lo) for multi-bit fields. */
#define <DEVNAME>_REG_STATUS		0x00
#define <DEVNAME>_REG_CONFIG		0x01
#define <DEVNAME>_REG_DATA		0x02

#define <DEVNAME>_CFG_FIELD_A_MSK	NO_OS_BIT(7)
#define <DEVNAME>_CFG_FIELD_B_MSK	NO_OS_GENMASK(6, 4)
#define <DEVNAME>_CFG_FIELD_C_MSK	NO_OS_GENMASK(3, 0)

/* Product / revision ID expected values */
#define <DEVNAME>_PRODUCT_ID		0xXX

/* ---------------- Enums -------------------------------------------- */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_input_cfg
 *  @brief Input channel configuration modes. */
enum <devname>_input_cfg {
	<DEVNAME>_IN_SINGLE_ENDED,
	<DEVNAME>_IN_DIFFERENTIAL,
};

/** @enum <devname>_ref_sel
 *  @brief Reference voltage selection. */
enum <devname>_ref_sel {
	<DEVNAME>_REF_INTERNAL,
	<DEVNAME>_REF_EXTERNAL,
};

/* Add more enums as needed: sequencer modes, bandwidth, etc. */

/* ---------------- Structures --------------------------------------- */

/**
 * @struct <devname>_config
 * @brief  ADC configuration — mirrors the on-chip config register.
 */
struct <devname>_config {
	/** Input channel configuration mode. */
	enum <devname>_input_cfg input_cfg;
	/** Input channel index (0-based). */
	uint8_t channel;
	/** Reference selection. */
	enum <devname>_ref_sel ref;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Device variant. */
	enum <devname>_device_id id;
	/** ADC configuration. */
	struct <devname>_config config;
	/** SPI initialization parameters. */
	struct no_os_spi_init_param spi_init;
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
	/** Active ADC configuration. */
	struct <devname>_config config;
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
};

/* ---------------- Function Prototypes ------------------------------ */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Read a register. */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint16_t *val);

/** @brief Write a register. */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint16_t val);

/** @brief Read raw ADC samples. */
int <devname>_read(struct <devname>_dev *dev, uint16_t *data,
		   uint32_t nb_samples);

/** @brief Write a new ADC configuration. */
int <devname>_write_config(struct <devname>_dev *dev,
			   struct <devname>_config *config);

/** @brief Read the current ADC configuration. */
int <devname>_read_config(struct <devname>_dev *dev,
			  struct <devname>_config *config);

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

/* ---------- Static helpers ----------------------------------------- */

/**
 * @brief Read a register over SPI.
 * @param dev   - Device descriptor.
 * @param addr  - Register address.
 * @param val   - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_read(struct <devname>_dev *dev, uint8_t addr,
		       uint16_t *val)
{
	uint8_t buf[3];
	int ret;

	buf[0] = addr | 0x80; /* read bit */
	buf[1] = 0x00;
	buf[2] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	*val = ((uint16_t)buf[1] << 8) | buf[2];

	return 0;
}

/**
 * @brief Write a register over SPI.
 * @param dev  - Device descriptor.
 * @param addr - Register address.
 * @param val  - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_dev *dev, uint8_t addr,
			uint16_t val)
{
	uint8_t buf[3];

	buf[0] = addr & 0x7F; /* write bit */
	buf[1] = (uint8_t)(val >> 8);
	buf[2] = (uint8_t)(val & 0xFF);

	return no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
}

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Read raw ADC samples.
 * @param dev        - Device descriptor.
 * @param data       - Buffer to store samples.
 * @param nb_samples - Number of samples to read.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read(struct <devname>_dev *dev, uint16_t *data,
		   uint32_t nb_samples)
{
	uint32_t i;
	int ret;

	for (i = 0; i < nb_samples; i++) {
		ret = <devname>_reg_read(dev, <DEVNAME>_REG_DATA, &data[i]);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Write a new ADC configuration.
 * @param dev    - Device descriptor.
 * @param config - Pointer to new configuration.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_write_config(struct <devname>_dev *dev,
			   struct <devname>_config *config)
{
	uint16_t reg_val;
	int ret;

	reg_val = no_os_field_prep(<DEVNAME>_CFG_FIELD_B_MSK,
				   config->input_cfg);
	reg_val |= no_os_field_prep(<DEVNAME>_CFG_FIELD_C_MSK,
				    config->channel);

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_CONFIG, reg_val);
	if (ret)
		return ret;

	dev->config = *config;

	return 0;
}

/**
 * @brief Read the current ADC configuration.
 * @param dev    - Device descriptor.
 * @param config - Pointer to store configuration.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read_config(struct <devname>_dev *dev,
			  struct <devname>_config *config)
{
	uint16_t reg_val;
	int ret;

	ret = <devname>_reg_read(dev, <DEVNAME>_REG_CONFIG, &reg_val);
	if (ret)
		return ret;

	config->input_cfg = no_os_field_get(<DEVNAME>_CFG_FIELD_B_MSK,
					    reg_val);
	config->channel = no_os_field_get(<DEVNAME>_CFG_FIELD_C_MSK,
					  reg_val);

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
	uint16_t product_id;
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->id = init_param->id;
	d->name = <devname>_device_name[d->id];

	ret = no_os_spi_init(&d->spi_desc, &init_param->spi_init);
	if (ret)
		goto free_dev;

	/* Verify product ID */
	ret = <devname>_reg_read(d, <DEVNAME>_REG_STATUS, &product_id);
	if (ret)
		goto free_spi;

	if (product_id != <DEVNAME>_PRODUCT_ID) {
		pr_err("Unexpected product ID: 0x%04X\n", product_id);
		ret = -ENODEV;
		goto free_spi;
	}

	/* Apply initial configuration */
	ret = <devname>_write_config(d, &init_param->config);
	if (ret)
		goto free_spi;

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
 * @brief Read raw ADC value for a channel.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	uint16_t data;
	int32_t val;
	int ret;

	ret = <devname>_read(desc-><devname>_dev, &data, 1);
	if (ret)
		return ret;

	val = (int32_t)data;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor (mV per LSB = Vref / 2^bits).
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	/*
	 * Example: 2.5V reference, 16-bit ADC
	 * scale = 2500 mV / 65536 = 0.038147 mV per LSB
	 * Expressed as IIO_VAL_INT_PLUS_MICRO: 0 + 38147
	 */
	val[0] = 0;
	val[1] = 38147;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/**
 * @brief Read offset (typically 0 for unipolar ADCs).
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_ch_attrs[] = {
	{ .name = "raw",    .show = <devname>_iio_read_raw },
	{ .name = "scale",  .show = <devname>_iio_read_scale },
	{ .name = "offset", .show = <devname>_iio_read_offset },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_scan_type = {
	.sign = 'u',          /* unsigned */
	.realbits = 16,       /* ADC resolution */
	.storagebits = 16,    /* container size */
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * ADC channels: IIO_VOLTAGE, ch_out = false (input), indexed.
 */
#define <DEVNAME>_IIO_CHANNEL(idx) \
	{ \
		.name = "voltage" #idx, \
		.ch_type = IIO_VOLTAGE, \
		.channel = (idx), \
		.scan_type = &<devname>_scan_type, \
		.scan_index = (idx), \
		.attributes = <devname>_iio_ch_attrs, \
		.ch_out = false, \
		.indexed = true, \
	}

static struct iio_channel <devname>_iio_channels[] = {
	<DEVNAME>_IIO_CHANNEL(0),
	<DEVNAME>_IIO_CHANNEL(1),
	/* Add more channels as needed */
	END_ATTRIBUTES_ARRAY
};

/* ---------- Buffered read callback --------------------------------- */

/**
 * @brief Read ADC samples into an IIO buffer.
 */
static int <devname>_iio_read_samples(void *dev, int16_t *buf,
				      uint32_t samples)
{
	struct <devname>_iio_desc *desc = dev;

	return <devname>_read(desc-><devname>_dev, (uint16_t *)buf, samples);
}

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels) - 1,
	.debug_reg_read = (int32_t (*)())<devname>_reg_read,
	.debug_reg_write = (int32_t (*)())<devname>_reg_write,
	.read_dev = (int32_t (*)())<devname>_iio_read_samples,
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

INCS += $(DRIVERS)/adc/<devname>/<devname>.h
SRCS += $(DRIVERS)/adc/<devname>/<devname>.c
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
INCS += $(DRIVERS)/adc/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/adc/<devname>/iio_<devname>.c

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

struct <devname>_init_param <devname>_ip = {
	.id = ID_<DEVNAME>,
	.spi_init = <devname>_spi_ip,
	.config = {
		.input_cfg = <DEVNAME>_IN_SINGLE_ENDED,
		.channel = 0,
		.ref = <DEVNAME>_REF_INTERNAL,
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
	uint16_t adc_data;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	while (1) {
		ret = <devname>_read(dev, &adc_data, 1);
		if (ret)
			goto free_dev;

		pr_info("ADC raw value: %u\r\n", adc_data);
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

The <DEVNAME> is a <resolution>-bit, <channels>-channel successive
approximation ADC with an SPI-compatible serial interface. It supports
sampling rates up to <rate>, with configurable input modes and reference
options. <Add 2-3 more sentences from the datasheet covering key specs
and features.>

Applications
------------

* Data acquisition systems
* Industrial process control
* Instrumentation

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface, verifies the product ID,
and applies the initial ADC configuration. The function uses goto-based
cleanup to release resources on failure.

Channel Configuration
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_write_config`` function programs the input channel mode,
channel index, and reference selection into the device configuration
register. The ``<devname>_read_config`` function reads back the current
configuration.

Sampling / Conversion
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_read`` function acquires one or more raw ADC samples
by issuing successive register reads. Each sample is a <resolution>-bit
unsigned value representing the input voltage relative to the selected
reference.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .id = ID_<DEVNAME>,
           .spi_init = { /* platform-specific SPI params */ },
           .config = {
               .input_cfg = <DEVNAME>_IN_SINGLE_ENDED,
               .channel = 0,
               .ref = <DEVNAME>_REF_INTERNAL,
           },
       };

       struct <devname>_dev *dev;
       uint16_t sample;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_read(dev, &sample, 1);
       if (ret)
           goto cleanup;

       /* sample contains the raw ADC value */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each ADC input as an
``IIO_VOLTAGE`` channel with ``raw``, ``scale``, and ``offset``
attributes. The IIO application can be used for real-time data
acquisition.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/adc/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/adc/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** — explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`).
2. **Linux kernel style** — tabs = 8 spaces, 80-char line limit.
3. **Doxygen** — document all public structs, fields, and functions.
4. **Return codes** — all functions return `int`, negative = error.
5. **Init signature** — `int <devname>_init(struct <devname>_dev **dev,
   struct <devname>_init_param *init_param)`.
6. **Allocation** — use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
7. **Goto cleanup** — in `_init()`, use goto labels to free in reverse
   allocation order on error.
8. **Remove order** — `_remove()` frees resources in reverse order of
   `_init()`.
9. **No globals** — all state lives in the device descriptor struct.
10. **Register macros** — `NO_OS_BIT()` for single bits,
    `NO_OS_GENMASK(hi, lo)` for fields; `no_os_field_prep()` /
    `no_os_field_get()` for field access.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> <resolution>-bit, <channels>-channel
successive approximation ADC with SPI interface.

Signed-off-by: Your Name <your.name@analog.com>
```
