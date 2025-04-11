/***************************************************************************//**
*   @file   w5500.c
*   @brief  Implementation of W5500 Driver
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

#include "w5500.h"

#include <errno.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_delay.h"

/***************************************************************************//**
 * @brief Write data to a W5500 register
 *
 * @param dev   - The device descriptor
 * @param block - The block select bits (common registers, socket registers, etc.)
 * @param addr  - The register address
 * @param data  - The data buffer to write
 * @param len   - The data length
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_reg_write(struct w5500_dev *dev, uint8_t block,
		    uint16_t addr, const uint8_t *data, uint16_t len)
{
	uint8_t spi_tx[3 + len];

	spi_tx[0] = W5500_BYTE_HIGH(addr);
	spi_tx[1] = W5500_BYTE_LOW(addr);
	spi_tx[2] = W5500_BSB(block) | W5500_RWB_WRITE | W5500_OM_VDM;

	memcpy(&spi_tx[3], data, len);

	struct no_os_spi_msg xfer = {
		.tx_buff = spi_tx,
		.rx_buff = NULL,
		.bytes_number = 3 + len,
		.cs_change = 1,
	};

	return no_os_spi_transfer(dev->spi, &xfer, 1);
}

/***************************************************************************//**
 * @brief Read data from a W5500 register
 *
 * @param dev   - The device descriptor
 * @param block - The block select bits (common registers, socket registers, etc.)
 * @param addr  - The register address
 * @param data  - The buffer to store read data
 * @param len   - The data length to read
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_reg_read(struct w5500_dev *dev, uint8_t block,
		   uint16_t addr, uint8_t *data, uint16_t len)
{
	uint8_t spi_buffer[3 + len];
	int ret;

	spi_buffer[0] = W5500_BYTE_HIGH(addr);
	spi_buffer[1] = W5500_BYTE_LOW(addr);
	spi_buffer[2] = W5500_BSB(block) | W5500_RWB_READ | W5500_OM_VDM;

	struct no_os_spi_msg xfer = {
		.tx_buff = spi_buffer,
		.rx_buff = spi_buffer,
		.bytes_number = 3 + len,
		.cs_change = 1,
	};

	ret = no_os_spi_transfer(dev->spi, &xfer, 1);
	if (ret)
		return ret;

	memcpy(data, &spi_buffer[3], len);

	return 0;
}

/***************************************************************************//**
 * @brief Read a 16-bit register reliably by reading until consecutive values match
 *
 * @param dev   - The device descriptor
 * @param block - The block select bits (common registers, socket registers, etc.)
 * @param addr  - The register address
 * @param value - Pointer to store the 16-bit value
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_read_16bit_reg(struct w5500_dev *dev, uint8_t block,
			 uint16_t addr, uint16_t *value)
{
	int ret;
	uint8_t data[2];
	uint16_t val1, val2;
	int attempts = 0;
	const int max_attempts = 5;

	do {
		ret = w5500_reg_read(dev, block, addr, data, 2);
		if (ret)
			return ret;
		val1 = (data[0] << 8) | data[1];

		ret = w5500_reg_read(dev, block, addr, data, 2);
		if (ret)
			return ret;
		val2 = (data[0] << 8) | data[1];

		attempts++;
	} while (val1 != val2 && attempts < max_attempts);

	if (attempts >= max_attempts)
		return -EIO;

	*value = val1;
	return 0;
}

/***************************************************************************//**
 * @brief Set MAC address
 *
 * @param dev - The device descriptor
 * @param mac - 6-byte MAC address array
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_set_mac(struct w5500_dev *dev, const uint8_t mac[6])
{
	return w5500_reg_write(dev, W5500_COMMON_REG, W5500_SHAR, mac, 6);
}

/***************************************************************************//**
 * @brief Set IP address
 *
 * @param dev - The device descriptor
 * @param ip  - 4-byte IP address array
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_set_ip(struct w5500_dev *dev, const uint8_t ip[4])
{
	return w5500_reg_write(dev, W5500_COMMON_REG, W5500_SIPR, ip, 4);
}

/***************************************************************************//**
 * @brief Set subnet mask
 *
 * @param dev    - The device descriptor
 * @param subnet - 4-byte subnet mask array
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_set_subnet(struct w5500_dev *dev, const uint8_t subnet[4])
{
	return w5500_reg_write(dev, W5500_COMMON_REG, W5500_SUBR, subnet, 4);
}

/***************************************************************************//**
 * @brief Set gateway address
 *
 * @param dev     - The device descriptor
 * @param gateway - 4-byte gateway address array
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_set_gateway(struct w5500_dev *dev, const uint8_t gateway[4])
{
	return w5500_reg_write(dev, W5500_COMMON_REG, W5500_GAR, gateway, 4);
}

/***************************************************************************//**
 * @brief Get MAC address
 *
 * @param dev - The device descriptor
 * @param mac - Buffer to store the 6-byte MAC address
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_get_mac(struct w5500_dev *dev, uint8_t mac[6])
{
	return w5500_reg_read(dev, W5500_COMMON_REG, W5500_SHAR, mac, 6);
}

/***************************************************************************//**
 * @brief Get IP address
 *
 * @param dev - The device descriptor
 * @param ip  - Buffer to store the 4-byte IP address
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_get_ip(struct w5500_dev *dev, uint8_t ip[4])
{
	return w5500_reg_read(dev, W5500_COMMON_REG, W5500_SIPR, ip, 4);
}

/***************************************************************************//**
 * @brief Get subnet mask
 *
 * @param dev    - The device descriptor
 * @param subnet - Buffer to store the 4-byte subnet mask
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_get_subnet(struct w5500_dev *dev, uint8_t subnet[4])
{
	return w5500_reg_read(dev, W5500_COMMON_REG, W5500_SUBR, subnet, 4);
}

/***************************************************************************//**
 * @brief Get gateway address
 *
 * @param dev     - The device descriptor
 * @param gateway - Buffer to store the 4-byte gateway address
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_get_gateway(struct w5500_dev *dev, uint8_t gateway[4])
{
	return w5500_reg_read(dev, W5500_COMMON_REG, W5500_GAR, gateway, 4);
}

/***************************************************************************//**
 * @brief Check the Ethernet link status
 *
 * @param dev - The device descriptor
 *
 * @return 0 if link is up, -ENETDOWN if link is down, other negative codes on error
*******************************************************************************/
int w5500_check_link_status(struct w5500_dev *dev)
{
	int ret;
	uint8_t phycfgr;

	ret = w5500_reg_read(dev, W5500_COMMON_REG, W5500_PHYCFGR, &phycfgr, 1);
	if (ret)
		return ret;

	if (!no_os_field_get(W5500_PHYCFGR_LNK, phycfgr))
		return -ENETDOWN;

	return 0;
}

/***************************************************************************//**
 * @brief Configure W5500 TCP parameters (RTR and RCR)
 *
 * @param dev         - The device descriptor
 * @param retry_time  - Retry timeout value in 100μs units (default: 2000 = 200ms)
 * @param retry_count - Maximum retry count (default: 8)
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_tcp_config(struct w5500_dev *dev, uint16_t retry_time,
		     uint8_t retry_count)
{
	int ret;
	uint8_t rtr_buf[2];

	if (retry_time) {
		rtr_buf[0] = W5500_BYTE_HIGH(retry_time);
		rtr_buf[1] = W5500_BYTE_LOW(retry_time);

		ret = w5500_reg_write(dev, W5500_COMMON_REG, W5500_RTR, rtr_buf, 2);
		if (ret)
			return ret;
	}

	if (retry_count)
		return w5500_reg_write(dev, W5500_COMMON_REG, W5500_RCR, &retry_count, 1);

	return 0;
}

/***************************************************************************//**
 * @brief Write to a socket register
 *
 * @param dev        - The device descriptor
 * @param sock_id    - Socket number (0-7)
 * @param reg_offset - Register offset
 * @param data       - Data to write
 * @param len        - Data length
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
static int w5500_socket_reg_write(struct w5500_dev *dev, uint8_t sock_id,
				  uint16_t reg_offset, const uint8_t *data,
				  uint16_t len)
{
	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	return w5500_reg_write(dev, W5500_SOCKET_REG_BLOCK(sock_id),
			       reg_offset, data, len);
}

/***************************************************************************//**
 * @brief Read from a socket register
 *
 * @param dev        - The device descriptor
 * @param sock_id    - Socket number (0-7)
 * @param reg_offset - Register offset
 * @param data       - Buffer to store data
 * @param len        - Data length to read
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
static int w5500_socket_reg_read(struct w5500_dev *dev, uint8_t sock_id,
				 uint16_t reg_offset, uint8_t *data,
				 uint16_t len)
{
	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	return w5500_reg_read(dev, W5500_SOCKET_REG_BLOCK(sock_id),
			      reg_offset, data, len);
}

/***************************************************************************//**
 * @brief Read status of a socket
 *
 * @param dev     - The device descriptor
 * @param sock_id - Socket number (0-7)
 * @param status  - Variable to store status
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_read_status(struct w5500_dev *dev, uint8_t sock_id,
			     uint8_t *status)
{
	return w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, status, 1);
}

/***************************************************************************//**
 * @brief Write a command to a socket's command register
 *
 * @param dev     - The device descriptor
 * @param sock_id - Socket number (0-7)
 * @param val     - Command value to write
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_command_write(struct w5500_dev *dev, uint8_t sock_id,
			       uint8_t val)
{
	uint8_t cmd = val;

	return w5500_socket_reg_write(dev, sock_id, W5500_Sn_CR, &cmd, 1);
}

/***************************************************************************//**
 * @brief Clear specific socket interrupt flags
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 * @param flags   - Interrupt flags to clear
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_clear_interrupt(struct w5500_dev *dev, uint8_t sock_id,
				 uint8_t flags)
{
	return w5500_socket_reg_write(dev, sock_id, W5500_Sn_IR, &flags, 1);
}

/***************************************************************************//**
 * @brief Initialize socket data structures
 *
 * @param dev - The device descriptor
 *
 * @note Internal function, no return value
*******************************************************************************/
static void w5500_sockets_init(struct w5500_dev *dev)
{
	for (int i = 0; i <= W5500_MAX_SOCK_NUMBER; i++) {
		dev->sockets[i].protocol = W5500_Sn_MR_CLOSE;
		dev->sockets[i].mss = 1460;
		dev->sockets[i].tx_buf_size = 2;
		dev->sockets[i].rx_buf_size = 2;
	}
}

/***************************************************************************//**
 * @brief Initialize a socket
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_init(struct w5500_dev *dev, uint8_t sock_id)
{
	int ret;
	uint8_t mss_buf[2];
	struct w5500_socket *sock = &dev->sockets[sock_id];

	mss_buf[0] = W5500_BYTE_HIGH(sock->mss);
	mss_buf[1] = W5500_BYTE_LOW(sock->mss);

	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_MR, &sock->protocol, 1);
	if (ret)
		return ret;

	if (sock->protocol == W5500_Sn_MR_TCP) {
		ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_MSSR, mss_buf, 2);
		if (ret)
			return ret;
	}

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_RXBUF_SIZE,
				     &sock->rx_buf_size, 1);
	if (ret)
		return ret;

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_TXBUF_SIZE,
				     &sock->tx_buf_size, 1);
	if (ret)
		return ret;

	return w5500_socket_command_write(dev, sock_id, W5500_Sn_CR_OPEN);
}

/***************************************************************************//**
 * @brief Open a socket with specified protocol
 *
 * @param dev       - The device descriptor
 * @param sock_id   - Socket ID to use (0-7)
 * @param protocol     - W5500 mode register protocol
 * @param buff_size - Buffer size (1, 2, 4, 8, 16 KB)
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_open(struct w5500_dev *dev, uint8_t sock_id, uint8_t protocol,
		      uint8_t buff_size)
{
	struct w5500_socket *sock;
	int ret;

	if (protocol == W5500_Sn_MR_MACRAW) {
		if (sock_id != 0)
			return -EINVAL;
	} else if (protocol != W5500_Sn_MR_UDP && protocol != W5500_Sn_MR_TCP) {
		return -EINVAL;
	}

	sock = &dev->sockets[sock_id];

	sock->protocol = protocol;
	sock->tx_buf_size = 2;
	sock->rx_buf_size = 2;

	ret = w5500_socket_init(dev, sock_id);
	if (ret) {
		sock->protocol = W5500_Sn_MR_CLOSE;
		return ret;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Close a socket
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_close(struct w5500_dev *dev, uint8_t sock_id)
{
	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	return w5500_socket_command_write(dev, sock_id, W5500_Sn_CR_CLOSE);
}

/***************************************************************************//**
 * @brief Connect to a remote host (TCP client mode)
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 * @param addr    - Socket address structure containing remote IP and port
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_connect(struct w5500_dev *dev, uint8_t sock_id,
			 struct w5500_socket_address *addr)
{
	int ret;
	uint8_t status;

	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	if (!addr)
		return -EINVAL;

	ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
	if (ret)
		return ret;

	if (status != W5500_Sn_SR_INIT)
		return -EINVAL;

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_DIPR, addr->ip, 4);
	if (ret)
		return ret;

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_DPORT, addr->port, 2);
	if (ret)
		return ret;

	return w5500_socket_command_write(dev, sock_id, W5500_Sn_CR_CONNECT);
}

/***************************************************************************//**
 * @brief Disconnect a TCP connection
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 *
 * @return 0 on success, -ENOTCONN if not connected, other negative error codes
*******************************************************************************/
int w5500_socket_disconnect(struct w5500_dev *dev, uint8_t sock_id)
{
	int ret;
	uint8_t status;

	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
	if (ret)
		return ret;

	if (status != W5500_Sn_SR_ESTABLISHED &&
	    status != W5500_Sn_SR_CLOSE_WAIT)
		return -ENOTCONN;

	return w5500_socket_command_write(dev, sock_id, W5500_Sn_CR_DISCON);
}

/***************************************************************************//**
 * @brief Send data through a socket
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 * @param buf     - Data buffer to send
 * @param len     - Data length
 *
 * @return Number of bytes sent on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_send(struct w5500_dev *dev, uint8_t sock_id, const void *buf,
		      uint16_t len)
{
	int ret;
	uint16_t free_size = 0;
	uint16_t wr_ptr;
	uint8_t wr_ptr_buf[2];
	uint8_t status;
	uint16_t total_sent = 0;
	uint16_t chunk_size;
	const uint8_t *data = (const uint8_t *)buf;

	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
	if (ret)
		return ret;

	if (status != W5500_Sn_SR_ESTABLISHED &&
	    status != W5500_Sn_SR_UDP &&
	    status != W5500_Sn_SR_MACRAW)
		return -ENOTCONN;

	while (total_sent < len) {
		ret = w5500_read_16bit_reg(dev, W5500_SOCKET_REG_BLOCK(sock_id),
					   W5500_Sn_TX_FSR, &free_size);
		if (ret)
			return ret;

		if (free_size == 0) {
			ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
			if (ret)
				return ret;

			if (status != W5500_Sn_SR_ESTABLISHED &&
			    status != W5500_Sn_SR_UDP &&
			    status != W5500_Sn_SR_MACRAW)
				return -ECONNRESET;

			no_os_mdelay(1);
			continue;
		}

		chunk_size = (len - total_sent) < free_size ? (len - total_sent) : free_size;

		ret = w5500_read_16bit_reg(dev, W5500_SOCKET_REG_BLOCK(sock_id),
					   W5500_Sn_TX_WR, &wr_ptr);
		if (ret)
			return ret;

		ret = w5500_reg_write(dev, W5500_SOCKET_TX_BUF_BLOCK(sock_id),
				      wr_ptr, data + total_sent, chunk_size);
		if (ret)
			return ret;

		wr_ptr += chunk_size;

		wr_ptr_buf[0] = W5500_BYTE_HIGH(wr_ptr);
		wr_ptr_buf[1] = W5500_BYTE_LOW(wr_ptr);

		ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_TX_WR,
					     wr_ptr_buf, 2);
		if (ret)
			return ret;

		ret = w5500_socket_command_write(dev, sock_id, W5500_Sn_CR_SEND);
		if (ret)
			return ret;

		total_sent += chunk_size;
	}

	return len;
}

/***************************************************************************//**
 * @brief Receive data from a socket
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 * @param buf     - Buffer to store received data
 * @param len     - Maximum length to receive
 *
 * @return Number of bytes received on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_recv(struct w5500_dev *dev, uint8_t sock_id, void *buf,
		      uint16_t len)
{
	int ret;
	uint16_t rx_size = 0;
	uint16_t rd_ptr;
	uint8_t rd_ptr_buf[2];
	uint8_t status;

	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
	if (ret)
		return ret;

	if (status != W5500_Sn_SR_ESTABLISHED &&
	    status != W5500_Sn_SR_UDP &&
	    status != W5500_Sn_SR_MACRAW)
		return -ENOTCONN;

	while (1) {
		ret = w5500_read_16bit_reg(dev, W5500_SOCKET_REG_BLOCK(sock_id),
					   W5500_Sn_RX_RSR, &rx_size);
		if (ret)
			return ret;

		if (rx_size > 0)
			break;

		ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
		if (ret)
			return ret;

		if (status != W5500_Sn_SR_ESTABLISHED &&
		    status != W5500_Sn_SR_UDP &&
		    status != W5500_Sn_SR_MACRAW)
			return -ECONNRESET;

		no_os_mdelay(1);
	}

	if (len > rx_size)
		len = rx_size;

	ret = w5500_read_16bit_reg(dev, W5500_SOCKET_REG_BLOCK(sock_id),
				   W5500_Sn_RX_RD, &rd_ptr);
	if (ret)
		return ret;

	ret = w5500_reg_read(dev, W5500_SOCKET_RX_BUF_BLOCK(sock_id),
			     rd_ptr, buf, len);
	if (ret)
		return ret;

	rd_ptr += len;

	rd_ptr_buf[0] = W5500_BYTE_HIGH(rd_ptr);
	rd_ptr_buf[1] = W5500_BYTE_LOW(rd_ptr);

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_RX_RD, rd_ptr_buf, 2);
	if (ret)
		return ret;

	ret = w5500_socket_command_write(dev, sock_id, W5500_Sn_CR_RECV);
	if (ret)
		return ret;

	ret = w5500_socket_clear_interrupt(dev, sock_id, W5500_Sn_IR_RECV);
	if (ret)
		return ret;

	return len;
}

/***************************************************************************//**
 * @brief Send data to a specific destination (UDP)
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 * @param buf     - Data buffer to send
 * @param len     - Data length
 * @param to      - Destination address (IP and port)
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_sendto(struct w5500_dev *dev, uint8_t sock_id, const void *buf,
			uint16_t len, struct w5500_socket_address *to)
{
	int ret;
	uint8_t status;
	uint16_t free_size = 0;
	uint16_t wr_ptr;
	uint8_t wr_ptr_buf[2];

	if (sock_id > W5500_MAX_SOCK_NUMBER || !buf || !to)
		return -EINVAL;

	ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
	if (ret)
		return ret;

	if (status != W5500_Sn_SR_UDP)
		return -EPROTOTYPE;

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_DIPR, to->ip, 4);
	if (ret)
		return ret;

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_DPORT, to->port, 2);
	if (ret)
		return ret;

	while (1) {
		ret = w5500_read_16bit_reg(dev, W5500_SOCKET_REG_BLOCK(sock_id),
					   W5500_Sn_TX_FSR, &free_size);
		if (ret)
			return ret;

		if (free_size >= len)
			break;

		ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
		if (ret)
			return ret;

		if (status != W5500_Sn_SR_UDP)
			return -EPROTOTYPE;

		no_os_mdelay(1);
	}

	ret = w5500_read_16bit_reg(dev, W5500_SOCKET_REG_BLOCK(sock_id),
				   W5500_Sn_TX_WR, &wr_ptr);
	if (ret)
		return ret;

	ret = w5500_reg_write(dev, W5500_SOCKET_TX_BUF_BLOCK(sock_id),
			      wr_ptr, buf, len);
	if (ret)
		return ret;

	wr_ptr += len;

	wr_ptr_buf[0] = W5500_BYTE_HIGH(wr_ptr);
	wr_ptr_buf[1] = W5500_BYTE_LOW(wr_ptr);

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_TX_WR,
				     wr_ptr_buf, 2);
	if (ret)
		return ret;

	return w5500_socket_command_write(dev, sock_id, W5500_Sn_CR_SEND);
}

/***************************************************************************//**
 * @brief Receive data from a socket with source address information (UDP)
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 * @param buf     - Buffer to store received data
 * @param len     - Maximum length to receive
 * @param from    - Structure to store source address information
 *
 * @return Number of bytes received on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_recvfrom(struct w5500_dev *dev, uint8_t sock_id,
			  void *buf, uint16_t len,
			  struct w5500_socket_address *from)
{
	int ret;
	uint16_t rx_size = 0;
	uint16_t rd_ptr;
	uint8_t rd_ptr_buf[2];
	uint8_t status;
	uint8_t header[8];
	uint16_t data_len;

	if (sock_id > W5500_MAX_SOCK_NUMBER || !buf)
		return -EINVAL;

	ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
	if (ret)
		return ret;

	if (status != W5500_Sn_SR_UDP)
		return -EPROTOTYPE;

	ret = w5500_read_16bit_reg(dev, W5500_SOCKET_REG_BLOCK(sock_id),
				   W5500_Sn_RX_RSR, &rx_size);
	if (ret)
		return ret;

	if (rx_size < 8)
		return 0;

	ret = w5500_read_16bit_reg(dev, W5500_SOCKET_REG_BLOCK(sock_id),
				   W5500_Sn_RX_RD, &rd_ptr);
	if (ret)
		return ret;

	ret = w5500_reg_read(dev, W5500_SOCKET_RX_BUF_BLOCK(sock_id),
			     rd_ptr, header, 8);
	if (ret)
		return ret;

	data_len = (header[6] << 8) | header[7];

	if (len > data_len)
		len = data_len;

	ret = w5500_reg_read(dev, W5500_SOCKET_RX_BUF_BLOCK(sock_id),
			     rd_ptr + 8, buf, len);
	if (ret)
		return ret;

	if (from) {
		from->ip[0] = header[0];
		from->ip[1] = header[1];
		from->ip[2] = header[2];
		from->ip[3] = header[3];
		from->port[0] = header[4];
		from->port[1] = header[5];
	}

	rd_ptr += data_len + 8;

	rd_ptr_buf[0] = W5500_BYTE_HIGH(rd_ptr);
	rd_ptr_buf[1] = W5500_BYTE_LOW(rd_ptr);

	ret = w5500_socket_reg_write(dev, sock_id, W5500_Sn_RX_RD,
				     rd_ptr_buf, 2);
	if (ret)
		return ret;

	ret = w5500_socket_command_write(dev, sock_id, W5500_Sn_CR_RECV);
	if (ret)
		return ret;

	return len;
}

/***************************************************************************//**
 * @brief Bind a socket to a specific port (server mode)
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 * @param port    - Local port number
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_bind(struct w5500_dev *dev, uint8_t sock_id, uint16_t port)
{
	uint8_t port_buf[2];

	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	port_buf[0] = W5500_BYTE_HIGH(port);
	port_buf[1] = W5500_BYTE_LOW(port);

	return w5500_socket_reg_write(dev, sock_id, W5500_Sn_PORT, port_buf, 2);
}

/***************************************************************************//**
 * @brief Set socket to listen mode (TCP server)
 *
 * @param dev     - Device descriptor
 * @param sock_id - Socket number (0-7)
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
int w5500_socket_listen(struct w5500_dev *dev, uint8_t sock_id)
{
	int ret;
	uint8_t status;

	if (sock_id > W5500_MAX_SOCK_NUMBER)
		return -EINVAL;

	ret = w5500_socket_reg_read(dev, sock_id, W5500_Sn_SR, &status, 1);
	if (ret)
		return ret;

	if (status != W5500_Sn_SR_INIT)
		return -EINVAL;

	return w5500_socket_command_write(dev, sock_id, W5500_Sn_CR_LISTEN);
}

/***************************************************************************//**
 * @brief Reset the W5500 chip using hardware or software method
 *
 * @param dev - The device descriptor
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_reset(struct w5500_dev *dev)
{
	int ret;
	uint8_t mr;
	uint8_t ver;

	if (dev->gpio_reset) {
		ret = no_os_gpio_set_value(dev->gpio_reset, 0);
		if (ret)
			return ret;

		/* Hold reset low for at least 500us per datasheet */
		no_os_udelay(500);

		ret = no_os_gpio_set_value(dev->gpio_reset, 1);
		if (ret)
			return ret;
	} else {
		mr = W5500_MR_RST;
		ret = w5500_reg_write(dev, W5500_COMMON_REG, W5500_MR, &mr, 1);
		if (ret)
			return ret;
	}

	/* Wait for reset to complete */
	no_os_mdelay(1);

	ret = w5500_reg_read(dev, W5500_COMMON_REG, W5500_MR, &mr, 1);
	if (ret)
		return ret;

	if (mr & W5500_MR_RST)
		return -ETIMEDOUT;

	ret = w5500_reg_read(dev, W5500_COMMON_REG, W5500_VERSIONR, &ver, 1);
	if (ret)
		return ret;

	if (ver != W5500_CHIP_VERSION)
		return -EINVAL;

	return 0;
}

/***************************************************************************//**
 * @brief Setup the W5500 chip with basic configuration
 *
 * @param dev - The device descriptor
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_setup(struct w5500_dev *dev)
{
	int ret;

	ret = w5500_reset(dev);
	if (ret)
		return ret;

	ret = w5500_set_mac(dev, dev->mac_addr);
	if (ret)
		return ret;

	ret = w5500_tcp_config(dev, dev->retry_time, dev->retry_count);
	if (ret)
		return ret;

	return w5500_check_link_status(dev);
}

/***************************************************************************//**
 * @brief Initialize the device
 *
 * @param device     - Pointer to the device descriptor to be initialized
 * @param init_param - The device's initialization parameters
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_init(struct w5500_dev **device,
	       struct w5500_init_param *init_param)
{
	struct w5500_dev *dev;
	int ret;

	dev = (struct w5500_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_gpio_get_optional(&dev->gpio_reset, init_param->gpio_reset);
	if (ret)
		goto free_dev;

	ret = no_os_gpio_get_optional(&dev->gpio_int, init_param->gpio_int);
	if (ret)
		goto free_gpio_reset;

	ret = no_os_spi_init(&dev->spi, init_param->spi_init);
	if (ret)
		goto free_gpio_int;

	memcpy(dev->mac_addr, init_param->mac_addr, 6);
	memcpy(&dev->retry_time, &init_param->retry_time, 1);
	memcpy(&dev->retry_count, &init_param->retry_count, 1);

	w5500_sockets_init(dev);

	ret = w5500_setup(dev);
	if (ret)
		goto free_spi;

	*device = dev;

	return 0;

free_spi:
	no_os_spi_remove(dev->spi);
free_gpio_int:
	no_os_gpio_remove(dev->gpio_int);
free_gpio_reset:
	no_os_gpio_remove(dev->gpio_reset);
free_dev:
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Free a device descriptor and release resources
 *
 * @param dev - The device descriptor to be removed
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_remove(struct w5500_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi);
	ret |= no_os_gpio_remove(dev->gpio_reset);
	ret |= no_os_gpio_remove(dev->gpio_int);

	no_os_free(dev);

	return ret;
}
