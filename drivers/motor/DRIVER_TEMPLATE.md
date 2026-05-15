# Motor Controller Driver Template

Reference driver: `drivers/motor/tmc5240/`

This template covers every file needed to add a new motor controller driver
to no-OS. Replace `<devname>` with the part number (e.g., `tmc5240`) and
`<DEVNAME>` with its uppercase form (e.g., `TMC5240`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `tmc5240.h`, `tmc5240.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_tmc5240.h`, `iio_tmc5240.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __TMC5240_H__` |
| Init param struct | `<devname>_init_param` | `tmc5240_init_param` |
| Device struct | `<devname>_dev` | `tmc5240_dev` |
| IIO descriptor | `<devname>_iio_dev` | `tmc5240_iio_dev` |
| Functions | `<devname>_<verb>()` | `tmc5240_init()` |
| Static helpers | `_<devname>_<verb>()` | `_tmc5240_spi_xfer()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_TMC5240` |
| Register addresses | `<DEVNAME>_REG_<NAME>` | `TMC5240_REG_GCONF` |
| Register masks | `<DEVNAME>_<FIELD>_MSK` | `TMC5240_SHAFT_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `TMC5240_MODE_POSITION` |

---

## 2. File Checklist

```
drivers/motor/<devname>/
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

doc/sphinx/source/drivers/motor/<devname>.rst   # Sphinx entry
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

#define <DEVNAME>_REG_GCONF		0x00
#define <DEVNAME>_REG_GSTAT		0x01
#define <DEVNAME>_REG_IOIN		0x04
#define <DEVNAME>_REG_DRV_CONF		0x0A
#define <DEVNAME>_REG_GLOBAL_SCALER	0x0B
#define <DEVNAME>_REG_IHOLD_IRUN	0x10
#define <DEVNAME>_REG_RAMPMODE		0x20
#define <DEVNAME>_REG_XACTUAL		0x21
#define <DEVNAME>_REG_VACTUAL		0x22
#define <DEVNAME>_REG_VSTART		0x23
#define <DEVNAME>_REG_A1		0x24
#define <DEVNAME>_REG_V1		0x25
#define <DEVNAME>_REG_AMAX		0x26
#define <DEVNAME>_REG_VMAX		0x27
#define <DEVNAME>_REG_DMAX		0x28
#define <DEVNAME>_REG_D1		0x2A
#define <DEVNAME>_REG_VSTOP		0x2B
#define <DEVNAME>_REG_XTARGET		0x2D
#define <DEVNAME>_REG_A2		0x2E
#define <DEVNAME>_REG_V2		0x2F
#define <DEVNAME>_REG_D2		0x30
#define <DEVNAME>_REG_AACTUAL		0x31
#define <DEVNAME>_REG_CHOPCONF		0x6C
#define <DEVNAME>_REG_DRV_STATUS	0x6F

/* ---------------- Field Masks ------------------------------------------ */

/** GCONF register fields */
#define <DEVNAME>_SHAFT_MSK		NO_OS_BIT(4)

/** IHOLD_IRUN register fields */
#define <DEVNAME>_IHOLD_MSK		NO_OS_GENMASK(4, 0)
#define <DEVNAME>_IRUN_MSK		NO_OS_GENMASK(12, 8)
#define <DEVNAME>_IHOLDDELAY_MSK	NO_OS_GENMASK(19, 16)
#define <DEVNAME>_IRUNDELAY_MSK		NO_OS_GENMASK(27, 24)

/** DRV_CONF register fields */
#define <DEVNAME>_SLOPE_CONTROL_MSK	NO_OS_GENMASK(5, 4)
#define <DEVNAME>_CURRENT_RANGE_MSK	NO_OS_GENMASK(1, 0)

/** CHOPCONF register fields */
#define <DEVNAME>_TOFF_MSK		NO_OS_GENMASK(3, 0)
#define <DEVNAME>_TBL_MSK		NO_OS_GENMASK(16, 15)
#define <DEVNAME>_MRES_MSK		NO_OS_GENMASK(27, 24)

/** Motion parameter masks (per-register field widths) */
#define <DEVNAME>_VMAX_MSK		NO_OS_GENMASK(22, 0)
#define <DEVNAME>_AMAX_MSK		NO_OS_GENMASK(17, 0)
#define <DEVNAME>_DMAX_MSK		NO_OS_GENMASK(17, 0)
#define <DEVNAME>_VSTART_MSK		NO_OS_GENMASK(17, 0)
#define <DEVNAME>_VSTOP_MSK		NO_OS_GENMASK(17, 0)
#define <DEVNAME>_A1_MSK		NO_OS_GENMASK(17, 0)
#define <DEVNAME>_V1_MSK		NO_OS_GENMASK(19, 0)
#define <DEVNAME>_A2_MSK		NO_OS_GENMASK(17, 0)
#define <DEVNAME>_V2_MSK		NO_OS_GENMASK(19, 0)
#define <DEVNAME>_D1_MSK		NO_OS_GENMASK(17, 0)
#define <DEVNAME>_D2_MSK		NO_OS_GENMASK(17, 0)
#define <DEVNAME>_RAMPMODE_MSK		NO_OS_GENMASK(1, 0)

/* Default motion profile values */
#define <DEVNAME>_DEFAULT_D1		10
#define <DEVNAME>_DEFAULT_D2		10
#define <DEVNAME>_DEFAULT_VSTOP		10

/* Product / version ID expected value */
#define <DEVNAME>_VERSION_ID		0xXX

/* ---------------- Enums ------------------------------------------------ */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_ramp_mode
 *  @brief Motion ramp mode selection. */
enum <devname>_ramp_mode {
	<DEVNAME>_MODE_POSITION = 0,
	<DEVNAME>_MODE_VELPOS   = 1,
	<DEVNAME>_MODE_VELNEG   = 2,
	<DEVNAME>_MODE_HOLD     = 3,
};

/* Add more enums as needed: microstepping, current range, etc. */

/* ---------------- Structures ------------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** SPI initialization parameters. */
	struct no_os_spi_init_param spi_init_param;
	/** Start velocity. */
	uint32_t vstart;
	/** First acceleration phase. */
	uint32_t a1;
	/** First velocity threshold. */
	uint32_t v1;
	/** Second acceleration phase. */
	uint32_t a2;
	/** Second velocity threshold. */
	uint32_t v2;
	/** Maximum acceleration. */
	uint32_t amax;
	/** Maximum velocity. */
	uint32_t vmax;
	/** Maximum deceleration. */
	uint32_t dmax;
	/** Second deceleration phase. */
	uint32_t d2;
	/** First deceleration phase. */
	uint32_t d1;
	/** Stop velocity. */
	uint32_t vstop;
	/** Internal clock frequency in Hz. */
	uint32_t clock;
	/** Motor step angle in millidegrees. */
	uint32_t step_angle_millidegrees;
	/** Microstepping resolution (0=256, 1=128, etc.). */
	uint8_t microsteps_res;
	/** Hold current delay. */
	uint8_t iholddelay;
	/** Run current delay. */
	uint8_t irundelay;
	/** Hold current (0-31). */
	uint8_t current_hold;
	/** Run current (0-31). */
	uint8_t current_run;
	/** Off-time setting. */
	uint8_t toff;
	/** Blank time setting. */
	uint8_t tbl;
	/** Global current scaler (0-255). */
	uint8_t global_scaler;
	/** Slope control for voltage ramping. */
	uint8_t slope_control;
	/** Current range selection. */
	uint8_t current_range;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_init().
 */
struct <devname>_dev {
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** Start velocity. */
	uint32_t vstart;
	/** First acceleration phase. */
	uint32_t a1;
	/** First velocity threshold. */
	uint32_t v1;
	/** Second acceleration phase. */
	uint32_t a2;
	/** Second velocity threshold. */
	uint32_t v2;
	/** Maximum acceleration. */
	uint32_t amax;
	/** Maximum velocity. */
	uint32_t vmax;
	/** Maximum deceleration. */
	uint32_t dmax;
	/** Second deceleration phase. */
	uint32_t d2;
	/** First deceleration phase. */
	uint32_t d1;
	/** Stop velocity. */
	uint32_t vstop;
	/** Internal clock frequency in Hz. */
	uint32_t clock;
	/** Motor step angle in millidegrees. */
	uint32_t step_angle_millidegrees;
	/** Microstepping resolution (0=256, 1=128, etc.). */
	uint8_t microsteps_res;
	/** Device ID for multi-instance support. */
	uint8_t id;
};

/* ---------------- Function Prototypes ---------------------------------- */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **device,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *device);

/** @brief Read a register. */
int <devname>_reg_read(struct <devname>_dev *device, uint8_t reg_addr,
		       uint32_t *reg_data);

/** @brief Write a register. */
int <devname>_reg_write(struct <devname>_dev *device, uint8_t reg_addr,
			uint32_t reg_data);

/** @brief Read-modify-write a register field. */
int <devname>_reg_update(struct <devname>_dev *device, uint8_t reg_addr,
			 uint32_t mask, uint32_t val);

/** @brief Apply the full motion profile from the device descriptor. */
int <devname>_set_motion_profile(struct <devname>_dev *device);

/** @brief Get the target position. */
int <devname>_get_target_pos(struct <devname>_dev *device, int32_t *position);

/** @brief Set the target position (switches to position mode). */
int <devname>_set_target_pos(struct <devname>_dev *device, int32_t position);

/** @brief Get the current actual position. */
int <devname>_get_current_pos(struct <devname>_dev *device, int32_t *position);

/** @brief Set (preset) the current position (switches to hold mode). */
int <devname>_set_current_pos(struct <devname>_dev *device, int32_t position);

/** @brief Get the current actual velocity. */
int <devname>_get_current_vel(struct <devname>_dev *device, int32_t *velocity);

/** @brief Set the target velocity (switches to velocity mode). */
int <devname>_set_target_vel(struct <devname>_dev *device, int32_t velocity);

/** @brief Get the current actual acceleration. */
int <devname>_get_current_accel(struct <devname>_dev *device,
				int32_t *acceleration);

/** @brief Get the shaft direction. */
int <devname>_get_shaft_dir(struct <devname>_dev *device, uint8_t *dir);

/** @brief Set the shaft direction. */
int <devname>_set_shaft_dir(struct <devname>_dev *device, uint8_t dir);

/** @brief Get maximum acceleration (AMAX). */
int <devname>_get_amax(struct <devname>_dev *device, int32_t *amax);

/** @brief Set maximum acceleration (AMAX). */
int <devname>_set_amax(struct <devname>_dev *device, int32_t amax);

/** @brief Get maximum velocity (VMAX). */
int <devname>_get_vmax(struct <devname>_dev *device, int32_t *vmax);

/** @brief Set maximum velocity (VMAX). */
int <devname>_set_vmax(struct <devname>_dev *device, int32_t vmax);

/** @brief Get maximum deceleration (DMAX). */
int <devname>_get_dmax(struct <devname>_dev *device, int32_t *dmax);

/** @brief Set maximum deceleration (DMAX). */
int <devname>_set_dmax(struct <devname>_dev *device, int32_t dmax);

/** @brief Get ramp mode. */
int <devname>_get_rampmode(struct <devname>_dev *device, int32_t *rampmode);

/** @brief Set ramp mode. */
int <devname>_set_rampmode(struct <devname>_dev *device, int32_t rampmode);

/** @brief Stop the motor. */
int <devname>_stop(struct <devname>_dev *device);

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

/* ---------- SPI communication helpers ---------------------------------- */

/**
 * @brief Read a 32-bit register over SPI.
 * @param device   - Device descriptor.
 * @param reg_addr - Register address (7-bit).
 * @param reg_data - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 *
 * Motor controller SPI protocol: 5-byte frame
 *   TX: [addr | 0x00 (read)] [0x00] [0x00] [0x00] [0x00]
 *   RX: [status]             [data MSB ... data LSB]
 */
int <devname>_reg_read(struct <devname>_dev *device, uint8_t reg_addr,
		       uint32_t *reg_data)
{
	uint8_t buf[5] = {0};
	int ret;

	if (!device || !reg_data)
		return -EINVAL;

	/* First transfer: send read request */
	buf[0] = reg_addr & 0x7F;

	ret = no_os_spi_write_and_read(device->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	/* Second transfer: clock out the read data */
	memset(buf, 0, sizeof(buf));
	buf[0] = reg_addr & 0x7F;

	ret = no_os_spi_write_and_read(device->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	*reg_data = ((uint32_t)buf[1] << 24) |
		    ((uint32_t)buf[2] << 16) |
		    ((uint32_t)buf[3] << 8)  |
		    (uint32_t)buf[4];

	return 0;
}

/**
 * @brief Write a 32-bit register over SPI.
 * @param device   - Device descriptor.
 * @param reg_addr - Register address (7-bit).
 * @param reg_data - Value to write.
 * @return 0 on success, negative error code otherwise.
 *
 * Motor controller SPI protocol: 5-byte frame
 *   TX: [addr | 0x80 (write)] [data MSB ... data LSB]
 */
int <devname>_reg_write(struct <devname>_dev *device, uint8_t reg_addr,
			uint32_t reg_data)
{
	uint8_t buf[5];

	if (!device)
		return -EINVAL;

	buf[0] = reg_addr | 0x80;
	buf[1] = (uint8_t)(reg_data >> 24);
	buf[2] = (uint8_t)(reg_data >> 16);
	buf[3] = (uint8_t)(reg_data >> 8);
	buf[4] = (uint8_t)(reg_data & 0xFF);

	return no_os_spi_write_and_read(device->spi_desc, buf, sizeof(buf));
}

/**
 * @brief Read-modify-write a register field.
 * @param device   - Device descriptor.
 * @param reg_addr - Register address.
 * @param mask     - Bitmask of the field(s) to update.
 * @param val      - New field value, pre-shifted to the correct position.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_reg_update(struct <devname>_dev *device, uint8_t reg_addr,
			 uint32_t mask, uint32_t val)
{
	uint32_t reg_val;
	int ret;

	ret = <devname>_reg_read(device, reg_addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= val & mask;

	return <devname>_reg_write(device, reg_addr, reg_val);
}

/* ---------- Motion profile --------------------------------------------- */

/**
 * @brief Apply the full motion profile from the device descriptor.
 * @param device - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 *
 * Configures all multi-phase acceleration/deceleration parameters.
 * Applies defaults for d1, d2, and vstop if they are zero.
 */
int <devname>_set_motion_profile(struct <devname>_dev *device)
{
	int ret;
	int32_t temp;

	ret = <devname>_set_amax(device, device->amax);
	if (ret)
		return ret;

	ret = <devname>_set_vmax(device, device->vmax);
	if (ret)
		return ret;

	ret = <devname>_set_dmax(device, device->dmax);
	if (ret)
		return ret;

	ret = <devname>_reg_write(device, <DEVNAME>_REG_VSTART,
				  device->vstart & <DEVNAME>_VSTART_MSK);
	if (ret)
		return ret;

	ret = <devname>_reg_write(device, <DEVNAME>_REG_V1,
				  device->v1 & <DEVNAME>_V1_MSK);
	if (ret)
		return ret;

	ret = <devname>_reg_write(device, <DEVNAME>_REG_A1,
				  device->a1 & <DEVNAME>_A1_MSK);
	if (ret)
		return ret;

	ret = <devname>_reg_write(device, <DEVNAME>_REG_V2,
				  device->v2 & <DEVNAME>_V2_MSK);
	if (ret)
		return ret;

	ret = <devname>_reg_write(device, <DEVNAME>_REG_A2,
				  device->a2 & <DEVNAME>_A2_MSK);
	if (ret)
		return ret;

	temp = device->d1 ? device->d1 : <DEVNAME>_DEFAULT_D1;
	ret = <devname>_reg_write(device, <DEVNAME>_REG_D1,
				  (uint32_t)(temp & <DEVNAME>_D1_MSK));
	if (ret)
		return ret;

	temp = device->d2 ? device->d2 : <DEVNAME>_DEFAULT_D2;
	ret = <devname>_reg_write(device, <DEVNAME>_REG_D2,
				  (uint32_t)(temp & <DEVNAME>_D2_MSK));
	if (ret)
		return ret;

	temp = device->vstop ? device->vstop : <DEVNAME>_DEFAULT_VSTOP;
	if (temp < (int32_t)device->vstart)
		temp = device->vstart;

	return <devname>_reg_write(device, <DEVNAME>_REG_VSTOP,
				   (uint32_t)(temp & <DEVNAME>_VSTOP_MSK));
}

/* ---------- Position / velocity / acceleration accessors --------------- */

/**
 * @brief Set the target position (switches to position mode).
 * @param device   - Device descriptor.
 * @param position - Target position in microsteps.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_target_pos(struct <devname>_dev *device, int32_t position)
{
	int ret;

	ret = <devname>_reg_write(device, <DEVNAME>_REG_RAMPMODE,
				  <DEVNAME>_MODE_POSITION);
	if (ret)
		return ret;

	ret = <devname>_set_motion_profile(device);
	if (ret)
		return ret;

	return <devname>_reg_write(device, <DEVNAME>_REG_XTARGET,
				   (uint32_t)position);
}

/**
 * @brief Get the target position.
 * @param device   - Device descriptor.
 * @param position - Pointer to store target position.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_target_pos(struct <devname>_dev *device, int32_t *position)
{
	uint32_t temp;
	int ret;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_XTARGET, &temp);
	if (ret)
		return ret;

	*position = (int32_t)temp;

	return 0;
}

/**
 * @brief Get the current actual position.
 * @param device   - Device descriptor.
 * @param position - Pointer to store actual position.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_current_pos(struct <devname>_dev *device, int32_t *position)
{
	uint32_t temp;
	int ret;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_XACTUAL, &temp);
	if (ret)
		return ret;

	*position = (int32_t)temp;

	return 0;
}

/**
 * @brief Preset the current position (switches to hold mode first).
 * @param device   - Device descriptor.
 * @param position - New position value in microsteps.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_current_pos(struct <devname>_dev *device, int32_t position)
{
	int ret;

	ret = <devname>_reg_write(device, <DEVNAME>_REG_RAMPMODE,
				  <DEVNAME>_MODE_HOLD);
	if (ret)
		return ret;

	return <devname>_reg_write(device, <DEVNAME>_REG_XACTUAL,
				   (uint32_t)position);
}

/**
 * @brief Set the target velocity (switches to velocity mode).
 * @param device   - Device descriptor.
 * @param velocity - Target velocity. Sign determines direction.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_target_vel(struct <devname>_dev *device, int32_t velocity)
{
	int ret;
	uint32_t amax;
	uint32_t vmax_abs;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_AMAX, &amax);
	if (ret)
		return ret;

	if (amax == 0) {
		ret = <devname>_reg_write(device, <DEVNAME>_REG_AMAX,
					  device->amax);
		if (ret)
			return ret;
	}

	vmax_abs = (velocity < 0) ? (uint32_t)(-velocity) : (uint32_t)velocity;

	if (vmax_abs > <DEVNAME>_VMAX_MSK)
		return -EINVAL;

	ret = <devname>_reg_write(device, <DEVNAME>_REG_VMAX, vmax_abs);
	if (ret)
		return ret;

	if (velocity < 0)
		return <devname>_reg_write(device, <DEVNAME>_REG_RAMPMODE,
					   <DEVNAME>_MODE_VELNEG);
	else
		return <devname>_reg_write(device, <DEVNAME>_REG_RAMPMODE,
					   <DEVNAME>_MODE_VELPOS);
}

/**
 * @brief Get the current actual velocity.
 * @param device   - Device descriptor.
 * @param velocity - Pointer to store velocity (sign-extended 24-bit).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_current_vel(struct <devname>_dev *device, int32_t *velocity)
{
	uint32_t reg_val;
	int ret;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_VACTUAL, &reg_val);
	if (ret)
		return ret;

	*velocity = no_os_sign_extend32(reg_val, 23);

	return 0;
}

/**
 * @brief Get the current actual acceleration.
 * @param device       - Device descriptor.
 * @param acceleration - Pointer to store acceleration (sign-extended 24-bit).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_current_accel(struct <devname>_dev *device,
				int32_t *acceleration)
{
	uint32_t temp;
	int ret;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_AACTUAL, &temp);
	if (ret)
		return ret;

	*acceleration = no_os_sign_extend32(temp, 23);

	return 0;
}

/**
 * @brief Get the shaft direction.
 * @param device - Device descriptor.
 * @param dir    - Pointer to store direction (0=normal, 1=reverse).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_shaft_dir(struct <devname>_dev *device, uint8_t *dir)
{
	uint32_t reg_val;
	int ret;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_GCONF, &reg_val);
	if (ret)
		return ret;

	*dir = no_os_field_get(<DEVNAME>_SHAFT_MSK, reg_val);

	return 0;
}

/**
 * @brief Set the shaft direction.
 * @param device - Device descriptor.
 * @param dir    - Direction (0=normal, 1=reverse).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_shaft_dir(struct <devname>_dev *device, uint8_t dir)
{
	return <devname>_reg_update(device, <DEVNAME>_REG_GCONF,
				    <DEVNAME>_SHAFT_MSK,
				    no_os_field_prep(<DEVNAME>_SHAFT_MSK, dir));
}

/**
 * @brief Set maximum acceleration (AMAX).
 * @param device - Device descriptor.
 * @param amax   - AMAX value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_amax(struct <devname>_dev *device, int32_t amax)
{
	return <devname>_reg_write(device, <DEVNAME>_REG_AMAX,
				   (uint32_t)(amax & <DEVNAME>_AMAX_MSK));
}

/**
 * @brief Get maximum acceleration (AMAX).
 * @param device - Device descriptor.
 * @param amax   - Pointer to store AMAX value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_amax(struct <devname>_dev *device, int32_t *amax)
{
	uint32_t temp;
	int ret;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_AMAX, &temp);
	if (ret)
		return ret;

	*amax = (int32_t)temp;

	return 0;
}

/**
 * @brief Set maximum velocity (VMAX).
 * @param device - Device descriptor.
 * @param vmax   - VMAX value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_vmax(struct <devname>_dev *device, int32_t vmax)
{
	return <devname>_reg_write(device, <DEVNAME>_REG_VMAX,
				   (uint32_t)(vmax & <DEVNAME>_VMAX_MSK));
}

/**
 * @brief Get maximum velocity (VMAX).
 * @param device - Device descriptor.
 * @param vmax   - Pointer to store VMAX value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_vmax(struct <devname>_dev *device, int32_t *vmax)
{
	uint32_t temp;
	int ret;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_VMAX, &temp);
	if (ret)
		return ret;

	*vmax = (int32_t)temp;

	return 0;
}

/**
 * @brief Set maximum deceleration (DMAX).
 * @param device - Device descriptor.
 * @param dmax   - DMAX value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_dmax(struct <devname>_dev *device, int32_t dmax)
{
	return <devname>_reg_write(device, <DEVNAME>_REG_DMAX,
				   (uint32_t)(dmax & <DEVNAME>_DMAX_MSK));
}

/**
 * @brief Get maximum deceleration (DMAX).
 * @param device - Device descriptor.
 * @param dmax   - Pointer to store DMAX value.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_dmax(struct <devname>_dev *device, int32_t *dmax)
{
	uint32_t temp;
	int ret;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_DMAX, &temp);
	if (ret)
		return ret;

	*dmax = (int32_t)temp;

	return 0;
}

/**
 * @brief Get ramp mode.
 * @param device   - Device descriptor.
 * @param rampmode - Pointer to store ramp mode (0=pos, 1=vel+, 2=vel-, 3=hold).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_rampmode(struct <devname>_dev *device, int32_t *rampmode)
{
	uint32_t temp;
	int ret;

	ret = <devname>_reg_read(device, <DEVNAME>_REG_RAMPMODE, &temp);
	if (ret)
		return ret;

	*rampmode = (int32_t)(temp & <DEVNAME>_RAMPMODE_MSK);

	return 0;
}

/**
 * @brief Set ramp mode.
 * @param device   - Device descriptor.
 * @param rampmode - Ramp mode (0=pos, 1=vel+, 2=vel-, 3=hold).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_rampmode(struct <devname>_dev *device, int32_t rampmode)
{
	return <devname>_reg_write(device, <DEVNAME>_REG_RAMPMODE,
				   (uint32_t)(rampmode & <DEVNAME>_RAMPMODE_MSK));
}

/**
 * @brief Stop the motor by setting VMAX to 0.
 * @param device - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_stop(struct <devname>_dev *device)
{
	return <devname>_reg_write(device, <DEVNAME>_REG_VMAX, 0);
}

/* ---------- Init / Remove ---------------------------------------------- */

/**
 * @brief Initialise the <DEVNAME> motor controller.
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
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

	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init_param);
	if (ret)
		goto err_free;

	dev->clock = init_param->clock;

	/* Configure driver settings (DRV_CONF) */
	ret = <devname>_reg_write(dev, <DEVNAME>_REG_DRV_CONF,
				  no_os_field_prep(<DEVNAME>_SLOPE_CONTROL_MSK,
						   init_param->slope_control) |
				  no_os_field_prep(<DEVNAME>_CURRENT_RANGE_MSK,
						   init_param->current_range));
	if (ret)
		goto err_spi;

	/* Configure motor current (GLOBAL_SCALER + IHOLD_IRUN) */
	ret = <devname>_reg_write(dev, <DEVNAME>_REG_GLOBAL_SCALER,
				  init_param->global_scaler);
	if (ret)
		goto err_spi;

	ret = <devname>_reg_write(dev, <DEVNAME>_REG_IHOLD_IRUN,
				  no_os_field_prep(<DEVNAME>_IRUNDELAY_MSK,
						   init_param->irundelay) |
				  no_os_field_prep(<DEVNAME>_IHOLDDELAY_MSK,
						   init_param->iholddelay) |
				  no_os_field_prep(<DEVNAME>_IRUN_MSK,
						   init_param->current_run) |
				  no_os_field_prep(<DEVNAME>_IHOLD_MSK,
						   init_param->current_hold));
	if (ret)
		goto err_spi;

	/* Set microstepping resolution and chopper settings */
	dev->microsteps_res = init_param->microsteps_res;

	ret = <devname>_reg_update(dev, <DEVNAME>_REG_CHOPCONF,
				   <DEVNAME>_TOFF_MSK | <DEVNAME>_TBL_MSK |
				   <DEVNAME>_MRES_MSK,
				   no_os_field_prep(<DEVNAME>_TOFF_MSK,
						    init_param->toff) |
				   no_os_field_prep(<DEVNAME>_TBL_MSK,
						    init_param->tbl) |
				   no_os_field_prep(<DEVNAME>_MRES_MSK,
						    init_param->microsteps_res));
	if (ret)
		goto err_spi;

	dev->step_angle_millidegrees = init_param->step_angle_millidegrees;

	/* Set initial ramp mode to position */
	ret = <devname>_reg_write(dev, <DEVNAME>_REG_RAMPMODE,
				  <DEVNAME>_MODE_POSITION);
	if (ret)
		goto err_spi;

	/* Store motion profile parameters */
	dev->amax = init_param->amax;
	dev->vmax = init_param->vmax;
	dev->dmax = init_param->dmax;
	dev->vstart = init_param->vstart;
	dev->v1 = init_param->v1;
	dev->a1 = init_param->a1;
	dev->v2 = init_param->v2;
	dev->a2 = init_param->a2;
	dev->d1 = init_param->d1;
	dev->d2 = init_param->d2;
	dev->vstop = init_param->vstop;

	/* Apply motion profile to hardware */
	ret = <devname>_set_motion_profile(dev);
	if (ret)
		goto err_spi;

	*device = dev;

	return 0;

err_spi:
	no_os_spi_remove(dev->spi_desc);
err_free:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free resources allocated by <devname>_init().
 * @param device - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_remove(struct <devname>_dev *device)
{
	int ret;

	if (!device)
		return -EINVAL;

	ret = no_os_spi_remove(device->spi_desc);
	if (ret)
		return ret;

	no_os_free(device);

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
 * @enum <devname>_iio_attr
 * @brief IIO attribute identifiers for channel and device attributes.
 */
enum <devname>_iio_attr {
	/** Raw channel value. */
	<DEVNAME>_CH_ATTR_RAW,
	/** Scale factor for unit conversion. */
	<DEVNAME>_CH_ATTR_SCALE,
	/** Calibration scale (motor step angle). */
	<DEVNAME>_CH_ATTR_CALIBSCALE,
	/** Position preset (sets XACTUAL, switches to hold mode). */
	<DEVNAME>_CH_ATTR_PRESET,
	/** Shaft direction device attribute. */
	<DEVNAME>_ATTR_DIRECTION,
	/** Stop motor device attribute. */
	<DEVNAME>_ATTR_STOP,
	/** Maximum acceleration device attribute. */
	<DEVNAME>_ATTR_AMAX,
	/** Maximum velocity device attribute. */
	<DEVNAME>_ATTR_VMAX,
	/** Maximum deceleration device attribute. */
	<DEVNAME>_ATTR_DMAX,
	/** Start velocity device attribute. */
	<DEVNAME>_ATTR_VSTART,
	/** First acceleration phase device attribute. */
	<DEVNAME>_ATTR_A1,
	/** First velocity threshold device attribute. */
	<DEVNAME>_ATTR_V1,
	/** Second acceleration phase device attribute. */
	<DEVNAME>_ATTR_A2,
	/** Second velocity threshold device attribute. */
	<DEVNAME>_ATTR_V2,
	/** First deceleration phase device attribute. */
	<DEVNAME>_ATTR_D1,
	/** Second deceleration phase device attribute. */
	<DEVNAME>_ATTR_D2,
	/** Stop velocity device attribute. */
	<DEVNAME>_ATTR_VSTOP,
};

/**
 * @struct <devname>_iio_dev
 * @brief  IIO device descriptor for motor controller.
 */
struct <devname>_iio_dev {
	/** Core motor controller descriptor. */
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
	struct <devname>_init_param *<devname>_init_param;
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

```c
/***************************************************************************//**
 *   @file   iio_<devname>.c
 *   @brief  Implementation of <DEVNAME> IIO Driver
 *   @author Your Name (your.name@analog.com)
 ********************************************************************************
 * ... (same BSD-3-Clause block) ...
 *******************************************************************************/

#include <errno.h>
#include "no_os_spi.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio.h"
#include "iio_<devname>.h"
#include "<devname>.h"

/* ---------- Forward declarations --------------------------------------- */

static int <devname>_iio_reg_read(struct <devname>_iio_dev *dev, uint32_t reg,
				  uint32_t *readval);

static int <devname>_iio_reg_write(struct <devname>_iio_dev *dev, uint32_t reg,
				   uint32_t writeval);

static int <devname>_iio_attr_show(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int <devname>_iio_attr_store(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

/* ---------- Device-level attributes (motion profile) ------------------- */

static struct iio_attribute <devname>_iio_attrs[] = {
	{
		.name = "direction",
		.priv = <DEVNAME>_ATTR_DIRECTION,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "stop",
		.priv = <DEVNAME>_ATTR_STOP,
		.store = <devname>_iio_attr_store,
		.show = NULL,
	},
	{
		.name = "amax",
		.priv = <DEVNAME>_ATTR_AMAX,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "vmax",
		.priv = <DEVNAME>_ATTR_VMAX,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "dmax",
		.priv = <DEVNAME>_ATTR_DMAX,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "vstart",
		.priv = <DEVNAME>_ATTR_VSTART,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "a1",
		.priv = <DEVNAME>_ATTR_A1,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "v1",
		.priv = <DEVNAME>_ATTR_V1,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "a2",
		.priv = <DEVNAME>_ATTR_A2,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "v2",
		.priv = <DEVNAME>_ATTR_V2,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "d1",
		.priv = <DEVNAME>_ATTR_D1,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "d2",
		.priv = <DEVNAME>_ATTR_D2,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "vstop",
		.priv = <DEVNAME>_ATTR_VSTOP,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel attributes ----------------------------------------- */

static struct iio_attribute <devname>_iio_acceleration_attrs[] = {
	{
		.name = "raw",
		.priv = <DEVNAME>_CH_ATTR_RAW,
		.store = NULL,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "scale",
		.priv = <DEVNAME>_CH_ATTR_SCALE,
		.store = NULL,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "calibscale",
		.priv = <DEVNAME>_CH_ATTR_CALIBSCALE,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
		.shared = IIO_SHARED_BY_ALL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute <devname>_iio_velocity_attrs[] = {
	{
		.name = "raw",
		.priv = <DEVNAME>_CH_ATTR_RAW,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "scale",
		.priv = <DEVNAME>_CH_ATTR_SCALE,
		.store = NULL,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "calibscale",
		.priv = <DEVNAME>_CH_ATTR_CALIBSCALE,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
		.shared = IIO_SHARED_BY_ALL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute <devname>_iio_position_attrs[] = {
	{
		.name = "raw",
		.priv = <DEVNAME>_CH_ATTR_RAW,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "scale",
		.priv = <DEVNAME>_CH_ATTR_SCALE,
		.store = NULL,
		.show = <devname>_iio_attr_show,
	},
	{
		.name = "calibscale",
		.priv = <DEVNAME>_CH_ATTR_CALIBSCALE,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "preset",
		.priv = <DEVNAME>_CH_ATTR_PRESET,
		.store = <devname>_iio_attr_store,
		.show = <devname>_iio_attr_show,
	},
	END_ATTRIBUTES_ARRAY
};

/* ---------- Channel definitions ---------------------------------------- */

/**
 * Motor controller IIO channels:
 *   - IIO_ANGL_VEL (angular velocity)  -- output, writable raw = set target
 *   - IIO_ACCEL    (angular accel.)     -- output, read-only raw = actual
 *   - IIO_ANGL     (angular position)   -- output, writable raw = set target
 */
static struct iio_channel <devname>_iio_channels[] = {
	{
		.name = "angular_velocity",
		.ch_type = IIO_ANGL_VEL,
		.channel = 0,
		.attributes = <devname>_iio_velocity_attrs,
		.ch_out = true,
	},
	{
		.name = "angular_acceleration",
		.ch_type = IIO_ACCEL,
		.channel = 0,
		.attributes = <devname>_iio_acceleration_attrs,
		.ch_out = true,
	},
	{
		.name = "angular_position",
		.ch_type = IIO_ANGL,
		.channel = 0,
		.attributes = <devname>_iio_position_attrs,
		.ch_out = true,
	},
};

/* ---------- IIO device structure --------------------------------------- */

static struct iio_device <devname>_iio_device = {
	.attributes = <devname>_iio_attrs,
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels),
	.debug_reg_read = (int32_t (*)(void *, uint32_t,
				       uint32_t *))<devname>_iio_reg_read,
	.debug_reg_write = (int32_t (*)(void *, uint32_t,
					uint32_t))<devname>_iio_reg_write,
};

/* ---------- Register access wrappers ----------------------------------- */

static int <devname>_iio_reg_read(struct <devname>_iio_dev *dev, uint32_t reg,
				  uint32_t *readval)
{
	if (!dev)
		return -ENODEV;

	if (reg > 0x7F)
		return -EINVAL;

	return <devname>_reg_read(dev-><devname>_dev, (uint8_t)reg, readval);
}

static int <devname>_iio_reg_write(struct <devname>_iio_dev *dev, uint32_t reg,
				   uint32_t writeval)
{
	if (!dev)
		return -ENODEV;

	if (reg > 0x7F)
		return -EINVAL;

	return <devname>_reg_write(dev-><devname>_dev, (uint8_t)reg, writeval);
}

/* ---------- Attribute show callback ------------------------------------ */

/**
 * @brief Read an IIO attribute.
 * @param dev     - IIO device descriptor (void* cast).
 * @param buf     - Buffer to format value into.
 * @param len     - Buffer length.
 * @param channel - Channel info (type used to dispatch per-channel reads).
 * @param priv    - Attribute identifier from enum <devname>_iio_attr.
 * @return Number of bytes written on success, negative error code otherwise.
 */
static int <devname>_iio_attr_show(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct <devname>_iio_dev *iio_desc;
	int ret;
	int32_t vals[2];
	uint8_t dir;

	if (!dev)
		return -ENODEV;

	iio_desc = (struct <devname>_iio_dev *)dev;

	switch (priv) {
	case <DEVNAME>_ATTR_DIRECTION:
		ret = <devname>_get_shaft_dir(iio_desc-><devname>_dev, &dir);
		if (ret)
			return ret;

		vals[0] = dir ? 1 : 0;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case <DEVNAME>_ATTR_AMAX:
		ret = <devname>_get_amax(iio_desc-><devname>_dev, &vals[0]);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case <DEVNAME>_ATTR_VMAX:
		ret = <devname>_get_vmax(iio_desc-><devname>_dev, &vals[0]);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case <DEVNAME>_ATTR_DMAX:
		ret = <devname>_get_dmax(iio_desc-><devname>_dev, &vals[0]);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case <DEVNAME>_ATTR_VSTART:
	case <DEVNAME>_ATTR_A1:
	case <DEVNAME>_ATTR_V1:
	case <DEVNAME>_ATTR_A2:
	case <DEVNAME>_ATTR_V2:
	case <DEVNAME>_ATTR_D1:
	case <DEVNAME>_ATTR_D2:
	case <DEVNAME>_ATTR_VSTOP:
		/* Each motion parameter follows the same get pattern.
		 * Map priv -> getter function, read into vals[0],
		 * return as IIO_VAL_INT.
		 * (See tmc5240 reference for per-case implementation.) */
		return -ENOTSUP; /* Replace with per-attribute getter calls */
	case <DEVNAME>_CH_ATTR_RAW:
		switch (channel->type) {
		case IIO_ACCEL:
			ret = <devname>_get_current_accel(
					iio_desc-><devname>_dev, &vals[0]);
			if (ret)
				return ret;
			break;
		case IIO_ANGL_VEL:
			ret = <devname>_get_current_vel(
					iio_desc-><devname>_dev, &vals[0]);
			if (ret)
				return ret;
			break;
		case IIO_ANGL:
			ret = <devname>_get_current_pos(
					iio_desc-><devname>_dev, &vals[0]);
			if (ret)
				return ret;
			break;
		default:
			return -ENOTSUP;
		}

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case <DEVNAME>_CH_ATTR_SCALE:
		switch (channel->type) {
		case IIO_ACCEL: {
			uint64_t clock_sq;

			clock_sq = (uint64_t)iio_desc-><devname>_dev->clock *
				   (uint64_t)iio_desc-><devname>_dev->clock;
			vals[0] = (int32_t)(clock_sq >> 40);
			vals[1] = 16 - iio_desc-><devname>_dev->microsteps_res;

			return iio_format_value(buf, len,
						IIO_VAL_FRACTIONAL_LOG2,
						2, vals);
		}
		case IIO_ANGL_VEL:
			vals[0] = iio_desc-><devname>_dev->clock;
			vals[1] = 32 - iio_desc-><devname>_dev->microsteps_res;

			return iio_format_value(buf, len,
						IIO_VAL_FRACTIONAL_LOG2,
						2, vals);
		case IIO_ANGL:
			vals[0] = 1;
			vals[1] = 1 << (8 - iio_desc-><devname>_dev->microsteps_res);

			return iio_format_value(buf, len,
						IIO_VAL_FRACTIONAL,
						2, vals);
		default:
			return -ENOTSUP;
		}
	case <DEVNAME>_CH_ATTR_CALIBSCALE:
		vals[0] = iio_desc-><devname>_dev->step_angle_millidegrees / 1000;
		vals[1] = (iio_desc-><devname>_dev->step_angle_millidegrees % 1000) * 1000;

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO,
					2, vals);
	case <DEVNAME>_CH_ATTR_PRESET:
		ret = <devname>_get_current_pos(iio_desc-><devname>_dev,
						&vals[0]);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	default:
		return -ENOTSUP;
	}
}

/* ---------- Attribute store callback ----------------------------------- */

/**
 * @brief Write an IIO attribute.
 * @param dev     - IIO device descriptor (void* cast).
 * @param buf     - Buffer containing value string.
 * @param len     - Buffer length.
 * @param channel - Channel info.
 * @param priv    - Attribute identifier from enum <devname>_iio_attr.
 * @return Number of bytes consumed on success, negative error code otherwise.
 */
static int <devname>_iio_attr_store(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct <devname>_iio_dev *iio_desc;
	int ret;
	int32_t val;
	int32_t val2;

	if (!dev)
		return -ENODEV;

	iio_desc = (struct <devname>_iio_dev *)dev;

	switch (priv) {
	case <DEVNAME>_ATTR_DIRECTION:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		if (val != 0 && val != 1)
			return -EINVAL;

		ret = <devname>_set_shaft_dir(iio_desc-><devname>_dev,
					      (uint8_t)val);
		if (ret)
			return ret;

		break;
	case <DEVNAME>_ATTR_STOP:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		if (val != 1)
			return -EINVAL;

		ret = <devname>_stop(iio_desc-><devname>_dev);
		if (ret)
			return ret;

		break;
	case <DEVNAME>_ATTR_AMAX:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = <devname>_set_amax(iio_desc-><devname>_dev, val);
		if (ret)
			return ret;

		iio_desc-><devname>_dev->amax = val;
		break;
	case <DEVNAME>_ATTR_VMAX:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = <devname>_set_vmax(iio_desc-><devname>_dev, val);
		if (ret)
			return ret;

		iio_desc-><devname>_dev->vmax = val;
		break;
	case <DEVNAME>_ATTR_DMAX:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = <devname>_set_dmax(iio_desc-><devname>_dev, val);
		if (ret)
			return ret;

		iio_desc-><devname>_dev->dmax = val;
		break;
	/* Repeat pattern for VSTART, A1, V1, A2, V2, D1, D2, VSTOP:
	 *   parse -> set_<param>() -> update dev-><param>
	 * (See tmc5240 reference for full per-case implementation.) */
	case <DEVNAME>_ATTR_VSTART:
	case <DEVNAME>_ATTR_A1:
	case <DEVNAME>_ATTR_V1:
	case <DEVNAME>_ATTR_A2:
	case <DEVNAME>_ATTR_V2:
	case <DEVNAME>_ATTR_D1:
	case <DEVNAME>_ATTR_D2:
	case <DEVNAME>_ATTR_VSTOP:
		return -ENOTSUP; /* Replace with per-attribute setter calls */
	case <DEVNAME>_CH_ATTR_RAW:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		switch (channel->type) {
		case IIO_ANGL_VEL:
			ret = <devname>_set_target_vel(
					iio_desc-><devname>_dev, val);
			if (ret)
				return ret;

			break;
		case IIO_ANGL:
			ret = <devname>_set_target_pos(
					iio_desc-><devname>_dev, val);
			if (ret)
				return ret;

			break;
		default:
			return -ENOTSUP;
		}
		break;
	case <DEVNAME>_CH_ATTR_CALIBSCALE:
		ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO,
				      &val, &val2);
		if (ret)
			return ret;

		val = val * 1000 + val2 / 1000;
		iio_desc-><devname>_dev->step_angle_millidegrees = val;
		break;
	case <DEVNAME>_CH_ATTR_PRESET:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = <devname>_set_current_pos(iio_desc-><devname>_dev, val);
		if (ret)
			return ret;

		break;
	default:
		return -ENOTSUP;
	}

	return len;
}

/* ---------- Init / Remove ---------------------------------------------- */

/**
 * @brief Initialise the IIO descriptor (wraps core driver init).
 * @param iio_dev    - Pointer to IIO device descriptor pointer.
 * @param init_param - IIO initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_iio_init(struct <devname>_iio_dev **iio_dev,
		       struct <devname>_iio_dev_init_param *init_param)
{
	struct <devname>_iio_dev *descriptor;
	int ret;

	if (!iio_dev || !init_param)
		return -EINVAL;

	descriptor = (struct <devname>_iio_dev *)no_os_calloc(1,
							      sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = <devname>_init(&descriptor-><devname>_dev,
			     init_param-><devname>_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &<devname>_iio_device;

	*iio_dev = descriptor;

	return 0;

free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free IIO descriptor resources.
 * @param desc - IIO device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_iio_remove(struct <devname>_iio_dev *desc)
{
	if (!desc)
		return -EINVAL;

	<devname>_remove(desc-><devname>_dev);
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
	$(INCLUDE)/no_os_units.h     \
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

INCS += $(DRIVERS)/motor/<devname>/<devname>.h
SRCS += $(DRIVERS)/motor/<devname>/<devname>.c
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
INCS += $(DRIVERS)/motor/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/motor/<devname>/iio_<devname>.c

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
	.spi_init_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_3,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
		.parent = NULL,
	},
	.clock = 16000000,
	.step_angle_millidegrees = 1800,
	.microsteps_res = 0,
	.amax = 1000,
	.vmax = 50000,
	.dmax = 1000,
	.vstart = 0,
	.v1 = 0,
	.a1 = 0,
	.v2 = 0,
	.a2 = 0,
	.d1 = 0,
	.d2 = 0,
	.vstop = 10,
	.current_hold = 8,
	.current_run = 16,
	.iholddelay = 6,
	.irundelay = 0,
	.toff = 3,
	.tbl = 2,
	.global_scaler = 128,
	.slope_control = 0,
	.current_range = 0,
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
	int32_t position;
	int32_t velocity;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	/* Move to position 51200 (one full revolution at 256 microsteps) */
	ret = <devname>_set_target_pos(dev, 51200);
	if (ret)
		goto free_dev;

	while (1) {
		ret = <devname>_get_current_pos(dev, &position);
		if (ret)
			goto free_dev;

		ret = <devname>_get_current_vel(dev, &velocity);
		if (ret)
			goto free_dev;

		pr_info("Position: %d  Velocity: %d\r\n", position, velocity);
		no_os_mdelay(500);
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
	struct <devname>_iio_dev *<devname>_iio_desc;
	struct <devname>_iio_dev_init_param <devname>_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	<devname>_iio_ip.<devname>_init_param = &<devname>_ip;
	ret = <devname>_iio_init(&<devname>_iio_desc, &<devname>_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "<devname>",
			.dev = <devname>_iio_desc,
			.dev_descriptor = <devname>_iio_desc->iio_dev,
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
	<devname>_iio_remove(<devname>_iio_desc);
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
#define SPI_MAX_SPEED	4000000
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

The <DEVNAME> is a stepper motor controller/driver with an SPI-compatible
serial interface. It integrates a motion controller with configurable
multi-phase acceleration/deceleration ramps, position and velocity
operating modes, and up to 256x microstepping. <Add 2-3 more sentences
from the datasheet covering key specifications and features.>

Applications
------------

* Stepper motor control
* Industrial automation and robotics
* Precision positioning systems
* 3D printers and CNC machines

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface, configures the driver
settings (current, chopper, microstepping), and applies the initial
motion profile. The function uses goto-based cleanup to release
resources on failure.

Motion Profile Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_motion_profile`` function programs the full
S-curve or trapezoidal motion profile into the device, including
multi-phase acceleration (A1, AMAX, A2), velocity thresholds
(V1, VMAX, V2), and deceleration parameters (D1, DMAX, D2, VSTOP).
Default values are applied for D1, D2, and VSTOP if they are zero.

Position Mode
~~~~~~~~~~~~~

The ``<devname>_set_target_pos`` function switches to position mode,
applies the motion profile, and writes the target position. The
motor will ramp to VMAX and decelerate to stop at the target.

Velocity Mode
~~~~~~~~~~~~~

The ``<devname>_set_target_vel`` function switches to velocity mode
and sets the target speed. Positive values select positive velocity
ramp mode, negative values select negative velocity ramp mode.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .spi_init_param = { /* platform-specific SPI params */ },
           .clock = 16000000,
           .step_angle_millidegrees = 1800,
           .microsteps_res = 0,
           .amax = 1000,
           .vmax = 50000,
           .dmax = 1000,
           .vstop = 10,
           .current_hold = 8,
           .current_run = 16,
           .toff = 3,
           .tbl = 2,
           .global_scaler = 128,
       };

       struct <devname>_dev *dev;
       int32_t position;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       /* Move to position 51200 microsteps */
       ret = <devname>_set_target_pos(dev, 51200);
       if (ret)
           goto cleanup;

       /* Read current position */
       ret = <devname>_get_current_pos(dev, &position);
       if (ret)
           goto cleanup;

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes three output channels:

* ``IIO_ANGL_VEL`` -- angular velocity with ``raw`` (read actual /
  write target), ``scale``, and ``calibscale`` attributes.
* ``IIO_ACCEL`` -- angular acceleration with read-only ``raw``,
  ``scale``, and ``calibscale`` attributes.
* ``IIO_ANGL`` -- angular position with ``raw`` (read actual /
  write target), ``scale``, ``calibscale``, and ``preset`` attributes.

Device-level attributes provide access to the full motion profile
(amax, vmax, dmax, vstart, a1, v1, a2, v2, d1, d2, vstop),
shaft direction, and motor stop control.

IIO Initialization Example
---------------------------

.. code-block:: C

   int iio_example_main()
   {
       int ret;
       struct <devname>_iio_dev *iio_desc;
       struct <devname>_iio_dev_init_param iio_ip;
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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/motor/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/motor/<devname>/README.rst
```

---

## 9. Key Conventions

1. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_calloc(...)`).
2. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
3. **Doxygen** -- document all public structs, fields, and functions.
4. **Return codes** -- all functions return `int`, negative = error.
5. **Init signature** -- `int <devname>_init(struct <devname>_dev **device,
   struct <devname>_init_param *init_param)`.
6. **Allocation** -- use `no_os_calloc` / `no_os_free`, never
   `malloc`/`free`.
7. **Goto cleanup** -- in `_init()`, use goto labels to free in reverse
   allocation order on error.
8. **Remove order** -- `_remove()` frees resources in reverse order of
   `_init()`.
9. **No globals** -- all state lives in the device descriptor struct.
   (Multi-instance lookup tables are acceptable for vendor API
   integration, as in the TMC5240 TMC-API pattern.)
10. **Register macros** -- `NO_OS_BIT()` for single bits,
    `NO_OS_GENMASK(hi, lo)` for fields; `no_os_field_prep()` /
    `no_os_field_get()` for field access.
11. **SPI protocol** -- motor controllers typically use 5-byte SPI frames
    (1 address + 4 data) with bit 7 = write flag. Adapt the frame format
    to match the specific device datasheet.
12. **UART alternative** -- some motor controllers (e.g., TMC7300) use
    UART instead of SPI. In that case, replace `no_os_spi` with
    `no_os_uart` throughout and implement the single-wire protocol.
13. **IIO channel types** -- use `IIO_ANGL_VEL` for velocity,
    `IIO_ACCEL` for acceleration, and `IIO_ANGL` for position.
    All motor channels are typically output (`ch_out = true`).
14. **Motion profile** -- store all ramp parameters in the device
    descriptor and apply them atomically through a
    `set_motion_profile()` function.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> stepper motor controller/driver with
SPI interface, configurable motion profiles, and position/velocity
operating modes.

Signed-off-by: Your Name <your.name@analog.com>
```
