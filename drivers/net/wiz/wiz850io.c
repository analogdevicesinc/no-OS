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
	uint8_t spi_tx[3 + len];

	spi_tx[0] = WIZ850IO_ADDR_HIGH(addr);
	spi_tx[1] = WIZ850IO_ADDR_LOW(addr);

	spi_tx[2] = WIZ850IO_BSB(block) | WIZ850IO_RWB_WRITE | WIZ850IO_OM_VDM;

	memcpy(&spi_tx[3], data, len);

	struct no_os_spi_msg xfer = {
	    .tx_buff = spi_tx,
	    .rx_buff = NULL,
	    .bytes_number = 3 + len,
	    .cs_change = 1,
	};

	return no_os_spi_transfer(dev->spi, &xfer, 1);
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

	spi_tx[0] = WIZ850IO_ADDR_HIGH(addr);
	spi_tx[1] = WIZ850IO_ADDR_LOW(addr);

	spi_tx[2] = WIZ850IO_BSB(block) | WIZ850IO_RWB_READ | WIZ850IO_OM_VDM;

	printf("READ: Addr=0x%04x Block=%d TX[0]=0x%02x TX[1]=0x%02x TX[2]=0x%02x\n",
	       addr, block, spi_tx[0], spi_tx[1], spi_tx[2]);

	struct no_os_spi_msg xfer = {
	    .tx_buff = spi_tx,
	    .rx_buff = spi_rx,
	    .bytes_number = 3 + len,
	    .cs_change = 1,
	};

	ret = no_os_spi_transfer(dev->spi, &xfer, 1);
	if (ret)
	{
		printf("SPI transfer failed with error %d\n", ret);
		return ret;
	}

	printf("READ: RX[0]=0x%02x RX[1]=0x%02x RX[2]=0x%02x ",
	       spi_rx[0], spi_rx[1], spi_rx[2]);

	printf("Data=");
	for (int i = 0; i < len; i++)
	{
		printf("0x%02x ", spi_rx[3 + i]);
	}
	printf("\n");

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
 * @brief Get socket status
 * @param dev - Device descriptor
 * @param sock_num - Socket number (0-7)
 * @param status - Pointer to store status value
 * @return 0 on success, negative error code otherwise
 */
int wiz850io_socket_status(struct wiz850io_dev *dev, uint8_t sock_num,
			   uint8_t *status)
{
	if (sock_num >= 8)
		return -EINVAL;

	return wiz850io_reg_read(dev, WIZ850IO_SOCKET0_REG + sock_num * 4,
				 WIZ850IO_Sn_SR(sock_num), status, 1);
}

/**
 * @brief Initialize socket with specific protocol
 * @param dev - Device descriptor
 * @param sock_num - Socket number (0-7)
 * @param protocol - Protocol (TCP, UDP, MACRAW)
 * @param port - Local port number
 * @return 0 on success, negative error code otherwise
 */
int wiz850io_socket_init(struct wiz850io_dev *dev, uint8_t sock_num,
			 uint8_t protocol, uint16_t port)
{
	int ret;
	uint8_t cmd;
	uint8_t status;
	uint8_t mode = protocol; // WIZ850IO_Sn_MR_TCP, WIZ850IO_Sn_MR_UDP, etc.
	uint8_t port_buf[2];

	if (sock_num > WIZ850IO_MAX_SOCK_NUMBER)
		return -EINVAL;

	ret = wiz850io_reg_write(dev, WIZ850IO_SOCKET0_REG + sock_num * 4,
				 WIZ850IO_Sn_MR(sock_num), &mode, 1);
	if (ret)
		return ret;

	port_buf[0] = (port >> 8) & 0xFF;
	port_buf[1] = port & 0xFF;
	ret = wiz850io_reg_write(dev, WIZ850IO_SOCKET0_REG + sock_num * 4,
				 WIZ850IO_Sn_PORT(sock_num), port_buf, 2);
	if (ret)
		return ret;

	cmd = WIZ850IO_Sn_CR_OPEN;
	ret = wiz850io_reg_write(dev, WIZ850IO_SOCKET0_REG + sock_num * 4,
				 WIZ850IO_Sn_CR(sock_num), &cmd, 1);
	if (ret)
		return ret;

	do
	{
		ret = wiz850io_socket_status(dev, sock_num, &status);
		if (ret)
			return ret;
	} while (status != WIZ850IO_Sn_SR_INIT && status != WIZ850IO_Sn_SR_UDP);

	return 0;
}

/**
 * @brief Closes a socket
 * @param dev - Device descriptor
 * @param sock_num - Socket number (0-7)
 * @return 0 on success, negative error code otherwise
 */
int wiz850io_socket_close(struct wiz850io_dev *dev, uint8_t sock_num)
{
	int ret;
	uint8_t cmd;
	uint8_t status;

	if (sock_num > WIZ850IO_MAX_SOCK_NUMBER)
		return -EINVAL;

	cmd = WIZ850IO_Sn_CR_CLOSE;
	ret = wiz850io_reg_write(dev, WIZ850IO_SOCKET0_REG + sock_num * 4,
				 WIZ850IO_Sn_CR(sock_num), &cmd, 1);
	if (ret)
		return ret;

	do
	{
		ret = wiz850io_socket_status(dev, sock_num, &status);
		if (ret)
			return ret;
	} while (status != WIZ850IO_Sn_SR_CLOSED);

	return 0;
}

/**
 * @brief Start listening on TCP socket
 * @param dev - Device descriptor
 * @param sock_num - Socket number (0-7)
 * @return 0 on success, negative error code otherwise
 */
int wiz850io_socket_listen(struct wiz850io_dev *dev, uint8_t sock_num)
{
	int ret;
	uint8_t cmd;
	uint8_t status;

	if (sock_num > WIZ850IO_MAX_SOCK_NUMBER)
		return -EINVAL;

	ret = wiz850io_socket_status(dev, sock_num, &status);
	if (ret)
		return ret;

	if (status != WIZ850IO_Sn_SR_INIT)
		return -ENODEV;

	cmd = WIZ850IO_Sn_CR_LISTEN;
	ret = wiz850io_reg_write(dev, WIZ850IO_SOCKET0_REG + sock_num * 4,
				 WIZ850IO_Sn_CR(sock_num), &cmd, 1);
	if (ret)
		return ret;

	ret = wiz850io_socket_status(dev, sock_num, &status);
	if (ret)
		return ret;

	if (status != WIZ850IO_Sn_SR_LISTEN)
		return -EIO;

	return 0;
}

int test_socket(struct wiz850io_dev *dev)
{
	int ret;
	uint8_t status;
	uint8_t sock_num = 0;
	uint8_t port_num = 80;

	ret = wiz850io_socket_init(dev, sock_num, WIZ850IO_Sn_MR_TCP, port_num);
	if (ret)
	{
		printf("Socket init failed: %d\n", ret);
		return ret;
	}

	ret = wiz850io_socket_listen(dev, sock_num);
	if (ret)
	{
		printf("Socket listen failed: %d\n", ret);
		return ret;
	}

	ret = wiz850io_socket_status(dev, sock_num, &status);
	if (ret)
	{
		printf("Get status failed: %d\n", ret);
		return ret;
	}

	printf("Socket status: 0x%02X\n", status);

	return wiz850io_socket_close(dev, sock_num);
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

	ret = wiz850io_reg_read(dev, WIZ850IO_COMMON_REG, WIZ850IO_VERSIONR, &ver, 1);
	if (ret)
		return ret;

	if (ver != WIZ850IO_CHIP_VERSION)
		return -EINVAL;

	uint8_t mr = WIZ850IO_MR_RST;
	ret = wiz850io_reg_write(dev, WIZ850IO_COMMON_REG, WIZ850IO_MR, &mr, 1);
	if (ret)
		return ret;

	no_os_mdelay(5);

	ret = wiz850io_reg_read(dev, WIZ850IO_COMMON_REG, WIZ850IO_MR, &mr, 1);
	if (ret || (mr & WIZ850IO_MR_RST))
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

	uint8_t phycfgr;
	ret = wiz850io_reg_read(dev, WIZ850IO_COMMON_REG, WIZ850IO_PHYCFGR, &phycfgr, 1);
	if (ret)
		return ret;

	if (!(phycfgr & WIZ850IO_PHYCFGR_LNK))
		return -ENETDOWN;

	printf("AAA phy something correct\n");

	ret = test_socket(dev);
	if (ret) {
		printf("test_socket failed with %d\n", ret);
		return ret;
	}

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
