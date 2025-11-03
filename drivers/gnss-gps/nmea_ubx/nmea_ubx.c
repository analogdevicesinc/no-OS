/***************************************************************************//**
*   @file   ublox_gnss.c
*   @brief  Implementation of GNSS Driver.
*   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
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

#include "nmea_ubx.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "no_os_util.h"
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include <stdlib.h>
#include <errno.h>

/* Functions Definitions */

/**
 * @brief Calculate UBX checksum
 * @param data - Data buffer
 * @param length - Data length
 * @param checksumA - Checksum A output
 * @param checksumB - Checksum B output
 */
void gnss_calculate_checksum(uint8_t *data, uint16_t length,
			     uint8_t *checksumA, uint8_t *checksumB)
{
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	uint16_t i;

	for (i = 0; i < length; i++) {
		ck_a += data[i];
		ck_b += ck_a;
	}

	*checksumA = ck_a;
	*checksumB = ck_b;
}

/**
 * @brief Verify UBX checksum
 * @param data - Data buffer
 * @param length - Data length
 * @param checksumA - Expected checksum A
 * @param checksumB - Expected checksum B
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_verify_checksum(uint8_t *data, uint16_t length,
			 uint8_t checksumA, uint8_t checksumB)
{
	uint8_t calc_a, calc_b;

	gnss_calculate_checksum(data, length, &calc_a, &calc_b);

	if (calc_a != checksumA || calc_b != checksumB)
		return -EBADMSG;

	return 0;
}

/**
 * @brief Initialize the GNSS device and configure communication.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int gnss_init(struct gnss_dev **device,
	      struct gnss_init_param init_param)
{
	struct gnss_dev *dev;
	int ret;
	uint8_t *scrap;

	if (!device)
		return -EINVAL;

	dev = (struct gnss_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;
	init_param.uart_init->baud_rate = init_param.baud_rate;

	/* Initialize UART */
	ret = no_os_uart_init(&dev->uart_desc, init_param.uart_init);
	if (ret)
		goto remove_device;

	/* Initialize GPIO reset if provided */
	if (init_param.gpio_reset) {
		dev->gpio_reset = init_param.gpio_reset;
		ret = no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_HIGH);
		if (ret)
			goto error_uart;
	}

	if (init_param.pps_enable) {
		dev->pps_enable = true;
		dev->pps_frequency = init_param.pps_frequency;
		dev->pps_pulse_length = init_param.pps_pulse_length;
	}
	/* Store device type for runtime protocol selection */
	dev->device_type = init_param.device_type;

	/* Store NMEA constellation type for NMEA-only devices */
	dev->nmea_constellation_type = init_param.nmea_constellation_type;

	/* Initialize position refresh counter */
	dev->position_refresh_counter = 0;

	/* Perform hardware reset if GPIO is available */
	if (dev->gpio_reset) {
		ret = gnss_hw_reset(dev);
		if (ret)
			pr_warning("HW reset failed: %d (continuing)\n", ret);
	} else if (dev->device_type == GNSS_DEVICE_UBX_CAPABLE) {
		ret = gnss_ubx_sw_reset(dev);
		if (ret)
			pr_warning("SW reset failed: %d (continuing)\n", ret);
	}

	if (dev->device_type == GNSS_DEVICE_UBX_CAPABLE) {
		/* UBX-capable device configuration */
		/* Configure UBX protocols if specified */
		/* Disable NMEA output for clean UBX operation */
		ret = gnss_ubx_set_input_protocol_uart1(dev, GNSS_PROTOCOL_UBX,
							init_param.ubx_input_enable, GNSS_CONFIG_LAYER_ALL);
		if (ret)
			pr_warning("Failed to enable UBX input protocol: %d\n", ret);

		ret = gnss_ubx_set_output_protocol_uart1(dev, GNSS_PROTOCOL_NMEA,
				init_param.nmea_output_enable, GNSS_CONFIG_LAYER_ALL);
		if (ret)
			pr_warning("Failed to disable NMEA output protocol: %d\n", ret);

		ret = gnss_ubx_set_output_protocol_uart1(dev, GNSS_PROTOCOL_UBX,
				init_param.ubx_output_enable, GNSS_CONFIG_LAYER_ALL);
		if (ret)
			pr_warning("Failed to disable NMEA output protocol: %d\n", ret);

		ret = gnss_ubx_set_input_protocol_uart1(dev, GNSS_PROTOCOL_NMEA,
							init_param.nmea_input_enable, GNSS_CONFIG_LAYER_ALL);
		if (ret)
			pr_warning("Failed to enable NMEA input protocol: %d\n", ret);

		no_os_mdelay(100);

		/* flush the UART port */
		scrap = no_os_calloc(500, sizeof(uint8_t));
		no_os_uart_read_nonblocking(dev->uart_desc, scrap, 500);
		no_os_free(scrap);

		/* Configure baudrate via UBX commands (if specified) */
		if (init_param.baud_rate > 0) {
			ret = gnss_ubx_configure_baudrate(dev, init_param.baud_rate);
			if (ret) {
				pr_warning("UBX baudrate configuration failed: %d (continuing)\n", ret);
			}
		}

	} else if (dev->device_type == GNSS_DEVICE_NMEA_ONLY) {
		/* NMEA-only device configuration */
		/* Use default baudrate if not specified */
		uint32_t baudrate = (init_param.baud_rate > 0) ? init_param.baud_rate : 38400;

		/* Configure NMEA device for timing operation */
		ret = gnss_configure_nmea_device(dev, baudrate, 1);  /* 1Hz GPRMC rate */
		if (ret) {
			pr_warning("NMEA device configuration failed: %d (continuing)\n", ret);
		}
	} else {
		pr_err("Invalid device type: %d\n", dev->device_type);
		ret = -EINVAL;
		goto error_reset;
	}

	*device = dev;

	return 0;

error_reset:
	if (dev->gpio_reset)
		no_os_gpio_remove(dev->gpio_reset);
error_uart:
	no_os_uart_remove(dev->uart_desc);
remove_device:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Remove the GNSS device and release allocated resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int gnss_remove(struct gnss_dev *dev)
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

/**
 * @brief Reset the GNSS device using hardware reset pin.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int gnss_hw_reset(struct gnss_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (!dev->gpio_reset)
		return -EINVAL;

	/* Pull reset low */
	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	/* Hold reset for 100ms */
	no_os_mdelay(100);

	/* Release reset */
	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/* Wait for device to boot */
	no_os_mdelay(1000);

	return 0;
}

/**
 * @brief Reset the device using SW reset
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_sw_reset(struct gnss_dev *dev)
{
	uint8_t payload[4] = {0xFF, 0xFF, 0x00, 0x00}; /* Cold start */
	int ret;

	if (!dev)
		return -EINVAL;

	ret = gnss_ubx_send_packet(dev, UBX_CLASS_CFG, 0x04, payload, 4);
	if (ret)
		return ret;

	/* Wait for reset to complete */
	no_os_mdelay(1000);

	return 0;
}

/**
 * @brief Write data to the device
 * @param dev - The device structure
 * @param data - Data buffer to write
 * @param size - Number of bytes to write
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_write(struct gnss_dev *dev, uint8_t *data, uint16_t size)
{
	int ret;

	if (!dev || !data || size == 0)
		return -EINVAL;
	/* UART is used in this implementation */
	return no_os_uart_write(dev->uart_desc, data, (uint32_t)size);
}

/**
 * @brief Read data from the device
 * @param dev - The device structure
 * @param data - Data buffer to read into
 * @param size - Number of bytes to read
 * @return number of bytes read in case of succes, negative error code otherwise
 */
int gnss_read(struct gnss_dev *dev, uint8_t *data,
	      uint16_t size)
{
	int ret;

	if (!dev || !data || size == 0)
		return -EINVAL;
	/* UART is used in this implementation */
	return no_os_uart_read(dev->uart_desc, data, (uint32_t)size);
}

/**
 * @brief Send UBX binary protocol packet to the GNSS device.
 * @param dev     - The device structure.
 * @param cls     - UBX message class.
 * @param id      - UBX message ID.
 * @param payload - Pointer to payload data buffer.
 * @param length  - Payload length in bytes.
 * @return ret 0 in case of success, negative error code otherwise.
 */
int gnss_ubx_send_packet(struct gnss_dev *dev, uint8_t cls,
			 uint8_t id, uint8_t *payload, uint16_t length)
{
	uint8_t *packet;
	uint8_t checksum_a, checksum_b;
	uint16_t packet_size;
	int ret;

	if (!dev)
		return -EINVAL;

	if (length > (UBX_MAX_PACKET_SIZE - UBX_HEADER_SIZE - UBX_CHECKSUM_SIZE))
		return -ERANGE;

	/* packet header is 6 bytes and CRC is 2 bytes */
	packet = no_os_calloc(length + 8, sizeof(uint8_t));

	/* Build packet header */
	packet[0] = UBX_SYNCH_1;
	packet[1] = UBX_SYNCH_2;
	packet[2] = cls;
	packet[3] = id;
	packet[4] = length & 0xFF;
	packet[5] = (length >> 8) & 0xFF;

	/* Copy payload */
	if (payload && length > 0)
		memcpy(&packet[6], payload, length);

	/* Calculate checksum over class, id, length, and payload */
	gnss_calculate_checksum(&packet[2], 4 + length, &checksum_a, &checksum_b);

	/* Add checksum */
	packet[6 + length] = checksum_a;
	packet[7 + length] = checksum_b;

	packet_size = 8 + length;

	/* Send packet */
	ret = gnss_write(dev, packet, packet_size);
	if (ret < 0) {
		no_os_free(packet);
		return ret;
	}

	no_os_free(packet);
	return 0;
}

/**
 * @brief UBX packet receive
 * @param dev - The device structure
 * @param packet - Packet structure to fill
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_receive_packet(struct gnss_dev *dev,
			    struct gnss_ubx_packet *packet)
{
	uint8_t *byte;
	uint8_t *header;
	uint16_t payload_index = 0;
	int ret, bytes_read;
	uint8_t *data;
	uint8_t payload_a, payload_b;

	if (!dev || !packet)
		return -EINVAL;

	header = no_os_calloc(6, sizeof(uint8_t));
	/* Read 6 bytes that represent the UBX header */
	ret = gnss_read(dev, header, 6);
	if (ret < 0)
		goto free_header;

	/* byte 1 - first sync byte */
	packet->sync1 = header[0];
	/* byte 2 - second sync byte */
	packet->sync2 = header[1];
	/* byte 3 - class byte */
	packet->cls = header[2];
	/* byte 4 - id byte */
	packet->id = header[3];
	/* bytes 5-6 - length bytes (little endian) */
	packet->length = no_os_get_unaligned_le16(&header[4]);
	/* Prepare buffer for payload depending on the packet length + 2 bytes CRC */
	byte = no_os_calloc(packet->length + 2, sizeof(uint8_t));
	packet->payload = no_os_calloc(packet->length, sizeof(uint8_t));
	/* Read payload + 2 checksum bytes */
	ret = gnss_read(dev, byte, packet->length + 2);
	if (ret < 0) {
		no_os_free(byte);
		goto free_payload;
	}

	/* Save the checksum values */
	packet->checksumA = byte[packet->length];
	packet->checksumB = byte[packet->length + 1];
	/* Copy payload data */
	memcpy(&packet->payload[0], byte, packet->length);
	no_os_free(byte);

	header[0] = packet->cls;
	header[1] = packet->id;
	no_os_put_unaligned_le16(packet->length, &header[2]);

	/* Calculate checksum over header and payload */
	data = no_os_calloc(packet->length + 4, sizeof(uint8_t));
	memcpy(&data[0], header, 4);
	if (packet->payload && packet->length > 0) {
		memcpy(&data[4], packet->payload, packet->length);
		ret = gnss_verify_checksum(data, packet->length + 4, packet->checksumA,
					   packet->checksumB);
		if (ret) {
			pr_warning("Checksum verification failed\n");
			if (packet->payload) {
				goto free_payload;
			} else
				goto free_data;
		} else {
			goto free_data;
		}
	} else {
		ret = gnss_verify_checksum(header, 4, packet->checksumA,
					   packet->checksumB);
		if (ret) {
			pr_warning("Checksum verification failed\n");
			goto free_data;
		} else {
			goto free_data;
		}
	}

free_payload:
	no_os_free(packet->payload);
	if (byte)
		no_os_free(byte);
free_data:
	no_os_free(data);
free_header:
	no_os_free(header);

	return ret;
}

/**
 * @brief Wait for ACK/NAK
 * @param dev - The device structure
 * @param cls - Expected class
 * @param id - Expected message ID
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_wait_for_ack(struct gnss_dev *dev, uint8_t cls, uint8_t id)
{
	struct gnss_ubx_packet packet;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = gnss_ubx_receive_packet(dev, &packet);
	if (ret)
		return ret;
	if (packet.cls == UBX_CLASS_ACK) {
		if (packet.id == UBX_ACK_ACK && packet.length >= 2) {
			if (packet.payload[0] == cls && packet.payload[1] == id) {
				/* ACK received */
				ret = 0;
				goto free_payload;
			}
		} else if (packet.id == UBX_ACK_NACK && packet.length >= 2) {
			if (packet.payload[0] == cls && packet.payload[1] == id) {
				/* NAK received */
				pr_warning("NAK received for class=0x%02X, id=0x%02X\n", cls, id);
				ret = -ENODATA;
				goto free_payload;
			}
		}
	} else {
		pr_warning("Received unexpected UBX packet while waiting for ACK/NAK\n");
		ret = -EINVAL;
	}

free_payload:
	if (packet.payload)
		no_os_free(packet.payload);
	return ret;
}

/**
 * @brief Set configuration value by key (no ACK version)
 * @param dev - The device structure
 * @param key_id - Configuration key ID
 * @param value - Value to set
 * @param value_size - Size of value (1, 2, 4, or 8 bytes)
 * @param layer - Configuration layer (RAM, BBR, Flash)
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_set_val_no_ack(struct gnss_dev *dev, uint32_t key_id,
			    uint64_t value, uint8_t value_size, uint8_t layer)
{
	uint8_t *payload;
	uint8_t payload_len;
	uint8_t layer_val;
	int ret;

	if (!dev)
		return -EINVAL;

	if (value_size != 1 && value_size != 2 && value_size != 4 && value_size != 8)
		return -EINVAL;

	/* Convert layer enum to bitfield */
	switch (layer) {
	case GNSS_CONFIG_LAYER_RAM:
		layer_val = VAL_LAYER_RAM;
		break;
	case GNSS_CONFIG_LAYER_BBR:
		layer_val = VAL_LAYER_BBR;
		break;
	case GNSS_CONFIG_LAYER_FLASH:
		layer_val = VAL_LAYER_FLASH;
		break;
	case GNSS_CONFIG_LAYER_ALL:
		layer_val = VAL_LAYER_ALL;
		break;
	default:
		return -EINVAL;
	}

	payload = no_os_calloc(value_size + 8, sizeof(uint8_t));
	/* Build payload */
	payload[0] = 0;			/* Version */
	payload[1] = layer_val;		/* Layer */
	payload[2] = 0;			/* Reserved */
	payload[3] = 0;			/* Reserved */

	/* Add key */
	no_os_put_unaligned_le32(key_id, &payload[4]);

	switch (value_size) {
	case 1:
		payload[8] = (uint8_t)value;
		break;
	case 2:
		no_os_put_unaligned_le16((uint16_t)value, &payload[8]);
		break;
	case 4:
		no_os_put_unaligned_le32((uint32_t)value, &payload[8]);
		break;
	case 8:
		no_os_put_unaligned_le64(value, &payload[8]);
		break;
	default:
		no_os_free(payload);
		return -EINVAL;
	}
	/* Send VALSET command */
	ret = gnss_ubx_send_packet(dev, UBX_CLASS_CFG, UBX_CFG_VALSET,
				   payload, value_size + 8);
	if (ret)
		goto free_payload;

	ret = 0;

free_payload:
	no_os_free(payload);
	return ret;
}

/**
 * @brief Set configuration value by key
 * @param dev - The device structure
 * @param key_id - Configuration key ID
 * @param value - Value to set
 * @param value_size - Size of value (1, 2, 4, or 8 bytes)
 * @param layer - Configuration layer (RAM, BBR, Flash)
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_set_val(struct gnss_dev *dev, uint32_t key_id,
		     uint64_t value, uint8_t value_size, uint8_t layer)
{
	uint8_t *payload;
	uint8_t payload_len;
	uint8_t layer_val;
	int ret;

	if (!dev)
		return -EINVAL;

	if (value_size != 1 && value_size != 2 && value_size != 4 && value_size != 8)
		return -EINVAL;

	/* Convert layer enum to bitfield */
	switch (layer) {
	case GNSS_CONFIG_LAYER_RAM:
		layer_val = VAL_LAYER_RAM;
		break;
	case GNSS_CONFIG_LAYER_BBR:
		layer_val = VAL_LAYER_BBR;
		break;
	case GNSS_CONFIG_LAYER_FLASH:
		layer_val = VAL_LAYER_FLASH;
		break;
	case GNSS_CONFIG_LAYER_ALL:
		layer_val = VAL_LAYER_ALL;
		break;
	default:
		return -EINVAL;
	}

	payload = no_os_calloc(value_size + 8, sizeof(uint8_t));
	/* Build payload */
	payload[0] = 0;			/* Version */
	payload[1] = layer_val;		/* Layer */
	payload[2] = 0;			/* Reserved */
	payload[3] = 0;			/* Reserved */

	/* Add key */
	no_os_put_unaligned_le32(key_id, &payload[4]);

	switch (value_size) {
	case 1:
		payload[8] = (uint8_t)value;
		break;
	case 2:
		no_os_put_unaligned_le16((uint16_t)value, &payload[8]);
		break;
	case 4:
		no_os_put_unaligned_le32((uint32_t)value, &payload[8]);
		break;
	case 8:
		no_os_put_unaligned_le64(value, &payload[8]);
		break;
	default:
		no_os_free(payload);
		return -EINVAL;
	}

	/* Send VALSET command */
	ret = gnss_ubx_send_packet(dev, UBX_CLASS_CFG, UBX_CFG_VALSET,
				   payload, value_size + 8);
	if (ret)
		goto free_payload;

	/* Wait for ACK */
	ret = gnss_ubx_wait_for_ack(dev, UBX_CLASS_CFG, UBX_CFG_VALSET);
	if (ret)
		goto free_payload;

	ret = 0;

free_payload:
	no_os_free(payload);
	return ret;
}

/**
 * @brief Get configuration value by key
 * @param dev - The device structure
 * @param key_id - Configuration key ID
 * @param value - Pointer to store the value
 * @param value_size - Size of value (1, 2, 4, or 8 bytes)
 * @param layer - Configuration layer (RAM, BBR, Flash)
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_get_val(struct gnss_dev *dev, uint32_t key_id,
		     uint64_t *value, uint8_t value_size, uint8_t layer)
{
	uint8_t *payload;
	uint8_t layer_val;
	struct gnss_ubx_packet response;
	int ret;

	if (!dev || !value)
		return -EINVAL;

	if (value_size != 1 && value_size != 2 && value_size != 4 && value_size != 8)
		return -EINVAL;

	/* Convert layer enum to bitfield */
	switch (layer) {
	case GNSS_CONFIG_LAYER_RAM:
		layer_val = VAL_LAYER_RAM;
		break;
	case GNSS_CONFIG_LAYER_BBR:
		layer_val = VAL_LAYER_BBR;
		break;
	case GNSS_CONFIG_LAYER_FLASH:
		layer_val = VAL_LAYER_FLASH;
		break;
	default:
		return -EINVAL;
	}

	payload = no_os_calloc(value_size + 8, sizeof(uint8_t));
	/* Build payload */
	payload[0] = 0;			/* Version */
	payload[1] = layer_val;		/* Layer */
	payload[2] = 0;			/* Reserved */
	payload[3] = 0;			/* Reserved */

	/* Add key */
	no_os_put_unaligned_le32(key_id, &payload[4]);

	memcpy(&payload[8], value, value_size);

	/* Send VALGET command */
	ret = gnss_ubx_send_packet(dev, UBX_CLASS_CFG, UBX_CFG_VALGET,
				   payload, 8);
	if (ret)
		goto free_payload;

	no_os_free(payload);

	/* Wait for response */
	ret = gnss_ubx_receive_packet(dev, &response);
	if (ret)
		goto free_response;

	/* Verify response */
	if (response.cls != UBX_CLASS_CFG || response.id != UBX_CFG_VALGET) {
		ret = -EIO;
		goto free_response;
	}

	if (response.length < (8 + value_size)) {
		ret = -EIO;
		goto free_response;
	}
	/* Extract value */
	switch (value_size) {
	case 1:
		*value = response.payload[8];
		break;
	case 2:
		*value = no_os_get_unaligned_le16(&response.payload[8]);
		break;
	case 4:
		*value = no_os_get_unaligned_le32(&response.payload[8]);
		break;
	case 8:
		*value = no_os_get_unaligned_le64(&response.payload[8]);
		break;
	default:
		ret = -EINVAL;
		goto free_response;
	}

	ret = 0;

free_payload:
	if (payload)
		no_os_free(payload);
free_response:
	if (response.payload)
		no_os_free(response.payload);

	return ret;
}

/**
 * @brief Clear save load configuration from flash/BBR
 * @param dev - The device structure
 * @param load_mask - Load mask (which layers to load)
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_clr_save_load_config(struct gnss_dev *dev, uint8_t mask)
{
	uint8_t payload[13] = {0};
	int ret;

	if (!dev)
		return -EINVAL;

	/* Clear/Save/Load configuration structure */
	payload[8] = mask;		/* Load mask */
	payload[12] = 0xFF;		/* Device mask (all devices) */
	payload[9] = 0xFF;
	payload[10] = 0xFF;
	payload[11] = 0xFF;

	ret = gnss_ubx_send_packet(dev, UBX_CLASS_CFG, UBX_CFG_CFG,
				   payload, 13);
	if (ret)
		return ret;

	/* Wait for ACK */
	ret = gnss_ubx_wait_for_ack(dev, UBX_CLASS_CFG, UBX_CFG_CFG);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Configure input protocol on UART1
 * @param dev - The device structure
 * @param protocol - Protocol type
 * @param enable - Enable/disable
 * @param layer - Configuration layer
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_set_input_protocol_uart1(struct gnss_dev *dev,
				      enum gnss_protocol protocol,
				      uint8_t enable, uint8_t layer)
{
	uint32_t key_id;

	if (!dev)
		return -EINVAL;

	switch (protocol) {
	case GNSS_PROTOCOL_UBX:
		key_id = UBLOX_CFG_UART1INPROT_UBX;
		break;
	case GNSS_PROTOCOL_NMEA:
		key_id = UBLOX_CFG_UART1INPROT_NMEA;
		break;
	case GNSS_PROTOCOL_RTCM3:
		key_id = UBLOX_CFG_UART1INPROT_RTCM3X;
		break;
	default:
		return -EINVAL;
	}

	return gnss_ubx_set_val_no_ack(dev, key_id, enable ? 1 : 0, 1, layer);
}

/**
 * @brief Configure output protocol on UART1
 * @param dev - The device structure
 * @param protocol - Protocol type
 * @param enable - Enable/disable
 * @param layer - Configuration layer
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_set_output_protocol_uart1(struct gnss_dev *dev,
				       enum gnss_protocol protocol,
				       uint8_t enable, uint8_t layer)
{
	uint32_t key_id;

	if (!dev)
		return -EINVAL;

	switch (protocol) {
	case GNSS_PROTOCOL_UBX:
		key_id = UBLOX_CFG_UART1OUTPROT_UBX;
		break;
	case GNSS_PROTOCOL_NMEA:
		key_id = UBLOX_CFG_UART1OUTPROT_NMEA;
		break;
	case GNSS_PROTOCOL_RTCM3:
		key_id = UBLOX_CFG_UART1OUTPROT_RTCM3X;
		break;
	default:
		return -EINVAL;
	}

	return gnss_ubx_set_val_no_ack(dev, key_id, enable ? 1 : 0, 1, layer);
}

/**
 * @brief Configure output protocol USB
 * @param dev - The device structure
 * @param protocol - Protocol type
 * @param enable - Enable/disable
 * @param layer - Configuration layer
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_set_output_protocol_usb(struct gnss_dev *dev,
				     enum gnss_protocol protocol,
				     uint8_t enable, uint8_t layer)
{
	uint32_t key_id;

	if (!dev)
		return -EINVAL;

	switch (protocol) {
	case GNSS_PROTOCOL_UBX:
		key_id = UBLOX_CFG_USBOUTPROT_UBX;
		break;
	case GNSS_PROTOCOL_NMEA:
		key_id = UBLOX_CFG_USBOUTPROT_NMEA;
		break;
	case GNSS_PROTOCOL_RTCM3:
		key_id = UBLOX_CFG_USBOUTPROT_RTCM3X;
		break;
	default:
		return -EINVAL;
	}

	return gnss_ubx_set_val(dev, key_id, enable ? 1 : 0, 1, layer);
}

/**
 * @brief Configure input protocol on UART2
 * @param dev - The device structure
 * @param protocol - Protocol type
 * @param enable - Enable/disable
 * @param layer - Configuration layer
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_set_input_protocol_uart2(struct gnss_dev *dev,
				      enum gnss_protocol protocol,
				      uint8_t enable, uint8_t layer)
{
	uint32_t key_id;

	if (!dev)
		return -EINVAL;

	switch (protocol) {
	case GNSS_PROTOCOL_UBX:
		key_id = UBLOX_CFG_UART2INPROT_UBX;
		break;
	case GNSS_PROTOCOL_NMEA:
		key_id = UBLOX_CFG_UART2INPROT_NMEA;
		break;
	case GNSS_PROTOCOL_RTCM3:
		key_id = UBLOX_CFG_UART2INPROT_RTCM3X;
		break;
	default:
		return -EINVAL;
	}

	return gnss_ubx_set_val(dev, key_id, enable ? 1 : 0, 1, layer);
}

/**
 * @brief Configure output protocol on UART2
 * @param dev - The device structure
 * @param protocol - Protocol type
 * @param enable - Enable/disable
 * @param layer - Configuration layer
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_set_output_protocol_uart2(struct gnss_dev *dev,
				       enum gnss_protocol protocol,
				       uint8_t enable, uint8_t layer)
{
	uint32_t key_id;

	if (!dev)
		return -EINVAL;

	switch (protocol) {
	case GNSS_PROTOCOL_UBX:
		key_id = UBLOX_CFG_UART2OUTPROT_UBX;
		break;
	case GNSS_PROTOCOL_NMEA:
		key_id = UBLOX_CFG_UART2OUTPROT_NMEA;
		break;
	case GNSS_PROTOCOL_RTCM3:
		key_id = UBLOX_CFG_UART2OUTPROT_RTCM3X;
		break;
	default:
		return -EINVAL;
	}

	return gnss_ubx_set_val(dev, key_id, enable ? 1 : 0, 1, layer);
}

/**
 * @brief Configure UBX device baudrate via UBX commands
 * @param dev - The device structure
 * @param baudrate - Desired baudrate
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_configure_baudrate(struct gnss_dev *dev,
				uint32_t baudrate)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Set UART1 baudrate using CFG-VALSET command */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_UART1_BAUDRATE, baudrate, 4,
			       GNSS_CONFIG_LAYER_ALL);
	if (ret) {
		pr_err("Failed to set UART1 baudrate to %lu: %d\n", baudrate, ret);
		return ret;
	}

	/* Small delay to allow configuration to take effect */
	no_os_mdelay(100);

	return 0;
}

/**
 * @brief Poll navigation data (NAV-PVT)
 * @param dev - The device structure
 * @param nav_data - Navigation data structure to fill
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_poll_nav_pvt(struct gnss_dev *dev,
			  struct gnss_ubx_nav_pvt *nav_data)
{
	struct gnss_ubx_packet response;
	int ret;

	if (!dev || !nav_data)
		return -EINVAL;

	/* Send poll message */
	ret = gnss_ubx_send_packet(dev, UBX_CLASS_NAV, UBX_NAV_PVT, NULL, 0);
	if (ret)
		return ret;

	/* Wait for response */
	ret = gnss_ubx_receive_packet(dev, &response);
	if (ret)
		return ret;

	/* Verify response */
	if (response.cls != UBX_CLASS_NAV || response.id != UBX_NAV_PVT) {
		if (response.payload)
			no_os_free(response.payload);
		return -EIO;
	}

	if (response.length < sizeof(struct gnss_ubx_nav_pvt)) {
		if (response.payload)
			no_os_free(response.payload);
		return -EIO;
	}

	/* Copy data */
	memcpy(nav_data, response.payload, sizeof(struct gnss_ubx_nav_pvt));

	if (response.payload)
		no_os_free(response.payload);

	return 0;
}

/**
 * @brief Poll time data (NAV-TIMEUTC)
 * @param dev - The device structure
 * @param time_data - Time data structure to fill
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_poll_nav_timeutc(struct gnss_dev *dev,
			      struct gnss_ubx_nav_timeutc *time_data)
{
	struct gnss_ubx_packet response;
	int ret;

	if (!dev || !time_data)
		return -EINVAL;

	/* Send poll message */

	ret = gnss_ubx_send_packet(dev, UBX_CLASS_NAV, UBX_NAV_TIMEUTC, NULL, 0);
	if (ret)
		return ret;


	/* Wait for response */
	ret = gnss_ubx_receive_packet(dev, &response);
	if (ret)
		return ret;


	/* Verify response */
	if (response.cls != UBX_CLASS_NAV || response.id != UBX_NAV_TIMEUTC) {
		if (response.payload)
			no_os_free(response.payload);
		return -EIO;
	}

	if (response.length < sizeof(struct gnss_ubx_nav_timeutc)) {
		if (response.payload)
			no_os_free(response.payload);
		return -EIO;
	}

	/* Copy data */
	memcpy(time_data, response.payload, sizeof(struct gnss_ubx_nav_timeutc));

	if (response.payload)
		no_os_free(response.payload);

	return 0;
}

/**
 * @brief Get latest navigation data
 * @param dev - The device structure
 * @param nav_data - Navigation data structure to fill
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_get_nav_data(struct gnss_dev *dev,
		      struct gnss_ubx_nav_pvt *nav_data)
{
	if (!dev || !nav_data)
		return -EINVAL;

	memcpy(nav_data, &dev->nav_data, sizeof(struct gnss_ubx_nav_pvt));
	return 0;
}

/**
 * @brief Get latest time data
 * @param dev - The device structure
 * @param time_data - Time data structure to fill
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_get_time_data(struct gnss_dev *dev,
		       struct gnss_ubx_nav_timeutc *time_data)
{
	if (!dev || !time_data)
		return -EINVAL;

	memcpy(time_data, &dev->time_data, sizeof(struct gnss_ubx_nav_timeutc));
	return 0;
}

/**
 * @brief Convert coordinates from 1e-7 degrees to float degrees
 * @param coord - Coordinate in 1e-7 degrees
 * @return Coordinate in degrees
 */
float gnss_coord_to_degrees(int32_t coord)
{
	return (float)coord / 1e7;
}

/**
 * @brief Convert velocity from cm/s to m/s
 * @param vel - Velocity in cm/s
 * @return Velocity in m/s
 */
float gnss_velocity_to_ms(int32_t vel)
{
	return (float)vel / 100.0;
}

/**
 * @brief Convert time accuracy from ns to ms
 * @param acc - Time accuracy in ns
 * @return Time accuracy in ms
 */
float gnss_time_acc_to_ms(uint32_t acc)
{
	return (float)acc / 1e6;
}

/**
 * @brief Convert position accuracy from mm to m
 * @param acc - Position accuracy in mm
 * @return Position accuracy in m
 */
float gnss_pos_acc_to_m(uint32_t acc)
{
	return (float)acc / 1000.0;
}

/**
 * @brief Get nanosecond fraction of current time from NAV-PVT data
 * @param dev - The device structure
 * @return Nanosecond fraction (can be negative), 0 on error
 */
int32_t gnss_ubx_get_nanoseconds(struct gnss_dev *dev)
{
	if (!dev)
		return 0;

	return dev->nav_data.nano;
}

/**
 * @brief Check if the current time is valid
 * @param dev - The device structure
 * @return true if time is valid, false otherwise
 */
bool gnss_ubx_get_time_valid(struct gnss_dev *dev)
{
	if (!dev)
		return false;

	/* Check time valid flag in NAV-PVT valid field (bit 1) */
	return (dev->nav_data.valid & 0x02) != 0;
}

/**
 * @brief Check if the current time is fully resolved
 * @param dev - The device structure
 * @return true if time is fully resolved, false otherwise
 */
bool gnss_ubx_get_time_fully_resolved(struct gnss_dev *dev)
{
	if (!dev)
		return false;

	/* Check time fully resolved flag in NAV-PVT valid field (bit 2) */
	return (dev->nav_data.valid & 0x04) != 0;
}

/**
 * @brief Get Unix epoch time with microsecond precision
 * @param dev - The device structure
 * @param microseconds - Pointer to store microsecond fraction
 * @return Unix epoch time in seconds, negative code on error
 */
uint32_t gnss_ubx_get_unix_epoch_with_micros(struct gnss_dev *dev,
		uint32_t *microseconds)
{
	uint32_t epoch_time = 0;
	int32_t nano;
	uint32_t micros;

	if (!dev || !microseconds)
		return -EINVAL;

	/* Calculate Unix epoch from date/time components */
	/* Simple calculation - may need more complex implementation */
	uint16_t year = dev->nav_data.year;
	uint8_t month = dev->nav_data.month;
	uint8_t day = dev->nav_data.day;
	uint8_t hour = dev->nav_data.hour;
	uint8_t minute = dev->nav_data.min;
	uint8_t second = dev->nav_data.sec;

	/* Basic Unix epoch calculation (approximate) */
	/* Note: This is a simplified calculation.*/
	/* Consider using a proper time library if necessary */
	if (year >= 1970) {
		/* Rough calculation - days since 1970 */
		uint32_t days = (year - 1970) * 365 + (year - 1970) / 4;
		/* Add days for current year */
		uint16_t month_days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
		if (month > 0 && month <= 12)
			days += month_days[month - 1] + day - 1;

		/* Convert to seconds */
		epoch_time = days * 86400 + hour * 3600 + minute * 60 + second;
	}

	/* Convert nanoseconds to microseconds */
	nano = dev->nav_data.nano;
	if (nano < 0) {
		/* Handle negative nanoseconds */
		micros = (uint32_t)(1000000 + (nano / 1000));
		if (epoch_time > 0)
			epoch_time -= 1; /* Subtract one second */
	} else {
		micros = (uint32_t)(nano / 1000);
	}

	*microseconds = micros;
	return epoch_time;
}

/**
 * @brief Get comprehensive precise time information
 * @param dev - The device structure
 * @param precise_time - Structure to store precise time data
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_get_precise_time(struct gnss_dev *dev,
			      struct gnss_precise_time *precise_time)
{
	if (!dev || !precise_time)
		return -EINVAL;

	/* Fill in the precise time structure */
	precise_time->unix_epoch = gnss_ubx_get_unix_epoch_with_micros(dev,
				   &precise_time->nanoseconds);
	precise_time->nanoseconds = dev->nav_data.nano;
	precise_time->time_accuracy = dev->nav_data.tAcc;
	precise_time->time_valid = gnss_ubx_get_time_valid(dev);
	precise_time->time_fully_resolved = gnss_ubx_get_time_fully_resolved(dev);
	precise_time->date_valid = (dev->nav_data.valid & 0x01) != 0;

	/* Copy date/time components */
	precise_time->year = dev->nav_data.year;
	precise_time->month = dev->nav_data.month;
	precise_time->day = dev->nav_data.day;
	precise_time->hour = dev->nav_data.hour;
	precise_time->minute = dev->nav_data.min;
	precise_time->second = dev->nav_data.sec;

	return 0;
}

/**
 * @brief Configure time pulse output for PPS (Pulse Per Second)
 * @param dev - The device structure
 * @param enable - Enable/disable time pulse
 * @param frequency_hz - Output frequency (typically 1 Hz for PPS)
 * @param pulse_length_us - Pulse length in microseconds
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_ubx_configure_time_pulse(struct gnss_dev *dev,
				  bool enable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Time pulse defined as period */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_PULSE_DEF, 0,
			       1, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set time pulse enable: %d\n", ret);
		return ret;
	}
	/* Time pulse length */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_PULSE_LENGTH_DEF,
			       1, 1, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set time pulse length mode: %d\n", ret);
		return ret;
	}
	/* Time pulse period */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_PERIOD_TP1,
			       (uint64_t)((uint64_t)S_TO_US / (uint64_t)dev->pps_frequency),
			       4, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set time pulse period: %d\n", ret);
		return ret;
	}
	/* Time pulse pulse period when locked to GNSS */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_PERIOD_LOCK_TP1,
			       (uint64_t)((uint64_t)S_TO_US / (uint64_t)dev->pps_frequency),
			       4, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set time pulse locked period: %d\n", ret);
		return ret;
	}
	/* Time pulse length in us */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_LEN_TP1,
			       (uint64_t)dev->pps_pulse_length * (uint64_t)MS_TO_US,
			       4, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set time pulse length: %d\n", ret);
		return ret;
	}
	/* Time pulse length in us when locked to GNSS */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_LEN_LOCK_TP1,
			       (uint64_t)dev->pps_pulse_length * (uint64_t)MS_TO_US,
			       4, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set time pulse locked length: %d\n", ret);
		return ret;
	}
	/* Enable PPS pin */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_TP1_ENA, 1,
			       1, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set time pulse enable: %d\n", ret);
		return ret;
	}
	/* Sync time pulse to GNSS */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_SYNC_GNSS_TP1, 1,
			       1, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to sync time pulse to GNSS: %d\n", ret);
		return ret;
	}
	/* USe locked parameters when possible */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_USE_LOCKED_TP1, 1,
			       1, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set use locked parameters: %d\n", ret);
		return ret;
	}
	/* Align to top of second */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_ALIGN_TO_TOW_TP1, 1,
			       1, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set align to second: %d\n", ret);
		return ret;
	}
	/* Set positive polarity (rising edge) */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_POL_TP1, 1,
			       1, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set time pulse polarity: %d\n", ret);
		return ret;
	}
	/* Set time grid */
	ret = gnss_ubx_set_val(dev, UBLOX_CFG_TP_TIMEGRID_TP1, 0,
			       1, GNSS_CONFIG_LAYER_RAM);
	if (ret) {
		pr_err("Failed to set time grid: %d\n", ret);
		return ret;
	}

	return 0;
}

/**
 * @brief Configure PPS output with optimal settings for timing sync
 * @param dev - The device structure
 * @param enable - Enable/disable PPS output
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_init_pps(struct gnss_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	if (dev->device_type == GNSS_DEVICE_UBX_CAPABLE) {
		return gnss_ubx_configure_time_pulse(dev, enable);
	} else {
		/* to implement */
		//gnss_configure_time_pulse_nmea(dev, enable);
	}
	return 0;
}

/**
 * @brief Check if time pulse is currently enabled
 * @param dev - The device structure
 * @return true if enabled, false otherwise
 */
bool gnss_ubx_is_time_pulse_enabled(struct gnss_dev *dev)
{
	uint64_t enabled = 0;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = gnss_ubx_get_val(dev, UBLOX_CFG_TP_TP1_ENA, &enabled,
			       1, GNSS_CONFIG_LAYER_RAM);
	if (ret)
		return ret;

	return (enabled != 0);
}

/**
 * @brief Calculate NMEA checksum for command
 * @param sentence - NMEA sentence without $ and *
 * @return Calculated checksum
 */
static uint8_t gnss_calculate_nmea_checksum(const char *sentence)
{
	uint8_t checksum = 0;
	while (*sentence) {
		checksum ^= *sentence++;
	}
	return checksum;
}

/**
 * @brief Configure baudrate via standard NMEA command for NMEA-only devices
 * @param dev - The device structure
 * @param baudrate - Desired baudrate
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_configure_baudrate_via_nmea(struct gnss_dev *dev,
				     uint32_t baudrate)
{
	char nmea_command[64];
	char sentence_content[48];
	uint8_t checksum;
	int ret;

	if (!dev)
		return -EINVAL;

	/* Build standard baudrate configuration command */
	snprintf(sentence_content, sizeof(sentence_content), "BRC,%lu", baudrate);

	/* Calculate checksum */
	checksum = gnss_calculate_nmea_checksum(sentence_content);

	/* Build complete NMEA command */
	snprintf(nmea_command, sizeof(nmea_command), "$%s*%02X\r\n", sentence_content,
		 checksum);

	/* Send command */
	ret = gnss_write(dev, (uint8_t *)nmea_command, strlen(nmea_command));
	if (ret < 0) {
		pr_warning("Failed to send NMEA baudrate command: %d\n", ret);
		return ret;
	}

	/* Wait for command to take effect */
	no_os_mdelay(1000);

	return 0;
}

/**
 * @brief Enable GPRMC/GNRMC message and disable others for clean timing via NMEA commands
 * @param dev - The device structure
 * @param rate_hz - GPRMC/GNRMC message rate in Hz (typically 1)
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_configure_messages_via_nmea(struct gnss_dev *dev,
				     uint8_t rate_hz)
{
	char nmea_command[64];
	char sentence_content[48];
	uint8_t checksum;
	int ret;
	/* List of message types to disable (base names without constellation prefix) */
	const char *messages_to_disable[] = {"GGA", "VTG", "GSA", "GSV", "GLL", "ZDA"};
	uint16_t num_disable = sizeof(messages_to_disable) / sizeof(
				       messages_to_disable[0]);
	/* All possible constellation prefixes that can appear in NMEA messages */
	const char *constellation_prefixes[] = {"GP", "GN", "GL", "GA", "BD", "QZ"};
	uint16_t num_prefixes = sizeof(constellation_prefixes) / sizeof(
					constellation_prefixes[0]);
	const char *rmc_variants[] = {"GPRMC", "GNRMC", "GLRMC", "GARMC", "BDRMC", "QZRMC", "RMC"};
	int num_rmc_variants = sizeof(rmc_variants) / sizeof(rmc_variants[0]);
	const char *target_rmc = (dev->nmea_constellation_type == NMEA_GPS_ONLY) ?
				 "GPRMC" : "GNRMC";
	/* Enable GPGGA message at lower rate for position fix information */
	const char *target_gga = (dev->nmea_constellation_type == NMEA_GPS_ONLY) ?
				 "GPGGA" : "GNGGA";

	if (!dev)
		return -EINVAL;

	/* Disable unwanted messages for ALL constellation types */
	for (int i = 0; i < num_disable; i++) {
		for (int j = 0; j < num_prefixes; j++) {
			snprintf(sentence_content, sizeof(sentence_content), "MSG,%s%s,0",
				 constellation_prefixes[j], messages_to_disable[i]);
			checksum = gnss_calculate_nmea_checksum(sentence_content);
			snprintf(nmea_command, sizeof(nmea_command), "$%s*%02X\r\n", sentence_content,
				 checksum);

			pr_debug("Disabling %s%s: %s", constellation_prefixes[j],
				 messages_to_disable[i], nmea_command);

			ret = gnss_write(dev, (uint8_t *)nmea_command, strlen(nmea_command));
			if (ret < 0) {
				pr_warning("Failed to disable %s%s message: %d\n",
					   constellation_prefixes[j], messages_to_disable[i], ret);
			}

			no_os_mdelay(50); /* Short delay between commands */
		}
	}

	/* Also disable base message types (without prefix) for compatibility */
	pr_debug("Disabling base message types (without constellation prefix)...\n");
	for (int i = 0; i < num_disable; i++) {
		snprintf(sentence_content, sizeof(sentence_content), "MSG,%s,0",
			 messages_to_disable[i]);
		checksum = gnss_calculate_nmea_checksum(sentence_content);
		snprintf(nmea_command, sizeof(nmea_command), "$%s*%02X\r\n", sentence_content,
			 checksum);

		pr_debug("Disabling base %s: %s", messages_to_disable[i], nmea_command);

		ret = gnss_write(dev, (uint8_t *)nmea_command, strlen(nmea_command));
		if (ret < 0) {
			pr_warning("Failed to disable base %s message: %d\n", messages_to_disable[i],
				   ret);
		}

		no_os_mdelay(50); /* Short delay between commands */
	}

	/* Disable all RMC variants except the one we want to enable */
	pr_debug("Disabling all RMC variants except target type...\n");
	for (int i = 0; i < num_rmc_variants; i++) {
		/* Skip the RMC variant we want to enable */
		if (strcmp(rmc_variants[i], target_rmc) == 0)
			continue;

		snprintf(sentence_content, sizeof(sentence_content), "MSG,%s,0",
			 rmc_variants[i]);
		checksum = gnss_calculate_nmea_checksum(sentence_content);
		snprintf(nmea_command, sizeof(nmea_command), "$%s*%02X\r\n", sentence_content,
			 checksum);

		pr_debug("Disabling %s: %s", rmc_variants[i], nmea_command);

		ret = gnss_write(dev, (uint8_t *)nmea_command, strlen(nmea_command));
		if (ret < 0) {
			pr_debug("Failed to disable %s message: %d (may not be supported)\n",
				 rmc_variants[i], ret);
		}

		no_os_mdelay(50); /* Short delay between commands */
	}

	/* Enable the desired RMC message at specified rate */
	snprintf(sentence_content, sizeof(sentence_content), "MSG,%s,%d", target_rmc,
		 rate_hz);
	checksum = gnss_calculate_nmea_checksum(sentence_content);
	snprintf(nmea_command, sizeof(nmea_command), "$%s*%02X\r\n", sentence_content,
		 checksum);

	ret = gnss_write(dev, (uint8_t *)nmea_command, strlen(nmea_command));
	if (ret < 0) {
		pr_err("Failed to enable %s message: %d\n", target_rmc, ret);
		return ret;
	}

	no_os_mdelay(500); /* Wait for configuration to take effect */

	/* Enable GPGGA at 1Hz (will be read every 10th cycle in software) */
	snprintf(sentence_content, sizeof(sentence_content), "MSG,%s,1", target_gga);
	checksum = gnss_calculate_nmea_checksum(sentence_content);
	snprintf(nmea_command, sizeof(nmea_command), "$%s*%02X\r\n", sentence_content,
		 checksum);

	ret = gnss_write(dev, (uint8_t *)nmea_command, strlen(nmea_command));
	if (ret < 0) {
		pr_warning("Failed to enable %s message: %d (continuing)\n", target_gga, ret);
		/* Non-fatal - continue without position data */
	}

	no_os_mdelay(500); /* Wait for configuration to take effect */

	return 0;
}

/**
 * @brief Configure NMEA-only device for timing operation
 * @param dev - The device structure
 * @param baudrate - Desired baudrate
 * @param message_rate_hz - GPRMC message rate
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_configure_nmea_device(struct gnss_dev *dev,
			       uint32_t baudrate, uint8_t message_rate_hz)
{
	int ret;

	if (!dev)
		return -EINVAL;;

	/* Configure baudrate */
	ret = gnss_configure_baudrate_via_nmea(dev, baudrate);
	if (ret) {
		pr_warning("NMEA baudrate configuration failed: %d (continuing)\n", ret);
		/* Non-fatal - device may already be at correct baudrate */
	}

	/* Configure message output */
	ret = gnss_configure_messages_via_nmea(dev, message_rate_hz);
	if (ret) {
		pr_warning("NMEA message configuration failed: %d (continuing)\n", ret);
		/* Non-fatal - device may have default configuration */
	}

	/* Try to force GPS time synchronization for devices with date issues */
	/* Common GPS time sync commands for various manufacturers */
	const char *time_sync_commands[] = {
		"$PGRMO,,2*75\r\n",          /* Garmin: Reset to factory defaults */
		"$PGRMC1,,,,,,,,W*76\r\n",   /* Garmin: Enable GPS time auto-sync */
		"$PMTK101*32\r\n",           /* MediaTek: Hot restart */
		"$PMTK104*37\r\n",           /* MediaTek: Cold restart */
		"$PSRF103,00,01,00,01*25\r\n" /* SiRF: Enable GPRMC with GPS time */
	};

	for (int i = 0; i < sizeof(time_sync_commands) / sizeof(time_sync_commands[0]);
	     i++) {
		ret = gnss_write(dev, (uint8_t *)time_sync_commands[i],
				 strlen(time_sync_commands[i]));
		if (ret < 0) {
			pr_warning("Failed to send time sync command %d: %d\n", i + 1, ret);
		}
		no_os_mdelay(1000); /* Wait between commands */
	}

	return 0;
}

/**
 * @brief Read NMEA sentence from UART (GPRMC or GPGGA)
 * @param dev - The device structure
 * @param sentence - Output buffer for NMEA sentence
 * @param max_len - Maximum buffer length
 * @param sentence_type - Sentence type to look for ("RMC" or "GGA")
 * @return 0 in case of success, negative error code otherwise
 */
static int gnss_read_nmea_sentence(struct gnss_dev *dev,
				   char *sentence, size_t max_len,
				   const char *sentence_type)
{
	uint8_t byte;
	uint16_t total_read = 0;
	int ret;
	bool found_start = false;
	bool is_target_sentence = false;

	if (!dev || !sentence || max_len == 0)
		return -EINVAL;

	/* Read until we find a complete GPRMC sentence */
	while (total_read < max_len - 1) {
		ret = gnss_read(dev, &byte, 1);
		if (ret <= 0) {
			pr_warning("NMEA read timeout at position %d\n", total_read);
			return -EIO;
		}

		sentence[total_read++] = (char)byte;

		/* Look for sentence start */
		if (!found_start && byte == '$') {
			total_read = 1; /* Reset to just the $ */
			sentence[0] = '$';
			found_start = true;
			continue;
		}

		if (!found_start)
			continue;

		/* Check for end of sentence */
		if (byte == '\n') {
			sentence[total_read] = '\0';

			/* Check if this is the requested sentence type based on constellation type */
			is_target_sentence = false;
			if (strcmp(sentence_type, "RMC") == 0) {
				if (dev->nmea_constellation_type == NMEA_GPS_ONLY) {
					/* GPS-only device: look for $GPRMC */
					if (strncmp(sentence, "$GPRMC", 6) == 0) {
						is_target_sentence = true;
						pr_debug("GPRMC (GPS) sentence received: %s", sentence);
					}
				} else {
					/* GNSS multi-constellation: look for $GNRMC */
					if (strncmp(sentence, "$GNRMC", 6) == 0) {
						is_target_sentence = true;
						pr_debug("GNRMC (GNSS) sentence received: %s", sentence);
					}
				}
			} else if (strcmp(sentence_type, "GGA") == 0) {
				if (dev->nmea_constellation_type == NMEA_GPS_ONLY) {
					/* GPS-only device: look for $GPGGA */
					if (strncmp(sentence, "$GPGGA", 6) == 0) {
						is_target_sentence = true;
						pr_debug("GPGGA (GPS) sentence received: %s", sentence);
					}
				} else {
					/* GNSS multi-constellation: look for $GNGGA */
					if (strncmp(sentence, "$GNGGA", 6) == 0) {
						is_target_sentence = true;
						pr_debug("GNGGA (GNSS) sentence received: %s", sentence);
					}
				}
			}

			if (is_target_sentence) {
				return 0;
			} else {
				/* Not the expected sentence, continue looking */
				found_start = false;
				total_read = 0;
				continue;
			}
		}

		/* Sanity check: prevent reading too much */
		if (total_read > 100) {  /* GPRMC should be ~70 chars */
			pr_warning("NMEA sentence too long, resetting\n");
			found_start = false;
			total_read = 0;
		}
	}

	return -ENODATA;  /* No GPRMC sentence found */
}

/**
 * @brief Parse GPRMC sentence for time and date information
 * @param sentence - Complete GPRMC sentence string
 * @param rmc_time - Output structure for parsed time data
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_parse_gprmc_sentence(const char *sentence,
			      struct gnss_nmea_time *rmc_time)
{
	char *sentence_copy;
	char *fields[15];  /* GPRMC has up to 12+ fields */
	char *token;
	int field_count = 0;
	float time_float;
	size_t len;
	int time_int, date_int;
	int ret = -EINVAL;
	bool data_valid = false;
	char *date_field = NULL;
	int date_field_index = -1;

	if (!sentence || !rmc_time)
		return -EINVAL;

	/* Initialize output structure */
	memset(rmc_time, 0, sizeof(struct gnss_nmea_time));

	/* Verify this is a RMC sentence (either GPRMC or GNRMC) */
	if (strncmp(sentence, "$GPRMC", 6) != 0
	    && strncmp(sentence, "$GNRMC", 6) != 0) {
		pr_debug("Not a RMC sentence: %.10s\n", sentence);
		return -ENODATA;
	}

	/* Create working copy for parsing */
	len = strlen(sentence);
	sentence_copy = no_os_calloc(len + 1, sizeof(char));
	if (!sentence_copy)
		return -ENOMEM;
	strcpy(sentence_copy, sentence);

	/* Split into fields by comma */
	token = strtok(sentence_copy, ",");
	while (token && field_count < 15) {
		fields[field_count++] = token;
		token = strtok(NULL, ",*");  /* Split by comma or asterisk (for checksum) */
	}

	/* Parse GPRMC: $GPRMC,hhmmss.ss,A/V,lat,N/S,lon,E/W,speed,course,ddmmyy,mag_var,E/W*hh */
	/* Fields: 0=GPRMC, 1=time, 2=status, 9=date (DDMMYY) */
	if (field_count >= 10) {
		/* Check status field (A=Active, V=Void) */
		data_valid = (fields[2] && fields[2][0] == 'A');

		/* Parse time (HHMMSS.sss) from field 1 */
		if (fields[1] && strlen(fields[1]) >= 6) {
			time_float = atof(fields[1]);
			time_int = (int)time_float;
			rmc_time->hour = time_int / 10000;
			rmc_time->minute = (time_int % 10000) / 100;
			rmc_time->second = time_int % 100;
			rmc_time->milliseconds = (uint16_t)((time_float - time_int) * 1000);
			rmc_time->time_valid = data_valid;

			pr_debug("Parsed time: %02d:%02d:%02d.%03d (valid=%s)\n",
				 rmc_time->hour, rmc_time->minute, rmc_time->second,
				 rmc_time->milliseconds, data_valid ? "YES" : "NO");
		}

		/* Parse date (DDMMYY) - try field[9] first, then field[8] if course is missing */
		/* Try field[9] first (standard position) */
		if (fields[9] && strlen(fields[9]) >= 6
		    && strspn(fields[9], "0123456789") == strlen(fields[9])) {
			date_field = fields[9];
			date_field_index = 9;
		}
		/* If field[9] doesn't look like a date, try field[8] (course field missing) */
		else if (fields[8] && strlen(fields[8]) >= 6
			 && strspn(fields[8], "0123456789") == strlen(fields[8])) {
			date_field = fields[8];
			date_field_index = 8;
		}

		if (date_field) {
			date_int = atoi(date_field);

			rmc_time->day = date_int / 10000;
			rmc_time->month = (date_int % 10000) / 100;
			rmc_time->year = 2000 + (date_int % 100);  /* Convert YY to YYYY */
			rmc_time->date_valid = data_valid && (rmc_time->year > 0 && rmc_time->month > 0
							      && rmc_time->day > 0);

			/* GPS Week Rollover Correction - only when GPS fix is valid but date is wrong */
			if (data_valid && rmc_time->date_valid && rmc_time->year < 2020) {
				uint16_t original_year = rmc_time->year;

				/* GPS week rollover period is 1024 weeks ≈ 19.69 years */
				/* Second major rollover was April 6, 2019 - many devices report dates ~19 years behind */
				rmc_time->year += 19;  /* Add one GPS week rollover period */

				/* If still too old, add another rollover period */
				if (rmc_time->year < 2020) {
					rmc_time->year += 19;
				}
			}

			pr_debug("Parsed date: %04d-%02d-%02d (valid=%s)\n",
				 rmc_time->year, rmc_time->month, rmc_time->day,
				 rmc_time->date_valid ? "YES" : "NO");
		}

		/* Calculate Unix epoch if both time and date are valid */
		if (rmc_time->time_valid && rmc_time->date_valid && rmc_time->year >= 1970) {
			/* Simplified Unix epoch calculation */
			uint32_t days = (rmc_time->year - 1970) * 365 + (rmc_time->year - 1970) / 4;
			uint16_t month_days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
			if (rmc_time->month > 0 && rmc_time->month <= 12)
				days += month_days[rmc_time->month - 1] + rmc_time->day - 1;

			rmc_time->unix_epoch = days * 86400 + rmc_time->hour * 3600 +
					       rmc_time->minute * 60 + rmc_time->second;

			pr_debug("Calculated Unix epoch: %lu\n", rmc_time->unix_epoch);
		}

		ret = 0;
	} else {
		pr_warning("Insufficient GPRMC fields: %d (need 10)\n", field_count);
		ret = -EINVAL;
	}

	no_os_free(sentence_copy);
	return ret;
}

/**
 * @brief Get timing data from NMEA-only device via GPRMC parsing
 * @param dev - The device structure
 * @param rmc_time - Output structure for time data
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_get_nmea_timing_data(struct gnss_dev *dev,
			      struct gnss_nmea_time *rmc_time)
{
	char nmea_sentence[128];
	int ret;
	int attempts = 0;
	const int max_attempts = 5;  /* Try up to 5 sentences to find GPRMC */

	if (!dev || !rmc_time)
		return -EINVAL;

	pr_debug("Getting NMEA timing data via GPRMC parsing\n");

	/* Read NMEA sentences until we get GPRMC */
	while (attempts < max_attempts) {
		ret = gnss_read_nmea_sentence(dev, nmea_sentence, sizeof(nmea_sentence), "RMC");
		if (ret == 0) {
			/* Parse the GPRMC sentence */
			ret = gnss_parse_gprmc_sentence(nmea_sentence, rmc_time);
			if (ret == 0) {
				pr_debug("NMEA timing data extracted successfully\n");
				return 0;
			}
		}

		attempts++;
		no_os_mdelay(100);  /* Small delay between attempts */
	}

	pr_warning("Failed to get valid GPRMC data after %d attempts\n", max_attempts);
	return -ENODATA;
}

/**
 * @brief Get position data from NMEA-only device via GPGGA parsing
 * @param dev - The device structure
 * @param position_data - Output structure for position data
 * @return 0 in case of success, negative error code otherwise
 */
static int gnss_get_nmea_position_data_internal(struct gnss_dev *dev,
		struct gnss_nmea_position *position_data)
{
	char nmea_sentence[128];
	int ret;
	int attempts = 0;
	const int max_attempts = 3;  /* Try up to 3 sentences to find GPGGA */

	if (!dev || !position_data)
		return -EINVAL;

	pr_debug("Getting NMEA position data via GPGGA parsing\n");

	/* Read NMEA sentences until we get GPGGA */
	while (attempts < max_attempts) {
		ret = gnss_read_nmea_sentence(dev, nmea_sentence, sizeof(nmea_sentence), "GGA");
		if (ret == 0) {
			/* Parse the GPGGA sentence */
			ret = gnss_parse_gpgga_sentence(nmea_sentence, position_data);
			if (ret == 0) {
				pr_debug("NMEA position data extracted successfully\n");
				return 0;
			}
		}

		attempts++;
		no_os_mdelay(100);  /* Small delay between attempts */
	}

	pr_debug("Failed to get valid GPGGA data after %d attempts\n", max_attempts);
	return -ENODATA;
}

/* Unified Timing API (UBX/NMEA Abstraction) */

/**
 * @brief Refresh timing data from device (unified API for both UBX and NMEA devices)
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_refresh_timing_data(struct gnss_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	switch (dev->device_type) {
	case GNSS_DEVICE_UBX_CAPABLE:
		/* For UBX devices, refresh NAV-PVT if validity check not needed NAV-TIMEUTC data
		can be used*/
		pr_debug("Refreshing timing data via UBX protocol\n");

		/* Poll navigation data for position and time */
		ret = gnss_ubx_poll_nav_pvt(dev, &dev->nav_data);
		if (ret) {
			pr_warning("Failed to poll NAV-PVT data: %d\n", ret);
			return ret;
		}

		pr_debug("UBX timing data refreshed successfully\n");
		break;

	case GNSS_DEVICE_NMEA_ONLY:
		/* For NMEA devices, read and cache timing data for consistent access */
		pr_debug("Refreshing timing data via NMEA protocol\n");

		/* Read and parse NMEA sentence once per measurement cycle */
		ret = gnss_get_nmea_timing_data(dev, &dev->nmea_timing_cache);
		if (ret) {
			pr_warning("Failed to refresh NMEA timing data: %d\n", ret);
			return ret;
		}

		pr_debug("NMEA timing data refreshed and cached successfully\n");

		/* Refresh position data every 10th cycle (10:1 ratio) */
		dev->position_refresh_counter++;
		if (dev->position_refresh_counter >= 10) {
			pr_debug("Refreshing NMEA position data (every 10th cycle)\n");

			ret = gnss_get_nmea_position_data_internal(dev, &dev->nmea_position_cache);
			if (ret) {
				pr_debug("Failed to refresh NMEA position data: %d (continuing)\n", ret);
				/* Non-fatal - timing data is still valid */
			} else {
				pr_debug("NMEA position data refreshed and cached successfully\n");
			}
			/* Reset counter */
			dev->position_refresh_counter = 0;
		}
		break;

	default:
		pr_err("Unknown device type: %d\n", dev->device_type);
		return -EINVAL;
	}

	return ret;
}

/**
 * @brief Get comprehensive timing information (unified API for both device types)
 * @param dev - The device structure
 * @param precise_time - Structure to store precise time data
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_get_unified_timing(struct gnss_dev *dev,
			    struct gnss_precise_time *precise_time)
{
	int ret;

	if (!dev || !precise_time)
		return -EINVAL;

	/* Initialize output structure */
	memset(precise_time, 0, sizeof(struct gnss_precise_time));

	switch (dev->device_type) {
	case GNSS_DEVICE_UBX_CAPABLE:
		pr_debug("Getting unified timing from UBX device\n");

		/* Use existing precise time function for UBX devices */
		ret = gnss_ubx_get_precise_time(dev, precise_time);
		if (ret) {
			pr_warning("Failed to get UBX precise time: %d\n", ret);
			return ret;
		}

		pr_warning("UBX timing: %04d-%02d-%02d %02d:%02d:%02d, nano=%ld, accuracy=%lu ns\n",
			   precise_time->year, precise_time->month, precise_time->day,
			   precise_time->hour, precise_time->minute, precise_time->second,
			   precise_time->nanoseconds, precise_time->time_accuracy);
		break;

	case GNSS_DEVICE_NMEA_ONLY:
		pr_debug("Getting unified timing from NMEA device (using cached data)\n");

		/* Use cached NMEA timing data (refreshed by ublox_gnss_refresh_timing_data) */
		/* Convert cached NMEA time to unified precise time structure */
		precise_time->unix_epoch = dev->nmea_timing_cache.unix_epoch;
		precise_time->nanoseconds = (int32_t)(dev->nmea_timing_cache.milliseconds *
						      1000000);  /* Convert ms to ns */
		precise_time->time_accuracy =
			1000000000;  /* 1 second accuracy estimate for NMEA */
		precise_time->time_valid = dev->nmea_timing_cache.time_valid;
		precise_time->time_fully_resolved = dev->nmea_timing_cache.time_valid
						    && dev->nmea_timing_cache.date_valid;
		precise_time->date_valid = dev->nmea_timing_cache.date_valid;
		precise_time->year = dev->nmea_timing_cache.year;
		precise_time->month = dev->nmea_timing_cache.month;
		precise_time->day = dev->nmea_timing_cache.day;
		precise_time->hour = dev->nmea_timing_cache.hour;
		precise_time->minute = dev->nmea_timing_cache.minute;
		precise_time->second = dev->nmea_timing_cache.second;

		pr_debug("NMEA timing (cached): %04d-%02d-%02d %02d:%02d:%02d, ms=%d, epoch=%lu\n",
			 dev->nmea_timing_cache.year, dev->nmea_timing_cache.month,
			 dev->nmea_timing_cache.day,
			 dev->nmea_timing_cache.hour, dev->nmea_timing_cache.minute,
			 dev->nmea_timing_cache.second,
			 dev->nmea_timing_cache.milliseconds, dev->nmea_timing_cache.unix_epoch);
		break;

	default:
		pr_err("Unknown device type: %d\n", dev->device_type);
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Check if timing data is valid (unified API for both device types)
 * @param dev - The device structure
 * @return true if timing data is valid, false otherwise
 */
bool gnss_is_timing_valid(struct gnss_dev *dev)
{
	if (!dev)
		return -EINVAL;

	switch (dev->device_type) {
	case GNSS_DEVICE_UBX_CAPABLE:
		/* Use existing UBX time validity check */
		return gnss_ubx_get_time_valid(dev);

	case GNSS_DEVICE_NMEA_ONLY:
		/* For NMEA devices, check cached timing data validity */
		return dev->nmea_timing_cache.time_valid;

	default:
		pr_err("Unknown device type: %d\n", dev->device_type);
		return -ENODEV;
	}
}

/**
 * @brief Get Unix epoch time with best available precision (unified API)
 * @param dev - The device structure
 * @param fractional_seconds - Pointer to store fractional seconds
 * @return Unix epoch time in seconds, negative error code otherwise
 */
uint32_t gnss_get_unix_epoch_unified(struct gnss_dev *dev,
				     uint32_t *fractional_seconds)
{
	uint32_t epoch_time = 0;

	if (!dev || !fractional_seconds)
		return -EINVAL;

	*fractional_seconds = 0;

	switch (dev->device_type) {
	case GNSS_DEVICE_UBX_CAPABLE:
		pr_debug("Getting Unix epoch from UBX device\n");

		/* Use existing UBX function for microsecond precision */
		epoch_time = gnss_ubx_get_unix_epoch_with_micros(dev, fractional_seconds);

		pr_debug("UBX epoch: %lu.%06lu\n", epoch_time, *fractional_seconds);
		break;

	case GNSS_DEVICE_NMEA_ONLY:
		pr_debug("Getting Unix epoch from NMEA device (using cached data)\n");

		/* Use cached NMEA timing data */
		if (dev->nmea_timing_cache.time_valid && dev->nmea_timing_cache.date_valid) {
			epoch_time = dev->nmea_timing_cache.unix_epoch;
			/* Convert milliseconds to microseconds for consistency with UBX API */
			*fractional_seconds = (uint32_t)(dev->nmea_timing_cache.milliseconds * 1000);

			pr_debug("NMEA epoch (cached): %lu.%03d000 (converted from ms)\n",
				 epoch_time, dev->nmea_timing_cache.milliseconds);
		} else {
			pr_warning("Cached NMEA timing data is not valid for epoch calculation\n");
			epoch_time = 0;
		}
		break;

	default:
		pr_err("Unknown device type: %d\n", dev->device_type);
		return -ENODEV;
	}

	return epoch_time;
}

/**
 * @brief Parse GPGGA sentence for position and fix information
 * @param sentence - Complete GPGGA sentence string
 * @param position_data - Output structure for parsed position data
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_parse_gpgga_sentence(const char *sentence,
			      struct gnss_nmea_position *position_data)
{
	int ret;
	char *sentence_copy;
	char *fields[15];  /* GPGGA has up to 14+ fields */
	char *token;
	int field_count = 0;
	size_t len;

	if (!sentence || !position_data)
		return -EINVAL;

	/* Initialize output structure */
	memset(position_data, 0, sizeof(struct gnss_nmea_position));

	/* Verify this is a GGA sentence (either GPGGA or GNGGA) */
	if (strncmp(sentence, "$GPGGA", 6) != 0
	    && strncmp(sentence, "$GNGGA", 6) != 0) {
		pr_debug("Not a GGA sentence: %.10s\n", sentence);
		return -ENODATA;
	}

	/* Create working copy for parsing */
	len = strlen(sentence);
	sentence_copy = no_os_calloc(len + 1, sizeof(char));
	if (!sentence_copy) {
		return -ENOMEM;
	}
	strcpy(sentence_copy, sentence);

	/* Split into fields by comma */
	token = strtok(sentence_copy, ",");
	while (token && field_count < 15) {
		fields[field_count++] = token;
		token = strtok(NULL, ",*");  /* Split by comma or asterisk (for checksum) */
	}

	pr_debug("GPGGA field count: %d\n", field_count);
	for (int i = 0; i < field_count && i < 12; i++) {
		pr_debug("  Field[%d]: '%s'\n", i, fields[i] ? fields[i] : "(null)");
	}

	/* Parse GPGGA: $GPGGA,hhmmss.ss,ddmm.mmmm,N/S,dddmm.mmmm,E/W,q,nn,h.h,a.a,M,g.g,M,d.d,ssss*hh */
	/* Fields: 0=GPGGA, 6=fix_quality, 7=satellites_used, 8=hdop, 9=altitude */
	if (field_count >= 10) {
		/* Parse fix quality from field 6 */
		if (fields[6] && strlen(fields[6]) > 0) {
			position_data->fix_quality = (uint8_t)atoi(fields[6]);
			position_data->fix_valid = (position_data->fix_quality > 0);
		}

		/* Parse number of satellites from field 7 */
		if (fields[7] && strlen(fields[7]) > 0) {
			position_data->satellites_used = (uint8_t)atoi(fields[7]);
		}

		/* Parse HDOP from field 8 */
		if (fields[8] && strlen(fields[8]) > 0) {
			position_data->hdop = atof(fields[8]);
		}

		/* Parse altitude from field 9 */
		if (fields[9] && strlen(fields[9]) > 0) {
			position_data->altitude = atof(fields[9]);
		}

		/* Parse latitude from fields 2,3 if available */
		if (fields[2] && fields[3] && strlen(fields[2]) > 0 && strlen(fields[3]) > 0) {
			float lat_deg, lat_min;
			sscanf(fields[2], "%2f%f", &lat_deg, &lat_min);
			position_data->latitude = lat_deg + lat_min / 60.0;
			if (fields[3][0] == 'S')
				position_data->latitude = -position_data->latitude;
		}

		/* Parse longitude from fields 4,5 if available */
		if (fields[4] && fields[5] && strlen(fields[4]) > 0 && strlen(fields[5]) > 0) {
			float lon_deg, lon_min;
			sscanf(fields[4], "%3f%f", &lon_deg, &lon_min);
			position_data->longitude = lon_deg + lon_min / 60.0;
			if (fields[5][0] == 'W')
				position_data->longitude = -position_data->longitude;
		}

		/* Position is valid if we have fix quality > 0 and coordinate data */
		position_data->position_valid = position_data->fix_valid &&
						(position_data->latitude != 0.0 || position_data->longitude != 0.0);

		pr_debug("Parsed GPGGA: fix=%d, sats=%d, hdop=%.2f, alt=%.2f, pos_valid=%s\n",
			 position_data->fix_quality, position_data->satellites_used,
			 position_data->hdop, position_data->altitude,
			 position_data->position_valid ? "YES" : "NO");

		ret = 0;
	} else {
		pr_warning("Insufficient GPGGA fields: %d (need 10)\n", field_count);
		ret = -EINVAL;
	}

	no_os_free(sentence_copy);
	return ret;
}

/**
 * @brief Get GPS fix quality and position data (NMEA-only devices)
 * @param dev - The device structure
 * @param position_data - Output structure for position data
 * @return 0 in case of success, negative error code otherwise
 */
int gnss_get_nmea_position_data(struct gnss_dev *dev,
				struct gnss_nmea_position *position_data)
{
	if (!dev || !position_data)
		return -EINVAL;

	if (dev->device_type != GNSS_DEVICE_NMEA_ONLY)
		return -ENOTSUP;

	/* Return cached position data */
	memcpy(position_data, &dev->nmea_position_cache,
	       sizeof(struct gnss_nmea_position));

	return 0;
}
