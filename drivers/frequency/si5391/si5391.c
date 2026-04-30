/***************************************************************************//**
 * @file si5391.c
 * @brief Implementation of Si5391 Driver.
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include "no_os_print_log.h"
#include "si5391.h"
#include "jesd204.h"

struct si5391_jesd204_priv {
	struct si5391_dev *device;
};

/******************************************************************************/
/********************** Macros and Constants Definitions ***********************/
/******************************************************************************/

/* Output register offset table */
static const uint16_t si5391_reg_output_offset[] = {
	0x0103, /* OUT0A */
	0x0108, /* OUT0 */
	0x010D, /* OUT1 */
	0x0112, /* OUT2 */
	0x0117, /* OUT3 */
	0x011C, /* OUT4 */
	0x0121, /* OUT5 */
	0x0126, /* OUT6 */
	0x012B, /* OUT7 */
	0x0130, /* OUT8 */
	0x0135, /* OUT9 */
	0x013A, /* OUT9A */
};

/* R divider register offset table */
static const uint16_t si5391_reg_rdiv_offset[] = {
	0x0247, /* R0A_REG */
	0x024A, /* R0_REG */
	0x024D, /* R1_REG */
	0x0250, /* R2_REG */
	0x0253, /* R3_REG */
	0x0256, /* R4_REG */
	0x0259, /* R5_REG */
	0x025C, /* R6_REG */
	0x025F, /* R7_REG */
	0x0262, /* R8_REG */
	0x0265, /* R9_REG */
	0x0268, /* R9A_REG */
};

/*
 * Default register configuration generated from ClockBuilder Pro.
 * These values configure the PLL, dividers, and outputs.
 */
static const struct si5391_reg_default si5391_reg_defaults[] = {
	{ 0x0006, 0x00 },
	{ 0x0007, 0x00 },
	{ 0x0008, 0x00 },
	{ 0x000B, 0x74 },
	{ 0x0017, 0xD0 },
	{ 0x0018, 0xFF },
	{ 0x0021, 0x0F },
	{ 0x0022, 0x00 },
	{ 0x002B, 0x02 },
	{ 0x002C, 0x20 },
	{ 0x002D, 0x00 },
	{ 0x002E, 0x00 },
	{ 0x002F, 0x00 },
	{ 0x0030, 0x00 },
	{ 0x0031, 0x00 },
	{ 0x0032, 0x00 },
	{ 0x0033, 0x00 },
	{ 0x0034, 0x00 },
	{ 0x0035, 0x00 },
	{ 0x0036, 0x00 },
	{ 0x0037, 0x00 },
	{ 0x0038, 0x00 },
	{ 0x0039, 0x00 },
	{ 0x003A, 0x00 },
	{ 0x003B, 0x00 },
	{ 0x003C, 0x00 },
	{ 0x003D, 0x00 },
	{ 0x0041, 0x00 },
	{ 0x0042, 0x00 },
	{ 0x0043, 0x00 },
	{ 0x0044, 0x00 },
	{ 0x009E, 0x00 },
	{ 0x0102, 0x01 },
	{ 0x0103, 0x01 },
	{ 0x0104, 0x09 },
	{ 0x0105, 0x3B },
	{ 0x0106, 0x28 },
	{ 0x0108, 0x01 },
	{ 0x0109, 0x09 },
	{ 0x010A, 0x3B },
	{ 0x010B, 0x28 },
	{ 0x010D, 0x01 },
	{ 0x010E, 0x09 },
	{ 0x010F, 0x3B },
	{ 0x0110, 0x28 },
	{ 0x0112, 0x01 },
	{ 0x0113, 0x09 },
	{ 0x0114, 0x3B },
	{ 0x0115, 0x28 },
	{ 0x0117, 0x01 },
	{ 0x0118, 0x09 },
	{ 0x0119, 0x3B },
	{ 0x011A, 0x28 },
	{ 0x011C, 0x01 },
	{ 0x011D, 0x09 },
	{ 0x011E, 0x3B },
	{ 0x011F, 0x28 },
	{ 0x0121, 0x01 },
	{ 0x0122, 0x09 },
	{ 0x0123, 0x3B },
	{ 0x0124, 0x28 },
	{ 0x0126, 0x01 },
	{ 0x0127, 0x09 },
	{ 0x0128, 0x3B },
	{ 0x0129, 0x28 },
	{ 0x012B, 0x01 },
	{ 0x012C, 0x09 },
	{ 0x012D, 0x3B },
	{ 0x012E, 0x28 },
	{ 0x0130, 0x02 },
	{ 0x0131, 0x09 },
	{ 0x0132, 0x3E },
	{ 0x0133, 0x18 },
	{ 0x0135, 0x02 },
	{ 0x0136, 0x09 },
	{ 0x0137, 0x3E },
	{ 0x0138, 0x18 },
	{ 0x013A, 0x01 },
	{ 0x013B, 0x09 },
	{ 0x013C, 0x3B },
	{ 0x013D, 0x28 },
	{ 0x013F, 0x00 },
	{ 0x0140, 0x00 },
	{ 0x0141, 0x40 },
	{ 0x0206, 0x00 },
	{ 0x0208, 0x00 },
	{ 0x0209, 0x00 },
	{ 0x020A, 0x00 },
	{ 0x020B, 0x00 },
	{ 0x020C, 0x00 },
	{ 0x020D, 0x00 },
	{ 0x020E, 0x00 },
	{ 0x020F, 0x00 },
	{ 0x0210, 0x00 },
	{ 0x0211, 0x00 },
	{ 0x0212, 0x00 },
	{ 0x0213, 0x00 },
	{ 0x0214, 0x00 },
	{ 0x0215, 0x00 },
	{ 0x0216, 0x00 },
	{ 0x0217, 0x00 },
	{ 0x0218, 0x00 },
	{ 0x0219, 0x00 },
	{ 0x021A, 0x00 },
	{ 0x021B, 0x00 },
	{ 0x021C, 0x00 },
	{ 0x021D, 0x00 },
	{ 0x021E, 0x00 },
	{ 0x021F, 0x00 },
	{ 0x0220, 0x00 },
	{ 0x0221, 0x00 },
	{ 0x0222, 0x00 },
	{ 0x0223, 0x00 },
	{ 0x0224, 0x00 },
	{ 0x0225, 0x00 },
	{ 0x0226, 0x00 },
	{ 0x0227, 0x00 },
	{ 0x0228, 0x00 },
	{ 0x0229, 0x00 },
	{ 0x022A, 0x00 },
	{ 0x022B, 0x00 },
	{ 0x022C, 0x00 },
	{ 0x022D, 0x00 },
	{ 0x022E, 0x00 },
	{ 0x022F, 0x00 },
	/* PLL configuration */
	{ 0x0235, 0x00 },
	{ 0x0236, 0x00 },
	{ 0x0237, 0x00 },
	{ 0x0238, 0x00 },
	{ 0x0239, 0xC8 },
	{ 0x023A, 0x00 },
	{ 0x023B, 0x00 },
	{ 0x023C, 0x00 },
	{ 0x023D, 0x00 },
	{ 0x023E, 0xC0 },
	/* R dividers */
	{ 0x0247, 0x00 },
	{ 0x0248, 0x00 },
	{ 0x0249, 0x00 },
	{ 0x024A, 0x00 },
	{ 0x024B, 0x00 },
	{ 0x024C, 0x00 },
	{ 0x024D, 0x00 },
	{ 0x024E, 0x00 },
	{ 0x024F, 0x00 },
	{ 0x0250, 0x00 },
	{ 0x0251, 0x00 },
	{ 0x0252, 0x00 },
	{ 0x0253, 0x00 },
	{ 0x0254, 0x00 },
	{ 0x0255, 0x00 },
	{ 0x0256, 0x00 },
	{ 0x0257, 0x00 },
	{ 0x0258, 0x00 },
	{ 0x0259, 0x00 },
	{ 0x025A, 0x00 },
	{ 0x025B, 0x00 },
	{ 0x025C, 0x00 },
	{ 0x025D, 0x00 },
	{ 0x025E, 0x00 },
	{ 0x025F, 0x00 },
	{ 0x0260, 0x00 },
	{ 0x0261, 0x00 },
	{ 0x0262, 0x03 },
	{ 0x0263, 0x00 },
	{ 0x0264, 0x00 },
	{ 0x0265, 0x01 },
	{ 0x0266, 0x00 },
	{ 0x0267, 0x00 },
	{ 0x0268, 0x00 },
	{ 0x0269, 0x00 },
	{ 0x026A, 0x00 },
	{ 0x026B, 0x00 },
	{ 0x026C, 0x00 },
	{ 0x026D, 0x00 },
	{ 0x026E, 0x00 },
	{ 0x026F, 0x00 },
	{ 0x0270, 0x00 },
	{ 0x0271, 0x00 },
	{ 0x0272, 0x00 },
	/* N dividers */
	{ 0x0302, 0x00 },
	{ 0x0303, 0x00 },
	{ 0x0304, 0x00 },
	{ 0x0305, 0x00 },
	{ 0x0306, 0x09 },
	{ 0x0307, 0x00 },
	{ 0x0308, 0x00 },
	{ 0x0309, 0x00 },
	{ 0x030A, 0x00 },
	{ 0x030B, 0x80 },
	{ 0x030C, 0x00 },
	{ 0x030D, 0x00 },
	{ 0x030E, 0x00 },
	{ 0x030F, 0x00 },
	{ 0x0310, 0x00 },
	{ 0x0311, 0x00 },
	{ 0x0312, 0x00 },
	{ 0x0313, 0x00 },
	{ 0x0314, 0x00 },
	{ 0x0315, 0x00 },
	{ 0x0316, 0x00 },
	{ 0x0317, 0x00 },
	{ 0x0318, 0x00 },
	{ 0x0319, 0x00 },
	{ 0x031A, 0x00 },
	{ 0x031B, 0x00 },
	{ 0x031C, 0x00 },
	{ 0x031D, 0x00 },
	{ 0x031E, 0x00 },
	{ 0x031F, 0x00 },
	{ 0x0320, 0x00 },
	{ 0x0321, 0x00 },
	{ 0x0322, 0x00 },
	{ 0x0323, 0x00 },
	{ 0x0324, 0x00 },
	{ 0x0325, 0x00 },
	{ 0x0326, 0x00 },
	{ 0x0327, 0x00 },
	{ 0x0328, 0x00 },
	{ 0x0329, 0x00 },
	{ 0x032A, 0x00 },
	{ 0x032B, 0x00 },
	{ 0x032C, 0x00 },
	{ 0x032D, 0x00 },
	{ 0x032E, 0x00 },
	{ 0x032F, 0x00 },
	{ 0x0330, 0x00 },
	{ 0x0331, 0x00 },
	{ 0x0332, 0x00 },
	{ 0x0333, 0x00 },
	{ 0x0334, 0x00 },
	{ 0x0335, 0x00 },
	{ 0x0336, 0x00 },
	{ 0x0337, 0x00 },
	{ 0x0338, 0x00 },
	{ 0x0339, 0x1F },
	/* Additional configuration */
	{ 0x090E, 0x02 },
	{ 0x091C, 0x04 },
	{ 0x0943, 0x00 },
	{ 0x0949, 0x00 },
	{ 0x094A, 0x00 },
	{ 0x094E, 0x49 },
	{ 0x094F, 0xF2 },
	{ 0x095E, 0x00 },
	{ 0x0A02, 0x00 },
	{ 0x0A03, 0x01 },
	{ 0x0A04, 0x01 },
	{ 0x0A05, 0x01 },
	{ 0x0A14, 0x00 },
	{ 0x0A1A, 0x00 },
	{ 0x0A20, 0x00 },
	{ 0x0A26, 0x00 },
	{ 0x0A2C, 0x00 },
	{ 0x0B44, 0x0F },
	{ 0x0B4A, 0x1E },
	{ 0x0B57, 0x0E },
	{ 0x0B58, 0x00 },
};

/* Preamble sequence */
static const struct si5391_reg_default si5391_preamble[] = {
	{ 0x0B25, 0x00 },
	{ 0x0502, 0x01 },
	{ 0x0505, 0x03 },
	{ 0x0957, 0x17 },
	{ 0x0B4E, 0x1A },
};

/******************************************************************************/
/************************ Private Functions ***********************************/
/******************************************************************************/

/*
 * The Si5391 uses a paged register space. Registers above 0xFF require
 * setting the PAGE register first. This helper manages page selection
 * and performs the actual I2C byte read/write.
 */

/**
 * @brief Write a single register on the Si5391.
 * @param dev - The device descriptor.
 * @param reg_addr - 16-bit register address (page in upper byte).
 * @param reg_data - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_reg_write(struct si5391_dev *dev, uint16_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[2];
	int32_t ret;
	uint8_t page = (reg_addr >> 8) & 0xFF;
	uint8_t addr = reg_addr & 0xFF;

	/* Set the page register */
	buf[0] = (uint8_t)(SI5391_PAGE & 0xFF);
	buf[1] = page;
	ret = no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
	if (ret != 0)
		return ret;

	/* Write the register */
	buf[0] = addr;
	buf[1] = reg_data;
	return no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
}

/**
 * @brief Read a single register from the Si5391.
 * @param dev - The device descriptor.
 * @param reg_addr - 16-bit register address (page in upper byte).
 * @param reg_data - Pointer to store the read value.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_reg_read(struct si5391_dev *dev, uint16_t reg_addr,
			uint8_t *reg_data)
{
	uint8_t buf[2];
	int32_t ret;
	uint8_t page = (reg_addr >> 8) & 0xFF;
	uint8_t addr = reg_addr & 0xFF;

	/* Set the page register */
	buf[0] = (uint8_t)(SI5391_PAGE & 0xFF);
	buf[1] = page;
	ret = no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
	if (ret != 0)
		return ret;

	/* Write register address, then read */
	buf[0] = addr;
	ret = no_os_i2c_write(dev->i2c_desc, buf, 1, 1);
	if (ret != 0)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, reg_data, 1, 1);
}

/**
 * @brief Bulk read multiple consecutive registers.
 * @param dev - The device descriptor.
 * @param reg_addr - Starting 16-bit register address.
 * @param data - Buffer to store read values.
 * @param count - Number of bytes to read.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_bulk_read(struct si5391_dev *dev, uint16_t reg_addr,
				uint8_t *data, uint8_t count)
{
	uint8_t buf[2];
	int32_t ret;
	uint8_t page = (reg_addr >> 8) & 0xFF;
	uint8_t addr = reg_addr & 0xFF;

	buf[0] = (uint8_t)(SI5391_PAGE & 0xFF);
	buf[1] = page;
	ret = no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
	if (ret != 0)
		return ret;

	buf[0] = addr;
	ret = no_os_i2c_write(dev->i2c_desc, buf, 1, 1);
	if (ret != 0)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, data, count, 1);
}

/**
 * @brief Bulk write multiple consecutive registers.
 * @param dev - The device descriptor.
 * @param reg_addr - Starting 16-bit register address.
 * @param data - Buffer of values to write.
 * @param count - Number of bytes to write.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_bulk_write(struct si5391_dev *dev, uint16_t reg_addr,
				 const uint8_t *data, uint8_t count)
{
	uint8_t buf[12]; /* page + addr + up to 10 data bytes */
	int32_t ret;
	uint8_t page = (reg_addr >> 8) & 0xFF;
	uint8_t addr = reg_addr & 0xFF;

	if (count > 10)
		return -EINVAL;

	/* Set the page register */
	buf[0] = (uint8_t)(SI5391_PAGE & 0xFF);
	buf[1] = page;
	ret = no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
	if (ret != 0)
		return ret;

	/* Write address + data */
	buf[0] = addr;
	memcpy(&buf[1], data, count);
	return no_os_i2c_write(dev->i2c_desc, buf, count + 1, 1);
}

/**
 * @brief Read-modify-write a register.
 * @param dev - The device descriptor.
 * @param reg_addr - Register address.
 * @param mask - Bits to modify.
 * @param val - New value for masked bits.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_reg_update_bits(struct si5391_dev *dev,
				      uint16_t reg_addr,
				      uint8_t mask, uint8_t val)
{
	uint8_t reg_data;
	int32_t ret;

	ret = si5391_reg_read(dev, reg_addr, &reg_data);
	if (ret != 0)
		return ret;

	reg_data = (reg_data & ~mask) | (val & mask);
	return si5391_reg_write(dev, reg_addr, reg_data);
}

/**
 * @brief Write an array of register defaults.
 * @param dev - The device descriptor.
 * @param values - Array of address/value pairs.
 * @param num_values - Number of entries.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_write_defaults(struct si5391_dev *dev,
				     const struct si5391_reg_default *values,
				     uint32_t num_values)
{
	uint32_t i;
	int32_t ret;

	for (i = 0; i < num_values; i++) {
		ret = si5391_reg_write(dev, values[i].address, values[i].value);
		if (ret != 0)
			return ret;
	}
	return 0;
}

/**
 * @brief Decode a 44-bit numerator and 32-bit denominator from registers.
 * @param dev - The device descriptor.
 * @param reg - Starting register address (10 bytes: 6 num + 4 den).
 * @param num - Output: 44-bit numerator.
 * @param den - Output: 32-bit denominator.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_decode_44_32(struct si5391_dev *dev, uint16_t reg,
				   uint64_t *num, uint32_t *den)
{
	uint8_t r[10];
	int32_t ret;

	ret = si5391_bulk_read(dev, reg, r, 10);
	if (ret != 0)
		return ret;

	/* 44-bit numerator: bytes 0..5, little-endian, top nibble of byte 5 */
	*num = ((uint64_t)(r[5] & 0x0F) << 40) |
	       ((uint64_t)r[4] << 32) |
	       ((uint64_t)r[3] << 24) |
	       ((uint64_t)r[2] << 16) |
	       ((uint64_t)r[1] << 8) |
	       (uint64_t)r[0];

	/* 32-bit denominator: bytes 6..9, little-endian */
	*den = ((uint32_t)r[9] << 24) |
	       ((uint32_t)r[8] << 16) |
	       ((uint32_t)r[7] << 8) |
	       (uint32_t)r[6];

	return 0;
}

/**
 * @brief Encode and write a 44-bit numerator and 32-bit denominator.
 * @param dev - The device descriptor.
 * @param reg - Starting register address.
 * @param n_num - 44-bit numerator.
 * @param n_den - 32-bit denominator.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_encode_44_32(struct si5391_dev *dev, uint16_t reg,
				   uint64_t n_num, uint32_t n_den)
{
	uint8_t r[10];

	/* Shift left as far as possible without overflowing */
	while (!(n_num & ((uint64_t)1 << 43)) && !(n_den & ((uint32_t)1 << 31))) {
		n_num <<= 1;
		n_den <<= 1;
	}

	/* 44 bits (6 bytes) numerator, little-endian */
	r[0] = (uint8_t)(n_num & 0xFF);
	r[1] = (uint8_t)((n_num >> 8) & 0xFF);
	r[2] = (uint8_t)((n_num >> 16) & 0xFF);
	r[3] = (uint8_t)((n_num >> 24) & 0xFF);
	r[4] = (uint8_t)((n_num >> 32) & 0xFF);
	r[5] = (uint8_t)((n_num >> 40) & 0x0F);

	/* 32 bits denominator, little-endian */
	r[6] = (uint8_t)(n_den & 0xFF);
	r[7] = (uint8_t)((n_den >> 8) & 0xFF);
	r[8] = (uint8_t)((n_den >> 16) & 0xFF);
	r[9] = (uint8_t)((n_den >> 24) & 0xFF);

	return si5391_bulk_write(dev, reg, r, 10);
}

/**
 * @brief Wait for the DEVICE_READY register to read 0x0F.
 * @param dev - The device descriptor.
 * @return 0 on success, -EIO on timeout.
 */
static int32_t si5391_wait_device_ready(struct si5391_dev *dev)
{
	uint8_t val;
	uint8_t addr;
	int32_t ret;
	int count;

	/*
	 * Datasheet: Do NOT access any register (including the PAGE register)
	 * before DEVICE_READY reads 0x0F. DEVICE_READY (0xFE) is available
	 * on every page, so read it directly via raw I2C without paging.
	 * May take up to 300ms.
	 */
	for (count = 0; count < 15; count++) {
		addr = (uint8_t)(SI5391_DEVICE_READY & 0xFF);
		ret = no_os_i2c_write(dev->i2c_desc, &addr, 1, 1);
		if (ret != 0) {
			printf("si5391: I2C write failed (attempt %d): %d\n",
			       count, ret);
			no_os_mdelay(20);
			continue;
		}
		ret = no_os_i2c_read(dev->i2c_desc, &val, 1, 1);
		if (ret != 0) {
			printf("si5391: I2C read failed (attempt %d): %d\n",
			       count, ret);
			no_os_mdelay(20);
			continue;
		}
		printf("si5391: DEVICE_READY[0xFE] = 0x%02X (attempt %d)\n",
		       val, count);
		if (val == 0x0F)
			return 0;
		no_os_mdelay(20);
	}
	printf("si5391: device ready timeout after %d attempts\n", count);
	return -EIO;
}

/**
 * @brief Probe and validate the chip ID.
 * @param dev - The device descriptor.
 * @return 0 on success, -EINVAL if chip is not recognized.
 */
static int32_t si5391_probe_chip_id(struct si5391_dev *dev)
{
	uint8_t reg[4];
	uint16_t model;
	int32_t ret;

	ret = si5391_bulk_read(dev, SI5391_PN_BASE, reg, 4);
	if (ret != 0)
		return ret;

	model = ((uint16_t)reg[1] << 8) | reg[0];

	if (model != 0x5391)
		return -EINVAL;

	dev->chip_id = model;
	dev->num_outputs = SI5391_MAX_NUM_OUTPUTS;
	dev->num_synth = SI5391_NUM_SYNTH;

	return 0;
}

/**
 * @brief Check if the PLL has already been programmed (non-zero M denominator).
 * @param dev - The device descriptor.
 * @param programmed - Output: true if already programmed.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_is_programmed(struct si5391_dev *dev, bool *programmed)
{
	uint8_t r[4];
	uint32_t m_den;
	int32_t ret;

	ret = si5391_bulk_read(dev, SI5391_PLL_M_DEN, r, 4);
	if (ret != 0)
		return ret;

	m_den = ((uint32_t)r[3] << 24) | ((uint32_t)r[2] << 16) |
		((uint32_t)r[1] << 8) | r[0];

	*programmed = (m_den != 0);
	return 0;
}

/**
 * @brief Configure the active input clock and route it to the PLL.
 * @param dev - The device descriptor.
 * @param index - Input index (0..2 for IN0..IN2, 3 for XTAL).
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_reparent(struct si5391_dev *dev, uint8_t index)
{
	int32_t ret;
	uint8_t val;

	val = (index << SI5391_IN_SEL_SHIFT) & SI5391_IN_SEL_MASK;
	val |= SI5391_IN_SEL_REGCTRL;

	ret = si5391_reg_update_bits(dev, SI5391_IN_SEL,
				     SI5391_IN_SEL_REGCTRL | SI5391_IN_SEL_MASK,
				     val);
	if (ret != 0)
		return ret;

	if (index < 3) {
		/* Enable input buffer for selected input */
		ret = si5391_reg_update_bits(dev, SI5391_IN_EN,
					     0x07, (1 << index));
		if (ret != 0)
			return ret;

		/* Enable input to phase detector */
		ret = si5391_reg_update_bits(dev, SI5391_INX_TO_PFD_EN,
					     0x7 << SI5391_INX_TO_PFD_SHIFT,
					     (1 << (index + SI5391_INX_TO_PFD_SHIFT)));
		if (ret != 0)
			return ret;

		/* Power down XTAL oscillator */
		ret = si5391_reg_update_bits(dev, SI5391_XAXB_CFG,
					     SI5391_XAXB_CFG_PDNB, 0);
		if (ret != 0)
			return ret;

		/* Set P divider to 1 */
		ret = si5391_reg_write(dev, SI5391_IN_PDIV(index), 1);
		if (ret != 0)
			return ret;

		ret = si5391_reg_write(dev, SI5391_IN_PSET(index), 1);
		if (ret != 0)
			return ret;

		ret = si5391_reg_write(dev, SI5391_PX_UPD, (1 << index));
		if (ret != 0)
			return ret;
	} else {
		/* XTAL input: disable all input buffers */
		ret = si5391_reg_update_bits(dev, SI5391_IN_EN, 0x07, 0);
		if (ret != 0)
			return ret;

		ret = si5391_reg_update_bits(dev, SI5391_INX_TO_PFD_EN,
					     0x7 << SI5391_INX_TO_PFD_SHIFT, 0);
		if (ret != 0)
			return ret;

		/* Power up XTAL oscillator, select clock mode */
		val = SI5391_XAXB_CFG_PDNB;
		if (dev->xaxb_ext_clk)
			val |= SI5391_XAXB_CFG_EXTCLK_EN;
		ret = si5391_reg_update_bits(dev, SI5391_XAXB_CFG,
					     SI5391_XAXB_CFG_PDNB |
					     SI5391_XAXB_CFG_EXTCLK_EN,
					     val);
		if (ret != 0)
			return ret;
	}

	return 0;
}

/**
 * @brief Send the preamble sequence required before programming.
 * @param dev - The device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_send_preamble(struct si5391_dev *dev)
{
	int32_t ret;
	uint8_t revision;

	ret = si5391_reg_read(dev, SI5391_DEVICE_REV, &revision);
	if (ret != 0)
		return ret;

	ret = si5391_reg_write(dev, 0x0B24, revision < 2 ? 0xD8 : 0xC0);
	if (ret != 0)
		return ret;

	ret = si5391_write_defaults(dev, si5391_preamble,
				    sizeof(si5391_preamble) /
				    sizeof(si5391_preamble[0]));
	if (ret != 0)
		return ret;

	/* Datasheet specifies a 300ms wait after preamble */
	no_os_mdelay(300);

	return 0;
}

/**
 * @brief Perform soft reset and write post-amble.
 * @param dev - The device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_finalize(struct si5391_dev *dev)
{
	int32_t ret;
	uint8_t revision;

	ret = si5391_reg_write(dev, SI5391_IO_VDD_SEL,
			       dev->iovdd_33 ? 1 : 0);
	if (ret != 0)
		return ret;

	ret = si5391_reg_read(dev, SI5391_DEVICE_REV, &revision);
	if (ret != 0)
		return ret;

	ret = si5391_reg_write(dev, SI5391_SOFT_RST, 0x01);
	if (ret != 0)
		return ret;

	ret = si5391_reg_write(dev, 0x0B24, revision < 2 ? 0xDB : 0xC3);
	if (ret != 0)
		return ret;

	ret = si5391_reg_write(dev, 0x0B25, 0x02);
	if (ret != 0)
		return ret;

	return 0;
}

/**
 * @brief Configure output format, common mode, amplitude, and VDD select.
 * @param dev - The device descriptor.
 * @param output_index - Output index (0..11).
 * @param config - Output configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t si5391_configure_output(struct si5391_dev *dev,
				       uint8_t output_index,
				       const struct si5391_output_config *config)
{
	int32_t ret;

	if (!(config->out_format_drv_bits & 0x07))
		return 0; /* Not configured */

	ret = si5391_reg_write(dev, SI5391_OUT_FORMAT(output_index),
			       config->out_format_drv_bits);
	if (ret != 0)
		return ret;

	ret = si5391_reg_write(dev, SI5391_OUT_CM(output_index),
			       config->out_cm_ampl_bits);
	if (ret != 0)
		return ret;

	ret = si5391_reg_update_bits(dev, SI5391_OUT_MUX_SEL(output_index),
				     SI5391_OUT_MUX_VDD_SEL_MASK,
				     config->vdd_sel_bits);
	if (ret != 0)
		return ret;

	return 0;
}

/**
 * @brief Compute the GCD of two 32-bit values.
 */
static uint32_t si5391_gcd(uint32_t a, uint32_t b)
{
	uint32_t t;

	while (b) {
		t = b;
		b = a % b;
		a = t;
	}
	return a;
}

/******************************************************************************/
/************************ Public Functions ************************************/
/******************************************************************************/

/**
 * @brief Get device status register.
 * @param dev - The device descriptor.
 * @param status - Output: raw status byte.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_status_get(struct si5391_dev *dev, uint8_t *status)
{
	return si5391_reg_read(dev, SI5391_STATUS, status);
}

/**
 * @brief Check if PLL is locked.
 * @param dev - The device descriptor.
 * @param locked - Output: true if PLL is locked.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_pll_locked(struct si5391_dev *dev, bool *locked)
{
	uint8_t status;
	int32_t ret;

	ret = si5391_reg_read(dev, SI5391_STATUS, &status);
	if (ret != 0)
		return ret;

	*locked = !(status & SI5391_STATUS_LOL);
	return 0;
}

/**
 * @brief Check if input clock is present.
 * @param dev - The device descriptor.
 * @param present - Output: true if input clock is present.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_input_present(struct si5391_dev *dev, bool *present)
{
	uint8_t status;
	int32_t ret;

	ret = si5391_reg_read(dev, SI5391_STATUS, &status);
	if (ret != 0)
		return ret;

	*present = !(status & SI5391_STATUS_LOSREF);
	return 0;
}

/**
 * @brief Clear sticky status bits.
 * @param dev - The device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_clear_sticky(struct si5391_dev *dev)
{
	return si5391_reg_write(dev, SI5391_STATUS_STICKY, 0);
}

/**
 * @brief Select the active input clock.
 * @param dev - The device descriptor.
 * @param input - Input index (0..3).
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_input_select(struct si5391_dev *dev, uint8_t input)
{
	if (input >= SI5391_NUM_INPUTS)
		return -EINVAL;

	dev->active_input = input;
	return si5391_reparent(dev, input);
}

/**
 * @brief Enable a synthesizer.
 * @param dev - The device descriptor.
 * @param synth_index - Synthesizer index (0..4).
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_synth_enable(struct si5391_dev *dev, uint8_t synth_index)
{
	uint8_t mask;
	int32_t ret;

	if (synth_index >= dev->num_synth)
		return -EINVAL;

	mask = (1 << synth_index);

	/* Power up */
	ret = si5391_reg_update_bits(dev, SI5391_SYNTH_N_PDNB, mask, mask);
	if (ret != 0)
		return ret;

	/* Enable clock input (bit=0 means enabled) */
	ret = si5391_reg_update_bits(dev, SI5391_SYNTH_N_CLK_DIS, mask, 0);
	if (ret != 0)
		return ret;

	/* Enable output */
	return si5391_reg_update_bits(dev, SI5391_SYNTH_N_CLK_TO_OUTX_EN,
				      mask, mask);
}

/**
 * @brief Disable a synthesizer.
 * @param dev - The device descriptor.
 * @param synth_index - Synthesizer index (0..4).
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_synth_disable(struct si5391_dev *dev, uint8_t synth_index)
{
	uint8_t mask;
	int32_t ret;

	if (synth_index >= dev->num_synth)
		return -EINVAL;

	mask = (1 << synth_index);

	/* Disable output */
	ret = si5391_reg_update_bits(dev, SI5391_SYNTH_N_CLK_TO_OUTX_EN,
				     mask, 0);
	if (ret != 0)
		return ret;

	/* Power down */
	ret = si5391_reg_update_bits(dev, SI5391_SYNTH_N_PDNB, mask, 0);
	if (ret != 0)
		return ret;

	/* Disable clock input (bit=1 means disabled) */
	return si5391_reg_update_bits(dev, SI5391_SYNTH_N_CLK_DIS, mask, mask);
}

/**
 * @brief Get the output frequency of a synthesizer.
 * @param dev - The device descriptor.
 * @param synth_index - Synthesizer index (0..4).
 * @param freq - Output: frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_synth_get_freq(struct si5391_dev *dev, uint8_t synth_index,
			      uint64_t *freq)
{
	uint64_t n_num;
	uint32_t n_den;
	uint64_t f;
	int32_t ret;

	if (synth_index >= dev->num_synth)
		return -EINVAL;

	ret = si5391_decode_44_32(dev, SI5391_SYNTH_N_NUM(synth_index),
				  &n_num, &n_den);
	if (ret != 0)
		return ret;

	if (!n_num || !n_den) {
		*freq = 0;
		return 0;
	}

	/*
	 * Output = VCO * n_den / n_num
	 * n_num and n_den are shifted left, so shift n_den right by 4
	 * to avoid overflow.
	 */
	f = dev->freq_vco;
	f *= (n_den >> 4);
	f /= (n_num >> 4);

	*freq = f;
	return 0;
}

/**
 * @brief Set the output frequency of a synthesizer.
 * @param dev - The device descriptor.
 * @param synth_index - Synthesizer index (0..4).
 * @param freq - Desired frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_synth_set_freq(struct si5391_dev *dev, uint8_t synth_index,
			      uint64_t freq)
{
	uint64_t n_num;
	uint32_t n_den;
	uint32_t remainder;
	uint32_t g;
	bool is_integer;
	int32_t ret;

	if (synth_index >= dev->num_synth || freq == 0)
		return -EINVAL;

	/* N = VCO / freq; n_num = integer part, remainder for fractional */
	n_num = dev->freq_vco;
	remainder = (uint32_t)(n_num % freq);
	n_num = n_num / freq;

	is_integer = (remainder == 0);
	if (is_integer) {
		n_den = 1;
	} else {
		g = si5391_gcd(remainder, (uint32_t)freq);
		n_den = (uint32_t)freq / g;
		n_num *= n_den;
		n_num += remainder / g;
	}

	/* Write N numerator and denominator */
	ret = si5391_encode_44_32(dev, SI5391_SYNTH_N_NUM(synth_index),
				  n_num, n_den);
	if (ret != 0)
		return ret;

	/* Set phase interpolator bypass for integer mode */
	ret = si5391_reg_update_bits(dev, SI5391_SYNTH_N_PIBYP,
				     (1 << synth_index),
				     is_integer ? (1 << synth_index) : 0);
	if (ret != 0)
		return ret;

	/* Trigger update */
	return si5391_reg_write(dev, SI5391_SYNTH_N_UPD(synth_index), 0x01);
}

/**
 * @brief Enable an output.
 * @param dev - The device descriptor.
 * @param output_index - Output index (0..11).
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_output_enable(struct si5391_dev *dev, uint8_t output_index)
{
	int32_t ret;

	if (output_index >= dev->num_outputs)
		return -EINVAL;

	/* Power up (clear PDN) */
	ret = si5391_reg_update_bits(dev, SI5391_OUT_CONFIG(output_index),
				     SI5391_OUT_CFG_PDN, 0);
	if (ret != 0)
		return ret;

	/* Enable output (set OE) */
	return si5391_reg_update_bits(dev, SI5391_OUT_CONFIG(output_index),
				      SI5391_OUT_CFG_OE, SI5391_OUT_CFG_OE);
}

/**
 * @brief Disable an output.
 * @param dev - The device descriptor.
 * @param output_index - Output index (0..11).
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_output_disable(struct si5391_dev *dev, uint8_t output_index)
{
	int32_t ret;

	if (output_index >= dev->num_outputs)
		return -EINVAL;

	/* Disable output (clear OE) */
	ret = si5391_reg_update_bits(dev, SI5391_OUT_CONFIG(output_index),
				     SI5391_OUT_CFG_OE, 0);
	if (ret != 0)
		return ret;

	/* Power down (set PDN) */
	return si5391_reg_update_bits(dev, SI5391_OUT_CONFIG(output_index),
				      SI5391_OUT_CFG_PDN, SI5391_OUT_CFG_PDN);
}

/**
 * @brief Get the output frequency including R divider.
 * @param dev - The device descriptor.
 * @param output_index - Output index (0..11).
 * @param freq - Output: frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_output_get_freq(struct si5391_dev *dev, uint8_t output_index,
			       uint64_t *freq)
{
	uint8_t cfg;
	uint8_t r[3];
	uint8_t mux;
	uint32_t r_divider;
	uint64_t synth_freq;
	int32_t ret;

	if (output_index >= dev->num_outputs)
		return -EINVAL;

	/* Find which synth feeds this output */
	ret = si5391_reg_read(dev, SI5391_OUT_MUX_SEL(output_index), &mux);
	if (ret != 0)
		return ret;
	mux &= 0x07;

	/* Get synth frequency */
	ret = si5391_synth_get_freq(dev, mux, &synth_freq);
	if (ret != 0)
		return ret;

	/* Check RDIV_FORCE2 */
	ret = si5391_reg_read(dev, SI5391_OUT_CONFIG(output_index), &cfg);
	if (ret != 0)
		return ret;

	if (cfg & SI5391_OUT_CFG_RDIV_FORCE2) {
		*freq = synth_freq / 2;
		return 0;
	}

	/* Read 24-bit R divider register */
	ret = si5391_bulk_read(dev, SI5391_OUT_R_REG(output_index), r, 3);
	if (ret != 0)
		return ret;

	r_divider = ((uint32_t)r[2] << 16) | ((uint32_t)r[1] << 8) | r[0];

	if (r_divider == 0) {
		*freq = 0;
		return 0;
	}

	/* Actual divider is 2 * (R_REG + 1) */
	r_divider = (r_divider + 1) * 2;
	*freq = synth_freq / r_divider;

	return 0;
}

/**
 * @brief Set the output frequency via the R divider.
 * @param dev - The device descriptor.
 * @param output_index - Output index (0..11).
 * @param freq - Desired frequency in Hz.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_output_set_freq(struct si5391_dev *dev, uint8_t output_index,
			       uint64_t freq)
{
	uint8_t mux;
	uint64_t synth_freq;
	uint32_t r_div;
	uint8_t r[3];
	int32_t ret;

	if (output_index >= dev->num_outputs || freq == 0)
		return -EINVAL;

	/* Find which synth feeds this output */
	ret = si5391_reg_read(dev, SI5391_OUT_MUX_SEL(output_index), &mux);
	if (ret != 0)
		return ret;
	mux &= 0x07;

	ret = si5391_synth_get_freq(dev, mux, &synth_freq);
	if (ret != 0)
		return ret;

	/* R divider = (synth_freq / freq) / 2, register value = R_div - 1 */
	r_div = (uint32_t)(synth_freq / freq) >> 1;

	if (r_div <= 1)
		r_div = 0;
	else if (r_div >= (1 << 24))
		r_div = (1 << 24) - 1;
	else
		r_div--;

	/* For divide-by-2, set RDIV_FORCE2 bit */
	ret = si5391_reg_update_bits(dev, SI5391_OUT_CONFIG(output_index),
				     SI5391_OUT_CFG_RDIV_FORCE2,
				     (r_div == 0) ? SI5391_OUT_CFG_RDIV_FORCE2 : 0);
	if (ret != 0)
		return ret;

	/* Write R divider; a zero value disables it, so write 1 as minimum */
	r[0] = r_div ? (r_div & 0xFF) : 1;
	r[1] = (r_div >> 8) & 0xFF;
	r[2] = (r_div >> 16) & 0xFF;

	return si5391_bulk_write(dev, SI5391_OUT_R_REG(output_index), r, 3);
}

/**
 * @brief Set which synthesizer drives an output.
 * @param dev - The device descriptor.
 * @param output_index - Output index (0..11).
 * @param synth_index - Synthesizer index (0..4).
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_output_set_mux(struct si5391_dev *dev, uint8_t output_index,
			      uint8_t synth_index)
{
	if (output_index >= dev->num_outputs ||
	    synth_index >= dev->num_synth)
		return -EINVAL;

	return si5391_reg_update_bits(dev, SI5391_OUT_MUX_SEL(output_index),
				     0x07, synth_index);
}

/**
 * @brief Get which synthesizer drives an output.
 * @param dev - The device descriptor.
 * @param output_index - Output index (0..11).
 * @param synth_index - Output: synthesizer index.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_output_get_mux(struct si5391_dev *dev, uint8_t output_index,
			      uint8_t *synth_index)
{
	uint8_t val;
	int32_t ret;

	if (output_index >= dev->num_outputs)
		return -EINVAL;

	ret = si5391_reg_read(dev, SI5391_OUT_MUX_SEL(output_index), &val);
	if (ret != 0)
		return ret;

	*synth_index = val & 0x07;
	return 0;
}

/**
 * @brief Recalculate the VCO frequency from the PLL M divider and input clock.
 * @param dev - The device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_recalc_vco(struct si5391_dev *dev)
{
	uint64_t m_num;
	uint32_t m_den;
	uint64_t res;
	uint32_t shift;
	int32_t ret;

	ret = si5391_decode_44_32(dev, SI5391_PLL_M_NUM, &m_num, &m_den);
	if (ret != 0)
		return ret;

	if (!m_num || !m_den)
		return -EIO;

	/* Shift right to fit in 64-bit math */
	shift = 0;
	res = m_num;
	while (res & 0xFFFF00000000ULL) {
		shift++;
		res >>= 1;
	}
	res *= dev->input_clk_freq;
	res /= (m_den >> shift);

	dev->freq_vco = res;
	return 0;
}

/**
 * @brief Synchronize all output clock dividers.
 *
 * Issues a soft reset to the output dividers to synchronize them.
 * This is the Si5391 equivalent of an output sync operation.
 *
 * @param dev - The device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_sync(struct si5391_dev *dev)
{
	int32_t ret;
	uint8_t status;
	uint32_t timeout;

	/* Issue a soft reset to resync dividers */
	ret = si5391_reg_write(dev, SI5391_SOFT_RST, 0x01);
	if (ret != 0)
		return ret;

	/* Wait for calibration to complete */
	timeout = 25;
	while (timeout--) {
		ret = si5391_reg_read(dev, SI5391_STATUS, &status);
		if (ret != 0)
			return ret;
		if (!(status & SI5391_STATUS_SYSINCAL))
			break;
		no_os_mdelay(10);
	}

	if (status & SI5391_STATUS_SYSINCAL)
		return -EIO;

	/* Clear sticky bits after sync */
	return si5391_reg_write(dev, SI5391_STATUS_STICKY, 0);
}

/******************************************************************************/
/************************ JESD204 FSM Ops ************************************/
/******************************************************************************/

/**
 * @brief JESD204 link_supported callback.
 *
 * Called during JESD204_OP_LINK_SUPPORTED for each link. Validates that the
 * LMFC/LEMC rate is compatible with the Si5391 output frequencies and
 * computes the GCD for SYSREF divider calculation.
 */
static int si5391_jesd204_link_supported(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct si5391_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct si5391_dev *dev = priv->device;
	int ret;
	unsigned long rate;

	if (reason != JESD204_STATE_OP_REASON_INIT) {
		dev->jdev_lmfc_lemc_rate = 0;
		dev->jdev_lmfc_lemc_gcd = 0;
		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	ret = jesd204_link_get_lmfc_lemc_rate(lnk, &rate);
	if (ret < 0)
		return ret;

	if (dev->jdev_lmfc_lemc_rate) {
		dev->jdev_lmfc_lemc_rate =
			no_os_min(dev->jdev_lmfc_lemc_rate, (uint32_t)rate);
		dev->jdev_lmfc_lemc_gcd =
			no_os_greatest_common_divisor(dev->jdev_lmfc_lemc_gcd,
						      (uint32_t)rate);
	} else {
		dev->jdev_lmfc_lemc_rate = (uint32_t)rate;
		dev->jdev_lmfc_lemc_gcd = (uint32_t)rate;
	}

	pr_debug("%s:%d link_num %u LMFC/LEMC %u/%lu gcd %u\n",
		 __func__, __LINE__, lnk->link_id, dev->jdev_lmfc_lemc_rate,
		 rate, dev->jdev_lmfc_lemc_gcd);

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 link_pre_setup callback.
 *
 * Called during JESD204_OP_LINK_PRE_SETUP for each link. Adjusts the
 * SYSREF-related output divider if a SYSREF output is being generated
 * by the Si5391.
 */
static int si5391_jesd204_link_pre_setup(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct si5391_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct si5391_dev *dev = priv->device;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	/*
	 * Adjust SYSREF rate if desired/max frequency constraints are set.
	 * The SYSREF rate must be a sub-multiple of the LMFC/LEMC GCD.
	 */
	if (dev->jdev_desired_sysref_freq &&
	    (dev->jdev_lmfc_lemc_gcd % dev->jdev_desired_sysref_freq == 0)) {
		dev->jdev_lmfc_lemc_gcd = dev->jdev_desired_sysref_freq;
	} else if (dev->jdev_max_sysref_freq) {
		while (dev->jdev_lmfc_lemc_gcd > dev->jdev_max_sysref_freq &&
		       (dev->jdev_lmfc_lemc_gcd %
			(dev->jdev_lmfc_lemc_gcd >> 1) == 0))
			dev->jdev_lmfc_lemc_gcd >>= 1;
	}

	pr_debug("%s:%d SYSREF target rate %u Hz\n", __func__, __LINE__,
		 dev->jdev_lmfc_lemc_gcd);

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 clk_sync callback.
 *
 * Called during JESD204_OP_CLK_SYNC_STAGE1. Synchronizes the Si5391
 * output dividers so that all clocks have a deterministic phase
 * relationship.
 */
static int si5391_jesd204_clks_sync(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason)
{
	struct si5391_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct si5391_dev *dev = priv->device;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	ret = si5391_sync(dev);
	if (ret)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 sysref_cb — issue a SYSREF pulse by resyncing output dividers.
 */
static int si5391_jesd204_sysref(struct jesd204_dev *jdev)
{
	struct si5391_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct si5391_dev *dev = priv->device;

	pr_debug("%s:%d\n", __func__, __LINE__);

	return si5391_sync(dev);
}

static const struct jesd204_dev_data jesd204_si5391_init = {
	.sysref_cb = si5391_jesd204_sysref,
	.sizeof_priv = sizeof(struct si5391_jesd204_priv),
	.state_ops = {
		[JESD204_OP_LINK_SUPPORTED] = {
			.per_link = si5391_jesd204_link_supported,
		},
		[JESD204_OP_LINK_PRE_SETUP] = {
			.per_link = si5391_jesd204_link_pre_setup,
		},
		[JESD204_OP_CLK_SYNC_STAGE1] = {
			.per_device = si5391_jesd204_clks_sync,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},
};

/**
 * @brief Initialize the Si5391 device.
 * @param device - Pointer to the device descriptor (output).
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_setup(struct si5391_dev **device,
		     const struct si5391_init_param *init_param)
{
	struct si5391_jesd204_priv *priv;
	struct si5391_dev *dev;
	int32_t ret;
	uint32_t i;
	bool already_programmed;
	uint8_t status;
	uint32_t timeout;

	dev = (struct si5391_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* Initialize I2C */
	printf("si5391: i2c_init...\n");
	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (ret != 0) {
		printf("si5391: i2c_init failed: %d\n", ret);
		goto error_free;
	}
	printf("si5391: i2c_init OK\n");

	dev->input_clk_freq = init_param->input_clk_freq;
	dev->active_input = init_param->active_input;
	dev->xaxb_ext_clk = init_param->xaxb_ext_clk;
	dev->iovdd_33 = init_param->iovdd_33;
	memcpy(dev->output_config, init_param->output_config,
	       sizeof(dev->output_config));

	/* Wait for device to be ready after power-up */
	printf("si5391: waiting for DEVICE_READY...\n");
	ret = si5391_wait_device_ready(dev);
	if (ret != 0) {
		/*
		 * Device responded on I2C but DEVICE_READY stuck at 0x00.
		 * This happens when NVM is blank — the device never self-boots.
		 * Try to program it anyway: send preamble + defaults, then
		 * re-check DEVICE_READY.
		 */
		printf("si5391: DEVICE_READY timeout — attempting blind programming (NVM may be blank)\n");

		ret = si5391_send_preamble(dev);
		if (ret != 0) {
			printf("si5391: preamble write failed: %d — going dummy\n", ret);
			dev->dummy = true;
			goto skip_hw;
		}

		ret = si5391_write_defaults(dev, si5391_reg_defaults,
					    sizeof(si5391_reg_defaults) /
					    sizeof(si5391_reg_defaults[0]));
		if (ret != 0) {
			printf("si5391: defaults write failed: %d — going dummy\n", ret);
			dev->dummy = true;
			goto skip_hw;
		}

		/* Re-check DEVICE_READY after programming */
		ret = si5391_wait_device_ready(dev);
		if (ret != 0) {
			printf("si5391: still not ready after programming — going dummy\n");
			dev->dummy = true;
			goto skip_hw;
		}
		printf("si5391: device ready after blind programming\n");
		already_programmed = true;
		goto skip_program;
	}

	dev->dummy = false;

	/* Identify the chip */
	printf("si5391: probing chip ID...\n");
	ret = si5391_probe_chip_id(dev);
	if (ret != 0) {
		printf("si5391: probe_chip_id failed: %d\n", ret);
		goto error_i2c;
	}
	printf("si5391: chip ID OK (0x%04X)\n", dev->chip_id);

	/* Determine if initialization is needed */
	if (init_param->reprogram) {
		already_programmed = false;
	} else {
		ret = si5391_is_programmed(dev, &already_programmed);
		if (ret != 0)
			goto error_i2c;
	}

	if (!already_programmed) {
		printf("si5391: sending preamble...\n");
		ret = si5391_send_preamble(dev);
		if (ret != 0) {
			printf("si5391: preamble failed: %d\n", ret);
			goto error_i2c;
		}

		printf("si5391: writing defaults...\n");
		ret = si5391_write_defaults(dev, si5391_reg_defaults,
					    sizeof(si5391_reg_defaults) /
					    sizeof(si5391_reg_defaults[0]));
		if (ret != 0) {
			printf("si5391: write_defaults failed: %d\n", ret);
			goto error_i2c;
		}
	}

skip_program:

	/* Select and enable the input clock */
	printf("si5391: reparent input %d...\n", dev->active_input);
	ret = si5391_reparent(dev, dev->active_input);
	if (ret != 0) {
		printf("si5391: reparent failed: %d\n", ret);
		goto error_i2c;
	}

	/* Configure PLL M divider if provided */
	if (init_param->pll_m_num && init_param->pll_m_den) {
		printf("si5391: setting PLL M = %u/%u...\n",
		       (unsigned)init_param->pll_m_num,
		       (unsigned)init_param->pll_m_den);
		ret = si5391_encode_44_32(dev, SI5391_PLL_M_NUM,
					  init_param->pll_m_num,
					  init_param->pll_m_den);
		if (ret != 0) {
			printf("si5391: PLL M config failed: %d\n", ret);
			goto error_i2c;
		}
	}

	/* Configure outputs */
	for (i = 0; i < SI5391_MAX_NUM_OUTPUTS; i++) {
		ret = si5391_configure_output(dev, i,
					      &init_param->output_config[i]);
		if (ret != 0) {
			printf("si5391: configure_output[%u] failed: %d\n", i, ret);
			goto error_i2c;
		}
	}

	if (!already_programmed) {
		printf("si5391: finalizing...\n");
		ret = si5391_finalize(dev);
		if (ret != 0) {
			printf("si5391: finalize failed: %d\n", ret);
			goto error_i2c;
		}
	}

	/* Wait for input clock present and PLL lock */
	printf("si5391: waiting for PLL lock...\n");
	timeout = 25;
	while (timeout--) {
		ret = si5391_reg_read(dev, SI5391_STATUS, &status);
		if (ret != 0)
			goto error_i2c;
		if (!(status & (SI5391_STATUS_LOSREF | SI5391_STATUS_LOL)))
			break;
		no_os_mdelay(10);
	}
	if (status & (SI5391_STATUS_LOSREF | SI5391_STATUS_LOL)) {
		printf("si5391: PLL lock failed — status=0x%02X (LOSREF=%d LOL=%d)\n",
		       status,
		       !!(status & SI5391_STATUS_LOSREF),
		       !!(status & SI5391_STATUS_LOL));
		ret = -EIO;
		goto error_i2c;
	}

	/* Clear sticky alarm bits */
	ret = si5391_reg_write(dev, SI5391_STATUS_STICKY, 0);
	if (ret != 0)
		goto error_i2c;

	/* Calculate VCO frequency */
	ret = si5391_recalc_vco(dev);
	if (ret != 0)
		goto error_i2c;

skip_hw:

	/* Store JESD204 SYSREF parameters */
	dev->jdev_max_sysref_freq = init_param->jdev_max_sysref_freq;
	dev->jdev_desired_sysref_freq = init_param->jdev_desired_sysref_freq;

	/* Register with the JESD204 framework */
	ret = jesd204_dev_register(&dev->jdev, &jesd204_si5391_init);
	if (ret != 0)
		goto error_i2c;

	priv = jesd204_dev_priv(dev->jdev);
	priv->device = dev;

	*device = dev;
	return 0;

error_i2c:
	no_os_i2c_remove(dev->i2c_desc);
error_free:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free the resources allocated by si5391_setup().
 * @param dev - The device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int32_t si5391_remove(struct si5391_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -EINVAL;

	ret = jesd204_dev_unregister(dev->jdev);
	if (ret != 0)
		return ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	no_os_free(dev);

	return ret;
}
