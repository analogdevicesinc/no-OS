/***************************************************************************//**
 *   @file   ad7616.c
 *   @brief  Implementation of AD7616 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Esteban Blanc (eblanc@baylibre.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 * Copyright 2023(c) BayLibre, SAS.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/* Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/AD7616.pdf */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "ad7616.h"
#include "no_os_gpio.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

#ifdef XILINX_PLATFORM
#include "spi_engine.h"
#include "axi_dmac.h"
#include "no_os_axi_io.h"
#else
#include "no_os_spi.h"
#endif

#define AD7616_GET_BIT(v, b)		((v >> b) & 1)
#define AD7616_TOGGLE_TIMEOUT_DELAY	(8192 * 1024)
#define SPI_ENGINE_WORDS_PER_READ	1

/**
 * @brief Compute the CRC for one channel. See CRC chapter in the datasheet
 * @param data - Data to check
 * @param crc - Initial crc
 * @return The computed CRC
 */
static uint8_t crc_channel(uint16_t data, uint8_t crc)
{
	uint8_t crc_out;

	uint8_t data_0 = AD7616_GET_BIT(data, 0);
	uint8_t data_1 = AD7616_GET_BIT(data, 1);
	uint8_t data_2 = AD7616_GET_BIT(data, 2);
	uint8_t data_3 = AD7616_GET_BIT(data, 3);
	uint8_t data_4 = AD7616_GET_BIT(data, 4);
	uint8_t data_5 = AD7616_GET_BIT(data, 5);
	uint8_t data_6 = AD7616_GET_BIT(data, 6);
	uint8_t data_7 = AD7616_GET_BIT(data, 7);
	uint8_t data_8 = AD7616_GET_BIT(data, 8);
	uint8_t data_9 = AD7616_GET_BIT(data, 9);
	uint8_t data_10 = AD7616_GET_BIT(data, 10);
	uint8_t data_11 = AD7616_GET_BIT(data, 11);
	uint8_t data_12 = AD7616_GET_BIT(data, 12);
	uint8_t data_13 = AD7616_GET_BIT(data, 13);
	uint8_t data_14 = AD7616_GET_BIT(data, 14);
	uint8_t data_15 = AD7616_GET_BIT(data, 15);

	uint8_t crc_0 = AD7616_GET_BIT(crc, 0);
	uint8_t crc_1 = AD7616_GET_BIT(crc, 1);
	uint8_t crc_2 = AD7616_GET_BIT(crc, 2);
	uint8_t crc_3 = AD7616_GET_BIT(crc, 3);
	uint8_t crc_4 = AD7616_GET_BIT(crc, 4);
	uint8_t crc_5 = AD7616_GET_BIT(crc, 5);
	uint8_t crc_6 = AD7616_GET_BIT(crc, 6);
	uint8_t crc_7 = AD7616_GET_BIT(crc, 7);

	crc_out = data_15 ^ data_13 ^ data_11 ^ data_7 ^ data_6 ^ data_5 ^
		  crc_3 ^ crc_5 ^ crc_7;

	crc_out = (crc_out << 1) | (data_14 ^ data_12 ^ data_10 ^ data_6 ^
				    data_5 ^ data_4 ^ crc_2 ^ crc_4 ^ crc_6);

	crc_out = (crc_out << 1) |
		  (data_15 ^ data_13 ^ data_11 ^ data_9 ^ data_5 ^ data_4 ^
		   data_3 ^ crc_1 ^ crc_3 ^ crc_5 ^ crc_7);

	crc_out = (crc_out << 1) |
		  (data_15 ^ data_14 ^ data_12 ^ data_10 ^ data_8 ^ data_4 ^
		   data_3 ^ data_2 ^ crc_0 ^ crc_2 ^ crc_4 ^ crc_6 ^ crc_7);

	crc_out = (crc_out << 1) |
		  (data_14 ^ data_13 ^ data_11 ^ data_9 ^ data_7 ^ data_3 ^
		   data_2 ^ data_1 ^ crc_1 ^ crc_3 ^ crc_5 ^ crc_6);

	crc_out = (crc_out << 1) | (data_15 ^ data_13 ^ data_12 ^ data_10 ^
				    data_8 ^ data_6 ^ data_2 ^ data_1 ^ data_0 ^
				    crc_0 ^ crc_2 ^ crc_4 ^ crc_5 ^ crc_7);

	crc_out = (crc_out << 1) |
		  (data_15 ^ data_14 ^ data_13 ^ data_12 ^ data_9 ^ data_6 ^
		   data_1 ^ data_0 ^ crc_1 ^ crc_4 ^ crc_5 ^ crc_6 ^ crc_7);

	crc_out = (crc_out << 1) | (data_14 ^ data_12 ^ data_8 ^ data_7 ^
				    data_6 ^ data_0 ^ crc_0 ^ crc_4 ^ crc_6);

	return crc_out;
}

/**
 * @brief Compute the CRC for multiples pairs of channel
 * @param results - Array of conversion result
 * @param samples - Number of conversion result
 * @return The computed CRC
 */
static uint8_t crc_channel_pairs(struct ad7616_conversion_result *results,
				 uint32_t samples)
{
	uint8_t crc = 0;
	uint8_t crc1 = 0;

	for (size_t i = 0; i < samples; i++) {
		crc1 = crc_channel(results[i].channel_a, crc);
		crc = crc_channel(results[i].channel_b, crc1);
	}

	return crc;
}

/**
 * @brief Check if the recived CRC is correct
 * @param results - Array of conversion result
 * @param samples - Number of conversion result
 * @param found_crc - CRC recived from the device
 * @return 0 if the computed CRC is the same as the device, 1 otherwise
 */
static int32_t check_crc(struct ad7616_conversion_result *results,
			 uint32_t samples, uint8_t found_crc)
{
	uint8_t computed_crc = crc_channel_pairs(results, samples);

	return !(computed_crc == found_crc);
}

/**
 * @brief Enable CRC
 * @param dev - The device structure.
 * @return 0 on success, an error code otherwise
 */
static int32_t ad7616_enable_crc(struct ad7616_dev *dev)
{
	int32_t ret = 0;

	ret = ad7616_write_mask(dev, AD7616_REG_CONFIG,
				AD7616_STATUSEN_MASK | AD7616_CRCEN_MASK,
				AD7616_STATUSEN | AD7616_CRCEN);
	if (ret != 0)
		return ret;

	dev->crc = 1;

	return 0;
}

/**
 * @brief Perform a toggle on the CONV GPIO and wait for the BUSY GPIO to go low.
 *        After this, the conversion result are ready for reading
 * @param dev - The device structure.
 * @return 0 on success, an error code otherwise
 */
static int32_t ad7616_toggle_conv(struct ad7616_dev *dev)
{
	int ret;
	uint8_t value;
	uint32_t timeout = AD7616_TOGGLE_TIMEOUT_DELAY;

	ret = no_os_gpio_direction_input(dev->gpio_busy);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_direction_output(dev->gpio_convst, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_direction_output(dev->gpio_convst, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	// Waiting for falling edge
	do {
		ret = no_os_gpio_get_value(dev->gpio_busy, &value);
		if (ret != 0)
			return ret;
		timeout--;
	} while (value != NO_OS_GPIO_LOW && timeout != 0);

	if (timeout == 0)
		return -ETIMEDOUT;

	return 0;
}

/**
 * @brief Reads all conversion results with optional CRC
 * @param dev - The device structure.
 * @param res - Array of conversion results. The caller is responsible for allocating enough space
 * @param crc - Pointer to store the CRC received from the device
 * @return 0 on success, an error code otherwise
 */
static int32_t ad7616_read_channels(struct ad7616_dev *dev,
				    struct ad7616_conversion_result *res,
				    uint8_t *crc)
{
	int32_t ret;
	uint32_t read_nb = dev->layers_nb * 2;
	uint16_t tmp = 0;

	for (uint32_t i = 0; i < read_nb; i++, tmp = 0) {
		ret = no_os_spi_write_and_read(dev->spi_desc, (void *)&tmp,
					       sizeof(tmp));
		if (ret != 0)
			return ret;

		tmp = no_os_get_unaligned_be16((void *)&tmp);

		if (i % 2 == 0)
			res[i / 2].channel_a = tmp;
		else
			res[i / 2].channel_b = tmp;
	}

	// Check CRC
	if (dev->crc) {
		ret = no_os_spi_write_and_read(dev->spi_desc, (void *)&tmp,
					       sizeof(tmp));
		if (ret != 0)
			return ret;

		tmp = no_os_get_unaligned_be16((void *)&tmp);

		*crc = tmp & 0xFF;
	}

	return 0;
}

/**
 * @brief Select a new source for a channel.
 * @param dev - The device structure.
 * @param ch - The new channel source
 * @return 0 on success, an error code otherwise
 */
int32_t ad7616_select_channel_source(struct ad7616_dev *dev, enum ad7616_ch ch)
{
	int ret;
	uint16_t mask = ch >= AD7616_VB0 ?  AD7616_CHB_MASK : AD7616_CHA_MASK;
	uint16_t data = (ch >= AD7616_VB0 ? (ch - AD7616_VB0) << AD7616_CHB_OFFSET :
			 ch);

	ret = ad7616_write_mask(dev, AD7616_REG_CHANNEL, mask, data);
	if (ret != 0)
		return ret;

	// When changing sources, next conversion is not correct.
	return ad7616_toggle_conv(dev);
}

/**
 * @brief Read the current selected channels.
 * @param dev - The device structure
 * @param ch_a - Pointer to store the current source for channel A
 * @param ch_b - Pointer to store the current source for channel B
 * @return 0 on success, an error code otherwise
 */
int32_t ad7616_read_channel_source(struct ad7616_dev *dev, enum ad7616_ch *ch_a,
				   enum ad7616_ch *ch_b)
{
	uint16_t ret;
	uint16_t val;

	ret = ad7616_read_mask(dev, AD7616_REG_CHANNEL, AD7616_CHANNELS_MASK, &val);
	if (ret != 0)
		return ret;

	*ch_a = val & 0xF;
	*ch_b = (val >> 4) + AD7616_VB0;

	return 0;
}

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_read(struct ad7616_dev *dev,
		    uint8_t reg_addr,
		    uint16_t *reg_data)
{
	if (dev->interface == AD7616_SERIAL)
		return ad7616_spi_read(dev, reg_addr, reg_data);
	else
		return ad7616_par_read(dev, reg_addr, reg_data);
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_write(struct ad7616_dev *dev,
		     uint8_t reg_addr,
		     uint16_t reg_data)
{
	if (dev->interface == AD7616_SERIAL)
		return ad7616_spi_write(dev, reg_addr, reg_data);
	else
		return ad7616_par_write(dev, reg_addr, reg_data);
}

/**
 * Read from device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_read_mask(struct ad7616_dev *dev,
			 uint8_t reg_addr,
			 uint16_t mask,
			 uint16_t *data)
{
	uint16_t reg_data;
	int32_t ret;

	if (dev->interface == AD7616_SERIAL)
		ret = ad7616_spi_read(dev, reg_addr, &reg_data);
	else
		ret = ad7616_par_read(dev, reg_addr, &reg_data);
	*data = (reg_data & mask);

	return ret;
}

/**
 * SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_write_mask(struct ad7616_dev *dev,
			  uint8_t reg_addr,
			  uint16_t mask,
			  uint16_t data)
{
	uint16_t reg_data;
	int32_t ret;

	if (dev->interface == AD7616_SERIAL)
		ret = ad7616_spi_read(dev, reg_addr, &reg_data);
	else
		ret = ad7616_par_read(dev, reg_addr, &reg_data);
	reg_data &= ~mask;
	reg_data |= data;
	if (dev->interface == AD7616_SERIAL)
		ret |= ad7616_spi_write(dev, reg_addr, reg_data);
	else
		ret |= ad7616_par_write(dev, reg_addr, reg_data);

	return ret;
}

/**
 * SPI read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_spi_read(struct ad7616_dev *dev,
			uint8_t reg_addr,
			uint16_t *reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	buf[0] = 0x00 | ((reg_addr & 0x3F) << 1);
	buf[1] = 0x00;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);

	buf[0] = 0x00 | ((reg_addr & 0x3F) << 1);
	buf[1] = 0x00;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	*reg_data = ((buf[0] & 0x01) << 8) | buf[1];

	return ret;
}

/**
 * SPI write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_spi_write(struct ad7616_dev *dev,
			 uint8_t reg_addr,
			 uint16_t reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	buf[0] = 0x80 | ((reg_addr & 0x3F) << 1) | ((reg_data & 0x100) >> 8);
	buf[1] = (reg_data & 0xFF);
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);

	return ret;
}

/**
 * Perform a full reset of the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_reset(struct ad7616_dev *dev)
{
	int32_t ret;

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	/* Low pulse width for a full reset should be at least 1200 ns */
	no_os_mdelay(20);
	ret |= no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	/* 15 ms are required to completely reconfigure the device */
	no_os_mdelay(150);

	return ret;
}

/**
 * Set the analog input range for the selected analog input channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 * 			   Accepted values: AD7616_VA0
 * 								AD7616_VA1
 * 								AD7616_VA2
 * 								AD7616_VA3
 * 								AD7616_VA4
 * 								AD7616_VA5
 * 								AD7616_VA6
 * 								AD7616_VA7
 * 								AD7616_VB0
 * 								AD7616_VB1
 * 								AD7616_VB2
 * 								AD7616_VB3
 * 								AD7616_VB4
 * 								AD7616_VB5
 * 								AD7616_VB6
 * 								AD7616_VB7
 * @param range - The analog input range.
 * 				  Accepted values: AD7616_2V5
 * 								   AD7616_5V
 * 								   AD7616_10V
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_set_range(struct ad7616_dev *dev,
			 enum ad7616_ch ch,
			 enum ad7616_range range)
{
	uint8_t	reg_addr;
	uint8_t	mask;
	uint8_t	data;
	uint8_t i;
	int32_t ret;

	if (dev->mode == AD7616_SW) {
		if (ch <= AD7616_VA7) {
			dev->va[ch] = range;
			if (ch <= AD7616_VA3) {
				reg_addr = AD7616_REG_INPUT_RANGE_A1;
				mask = AD7616_INPUT_RANGE(ch, AD7616_10V);
				data = AD7616_INPUT_RANGE(ch, range);
			} else {
				reg_addr = AD7616_REG_INPUT_RANGE_A2;
				mask = AD7616_INPUT_RANGE(ch - AD7616_VA4, AD7616_10V);
				data = AD7616_INPUT_RANGE(ch - AD7616_VA4, range);
			}
		} else {
			dev->vb[ch - AD7616_VB0] = range;
			if (ch <= AD7616_VB3) {
				reg_addr = AD7616_REG_INPUT_RANGE_B1;
				mask = AD7616_INPUT_RANGE(ch - AD7616_VB0, AD7616_10V);
				data = AD7616_INPUT_RANGE(ch - AD7616_VB0, range);
			} else {
				reg_addr = AD7616_REG_INPUT_RANGE_B2;
				mask = AD7616_INPUT_RANGE(ch - AD7616_VB4, AD7616_10V);
				data = AD7616_INPUT_RANGE(ch - AD7616_VB4, range);
			}
		}
		ret = ad7616_write_mask(dev, reg_addr, mask, data);
	} else {
		for (i = 0; i <= AD7616_VA7; i++) {
			dev->va[i] = range;
			dev->vb[i] = range;
		}
		ret = no_os_gpio_set_value(dev->gpio_hw_rngsel0, ((range & 0x01) >> 0));
		ret |= no_os_gpio_set_value(dev->gpio_hw_rngsel1, ((range & 0x02) >> 1));
	}

	return ret;
}

/**
 * Set the operation mode (software or hardware).
 * @param dev - The device structure.
 * @param mode - The operation mode.
 * 				 Accepted values: AD7616_SW
 * 								  AD7616_HW
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_set_mode(struct ad7616_dev *dev,
			enum ad7616_mode mode)
{
	uint8_t i;
	int32_t ret = 0;

	dev->mode = mode;
	for (i = 0; i <= AD7616_VA7; i++) {
		ret |= ad7616_set_range(dev, i, dev->va[i]);
		ret |= ad7616_set_range(dev, i + AD7616_VB0, dev->vb[i]);
	}

	return ret;
}

/**
 * Set the oversampling ratio.
 * @param dev - The device structure.
 * @param osr - The oversampling ratio.
 * 				Accepted values: AD7616_OSR_0
 * 								 AD7616_OSR_2
 * 								 AD7616_OSR_4
 * 								 AD7616_OSR_8
 * 								 AD7616_OSR_16
 * 								 AD7616_OSR_32
 * 								 AD7616_OSR_64
 * 								 AD7616_OSR_128
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_set_oversampling_ratio(struct ad7616_dev *dev,
				      enum ad7616_osr osr)
{
	int32_t ret = 0;

	if (dev->mode == AD7616_SW) {
		ret = ad7616_write_mask(dev, AD7616_REG_CONFIG,
					AD7616_OS(0x7), AD7616_OS(osr));
	} else {
		ret = no_os_gpio_set_value(dev->gpio_os0, ((osr & 0x01) >> 0));
		ret |= no_os_gpio_set_value(dev->gpio_os1, ((osr & 0x02) >> 1));
		ret |= no_os_gpio_set_value(dev->gpio_os2, ((osr & 0x04) >> 2));
	}

	if (ret != 0)
		return ret;

	dev->osr = osr;

	return 0;
}

#ifdef XILINX_PLATFORM
/**
 * @brief Read from device in serial mode.
 *        Enter register mode to read/write registers,
 *        specific to Zynqmp platform that use an FPGA
 * @param dev - ad7616_dev device handler.
 * @param buf - data buffer.
 * @param samples - sample number.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t ad7616_read_data_serial_zynqmp(struct ad7616_dev *dev,
		uint32_t *buf, uint32_t samples)
{
	int32_t ret;
	uint32_t commands_data[1] = {0x00};
	struct spi_engine_offload_message msg;
	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		READ(SPI_ENGINE_WORDS_PER_READ),
		CS_HIGH,
	};

	ret = no_os_pwm_enable(dev->trigger_pwm_desc);
	if (ret != 0)
		return ret;

	dev->spi_desc->mode = NO_OS_SPI_MODE_3;
	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);

	ret = spi_engine_offload_init(dev->spi_desc, dev->offload_init_param);
	if (ret != 0)
		return ret;

	msg.commands_data = commands_data;
	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	msg.rx_addr = (uint32_t)buf;

	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_CTRL,
			   AD7616_CTRL_RESETN | AD7616_CTRL_CNVST_EN);

	ret = spi_engine_offload_transfer(dev->spi_desc, msg, samples);
	if (ret != 0)
		return ret;

	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_CTRL, AD7616_CTRL_RESETN);

	if (dev->dcache_invalidate_range)
		dev->dcache_invalidate_range(msg.rx_addr, samples * 4);

	return ret;
}
#endif

/**
 * @brief Read conversion result from device, checking with CRC if enabled.
 * @param dev - ad7616_dev device handler.
 * @param results - results buffer.
 * @param samples - sample number.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad7616_read_data_serial(struct ad7616_dev *dev,
				struct ad7616_conversion_result *results,
				uint32_t samples)
{
	int32_t ret;

	memset(results, 0, sizeof(*results) * samples);

#ifdef XILINX_PLATFORM
	return ad7616_read_data_serial_zynqmp(dev, (void *)results, samples);
#endif

	for (uint32_t i = 0; i < samples; i++) {
		ret = ad7616_toggle_conv(dev);
		if (ret != 0)
			return ret;

		uint8_t crc;
		ret = ad7616_read_channels(dev, results + (i * dev->layers_nb),
					   &crc);
		if (ret != 0)
			return ret;

		if (dev->crc) {
			ret = check_crc(results + (i * dev->layers_nb),
					dev->layers_nb, crc);
			if (ret != 0)
				return ret;
		}
	}
	return 0;
}

/**
 * PAR read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_par_read(struct ad7616_dev *dev, uint8_t reg_addr,
			uint16_t *reg_data)
{
#ifdef XILINX_PLATFORM
	uint32_t read;

	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_WRITE_DATA,
			   0x0000 | ((reg_addr & 0x3F) << 9));
	no_os_udelay(50);
	no_os_axi_io_read(dev->core_baseaddr, AD7616_REG_UP_READ_DATA, &read);
	*reg_data = read & 0xFF;
	no_os_mdelay(1);

	return 0;
#endif

	return -ENOSYS;
}

/**
 * PAR write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_par_write(struct ad7616_dev *dev, uint8_t reg_addr,
			 uint16_t reg_data)
{
#ifdef XILINX_PLATFORM
	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_WRITE_DATA,
			   0x8000 | ((reg_addr & 0x3F) << 9) |
			   (reg_data & 0xFF));
	no_os_mdelay(1);

	return 0;
#endif

	return -ENOSYS;
}

/**
 * @brief Read from device in parallel mode.
 *        Enter register mode to read/write registers
 * @param dev - ad7616_dev device handler.
 * @param buf - data buffer.
 * @param samples - sample number.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad7616_read_data_parallel(struct ad7616_dev *dev,
				  uint32_t *buf,
				  uint32_t samples)
{
#ifdef XILINX_PLATFORM
	int32_t ret;
	struct axi_dmac			*dmac;
	struct axi_dmac_init	dmac_init;

	dmac_init.name = "ADC DMAC";
	dmac_init.base = dev->offload_init_param->rx_dma_baseaddr;
	dmac_init.irq_option = IRQ_DISABLED;

	axi_dmac_init(&dmac, &dmac_init);
	if(!dmac)
		return -1;

	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_CTRL,
			   AD7616_CTRL_RESETN | AD7616_CTRL_CNVST_EN);

	struct axi_dma_transfer transfer = {
		// Number of bytes to writen/read
		.size = samples,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode (CYCLIC?)
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)buf
	};
	ret = axi_dmac_transfer_start(dmac, &transfer);
	if (ret != 0)
		return ret;
	/* Wait until transfer finishes */
	ret = axi_dmac_transfer_wait_completion(dmac, 500);
	if(ret)
		return ret;
	if (dev->dcache_invalidate_range)
		dev->dcache_invalidate_range((uintptr_t)buf, samples);

	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_CTRL, AD7616_CTRL_RESETN);

	return 0;
#endif

	return -ENOSYS;
}

#ifdef XILINX_PLATFORM
/**
 * Initialize the AXI_AD7616 IP core device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_core_setup(struct ad7616_dev *dev)
{
	uint32_t type;

	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_CTRL, 0x00);
	no_os_mdelay(10);
	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_CTRL, AD7616_CTRL_RESETN);
	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_CONV_RATE, 100);
	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_CTRL,
			   AD7616_CTRL_RESETN | AD7616_CTRL_CNVST_EN);
	no_os_mdelay(10);
	no_os_axi_io_write(dev->core_baseaddr, AD7616_REG_UP_CTRL, AD7616_CTRL_RESETN);
	no_os_mdelay(10);

	no_os_axi_io_read(dev->core_baseaddr, AD7616_REG_UP_IF_TYPE, &type);

	if (type)
		dev->interface = AD7616_PARALLEL;
	else
		dev->interface = AD7616_SERIAL;

	return 0;
}
#endif

/**
 * @brief Setup the sequencer layers
 * @param dev The device structure
 * @param layers Arrays of layers
 * @param layers_nb Nubers of items in the layers array.
 * @param burst 1 if needed, 0 otherwise
 * @return 0 on success, an error value otherwise
 */
int32_t ad7616_setup_sequencer(struct ad7616_dev *dev,
			       struct ad7616_sequencer_layer *layers,
			       uint32_t layers_nb, uint8_t burst)
{
	int ret;
	for (uint32_t i = 0; i < layers_nb; i++) {
		uint16_t data = 0;

		// Last one ?
		if (i == layers_nb - 1)
			data |= AD7616_SSREN;
		data |= (layers[i].ch_b - AD7616_VB0) << 4;
		data |= layers[i].ch_a;

		ret = ad7616_write(dev, AD7616_REG_SEQUENCER_STACK(i), data);
		if (ret != 0)
			return ret;
	}

	if (burst) {
		ret = ad7616_write_mask(dev, AD7616_REG_CONFIG, AD7616_BURSTEN_MASK,
					AD7616_BURSTEN(1));
		if (ret != 0)
			return ret;
	}

	// Enable channel sequencer
	ret = ad7616_write_mask(dev, AD7616_REG_CONFIG, AD7616_SEQEN_MASK,
				AD7616_SEQEN(1));
	if (ret != 0)
		return ret;

	dev->layers_nb = layers_nb;

	// Dummy CONV pulse as asked in datasheet
	return ad7616_toggle_conv(dev);
}

int32_t ad7616_disable_sequencer(struct ad7616_dev *dev)
{
	dev->layers_nb = 1;
	return ad7616_write_mask(dev, AD7616_REG_CONFIG,
				 AD7616_SEQEN_MASK | AD7616_BURSTEN_MASK,
				 AD7616_BURSTEN(0) | AD7616_SEQEN(0));
}

/** Self test
 * @brief Perform a self test. Channels selection will be left untouched
 * @param dev The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad7616_self_test(struct ad7616_dev *dev)
{
	int ret;
	struct ad7616_conversion_result result = { 0 };
	enum ad7616_ch ch_save[2];

	ret = ad7616_read_channel_source(dev, ch_save, ch_save + 1);
	if (ret != 0)
		return ret;

	ret = ad7616_select_channel_source(dev, AD7616_VA_SELF_TEST);
	if (ret != 0)
		return ret;

	ret = ad7616_select_channel_source(dev, AD7616_VB_SELF_TEST);
	if (ret != 0)
		return ret;

	// First toggle to remove pending garbage
	ret = ad7616_toggle_conv(dev);
	if (ret != 0)
		return ret;

	ret = ad7616_read_data_serial(dev, &result, 1);
	if (ret != 0)
		return ret;

	ret = ad7616_select_channel_source(dev, ch_save[0]);
	if (ret != 0)
		return ret;

	ret = ad7616_select_channel_source(dev, ch_save[1]);
	if (ret != 0)
		return ret;

	return (result.channel_a == AD7616_CHANNEL_A_SELF_TEST_VALUE &&
		result.channel_b == AD7616_CHANNEL_B_SELF_TEST_VALUE) ?
	       0 :
	       -EINVAL;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_setup(struct ad7616_dev **device,
		     struct ad7616_init_param *init_param)
{
	struct ad7616_dev *dev;
	uint8_t i;
	int32_t ret = 0;

	dev = (struct ad7616_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev) {
		return -1;
	}

#ifdef XILINX_PLATFORM
	ret = axi_clkgen_init(&dev->clkgen, init_param->clkgen_init);
	if (ret != 0)
		goto cleanup;

	ret = axi_clkgen_set_rate(dev->clkgen, init_param->axi_clkgen_rate);
	if (ret != 0)
		goto cleanup;

	ret = no_os_pwm_init(&dev->trigger_pwm_desc, init_param->trigger_pwm_init);
	if (ret != 0)
		goto cleanup;

	dev->core_baseaddr = init_param->core_baseaddr;
	dev->offload_init_param = init_param->offload_init_param;
	dev->reg_access_speed = init_param->reg_access_speed;
	dev->dcache_invalidate_range = init_param->dcache_invalidate_range;

	ad7616_core_setup(dev);
#else
	dev->interface = AD7616_SERIAL;
#endif

	if (dev->interface == AD7616_SERIAL) {
		ret = no_os_spi_init(&dev->spi_desc, init_param->spi_param);
		if (ret != 0)
			goto cleanup;
#ifdef XILINX_PLATFORM
		spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);
#endif
	}

	ret = no_os_gpio_get_optional(&dev->gpio_hw_rngsel0,
				      init_param->gpio_hw_rngsel0_param);
	if (ret != 0)
		goto cleanup;

	ret = no_os_gpio_get_optional(&dev->gpio_hw_rngsel1,
				      init_param->gpio_hw_rngsel1_param);
	if (ret != 0)
		goto cleanup;

	ret = no_os_gpio_get_optional(&dev->gpio_reset, init_param->gpio_reset_param);
	if (ret != 0)
		goto cleanup;

	ret = no_os_gpio_get_optional(&dev->gpio_os0, init_param->gpio_os0_param);
	if (ret != 0)
		goto cleanup;

	ret = no_os_gpio_get_optional(&dev->gpio_os1, init_param->gpio_os1_param);
	if (ret != 0)
		goto cleanup;

	ret = no_os_gpio_get_optional(&dev->gpio_os2, init_param->gpio_os2_param);
	if (ret != 0)
		goto cleanup;

	ret = no_os_gpio_get_optional(&dev->gpio_convst,
				      init_param->gpio_convst_param);
	if (ret != 0)
		goto cleanup;

	ret = no_os_gpio_get_optional(&dev->gpio_busy,
				      init_param->gpio_busy_param);
	if (ret != 0)
		goto cleanup;

	if (dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_HIGH);
		if (ret != 0)
			goto cleanup;

		ret = ad7616_reset(dev);
		if (ret != 0)
			goto cleanup;
	}

	for (i = 0; i <= AD7616_VA7; i++) {
		dev->va[i] = init_param->va[i] ? init_param->va[i] : AD7616_10V;
		dev->vb[i] = init_param->vb[i] ? init_param->vb[i] : AD7616_10V;
	}
	ret = ad7616_set_mode(dev, init_param->mode);
	if (ret != 0)
		goto cleanup;

	ret = ad7616_set_oversampling_ratio(dev, init_param->osr);
	if (ret != 0)
		goto cleanup;

	*device = dev;

	dev->layers_nb = 1;

	if (init_param->crc) {
		ret = ad7616_enable_crc(dev);
		if (ret != 0)
			goto cleanup;
	} else {
		dev->crc = 0;
	}

	return ad7616_self_test(dev);

cleanup:
	ad7616_remove(dev);

	return ret;
}

void ad7616_remove(struct ad7616_dev *dev)
{
	if (!dev)
		return;

#ifdef XILINX_PLATFORM
	if (dev->clkgen)
		axi_clkgen_remove(dev->clkgen);

	no_os_pwm_remove(dev->trigger_pwm_desc);
#endif

	if (dev->interface == AD7616_SERIAL)
		no_os_spi_remove(dev->spi_desc);

	no_os_gpio_remove(dev->gpio_hw_rngsel0);
	no_os_gpio_remove(dev->gpio_hw_rngsel1);
	no_os_gpio_remove(dev->gpio_reset);
	no_os_gpio_remove(dev->gpio_os0);
	no_os_gpio_remove(dev->gpio_os1);
	no_os_gpio_remove(dev->gpio_os2);
	no_os_gpio_remove(dev->gpio_convst);
	no_os_gpio_remove(dev->gpio_busy);

	no_os_free(dev);
}
