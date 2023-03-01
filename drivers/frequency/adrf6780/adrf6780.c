/***************************************************************************//**
 *   @file   adrf6780.c
 *   @brief  Implementation of adrf6780 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <malloc.h>
#include "adrf6780.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Writes data to ADRF6780 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_spi_write(struct adrf6780_dev *dev, uint8_t reg_addr,
		       uint16_t data)
{
	uint8_t buff[ADRF6780_BUFF_SIZE_BYTES];

	/*
	    |                Byte 0           |     Byte 1     |       Byte 2        |
	    | 0 | Addr bits 5-0 | Data bit 15 | Data bits 14-7 | Data bits 6 - 0 | 0 |
	*/
	buff[0] = ADRF6780_SPI_WRITE_CMD | (reg_addr << 1) | (data >> 15);
	buff[1] = data >> 7;
	buff[2] = data << 1;

	return no_os_spi_write_and_read(dev->spi_desc, buff,
					ADRF6780_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADRF6780 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_spi_read(struct adrf6780_dev *dev, uint8_t reg_addr,
		      uint16_t *data)
{
	uint8_t buff[ADRF6780_BUFF_SIZE_BYTES];
	int ret;

	buff[0] = ADRF6780_SPI_READ_CMD | (reg_addr << 1);
	buff[1] = 0;
	buff[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADRF6780_BUFF_SIZE_BYTES);
	if(ret)
		return ret;

	/*
	    |                Byte 0           |     Byte 1     |       Byte 2        |
	    | 1 | Addr bits 5-0 | Data bit 15 | Data bits 14-7 | Data bits 6 - 0 | 0 |
	*/
	*data = ((buff[0] & NO_OS_BIT(0)) << 15 | buff[1] << 7 | buff[2] >> 1);

	return 0;
}

/**
 * @brief Update ADRF6780 register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param data - Data written to the device (requires prior bit shifting).
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_spi_update_bits(struct adrf6780_dev *dev, uint8_t reg_addr,
			     uint16_t mask, uint16_t data)
{
	uint16_t read_val;
	int ret;

	ret = adrf6780_spi_read(dev, reg_addr, &read_val);
	if (ret)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	return adrf6780_spi_write(dev, reg_addr, read_val);
}

/**
 * @brief Set IMD Performance Improvement.
 * @param dev - The device structure.
 * @param rdac_lin - RDAC 8bits data.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_set_rdac_linearize(struct adrf6780_dev *dev, uint8_t rdac_lin)
{
	return adrf6780_spi_write(dev, ADRF6780_REG_LINEARIZE,
				  no_os_field_prep(ADRF6780_RDAC_LINEARIZE_MSK, rdac_lin));
}

/**
 * @brief Get IMD Performance Improvement.
 * @param dev - The device structure.
 * @param rdac_lin - RDAC 8bits data.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_get_rdac_linearize(struct adrf6780_dev *dev, uint8_t *rdac_lin)
{
	uint16_t data;
	int ret;

	ret = adrf6780_spi_read(dev, ADRF6780_REG_LINEARIZE, &data);
	if (ret)
		return ret;

	*rdac_lin = no_os_field_get(ADRF6780_RDAC_LINEARIZE_MSK, data);

	return 0;
}

/**
 * @brief Set I/Q Phase Accuracy.
 * @param dev - The device structure.
 * @param i_data - I Phase Accuracy.
 * @param q_data - Q Phase Accuracy.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_set_cdac_iq_phase_accuracy(struct adrf6780_dev *dev,
					uint8_t i_data, uint8_t q_data)
{
	int ret;

	ret = adrf6780_spi_update_bits(dev,
				       ADRF6780_REG_LO_PATH,
				       ADRF6780_I_PATH_PHASE_ACCURACY_MSK,
				       no_os_field_prep(ADRF6780_I_PATH_PHASE_ACCURACY_MSK, i_data));
	if (ret)
		return ret;

	return adrf6780_spi_update_bits(dev,
					ADRF6780_REG_LO_PATH,
					ADRF6780_Q_PATH_PHASE_ACCURACY_MSK,
					no_os_field_prep(ADRF6780_Q_PATH_PHASE_ACCURACY_MSK, q_data));
}

/**
 * @brief Get I/Q Phase Accuracy.
 * @param dev - The device structure.
 * @param i_data - I Phase Accuracy.
 * @param q_data - Q Phase Accuracy.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_get_cdac_iq_phase_accuracy(struct adrf6780_dev *dev,
					uint8_t *i_data, uint8_t *q_data)
{
	uint16_t data;
	int ret;

	ret = adrf6780_spi_read(dev, ADRF6780_REG_LO_PATH, &data);
	if (ret)
		return ret;

	*i_data = no_os_field_get(ADRF6780_I_PATH_PHASE_ACCURACY_MSK, data);
	*q_data = no_os_field_get(ADRF6780_Q_PATH_PHASE_ACCURACY_MSK, data);

	return 0;
}

/**
 * @brief Read ADC raw data.
 * @param dev - The device structure.
 * @param data - ADC Data.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_read_adc_raw(struct adrf6780_dev *dev, uint16_t *data)
{
	int ret;

	ret = adrf6780_spi_update_bits(dev, ADRF6780_REG_ADC_CONTROL,
				       ADRF6780_ADC_EN_MSK |
				       ADRF6780_ADC_CLOCK_EN_MSK |
				       ADRF6780_ADC_START_MSK,
				       no_os_field_prep(ADRF6780_ADC_EN_MSK, 1) |
				       no_os_field_prep(ADRF6780_ADC_CLOCK_EN_MSK, 1) |
				       no_os_field_prep(ADRF6780_ADC_START_MSK, 1));
	if (ret)
		return ret;

	/* Recommended delay for the ADC to be ready */
	no_os_udelay(200);

	ret = adrf6780_spi_read(dev, ADRF6780_REG_ADC_OUTPUT, data);
	if (ret)
		return ret;

	if (!(*data & ADRF6780_ADC_STATUS_MSK))
		return -EINVAL;

	ret = adrf6780_spi_update_bits(dev, ADRF6780_REG_ADC_CONTROL,
				       ADRF6780_ADC_START_MSK,
				       no_os_field_prep(ADRF6780_ADC_START_MSK, 0));
	if (ret)
		return ret;

	return adrf6780_spi_read(dev, ADRF6780_REG_ADC_OUTPUT, data);
}

/**
 * @brief Software Reset.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_soft_reset(struct adrf6780_dev *dev)
{
	int ret;

	ret = adrf6780_spi_update_bits(dev, ADRF6780_REG_CONTROL,
				       ADRF6780_SOFT_RESET_MSK,
				       no_os_field_prep(ADRF6780_SOFT_RESET_MSK, 1));
	if (ret)
		return ret;

	return adrf6780_spi_update_bits(dev, ADRF6780_REG_CONTROL,
					ADRF6780_SOFT_RESET_MSK,
					no_os_field_prep(ADRF6780_SOFT_RESET_MSK, 0));
}

/**
 * @brief Initializes the adrf6780.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
int adrf6780_init(struct adrf6780_dev **device,
		  struct adrf6780_init_param *init_param)
{
	uint16_t chip_id, enable_reg_msk, enable_reg;
	struct adrf6780_dev *dev;
	int ret;

	dev = (struct adrf6780_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->lo_in = init_param->lo_in;
	dev->vga_buff_en = init_param->vga_buff_en;
	dev->lo_buff_en = init_param->lo_buff_en;
	dev->if_mode_en = init_param->if_mode_en;
	dev->lo_x2_en = init_param->lo_x2_en;
	dev->lo_en = init_param->lo_en;
	dev->lo_in = init_param->lo_in;
	dev->uc_bias_en = init_param->uc_bias_en;
	dev->lo_sideband = init_param->lo_sideband;
	dev->vdet_out_en = init_param->vdet_out_en;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	/* Perform a software reset */
	ret = adrf6780_soft_reset(dev);
	if (ret)
		goto error_spi;

	ret = adrf6780_spi_read(dev, ADRF6780_REG_CONTROL, &chip_id);
	if (ret)
		goto error_spi;

	chip_id = no_os_field_get(ADRF6780_CHIP_ID_MSK, chip_id);
	if (chip_id != ADRF6780_CHIP_ID) {
		ret = -EINVAL;
		goto error_spi;
	}

	enable_reg_msk = ADRF6780_VGA_BUFFER_EN_MSK |
			 ADRF6780_DETECTOR_EN_MSK |
			 ADRF6780_LO_BUFFER_EN_MSK |
			 ADRF6780_IF_MODE_EN_MSK |
			 ADRF6780_IQ_MODE_EN_MSK |
			 ADRF6780_LO_X2_EN_MSK |
			 ADRF6780_LO_PPF_EN_MSK |
			 ADRF6780_LO_EN_MSK |
			 ADRF6780_UC_BIAS_EN_MSK;

	enable_reg = no_os_field_prep(ADRF6780_VGA_BUFFER_EN_MSK, dev->vga_buff_en) |
		     no_os_field_prep(ADRF6780_DETECTOR_EN_MSK, 1) |
		     no_os_field_prep(ADRF6780_LO_BUFFER_EN_MSK, dev->lo_buff_en) |
		     no_os_field_prep(ADRF6780_IF_MODE_EN_MSK, dev->if_mode_en) |
		     no_os_field_prep(ADRF6780_IQ_MODE_EN_MSK, !(dev->if_mode_en)) |
		     no_os_field_prep(ADRF6780_LO_X2_EN_MSK, dev->lo_x2_en) |
		     no_os_field_prep(ADRF6780_LO_PPF_EN_MSK, !(dev->lo_x2_en)) |
		     no_os_field_prep(ADRF6780_LO_EN_MSK, dev->lo_en) |
		     no_os_field_prep(ADRF6780_UC_BIAS_EN_MSK, dev->uc_bias_en);

	ret = adrf6780_spi_update_bits(dev, ADRF6780_REG_ENABLE,
				       enable_reg_msk, enable_reg);
	if (ret)
		return ret;

	ret = adrf6780_spi_update_bits(dev, ADRF6780_REG_LO_PATH,
				       ADRF6780_LO_SIDEBAND_MSK,
				       no_os_field_prep(ADRF6780_LO_SIDEBAND_MSK, dev->lo_sideband));
	if (ret)
		return ret;

	ret = adrf6780_spi_update_bits(dev, ADRF6780_REG_ADC_CONTROL,
				       ADRF6780_VDET_OUTPUT_SELECT_MSK,
				       no_os_field_prep(ADRF6780_VDET_OUTPUT_SELECT_MSK, dev->vdet_out_en));
	if (ret)
		return ret;

	*device = dev;

	return 0;
error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief ADRF6780 Resources Deallocation.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adrf6780_remove(struct adrf6780_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
