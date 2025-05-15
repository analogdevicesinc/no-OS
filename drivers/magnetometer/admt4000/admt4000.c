/***************************************************************************//**
 *   @file   admt4000.c
 *   @brief  Implementation of ADMT4000 Driver.
 *   @details This file contains the implementation of the ADMT4000 driver.
 *            It includes functions for initializing the device, reading and
 *            writing registers, computing CRC, ECC encoding, and more.
 *   @authors Marc Sosa
 *            Jose Ramon San Buenaventura
 *            Louijie Compo
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "admt4000.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

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

static uint32_t admt4000_angle_conv_factors[] = {
	ADMT4000_ABS_ANGLE_RES,
	ADMT4000_ANGLE_RES,
};

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the admt4000
 *        part is present.
 *
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_init(struct admt4000_dev **device,
		  struct admt4000_init_param init_param)
{
	bool bool_temp;
	int ret;
	struct admt4000_dev *dev;

	if (init_param.dev_vdd < ADMT4000_3P3V || init_param.dev_vdd > ADMT4000_5V)
		return -EINVAL;

	dev = (struct admt4000_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret)
		goto err;

	/* GPIO */
	/**  Get Descriptors **/
	ret = no_os_gpio_get(&dev->gpio_coil_rs,
			     &init_param.gpio_coil_rs);
	if (ret)
		goto err;

	ret = no_os_gpio_get(&dev->gpio_gpio0_busy,
			     &init_param.gpio_gpio0_busy);
	if (ret)
		goto err;

	ret = no_os_gpio_get(&dev->gpio_shdn_n,
			     &init_param.gpio_shdn_n);
	if (ret)
		goto err;

	ret = no_os_gpio_get(&dev->gpio_cnv,
			     &init_param.gpio_cnv);
	if (ret)
		goto err;

	ret = no_os_gpio_get(&dev->gpio_acalc,
			     &init_param.gpio_acalc);
	if (ret)
		goto err;

	/** Set Direction **/
	ret = no_os_gpio_direction_output(dev->gpio_coil_rs,
					  NO_OS_GPIO_LOW);
	if (ret)
		goto err;

	ret = no_os_gpio_direction_input(dev->gpio_gpio0_busy);
	if (ret)
		goto err;

	ret = no_os_gpio_direction_output(dev->gpio_shdn_n,
					  NO_OS_GPIO_HIGH);
	if (ret)
		goto err;

	ret = no_os_gpio_direction_input(dev->gpio_cnv);
	if (ret)
		goto err;

	ret = no_os_gpio_direction_input(dev->gpio_acalc);
	if (ret)
		goto err;

	/* Delay for GPIO to lines to settle before clearing faults*/
	no_os_mdelay(10);

	ret = admt4000_clear_all_faults(dev);
	if (ret)
		goto err;

	if (init_param.dev_vdd == ADMT4000_3P3V)
		dev->fixed_conv_factor_mv = 412500; // in uV
	else
		dev->fixed_conv_factor_mv = 300000; // in uV

	ret = admt4000_get_cnv_mode(dev, &bool_temp);
	if (ret)
		return ret;

	dev->is_one_shot = bool_temp;

	ret = admt4000_get_page(dev, &bool_temp);
	if (ret)
		return ret;

	dev->is_page_zero = bool_temp;

	*device = dev;

	return 0;

err:
	admt4000_remove(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by admt4000_init().
 *
 * @param device - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_remove(struct admt4000_dev *device)
{
	int ret;

	if (!device)
		return -EINVAL;

	if (device->spi_desc) {
		ret = no_os_spi_remove(device->spi_desc);
		if (ret)
			return ret;
		device->spi_desc = NULL;
	}

	if (device->gpio_coil_rs) {
		ret = no_os_gpio_remove(device->gpio_coil_rs);
		if (ret)
			return ret;
		device->gpio_coil_rs = NULL;
	}

	if (device->gpio_gpio0_busy) {
		ret = no_os_gpio_remove(device->gpio_gpio0_busy);
		if (ret)
			return ret;
		device->gpio_gpio0_busy = NULL;
	}

	if (device->gpio_shdn_n) {
		ret = no_os_gpio_remove(device->gpio_shdn_n);
		if (ret)
			return ret;
		device->gpio_shdn_n = NULL;
	}

	if (device->gpio_cnv) {
		ret = no_os_gpio_remove(device->gpio_cnv);
		if (ret)
			return ret;
		device->gpio_cnv = NULL;
	}

	if (device->gpio_acalc) {
		ret = no_os_gpio_remove(device->gpio_acalc);
		if (ret)
			return ret;
		device->gpio_acalc = NULL;
	}

	no_os_free(device);

	return 0;
}

/***************************************************************************//**
 * @brief Computes the CRC byte based on data input.
 *
 * @param reg_addr - Address of the register.
 * @param reg_data - Data read from the register.
 * @param excess - Other bits (non-register related).
 * @param is_write - Computation flag depending on operation used.
 * @param crc_ret - Stores the computed CRC value.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_compute_crc(long reg_addr, uint16_t reg_data, uint8_t excess,
			 bool is_write, uint8_t *crc_ret)
{
	int crc[] = {1, 1, 1, 1, 1};
	int poly;
	int i, xor;
	uint32_t data_in;

	if (reg_addr > ADMT4000_02_REG_ECCDIS)
		return -EINVAL;

	if (is_write) {
		reg_addr = (reg_addr & ADMT4000_RW_MASK) | ADMT4000_WR_EN;
		data_in = ((reg_addr << 16) | reg_data) << 3;
	} else {
		reg_addr = (reg_addr & ADMT4000_RW_MASK);
		data_in = ((reg_addr << 16) | reg_data) << 3 | excess;
	}

	for (i = 25; i >= 0; i--) {
		xor = ((data_in >> i) & 0x1) ^ crc[4];
		poly = crc[1] ^ xor;

		crc[4] = crc[3];
		crc[3] = crc[2];
		crc[2] = poly;
		crc[1] = crc[0];
		crc[0] = xor;
	}

	*crc_ret = 16 * crc[4] + 8 * crc[3] + 4 * crc[2] + 2 * crc[1] + crc[0];

	return 0;
}

/***************************************************************************//**
 * @brief ECC Encoding process.
 *
 * @param parity_num - Stores the computed number of parity bits to use.
 * @param code_length - Stores the computed length of new code (old + parity bits).
 * @param code - Stores the array where the computed codes will be stored.
 * @param input - Contains the input data to be encoded from certain ADMT4000 registers.
 * @param size_code - Size of the code array.
 * @param size_input - Size of the input array.
 * @param ecc - Value to store ECC register compatible data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_ecc_encode(uint8_t *parity_num, uint8_t *code_length,
			uint8_t *code, uint8_t *input, uint8_t size_code, uint8_t size_input,
			uint8_t *ecc)
{
	int i = 0, j = 0, k = 0, n, m;
	int eff_pos, value;
	uint8_t position, xtract;

	*ecc = 0; // initialize

	n = size_code;
	m = size_input;

	if (n > 16 || m > 16)
		return -EINVAL;

	/* Compute parity bits needed */
	*parity_num = 0;

	while ((m * 8) > (1 << i) - (i + 1)) {
		*parity_num += 1;
		i++;
	}

	/* In bits */
	*code_length = *parity_num + (m * 8);

	for (i = 0; i < *code_length; i++) {
		/* Set to 0 */
		if (i == ((1 << k) - 1)) {
			code[(i / 8)] &= (uint8_t)~NO_OS_BIT((i &
							      0x7)); //i & 0x7 is equivalent to i % 8

			k++;
		}
		/* Get from input byte array */
		else {
			code[(i / 8)] &= (uint8_t)~NO_OS_BIT((i & 0x7));

			xtract = (uint8_t)no_os_field_get(NO_OS_BIT(j & 0x7), input[(j / 8)]);
			xtract = (uint8_t)no_os_field_prep(NO_OS_BIT((i & 0x7)), xtract);

			code[(i / 8)] |= xtract;

			j++;
		}
	}

	//Assignment of value bits inside the code array
	/* parity_num is in BITS */
	for (i = 0; i < *parity_num; i++) {
		position = 1 << i;

		value = admt4000_hamming_calc(position, *code_length, code);

		eff_pos = position - 1;

		code[(eff_pos / 8)] &= ~NO_OS_BIT(eff_pos &
						  0x7); //i & 0x7 is equivalent to i % 8
		code[(eff_pos / 8)] |= (NO_OS_BIT(eff_pos & 0x7) * value);

		*ecc |= (value << i);
	}

	value = 0;
	for (i = 0; i < *code_length; i++) {
		uint8_t bit = no_os_field_get(NO_OS_BIT(i & 0x7), code[(i / 8)]);
		if (bit)
			value++;
	}

	value &= 0x1;

	*ecc |= (value << *parity_num);

	return 0;
}

/***************************************************************************//**
 * @brief Computes the Hamming Distance.
 *
 * @param position - Bit position.
 * @param code_length - Length of code (original data + parity bits). Should be in bits.
 * @param code - Array containing bytes to iterate through.
 *
 * @return 0 or 1 (to be appended during ECC encoding process).
*******************************************************************************/
int admt4000_hamming_calc(uint8_t position, uint8_t code_length, uint8_t *code)
{
	uint8_t bit_extract;
	int count = 0, i, j;
	i = position - 1;

	if (NO_OS_ARRAY_SIZE(code) > 16)
		return -EINVAL;

	while (i < code_length) {
		for (j = i; j < i + position; j++) {
			bit_extract = no_os_field_get(NO_OS_BIT((j & 0x7)), code[(j / 8)]);
			if (bit_extract)
				count++;
		}

		i += (2 * position);
	}

	if (!(count % 2))
		return 0;

	return 1;
}

/***************************************************************************//**
 * @brief Checks if error is present in input code.
 *
 * @param parity_num - Number of parity bits appended.
 * @param code_length - Length of code (original data + parity bits). Should be in bits.
 * @param code - Array containing bytes to iterate through.
 * @param error_pos - Bit position where error is detected.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_error_check(uint8_t parity_num, uint8_t code_length, uint8_t *code,
			 uint8_t *error_pos)
{
	int position;
	int i;
	int value;

	if (NO_OS_ARRAY_SIZE(code) > 16)
		return -EINVAL;

	*error_pos = 0;

	for (i = 0; i < parity_num; i++) {
		position = 1 << i;
		value = admt4000_hamming_calc(position, code_length, code);

		if (value)
			*error_pos += position;
	}

	if (*error_pos)
		return -EBADMSG;

	return 0;
}

/***************************************************************************//**
 * @brief Updates the ECC config register contents.
 *
 * @param device - The device structure.
 * @param ecc_val - ECC Register value computed.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_update_ecc(struct admt4000_dev *device, uint16_t *ecc_val)
{
	/*
	 * ECC related variables:
	 * for_encode[15] - contains the 15 byte register data to be encoded
	 * ecc[2] - stores the generated ECC for CONFIG0 and CONFIG1
	 * parity_num - stores the number of parity bits generated
	 * code_len - stores the length of the encoded data (in bits)
	 * encoded[16] - stores the encoded data (original register data + parity bits inserted)
	 *
	 * Others:
	 * ret - error code of function/s ran
	 * temp - stores 16 bit data read from each registers
	 */
	int ret;
	uint8_t for_encode[15] = {0}, ecc[2] = {0};
	uint8_t parity_num, code_len, encoded[16] = {0};
	uint16_t temp;

	/*
	 * ECC1 (needs padding)
	 */
	for (int i = 7; i < 11; i++) {
		ret = admt4000_read(device, admt4000_ecc_control_registers[i], &temp, NULL);
		if (ret)
			return ret;

		if (i != 7)
			no_os_put_unaligned_le16(temp, for_encode + (2 * (i - 8) + 1));
		else
			for_encode[14] = no_os_field_get(ADMT4000_HI_BYTE, temp);
	}

	ret = admt4000_ecc_encode(&parity_num, &code_len,
				  encoded, for_encode, 16, 15, ecc);
	if (ret)
		return ret;

	/*
	 * ECC0 (no padding)
	 */
	for (int i = 0; i < 8; i++) {
		ret = admt4000_read(device, admt4000_ecc_control_registers[i], &temp, NULL);
		if (ret)
			return ret;

		if (i != 7)
			no_os_put_unaligned_le16(temp, for_encode + (2 * i));
		else
			for_encode[14] = no_os_field_get(ADMT4000_LOW_BYTE, temp);
	}

	/*
	 * ECC1 (needs padding)
	 */
	ret = admt4000_ecc_encode(&parity_num, &code_len,
				  encoded, for_encode, 16, 15, ecc + 1);
	if (ret)
		return ret;

	/* Format ECC data to write in actual register */
	temp = no_os_get_unaligned_be16(ecc);

	/* Store in return variable */
	if (temp != NULL)
		*ecc_val = temp;
	else
		return -EINVAL;

	/* All registers of interest are in page 2 */
	ret = admt4000_set_page(device, false);
	if (ret)
		return ret;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	/* Update ECC register contents */
	ret = admt4000_write(device, ADMT4000_02_REG_ECCEDC, temp);
	if (ret)
		return ret;

	/* Enable ECC functionality */
	return admt4000_ecc_config(device, true);
}

/***************************************************************************//**
 * @brief Reads data from a certain ADMT400x Register.
 *
 * @param device - The device structure.
 * @param reg_addr - Address of the register.
 * @param reg_data - Stores data read from the register.
 * @param verif - Stores the Fault, and CRC bits returned.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_read(struct admt4000_dev *device, uint8_t reg_addr,
		  uint16_t *reg_data, uint8_t *verif)
{
	int i, ret;
	uint8_t buf[4];
	uint8_t excess, temp;

	if (reg_addr > ADMT4000_02_REG_ECCDIS)
		return -EINVAL;

	for (i = 0; i < 4; i++)
		buf[i] = reg_addr & ADMT4000_RW_MASK;

	ret = no_os_spi_write_and_read(device->spi_desc, buf, 4);
	if (ret)
		return ret;

	temp = no_os_field_get(ADMT4000_LIFE_CTR | ADMT4000_FAULT_MASK, buf[3]);

	*reg_data = no_os_get_unaligned_be16(buf + 1);

	ret = admt4000_compute_crc(reg_addr, *reg_data, temp, false, &excess);
	if (ret)
		return ret;

	if (no_os_field_get(ADMT4000_RCV_CRC, buf[3]) != excess)
		return -EBADMSG;

	if (buf[3] != NULL)
		*verif = buf[3];
	else
		return -EINVAL;

	return 0;
}

/***************************************************************************//**
 * @brief Writes data to a certain ADMT400x Register.
 *
 * @param device - The device structure.
 * @param reg_addr - Address of the register.
 * @param reg_data - Data to write to the chosen register (16 bit wide).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_write(struct admt4000_dev *device, uint8_t reg_addr,
		   uint16_t reg_data)
{
	int ret;
	uint8_t buf[4];
	uint8_t verif;

	if (reg_addr > ADMT4000_02_REG_ECCDIS)
		return -EINVAL;

	ret = admt4000_compute_crc(reg_addr, reg_data, 0, true, &verif);
	if (ret)
		return ret;

	buf[0] = (reg_addr & ADMT4000_RW_MASK) | ADMT4000_WR_EN;
	buf[1] = no_os_field_get(ADMT4000_HI_BYTE, reg_data);
	buf[2] = no_os_field_get(ADMT4000_LOW_BYTE, reg_data);
	buf[3] = verif;

	return no_os_spi_write_and_read(device->spi_desc, buf, 4);
}

/***************************************************************************//**
 * @brief Updates the contents of selected register.
 *
 * @param device - The device structure.
 * @param reg_addr - Address of the register.
 * @param update_mask - Mask to update to selected register bit/s.
 * @param update_val - Value used to update on selected register.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_reg_update(struct admt4000_dev *device, uint8_t reg_addr,
			uint16_t update_mask, uint16_t update_val)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (reg_addr > ADMT4000_02_REG_ECCDIS)
		return -EINVAL;

	ret = admt4000_read(device, reg_addr, &temp, &verif);
	if (ret)
		return ret;

	temp &= ~update_mask;
	temp |= update_val;

	return admt4000_write(device, reg_addr, temp);
}

/***************************************************************************//**
 * @brief Configures the page settings.
 *
 * @param device - The device structure.
 * @param is_page_zero - Set true for page 0 operations, false for page 2.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_page(struct admt4000_dev *device, bool is_page_zero)
{
	int ret;

	if (is_page_zero)
		ret = admt4000_reg_update(device, ADMT4000_AGP_REG_CNVPAGE, ADMT4000_PAGE_MASK,
					  no_os_field_prep(ADMT4000_PAGE_MASK, 0x00));
	else
		ret = admt4000_reg_update(device, ADMT4000_AGP_REG_CNVPAGE, ADMT4000_PAGE_MASK,
					  no_os_field_prep(ADMT4000_PAGE_MASK, 0x02));
	if (ret)
		return ret;

	device->is_page_zero = is_page_zero;

	return 0;
}

/***************************************************************************//**
 * @brief Obtains the page settings.
 *
 * @param device - The device structure.
 * @param is_page_zero - Stores page config (True for page 0. Page 2 otherwise).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_page(struct admt4000_dev *device, bool *is_page_zero)
{
	int ret;
	uint16_t temp;

	ret = admt4000_read(device, ADMT4000_AGP_REG_CNVPAGE, &temp, NULL);
	if (ret)
		return ret;

	if (!no_os_field_get(ADMT4000_PAGE_MASK, temp))
		*is_page_zero = true;
	else if (no_os_field_get(ADMT4000_PAGE_MASK, temp) == 0x2)
		*is_page_zero = false;
	else
		return -EINVAL;

	return 0;
}

/***************************************************************************//**
 * @brief Toggle the CNV bits to mimic rising/falling edge in CNV pin.
 *
 * @param device - The device structure.
 * @param is_rising - Set to true for rising, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_cnv(struct admt4000_dev *device, bool is_rising)
{
	int ret;

	if (is_rising) {
		uint32_t temp = no_os_field_prep(ADMT4000_CNV_EDGE_MASK, ADMT4000_RISING_EDGE);
		ret = admt4000_reg_update(device, ADMT4000_AGP_REG_CNVPAGE,
					  ADMT4000_CNV_EDGE_MASK, temp);
	} else {
		uint32_t temp = no_os_field_prep(ADMT4000_CNV_EDGE_MASK, ADMT4000_FALLING_EDGE);
		ret =  admt4000_reg_update(device, ADMT4000_AGP_REG_CNVPAGE,
					   ADMT4000_CNV_EDGE_MASK, temp);
	}
	if (ret)
		return ret;

	device->edge = is_rising;

	return 0;
}

/***************************************************************************//**
 * @brief Get the CNV bits configuration.
 *
 * @param device - The device structure.
 * @param is_rising - Set to true for rising, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_cnv(struct admt4000_dev *device, bool *is_rising)
{
	int ret;
	uint16_t temp;

	ret = admt4000_read(device, ADMT4000_AGP_REG_CNVPAGE, &temp, NULL);
	if (ret)
		return ret;

	if (!no_os_field_get(ADMT4000_CNV_EDGE_MASK, temp))
		*is_rising = false;
	else if (no_os_field_get(ADMT4000_CNV_EDGE_MASK, temp) == ADMT4000_RISING_EDGE)
		*is_rising = true;
	else
		return -EINVAL;

	return 0;
}

/***************************************************************************//**
 * @brief Reads ANGLE and ABSANGLE register contents in 1 CS frame.
 *
 * @param device - The device structure.
 * @param angle_data - Stores data read from the registers.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_raw_angle_read(struct admt4000_dev *device, uint16_t *angle_data)
{
	int i, ret;
	uint8_t buf[8] = { 0 };
	uint8_t excess, temp;
	uint8_t admt4000_angle_regs[] = {
		ADMT4000_AGP_REG_ABSANGLE,
		ADMT4000_AGP_REG_ANGLE,
	};

	for (i = 0; i < 2; i++)
		buf[0 + (4 * i)] = admt4000_angle_regs[i] & ADMT4000_RW_MASK;

	ret = no_os_spi_write_and_read(device->spi_desc, buf, 8);
	if (ret)
		return ret;

	for (i = 0; i < 2; i++) {
		temp = no_os_field_get(ADMT4000_LIFE_CTR | ADMT4000_FAULT_MASK, buf[3 + 4 * i]);

		*angle_data = no_os_get_unaligned_be16(buf + 1 + 4 * i);

		ret = admt4000_compute_crc(admt4000_angle_regs[i], *angle_data, temp, false,
					   &excess);
		if (ret)
			return ret;

		if (no_os_field_get(ADMT4000_RCV_CRC, buf[3 + 4 * i]) != excess)
			return -EBADMSG;

		angle_data++;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Get the number of turns and angle information (Raw).
 *
 * @param device - The device structure.
 * @param turns - Stores the number of turns read (Raw).
 * @param angle - Stores the angle values read (Raw, extracted).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_raw_turns_and_angle(struct admt4000_dev *device,
				     uint8_t *turns,
				     uint16_t *angle)
{
	int i, ret;
	uint16_t admt4000_angle_masks[] = {
		//ADMT4000_ABS_ANGLE_MASK,
		NO_OS_GENMASK(15, 0),
		ADMT4000_ANGLE_MASK,
	};

	ret = admt4000_raw_angle_read(device, angle);
	if (ret)
		return ret;

	*turns = no_os_field_get(ADMT4000_TURN_CNT_MASK, angle[0]);

	for (i = 0; i < 2; i++)
		angle[i] = no_os_field_get(admt4000_angle_masks[i], angle[i]);

	return 0;
}

/***************************************************************************//**
 * @brief Get the number Cosine(Raw).
 *
 * @param device - The device structure.
 * @param turns - Stores the number of turns read (Raw).
 * @param angle - Stores the angle values read (Raw, extracted).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_cos(struct admt4000_dev *device, uint16_t *val)
{
	//buf[0] = ADMT4000_RAW_COSINE_MASK  & ADMT4000_RW_MASK;
	int ret;
	uint16_t raw_temp;
	uint8_t verif;

	if (!device->is_page_zero) {
		ret = admt4000_set_page(device, true);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_00_REG_COSINE, &raw_temp, &verif);
	if (ret)
		return ret;

	*val = no_os_field_get(ADMT4000_RAW_COSINE_MASK, raw_temp);

	return 0;
}


/***************************************************************************//**
 * @brief Get the number Sine(Raw).
 *
 * @param device - The device structure.
 * @param turns - Stores the number of turns read (Raw).
 * @param angle - Stores the angle values read (Raw, extracted).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_sin(struct admt4000_dev *device, uint16_t *val)
{
	int ret;
	uint16_t raw_temp;
	uint8_t verif;

	if (!device->is_page_zero) {
		ret = admt4000_set_page(device, true);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_00_REG_SINE, &raw_temp, &verif);
	if (ret)
		return ret;

	*val = no_os_field_get(ADMT4000_RAW_SINE_MASK, raw_temp);

	return 0;
}

/***************************************************************************//**
 * @brief Get status of selected GPIO.
 *
 * @param device - The device structure.
 * @param gpio - GPIO number (0 to 5).
 * @param logic - Stores the truth value of selected GPIO.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_gpio(struct admt4000_dev *device, uint8_t gpio, bool *logic)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	ret = admt4000_read(device, ADMT4000_AGP_REG_DGIO, &temp, &verif);
	if (ret)
		return ret;

	*logic = (bool) no_os_field_get(ADMT4000_GPIO_LOGIC(gpio), temp);

	return 0;
}

/***************************************************************************//**
 * @brief Configure selected GPIO.
 *
 * @param device - The device structure.
 * @param gpio - GPIO number (0 to 5).
 * @param logic - Logic level to set for GPIO.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_gpio(struct admt4000_dev *device, uint8_t gpio, bool logic)
{
	int ret;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	uint32_t temp = no_os_field_prep(ADMT4000_GPIO_LOGIC(gpio), (uint8_t)logic);
	ret = admt4000_reg_update(device, ADMT4000_AGP_REG_DGIO,
				  ADMT4000_GPIO_LOGIC(gpio), temp);

	if (ret)
		return ret;

	device->gpios[gpio].logic_state = logic;

	return 0;
}

/***************************************************************************//**
 * @brief Clear all faults from fault registers.
 *
 * @param device - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_clear_all_faults(struct admt4000_dev *device)
{
	return admt4000_reg_update(device, ADMT4000_AGP_REG_FAULT, ADMT4000_ALL_FAULTS,
				   0);
}

/***************************************************************************//**
 * @brief Check selected fault status from fault register.
 *
 * @param device - The device structure.
 * @param fault - Fault Flag to investigate.
 * @param is_flagged - Stores the truth value of selected fault.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_read_fault(struct admt4000_dev *device, enum admt4000_faults fault,
			bool *is_flagged)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (fault == ADMT4000_FAULT_RESERVED)
		return -EINVAL;

	ret = admt4000_read(device, ADMT4000_AGP_REG_FAULT, &temp, &verif);
	if (ret)
		return ret;

	*is_flagged = (bool) no_os_field_get(NO_OS_BIT(fault), temp);

	return 0;
}

/***************************************************************************//**
 * @brief Get the secondary angle information.
 *
 * @param device - The device structure.
 * @param angle - Stores the angle value read (Raw).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_secondary_angle(struct admt4000_dev *device, uint16_t *angle)
{
	int ret;
	uint16_t raw_temp;

	ret = admt4000_read(device, ADMT4000_AGP_REG_ANGLESEC, &raw_temp, NULL);
	if (ret)
		return ret;

	/* Device in continuous mode and status is not set = INVALID */
	if (!device->is_one_shot
	    && !no_os_field_get(ADMT4000_ANGLE_STAT_MASK, raw_temp))
		return -EBADMSG;

	*angle = no_os_field_get(ADMT4000_ANGLE_MASK, raw_temp);

	return 0;
}

/***************************************************************************//**
 * @brief Get the secondary angle information (Degrees).
 *
 * @param device - The device structure.
 * @param angle - Stores the angle value read (Degrees).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_converted_secondary_angle(struct admt4000_dev *device,
		uint32_t *angle)
{
	int ret;
	uint16_t raw_temp;

	ret = admt4000_get_secondary_angle(device, &raw_temp);
	if (ret)
		return ret;

	*angle = raw_temp * admt4000_angle_conv_factors[1];

	return 0;
}

/***************************************************************************//**
 * @brief Get SINE, COSINE, SECANGLI, or SECANGLQ angle register value.
 *
 * @param device - The device structure.
 * @param raw_angle - Stores the angle value read.
 * @param angle_type - Type of angle to read.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_angle(struct admt4000_dev *device, uint16_t *raw_angle,
		       enum admt4000_angle_type angle_type)
{
	int ret;
	uint16_t raw_temp;
	uint8_t verif;

	if (angle_type < ADMT4000_RAW_SINE || angle_type > ADMT4000_RAW_SECANGLEQ)
		return -EINVAL;

	if (!device->is_page_zero) {
		ret = admt4000_set_page(device, true);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_RAW_ANGLE_REG(angle_type), &raw_temp,
			    &verif);
	if (ret)
		return ret;

	/* Device in continuous mode and status is not set = INVALID */
	if (!device->is_one_shot
	    && !no_os_field_get(ADMT4000_ANGLE_STAT_MASK, raw_temp))
		return -EBADMSG;

	*raw_angle = no_os_field_get(ADMT4000_RAW_ANGLE_MASK, raw_temp);

	return 0;
}

/***************************************************************************//**
 * @brief Get RADIUS vector value.
 *
 * @param device - The device structure.
 * @param radius - Stores the vector read.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_radius(struct admt4000_dev *device, uint16_t *radius)
{
	int ret;
	uint16_t raw_temp;
	uint8_t verif;

	if (!device->is_page_zero) {
		ret = admt4000_set_page(device, true);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_00_REG_RADIUS, &raw_temp, &verif);
	if (ret)
		return ret;

	*radius = no_os_field_get(ADMT4000_RADIUS_MASK, raw_temp);

	return 0;
}

/***************************************************************************//**
 * @brief Get RADIUS vector value (mV/V).
 *
 * @param device - The device structure.
 * @param radius - Stores Magnitude value (mV/V).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_converted_radius(struct admt4000_dev *device, uint32_t *radius)
{
	int ret;
	uint16_t raw_temp;

	ret = admt4000_get_radius(device, &raw_temp);
	if (ret)
		return ret;

	*radius = raw_temp * ADMT4000_RADIUS_RES;

	return 0;
}

/***************************************************************************//**
 * @brief Probe Reference Resistor status.
 *
 * @param device - The device structure.
 * @param ref_res_num - Reference resistor number to probe.
 * @param is_ok - Stores the logic level of the reference resistor probed.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_ref_res_state(struct admt4000_dev *device, uint8_t ref_res_num,
			       bool *is_ok)
{
	int ret;
	uint16_t raw_temp;
	uint8_t verif;

	if (ref_res_num > 7)
		return -EINVAL;

	if (!device->is_page_zero) {
		ret = admt4000_set_page(device, true);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_00_REG_DIAG1, &raw_temp, &verif);
	if (ret)
		return ret;

	*is_ok = (bool)no_os_field_get(ADMT4000_REF_RES(ref_res_num), raw_temp);

	return 0;
}

/***************************************************************************//**
 * @brief Get fixed voltage from GMR turn count sensor AFE.
 *
 * @param device - The device structure.
 * @param fixed_volt - Stores the fixed voltage read (Raw).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_fixed_voltage(struct admt4000_dev *device, uint8_t *fixed_volt)
{
	int ret;
	uint16_t raw_temp;
	uint8_t verif;

	if (!device->is_page_zero) {
		ret = admt4000_set_page(device, true);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_00_REG_DIAG1, &raw_temp, &verif);
	if (ret)
		return ret;

	*fixed_volt = no_os_field_get(ADMT4000_AFEDIAG2_MASK, raw_temp);

	return 0;
}

/***************************************************************************//**
 * @brief Get fixed voltage from GMR turn count sensor AFE (Volts).
 *
 * @param device - The device structure.
 * @param fixed_volt - Stores the fixed voltage read (Volts).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_converted_fixed_voltage(struct admt4000_dev *device,
		uint32_t *fixed_volt)
{
	int ret;
	uint32_t res;
	uint8_t raw_temp;

	ret = admt4000_get_fixed_voltage(device, &raw_temp);
	if (ret)
		return ret;

	if (device->dev_vdd == ADMT4000_3P3V)
		res = ADMT4000_FIXED_VOLT_3P3V_RES;
	else
		res = ADMT4000_FIXED_VOLT_5V_RES;

	if (raw_temp > ADMT4000_8BIT_THRES) {
		*fixed_volt = (raw_temp - ADMT4000_8BIT_TWOS) * res;
	} else
		*fixed_volt = raw_temp * res;

	return 0;
}

/***************************************************************************//**
 * @brief Get Diagnostic Sensor Measurement (+57% or -57%).
 *
 * @param device - The device structure.
 * @param diag_meas - Stores the data read (Raw).
 * @param is_pos - Controls which diagnostic resistor value is read (pos, neg, both).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_diag_res(struct admt4000_dev *device, uint16_t *diag_meas,
			  uint8_t is_pos)
{
	int ret;
	uint16_t raw_temp;
	uint8_t verif;

	if (!device->is_page_zero) {
		ret = admt4000_set_page(device, true);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_00_REG_DIAG2, &raw_temp, &verif);
	if (ret)
		return ret;

	switch (is_pos) {
	case 0: // positive
		*diag_meas = no_os_field_get(ADMT4000_AFEDIAG1_MASK, raw_temp);
		break;
	case 1: // negative
		*diag_meas = no_os_field_get(ADMT4000_AFEDIAG0_MASK, raw_temp);
		break;
	case 2: // both
		*diag_meas = raw_temp;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Get Diagnostic Sensor Measurement (+57% or -57%).
 *
 * @param device - The device structure.
 * @param diag_meas - Stores the percentage value.
 * @param is_pos - Controls which diagnostic resistor value is read (pos, neg, both).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_converted_diag_res(struct admt4000_dev *device,
				    uint32_t *diag_meas,
				    uint8_t is_pos)
{
	int ret;
	uint16_t raw_temp;

	/* Cannot convert when using BOTH readings */
	if (is_pos == 2)
		return -EINVAL;

	ret = admt4000_get_diag_res(device, &raw_temp, is_pos);
	if (ret)
		return ret;

	if (raw_temp > ADMT4000_8BIT_THRES)
		*diag_meas = (raw_temp - ADMT4000_8BIT_TWOS) * ADMT4000_DIAG_RESISTOR_RES;
	else
		*diag_meas = raw_temp * ADMT4000_DIAG_RESISTOR_RES;

	return 0;
}

/***************************************************************************//**
 * @brief Get primary temperature (Raw).
 *
 * @param device - The device structure.
 * @param temp - Raw temperature data.
 * @param is_primary - Set true to get from primary temp register, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_temp(struct admt4000_dev *device, uint16_t *temp,
		      bool is_primary)
{
	int ret;
	uint16_t raw_temp;
	uint8_t verif;

	if (!device->is_page_zero) {
		ret = admt4000_set_page(device, true);
		if (ret)
			return ret;
	}

	if (is_primary)
		ret = admt4000_read(device, ADMT4000_00_REG_TMP0, &raw_temp, &verif);
	else
		ret = admt4000_read(device, ADMT4000_00_REG_TMP1, &raw_temp, &verif);
	if (ret)
		return ret;

	*temp = no_os_field_get(ADMT4000_TEMP_MASK, raw_temp);

	return 0;
}

/***************************************************************************//**
 * @brief Get primary temperature (Degrees).
 *
 * @param device - The device structure.
 * @param temp - Temperature data (Degrees).
 * @param is_primary - Set true to get from primary temp register, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_converted_temp(struct admt4000_dev *device, uint32_t *temp,
				bool is_primary)
{
	int ret;
	uint16_t raw_temp;

	ret = admt4000_get_temp(device, &raw_temp, is_primary);
	if (ret)
		return ret;

	if (device->dev_vdd == ADMT4000_3P3V && !is_primary)
		*temp = ((raw_temp - 1208.0) / 13.61) * ADMT4000_SF;
	else if (device->dev_vdd == ADMT4000_3P3V && is_primary)
		*temp = ((raw_temp - 1150.0) / 16.32) * ADMT4000_SF;
	else if (device->dev_vdd == ADMT4000_5V && !is_primary)
		*temp = ((raw_temp - 1238.0) / 13.45) * ADMT4000_SF;
	else if (device->dev_vdd == ADMT4000_5V && is_primary)
		*temp = ((raw_temp - 1145.0) / 16.27) * ADMT4000_SF;
	else
		return -EINVAL;

	return 0;
}

/***************************************************************************//**
 * @brief Gets storage general config.
 *
 * @param device - The device structure.
 * @param storage - Stores the storage bits read.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_storage_config(struct admt4000_dev *device, uint8_t *storage)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_02_REG_GENERAL, &temp, &verif);
	if (ret)
		return ret;

	*storage = no_os_field_prep(ADMT4000_STORAGE_MSB_XTRACT,
				    no_os_field_get(ADMT4000_STORAGE_MSB, temp));
	*storage |= no_os_field_prep(ADMT4000_STORAGE_BIT6_XTRACT,
				     no_os_field_get(ADMT4000_STORAGE_BIT6, temp));
	*storage |= no_os_field_prep(ADMT4000_STORAGE_MASK1_XTRACT,
				     no_os_field_get(ADMT4000_STORAGE_MASK1, temp));
	*storage |= no_os_field_prep(ADMT4000_STORAGE_MASK0_XTRACT,
				     no_os_field_get(ADMT4000_STORAGE_MASK0, temp));

	return 0;
}

/***************************************************************************//**
 * @brief Sets storage general config.
 *
 * @param device - The device structure.
 * @param storage - Data to write on storage bits.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_storage_config(struct admt4000_dev *device, uint8_t storage)
{
	int ret;
	uint16_t temp = 0;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	temp = no_os_field_prep(ADMT4000_STORAGE_MSB,
				no_os_field_get(ADMT4000_STORAGE_MSB_XTRACT, storage));
	temp |= no_os_field_prep(ADMT4000_STORAGE_BIT6,
				 no_os_field_get(ADMT4000_STORAGE_BIT6_XTRACT, storage));
	temp |= no_os_field_prep(ADMT4000_STORAGE_MASK1,
				 no_os_field_get(ADMT4000_STORAGE_MASK1_XTRACT, storage));
	temp |= no_os_field_prep(ADMT4000_STORAGE_MASK0,
				 no_os_field_get(ADMT4000_STORAGE_MASK0_XTRACT, storage));

	ret =  admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				   ADMT4000_STORAGE_MASK_FULL, temp);
	if (ret)
		return ret;

	/* Update ECC registers and re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Gets conversion start's sync mode.
 *
 * @param device - The device structure.
 * @param sync - Stores the Conversion Sync Setting.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_cnv_sync(struct admt4000_dev *device,
			  enum admt4000_cnv_sync *sync)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_02_REG_GENERAL, &temp, &verif);
	if (ret)
		return ret;

	*sync = no_os_field_get(ADMT4000_CNV_SYNC_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Sets conversion start's sync mode.
 *
 * @param device - The device structure.
 * @param sync - Conversion Sync setting to write.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_cnv_sync(struct admt4000_dev *device,
			  enum admt4000_cnv_sync sync)
{
	int ret;

	if (sync != ADMT4000_SEQ_START && sync != ADMT4000_SYNC_EDGE)
		return -EINVAL;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	ret = admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				  ADMT4000_CNV_SYNC_MASK, no_os_field_prep(ADMT4000_CNV_SYNC_MASK, sync));

	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Gets current filter setting.
 *
 * @param device - The device structure.
 * @param is_filtered - Stores the logic level of the filter enable bit.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_angle_filt(struct admt4000_dev *device, bool *is_filtered)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_02_REG_GENERAL, &temp, &verif);
	if (ret)
		return ret;

	*is_filtered = (bool) no_os_field_get(ADMT4000_ANGL_FILT_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Sets the filter setting.
 *
 * @param device - The device structure.
 * @param is_filtered - Set true to enable filter, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_angle_filt(struct admt4000_dev *device, bool is_filtered)
{
	int ret;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	uint32_t temp = no_os_field_prep(ADMT4000_ANGL_FILT_MASK, (uint8_t)is_filtered);
	ret =  admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				   ADMT4000_ANGL_FILT_MASK, temp);

	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Gets 8th harmonic control setting.
 *
 * @param device - The device structure.
 * @param is_user_supplied - Stores the logic level of the 8th harmonic correction source bit.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_h8_ctrl(struct admt4000_dev *device, bool *is_user_supplied)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_02_REG_GENERAL, &temp, &verif);
	if (ret)
		return ret;

	*is_user_supplied = (bool)no_os_field_get(ADMT4000_H8_CTRL_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Sets 8th harmonic control setting.
 *
 * @param device - The device structure.
 * @param is_user_supplied - Sets the logic level of the 8th harmonic correction source bit.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_h8_ctrl(struct admt4000_dev *device, bool is_user_supplied)
{
	int ret;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	uint32_t temp = no_os_field_prep(ADMT4000_H8_CTRL_MASK,
					 (uint8_t)is_user_supplied);
	ret = admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				  ADMT4000_H8_CTRL_MASK, temp);

	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Obtains sequencer mode.
 *
 * @param device - The device structure.
 * @param seq - The sequence mode to configure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_seq_mode(struct admt4000_dev *device,
			  enum admt4000_seq_mode *seq)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_02_REG_GENERAL, &temp, &verif);
	if (ret)
		return ret;

	*seq = no_os_field_get(ADMT4000_SEQ_MODE_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Configures between fast mode and diagnostic sequencer mode.
 *
 * @param device - The device structure.
 * @param seq - Stores the obtained sequence mode.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_seq_mode(struct admt4000_dev *device,
			  enum admt4000_seq_mode seq)
{
	int ret;

	if (seq != ADMT4000_MODE2 && seq != ADMT4000_MODE1)
		return -EINVAL;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	ret = admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				  ADMT4000_SEQ_MODE_MASK, no_os_field_prep(ADMT4000_SEQ_MODE_MASK, seq));
	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Obtains the conversion mode.
 *
 * @param device - The device structure.
 * @param is_one_shot - Stores the conversion mode bit value.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_cnv_mode(struct admt4000_dev *device, bool *is_one_shot)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_02_REG_GENERAL, &temp, &verif);
	if (ret)
		return ret;

	*is_one_shot = (bool) no_os_field_get(ADMT4000_CNV_MODE_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Configures the conversion mode: one shot or continuous.
 *
 * @param device - The device structure.
 * @param is_one_shot - Set to true for one shot setting, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_cnv_mode(struct admt4000_dev *device, bool is_one_shot)
{
	int ret;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	uint32_t temp = no_os_field_prep(ADMT4000_CNV_MODE_MASK, (uint8_t)is_one_shot);
	ret = admt4000_reg_update(device, ADMT4000_02_REG_GENERAL,
				  ADMT4000_CNV_MODE_MASK, (uint16_t)temp);

	if (ret)
		return ret;

	device->is_one_shot = is_one_shot;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief En/Disables GPIO.
 *
 * @param device - The device structure.
 * @param gpio - The GPIO number to configure.
 * @param is_en - Set to true to enable selected GPIO.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_io_en(struct admt4000_dev *device, uint8_t gpio, bool is_en)
{
	int ret;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	if (is_en)
		ret = admt4000_reg_update(device, ADMT4000_02_REG_DIGIOEN,
					  ADMT4000_DIG_IO_EN(gpio), ADMT4000_DIG_IO_EN(gpio));
	else
		ret = admt4000_reg_update(device, ADMT4000_02_REG_DIGIOEN,
					  ADMT4000_DIG_IO_EN(gpio), 0);
	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Configure GPIO function.
 *
 * @param device - The device structure.
 * @param gpio - The GPIO number to configure.
 * @param is_alt_func - Set to true to configure GPIO for alternative function, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_gpio_func(struct admt4000_dev *device, uint8_t gpio,
		       bool is_alt_func)
{
	int ret;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	if (is_alt_func)
		ret =  admt4000_reg_update(device, ADMT4000_02_REG_DIGIOEN,
					   ADMT4000_GPIO_FUNC(gpio), 0);
	else
		ret =  admt4000_reg_update(device, ADMT4000_02_REG_DIGIOEN,
					   ADMT4000_GPIO_FUNC(gpio), ADMT4000_GPIO_FUNC(gpio));
	if (ret)
		return ret;

	device->gpios[gpio].is_alt_pin = is_alt_func;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Get Angle Threshold (Raw).
 *
 * @param device - The device structure.
 * @param thres - Stores threshold data (Raw).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_angle_thres(struct admt4000_dev *device, uint16_t *thres)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_02_REG_ANGLECK, &temp, &verif);
	if (ret)
		return ret;

	*thres = (bool) no_os_field_get(ADMT4000_ANGL_CHK_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Get Angle Threshold (Degrees).
 *
 * @param device - The device structure.
 * @param thres - Stores threshold data (Degrees).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_converted_angle_thres(struct admt4000_dev *device,
				       uint32_t *thres)
{
	int ret;
	uint16_t temp;

	ret = admt4000_get_angle_thres(device, &temp);
	if (ret)
		return ret;

	*thres = temp * admt4000_angle_conv_factors[1] / 2;

	return 0;
}

/***************************************************************************//**
 * @brief Set or get Angle Threshold.
 *
 * @param device - The device structure.
 * @param thres - Threshold data to set.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_angle_thres(struct admt4000_dev *device, uint16_t thres)
{
	int ret;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	if (!thres)
		device->eck_type = ADMT4000_ALWAYS_FLAG;
	else if (thres > 0x201 && thres < 0x3FF)
		device->eck_type = ADMT4000_DISABLE_CHECK;
	else if (thres < 0x201)
		device->eck_type = ADMT4000_VALID;
	else
		return -EINVAL;

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	ret = admt4000_reg_update(device, ADMT4000_02_REG_ANGLECK,
				  ADMT4000_ANGL_CHK_MASK, no_os_field_prep(ADMT4000_ANGL_CHK_MASK, thres));
	if (ret)
		return ret;

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Set Angle Threshold (degrees).
 *
 * @param device - The device structure.
 * @param thres - Threshold data to set.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_converted_angle_thres(struct admt4000_dev *device,
				       uint32_t thres)
{
	uint16_t raw_temp;

	if (thres < 0.0 || thres > 180.0)
		return -EINVAL;

	raw_temp = thres / (admt4000_angle_conv_factors[1] / 2);

	return admt4000_set_angle_thres(device, raw_temp);
}

/***************************************************************************//**
 * @brief Get current conversion count.
 *
 * @param device - The device structure.
 * @param cnt - Stores the count data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_cnv_cnt(struct admt4000_dev *device, uint8_t *cnt)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	ret = admt4000_read(device, ADMT4000_02_REG_CNVCNT, &temp, &verif);
	if (ret)
		return ret;

	*cnt = (bool) no_os_field_get(ADMT4000_CNV_CTR_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Set current conversion count.
 *
 * @param device - The device structure.
 * @param cnt - Count data to store.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_cnv_cnt(struct admt4000_dev *device, uint8_t cnt)
{
	int ret;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	return admt4000_reg_update(device, ADMT4000_02_REG_CNVCNT,
				   ADMT4000_CNV_CTR_MASK, no_os_field_prep(ADMT4000_CNV_CTR_MASK, cnt));
}

/***************************************************************************//**
 * @brief Read from HMAG registers.
 *
 * @param device - The device structure.
 * @param hmag - The HxMAG register to interact with (1,2,3,8).
 * @param mag - Stores the HMAG data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_hmag_config(struct admt4000_dev *device, uint8_t hmag,
			     uint16_t *mag)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	switch (hmag) {
	case 1:
		ret = admt4000_read(device, ADMT4000_02_REG_H1MAG, &temp, &verif);
		break;
	case 2:
		ret = admt4000_read(device, ADMT4000_02_REG_H2MAG, &temp, &verif);
		break;
	case 3:
		ret = admt4000_read(device, ADMT4000_02_REG_H3MAG, &temp, &verif);
		break;
	case 8:
		ret = admt4000_read(device, ADMT4000_02_REG_H8MAG, &temp, &verif);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	if (hmag < 3)
		*mag = no_os_field_get(ADMT4000_H_11BIT_MAG_MASK, temp);
	else
		*mag = no_os_field_get(ADMT4000_H_8BIT_MAG_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Read from HMAG registers (converted).
 *
 * @param device - The device structure.
 * @param hmag - The HxMAG register to interact with (1,2,3,8).
 * @param mag - Stores the HMAG data (Converted).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_converted_hmag_config(struct admt4000_dev *device,
				       uint8_t hmag, uint32_t *mag)
{
	int ret;
	uint16_t temp;

	ret = admt4000_get_hmag_config(device, hmag, &temp);
	if (ret)
		return ret;

	*mag = temp * ADMT4000_HMAG_RES;

	return 0;
}

/***************************************************************************//**
 * @brief Write to HMAG registers (Raw).
 *
 * @param device - The device structure.
 * @param hmag - The HxMAG register to interact with.
 * @param mag - HMAG data to write (Raw).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_hmag_config(struct admt4000_dev *device, uint8_t hmag,
			     uint16_t mag)
{
	int ret;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	switch (hmag) {
	case 1:
		if (mag > ADMT4000_11BIT_MAX)
			return -EINVAL;
		return admt4000_reg_update(device, ADMT4000_02_REG_H1MAG,
					   ADMT4000_H_11BIT_MAG_MASK, no_os_field_prep(ADMT4000_H_11BIT_MAG_MASK, mag));
	case 2:
		if (mag > ADMT4000_11BIT_MAX)
			return -EINVAL;
		return admt4000_reg_update(device, ADMT4000_02_REG_H2MAG,
					   ADMT4000_H_11BIT_MAG_MASK, no_os_field_prep(ADMT4000_H_11BIT_MAG_MASK, mag));
	case 3:
		if (mag > ADMT4000_8BIT_MAX)
			return -EINVAL;
		return admt4000_reg_update(device, ADMT4000_02_REG_H3MAG,
					   ADMT4000_H_8BIT_MAG_MASK, no_os_field_prep(ADMT4000_H_8BIT_MAG_MASK, mag));
	case 8:
		if (mag > ADMT4000_8BIT_MAX)
			return -EINVAL;
		return admt4000_reg_update(device, ADMT4000_02_REG_H8MAG,
					   ADMT4000_H_8BIT_MAG_MASK, no_os_field_prep(ADMT4000_H_8BIT_MAG_MASK, mag));
	default:
		return -EINVAL;
	}

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Write to HMAG registers (Converted).
 *
 * @param device - The device structure.
 * @param hmag - The HxMAG register to interact with.
 * @param mag - HMAG data to write (Converted).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_converted_hmag_config(struct admt4000_dev *device,
				       uint8_t hmag, uint32_t mag)
{
	uint16_t temp;

	if (mag < 0)
		return -EINVAL;

	temp = (mag * ADMT4000_CORDIC_SCALER) / ADMT4000_HMAG_RES;

	return admt4000_set_hmag_config(device, hmag, temp);
}

/***************************************************************************//**
 * @brief Read from HPHA registers (Raw).
 *
 * @param device - The device structure.
 * @param hpha - The HxPHA register to interact with (1,2,3,8).
 * @param pha - Stores the HPHA data (Raw).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_hphase_config(struct admt4000_dev *device, uint8_t hpha,
			       uint16_t *pha)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	switch (hpha) {
	case 1:
		ret = admt4000_read(device, ADMT4000_02_REG_H1PH, &temp, &verif);
		break;
	case 2:
		ret = admt4000_read(device, ADMT4000_02_REG_H2PH, &temp, &verif);
		break;
	case 3:
		ret = admt4000_read(device, ADMT4000_02_REG_H3PH, &temp, &verif);
		break;
	case 8:
		ret = admt4000_read(device, ADMT4000_02_REG_H8PH, &temp, &verif);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	*pha = no_os_field_get(ADMT4000_H_12BIT_PHA_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Read from HPHA registers (Converted).
 *
 * @param device - The device structure.
 * @param hpha - The HxPHA register to interact with (1,2,3,8).
 * @param pha - Stores the HPHA data (Converted).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_converted_hphase_config(struct admt4000_dev *device,
		uint8_t hpha, uint32_t *pha)
{
	int ret;
	uint16_t temp;

	ret = admt4000_get_hphase_config(device, hpha, &temp);
	if (ret)
		return ret;

	*pha = temp * ADMT4000_HPHA_RES;

	return 0;
}

/***************************************************************************//**
 * @brief Write to HPHA registers (Raw).
 *
 * @param device - The device structure.
 * @param hpha - The HxPHA register to interact with (1,2,3,8).
 * @param pha - HPHA data to write (Raw).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_hphase_config(struct admt4000_dev *device, uint8_t hpha,
			       uint16_t pha)
{
	int ret;

	if (pha > ADMT4000_12BIT_MAX)
		return -EINVAL;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	/* Disable ECC functionality */
	ret = admt4000_ecc_config(device, false);
	if (ret)
		return ret;

	switch (hpha) {
	case 1:
		return admt4000_reg_update(device, ADMT4000_02_REG_H1PH,
					   ADMT4000_H_12BIT_PHA_MASK, no_os_field_prep(ADMT4000_H_12BIT_PHA_MASK, pha));
	case 2:
		return admt4000_reg_update(device, ADMT4000_02_REG_H2PH,
					   ADMT4000_H_12BIT_PHA_MASK, no_os_field_prep(ADMT4000_H_12BIT_PHA_MASK, pha));
	case 3:
		return admt4000_reg_update(device, ADMT4000_02_REG_H3PH,
					   ADMT4000_H_12BIT_PHA_MASK, no_os_field_prep(ADMT4000_H_12BIT_PHA_MASK, pha));
	case 8:
		return admt4000_reg_update(device, ADMT4000_02_REG_H8PH,
					   ADMT4000_H_12BIT_PHA_MASK, no_os_field_prep(ADMT4000_H_12BIT_PHA_MASK, pha));
	default:
		return -EINVAL;
	}

	/* Update ECC register & re-enable ECC functionality */
	return admt4000_update_ecc(device, NULL);
}

/***************************************************************************//**
 * @brief Write to HPHA registers (Converted).
 *
 * @param device - The device structure.
 * @param hpha - The HxPHA register to interact with (1,2,3,8).
 * @param pha - HPHA data to write (Converted).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_set_converted_hphase_config(struct admt4000_dev *device,
		uint8_t hpha, uint32_t pha)
{
	uint16_t temp;

	if (pha < 0)
		return -EINVAL;

	temp = (pha * ADMT4000_CORDIC_SCALER) / ADMT4000_HPHA_RES;

	return admt4000_set_hphase_config(device, hpha, temp);
}

/***************************************************************************//**
 * @brief Read Unique ID from register/s.
 *
 * @param device - The device structure.
 * @param id_num - The ID register to read from.
 * @param id - Stores the ID value read.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_get_id(struct admt4000_dev *device, uint8_t id_num, uint16_t *id)
{
	int ret;
	uint16_t temp;
	uint8_t verif;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	switch (id_num) {
	case 0:
		ret = admt4000_read(device, ADMT4000_02_REG_UNIQD0, &temp, &verif);
		break;
	case 1:
		ret = admt4000_read(device, ADMT4000_02_REG_UNIQD1, &temp, &verif);
		break;
	case 2:
		ret = admt4000_read(device, ADMT4000_02_REG_UNIQD2, &temp, &verif);
		break;
	case 3:
		ret = admt4000_read(device, ADMT4000_02_REG_UNIQD3, &temp, &verif);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	if (id_num < 1)
		*id = no_os_field_get(ADMT4000_ID0_MASK, temp);
	else
		*id = temp;

	return 0;
}

/***************************************************************************//**
 * @brief En/disables the ECC functionality.
 *
 * @param device - The device structure.
 * @param is_en - Set to true enable ECC, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_ecc_config(struct admt4000_dev *device, bool is_en)
{
	int ret;

	if (device->is_page_zero) {
		ret = admt4000_set_page(device, false);
		if (ret)
			return ret;
	}

	if (is_en)
		return admt4000_write(device, ADMT4000_02_REG_ECCDIS, ADMT4000_ECC_EN_COMM);

	return admt4000_write(device, ADMT4000_02_REG_ECCDIS, ADMT4000_ECC_DIS_COMM);
}

/***************************************************************************//**
 * @brief Transitions SDP's GPIO pin to high-logic.
 *
 * @param device - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_sdp_pulse_coil_rs(struct admt4000_dev *device)
{
	int ret;
	uint8_t direction;
	ret = no_os_gpio_get_direction(device->gpio_coil_rs, &direction);
	if (ret)
		return ret;
	if (direction != NO_OS_GPIO_OUT)
		return -EIO;

	ret = admt4000_set_cnv(device, false);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(device->gpio_coil_rs, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	// Pulse duration is 5ms and functional.
	no_os_mdelay(5);

	ret = no_os_gpio_set_value(device->gpio_coil_rs, NO_OS_GPIO_LOW);

	return ret;
}

/***************************************************************************//**
 * @brief Get value of ADMT4000 GPIO_BUSY.
 *
 * @param device - The device structure.
 * @param val - Buffer for the value.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_sdp_getval_gpio0_busy(struct admt4000_dev *device, uint8_t *val)
{
	int ret;
	uint8_t direction;

	ret = no_os_gpio_get_direction(device->gpio_gpio0_busy, &direction);
	if (ret)
		return ret;
	if (direction != NO_OS_GPIO_IN)
		return -EIO;
	ret = no_os_gpio_get_value(device->gpio_gpio0_busy, val);

	return ret;
}

/***************************************************************************//**
 * @brief Get value of ADMT4000 CNV.
 *
 * @param device - The device structure.
 * @param val - Buffer for the value.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_sdp_getval_cnv(struct admt4000_dev *device, uint8_t *val)
{
	int ret;
	uint8_t direction;

	ret = no_os_gpio_get_direction(device->gpio_cnv, &direction);
	if (ret)
		return ret;
	if (direction != NO_OS_GPIO_IN)
		return -EIO;
	ret = no_os_gpio_get_value(device->gpio_cnv, val);

	return ret;
}

/***************************************************************************//**
 * @brief Get value of ADMT4000 ACALC.
 *
 * @param device - The device structure.
 * @param val - Buffer for the value.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int admt4000_sdp_getval_acalc(struct admt4000_dev *device, uint8_t *val)
{
	int ret;
	uint8_t direction;

	ret = no_os_gpio_get_direction(device->gpio_acalc, &direction);
	if (ret)
		return ret;
	if (direction != NO_OS_GPIO_IN)
		return -EIO;
	ret = no_os_gpio_get_value(device->gpio_acalc, val);

	return ret;
}
