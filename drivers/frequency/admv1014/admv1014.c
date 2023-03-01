/***************************************************************************//**
 *   @file   admv1014.c
 *   @brief  Implementation of admv1014 Driver.
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
#include "admv1014.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/*************************** Variables Definition *****************************/
/******************************************************************************/

static const int mixer_vgate_table[] = {
	106, 107, 108, 110, 111, 112, 113, 114,
	117, 118, 119, 120, 122, 123, 44, 45
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Writes data to ADMV1014 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv1014_spi_write(struct admv1014_dev *dev, uint8_t reg_addr,
		       uint16_t data)
{
	uint8_t buff[ADMV1014_BUFF_SIZE_BYTES];

	/*
	    |                Byte 0           |     Byte 1     |       Byte 2        |
	    | 0 | Addr bits 5-0 | Data bit 15 | Data bits 14-7 | Data bits 6 - 0 | 0 |
	*/
	buff[0] = ADMV1014_SPI_WRITE_CMD | (reg_addr << 1) | (data >> 15);
	buff[1] = data >> 7;
	buff[2] = data << 1;

	return no_os_spi_write_and_read(dev->spi_desc, buff,
					ADMV1014_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADMV1014 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv1014_spi_read(struct admv1014_dev *dev, uint8_t reg_addr,
		      uint16_t *data)
{
	uint8_t buff[ADMV1014_BUFF_SIZE_BYTES];
	int ret;

	buff[0] = ADMV1014_SPI_READ_CMD | (reg_addr << 1);
	buff[1] = 0;
	buff[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADMV1014_BUFF_SIZE_BYTES);
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
 * @brief Update ADMV1014 register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param data - Data written to the device (requires prior bit shifting).
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv1014_spi_update_bits(struct admv1014_dev *dev, uint8_t reg_addr,
			     uint16_t mask, uint16_t data)
{
	uint16_t read_val;
	int ret;

	ret = admv1014_spi_read(dev, reg_addr, &read_val);
	if (ret)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	return admv1014_spi_write(dev, reg_addr, read_val);
}

/**
 * @brief Set Digital Rx Detector
 * @param dev - The device structure.
 * @param det_prog - Digital Rx Detector gain.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1014_set_det_prog(struct admv1014_dev *dev,
			  enum admv1014_det_prog det_prog)
{
	return admv1014_spi_update_bits(dev, ADMV1014_REG_MIXER,
					ADMV1014_DET_PROG_MSK,
					no_os_field_prep(ADMV1014_DET_PROG_MSK, det_prog));
}

/**
 * @brief Get Digital Rx Detector
 * @param dev - The device structure.
 * @param det_prog - Digital Rx Detector gain.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1014_get_det_prog(struct admv1014_dev *dev,
			  enum admv1014_det_prog *det_prog)
{
	uint16_t data;
	int ret;

	ret = admv1014_spi_read(dev, ADMV1014_REG_MIXER, &data);
	if (ret)
		return ret;

	*det_prog = no_os_field_get(ADMV1014_DET_PROG_MSK, data);

	return 0;
}

/**
 * @brief Set Baseband Amp Gain
 * @param dev - The device structure.
 * @param gain - Digital Rx Detector gain.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1014_set_bb_amp_gain(struct admv1014_dev *dev, uint8_t gain)
{
	return admv1014_spi_update_bits(dev, ADMV1014_REG_BB_AMP_AGC,
					ADMV1014_BB_AMP_GAIN_CTRL_MSK,
					no_os_field_prep(ADMV1014_BB_AMP_GAIN_CTRL_MSK, gain));
}

/**
 * @brief Get Baseband Amp Gain
 * @param dev - The device structure.
 * @param gain - Digital Rx Detector gain.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1014_get_bb_amp_gain(struct admv1014_dev *dev, uint8_t *gain)
{
	uint16_t data;
	int ret;

	ret = admv1014_spi_read(dev, ADMV1014_REG_BB_AMP_AGC, &data);
	if (ret)
		return ret;

	*gain = no_os_field_get(ADMV1014_BB_AMP_GAIN_CTRL_MSK, data);

	return 0;
}

/**
 * @brief Set LO Amp Phase
 * @param dev - The device structure.
 * @param i_phase - The I phase adjust value.
 * @param q_phase - The Q phase adjust value.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1014_set_phase(struct admv1014_dev *dev, uint8_t i_phase,
		       uint8_t q_phase)
{
	return admv1014_spi_update_bits(dev, ADMV1014_REG_LO_AMP_PHASE_ADJUST1,
					ADMV1014_LOAMP_PH_ADJ_I_FINE_MSK |
					ADMV1014_LOAMP_PH_ADJ_Q_FINE_MSK,
					no_os_field_prep(ADMV1014_LOAMP_PH_ADJ_I_FINE_MSK, i_phase) |
					no_os_field_prep(ADMV1014_LOAMP_PH_ADJ_Q_FINE_MSK, q_phase));
}

/**
 * @brief Get LO Amp Phase
 * @param dev - The device structure.
 * @param i_phase - The I phase adjust value.
 * @param q_phase - The Q phase adjust value.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1014_get_phase(struct admv1014_dev *dev, uint8_t *i_phase,
		       uint8_t *q_phase)
{
	uint16_t data;
	int ret;

	ret = admv1014_spi_read(dev, ADMV1014_REG_LO_AMP_PHASE_ADJUST1, &data);
	if (ret)
		return ret;

	*i_phase = no_os_field_get(ADMV1014_LOAMP_PH_ADJ_I_FINE_MSK, data);
	*q_phase = no_os_field_get(ADMV1014_LOAMP_PH_ADJ_Q_FINE_MSK, data);

	return 0;
}

/**
 * @brief Set IF Amp Gain
 * @param dev - The device structure.
 * @param i_coarse_gain - The I coarse gain value.
 * @param q_coarse_gain - The I coarse gain value.
 * @param i_fine_gain - The Q fine gain value.
 * @param q_fine_gain - The Q fine gain value.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1014_set_if_amp_gain(struct admv1014_dev *dev, uint8_t i_coarse_gain,
			     uint8_t q_coarse_gain, uint8_t i_fine_gain, uint8_t q_fine_gain)

{
	int ret;

	ret = admv1014_spi_update_bits(dev, ADMV1014_REG_IF_AMP,
				       ADMV1014_IF_AMP_COARSE_GAIN_I_MSK |
				       ADMV1014_IF_AMP_FINE_GAIN_Q_MSK |
				       ADMV1014_IF_AMP_FINE_GAIN_I_MSK,
				       no_os_field_prep(ADMV1014_IF_AMP_COARSE_GAIN_I_MSK, i_coarse_gain) |
				       no_os_field_prep(ADMV1014_IF_AMP_FINE_GAIN_Q_MSK, q_fine_gain) |
				       no_os_field_prep(ADMV1014_IF_AMP_FINE_GAIN_I_MSK, i_fine_gain));
	if (ret)
		return ret;

	return admv1014_spi_update_bits(dev, ADMV1014_REG_IF_AMP_BB_AMP,
					ADMV1014_IF_AMP_COARSE_GAIN_Q_MSK,
					no_os_field_prep(ADMV1014_IF_AMP_COARSE_GAIN_Q_MSK, q_coarse_gain));
}

/**
 * @brief Get IF Amp Gain
 * @param dev - The device structure.
 * @param i_coarse_gain - The I coarse gain value.
 * @param q_coarse_gain - The I coarse gain value.
 * @param i_fine_gain - The Q fine gain value.
 * @param q_fine_gain - The Q fine gain value.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1014_get_if_amp_gain(struct admv1014_dev *dev, uint8_t *i_coarse_gain,
			     uint8_t *q_coarse_gain, uint8_t *i_fine_gain, uint8_t *q_fine_gain)
{
	uint16_t data;
	int ret;

	ret = admv1014_spi_read(dev, ADMV1014_REG_IF_AMP, &data);
	if (ret)
		return ret;

	*i_coarse_gain = no_os_field_get(ADMV1014_IF_AMP_COARSE_GAIN_I_MSK, data);
	*q_fine_gain = no_os_field_get(ADMV1014_IF_AMP_FINE_GAIN_Q_MSK, data);
	*i_fine_gain = no_os_field_get(ADMV1014_IF_AMP_FINE_GAIN_I_MSK, data);

	ret = admv1014_spi_read(dev, ADMV1014_REG_IF_AMP_BB_AMP, &data);
	if (ret)
		return ret;

	*i_coarse_gain = no_os_field_get(ADMV1014_IF_AMP_COARSE_GAIN_Q_MSK, data);

	return 0;
}

/**
 * @brief Update Quad Filters.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
static int admv1014_update_quad_filters(struct admv1014_dev *dev)
{
	unsigned int filt_raw;

	if ((dev->lo_in < 5400000000) || (dev->lo_in > 10250000000))
		return -EINVAL;

	if ((dev->lo_in >= 5400000000) && (dev->lo_in <= 7000000000))
		filt_raw = LO_BAND_5_4_TO_7_GHZ;
	else if ((dev->lo_in > 7000000000) && (dev->lo_in <= 8000000000))
		filt_raw = LO_BAND_5_4_TO_8_GHZ;
	else if ((dev->lo_in > 8000000000) && (dev->lo_in <= 9200000000))
		filt_raw = LO_BAND_6_6_TO_9_2_GHZ;
	else
		filt_raw = LO_BAND_8_62_TO_10_25_GHZ;

	return admv1014_spi_update_bits(dev, ADMV1014_REG_QUAD,
					ADMV1014_QUAD_FILTERS_MSK,
					no_os_field_prep(ADMV1014_QUAD_FILTERS_MSK, filt_raw));
}

/**
 * @brief Update Mixer Gate Voltage.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
static int admv1014_update_vcm_settings(struct admv1014_dev *dev)
{
	unsigned int i, vcm_comp, bb_sw_high_low_cm;
	int ret;

	/* See Datasheet Common-Mode Voltage Settings Table 6 */
	for (i = 0; i < NO_OS_ARRAY_SIZE(mixer_vgate_table); i++) {
		vcm_comp = 1050 + (i * 50) + (i / 8 * 50);
		if (dev->vcm_mv == vcm_comp) {
			ret = admv1014_spi_update_bits(dev, ADMV1014_REG_MIXER,
						       ADMV1014_MIXER_VGATE_MSK,
						       no_os_field_prep(ADMV1014_MIXER_VGATE_MSK,
								       mixer_vgate_table[i]));
			if (ret)
				return ret;

			bb_sw_high_low_cm = ~(i / 8);

			return admv1014_spi_update_bits(dev, ADMV1014_REG_BB_AMP_AGC,
							ADMV1014_BB_AMP_REF_GEN_MSK |
							ADMV1014_BB_SWITCH_HIGH_LOW_CM_MSK,
							no_os_field_prep(ADMV1014_BB_AMP_REF_GEN_MSK, i) |
							no_os_field_prep(ADMV1014_BB_SWITCH_HIGH_LOW_CM_MSK,
									bb_sw_high_low_cm));
		}
	}

	return -EINVAL;
}

/**
 * @brief Initializes the admv1014.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
int admv1014_init(struct admv1014_dev **device,
		  struct admv1014_init_param *init_param)
{
	struct admv1014_dev *dev;
	uint16_t chip_id, enable_reg, enable_reg_msk;
	int ret;

	dev = (struct admv1014_dev *)no_os_calloc(1, sizeof(*dev));
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
	dev->vcm_mv = init_param->vcm_mv;
	dev->p1db_comp_en = init_param->p1db_comp_en;

	/* Perform a software reset */
	ret = admv1014_spi_update_bits(dev, ADMV1014_REG_SPI_CONTROL,
				       ADMV1014_SPI_SOFT_RESET_MSK,
				       no_os_field_prep(ADMV1014_SPI_SOFT_RESET_MSK, 1));
	if (ret)
		goto error_spi;

	ret = admv1014_spi_update_bits(dev, ADMV1014_REG_SPI_CONTROL,
				       ADMV1014_SPI_SOFT_RESET_MSK,
				       no_os_field_prep(ADMV1014_SPI_SOFT_RESET_MSK, 0));
	if (ret)
		goto error_spi;

	ret = admv1014_spi_write(dev, ADMV1014_REG_VVA_TEMP_COMP, 0x727C);
	if (ret)
		goto error_spi;

	ret = admv1014_spi_read(dev, ADMV1014_REG_SPI_CONTROL, &chip_id);
	if (ret)
		goto error_spi;

	chip_id = no_os_field_get(ADMV1014_CHIP_ID_MSK, chip_id);
	if (chip_id != ADMV1014_CHIP_ID) {
		ret = -EINVAL;
		goto error_spi;
	}

	ret = admv1014_spi_update_bits(dev, ADMV1014_REG_QUAD,
				       ADMV1014_QUAD_SE_MODE_MSK,
				       no_os_field_prep(ADMV1014_QUAD_SE_MODE_MSK,
						       dev->quad_se_mode));
	if (ret)
		goto error_spi;

	ret = admv1014_update_quad_filters(dev);
	if (ret)
		goto error_spi;

	ret = admv1014_update_vcm_settings(dev);
	if (ret)
		goto error_spi;

	enable_reg_msk = ADMV1014_P1DB_COMPENSATION_MSK |
			 ADMV1014_IF_AMP_PD_MSK |
			 ADMV1014_BB_AMP_PD_MSK |
			 ADMV1014_DET_EN_MSK;

	enable_reg = no_os_field_prep(ADMV1014_P1DB_COMPENSATION_MSK,
				      dev->p1db_comp_en ? 3 : 0) |
		     no_os_field_prep(ADMV1014_IF_AMP_PD_MSK, !(dev->input_mode)) |
		     no_os_field_prep(ADMV1014_BB_AMP_PD_MSK, dev->input_mode) |
		     no_os_field_prep(ADMV1014_DET_EN_MSK, dev->det_en);

	ret = admv1014_spi_update_bits(dev, ADMV1014_REG_ENABLE, enable_reg_msk,
				       enable_reg);
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
 * @brief ADMV1014 Resources Deallocation.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv1014_remove(struct admv1014_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
