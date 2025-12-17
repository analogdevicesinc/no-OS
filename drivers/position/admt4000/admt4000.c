/***************************************************************************//**
 *   @file   admt4000.c
 *   @brief  Implementation of ADMT4000 Driver.
 *   @details This file contains the implementation of the ADMT4000 driver.
 *            It includes functions for initializing the device, reading and
 *            writing registers, computing CRC, ECC encoding, and more.
 *   @author Marc Sosa (marcpaolo.sosa@analog.com)
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 *   @author Louijie Compo  (louijie.compo@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "admt4000.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

static int admt4000_set_page(struct admt4000_dev *device, uint8_t page);
static int admt4000_ecc_config(struct admt4000_dev *device, bool is_en);
static int admt4000_update_ecc(struct admt4000_dev *device, uint16_t *ecc_val);
static int admt4000_config(struct admt4000_dev *device,
			   struct admt4000_init_param *init_param);
static int admt4000_gpio_output_en(struct admt4000_dev *device, uint8_t gpio,
				   bool gpio_en);

static uint8_t admt4000_ecc_control_registers[] = {
	ADMT4000_02_REG_GENERAL,
	ADMT4000_02_REG_DIGIOEN,
	ADMT4000_02_REG_ANGLECK,
	ADMT4000_02_REG_H1MAG,
	ADMT4000_02_REG_H1PH,
	ADMT4000_02_REG_H2MAG,
	ADMT4000_02_REG_H2PH,
	ADMT4000_02_REG_H3MAG,
	ADMT4000_02_REG_H3PH,
	ADMT4000_02_REG_H8MAG,
	ADMT4000_02_REG_H8PH,
};

/**
 * @brief Configure the ADMT4000 device with initialization parameters.
 * @param device - The device structure.
 * @param init_param - The structure for the device initial parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
static int admt4000_config(struct admt4000_dev *device,
			   struct admt4000_init_param *init_param)
{
	int ret, i;
	uint16_t temp;

	if (!device || !device->spi_desc || !init_param)
		return -ENODEV;

	/* Invalidate page cache after operations that might reset the device */
	device->page_cache_valid = false;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_AGP_REG_FAULT, &temp, NULL);
	if (ret)
		return ret;

	/* Configure GPIO functions for all pins */
	for (i = 0; i <= ADMT4000_MAX_GPIO_INDEX; i++) {
		ret = admt4000_gpio_config(device, i,
					   init_param->admt4000_gpio_ip[i].alt_mode,
					   init_param->admt4000_gpio_ip[i].output_en,
					   init_param->admt4000_gpio_ip[i].init_state);
		if (ret)
			return ret;
	}

	/* Delay for GPIO lines to settle before clearing faults */
	no_os_mdelay(10);

	ret = admt4000_clear_all_faults(device);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_AGP_REG_FAULT, &temp, NULL);
	if (ret)
		return ret;

	ret = admt4000_get_conv_mode(device, &device->is_one_shot);
	if (ret)
		return ret;

	ret = admt4000_set_page(device, 0);
	if (ret)
		return ret;

	/* Force restart acquisition sequence before returning */
	return admt4000_toggle_cnv(device);
}

/**
 * @brief Reinitialize device using stored configuration.
 *
 * Convenience function to restore device configuration after
 * magnetic reset or power cycle events using the parameters
 * from initial initialization.
 *
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_reinitialize(struct admt4000_dev *device)
{
	if (!device)
		return -EINVAL;

	return admt4000_config(device, &device->default_config);
}

/**
 * @brief Perform hardware reset via reset pin.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_hard_reset(struct admt4000_dev *device)
{
	int ret;

	if (!device)
		return -EINVAL;

	if (!device->gpio_reset_desc)
		return -ENODEV;

	/* Assert reset (active low) */
	ret = no_os_gpio_set_value(device->gpio_reset_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	/* Hold for 1ms */
	no_os_mdelay(ADMT4000_RESET_HOLD_TIME_MS);

	/* Deassert reset */
	ret = no_os_gpio_set_value(device->gpio_reset_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/* Wait for device startup */
	no_os_mdelay(ADMT4000_STARTUP_TIME_MS);

	/* Invalidate page cache */
	device->page_cache_valid = false;

	return 0;
}

/**
 * @brief Initializes the admt4000.
 * @param device - The device structure.
 * @param init_param - The structure for the device initial parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_init(struct admt4000_dev **device,
		  struct admt4000_init_param *init_param)
{
	struct admt4000_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct admt4000_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->default_config = *init_param;

	/* Initialize page cache as invalid */
	dev->page_cache_valid = false;
	dev->current_page = 0;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init_param);
	if (ret)
		goto err;

	/* Initialize optional reset GPIO */
	no_os_gpio_get_optional(&dev->gpio_reset_desc,
				&init_param->gpio_reset_ip);

	if (dev->gpio_reset_desc) {
		ret = no_os_gpio_direction_output(dev->gpio_reset_desc,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto err_gpio_reset;
	}

	/* Configure GPIO1 (CNV) if descriptor provided */
	no_os_gpio_get_optional(&dev->gpio_cnv_desc,
				&init_param->gpio_cnv_ip);
	if (dev->gpio_cnv_desc
	    && init_param->admt4000_gpio_ip[1].alt_mode) {
		ret = no_os_gpio_direction_output(dev->gpio_cnv_desc, NO_OS_GPIO_LOW);
		if (ret)
			goto err_gpio_cnv;
	}

	/* Configure GPIO3 (ACALC) if descriptor provided */
	no_os_gpio_get_optional(&dev->gpio_acalc_desc,
				&init_param->gpio_acalc_ip);
	if (dev->gpio_acalc_desc
	    && init_param->admt4000_gpio_ip[3].alt_mode) {
		ret = no_os_gpio_direction_input(dev->gpio_acalc_desc);
		if (ret)
			goto err_gpio_acalc;
	}

	/* Configure GPIO4 (FAULT) if descriptor provided */
	no_os_gpio_get_optional(&dev->gpio_fault_desc,
				&init_param->gpio_fault_ip);
	if (dev->gpio_fault_desc
	    && init_param->admt4000_gpio_ip[4].alt_mode) {
		ret = no_os_gpio_direction_input(dev->gpio_fault_desc);
		if (ret)
			goto err_gpio_fault;
	}

	ret = admt4000_config(dev, init_param);
	if (ret)
		goto err_gpio_fault;

	*device = dev;

	return 0;

err_gpio_fault:
	if (dev->gpio_fault_desc)
		no_os_gpio_remove(dev->gpio_fault_desc);

err_gpio_acalc:
	if (dev->gpio_acalc_desc)
		no_os_gpio_remove(dev->gpio_acalc_desc);

err_gpio_cnv:
	if (dev->gpio_cnv_desc)
		no_os_gpio_remove(dev->gpio_cnv_desc);

err_gpio_reset:
	if (dev->gpio_reset_desc)
		no_os_gpio_remove(dev->gpio_reset_desc);

	no_os_spi_remove(dev->spi_desc);

err:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by admt4000_init().
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_remove(struct admt4000_dev *device)
{
	if (!device)
		return -ENODEV;

	if (device->spi_desc) {
		no_os_spi_remove(device->spi_desc);
		device->spi_desc = NULL;
	}

	if (device->gpio_acalc_desc) {
		no_os_gpio_remove(device->gpio_acalc_desc);
		device->gpio_acalc_desc = NULL;
	}

	if (device->gpio_fault_desc) {
		no_os_gpio_remove(device->gpio_fault_desc);
		device->gpio_fault_desc = NULL;
	}

	if (device->gpio_cnv_desc) {
		no_os_gpio_remove(device->gpio_cnv_desc);
		device->gpio_cnv_desc = NULL;
	}

	if (device->gpio_reset_desc) {
		no_os_gpio_remove(device->gpio_reset_desc);
		device->gpio_reset_desc = NULL;
	}

	no_os_free(device);

	return 0;
}

/**
 * @brief Computes the CRC byte based on data input.
 * CRC polynomial: x^5 + x^2 + 1 (0x25)
 * Initial value: 0x1F
 * @param data_in - Data input for CRC computation.
 * @param crc_ret - Stores the computed CRC value.
 * @return 0 in case of success, negative error code otherwise.
 */
static int admt4000_compute_crc(uint32_t data_in, uint8_t *crc_ret)
{
	int i;
	uint8_t xor;
	uint8_t crc = 0x1F; /* Initial CRC value */

	if (!crc_ret)
		return -EINVAL;

	for (i = ADMT4000_MAX_CRC_BIT_LEN; i >= 0; i--) {
		xor = ((data_in >> i) & 0x1) ^ (crc >> 4);
		crc = ((crc << 1) & 0x1E) | xor;
		if (xor)
			crc ^= 0x04;

	}

	*crc_ret = crc & 0x1F;
	return 0;
}

/**
 * @brief Computes the Hamming Distance.
 * @param position - Bit position.
 * @param code_length - Length of code (original data + parity bits) in bits.
 * @param code - Array containing bytes to iterate through.
 * @return 0 or 1 (to be appended during ECC encoding process).
 */
static int admt4000_hamming_calc(uint8_t position, uint8_t code_length,
				 uint8_t *code)
{
	int count = 0, i, j;
	uint8_t bit_extract;

	i = position - 1;

	if (!code || position == 0 || position > code_length)
		return -EINVAL;

	while (i < code_length) {
		for (j = i; j < i + position; j++) {
			bit_extract = no_os_field_get(NO_OS_BIT((j & 0x7)),
						      code[(j / 8)]);
			if (bit_extract)
				count++;
		}

		i += (2 * position);
	}

	if (!(count % 2))
		return 0;

	return 1;
}

/**
 * @brief ECC Encoding process.
 * @param code - Stores the array where the computed codes will be stored.
 * @param input - Contains the input data to be encoded from certain ADMT4000
 *                registers.
 * @param input_size - Size of the input array.
 * @param ecc - Value to store ECC register compatible data.
 * @return 0 in case of success, negative error code otherwise.
 */
static int admt4000_ecc_encode(uint8_t *code, uint8_t *input,
			       uint8_t input_size,
			       uint8_t *ecc)
{
	int i = 0, j = 0, k = 0;
	int eff_pos;
	int value;
	uint8_t position;
	uint8_t xtract;
	uint8_t parity_num;
	uint8_t code_length;

	*ecc = 0;

	if (input_size > 16)
		return -EINVAL;

	/* Compute parity bits needed */
	parity_num = 0;
	while ((input_size * 8) > (1 << i) - (i + 1)) {
		parity_num += 1;
		i++;
	}

	/* In bits */
	code_length = parity_num + (input_size * 8);
	for (i = 0; i < code_length; i++) {
		if (i == ((1 << k) - 1)) {
			code[(i / 8)] &= (uint8_t)~NO_OS_BIT((i & 0x7));
			k++;
		} else {
			code[(i / 8)] &= (uint8_t)~NO_OS_BIT((i & 0x7));
			xtract = (uint8_t)no_os_field_get(NO_OS_BIT(j & 0x7), input[(j / 8)]);
			xtract = (uint8_t)no_os_field_prep(NO_OS_BIT((i & 0x7)), xtract);
			code[(i / 8)] |= xtract;
			j++;
		}
	}

	for (i = 0; i < parity_num; i++) {
		position = 1 << i;
		value = admt4000_hamming_calc(position, code_length, code);
		eff_pos = position - 1;
		code[(eff_pos / 8)] &= ~NO_OS_BIT(eff_pos & 0x7);
		code[(eff_pos / 8)] |= (NO_OS_BIT(eff_pos & 0x7) * value);
		*ecc |= (value << i);
	}

	value = 0;
	for (i = 0; i < code_length; i++) {
		uint8_t bit = no_os_field_get(NO_OS_BIT(i & 0x7),
					      code[(i / 8)]);
		if (bit)
			value++;
	}

	value &= 0x1;
	*ecc |= (value << parity_num);

	return 0;
}

/**
 * @brief Updates the ECC config register contents.
 * @param device - The device structure.
 * @param ecc_val - ECC Register value computed.
 * @return 0 in case of success, negative error code otherwise.
 */
static int admt4000_update_ecc(struct admt4000_dev *device, uint16_t *ecc_val)
{
	int ret;
	int i;
	uint16_t temp;
	uint8_t for_encode[15] = {0};
	uint8_t ecc[2] = {0};
	uint8_t encoded[16] = {0};

	if (!device)
		return -EINVAL;

	/* ECC_CONFIG1 (needs padding) involves the following registers:
	 * ADMT4000_02_REG_H3MAG HIGH BYTE
	 * ADMT4000_02_REG_H3PH
	 * ADMT4000_02_REG_H8MAG
	 * ADMT4000_02_REG_H8PH
	 */
	for (i = 7; i < 11; i++) {
		ret = admt4000_reg_read(device, admt4000_ecc_control_registers[i],
					&temp, NULL);
		if (ret)
			return ret;

		if (i != 7)
			no_os_put_unaligned_le16(temp, for_encode + (2 * (i - 8) + 1));
		else
			for_encode[14] = no_os_field_get(ADMT4000_HI_BYTE, temp);
	}

	ret = admt4000_ecc_encode(encoded, for_encode, 16, ecc);
	if (ret)
		return ret;

	/* ECC CONFIG0 (no padding) involves the following registers:
	 * ADMT4000_02_REG_GENERAL
	 * ADMT4000_02_REG_DIGIOEN
	 * ADMT4000_02_REG_ANGLECK
	 * ADMT4000_02_REG_H1MAG
	 * ADMT4000_02_REG_H1PH
	 * ADMT4000_02_REG_H2MAG
	 * ADMT4000_02_REG_H2PH
	 * ADMT4000_02_REG_H3MAG LOW BYTE
	*/
	for (i = 0; i < 8; i++) {
		ret = admt4000_reg_read(device, admt4000_ecc_control_registers[i],
					&temp, NULL);
		if (ret)
			return ret;

		if (i != 7)
			no_os_put_unaligned_le16(temp, for_encode + (2 * i));
		else
			for_encode[14] = no_os_field_get(ADMT4000_LOW_BYTE, temp);
	}

	/* ECC1 (needs padding) */
	ret = admt4000_ecc_encode(encoded, for_encode, 16, ecc + 1);
	if (ret)
		return ret;

	/* Format ECC data to write in actual register */
	temp = no_os_get_unaligned_be16(ecc);

	/* Store in return variable if requested */
	if (ecc_val)
		*ecc_val = temp;

	/* All registers of interest are in page 2 */
	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	/* Update ECC register contents */
	ret = admt4000_reg_write(device, ADMT4000_02_REG_ECCEDC, temp);
	if (ret)
		return ret;

	/* Enable ECC functionality */
	return admt4000_ecc_config(device, true);
}

/**
 * @brief Reads data from a certain ADMT400x Register.
 * @param device - The device structure.
 * @param reg_addr - Address of the register.
 * @param reg_data - Stores data read from the register.
 * @param verif - Stores the Fault, and CRC bits returned.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_reg_read(struct admt4000_dev *device, uint8_t reg_addr,
		      uint16_t *reg_data, uint8_t *verif)
{
	int ret;
	uint32_t crc_data_in;
	uint8_t buf[4];
	uint8_t crc;
	uint8_t temp;

	if (!device || !device->spi_desc || !reg_data)
		return -EINVAL;

	buf[0] = reg_addr;

	ret = no_os_spi_write_and_read(device->spi_desc, buf, 4);
	if (ret)
		return ret;

	temp = no_os_field_get(ADMT4000_LIFE_CTR | ADMT4000_FAULT_MASK, buf[3]);

	*reg_data = no_os_get_unaligned_be16(buf + 1);
	crc_data_in = (((uint32_t)reg_addr << 16) | *reg_data) << 3 | temp;
	ret = admt4000_compute_crc(crc_data_in, &crc);
	if (ret)
		return ret;

	if (no_os_field_get(ADMT4000_RCV_CRC, buf[3]) != crc)
		return -EBADMSG;

	if (verif)
		*verif = buf[3];

	return 0;
}

/**
 * @brief Writes data to a certain ADMT400x Register.
 * @param device - The device structure.
 * @param reg_addr - Address of the register.
 * @param reg_data - Data to write to the chosen register (16 bit wide).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_reg_write(struct admt4000_dev *device, uint8_t reg_addr,
		       uint16_t reg_data)
{
	int ret;
	uint32_t crc_data_in;
	uint8_t buf[4];
	uint8_t crc;

	if (!device || !device->spi_desc)
		return -EINVAL;

	reg_addr = reg_addr | ADMT4000_WR_EN;

	crc_data_in = (((uint32_t)reg_addr << 16) | reg_data) << 3;
	ret = admt4000_compute_crc(crc_data_in, &crc);
	if (ret)
		return ret;

	buf[0] = reg_addr;
	no_os_put_unaligned_be16(reg_data, buf + 1);
	buf[3] = crc;

	return no_os_spi_write_and_read(device->spi_desc, buf, 4);
}

/**
 * @brief Updates the contents of selected register.
 * @param device - The device structure.
 * @param reg_addr - Address of the register.
 * @param update_mask - Mask to update to selected register bit/s.
 * @param update_val - Value used to update on selected register.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_reg_update(struct admt4000_dev *device, uint8_t reg_addr,
			uint16_t update_mask, uint16_t update_val)
{
	int ret;
	uint16_t temp;

	if (!device || !device->spi_desc)
		return -EINVAL;

	ret = admt4000_reg_read(device, reg_addr, &temp, NULL);
	if (ret)
		return ret;

	temp &= ~update_mask;
	temp |= update_val;

	return admt4000_reg_write(device, reg_addr, temp);
}

/**
 * @brief Configures the page settings.
 * @param device - The device structure.
 * @param page - Page to switch to (0 or 2).
 * @return 0 in case of success, negative error code otherwise.
 */
static int admt4000_set_page(struct admt4000_dev *device, uint8_t page)
{
	int ret;

	if (!device || !device->spi_desc)
		return -EINVAL;

	if (page != 0 && page != 2)
		return -EINVAL;

	/* Check cache before switching */
	if (device->page_cache_valid && device->current_page == page) {
		/* Already on correct page */
		return 0;
	}

	/* Perform actual page switch */
	ret = admt4000_reg_update(device, ADMT4000_AGP_REG_CNVPAGE,
				  ADMT4000_PAGE_MASK,
				  no_os_field_prep(ADMT4000_PAGE_MASK, page));
	if (ret)
		return ret;

	/* Update cache */
	device->current_page = page;
	device->page_cache_valid = true;

	return 0;
}

/**
 * @brief Toggle the CNV to stop/start conversion.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_toggle_cnv(struct admt4000_dev *device)
{
	int ret;
	uint16_t temp;

	if (!device || !device->spi_desc)
		return -EINVAL;

	if (device->gpios[1].alt_mode) {
		if (!device->gpio_cnv_desc)
			return -EPERM;

		ret = no_os_gpio_set_value(device->gpio_cnv_desc, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		no_os_udelay(ADMT4000_CNV_PULSE_WIDTH_US);
		ret = no_os_gpio_set_value(device->gpio_cnv_desc, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	} else {
		temp = no_os_field_prep(ADMT4000_CNV_EDGE_MASK,
					ADMT4000_RISING_EDGE);
		ret = admt4000_reg_update(device, ADMT4000_AGP_REG_CNVPAGE,
					  ADMT4000_CNV_EDGE_MASK, temp);
		if (ret)
			return ret;

		temp = no_os_field_prep(ADMT4000_CNV_EDGE_MASK,
					ADMT4000_FALLING_EDGE);
		ret = admt4000_reg_update(device, ADMT4000_AGP_REG_CNVPAGE,
					  ADMT4000_CNV_EDGE_MASK, temp);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Reads ANGLE and ABSANGLE register contents in 1 CS frame.
 * @param device - The device structure.
 * @param angle_data - Stores data read from the registers.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_raw_angle_read(struct admt4000_dev *device, uint16_t *angle_data)
{
	int ret;
	int i;
	uint32_t crc_data_in;
	uint8_t buf[8] = { 0 };
	uint8_t crc;
	uint8_t temp;
	uint8_t admt4000_angle_regs[] = {
		ADMT4000_AGP_REG_ABSANGLE,
		ADMT4000_AGP_REG_ANGLE,
	};

	if (!device || !device->spi_desc || !angle_data)
		return -EINVAL;

	/* Set up buffer with register addresses to read */
	for (i = 0; i < 2; i++)
		buf[0 + (4 * i)] = admt4000_angle_regs[i];

	ret = no_os_spi_write_and_read(device->spi_desc, buf, 8);
	if (ret)
		return ret;

	/* Extract angle data from buffer (4 bytes each) */
	for (i = 0; i < 2; i++) {
		temp = no_os_field_get(ADMT4000_LIFE_CTR | ADMT4000_FAULT_MASK,
				       buf[3 + 4 * i]);

		*angle_data = no_os_get_unaligned_be16(buf + 1 + 4 * i);
		crc_data_in = (((uint32_t)admt4000_angle_regs[i] << 16) |
			       (*angle_data)) << 3 | temp;
		ret = admt4000_compute_crc(crc_data_in, &crc);
		if (ret)
			return ret;

		if (no_os_field_get(ADMT4000_RCV_CRC, buf[3 + 4 * i]) != crc)
			return -EBADMSG;

		angle_data++;
	}

	return 0;
}

/**
 * @brief Get the number of turns and angle information (Raw).
 * @param device - The device structure.
 * @param turns - Stores the number of turns read (Raw).
 * @param angle - Stores the angle values read (Raw, extracted).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_raw_turns_and_angle(struct admt4000_dev *device,
				     uint8_t *turns, uint16_t *angle)
{
	int i, ret;
	uint16_t admt4000_angle_masks[] = {
		ADMT4000_ABS_ANGLE_MASK,
		ADMT4000_ANGLE_MASK,
	};

	if (!device || !device->spi_desc || !turns || !angle)
		return -EINVAL;

	ret = admt4000_raw_angle_read(device, angle);
	if (ret)
		return ret;

	*turns = no_os_field_get(ADMT4000_QUARTER_TURN_CNT_MASK, angle[0]);

	for (i = 0; i < 2; i++)
		angle[i] = no_os_field_get(admt4000_angle_masks[i], angle[i]);

	return 0;
}

/**
 * @brief Convert raw quarter turn count to equivalent signed value.
 * @param raw_turns - Raw quarter turn count read from device.
 * @param quarter_turns - Pointer to store the signed quarter turn count.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_quarter_turns_cnt(uint8_t raw_turns, int16_t *quarter_turns)
{
	if (!quarter_turns)
		return -EINVAL;

	if (raw_turns > ADMT4000_QUARTER_TURNS_MAX)
		*quarter_turns = no_os_sign_extend16((int16_t)raw_turns -
						     ADMT4000_QUARTER_TURNS_RES, 7);
	else
		*quarter_turns = (int16_t)raw_turns;

	return 0;
}

/**
 * @brief Get the cosine value from the ADMT4000 device.
 * This function reads the cosine portion of the AMR angle sensor without post-
 * processing.
 * @param device - The device structure.
 * @param val - Pointer to a variable where cosine value will be stored.
 * @param is_new_data - Optional pointer to store new data flag (can be NULL).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_cos(struct admt4000_dev *device, int16_t *val,
		     bool *is_new_data)
{
	int ret;
	uint16_t raw_val;

	if (!device || !device->spi_desc || !val)
		return -EINVAL;

	ret = admt4000_set_page(device, 0);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_00_REG_COSINE, &raw_val, NULL);
	if (ret)
		return ret;

	*val = no_os_sign_extend16(no_os_field_get(ADMT4000_RAW_COSINE_MASK, raw_val),
				   13);

	if (is_new_data)
		*is_new_data = (bool)no_os_field_get(ADMT4000_NEW_DATA_MASK, raw_val);

	return 0;
}

/**
 * @brief Retrieve the sine value from the ADMT4000 device.
 * This function reads the sine portion of the AMR angle sensor without post-
 * processing.
 * @param device - The device structure.
 * @param val - Pointer to a variable where sine value will be stored.
 * @param is_new_data - Optional pointer to store new data flag (can be NULL).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_sin(struct admt4000_dev *device, int16_t *val,
		     bool *is_new_data)
{
	int ret;
	uint16_t raw_val;

	if (!device || !device->spi_desc || !val)
		return -EINVAL;

	ret = admt4000_set_page(device, 0);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_00_REG_SINE, &raw_val, NULL);
	if (ret)
		return ret;

	*val = no_os_sign_extend16(no_os_field_get(ADMT4000_RAW_SINE_MASK, raw_val),
				   13);

	if (is_new_data)
		*is_new_data = (bool)no_os_field_get(ADMT4000_NEW_DATA_MASK, raw_val);

	return 0;
}

/**
 * @brief Get status of selected GPIO.
 * @param device - The device structure.
 * @param gpio - GPIO number (0 to 5).
 * @param logic - Stores the truth value of selected GPIO.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_gpio(struct admt4000_dev *device, uint8_t gpio, bool *logic)
{
	int ret;
	uint16_t temp;

	if (!device || !device->spi_desc || !logic)
		return -EINVAL;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	ret = admt4000_reg_read(device, ADMT4000_AGP_REG_DGIO, &temp, NULL);
	if (ret)
		return ret;

	*logic = (bool)no_os_field_get(ADMT4000_GPIO_LOGIC(gpio), temp);

	return 0;
}

/**
 * @brief Configure selected GPIO.
 * @param device - The device structure.
 * @param gpio - GPIO number (0 to 5).
 * @param logic - Logic level to set for GPIO.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_gpio(struct admt4000_dev *device, uint8_t gpio, bool logic)
{
	int ret;
	uint16_t temp;

	if (!device || !device->spi_desc)
		return -EINVAL;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	temp = no_os_field_prep(ADMT4000_GPIO_LOGIC(gpio),
				(uint8_t)logic);
	ret = admt4000_reg_update(device, ADMT4000_AGP_REG_DGIO,
				  ADMT4000_GPIO_LOGIC(gpio), temp);

	if (ret)
		return ret;

	device->gpios[gpio].logic_state = logic;

	return 0;
}

/**
 * @brief Clear all faults from fault registers.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_clear_all_faults(struct admt4000_dev *device)
{
	if (!device)
		return -EINVAL;

	return admt4000_reg_update(device, ADMT4000_AGP_REG_FAULT,
				   ADMT4000_ALL_FAULTS, 0);
}

/**
 * @brief Get all fault flags from the fault register.
 * @param device - The device structure.
 * @param faults - Pointer to store the fault register value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_faults(struct admt4000_dev *device, uint16_t *faults)
{
	int ret;

	if (!device || !faults)
		return -EINVAL;

	ret = admt4000_reg_read(device, ADMT4000_AGP_REG_FAULT, faults, NULL);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get RADIUS vector value.
 * @param device - The device structure.
 * @param radius - Stores the radius value read.
 * @param is_new_data - Optional pointer to store new data flag (can be NULL).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_radius(struct admt4000_dev *device, uint16_t *radius,
			bool *is_new_data)
{
	int ret;
	uint16_t raw_temp;

	if (!device || !radius)
		return -EINVAL;

	ret = admt4000_set_page(device, 0);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_00_REG_RADIUS, &raw_temp, NULL);
	if (ret)
		return ret;

	*radius = no_os_field_get(ADMT4000_RADIUS_MASK, raw_temp);

	if (is_new_data)
		*is_new_data = (bool)no_os_field_get(ADMT4000_NEW_DATA_MASK, raw_temp);

	return 0;
}

/**
 * @brief Convert raw RADIUS value to scaled mV/V.
 * @param raw_radius - Raw RADIUS value.
 * @param radius - Stores Magnitude value (mV/V).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_radius_raw_to_scaled(uint16_t raw_radius, float *radius)
{
	if (!radius)
		return -EINVAL;

	*radius = ((float)raw_radius * ADMT4000_RADIUS_RES) / MICRO;

	return 0;
}

/**
 * @brief Get raw temperature reading.
 * @param device - The device structure.
 * @param temp - Raw temperature ADC value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_temp(struct admt4000_dev *device, uint16_t *temp)
{
	int ret;
	uint16_t raw_temp;

	if (!device || !temp)
		return -EINVAL;

	ret = admt4000_set_page(device, 0);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_00_REG_TMP, &raw_temp, NULL);
	if (ret)
		return ret;

	*temp = no_os_field_get(ADMT4000_TEMP_MASK, raw_temp);

	return 0;
}

/**
 * @brief Convert raw temperature value to scaled temperature (degrees Celsius).
 * @param temp_raw - Raw temperature ADC value.
 * @param temp - Temperature data (degrees Celsius).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_temp_raw_to_scaled(uint16_t temp_raw, float *temp)
{
	if (!temp)
		return -EINVAL;

	/* Temperature conversion formula:
	 * T(°C) = (ADC - 1168.0) / 15.66
	 * Where: 1168.0 = ADC offset at 0°C
	 *        15.66  = ADC counts per degree Celsius (sensitivity)
	 * T(°C) = (ADC + (-1168)) * 1000 / 15660 to avoid float operations
	 * and maintain precision (milli-degrees Celsius).
	 */
	*temp = ((temp_raw + ADMT4000_TEMP_OFFSET) * MILLI /
		 ADMT4000_TEMP_SCALE_MILLIS);

	return 0;
}

/**
 * @brief Convert raw angle value to scaled degrees
 * @param raw_angle - Input raw angle value
 * @param scaled_angle - Output scaled angle in degrees
 * @return 0 in case of success, negative error code otherwise.
 * @details Scales raw angle register value to physical units using
 *          device-specific resolution factors.
 *          Angle: raw * 360 / 4096 (for 12-bit angle resolution)
 */
int admt4000_angle_raw_to_scaled(uint16_t raw_angle, float *scaled_angle)
{
	if (!scaled_angle)
		return -EINVAL;

	*scaled_angle = (float)raw_angle * ADMT4000_FULL_TURN_DEGREES /
			ADMT4000_ANGLE_RES;

	return 0;
}

/**
 * @brief Convert raw harmonic coefficients to scaled format
 * @param raw_coeff - Input raw calibration coefficients
 * @param scaled_mag - Output scaled magnitude
 * @param scaled_phase - Output scaled phase
 * @return 0 in case of success, negative error code otherwise.
 * @details Scales raw register values to physical units using device-specific
 *          resolution factors.
 *          Magnitude: raw * 0.005493/0.6072
 *          Phase: raw * 0.087891
 */
int admt4000_harmonic_raw_to_scaled(
	const struct admt4000_harmonic_coeff *raw_coeff,
	float *scaled_mag, float *scaled_phase)
{
	if (!raw_coeff || !scaled_mag || !scaled_phase)
		return -EINVAL;

	*scaled_mag = (float)raw_coeff->magnitude_raw * ADMT4000_HMAG_SCALER;
	*scaled_phase = (float)raw_coeff->phase_raw * ADMT4000_HPHA_SCALER;

	return 0;
}

/**
 * @brief Convert scaled harmonic values to raw coefficients
 * @param scaled_mag - Input scaled magnitude
 * @param scaled_phase - Input scaled phase
 * @param raw_coeff - Output raw calibration coefficients
 * @return 0 in case of success, negative error code otherwise.
 * @details Inverse conversion for writing scaled values to hardware registers.
 *          Uses CORDIC scaler (0.6072) to compensate for hardware scaling.
 */
int admt4000_harmonic_scaled_to_raw(
	uint32_t scaled_mag, uint32_t scaled_phase,
	struct admt4000_harmonic_coeff *raw_coeff)
{
	if (!raw_coeff)
		return -EINVAL;

	raw_coeff->magnitude_raw = (uint16_t)((scaled_mag) / ADMT4000_HMAG_SCALER);
	raw_coeff->phase_raw = (uint16_t)((scaled_phase) / ADMT4000_HPHA_SCALER);

	return 0;
}

/**
 * @brief Gets storage general config.
 * @param device - The device structure.
 * @param storage - Stores the storage bits read.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_storage_config(struct admt4000_dev *device, uint8_t *storage)
{
	int ret;
	uint16_t temp;

	if (!device || !storage)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_02_REG_GENERAL, &temp, NULL);
	if (ret)
		return ret;

	*storage = no_os_field_prep(ADMT4000_STORAGE_BIT7,
				    no_os_field_get(ADMT4000_STORAGE_MASK_BIT7, temp));
	*storage |= no_os_field_prep(ADMT4000_STORAGE_BIT6,
				     no_os_field_get(ADMT4000_STORAGE_MASK_BIT6, temp));
	*storage |= no_os_field_prep(ADMT4000_STORAGE_BIT5_3,
				     no_os_field_get(ADMT4000_STORAGE_MASK_BIT5_3, temp));
	*storage |= no_os_field_prep(ADMT4000_STORAGE_BIT2_0,
				     no_os_field_get(ADMT4000_STORAGE_MASK_BIT2_0, temp));

	return 0;
}

/**
 * @brief Sets storage general config.
 * @param device - The device structure.
 * @param storage - Data to write on storage bits.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_storage_config(struct admt4000_dev *device, uint8_t storage)
{
	int ret;
	uint16_t temp = 0;

	if (!device)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	temp = no_os_field_prep(ADMT4000_STORAGE_MASK_BIT7,
				no_os_field_get(ADMT4000_STORAGE_BIT7, storage));
	temp |= no_os_field_prep(ADMT4000_STORAGE_MASK_BIT6,
				 no_os_field_get(ADMT4000_STORAGE_BIT6, storage));
	temp |= no_os_field_prep(ADMT4000_STORAGE_MASK_BIT5_3,
				 no_os_field_get(ADMT4000_STORAGE_BIT5_3, storage));
	temp |= no_os_field_prep(ADMT4000_STORAGE_MASK_BIT2_0,
				 no_os_field_get(ADMT4000_STORAGE_BIT2_0, storage));

	ret = admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				  ADMT4000_STORAGE_MASK_FULL, temp);
	if (ret)
		return ret;

	/* Update ECC registers and re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/**
 * @brief Gets current filter setting.
 * @param device - The device structure.
 * @param is_filtered - Stores the logic level of the filter enable bit.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_angle_filt(struct admt4000_dev *device, bool *is_filtered)
{
	int ret;
	uint16_t temp;

	if (!device || !is_filtered)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_02_REG_GENERAL, &temp, NULL);
	if (ret)
		return ret;

	*is_filtered = (bool)no_os_field_get(ADMT4000_ANGL_FILT_MASK, temp);

	return 0;
}

/**
 * @brief Sets the filter setting.
 * @param device - The device structure.
 * @param is_filtered - Set true to enable filter, false otherwise.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_angle_filt(struct admt4000_dev *device, bool is_filtered)
{
	int ret;
	uint16_t temp;

	if (!device)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	temp = no_os_field_prep(ADMT4000_ANGL_FILT_MASK, (uint8_t)is_filtered);
	ret = admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				  ADMT4000_ANGL_FILT_MASK, temp);

	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/**
 * @brief Gets 8th harmonic correction source.
 * @param device - The device structure.
 * @param source - The harmonic correction source.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_h8_ctrl(struct admt4000_dev *device,
			 enum admt4000_harmonic_corr_src *source)
{
	int ret;
	uint16_t temp;

	if (!device || !source)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_02_REG_GENERAL, &temp, NULL);
	if (ret)
		return ret;

	*source = (enum admt4000_harmonic_corr_src)no_os_field_get(
			  ADMT4000_H8_CTRL_MASK, temp);

	return 0;
}

/**
 * @brief Sets 8th harmonic correction source.
 * @param device - The device structure.
 * @param source - The harmonic correction source.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_h8_ctrl(struct admt4000_dev *device,
			 enum admt4000_harmonic_corr_src source)
{
	int ret;
	uint16_t temp;

	if (!device)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	temp = no_os_field_prep(ADMT4000_H8_CTRL_MASK, (uint8_t)source);
	ret = admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				  ADMT4000_H8_CTRL_MASK, temp);

	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/**
 * @brief Obtains conversion synchronization mode.
 * @param device - The device structure.
 * @param mode - The conversion synchronization mode configured.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_conv_sync_mode(struct admt4000_dev *device,
				enum admt4000_conv_sync_mode *mode)
{
	int ret;
	uint16_t temp;

	if (!device || !mode)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_02_REG_GENERAL, &temp, NULL);
	if (ret)
		return ret;

	*mode = no_os_field_get(ADMT4000_CONV_SYNC_MODE_MASK, temp);

	return 0;
}

/**
 * @brief Configures conversion synchronization mode.
 * @param device - The device structure.
 * @param mode - The conversion synchronization mode to configure.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_conv_sync_mode(struct admt4000_dev *device,
				enum admt4000_conv_sync_mode mode)
{
	int ret;

	if (!device)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	ret = admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				  ADMT4000_CONV_SYNC_MODE_MASK,
				  no_os_field_prep(ADMT4000_CONV_SYNC_MODE_MASK,
						  mode));
	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/**
 * @brief Obtains the conversion mode.
 * @param device - The device structure.
 * @param is_one_shot - Stores the conversion mode bit value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_conv_mode(struct admt4000_dev *device, bool *is_one_shot)
{
	int ret;
	uint16_t temp;

	if (!device || !is_one_shot)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_02_REG_GENERAL, &temp, NULL);
	if (ret)
		return ret;

	*is_one_shot = (bool)no_os_field_get(ADMT4000_CNV_MODE_MASK, temp);

	return 0;
}

/**
 * @brief Configures the conversion mode: one shot or continuous.
 * @param device - The device structure.
 * @param is_one_shot - Set to true for one shot setting, false otherwise.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_conv_mode(struct admt4000_dev *device, bool is_one_shot)
{
	int ret;
	uint16_t temp;

	if (!device)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	temp = no_os_field_prep(ADMT4000_CNV_MODE_MASK, (uint8_t)is_one_shot);
	ret = admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				  ADMT4000_CNV_MODE_MASK, (uint16_t)temp);

	if (ret)
		return ret;

	device->is_one_shot = is_one_shot;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

static int admt4000_gpio_output_en(struct admt4000_dev *device, uint8_t gpio,
				   bool enable)
{
	int ret;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	if (enable)
		ret = admt4000_reg_update(device, ADMT4000_02_REG_DIGIOEN,
					  ADMT4000_DIG_IO_EN(gpio),
					  ADMT4000_DIG_IO_EN(gpio));
	else
		ret = admt4000_reg_update(device, ADMT4000_02_REG_DIGIOEN,
					  ADMT4000_DIG_IO_EN(gpio), 0);

	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/**
 * @brief Configure GPIO mode, direction, and initial state.
 *
 * GPIO Functions
 *
 * The ADMT4000 has 6 GPIO pins (GPIO0-GPIO5) that can be configured as either:
 * - Normal GPIO mode: Standard digital I/O pins
 * - Alternate function mode: Device-specific functions
 *
 * Alternate Functions:
 * - GPIO0: BUSY (output only) - Conversion busy indicator
 * - GPIO1: CNV (input only) - External conversion trigger (active low)
 * - GPIO3: ACALC (output only) - Auto-calibration trigger
 * - GPIO5: BOOTLOAD (output only) - Bootloader mode control
 *
 * When a GPIO is in alternate function mode:
 * - The device hardware controls the pin behavior
 * - Normal GPIO operations (set_gpio, get_gpio) may have no effect
 * - Direction is determined by the alternate function
 * - Application should not attempt to control the pin manually
 *
 * GPIO direction defaults to input mode at init time. If direction configured
 * at init conflicts with the selected alternate function, alternate function's
 * required direction takes precedence.
 *
 * @param device - The device structure.
 * @param gpio - GPIO number (0-5).
 * @param alt_mode - Alternate function flag (true = alt, false = normal).
 * @param output_en - Direction flag (true = output, false = input). Ignored in
 *                    alt mode.
 * @param init_state - Initial output state (true = high, false = low). Only
 *                     applied if output_en is true and alt_mode is false.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_gpio_config(struct admt4000_dev *device, uint8_t gpio,
			 bool alt_mode, bool output_en, bool init_state)
{
	int ret;
	bool direction;

	if (!device || gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	if (alt_mode)
		ret = admt4000_reg_update(device, ADMT4000_02_REG_DIGIOEN,
					  ADMT4000_GPIO_FUNC(gpio), 0);
	else
		ret = admt4000_reg_update(device, ADMT4000_02_REG_DIGIOEN,
					  ADMT4000_GPIO_FUNC(gpio),
					  ADMT4000_GPIO_FUNC(gpio));

	if (ret)
		return ret;

	/* Store alternate function state */
	device->gpios[gpio].alt_mode = alt_mode;

	/* Determine GPIO direction based on alt function or user preference */
	switch (gpio) {
	case 0:  /* GPIO0: BUSY */
		if (alt_mode) {
			/* BUSY: Output only */
			direction = true;
		} else {
			/* Normal GPIO: Use provided direction */
			direction = output_en;
		}
		break;

	case 1:  /* GPIO1: CNV */
		if (alt_mode) {
			/* CNV: Input only */
			direction = false;
		} else {
			/* Normal GPIO: Use provided direction */
			direction = output_en;
		}
		break;

	case 2:  /* GPIO2: Reserved */
		/* Use provided direction (normally input) */
		direction = output_en;
		break;

	case 3:  /* GPIO3: ACALC */
		if (alt_mode) {
			/* ACALC: Output only */
			direction = true;
		} else {
			/* Normal GPIO: Use provided direction */
			direction = output_en;
		}
		break;

	case 4:  /* GPIO4: Normal GPIO */
		/* Use provided direction */
		direction = output_en;
		break;

	case 5:  /* GPIO5: BOOTLOAD */
		if (alt_mode) {
			/* BOOTLOAD: Output only */
			direction = true;
		} else {
			/* Normal GPIO: Use provided direction */
			direction = output_en;
		}
		break;

	default:
		return -EINVAL;
	}

	/* Configure GPIO direction */
	ret = admt4000_gpio_output_en(device, gpio, direction);
	if (ret)
		return ret;

	/* Store direction state */
	device->gpios[gpio].output_en = direction;

	/* Set initial GPIO state if configured as output and not in alt mode */
	if (!alt_mode && direction) {
		ret = admt4000_set_gpio(device, gpio, init_state);
		if (ret)
			return ret;
	}

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/**
 * @brief Get current GPIO function configuration.
 * @param device - The device structure.
 * @param gpio - GPIO number (0-5).
 * @param alt_mode - Pointer to store result (true = alt, false = normal).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_gpio_func(struct admt4000_dev *device, uint8_t gpio,
			   bool *alt_mode)
{
	if (!device || !alt_mode)
		return -EINVAL;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	*alt_mode = device->gpios[gpio].alt_mode;

	return 0;
}

/**
 * @brief Get current conversion count.
 * @param device - The device structure.
 * @param cnt - Stores the count data.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_cnv_cnt(struct admt4000_dev *device, uint8_t *cnt)
{
	int ret;
	uint16_t temp;

	if (!device || !cnt)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	ret = admt4000_reg_read(device, ADMT4000_02_REG_CNVCNT, &temp, NULL);
	if (ret)
		return ret;

	*cnt = no_os_field_get(ADMT4000_CNV_CTR_MASK, temp);

	return 0;
}

/**
 * @brief Read Unique ID from register/s.
 * @param device - The device structure.
 * @param id_num - The ID register to read from.
 * @param id - Stores the ID value read.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_id(struct admt4000_dev *device, uint8_t id_num, uint16_t *id)
{
	int ret;
	uint16_t temp;

	if (!device || !id)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	switch (id_num) {
	case 0:
		ret = admt4000_reg_read(device, ADMT4000_02_REG_UNIQD0, &temp, NULL);
		break;
	case 1:
		ret = admt4000_reg_read(device, ADMT4000_02_REG_UNIQD1, &temp, NULL);
		break;
	case 2:
		ret = admt4000_reg_read(device, ADMT4000_02_REG_UNIQD2, &temp, NULL);
		break;
	case 3:
		ret = admt4000_reg_read(device, ADMT4000_02_REG_UNIQD3, &temp, NULL);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	if (id_num == 0)
		*id = no_os_field_get(ADMT4000_ID0_MASK, temp);
	else
		*id = temp;

	return 0;
}

static int admt4000_ecc_config(struct admt4000_dev *device, bool is_en)
{
	int ret;

	if (!device)
		return -EINVAL;

	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	if (is_en)
		return admt4000_reg_write(device, ADMT4000_02_REG_ECCDIS,
					  ADMT4000_ECC_EN_COMM);

	return admt4000_reg_write(device, ADMT4000_02_REG_ECCDIS,
				  ADMT4000_ECC_DIS_COMM);
}

/**
 * @brief Get harmonic configuration (magnitude and phase together).
 * @param device - The device structure.
 * @param harmonic - Harmonic number (1, 2, 3, or 8).
 * @param config - Output structure for magnitude and phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_harmonic(struct admt4000_dev *device, uint8_t harmonic,
			  struct admt4000_harmonic_coeff *config)
{
	int ret;
	uint16_t mag_reg, phase_reg;
	uint16_t mag_temp, phase_temp;
	uint16_t mag_mask;

	if (!device || !device->spi_desc || !config)
		return -EINVAL;

	/* Ensure page 2 */
	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Determine registers and masks based on harmonic */
	switch (harmonic) {
	case 1:
		mag_reg = ADMT4000_02_REG_H1MAG;
		phase_reg = ADMT4000_02_REG_H1PH;
		mag_mask = ADMT4000_H_11BIT_MAG_MASK;
		break;
	case 2:
		mag_reg = ADMT4000_02_REG_H2MAG;
		phase_reg = ADMT4000_02_REG_H2PH;
		mag_mask = ADMT4000_H_11BIT_MAG_MASK;
		break;
	case 3:
		mag_reg = ADMT4000_02_REG_H3MAG;
		phase_reg = ADMT4000_02_REG_H3PH;
		mag_mask = ADMT4000_H_8BIT_MAG_MASK;
		break;
	case 8:
		mag_reg = ADMT4000_02_REG_H8MAG;
		phase_reg = ADMT4000_02_REG_H8PH;
		mag_mask = ADMT4000_H_8BIT_MAG_MASK;
		break;
	default:
		return -EINVAL;
	}

	/* Read magnitude */
	ret = admt4000_reg_read(device, mag_reg, &mag_temp, NULL);
	if (ret)
		return ret;

	/* Read phase */
	ret = admt4000_reg_read(device, phase_reg, &phase_temp, NULL);
	if (ret)
		return ret;

	/* Extract values with appropriate masks */
	config->magnitude_raw = no_os_field_get(mag_mask, mag_temp);
	config->phase_raw = no_os_field_get(ADMT4000_H_12BIT_PHA_MASK, phase_temp);

	return 0;
}

/**
 * @brief Set harmonic configuration (magnitude and phase together).
 * @param device - The device structure.
 * @param harmonic - Harmonic number (1, 2, 3, or 8).
 * @param config - Input structure for magnitude and phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_harmonic(struct admt4000_dev *device, uint8_t harmonic,
			  const struct admt4000_harmonic_coeff *config)
{
	int ret, ecc_ret;
	uint16_t mag_reg, phase_reg;
	uint16_t mag_mask;
	uint16_t mag_max;

	if (!device || !device->spi_desc || !config)
		return -EINVAL;

	/* Determine registers, masks, and limits based on harmonic */
	switch (harmonic) {
	case 1:
		mag_reg = ADMT4000_02_REG_H1MAG;
		phase_reg = ADMT4000_02_REG_H1PH;
		mag_mask = ADMT4000_H_11BIT_MAG_MASK;
		mag_max = ADMT4000_11BIT_MAX;
		break;
	case 2:
		mag_reg = ADMT4000_02_REG_H2MAG;
		phase_reg = ADMT4000_02_REG_H2PH;
		mag_mask = ADMT4000_H_11BIT_MAG_MASK;
		mag_max = ADMT4000_11BIT_MAX;
		break;
	case 3:
		mag_reg = ADMT4000_02_REG_H3MAG;
		phase_reg = ADMT4000_02_REG_H3PH;
		mag_mask = ADMT4000_H_8BIT_MAG_MASK;
		mag_max = ADMT4000_8BIT_MAX;
		break;
	case 8:
		mag_reg = ADMT4000_02_REG_H8MAG;
		phase_reg = ADMT4000_02_REG_H8PH;
		mag_mask = ADMT4000_H_8BIT_MAG_MASK;
		mag_max = ADMT4000_8BIT_MAX;
		break;
	default:
		return -EINVAL;
	}

	/* Validate ranges */
	if (config->magnitude_raw > mag_max)
		return -EINVAL;
	if (config->phase_raw > ADMT4000_12BIT_MAX)
		return -EINVAL;

	/* Ensure page 2 */
	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Single ECC cycle for both writes */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	/* Write magnitude */
	ret = admt4000_reg_update(device, mag_reg, mag_mask,
				  no_os_field_prep(mag_mask, config->magnitude_raw));
	if (ret)
		goto ecc_restore;

	/* Write phase */
	ret = admt4000_reg_update(device, phase_reg, ADMT4000_H_12BIT_PHA_MASK,
				  no_os_field_prep(ADMT4000_H_12BIT_PHA_MASK,
						  config->phase_raw));

ecc_restore:
	/* Re-enable ECC */
	ecc_ret = admt4000_update_ecc(device, NULL);

	return ret ? ret : ecc_ret;
}

/**
 * @brief Get all harmonic configurations in a single operation.
 * @param device - The device structure.
 * @param calib - Output structure for all 4 harmonics (1, 2, 3, 8).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_harmonics_calib(struct admt4000_dev *device,
				 struct admt4000_harmonics_calib *calib)
{
	int ret;

	if (!device || !device->spi_desc || !calib)
		return -EINVAL;

	/* Read all 4 harmonics */
	ret = admt4000_get_harmonic(device, 1, &calib->h1);
	if (ret)
		return ret;

	ret = admt4000_get_harmonic(device, 2, &calib->h2);
	if (ret)
		return ret;

	ret = admt4000_get_harmonic(device, 3, &calib->h3);
	if (ret)
		return ret;

	ret = admt4000_get_harmonic(device, 8, &calib->h8);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Set all harmonic configurations in a single operation.
 * @param device - The device structure.
 * @param calib - Input structure for all 4 harmonics (1, 2, 3, 8).
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_harmonics_calib(struct admt4000_dev *device,
				 const struct admt4000_harmonics_calib *calib)
{
	int ret, ecc_ret;

	if (!device || !device->spi_desc || !calib)
		return -EINVAL;

	/* Validate all ranges before starting */
	if (calib->h1.magnitude_raw > ADMT4000_11BIT_MAX ||
	    calib->h1.phase_raw > ADMT4000_12BIT_MAX)
		return -EINVAL;

	if (calib->h2.magnitude_raw > ADMT4000_11BIT_MAX ||
	    calib->h2.phase_raw > ADMT4000_12BIT_MAX)
		return -EINVAL;

	if (calib->h3.magnitude_raw > ADMT4000_8BIT_MAX ||
	    calib->h3.phase_raw > ADMT4000_12BIT_MAX)
		return -EINVAL;

	if (calib->h8.magnitude_raw > ADMT4000_8BIT_MAX ||
	    calib->h8.phase_raw > ADMT4000_12BIT_MAX)
		return -EINVAL;

	/* Ensure page 2 */
	ret = admt4000_set_page(device, 2);
	if (ret)
		return ret;

	/* Single ECC cycle for all 8 register writes */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	/* Write H1 magnitude and phase */
	ret = admt4000_reg_update(device, ADMT4000_02_REG_H1MAG,
				  ADMT4000_H_11BIT_MAG_MASK,
				  no_os_field_prep(ADMT4000_H_11BIT_MAG_MASK,
						  calib->h1.magnitude_raw));
	if (ret)
		goto ecc_restore;

	ret = admt4000_reg_update(device, ADMT4000_02_REG_H1PH,
				  ADMT4000_H_12BIT_PHA_MASK,
				  no_os_field_prep(ADMT4000_H_12BIT_PHA_MASK,
						  calib->h1.phase_raw));
	if (ret)
		goto ecc_restore;

	/* Write H2 magnitude and phase */
	ret = admt4000_reg_update(device, ADMT4000_02_REG_H2MAG,
				  ADMT4000_H_11BIT_MAG_MASK,
				  no_os_field_prep(ADMT4000_H_11BIT_MAG_MASK,
						  calib->h2.magnitude_raw));
	if (ret)
		goto ecc_restore;

	ret = admt4000_reg_update(device, ADMT4000_02_REG_H2PH,
				  ADMT4000_H_12BIT_PHA_MASK,
				  no_os_field_prep(ADMT4000_H_12BIT_PHA_MASK,
						  calib->h2.phase_raw));
	if (ret)
		goto ecc_restore;

	/* Write H3 magnitude and phase */
	ret = admt4000_reg_update(device, ADMT4000_02_REG_H3MAG,
				  ADMT4000_H_8BIT_MAG_MASK,
				  no_os_field_prep(ADMT4000_H_8BIT_MAG_MASK,
						  calib->h3.magnitude_raw));
	if (ret)
		goto ecc_restore;

	ret = admt4000_reg_update(device, ADMT4000_02_REG_H3PH,
				  ADMT4000_H_12BIT_PHA_MASK,
				  no_os_field_prep(ADMT4000_H_12BIT_PHA_MASK,
						  calib->h3.phase_raw));
	if (ret)
		goto ecc_restore;

	/* Write H8 magnitude and phase */
	ret = admt4000_reg_update(device, ADMT4000_02_REG_H8MAG,
				  ADMT4000_H_8BIT_MAG_MASK,
				  no_os_field_prep(ADMT4000_H_8BIT_MAG_MASK,
						  calib->h8.magnitude_raw));
	if (ret)
		goto ecc_restore;

	ret = admt4000_reg_update(device, ADMT4000_02_REG_H8PH,
				  ADMT4000_H_12BIT_PHA_MASK,
				  no_os_field_prep(ADMT4000_H_12BIT_PHA_MASK,
						  calib->h8.phase_raw));

ecc_restore:
	/* Re-enable ECC */
	ecc_ret = admt4000_update_ecc(device, NULL);

	return ret ? ret : ecc_ret;
}
