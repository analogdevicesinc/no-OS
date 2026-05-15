# Machine Control Signal (MCS) Driver Template

Reference driver: `drivers/mcs/mcs_gpio/`

This template covers every file needed to add a new machine control signal
driver to no-OS. Replace `<devname>` with the part number (e.g., `mcs_gpio`)
and `<DEVNAME>` with its uppercase form (e.g., `MCS_GPIO`) throughout.

---

## 1. Naming Conventions

MCS drivers use `_dev` for the device descriptor struct and interact
with GPIOs and the JESD204 framework rather than register-based SPI/I2C
peripherals.

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `mcs_gpio.h`, `mcs_gpio.c` |
| Include guard | `#ifndef <DEVNAME>_H_` | `#ifndef MCS_GPIO_H_` |
| Init param struct | `<devname>_init_param` | `mcs_gpio_init_param` |
| Device struct | `<devname>_dev` | `mcs_gpio_dev` |
| Functions | `<devname>_<verb>()` | `mcs_gpio_init()` |
| JESD204 priv struct | `<devname>_jesd204_priv` | `mcs_gpio_jesd204_priv` |
| JESD204 data | `jesd204_<devname>_init` | `jesd204_mcs_gpio_init` |
| Signal defines | `<DEVNAME>_<SIGNAL>` | `MCS_GPIO_SYNC_REQ` |

---

## 2. File Checklist

```
drivers/mcs/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
    README.rst               # Driver documentation

doc/sphinx/source/drivers/mcs/<devname>.rst   # Sphinx entry
```

MCS drivers typically do not include IIO files. The driver provides
GPIO-based synchronisation signals for JESD204 topologies rather than
exposing measurement channels.

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
#ifndef <DEVNAME>_H_
#define <DEVNAME>_H_

#include <stdbool.h>
#include <stdint.h>

/* Forward declarations */
struct no_os_gpio_desc;
struct no_os_gpio_init_param;
struct jesd204_dev;

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** GPIO descriptor for the MCS signal. */
	struct no_os_gpio_desc *gpio_req;
	/** JESD204 device handle (registered during init). */
	struct jesd204_dev *jdev;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** GPIO init parameters for the MCS signal line. */
	struct no_os_gpio_init_param *gpio_req;
};

/* Initialize the device. */
int32_t <devname>_init(struct <devname>_dev **device,
		       const struct <devname>_init_param *init_param);

/* Remove the device. */
int32_t <devname>_remove(struct <devname>_dev *device);

#endif /* <DEVNAME>_H_ */
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

#include <stdlib.h>
#include <stdio.h>
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "<devname>.h"
#include "jesd204.h"

/**
 * @struct <devname>_jesd204_priv
 * @brief  JESD204 private data -- links the JESD FSM back to the driver.
 */
struct <devname>_jesd204_priv {
	struct <devname>_dev *<devname>;
};

/* ---------- JESD204 state operations -------------------------------- */

/**
 * @brief JESD204 clock sync stage 2 callback.
 *
 * Toggles the MCS GPIO high then low to generate a synchronisation
 * pulse at the appropriate point in the JESD204 state machine.
 *
 * @param jdev   - JESD204 device handle.
 * @param reason - State operation reason (only INIT is handled).
 * @return JESD204_STATE_CHANGE_DONE on success, negative error code otherwise.
 */
static int <devname>_jesd204_clks_sync2(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason)
{
	struct <devname>_jesd204_priv *priv =
		(struct <devname>_jesd204_priv *)jesd204_dev_priv(jdev);
	struct <devname>_dev *dev = priv-><devname>;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	ret = no_os_gpio_set_value(dev->gpio_req, 1);
	if (ret)
		return ret;
	ret = no_os_gpio_set_value(dev->gpio_req, 0);
	if (ret)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * JESD204 device data structure -- registers the clock sync stage 2
 * callback with the JESD204 framework.
 */
static const struct jesd204_dev_data jesd204_<devname>_init = {
	.state_ops = {
		[JESD204_OP_CLK_SYNC_STAGE2] = {
			.per_device = <devname>_jesd204_clks_sync2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},
};

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Initialize the device.
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t <devname>_init(struct <devname>_dev **device,
		       const struct <devname>_init_param *init_param)
{
	struct <devname>_jesd204_priv *priv;
	struct <devname>_dev *dev;
	int32_t ret;

	dev = (struct <devname>_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_gpio_get(&dev->gpio_req, init_param->gpio_req);
	if (ret)
		goto error_dev;

	if (dev->gpio_req) {
		ret = no_os_gpio_direction_output(dev->gpio_req, 0);
		if (ret)
			goto error_gpio;
	}

	ret = jesd204_dev_register(&dev->jdev, &jesd204_<devname>_init);
	if (ret)
		goto error_gpio;

	priv = (struct <devname>_jesd204_priv *)jesd204_dev_priv(dev->jdev);
	priv-><devname> = dev;

	*device = dev;

	return 0;

error_gpio:
	no_os_gpio_remove(dev->gpio_req);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Remove the device - release resources.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t <devname>_remove(struct <devname>_dev *device)
{
	int32_t ret;

	if (!device)
		return -EINVAL;

	ret = no_os_gpio_remove(device->gpio_req);

	no_os_free(device);

	return ret;
}
```

---

## 5. IIO Header (not applicable)

MCS drivers operate at the JESD204 topology level and do not expose
measurement channels. There is no IIO header or source file for this
driver category. If a future MCS variant requires IIO support (e.g.,
for exposing synchronisation status as diagnostic channels), follow
the IIO patterns from the accelerometer or temperature templates.

---

## 6. IIO Source (not applicable)

See Section 5 above. No IIO source file is needed for MCS drivers.

---

## 7. Project Example

MCS drivers are typically used as part of a larger JESD204 project
(e.g., `projects/ad9081`) rather than having a standalone project.
The example below follows the `ltc2983` project structure pattern
to show how an MCS driver would be integrated.

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
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_util.c  \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/mcs/<devname>/<devname>.h
SRCS += $(DRIVERS)/mcs/<devname>/<devname>.c
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

struct no_os_gpio_init_param gpio_mcs_ip = {
	.number = GPIO_MCS_NR,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct <devname>_init_param <devname>_ip = {
	.gpio_req = &gpio_mcs_ip,
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
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
 *******************************************************************************/
int basic_example_main()
{
	struct <devname>_dev *dev;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	pr_info("<DEVNAME> initialised -- jdev=%p\r\n",
		(void *)dev->jdev);

	/*
	 * In a real application the MCS device is added to a
	 * jesd204_topology_dev array and the JESD204 FSM handles
	 * the synchronisation pulse.  This basic example only
	 * verifies that initialisation succeeds.
	 */

	<devname>_remove(dev);

	return 0;

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

#ifdef XILINX_PLATFORM
#include "xilinx/parameters.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
```

### 7.10 `projects/<project_name>/src/platform/xilinx/main.c`

```c
/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Xilinx platform of <devname> project.
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

#ifdef BASIC_EXAMPLE
error:
	no_os_uart_remove(uart);
#endif
	return 0;
}
```

### 7.11 `projects/<project_name>/src/platform/xilinx/parameters.h`

```c
/***************************************************************************//**
 *   @brief  Definitions specific to Xilinx platform used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "xil_gpio.h"
#include "xparameters.h"

#define UART_DEVICE_ID	XPAR_AXI_UART_DEVICE_ID
#define UART_BAUDRATE	115200
#define UART_OPS	&xil_uart_ops
#define UART_EXTRA	&xil_uart_extra

#define GPIO_MCS_NR	0
#define GPIO_OPS	&xil_gpio_ops
#define GPIO_EXTRA	&xil_gpio_extra

extern struct xil_uart_init_param xil_uart_extra;
extern struct xil_gpio_init_param xil_gpio_extra;

#endif /* __PARAMETERS_H__ */
```

### 7.12 `projects/<project_name>/src/platform/xilinx/parameters.c`

```c
/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Xilinx platform data used by <devname> project.
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/
#include "parameters.h"

struct xil_uart_init_param xil_uart_extra = {
	.type = UART_PS,
};

struct xil_gpio_init_param xil_gpio_extra = {
	.type = GPIO_PS,
	.device_id = XPAR_PS7_GPIO_0_DEVICE_ID,
};
```

### 7.13 `projects/<project_name>/src/platform/xilinx/platform_src.mk`

```makefile
INCS +=	$(PLATFORM_DRIVERS)/xilinx_gpio.h      \
	$(PLATFORM_DRIVERS)/xilinx_irq.h       \
	$(PLATFORM_DRIVERS)/xilinx_uart.h      \
	$(PLATFORM_DRIVERS)/xilinx_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/xilinx_delay.c     \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c      \
	$(PLATFORM_DRIVERS)/xilinx_irq.c       \
	$(PLATFORM_DRIVERS)/xilinx_uart.c      \
	$(PLATFORM_DRIVERS)/xilinx_uart_stdio.c
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

Any GPIO required in a project featuring JESD204 and multi-chip
synchronisation.

Overview
--------

The <DEVNAME> driver provides the infrastructure for adding a GPIO
to a JESD204 topology for projects that require multi-chip
synchronisation. The driver registers a JESD204 state operation
callback that toggles the GPIO at the appropriate point in the
JESD204 state machine to synchronise multiple devices.

Applications
------------

* MXFE Multi-Chip Synchronisation
* Multi-device JESD204 clock alignment

Configuration
-------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, requests the GPIO described in the init parameters,
sets it as an output (driven low), and registers the device with
the JESD204 framework. The function uses goto-based cleanup to
release resources on failure.

JESD204 Integration
~~~~~~~~~~~~~~~~~~~

After initialisation the device's ``jdev`` field is added to the
``jesd204_topology_dev`` array alongside other devices in the
JESD204 link. When the JESD204 FSM reaches the clock sync stage 2
the driver toggles the MCS GPIO high then low to generate the
synchronisation pulse.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_gpio.h"
   #include "jesd204.h"

   int main()
   {
       struct <devname>_dev *mcs_dev;
       struct no_os_gpio_init_param gpio_req_init = {
           .number = PHY_SYNC,
           .platform_ops = &xil_gpio_ops,
           .extra = &xil_gpio_param,
       };
       struct <devname>_init_param mcs_init = {
           .gpio_req = &gpio_req_init,
       };
       int ret;

       ret = <devname>_init(&mcs_dev, &mcs_init);
       if (ret)
           return ret;

       /* Add to JESD204 topology */
       struct jesd204_topology_dev devs[] = {
           /* ... other devices ... */
           {
               .jdev = mcs_dev->jdev,
               .link_ids = {DEFRAMER_LINK0_TX},
               .links_number = 1,
           },
       };

       /* Run JESD204 FSM -- MCS pulse is generated automatically */

   cleanup:
       <devname>_remove(mcs_dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/mcs/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/mcs/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`,
   `(struct <devname>_jesd204_priv *)jesd204_dev_priv(jdev)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int32_t`, negative = error.
   JESD204 callbacks return `JESD204_STATE_CHANGE_DONE` on success.
5. **Init signature** -- `int32_t <devname>_init(struct <devname>_dev
   **device, const struct <devname>_init_param *init_param)` (note:
   init_param passed by pointer with `const` qualifier).
6. **GPIO-based design** -- MCS drivers use `no_os_gpio_*` functions
   for signal control rather than SPI/I2C register access. The init
   param contains a `no_os_gpio_init_param *`.
7. **JESD204 integration** -- the driver registers with the JESD204
   framework via `jesd204_dev_register()` and provides state operation
   callbacks. A private data struct links the JESD204 handle back to
   the driver's device descriptor.
8. **Allocation** -- use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
9. **Goto cleanup** -- in `_init()`, use goto labels to free in reverse
   allocation order on error (GPIO first, then the device struct).
10. **Remove order** -- `_remove()` frees GPIO resources then the
    device struct. The function checks for a NULL device pointer.
11. **No globals** -- all state lives in the device descriptor struct.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> GPIO-based machine control signal
driver for multi-chip synchronisation in JESD204 topologies.

Signed-off-by: Your Name <your.name@analog.com>
```
