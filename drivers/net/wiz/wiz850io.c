/***************************************************************************//**
*   @file   wiz55.c
*   @brief  Implementation of WIZ Driver
*   @author Alisa-Dariana Roman (alisa.roman@analog.com)
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#include <errno.h>

#include "wiz850io.h"

/**
 * @brief Write data to a W5500 register
 * @param dev - The device descriptor
 * @param block - The block select bits (common registers, socket registers, etc.)
 * @param addr - The register address
 * @param data - The data buffer to write
 * @param len - The data length
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_reg_write(struct wiz850io_dev *dev, uint8_t block,
		       uint16_t addr, const uint8_t *data, uint16_t len)
{
	int ret;
	uint8_t spi_tx[3 + len]; // Address(2) + Control(1) + Data(len)

	// Prepare the address phase (16 bits, MSB first)
	spi_tx[0] = (addr >> 8) & 0xFF;
	spi_tx[1] = addr & 0xFF;

	// Control phase: Block select, Write operation (1), Variable data length (00)
	spi_tx[2] = (block << 3) | 0x04; // RWB=1, OM=00

	// Copy data to transmit buffer
	memcpy(&spi_tx[3], data, len);

	// Define SPI transaction
	struct no_os_spi_msg xfer = {
	    .tx_buff = spi_tx,
	    .rx_buff = NULL,
	    .bytes_number = 3 + len,
	    .cs_change = 1,
	};

	// Execute SPI transaction
	ret = no_os_spi_transfer(dev->spi, &xfer, 1);

	return ret;
}

/**
 * @brief Read data from a W5500 register
 * @param dev - The device descriptor
 * @param block - The block select bits (common registers, socket registers, etc.)
 * @param addr - The register address
 * @param data - The buffer to store read data
 * @param len - The data length to read
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_reg_read(struct wiz850io_dev *dev, uint8_t block,
		      uint16_t addr, uint8_t *data, uint16_t len)
{
	int ret;
	uint8_t spi_tx[3];
	uint8_t spi_rx[3 + len];

	// Address bytes
	spi_tx[0] = (addr >> 8) & 0xFF;
	spi_tx[1] = addr & 0xFF;

	// Control byte: Block select in bits 7-3, read=0 in bit 2
	spi_tx[2] = (block << 3) & 0xF8; // Clear bits 2-0

	printf("READ: Addr=0x%04x Block=%d TX[0]=0x%02x TX[1]=0x%02x TX[2]=0x%02x\n",
	       addr, block, spi_tx[0], spi_tx[1], spi_tx[2]);

	// Define SPI transaction
	struct no_os_spi_msg xfer = {
	    .tx_buff = spi_tx,
	    .rx_buff = spi_rx,
	    .bytes_number = 3 + len,
	    .cs_change = 1,
	};

	// Execute SPI transaction
	ret = no_os_spi_transfer(dev->spi, &xfer, 1);
	if (ret)
	{
		printf("SPI transfer failed with error %d\n", ret);
		return ret;
	}

	printf("READ: RX[0]=0x%02x RX[1]=0x%02x RX[2]=0x%02x ",
	       spi_rx[0], spi_rx[1], spi_rx[2]);

	// Print received data bytes
	printf("Data=");
	for (int i = 0; i < len; i++)
	{
		printf("0x%02x ", spi_rx[3 + i]);
	}
	printf("\n");

	// Copy received data to output buffer (skip address and control bytes)
	memcpy(data, &spi_rx[3], len);

	return 0;
}

/**
 * @brief Set MAC address
 * @param dev - The device descriptor
 * @param mac - 6-byte MAC address array
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_set_mac(struct wiz850io_dev *dev, const uint8_t mac[6])
{
	return wiz850io_reg_write(dev, WIZ850IO_COMMON_REG, WIZ850IO_SHAR, mac, 6);
}

/**
 * @brief Set IP address
 * @param dev - The device descriptor
 * @param ip - 4-byte IP address array
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_set_ip(struct wiz850io_dev *dev, const uint8_t ip[4])
{
	return wiz850io_reg_write(dev, WIZ850IO_COMMON_REG, WIZ850IO_SIPR, ip, 4);
}

/**
 * @brief Set subnet mask
 * @param dev - The device descriptor
 * @param subnet - 4-byte subnet mask array
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_set_subnet(struct wiz850io_dev *dev, const uint8_t subnet[4])
{
	return wiz850io_reg_write(dev, WIZ850IO_COMMON_REG, WIZ850IO_SUBR, subnet, 4);
}

/**
 * @brief Set gateway address
 * @param dev - The device descriptor
 * @param gateway - 4-byte gateway address array
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_set_gateway(struct wiz850io_dev *dev, const uint8_t gateway[4])
{
	return wiz850io_reg_write(dev, WIZ850IO_COMMON_REG, WIZ850IO_GAR, gateway, 4);
}

/**
 * @brief Get MAC address
 * @param dev - The device descriptor
 * @param mac - Buffer to store the 6-byte MAC address
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_get_mac(struct wiz850io_dev *dev, uint8_t mac[6])
{
	return wiz850io_reg_read(dev, WIZ850IO_COMMON_REG, WIZ850IO_SHAR, mac, 6);
}

/**
 * @brief Get IP address
 * @param dev - The device descriptor
 * @param ip - Buffer to store the 4-byte IP address
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_get_ip(struct wiz850io_dev *dev, uint8_t ip[4])
{
	return wiz850io_reg_read(dev, WIZ850IO_COMMON_REG, WIZ850IO_SIPR, ip, 4);
}

/**
 * @brief Get subnet mask
 * @param dev - The device descriptor
 * @param subnet - Buffer to store the 4-byte subnet mask
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_get_subnet(struct wiz850io_dev *dev, uint8_t subnet[4])
{
	return wiz850io_reg_read(dev, WIZ850IO_COMMON_REG, WIZ850IO_SUBR, subnet, 4);
}

/**
 * @brief Get gateway address
 * @param dev - The device descriptor
 * @param gateway - Buffer to store the 4-byte gateway address
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_get_gateway(struct wiz850io_dev *dev, uint8_t gateway[4])
{
	return wiz850io_reg_read(dev, WIZ850IO_COMMON_REG, WIZ850IO_GAR, gateway, 4);
}

/**
 * @brief Setup the W5500 chip with basic configuration
 * @param dev - The device descriptor
 * @param config - Network configuration parameters
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_setup(struct wiz850io_dev *dev)
{
	int ret;
	uint8_t ver;
	uint8_t read_mac[6];
	uint8_t read_ip[4];
	uint8_t read_subnet[4];
	uint8_t read_gateway[4];

	ret = wiz850io_reg_read(dev, WIZ850IO_COMMON_REG, 0x0039, &ver, 1);
	if (ret)
		return ret;

	if (ver != 0x04)
		return -EINVAL;

	// Software reset
	uint8_t mr = 0x80; // RST bit
	ret = wiz850io_reg_write(dev, WIZ850IO_COMMON_REG, 0x0000, &mr, 1);
	if (ret)
		return ret;

	no_os_mdelay(5);

	ret = wiz850io_reg_read(dev, WIZ850IO_COMMON_REG, 0x0000, &mr, 1);
	if (ret || (mr & 0x80))
		return ret ? ret : -ETIMEDOUT;

	ret = wiz850io_set_mac(dev, dev->mac_addr);
	if (ret)
		return ret;

	ret = wiz850io_set_ip(dev, dev->ip_addr);
	if (ret)
		return ret;

	ret = wiz850io_set_subnet(dev, dev->subnet_mask);
	if (ret)
		return ret;

	ret = wiz850io_set_gateway(dev, dev->gateway);
	if (ret)
		return ret;

	ret = wiz850io_get_mac(dev, read_mac);
	if (ret)
		return ret;
	if (memcmp(dev->mac_addr, read_mac, 6) != 0)
		return -EIO;

	ret = wiz850io_get_ip(dev, read_ip);
	if (ret)
		return ret;
	if (memcmp(dev->ip_addr, read_ip, 4) != 0)
		return -EIO;

	ret = wiz850io_get_subnet(dev, read_subnet);
	if (ret)
		return ret;
	if (memcmp(dev->subnet_mask, read_subnet, 4) != 0)
		return -EIO;

	ret = wiz850io_get_gateway(dev, read_gateway);
	if (ret)
		return ret;
	if (memcmp(dev->gateway, read_gateway, 4) != 0)
		return -EIO;

	return 0;
}

/**
 * @brief Initialize the device
 * @param device - the device descriptor to be initialized
 * @param init_param - the device's parameter
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_init(struct wiz850io_dev **device,
		  struct wiz850io_init_param *init_param)
{
	struct wiz850io_dev *dev;
	int ret;

	dev = (struct wiz850io_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	// ret = no_os_gpio_get(&dev->gpio_reset, &init_param->gpio_reset);
	// if (ret)
	//     goto error_1;

	ret = no_os_spi_init(&dev->spi, &init_param->spi_init);
	if (ret)
		goto error_2;

	memcpy(dev->mac_addr, init_param->mac_addr, 6);
	memcpy(dev->ip_addr, init_param->ip_addr, 4);
	memcpy(dev->subnet_mask, init_param->subnet_mask, 4);
	memcpy(dev->gateway, init_param->gateway, 4);

	ret = wiz850io_setup(dev);
	if (ret)
		goto error_3;

	*device = dev;

	return 0;

error_3:
	no_os_spi_remove(dev->spi);
error_2:
// no_os_gpio_remove(dev->gpio_reset);
error_1:
	free(dev);

	return ret;
}

/**
 * @brief Free a device descriptor
 * @param dev - the device descriptor to be removed.
 * @return 0 in case of success, negative error code otherwise
 */
int wiz850io_remove(struct wiz850io_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi);
	ret |= no_os_gpio_remove(dev->gpio_reset);

	free(dev);

	return ret;
}
