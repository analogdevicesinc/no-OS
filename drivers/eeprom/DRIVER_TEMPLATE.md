# EEPROM Driver Template

Reference drivers: `drivers/eeprom/24xx32a/`, `drivers/eeprom/m24512/`

This template covers every file needed to add a new I2C EEPROM driver to
no-OS.  Replace `<devname>` with the part number (e.g., `24xx32a`) and
`<DEVNAME>` with its uppercase form (e.g., `EEPROM_24XX32A`) throughout.

EEPROM drivers in no-OS implement the `no_os_eeprom_platform_ops`
interface, which provides a uniform read/write/init/remove API through
the `no_os_eeprom.h` abstraction layer.

---

## 1. Naming Conventions

EEPROM drivers use `_dev` for the device descriptor struct and implement
the `no_os_eeprom_platform_ops` interface.

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `24xx32a.h`, `24xx32a.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __EEPROM_24XX32A_H__` |
| Init param struct | `<devname>_init_param` | `eeprom_24xx32a_init_param` |
| Device struct | `<devname>_dev` | `eeprom_24xx32a_dev` |
| Functions | `<devname>_<verb>()` | `eeprom_24xx32a_init()` |
| Platform ops | `eeprom_<devname>_ops` | `eeprom_24xx32a_ops` |
| Address macro | `<DEVNAME>_ADDR(x)` | `EEPROM_24XX32A_ADDR(x)` |
| Size defines | `<DEVNAME>_<PARAM>` | `EEPROM_24XX32A_PAGE_SIZE` |

---

## 2. File Checklist

```
drivers/eeprom/<devname>/
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

doc/sphinx/source/drivers/eeprom/<devname>.rst   # Sphinx entry
```

Note: IIO files are not typically needed for EEPROM drivers since
EEPROMs are storage devices, not data-acquisition devices.

---

## 3. Driver Header (`<devname>.h`)

```c
/***************************************************************************//**
 *   @file   <devname>.h
 *   @brief  Header file of <DEVNAME> EEPROM Driver
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
#include "no_os_eeprom.h"

/* ---- I2C Address -------------------------------------------------- */

/**
 * I2C base address.  x = A2:A1:A0 pin state (0-7).
 * Example: 24XX32A base = 0x50.
 */
#define <DEVNAME>_ADDR(x)		(0x50 + (x))

/* ---- Memory Layout ------------------------------------------------ */

/** Total memory size in bytes. */
#define <DEVNAME>_MEMORY_SIZE		4096U

/** Page size in bytes (write operations wrap at page boundaries). */
#define <DEVNAME>_PAGE_SIZE		32U

/** Total number of pages. */
#define <DEVNAME>_NUM_PAGES		(<DEVNAME>_MEMORY_SIZE / <DEVNAME>_PAGE_SIZE)

/** Maximum valid address. */
#define <DEVNAME>_MAX_ADDRESS		(<DEVNAME>_MEMORY_SIZE - 1)

/* ---- Timing ------------------------------------------------------- */

/** Write cycle time in milliseconds (per datasheet). */
#define <DEVNAME>_WRITE_CYCLE_MS	5U

/* ---- Address Helpers ---------------------------------------------- */

/** Extract high byte of a 16-bit memory address. */
#define <DEVNAME>_ADDR_HIGH(addr)	((uint8_t)(((addr) >> 8) & 0xFF))

/** Extract low byte of a 16-bit memory address. */
#define <DEVNAME>_ADDR_LOW(addr)	((uint8_t)((addr) & 0xFF))

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters for the EEPROM.
 *
 * Passed as the `extra` field of `no_os_eeprom_init_param`.
 */
struct <devname>_init_param {
	/** I2C initialization parameters. */
	struct no_os_i2c_init_param i2c_init;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 *
 * Stored in the `extra` field of `no_os_eeprom_desc`.
 */
struct <devname>_dev {
	/** I2C descriptor. */
	struct no_os_i2c_desc *i2c_desc;
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Initialise the EEPROM device. */
int32_t <devname>_init(struct no_os_eeprom_desc **desc,
		       const struct no_os_eeprom_init_param *param);

/** @brief Free resources allocated by <devname>_init(). */
int32_t <devname>_remove(struct no_os_eeprom_desc *desc);

/** @brief Read data from the EEPROM. */
int32_t <devname>_read(struct no_os_eeprom_desc *desc, uint32_t address,
		       uint8_t *data, uint16_t bytes);

/** @brief Write data to the EEPROM. */
int32_t <devname>_write(struct no_os_eeprom_desc *desc, uint32_t address,
			uint8_t *data, uint16_t bytes);

/** @brief EEPROM platform ops. */
extern const struct no_os_eeprom_platform_ops eeprom_<devname>_ops;

#endif /* __<DEVNAME>_H__ */
```

---

## 4. Driver Source (`<devname>.c`)

```c
/***************************************************************************//**
 *   @file   <devname>.c
 *   @brief  Implementation of <DEVNAME> EEPROM Driver
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * ... (same BSD-3-Clause block as header) ...
 *******************************************************************************/

#include <string.h>

#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/**
 * @brief Read data from the EEPROM.
 * @param desc    - EEPROM descriptor.
 * @param address - Starting memory address.
 * @param data    - Buffer to store read data.
 * @param bytes   - Number of bytes to read.
 * @return 0 on success, negative error code otherwise.
 *
 * Sends a 2-byte address (big-endian) then reads the requested
 * number of bytes.  Sequential reads wrap automatically at the
 * end of the address space.
 */
int32_t <devname>_read(struct no_os_eeprom_desc *desc, uint32_t address,
		       uint8_t *data, uint16_t bytes)
{
	struct <devname>_dev *dev;
	uint8_t addr_buf[2];
	int ret;

	if (!desc || !desc->extra || !data || bytes == 0)
		return -EINVAL;

	if (address > <DEVNAME>_MAX_ADDRESS)
		return -EINVAL;

	dev = (struct <devname>_dev *)desc->extra;

	/* Set up 16-bit address (big-endian). */
	addr_buf[0] = <DEVNAME>_ADDR_HIGH(address);
	addr_buf[1] = <DEVNAME>_ADDR_LOW(address);

	/* Write address with no stop bit (repeated start). */
	ret = no_os_i2c_write(dev->i2c_desc, addr_buf, 2, 0);
	if (ret)
		return ret;

	/* Read data with stop bit. */
	ret = no_os_i2c_read(dev->i2c_desc, data, bytes, 1);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Write data to the EEPROM (handles page boundaries).
 * @param desc    - EEPROM descriptor.
 * @param address - Starting memory address.
 * @param data    - Data buffer to write.
 * @param bytes   - Number of bytes to write.
 * @return 0 on success, negative error code otherwise.
 *
 * Writes are broken into page-aligned chunks.  Each page write is
 * followed by a delay of <DEVNAME>_WRITE_CYCLE_MS to allow the
 * internal write cycle to complete.
 */
int32_t <devname>_write(struct no_os_eeprom_desc *desc, uint32_t address,
			uint8_t *data, uint16_t bytes)
{
	struct <devname>_dev *dev;
	uint8_t buf[<DEVNAME>_PAGE_SIZE + 2];
	uint16_t bytes_written = 0;
	uint16_t page_offset;
	uint16_t chunk;
	int ret;

	if (!desc || !desc->extra || !data || bytes == 0)
		return -EINVAL;

	if (address > <DEVNAME>_MAX_ADDRESS)
		return -EINVAL;

	dev = (struct <devname>_dev *)desc->extra;

	while (bytes_written < bytes) {
		/* Calculate how many bytes fit in the current page. */
		page_offset = (address + bytes_written) % <DEVNAME>_PAGE_SIZE;
		chunk = <DEVNAME>_PAGE_SIZE - page_offset;
		if (chunk > (bytes - bytes_written))
			chunk = bytes - bytes_written;

		/* Prepare address + data buffer. */
		buf[0] = <DEVNAME>_ADDR_HIGH(address + bytes_written);
		buf[1] = <DEVNAME>_ADDR_LOW(address + bytes_written);
		memcpy(&buf[2], &data[bytes_written], chunk);

		ret = no_os_i2c_write(dev->i2c_desc, buf, chunk + 2, 1);
		if (ret)
			return ret;

		/* Wait for internal write cycle to complete. */
		no_os_mdelay(<DEVNAME>_WRITE_CYCLE_MS);

		bytes_written += chunk;
	}

	return 0;
}

/**
 * @brief Initialise the <DEVNAME> EEPROM device.
 * @param desc  - Pointer to EEPROM descriptor pointer (allocated here).
 * @param param - EEPROM initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_init(struct no_os_eeprom_desc **desc,
		       const struct no_os_eeprom_init_param *param)
{
	struct no_os_eeprom_desc *eeprom_desc;
	struct <devname>_init_param *eeprom_ip;
	struct <devname>_dev *dev;
	int ret;

	if (!desc || !param)
		return -EINVAL;

	eeprom_desc = (struct no_os_eeprom_desc *)no_os_calloc(1,
			sizeof(*eeprom_desc));
	if (!eeprom_desc)
		return -ENOMEM;

	eeprom_desc->device_id = param->device_id;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev) {
		ret = -ENOMEM;
		goto free_eeprom_desc;
	}

	eeprom_ip = (struct <devname>_init_param *)param->extra;

	ret = no_os_i2c_init(&dev->i2c_desc, &eeprom_ip->i2c_init);
	if (ret)
		goto free_dev;

	eeprom_desc->extra = dev;
	*desc = eeprom_desc;

	return 0;

free_dev:
	no_os_free(dev);
free_eeprom_desc:
	no_os_free(eeprom_desc);

	return ret;
}

/**
 * @brief Free resources allocated by <devname>_init().
 * @param desc - EEPROM descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_remove(struct no_os_eeprom_desc *desc)
{
	struct <devname>_dev *dev;
	int ret;

	if (!desc || !desc->extra)
		return -EINVAL;

	dev = (struct <devname>_dev *)desc->extra;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);
	no_os_free(desc);

	return 0;
}

/**
 * @brief <DEVNAME> EEPROM platform ops structure.
 */
const struct no_os_eeprom_platform_ops eeprom_<devname>_ops = {
	.init = &<devname>_init,
	.read = &<devname>_read,
	.write = &<devname>_write,
	.remove = &<devname>_remove,
};
```

---

## 5. IIO Header

IIO support is **not typical** for EEPROM drivers.  EEPROMs are storage
devices, not data-acquisition devices, so the IIO subsystem (channels,
scan types, buffered reads) does not apply.

If IIO support is needed for a specific use case (e.g., exposing
EEPROM contents as debug attributes), create `iio_<devname>.h` and
`iio_<devname>.c` following the pattern in the temperature sensor
template, but replace channels with custom debug attributes that
map to read/write operations on the EEPROM address space.

---

## 6. IIO Source

See Section 5.  IIO is not a standard requirement for EEPROM drivers.
Skip this file unless a project-specific need arises.

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
	$(INCLUDE)/no_os_i2c.h       \
	$(INCLUDE)/no_os_eeprom.h    \
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
	$(DRIVERS)/api/no_os_i2c.c  \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_util.c  \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/eeprom/<devname>/<devname>.h
SRCS += $(DRIVERS)/eeprom/<devname>/<devname>.c

INCS += $(INCLUDE)/no_os_eeprom.h
SRCS += $(NO-OS)/util/no_os_eeprom.c
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
extern struct no_os_eeprom_init_param <devname>_ip;

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

static struct <devname>_init_param <devname>_extra = {
	.i2c_init = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 400000,
		.slave_address = <DEVNAME>_ADDR(0),
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA,
	},
};

struct no_os_eeprom_init_param <devname>_ip = {
	.device_id = 0,
	.platform_ops = &eeprom_<devname>_ops,
	.extra = &<devname>_extra,
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
#include "no_os_eeprom.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

#include <string.h>

/** Test data size in bytes. */
#define TEST_DATA_SIZE		32

/** EEPROM address to use for the test. */
#define TEST_ADDRESS		0x0000

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute the write/read test and return 0 on success.
 *******************************************************************************/
int basic_example_main()
{
	struct no_os_eeprom_desc *desc;
	uint8_t write_buf[TEST_DATA_SIZE];
	uint8_t read_buf[TEST_DATA_SIZE];
	uint16_t i;
	int ret;

	pr_info("\r\nRunning <DEVNAME> EEPROM Basic Example\r\n");

	ret = no_os_eeprom_init(&desc, &<devname>_ip);
	if (ret)
		goto error;

	/* Fill write buffer with test pattern. */
	for (i = 0; i < TEST_DATA_SIZE; i++)
		write_buf[i] = (uint8_t)(i & 0xFF);

	/* Write test data. */
	ret = no_os_eeprom_write(desc, TEST_ADDRESS, write_buf, TEST_DATA_SIZE);
	if (ret)
		goto free_dev;

	/* Read back and verify. */
	ret = no_os_eeprom_read(desc, TEST_ADDRESS, read_buf, TEST_DATA_SIZE);
	if (ret)
		goto free_dev;

	if (memcmp(write_buf, read_buf, TEST_DATA_SIZE) != 0) {
		pr_info("Error: data mismatch!\r\n");
		ret = -1;
		goto free_dev;
	}

	pr_info("EEPROM write/read test passed.\r\n");

	no_os_eeprom_remove(desc);

	return 0;

free_dev:
	no_os_eeprom_remove(desc);
error:
	pr_info("Error: %d\r\n", ret);
	return ret;
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
#error No example selected. Please enable BASIC_EXAMPLE and rebuild.
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
#include "maxim_i2c.h"
#include "maxim_gpio.h"

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

struct max_i2c_init_param max_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
```

### 7.13 `projects/<project_name>/src/platform/maxim/platform_src.mk`

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

The <DEVNAME> is an I2C-compatible serial EEPROM with <memory_size>
of non-volatile memory organized as <pages> pages of <page_size> bytes
each.  It operates from a <voltage_range> supply and supports a
<speed> I2C interface.  <Add 2-3 more sentences from the datasheet
covering key specs: write endurance, data retention, write protection
features, address pin configuration.>

Applications
------------

* Non-volatile configuration storage
* Calibration data storage
* User data logging
* Serial number / identification storage
* Board-level manufacturing data

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the EEPROM
descriptor and the device-specific structure, then initialises the
I2C communication interface.  Error handling releases any allocated
resources on failure.  The driver is used through the
``no_os_eeprom_platform_ops`` abstraction.

Reading Data
~~~~~~~~~~~~

The ``<devname>_read`` function sets the 16-bit memory address
(big-endian) and performs a sequential read of the requested number
of bytes.  Sequential reads automatically wrap at the end of the
address space.

Writing Data
~~~~~~~~~~~~

The ``<devname>_write`` function handles page-boundary alignment
automatically.  Data is split into page-sized chunks, and each page
write is followed by a delay of ``<DEVNAME>_WRITE_CYCLE_MS`` to allow
the internal write cycle to complete.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_eeprom.h"

   int main()
   {
       struct <devname>_init_param eeprom_extra = {
           .i2c_init = {
               .device_id     = 1,
               .max_speed_hz  = 400000,
               .slave_address = <DEVNAME>_ADDR(0),
               .platform_ops  = &max_i2c_ops,
               .extra         = &i2c_extra,
           },
       };

       struct no_os_eeprom_init_param eeprom_ip = {
           .device_id = 0,
           .platform_ops = &eeprom_<devname>_ops,
           .extra = &eeprom_extra,
       };

       struct no_os_eeprom_desc *desc;
       uint8_t data[32];
       int ret;

       ret = no_os_eeprom_init(&desc, &eeprom_ip);
       if (ret)
           return ret;

       ret = no_os_eeprom_read(desc, 0x0000, data, sizeof(data));
       if (ret)
           goto cleanup;

       /* data[] now holds the first 32 bytes from EEPROM */

   cleanup:
       no_os_eeprom_remove(desc);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/eeprom/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/eeprom/<devname>/README.rst
```

---

## 9. Key Conventions

1. **Platform ops pattern** -- EEPROM drivers implement the
   `no_os_eeprom_platform_ops` interface (`init`, `read`, `write`,
   `remove`).  Users interact through the `no_os_eeprom_*()` API.
2. **`extra` field** -- the driver-specific `init_param` struct is
   passed via the `extra` field of `no_os_eeprom_init_param`; the
   driver-specific `_dev` struct is stored in the `extra` field of
   `no_os_eeprom_desc`.
3. **I2C communication** -- use `no_os_i2c_write()` and
   `no_os_i2c_read()` with the stop-bit parameter:
   - Write address bytes: `no_os_i2c_write(..., 2, 0)` (no stop for
     read) or include data for write.
   - Read data: `no_os_i2c_read(..., bytes, 1)` (with stop).
   - Write data: `no_os_i2c_write(..., len + 2, 1)` (with stop).
4. **Big-endian addresses** -- EEPROM memory addresses are 16-bit
   big-endian.  Use `<DEVNAME>_ADDR_HIGH()` / `<DEVNAME>_ADDR_LOW()`
   macros or `no_os_put_unaligned_be16()`.
5. **Page-aligned writes** -- writes must not cross page boundaries.
   The driver splits data into page-sized chunks automatically.
6. **Write cycle delay** -- after each page write, wait
   `<DEVNAME>_WRITE_CYCLE_MS` (typically 5 ms per datasheet).
7. **Address macro** -- `<DEVNAME>_ADDR(x)` where `x` is the A2:A1:A0
   pin state (0-7), base address is typically 0x50.
8. **No IIO** -- EEPROM drivers do not use the IIO subsystem.
9. All other conventions (C++ compat with explicit void* casts, tabs,
   Doxygen, `no_os_calloc`/`no_os_free`, goto cleanup, negative error
   codes, etc.) match the standard no-OS driver conventions.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> I2C EEPROM with <memory_size> of
non-volatile storage organized as <pages> pages of <page_size> bytes.

Signed-off-by: Your Name <your.name@analog.com>
```
