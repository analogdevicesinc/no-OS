# Display Driver Template

Reference driver: `drivers/display/nhd_c12832a1z/`

This template covers every file needed to add a new display controller
driver to no-OS.  Replace `<devname>` with the part number (e.g.,
`nhd_c12832a1z`) and `<DEVNAME>` with its uppercase form (e.g.,
`NHD_C12832A1Z`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `nhd_c12832a1z.h`, `nhd_c12832a1z.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __NHD_C12832A1Z_H__` |
| Init param struct | `<devname>_init_param` | `nhd_c12832a1z_init_param` |
| Device struct | `<devname>_dev` | `nhd_c12832a1z_dev` |
| Functions | `<devname>_<verb>()` | `nhd_c12832a1z_init()` |
| Static helpers | `_<devname>_<verb>()` | `_nhd_c12832a1z_send_page()` |
| Command macros | `<DEVNAME>_<CMD>` | `NHD_C12832A1Z_DISP_ON` |
| Display constants | `<DEVNAME>_<CONST>` | `NHD_C12832A1Z_DC_DATA` |

**Note:** Display drivers do not typically have IIO files. Displays are
output-only peripherals that do not produce measurement data, so the
IIO subsystem is not applicable.

---

## 2. File Checklist

```
drivers/display/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
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

doc/sphinx/source/drivers/display/<devname>.rst   # Sphinx entry
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
#include "no_os_gpio.h"
#include "no_os_spi.h"

/* ---------------- Display Commands ------------------------------------- */

/** Reset pin states. */
#define <DEVNAME>_RST_ON		0U
#define <DEVNAME>_RST_OFF		1U

/** Data/Command pin states — DC high = data, DC low = command. */
#define <DEVNAME>_DC_DATA		1U
#define <DEVNAME>_DC_CMD		0U

/** Display on/off commands. */
#define <DEVNAME>_DISP_ON		0xAFU
#define <DEVNAME>_DISP_OFF		0xAEU

/** Controller-specific init commands (examples). */
#define <DEVNAME>_ADC_NORMAL		0xA0U
#define <DEVNAME>_ADC_REVERSE		0xA1U
#define <DEVNAME>_COM_NORMAL		0xC0U
#define <DEVNAME>_COM_REVERSE		0xC8U
#define <DEVNAME>_LCD_BIAS		0xA2U
#define <DEVNAME>_PWR_CTRL		0x2FU
#define <DEVNAME>_RES_RATIO		0x21U
#define <DEVNAME>_ELECTRIC_VOL		0x81U
#define <DEVNAME>_ELECTRIC_VAL		0x20U

/* ---------------- Display Dimensions ----------------------------------- */

/** Number of pixel columns. */
#define <DEVNAME>_NR_COLUMNS		128
/** Number of pages (rows / 8 for monochrome displays). */
#define <DEVNAME>_NR_PAGES		4
/** Page start address base. */
#define <DEVNAME>_PAGE_START_ADDR	0xB0
/** Display start line offset. */
#define <DEVNAME>_START_OFFSET		0x40
/** Maximum number of characters per screen. */
#define <DEVNAME>_NR_CHAR		64

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Data/Command GPIO init param. */
	struct no_os_gpio_init_param	*dc_pin_ip;
	/** Reset GPIO init param (optional — can be NULL). */
	struct no_os_gpio_init_param	*reset_pin_ip;
	/** SPI init param. */
	struct no_os_spi_init_param	*spi_ip;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** Data/Command GPIO descriptor. */
	struct no_os_gpio_desc		*dc_pin;
	/** Reset GPIO descriptor. */
	struct no_os_gpio_desc		*reset_pin;
	/** SPI descriptor. */
	struct no_os_spi_desc		*spi_desc;
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Write a command byte to the display controller. */
int <devname>_write_cmd(struct <devname>_dev *dev, uint8_t cmd);

/** @brief Write a data byte to the display controller. */
int <devname>_write_data(struct <devname>_dev *dev, uint8_t data);

/** @brief Print a string on the display. */
int <devname>_print_string(struct <devname>_dev *dev, char *msg);

/** @brief Clear the display. */
int <devname>_clear(struct <devname>_dev *dev);

/** @brief Initialise the display device. */
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/*
 * Font lookup table — 8-byte bitmaps for each ASCII character.
 * Displays use a local font table instead of register-based data.
 * This is typically a static const uint8_t ASC16[256][8] array.
 */
static const uint8_t font_8x8[256][8] = {
	/* ... 256 entries of 8-byte glyph bitmaps ... */
};

/* ---------- Command / Data helpers --------------------------------- */

/**
 * @brief Write a command byte to the display controller.
 *
 * Sets the DC pin low (command mode), then sends the byte over SPI.
 *
 * @param dev - Device descriptor.
 * @param cmd - Command byte.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_write_cmd(struct <devname>_dev *dev, uint8_t cmd)
{
	int ret;

	if (!dev->spi_desc || !dev->dc_pin)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->dc_pin, <DEVNAME>_DC_CMD);
	if (ret)
		return ret;

	return no_os_spi_write_and_read(dev->spi_desc, &cmd, 1U);
}

/**
 * @brief Write a data byte to the display controller.
 *
 * Sets the DC pin high (data mode), then sends the byte over SPI.
 *
 * @param dev  - Device descriptor.
 * @param data - Data byte.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_write_data(struct <devname>_dev *dev, uint8_t data)
{
	int ret;

	if (!dev->spi_desc || !dev->dc_pin)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->dc_pin, <DEVNAME>_DC_DATA);
	if (ret)
		return ret;

	return no_os_spi_write_and_read(dev->spi_desc, &data, 1U);
}

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Print a string on the display.
 *
 * Renders each character from the font table into a local framebuffer,
 * then flushes the framebuffer to the display page by page.
 *
 * @param dev - Device descriptor.
 * @param msg - Null-terminated string to display.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_print_string(struct <devname>_dev *dev, char *msg)
{
	int ret;
	unsigned int i, j;
	uint8_t framebuffer[<DEVNAME>_NR_PAGES][<DEVNAME>_NR_COLUMNS] = { 0 };
	uint8_t page = <DEVNAME>_PAGE_START_ADDR;
	int32_t count = strlen(msg);

	if (count > <DEVNAME>_NR_CHAR)
		count = <DEVNAME>_NR_CHAR;

	/* Render characters into the framebuffer */
	for (j = 0; j < count; ++j) {
		int y = j >> 4;           /* page index */
		int x = (j & 0xF) << 3;  /* column offset (8 pixels wide) */

		for (i = 0; i < 8; i++)
			framebuffer[y][x + i] = font_8x8[(uint8_t)msg[j]][i];
	}

	/* Turn off display during update */
	ret = <devname>_write_cmd(dev, <DEVNAME>_DISP_OFF);
	if (ret)
		return ret;

	ret = <devname>_write_cmd(dev, <DEVNAME>_START_OFFSET);
	if (ret)
		return ret;

	/* Flush each page to the display */
	for (i = 0; i < <DEVNAME>_NR_PAGES; i++) {
		ret = <devname>_write_cmd(dev, page);
		if (ret)
			return ret;

		ret = <devname>_write_cmd(dev, 0x10); /* column addr upper */
		if (ret)
			return ret;

		ret = <devname>_write_cmd(dev, 0x00); /* column addr lower */
		if (ret)
			return ret;

		for (j = 0; j < <DEVNAME>_NR_COLUMNS; j++) {
			ret = <devname>_write_data(dev, framebuffer[i][j]);
			if (ret)
				return ret;
		}
		page++;
	}

	return <devname>_write_cmd(dev, <DEVNAME>_DISP_ON);
}

/**
 * @brief Clear the display.
 *
 * Writes zeroes to every column on every page, blanking the screen.
 *
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_clear(struct <devname>_dev *dev)
{
	int ret;
	unsigned int i, j;
	uint8_t page = <DEVNAME>_PAGE_START_ADDR;

	ret = <devname>_write_cmd(dev, <DEVNAME>_DISP_OFF);
	if (ret)
		return ret;

	ret = <devname>_write_cmd(dev, <DEVNAME>_START_OFFSET);
	if (ret)
		return ret;

	for (i = 0; i < <DEVNAME>_NR_PAGES; i++) {
		ret = <devname>_write_cmd(dev, page);
		if (ret)
			return ret;

		ret = <devname>_write_cmd(dev, 0x10);
		if (ret)
			return ret;

		ret = <devname>_write_cmd(dev, 0x00);
		if (ret)
			return ret;

		for (j = 0; j < <DEVNAME>_NR_COLUMNS; j++) {
			ret = <devname>_write_data(dev, 0x00);
			if (ret)
				return ret;
		}
		page++;
	}

	return <devname>_write_cmd(dev, <DEVNAME>_DISP_ON);
}

/**
 * @brief Initialise the display device.
 *
 * Allocates the device descriptor, initialises SPI and GPIO pins,
 * performs hardware reset (if reset pin is provided), sends the
 * controller initialisation command sequence, and clears the display.
 *
 * @param dev        - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *d;
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = no_os_spi_init(&d->spi_desc, init_param->spi_ip);
	if (ret)
		goto free_dev;

	ret = no_os_gpio_get(&d->dc_pin, init_param->dc_pin_ip);
	if (ret)
		goto free_spi;

	ret = no_os_gpio_get_optional(&d->reset_pin, init_param->reset_pin_ip);
	if (ret)
		goto free_dc;

	/* Configure DC pin as output, default to command mode */
	ret = no_os_gpio_direction_output(d->dc_pin, <DEVNAME>_DC_CMD);
	if (ret)
		goto free_rst;

	/* Hardware reset sequence (if reset pin is available) */
	if (d->reset_pin) {
		ret = no_os_gpio_direction_output(d->reset_pin,
						  <DEVNAME>_RST_ON);
		if (ret)
			goto free_rst;

		no_os_udelay(3U);

		ret = no_os_gpio_set_value(d->reset_pin, <DEVNAME>_RST_OFF);
		if (ret)
			goto free_rst;
	}

	/* Controller initialisation command sequence */
	ret = <devname>_write_cmd(d, <DEVNAME>_ADC_NORMAL);
	if (ret)
		goto free_rst;

	ret = <devname>_write_cmd(d, <DEVNAME>_DISP_OFF);
	if (ret)
		goto free_rst;

	ret = <devname>_write_cmd(d, <DEVNAME>_COM_REVERSE);
	if (ret)
		goto free_rst;

	ret = <devname>_write_cmd(d, <DEVNAME>_LCD_BIAS);
	if (ret)
		goto free_rst;

	ret = <devname>_write_cmd(d, <DEVNAME>_PWR_CTRL);
	if (ret)
		goto free_rst;

	ret = <devname>_write_cmd(d, <DEVNAME>_RES_RATIO);
	if (ret)
		goto free_rst;

	ret = <devname>_write_cmd(d, <DEVNAME>_ELECTRIC_VOL);
	if (ret)
		goto free_rst;

	ret = <devname>_write_cmd(d, <DEVNAME>_ELECTRIC_VAL);
	if (ret)
		goto free_rst;

	/* Clear the display */
	ret = <devname>_clear(d);
	if (ret)
		goto free_rst;

	*dev = d;

	return 0;

free_rst:
	no_os_gpio_remove(d->reset_pin);
free_dc:
	no_os_gpio_remove(d->dc_pin);
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

	ret = no_os_gpio_remove(dev->reset_pin);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->dc_pin);
	if (ret)
		return ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
```

---

## 5. IIO Header — Not Applicable

Display controllers are output-only peripherals and do not produce
measurement data. The IIO subsystem (Industrial I/O) is designed for
sensors and data acquisition devices, so IIO support is not typical
for display drivers.

If your display is part of a larger system that requires IIO
integration (e.g., rendering IIO channel data on screen), the IIO
logic belongs in the project example layer, not in the display driver
itself.

---

## 6. IIO Source — Not Applicable

See section 5 above. No IIO source file is needed for display drivers.

---

## 7. Project Example

### 7.1 `projects/<project_name>/Makefile`

```makefile
# Select the example you want to enable by choosing y for enabling and n for disabling
BASIC_EXAMPLE = y

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
```

### 7.2 `projects/<project_name>/builds.json`

```json
{
  "<platform>": {
    "basic_example_<target>": {
      "flags" : "BASIC_EXAMPLE=y TARGET=<target>"
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

INCS += $(DRIVERS)/display/<devname>/<devname>.h
SRCS += $(DRIVERS)/display/<devname>/<devname>.c
```

### 7.4 `projects/<project_name>/src/examples/examples_src.mk`

```makefile
ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
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

static struct no_os_spi_init_param <devname>_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_MAX_SPEED,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_3,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.parent = NULL,
};

static struct no_os_gpio_init_param <devname>_dc_ip = {
	.number = DC_PIN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

static struct no_os_gpio_init_param <devname>_rst_ip = {
	.number = RST_PIN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct <devname>_init_param <devname>_ip = {
	.dc_pin_ip = &<devname>_dc_ip,
	.reset_pin_ip = &<devname>_rst_ip,
	.spi_ip = &<devname>_spi_ip,
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
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	ret = <devname>_clear(dev);
	if (ret)
		goto free_dev;

	ret = <devname>_print_string(dev, "Hello, World!");
	if (ret)
		goto free_dev;

	while (1) {
		/* Display content persists — nothing to poll. */
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

#include "platform_includes.h"
#include "common_data.h"

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

#if (BASIC_EXAMPLE != 1)
#error No example project enabled. Please enable BASIC_EXAMPLE and rebuild.
#endif

#ifdef BASIC_EXAMPLE
error:
	no_os_uart_remove(uart);
#endif
	return 0;
}
```

### 7.11 `projects/<project_name>/src/platform/maxim/parameters.h`

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

#define GPIO_OPS	&max_gpio_ops
#define GPIO_EXTRA	&max_gpio_extra

/** Data/Command GPIO pin number. */
#define DC_PIN		10
/** Reset GPIO pin number. */
#define RST_PIN		11

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;
extern struct max_gpio_init_param max_gpio_extra;

#endif /* __PARAMETERS_H__ */
```

### 7.12 `projects/<project_name>/src/platform/maxim/parameters.c`

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

struct max_gpio_init_param max_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
```

### 7.13 `projects/<project_name>/src/platform/maxim/platform_src.mk`

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

The <DEVNAME> is a <dimensions> monochrome graphic LCD module with an
integrated <controller> display controller. It communicates via a
3-wire or 4-wire SPI interface with a separate Data/Command (DC)
control pin. The display is organised as <pages> pages of <columns>
columns, where each byte written in data mode maps directly to 8
vertical pixels on the current page. <Add 2-3 more sentences from the
datasheet covering key display specs and features.>

Applications
------------

* Status displays for embedded systems
* Menu-driven user interfaces
* Real-time data visualisation on evaluation boards

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface and GPIO pins (DC and
optional reset), performs a hardware reset sequence, sends the
controller-specific initialisation commands, and clears the display.
The function uses goto-based cleanup to release resources on failure.

Command vs. Data Mode
~~~~~~~~~~~~~~~~~~~~~

Display controllers distinguish between command bytes and data bytes
using the DC (Data/Command) GPIO pin:

- **DC low** — the byte is interpreted as a controller command
  (``<devname>_write_cmd``).
- **DC high** — the byte is interpreted as display RAM data
  (``<devname>_write_data``).

Framebuffer and Rendering
~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_print_string`` function renders characters into a
local framebuffer array using an 8x8 font lookup table, then flushes
the framebuffer to the display page by page. The
``<devname>_clear`` function blanks the screen by writing zeroes to
all display RAM.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"
   #include "no_os_gpio.h"

   int main()
   {
       struct no_os_spi_init_param spi_ip = {
           /* platform-specific SPI params */
       };
       struct no_os_gpio_init_param dc_ip = {
           /* DC pin params */
       };
       struct no_os_gpio_init_param rst_ip = {
           /* Reset pin params */
       };
       struct <devname>_init_param ip = {
           .dc_pin_ip = &dc_ip,
           .reset_pin_ip = &rst_ip,
           .spi_ip = &spi_ip,
       };

       struct <devname>_dev *dev;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_print_string(dev, "Hello!");
       if (ret)
           goto cleanup;

       /* Display content persists until cleared or overwritten */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/display/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/display/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int`, negative = error.
5. **Init signature** -- `int <devname>_init(struct <devname>_dev **dev,
   struct <devname>_init_param *init_param)`.
6. **Allocation** -- use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
7. **Goto cleanup** -- in `_init()`, use goto labels to free in reverse
   allocation order on error (SPI, DC GPIO, reset GPIO).
8. **Remove order** -- `_remove()` frees resources in reverse order of
   `_init()`: reset GPIO, DC GPIO, SPI, device struct.
9. **No globals** -- all state lives in the device descriptor struct.
10. **DC pin pattern** -- display drivers toggle the Data/Command GPIO
    before every SPI transfer; commands set DC low, pixel data sets
    DC high. This is the fundamental difference from register-based
    device drivers.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> display driver support

Add support for the <DEVNAME> <dimensions> monochrome graphic LCD
display with <controller> controller and SPI interface.

Signed-off-by: Your Name <your.name@analog.com>
```
