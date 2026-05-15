# IO-Link Driver Template

Reference driver: `drivers/io-link/max22516/`

This template covers every file needed to add a new IO-Link transceiver
driver to no-OS. Replace `<devname>` with the part number (e.g.,
`max22516`) and `<DEVNAME>` with its uppercase form (e.g., `MAX22516`)
throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `max22516.h`, `max22516.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __MAX22516_H__` |
| Init param struct | `<devname>_init_param` | `max22516_init_param` |
| Device struct | `<devname>_dev` | `max22516_dev` |
| Functions | `<devname>_<verb>()` | `max22516_init()` |
| Static helpers | `_<devname>_<verb>()` | `_max22516_build_frame()` |
| Register addresses | `<DEVNAME>_REG_<NAME>` | `MAX22516_REG_CHIP_ID` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `MAX22516_CQ_CTRL1_CQ_EN_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `MAX22516_CQ_MODE_PP` |

---

## 2. File Checklist

```
drivers/io-link/<devname>/
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

doc/sphinx/source/drivers/io-link/<devname>.rst   # Sphinx entry
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
#include "no_os_util.h"

/* ---------------- Register Map ----------------------------------------- */

#define <DEVNAME>_REG_CHIP_ID		0x00
#define <DEVNAME>_REG_REV_ID		0x01
#define <DEVNAME>_REG_STATUS		0x02
#define <DEVNAME>_REG_IOL_CFG		0x03
#define <DEVNAME>_REG_INT		0x04
#define <DEVNAME>_REG_INT_EN		0x05
#define <DEVNAME>_REG_CQ_CTRL		0x06
#define <DEVNAME>_REG_DO_CTRL		0x07
#define <DEVNAME>_REG_TX_CTRL		0x08
#define <DEVNAME>_REG_EVENT_FLAG	0x09

/* ---------------- Field Masks ------------------------------------------ */

/** Use NO_OS_BIT() for single-bit fields, NO_OS_GENMASK(hi, lo) for
 *  multi-bit fields. */
#define <DEVNAME>_CQ_CTRL_EN_MSK	NO_OS_BIT(0)
#define <DEVNAME>_CQ_CTRL_PP_MSK	NO_OS_BIT(2)
#define <DEVNAME>_CQ_CTRL_NPN_MSK	NO_OS_BIT(3)
#define <DEVNAME>_DO_CTRL_EN_MSK	NO_OS_BIT(0)
#define <DEVNAME>_DO_CTRL_PP_MSK	NO_OS_BIT(2)
#define <DEVNAME>_TX_CTRL_CQTX_MSK	NO_OS_BIT(7)
#define <DEVNAME>_TX_CTRL_DOTX_MSK	NO_OS_BIT(4)
#define <DEVNAME>_EVENT_FLG_MSK		NO_OS_BIT(0)

/* Product / revision ID expected values */
#define <DEVNAME>_CHIP_ID		0xXX

/* SPI protocol definitions */
#define <DEVNAME>_SPI_READ_CMD		NO_OS_BIT(7)
#define <DEVNAME>_SPI_DUMMY_DATA	0x00
#define <DEVNAME>_BUFF_SIZE_BYTES	64

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_cq_mode
 *  @brief CQ output driver configuration modes. */
enum <devname>_cq_mode {
	<DEVNAME>_CQ_MODE_DISABLED,
	<DEVNAME>_CQ_MODE_PP,
	<DEVNAME>_CQ_MODE_PNP,
	<DEVNAME>_CQ_MODE_NPN,
};

/** @enum <devname>_do_mode
 *  @brief DO output driver configuration modes. */
enum <devname>_do_mode {
	<DEVNAME>_DO_MODE_DISABLED,
	<DEVNAME>_DO_MODE_PP,
	<DEVNAME>_DO_MODE_PNP,
	<DEVNAME>_DO_MODE_NPN,
};

/* Add more enums as needed: watchdog modes, LED modes, etc. */

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** SPI initialization parameters. */
	struct no_os_spi_init_param *spi_init;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** Buffer used for SPI communication. */
	uint8_t comm_buff[<DEVNAME>_BUFF_SIZE_BYTES];
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Write a register. */
int <devname>_write(struct <devname>_dev *dev, uint8_t reg_addr,
		    uint8_t data);

/** @brief Read a register. */
int <devname>_read(struct <devname>_dev *dev, uint8_t reg_addr,
		   uint8_t *data);

/** @brief Read-modify-write a register field. */
int <devname>_update(struct <devname>_dev *dev, uint8_t reg_addr,
		     uint8_t mask, uint8_t data);

/** @brief Burst-write consecutive registers. */
int <devname>_burst_write(struct <devname>_dev *dev, uint8_t reg_addr,
			  uint8_t count, uint8_t *data);

/** @brief Burst-read consecutive registers. */
int <devname>_burst_read(struct <devname>_dev *dev, uint8_t reg_addr,
			 uint8_t count, uint8_t *data);

/** @brief Configure the CQ output driver mode. */
int <devname>_setup_cq(struct <devname>_dev *dev,
		       enum <devname>_cq_mode mode);

/** @brief Configure the DO output driver mode. */
int <devname>_setup_do(struct <devname>_dev *dev,
		       enum <devname>_do_mode mode);

/** @brief Set an IO-Link event (qualifier + code). */
int <devname>_set_event(struct <devname>_dev *dev, uint8_t ev_qual,
			uint16_t ev_code);

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
#include "no_os_error.h"
#include "no_os_util.h"

/* ---------- SPI register access ------------------------------------ */

/**
 * @brief Write a register over SPI.
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param data     - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_write(struct <devname>_dev *dev, uint8_t reg_addr,
		    uint8_t data)
{
	dev->comm_buff[0] = reg_addr;
	dev->comm_buff[1] = data;

	return no_os_spi_write_and_read(dev->spi_desc, dev->comm_buff, 2);
}

/**
 * @brief Read a register over SPI.
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param data     - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_read(struct <devname>_dev *dev, uint8_t reg_addr,
		   uint8_t *data)
{
	int ret;

	dev->comm_buff[0] = <DEVNAME>_SPI_READ_CMD | reg_addr;
	dev->comm_buff[1] = <DEVNAME>_SPI_DUMMY_DATA;

	ret = no_os_spi_write_and_read(dev->spi_desc, dev->comm_buff, 2);
	if (ret)
		return ret;

	*data = dev->comm_buff[1];

	return 0;
}

/**
 * @brief Read-modify-write a register field.
 * @param dev      - Device descriptor.
 * @param reg_addr - Register address.
 * @param mask     - Bit mask of the field to update.
 * @param data     - New field value (pre-shifted or use no_os_field_prep).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_update(struct <devname>_dev *dev, uint8_t reg_addr,
		     uint8_t mask, uint8_t data)
{
	uint8_t read_val;
	int ret;

	ret = <devname>_read(dev, reg_addr, &read_val);
	if (ret)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	return <devname>_write(dev, reg_addr, read_val);
}

/**
 * @brief Burst-write consecutive registers.
 * @param dev      - Device descriptor.
 * @param reg_addr - Starting register address.
 * @param count    - Number of bytes to write.
 * @param data     - Data buffer to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_burst_write(struct <devname>_dev *dev, uint8_t reg_addr,
			  uint8_t count, uint8_t *data)
{
	uint8_t i;

	dev->comm_buff[0] = reg_addr;

	for (i = 0; i < count; i++)
		dev->comm_buff[1 + i] = data[i];

	return no_os_spi_write_and_read(dev->spi_desc, dev->comm_buff,
					count + 1);
}

/**
 * @brief Burst-read consecutive registers.
 * @param dev      - Device descriptor.
 * @param reg_addr - Starting register address.
 * @param count    - Number of bytes to read.
 * @param data     - Buffer to store read data.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_burst_read(struct <devname>_dev *dev, uint8_t reg_addr,
			 uint8_t count, uint8_t *data)
{
	uint8_t i;
	int ret;

	dev->comm_buff[0] = <DEVNAME>_SPI_READ_CMD | reg_addr;

	ret = no_os_spi_write_and_read(dev->spi_desc, dev->comm_buff,
				       count + 1);
	if (ret)
		return ret;

	for (i = 0; i < count; i++)
		data[i] = dev->comm_buff[1 + i];

	return 0;
}

/* ---------- IO-Link output driver configuration -------------------- */

/**
 * @brief Configure the CQ output driver mode.
 * @param dev  - Device descriptor.
 * @param mode - CQ driver mode (disabled, push-pull, PNP, NPN).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_setup_cq(struct <devname>_dev *dev,
		       enum <devname>_cq_mode mode)
{
	uint8_t reg_val;

	switch (mode) {
	case <DEVNAME>_CQ_MODE_DISABLED:
		reg_val = 0;
		break;
	case <DEVNAME>_CQ_MODE_PP:
		reg_val = <DEVNAME>_CQ_CTRL_EN_MSK | <DEVNAME>_CQ_CTRL_PP_MSK;
		break;
	case <DEVNAME>_CQ_MODE_PNP:
		reg_val = <DEVNAME>_CQ_CTRL_EN_MSK;
		break;
	case <DEVNAME>_CQ_MODE_NPN:
		reg_val = <DEVNAME>_CQ_CTRL_EN_MSK | <DEVNAME>_CQ_CTRL_NPN_MSK;
		break;
	default:
		return -EINVAL;
	}

	return <devname>_write(dev, <DEVNAME>_REG_CQ_CTRL, reg_val);
}

/**
 * @brief Configure the DO output driver mode.
 * @param dev  - Device descriptor.
 * @param mode - DO driver mode (disabled, push-pull, PNP, NPN).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_setup_do(struct <devname>_dev *dev,
		       enum <devname>_do_mode mode)
{
	uint8_t reg_val;

	switch (mode) {
	case <DEVNAME>_DO_MODE_DISABLED:
		reg_val = 0;
		break;
	case <DEVNAME>_DO_MODE_PP:
		reg_val = <DEVNAME>_DO_CTRL_EN_MSK | <DEVNAME>_DO_CTRL_PP_MSK;
		break;
	case <DEVNAME>_DO_MODE_PNP:
		reg_val = <DEVNAME>_DO_CTRL_EN_MSK;
		break;
	case <DEVNAME>_DO_MODE_NPN:
		reg_val = <DEVNAME>_DO_CTRL_EN_MSK;
		break;
	default:
		return -EINVAL;
	}

	return <devname>_write(dev, <DEVNAME>_REG_DO_CTRL, reg_val);
}

/* ---------- IO-Link event handling --------------------------------- */

/**
 * @brief Set an IO-Link event (qualifier + event code).
 * @param dev     - Device descriptor.
 * @param ev_qual - Event qualifier byte.
 * @param ev_code - 16-bit event code.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_event(struct <devname>_dev *dev, uint8_t ev_qual,
			uint16_t ev_code)
{
	int ret;

	/* Write event qualifier and code registers, then set the
	 * event flag to signal the IO-Link master. */
	ret = <devname>_write(dev, <DEVNAME>_REG_EVENT_QUAL, ev_qual);
	if (ret)
		return ret;

	ret = <devname>_write(dev, <DEVNAME>_REG_EVENT_CODE_MSB,
			      (uint8_t)(ev_code >> 8));
	if (ret)
		return ret;

	ret = <devname>_write(dev, <DEVNAME>_REG_EVENT_CODE_LSB,
			      (uint8_t)(ev_code & 0xFF));
	if (ret)
		return ret;

	return <devname>_write(dev, <DEVNAME>_REG_EVENT_FLAG,
			       <DEVNAME>_EVENT_FLG_MSK);
}

/* ---------- Init / Remove ------------------------------------------ */

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
	uint8_t chip_id;
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = no_os_spi_init(&d->spi_desc, init_param->spi_init);
	if (ret)
		goto free_dev;

	/* Verify chip ID */
	ret = <devname>_read(d, <DEVNAME>_REG_CHIP_ID, &chip_id);
	if (ret)
		goto free_spi;

	if (chip_id != <DEVNAME>_CHIP_ID) {
		pr_err("Unexpected chip ID: 0x%02X\n", chip_id);
		ret = -ENODEV;
		goto free_spi;
	}

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

## 5. IIO Support

IIO is generally **not applicable** for IO-Link transceivers. These
devices act as protocol-level interfaces between an IO-Link master and
a field-level sensor/actuator. They do not produce sampled data streams
the way ADCs or DACs do, so there is no natural mapping to IIO channels.

If your specific device exposes measurable quantities (e.g., on-chip
temperature, supply voltage monitoring), you may add an IIO layer
following the ADC template pattern, but this is the exception, not the
rule.

---

## 6. (Reserved for IIO Source)

See Section 5 — not typically needed for IO-Link transceivers.

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

INCS += $(DRIVERS)/io-link/<devname>/<devname>.h
SRCS += $(DRIVERS)/io-link/<devname>/<devname>.c
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
	.spi_init = &<devname>_spi_ip,
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
	uint8_t chip_id;
	uint8_t status;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	ret = <devname>_read(dev, <DEVNAME>_REG_CHIP_ID, &chip_id);
	if (ret)
		goto free_dev;

	pr_info("Chip ID: 0x%02X\r\n", chip_id);

	/* Configure CQ output as push-pull */
	ret = <devname>_setup_cq(dev, <DEVNAME>_CQ_MODE_PP);
	if (ret)
		goto free_dev;

	while (1) {
		ret = <devname>_read(dev, <DEVNAME>_REG_STATUS, &status);
		if (ret)
			goto free_dev;

		pr_info("Status: 0x%02X\r\n", status);
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

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;

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

The <DEVNAME> is an IO-Link device transceiver with an SPI-compatible
configuration interface. It implements the IO-Link physical layer (PHY)
and provides configurable CQ and DO output drivers for communication
with an IO-Link master. <Add 2-3 more sentences from the datasheet
covering key features such as supported baud rates (COM1/COM2/COM3),
integrated LED drivers, watchdog, and diagnostic capabilities.>

Applications
------------

* IO-Link sensor interfaces
* Industrial field-level communication
* Factory automation

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface, and verifies the chip ID
by reading the identification register. The function uses goto-based
cleanup to release resources on failure.

CQ / DO Output Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_setup_cq`` and ``<devname>_setup_do`` functions
configure the CQ (communication/quality) and DO (digital output)
driver modes. Supported modes include disabled, push-pull (PP),
PNP, and NPN.

Event Handling
~~~~~~~~~~~~~~

The ``<devname>_set_event`` function programs an IO-Link event by
writing the event qualifier, event code, and setting the event flag
to signal the IO-Link master.

Register Access
~~~~~~~~~~~~~~~

The driver provides ``<devname>_read``, ``<devname>_write``, and
``<devname>_update`` functions for individual register access, as
well as ``<devname>_burst_read`` and ``<devname>_burst_write`` for
multi-byte transfers (e.g., process data FIFO access).

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct no_os_spi_init_param spi_ip = {
           /* platform-specific SPI params */
       };

       struct <devname>_init_param ip = {
           .spi_init = &spi_ip,
       };

       struct <devname>_dev *dev;
       uint8_t chip_id;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_read(dev, <DEVNAME>_REG_CHIP_ID, &chip_id);
       if (ret)
           goto cleanup;

       /* Configure CQ output as push-pull */
       ret = <devname>_setup_cq(dev, <DEVNAME>_CQ_MODE_PP);
       if (ret)
           goto cleanup;

       /* chip_id and CQ output are now configured */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/io-link/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/io-link/<devname>/README.rst
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
   allocation order on error.
8. **Remove order** -- `_remove()` frees resources in reverse order of
   `_init()`.
9. **No globals** -- all state lives in the device descriptor struct.
10. **Register macros** -- `NO_OS_BIT()` for single bits,
    `NO_OS_GENMASK(hi, lo)` for fields; `no_os_field_prep()` /
    `no_os_field_get()` for field access.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> IO-Link device transceiver with
SPI-compatible configuration interface. The driver provides CQ/DO
output configuration, event handling, and register access.

Signed-off-by: Your Name <your.name@analog.com>
```
