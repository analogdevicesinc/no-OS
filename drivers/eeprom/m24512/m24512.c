/***************************************************************************//**
 *   @file   m24512.c
 *   @brief  Implementation of M24512 EEPROM driver
 *   @author robert.budai@analog.com
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG Devices, INC. "AS IS" AND ANY EXPRESS OR
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

#include <string.h>

#include "m24512.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_eeprom.h"

/**
 * @brief Set write protection state
 * @param dev - Device descriptor
 * @param protect - true to enable write protection, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
static int m24512_set_write_protection(struct m24512_dev *dev, bool protect);

/**
 * @brief Explicitly enable write operations (drive WC pin LOW)
 * @param dev - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
static int m24512_enable_write(struct m24512_dev *dev);

/**
 * @brief Perform initialization test with write/read back verification
 * @param dev - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
static int m24512_init_test(struct m24512_dev *dev);

/**
 * @brief Perform acknowledge polling to check if device is ready
 * @param dev - Device descriptor
 * @return true if ready, false if busy
 */
static bool m24512_ack_poll(struct m24512_dev *dev);

/**
 * @brief Write data to EEPROM with address setup
 * @param dev - Device descriptor
 * @param addr - Memory address
 * @param data - Data to write
 * @param len - Number of bytes to write
 * @return 0 in case of success, negative error code otherwise
 */
static int m24512_write_raw(struct m24512_dev *dev, uint16_t addr,
			    const uint8_t *data, uint16_t len);

/**
 * @brief Initialize the M24512 EEPROM device
 */

int32_t m24512_init(struct no_os_eeprom_desc **desc,
		    const struct no_os_eeprom_init_param *param)
{
	struct m24512_init_param *m24512_init_param;
	struct no_os_eeprom_desc *eeprom_desc;
	struct m24512_dev *m24512_dev;
	int ret;

	if (!desc || !param)
		return -EINVAL;

	eeprom_desc = (struct no_os_eeprom_desc *)no_os_calloc(1, sizeof(*eeprom_desc));
	if (!eeprom_desc)
		return -ENOMEM;

	eeprom_desc->device_id = param->device_id;

	m24512_dev = (struct m24512_dev *)no_os_calloc(1, sizeof(*m24512_dev));
	if (!m24512_dev) {
		ret = -ENOMEM;
		goto remove_eeprom_desc;
	}

	m24512_init_param = param->extra;

	ret = no_os_i2c_init(&m24512_dev->i2c_desc, &m24512_init_param->i2c_init);
	if (ret)
		goto remove_m24512_dev;

	// Store device I2C address
	m24512_dev->i2c_addr = m24512_init_param->i2c_addr;

	// Initialize write control GPIO if provided
	if (m24512_init_param->wc_gpio_init) {
		ret = no_os_gpio_get(&m24512_dev->wc_gpio_desc,
				     m24512_init_param->wc_gpio_init);
		if (ret)
			goto remove_i2c_desc;
		ret = no_os_gpio_direction_output(m24512_dev->wc_gpio_desc, NO_OS_GPIO_LOW);
		if (ret)
			goto remove_wc_gpio_desc;
		ret = m24512_enable_write(m24512_dev);
		if (ret)
			goto remove_wc_gpio_desc;
	} else {
		m24512_dev->wc_gpio_desc = NULL;
	}

	// Test communication by performing acknowledge polling
	if (!m24512_ack_poll(m24512_dev)) {
		ret = -ENODEV;
		if (m24512_dev->wc_gpio_desc)
			goto remove_wc_gpio_desc;
		goto remove_i2c_desc;
	}

	eeprom_desc->extra = m24512_dev;
	*desc = eeprom_desc;
	return 0;

remove_wc_gpio_desc:
	no_os_gpio_remove(m24512_dev->wc_gpio_desc);
remove_i2c_desc:
	no_os_i2c_remove(m24512_dev->i2c_desc);
remove_m24512_dev:
	no_os_free(m24512_dev);
remove_eeprom_desc:
	no_os_free(eeprom_desc);
	return ret;
}

/**
 * @brief Free resources allocated by M24512 device
 */
int32_t m24512_remove(struct no_os_eeprom_desc *desc)
{
	struct m24512_dev *dev;
	if (!desc)
		return -EINVAL;

	dev = (struct m24512_dev *)desc->extra;
	if (!dev)
		return -EINVAL;

	if (dev->wc_gpio_desc)
		no_os_gpio_remove(dev->wc_gpio_desc);

	if (dev->i2c_desc)
		no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Read multiple bytes from EEPROM
 */
int32_t m24512_read_data(struct no_os_eeprom_desc *desc, uint32_t address,
			 uint8_t *data, uint16_t len)
{
	uint16_t addr = (uint16_t)address;
	struct m24512_dev *dev;
	uint8_t addr_buf[2];
	int ret;

	if (!desc || !data || len == 0 || !m24512_is_valid_addr(address) ||
	    !m24512_is_valid_addr(address + len - 1))
		return -EINVAL;

	dev = (struct m24512_dev *)desc->extra;
	if (!dev)
		return -EINVAL;

	// Wait for any ongoing write operation to complete
	ret = m24512_wait_ready(dev, M24512_WRITE_CYCLE_TIME);
	if (ret)
		return ret;

	// Set up address bytes (big-endian)
	addr_buf[0] = M24512_ADDR_HIGH_BYTE(addr);
	addr_buf[1] = M24512_ADDR_LOW_BYTE(addr);

	// Write address, then read data
	ret = no_os_i2c_write(dev->i2c_desc, addr_buf, 2, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, data, len, 1);
	return ret;
}

/**
 * @brief Write multiple bytes to EEPROM (handles page boundaries)
 */
int32_t m24512_write_data(struct no_os_eeprom_desc *desc, uint32_t address,
			  uint8_t *data, uint16_t len)
{
	uint16_t addr = (uint16_t)address;
	uint16_t bytes_written = 0;
	uint16_t current_addr = addr;
	struct m24512_dev *dev;
	uint16_t bytes_to_write;
	uint16_t page_offset;
	int ret;

	if (!desc || !data || len == 0 || !m24512_is_valid_addr(addr) ||
	    !m24512_is_valid_addr(addr + len - 1))
		return -EINVAL;

	dev = (struct m24512_dev *)desc->extra;

	// Disable write protection
	ret = m24512_set_write_protection(dev, false);
	if (ret)
		return ret;

	while (bytes_written < len) {
		// Calculate how many bytes we can write in current page
		page_offset = m24512_addr_to_offset(current_addr);
		bytes_to_write = M24512_PAGE_SIZE - page_offset;

		// Don't write more than requested
		if (bytes_to_write > (len - bytes_written))
			bytes_to_write = len - bytes_written;

		// Perform the write operation
		ret = m24512_write_raw(dev, current_addr,
				       &data[bytes_written], bytes_to_write);
		if (ret) {
			m24512_set_write_protection(dev, true);
			return ret;
		}

		// Wait for write cycle to complete
		ret = m24512_wait_ready(dev, M24512_WRITE_CYCLE_TIME);
		if (ret) {
			m24512_set_write_protection(dev, true);
			return ret;
		}

		bytes_written += bytes_to_write;
		current_addr += bytes_to_write;
	}

	// Re-enable write protection
	m24512_set_write_protection(dev, true);
	return 0;
}

/**
 * @brief Check if EEPROM is ready (write cycle completed)
 */
bool m24512_is_ready(struct m24512_dev *dev)
{
	if (!dev)
		return false;

	return m24512_ack_poll(dev);
}

/**
 * @brief Wait for write cycle completion
 */
int m24512_wait_ready(struct m24512_dev *dev, uint32_t timeout_ms)
{
	uint32_t attempts;
	uint32_t max_attempts;

	if (!dev)
		return -EINVAL;

	// Calculate maximum attempts based on timeout
	// Each attempt includes a 1ms delay, so max_attempts = timeout_ms
	max_attempts = timeout_ms;
	if (max_attempts == 0)
		max_attempts = 1;

	for (attempts = 0; attempts < max_attempts; attempts++) {
		if (m24512_ack_poll(dev))
			return 0;

		// Wait 1ms between polling attempts
		no_os_mdelay(1);
	}

	return -ETIME;
}

/**
 * @brief Get device information
 */
int m24512_get_info(struct m24512_dev *dev, uint32_t *memory_size,
		    uint16_t *page_size, uint16_t *num_pages)
{
	if (!dev)
		return -EINVAL;

	if (memory_size)
		*memory_size = M24512_MEMORY_SIZE;

	if (page_size)
		*page_size = M24512_PAGE_SIZE;

	if (num_pages)
		*num_pages = M24512_NUM_PAGES;

	return 0;
}

/**
 * @brief Set write protection state
 */
static int m24512_set_write_protection(struct m24512_dev *dev, bool protect)
{
	int ret;

	if (!dev || !dev->wc_gpio_desc)
		return 0; // No WC pin connected, protection not available

	// WC pin: LOW = Write enabled, HIGH = Write protected
	ret = no_os_gpio_set_value(dev->wc_gpio_desc,
				   protect ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);

	if (ret == 0) {
		// Add small delay to ensure WC pin settles
		no_os_udelay(10); // 10 microseconds should be sufficient
	}

	return ret;
}

/**
 * @brief Explicitly enable write operations (drive WC pin LOW)
 */
static int m24512_enable_write(struct m24512_dev *dev)
{
	if (!dev)
		return -EINVAL;

	// Force WC pin LOW to enable writes
	return m24512_set_write_protection(dev, false);
}

/**
 * @brief Perform acknowledge polling to check if device is ready
 */
static bool m24512_ack_poll(struct m24512_dev *dev)
{
	uint8_t dummy = 0;
	int ret;

	if (!dev)
		return false;

	// Try to write to device - if ACK is received, device is ready
	ret = no_os_i2c_write(dev->i2c_desc, &dummy, 0, 1);

	// Device is ready if no error occurred
	return (ret == 0);
}

/**
 * @brief Write data to EEPROM with address setup
 */
static int m24512_write_raw(struct m24512_dev *dev, uint16_t addr,
			    const uint8_t *data, uint16_t len)
{
	uint8_t *write_buf;
	int ret;

	if (!dev || !data || len == 0)
		return -EINVAL;

	// Allocate buffer for address + data
	write_buf = (uint8_t *)no_os_calloc(len + 2, sizeof(uint8_t));
	if (!write_buf)
		return -ENOMEM;

	// Set up address bytes (big-endian)
	write_buf[0] = M24512_ADDR_HIGH_BYTE(addr);
	write_buf[1] = M24512_ADDR_LOW_BYTE(addr);

	// Copy data
	memcpy(&write_buf[2], data, len);

	// Ensure write protection is disabled right before write
	ret = m24512_set_write_protection(dev, false);
	if (ret) {
		no_os_free(write_buf);
		return ret;
	}

	// Perform write operation
	ret = no_os_i2c_write(dev->i2c_desc, write_buf, len + 2, 1);

	// Give EEPROM time to start internal write cycle
	if (ret == 0) {
		no_os_udelay(500);  // 500 microseconds delay
	}

	no_os_free(write_buf);
	return ret;
}

/**
 * M24512 EEPROM specific ops structure
 */
const struct no_os_eeprom_platform_ops eeprom_m24512_ops = {
	.init = &m24512_init,
	.remove = &m24512_remove,
	.read = &m24512_read_data,
	.write = &m24512_write_data,
};