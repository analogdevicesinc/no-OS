# AFE Driver Template

Reference driver: `drivers/afe/ad4110/`

This template covers every file needed to add a new AFE driver to no-OS.
Replace `<devname>` with the part number (e.g., `ad4110`) and `<DEVNAME>`
with its uppercase form (e.g., `AD4110`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `ad4110.h`, `ad4110.c` |
| IIO files | `iio_<devname>.h`, `iio_<devname>.c` | `iio_ad4110.h`, `iio_ad4110.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __AD4110_H__` |
| Init param struct | `<devname>_init_param` | `ad4110_init_param` |
| Device struct | `<devname>_dev` | `ad4110_dev` |
| IIO descriptor | `<devname>_iio_desc` | `ad4110_iio_desc` |
| Functions | `<devname>_<verb>()` | `ad4110_setup()` |
| Static helpers | `_<devname>_<verb>()` | `_ad4110_compute_crc8()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_AD4110` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `AD4110_REG_AFE_CNTRL2_IMODE_MSK` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `AD4110_VOLTAGE_MODE` |

---

## 2. File Checklist

```
drivers/afe/<devname>/
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

doc/sphinx/source/drivers/afe/<devname>.rst   # Sphinx entry
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
#include "no_os_irq.h"

/* ---------------- Command / Address Helpers ------------------------ */

#define <DEVNAME>_CMD_WR_COM_REG(x)	(0x00 | ((x) & 0xF))
#define <DEVNAME>_CMD_READ_COM_REG(x)	(0x40 | ((x) & 0xF))

/* Register map selectors (dual register map: AFE + ADC). */
#define <DEVNAME>_ADC			0x00
#define <DEVNAME>_AFE			0x01

/****************************** AFE Register Map ******************************/

#define <DEVNAME>_REG_AFE_STATUS	0x0
#define <DEVNAME>_REG_AFE_CNTRL1	0x1
#define <DEVNAME>_REG_AFE_CLK_CTRL	0x3
#define <DEVNAME>_REG_AFE_CNTRL2	0x4
#define <DEVNAME>_REG_PGA_RTD_CTRL	0x5
#define <DEVNAME>_REG_AFE_ERR_DISABLE	0x6

/***************************** ADC Register Map *******************************/

#define <DEVNAME>_REG_ADC_STATUS	0x0
#define <DEVNAME>_REG_ADC_MODE		0x1
#define <DEVNAME>_REG_ADC_INTERFACE	0x2
#define <DEVNAME>_REG_ADC_CONFIG	0x3
#define <DEVNAME>_REG_DATA		0x4
#define <DEVNAME>_REG_FILTER		0x5
#define <DEVNAME>_REG_ADC_GPIO_CONFIG	0x6
#define <DEVNAME>_REG_ID		0x7

/* AFE_CNTRL2 field masks */
#define <DEVNAME>_AFE_CNTRL2_IMODE_MSK		NO_OS_BIT(1)
#define <DEVNAME>_AFE_CNTRL2_EXT_R_SEL_MSK	NO_OS_BIT(2)
#define <DEVNAME>_AFE_CNTRL2_EN_FLD_PWR_MSK	NO_OS_BIT(3)

/* PGA_RTD_CTRL field masks */
#define <DEVNAME>_PGA_GAIN_CH_MSK		NO_OS_GENMASK(7, 4)
#define <DEVNAME>_PGA_RTD_23W_EN_MSK		NO_OS_BIT(15)

/* ADC_MODE field masks */
#define <DEVNAME>_ADC_MODE_MSK			NO_OS_GENMASK(6, 4)
#define <DEVNAME>_ADC_MODE_REF_EN		NO_OS_BIT(15)

/* ADC_CONFIG field masks */
#define <DEVNAME>_ADC_CONFIG_CHAN_EN_MSK	NO_OS_GENMASK(3, 0)
#define <DEVNAME>_ADC_CONFIG_REF_SEL_MSK	NO_OS_GENMASK(5, 4)

/* FILTER field masks */
#define <DEVNAME>_FILTER_ODR_MSK		NO_OS_GENMASK(4, 0)

/* ADC conversion timeout */
#define <DEVNAME>_ADC_CONV_TIMEOUT		10000

/* ---------------- Enums -------------------------------------------- */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_adc_mode
 *  @brief ADC operating modes. */
enum <devname>_adc_mode {
	<DEVNAME>_CONTINOUS_CONV_MODE = 0,
	<DEVNAME>_SINGLE_CONV_MODE = 1,
	<DEVNAME>_STANDBY_MODE = 2,
	<DEVNAME>_PW_DOWN_MODE = 3,
	<DEVNAME>_SYS_OFFSET_CAL = 6,
	<DEVNAME>_SYS_GAIN_CAL = 7,
};

/** @enum <devname>_op_mode
 *  @brief AFE input operating modes. */
enum <devname>_op_mode {
	<DEVNAME>_VOLTAGE_MODE,
	<DEVNAME>_CURRENT_MODE,
	<DEVNAME>_CURRENT_MODE_EXT_R_SEL,
	<DEVNAME>_THERMOCOUPLE,
	<DEVNAME>_FLD_POWER_MODE,
	<DEVNAME>_RTD_2W_MODE,
	<DEVNAME>_RTD_3W_MODE,
	<DEVNAME>_RTD_4W_MODE,
};

/** @enum <devname>_voltage_reference
 *  @brief Voltage reference selection. */
enum <devname>_voltage_reference {
	<DEVNAME>_EXT_REF = 0,
	<DEVNAME>_INT_2_5V_REF = 2,
	<DEVNAME>_AVDD5_REF = 3,
};

/** @enum <devname>_gain
 *  @brief PGA gain selection (0.2x to 24x). */
enum <devname>_gain {
	<DEVNAME>_GAIN_0_2,
	<DEVNAME>_GAIN_0_25,
	<DEVNAME>_GAIN_0_3,
	<DEVNAME>_GAIN_0_375,
	<DEVNAME>_GAIN_0_5,
	<DEVNAME>_GAIN_0_75,
	<DEVNAME>_GAIN_1,
	<DEVNAME>_GAIN_1_5,
	<DEVNAME>_GAIN_2,
	<DEVNAME>_GAIN_3,
	<DEVNAME>_GAIN_4,
	<DEVNAME>_GAIN_6,
	<DEVNAME>_GAIN_8,
	<DEVNAME>_GAIN_12,
	<DEVNAME>_GAIN_16,
	<DEVNAME>_GAIN_24,
};

/** @enum <devname>_odr
 *  @brief Output data rate selection. */
enum <devname>_odr {
	<DEVNAME>_ODR_125_KSPS,
	<DEVNAME>_ODR_62P5_KSPS,
	<DEVNAME>_ODR_31P25_KSPS,
	<DEVNAME>_ODR_25_KSPS,
	/* Add more rates as needed */
};

/* Add more enums as needed: clock config, CRC mode, filter order, etc. */

/* ---------------- Structures --------------------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_setup().
 */
struct <devname>_init_param {
	/** SPI initialization parameters. */
	struct no_os_spi_init_param spi_init;
	/** AFE operating mode (voltage/current/thermocouple/RTD). */
	enum <devname>_op_mode op_mode;
	/** PGA gain setting. */
	enum <devname>_gain gain;
	/** Output data rate. */
	enum <devname>_odr odr;
	/** Voltage reference selection. */
	enum <devname>_voltage_reference volt_ref;
	/** IRQ controller descriptor (for continuous mode). */
	struct no_os_irq_ctrl_desc *irq_desc;
	/** Data-ready GPIO pin number. */
	uint32_t nready_pin;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor -- allocated by <devname>_setup().
 */
struct <devname>_dev {
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_dev;
	/** Active AFE operating mode. */
	enum <devname>_op_mode op_mode;
	/** Active PGA gain. */
	enum <devname>_gain gain;
	/** Active output data rate. */
	enum <devname>_odr odr;
	/** Active voltage reference. */
	enum <devname>_voltage_reference volt_ref;
	/** IRQ controller descriptor. */
	struct no_os_irq_ctrl_desc *irq_desc;
	/** Data-ready GPIO pin number. */
	uint32_t nready_pin;
};

/**
 * @struct <devname>_callback_ctx
 * @brief  Context passed to the continuous-read IRQ handler.
 */
struct <devname>_callback_ctx {
	/** Device descriptor. */
	struct <devname>_dev *dev;
	/** Pointer to sample buffer. */
	uint32_t *buffer;
	/** Remaining samples to read. */
	uint32_t buffer_size;
};

/* ---------------- Function Prototypes ------------------------------ */

/** @brief Initialise and configure the device. */
int32_t <devname>_setup(struct <devname>_dev **device,
			struct <devname>_init_param init_param);

/** @brief Free resources allocated by <devname>_setup(). */
int32_t <devname>_remove(struct <devname>_dev *dev);

/** @brief Read a register (AFE or ADC register map). */
int32_t <devname>_spi_int_reg_read(struct <devname>_dev *dev,
				   uint8_t reg_map,
				   uint8_t reg_addr,
				   uint32_t *reg_data);

/** @brief Write a register (AFE or ADC register map). */
int32_t <devname>_spi_int_reg_write(struct <devname>_dev *dev,
				    uint8_t reg_map,
				    uint8_t reg_addr,
				    uint32_t reg_data);

/** @brief Set the ADC conversion mode. */
int32_t <devname>_set_adc_mode(struct <devname>_dev *dev,
			       enum <devname>_adc_mode mode);

/** @brief Set the PGA gain. */
int32_t <devname>_set_gain(struct <devname>_dev *dev,
			   enum <devname>_gain gain);

/** @brief Set the voltage reference. */
int32_t <devname>_set_reference(struct <devname>_dev *dev,
				enum <devname>_voltage_reference ref);

/** @brief Set the AFE operating mode. */
int32_t <devname>_set_op_mode(struct <devname>_dev *dev,
			      enum <devname>_op_mode mode);

/** @brief Read samples in continuous conversion mode using IRQ. */
int32_t <devname>_continuous_read(struct <devname>_dev *dev,
				  uint32_t *buffer,
				  uint32_t buffer_size);

/** @brief Perform a single ADC conversion and return the result. */
int <devname>_do_single_read(struct <devname>_dev *dev,
			     uint32_t *buffer);

/** @brief Enable or disable an ADC channel. */
int <devname>_set_channel_status(struct <devname>_dev *dev,
				 uint8_t chan_id,
				 bool status);

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* ---------- Static helpers ----------------------------------------- */

/**
 * @brief Write a register over SPI (AFE or ADC register map).
 * @param dev      - Device descriptor.
 * @param reg_map  - Register map selector (<DEVNAME>_ADC or <DEVNAME>_AFE).
 * @param reg_addr - Register address.
 * @param reg_data - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_spi_int_reg_write(struct <devname>_dev *dev,
				    uint8_t reg_map,
				    uint8_t reg_addr,
				    uint32_t reg_data)
{
	uint8_t buf[4];

	buf[0] = (reg_map << 7) | <DEVNAME>_CMD_WR_COM_REG(reg_addr);
	buf[1] = (reg_data & 0xFF00) >> 8;
	buf[2] = (reg_data & 0x00FF) >> 0;

	return no_os_spi_write_and_read(dev->spi_dev, buf, 3);
}

/**
 * @brief Read a register over SPI (AFE or ADC register map).
 * @param dev      - Device descriptor.
 * @param reg_map  - Register map selector (<DEVNAME>_ADC or <DEVNAME>_AFE).
 * @param reg_addr - Register address.
 * @param reg_data - Pointer to store read value.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_spi_int_reg_read(struct <devname>_dev *dev,
				   uint8_t reg_map,
				   uint8_t reg_addr,
				   uint32_t *reg_data)
{
	uint8_t buf[4];
	int32_t ret;

	buf[0] = (reg_map << 7) | <DEVNAME>_CMD_READ_COM_REG(reg_addr);
	memset(buf + 1, 0xAA, 3);

	ret = no_os_spi_write_and_read(dev->spi_dev, buf, 3);
	if (ret)
		return ret;

	*reg_data = (buf[1] << 8) | buf[2];

	return 0;
}

/**
 * @brief Write a register field using a read-modify-write cycle.
 * @param dev      - Device descriptor.
 * @param reg_map  - Register map selector.
 * @param reg_addr - Register address.
 * @param data     - Data to write (pre-shifted).
 * @param mask     - Bit mask for the field.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t <devname>_spi_int_reg_write_msk(struct <devname>_dev *dev,
					       uint8_t reg_map,
					       uint8_t reg_addr,
					       uint32_t data,
					       uint16_t mask)
{
	int32_t ret;
	uint32_t reg_data;

	ret = <devname>_spi_int_reg_read(dev, reg_map, reg_addr, &reg_data);
	if (ret)
		return ret;
	reg_data &= ~mask;
	reg_data |= data;
	return <devname>_spi_int_reg_write(dev, reg_map, reg_addr, reg_data);
}

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Set the ADC conversion mode.
 * @param dev  - Device descriptor.
 * @param mode - ADC mode (continuous/single/standby/power-down/calibration).
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_set_adc_mode(struct <devname>_dev *dev,
			       enum <devname>_adc_mode mode)
{
	return <devname>_spi_int_reg_write_msk(dev,
					       <DEVNAME>_ADC,
					       <DEVNAME>_REG_ADC_MODE,
					       no_os_field_prep(<DEVNAME>_ADC_MODE_MSK, mode),
					       <DEVNAME>_ADC_MODE_MSK);
}

/**
 * @brief Set the PGA gain.
 * @param dev  - Device descriptor.
 * @param gain - Gain value (0.2x to 24x).
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_set_gain(struct <devname>_dev *dev,
			   enum <devname>_gain gain)
{
	int32_t ret;

	ret = <devname>_spi_int_reg_write_msk(dev,
					      <DEVNAME>_AFE,
					      <DEVNAME>_REG_PGA_RTD_CTRL,
					      no_os_field_prep(<DEVNAME>_PGA_GAIN_CH_MSK, gain),
					      <DEVNAME>_PGA_GAIN_CH_MSK);
	if (ret)
		return ret;

	dev->gain = gain;

	return 0;
}

/**
 * @brief Set the voltage reference.
 * @param dev - Device descriptor.
 * @param ref - Voltage reference selection.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_set_reference(struct <devname>_dev *dev,
				enum <devname>_voltage_reference ref)
{
	int32_t ret;

	if (ref == <DEVNAME>_INT_2_5V_REF) {
		ret = <devname>_spi_int_reg_write_msk(dev,
						      <DEVNAME>_ADC,
						      <DEVNAME>_REG_ADC_MODE,
						      <DEVNAME>_ADC_MODE_REF_EN,
						      <DEVNAME>_ADC_MODE_REF_EN);
		if (ret)
			return ret;
	}

	ret = <devname>_spi_int_reg_write_msk(dev,
					      <DEVNAME>_ADC,
					      <DEVNAME>_REG_ADC_CONFIG,
					      no_os_field_prep(<DEVNAME>_ADC_CONFIG_REF_SEL_MSK, ref),
					      <DEVNAME>_ADC_CONFIG_REF_SEL_MSK);
	if (ret)
		return ret;

	dev->volt_ref = ref;

	return 0;
}

/**
 * @brief Set the AFE operating mode.
 * @param dev  - Device descriptor.
 * @param mode - Operating mode (voltage/current/thermocouple/RTD 2W/3W/4W).
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_set_op_mode(struct <devname>_dev *dev,
			      enum <devname>_op_mode mode)
{
	int32_t ret;

	switch (mode) {
	case <DEVNAME>_VOLTAGE_MODE:
		ret = <devname>_spi_int_reg_write_msk(dev,
						      <DEVNAME>_AFE,
						      <DEVNAME>_REG_AFE_CNTRL2,
						      0,
						      <DEVNAME>_AFE_CNTRL2_IMODE_MSK);
		break;
	case <DEVNAME>_CURRENT_MODE:
		ret = <devname>_spi_int_reg_write_msk(dev,
						      <DEVNAME>_AFE,
						      <DEVNAME>_REG_AFE_CNTRL2,
						      <DEVNAME>_AFE_CNTRL2_IMODE_MSK,
						      <DEVNAME>_AFE_CNTRL2_IMODE_MSK);
		break;
	/* Add cases for THERMOCOUPLE, RTD_2W, RTD_3W, RTD_4W as needed. */
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	dev->op_mode = mode;

	return 0;
}

/**
 * @brief Enable or disable an ADC channel.
 * @param dev     - Device descriptor.
 * @param chan_id - Channel index (0-based).
 * @param status  - true to enable, false to disable.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_channel_status(struct <devname>_dev *dev,
				 uint8_t chan_id,
				 bool status)
{
	if (status)
		return <devname>_spi_int_reg_write_msk(dev, <DEVNAME>_ADC,
						       <DEVNAME>_REG_ADC_CONFIG,
						       (1 << chan_id),
						       (1 << chan_id));
	else
		return <devname>_spi_int_reg_write_msk(dev, <DEVNAME>_ADC,
						       <DEVNAME>_REG_ADC_CONFIG,
						       0, (1 << chan_id));
}

/**
 * @brief Wait for the ADC data-ready status bit.
 * @param dev     - Device descriptor.
 * @param timeout - Maximum polling iterations.
 * @return 0 on success, -ETIMEDOUT on timeout.
 */
static int <devname>_wait_for_rdy_low(struct <devname>_dev *dev,
				      uint32_t timeout)
{
	uint32_t read_data;
	int ret;

	while (timeout--) {
		ret = <devname>_spi_int_reg_read(dev, <DEVNAME>_ADC,
						 <DEVNAME>_REG_ADC_STATUS,
						 &read_data);
		if (ret)
			return ret;

		if (!(read_data & NO_OS_BIT(7)))
			return 0;
	}

	return -ETIMEDOUT;
}

/**
 * @brief Perform a single ADC conversion.
 * @param dev    - Device descriptor.
 * @param buffer - Pointer to store the conversion result.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_do_single_read(struct <devname>_dev *dev, uint32_t *buffer)
{
	int ret;

	ret = <devname>_set_adc_mode(dev, <DEVNAME>_SINGLE_CONV_MODE);
	if (ret)
		return ret;

	ret = <devname>_wait_for_rdy_low(dev, <DEVNAME>_ADC_CONV_TIMEOUT);
	if (ret)
		return ret;

	return <devname>_spi_int_reg_read(dev, <DEVNAME>_ADC,
					  <DEVNAME>_REG_DATA, buffer);
}

/**
 * @brief IRQ handler for continuous ADC reads.
 */
static void irq_adc_read(struct <devname>_callback_ctx *ctx)
{
	struct <devname>_dev *dev = ctx->dev;

	if (ctx->buffer_size > 0) {
		<devname>_spi_int_reg_read(dev, <DEVNAME>_ADC,
					   <DEVNAME>_REG_DATA, ctx->buffer);
		ctx->buffer_size--;
		ctx->buffer++;
		no_os_irq_enable(dev->irq_desc, dev->nready_pin);
	} else {
		no_os_irq_disable(dev->irq_desc, dev->nready_pin);
	}
}

/**
 * @brief Read samples in continuous conversion mode using IRQ.
 * @param dev         - Device descriptor.
 * @param buffer      - Sample buffer.
 * @param buffer_size - Number of samples to read.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_continuous_read(struct <devname>_dev *dev,
				  uint32_t *buffer,
				  uint32_t buffer_size)
{
	int32_t ret;

	struct <devname>_callback_ctx ctx = {
		.dev = dev,
		.buffer = buffer,
		.buffer_size = buffer_size,
	};

	struct no_os_callback_desc irq_callback = {
		.callback = &irq_adc_read,
		.ctx = &ctx,
	};

	ret = no_os_irq_register_callback(dev->irq_desc, dev->nready_pin,
					  &irq_callback);
	if (ret)
		return ret;

	ret = <devname>_set_adc_mode(dev, <DEVNAME>_CONTINOUS_CONV_MODE);
	if (ret)
		return ret;

	no_os_mdelay(2U);

	ret = no_os_irq_enable(dev->irq_desc, dev->nready_pin);
	if (ret)
		return ret;

	while (ctx.buffer_size != 0U)
		;

	return 0;
}

/**
 * @brief Initialise and configure the <DEVNAME> device.
 * @param device     - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_setup(struct <devname>_dev **device,
			struct <devname>_init_param init_param)
{
	struct <devname>_dev *dev;
	int32_t ret;

	dev = (struct <devname>_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_dev, &init_param.spi_init);
	if (ret)
		goto err_dev;

	dev->op_mode = init_param.op_mode;
	dev->gain = init_param.gain;
	dev->volt_ref = init_param.volt_ref;
	dev->odr = init_param.odr;
	dev->irq_desc = init_param.irq_desc;
	dev->nready_pin = init_param.nready_pin;

	/* Apply initial configuration */
	ret = <devname>_set_op_mode(dev, dev->op_mode);
	if (ret)
		goto err_spi;

	ret = <devname>_set_gain(dev, dev->gain);
	if (ret)
		goto err_spi;

	ret = <devname>_set_reference(dev, dev->volt_ref);
	if (ret)
		goto err_spi;

	*device = dev;

	pr_info("<DEVNAME> successfully initialized\n");

	return 0;

err_spi:
	no_os_spi_remove(dev->spi_dev);
err_dev:
	no_os_free(dev);
	pr_err("<DEVNAME> initialization error (%d)\n", ret);

	return ret;
}

/**
 * @brief Free resources allocated by <devname>_setup().
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int32_t <devname>_remove(struct <devname>_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_remove(dev->spi_dev);

	no_os_free(dev);

	return ret;
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
 *
 * In voltage mode this reads IIO_VOLTAGE; in current mode it reads
 * IIO_CURRENT.  The channel type is set at init time based on op_mode.
 */
static int <devname>_iio_read_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct <devname>_iio_desc *desc = dev;
	uint32_t data;
	int32_t val;
	int ret;

	ret = <devname>_do_single_read(desc-><devname>_dev, &data);
	if (ret)
		return ret;

	val = (int32_t)data;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read scale factor (mV per LSB = Vref / 2^bits / gain).
 */
static int <devname>_iio_read_scale(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val[2];

	/*
	 * Example: 2.5V reference, 24-bit ADC, gain = 1
	 * scale = 2500 mV / 16777216 = 0.000149 mV per LSB
	 * Expressed as IIO_VAL_INT_PLUS_MICRO: 0 + 149
	 * Adjust for actual gain and reference at runtime.
	 */
	val[0] = 0;
	val[1] = 149;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/**
 * @brief Read offset (typically 0 for unipolar, -2^(bits-1) for bipolar).
 */
static int <devname>_iio_read_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the current sampling frequency.
 */
static int <devname>_iio_read_samp_freq(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	int32_t val = 125000; /* Default: 125 kSPS — adjust per ODR */

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/* ---------- Attribute table ---------------------------------------- */

static struct iio_attribute <devname>_iio_ch_attrs[] = {
	{ .name = "raw",                .show = <devname>_iio_read_raw },
	{ .name = "scale",              .show = <devname>_iio_read_scale },
	{ .name = "offset",             .show = <devname>_iio_read_offset },
	{ .name = "sampling_frequency", .show = <devname>_iio_read_samp_freq },
	END_ATTRIBUTES_ARRAY
};

/* ---------- Scan type ---------------------------------------------- */

static struct scan_type <devname>_scan_type = {
	.sign = 'u',          /* unsigned (or 's' for bipolar) */
	.realbits = 24,       /* ADC resolution */
	.storagebits = 32,    /* container size */
	.shift = 0,
	.is_big_endian = false,
};

/* ---------- Channel definitions ------------------------------------ */

/**
 * AFE channels: IIO_VOLTAGE or IIO_CURRENT depending on operating mode.
 * ch_out = false (input channels).
 */
#define <DEVNAME>_IIO_VOLTAGE_CHANNEL(idx) \
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

#define <DEVNAME>_IIO_CURRENT_CHANNEL(idx) \
	{ \
		.name = "current" #idx, \
		.ch_type = IIO_CURRENT, \
		.channel = (idx), \
		.scan_type = &<devname>_scan_type, \
		.scan_index = (idx), \
		.attributes = <devname>_iio_ch_attrs, \
		.ch_out = false, \
		.indexed = true, \
	}

static struct iio_channel <devname>_iio_channels[] = {
	<DEVNAME>_IIO_VOLTAGE_CHANNEL(0),
	<DEVNAME>_IIO_VOLTAGE_CHANNEL(1),
	/* Switch to <DEVNAME>_IIO_CURRENT_CHANNEL when in current mode. */
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

	return <devname>_continuous_read(desc-><devname>_dev,
					(uint32_t *)buf, samples);
}

/* ---------- IIO device structure ----------------------------------- */

static struct iio_device <devname>_iio_dev = {
	.channels = <devname>_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(<devname>_iio_channels) - 1,
	.debug_reg_read = (int32_t (*)())<devname>_spi_int_reg_read,
	.debug_reg_write = (int32_t (*)())<devname>_spi_int_reg_write,
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

	ret = <devname>_setup(&d-><devname>_dev,
			      *init_param-><devname>_init_param);
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

INCS += $(DRIVERS)/afe/<devname>/<devname>.h
SRCS += $(DRIVERS)/afe/<devname>/<devname>.c
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
INCS += $(DRIVERS)/afe/<devname>/iio_<devname>.h
SRCS += $(DRIVERS)/afe/<devname>/iio_<devname>.c

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
	.spi_init = <devname>_spi_ip,
	.op_mode = <DEVNAME>_VOLTAGE_MODE,
	.gain = <DEVNAME>_GAIN_1,
	.odr = <DEVNAME>_ODR_125_KSPS,
	.volt_ref = <DEVNAME>_INT_2_5V_REF,
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
	uint32_t adc_data;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_setup(&dev, <devname>_ip);
	if (ret)
		goto error;

	/* Configure operating mode */
	ret = <devname>_set_op_mode(dev, <DEVNAME>_VOLTAGE_MODE);
	if (ret)
		goto free_dev;

	/* Set gain and reference */
	ret = <devname>_set_gain(dev, <DEVNAME>_GAIN_1);
	if (ret)
		goto free_dev;

	ret = <devname>_set_reference(dev, <DEVNAME>_INT_2_5V_REF);
	if (ret)
		goto free_dev;

	while (1) {
		ret = <devname>_do_single_read(dev, &adc_data);
		if (ret)
			goto free_dev;

		pr_info("AFE raw value: %lu\r\n", (unsigned long)adc_data);
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

The <DEVNAME> is a universal input analog front end with an integrated
<resolution>-bit sigma-delta ADC. It supports multiple input modes
including voltage, current, thermocouple, and 2-wire/3-wire/4-wire RTD
measurements. The integrated PGA provides gains from 0.2x to 24x, and
the SPI interface uses a dual register map for independent AFE and ADC
configuration. <Add 2-3 more sentences from the datasheet covering
key specs and features.>

Applications
------------

* Industrial process control
* Temperature measurement (thermocouple / RTD)
* Programmable logic controllers
* Field instruments

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_setup`` function allocates memory for the device
descriptor, initialises the SPI interface, and applies the initial
AFE and ADC configuration including operating mode, gain, voltage
reference, and output data rate. The function uses goto-based cleanup
to release resources on failure.

Operating Mode Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_op_mode`` function configures the AFE front end
for the desired input type: voltage, current, current with external
sense resistor, thermocouple, field power, or RTD (2-wire, 3-wire,
4-wire). Each mode programs the appropriate AFE control register
fields such as IMODE, VBIAS, and RTD excitation currents.

Gain and Reference
~~~~~~~~~~~~~~~~~~

The ``<devname>_set_gain`` function programs the PGA gain through
the AFE register map, selecting from 16 gain settings (0.2x to 24x).
The ``<devname>_set_reference`` function configures the ADC voltage
reference: external, internal 2.5V (with REF_EN), or AVDD5.

Data Acquisition
~~~~~~~~~~~~~~~~

Two acquisition modes are provided. The ``<devname>_do_single_read``
function sets the ADC to single conversion mode, polls the status
register for data-ready, and reads the result. The
``<devname>_continuous_read`` function uses an IRQ-driven approach
with a callback to fill a sample buffer at the configured output
data rate.

Channel Configuration
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_channel_status`` function enables or disables
individual ADC channels by modifying the channel enable mask in the
ADC configuration register. Up to four channels can be independently
controlled.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .spi_init = { /* platform-specific SPI params */ },
           .op_mode = <DEVNAME>_VOLTAGE_MODE,
           .gain = <DEVNAME>_GAIN_1,
           .odr = <DEVNAME>_ODR_125_KSPS,
           .volt_ref = <DEVNAME>_INT_2_5V_REF,
       };

       struct <devname>_dev *dev;
       uint32_t sample;
       int ret;

       ret = <devname>_setup(&dev, ip);
       if (ret)
           return ret;

       ret = <devname>_do_single_read(dev, &sample);
       if (ret)
           goto cleanup;

       /* sample contains the raw ADC value */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }

IIO Support
-----------

The <DEVNAME> IIO driver exposes each AFE input as an ``IIO_VOLTAGE``
or ``IIO_CURRENT`` channel (depending on the configured operating mode)
with ``raw``, ``scale``, ``offset``, and ``sampling_frequency``
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

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/afe/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/afe/<devname>/README.rst
```

---

## 9. Key Conventions

1. **Dual register map** -- AFE drivers typically address two independent
   register banks (AFE + ADC) via a `reg_map` parameter on every
   register access function.
2. **C/C++ compatibility** -- explicit casts from `void *` (e.g.,
   `(struct <devname>_dev *)no_os_malloc(...)`).
3. **Linux kernel style** -- tabs = 8 spaces, 80-char line limit.
4. **Doxygen** -- document all public structs, fields, and functions.
5. **Return codes** -- all functions return `int` or `int32_t`,
   negative = error.
6. **Init signature** -- `int32_t <devname>_setup(struct <devname>_dev
   **device, struct <devname>_init_param init_param)` (note: init_param
   passed by value, matching the ad4110 convention).
7. **Allocation** -- use `no_os_malloc` / `no_os_calloc` / `no_os_free`,
   never `malloc`/`free`.
8. **Goto cleanup** -- in `_setup()`, use goto labels to free in reverse
   allocation order on error.
9. **Remove order** -- `_remove()` frees resources in reverse order of
   `_setup()`.
10. **No globals** -- all state lives in the device descriptor struct.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> universal input analog front end with
integrated <resolution>-bit sigma-delta ADC. Supports voltage,
current, thermocouple, and RTD measurement modes over SPI.

Signed-off-by: Your Name <your.name@analog.com>
```
