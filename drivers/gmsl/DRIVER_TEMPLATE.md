# GMSL SerDes Driver Template

Reference drivers: `drivers/gmsl/serializer/camera/max96793/` (serializer),
`drivers/gmsl/deserializer/camera/max96792/` (deserializer).

This template covers every file needed to add a new GMSL serializer or
deserializer driver to no-OS.  Replace `<devname>` with the part number
(e.g., `max96793`) and `<DEVNAME>` with its uppercase form (e.g.,
`MAX96793`) throughout.  Replace `<ser|des>` with `serializer` or
`deserializer` depending on the device type.

---

## 1. Naming Conventions

GMSL drivers use the shared `struct gmsl_dev` handle rather than a
per-part descriptor.  Part-specific capabilities, state, and
configuration are stored in the `void *` members of `gmsl_dev`.

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `max96793.h`, `max96793.c` |
| CSI files | `<devname>_csi.h`, `<devname>_csi.c` | `max96793_csi.h`, `max96793_csi.c` |
| Diagnostics files | `<devname>_diag.h`, `<devname>_diag.c` | `max96793_diag.h`, `max96793_diag.c` |
| Register map | `<devname>_regs.h` | `max96793_regs.h` |
| Include guard | `#ifndef <DEVNAME>_H` | `#ifndef MAX96793_H` |
| Device handle | `struct gmsl_dev` (shared) | `struct gmsl_dev *dev` |
| Init param (ser) | `struct gmsl_dev_csi_ser_init_param` | common structure |
| Init param (des) | `struct gmsl_dev_csi_des_init_param` | common structure |
| Capabilities | `<devname>_dev_capabilities` | `max96793_dev_capabilities` |
| State | `<devname>_state` | `max96793_state` |
| Functions | `<devname>_<verb>()` | `max96793_init()` |
| Register defines | `<REG_NAME>_<BLOCK>_<FIELD>_MASK` | `PHY1_HS_ERR_MIPI_RX_..._MASK` |
| Field position | `<REG_NAME>_<BLOCK>_<FIELD>_POS` | `PHY1_HS_ERR_MIPI_RX_..._POS` |

---

## 2. File Checklist

```
drivers/gmsl/<ser|des>/camera/<devname>/
    <devname>.h              # Driver header (init/remove + device-specific types)
    <devname>.c              # Driver implementation (init/remove + diag ops table)
    <devname>_csi.h          # CSI/MIPI configuration header
    <devname>_csi.c          # CSI/MIPI configuration implementation
    <devname>_diag.h         # Diagnostics header
    <devname>_diag.c         # Diagnostics implementation
    <devname>_regs.h         # Register map definitions

drivers/gmsl/common/        # Shared across all GMSL parts (already present)
    gmsl_common.h            # Common enums, gmsl_dev struct, diag ops vtable
    gmsl_cam_ser.h           # Serializer common init param / pipe / PHY structs
    gmsl_cam_des.h           # Deserializer common init param / pipe / PHY structs
    gmsl_reg_access.h/.c     # I2C register read/write/update helpers

drivers/gmsl/gmsl_dbg/
    gmsl_dbg.h               # Debug/logging macros

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

doc/sphinx/source/drivers/gmsl/<devname>.rst   # Sphinx entry
```

---

## 3. Driver Header (`<devname>.h`)

```c
/*******************************************************************************
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
******************************************************************************/
#ifndef <DEVNAME>_H
#define <DEVNAME>_H

#include <stdbool.h>
#include <stdint.h>
#include "gmsl_common.h"

/* ---- Device Constants --------------------------------------------- */

/** Number of line fault monitors. */
#define <DEVNAME>_LMN_COUNT		(2u)

/** Number of MIPI PHYs. */
#define <DEVNAME>_MIPI_PHY_COUNT	(2u)

/** Size of device state memory. */
#define <DEVNAME>_DEV_STATE_MEM_SIZE	(sizeof(struct <devname>_state))

/* ---- Enumerations ------------------------------------------------- */

/**
 * @enum <devname>_phy
 * @brief Enumeration for <DEVNAME> PHY.
 */
enum <devname>_phy {
	<DEVNAME>_PHY_1 = 0u,	/*!< Phy 1 */
	<DEVNAME>_PHY_2 = 1u,	/*!< Phy 2 */
};

/* ---- Structures --------------------------------------------------- */

/**
 * @struct <devname>_dev_capabilities
 * @brief  Structure representing the capabilities of the <DEVNAME> device.
 */
struct <devname>_dev_capabilities {
	uint8_t max_links_count;	/*!< Maximum number of Links */
	uint8_t max_lmn_count;		/*!< Maximum number of Line Fault Monitors */
	uint8_t max_phy_ctrlr;		/*!< Maximum number of Phy controllers */
};

/**
 * @struct <devname>_state
 * @brief  Structure representing <DEVNAME> device state.
 */
struct <devname>_state {
	uint8_t dummy;		/*!< Placeholder for device state */
};

/* ---- Function Prototypes ------------------------------------------ */

/** @brief Initialize the <DEVNAME> device. */
int32_t <devname>_init(struct gmsl_dev **device,
		       struct no_os_i2c_desc *i2c_desc,
		       void *init_param);

/** @brief Free memory allocated by <devname>_init(). */
int32_t <devname>_remove(struct gmsl_dev *dev);

#endif /* <DEVNAME>_H */
```

---

## 4. Driver Source (`<devname>.c`)

```c
/*******************************************************************************
 *   @file   <devname>.c
 *   @brief  Driver Implementation of <DEVNAME> <Serializer|Deserializer>.
 *   @author Your Name (your.name@analog.com)
********************************************************************************
 * ... (same BSD-3-Clause block as header) ...
******************************************************************************/

#include "<devname>.h"
#include "<devname>_diag.h"
#include "<devname>_csi.h"
#include "gmsl_dbg.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "gmsl_reg_access.h"

/* For serializer: */
#include "gmsl_cam_ser.h"
/* For deserializer: */
/* #include "gmsl_cam_des.h" */

#define <DEVNAME>_MAX_LINKS		(1U)
#define <DEVNAME>_MAX_LMN		(2U)
#define <DEVNAME>_MAX_PHY_CTRLR		(1U)

#define DEV_NAME "<DEVNAME>"

/*=================================== STATIC ==================================*/

#ifndef ADI_DISABLE_DOCUMENTATION

static const struct gmsl_dev_diag_ops <devname>_diag_ops = {
	.diag_check_device_id = <devname>_dev_diag_get_device_id,
	.diag_check_device_rev = <devname>_dev_diag_check_device_rev,
	.diag_check_part_config = <devname>_dev_diag_check_part_config_status,
	.diag_check_link_lock_status = <devname>_link_diag_check_link_lock_status,
	.diag_check_decode_error_status = <devname>_link_diag_check_decode_error_status,
	.diag_check_idle_err_status = <devname>_link_diag_check_idle_error_status,
	.diag_check_max_retrans_status = <devname>_link_diag_check_max_retransmission_error_status,
	.diag_check_line_fault_status = <devname>_link_diag_check_line_fault_status,
	.diag_check_eom_status = <devname>_link_diag_check_eye_open_monitor_status,
	.diag_check_mipi_rx_err_status = <devname>_csi_diag_check_mipi_rx_err_status,
	.diag_check_overflow_status = <devname>_pipe_diag_check_vid_tx_overflow_status,
	.diag_check_mem_ecc_2b_err_status = <devname>_mem_diag_check_uncorrectable_2b_mem_ecc_err_status,
	/* Set unsupported ops to NULL */
	.diag_check_dp_fec_support = NULL,
	.diag_check_hdcp_status = NULL,
	.diag_check_link_training_status = NULL,
	.diag_check_video_stream_dsc_compression = NULL,
};

#endif /* ADI_DISABLE_DOCUMENTATION */

/**
 * @brief Initialize the <DEVNAME> device.
 *
 * Allocates the shared gmsl_dev handle, populates device capabilities,
 * stores I2C descriptor and init configuration, and allocates device
 * state memory.
 *
 * @param device     - Pointer to the GMSL device handle pointer (allocated here).
 * @param i2c_desc   - I2C descriptor for register access.
 * @param init_param - Initialization parameters (cast to
 *                     gmsl_dev_csi_ser_init_param or
 *                     gmsl_dev_csi_des_init_param internally).
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_init(struct gmsl_dev **device,
		       struct no_os_i2c_desc *i2c_desc,
		       void *init_param)
{
	int32_t ret = 0;
	struct gmsl_dev *dev;
	struct <devname>_dev_capabilities *dev_cap;
	/* For serializer: */
	struct gmsl_dev_csi_ser_init_param *init_cfg_param =
		(struct gmsl_dev_csi_ser_init_param *)init_param;
	/* For deserializer use gmsl_dev_csi_des_init_param instead. */

	if (!device) {
		ret = -EINVAL;
		goto init_error;
	}

	dev = (struct gmsl_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev) {
		ret = -ENOMEM;
		goto init_error;
	}

	dev_cap = (struct <devname>_dev_capabilities *)no_os_calloc(1,
			sizeof(*dev_cap));
	if (!dev_cap) {
		ret = -ENOMEM;
		goto error_remove_dev;
	}

	dev_cap->max_links_count = <DEVNAME>_MAX_LINKS;
	dev_cap->max_lmn_count = <DEVNAME>_MAX_LMN;
	dev_cap->max_phy_ctrlr = <DEVNAME>_MAX_PHY_CTRLR;

	dev->index = init_cfg_param->index;
	dev->part_id = 0;	/* TODO: read from hardware */
	dev->rev_id = 0;	/* TODO: read from hardware */
	dev->dev_cap = (void *)dev_cap;
	dev->i2c_desc = i2c_desc;
	dev->dev_config = init_param;
	dev->dev_diag = (void *)&<devname>_diag_ops;

	if (init_cfg_param->dev_name != NULL)
		strcpy(dev->dev_name, init_cfg_param->dev_name);
	else
		strcpy(dev->dev_name, DEV_NAME);

	dev->dev_state = no_os_calloc(1, <DEVNAME>_DEV_STATE_MEM_SIZE);
	if (!dev->dev_state) {
		ret = -ENOMEM;
		goto error_remove_cap;
	}

	*device = dev;

	GMSL_LOG_INFO("%s-0x%x initialization passed", dev->dev_name,
		      dev->i2c_desc->slave_address);

	goto exit;

error_remove_cap:
	no_os_free(dev_cap);

error_remove_dev:
	no_os_free(dev);

init_error:
	GMSL_LOG_ERROR("<DEVNAME> Initialization failed");

exit:
	return ret;
}

/**
 * @brief Free memory allocated by <devname>_init().
 * @param dev - Device handle.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_remove(struct gmsl_dev *dev)
{
	int32_t ret = 0;

	if (!dev) {
		GMSL_LOG_ERROR("%s remove failed", (char *)DEV_NAME);
		return -EINVAL;
	}

	if (dev->dev_state != NULL)
		no_os_free(dev->dev_state);
	else
		ret = -EINVAL;

	if (dev->dev_cap != NULL)
		no_os_free(dev->dev_cap);
	else
		ret = -EINVAL;

	no_os_free(dev);

	GMSL_LOG_INFO("%s remove passed", (char *)DEV_NAME);

	return ret;
}
```

---

## 5. IIO Header

IIO support is **not typical** for GMSL serializer/deserializer drivers.
These devices handle high-speed video/data links rather than sensor
data channels, so IIO attributes and buffered reads are generally not
applicable.

If IIO is needed for a specific use case (e.g., exposing link
diagnostics as IIO attributes), follow the same pattern as the
temperature template (Section 5) but adapt the channel type and
attributes to represent link status rather than sensor data.

---

## 6. IIO Source

See Section 5 above.  No IIO skeleton is provided because GMSL
drivers do not expose sampled data channels.

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

# GMSL common layer
INCS += $(DRIVERS)/gmsl/common/gmsl_common.h      \
	$(DRIVERS)/gmsl/common/gmsl_reg_access.h   \
	$(DRIVERS)/gmsl/common/gmsl_cam_ser.h      \
	$(DRIVERS)/gmsl/common/gmsl_cam_des.h      \
	$(DRIVERS)/gmsl/gmsl_dbg/gmsl_dbg.h

SRCS += $(DRIVERS)/gmsl/common/gmsl_reg_access.c

# Part-specific driver (serializer example)
INCS += $(DRIVERS)/gmsl/<ser|des>/camera/<devname>/<devname>.h      \
	$(DRIVERS)/gmsl/<ser|des>/camera/<devname>/<devname>_csi.h  \
	$(DRIVERS)/gmsl/<ser|des>/camera/<devname>/<devname>_diag.h \
	$(DRIVERS)/gmsl/<ser|des>/camera/<devname>/<devname>_regs.h

SRCS += $(DRIVERS)/gmsl/<ser|des>/camera/<devname>/<devname>.c      \
	$(DRIVERS)/gmsl/<ser|des>/camera/<devname>/<devname>_csi.c  \
	$(DRIVERS)/gmsl/<ser|des>/camera/<devname>/<devname>_diag.c
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
#include "gmsl_cam_ser.h"   /* or gmsl_cam_des.h for deserializer */

extern struct no_os_uart_init_param uip;
extern struct no_os_i2c_init_param <devname>_i2c_ip;
extern struct gmsl_dev_csi_ser_init_param <devname>_ip;

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

struct no_os_i2c_init_param <devname>_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 400000,
	.slave_address = <DEVNAME>_I2C_ADDR,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

struct gmsl_dev_csi_ser_init_param <devname>_ip = {
	.dev_name = "<DEVNAME>",
	.index = 0,
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
#include "no_os_i2c.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct gmsl_dev *dev;
	struct no_os_i2c_desc *i2c_desc;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = no_os_i2c_init(&i2c_desc, &<devname>_i2c_ip);
	if (ret)
		goto error;

	ret = <devname>_init(&dev, i2c_desc, &<devname>_ip);
	if (ret)
		goto free_i2c;

	pr_info("<DEVNAME> initialized successfully\r\n");
	pr_info("Device: %s, Part ID: 0x%x\r\n",
		dev->dev_name, dev->part_id);

	/* Main loop: run diagnostics or link monitoring. */
	while (1) {
		/* TODO: Add link status polling, CSI setup, etc. */
		no_os_mdelay(1000);
	}

	<devname>_remove(dev);
free_i2c:
	no_os_i2c_remove(i2c_desc);
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

/* Default I2C address for the GMSL device. */
#define <DEVNAME>_I2C_ADDR	0x40

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

The <DEVNAME> is a GMSL <serializer|deserializer> that
<receives|outputs> video on a MIPI CSI-2 interface and
<outputs|receives> it <to|from> a GMSL2 or GMSL3 serial link.
It sends and receives bidirectional control channel data across the
same GMSL link.  GMSL data can be transported over coaxial or shielded
twisted pair (STP) cables.  <Add 2-3 more sentences covering key specs:
link rates, number of links, CSI-2 lanes, I2C configuration.>

Applications
------------

* Automotive camera systems
* ADAS (Advanced Driver Assistance Systems)
* Surround view monitoring
* Driver monitoring systems
* Machine vision

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates the shared ``gmsl_dev``
handle, populates device capabilities (link count, LMN count, PHY
controllers), stores the I2C descriptor for register access, and
assigns the diagnostic operations table.

CSI Configuration
~~~~~~~~~~~~~~~~~

The ``<devname>_csi_init`` function configures the MIPI CSI-2
interface including port setup, lane mapping, lane polarity, and
pipe configuration for video data routing.

Link Management
~~~~~~~~~~~~~~~

GMSL link mode (GMSL2 NRZ or GMSL3 PAM4), PHY rate (3G/6G/12G),
cable type (STP/coax), and FEC settings are configured through the
common GMSL structures and register access layer.

Diagnostics
~~~~~~~~~~~

The ``gmsl_dev_diag_ops`` function table provides comprehensive
diagnostic checks including device ID verification, link lock status,
decode errors, line fault monitoring, EOM (eye-open monitor), and
MIPI status.

Register Access
~~~~~~~~~~~~~~~

All register access uses the shared ``gmsl_reg_read``,
``gmsl_reg_write``, and ``gmsl_reg_update`` functions which operate
on 16-bit register addresses with 8-bit data over I2C.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "gmsl_cam_ser.h"
   #include "no_os_i2c.h"

   int main()
   {
       struct no_os_i2c_init_param i2c_ip = {
           .device_id     = 1,
           .max_speed_hz  = 400000,
           .slave_address = 0x40,
           .platform_ops  = &max_i2c_ops,
           .extra         = &i2c_extra,
       };

       struct gmsl_dev_csi_ser_init_param init_param = {
           .dev_name = "<DEVNAME>",
           .index    = 0,
       };

       struct no_os_i2c_desc *i2c_desc;
       struct gmsl_dev *dev;
       int ret;

       ret = no_os_i2c_init(&i2c_desc, &i2c_ip);
       if (ret)
           return ret;

       ret = <devname>_init(&dev, i2c_desc, &init_param);
       if (ret)
           goto cleanup;

       /* Device is ready for CSI configuration and link setup */

   cleanup:
       <devname>_remove(dev);
       no_os_i2c_remove(i2c_desc);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/gmsl/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/gmsl/<ser|des>/camera/<devname>/README.rst
```

---

## 9. Key Conventions

1. **Shared `gmsl_dev` handle** -- all GMSL drivers use the common
   `struct gmsl_dev` rather than part-specific descriptors.
   Part-specific data is stored via `void *dev_cap`, `void *dev_config`,
   `void *dev_state`, and `void *dev_diag`.
2. **I2C with 16-bit register addresses** -- use `gmsl_reg_read()`,
   `gmsl_reg_write()`, and `gmsl_reg_update()` from
   `gmsl_reg_access.h`.  Register addresses are 16-bit; data is 8-bit.
3. **Register field extraction** -- use the `VALBITFIELD(reg_val, field)`
   macro from `gmsl_common.h` which applies `_MASK` and `_POS` suffixes.
4. **Error-checked register access macros** --
   `REG_WRITE_RETURN_ON_ERR()`, `REG_READ_RETURN_ON_ERR()`, and
   `REG_UPDATE_RETURN_ON_ERR()` wrap the register functions with
   early return on error.
5. **Diagnostic operations table** -- each driver populates a static
   `struct gmsl_dev_diag_ops` and stores it in `dev->dev_diag`.
   Set unsupported operations to `NULL`.
6. **Serializer vs. Deserializer init params** -- serializers use
   `struct gmsl_dev_csi_ser_init_param`; deserializers use
   `struct gmsl_dev_csi_des_init_param`.
7. **CSI/MIPI configuration** -- separated into `<devname>_csi.h/.c`
   for port, PHY, pipe, and lane configuration.
8. **Debug logging** -- use `GMSL_LOG_INFO()`, `GMSL_LOG_ERROR()`,
   `GMSL_LOG_DEBUG()`, and `GMSL_LOG_WARNING()` from `gmsl_dbg.h`.
   These are compiled out when `_DEBUG` is not defined.
9. **Goto-based cleanup in init** -- allocations are unwound with
   labeled gotos (`error_remove_cap`, `error_remove_dev`, etc.).
10. **C++ compatibility** -- use explicit casts for `void *` (e.g.,
    `(struct gmsl_dev *)no_os_calloc(...)`), use `no_os_calloc` /
    `no_os_free`, and use tabs for indentation.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> GMSL <serializer|deserializer> with
MIPI CSI-2 interface and GMSL2/GMSL3 serial link support.

Signed-off-by: Your Name <your.name@analog.com>
```
