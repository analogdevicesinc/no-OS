/***************************************************************************//**
 *   @file   m24512.h
 *   @brief  Header file for M24512 EEPROM driver
 *   @author robert.budai@analog.com
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __M24512_H__
#define __M24512_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_error.h"
#include "no_os_eeprom.h"

#define M24512_MEMORY_SIZE              65536U      // 64KB (512Kbit)
#define M24512_PAGE_SIZE                128U        // 128 bytes per page
#define M24512_NUM_PAGES                512U        // Total number of pages
#define M24512_MAX_ADDRESS              (M24512_MEMORY_SIZE - 1)

// Timing specifications (in milliseconds)
#define M24512_WRITE_CYCLE_TIME         5U          // Maximum write cycle time
#define M24512_BYTE_WRITE_TIME          5U          // Byte/page write time
#define M24512_PAGE_WRITE_TIME          5U          // Page write time

// Address format
#define M24512_ADDR_HIGH_BYTE(addr)     ((uint8_t)((addr >> 8) & 0xFF))
#define M24512_ADDR_LOW_BYTE(addr)      ((uint8_t)(addr & 0xFF))

/**
 * @struct m24512_init_param
 * @brief M24512 initialization parameters
 */
struct m24512_init_param {
	/** I2C initialization parameters */
	struct no_os_i2c_init_param i2c_init;
	/** Device I2C address (0x50-0x57 depending on A2,A1,A0 pins) */
	uint8_t i2c_addr;
	/** Write Control GPIO initialization parameters (optional, can be NULL) */
	struct no_os_gpio_init_param *wc_gpio_init;
};

/**
 * @struct m24512_dev
 * @brief M24512 device descriptor
 */
struct m24512_dev {
	/** I2C descriptor */
	struct no_os_i2c_desc *i2c_desc;
	/** Device I2C address */
	uint8_t i2c_addr;
	/** Write Control GPIO descriptor (NULL if not used) */
	struct no_os_gpio_desc *wc_gpio_desc;
};

/**
 * @brief Initialize the M24512 EEPROM device
 * @param desc - Pointer to device descriptor
 * @param param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int32_t m24512_init(struct no_os_eeprom_desc **desc,
		    const struct no_os_eeprom_init_param *param);

/**
 * @brief Free resources allocated by M24512 device
 * @param desc - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int32_t m24512_remove(struct no_os_eeprom_desc *desc);

/**
 * @brief Read multiple bytes from EEPROM
 * @param desc - Device descriptor
 * @param address - Starting memory address
 * @param data - Buffer to store read data
 * @param len - Number of bytes to read
 * @return 0 in case of success, negative error code otherwise
 */
int32_t m24512_read_data(struct no_os_eeprom_desc *desc, uint32_t address,
			 uint8_t *data, uint16_t len);

/**
 * @brief Write multiple bytes to EEPROM (handles page boundaries)
 * @param desc - Device descriptor
 * @param address - Starting memory address
 * @param data - Data buffer to write
 * @param len - Number of bytes to write
 * @return 0 in case of success, negative error code otherwise
 */
int32_t m24512_write_data(struct no_os_eeprom_desc *desc, uint32_t address,
			  uint8_t *data, uint16_t len);

/**
 * @brief Check if EEPROM is ready (write cycle completed)
 * @param dev - Device descriptor
 * @return true if ready, false if busy
 */
bool m24512_is_ready(struct m24512_dev *dev);

/**
 * @brief Wait for write cycle completion
 * @param dev - Device descriptor
 * @param timeout_ms - Maximum timeout in milliseconds
 * @return 0 if ready, negative error code on timeout
 */
int m24512_wait_ready(struct m24512_dev *dev, uint32_t timeout_ms);

/**
 * @brief Get device information
 * @param dev - Device descriptor
 * @param memory_size - Pointer to store memory size
 * @param page_size - Pointer to store page size
 * @param num_pages - Pointer to store number of pages
 * @return 0 in case of success, negative error code otherwise
 */
int m24512_get_info(struct m24512_dev *dev, uint32_t *memory_size,
		    uint16_t *page_size, uint16_t *num_pages);

/**
 * @brief Calculate page number from memory address
 * @param addr - Memory address
 * @return Page number
 */
static inline uint16_t m24512_addr_to_page(uint16_t addr)
{
	return addr / M24512_PAGE_SIZE;
}

/**
 * @brief Calculate offset within page from memory address
 * @param addr - Memory address
 * @return Offset within page
 */
static inline uint8_t m24512_addr_to_offset(uint16_t addr)
{
	return addr % M24512_PAGE_SIZE;
}

/**
 * @brief Convert page number and offset to memory address
 * @param page - Page number
 * @param offset - Offset within page
 * @return Memory address
 */
static inline uint16_t m24512_page_to_addr(uint16_t page, uint8_t offset)
{
	return (page * M24512_PAGE_SIZE) + offset;
}

/**
 * @brief Validate memory address
 * @param addr - Memory address to validate
 * @return true if valid, false otherwise
 */
static inline bool m24512_is_valid_addr(uint16_t addr)
{
	return addr <= M24512_MAX_ADDRESS;
}

/**
 * @brief Validate page number
 * @param page - Page number to validate
 * @return true if valid, false otherwise
 */
static inline bool m24512_is_valid_page(uint16_t page)
{
	return page < M24512_NUM_PAGES;
}

/**
 * @brief M24512 EEPROM specific ops structure
 */
extern const struct no_os_eeprom_platform_ops eeprom_m24512_ops;

#endif /* __M24512_H__ */
