# Analog Mux/Switch Driver Template

Reference driver: `drivers/mux/adg2404/`

This template covers every file needed to add a new analog multiplexer or
switch driver to no-OS. Replace `<devname>` with the part number (e.g.,
`adg2404`) and `<DEVNAME>` with its uppercase form (e.g., `ADG2404`)
throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adg2404.h`, `adg2404.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADG2404_H__` |
| Init param struct | `<devname>_init_param` | `adg2404_init_param` |
| Device struct | `<devname>_dev` | `adg2404_dev` |
| Functions | `<devname>_<verb>()` | `adg2404_init()` |
| Channel enum | `<DEVNAME>_<CH>` | `ADG2404_S1` |
| Channel off | `<DEVNAME>_CH_OFF` | `ADG2404_CH_OFF` |

---

## 2. File Checklist

```
drivers/mux/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
    README.rst               # Driver documentation

projects/eval-<devname>/
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
            <platform>/
                main.c
                parameters.h
                parameters.c

doc/sphinx/source/drivers/mux/<devname>.rst   # Sphinx entry
```

Note: IIO files are generally not applicable for analog mux/switch devices
since these are passive routing components with no data conversion or
sampling capability. If a particular device does require IIO support (e.g.,
SPI-controlled muxes with diagnostics registers), follow the IIO patterns
from `drivers/accel/DRIVER_TEMPLATE.md` adapted to the device's feature
set.

---

## 3. Driver Header (`<devname>.h`)

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
#ifndef __<DEVNAME>_H__
#define __<DEVNAME>_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"

/* ---------------- Channel Enumeration ---------------------------------- */

/**
 * @enum <devname>_channel
 * @brief Multiplexer channel selection.
 *
 * Use <DEVNAME>_CH_OFF to disable all channels (high-impedance).
 * <DEVNAME>_S1 through <DEVNAME>_S<N> select the corresponding
 * switch input to be connected to the common output.
 */
enum <devname>_channel {
	<DEVNAME>_CH_OFF = 0,
	<DEVNAME>_S1,
	<DEVNAME>_S2,
	<DEVNAME>_S3,
	<DEVNAME>_S4,
	/* Add more channels as needed for larger muxes. */
};

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 *
 * GPIO-based mux/switch devices use address lines (A0, A1, ...)
 * and an enable line (EN) to control channel selection.
 * Adjust the number of address GPIOs to match the device.
 */
struct <devname>_dev {
	/** GPIO descriptor for address line A0. */
	struct no_os_gpio_desc	*gpio_a0;
	/** GPIO descriptor for address line A1. */
	struct no_os_gpio_desc	*gpio_a1;
	/** GPIO descriptor for enable line. */
	struct no_os_gpio_desc	*gpio_en;
	/*
	 * Add more address GPIOs (gpio_a2, ...) for muxes with
	 * more than 4 channels. Each address bit doubles the
	 * channel count: 1 bit = 2 ch, 2 bits = 4 ch, 3 bits = 8 ch.
	 */
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 *
 * Each GPIO init param struct configures the platform-specific
 * port, pin, pull, and ops for the corresponding control line.
 */
struct <devname>_init_param {
	/** GPIO init param for address line A0. */
	struct no_os_gpio_init_param	gpio_a0;
	/** GPIO init param for address line A1. */
	struct no_os_gpio_init_param	gpio_a1;
	/** GPIO init param for enable line. */
	struct no_os_gpio_init_param	gpio_en;
	/* Add more address GPIO init params as needed. */
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Select a multiplexer channel. */
int <devname>_select_channel(struct <devname>_dev *dev,
			     enum <devname>_channel channel);

/** @brief Enable or disable the multiplexer. */
int <devname>_enable(struct <devname>_dev *dev, bool enable);

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **device,
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
 *   @brief  Implementation of <DEVNAME> Driver.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * ... (same BSD-3-Clause block as header) ...
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "<devname>.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Select the multiplexer channel.
 * @param dev     - Device descriptor.
 * @param channel - Channel to select (<DEVNAME>_CH_OFF disables all,
 *                  <DEVNAME>_S1 through <DEVNAME>_S<N> select a channel).
 * @return 0 on success, negative error code otherwise.
 *
 * The function encodes the channel number into address line GPIO
 * values. If <DEVNAME>_CH_OFF is requested, the mux is disabled
 * via the enable pin. Otherwise the mux is enabled first, then
 * the address lines are driven to the appropriate logic levels.
 */
int <devname>_select_channel(struct <devname>_dev *dev,
			     enum <devname>_channel channel)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (channel > <DEVNAME>_S4)
		return -EINVAL;

	if (channel == <DEVNAME>_CH_OFF)
		return <devname>_enable(dev, false);

	ret = <devname>_enable(dev, true);
	if (ret)
		return ret;

	/*
	 * Encode (channel - 1) into address bits.
	 * A0 = bit 0, A1 = bit 1, etc.
	 * For muxes with more address lines, extend accordingly.
	 */
	ret = no_os_gpio_set_value(dev->gpio_a0, (channel - 1) & 0x01);
	if (ret)
		return ret;

	return no_os_gpio_set_value(dev->gpio_a1,
				    ((channel - 1) >> 1) & 0x01);
}

/**
 * @brief Enable or disable the multiplexer.
 * @param dev    - Device descriptor.
 * @param enable - true to enable, false to disable (high-impedance).
 * @return 0 on success, negative error code otherwise.
 *
 * Check the datasheet for enable polarity. Some devices use an
 * active-low enable pin; adjust the logic accordingly.
 */
int <devname>_enable(struct <devname>_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_en, enable);
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Pointer to initialization parameters.
 * @return 0 on success, negative error code otherwise.
 *
 * Requests all GPIO descriptors, configures them as outputs
 * driven low (mux disabled, channel 0 address), and stores
 * the descriptors in the device structure.
 */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_gpio_get(&dev->gpio_a0, &init_param->gpio_a0);
	if (ret)
		goto error_a0;

	ret = no_os_gpio_get(&dev->gpio_a1, &init_param->gpio_a1);
	if (ret)
		goto error_a1;

	ret = no_os_gpio_get(&dev->gpio_en, &init_param->gpio_en);
	if (ret)
		goto error_en;

	ret = no_os_gpio_direction_output(dev->gpio_a0, NO_OS_GPIO_LOW);
	if (ret)
		goto error_config;

	ret = no_os_gpio_direction_output(dev->gpio_a1, NO_OS_GPIO_LOW);
	if (ret)
		goto error_config;

	ret = no_os_gpio_direction_output(dev->gpio_en, NO_OS_GPIO_LOW);
	if (ret)
		goto error_config;

	*device = dev;

	return 0;

error_config:
	no_os_gpio_remove(dev->gpio_en);
error_en:
	no_os_gpio_remove(dev->gpio_a1);
error_a1:
	no_os_gpio_remove(dev->gpio_a0);
error_a0:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by <devname>_init().
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 *
 * Releases all GPIO descriptors and frees the device structure.
 * Resources are freed in reverse allocation order.
 */
int <devname>_remove(struct <devname>_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_gpio_remove(dev->gpio_en);
	no_os_gpio_remove(dev->gpio_a1);
	no_os_gpio_remove(dev->gpio_a0);

	no_os_free(dev);

	return 0;
}
```

---

## 5. IIO Header

IIO support is generally **not applicable** for analog mux/switch devices.

These devices are passive signal-routing components controlled entirely
through GPIO lines. They do not perform data conversion, sampling, or
measurement, and therefore have no data channels, attributes, or buffer
modes to expose through the IIO subsystem.

If your mux device is SPI-controlled and provides diagnostics registers
(e.g., ADGS1408, ADGS5412), you may optionally add IIO support for
register debug access. In that case, follow the IIO patterns from
`drivers/accel/DRIVER_TEMPLATE.md` adapted to your device.

---

## 6. IIO Source

See Section 5. No IIO source is needed for GPIO-based mux/switch devices.

For SPI-controlled devices with register access, provide at minimum:
- `debug_reg_read` / `debug_reg_write` callbacks
- An empty channel list (no data channels)
- Init/remove functions that wrap the core driver

---

## 7. Project Example

### 7.1 `projects/eval-<devname>/Makefile`

```makefile
# Select the example you want to enable by choosing y for enabling and n for disabling
BASIC_EXAMPLE = y

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
```

### 7.2 `projects/eval-<devname>/builds.json`

```json
{
  "<platform>": {
    "basic_example_<target>": {
      "flags" : "BASIC_EXAMPLE=y TARGET=<target>"
    }
  }
}
```

### 7.3 `projects/eval-<devname>/src.mk`

```makefile
include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

NO_OS_INC_DIRS += \
	$(INCLUDE) \
	$(DRIVERS)/mux/<devname>

INCS += $(INCLUDE)/no_os_delay.h     \
	$(INCLUDE)/no_os_error.h     \
	$(INCLUDE)/no_os_gpio.h      \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_alloc.h     \
	$(INCLUDE)/no_os_irq.h       \
	$(INCLUDE)/no_os_list.h      \
	$(INCLUDE)/no_os_uart.h      \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_irq.c  \
	$(DRIVERS)/api/no_os_dma.c  \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_lf256fifo.c

SRCS += $(DRIVERS)/mux/<devname>/<devname>.c
```

### 7.4 `projects/eval-<devname>/src/examples/examples_src.mk`

```makefile
ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif
```

### 7.5 `projects/eval-<devname>/src/common/common_data.h`

```c
/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-<devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "<devname>.h"
#include "no_os_uart.h"
#include "parameters.h"

extern struct <devname>_init_param <devname>_ip;
extern struct no_os_uart_init_param <devname>_uart_ip;

#endif /* __COMMON_DATA_H__ */
```

### 7.6 `projects/eval-<devname>/src/common/common_data.c`

```c
/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-<devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param <devname>_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct <devname>_init_param <devname>_ip = {
	.gpio_a0 = {
		.port = GPIO_A0_PORT,
		.number = GPIO_A0_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	},
	.gpio_a1 = {
		.port = GPIO_A1_PORT,
		.number = GPIO_A1_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	},
	.gpio_en = {
		.port = GPIO_EN_PORT,
		.number = GPIO_EN_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	}
};
```

### 7.7 `projects/eval-<devname>/src/examples/basic/basic_example.h`

```c
/***************************************************************************//**
 *   @file   basic_example.h
 *   @brief  Basic example header for eval-<devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __BASIC_EXAMPLE_H__
#define __BASIC_EXAMPLE_H__

int example_main(void);

#endif /* __BASIC_EXAMPLE_H__ */
```

### 7.8 `projects/eval-<devname>/src/examples/basic/basic_example.c`

```c
/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for eval-<devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "<devname>.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"

/**
 * @brief Main function for basic example.
 * @return 0 in case of success, negative error code otherwise.
 */
int example_main(void)
{
	int ret;
	int i;
	struct <devname>_dev *<devname>_dev = NULL;
	struct no_os_uart_desc *uart_desc;
	enum <devname>_channel channels[] = {
		<DEVNAME>_S1,
		<DEVNAME>_S2,
		<DEVNAME>_S3,
		<DEVNAME>_S4,
		<DEVNAME>_CH_OFF
	};

	/* Initialize UART */
	ret = no_os_uart_init(&uart_desc, &<devname>_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	/* Initialize mux by creating the descriptors */
	ret = <devname>_init(&<devname>_dev, &<devname>_ip);
	if (ret)
		goto error_uart;

	no_os_udelay(1);

	/* Disable mux to start in a known state */
	ret = <devname>_enable(<devname>_dev, false);
	if (ret)
		goto error;

	no_os_udelay(1);

	/* Enable mux */
	ret = <devname>_enable(<devname>_dev, true);
	if (ret)
		goto error;

	no_os_udelay(1);

	/*
	 * Cycle through all channels to verify the truth table.
	 * Each channel is held for 3 seconds before advancing.
	 */
	for (i = 0; i < (int)NO_OS_ARRAY_SIZE(channels); i++) {
		ret = <devname>_select_channel(<devname>_dev, channels[i]);
		if (ret)
			goto error;
		no_os_mdelay(3000);
	}

	/* Disable mux */
	ret = <devname>_enable(<devname>_dev, false);
	if (ret)
		goto error;

	no_os_udelay(1);

	<devname>_remove(<devname>_dev);
	no_os_uart_remove(uart_desc);
	return 0;

error:
	pr_info("Error!\n");
	<devname>_remove(<devname>_dev);
error_uart:
	no_os_uart_remove(uart_desc);
	return ret;
}
```

### 7.9 `projects/eval-<devname>/src/platform/<platform>/main.c`

```c
/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for <platform> platform of eval-<devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

extern int example_main(void);

int main(void)
{
	return example_main();
}
```

### 7.10 `projects/eval-<devname>/src/platform/<platform>/parameters.h`

```c
/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to <platform> platform used by
 *           eval-<devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_irq.h"

#define UART_DEVICE_ID  0
#define UART_IRQ_ID     UART0_IRQn
#define UART_BAUDRATE   57600
#define UART_EXTRA      &<devname>_uart_extra_ip
#define UART_OPS        &max_uart_ops

/* GPIO Port and Pin definitions for <DEVNAME> */
#define GPIO_A0_PORT    1
#define GPIO_A0_PIN     6
#define GPIO_A1_PORT    1
#define GPIO_A1_PIN     7
#define GPIO_EN_PORT    1
#define GPIO_EN_PIN     8

#define GPIO_OPS        &max_gpio_ops
#define GPIO_EXTRA      &<devname>_gpio_extra_ip

extern struct max_uart_init_param <devname>_uart_extra_ip;
extern struct max_gpio_init_param <devname>_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */
```

### 7.11 `projects/eval-<devname>/src/platform/<platform>/parameters.c`

```c
/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of <platform> platform data used by eval-<devname>
 *           project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "parameters.h"

struct max_uart_init_param <devname>_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_gpio_init_param <devname>_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
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

The <DEVNAME> is a <description, e.g., CMOS 4-channel analog multiplexer
with low on-resistance and fast switching times>. It uses <N> digital
address lines (A0, A1, ...) and an enable pin to select one of <M>
input channels and route it to a single output. <Add 2-3 more sentences
from the datasheet covering key specs such as on-resistance, bandwidth,
supply voltage range, and package options.>

Applications
------------

* Signal routing and channel selection
* Data acquisition front-end multiplexing
* Sensor array switching
* Automatic test equipment (ATE)

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, requests GPIO descriptors for all address lines and the
enable pin, configures them as outputs driven low (mux disabled,
channel address 0), and stores the descriptors in the device
structure. The function uses goto-based cleanup to release GPIOs
in reverse order on failure.

Channel Selection
~~~~~~~~~~~~~~~~~

The ``<devname>_select_channel`` function encodes the desired
channel number into binary values on the address line GPIOs. If
``<DEVNAME>_CH_OFF`` is requested, the mux is disabled via the
enable pin. For any other channel, the mux is first enabled, then
the address lines are driven to the appropriate logic levels.

Enable/Disable
~~~~~~~~~~~~~~

The ``<devname>_enable`` function directly controls the enable
GPIO pin. When disabled, all switches are open and the output is
in a high-impedance state.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_gpio.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .gpio_a0 = { /* platform-specific GPIO params */ },
           .gpio_a1 = { /* platform-specific GPIO params */ },
           .gpio_en = { /* platform-specific GPIO params */ },
       };

       struct <devname>_dev *dev;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       /* Select channel S1 */
       ret = <devname>_select_channel(dev, <DEVNAME>_S1);
       if (ret)
           goto cleanup;

       /* ... perform measurement through the mux ... */

       /* Disable mux when done */
       ret = <devname>_enable(dev, false);
       if (ret)
           goto cleanup;

   cleanup:
       <devname>_remove(dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/mux/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/mux/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int`, negative = error.
5. **Init signature** -- `int <devname>_init(struct <devname>_dev **device,
   struct <devname>_init_param *init_param)` (pointer to init_param
   for GPIO-only devices without unions).
6. **GPIO-based control** -- mux/switch devices typically have no
   register interface. Channel selection is purely through GPIO
   address lines and an enable pin.
7. **Allocation** -- use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
8. **Goto cleanup** -- in `_init()`, use goto labels to free GPIOs in
   reverse allocation order on error. Each GPIO gets its own error
   label for precise cleanup.
9. **Remove order** -- `_remove()` frees GPIOs in reverse order of
   `_init()`, then frees the device struct.
10. **No globals** -- all state lives in the device descriptor struct.
11. **No IIO by default** -- mux/switch devices are passive routing
    components. Only add IIO support when the device has SPI/I2C
    registers providing diagnostics or status information.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> analog multiplexer/switch with GPIO-based
channel selection and enable control.

Signed-off-by: Your Name <your.name@analog.com>
```
