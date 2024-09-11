/***************************************************************************//**
*   @file   ade7913.c
*   @brief  Implementation of ADE7913 Driver.
*   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "ade7913.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc8.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"


/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_read(struct ade7913_dev *dev, uint8_t reg_addr, uint8_t *reg_data)
{
	int ret;
	/* no of read bytes, depends on burst mode or register number of bits */
	uint8_t no_of_read_bytes = 2;
	/* offset of data read in the read buffer */
	uint8_t data_byte_offset = 1;
	/* position of byte in buffer to start read to */
	uint8_t position = 0;
	/* buffer for data read (large enough for a burst read) */
	uint8_t buff[16] = { 0 };

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	/* check if burst mode enabled */
	// no_of_read_bytes = no of register bytes + 1(cmd)
	if (dev->burst_mode)
		no_of_read_bytes = 15; // 3*24 bits + 2*16 bits + 1*8 bits
	else if ((reg_addr >= ADE7913_REG_IWV) & (reg_addr <= ADE7913_REG_V2WV))
		no_of_read_bytes = 4; // 24 bits registers
	else if ((reg_addr >= ADE7913_REG_ADC_CRC) & (reg_addr <=
			ADE7913_REG_CNT_SNAPSHOT))
		no_of_read_bytes = 3; // 16 bits registers
	else
		no_of_read_bytes = 2; // 8 bits registers

	/* set address to read from */
	buff[0] = (uint8_t) no_os_field_prep(NO_OS_GENMASK(8,3), reg_addr);
	/* set read bit */
	buff[0] |= ADE7913_SPI_READ;

	/* send read command */
	ret = no_os_spi_write_and_read(dev->spi_desc, &buff[position],
				       no_of_read_bytes);
	if (ret)
		return ret;

	/* get data */
	if (dev->burst_mode) {
		dev->i_wav = no_os_sign_extend32(no_os_get_unaligned_be24
						 (&buff[data_byte_offset]),23);
		dev->v1_wav = no_os_sign_extend32(no_os_get_unaligned_be24
						  (&buff[data_byte_offset+3]),23);
		dev->v2_wav = no_os_sign_extend32(no_os_get_unaligned_be24
						  (&buff[data_byte_offset+6]),23);
		dev->adc_crc = no_os_sign_extend16(no_os_get_unaligned_be16
						   (&buff[data_byte_offset+9]), 15);
		dev->status0 = buff[data_byte_offset+11];
		dev->cnt_snapshot = no_os_sign_extend16(no_os_get_unaligned_be16
							(&buff[data_byte_offset+12]), 15);;
	} else if (reg_addr == ADE7913_REG_IWV) {
		dev->i_wav = no_os_sign_extend32(no_os_get_unaligned_be24
						 (&buff[data_byte_offset]),23);
	} else if (reg_addr == ADE7913_REG_V1WV) {
		dev->v1_wav = no_os_sign_extend32(no_os_get_unaligned_be24
						  (&buff[data_byte_offset]),23);
	} else if (reg_addr == ADE7913_REG_V2WV) {
		dev->v2_wav = no_os_sign_extend32(no_os_get_unaligned_be24
						  (&buff[data_byte_offset]),23);
	} else if (reg_addr == ADE7913_REG_ADC_CRC) {
		dev->adc_crc = no_os_sign_extend16(no_os_get_unaligned_be16
						   (&buff[data_byte_offset]), 15);
	} else if (reg_addr == ADE7913_REG_CNT_SNAPSHOT) {
		dev->cnt_snapshot = no_os_sign_extend16(no_os_get_unaligned_be16
							(&buff[data_byte_offset]), 15);;
	}

	/* set read data */
	*reg_data = buff[position + data_byte_offset];

	return 0;
}

/**
 * @brief Read multiple devices
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_read_waveforms(struct ade7913_dev *dev, uint8_t reg_addr,
			   uint8_t *reg_data)
{
	int ret;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	if (dev->no_devs < 1 || dev->no_devs > 4)
		return -EINVAL;
	/* read waveforms from the number of devices connected */
	switch (dev->no_devs) {
	case 3 :
		// third device
		dev->spi_desc = dev->spi_desc2;
		ret = ade7913_read(dev, reg_addr,
				   &reg_data);
		if (ret)
			return ret;
		dev->i_wav_m[2] = dev->i_wav;
		dev->v1_wav_m[2] = dev->v1_wav;
		dev->v2_wav_m[2] = dev->v2_wav;
		// second device
		dev->spi_desc = dev->spi_desc1;
		ret = ade7913_read(dev, reg_addr,
				   &reg_data);
		if (ret)
			return ret;
		dev->i_wav_m[1] = dev->i_wav;
		dev->v1_wav_m[1] = dev->v1_wav;
		dev->v2_wav_m[1] = dev->v2_wav;
		// first device
		dev->spi_desc = dev->spi_desc0;
		ret = ade7913_read(dev, reg_addr,
				   &reg_data);
		if (ret)
			return ret;
		dev->i_wav_m[0] = dev->i_wav;
		dev->v1_wav_m[0] = dev->v1_wav;
		dev->v2_wav_m[0] = dev->v2_wav;
		break;
	case 2 :
		// second device
		dev->spi_desc = dev->spi_desc1;
		ret = ade7913_read(dev, reg_addr,
				   &reg_data);
		if (ret)
			return ret;
		dev->i_wav_m[1] = dev->i_wav;
		dev->v1_wav_m[1] = dev->v1_wav;
		dev->v2_wav_m[1] = dev->v2_wav;
		// first device
		dev->spi_desc = dev->spi_desc0;
		ret = ade7913_read(dev, reg_addr,
				   &reg_data);
		if (ret)
			return ret;
		dev->i_wav_m[0] = dev->i_wav;
		dev->v1_wav_m[0] = dev->v1_wav;
		dev->v2_wav_m[0] = dev->v2_wav;
		break;
	case 1 :
		// single device
		dev->spi_desc = dev->spi_desc0;
		ret = ade7913_read(dev, reg_addr,
				   &reg_data);
		if (ret)
			return ret;
		dev->i_wav_m[0] = dev->i_wav;
		dev->v1_wav_m[0] = dev->v1_wav;
		dev->v2_wav_m[0] = dev->v2_wav;
		break;

	}

	return 0;
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_write(struct ade7913_dev *dev, uint8_t reg_addr, uint8_t reg_data)
{
	int ret;
	/* buffer for write */
	uint8_t buff[2] = {0};

	if (!dev)
		return -ENODEV;

	/* set address to read from */
	buff[0] = (uint8_t) no_os_field_prep(NO_OS_GENMASK(8,3), reg_addr);
	/* set data to write */
	buff[1] = reg_data;

	/* send write command */
	ret = no_os_spi_write_and_read(dev->spi_desc, buff, 2);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Write broadcast.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_write_broadcast(struct ade7913_dev *dev, uint8_t reg_addr,
			    uint8_t *reg_data)
{
	int ret;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	switch (dev->no_devs) {
	case 3 :
		// third device
		dev->spi_desc = dev->spi_desc2;
		ret = ade7913_write(dev, reg_addr, reg_data);
		if (ret)
			return ret;
		// second device
		dev->spi_desc = dev->spi_desc1;
		ret = ade7913_write(dev, reg_addr, reg_data);
		if (ret)
			return ret;
		// first device
		dev->spi_desc = dev->spi_desc0;
		ret = ade7913_write(dev, reg_addr, reg_data);
		if (ret)
			return ret;
		break;
	case 2 :
		// second device
		dev->spi_desc = dev->spi_desc1;
		ret = ade7913_write(dev, reg_addr, reg_data);
		if (ret)
			return ret;
		// first device
		dev->spi_desc = dev->spi_desc0;
		ret = ade7913_write(dev, reg_addr, reg_data);
		if (ret)
			return ret;
		break;
	}

	return 0;
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ade7913_update_bits(struct ade7913_dev *dev, uint8_t reg_addr,
			       uint8_t mask, uint8_t reg_data)
{
	int ret;
	// data read from register
	uint8_t data;

	if (!dev)
		return -ENODEV;

	// read the register data
	ret = ade7913_read(dev, reg_addr, &data);
	if (ret)
		return ret;
	// apply the mask
	data &= ~mask;
	// update the specified bits
	data |= reg_data & mask;

	// write the new data to the register
	return ade7913_write(dev, reg_addr, data);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_init(struct ade7913_dev **device,
		 struct ade7913_init_param init_param)
{
	int ret;
	// device strucuture
	struct ade7913_dev *dev;
	// product version
	uint8_t *v_product;
	// register value
	uint8_t reg_val;
	// variables for waveforms
	int32_t *i_wav;
	int32_t *v1_wav;
	int32_t *v2_wav;
	// register data
	uint8_t data;
	// comms up timeout
	uint16_t timeout;

	/* If init_param.no_devs uninitialized, then 1 device */
	if (init_param.no_devs == 0)
		init_param.no_devs = 1;

	// The number of devices is set to < 3
	if (init_param.no_devs > 3)
		return -EINVAL;

	// allocate memory for the dev strucuture
	dev = (struct ade7913_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	// initialize the number of devices
	dev->no_devs = init_param.no_devs;
	// initialize burst mode
	dev->burst_mode = init_param.burst_mode;

	// allocate memory for the waveforms depending on the number of dev
	i_wav = (int32_t *)no_os_calloc(dev->no_devs, sizeof(i_wav));
	if (!i_wav)
		goto err_alloc_i_wav;
	v1_wav = (int32_t *)no_os_calloc(dev->no_devs, sizeof(v2_wav));
	if (!v1_wav)
		goto err_alloc_v1_wav;
	v2_wav = (int32_t *)no_os_calloc(dev->no_devs, sizeof(v2_wav));
	if (!v2_wav)
		goto err_alloc_v2_wav;

	v_product = (uint8_t *)no_os_calloc(dev->no_devs, sizeof(v_product));
	if (!v_product)
		goto err_alloc_v_product;

	dev->i_wav_m = i_wav;
	dev->v1_wav_m = v1_wav;
	dev->v2_wav_m = v2_wav;
	dev->ver_product = v_product;

	/* SPI Initialization */
	// multpile descriptors if more than one device available
	switch (dev->no_devs) {
	case 3 :
		// spi descriptor for first device
		ret = no_os_spi_init(&dev->spi_desc0, init_param.spi_init0);
		if (ret)
			goto error_dev;
		/* Read version product dev1 */
		dev->spi_desc = dev->spi_desc0;
		/* Wait until devices comm is initialized*/
		do {
			ret = ade7913_read(dev, ADE7913_REG_STATUS0, &data);
			no_os_mdelay(5);
			timeout++;
			if (timeout == 20) {
				ret = -ENOTCONN;
				goto error_dev;
			}
		} while (data);
		ret = ade7913_get_version_product(dev, &reg_val);
		if (ret)
			goto error_spi0;
		v_product[0] = reg_val;
		// spi descriptor for second device
		ret = no_os_spi_init(&dev->spi_desc1, init_param.spi_init1);
		if (ret)
			goto error_spi0;
		/* Read version product dev2 */
		dev->spi_desc = dev->spi_desc1;
		/* Wait until devices comm is initialized*/
		do {
			ret = ade7913_read(dev, ADE7913_REG_STATUS0, &data);
			no_os_mdelay(5);
			timeout++;
			if (timeout == 20) {
				ret = -ENOTCONN;
				goto error_dev;
			}
		} while (data);
		ret = ade7913_get_version_product(dev, &reg_val);
		if (ret)
			goto error_spi1;
		v_product[1] = reg_val;
		// spi descriptor for third device
		ret = no_os_spi_init(&dev->spi_desc2, init_param.spi_init2);
		if (ret)
			goto error_spi1;
		/* Read version product dev3 */
		dev->spi_desc = dev->spi_desc2;
		/* Wait until devices comm is initialized*/
		do {
			ret = ade7913_read(dev, ADE7913_REG_STATUS0, &data);
			no_os_mdelay(5);
			timeout++;
			if (timeout == 20) {
				ret = -ENOTCONN;
				goto error_dev;
			}
		} while (data);
		ret = ade7913_get_version_product(dev, &reg_val);
		if (ret)
			goto error_spi2;
		v_product[2] = reg_val;
		break;
	case 2 :
		// spi descriptor for first device
		ret = no_os_spi_init(&dev->spi_desc0, init_param.spi_init0);
		if (ret)
			goto error_dev;
		/* Read version product dev1 */
		dev->spi_desc = dev->spi_desc0;
		/* Wait until devices comm is initialized*/
		do {
			ret = ade7913_read(dev, ADE7913_REG_STATUS0, &data);
			no_os_mdelay(5);
			timeout++;
			if (timeout == 20) {
				ret = -ENOTCONN;
				goto error_dev;
			}
		} while (data);
		ret = ade7913_get_version_product(dev, &reg_val);
		if (ret)
			goto error_spi0;
		v_product[0] = reg_val;
		// spi descriptor for second device
		ret = no_os_spi_init(&dev->spi_desc1, init_param.spi_init1);
		if (ret)
			goto error_spi0;
		/* Read version product dev2 */
		dev->spi_desc = dev->spi_desc1;
		/* Wait until devices comm is initialized*/
		do {
			ret = ade7913_read(dev, ADE7913_REG_STATUS0, &data);
			no_os_mdelay(5);
			timeout++;
			if (timeout == 20) {
				ret = -ENOTCONN;
				goto error_dev;
			}
		} while (data);
		ret = ade7913_get_version_product(dev, &reg_val);
		if (ret)
			goto error_spi1;
		v_product[1] = reg_val;
		break;
	case 1 :
		// spi descriptor for single device
		ret = no_os_spi_init(&dev->spi_desc0, init_param.spi_init0);
		if (ret)
			goto error_dev;
		/* Read version product dev1 */
		dev->spi_desc = dev->spi_desc0;
		/* Wait until devices comm is initialized*/
		do {
			ret = ade7913_read(dev, ADE7913_REG_STATUS0, &data);
			no_os_mdelay(5);
			timeout++;
			if (timeout == 20) {
				ret = -ENOTCONN;
				goto error_dev;
			}
		} while (data);
		ret = ade7913_get_version_product(dev, &reg_val);
		if (ret)
			goto error_spi0;
		v_product[0] = reg_val;
		break;
	}

	// initilize the spi for the first device
	dev->spi_desc = dev->spi_desc0;

	*device = dev;

	return 0;

error_spi2:
	no_os_spi_remove(dev->spi_desc2);
error_spi1:
	no_os_spi_remove(dev->spi_desc1);
error_spi0:
	no_os_spi_remove(dev->spi_desc0);
error_dev:
	no_os_free(v_product);
err_alloc_v_product:
	no_os_free(v2_wav);
err_alloc_v2_wav:
	no_os_free(v1_wav);
err_alloc_v1_wav:
	no_os_free(i_wav);
err_alloc_i_wav:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_remove(struct ade7913_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc2);
	if (ret)
		return ret;
	ret = no_os_spi_remove(dev->spi_desc1);
	if (ret)
		return ret;
	ret = no_os_spi_remove(dev->spi_desc0);
	if (ret)
		return ret;
	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev->ver_product);
	no_os_free(dev->i_wav_m);
	no_os_free(dev->v1_wav_m);
	no_os_free(dev->v2_wav_m);
	no_os_free(dev);

	return 0;
}

/**
 * @brief Reset the device using SW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_sw_reset(struct ade7913_dev *dev)
{
	int ret;
	uint8_t *ver_product;
	// reset on variable must be 0 after reset
	uint8_t reset_on;

	if (!dev)
		return -ENODEV;

	ret = ade7913_update_bits(dev, ADE7913_REG_CONFIG, ADE7913_SWRST_MSK, ENABLE);
	if (ret)
		return ret;

	/* Wait for device to initialize */
	do {
		ret = ade7913_read(dev, ADE7913_REG_STATUS0, &reset_on);
		if (ret)
			return ret;
	} while (reset_on);

	/* Read version product */
	ret = ade7913_get_version_product(dev, &ver_product);
	if (ret)
		return ret;

	dev->ver_product = ver_product;

	return 0;
}

/**
 * @brief Lock device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_wr_lock(struct ade7913_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade7913_write(dev, ADE7913_REG_LOCK, ADE7913_LOCK_KEY);
}

/**
 * @brief Unlock device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_wr_unlock(struct ade7913_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade7913_write(dev, ADE7913_REG_LOCK, ADE7913_UNLOCK_KEY);
}

/**
 * @brief Get synchronization counter value.
 * @param dev - The device structure.
 * @param counter - Counter value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_get_sync_cnt_val(struct ade7913_dev *dev, uint16_t *counter)
{
	int ret;
	uint8_t reg_val[2];

	if (!dev)
		return -ENODEV;
	if (!counter)
		return -EINVAL;

	ret = ade7913_read(dev, ADE7913_REG_COUNTER1, &reg_val[1]);
	if (ret)
		return ret;
	ret = ade7913_read(dev, ADE7913_REG_COUNTER0, &reg_val[0]);
	if (ret)
		return ret;

	*counter = no_os_get_unaligned_le16(reg_val);

	return 0;
}

/**
 * @brief Set clkout enable.
 * @param dev - The device structure.
 * @param clkout_en - 0 - dready functionality enabled 1 - clkout functionality enabled.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_set_clkout_en(struct ade7913_dev *dev,
			  uint8_t clkout_en)
{
	if (!dev)
		return -ENODEV;

	return ade7913_update_bits(dev, ADE7913_REG_CONFIG,
				   ADE7913_CLKOUT_EN_MSK, no_os_field_prep(ADE7913_CLKOUT_EN_MSK, clkout_en));
}

/**
 * @brief Power down enable.
 * @param dev - The device structure.
 * @param pwrdwn - 0 - dc-dc is functional, 1 - dc-dc is turned off.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_pwrdwn(struct ade7913_dev *dev,
		   uint8_t pwrdwn)
{
	if (!dev)
		return -ENODEV;

	return ade7913_update_bits(dev, ADE7913_REG_CONFIG,
				   ADE7913_PWRDWN_EN_MSK, no_os_field_prep(ADE7913_PWRDWN_EN_MSK, pwrdwn));
}

/**
 * @brief Temperature enable.
 * @param dev - The device structure.
 * @param temp_en - 0 - voltage V2P V2M measured, 1 - internal temperature sensor measured .
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_temp_en(struct ade7913_dev *dev,
		    uint8_t temp_en)
{
	if (!dev)
		return -ENODEV;

	return ade7913_update_bits(dev, ADE7913_REG_CONFIG,
				   ADE7913_TEMP_EN_MSK, no_os_field_prep(ADE7913_TEMP_EN_MSK, temp_en));
}

/**
 * @brief Sync enable.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_sync_en(struct ade7913_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade7913_update_bits(dev, ADE7913_REG_SYNC_SNAP,
				   ADE7913_SYNC_MSK, no_os_field_prep(ADE7913_SYNC_MSK, ENABLE));
}

/**
 * @brief Set ADC frequency
 * @param dev - The device structure.
 * @param frequency - output frequency
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_adc_freq(struct ade7913_dev *dev,
		     enum ade7913_adc_freq_e frequency)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (frequency) {
	case ADE7913_ADC_FREQ_1KHZ:
		ret = ade9153a_update_bits(dev, ADE7913_REG_CONFIG, ADE7913_ADC_FREQ_MSK,
					   no_os_field_prep(ADE7913_ADC_FREQ_MSK,
							   ADE7913_ADC_FREQ_1KHZ));
		break;
	case ADE7913_ADC_FREQ_2KHZ:
		ret = ade9153a_update_bits(dev, ADE7913_REG_CONFIG, ADE7913_ADC_FREQ_MSK,
					   no_os_field_prep(ADE7913_ADC_FREQ_MSK,
							   ADE7913_ADC_FREQ_2KHZ));
		break;
	case ADE7913_ADC_FREQ_4KHZ:
		ret = ade9153a_update_bits(dev, ADE7913_REG_CONFIG, ADE7913_ADC_FREQ_MSK,
					   no_os_field_prep(ADE7913_ADC_FREQ_MSK,
							   ADE7913_ADC_FREQ_4KHZ));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE7913_REG_CONFIG, ADE7913_ADC_FREQ_MSK,
					   no_os_field_prep(ADE7913_ADC_FREQ_MSK,
							   ADE7913_ADC_FREQ_8KHZ));
		break;
	}

	return ret;
}

/**
 * @brief Digital lpf bandwith select.
 * @param dev - The device structure.
 * @param bw - 0 - BW = 3.3kHz, 1 - BW = 2kHz (for CLKIN 4.096 MHz and ADC freq 8kHz).
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_lfp_bw(struct ade7913_dev *dev, uint8_t bw)
{
	if (!dev)
		return -ENODEV;

	return ade7913_update_bits(dev, ADE7913_REG_CONFIG,
				   ADE7913_BW_MSK, no_os_field_prep(ADE7913_BW_MSK, bw));
}

/**
 * @brief CRC of config registers status.
 * @param dev - The device structure.
 * @param status - status indicator
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_crc_status(struct ade7913_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade7913_read(dev, ADE7913_REG_STATUS0, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE7913_CRC_STAT_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief IC config regs protection status.
 * @param dev - The device structure.
 * @param status - status indicator
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_ic_prot_status(struct ade7913_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade7913_read(dev, ADE7913_REG_STATUS0, &reg_val);
	if (ret)
		return ret;

	status = no_os_test_bit(no_os_find_first_set_bit(ADE7913_IC_PROT_MSK),
				&reg_val);

	return 0;
}

/**
 * @brief Set EMI CTRL register.
 * @param dev - The device structure.
 * @param emi_ctrl - output frequency
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_emi_ctrl(struct ade7913_dev *dev,
		     uint8_t emi_ctrl)
{
	int ret;

	if (!dev)
		return -ENODEV;

	return ade7913_write(dev,ADE7913_REG_EMI_CTRL, emi_ctrl);
}

/**
 * @brief ADC not accesed during one period status.
 * @param dev - The device structure.
 * @param status - status indicator
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_adc_na_status(struct ade7913_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade7913_read(dev, ADE7913_REG_STATUS1, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE7913_ADC_NA_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief Cnt snapshot.
 * @param dev - The device structure.
 * @param val - snapshot value of counter.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_cnt_snapshot_val(struct ade7913_dev *dev, uint16_t *val)
{
	int ret;
	/* register value read */
	uint16_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE7913_REG_CNT_SNAPSHOT, &reg_val);
	if (ret)
		return ret;
	*val = reg_val & (uint32_t)(ADE7913_CNT_VAL_MSK);

	return 0;
}

/**
 * @brief Get version product value.
 * @param dev - The device structure.
 * @param ver_product - VErsion product value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7913_get_version_product(struct ade7913_dev *dev, uint8_t *ver_product)
{
	int ret;
	uint8_t reg_val;
	uint8_t val;

	if (!dev)
		return -ENODEV;
	if (!ver_product)
		return -EINVAL;

	ret = ade7913_read(dev, ADE7913_REG_STATUS1, &reg_val);
	if (ret)
		return ret;
	val = reg_val & (uint8_t)(ADE7913_VERSION_MSK);

	switch (val) {
	case 2:
		*ver_product = ADE7913_2_CHANNEL_ADE7912;
		break;
	case 3:
		*ver_product = ADE7913_3_CHANNEL_ADE7913;
		break;
	default:
		ret = -ENODEV;
		break;
	}

	return 0;
}
