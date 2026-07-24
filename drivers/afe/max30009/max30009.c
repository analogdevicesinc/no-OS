/***************************************************************************//**
 *   @file   max30009.c
 *   @brief  Implementation of MAX30009 driver
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "max30009.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

#define MAX30009_SPI_READ                       0x80
#define MAX30009_SPI_WRITE                      0x00

/**
 * @brief Internal SPI register access function
 * @param device - MAX30009 device descriptor
 * @param reg_addr - Register address
 * @param data - Pointer to data buffer
 * @param len - Number of bytes to transfer
 * @param is_read - true for read, false for write
 * @return 0 in case of success, negative error code otherwise
 */
static int max30009_spi_reg_access(struct max30009_dev *device,
				   uint8_t reg_addr,
				   uint8_t *data,
				   uint32_t len,
				   bool is_read)
{
	uint8_t *buf;
	uint32_t i;
	int ret;

	buf = no_os_calloc(len + 2, sizeof(*buf));
	if (!buf)
		return -ENOMEM;

	buf[0] = reg_addr;
	buf[1] = is_read ? MAX30009_SPI_READ : MAX30009_SPI_WRITE;

	if (!is_read) {
		for (i = 0; i < len; i++)
			buf[2 + i] = data[i];
	}

	ret = no_os_spi_write_and_read(device->spi_desc, buf, len + 2);

	if (is_read && !ret) {
		for (i = 0; i < len; i++)
			data[i] = buf[2 + i]; /* Data is on 3rd byte */
	}

	no_os_free(buf);

	return ret;
}

/**
 * @brief Internal I2C register access function
 * @param device - MAX30009 device descriptor
 * @param reg_addr - Register address
 * @param data - Pointer to data buffer
 * @param len - Number of bytes to transfer
 * @param is_read - true for read, false for write
 * @return 0 in case of success, negative error code otherwise
 */
static int max30009_i2c_reg_access(struct max30009_dev *device,
				   uint8_t reg_addr,
				   uint8_t *data,
				   uint32_t len,
				   bool is_read)
{
	uint8_t *buf;
	uint32_t i;
	int ret;

	if (is_read) {
		ret = no_os_i2c_write(device->i2c_desc, &reg_addr, 1, 0);
		if (ret)
			return ret;

		return no_os_i2c_read(device->i2c_desc, data, len, 1);
	}

	buf = no_os_calloc(len + 1, sizeof(*buf));
	if (!buf)
		return -ENOMEM;

	buf[0] = reg_addr;
	for (i = 0; i < len; i++)
		buf[1 + i] = data[i];

	ret = no_os_i2c_write(device->i2c_desc, buf, len + 1, 1);
	no_os_free(buf);

	return ret;
}

/**
 * @brief Internal register access dispatcher
 * @param device - MAX30009 device descriptor
 * @param reg_addr - Register address
 * @param data - Pointer to data buffer
 * @param len - Number of bytes to transfer
 * @param is_read - true for read, false for write
 * @return 0 in case of success, negative error code otherwise
 */
static int max30009_reg_access(struct max30009_dev *device, uint8_t reg_addr,
			       uint8_t *data, uint32_t len, bool is_read)
{
	if (device->spi_desc)
		return max30009_spi_reg_access(device, reg_addr, data, len, is_read);
	else if (device->i2c_desc)
		return max30009_i2c_reg_access(device, reg_addr, data, len, is_read);
	else
		return -EINVAL;
}

/**
 * @brief Read multiple bytes from register
 * @param device - MAX30009 device descriptor
 * @param reg_addr - Register address to read from
 * @param data - Pointer to buffer where read data will be stored
 * @param len - Number of bytes to read
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_reg_read_multiple(struct max30009_dev *device, uint8_t reg_addr,
			       uint8_t *data, uint32_t len)
{
	return max30009_reg_access(device, reg_addr, data, len, true);
}

/**
 * @brief Read a single byte from register
 * @param device - MAX30009 device descriptor
 * @param reg_addr - Register address to read from
 * @param data - Pointer to variable where read byte will be stored
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_reg_read(struct max30009_dev *device, uint8_t reg_addr,
		      uint8_t *data)
{
	return max30009_reg_read_multiple(device, reg_addr, data, 1);
}

/**
 * @brief Write multiple bytes to register
 * @param device - MAX30009 device descriptor
 * @param reg_addr - Register address to write to
 * @param data - Pointer to buffer containing data to be written
 * @param len - Number of bytes to write
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_reg_write_multiple(struct max30009_dev *device, uint8_t reg_addr,
				uint8_t *data, uint32_t len)
{
	return max30009_reg_access(device, reg_addr, data, len, false);
}

/**
 * @brief Write a single byte to register
 * @param device - MAX30009 device descriptor
 * @param reg_addr - Register address to write to
 * @param data - Byte value to be written
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_reg_write(struct max30009_dev *device, uint8_t reg_addr,
		       uint8_t data)
{
	return max30009_reg_write_multiple(device, reg_addr, &data, 1);
}

/**
 * @brief Update specific bits in a register
 * @param device - MAX30009 device descriptor
 * @param reg_addr - Register address to update
 * @param mask - Bit mask indicating which bits to update
 * @param value - New value for the field, given in raw (unshifted) form. It is
 *		  shifted into the mask position internally via no_os_field_prep,
 *		  so callers must NOT pre-shift with no_os_field_prep.
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_reg_update(struct max30009_dev *device, uint8_t reg_addr,
			uint8_t mask, uint8_t value)
{
	uint8_t reg_val;
	int ret;

	ret = max30009_reg_read(device, reg_addr, &reg_val);
	if (ret)
		return ret;

	reg_val = (reg_val & ~mask) | no_os_field_prep(mask, value);

	return max30009_reg_write(device, reg_addr, reg_val);
}

/**
 * @brief Perform software reset
 * @param device - MAX30009 device descriptor
 * @return 0 in case of success, negative error code otherwise
 * @note Follows the datasheet-required soft-reset sequence: BIOZ_BG_EN=1,
 *	 SHDN=0, REF_CLK_SEL=0, PLL_EN=0, wait >=1ms, RESET=1.
 *	 Re-enable the PLL after soft reset via max30009_pll_enable().
 */
int max30009_soft_reset(struct max30009_dev *device)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = max30009_bioz_bg_enable(device, true);
	if (ret)
		return ret;

	ret = max30009_set_power_mode(device, false);
	if (ret)
		return ret;

	ret = max30009_reg_update(device,
				  MAX30009_REG_PLL_CONFIGURATION4,
				  MAX30009_REF_CLK_SEL_MSK,
				  0);
	if (ret)
		return ret;

	ret = max30009_pll_enable(device, false);
	if (ret)
		return ret;

	no_os_mdelay(10);
	ret = max30009_reg_update(device,
				  MAX30009_REG_SYSTEM_CONFIGURATION1,
				  MAX30009_RESET_MSK,
				  1);
	if (ret)
		return ret;

	no_os_mdelay(1);

	return ret;
}

/**
 * @brief Read device part ID
 * @param device - MAX30009 device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_verify_part_id(struct max30009_dev *device)
{
	uint8_t part_id;
	int ret;

	if (!device)
		return -EINVAL;

	ret = max30009_reg_read(device, MAX30009_REG_PART_ID, &part_id);
	if (ret)
		return ret;

	if (part_id != MAX30009_PART_ID_VALUE)
		return -ENODEV;

	return 0;
}

/**
 * @brief Set power mode (shutdown or active)
 * @param device - MAX30009 device descriptor
 * @param shutdown - True to enter shutdown mode, false for active mode
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_power_mode(struct max30009_dev *device, bool shutdown)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_SYSTEM_CONFIGURATION1,
				   MAX30009_SHDN_MSK,
				   shutdown);
}

/**
 * @brief Initialize the MAX30009 device
 * @param device - Pointer to the device descriptor pointer
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_init(struct max30009_dev **device,
		  struct max30009_init_param *init_param)
{
	struct max30009_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct max30009_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->fifo_read_buf = no_os_calloc(
				     MAX30009_FIFO_DEPTH * MAX30009_FIFO_DATA_SIZE,
				     sizeof(*dev->fifo_read_buf));
	if (!dev->fifo_read_buf) {
		ret = -ENOMEM;
		goto error_alloc;
	}

	if (init_param->use_i2c) {
		ret = no_os_gpio_get(&dev->csb_gpio_desc, &init_param->csb_gpio_init);
		if (ret)
			goto error_alloc;

		ret = no_os_gpio_direction_output(dev->csb_gpio_desc,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto error_csb_gpio;

		no_os_mdelay(10);

		ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	} else {
		ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	}
	if (ret)
		goto error_csb_gpio;

	if (init_param->int_gpio_init) {
		ret = no_os_gpio_get(&dev->int_gpio, init_param->int_gpio_init);
		if (ret)
			goto error_comms;

		ret = no_os_gpio_direction_input(dev->int_gpio);
		if (ret)
			goto error_int_gpio;
	}

	no_os_mdelay(100);

	ret = max30009_verify_part_id(dev);
	if (ret)
		goto error_int_gpio;

	ret = max30009_soft_reset(dev);
	if (ret)
		goto error_int_gpio;

	*device = dev;

	return 0;

error_int_gpio:
	if (dev->int_gpio)
		no_os_gpio_remove(dev->int_gpio);
error_comms:
	if (dev->spi_desc)
		no_os_spi_remove(dev->spi_desc);
	if (dev->i2c_desc)
		no_os_i2c_remove(dev->i2c_desc);
error_csb_gpio:
	if (dev->csb_gpio_desc)
		no_os_gpio_remove(dev->csb_gpio_desc);
error_alloc:
	no_os_free(dev->fifo_read_buf);
	no_os_free(dev);
	return ret;
}

/**
 * @brief Remove the MAX30009 device and free resources
 * @param device - MAX30009 device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_remove(struct max30009_dev *device)
{
	int ret = 0;

	if (!device)
		return -ENODEV;

	ret |= max30009_bioz_enable(device, false);
	ret |= max30009_set_power_mode(device, true);

	if (device->csb_gpio_desc)
		ret |= no_os_gpio_remove(device->csb_gpio_desc);

	if (device->int_gpio)
		ret |= no_os_gpio_remove(device->int_gpio);

	if (device->spi_desc)
		ret |= no_os_spi_remove(device->spi_desc);
	else if (device->i2c_desc)
		ret |= no_os_i2c_remove(device->i2c_desc);

	no_os_free(device->fifo_read_buf);
	no_os_free(device);

	return ret;
}

/**
 * @brief Get device status
 * @param device - MAX30009 device descriptor
 * @param status - Pointer to store device status information
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_get_status(struct max30009_dev *device,
			struct max30009_status *status)
{
	int ret;
	uint8_t status1, status2;

	if (!device || !status)
		return -EINVAL;

	ret = max30009_reg_read(device, MAX30009_REG_STATUS1, &status1);
	if (ret)
		return ret;

	ret = max30009_reg_read(device, MAX30009_REG_STATUS2, &status2);
	if (ret)
		return ret;

	status->a_full = (status1 & MAX30009_STATUS1_A_FULL_MSK) != 0;
	status->fifo_data_rdy = (status1 &
				 MAX30009_STATUS1_FIFO_DATA_RDY_MSK) != 0;
	status->freq_unlock = (status1 & MAX30009_STATUS1_FREQ_UNLOCK_MSK) != 0;
	status->freq_lock = (status1 & MAX30009_STATUS1_FREQ_LOCK_MSK) != 0;
	status->phase_unlock = (status1 &
				MAX30009_STATUS1_PHASE_UNLOCK_MSK) != 0;
	status->phase_lock = (status1 & MAX30009_STATUS1_PHASE_LOCK_MSK) != 0;
	status->pwr_rdy = (status1 & MAX30009_STATUS1_PWR_RDY_MSK) != 0;

	status->leads_on = (status2 & MAX30009_STATUS2_LON_MSK) != 0;
	status->bioz_over = (status2 & MAX30009_STATUS2_BIOZ_OVER_MSK) != 0;
	status->bioz_undr = (status2 & MAX30009_STATUS2_BIOZ_UNDR_MSK) != 0;
	status->drv_oor = (status2 & MAX30009_STATUS2_DRV_OOR_MSK) != 0;

	return 0;
}

/**
 * @brief Wait for frequency and phase lock
 * @param device - MAX30009 device descriptor
 * @param timeout_ms - Timeout in milliseconds
 * @return 0 in case of success, -ETIMEDOUT if timeout,
 *         negative error code otherwise
 */
static int max30009_wait_for_lock(struct max30009_dev *device,
				  uint32_t timeout_ms)
{
	int ret;
	uint8_t status;
	uint32_t elapsed = 0;

	if (!device)
		return -ENODEV;

	while (elapsed < timeout_ms) {
		ret = max30009_reg_read(device, MAX30009_REG_STATUS1, &status);
		if (ret)
			return ret;

		if ((status & MAX30009_STATUS1_FREQ_LOCK_MSK) &&
		    (status & MAX30009_STATUS1_PHASE_LOCK_MSK))
			return 0;

		no_os_mdelay(1);
		elapsed++;
	}

	return -ETIMEDOUT;
}

/**
 * @brief Check lead-off detection status
 * @param device - MAX30009 device descriptor
 * @param leads_on - True if leads connected
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_check_lead_off(struct max30009_dev *device, bool *leads_on)
{
	int ret;
	uint8_t status;

	if (!device || !leads_on)
		return -EINVAL;

	ret = max30009_reg_read(device, MAX30009_REG_STATUS2, &status);
	if (ret)
		return ret;

	*leads_on = (status & MAX30009_STATUS2_LON_MSK) != 0;

	return 0;
}

/**
 * @brief Flush FIFO contents
 * @param device - MAX30009 device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_fifo_flush(struct max30009_dev *device)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_FIFO_CONFIGURATION2,
				   MAX30009_FLUSH_FIFO_MSK,
				   1);
}

/**
 * @brief Get FIFO data count
 * @param device - MAX30009 device descriptor
 * @param count - Pointer to store FIFO data count (0-256)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_fifo_get_count(struct max30009_dev *device, uint16_t *count)
{
	int ret;
	uint8_t cnt1, cnt2;
	uint16_t fifo_count;

	if (!device || !count)
		return -EINVAL;

	ret = max30009_reg_read(device, MAX30009_REG_FIFO_COUNTER1, &cnt1);
	if (ret)
		return ret;

	ret = max30009_reg_read(device, MAX30009_REG_FIFO_COUNTER2, &cnt2);
	if (ret)
		return ret;

	fifo_count = (uint16_t)(((uint16_t)(cnt1 & MAX30009_FIFO_DATA_COUNT_MSB_MSK)
				 >> 7) << 8) | (uint16_t)cnt2;
	if (fifo_count > MAX30009_FIFO_DEPTH)
		fifo_count = MAX30009_FIFO_DEPTH;
	*count = fifo_count;

	return 0;
}

/**
 * @brief Get FIFO overflow data count
 * @param device - MAX30009 device descriptor
 * @param count - Pointer to store FIFO overflow data count (0-128)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_fifo_get_ovf_count(struct max30009_dev *device, uint16_t *count)
{
	int ret;
	uint8_t reg_val;

	if (!device || !count)
		return -EINVAL;

	ret = max30009_reg_read(device, MAX30009_REG_FIFO_COUNTER1, &reg_val);
	if (ret)
		return ret;

	*count = no_os_field_get(MAX30009_OVF_COUNTER_MSK, reg_val);
	return 0;
}

/**
 * @brief Read data from FIFO
 * @param device - MAX30009 device descriptor
 * @param data - Buffer to store FIFO data
 * @param len Number of bytes to read from the FIFO.
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_read_fifo_data(struct max30009_dev *device, uint8_t *data,
			    uint32_t len)
{
	if (!device || !data)
		return -EINVAL;

	return max30009_reg_read_multiple(device,
					  MAX30009_REG_FIFO_DATA_REGISTER,
					  data, len);
}

/**
 * @brief Decode a 3-byte FIFO sample into type and data
 * @param raw_data - 3-byte raw FIFO data
 * @param sample - Pointer to store decoded sample (type and data)
 * @return 0 in case of success, negative error code otherwise
 *
 * FIFO Data Format (24 bits total):
 * - Bits 23-20: TAG[3:0] - Sample type identifier
 * - Bits 19-0: DATA[19:0] - 20-bit signed measurement data
 *
 * TAG Values:
 * - 0x1: BioZ In-Phase sample
 * - 0x2: BioZ Quadrature sample
 * - 0xE: Marker (0xFFFFFE - all 1s except bit 0)
 * - 0xF: Invalid sample (0xFFFFFF - all 1s)
 */
int max30009_fifo_decode_sample(uint8_t *raw_data,
				struct max30009_fifo_sample *sample)
{
	uint32_t raw_value;
	uint8_t tag;

	if (!raw_data || !sample)
		return -EINVAL;

	raw_value = ((uint32_t)raw_data[0] << 16) |
		    ((uint32_t)raw_data[1] << 8) |
		    ((uint32_t)raw_data[2]);

	tag = (raw_value >> MAX30009_FIFO_TAG_SHIFT) & MAX30009_FIFO_TAG_MASK;

	if (raw_value == 0xFFFFFF) {
		sample->type = MAX30009_FIFO_SAMPLE_INVALID;
		sample->data = 0;
		return 0;
	} else if (raw_value == 0xFFFFFE) {
		sample->type = MAX30009_FIFO_SAMPLE_MARKER;
		sample->data = 0;
		return 0;
	}

	switch (tag) {
	case 0x1:
		sample->type = MAX30009_FIFO_SAMPLE_IN_PHASE;
		break;
	case 0x2:
		sample->type = MAX30009_FIFO_SAMPLE_QUADRATURE;
		break;
	default:
		sample->type = MAX30009_FIFO_SAMPLE_INVALID;
		sample->data = 0;
		return 0;
	}

	sample->data = (int32_t)(raw_value & MAX30009_FIFO_DATA_MASK);

	/* Sign-extend 20-bit two's complement value to 32 bits */
	if (sample->data & MAX30009_FIFO_SIGN_BIT)
		sample->data |= (int32_t)MAX30009_FIFO_SIGN_EXT;

	return 0;
}

/**
 * @brief Get FIFO read and write pointers
 * @param device - Device descriptor
 * @param write_ptr - Pointer to store write pointer value (0-255)
 * @param read_ptr - Pointer to store read pointer value (0-255)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_get_fifo_pointers(struct max30009_dev *device,
			       uint8_t *write_ptr,
			       uint8_t *read_ptr)
{
	int ret;

	if (!device || !write_ptr || !read_ptr)
		return -EINVAL;

	ret = max30009_reg_read(device, MAX30009_REG_FIFO_WRITE_POINTER,
				write_ptr);
	if (ret)
		return ret;

	return max30009_reg_read(device, MAX30009_REG_FIFO_READ_POINTER,
				 read_ptr);
}

/**
 * @brief Set FIFO watermark level
 * @param device - Device descriptor
 * @param watermark - Watermark level (number of samples before FIFO
 *                    generates an interrupt).
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_fifo_watermark(struct max30009_dev *device,
				uint8_t watermark)
{
	if (!device)
		return -EINVAL;

	if (watermark < 1 || watermark > MAX30009_FIFO_DEPTH)
		return -EINVAL;

	return max30009_reg_write(device, MAX30009_REG_FIFO_CONFIGURATION1,
				  MAX30009_FIFO_DEPTH - watermark);
}

/**
 * @brief Gets the FIFO watermark level of the device.
 * @param device - Device descriptor.
 * @param watermark - Pointer to a variable where the current FIFO
 *                    watermark level
 * @return 0 in case of success, negative error code otherwise.
 */
int max30009_get_fifo_watermark(struct max30009_dev *device, uint8_t *watermark)
{
	uint8_t reg_val;
	int ret;

	ret = max30009_reg_read(device, MAX30009_REG_FIFO_CONFIGURATION1,
				&reg_val);
	if (ret)
		return ret;

	*watermark = MAX30009_FIFO_DEPTH - reg_val;

	return 0;
}

/**
 * @brief Set FIFO rollover mode
 * @param device - Device descriptor
 * @param rollover - True to enable rollover mode, false to stop when full
 * @return 0 in case of success, negative error code otherwise
 * @note In rollover mode, new data overwrites oldest data when FIFO is full
 */
int max30009_set_fifo_rollover(struct max30009_dev *device, bool rollover)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_FIFO_CONFIGURATION2,
				   MAX30009_FIFO_RO_MSK,
				   rollover);
}

/**
 * @brief Set almost-full interrupt type
 * @param device - Device descriptor
 * @param type - True for asserted when count >= threshold,
 *               False for count > threshold
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_a_full_type(struct max30009_dev *device, bool type)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_FIFO_CONFIGURATION2,
				   MAX30009_A_FULL_TYPE_MSK,
				   type);
}

/**
 * @brief Insert marker into FIFO data stream
 * @param device - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 * @note Marker is used to identify specific data points in FIFO stream
 */
int max30009_insert_fifo_marker(struct max30009_dev *device)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_FIFO_CONFIGURATION2,
				   MAX30009_FIFO_MARK_MSK,
				   1);
}

/**
 * @brief Clear FIFO status flags
 * @param device - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_clear_fifo_status(struct max30009_dev *device)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_FIFO_CONFIGURATION2,
				   MAX30009_FIFO_STAT_CLR_MSK,
				   1);
}

/**
 * @brief Trigger timing system reset for PLL synchronization
 * @param device - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 * @note Use this after PLL configuration changes to synchronize timing system
 */
int max30009_timing_system_reset(struct max30009_dev *device)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = max30009_reg_update(device, MAX30009_REG_SYSTEM_SYNC,
				  MAX30009_TIMING_SYS_RESET_MSK,
				  1);
	if (ret)
		return ret;

	no_os_mdelay(1);

	return 0;
}

/**
 * @brief Set master/slave mode for PLL synchronization
 * @param device - Device descriptor
 * @param is_master - True for master mode (generates FCLK_SYS),
 *                    False for slave mode (uses external FCLK_SYS)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_master_mode(struct max30009_dev *device, bool is_master)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_SYSTEM_CONFIGURATION1,
				   MAX30009_MASTER_MSK,
				   is_master);
}

/**
 * @brief Configure INT pin function and output
 * @param device - Device descriptor
 * @param func_cfg - Function configuration (0-3)
 * @param out_cfg - Output configuration (0-3)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_configure_int_pin(struct max30009_dev *device,
			       uint8_t func_cfg,
			       uint8_t out_cfg)
{
	int ret;

	if (!device)
		return -ENODEV;

	if (func_cfg > 3 || out_cfg > 3)
		return -EINVAL;

	ret = max30009_reg_update(device,
				  MAX30009_REG_PIN_FUNC_CONFIGURATION,
				  MAX30009_INT_FCFG_MSK,
				  func_cfg);
	if (ret)
		return ret;

	return max30009_reg_update(device,
				   MAX30009_REG_OUTPUT_PIN_CONFIGURATION,
				   MAX30009_INT_OCFG_MSK,
				   out_cfg);
}

/**
 * @brief Configure TRIG pin input and output
 * @param device - Device descriptor
 * @param input_cfg - Input configuration (0 or 1)
 * @param output_cfg - Output configuration (0-3)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_configure_trig_pin(struct max30009_dev *device,
				uint8_t input_cfg,
				uint8_t output_cfg)
{
	int ret;

	if (!device)
		return -ENODEV;

	if (input_cfg > 1 || output_cfg > 3)
		return -EINVAL;

	ret = max30009_reg_update(device,
				  MAX30009_REG_PIN_FUNC_CONFIGURATION,
				  MAX30009_TRIG_ICFG_MSK,
				  input_cfg);
	if (ret)
		return ret;

	return max30009_reg_update(device,
				   MAX30009_REG_OUTPUT_PIN_CONFIGURATION,
				   MAX30009_TRIG_OCFG_MSK,
				   output_cfg);
}

/**
 * @brief Configure I2C broadcast address
 * @param device - Device descriptor
 * @param enable - True to enable broadcast, false to disable
 * @param broadcast_addr - Broadcast address (7-bit, shifted left by 1)
 * @return 0 in case of success, negative error code otherwise
 * @note Allows multiple devices to respond to broadcast address
 */
int max30009_configure_i2c_broadcast(struct max30009_dev *device,
				     bool enable,
				     uint8_t broadcast_addr)
{
	uint8_t reg_val;

	if (!device)
		return -ENODEV;

	reg_val = no_os_field_prep(MAX30009_I2C_BCAST_ADDR_MSK, broadcast_addr);
	if (enable)
		reg_val |= MAX30009_I2C_BCAST_EN_MSK;

	return max30009_reg_write(device, MAX30009_REG_I2C_BROADCAST_ADDRESS,
				  reg_val);
}

/**
 * @brief Disable I2C interface (SPI only mode)
 * @param device - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 * @note After disabling I2C, device only responds to SPI
 */
int max30009_disable_i2c(struct max30009_dev *device)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_SYSTEM_CONFIGURATION1,
				   MAX30009_DISABLE_I2C_MSK,
				   1);
}

/**
 * @brief Configure PLL dividers
 * @param device - Device descriptor
 * @param mdiv - M divider value (0-1023, PLL_CLK = (MDIV+1) × REF_CLK)
 * @param ndiv - N divider value (MAX30009_NDIV_512 or MAX30009_NDIV_1024)
 * @param kdiv - K divider value (MAX30009_KDIV_1 to MAX30009_KDIV_8192)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_pll_set_dividers(struct max30009_dev *device, uint16_t mdiv,
			      enum max30009_pll_ndiv ndiv,
			      enum max30009_pll_kdiv kdiv)
{
	int ret;
	uint32_t pll_clk;

	if (!device)
		return -ENODEV;

	/* Calculate PLL_CLK to validate range (14MHz - 28MHz) */
	pll_clk = device->ref_clk_freq * (mdiv + 1);
	if (pll_clk < 14000000 || pll_clk > 28000000)
		return -EINVAL;

	ret = max30009_reg_update(device, MAX30009_REG_PLL_CONFIGURATION1,
				  MAX30009_MDIV_MSB_MSK, mdiv >> 8);
	if (ret)
		return ret;

	ret = max30009_reg_update(device, MAX30009_REG_PLL_CONFIGURATION1,
				  MAX30009_NDIV_MSK, ndiv);
	if (ret)
		return ret;

	ret = max30009_reg_update(device, MAX30009_REG_PLL_CONFIGURATION1,
				  MAX30009_KDIV_MSK, kdiv);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_PLL_CONFIGURATION2,
				 (uint8_t)(mdiv & 0xFF));
	if (ret)
		return ret;

	device->pll_config.mdiv = mdiv;
	device->pll_config.ndiv = ndiv;
	device->pll_config.kdiv = kdiv;

	return 0;
}

/**
 * @brief Enable or disable PLL
 * @param device - Device descriptor
 * @param enable - True to enable PLL, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_pll_enable(struct max30009_dev *device, bool enable)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = max30009_reg_update(device,
				  MAX30009_REG_PLL_CONFIGURATION1,
				  MAX30009_PLL_EN_MSK,
				  enable);
	if (ret)
		return ret;

	device->pll_config.pll_enabled = enable;

	if (!enable)
		return 0;

	return max30009_wait_for_lock(device, MAX30009_PLL_LOCK_TIMEOUT_MS);
}

/**
 * @brief Set PLL lock window
 * @param device - Device descriptor
 * @param lock_window - True to enable extended lock window, false for normal
  * @return 0 in case of success, negative error code otherwise
 */
int max30009_pll_set_lock_window(struct max30009_dev *device, bool lock_window)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_PLL_CONFIGURATION3,
				   MAX30009_PLL_LOCK_WNDW_MSK,
				   lock_window);
}

/**
 * @brief Configure clock source
 * @param device - Device descriptor
 * @param use_external - True for external clock, false for internal oscillator
 * @param use_32768_hz - True for 32.768 kHz, false for 32.0 kHz
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_clock_source(struct max30009_dev *device, bool use_external,
			      bool use_32768_hz)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = max30009_reg_update(device, MAX30009_REG_PLL_CONFIGURATION4,
				  MAX30009_REF_CLK_SEL_MSK,
				  use_external);
	if (ret)
		return ret;

	ret = max30009_reg_update(device, MAX30009_REG_PLL_CONFIGURATION4,
				  MAX30009_CLK_FREQ_SEL_MSK,
				  use_32768_hz);
	if (ret)
		return ret;

	device->ref_clk_freq = use_32768_hz ? MAX30009_REF_CLK_32768_HZ :
			       MAX30009_REF_CLK_32000_HZ;

	return 0;
}

/**
 * @brief Fine tune internal clock frequency
 * @param device - Device descriptor
 * @param tune_val - Fine tune value (0-31, ±0.78% adjustment range)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_clock_fine_tune(struct max30009_dev *device, uint8_t tune_val)
{
	if (!device)
		return -ENODEV;

	if (tune_val > 31)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_PLL_CONFIGURATION4,
				   MAX30009_CLK_FINE_TUNE_MSK,
				   tune_val);
}

/**
 * @brief Enable or disable BioZ measurement
 * @param device - MAX30009 device descriptor
 * @param enable - True to enable BioZ, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_bioz_enable(struct max30009_dev *device, bool enable)
{
	int ret;
	uint8_t en_mask = MAX30009_BIOZ_I_EN_MSK |
			  MAX30009_BIOZ_Q_EN_MSK |
			  MAX30009_BIOZ_BG_EN_MSK;

	if (!device)
		return -ENODEV;

	/* en_mask covers bits[2:0]; 0x07 is the unshifted
	 * logical value with all three channel bits set. */
	ret = max30009_reg_update(device, MAX30009_REG_BIOZ_CONFIGURATION1,
				  en_mask, enable ? 0x07 : 0);
	if (ret)
		return ret;

	if (enable)
		no_os_mdelay(100);

	return 0;
}

/**
 * @brief Enable or disable BioZ I channel
 * @param device - MAX30009 device descriptor
 * @param enable - True to enable I channel, false to disable
  * @return 0 in case of success, negative error code otherwise
 */
int max30009_bioz_i_enable(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device, MAX30009_REG_BIOZ_CONFIGURATION1,
				   MAX30009_BIOZ_I_EN_MSK,
				   enable);
}

/**
 * @brief Enable or disable BioZ Q channel
 * @param device - MAX30009 device descriptor
 * @param enable - True to enable Q channel, false to disable
  * @return 0 in case of success, negative error code otherwise
 */
int max30009_bioz_q_enable(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device, MAX30009_REG_BIOZ_CONFIGURATION1,
				   MAX30009_BIOZ_Q_EN_MSK,
				   enable);
}

/**
 * @brief Enable or disable BioZ bandgap
 * @param device - MAX30009 device descriptor
 * @param enable - True to enable bandgap, false to disable
  * @return 0 in case of success, negative error code otherwise
 */
int max30009_bioz_bg_enable(struct max30009_dev *device, bool enable)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = max30009_reg_update(device, MAX30009_REG_BIOZ_CONFIGURATION1,
				  MAX30009_BIOZ_BG_EN_MSK,
				  enable);
	if (ret)
		return ret;

	if (enable)
		no_os_mdelay(100);

	return 0;
}

/**
 * @brief Set DAC oversampling ratio
 * @param device - Device descriptor
 * @param osr - DAC OSR value (MAX30009_DAC_OSR_32, _64, _128, or _256)
 * @return 0 in case of success, negative error code otherwise
 * @note Affects stimulus frequency: F_BIOZ = PLL_CLK / (KDIV × DAC_OSR)
 */
int max30009_set_dac_osr(struct max30009_dev *device, enum max30009_dac_osr osr)
{
	int ret;

	if (!device)
		return -ENODEV;

	if (osr > MAX30009_DAC_OSR_256)
		return -EINVAL;

	ret = max30009_reg_update(device,
				  MAX30009_REG_BIOZ_CONFIGURATION1,
				  MAX30009_BIOZ_DAC_OSR_MSK,
				  osr);
	if (ret)
		return ret;

	device->bioz_config.dac_osr = osr;
	return 0;
}

/**
 * @brief Set ADC oversampling ratio
 * @param device - Device descriptor
 * @param osr - ADC OSR value (MAX30009_ADC_OSR_8 through _1024)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_adc_osr(struct max30009_dev *device, enum max30009_adc_osr osr)
{
	int ret;

	if (!device)
		return -ENODEV;

	if (osr > MAX30009_ADC_OSR_1024)
		return -EINVAL;

	ret = max30009_reg_update(device,
				  MAX30009_REG_BIOZ_CONFIGURATION1,
				  MAX30009_BIOZ_ADC_OSR_MSK,
				  osr);
	if (ret)
		return ret;

	device->bioz_config.adc_osr = osr;
	return 0;
}

/**
 * @brief Set digital high-pass filter
 * @param device - Device descriptor
 * @param dhpf - Digital HPF setting
 * @return 0 in case of success, negative error code otherwise
 * @note Used to remove DC offset from bioimpedance signal
 */
int max30009_set_digital_hpf(struct max30009_dev *device,
			     enum max30009_dhpf dhpf)
{
	if (!device)
		return -ENODEV;

	if (dhpf > MAX30009_DHPF_0_002_SR)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION2,
				   MAX30009_BIOZ_DHPF_MSK,
				   dhpf);
}

/**
 * @brief Set digital low-pass filter
 * @param device - Device descriptor
 * @param dlpf - Digital LPF setting
 * @return 0 in case of success, negative error code otherwise
 * @note Used for anti-aliasing and noise reduction
 */
int max30009_set_digital_lpf(struct max30009_dev *device,
			     enum max30009_dlpf dlpf)
{
	if (!device)
		return -ENODEV;

	if (dlpf > MAX30009_DLPF_0_25_SR)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION2,
				   MAX30009_BIOZ_DLPF_MSK,
				   dlpf);
}

/**
 * @brief Set analog high-pass filter
 * @param device - Device descriptor
 * @param ahpf - Analog HPF setting (BYPASS, or 100Hz-10kHz)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_analog_hpf(struct max30009_dev *device,
			    enum max30009_ahpf ahpf)
{
	if (!device)
		return -ENODEV;

	if (ahpf > MAX30009_AHPF_848K_OHM)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION5,
				   MAX30009_BIOZ_AHPF_MSK,
				   ahpf);
}

/**
 * @brief Set drive mode (current, voltage, H-bridge, or standby)
 * @param device - Device descriptor
 * @param mode - Drive mode (SINE_CURRENT, SINE_VOLTAGE, H_BRIDGE, or STANDBY)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_drive_mode(struct max30009_dev *device,
			    enum max30009_drive_mode mode)
{
	if (!device)
		return -ENODEV;

	if (mode > MAX30009_DRIVE_STANDBY)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION3,
				   MAX30009_BIOZ_DRV_MODE_MSK,
				   mode);
}

/**
 * @brief Set drive current with detailed control
 * @param device - Device descriptor
 * @param idrv_range - Current range setting (0-3)
 * @param vdrv_mag - Voltage magnitude setting (0-3)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_drive_current_detailed(struct max30009_dev *device,
					uint8_t idrv_range, uint8_t vdrv_mag)
{
	int ret;

	if (!device)
		return -ENODEV;

	if (idrv_range > 3 || vdrv_mag > 3)
		return -EINVAL;

	ret = max30009_reg_update(device,
				  MAX30009_REG_BIOZ_CONFIGURATION3,
				  MAX30009_BIOZ_IDRV_RGE_MSK,
				  idrv_range);
	if (ret)
		return ret;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION3,
				   MAX30009_BIOZ_VDRV_MAG_MSK,
				   vdrv_mag);
}

/**
 * @brief Enable or disable external resistor for current drive
 * @param device - Device descriptor
 * @param enable - True to enable external resistor, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_external_resistor(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION3,
				   MAX30009_BIOZ_EXT_RES_MSK,
				   enable);
}

/**
 * @brief Enable or disable rapid lead-off detection
 * @param device - Device descriptor
 * @param enable - True to enable rapid mode, false for normal
 * @return 0 in case of success, negative error code otherwise
 * @note Rapid mode detects lead-off faster but with higher current consumption
 */
int max30009_enable_rapid_lead_off(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION3,
				   MAX30009_LOFF_RAPID_MSK,
				   enable);
}

/**
 * @brief Enable or disable fast start mode
 * @param device - Device descriptor
 * @param enable - True to enable fast start, false to disable
 * @param manual - True for manual trigger, false for automatic
 * @return 0 in case of success, negative error code otherwise
 * @note Fast start reduces settling time for faster measurements
 */
int max30009_enable_fast_start(struct max30009_dev *device,
			       bool enable,
			       bool manual)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = max30009_reg_update(device,
				  MAX30009_REG_BIOZ_CONFIGURATION4,
				  MAX30009_BIOZ_FAST_START_EN_MSK,
				  enable);
	if (ret)
		return ret;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION4,
				   MAX30009_BIOZ_FAST_MANUAL_MSK,
				   manual);
}

/**
 * @brief Set INA (Instrumentation Amplifier) mode
 * @param device - Device descriptor
 * @param low_power - True for low-power mode, false for low-noise mode
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_ina_mode(struct max30009_dev *device, bool low_power)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION5,
				   MAX30009_BIOZ_INA_MODE_MSK,
				   low_power);
}

/**
 * @brief Disable differential mode (enable single-ended)
 * @param device - Device descriptor
 * @param disable - True to disable differential mode, false for differential
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_disable_differential_mode(struct max30009_dev *device,
				       bool disable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION5,
				   MAX30009_BIOZ_DM_DIS_MSK,
				   disable);
}

/**
 * @brief Set amplifier range
 * @param device - Device descriptor
 * @param amp_range - Amplifier range setting (0-3)
 * @return 0 in case of success, negative error code otherwise
 * @note Optimizes ADC dynamic range for different electrode impedances
 */
int max30009_set_amp_range(struct max30009_dev *device, uint8_t amp_range)
{
	if (!device)
		return -ENODEV;

	if (amp_range > 3)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION6,
				   MAX30009_BIOZ_AMP_RGE_MSK,
				   amp_range);
}

/**
 * @brief Set amplifier bandwidth
 * @param device - Device descriptor
 * @param amp_bw - Amplifier bandwidth setting (0-3)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_amp_bandwidth(struct max30009_dev *device, uint8_t amp_bw)
{
	if (!device)
		return -ENODEV;

	if (amp_bw > 3)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION6,
				   MAX30009_BIOZ_AMP_BW_MSK,
				   amp_bw);
}

/**
 * @brief Enable or disable external capacitor
 * @param device - Device descriptor
 * @param enable - True to enable external capacitor, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_external_cap(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION6,
				   MAX30009_BIOZ_EXT_CAP_MSK,
				   enable);
}

/**
 * @brief Enable or disable DC restore circuit
 * @param device - Device descriptor
 * @param enable - True to enable DC restore, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_dc_restore(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION6,
				   MAX30009_BIOZ_DC_RESTORE_MSK,
				   enable);
}

/**
 * @brief Enable or disable drive reset
 * @param device - Device descriptor
 * @param enable - True to enable drive reset, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_drive_reset(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION6,
				   MAX30009_BIOZ_DRV_RESET_MSK,
				   enable);
}

/**
 * @brief Enable or disable DAC reset
 * @param device - Device descriptor
 * @param enable - True to enable DAC reset, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_dac_reset(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION6,
				   MAX30009_BIOZ_DAC_RESET_MSK,
				   enable);
}

/**
 * @brief Set comparison mode for threshold detection
 * @param device - Device descriptor
 * @param cmp_mode - Comparison mode (0-3)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_comparison_mode(struct max30009_dev *device, uint8_t cmp_mode)
{
	if (!device)
		return -ENODEV;

	if (cmp_mode > 3)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION2,
				   MAX30009_BIOZ_CMP_MSK,
				   cmp_mode);
}

/**
 * @brief Enable or disable BioZ threshold detection
 * @param device - Device descriptor
 * @param enable - True to enable threshold detection, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_bioz_threshold(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION2,
				   MAX30009_EN_BIOZ_THRESH_MSK,
				   enable);
}

/**
 * @brief Set BioZ threshold values
 * @param device - Device descriptor
 * @param low_thresh - Low threshold value (0-255)
 * @param high_thresh - High threshold value (0-255)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_bioz_thresholds(struct max30009_dev *device,
				 uint8_t low_thresh, uint8_t high_thresh)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_LOW_THRESHOLD,
				 low_thresh);
	if (ret)
		return ret;

	return max30009_reg_write(device, MAX30009_REG_BIOZ_HIGH_THRESHOLD,
				  high_thresh);
}

/**
 * @brief Set demodulation clock phases
 * @param device - Device descriptor
 * @param i_clk_phase - I channel clock phase (true for inverted)
 * @param q_clk_phase - Q channel clock phase (true for inverted)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_demod_clk_phases(struct max30009_dev *device,
				  bool i_clk_phase, bool q_clk_phase)
{
	uint8_t reg_val = 0;

	if (!device)
		return -ENODEV;

	if (i_clk_phase)
		reg_val |= 0x01;

	if (q_clk_phase)
		reg_val |= 0x02;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION7,
				   MAX30009_BIOZ_I_CLK_PHASE_MSK |
				   MAX30009_BIOZ_Q_CLK_PHASE_MSK,
				   reg_val);
}

/**
 * @brief Enable or disable INA chopping
 * @param device - Device descriptor
 * @param enable - True to enable chopping (improves CMRR), false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_ina_chopping(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION7,
				   MAX30009_BIOZ_INA_CHOP_EN_MSK,
				   enable);
}

/**
 * @brief Set channel frequency select
 * @param device - Device descriptor
 * @param fsel - Frequency select bit
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_channel_freq_select(struct max30009_dev *device, bool fsel)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION7,
				   MAX30009_BIOZ_CH_FSEL_MSK,
				   fsel);
}

/**
 * @brief Enable or disable standby mode
 * @param device - Device descriptor
 * @param keep_rx_on - True to keep receive path on in standby,
 *                     false to power down completely
 * @return 0 in case of success, negative error code otherwise
 * @note Standby mode reduces power consumption while maintaining quick wake-up
 */
int max30009_enable_standby_mode(struct max30009_dev *device,
				 bool keep_rx_on)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_CONFIGURATION7,
				   MAX30009_BIOZ_STBYON_MSK,
				   keep_rx_on);
}

/**
 * @brief Enable or disable calibration mode
 * @param device - Device descriptor
 * @param enable - True to enable calibration, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_calibration(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_MUX_CONFIGURATION1,
				   MAX30009_CAL_EN_MSK,
				   enable);
}

/**
 * @brief Enable or disable calibration MUX
 * @param device - Device descriptor
 * @param enable - True to enable calibration MUX, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_calibration_mux(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_MUX_CONFIGURATION1,
				   MAX30009_MUX_EN_MSK,
				   enable);
}

/**
 * @brief Set calibration connect-only mode
 * @param device - Device descriptor
 * @param cal_only - True for connect-only mode, false for normal
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_calibration_connect_only(struct max30009_dev *device,
				      bool cal_only)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_MUX_CONFIGURATION1,
				   MAX30009_CONNECT_CAL_ONLY_MSK,
				   cal_only);
}

/**
 * @brief Set BIA (Body Impedance Analysis) load resistor
 * @param device - Device descriptor
 * @param rsel - Resistor selection (280, 600, 900, or 5100 ohms)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_bia_load_resistor(struct max30009_dev *device,
				   enum max30009_bia_rsel rsel)
{
	if (!device)
		return -ENODEV;

	if (rsel > MAX30009_BIA_280_OHM)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_MUX_CONFIGURATION1,
				   MAX30009_BMUX_RSEL_MSK,
				   rsel);
}

/**
 * @brief Enable or disable BIA BIST (Built-In Self Test)
 * @param device - Device descriptor
 * @param enable - True to enable BIST, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_bia_bist(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_MUX_CONFIGURATION1,
				   MAX30009_BMUX_BIST_EN_MSK,
				   enable);
}

/**
 * @brief Set GSR (Galvanic Skin Response) load resistor
 * @param device - Device descriptor
 * @param rsel - Resistor selection (25.7k, 101k, 505k, or 1M ohms)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_gsr_load_resistor(struct max30009_dev *device,
				   enum max30009_gsr_rsel rsel)
{
	if (!device)
		return -ENODEV;

	if (rsel > MAX30009_GSR_1M_OHM)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_MUX_CONFIGURATION2,
				   MAX30009_GSR_RSEL_MSK,
				   rsel);
}

/**
 * @brief Enable or disable GSR load resistor
 * @param device - Device descriptor
 * @param enable - True to enable GSR load, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_gsr_load(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_MUX_CONFIGURATION2,
				   MAX30009_GSR_LOAD_EN_MSK,
				   enable);
}

/**
 * @brief Enable or disable internal load resistor
 * @param device - Device descriptor
 * @param enable - True to enable internal load, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_internal_load(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_MUX_CONFIGURATION2,
				   MAX30009_EN_INT_INLOAD_MSK,
				   enable);
}

/**
 * @brief Enable or disable external load connection
 * @param device - Device descriptor
 * @param enable - True to enable external load connection, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_external_load(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_BIOZ_MUX_CONFIGURATION2,
				   MAX30009_EN_EXT_INLOAD_MSK,
				   enable);
}

/**
 * @brief Assign electrodes to measurement channels
 * @param device - Device descriptor
 * @param bip_assign - BIP (positive receive) electrode assignment
 * @param bin_assign - BIN (negative receive) electrode assignment
 * @param drvp_assign - DRVP (positive drive) electrode assignment
 * @param drvn_assign - DRVN (negative drive) electrode assignment
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_assign_electrodes(struct max30009_dev *device,
			       uint8_t bip_assign,
			       uint8_t bin_assign,
			       uint8_t drvp_assign,
			       uint8_t drvn_assign)
{
	uint8_t reg_val;

	if (!device)
		return -ENODEV;

	if (bip_assign > 2 ||
	    bin_assign > 2 ||
	    drvp_assign > 3 ||
	    drvn_assign > 3)
		return -EINVAL;

	reg_val = no_os_field_prep(MAX30009_BIP_ASSIGN_MSK, bip_assign) |
		  no_os_field_prep(MAX30009_BIN_ASSIGN_MSK, bin_assign) |
		  no_os_field_prep(MAX30009_DRVP_ASSIGN_MSK, drvp_assign) |
		  no_os_field_prep(MAX30009_DRVN_ASSIGN_MSK, drvn_assign);

	return max30009_reg_write(device, MAX30009_REG_BIOZ_MUX_CONFIGURATION3,
				  reg_val);
}

/**
 * @brief Read BIST error value
 * @param device - Device descriptor
 * @param error - Pointer to store BIST error value
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_read_bist_error(struct max30009_dev *device, uint8_t *error)
{
	if (!device || !error)
		return -EINVAL;

	return max30009_reg_read(device, MAX30009_REG_BIOZ_MUX_CONFIGURATION4,
				 error);
}

/**
 * @brief Configure lead-off detection
 * @param device - MAX30009 device descriptor
 * @param enable - True to enable lead-off detection, false to disable
 * @param threshold - Lead-off detection threshold (0-15)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_lead_off_config(struct max30009_dev *device, bool enable,
			     uint8_t threshold)
{
	int ret;
	uint8_t cfg_val;

	if (!device)
		return -ENODEV;

	if (threshold > 15)
		return -EINVAL;

	ret = max30009_reg_update(device,
				  MAX30009_REG_DC_LEAD_DETECT_THRESHOLD,
				  MAX30009_LOFF_THRESH_MSK,
				  threshold);
	if (ret)
		return ret;

	/*
	 * pass 0x03 (bits [1:0] set) to enable both, or 0 to disable both.
	 */
	cfg_val = enable ? 0x03 : 0;
	return max30009_reg_update(device,
				   MAX30009_REG_DC_LEADS_CONFIGURATION,
				   MAX30009_EN_LON_DET_MSK |
				   MAX30009_EN_LOFF_DET_MSK,
				   cfg_val);
}

/**
 * @brief Set lead-off detection current magnitude
 * @param device - Device descriptor
 * @param current - Lead-off current setting (5nA, 11nA, 22nA, 55nA, or 110nA)
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_lead_off_current(struct max30009_dev *device,
				  enum max30009_loff_current current)
{
	if (!device)
		return -ENODEV;

	if (current > MAX30009_LOFF_100NA)
		return -EINVAL;

	return max30009_reg_update(device,
				   MAX30009_REG_DC_LEADS_CONFIGURATION,
				   MAX30009_LOFF_IMAG_MSK,
				   current);
}

/**
 * @brief Set lead-off detection current polarity
 * @param device - Device descriptor
 * @param positive - True for positive polarity, false for negative polarity
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_lead_off_polarity(struct max30009_dev *device, bool positive)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_DC_LEADS_CONFIGURATION,
				   MAX30009_LOFF_IPOL_MSK,
				   positive);
}

/**
 * @brief Enable or disable external lead-off detection
 * @param device - Device descriptor
 * @param enable - True to enable external lead-off detection, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_external_lead_off(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_DC_LEADS_CONFIGURATION,
				   MAX30009_EN_EXT_LOFF_MSK,
				   enable);
}

/**
 * @brief Enable or disable drive out-of-range detection
 * @param device - Device descriptor
 * @param enable - True to enable drive OOR detection, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_drive_oor_detect(struct max30009_dev *device, bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_DC_LEADS_CONFIGURATION,
				   MAX30009_EN_DRV_OOR_MSK,
				   enable);
}

/**
 * @brief Enable or disable ultra-low-power lead-on detection
 * @param device - Device descriptor
 * @param enable - True to enable ULP lead-on detection, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_ultra_low_power_lead_on(struct max30009_dev *device,
		bool enable)
{
	if (!device)
		return -ENODEV;

	return max30009_reg_update(device,
				   MAX30009_REG_DC_LEADS_CONFIGURATION,
				   MAX30009_EN_LON_DET_MSK,
				   enable);
}

/**
 * @brief Get detailed lead detection status
 * @param device - Device descriptor
 * @param status - Pointer to store detailed lead status information
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_get_detailed_lead_status(struct max30009_dev *device,
				      struct max30009_lead_status *status)
{
	int ret;
	uint8_t status2;

	if (!device || !status)
		return -EINVAL;

	ret = max30009_reg_read(device, MAX30009_REG_STATUS2, &status2);
	if (ret)
		return ret;

	status->leads_on = (status2 & MAX30009_STATUS2_LON_MSK) != 0;
	status->bioz_over = (status2 & MAX30009_STATUS2_BIOZ_OVER_MSK) != 0;
	status->bioz_undr = (status2 & MAX30009_STATUS2_BIOZ_UNDR_MSK) != 0;
	status->drv_oor = (status2 & MAX30009_STATUS2_DRV_OOR_MSK) != 0;
	status->dc_loff_ph = (status2 & MAX30009_STATUS2_DC_LOFF_PH_MSK) != 0;
	status->dc_loff_pl = (status2 & MAX30009_STATUS2_DC_LOFF_PL_MSK) != 0;
	status->dc_loff_nh = (status2 & MAX30009_STATUS2_DC_LOFF_NH_MSK) != 0;
	status->dc_loff_nl = (status2 & MAX30009_STATUS2_DC_LOFF_NL_MSK) != 0;

	return 0;
}

/**
 * @brief Configure lead bias resistor and enable for receive electrodes
 * @param device - Device descriptor
 * @param rbias_value - Bias resistor value (50M, 100M, 200M ohms)
 * @param enable_bip - True to enable bias on BIP (positive receive) electrode
 * @param enable_bin - True to enable bias on BIN (negative receive) electrode
 * @return 0 in case of success, negative error code otherwise
 * @note Lead bias pulls electrodes to mid-supply for proper DC operating point
 */
int max30009_set_lead_bias(struct max30009_dev *device,
			   enum max30009_rbias_val rbias_value,
			   bool enable_bip,
			   bool enable_bin)
{
	int ret;

	if (!device)
		return -ENODEV;

	if (rbias_value > MAX30009_RBIAS_200M_OHM)
		return -EINVAL;

	ret = max30009_reg_update(device,
				  MAX30009_REG_LEAD_BIAS_CONFIGURATION1,
				  MAX30009_RBIAS_VAL_MSK,
				  rbias_value);
	if (ret)
		return ret;

	ret = max30009_reg_update(device,
				  MAX30009_REG_LEAD_BIAS_CONFIGURATION1,
				  MAX30009_EN_RBIAS_BIP_MSK,
				  enable_bip);
	if (ret)
		return ret;

	return max30009_reg_update(device,
				   MAX30009_REG_LEAD_BIAS_CONFIGURATION1,
				   MAX30009_EN_RBIAS_BIN_MSK,
				   enable_bin);
}

/**
 * @brief Set complete PLL configuration
 * @param device - Device descriptor
 * @param pll_config - PLL configuration structure
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_pll_config(struct max30009_dev *device,
			    struct max30009_pll_config *pll_config)
{
	int ret;

	if (!device || !pll_config)
		return -EINVAL;

	if (pll_config->clk_fine_tune > 31)
		return -EINVAL;

	ret = max30009_set_clock_source(device,
					pll_config->use_external_clk,
					pll_config->use_32768_hz);
	if (ret)
		return ret;

	ret = max30009_clock_fine_tune(device, pll_config->clk_fine_tune);
	if (ret)
		return ret;

	ret = max30009_pll_set_dividers(device, pll_config->mdiv,
					pll_config->ndiv,
					pll_config->kdiv);
	if (ret)
		return ret;

	ret = max30009_pll_set_lock_window(device, pll_config->pll_lock_wndw);
	if (ret)
		return ret;

	memcpy(&device->pll_config, pll_config,
	       sizeof(struct max30009_pll_config));

	return 0;
}

/**
 * @brief Get complete PLL configuration
 * @param device - Device descriptor
 * @param pll_config - Pointer to store PLL configuration
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_get_pll_config(struct max30009_dev *device,
			    struct max30009_pll_config *pll_config)
{
	if (!device || !pll_config)
		return -EINVAL;

	memcpy(pll_config, &device->pll_config,
	       sizeof(struct max30009_pll_config));

	return 0;
}

/**
 * @brief Set complete FIFO configuration
 * @param device - Device descriptor
 * @param fifo_config - FIFO configuration structure
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_fifo_config(struct max30009_dev *device,
			     struct max30009_fifo_config *fifo_config)
{
	int ret;

	if (!device || !fifo_config)
		return -EINVAL;

	if (fifo_config->watermark < 1)
		return -EINVAL;

	ret = max30009_set_fifo_watermark(device, fifo_config->watermark);
	if (ret)
		return ret;

	ret = max30009_set_fifo_rollover(device,
					 fifo_config->fifo_rollover);
	if (ret)
		return ret;

	ret = max30009_set_a_full_type(device, fifo_config->a_full_type);
	if (ret)
		return ret;

	if (fifo_config->fifo_stat_clr) {
		ret = max30009_clear_fifo_status(device);
		if (ret)
			return ret;
	}

	memcpy(&device->fifo_config, fifo_config,
	       sizeof(struct max30009_fifo_config));

	return 0;
}

/**
 * @brief Get complete FIFO configuration
 * @param device - Device descriptor
 * @param fifo_config - Pointer to store FIFO configuration
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_get_fifo_config(struct max30009_dev *device,
			     struct max30009_fifo_config *fifo_config)
{
	if (!device || !fifo_config)
		return -EINVAL;

	memcpy(fifo_config, &device->fifo_config,
	       sizeof(struct max30009_fifo_config));

	return 0;
}

/**
 * @brief Set complete BioZ configuration
 * @param device - Device descriptor
 * @param bioz_config - BioZ configuration structure
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_bioz_config(struct max30009_dev *device,
			     struct max30009_bioz_config *bioz_config)
{
	int ret;
	uint8_t cfg1_val, cfg2_val, cfg3_val, cfg4_val;
	uint8_t cfg5_val, cfg6_val, cfg7_val;

	if (!device || !bioz_config)
		return -EINVAL;

	cfg1_val = no_os_field_prep(MAX30009_BIOZ_DAC_OSR_MSK,
				    bioz_config->dac_osr) |
		   no_os_field_prep(MAX30009_BIOZ_ADC_OSR_MSK,
				    bioz_config->adc_osr);

	cfg2_val = no_os_field_prep(MAX30009_BIOZ_DHPF_MSK, bioz_config->dhpf) |
		   no_os_field_prep(MAX30009_BIOZ_DLPF_MSK, bioz_config->dlpf) |
		   no_os_field_prep(MAX30009_BIOZ_CMP_MSK,
				    bioz_config->cmp_mode) |
		   (bioz_config->en_bioz_thresh ?
		    MAX30009_EN_BIOZ_THRESH_MSK : 0);

	cfg3_val = (bioz_config->ext_res ? MAX30009_BIOZ_EXT_RES_MSK : 0) |
		   (bioz_config->loff_rapid ? MAX30009_LOFF_RAPID_MSK : 0) |
		   no_os_field_prep(MAX30009_BIOZ_VDRV_MAG_MSK,
				    bioz_config->vdrv_mag) |
		   no_os_field_prep(MAX30009_BIOZ_IDRV_RGE_MSK,
				    bioz_config->idrv_rge) |
		   no_os_field_prep(MAX30009_BIOZ_DRV_MODE_MSK,
				    bioz_config->drv_mode);

	cfg4_val = (bioz_config->fast_manual ?
		    MAX30009_BIOZ_FAST_MANUAL_MSK : 0) |
		   (bioz_config->fast_start_en ?
		    MAX30009_BIOZ_FAST_START_EN_MSK : 0);

	cfg5_val = no_os_field_prep(MAX30009_BIOZ_AHPF_MSK, bioz_config->ahpf) |
		   (bioz_config->ina_mode ? MAX30009_BIOZ_INA_MODE_MSK : 0) |
		   (bioz_config->dm_dis ? MAX30009_BIOZ_DM_DIS_MSK : 0) |
		   no_os_field_prep(MAX30009_BIOZ_GAIN_MSK, bioz_config->gain);

	cfg6_val = (bioz_config->ext_cap ? MAX30009_BIOZ_EXT_CAP_MSK : 0) |
		   (bioz_config->dc_restore ?
		    MAX30009_BIOZ_DC_RESTORE_MSK : 0) |
		   (bioz_config->drv_reset ? MAX30009_BIOZ_DRV_RESET_MSK : 0) |
		   (bioz_config->dac_reset ? MAX30009_BIOZ_DAC_RESET_MSK : 0) |
		   no_os_field_prep(MAX30009_BIOZ_AMP_RGE_MSK,
				    bioz_config->amp_rge) |
		   no_os_field_prep(MAX30009_BIOZ_AMP_BW_MSK,
				    bioz_config->amp_bw);

	cfg7_val = (bioz_config->stbyon ? MAX30009_BIOZ_STBYON_MSK : 0) |
		   (bioz_config->q_clk_phase ?
		    MAX30009_BIOZ_Q_CLK_PHASE_MSK : 0) |
		   (bioz_config->i_clk_phase ?
		    MAX30009_BIOZ_I_CLK_PHASE_MSK : 0) |
		   (bioz_config->ina_chop_en ?
		    MAX30009_BIOZ_INA_CHOP_EN_MSK : 0) |
		   (bioz_config->ch_fsel ? MAX30009_BIOZ_CH_FSEL_MSK : 0);

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_CONFIGURATION1,
				 cfg1_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_CONFIGURATION2,
				 cfg2_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_CONFIGURATION3,
				 cfg3_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_CONFIGURATION4,
				 cfg4_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_CONFIGURATION5,
				 cfg5_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_CONFIGURATION6,
				 cfg6_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_CONFIGURATION7,
				 cfg7_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_LOW_THRESHOLD,
				 bioz_config->lo_thresh);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_HIGH_THRESHOLD,
				 bioz_config->hi_thresh);
	if (ret)
		return ret;

	memcpy(&device->bioz_config, bioz_config,
	       sizeof(struct max30009_bioz_config));

	return 0;
}

/**
 * @brief Get complete BioZ configuration
 * @param device - Device descriptor
 * @param bioz_config - Pointer to store BioZ configuration
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_get_bioz_config(struct max30009_dev *device,
			     struct max30009_bioz_config *bioz_config)
{
	if (!device || !bioz_config)
		return -EINVAL;

	memcpy(bioz_config, &device->bioz_config,
	       sizeof(struct max30009_bioz_config));

	return 0;
}

/**
 * @brief Set complete BioZ MUX configuration
 * @param device - Device descriptor
 * @param mux_config - BioZ MUX configuration structure
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_bioz_mux_config(struct max30009_dev *device,
				 struct max30009_bioz_mux_config *mux_config)
{
	int ret;
	uint8_t mux1_val, mux2_val, mux3_val;
	uint8_t max_assign = (uint8_t)NO_OS_GENMASK(1, 0);

	if (!device || !mux_config)
		return -EINVAL;

	if (mux_config->bip_assign > max_assign ||
	    mux_config->bin_assign > max_assign ||
	    mux_config->drvp_assign > max_assign ||
	    mux_config->drvn_assign > max_assign)
		return -EINVAL;

	mux1_val = no_os_field_prep(MAX30009_BMUX_RSEL_MSK,
				    mux_config->bmux_rsel) |
		   (mux_config->bmux_bist_en ? MAX30009_BMUX_BIST_EN_MSK : 0) |
		   (mux_config->connect_cal_only ?
		    MAX30009_CONNECT_CAL_ONLY_MSK : 0) |
		   (mux_config->mux_en ? MAX30009_MUX_EN_MSK : 0) |
		   (mux_config->cal_en ? MAX30009_CAL_EN_MSK : 0);

	mux2_val = no_os_field_prep(MAX30009_GSR_RSEL_MSK,
				    mux_config->bmux_gsr_rsel) |
		   (mux_config->gsr_load_en ? MAX30009_GSR_LOAD_EN_MSK : 0) |
		   (mux_config->en_ext_inload ?
		    MAX30009_EN_EXT_INLOAD_MSK : 0) |
		   (mux_config->en_int_inload ? MAX30009_EN_INT_INLOAD_MSK : 0);

	mux3_val = no_os_field_prep(MAX30009_BIP_ASSIGN_MSK,
				    mux_config->bip_assign) |
		   no_os_field_prep(MAX30009_BIN_ASSIGN_MSK,
				    mux_config->bin_assign) |
		   no_os_field_prep(MAX30009_DRVP_ASSIGN_MSK,
				    mux_config->drvp_assign) |
		   no_os_field_prep(MAX30009_DRVN_ASSIGN_MSK,
				    mux_config->drvn_assign);

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_MUX_CONFIGURATION1,
				 mux1_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_MUX_CONFIGURATION2,
				 mux2_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_BIOZ_MUX_CONFIGURATION3,
				 mux3_val);
	if (ret)
		return ret;

	memcpy(&device->bioz_mux_config, mux_config,
	       sizeof(struct max30009_bioz_mux_config));

	return 0;
}

/**
 * @brief Get complete BioZ MUX configuration
 * @param device - Device descriptor
 * @param mux_config - Pointer to store BioZ MUX configuration
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_get_bioz_mux_config(struct max30009_dev *device,
				 struct max30009_bioz_mux_config *mux_config)
{
	if (!device || !mux_config)
		return -EINVAL;

	memcpy(mux_config, &device->bioz_mux_config,
	       sizeof(struct max30009_bioz_mux_config));

	return 0;
}

/**
 * @brief Set complete lead detection configuration
 * @param device - Device descriptor
 * @param lead_config - Lead detection configuration structure
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_set_lead_detect_config(struct max30009_dev *device,
				    struct max30009_lead_detect_config *lead_config)
{
	int ret;
	uint8_t cfg_val, thresh_val;

	if (!device || !lead_config)
		return -EINVAL;

	cfg_val = (lead_config->en_lon_det ? MAX30009_EN_LON_DET_MSK : 0) |
		  (lead_config->en_loff_det ? MAX30009_EN_LOFF_DET_MSK : 0) |
		  (lead_config->en_ext_loff ? MAX30009_EN_EXT_LOFF_MSK : 0) |
		  (lead_config->en_drv_oor ? MAX30009_EN_DRV_OOR_MSK : 0) |
		  (lead_config->loff_ipol ? MAX30009_LOFF_IPOL_MSK : 0) |
		  no_os_field_prep(MAX30009_LOFF_IMAG_MSK,
				   lead_config->loff_imag);

	thresh_val = no_os_field_prep(MAX30009_LOFF_THRESH_MSK,
				      lead_config->loff_thresh);

	ret = max30009_reg_write(device, MAX30009_REG_DC_LEADS_CONFIGURATION,
				 cfg_val);
	if (ret)
		return ret;

	ret = max30009_reg_write(device, MAX30009_REG_DC_LEAD_DETECT_THRESHOLD,
				 thresh_val);
	if (ret)
		return ret;

	ret = max30009_set_lead_bias(device, lead_config->rbias_value,
				     lead_config->en_rbias_bip,
				     lead_config->en_rbias_bin);
	if (ret)
		return ret;

	memcpy(&device->lead_detect_config, lead_config,
	       sizeof(struct max30009_lead_detect_config));

	return 0;
}

/**
 * @brief Get complete lead detection configuration
 * @param device - Device descriptor
 * @param lead_config - Pointer to store lead detection configuration
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_get_lead_detect_config(struct max30009_dev *device,
				    struct max30009_lead_detect_config *lead_config)
{
	if (!device || !lead_config)
		return -EINVAL;

	memcpy(lead_config, &device->lead_detect_config,
	       sizeof(struct max30009_lead_detect_config));

	return 0;
}

/**
 * @brief Configure interrupt enables
 * @param device - MAX30009 device descriptor
 * @param int1_mask - Interrupt enable mask for INTERRUPT_ENABLE1 register
 * @param int2_mask - Interrupt enable mask for INTERRUPT_ENABLE2 register
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_interrupt_config(struct max30009_dev *device, uint8_t int1_mask,
			      uint8_t int2_mask)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = max30009_reg_write(device, MAX30009_REG_INTERRUPT_ENABLE1,
				 int1_mask);
	if (ret)
		return ret;

	return max30009_reg_write(device, MAX30009_REG_INTERRUPT_ENABLE2,
				  int2_mask);
}

/**
 * @brief Enable or disable a specific interrupt
 * @param device - MAX30009 device descriptor
 * @param interrupt - Interrupt type from max30009_interrupt enum
 * @param enable - True to enable, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_enable_interrupt(struct max30009_dev *device,
			      enum max30009_interrupt interrupt,
			      bool enable)
{
	uint8_t reg_addr;
	uint8_t mask;

	if (!device)
		return -ENODEV;

	switch (interrupt) {
	case MAX30009_INT_A_FULL:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE1;
		mask = MAX30009_A_FULL_EN_MSK;
		break;
	case MAX30009_INT_FIFO_DATA_RDY:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE1;
		mask = MAX30009_FIFO_DATA_RDY_EN_MSK;
		break;
	case MAX30009_INT_FREQ_UNLOCK:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE1;
		mask = MAX30009_FREQ_UNLOCK_EN_MSK;
		break;
	case MAX30009_INT_FREQ_LOCK:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE1;
		mask = MAX30009_FREQ_LOCK_EN_MSK;
		break;
	case MAX30009_INT_PHASE_UNLOCK:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE1;
		mask = MAX30009_PHASE_UNLOCK_EN_MSK;
		break;
	case MAX30009_INT_PHASE_LOCK:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE1;
		mask = MAX30009_PHASE_LOCK_EN_MSK;
		break;
	case MAX30009_INT_LON:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE2;
		mask = MAX30009_LON_EN_MSK;
		break;
	case MAX30009_INT_BIOZ_OVER:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE2;
		mask = MAX30009_BIOZ_OVER_EN_MSK;
		break;
	case MAX30009_INT_BIOZ_UNDR:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE2;
		mask = MAX30009_BIOZ_UNDR_EN_MSK;
		break;
	case MAX30009_INT_DRV_OOR:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE2;
		mask = MAX30009_DRV_OOR_EN_MSK;
		break;
	case MAX30009_INT_DC_LOFF_PH:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE2;
		mask = MAX30009_DC_LOFF_PH_EN_MSK;
		break;
	case MAX30009_INT_DC_LOFF_PL:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE2;
		mask = MAX30009_DC_LOFF_PL_EN_MSK;
		break;
	case MAX30009_INT_DC_LOFF_NH:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE2;
		mask = MAX30009_DC_LOFF_NH_EN_MSK;
		break;
	case MAX30009_INT_DC_LOFF_NL:
		reg_addr = MAX30009_REG_INTERRUPT_ENABLE2;
		mask = MAX30009_DC_LOFF_NL_EN_MSK;
		break;
	default:
		return -EINVAL;
	}

	return max30009_reg_update(device, reg_addr, mask, enable);
}

/**
 * @brief Clear interrupt status registers
 * @param device - MAX30009 device descriptor
 * @return 0 in case of success, negative error code otherwise
 * @note Reading the status registers clears them
 */
int max30009_clear_status(struct max30009_dev *device)
{
	uint8_t status[2];
	int ret;

	if (!device)
		return -ENODEV;

	ret = max30009_reg_read(device, MAX30009_REG_STATUS1, &status[0]);
	if (ret)
		return ret;

	return max30009_reg_read(device, MAX30009_REG_STATUS2, &status[1]);
}
