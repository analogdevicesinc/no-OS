# Digital I/O Driver Template

Reference driver: `drivers/digital-io/max14914/`

This template covers every file needed to add a new digital I/O driver to
no-OS. Replace `<devname>` with the part number (e.g., `max14914`) and
`<DEVNAME>` with its uppercase form (e.g., `MAX14914`) throughout.

Digital I/O drivers in this category control industrial digital input/output
devices. They fall into two sub-types:

- **GPIO-controlled** (e.g., MAX14914, MAX14919): the device has no register
  bus; all control is through GPIOs. The device descriptor uses `_desc`
  naming (not `_dev`).
- **SPI-controlled** (e.g., MAX14906, MAX22200, MAX22915): the device has
  a SPI register interface. These may use either `_desc` or `_dev` naming.

This template shows both patterns and notes where they diverge.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `max14914.h`, `max14914.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_max14906.h`, `iio_max14906.c` |
| Include guard | `#ifndef _<DEVNAME>_H` | `#ifndef _MAX14914_H` |
| Init param struct | `<devname>_init_param` | `max14914_init_param` |
| Device struct | `<devname>_desc` | `max14914_desc` |
| IIO descriptor | `<devname>_iio_desc` | `max14906_iio_desc` |
| Functions | `<devname>_<verb>()` | `max14914_init()` |
| Channel count | `<DEVNAME>_<NAME>_CHANNELS` | `MAX14919_OUT_CHANNELS` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `MAX14914_DIGITAL_OUTPUT_PP` |
| Register addresses | `<DEVNAME>_REG_<NAME>` | `MAX22915_REG_SET_OUTPUT` |
| Field masks | `<DEVNAME>_<FIELD>_MASK` | `MAX22915_CH_MASK(channel)` |

**Note on `_desc` vs `_dev` naming:** Many digital I/O drivers (especially
GPIO-controlled ones) name the device struct `<devname>_desc` rather than
`<devname>_dev`. Both conventions are acceptable; be consistent within a
single driver.

---

## 2. File Checklist

```
drivers/digital-io/<devname>/
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

doc/sphinx/source/drivers/digital-io/<devname>.rst   # Sphinx entry
```

---

## 3. Driver Header (`<devname>.h`)

### 3.1 GPIO-controlled device (e.g., MAX14914)

```c
/***************************************************************************//**
 *   @file   <devname>.h
 *   @brief  Header file of <DEVNAME> Driver.
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
#ifndef _<DEVNAME>_H
#define _<DEVNAME>_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_util.h"

/* ---------------- Constants -------------------------------------------- */

#define <DEVNAME>_OUT_CHANNELS		4

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_state
 *  @brief Device operating state / pin configuration. */
enum <devname>_state {
	<DEVNAME>_DIGITAL_OUTPUT_PP,
	<DEVNAME>_DIGITAL_OUTPUT_HIGH_SIDE,
	<DEVNAME>_DIGITAL_OUTPUT_LOW_SIDE,
	<DEVNAME>_DIGITAL_INPUT_IEC_TYPE1_3,
	<DEVNAME>_DIGITAL_INPUT_IEC_TYPE2
};

/* Add more enums as needed: output channel selection, fault types, etc. */

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_desc
 * @brief  Structure holding the <DEVNAME> device descriptor.
 */
struct <devname>_desc {
	/** Current operating state. */
	enum <devname>_state current_state;
	/** Fault indication GPIO (input). */
	struct no_os_gpio_desc *fault_gpio;
	/** Digital output current level GPIO (input). */
	struct no_os_gpio_desc *doilvl_gpio;
	/** Digital input enable GPIO (output). */
	struct no_os_gpio_desc *di_en_gpio;
	/** IN control GPIO (output). */
	struct no_os_gpio_desc *in_gpio;
	/** Push-pull mode GPIO (output). */
	struct no_os_gpio_desc *pp_gpio;
};

/**
 * @struct <devname>_init_param
 * @brief  Structure holding the <DEVNAME> initialization parameter.
 */
struct <devname>_init_param {
	/** Fault GPIO init parameter. */
	struct no_os_gpio_init_param *fault_init_param;
	/** DOI level GPIO init parameter. */
	struct no_os_gpio_init_param *doilvl_init_param;
	/** Digital input enable GPIO init parameter. */
	struct no_os_gpio_init_param *di_en_init_param;
	/** IN control GPIO init parameter. */
	struct no_os_gpio_init_param *in_init_param;
	/** Push-pull mode GPIO init parameter. */
	struct no_os_gpio_init_param *pp_init_param;
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Set the device operating state. */
int <devname>_set_state(struct <devname>_desc *desc,
			enum <devname>_state state);

/** @brief Initialize the device descriptor. */
int <devname>_init(struct <devname>_desc **desc,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_desc *desc);

#endif /* _<DEVNAME>_H */
```

### 3.2 SPI-controlled device (e.g., MAX22915)

For SPI-based digital I/O devices, the header includes register map
definitions, field masks, and SPI communication buffers:

```c
#ifndef _<DEVNAME>_H
#define _<DEVNAME>_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_util.h"

#define <DEVNAME>_MAX_CHANNELS		8
#define <DEVNAME>_SPI_FRAME_SIZE	2

/* ---------------- Register Map ----------------------------------------- */

#define <DEVNAME>_REG_SET_OUTPUT	0x00
#define <DEVNAME>_REG_INTERRUPT		0x03
#define <DEVNAME>_REG_FAULT		0x04
#define <DEVNAME>_REG_CONFIG		0x0D

/* ---------------- Field Masks ------------------------------------------ */

#define <DEVNAME>_ADDR_MASK		NO_OS_GENMASK(5, 1)
#define <DEVNAME>_RW_MASK		NO_OS_BIT(0)
#define <DEVNAME>_CH_MASK(channel)	NO_OS_BIT(channel)

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_sw_state
 *  @brief Output switch state. */
enum <devname>_sw_state {
	<DEVNAME>_SWITCH_OPEN,
	<DEVNAME>_SWITCH_CLOSE,
};

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_desc
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_desc {
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** Per-channel switch state cache. */
	enum <devname>_sw_state sw_state[<DEVNAME>_MAX_CHANNELS];
	/** SPI communication buffer. */
	uint8_t buff[<DEVNAME>_SPI_FRAME_SIZE + 1];
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** SPI initialization parameters. */
	struct no_os_spi_init_param *spi_init;
	/** Initial switch state per channel. */
	enum <devname>_sw_state sw_state[<DEVNAME>_MAX_CHANNELS];
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Initialize and configure the device. */
int <devname>_init(struct <devname>_desc **desc,
		   struct <devname>_init_param *init_param);

/** @brief Write a register. */
int <devname>_reg_write(struct <devname>_desc *desc, uint8_t reg_addr,
			uint8_t reg_data);

/** @brief Read a register. */
int <devname>_reg_read(struct <devname>_desc *desc, uint8_t reg_addr,
		       uint8_t *reg_data);

/** @brief Set the switch state of a channel. */
int <devname>_set_sw_state(struct <devname>_desc *desc,
			   uint8_t channel,
			   enum <devname>_sw_state state);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_desc *desc);

#endif /* _<DEVNAME>_H */
```

---

## 4. Driver Source (`<devname>.c`)

### 4.1 GPIO-controlled device (e.g., MAX14914)

```c
/***************************************************************************//**
 *   @file   <devname>.c
 *   @brief  Source file of <DEVNAME> Driver.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * ... (same BSD-3-Clause block as header) ...
 *******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "<devname>.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/**
 * @brief Set the operating state of the <DEVNAME>.
 * @param desc  - Device descriptor.
 * @param state - Target operating state.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_state(struct <devname>_desc *desc,
			enum <devname>_state state)
{
	int ret;

	switch (state) {
	case <DEVNAME>_DIGITAL_OUTPUT_PP:
		ret = no_os_gpio_set_value(desc->di_en_gpio,
					   NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->pp_gpio,
					   NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		break;
	case <DEVNAME>_DIGITAL_OUTPUT_HIGH_SIDE:
		ret = no_os_gpio_set_value(desc->di_en_gpio,
					   NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->pp_gpio,
					   NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->in_gpio,
					   NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		break;
	/* Add more states as needed. */
	default:
		return -EINVAL;
	}

	desc->current_state = state;

	return 0;
}

/**
 * @brief Initialize the <DEVNAME> device descriptor.
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

	ret = no_os_gpio_get(&descriptor->di_en_gpio,
			     init_param->di_en_init_param);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(descriptor->di_en_gpio,
					  NO_OS_GPIO_LOW);
	if (ret)
		goto di_en_error;

	ret = no_os_gpio_get(&descriptor->pp_gpio,
			     init_param->pp_init_param);
	if (ret)
		goto di_en_error;

	ret = no_os_gpio_direction_output(descriptor->pp_gpio,
					  NO_OS_GPIO_LOW);
	if (ret)
		goto pp_error;

	ret = no_os_gpio_get(&descriptor->in_gpio,
			     init_param->in_init_param);
	if (ret)
		goto pp_error;

	ret = no_os_gpio_direction_output(descriptor->in_gpio,
					  NO_OS_GPIO_LOW);
	if (ret)
		goto in_error;

	ret = no_os_gpio_get(&descriptor->doilvl_gpio,
			     init_param->doilvl_init_param);
	if (ret)
		goto in_error;

	ret = no_os_gpio_direction_input(descriptor->doilvl_gpio);
	if (ret)
		goto doilvl_error;

	ret = no_os_gpio_get(&descriptor->fault_gpio,
			     init_param->fault_init_param);
	if (ret)
		goto doilvl_error;

	ret = no_os_gpio_direction_input(descriptor->fault_gpio);
	if (ret)
		goto fault_error;

	descriptor->current_state = <DEVNAME>_DIGITAL_OUTPUT_HIGH_SIDE;
	*desc = descriptor;

	return 0;

fault_error:
	no_os_gpio_remove(descriptor->fault_gpio);
doilvl_error:
	no_os_gpio_remove(descriptor->doilvl_gpio);
in_error:
	no_os_gpio_remove(descriptor->in_gpio);
pp_error:
	no_os_gpio_remove(descriptor->pp_gpio);
di_en_error:
	no_os_gpio_remove(descriptor->di_en_gpio);
error:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param desc - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_remove(struct <devname>_desc *desc)
{
	if (!desc)
		return -ENODEV;

	no_os_gpio_remove(desc->fault_gpio);
	no_os_gpio_remove(desc->doilvl_gpio);
	no_os_gpio_remove(desc->in_gpio);
	no_os_gpio_remove(desc->pp_gpio);
	no_os_gpio_remove(desc->di_en_gpio);
	no_os_free(desc);

	return 0;
}
```

### 4.2 SPI-controlled device (e.g., MAX22915)

```c
/***************************************************************************//**
 *   @file   <devname>.c
 *   @brief  Source file of <DEVNAME> Driver.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * ... (same BSD-3-Clause block as header) ...
 *******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "<devname>.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

/**
 * @brief Write a register value.
 * @param desc     - Device descriptor.
 * @param reg_addr - Register address.
 * @param reg_data - Data to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_write(struct <devname>_desc *desc, uint8_t reg_addr,
			uint8_t reg_data)
{
	desc->buff[0] = no_os_field_prep(<DEVNAME>_ADDR_MASK, reg_addr);
	desc->buff[1] = reg_data;

	return no_os_spi_write_and_read(desc->spi_desc, desc->buff,
					<DEVNAME>_SPI_FRAME_SIZE);
}

/**
 * @brief Read a register value.
 * @param desc     - Device descriptor.
 * @param reg_addr - Register address.
 * @param reg_data - Pointer to store read data.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_read(struct <devname>_desc *desc, uint8_t reg_addr,
		       uint8_t *reg_data)
{
	int ret;

	desc->buff[0] = no_os_field_prep(<DEVNAME>_ADDR_MASK, reg_addr) |
			<DEVNAME>_RW_MASK;
	desc->buff[1] = 0x00;

	ret = no_os_spi_write_and_read(desc->spi_desc, desc->buff,
				       <DEVNAME>_SPI_FRAME_SIZE);
	if (ret)
		return ret;

	*reg_data = desc->buff[1];

	return 0;
}

/**
 * @brief Set the switch state of a channel.
 * @param desc    - Device descriptor.
 * @param channel - Channel index (0-based).
 * @param state   - Desired switch state.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_sw_state(struct <devname>_desc *desc,
			   uint8_t channel,
			   enum <devname>_sw_state state)
{
	uint8_t reg_val;
	int ret;

	if (channel >= <DEVNAME>_MAX_CHANNELS)
		return -EINVAL;

	ret = <devname>_reg_read(desc, <DEVNAME>_REG_SET_OUTPUT, &reg_val);
	if (ret)
		return ret;

	if (state == <DEVNAME>_SWITCH_CLOSE)
		reg_val |= <DEVNAME>_CH_MASK(channel);
	else
		reg_val &= ~<DEVNAME>_CH_MASK(channel);

	ret = <devname>_reg_write(desc, <DEVNAME>_REG_SET_OUTPUT, reg_val);
	if (ret)
		return ret;

	desc->sw_state[channel] = state;

	return 0;
}

/**
 * @brief Initialize the <DEVNAME> device descriptor.
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

	ret = no_os_spi_init(&descriptor->spi_desc, init_param->spi_init);
	if (ret)
		goto free_desc;

	/* Apply initial channel configuration. */
	memcpy(descriptor->sw_state, init_param->sw_state,
	       sizeof(descriptor->sw_state));

	*desc = descriptor;

	return 0;

free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param desc - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_remove(struct <devname>_desc *desc)
{
	if (!desc)
		return -ENODEV;

	no_os_spi_remove(desc->spi_desc);
	no_os_free(desc);

	return 0;
}
```

---

## 5. IIO Header (`iio_<devname>.h`)

```c
/***************************************************************************//**
 *   @file   iio_<devname>.h
 *   @brief  Header file of <DEVNAME> IIO Driver.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef IIO_<DEVNAME>_H
#define IIO_<DEVNAME>_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "<devname>.h"

#define <DEVNAME>_FUNCTION_CNT	3

/**
 * @brief Configuration structure for a <DEVNAME> channel.
 */
struct <devname>_ch_config {
	bool enabled;
	/* Add per-channel settings: function, direction, etc. */
};

/**
 * @brief <DEVNAME> specific IIO descriptor.
 */
struct <devname>_iio_desc {
	/** Core driver descriptor. */
	struct <devname>_desc *<devname>_desc;
	/** IIO device structure. */
	struct iio_device *iio_dev;
	/** Bitmask of active channels. */
	uint32_t active_channels;
	/** Number of active channels. */
	uint32_t no_active_channels;
	/** Per-channel configuration. */
	struct <devname>_ch_config channel_configs[<DEVNAME>_OUT_CHANNELS];
};

/**
 * @brief Initialization parameter for the <DEVNAME> IIO descriptor.
 */
struct <devname>_iio_desc_init_param {
	/** Core driver init params. */
	struct <devname>_init_param *<devname>_init_param;
	/** Per-channel configuration. */
	struct <devname>_ch_config channel_configs[<DEVNAME>_OUT_CHANNELS];
};

/** @brief Initialize the <DEVNAME> IIO descriptor. */
int <devname>_iio_init(struct <devname>_iio_desc **,
		       struct <devname>_iio_desc_init_param *);

/** @brief Free resources allocated by the init function. */
int <devname>_iio_remove(struct <devname>_iio_desc *);

#endif /* IIO_<DEVNAME>_H */
```

---

## 6. IIO Source (`iio_<devname>.c`)

```c
/***************************************************************************//**
 *   @file   iio_<devname>.c
 *   @brief  Source file of IIO <DEVNAME> Driver.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "<devname>.h"
#include "iio_<devname>.h"

/* ---------- Channel macro ---------------------------------------------- */

#define <DEVNAME>_CHANNEL(_addr)			\
	{						\
		.ch_type = IIO_VOLTAGE,			\
		.indexed = 1,				\
		.channel = _addr,			\
		.address = _addr,			\
	}

/* ---------- Forward declarations --------------------------------------- */

static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
static int <devname>_iio_write_raw(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

/* ---------- Attribute callbacks ---------------------------------------- */

/**
 * @brief Read the raw value for a specific channel.
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Private descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = (struct <devname>_iio_desc *)dev;
	uint32_t val;
	int ret;

	/*
	 * Read the channel state.
	 * For GPIO-based: use no_os_gpio_get_value() on the appropriate pin.
	 * For SPI-based: use the register read API.
	 */

	/* TODO: implement channel read. */
	val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Write the raw value for a specific channel.
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer with the value to write.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Private descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int <devname>_iio_write_raw(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct <devname>_iio_desc *desc = (struct <devname>_iio_desc *)dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	/* TODO: implement channel write. */

	return 0;
}

/**
 * @brief Read the offset attribute (typically 0 for digital I/O).
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the scale attribute (typically 1 for digital I/O).
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val = 1;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/* ---------- Output channel attributes ---------------------------------- */

static struct iio_attribute <devname>_out_attrs[] = {
	{
		.name = "raw",
		.show = <devname>_iio_read_raw,
		.store = <devname>_iio_write_raw,
	},
	{
		.name = "offset",
		.show = <devname>_iio_read_offset,
	},
	{
		.name = "scale",
		.show = <devname>_iio_read_scale,
	},
	/*
	 * Add device-specific attributes as needed:
	 * - do_mode / do_mode_available (output mode selection)
	 * - current_limit / current_limit_available
	 * - IEC_type / IEC_type_available (input type selection)
	 */
	END_ATTRIBUTES_ARRAY
};

/* ---------- Input channel attributes ----------------------------------- */

static struct iio_attribute <devname>_in_attrs[] = {
	{
		.name = "raw",
		.show = <devname>_iio_read_raw,
	},
	{
		.name = "offset",
		.show = <devname>_iio_read_offset,
	},
	{
		.name = "scale",
		.show = <devname>_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- IIO device structure --------------------------------------- */

static struct iio_device <devname>_iio_dev = {
	/* channels and num_ch set dynamically in setup_channels(). */
};

/* ---------- Channel setup ---------------------------------------------- */

/**
 * @brief Configure IIO channels based on the enabled channel configs.
 * @param desc - The IIO device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int <devname>_iio_setup_channels(struct <devname>_iio_desc *desc)
{
	struct iio_channel *channels;
	uint32_t enabled_ch = 0;
	uint32_t ch_offset = 0;
	uint32_t i;

	for (i = 0; i < <DEVNAME>_OUT_CHANNELS; i++)
		if (desc->channel_configs[i].enabled)
			enabled_ch++;

	channels = (struct iio_channel *)no_os_calloc(enabled_ch,
						      sizeof(*channels));
	if (!channels)
		return -ENOMEM;

	for (i = 0; i < <DEVNAME>_OUT_CHANNELS; i++) {
		if (!desc->channel_configs[i].enabled)
			continue;

		channels[ch_offset] =
			(struct iio_channel)<DEVNAME>_CHANNEL(i);

		/*
		 * Set direction and attributes based on channel config.
		 * Output channels: ch_out = 1, use <devname>_out_attrs.
		 * Input channels:  ch_out = 0, use <devname>_in_attrs.
		 */
		channels[ch_offset].attributes = <devname>_out_attrs;
		channels[ch_offset].ch_out = 1;

		ch_offset++;
	}

	desc->iio_dev->channels = channels;
	desc->iio_dev->num_ch = enabled_ch;

	return 0;
}

/* ---------- Init / Remove ---------------------------------------------- */

/**
 * @brief Initialize the <DEVNAME> IIO descriptor.
 * @param iio_desc   - The IIO device descriptor.
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_iio_init(struct <devname>_iio_desc **iio_desc,
		       struct <devname>_iio_desc_init_param *init_param)
{
	struct <devname>_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param-><devname>_init_param)
		return -EINVAL;

	descriptor = (struct <devname>_iio_desc *)no_os_calloc(1,
							       sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = <devname>_init(&descriptor-><devname>_desc,
			     init_param-><devname>_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &<devname>_iio_dev;

	memcpy(&descriptor->channel_configs, &init_param->channel_configs,
	       sizeof(descriptor->channel_configs));

	ret = <devname>_iio_setup_channels(descriptor);
	if (ret)
		goto free_dev;

	*iio_desc = descriptor;

	return 0;

free_dev:
	<devname>_remove(descriptor-><devname>_desc);
free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The IIO device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_iio_remove(struct <devname>_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	<devname>_remove(iio_desc-><devname>_desc);
	no_os_free(iio_desc);

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
	$(INCLUDE)/no_os_alloc.h     \
	$(INCLUDE)/no_os_irq.h       \
	$(INCLUDE)/no_os_list.h      \
	$(INCLUDE)/no_os_dma.h       \
	$(INCLUDE)/no_os_uart.h      \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_util.h      \
	$(INCLUDE)/no_os_units.h     \
	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_irq.c  \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_dma.c  \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_util.c  \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/digital-io/<devname>/<devname>.h
SRCS += $(DRIVERS)/digital-io/<devname>/<devname>.c
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
INCS += $(DRIVERS)/digital-io/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/digital-io/<devname>/iio_<devname>.c

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

extern struct no_os_uart_init_param <devname>_uart_ip;
extern struct no_os_gpio_init_param <devname>_fault_ip;
extern struct no_os_gpio_init_param <devname>_doilvl_ip;
extern struct no_os_gpio_init_param <devname>_di_en_ip;
extern struct no_os_gpio_init_param <devname>_in_ip;
extern struct no_os_gpio_init_param <devname>_pp_ip;
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

struct no_os_gpio_init_param <devname>_fault_ip = {
	.port = GPIO_FAULT_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_FAULT_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param <devname>_doilvl_ip = {
	.port = GPIO_DOILVL_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_DOILVL_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param <devname>_di_en_ip = {
	.port = GPIO_DI_EN_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_DI_EN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param <devname>_in_ip = {
	.port = GPIO_IN_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_IN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param <devname>_pp_ip = {
	.port = GPIO_PP_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_PP_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct <devname>_init_param <devname>_ip = {
	.di_en_init_param = &<devname>_di_en_ip,
	.doilvl_init_param = &<devname>_doilvl_ip,
	.fault_init_param = &<devname>_fault_ip,
	.in_init_param = &<devname>_in_ip,
	.pp_init_param = &<devname>_pp_ip,
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
	int ret;

	ret = <devname>_init(&desc, &<devname>_ip);
	if (ret)
		goto exit;

	/* Set the device to digital input mode. */
	ret = <devname>_set_state(desc,
				  <DEVNAME>_DIGITAL_INPUT_IEC_TYPE2);
	if (ret)
		goto free_dev;

	pr_info("<DEVNAME> successfully configured as "
		"IEC Type 2 Digital Input.\n");

free_dev:
	<devname>_remove(desc);
exit:
	if (ret)
		pr_info("Error!\n");

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
	struct <devname>_iio_desc *<devname>_iio;
	struct <devname>_iio_desc_init_param <devname>_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	<devname>_iio_ip.<devname>_init_param = &<devname>_ip;

	/* Configure channels -- enable and set direction as needed. */
	/* <devname>_iio_ip.channel_configs[0].enabled = true; */

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
	app_init_param.uart_init_params = <devname>_uart_ip;

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

	ret = no_os_uart_init(&uart, &<devname>_uart_ip);
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
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_gpio.h"
#include "maxim_gpio_irq.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_IRQ_ID		UART0_IRQn
#define UART_DEVICE_ID		0
#define UART_BAUDRATE		57600
#define UART_EXTRA		&<devname>_uart_extra
#define UART_OPS		&max_uart_ops

/* GPIO definitions -- adjust port/pin numbers per hardware. */
#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&<devname>_gpio_extra_ip
#define GPIO_FAULT_PORT_NUM	2
#define GPIO_FAULT_PIN_NUM	21
#define GPIO_DOILVL_PORT_NUM	1
#define GPIO_DOILVL_PIN_NUM	0
#define GPIO_DI_EN_PORT_NUM	1
#define GPIO_DI_EN_PIN_NUM	1
#define GPIO_IN_PORT_NUM	1
#define GPIO_IN_PIN_NUM		2
#define GPIO_PP_PORT_NUM	1
#define GPIO_PP_PIN_NUM		3

extern struct max_gpio_init_param <devname>_gpio_extra_ip;
extern struct max_uart_init_param <devname>_uart_extra;

#endif /* __PARAMETERS_H__ */
```

### 7.14 `projects/<project_name>/src/platform/maxim/parameters.c`

```c
/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#include "parameters.h"

struct max_uart_init_param <devname>_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_gpio_init_param <devname>_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
```

### 7.15 `projects/<project_name>/src/platform/maxim/platform_src.mk`

```makefile
INCS +=	$(PLATFORM_DRIVERS)/maxim_gpio.h      \
	$(PLATFORM_DRIVERS)/maxim_irq.h       \
	$(PLATFORM_DRIVERS)/maxim_gpio_irq.h  \
	$(PLATFORM_DRIVERS)/maxim_uart.h      \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
	$(PLATFORM_DRIVERS)/maxim_gpio.c      \
	$(PLATFORM_DRIVERS)/maxim_gpio_irq.c  \
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

The <DEVNAME> is a configurable industrial digital input/output device.
<Describe whether it is GPIO-controlled or SPI-controlled, key specs
such as voltage range, number of channels, IEC 61131-2 compliance,
current limiting, and fault detection. Add 2-3 sentences from the
datasheet.>

Applications
------------

* Industrial digital input/output
* PLC and factory automation
* Motor control
* Building automation

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the GPIO pins (or SPI interface), configures
pin directions, and sets the device to a known default state. The
function uses goto-based cleanup to release GPIOs in reverse
allocation order on failure.

State Configuration
~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_state`` function selects the operating mode of
the device (e.g., digital output push-pull, high-side, low-side, or
digital input with IEC Type 1/3 or Type 2 compliance). Each mode
is achieved by driving the appropriate GPIO control pins.

Fault Detection
~~~~~~~~~~~~~~~

The device provides a fault output pin (active-low) that indicates
overcurrent, overtemperature, or open-wire conditions. Read the fault
GPIO to detect error states.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_gpio.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .fault_init_param = &fault_gpio_ip,
           .doilvl_init_param = &doilvl_gpio_ip,
           .di_en_init_param = &di_en_gpio_ip,
           .in_init_param = &in_gpio_ip,
           .pp_init_param = &pp_gpio_ip,
       };

       struct <devname>_desc *desc;
       int ret;

       ret = <devname>_init(&desc, &ip);
       if (ret)
           return ret;

       ret = <devname>_set_state(desc,
                                <DEVNAME>_DIGITAL_INPUT_IEC_TYPE2);
       if (ret)
           goto cleanup;

       /* Device is now configured as an IEC Type 2 digital input. */

   cleanup:
       <devname>_remove(desc);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each channel as an ``IIO_VOLTAGE``
channel with ``raw``, ``scale``, and ``offset`` attributes. Output
channels also support ``do_mode`` and ``current_limit`` attributes
where applicable. Input channels may expose ``IEC_type`` selection.

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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/digital-io/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/digital-io/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_desc *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int`, negative = error.
5. **Init signature** -- `int <devname>_init(struct <devname>_desc **desc,
   struct <devname>_init_param *init_param)` (note: init_param passed
   by pointer for digital I/O drivers).
6. **`_desc` naming** -- GPIO-controlled digital I/O drivers typically use
   `<devname>_desc` for the device struct (not `_dev`). SPI-based drivers
   may use either convention.
7. **GPIO pattern** -- for GPIO-controlled devices, `init_param` holds
   `struct no_os_gpio_init_param *` pointers and the descriptor holds
   `struct no_os_gpio_desc *` pointers. Each GPIO is initialized with
   `no_os_gpio_get()` + `no_os_gpio_direction_output/input()`.
8. **Allocation** -- use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
9. **Goto cleanup** -- in `_init()`, use goto labels to free in reverse
   allocation order on error; each GPIO gets its own error label.
10. **Remove order** -- `_remove()` frees resources in reverse order of
    `_init()`, then calls `no_os_free()` on the descriptor.
11. **No globals** -- all state lives in the device descriptor struct.
12. **Bit macros** -- `NO_OS_BIT()` for single bits,
    `NO_OS_GENMASK(hi, lo)` for fields; `no_os_field_prep()` /
    `no_os_field_get()` for field access.
13. **Fault / status pins** -- configure as `no_os_gpio_direction_input()`
    and read with `no_os_gpio_get_value()`.
14. **Current limiting** -- expose as a configurable attribute (GPIO pin
    or SPI register) where the device supports it.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> configurable industrial digital
input/output device with GPIO-based control interface.

Signed-off-by: Your Name <your.name@analog.com>
```
