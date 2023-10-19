/***************************************************************************//**
 *   @file   admv1013.c
 *   @brief  Implementation of admv1013 Driver.
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
#include "admv1013.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Writes data to ADMV1013 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv1013_spi_write(struct admv1013_dev *dev, uint8_t reg_addr,
		       uint16_t data)
{
	uint8_t buff[ADMV1013_BUFF_SIZE_BYTES];

	/*
	    |                Byte 0           |     Byte 1     |       Byte 2        |
	    | 0 | Addr bits 5-0 | Data bit 15 | Data bits 14-7 | Data bits 6 - 0 | 0 |
	*/
	buff[0] = ADMV1013_SPI_WRITE_CMD | (reg_addr << 1) | (data >> 15);
	buff[1] = data >> 7;
	buff[2] = data << 1;

	return no_os_spi_write_and_read(dev->spi_desc, buff,
					ADMV1013_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADMV1013 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv1013_spi_read(struct admv1013_dev *dev, uint8_t reg_addr,
		      uint16_t *data)
{
	uint8_t buff[ADMV1013_BUFF_SIZE_BYTES];
	int ret;

	buff[0] = ADMV1013_SPI_READ_CMD | (reg_addr << 1);
	buff[1] = 0;
	buff[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADMV1013_BUFF_SIZE_BYTES);
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
 * @brief Update ADMV1013 register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param data - Data written to the device (requires prior bit shifting).
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv1013_spi_update_bits(struct admv1013_dev *dev, uint8_t reg_addr,
			     uint16_t mask, uint16_t data)
{
	uint16_t read_val;
	int ret;

	ret = admv1013_spi_read(dev, reg_addr, &read_val);
	if (ret)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	return admv1013_spi_write(dev, reg_addr, read_val);
}

/**
 * @brief Update Mixer Gate Voltage.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
static int admv1013_update_mixer_vgate(struct admv1013_dev *dev)
{
	unsigned int mixer_vgate;

	if (dev->vcm_uv <= 1800000)
		mixer_vgate = MIXER_GATE_0_to_1_8_V(dev->vcm_uv);
	else if (dev->vcm_uv > 1800000 && dev->vcm_uv <= 2600000)
		mixer_vgate = MIXER_GATE_1_8_to_2_6_V(dev->vcm_uv);
	else
		return -EINVAL;

	return admv1013_spi_update_bits(dev, ADMV1013_REG_LO_AMP_I,
					ADMV1013_MIXER_VGATE_MSK,
					no_os_field_prep(ADMV1013_MIXER_VGATE_MSK, mixer_vgate));
}

/**
 * @brief Update Quad Filters.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
static int admv1013_update_quad_filters(struct admv1013_dev *dev)
{
	unsigned int filt_raw;

	if ((dev->lo_in < 5400000000) || (dev->lo_in > 10250000000))
		return -EINVAL;

	if ((dev->lo_in >= 5400000000) && (dev->lo_in <= 7000000000))
		filt_raw = LO_BAND_5_4_TO_7_GHZ;
	else if ((dev->lo_in >= 5400000000) && (dev->lo_in <= 8000000000))
		filt_raw = LO_BAND_5_4_TO_8_GHZ;
	else if ((dev->lo_in >= 6600000000) && (dev->lo_in <= 9200000000))
		filt_raw = LO_BAND_6_6_TO_9_2_GHZ;
	else
		filt_raw = LO_BAND_8_62_TO_10_25_GHZ;

	return admv1013_spi_update_bits(dev, ADMV1013_REG_QUAD,
					ADMV1013_QUAD_FILTERS_MSK,
					no_os_field_prep(ADMV1013_QUAD_FILTERS_MSK, filt_raw));
}

/**
 * @brief Set I/Q Phase Accuracy
 * @param dev - The device structure.
 * @param i_phase - The I Data.
 * @param q_phase - The Q Data.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1013_set_iq_phase(struct admv1013_dev *dev, uint8_t i_phase,
			  uint8_t q_phase)
{
	int ret;

	i_phase = no_os_field_prep(ADMV1013_LOAMP_PH_ADJ_FINE_MSK, i_phase);
	q_phase = no_os_field_prep(ADMV1013_LOAMP_PH_ADJ_FINE_MSK, q_phase);

	ret = admv1013_spi_update_bits(dev, ADMV1013_REG_LO_AMP_I,
				       ADMV1013_LOAMP_PH_ADJ_FINE_MSK,
				       i_phase);
	if (ret)
		return ret;

	return admv1013_spi_update_bits(dev, ADMV1013_REG_LO_AMP_Q,
					ADMV1013_LOAMP_PH_ADJ_FINE_MSK,
					q_phase);
}

/**
 * @brief Get I/Q Phase Accuracy
 * @param dev - The device structure.
 * @param i_phase - The I Data.
 * @param q_phase - The Q Data.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1013_get_iq_phase(struct admv1013_dev *dev, uint8_t *i_phase,
			  uint8_t *q_phase)
{
	uint16_t data;
	int ret;

	ret = admv1013_spi_read(dev, ADMV1013_REG_LO_AMP_I, &data);
	if (ret)
		return ret;

	*i_phase = no_os_field_get(ADMV1013_LOAMP_PH_ADJ_FINE_MSK, data);

	ret = admv1013_spi_read(dev, ADMV1013_REG_LO_AMP_Q, &data);
	if (ret)
		return ret;

	*q_phase = no_os_field_get(ADMV1013_LOAMP_PH_ADJ_FINE_MSK, data);

	return 0;
}

/**
 * @brief Set I/Q Offset Accuracy
 * @param dev - The device structure.
 * @param i_offset_p - The I Data Positive.
 * @param i_offset_n - The I Data Negative.
 * @param q_offset_p - The Q Data Positive.
 * @param q_offset_n - The Q Data Negative.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1013_set_iq_offset(struct admv1013_dev *dev, uint8_t i_offset_p,
			   uint8_t i_offset_n, uint8_t q_offset_p,
			   uint8_t q_offset_n)
{
	int ret;

	ret = admv1013_spi_update_bits(dev, ADMV1013_REG_OFFSET_ADJUST_I,
				       ADMV1013_MIXER_OFF_ADJ_P_MSK |
				       ADMV1013_MIXER_OFF_ADJ_N_MSK,
				       no_os_field_prep(ADMV1013_MIXER_OFF_ADJ_P_MSK, i_offset_p) |
				       no_os_field_prep(ADMV1013_MIXER_OFF_ADJ_N_MSK, i_offset_n));
	if (ret)
		return ret;

	return admv1013_spi_update_bits(dev, ADMV1013_REG_OFFSET_ADJUST_Q,
					ADMV1013_MIXER_OFF_ADJ_P_MSK |
					ADMV1013_MIXER_OFF_ADJ_N_MSK,
					no_os_field_prep(ADMV1013_MIXER_OFF_ADJ_P_MSK, q_offset_p) |
					no_os_field_prep(ADMV1013_MIXER_OFF_ADJ_N_MSK, q_offset_n));
}

/**
 * @brief Get I/Q Offset Accuracy
 * @param dev - The device structure.
 * @param i_offset_p - The I Data Positive.
 * @param i_offset_n - The I Data Negative.
 * @param q_offset_p - The Q Data Positive.
 * @param q_offset_n - The Q Data Negative.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1013_get_iq_offset(struct admv1013_dev *dev, uint8_t *i_offset_p,
			   uint8_t *i_offset_n, uint8_t *q_offset_p,
			   uint8_t *q_offset_n)
{
	uint16_t data;
	int ret;

	ret = admv1013_spi_read(dev, ADMV1013_REG_OFFSET_ADJUST_I, &data);
	if (ret)
		return ret;

	*i_offset_p = no_os_field_get(ADMV1013_MIXER_OFF_ADJ_P_MSK, data);
	*i_offset_n = no_os_field_get(ADMV1013_MIXER_OFF_ADJ_N_MSK, data);

	ret = admv1013_spi_read(dev, ADMV1013_REG_OFFSET_ADJUST_Q, &data);
	if (ret)
		return ret;

	*q_offset_p = no_os_field_get(ADMV1013_MIXER_OFF_ADJ_P_MSK, data);
	*q_offset_n = no_os_field_get(ADMV1013_MIXER_OFF_ADJ_N_MSK, data);

	return 0;
}

/**
 * @brief Initializes the admv1013.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1013_init(struct admv1013_dev **device,
		  struct admv1013_init_param *init_param)
{
	struct admv1013_dev *dev;
	uint16_t data;
	int ret;

	dev = (struct admv1013_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	dev->lo_in = init_param->lo_in;
	dev->input_mode = init_param->input_mode;
	dev->quad_se_mode = init_param->quad_se_mode;
	dev->det_en = init_param->det_en;
	dev->vcm_uv = init_param->vcm_uv;

	/* Perform a software reset */
	ret = admv1013_spi_update_bits(dev, ADMV1013_REG_SPI_CONTROL,
				       ADMV1013_SPI_SOFT_RESET_MSK,
				       no_os_field_prep(ADMV1013_SPI_SOFT_RESET_MSK, 1));
	if (ret)
		goto error_spi;

	ret = admv1013_spi_update_bits(dev, ADMV1013_REG_SPI_CONTROL,
				       ADMV1013_SPI_SOFT_RESET_MSK,
				       no_os_field_prep(ADMV1013_SPI_SOFT_RESET_MSK, 0));
	if (ret)
		goto error_spi;

	ret = admv1013_spi_read(dev, ADMV1013_REG_SPI_CONTROL, &data);
	if (ret)
		goto error_spi;

	data = no_os_field_get(ADMV1013_CHIP_ID_MSK, data);
	if (data != ADMV1013_CHIP_ID) {
		ret = -EINVAL;
	}

	/* 0xE700 - Datasheet Value for Temperature Compensation */
	ret = admv1013_spi_write(dev, ADMV1013_REG_VVA_TEMP_COMP, 0xE700);
	if (ret)
		goto error_spi;

	data = no_os_field_prep(ADMV1013_QUAD_SE_MODE_MSK, dev->quad_se_mode);

	ret = admv1013_spi_update_bits(dev, ADMV1013_REG_QUAD,
				       ADMV1013_QUAD_SE_MODE_MSK, data);
	if (ret)
		goto error_spi;

	ret = admv1013_update_mixer_vgate(dev);
	if (ret)
		goto error_spi;

	ret = admv1013_update_quad_filters(dev);
	if (ret)
		goto error_spi;

	ret = admv1013_spi_update_bits(dev, ADMV1013_REG_ENABLE,
				       ADMV1013_DET_EN_MSK |
				       ADMV1013_MIXER_IF_EN_MSK,
				       dev->det_en |
				       dev->input_mode);
	if (ret)
		goto error_spi;

	*device = dev;

	return 0;
error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief ADMV1013 Resources Deallocation.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv1013_remove(struct admv1013_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
