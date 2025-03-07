/***************************************************************************//**
 *   @file   ad7606.c
 *   @brief  Implementation of ad7606 Driver.
 *   @author Stefan Popa (stefan.popa@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ad7606.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_crc.h"
#include "no_os_alloc.h"

#ifdef XILINX_PLATFORM
#include "spi_engine.h"
#include "no_os_axi_io.h"
#include <unistd.h>
#include <errno.h>
#endif

#define AD7606_SPI_ENG_DATA_WIDTH			0x0C
#define AD7606_SPI_ENG_OFFLOAD_ADDR_WIDTH		0x10
#define AD7606_SPI_ENG_OFFLOAD_FIFO_WIDTH		0x14

#define AD7606_CONFIG_WR				0x80
#define AD7606_CONFIG_RD				0x84

#define AD7606_CONFIG_CTRL				0x8C
#define AD7606_CONFIG_CTRL_DEFAULT			0x00
#define AD7606_CONFIG_CTRL_READ_OP			0x03
#define AD7606_CONFIG_CTRL_WRITE_OP			0x01

#define AD7606_CHAN_CTRL(c)				(0x0400 + (c) * 0x40)
#define AD7606_CHAN_CTRL_ENABLE				0x01
#define AD7606_CHAN_CTRL_DISABLE			0x00

#define AD7606_CORE_CNTRL_3				0x4C

#define AD7606_CORE_RESET				0x40

#define AD7606_SERIAL_CORE_ENABLE			0x00
#define AD7606_SERIAL_CORE_DISABLE			0x01

#define AD7606_PARALLEL_CORE_ENABLE			0x01
#define AD7606_PARALLEL_CORE_DISABLE			0x00

struct ad7606_chip_info {
	const char *name;
	uint8_t num_channels;
	uint8_t bits;
	uint8_t max_dout_lines;
	bool has_oversampling;
	bool has_registers;
	uint8_t device_id;
	const struct ad7606_range *hw_range_table;
	uint32_t hw_range_table_sz;
	const struct ad7606_range *sw_range_table;
	uint32_t sw_range_table_sz;
};

NO_OS_DECLARE_CRC8_TABLE(ad7606_crc8);
NO_OS_DECLARE_CRC16_TABLE(ad7606_crc16);

static const struct ad7606_range ad7606_range_table[] = {
	{-5000, 5000, AD7606_HW_RANGE},		/* RANGE pin LOW */
	{-10000, 10000, AD7606_HW_RANGE},	/* RANGE pin HIGH */
};

static const struct ad7606_range ad7609_range_table[] = {
	{-10000, 10000, AD7606_HW_RANGE},	/* RANGE pin LOW */
	{-20000, 20000, AD7606_HW_RANGE},	/* RANGE pin HIGH */
};

static const struct ad7606_range ad7606b_range_table[] = {
	{-2500, 2500, AD7606_SW_RANGE_SINGLE_ENDED_BIPOLAR},	/* 0000 */
	{-5000, 5000, AD7606_SW_RANGE_SINGLE_ENDED_BIPOLAR},	/* 0001 */
	{-10000, 10000, AD7606_SW_RANGE_SINGLE_ENDED_BIPOLAR},	/* 0010-1011 */
	/* Register values can be selected all the way up to 1011,
	 * but they will all configure the last range in this array
	 */
};

static const struct ad7606_range ad7606c_range_table[] = {
	{-2500, 2500, AD7606_SW_RANGE_SINGLE_ENDED_BIPOLAR},	/* 0000 */
	{-5000, 5000, AD7606_SW_RANGE_SINGLE_ENDED_BIPOLAR},	/* 0001 */
	{-6250, 6250, AD7606_SW_RANGE_SINGLE_ENDED_BIPOLAR},	/* 0010 */
	{-10000, 10000, AD7606_SW_RANGE_SINGLE_ENDED_BIPOLAR},	/* 0011 */
	{-12500, 12500, AD7606_SW_RANGE_SINGLE_ENDED_BIPOLAR},	/* 0100 */
	{0, 5000, AD7606_SW_RANGE_SINGLE_ENDED_UNIPOLAR},	/* 0101 */
	{0, 10000, AD7606_SW_RANGE_SINGLE_ENDED_UNIPOLAR},	/* 0110 */
	{0, 12500, AD7606_SW_RANGE_SINGLE_ENDED_UNIPOLAR},	/* 0111 */
	{-5000, 5000, AD7606_SW_RANGE_DIFFERENTIAL_BIPOLAR},	/* 1000 */
	{-10000, 10000, AD7606_SW_RANGE_DIFFERENTIAL_BIPOLAR},	/* 1001 */
	{-12500, 12500, AD7606_SW_RANGE_DIFFERENTIAL_BIPOLAR},	/* 1010 */
	{-20000, 20000, AD7606_SW_RANGE_DIFFERENTIAL_BIPOLAR},	/* 1011 */
	/* Register values can be selected all the way up to 1111,
	 * but they will all configure the last range in this array
	 */
};

static const struct ad7606_chip_info ad7606_chip_info_tbl[] = {
	[ID_AD7605_4] = {
		.name = "AD7605-4",
		.num_channels = 4,
		.bits = 16,
		.max_dout_lines = AD7606_2_DOUT,
		.has_oversampling = false,
		.hw_range_table = ad7606_range_table,
		.hw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606_range_table),
	},
	[ID_AD7606_4] = {
		.name = "AD7606-4",
		.num_channels = 4,
		.bits = 16,
		.max_dout_lines = AD7606_2_DOUT,
		.has_oversampling = true,
		.hw_range_table = ad7606_range_table,
		.hw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606_range_table),
	},
	[ID_AD7606_6] = {
		.name = "AD7606-6",
		.num_channels = 6,
		.bits = 16,
		.max_dout_lines = AD7606_2_DOUT,
		.has_oversampling = true,
		.hw_range_table = ad7606_range_table,
		.hw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606_range_table),
	},
	[ID_AD7606_8] = {
		.name = "AD7606-8",
		.num_channels = 8,
		.bits = 16,
		.max_dout_lines = AD7606_2_DOUT,
		.has_oversampling = true,
		.hw_range_table = ad7606_range_table,
		.hw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606_range_table),
	},
	[ID_AD7606B] = {
		.name = "AD7606B",
		.num_channels = 8,
		.bits = 16,
		.max_dout_lines = AD7606_4_DOUT,
		.has_oversampling = true,
		.has_registers = true,
		.device_id = 0x1,
		.hw_range_table = ad7606_range_table,
		.hw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606_range_table),
		.sw_range_table = ad7606b_range_table,
		.sw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606b_range_table),
	},
	[ID_AD7606C_16] = {
		.name = "AD7606C-16",
		.num_channels = 8,
		.bits = 16,
		.max_dout_lines = AD7606_8_DOUT,
		.has_oversampling = true,
		.has_registers = true,
		.device_id = 0x2,
		.hw_range_table = ad7606_range_table,
		.hw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606_range_table),
		.sw_range_table = ad7606c_range_table,
		.sw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606c_range_table),
	},
	[ID_AD7606C_18] = {
		.name = "AD7606C-18",
		.num_channels = 8,
		.bits = 18,
		.max_dout_lines = AD7606_8_DOUT,
		.has_oversampling = true,
		.has_registers = true,
		.device_id = 0x3,
		.hw_range_table = ad7606_range_table,
		.hw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606_range_table),
		.sw_range_table = ad7606c_range_table,
		.sw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606c_range_table),
	},
	[ID_AD7608] = {
		.name = "AD7608",
		.num_channels = 8,
		.bits = 18,
		.max_dout_lines = AD7606_2_DOUT,
		.has_oversampling = true,
		.hw_range_table = ad7606_range_table,
		.hw_range_table_sz = NO_OS_ARRAY_SIZE(ad7606_range_table),
	},
	[ID_AD7609] = {
		.name = "AD7609",
		.num_channels = 8,
		.bits = 18,
		.max_dout_lines = AD7606_2_DOUT,
		.has_oversampling = true,
		.hw_range_table = ad7609_range_table,
		.hw_range_table_sz = NO_OS_ARRAY_SIZE(ad7609_range_table),
	},
};

static const uint16_t tconv_max[] = {
	1, 	/* AD7606_OSR_1 */
	3,	/* AD7606_OSR_2 */
	5,	/* AD7606_OSR_4 */
	10,	/* AD7606_OSR_8 */
	20,	/* AD7606_OSR_16 */
	41,	/* AD7606_OSR_32 */
	81,	/* AD7606_OSR_64 */
	162,	/* AD7606_OSR_128 */
	324	/* AD7606_OSR_256 */
};

/**
 * @struct ad7606_axi_dev
 * @brief Structure for AXI FPGA cores
 */
struct ad7606_axi_dev {
	/* Set to 'true' if the AXI modules have been initialized */
	bool initialized;
#ifdef XILINX_PLATFORM
	/* Clock gen for hdl design structure */
	struct axi_clkgen *clkgen;
	/* Trigger conversion PWM generator descriptor */
	struct no_os_pwm_desc *trigger_pwm_desc;
	/* SPI Engine offload parameters */
	struct spi_engine_offload_init_param offload_init_param;
	/* AXI DMA controller for parallel sample capture */
	struct axi_dmac *dmac;
	/* AXI Core */
	uint32_t core_baseaddr;
	/* RX DMA base address */
	uint32_t rx_dma_baseaddr;
	uint32_t reg_access_speed;
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
#endif
};

/***************************************************************************//**
 * @brief Returns the number of channels this ADC has
 *
 * @param dev  - The device structure.
 * @return ret - number of channels or -EINVAL if 'dev' is null.
 *
*******************************************************************************/
int32_t ad7606_get_channels_number(struct ad7606_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->num_channels;
}

/***************************************************************************//**
 * @brief Write register using AXI core via Parallel interface
 *
 * @param dev        - The device structure.
 * @param reg        - Register address
 * @param val        - Register value
 *
*******************************************************************************/
static int ad7606_parallel_mode_write_reg(struct ad7606_dev *dev,
		uint8_t reg_addr,
		uint8_t reg_data)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;
	uint32_t wr = AD7606_PARALLEL_WR_FLAG_MSK(reg_addr) << 8 | reg_data;

	/* Some waits are required, such that the core can talk to the ADC */

	no_os_axi_io_write(axi->core_baseaddr, AD7606_CONFIG_WR, wr);
	usleep(1);
	no_os_axi_io_write(axi->core_baseaddr, AD7606_CONFIG_CTRL,
			   AD7606_CONFIG_CTRL_WRITE_OP);
	usleep(1);
	no_os_axi_io_write(axi->core_baseaddr, AD7606_CONFIG_CTRL,
			   AD7606_CONFIG_CTRL_DEFAULT);

	dev->reg_mode = true;
#endif

	return 0;
}

/***************************************************************************//**
 * @brief Read register using AXI core via Parallel interface
 *
 * This will perform a write of the address of the register and then read
 * the value of the register.
 *
 * @param dev        - The device structure.
 * @param reg        - Register address
 * @param val        - Register value
 *
*******************************************************************************/
static int ad7606_parallel_mode_read_reg(struct ad7606_dev *dev,
		uint8_t reg_addr,
		uint8_t *reg_data)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;
	uint32_t wr = AD7606_PARALLEL_RD_FLAG_MSK(reg_addr) << 8;
	uint32_t rd = 0;

	/* Some waits are required, such that the core can talk to the ADC */

	no_os_axi_io_write(axi->core_baseaddr, AD7606_CONFIG_WR, wr);
	usleep(1);
	no_os_axi_io_write(axi->core_baseaddr, AD7606_CONFIG_CTRL,
			   AD7606_CONFIG_CTRL_WRITE_OP);
	usleep(1);
	no_os_axi_io_write(axi->core_baseaddr, AD7606_CONFIG_CTRL,
			   AD7606_CONFIG_CTRL_DEFAULT);

	usleep(1);
	no_os_axi_io_write(axi->core_baseaddr, AD7606_CONFIG_CTRL,
			   AD7606_CONFIG_CTRL_READ_OP);
	usleep(1);
	no_os_axi_io_read(axi->core_baseaddr, AD7606_CONFIG_RD, &rd);

	usleep(1);
	no_os_axi_io_write(axi->core_baseaddr, AD7606_CONFIG_CTRL,
			   AD7606_CONFIG_CTRL_DEFAULT);

	if (reg_data)
		*reg_data = rd;

	dev->reg_mode = true;
#endif

	return 0;
}

/***************************************************************************//**
 * @brief Read a device register via SPI.
 *
 * This function performs CRC8 computation and checking if enabled in the device.
 *
 * @param dev        - The device structure.
 * @param reg_addr   - Register address in device memory.
 * @param reg_data   - Pointer to the location where to store the register value.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ENOTSUP - Device not in software mode.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_spi_reg_read(struct ad7606_dev *dev, uint8_t reg_addr,
			    uint8_t *reg_data)
{
	uint8_t buf[3];
	uint8_t crc;
	uint32_t sz = 2;
	int32_t ret;

	if (!dev->sw_mode)
		return -ENOTSUP;

	buf[0] = AD7606_SERIAL_RD_FLAG_MSK(reg_addr);
	buf[1] = 0x00;
	if (dev->digital_diag_enable.int_crc_err_en) {
		crc = no_os_crc8(ad7606_crc8, buf, 2, 0);
		buf[2] = crc;
		sz += 1;
	}
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sz);
	if (ret < 0)
		return ret;

	dev->reg_mode = true;

	buf[0] = AD7606_SERIAL_RD_FLAG_MSK(reg_addr);
	buf[1] = 0x00;
	if (dev->digital_diag_enable.int_crc_err_en) {
		crc = no_os_crc8(ad7606_crc8, buf, 2, 0);
		buf[2] = crc;
	}
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sz);
	if (ret < 0)
		return ret;

	if (dev->digital_diag_enable.int_crc_err_en) {
		crc = no_os_crc8(ad7606_crc8, buf, 2, 0);
		if (crc != buf[2])
			return -EBADMSG;
	}

	if (reg_data)
		*reg_data = buf[1];

	return ret;
}

/***************************************************************************//**
 * @brief Write a device register via SPI.
 *
 * This function performs CRC8 computation and checking if enabled in the device.
 *
 * @param dev        - The device structure.
 * @param reg_addr   - Register address in device memory.
 * @param reg_data   - Value to write to register.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ENOTSUP - Device not in software mode.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_spi_reg_write(struct ad7606_dev *dev, uint8_t reg_addr,
			     uint8_t reg_data)
{
	uint8_t buf[3];
	int32_t ret;
	uint8_t crc;
	uint32_t sz = 2;

	if (!dev->sw_mode)
		return -ENOTSUP;

	/* Dummy read to place the chip in register mode. */
	if (!dev->reg_mode) {
		ret = ad7606_spi_reg_read(dev, reg_addr, NULL);
		if (ret < 0)
			return ret;
	}

	buf[0] = AD7606_SERIAL_WR_FLAG_MSK(reg_addr);
	buf[1] = reg_data;
	if (dev->digital_diag_enable.int_crc_err_en) {
		crc = no_os_crc8(ad7606_crc8, buf, 2, 0);
		buf[2] = crc;
		sz += 1;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sz);
	if (ret < 0)
		return ret;

	return ret;
}

/***************************************************************************//**
 * @brief Write a device register via SPI or AXI Parallel core.
 *
 * The behavior of this function varies slightly depending on whether
 * the operation is done via SPI or AXI core.
 *
 * @param dev        - The device structure.
 * @param reg_addr   - Register address in device memory.
 * @param reg_data   - Value to write to register.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ENOTSUP - Device not in software mode.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_reg_read(struct ad7606_dev *dev,
			uint8_t reg_addr,
			uint8_t *reg_data)
{
	if (dev->parallel_interface)
		return ad7606_parallel_mode_read_reg(dev, reg_addr, reg_data);

	return ad7606_spi_reg_read(dev, reg_addr, reg_data);
}

/***************************************************************************//**
 * @brief Write a device register via SPI or AXI Parallel core.
 *
 * The behavior of this function varies slightly depending on whether
 * the operation is done via SPI or AXI core.
 *
 * @param dev        - The device structure.
 * @param reg_addr   - Register address in device memory.
 * @param reg_data   - Value to write to register.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ENOTSUP - Device not in software mode.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_reg_write(struct ad7606_dev *dev,
			 uint8_t reg_addr,
			 uint8_t reg_data)
{
	if (dev->parallel_interface)
		return ad7606_parallel_mode_write_reg(dev, reg_addr, reg_data);

	return ad7606_spi_reg_write(dev, reg_addr, reg_data);
}

/***************************************************************************//**
 * @brief Write a device register via SPI with masking.
 *
 * @param dev        - The device structure.
 * @param addr       - Register address in device memory.
 * @param mask       - Only bits set to 1 in mask will be modified.
 * @param val        - Value to write to register.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ENOTSUP - Device not in software mode.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_reg_write_mask(struct ad7606_dev *dev,
			      uint32_t addr,
			      uint32_t mask,
			      uint32_t val)
{
	uint8_t reg_data;
	int ret;

	ret = ad7606_reg_read(dev, addr, &reg_data);
	if (ret < 0)
		return ret;

	reg_data &= ~mask;
	reg_data |= val;

	return ad7606_reg_write(dev, addr, reg_data);
}

/* Internal function to copy the content of a buffer in 18-bit chunks to a 32-bit buffer by
 * extending the chunks to 32-bit size. */
static int32_t cpy18b32b(uint8_t *psrc, uint32_t srcsz, uint32_t *pdst)
{
	unsigned int i, j;

	if (srcsz % 9)
		return -EINVAL;

	for (i = 0; i < srcsz; i += 9) {
		j = 4 * (i / 9);
		pdst[j + 0] = ((uint32_t)(psrc[i + 0] & 0xff) << 10) | ((
					uint32_t)psrc[i + 1] << 2)
			      | ((uint32_t)psrc[i + 2] >> 6);
		pdst[j + 1] = ((uint32_t)(psrc[i + 2] & 0x3f) << 12) | ((
					uint32_t)psrc[i + 3] << 4)
			      | ((uint32_t)psrc[i + 4] >> 4);
		pdst[j + 2] = ((uint32_t)(psrc[i + 4] & 0x0f) << 14) | ((
					uint32_t)psrc[i + 5] << 6)
			      | ((uint32_t)psrc[i + 6] >> 2);
		pdst[j + 3] = ((uint32_t)(psrc[i + 6] & 0x03) << 16) | ((
					uint32_t)psrc[i + 7] << 8)
			      | ((uint32_t)psrc[i + 8] >> 0);
	}
	return 0;
}

/* Internal function to copy the content of a buffer in 26-bit chunks to a 32-bit buffer by
 * extending the chunks to 32-bit size. */
static int32_t cpy26b32b(uint8_t *psrc, uint32_t srcsz, uint32_t *pdst)
{
	unsigned int i, j;

	if (srcsz % 13)
		return -EINVAL;

	for (i = 0; i < srcsz; i += 13) {
		j = 4 * (i / 13);
		pdst[j + 0] = ((uint32_t)(psrc[i + 0] & 0xff) << 18) | ((
					uint32_t)psrc[i + 1] << 10)
			      | ((uint32_t)psrc[i + 2] << 2) | ((uint32_t)psrc[i + 3] >> 6);
		pdst[j + 1] = ((uint32_t)(psrc[i + 3] & 0x3f) << 20) | ((
					uint32_t)psrc[i + 4] << 12)
			      | ((uint32_t)psrc[i + 5] << 4) | ((uint32_t)psrc[i + 6] >> 4);
		pdst[j + 2] = ((uint32_t)(psrc[i + 6] & 0x0f) << 22) | ((
					uint32_t)psrc[i + 7] << 14)
			      | ((uint32_t)psrc[i + 8] << 6) | ((uint32_t)psrc[i + 9] >> 2);
		pdst[j + 3] = ((uint32_t)(psrc[i + 9] & 0x03) << 24) | ((
					uint32_t)psrc[i + 10] << 16)
			      | ((uint32_t)psrc[i + 11] << 8) | ((uint32_t)psrc[i + 12] >> 0);
	}
	return 0;
}

/***************************************************************************//**
 * @brief Toggle the CONVST pin to start a conversion.
 *
 * If needed, this function also puts the device in ADC reading mode by a write
 * at address zero.
 *
 * @param dev        - The device structure.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -EIO - CONVST GPIO not available.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_convst(struct ad7606_dev *dev)
{
	int32_t ret;

	if (dev->reg_mode) {
		/* Enter ADC reading mode by writing at address zero. */
		ret = ad7606_reg_write(dev, 0, 0);
		if (ret < 0)
			return ret;

		dev->reg_mode = false;
	}

	ret = no_os_gpio_set_value(dev->gpio_convst, 0);
	if (ret < 0)
		return ret;

	/* wait LP_CNV time */
	no_os_udelay(1);

	return no_os_gpio_set_value(dev->gpio_convst, 1);
}

/***************************************************************************//**
 * @brief Read conversion data.
 *
 * This function performs CRC16 computation and checking if enabled in the device.
 * If the status is enabled in device settings, each sample of data will contain
 * status information in the lowest 8 bits.
 *
 * The output buffer provided by the user should be as wide as to be able to
 * contain 1 sample from each channel since this function reads conversion data
 * across all channels.
 *
 * @param dev        - The device structure.
 * @param data       - Pointer to location of buffer where to store the data.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -EBADMSG - CRC computation mismatch.
 *                  -ENOTSUP - Device bits per sample not supported.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_spi_data_read(struct ad7606_dev *dev, uint32_t *data)
{
	uint32_t sz;
	int32_t ret, i;
	uint16_t crc, icrc;
	uint8_t bits = ad7606_chip_info_tbl[dev->device_id].bits;
	uint8_t sbits = dev->config.status_header ? 8 : 0;
	uint8_t nchannels = ad7606_chip_info_tbl[dev->device_id].num_channels;

	sz = nchannels * (bits + sbits);

	/* Number of bits to read, corresponds to SCLK cycles in transfer.
	 * This should always be a multiple of 8 to work with most SPI's.
	 * With this chip family this holds true because we either:
	 *  - multiply 8 channels * bits per sample
	 *  - multiply 4 channels * bits per sample (always multiple of 2)
	 * Therefore, due to design reasons, we don't check for the
	 * remainder of this division because it is zero by design.
	 */
	sz /= 8;

	if (dev->digital_diag_enable.int_crc_err_en) {
		sz += 2;
	}

	memset(dev->data, 0, sz);
	ret = no_os_spi_write_and_read(dev->spi_desc, dev->data, sz);
	if (ret < 0)
		return ret;

	if (dev->digital_diag_enable.int_crc_err_en) {
		sz -= 2;
		crc = no_os_crc16(ad7606_crc16, dev->data, sz, 0);
		icrc = ((uint16_t)dev->data[sz] << 8) |
		       dev->data[sz + 1];
		if (icrc != crc)
			return -EBADMSG;
	}

	switch (bits) {
	case 18:
		if (dev->config.status_header)
			ret = cpy26b32b(dev->data, sz, data);
		else
			ret = cpy18b32b(dev->data, sz, data);
		if (ret < 0)
			return ret;
		break;
	case 16:
		for (i = 0; i < nchannels; i++) {
			if (dev->config.status_header) {
				data[i] = (uint32_t)dev->data[i * 3] << 16;
				data[i] |= (uint32_t)dev->data[i * 3 + 1] << 8;
				data[i] |= (uint32_t)dev->data[i * 3 + 2];
			} else {
				data[i] = (uint32_t)dev->data[i * 2] << 8;
				data[i] |= (uint32_t)dev->data[i * 2 + 1];
			}
		}
		break;
	default:
		ret = -ENOTSUP;
		break;
	};

	return ret;
}

/***************************************************************************//**
 * @brief Prepares the SPI Engine offload and enables the PWM.
 *
 * @param dev        - The device structure.
 *
 * @return 0 on success, or negative error code.
*******************************************************************************/
static int32_t ad7606_parallel_capture_pre_enable(struct ad7606_dev *dev)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;
	struct axi_dmac_init dmac_init;
	int32_t i, ret;

	dmac_init.name = "ADC DMAC";
	dmac_init.base = axi->rx_dma_baseaddr;
	dmac_init.irq_option = IRQ_DISABLED;

	ret = axi_dmac_init(&axi->dmac, &dmac_init);
	if (ret)
		return ret;

	/* Enable channels */
	for (i = 0; i < dev->num_channels; i++) {
		no_os_axi_io_write(axi->core_baseaddr, AD7606_CHAN_CTRL(i),
				   AD7606_CHAN_CTRL_ENABLE);
	}

	return no_os_pwm_enable(axi->trigger_pwm_desc);
#endif
}

/***************************************************************************//**
 * @brief Disables PWM and the SPI engine core
 *
 * @param dev        - The device structure.
*******************************************************************************/
static void ad7606_parallel_capture_post_disable(struct ad7606_dev *dev)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;
	uint32_t i;

	for (i = 0; i < dev->num_channels; i++) {
		no_os_axi_io_write(axi->core_baseaddr, AD7606_CHAN_CTRL(i),
				   AD7606_CHAN_CTRL_DISABLE);
	}

	axi_dmac_remove(axi->dmac);
	no_os_pwm_disable(axi->trigger_pwm_desc);
	axi->dmac = NULL;
#endif
}

/***************************************************************************//**
 * @brief Read multiple samples using the AXI core via Parallel interface
 *
 * @param dev        - The device structure.
 * @param data       - Pointer to location of buffer where to store the data.
 * @param samples    - Number of samples to read
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ETIME - Timeout while waiting for the BUSY signal.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
static int32_t ad7606_read_raw_data_parallel(struct ad7606_dev *dev,
		uint32_t *buf, uint32_t samples)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;
	struct axi_dma_transfer transfer = {
		// Number of bytes to writen/read
		.size = samples * 8,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode (CYCLIC?)
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)buf
	};
	int32_t ret;

	ret = axi_dmac_transfer_start(axi->dmac, &transfer);
	if (ret)
		return ret;

	/* Wait until transfer finishes */
	ret = axi_dmac_transfer_wait_completion(axi->dmac, 500);
	if (ret)
		return ret;

	if (axi->dcache_invalidate_range)
		axi->dcache_invalidate_range(transfer.dest_addr, samples * sizeof(uint32_t));

	return 0;
#endif
}

/***************************************************************************//**
 * @brief Prepares the SPI Engine and enables the PWM.
 *
 * @param dev        - The device structure.
 *
 * @return 0 on success, or negative error code.
*******************************************************************************/
static int32_t ad7606_spi_engine_capture_pre_enable(struct ad7606_dev *dev)
{
#ifdef XILINX_PLATFORM
	const uint8_t bits = ad7606_chip_info_tbl[dev->device_id].bits;
	struct ad7606_axi_dev *axi = &dev->axi_dev;

	dev->spi_desc->mode = NO_OS_SPI_MODE_2;
	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);
	spi_engine_set_transfer_width(dev->spi_desc, bits);

	return no_os_pwm_enable(axi->trigger_pwm_desc);
#endif
}

/***************************************************************************//**
 * @brief Disables PWM
 *
 * @param dev        - The device structure.
*******************************************************************************/
static void ad7606_spi_engine_capture_post_disable(struct ad7606_dev *dev)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;

	no_os_pwm_disable(axi->trigger_pwm_desc);
#endif
}

/***************************************************************************//**
 * @brief Read multiple samples using the AXI SPI engine code
 *
 * @param dev        - The device structure.
 * @param data       - Pointer to location of buffer where to store the data.
 * @param samples    - Number of samples to read
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ETIME - Timeout while waiting for the BUSY signal.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
static int32_t ad7606_read_raw_data_spi_engine(struct ad7606_dev *dev,
		uint32_t *buf, uint32_t samples)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;
	int32_t ret;
	uint32_t commands_data[2] = {0x00, 0x00};
	struct spi_engine_offload_message msg = {};
	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		READ(2),
		CS_HIGH,
	};

	ret = spi_engine_offload_init(dev->spi_desc, &axi->offload_init_param);
	if (ret)
		return ret;

	msg.commands_data = commands_data;
	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	msg.rx_addr = (uint32_t)buf;

	ret = spi_engine_offload_transfer(dev->spi_desc, msg, samples);
	if (ret)
		goto error;

	if (axi->dcache_invalidate_range)
		axi->dcache_invalidate_range(msg.rx_addr, samples * sizeof(uint32_t));

error:
	return ret;
#endif
}

/***************************************************************************//**
 * @brief Blocking conversion start and read data (for a single sample from all
 *        channels).
 *
 * This function performs a conversion start and then proceeds to reading
 * the conversion data.
 *
 * @param dev        - The device structure.
 * @param data       - Pointer to location of buffer where to store the data.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ETIME - Timeout while waiting for the BUSY signal.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_read_one_sample(struct ad7606_dev *dev, uint32_t * data)
{
	int32_t ret;
	uint8_t busy;
	uint32_t timeout = tconv_max[AD7606_OSR_256];

	ret = ad7606_convst(dev);
	if (ret < 0)
		return ret;

	if (dev->gpio_busy) {
		/* Wait for BUSY falling edge */
		while (timeout) {
			ret = no_os_gpio_get_value(dev->gpio_busy, &busy);
			if (ret < 0)
				return ret;

			if (busy == 0)
				break;

			no_os_udelay(1);
			timeout--;
		}

		if (timeout == 0)
			return -ETIME;
	} else {
		/* wait CONV time */
		no_os_udelay(tconv_max[dev->oversampling.os_ratio]);
	}

	return ad7606_spi_data_read(dev, data);
}

/***************************************************************************//**
 * @brief Prepares buffer capture for an AXI SPI Engine or AXI Parallel interface
 *
 * @param dev        - The device structure.
 *
 * @return 0 on success, or negative error code.
*******************************************************************************/
int32_t ad7606_capture_pre_enable(struct ad7606_dev *dev)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;

	if (!axi->initialized)
		return 0;

	if (dev->parallel_interface)
		return ad7606_parallel_capture_pre_enable(dev);

	return ad7606_spi_engine_capture_pre_enable(dev);
#endif
}

/***************************************************************************//**
 * @brief Disables buffer capture for an AXI SPI Engine or AXI Parallel interface
 *
 * @param dev        - The device structure.
*******************************************************************************/
void ad7606_capture_post_disable(struct ad7606_dev *dev)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;

	if (!axi->initialized)
		return;

	if (dev->parallel_interface)
		return ad7606_parallel_capture_post_disable(dev);

	return ad7606_spi_engine_capture_post_disable(dev);
#endif
}

/***************************************************************************//**
 * @brief Read muliple raw samples from device.
 *
 * This function performs a series of conversion starts and then proceeds to
 * reading the conversion data (after each conversion).
 *
 * @param dev        - The device structure.
 * @param data       - Pointer to location of buffer where to store the data.
 * @param samples    - Number of samples to read
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ETIME - Timeout while waiting for the BUSY signal.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_read_samples(struct ad7606_dev *dev, uint32_t * data,
			    uint32_t samples)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;
	uint32_t nchannels, i, sample_size;
	int32_t ret;

	if (dev->reg_mode) {
		/* Enter ADC reading mode by writing at address zero. */
		ret = ad7606_reg_write(dev, 0, 0);
		if (ret < 0)
			return ret;

		dev->reg_mode = false;
	}

	if (axi->initialized) {
		if (dev->parallel_interface)
			return ad7606_read_raw_data_parallel(dev, data, samples);
		return ad7606_read_raw_data_spi_engine(dev, data, samples);
	}

	nchannels = ad7606_chip_info_tbl[dev->device_id].num_channels;
	sample_size = nchannels * sizeof(uint32_t);

	for (i = 0; i < samples; i++) {
		ret = ad7606_read_one_sample(dev, data);
		if (ret)
			return ret;
		data += sample_size;
	}
#endif

	return 0;
}

/* Internal function to reset device settings to default state after chip reset. */
static inline void ad7606_reset_settings(struct ad7606_dev *dev)
{
	int i;
	const struct ad7606_range *rt = dev->sw_mode ?
						ad7606_chip_info_tbl[dev->device_id].sw_range_table :
						ad7606_chip_info_tbl[dev->device_id].hw_range_table;

	for (i = 0; i < dev->num_channels; i++) {
		if (dev->sw_mode)
			ad7606_set_ch_range(dev, i, rt[3]);
		else
			ad7606_set_ch_range(dev, i, rt[0]);

		dev->offset_ch[i] = 0;
		dev->phase_ch[i] = 0;
		dev->gain_ch[i] = 0;
	}

	dev->oversampling.os_ratio = AD7606_OSR_1;
	dev->oversampling.os_pad = 0;
	dev->config.op_mode = AD7606_NORMAL;
	dev->config.dout_format = AD7606_2_DOUT;
	dev->config.ext_os_clock = false;
	dev->config.status_header = false;
	dev->digital_diag_enable.rom_crc_err_en = true;
	dev->digital_diag_enable.mm_crc_err_en = false;
	dev->digital_diag_enable.int_crc_err_en = false;
	dev->digital_diag_enable.spi_write_err_en = false;
	dev->digital_diag_enable.spi_read_err_en = false;
	dev->digital_diag_enable.busy_stuck_high_err_en = false;
	dev->digital_diag_enable.clk_fs_os_counter_en = false;
	dev->digital_diag_enable.interface_check_en = false;
	dev->reg_mode = false;
}

/***************************************************************************//**
 * @brief Reset the device by toggling the reset GPIO.
 *
 * @param dev        - The device structure.
 *
 * @return ret - return code.
 *         Example: -EIO - Reset GPIO not available.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_reset(struct ad7606_dev *dev)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi = &dev->axi_dev;
#endif
	int32_t ret;

	ret = no_os_gpio_set_value(dev->gpio_reset, 1);
	if (ret < 0)
		return ret;

	no_os_udelay(3);

	ret = no_os_gpio_set_value(dev->gpio_reset, 0);
	if (ret < 0)
		return ret;

	ad7606_reset_settings(dev);

#ifdef XILINX_PLATFORM
	/* Enable core in parallel mode, to be able to read/write registers */
	if (dev->parallel_interface)
		no_os_axi_io_write(axi->core_baseaddr, AD7606_CORE_RESET,
				   AD7606_PARALLEL_CORE_ENABLE);
#endif

	return ret;
}

/* Internal function that initializes GPIOs. */
static int32_t ad7606_request_gpios(struct ad7606_dev *dev,
				    struct ad7606_init_param *init_param)
{
	int32_t ret;

	ret = no_os_gpio_get_optional(&dev->gpio_reset, init_param->gpio_reset);
	if (ret < 0)
		return ret;

	if (dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_LOW);
		if (ret < 0)
			return ret;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_convst, init_param->gpio_convst);
	if (ret < 0)
		return ret;

	if (dev->gpio_convst) {
		ret = no_os_gpio_direction_output(dev->gpio_convst, NO_OS_GPIO_LOW);
		if (ret < 0)
			return ret;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_busy, init_param->gpio_busy);
	if (ret < 0)
		return ret;

	if (dev->gpio_busy) {
		ret = no_os_gpio_direction_input(dev->gpio_busy);
		if (ret < 0)
			return ret;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_stby_n, init_param->gpio_stby_n);
	if (ret < 0)
		return ret;

	if (dev->gpio_stby_n) {
		ret = no_os_gpio_direction_output(dev->gpio_stby_n, NO_OS_GPIO_HIGH);
		if (ret < 0)
			return ret;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_range, init_param->gpio_range);
	if (ret < 0)
		return ret;

	if (dev->gpio_range) {
		ret = no_os_gpio_direction_output(dev->gpio_range, NO_OS_GPIO_LOW);
		if (ret < 0)
			return ret;
	}

	if (!ad7606_chip_info_tbl[dev->device_id].has_oversampling)
		return ret;

	ret = no_os_gpio_get_optional(&dev->gpio_os0, init_param->gpio_os0);
	if (ret < 0)
		return ret;

	if (dev->gpio_os0) {
		ret = no_os_gpio_direction_output(dev->gpio_os0, NO_OS_GPIO_LOW);
		if (ret < 0)
			return ret;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_os1, init_param->gpio_os1);
	if (ret < 0)
		return ret;

	if (dev->gpio_os1) {
		ret = no_os_gpio_direction_output(dev->gpio_os1, NO_OS_GPIO_LOW);
		if (ret < 0)
			return ret;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_os2, init_param->gpio_os2);
	if (ret < 0)
		return ret;

	if (dev->gpio_os2) {
		ret = no_os_gpio_direction_output(dev->gpio_os2, NO_OS_GPIO_LOW);
		if (ret < 0)
			return ret;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_par_ser, init_param->gpio_par_ser);
	if (ret < 0)
		return ret;

	if (dev->gpio_par_ser) {
		uint8_t pin;

		if (dev->parallel_interface)
			pin = NO_OS_GPIO_LOW;
		else
			pin = NO_OS_GPIO_HIGH;

		/* Driver currently supports only serial interface, therefore,
		 * if available, pull the GPIO HIGH. */
		ret = no_os_gpio_direction_output(dev->gpio_par_ser, pin);
		if (ret < 0)
			return ret;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Set the oversampling ratio.
 *
 * In hardware mode, it silently sets AD7606_OSR_64 if higher oversampling
 * is provided.
 *
 * @param dev          - The device structure.
 * @param oversampling - Oversampling settings.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_set_oversampling(struct ad7606_dev *dev,
				struct ad7606_oversampling oversampling)
{
	int32_t ret;
	uint8_t val;

	if (dev->sw_mode) {
		val = no_os_field_prep(AD7606_OS_RATIO_MSK, oversampling.os_ratio);
		val |= no_os_field_prep(AD7606_OS_PAD_MSK, oversampling.os_pad);
		ret = ad7606_reg_write(dev, AD7606_REG_OVERSAMPLING, val);
		if (ret < 0)
			return ret;
	} else {
		/* In hardware mode, OSR 128 and 256 are not avaialable */
		if (oversampling.os_ratio > AD7606_OSR_64)
			oversampling.os_ratio = AD7606_OSR_64;

		ret = no_os_gpio_set_value(dev->gpio_os0,
					   ((oversampling.os_ratio & 0x01) >> 0));
		if (ret < 0)
			return ret;

		ret = no_os_gpio_set_value(dev->gpio_os1,
					   ((oversampling.os_ratio & 0x02) >> 1));
		if (ret < 0)
			return ret;

		ret = no_os_gpio_set_value(dev->gpio_os2,
					   ((oversampling.os_ratio & 0x04) >> 2));
		if (ret < 0)
			return ret;
	}

	dev->oversampling = oversampling;

	return 0;
}

/***************************************************************************//**
 * @brief Get the oversampling ratio.
 *
 * Return the current oversampling ratio.
 *
 * @param dev          - The device structure.
 * @param oversampling - Pointer to an object where to store oversampling settings.
 *
 * @return ret - return code.
 *         Example: -EINVAL is 'dev' or 'oversampling' are NULL.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_get_oversampling(struct ad7606_dev *dev,
				struct ad7606_oversampling *oversampling)
{
	if (!dev || !oversampling)
		return -EINVAL;

	*oversampling = dev->oversampling;

	return 0;
}

/***************************************************************************//**
 * @brief Get the available channel ranges for the given channel
 *
 * @param dev          - The device structure.
 * @param ch           - The channel number
 * @param num_ranges   - A pointer to store the number of returned ranges.
 *
 * @return A pointer to the ranges of the devices, or NULL if 'dev' or
 *         'num_ranges' is NULL.
*******************************************************************************/
const struct ad7606_range *ad7606_get_ch_ranges(struct ad7606_dev *dev,
		uint8_t ch,
		uint32_t *num_ranges)
{
	const struct ad7606_chip_info *info;
	const struct ad7606_range *rt;
	uint32_t i;

	if (!dev || !num_ranges)
		return NULL;

	info = &ad7606_chip_info_tbl[dev->device_id];

	if (!dev->sw_mode) {
		*num_ranges = info->hw_range_table_sz;
		return info->hw_range_table;
	}

	/* For SW mode, we need to select the ranges; luckily they are contiguous */
	*num_ranges = 0;
	rt = NULL;
	for (i = 0; i < info->sw_range_table_sz; i++) {
		const struct ad7606_range *e = &info->sw_range_table[i];
		if (dev->range_ch_type[ch] != e->type)
			continue;
		if (!rt)
			rt = e;
		(*num_ranges)++;
	}

	return rt;
}

/* Internal function to find the index of a given operation range in the
 * operation range table specific to a device. */
static int8_t ad7606_find_range(struct ad7606_dev *dev,
				struct ad7606_range range)
{
	uint8_t i;
	int8_t v = -1;
	const struct ad7606_range *rt = dev->sw_mode ?
						ad7606_chip_info_tbl[dev->device_id].sw_range_table :
						ad7606_chip_info_tbl[dev->device_id].hw_range_table;

	uint32_t rtsz = dev->sw_mode ?
			ad7606_chip_info_tbl[dev->device_id].sw_range_table_sz :
			ad7606_chip_info_tbl[dev->device_id].hw_range_table_sz;

	for (i = 0; i < rtsz; i++) {
		if (range.min != rt[i].min)
			continue;
		if (range.max != rt[i].max)
			continue;
		if (range.type != rt[i].type)
			continue;
		v = i;
		break;
	}

	return v;
}

/***************************************************************************//**
 * @brief Returns true if SW mode is enabled.
 *
 * @param dev        - The device structure.
 *
 * @return true if software mode is enabled, false otherwise.
*******************************************************************************/
bool ad7606_sw_mode_enabled(struct ad7606_dev *dev)
{
	if (!dev)
		return false;

	return dev->sw_mode;
}

/***************************************************************************//**
 * @brief Set the channel operation range.
 *
 * @param dev        - The device structure.
 * @param ch         - Channel number (0-7).
 * @param range      - Operation range.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -EINVAL - Invalid input.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_set_ch_range(struct ad7606_dev *dev, uint8_t ch,
			    struct ad7606_range range)
{
	const struct ad7606_chip_info *info;
	int value;
	int32_t ret;

	if (range.min > range.max)
		return -EINVAL;

	if (ch >= dev->num_channels)
		return -EINVAL;

	value = ad7606_find_range(dev, range);
	if (value < 0)
		return -EINVAL;

	if (dev->sw_mode)
		ret = ad7606_reg_write_mask(dev, AD7606_REG_RANGE_CH_ADDR(ch),
					    AD7606_RANGE_CH_MSK(ch),
					    AD7606_RANGE_CH_MODE(ch, value));
	else
		ret = no_os_gpio_set_value(dev->gpio_range, value);

	if (ret)
		return ret;

	info = &ad7606_chip_info_tbl[dev->device_id];

	dev->range_ch_type[ch] = range.type;
	dev->scale_ch[ch] = (double)(range.max - range.min) / (double)(1 << info->bits);

	return ret;
}

/***************************************************************************//**
 * @brief Get the value of scale for the channel
 *
 * @param dev        - The device structure.
 * @param ch         - The channel number.
 * @param scale      - Pointer where to store the channel scale.
 *
 * @return ret - return code.
 *         Example: -EINVAL - Invalid input.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_get_ch_scale(struct ad7606_dev *dev, uint8_t ch, double *scale)
{
	if (!scale)
		return -EINVAL;

	if (ch >= dev->num_channels)
		return -EINVAL;

	*scale = dev->scale_ch[ch];

	return 0;
}

/***************************************************************************//**
 * @brief Get the resolution bits of this device.
 *
 * @param dev        - The device structure.
 *
 * @return ret - return code.
 *         Example: -EINVAL - Invalid input.
 *                  The number of resolution bits for this device.
*******************************************************************************/
int32_t ad7606_get_resolution_bits(struct ad7606_dev *dev)
{
	const struct ad7606_chip_info *info;

	info = &ad7606_chip_info_tbl[dev->device_id];

	return info->bits;
}

/***************************************************************************//**
 * @brief Set the channel offset.
 *
 * The offset parameter is a signed 8-bit integer ranging from -128 to 127 to
 * make it intuitive and user-friendly.
 *
 * This offset gets converted to the register representation where 0x80 is
 * calibration offset 0, 0x0 is calibration offset -128 and 0xFF is calibration
 * offset 127, etc.
 *
 * @param dev        - The device structure.
 * @param ch         - Channel number (0-7).
 * @param offset     - Offset calibration amount (-128...127).
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -EINVAL - Invalid input.
 *                  -ENOTSUP - Device not in software mode.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_set_ch_offset(struct ad7606_dev *dev, uint8_t ch,
			     int8_t offset)
{
	int ret;
	uint8_t value = (uint8_t)(offset - 0x80);

	if (ch >= dev->num_channels)
		return -EINVAL;

	if (!dev->sw_mode)
		return -ENOTSUP;

	ret = ad7606_reg_write(dev, AD7606_REG_OFFSET_CH(ch), value);
	if (ret < 0)
		return ret;

	dev->offset_ch[ch] = offset;

	return ret;
}

/***************************************************************************//**
 * @brief Set the channel phase.
 *
 * @param dev        - The device structure.
 * @param ch         - Channel number (0-7).
 * @param phase      - Phase calibration amount.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -EINVAL - Invalid input.
 *                  -ENOTSUP - Device not in software mode.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_set_ch_phase(struct ad7606_dev *dev, uint8_t ch,
			    uint8_t phase)
{
	int ret;

	if (ch >= dev->num_channels)
		return -EINVAL;

	if (!dev->sw_mode)
		return -ENOTSUP;

	ret = ad7606_reg_write(dev, AD7606_REG_PHASE_CH(ch), phase);
	if (ret < 0)
		return ret;

	dev->phase_ch[ch] = phase;

	return ret;
}

/***************************************************************************//**
 * @brief Set the channel gain.
 *
 * @param dev        - The device structure.
 * @param ch         - Channel number (0-7).
 * @param gain       - Gain calibration amount.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -EINVAL - Invalid input.
 *                  -ENOTSUP - Device not in software mode.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_set_ch_gain(struct ad7606_dev *dev, uint8_t ch,
			   uint8_t gain)
{
	int ret;

	if (ch >= dev->num_channels)
		return -EINVAL;

	if (!dev->sw_mode)
		return -ENOTSUP;

	gain = no_os_field_get(AD7606_GAIN_MSK, gain);
	ret = ad7606_reg_write(dev, AD7606_REG_GAIN_CH(ch), gain);
	if (ret < 0)
		return ret;

	dev->gain_ch[ch] = gain;

	return ret;
}

/***************************************************************************//**
 * @brief Set the device config register.
 *
 * Configuration structure affects the CONFIG register of the device.
 *
 * @param dev        - The device structure.
 * @param config     - Configuration structure.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -EIO - GPIO not available.
 *                  -EINVAL - Invalid input.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_set_config(struct ad7606_dev *dev,
			  struct ad7606_config config)
{
	int32_t ret;
	uint8_t val = 0;
	uint8_t range_pin, stby_n_pin;

	if (dev->sw_mode) {

		val |= no_os_field_prep(AD7606_CONFIG_OPERATION_MODE_MSK, config.op_mode);
		if ((uint8_t)config.dout_format > (uint8_t)dev->max_dout_lines)
			return -EINVAL;
		val |= no_os_field_prep(AD7606_CONFIG_DOUT_FORMAT_MSK, config.dout_format);
		val |= no_os_field_prep(AD7606_CONFIG_EXT_OS_CLOCK_MSK, config.ext_os_clock);
		val |= no_os_field_prep(AD7606_CONFIG_STATUS_HEADER_MSK, config.status_header);

		ret = ad7606_reg_write(dev, AD7606_REG_CONFIG, val);
		if (ret)
			return ret;
	} else {
		switch (config.op_mode) {
		case AD7606_NORMAL:
			range_pin = NO_OS_GPIO_LOW;
			stby_n_pin = NO_OS_GPIO_HIGH;
			break;
		case AD7606_STANDBY:
			range_pin = NO_OS_GPIO_LOW;
			stby_n_pin = NO_OS_GPIO_LOW;
			break;
		case AD7606_SHUTDOWN:
			range_pin = NO_OS_GPIO_HIGH;
			stby_n_pin = NO_OS_GPIO_LOW;
			break;
		default:
			return -EINVAL;
		};

		ret = no_os_gpio_set_value(dev->gpio_stby_n, stby_n_pin);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(dev->gpio_range, range_pin);
		if (ret)
			return ret;
	}

	dev->config = config;

	return ret;
}

/***************************************************************************//**
 * @brief Set the device digital diagnostics configuration.
 *
 * Digital diagnostics structure affects the DIGITAL_DIAG register of the device.
 *
 * @param dev        - The device structure.
 * @param diag       - Configuration structure.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  -ENOTSUP - Device not in software mode.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_set_digital_diag(struct ad7606_dev *dev,
				struct ad7606_digital_diag diag)
{
	int32_t ret;
	uint8_t val = 0;

	if (!dev->sw_mode)
		return -ENOTSUP;

	val |= no_os_field_prep(AD7606_ROM_CRC_ERR_EN_MSK, diag.rom_crc_err_en);
	val |= no_os_field_prep(AD7606_MM_CRC_ERR_EN_MSK, diag.mm_crc_err_en);
	val |= no_os_field_prep(AD7606_INT_CRC_ERR_EN_MSK, diag.int_crc_err_en);
	val |= no_os_field_prep(AD7606_SPI_WRITE_ERR_EN_MSK, diag.spi_write_err_en);
	val |= no_os_field_prep(AD7606_SPI_READ_ERR_EN_MSK, diag.spi_read_err_en);
	val |= no_os_field_prep(AD7606_BUSY_STUCK_HIGH_ERR_EN_MSK,
				diag.busy_stuck_high_err_en);
	val |= no_os_field_prep(AD7606_CLK_FS_OS_COUNTER_EN_MSK,
				diag.clk_fs_os_counter_en);
	val |= no_os_field_prep(AD7606_INTERFACE_CHECK_EN_MSK, diag.interface_check_en);

	ret = ad7606_reg_write(dev, AD7606_REG_DIGITAL_DIAG_ENABLE, val);
	if (ret < 0)
		return ret;

	dev->digital_diag_enable = diag;

	return ret;
}

/***************************************************************************//**
 * @brief Initialize AXI FPGA modules, if configured
 *
 * The device may function in correlation with AXI SPI Engine, AXI ADC,
 * AXI PWM and other FPGA elements.
 * When it's configured (with AXI support) this function will initialize
 * all the AXI elements (provided via init_param).
 *
 * @param device     - Pointer to location of device structure to write.
 * @param init_param - Pointer to configuration of the driver.
 *
 * @return ret - return code.
 *         Example: -ENOMEM - Memory allocation error.
 *                  0 - No errors encountered.
*******************************************************************************/
static int32_t ad7606_axi_init(struct ad7606_dev *device,
			       struct ad7606_init_param *init_param)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_init_param *axi_init = init_param->axi_init;
	struct ad7606_axi_dev *axi = &device->axi_dev;
	int32_t ret;

	axi->initialized = false;

	if (!axi_init)
		return 0;

	if (!device->parallel_interface) {
		ret = axi_clkgen_init(&axi->clkgen, axi_init->clkgen_init);
		if (ret != 0)
			return ret;

		ret = axi_clkgen_set_rate(axi->clkgen, axi_init->axi_clkgen_rate);
		if (ret != 0)
			goto error;
	}

	ret = no_os_pwm_init(&axi->trigger_pwm_desc, axi_init->trigger_pwm_init);
	if (ret != 0)
		goto error;

	/* Make sure the PWM is disabled on start-up */
	no_os_pwm_disable(axi->trigger_pwm_desc);

	memcpy(&axi->offload_init_param, axi_init->offload_init_param,
	       sizeof(axi->offload_init_param));

	axi->core_baseaddr = axi_init->core_baseaddr;
	axi->rx_dma_baseaddr = axi_init->rx_dma_baseaddr;
	axi->reg_access_speed = axi_init->reg_access_speed;
	axi->dcache_invalidate_range = axi_init->dcache_invalidate_range;

	no_os_axi_io_write(axi->core_baseaddr, AD7606_CORE_RESET,
			   AD7606_SERIAL_CORE_ENABLE);
	no_os_axi_io_write(axi->core_baseaddr, AD7606_CORE_RESET,
			   AD7606_SERIAL_CORE_DISABLE);

	axi->initialized = true;

	/* Note: more validation will be added later */
error:
	return ret;
#endif
}

/***************************************************************************//**
 * @brief Initialize the ad7606 device structure.
 *
 * Performs memory allocation of the device structure.
 *
 * @param device     - Pointer to location of device structure to write.
 * @param init_param - Pointer to configuration of the driver.
 *
 * @return ret - return code.
 *         Example: -ENOMEM - Memory allocation error.
 *                  -EIO - SPI communication error.
 *                  -EIO - GPIO initialization error.
 *                  -ENODEV - Unexpected device id.
 *                  -EBADMSG - CRC computation mismatch.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_init(struct ad7606_dev **device,
		    struct ad7606_init_param *init_param)
{
	const struct ad7606_chip_info *info;
	struct ad7606_dev *dev;
	uint8_t reg, id;
	int32_t i, ret;

	no_os_crc8_populate_msb(ad7606_crc8, 0x7);
	no_os_crc16_populate_msb(ad7606_crc16, 0x755b);

	dev = (struct ad7606_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->device_id = init_param->device_id;
	dev->parallel_interface = init_param->parallel_interface;
	info = &ad7606_chip_info_tbl[dev->device_id];
	printf("Initializing device %s, num-channels %u SDI lines %u\n",
	       info->name, info->num_channels, 1 << info->max_dout_lines);

#ifdef XILINX_PLATFORM
	ret = ad7606_axi_init(dev, init_param);
	if (ret != 0)
		goto error;

	if (!dev->axi_dev.initialized && !dev->parallel_interface) {
		ret = -EINVAL;
		printf("Parallel interface requires an AXI Core module\n");
		goto error;
	}
#endif

	dev->num_channels = info->num_channels;
	dev->max_dout_lines = info->max_dout_lines;
	if (info->has_registers)
		dev->sw_mode = init_param->sw_mode;

	if (dev->parallel_interface) {
		printf("Notice: Parallel mode enabled, forcing SW mode\n");
		dev->sw_mode = true;
	}

	ret = ad7606_request_gpios(dev, init_param);
	if (ret < 0)
		goto error;

	if (dev->sw_mode) {
		ret = no_os_gpio_set_value(dev->gpio_os0, NO_OS_GPIO_HIGH);
		if (ret < 0)
			goto error;

		ret = no_os_gpio_set_value(dev->gpio_os1, NO_OS_GPIO_HIGH);
		if (ret < 0)
			goto error;

		ret = no_os_gpio_set_value(dev->gpio_os2, NO_OS_GPIO_HIGH);
		if (ret < 0)
			goto error;
	}

	ret = ad7606_reset(dev);
	if (ret < 0)
		goto error;

	/* Copy init parameters here, ad7606_reset() will clear these */
	memcpy(dev->gain_ch, init_param->gain_ch, sizeof(dev->gain_ch));
	memcpy(dev->phase_ch, init_param->phase_ch, sizeof(dev->phase_ch));
	memcpy(dev->offset_ch, init_param->offset_ch, sizeof(dev->offset_ch));

	/* wait DEVICE_SETUP time */
	no_os_udelay(253);

	if (!dev->parallel_interface) {
		ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
		if (ret < 0)
			goto error;
	}

	if (dev->sw_mode) {
		ret = ad7606_reg_read(dev, AD7606_REG_ID, &reg);
		if (ret < 0)
			goto error;

		id = info->device_id;
		if (no_os_field_get(AD7606_ID_DEVICE_ID_MSK, reg) != id) {
			printf("ad7606: device id mismatch, expected 0x%.2x, got 0x%.2x\n",
			       id,
			       (int)no_os_field_get(AD7606_ID_DEVICE_ID_MSK, reg));
			ret = -ENODEV;
			goto error;
		}

		ret = ad7606_set_digital_diag(dev, init_param->digital_diag_enable);
		if (ret < 0)
			goto error;

		ret = ad7606_set_config(dev, init_param->config);
		if (ret < 0)
			goto error;

		for (i = 0; i < dev->num_channels; i++) {
			ret = ad7606_set_ch_offset(dev, i, init_param->offset_ch[i]);
			if (ret < 0)
				goto error;
		}

		for (i = 0; i < dev->num_channels; i++) {
			ret = ad7606_set_ch_phase(dev, i, init_param->phase_ch[i]);
			if (ret < 0)
				goto error;
		}

		for (i = 0; i < dev->num_channels; i++) {
			ret = ad7606_set_ch_gain(dev, i, init_param->gain_ch[i]);
			if (ret < 0)
				goto error;
		}
	}

	for (i = 0; i < dev->num_channels; i++) {
		struct ad7606_range *rt;

		if (i > 0)
			rt = &init_param->range_ch[i];
		else
			rt = &init_param->range_ch[0];

		ret = ad7606_set_ch_range(dev, i, *rt);
		if (ret < 0)
			goto error;
	}

	ret = no_os_gpio_set_value(dev->gpio_convst, 1);
	if (ret < 0)
		goto error;

	if (info->has_oversampling)
		ad7606_set_oversampling(dev, init_param->oversampling);

	*device = dev;

	printf("ad7606 successfully initialized\n");

	return ret;
error:
	printf("ad7606 initialization failed\n");
	ad7606_remove(dev);
	return ret;
}

/*******************************************************************************
 * @brief correct spi read serial raw data
 *        1. sign extend for hardware/bipolar mode in case of negative value
 *        2. split data and status information into 2 arrays
 *        note: this function should be called after read a sample through spi
 *              serial interface( parallel interface not supported yet)
 *
 * @param dev          - The device structure.
 * @param buf          - pointer to data buffer read by spi.
 * @param data         - pointer to data buffer after correction
 * @param status       - pointer to status information buffer,
 *                       input null for status_header disable
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid status information buffer pointer
 *                  -EINVAL - No valid data buffer pointer
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_data_correction_serial(struct ad7606_dev *dev,
				      uint32_t *buf, int32_t *data, uint8_t *status)
{
	uint8_t i = 0;
	uint8_t num_ch = dev->num_channels;
	uint32_t *pbuf = buf;
	uint8_t bits = ad7606_chip_info_tbl[dev->device_id].bits;

	if (dev->config.status_header) {
		// validate status pointers
		if (!status)
			return EINVAL;

		for (i = 0; i < num_ch; i++) {
			*status = (uint8_t) * pbuf | 0xff;
			*pbuf = *pbuf >> 8;
			pbuf++;
			status++;
		}
	}

	// validate data pointers
	if (!status)
		return EINVAL;

	// correct negative data value
	for (i = 0; i < num_ch; i++) {
		// if negative value exist (hardware/bipolar)
		if (dev->range_ch_type[i] != AD7606_SW_RANGE_SINGLE_ENDED_UNIPOLAR)
			*data = no_os_sign_extend32(*buf, bits - 1);
		buf++;
		data++;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Free any resource used by the driver.
 *
 * @param dev        - The device structure.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  0 - No errors encountered.
*******************************************************************************/
void ad7606_axi_remove(struct ad7606_dev *dev)
{
#ifdef XILINX_PLATFORM
	struct ad7606_axi_dev *axi;

	if (!dev)
		return;

	axi = &dev->axi_dev;

	if (dev->parallel_interface)
		no_os_axi_io_write(axi->core_baseaddr, AD7606_CORE_RESET,
				   AD7606_PARALLEL_CORE_DISABLE);

	no_os_pwm_remove(axi->trigger_pwm_desc);
	if (!dev->parallel_interface)
		axi_clkgen_remove(axi->clkgen);
#endif
}

/***************************************************************************//**
 * @brief Free any resource used by the driver.
 *
 * @param dev        - The device structure.
 *
 * @return ret - return code.
 *         Example: -EIO - SPI communication error.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7606_remove(struct ad7606_dev *dev)
{
	int32_t ret = 0;

	no_os_gpio_remove(dev->gpio_reset);
	no_os_gpio_remove(dev->gpio_convst);
	no_os_gpio_remove(dev->gpio_busy);
	no_os_gpio_remove(dev->gpio_stby_n);
	no_os_gpio_remove(dev->gpio_range);
	no_os_gpio_remove(dev->gpio_os0);
	no_os_gpio_remove(dev->gpio_os1);
	no_os_gpio_remove(dev->gpio_os2);
	no_os_gpio_remove(dev->gpio_par_ser);

	if (!dev->parallel_interface)
		ret = no_os_spi_remove(dev->spi_desc);

#ifdef XILINX_PLATFORM
	ad7606_axi_remove(dev);
#endif

	no_os_free(dev);

	return ret;
}
