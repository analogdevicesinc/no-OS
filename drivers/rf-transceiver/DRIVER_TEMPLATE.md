# RF Transceiver Driver Template

Reference driver: `drivers/rf-transceiver/adf7023/`

This template covers every file needed to add a new RF transceiver driver to
no-OS. Replace `<devname>` with the part number (e.g., `adf7023`) and
`<DEVNAME>` with its uppercase form (e.g., `ADF7023`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `adf7023.h`, `adf7023.c` |
| Config header | `<devname>_config.h` | `adf7023_config.h` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __ADF7023_H__` |
| Init param struct | `<devname>_init_param` | `adf7023_init_param` |
| Device struct | `<devname>_dev` | `adf7023_dev` |
| Functions | `<devname>_<verb>()` | `adf7023_init()` |
| Static helpers | `_<devname>_<verb>()` | `_adf7023_wait_ready()` |
| Device ID enum | `ID_<DEVNAME>` | `ID_ADF7023` |
| Register addresses | `<DEVNAME>_REG_<NAME>` | `ADF7023_REG_RADIO_CFG_0` |
| Register masks | `<DEVNAME>_<REG>_<FIELD>_MSK` | `ADF7023_CFG8_PA_LEVEL_MSK` |
| FW state defines | `<DEVNAME>_FW_STATE_<STATE>` | `ADF7023_FW_STATE_PHY_RX` |
| Radio commands | `<DEVNAME>_CMD_<NAME>` | `ADF7023_CMD_PHY_TX` |
| Enum values | `<DEVNAME>_<CATEGORY>_<VALUE>` | `ADF7023_MOD_FSK` |

---

## 2. File Checklist

```
drivers/rf-transceiver/<devname>/
    <devname>.h              # Driver header
    <devname>.c              # Driver implementation
    <devname>_config.h       # Default radio configuration (optional)
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

doc/sphinx/source/drivers/rf-transceiver/<devname>.rst   # Sphinx entry
```

> **Note:** IIO wrappers are not typical for RF transceiver drivers.
> Packet-based radios expose TX/RX operations, state machine
> transitions, and RF parameter configuration rather than continuous
> data channels.

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
#include "no_os_gpio.h"
#include "no_os_util.h"

/* ---------- SPI Memory Access Commands -------------------------------- */

#define <DEVNAME>_SPI_MEM_WR	0x18
#define <DEVNAME>_SPI_MEM_RD	0x38
#define <DEVNAME>_SPI_NOP	0xFF

/* ---------- Radio Controller Commands --------------------------------- */

#define <DEVNAME>_CMD_PHY_OFF		0xB0
#define <DEVNAME>_CMD_PHY_ON		0xB1
#define <DEVNAME>_CMD_PHY_RX		0xB2
#define <DEVNAME>_CMD_PHY_TX		0xB5
#define <DEVNAME>_CMD_PHY_SLEEP		0xBA
#define <DEVNAME>_CMD_CONFIG_DEV	0xBB
#define <DEVNAME>_CMD_GET_RSSI		0xBC
#define <DEVNAME>_CMD_HW_RESET		0xC8

/* ---------- Firmware States ------------------------------------------- */

#define <DEVNAME>_FW_STATE_PHY_OFF	0x11
#define <DEVNAME>_FW_STATE_PHY_ON	0x12
#define <DEVNAME>_FW_STATE_PHY_RX	0x13
#define <DEVNAME>_FW_STATE_PHY_TX	0x14
#define <DEVNAME>_FW_STATE_PHY_SLEEP	0x06

/* ---------- Status Word ----------------------------------------------- */

#define <DEVNAME>_STATUS_SPI_READY_MSK	NO_OS_BIT(7)
#define <DEVNAME>_STATUS_IRQ_MSK	NO_OS_BIT(6)
#define <DEVNAME>_STATUS_CMD_READY_MSK	NO_OS_BIT(5)
#define <DEVNAME>_STATUS_FW_STATE_MSK	NO_OS_GENMASK(4, 0)

/* ---------- Register / Memory Map ------------------------------------- */

/** BBRAM (Battery Backup RAM) configuration registers. */
#define <DEVNAME>_REG_INTERRUPT_MASK_0	0x100
#define <DEVNAME>_REG_CHANNEL_FREQ_0	0x109
#define <DEVNAME>_REG_CHANNEL_FREQ_1	0x10A
#define <DEVNAME>_REG_CHANNEL_FREQ_2	0x10B
#define <DEVNAME>_REG_RADIO_CFG_0	0x10C
#define <DEVNAME>_REG_RADIO_CFG_8	0x114
#define <DEVNAME>_REG_RADIO_CFG_9	0x115
#define <DEVNAME>_REG_MODE_CONTROL	0x11A
#define <DEVNAME>_REG_PACKET_LEN_CTRL	0x126
#define <DEVNAME>_REG_PACKET_LEN_MAX	0x127
#define <DEVNAME>_REG_TX_BASE_ADR	0x124
#define <DEVNAME>_REG_RX_BASE_ADR	0x125

/* Register field masks — use NO_OS_BIT() for single bits,
 * NO_OS_GENMASK(hi, lo) for multi-bit fields. */
#define <DEVNAME>_CFG8_PA_LEVEL_MSK	NO_OS_GENMASK(6, 3)
#define <DEVNAME>_CFG8_PA_RAMP_MSK	NO_OS_GENMASK(2, 0)
#define <DEVNAME>_CFG9_MOD_SCHEME_MSK	NO_OS_GENMASK(5, 3)
#define <DEVNAME>_CFG9_IFBW_MSK		NO_OS_GENMASK(7, 6)

/** MCR (Modem Configuration RAM) registers. */
#define <DEVNAME>_MCR_RSSI_READBACK	0x312

/* ---------- Default TX/RX base addresses ------------------------------ */

#define <DEVNAME>_TX_BASE_ADR_DEFAULT	0x10
#define <DEVNAME>_RX_BASE_ADR_DEFAULT	0x10

/* ---------- Enums ----------------------------------------------------- */

/** @enum <devname>_device_id
 *  @brief Supported device IDs. */
enum <devname>_device_id {
	ID_<DEVNAME>,
	/* ID_<DEVNAME_VARIANT>, */
};

/** @enum <devname>_fw_state
 *  @brief Radio firmware / PHY states. */
enum <devname>_fw_state {
	<DEVNAME>_STATE_PHY_OFF,
	<DEVNAME>_STATE_PHY_ON,
	<DEVNAME>_STATE_PHY_RX,
	<DEVNAME>_STATE_PHY_TX,
	<DEVNAME>_STATE_PHY_SLEEP,
};

/** @enum <devname>_mod_scheme
 *  @brief Modulation scheme selection. */
enum <devname>_mod_scheme {
	<DEVNAME>_MOD_2FSK,
	<DEVNAME>_MOD_2GFSK,
	<DEVNAME>_MOD_OOK,
	/* Add more as needed */
};

/* ---------- Structures ------------------------------------------------ */

/**
 * @struct <devname>_rf_config
 * @brief  RF configuration — channel frequency, data rate, deviation, etc.
 */
struct <devname>_rf_config {
	/** Channel frequency in Hz (e.g., 433000000). */
	uint32_t channel_freq_hz;
	/** Data rate in bps (e.g., 100000). */
	uint32_t data_rate_bps;
	/** Frequency deviation in Hz (FSK modes). */
	uint32_t freq_deviation_hz;
	/** PA output power level (device-specific coding). */
	uint8_t pa_level;
	/** Modulation scheme. */
	enum <devname>_mod_scheme mod;
};

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** Device variant. */
	enum <devname>_device_id id;
	/** RF configuration. */
	struct <devname>_rf_config rf_config;
	/** SPI initialization parameters. */
	struct no_os_spi_init_param spi_init;
	/** GPIO chip-select (for manual CS control, if needed). */
	struct no_os_gpio_init_param gpio_cs;
	/** GPIO MISO (for SPI-ready polling, if needed). */
	struct no_os_gpio_init_param gpio_miso;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** Device name string. */
	const char *name;
	/** Device variant. */
	enum <devname>_device_id id;
	/** Active RF configuration. */
	struct <devname>_rf_config rf_config;
	/** Current firmware / PHY state. */
	enum <devname>_fw_state fw_state;
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** GPIO chip-select descriptor (optional). */
	struct no_os_gpio_desc *gpio_cs;
	/** GPIO MISO descriptor (optional). */
	struct no_os_gpio_desc *gpio_miso;
};

/* ---------- Function Prototypes --------------------------------------- */

/** @brief Initialise the device. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Read the device status word. */
int <devname>_get_status(struct <devname>_dev *dev, uint8_t *status);

/** @brief Issue a radio controller command. */
int <devname>_set_command(struct <devname>_dev *dev, uint8_t command);

/** @brief Transition to a firmware / PHY state and wait. */
int <devname>_set_fw_state(struct <devname>_dev *dev,
			   enum <devname>_fw_state state);

/** @brief Read data from packet RAM. */
int <devname>_get_ram(struct <devname>_dev *dev, uint32_t address,
		      uint32_t length, uint8_t *data);

/** @brief Write data to packet RAM. */
int <devname>_set_ram(struct <devname>_dev *dev, uint32_t address,
		      uint32_t length, uint8_t *data);

/** @brief Receive one packet. */
int <devname>_receive_packet(struct <devname>_dev *dev,
			     uint8_t *packet, uint8_t *length);

/** @brief Transmit one packet. */
int <devname>_transmit_packet(struct <devname>_dev *dev,
			      uint8_t *packet, uint8_t length);

/** @brief Set the channel frequency. */
int <devname>_set_channel_freq(struct <devname>_dev *dev,
			       uint32_t freq_hz);

/** @brief Set the data rate. */
int <devname>_set_data_rate(struct <devname>_dev *dev,
			    uint32_t data_rate_bps);

/** @brief Set the frequency deviation. */
int <devname>_set_freq_deviation(struct <devname>_dev *dev,
				 uint32_t freq_dev_hz);

/** @brief Set the PA output power level. */
int <devname>_set_pa_level(struct <devname>_dev *dev, uint8_t level);

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

/* Device name lookup table (indexed by enum <devname>_device_id). */
static const char * const <devname>_device_name[] = {
	[ID_<DEVNAME>] = "<DEVNAME>",
};

/* ---------- Static helpers ----------------------------------------- */

/**
 * @brief Transfer one byte over SPI (write and optionally read back).
 * @param dev        - Device descriptor.
 * @param write_byte - Byte to write.
 * @param read_byte  - Pointer to store read byte (NULL to discard).
 * @return 0 on success, negative error code otherwise.
 */
static int _<devname>_write_read_byte(struct <devname>_dev *dev,
				      uint8_t write_byte,
				      uint8_t *read_byte)
{
	uint8_t data;
	int ret;

	data = write_byte;
	ret = no_os_spi_write_and_read(dev->spi_desc, &data, 1);
	if (ret)
		return ret;

	if (read_byte)
		*read_byte = data;

	return 0;
}

/**
 * @brief Wait for SPI_READY in the status word with a timeout.
 * @param dev     - Device descriptor.
 * @param timeout - Maximum number of polling iterations.
 * @return 0 on success, -ETIMEDOUT if the device did not become ready.
 */
static int _<devname>_wait_ready(struct <devname>_dev *dev,
				 uint16_t timeout)
{
	uint8_t status;
	int ret;

	while (timeout--) {
		ret = <devname>_get_status(dev, &status);
		if (ret)
			return ret;

		if (status & <DEVNAME>_STATUS_CMD_READY_MSK)
			return 0;
	}

	return -ETIMEDOUT;
}

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Read the device status word.
 * @param dev    - Device descriptor.
 * @param status - Pointer to store the status byte.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_status(struct <devname>_dev *dev, uint8_t *status)
{
	int ret;

	ret = _<devname>_write_read_byte(dev, <DEVNAME>_SPI_NOP, NULL);
	if (ret)
		return ret;

	return _<devname>_write_read_byte(dev, <DEVNAME>_SPI_NOP, status);
}

/**
 * @brief Issue a radio controller command.
 * @param dev     - Device descriptor.
 * @param command - Command byte (e.g., <DEVNAME>_CMD_PHY_TX).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_command(struct <devname>_dev *dev, uint8_t command)
{
	return _<devname>_write_read_byte(dev, command, NULL);
}

/**
 * @brief Transition to a firmware / PHY state and wait.
 * @param dev   - Device descriptor.
 * @param state - Target firmware state.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_fw_state(struct <devname>_dev *dev,
			   enum <devname>_fw_state state)
{
	uint8_t cmd;
	uint8_t fw_target;
	uint8_t status;
	int ret;

	switch (state) {
	case <DEVNAME>_STATE_PHY_OFF:
		cmd = <DEVNAME>_CMD_PHY_OFF;
		fw_target = <DEVNAME>_FW_STATE_PHY_OFF;
		break;
	case <DEVNAME>_STATE_PHY_ON:
		cmd = <DEVNAME>_CMD_PHY_ON;
		fw_target = <DEVNAME>_FW_STATE_PHY_ON;
		break;
	case <DEVNAME>_STATE_PHY_RX:
		cmd = <DEVNAME>_CMD_PHY_RX;
		fw_target = <DEVNAME>_FW_STATE_PHY_RX;
		break;
	case <DEVNAME>_STATE_PHY_TX:
		cmd = <DEVNAME>_CMD_PHY_TX;
		fw_target = <DEVNAME>_FW_STATE_PHY_TX;
		break;
	default:
		cmd = <DEVNAME>_CMD_PHY_SLEEP;
		fw_target = <DEVNAME>_FW_STATE_PHY_SLEEP;
		break;
	}

	ret = <devname>_set_command(dev, cmd);
	if (ret)
		return ret;

	/* Poll until the device reaches the target state. */
	do {
		ret = <devname>_get_status(dev, &status);
		if (ret)
			return ret;
	} while (no_os_field_get(<DEVNAME>_STATUS_FW_STATE_MSK,
				 status) != fw_target);

	dev->fw_state = state;

	return 0;
}

/**
 * @brief Read data from packet RAM.
 * @param dev     - Device descriptor.
 * @param address - Start address in packet RAM.
 * @param length  - Number of bytes to read.
 * @param data    - Buffer to store read data.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_ram(struct <devname>_dev *dev, uint32_t address,
		      uint32_t length, uint8_t *data)
{
	uint8_t buf[3];
	int ret;

	buf[0] = <DEVNAME>_SPI_MEM_RD | ((address & 0x700) >> 8);
	buf[1] = address & 0xFF;
	buf[2] = <DEVNAME>_SPI_NOP;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	return no_os_spi_write_and_read(dev->spi_desc, data, length);
}

/**
 * @brief Write data to packet RAM.
 * @param dev     - Device descriptor.
 * @param address - Start address in packet RAM.
 * @param length  - Number of bytes to write.
 * @param data    - Buffer containing data to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_ram(struct <devname>_dev *dev, uint32_t address,
		      uint32_t length, uint8_t *data)
{
	uint8_t buf[2];
	int ret;

	buf[0] = <DEVNAME>_SPI_MEM_WR | ((address & 0x700) >> 8);
	buf[1] = address & 0xFF;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	return no_os_spi_write_and_read(dev->spi_desc, data, length);
}

/**
 * @brief Receive one packet.
 * @param dev    - Device descriptor.
 * @param packet - Buffer to store received payload.
 * @param length - Pointer to store the number of received bytes.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_receive_packet(struct <devname>_dev *dev,
			     uint8_t *packet, uint8_t *length)
{
	uint8_t interrupt_reg = 0;
	int ret;

	ret = <devname>_set_fw_state(dev, <DEVNAME>_STATE_PHY_ON);
	if (ret)
		return ret;

	ret = <devname>_set_fw_state(dev, <DEVNAME>_STATE_PHY_RX);
	if (ret)
		return ret;

	/* Poll for CRC-correct interrupt. */
	while (!(interrupt_reg & NO_OS_BIT(2))) {
		ret = <devname>_get_ram(dev, <DEVNAME>_MCR_RSSI_READBACK - 0x24,
					0x1, &interrupt_reg);
		if (ret)
			return ret;
	}

	ret = <devname>_get_ram(dev, <DEVNAME>_RX_BASE_ADR_DEFAULT,
				1, length);
	if (ret)
		return ret;

	return <devname>_get_ram(dev, <DEVNAME>_RX_BASE_ADR_DEFAULT + 2,
				 *length - 2, packet);
}

/**
 * @brief Transmit one packet.
 * @param dev    - Device descriptor.
 * @param packet - Buffer containing payload to transmit.
 * @param length - Number of payload bytes.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_transmit_packet(struct <devname>_dev *dev,
			      uint8_t *packet, uint8_t length)
{
	uint8_t header[2];
	int ret;

	header[0] = 2 + length;
	header[1] = 0x00; /* address match offset — adjust as needed */

	ret = <devname>_set_ram(dev, <DEVNAME>_TX_BASE_ADR_DEFAULT,
				2, header);
	if (ret)
		return ret;

	ret = <devname>_set_ram(dev, <DEVNAME>_TX_BASE_ADR_DEFAULT + 2,
				length, packet);
	if (ret)
		return ret;

	ret = <devname>_set_fw_state(dev, <DEVNAME>_STATE_PHY_ON);
	if (ret)
		return ret;

	return <devname>_set_fw_state(dev, <DEVNAME>_STATE_PHY_TX);
}

/**
 * @brief Set the channel frequency.
 * @param dev     - Device descriptor.
 * @param freq_hz - Channel frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_channel_freq(struct <devname>_dev *dev,
			       uint32_t freq_hz)
{
	uint32_t freq_code;
	uint8_t buf[3];
	int ret;

	/*
	 * Convert Hz to the device's frequency register coding.
	 * Example: freq_code = freq_hz * 65535 / f_xtal
	 */
	freq_code = (uint32_t)(((uint64_t)freq_hz * 65535) / 26000000);

	buf[0] = (uint8_t)(freq_code & 0xFF);
	buf[1] = (uint8_t)((freq_code >> 8) & 0xFF);
	buf[2] = (uint8_t)((freq_code >> 16) & 0xFF);

	ret = <devname>_set_ram(dev, <DEVNAME>_REG_CHANNEL_FREQ_0,
				3, buf);
	if (ret)
		return ret;

	dev->rf_config.channel_freq_hz = freq_hz;

	return 0;
}

/**
 * @brief Set the data rate.
 * @param dev           - Device descriptor.
 * @param data_rate_bps - Data rate in bits per second.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_data_rate(struct <devname>_dev *dev,
			    uint32_t data_rate_bps)
{
	/* Write data rate to the appropriate config registers,
	 * then reconfigure the device. */
	int ret;

	/* Placeholder: encode data_rate_bps into register values
	 * and write via <devname>_set_ram(). */

	ret = <devname>_set_fw_state(dev, <DEVNAME>_STATE_PHY_OFF);
	if (ret)
		return ret;

	ret = <devname>_set_command(dev, <DEVNAME>_CMD_CONFIG_DEV);
	if (ret)
		return ret;

	dev->rf_config.data_rate_bps = data_rate_bps;

	return 0;
}

/**
 * @brief Set the frequency deviation (FSK modes).
 * @param dev        - Device descriptor.
 * @param freq_dev_hz - Frequency deviation in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_freq_deviation(struct <devname>_dev *dev,
				 uint32_t freq_dev_hz)
{
	int ret;

	/* Placeholder: encode freq_dev_hz into register values
	 * and write via <devname>_set_ram(). */

	ret = <devname>_set_fw_state(dev, <DEVNAME>_STATE_PHY_OFF);
	if (ret)
		return ret;

	ret = <devname>_set_command(dev, <DEVNAME>_CMD_CONFIG_DEV);
	if (ret)
		return ret;

	dev->rf_config.freq_deviation_hz = freq_dev_hz;

	return 0;
}

/**
 * @brief Set the PA output power level.
 * @param dev   - Device descriptor.
 * @param level - PA power level (device-specific coding).
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_set_pa_level(struct <devname>_dev *dev, uint8_t level)
{
	uint8_t reg_val;
	int ret;

	ret = <devname>_get_ram(dev, <DEVNAME>_REG_RADIO_CFG_8,
				1, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~<DEVNAME>_CFG8_PA_LEVEL_MSK;
	reg_val |= no_os_field_prep(<DEVNAME>_CFG8_PA_LEVEL_MSK, level);

	ret = <devname>_set_ram(dev, <DEVNAME>_REG_RADIO_CFG_8,
				1, &reg_val);
	if (ret)
		return ret;

	dev->rf_config.pa_level = level;

	return 0;
}

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
	int ret;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->id = init_param->id;
	d->name = <devname>_device_name[d->id];

	/* Initialise SPI */
	ret = no_os_spi_init(&d->spi_desc, &init_param->spi_init);
	if (ret)
		goto free_dev;

	/* Initialise GPIOs (optional — remove if not needed) */
	ret = no_os_gpio_get(&d->gpio_cs, &init_param->gpio_cs);
	if (ret)
		goto free_spi;

	if (d->gpio_cs) {
		ret = no_os_gpio_direction_output(d->gpio_cs,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto free_gpio_cs;
	}

	ret = no_os_gpio_get(&d->gpio_miso, &init_param->gpio_miso);
	if (ret)
		goto free_gpio_cs;

	/* Wait for SPI ready */
	ret = _<devname>_wait_ready(d, 1000);
	if (ret)
		goto free_gpio_miso;

	/* Apply default radio configuration from BBRAM */
	ret = <devname>_set_command(d, <DEVNAME>_CMD_CONFIG_DEV);
	if (ret)
		goto free_gpio_miso;

	/* Apply user RF configuration */
	ret = <devname>_set_channel_freq(d,
					 init_param->rf_config.channel_freq_hz);
	if (ret)
		goto free_gpio_miso;

	d->rf_config = init_param->rf_config;

	*dev = d;

	return 0;

free_gpio_miso:
	no_os_gpio_remove(d->gpio_miso);
free_gpio_cs:
	no_os_gpio_remove(d->gpio_cs);
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

	ret = no_os_gpio_remove(dev->gpio_miso);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_cs);
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

## 5. IIO Header

> IIO wrappers are not typical for RF transceiver drivers.
> Packet-based radios do not map naturally to IIO channels and
> continuous data flows. If your device does require IIO support
> (e.g., for RSSI monitoring or debug register access), follow the
> pattern in `drivers/rf-transceiver/hmc630x/iio_hmc630x.h` or the
> ADC template at `drivers/adc/DRIVER_TEMPLATE.md` section 5.

---

## 6. IIO Source

> See the note in section 5 above. If IIO is needed, follow the
> pattern in `drivers/rf-transceiver/hmc630x/iio_hmc630x.c` or
> `drivers/adc/DRIVER_TEMPLATE.md` section 6.

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

INCS += $(DRIVERS)/rf-transceiver/<devname>/<devname>.h
SRCS += $(DRIVERS)/rf-transceiver/<devname>/<devname>.c
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
	.id = ID_<DEVNAME>,
	.spi_init = <devname>_spi_ip,
	.rf_config = {
		.channel_freq_hz = 433000000,
		.data_rate_bps = 100000,
		.freq_deviation_hz = 50000,
		.pa_level = 0x0F,
		.mod = <DEVNAME>_MOD_2GFSK,
	},
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
	uint8_t tx_data[] = "Hello RF";
	uint8_t rx_data[64];
	uint8_t rx_len;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	pr_info("Device initialised. Sending packet...\r\n");

	ret = <devname>_transmit_packet(dev, tx_data, sizeof(tx_data));
	if (ret)
		goto free_dev;

	pr_info("Packet sent. Waiting for response...\r\n");

	while (1) {
		ret = <devname>_receive_packet(dev, rx_data, &rx_len);
		if (ret)
			goto free_dev;

		pr_info("Received %u bytes\r\n", rx_len);
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

The <DEVNAME> is a low-power, high-performance RF transceiver operating
in the sub-GHz ISM bands. It integrates a packet radio engine with a
configurable state machine, supporting modulation schemes such as 2-FSK,
2-GFSK, and OOK. <Add 2-3 more sentences from the datasheet covering
frequency range, data rate, sensitivity, and PA output power.>

Applications
------------

* Wireless sensor networks
* Industrial telemetry and remote control
* Smart metering (AMI)
* Home and building automation

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the SPI interface, optionally configures GPIO
pins for chip-select and SPI-ready polling, waits for the device to
become ready, applies the default BBRAM configuration, and sets the
channel frequency. The function uses goto-based cleanup to release
resources on failure.

State Machine
~~~~~~~~~~~~~

The radio operates through a set of firmware states: ``PHY_SLEEP``,
``PHY_OFF``, ``PHY_ON``, ``PHY_RX``, and ``PHY_TX``. The
``<devname>_set_fw_state`` function issues the appropriate command and
polls the status word until the device transitions to the target state.

Packet TX / RX
~~~~~~~~~~~~~~

The ``<devname>_transmit_packet`` function writes the payload into
packet RAM and transitions the radio to ``PHY_TX``. The
``<devname>_receive_packet`` function transitions the radio to
``PHY_RX`` and polls for a CRC-correct interrupt before reading the
received data from packet RAM.

RF Parameter Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_set_channel_freq``, ``<devname>_set_data_rate``,
``<devname>_set_freq_deviation``, and ``<devname>_set_pa_level``
functions allow runtime adjustment of the radio's operating parameters.
After changing data rate or frequency deviation, the device is
reconfigured by issuing the ``CONFIG_DEV`` command.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"
   #include "no_os_spi.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .id = ID_<DEVNAME>,
           .spi_init = { /* platform-specific SPI params */ },
           .rf_config = {
               .channel_freq_hz = 433000000,
               .data_rate_bps = 100000,
               .freq_deviation_hz = 50000,
               .pa_level = 0x0F,
               .mod = <DEVNAME>_MOD_2GFSK,
           },
       };

       struct <devname>_dev *dev;
       uint8_t tx_data[] = "Hello";
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_transmit_packet(dev, tx_data, sizeof(tx_data));
       if (ret)
           goto cleanup;

       /* Packet transmitted successfully */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/rf-transceiver/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/rf-transceiver/<devname>/README.rst
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
   Radio configuration (channel frequency, data rate, power level) is
   stored in `rf_config` within the device struct.
10. **Register macros** -- `NO_OS_BIT()` for single bits,
    `NO_OS_GENMASK(hi, lo)` for fields; `no_os_field_prep()` /
    `no_os_field_get()` for field access.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> rf transceiver driver support

Add support for the <DEVNAME> sub-GHz RF transceiver with SPI interface,
packet radio engine, and configurable state machine.

Signed-off-by: Your Name <your.name@analog.com>
```
