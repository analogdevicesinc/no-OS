# Analog Switch Driver Template

Reference driver: `drivers/switch/adg1712/`

This template covers every file needed to add a new analog switch driver to
no-OS. Replace `<devname>` with the part number (e.g., `adg1712`) and
`<DEVNAME>` with its uppercase form (e.g., `ADG1712`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adg1712.h`, `adg1712.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_adg1712.h`, `iio_adg1712.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADG1712_H__` |
| Init param struct | `<devname>_init_param` | `adg1712_init_param` |
| Device struct | `<devname>_dev` | `adg1712_dev` |
| IIO descriptor | `<devname>_iio_dev` | `adg1712_iio_dev` |
| Functions | `<devname>_<verb>()` | `adg1712_init()` |
| Static helpers | `_<devname>_<verb>()` | `_adg1712_get_gpio()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADG1712` |
| Switch enum | `<DEVNAME>_SW<N>` | `ADG1712_SW1` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADG1712_SW1` |

---

## 2. File Checklist

```
drivers/switch/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
    iio_<devname>.h          # IIO header (uncommon for switches)
    iio_<devname>.c          # IIO implementation (uncommon for switches)
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
        platform/
            <platform>/
                main.c
                parameters.h
                parameters.c
                platform_src.mk

doc/sphinx/source/drivers/switch/<devname>.rst   # Sphinx entry
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
#include "no_os_gpio.h"

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_chip_id
 *  @brief Supported device IDs. */
enum <devname>_chip_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_switch
 *  @brief Switch channel selection. */
enum <devname>_switch {
	<DEVNAME>_SW1,
	<DEVNAME>_SW2,
	<DEVNAME>_SW3,
	<DEVNAME>_SW4,
};

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** Device variant. */
	enum <devname>_chip_id chip_id;
	/** GPIO descriptors for each switch control input. */
	struct no_os_gpio_desc *gpio_in1;
	struct no_os_gpio_desc *gpio_in2;
	struct no_os_gpio_desc *gpio_in3;
	struct no_os_gpio_desc *gpio_in4;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Device variant. */
	enum <devname>_chip_id chip_id;
	/** GPIO init params for each switch control input. */
	struct no_os_gpio_init_param gpio_in1;
	struct no_os_gpio_init_param gpio_in2;
	struct no_os_gpio_init_param gpio_in3;
	struct no_os_gpio_init_param gpio_in4;
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Set the state (on/off) of a specific switch. */
int <devname>_set_switch_state(struct <devname>_dev *dev,
			       enum <devname>_switch sw,
			       bool enable);

/** @brief Get the current state of a specific switch. */
int <devname>_get_switch_state(struct <devname>_dev *dev,
			       enum <devname>_switch sw,
			       bool *enable);

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
 *   @brief  Implementation of <DEVNAME> Driver
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * Copyright YYYY(c) Analog Devices, Inc.
 *
 * ... (same BSD-3-Clause block as header) ...
 *******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "<devname>.h"

/* ---------- Static helpers ----------------------------------------- */

/**
 * @brief Map a switch enum to the corresponding GPIO descriptor.
 * @param dev - Device descriptor.
 * @param sw  - Switch to look up.
 * @return Pointer to the GPIO descriptor, or NULL if invalid.
 */
static struct no_os_gpio_desc *_<devname>_get_gpio(struct <devname>_dev *dev,
						   enum <devname>_switch sw)
{
	switch (sw) {
	case <DEVNAME>_SW1:
		return dev->gpio_in1;
	case <DEVNAME>_SW2:
		return dev->gpio_in2;
	case <DEVNAME>_SW3:
		return dev->gpio_in3;
	case <DEVNAME>_SW4:
		return dev->gpio_in4;
	default:
		return NULL;
	}
}

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Set the state of a specific switch.
 * @param dev    - Device descriptor.
 * @param sw     - Switch to control (<DEVNAME>_SW1 to <DEVNAME>_SW4).
 * @param enable - true to close (enable) the switch, false to open (disable).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_switch_state(struct <devname>_dev *dev,
			       enum <devname>_switch sw,
			       bool enable)
{
	struct no_os_gpio_desc *gpio;

	if (!dev)
		return -EINVAL;

	gpio = _<devname>_get_gpio(dev, sw);
	if (!gpio)
		return -EINVAL;

	return no_os_gpio_set_value(gpio, enable);
}

/**
 * @brief Get the current state of a specific switch.
 * @param dev    - Device descriptor.
 * @param sw     - Switch to read (<DEVNAME>_SW1 to <DEVNAME>_SW4).
 * @param enable - Pointer to store the current state (true if closed).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_switch_state(struct <devname>_dev *dev,
			       enum <devname>_switch sw,
			       bool *enable)
{
	struct no_os_gpio_desc *gpio;
	uint8_t value;
	int ret;

	if (!dev || !enable)
		return -EINVAL;

	gpio = _<devname>_get_gpio(dev, sw);
	if (!gpio)
		return -EINVAL;

	ret = no_os_gpio_get_value(gpio, &value);
	if (ret)
		return ret;

	*enable = (value == NO_OS_GPIO_HIGH);

	return 0;
}

/**
 * @brief Initialise the <DEVNAME> device.
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Pointer to initialization parameters.
 * @return 0 on success, negative error code otherwise.
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

	ret = no_os_gpio_get(&dev->gpio_in1, &init_param->gpio_in1);
	if (ret)
		goto error_dev;

	ret = no_os_gpio_get(&dev->gpio_in2, &init_param->gpio_in2);
	if (ret)
		goto error_gpio1;

	ret = no_os_gpio_get(&dev->gpio_in3, &init_param->gpio_in3);
	if (ret)
		goto error_gpio2;

	ret = no_os_gpio_get(&dev->gpio_in4, &init_param->gpio_in4);
	if (ret)
		goto error_gpio3;

	/* Configure all GPIOs as outputs, default to switch disabled (LOW). */
	ret = no_os_gpio_direction_output(dev->gpio_in1, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio4;

	ret = no_os_gpio_direction_output(dev->gpio_in2, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio4;

	ret = no_os_gpio_direction_output(dev->gpio_in3, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio4;

	ret = no_os_gpio_direction_output(dev->gpio_in4, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio4;

	dev->chip_id = init_param->chip_id;

	*device = dev;

	return 0;

error_gpio4:
	no_os_gpio_remove(dev->gpio_in4);
error_gpio3:
	no_os_gpio_remove(dev->gpio_in3);
error_gpio2:
	no_os_gpio_remove(dev->gpio_in2);
error_gpio1:
	no_os_gpio_remove(dev->gpio_in1);
error_dev:
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
	if (!dev)
		return -EINVAL;

	no_os_gpio_remove(dev->gpio_in4);
	no_os_gpio_remove(dev->gpio_in3);
	no_os_gpio_remove(dev->gpio_in2);
	no_os_gpio_remove(dev->gpio_in1);
	no_os_free(dev);

	return 0;
}
```

---

## 5. IIO Header (`iio_<devname>.h`)

> **Note:** IIO support is uncommon for analog switch drivers. Switches are
> simple GPIO-controlled on/off devices with no ADC/DAC data channels, so
> IIO is typically not needed. If your application requires IIO-based remote
> switch control, the skeleton below can be used as a starting point.

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
	struct <devname>_dev *<devname>_dev;
	/** IIO device structure. */
	struct iio_device *iio_dev;
};

/**
 * @struct <devname>_iio_dev_init_param
 * @brief  IIO initialization parameters.
 */
struct <devname>_iio_dev_init_param {
	/** Core driver init params. */
	struct <devname>_init_param *<devname>_dev_init;
};

/** @brief Initialise the IIO descriptor. */
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_dev_init_param *init_param);

/** @brief Free IIO descriptor resources. */
int <devname>_iio_remove(struct <devname>_iio_dev *desc);

#endif /* __IIO_<DEVNAME>_H__ */
```

---

## 6. IIO Source (`iio_<devname>.c`)

> **Note:** Most analog switch drivers do not require IIO support. The
> skeleton below exposes each switch as an `IIO_VOLTAGE` output channel
> with a boolean `raw` attribute (0 = open, 1 = closed). Only implement
> this if remote IIO-based switch control is required.

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
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "iio_<devname>.h"
#include "<devname>.h"

/* TODO: Implement IIO attributes and channels if needed.
 * For most switch drivers, IIO is not used. See Section 5 note. */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 */
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_dev_init_param *init_param)
{
	struct <devname>_iio_dev *d;
	int ret;

	d = (struct <devname>_iio_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = <devname>_init(&d-><devname>_dev,
			     init_param-><devname>_dev_init);
	if (ret)
		goto free_desc;

	*iio_dev = d;

	return 0;

free_desc:
	no_os_free(d);

	return ret;
}

/**
 * @brief Free IIO descriptor resources.
 */
int <devname>_iio_remove(struct <devname>_iio_dev *desc)
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

### 7.1 `projects/eval-<devname>/Makefile`

```makefile
EXAMPLE ?= basic

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
```

### 7.2 `projects/eval-<devname>/builds.json`

```json
{
  "<platform>": {
    "basic_example_<target>": {
      "flags" : "EXAMPLE=basic TARGET=<target>"
    }
  }
}
```

### 7.3 `projects/eval-<devname>/src.mk`

```makefile
NO_OS_INC_DIRS += \
		$(INCLUDE) \
		$(DRIVERS)/switch/<devname>

SRCS += $(DRIVERS)/api/no_os_gpio.c \
		$(NO-OS)/util/no_os_lf256fifo.c \
		$(DRIVERS)/api/no_os_irq.c \
		$(NO-OS)/util/no_os_list.c \
		$(DRIVERS)/api/no_os_uart.c \
		$(NO-OS)/util/no_os_util.c \
		$(NO-OS)/util/no_os_alloc.c \
		$(NO-OS)/util/no_os_mutex.c

SRCS += $(DRIVERS)/switch/<devname>/<devname>.c
```

### 7.4 `projects/eval-<devname>/src/common/common_data.h`

```c
/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-<devname> examples.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_uart.h"
#include "<devname>.h"

extern struct no_os_uart_init_param <devname>_uart_ip;
extern struct <devname>_init_param <devname>_init_param;

#endif /* __COMMON_DATA_H__ */
```

### 7.5 `projects/eval-<devname>/src/common/common_data.c`

```c
/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-<devname> examples.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "common_data.h"
#include "no_os_gpio.h"

struct no_os_uart_init_param <devname>_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct <devname>_init_param <devname>_init_param = {
	.chip_id = ID_<DEVNAME>,
	.gpio_in1 = {
		.port = GPIO_SW_PORT_NUM,
		.number = GPIO_SW1_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in2 = {
		.port = GPIO_SW_PORT_NUM,
		.number = GPIO_SW2_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in3 = {
		.port = GPIO_SW_PORT_NUM,
		.number = GPIO_SW3_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in4 = {
		.port = GPIO_SW_PORT_NUM,
		.number = GPIO_SW4_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	}
};
```

### 7.6 `projects/eval-<devname>/src/examples/basic/basic_example.c`

```c
/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example code for eval-<devname> project
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

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
int example_main()
{
	struct <devname>_dev *dev;
	struct no_os_uart_desc *uart_desc;
	enum <devname>_switch sw;
	enum <devname>_switch i;
	bool enabled;
	int ret;

	ret = no_os_uart_init(&uart_desc, &<devname>_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = <devname>_init(&dev, &<devname>_init_param);
	if (ret) {
		pr_err("Failed to initialize <DEVNAME>: %d\r\n", ret);
		goto exit_uart;
	}

	pr_info("<DEVNAME> initialized successfully\r\n");

	while (1) {
		for (sw = <DEVNAME>_SW1; sw <= <DEVNAME>_SW4; sw++) {

			ret = <devname>_set_switch_state(dev, sw, true);
			if (ret) {
				pr_err("Failed to enable switch %d: %d\r\n",
				       sw + 1, ret);
				goto exit;
			}

			for (i = <DEVNAME>_SW1; i <= <DEVNAME>_SW4; i++) {
				ret = <devname>_get_switch_state(dev, i,
								 &enabled);
				if (ret) {
					pr_err("Failed to get switch %d "
					       "state: %d\r\n", i + 1, ret);
					goto exit;
				}
				pr_info("SW%d:%d ", i + 1, enabled);
			}
			pr_info("\r\n");

			no_os_mdelay(1000);

			ret = <devname>_set_switch_state(dev, sw, false);
			if (ret) {
				pr_err("Failed to disable switch %d: %d\r\n",
				       sw + 1, ret);
				goto exit;
			}

		}
	}

exit:
	<devname>_remove(dev);
exit_uart:
	no_os_uart_remove(uart_desc);
	if (ret)
		pr_err("Example terminated with error: %d\r\n", ret);
	return ret;
}
```

### 7.7 `projects/eval-<devname>/src/platform/maxim/main.c`

```c
/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-<devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

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

### 7.8 `projects/eval-<devname>/src/platform/maxim/parameters.h`

```c
/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-<devname>
 *           project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_EXTRA      &<devname>_uart_extra_ip
#define UART_OPS        &max_uart_ops

#define GPIO_OPS    &max_gpio_ops
#define GPIO_EXTRA  &<devname>_gpio_extra_ip

extern struct max_uart_init_param <devname>_uart_extra_ip;
extern struct max_gpio_init_param <devname>_gpio_extra_ip;

#define GPIO_SW_PORT_NUM    0
#define GPIO_SW1_PIN_NUM    31
#define GPIO_SW2_PIN_NUM    25
#define GPIO_SW3_PIN_NUM    24
#define GPIO_SW4_PIN_NUM    30

#endif /* __PARAMETERS_H__ */
```

### 7.9 `projects/eval-<devname>/src/platform/maxim/parameters.c`

```c
/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by eval-<devname> project.
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

### 7.10 `projects/eval-<devname>/src/platform/maxim/platform_src.mk`

```makefile
INCS += $(PLATFORM_DRIVERS)/maxim_gpio.h      \
	$(PLATFORM_DRIVERS)/maxim_irq.h       \
	$(PLATFORM_DRIVERS)/maxim_uart.h      \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
	$(PLATFORM_DRIVERS)/maxim_gpio.c      \
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

The <DEVNAME> is a low-voltage, low on-resistance quad SPST (Single Pole
Single Throw) analog switch designed for applications requiring
high-performance signal routing with minimal signal distortion.

<Add details from the datasheet: supply voltage range, on-resistance,
leakage current, and any relevant specifications.>

Both devices operate from a single supply and are controlled via digital
GPIO inputs. Each input (IN1..IN4) controls the corresponding switch pair
(S1-D1, S2-D2, S3-D3, S4-D4). A logic high closes the switch; a logic
low opens it.

Applications
------------

* Audio and video signal routing
* Communications systems
* Test and measurement equipment
* Data acquisition systems
* Instrumentation

<DEVNAME> Device Configuration
-------------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, requests GPIO descriptors for each switch control input
(IN1..IN4), configures them as outputs defaulting to LOW (all switches
open), and stores the chip ID. The function uses goto-based cleanup to
release resources on failure.

Switch Control
~~~~~~~~~~~~~~

The ``<devname>_set_switch_state`` function sets the GPIO output for a
given switch to HIGH (close) or LOW (open).

The ``<devname>_get_switch_state`` function reads the GPIO output value
for a given switch and returns its current state as a boolean.

<DEVNAME> Driver Initialization Example
-----------------------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_gpio.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .chip_id = ID_<DEVNAME>,
           .gpio_in1 = {
               .number = 1,
               .platform_ops = &gpio_ops,
           },
           .gpio_in2 = {
               .number = 2,
               .platform_ops = &gpio_ops,
           },
           .gpio_in3 = {
               .number = 3,
               .platform_ops = &gpio_ops,
           },
           .gpio_in4 = {
               .number = 4,
               .platform_ops = &gpio_ops,
           },
       };

       struct <devname>_dev *dev;
       bool enabled;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       /* Close switch 1 */
       ret = <devname>_set_switch_state(dev, <DEVNAME>_SW1, true);
       if (ret)
           goto cleanup;

       /* Read switch 1 state */
       ret = <devname>_get_switch_state(dev, <DEVNAME>_SW1, &enabled);
       if (ret)
           goto cleanup;

       /* enabled == true */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/switch/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/switch/<devname>/README.rst
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
   since switches use GPIO only, no SPI/I2C unions needed).
6. **GPIO-only interface** -- switch drivers are controlled entirely
   through GPIO pins (no SPI/I2C registers). Each switch channel maps
   to one GPIO output: HIGH = switch closed, LOW = switch open.
7. **Allocation** -- use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
8. **Goto cleanup** -- in `_init()`, use goto labels to free GPIOs in
   reverse allocation order on error.
9. **Remove order** -- `_remove()` frees resources in reverse order of
   `_init()`.
10. **No globals** -- all state lives in the device descriptor struct.
11. **No register macros** -- switch drivers typically have no registers;
    `NO_OS_BIT()` / `NO_OS_GENMASK()` are only needed if the device has
    a register-based interface (e.g., SPI-controlled multiplexers).

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> low-voltage quad SPST analog switch
with GPIO-based control interface.

Signed-off-by: Your Name <your.name@analog.com>
```
