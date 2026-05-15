# GNSS/GPS Driver Template

Reference driver: `drivers/gnss-gps/nmea_ubx/`

This template covers every file needed to add a new GNSS/GPS receiver driver
to no-OS. Replace `<devname>` with the part number (e.g., `nmea_ubx`) and
`<DEVNAME>` with its uppercase form (e.g., `NMEA_UBX`) throughout.

---

## 1. Naming Conventions

| Item | Pattern | Example |
|---|---|---|
| Files | `<devname>.h`, `<devname>.c` | `nmea_ubx.h`, `nmea_ubx.c` |
| Include guard | `#ifndef __<DEVNAME>_H__` | `#ifndef __NMEA_UBX_H__` |
| Init param struct | `<devname>_init_param` | `gnss_init_param` |
| Device struct | `<devname>_dev` | `gnss_dev` |
| Functions | `<devname>_<verb>()` | `gnss_init()` |
| Static helpers | `_<devname>_<verb>()` | `_gnss_parse_field()` |
| Device type enum | `<DEVNAME>_DEVICE_<TYPE>` | `GNSS_DEVICE_UBX_CAPABLE` |
| Protocol constants | `UBX_<GROUP>_<NAME>` | `UBX_CLASS_NAV` |
| Config keys | `UBLOX_CFG_<GROUP>_<NAME>` | `UBLOX_CFG_UART1_BAUDRATE` |

---

## 2. File Checklist

```
drivers/gnss-gps/<devname>/
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

doc/sphinx/source/drivers/gnss-gps/<devname>.rst   # Sphinx entry
```

Note: IIO support is not typical for GNSS/GPS drivers. These devices
output serial NMEA/UBX data streams rather than sampled ADC channels,
so the IIO framework is generally not applicable.

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
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_util.h"

/* ---------------- Protocol Constants ---------------------------------- */

/** Maximum NMEA sentence length (including CR+LF). */
#define <DEVNAME>_NMEA_MAX_LEN		82

/** Maximum binary packet size (header + payload + checksum). */
#define <DEVNAME>_MAX_PACKET_SIZE	1024

/** Binary protocol header size (sync + class + id + length). */
#define <DEVNAME>_HEADER_SIZE		6

/** Binary protocol checksum size. */
#define <DEVNAME>_CHECKSUM_SIZE		2

/* Binary protocol sync bytes */
#define <DEVNAME>_SYNC_1		0xB5
#define <DEVNAME>_SYNC_2		0x62

/* Binary protocol class IDs */
#define <DEVNAME>_CLASS_NAV		0x01
#define <DEVNAME>_CLASS_CFG		0x06
#define <DEVNAME>_CLASS_ACK		0x05

/* Binary protocol message IDs */
#define <DEVNAME>_NAV_PVT		0x07
#define <DEVNAME>_NAV_TIMEUTC		0x21

/* ACK/NACK message IDs */
#define <DEVNAME>_ACK_ACK		0x01
#define <DEVNAME>_ACK_NACK		0x00

/* Configuration key IDs (vendor-specific) */
#define <DEVNAME>_CFG_UART1_BAUDRATE	0x40520001
#define <DEVNAME>_CFG_TP_FREQ_TP1	0x40050024
#define <DEVNAME>_CFG_TP_TP1_ENA	0x10050007

/* Time conversion helpers */
#define <DEVNAME>_S_TO_US		1000000
#define <DEVNAME>_MS_TO_US		1000

/* Enable / Disable */
#define <DEVNAME>_ENABLE		1u
#define <DEVNAME>_DISABLE		0u

/* ---------------- Enums -------------------------------------------- */

/** @enum <devname>_device_type
 *  @brief Device type for protocol selection. */
enum <devname>_device_type {
	/** Devices supporting vendor binary protocol (nanosecond precision). */
	<DEVNAME>_DEVICE_BINARY_CAPABLE,
	/** NMEA-only devices (millisecond precision). */
	<DEVNAME>_DEVICE_NMEA_ONLY,
};

/** @enum <devname>_nmea_constellation
 *  @brief NMEA constellation prefix type. */
enum <devname>_nmea_constellation {
	/** GPS-only ($GPxxx sentences). */
	<DEVNAME>_NMEA_GPS_ONLY,
	/** Multi-constellation ($GNxxx sentences). */
	<DEVNAME>_NMEA_GNSS_MULTI,
};

/** @enum <devname>_protocol
 *  @brief Communication protocol types. */
enum <devname>_protocol {
	/** Vendor proprietary binary protocol. */
	<DEVNAME>_PROTOCOL_BINARY,
	/** NMEA 0183 ASCII protocol. */
	<DEVNAME>_PROTOCOL_NMEA,
	/** RTCM3 corrections protocol. */
	<DEVNAME>_PROTOCOL_RTCM3,
};

/** @enum <devname>_config_layer
 *  @brief Configuration storage layers. */
enum <devname>_config_layer {
	/** RAM — volatile, lost on reset. */
	<DEVNAME>_CFG_LAYER_RAM,
	/** Battery-backed RAM — persistent during backup. */
	<DEVNAME>_CFG_LAYER_BBR,
	/** Flash — permanent storage. */
	<DEVNAME>_CFG_LAYER_FLASH,
	/** All layers simultaneously. */
	<DEVNAME>_CFG_LAYER_ALL,
};

/* ---------------- Data Structures ---------------------------------- */

/**
 * @struct <devname>_nav_pvt
 * @brief  Position, Velocity, Time solution from binary NAV-PVT message.
 */
struct <devname>_nav_pvt {
	/** GPS time of week (ms). */
	uint32_t iTOW;
	/** Year (UTC). */
	uint16_t year;
	/** Month (UTC). */
	uint8_t month;
	/** Day (UTC). */
	uint8_t day;
	/** Hour (UTC). */
	uint8_t hour;
	/** Minute (UTC). */
	uint8_t min;
	/** Second (UTC). */
	uint8_t sec;
	/** Validity flags. */
	uint8_t valid;
	/** Time accuracy estimate (ns). */
	uint32_t tAcc;
	/** Fraction of second (ns, signed). */
	int32_t nano;
	/** GNSS fix type. */
	uint8_t fixType;
	/** Fix status flags. */
	uint8_t flags;
	/** Number of satellites used. */
	uint8_t numSV;
	/** Longitude (1e-7 degrees). */
	int32_t lon;
	/** Latitude (1e-7 degrees). */
	int32_t lat;
	/** Height above MSL (mm). */
	int32_t hMSL;
	/** Ground speed (mm/s). */
	int32_t gSpeed;
	/** Heading of motion (1e-5 degrees). */
	int32_t headMot;
};

/**
 * @struct <devname>_nmea_time
 * @brief  Time data parsed from NMEA RMC sentence.
 */
struct <devname>_nmea_time {
	/** Hour (0-23). */
	uint8_t hour;
	/** Minute (0-59). */
	uint8_t minute;
	/** Second (0-59). */
	uint8_t second;
	/** Milliseconds (0-999). */
	uint16_t milliseconds;
	/** Day (1-31). */
	uint8_t day;
	/** Month (1-12). */
	uint8_t month;
	/** Year (e.g., 2025). */
	uint16_t year;
	/** Time data validity. */
	bool time_valid;
	/** Date data validity. */
	bool date_valid;
	/** Calculated Unix epoch (seconds). */
	uint32_t unix_epoch;
};

/**
 * @struct <devname>_nmea_position
 * @brief  Position and fix quality from NMEA GGA sentence.
 */
struct <devname>_nmea_position {
	/** Fix quality: 0=invalid, 1=GPS, 2=DGPS. */
	uint8_t fix_quality;
	/** Number of satellites in use. */
	uint8_t satellites_used;
	/** Horizontal dilution of precision. */
	float hdop;
	/** Altitude above MSL (metres). */
	float altitude;
	/** Latitude (decimal degrees). */
	float latitude;
	/** Longitude (decimal degrees). */
	float longitude;
	/** Position data validity. */
	bool position_valid;
	/** Fix validity. */
	bool fix_valid;
};

/**
 * @struct <devname>_precise_time
 * @brief  Unified precise time with nanosecond resolution.
 */
struct <devname>_precise_time {
	/** Unix epoch time (seconds). */
	uint32_t unix_epoch;
	/** Nanosecond fraction (can be negative). */
	int32_t nanoseconds;
	/** Time accuracy estimate (ns). */
	uint32_t time_accuracy;
	/** Time validity flag. */
	bool time_valid;
	/** Time fully resolved flag. */
	bool time_fully_resolved;
	/** Date validity flag. */
	bool date_valid;
	/** Year (UTC). */
	uint16_t year;
	/** Month (UTC). */
	uint8_t month;
	/** Day (UTC). */
	uint8_t day;
	/** Hour (UTC). */
	uint8_t hour;
	/** Minute (UTC). */
	uint8_t minute;
	/** Second (UTC). */
	uint8_t second;
};

/**
 * @struct <devname>_binary_packet
 * @brief  Binary protocol packet structure.
 */
struct <devname>_binary_packet {
	/** Sync character 1. */
	uint8_t sync1;
	/** Sync character 2. */
	uint8_t sync2;
	/** Message class. */
	uint8_t cls;
	/** Message ID. */
	uint8_t id;
	/** Payload length. */
	uint16_t length;
	/** Payload data. */
	uint8_t *payload;
	/** Checksum byte A. */
	uint8_t checksumA;
	/** Checksum byte B. */
	uint8_t checksumB;
};

/* ---------------- Init / Device Structures ------------------------- */

/**
 * @struct <devname>_init_param
 * @brief  Initialization parameters passed to <devname>_init().
 */
struct <devname>_init_param {
	/** UART initialization parameters. */
	struct no_os_uart_init_param *uart_init;
	/** GPIO descriptor for hardware reset (optional, NULL if unused). */
	struct no_os_gpio_desc *gpio_reset;
	/** IRQ controller descriptor (optional). */
	struct no_os_irq_ctrl_desc *irq_ctrl;
	/** Device type — determines protocol and configuration method. */
	enum <devname>_device_type device_type;
	/** NMEA constellation type (only for NMEA-only devices). */
	enum <devname>_nmea_constellation nmea_constellation_type;
	/** Enable binary protocol on input. */
	uint8_t binary_input_enable;
	/** Enable NMEA protocol on input. */
	uint8_t nmea_input_enable;
	/** Enable binary protocol on output. */
	uint8_t binary_output_enable;
	/** Enable NMEA protocol on output. */
	uint8_t nmea_output_enable;
	/** UART baud rate. */
	uint32_t baud_rate;
	/** PPS output frequency (Hz). */
	uint32_t pps_frequency;
	/** PPS pulse length (ms). */
	uint32_t pps_pulse_length;
	/** PPS enable flag. */
	uint8_t pps_enable;
};

/**
 * @struct <devname>_dev
 * @brief  Device descriptor — allocated by <devname>_init().
 */
struct <devname>_dev {
	/** UART descriptor. */
	struct no_os_uart_desc *uart_desc;
	/** GPIO reset descriptor (optional). */
	struct no_os_gpio_desc *gpio_reset;
	/** IRQ controller descriptor (optional). */
	struct no_os_irq_ctrl_desc *irq_ctrl;
	/** Device type. */
	enum <devname>_device_type device_type;
	/** NMEA constellation type. */
	enum <devname>_nmea_constellation nmea_constellation_type;
	/** Receive buffer. */
	uint8_t rx_buffer[<DEVNAME>_MAX_PACKET_SIZE];
	/** Transmit buffer. */
	uint8_t tx_buffer[<DEVNAME>_MAX_PACKET_SIZE];
	/** Cached NAV-PVT data (binary-capable devices). */
	struct <devname>_nav_pvt nav_data;
	/** Cached NMEA timing data. */
	struct <devname>_nmea_time nmea_timing_cache;
	/** Cached NMEA position data. */
	struct <devname>_nmea_position nmea_position_cache;
	/** PPS output frequency (Hz). */
	uint32_t pps_frequency;
	/** PPS pulse length (ms). */
	uint32_t pps_pulse_length;
	/** PPS enable flag. */
	uint8_t pps_enable;
};

/* ---------------- Function Prototypes ------------------------------ */

/** @brief Initialise the GNSS device and configure communication. */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param);

/** @brief Free resources allocated by <devname>_init(). */
int <devname>_remove(struct <devname>_dev *dev);

/** @brief Read raw data from the GNSS device via UART. */
int <devname>_read(struct <devname>_dev *dev, uint8_t *data,
		   uint16_t size);

/** @brief Write raw data to the GNSS device via UART. */
int <devname>_write(struct <devname>_dev *dev, uint8_t *data,
		    uint16_t size);

/** @brief Perform hardware reset using GPIO. */
int <devname>_hw_reset(struct <devname>_dev *dev);

/** @brief Perform software reset via binary protocol command. */
int <devname>_sw_reset(struct <devname>_dev *dev);

/** @brief Send a binary protocol packet. */
int <devname>_send_packet(struct <devname>_dev *dev, uint8_t cls,
			  uint8_t id, uint8_t *payload, uint16_t length);

/** @brief Receive a binary protocol packet. */
int <devname>_receive_packet(struct <devname>_dev *dev,
			     struct <devname>_binary_packet *packet);

/** @brief Poll navigation PVT data from a binary-capable device. */
int <devname>_poll_nav_pvt(struct <devname>_dev *dev,
			   struct <devname>_nav_pvt *nav_data);

/** @brief Refresh timing data (unified API for binary and NMEA). */
int <devname>_refresh_timing(struct <devname>_dev *dev);

/** @brief Get unified precise time (binary or NMEA). */
int <devname>_get_precise_time(struct <devname>_dev *dev,
			       struct <devname>_precise_time *time);

/** @brief Get NMEA position data. */
int <devname>_get_position(struct <devname>_dev *dev,
			   struct <devname>_nmea_position *position);

/** @brief Configure PPS time-pulse output. */
int <devname>_configure_pps(struct <devname>_dev *dev, bool enable);

/** @brief Calculate checksum over binary packet data. */
void <devname>_calculate_checksum(uint8_t *data, uint16_t length,
				  uint8_t *ck_a, uint8_t *ck_b);

/** @brief Parse an NMEA RMC sentence into time data. */
int <devname>_parse_rmc(const char *sentence,
			struct <devname>_nmea_time *time_data);

/** @brief Parse an NMEA GGA sentence into position data. */
int <devname>_parse_gga(const char *sentence,
			struct <devname>_nmea_position *position_data);

/** @brief Convert coordinates from 1e-7 degrees to float degrees. */
float <devname>_coord_to_degrees(int32_t coord);

/** @brief Calculate Unix epoch from date/time components. */
uint32_t <devname>_calculate_unix_epoch(uint16_t year, uint8_t month,
					uint8_t day, uint8_t hour,
					uint8_t minute, uint8_t second);

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
#include <stdio.h>
#include <errno.h>
#include "<devname>.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* ---------- Static helpers ----------------------------------------- */

/**
 * @brief Calculate binary protocol checksum (Fletcher-8).
 * @param data   - Data buffer (class, id, length, payload).
 * @param length - Number of bytes to checksum.
 * @param ck_a   - Output checksum byte A.
 * @param ck_b   - Output checksum byte B.
 */
void <devname>_calculate_checksum(uint8_t *data, uint16_t length,
				  uint8_t *ck_a, uint8_t *ck_b)
{
	uint8_t a = 0;
	uint8_t b = 0;
	uint16_t i;

	for (i = 0; i < length; i++) {
		a += data[i];
		b += a;
	}

	*ck_a = a;
	*ck_b = b;
}

/**
 * @brief Parse a decimal integer from an NMEA field.
 * @param str    - Pointer into the NMEA sentence.
 * @param digits - Number of digits to consume.
 * @return Parsed integer value.
 */
static int _<devname>_parse_int(const char *str, uint8_t digits)
{
	int val = 0;
	uint8_t i;

	for (i = 0; i < digits; i++)
		val = val * 10 + (str[i] - '0');

	return val;
}

/* ---------- Raw I/O ------------------------------------------------ */

/**
 * @brief Write raw data to the GNSS device via UART.
 * @param dev  - Device descriptor.
 * @param data - Data buffer to write.
 * @param size - Number of bytes to write.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_write(struct <devname>_dev *dev, uint8_t *data,
		    uint16_t size)
{
	if (!dev || !data || size == 0)
		return -EINVAL;

	return no_os_uart_write(dev->uart_desc, data, (uint32_t)size);
}

/**
 * @brief Read raw data from the GNSS device via UART.
 * @param dev  - Device descriptor.
 * @param data - Buffer to store read data.
 * @param size - Number of bytes to read.
 * @return Number of bytes read, or negative error code.
 */
int <devname>_read(struct <devname>_dev *dev, uint8_t *data,
		   uint16_t size)
{
	if (!dev || !data || size == 0)
		return -EINVAL;

	return no_os_uart_read(dev->uart_desc, data, (uint32_t)size);
}

/* ---------- Binary protocol ---------------------------------------- */

/**
 * @brief Send a binary protocol packet to the GNSS device.
 * @param dev     - Device descriptor.
 * @param cls     - Message class.
 * @param id      - Message ID.
 * @param payload - Payload data buffer (may be NULL if length is 0).
 * @param length  - Payload length in bytes.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_send_packet(struct <devname>_dev *dev, uint8_t cls,
			  uint8_t id, uint8_t *payload, uint16_t length)
{
	uint8_t *packet;
	uint8_t ck_a, ck_b;
	uint16_t packet_size;
	int ret;

	if (!dev)
		return -EINVAL;

	if (length > (<DEVNAME>_MAX_PACKET_SIZE -
		      <DEVNAME>_HEADER_SIZE - <DEVNAME>_CHECKSUM_SIZE))
		return -ERANGE;

	packet_size = <DEVNAME>_HEADER_SIZE + length + <DEVNAME>_CHECKSUM_SIZE;
	packet = (uint8_t *)no_os_calloc(packet_size, sizeof(uint8_t));
	if (!packet)
		return -ENOMEM;

	/* Build header */
	packet[0] = <DEVNAME>_SYNC_1;
	packet[1] = <DEVNAME>_SYNC_2;
	packet[2] = cls;
	packet[3] = id;
	packet[4] = length & 0xFF;
	packet[5] = (length >> 8) & 0xFF;

	/* Copy payload */
	if (payload && length > 0)
		memcpy(&packet[6], payload, length);

	/* Checksum over class + id + length + payload */
	<devname>_calculate_checksum(&packet[2], 4 + length, &ck_a, &ck_b);
	packet[6 + length] = ck_a;
	packet[7 + length] = ck_b;

	ret = <devname>_write(dev, packet, packet_size);

	no_os_free(packet);

	return (ret < 0) ? ret : 0;
}

/* ---------- Hardware / Software Reset ------------------------------ */

/**
 * @brief Perform hardware reset using the GPIO reset pin.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_hw_reset(struct <devname>_dev *dev)
{
	int ret;

	if (!dev || !dev->gpio_reset)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(100);

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	return 0;
}

/**
 * @brief Perform software reset via binary protocol command.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_sw_reset(struct <devname>_dev *dev)
{
	uint8_t payload[4] = {0xFF, 0xFF, 0x00, 0x00}; /* Cold start */
	int ret;

	if (!dev)
		return -EINVAL;

	ret = <devname>_send_packet(dev, <DEVNAME>_CLASS_CFG, 0x04,
				    payload, 4);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	return 0;
}

/* ---------- NMEA Parsing ------------------------------------------- */

/**
 * @brief Parse an NMEA RMC sentence into time data.
 * @param sentence  - Null-terminated NMEA sentence string.
 * @param time_data - Pointer to time structure to populate.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_parse_rmc(const char *sentence,
			struct <devname>_nmea_time *time_data)
{
	/* Validate sentence starts with $GxRMC */
	if (!sentence || !time_data)
		return -EINVAL;

	/* Parse time field: hhmmss.sss */
	/* Parse status: A = active, V = void */
	/* Parse date field: ddmmyy */
	/* Implementation specific to NMEA 0183 standard */

	return 0;
}

/**
 * @brief Parse an NMEA GGA sentence into position data.
 * @param sentence      - Null-terminated NMEA sentence string.
 * @param position_data - Pointer to position structure to populate.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_parse_gga(const char *sentence,
			struct <devname>_nmea_position *position_data)
{
	if (!sentence || !position_data)
		return -EINVAL;

	/* Parse latitude, longitude, fix quality, satellites, HDOP, altitude */
	/* Implementation specific to NMEA 0183 standard */

	return 0;
}

/* ---------- Coordinate / Time Helpers ------------------------------ */

/**
 * @brief Convert coordinates from 1e-7 degrees to float degrees.
 * @param coord - Coordinate in 1e-7 degrees.
 * @return Coordinate in decimal degrees.
 */
float <devname>_coord_to_degrees(int32_t coord)
{
	return (float)coord / 1e7f;
}

/**
 * @brief Calculate Unix epoch from date/time components.
 * @param year   - Year (e.g., 2025).
 * @param month  - Month (1-12).
 * @param day    - Day (1-31).
 * @param hour   - Hour (0-23).
 * @param minute - Minute (0-59).
 * @param second - Second (0-59).
 * @return Unix epoch in seconds.
 */
uint32_t <devname>_calculate_unix_epoch(uint16_t year, uint8_t month,
					uint8_t day, uint8_t hour,
					uint8_t minute, uint8_t second)
{
	/* Standard Unix epoch calculation from UTC components */
	/* Account for leap years and month lengths */
	return 0; /* Replace with actual implementation */
}

/* ---------- Public API --------------------------------------------- */

/**
 * @brief Refresh timing data from the device.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 *
 * For binary-capable devices, polls NAV-PVT. For NMEA-only devices,
 * reads and parses GPRMC/GNGGA sentences from the UART stream.
 */
int <devname>_refresh_timing(struct <devname>_dev *dev)
{
	if (!dev)
		return -EINVAL;

	if (dev->device_type == <DEVNAME>_DEVICE_BINARY_CAPABLE) {
		/* Poll NAV-PVT via binary protocol */
		return <devname>_poll_nav_pvt(dev, &dev->nav_data);
	}

	/* NMEA-only: read UART and parse sentences */
	/* Read incoming bytes, assemble sentences, parse RMC + GGA */

	return 0;
}

/**
 * @brief Get unified precise time information.
 * @param dev  - Device descriptor.
 * @param time - Pointer to precise time structure.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_get_precise_time(struct <devname>_dev *dev,
			       struct <devname>_precise_time *time)
{
	if (!dev || !time)
		return -EINVAL;

	if (dev->device_type == <DEVNAME>_DEVICE_BINARY_CAPABLE) {
		time->year = dev->nav_data.year;
		time->month = dev->nav_data.month;
		time->day = dev->nav_data.day;
		time->hour = dev->nav_data.hour;
		time->minute = dev->nav_data.min;
		time->second = dev->nav_data.sec;
		time->nanoseconds = dev->nav_data.nano;
		time->time_accuracy = dev->nav_data.tAcc;
		time->time_valid = (dev->nav_data.valid &
				    NO_OS_BIT(0)) != 0;
		time->date_valid = (dev->nav_data.valid &
				    NO_OS_BIT(1)) != 0;
		time->time_fully_resolved = (dev->nav_data.valid &
					     NO_OS_BIT(2)) != 0;
	} else {
		time->year = dev->nmea_timing_cache.year;
		time->month = dev->nmea_timing_cache.month;
		time->day = dev->nmea_timing_cache.day;
		time->hour = dev->nmea_timing_cache.hour;
		time->minute = dev->nmea_timing_cache.minute;
		time->second = dev->nmea_timing_cache.second;
		time->nanoseconds = (int32_t)dev->nmea_timing_cache.milliseconds
				    * 1000000;
		time->time_accuracy = <DEVNAME>_MS_TO_US * 1000;
		time->time_valid = dev->nmea_timing_cache.time_valid;
		time->date_valid = dev->nmea_timing_cache.date_valid;
		time->time_fully_resolved = time->time_valid &&
					    time->date_valid;
	}

	time->unix_epoch = <devname>_calculate_unix_epoch(time->year,
				time->month, time->day, time->hour,
				time->minute, time->second);

	return 0;
}

/**
 * @brief Initialise the GNSS device.
 * @param dev        - Pointer to device descriptor pointer (allocated here).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int <devname>_init(struct <devname>_dev **dev,
		   struct <devname>_init_param *init_param)
{
	struct <devname>_dev *d;
	int ret;

	if (!dev || !init_param)
		return -EINVAL;

	d = (struct <devname>_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->device_type = init_param->device_type;
	d->nmea_constellation_type = init_param->nmea_constellation_type;
	d->pps_enable = init_param->pps_enable;
	d->pps_frequency = init_param->pps_frequency;
	d->pps_pulse_length = init_param->pps_pulse_length;

	/* Initialise UART */
	init_param->uart_init->baud_rate = init_param->baud_rate;
	ret = no_os_uart_init(&d->uart_desc, init_param->uart_init);
	if (ret)
		goto free_dev;

	/* Initialise GPIO reset (optional) */
	if (init_param->gpio_reset) {
		d->gpio_reset = init_param->gpio_reset;
		ret = no_os_gpio_direction_output(d->gpio_reset,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto free_uart;
	}

	/* Perform hardware reset if GPIO is available */
	if (d->gpio_reset) {
		ret = <devname>_hw_reset(d);
		if (ret)
			pr_warning("HW reset failed: %d (continuing)\n", ret);
	}

	/* Device-type-specific configuration */
	if (d->device_type == <DEVNAME>_DEVICE_BINARY_CAPABLE) {
		/* Configure binary protocol I/O on UART */
		/* Enable/disable NMEA and binary on input/output */
	} else if (d->device_type == <DEVNAME>_DEVICE_NMEA_ONLY) {
		/* Configure NMEA message rates */
	} else {
		ret = -EINVAL;
		goto free_gpio;
	}

	/* Configure PPS if requested */
	if (d->pps_enable) {
		ret = <devname>_configure_pps(d, true);
		if (ret)
			pr_warning("PPS config failed: %d (continuing)\n",
				   ret);
	}

	*dev = d;

	return 0;

free_gpio:
	if (d->gpio_reset)
		no_os_gpio_remove(d->gpio_reset);
free_uart:
	no_os_uart_remove(d->uart_desc);
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
	if (!dev)
		return -EINVAL;

	if (dev->gpio_reset)
		no_os_gpio_remove(dev->gpio_reset);

	if (dev->uart_desc)
		no_os_uart_remove(dev->uart_desc);

	no_os_free(dev);

	return 0;
}
```

---

## 5. IIO Support

IIO support is **not applicable** for GNSS/GPS receiver drivers. GNSS
devices output serial data streams (NMEA sentences or vendor binary
packets) that represent position/velocity/time solutions rather than
sampled analog channels. The IIO subsystem's channel-based model
(`IIO_VOLTAGE`, scan types, buffered reads) does not map to this
data model.

If your device includes an analog front-end (e.g., an IF sampler) that
exposes raw ADC channels, refer to the ADC driver template instead.

---

## 6. IIO Source

Not applicable for GNSS/GPS drivers. See Section 5 above.

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
	$(INCLUDE)/no_os_uart.h      \
	$(INCLUDE)/no_os_alloc.h     \
	$(INCLUDE)/no_os_irq.h       \
	$(INCLUDE)/no_os_util.h      \
	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_irq.c  \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_util.c  \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/gnss-gps/<devname>/<devname>.h
SRCS += $(DRIVERS)/gnss-gps/<devname>/<devname>.c
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
extern struct no_os_uart_init_param <devname>_uart_ip;
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

struct no_os_uart_init_param <devname>_uart_ip = {
	.device_id = GNSS_UART_DEVICE_ID,
	.baud_rate = GNSS_UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = GNSS_UART_OPS,
	.extra = GNSS_UART_EXTRA,
};

struct <devname>_init_param <devname>_ip = {
	.uart_init = &<devname>_uart_ip,
	.gpio_reset = NULL,
	.device_type = <DEVNAME>_DEVICE_BINARY_CAPABLE,
	.nmea_constellation_type = <DEVNAME>_NMEA_GNSS_MULTI,
	.binary_input_enable = <DEVNAME>_ENABLE,
	.binary_output_enable = <DEVNAME>_ENABLE,
	.nmea_input_enable = <DEVNAME>_DISABLE,
	.nmea_output_enable = <DEVNAME>_DISABLE,
	.baud_rate = 38400,
	.pps_enable = 1,
	.pps_frequency = 1,
	.pps_pulse_length = 100,
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
	struct <devname>_precise_time timing;
	struct <devname>_nmea_position position;
	int ret;

	pr_info("\r\nRunning <DEVNAME> Basic Example\r\n");

	ret = <devname>_init(&dev, &<devname>_ip);
	if (ret)
		goto error;

	while (1) {
		ret = <devname>_refresh_timing(dev);
		if (ret)
			goto free_dev;

		ret = <devname>_get_precise_time(dev, &timing);
		if (ret)
			goto free_dev;

		if (timing.time_valid) {
			pr_info("UTC: %04u-%02u-%02u %02u:%02u:%02u\r\n",
				timing.year, timing.month, timing.day,
				timing.hour, timing.minute, timing.second);
		}

		ret = <devname>_get_position(dev, &position);
		if (ret)
			goto free_dev;

		if (position.fix_valid) {
			pr_info("Lat: %.6f  Lon: %.6f  Alt: %.1f m  Sats: %u\r\n",
				position.latitude, position.longitude,
				position.altitude, position.satellites_used);
		}

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
#include "maxim_gpio.h"

/* Console UART (debug/stdio output) */
#define UART_IRQ_ID		UART1_IRQn
#define UART_DEVICE_ID		1
#define UART_BAUDRATE		115200
#define UART_OPS		&max_uart_ops
#define UART_EXTRA		&max_uart_extra

/* GNSS device UART */
#define GNSS_UART_DEVICE_ID	2
#define GNSS_UART_BAUDRATE	38400
#define GNSS_UART_OPS		&max_uart_ops
#define GNSS_UART_EXTRA		&max_gnss_uart_extra

extern struct max_uart_init_param max_uart_extra;
extern struct max_uart_init_param max_gnss_uart_extra;

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

struct max_uart_init_param max_gnss_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};
```

### 7.13 `projects/<project_name>/src/platform/maxim/platform_src.mk`

```makefile
INCS +=	$(PLATFORM_DRIVERS)/maxim_gpio.h      \
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

The <DEVNAME> is a multi-constellation GNSS receiver module supporting
GPS, GLONASS, Galileo, and BeiDou satellite systems. It communicates
over UART using both NMEA 0183 ASCII sentences and a vendor-specific
binary protocol. The driver provides a unified API for position,
velocity, and precise time data with configurable PPS output.

Applications
------------

* Precision timing and synchronization
* Asset tracking and fleet management
* Geofencing and location-based services
* Time-stamping for data acquisition systems

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``<devname>_init`` function allocates memory for the device
descriptor, initialises the UART interface, optionally performs a
hardware reset via GPIO, and configures the receiver's protocol and
message output settings. The function uses goto-based cleanup to
release resources on failure.

Position and Timing
~~~~~~~~~~~~~~~~~~~

The ``<devname>_refresh_timing`` function retrieves the latest
navigation solution from the receiver. For binary-capable devices it
polls NAV-PVT messages; for NMEA-only devices it reads and parses
RMC and GGA sentences from the UART stream.

The ``<devname>_get_precise_time`` function returns unified time
information with the best available precision (nanosecond for binary
protocol, millisecond for NMEA).

The ``<devname>_get_position`` function returns latitude, longitude,
altitude, fix quality, and satellite count from the latest fix.

PPS Configuration
~~~~~~~~~~~~~~~~~

The ``<devname>_configure_pps`` function enables or disables the
pulse-per-second output with configurable frequency and pulse width.
PPS is aligned to the GNSS time grid for precision timing applications.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "<devname>.h"

   int main()
   {
       struct <devname>_init_param ip = {
           .uart_init = &gnss_uart_ip,
           .gpio_reset = NULL,
           .device_type = <DEVNAME>_DEVICE_BINARY_CAPABLE,
           .baud_rate = 38400,
           .binary_input_enable = <DEVNAME>_ENABLE,
           .binary_output_enable = <DEVNAME>_ENABLE,
           .pps_enable = 1,
           .pps_frequency = 1,
           .pps_pulse_length = 100,
       };

       struct <devname>_dev *dev;
       struct <devname>_precise_time timing;
       int ret;

       ret = <devname>_init(&dev, &ip);
       if (ret)
           return ret;

       ret = <devname>_refresh_timing(dev);
       if (ret)
           goto cleanup;

       ret = <devname>_get_precise_time(dev, &timing);
       if (ret)
           goto cleanup;

       /* timing contains UTC date/time with nanosecond precision */

   cleanup:
       <devname>_remove(dev);
       return ret;
   }
```

### 8.2 Sphinx Entry (`doc/sphinx/source/drivers/gnss-gps/<devname>.rst`)

```rst
.. include:: ../../../../../drivers/gnss-gps/<devname>/README.rst
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
10. **UART-based communication** -- GNSS drivers communicate via UART
    rather than SPI/I2C. The project needs separate UART init params
    for the console (stdio) and the GNSS device.

---

## 10. Commit Conventions

```
drivers: <devname>: add <devname> driver support

Add support for the <DEVNAME> multi-constellation GNSS receiver with
UART-based NMEA 0183 and vendor binary protocol communication.

Signed-off-by: Your Name <your.name@analog.com>
```
