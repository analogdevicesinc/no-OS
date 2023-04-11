/***************************************************************************//**
 *   @file   ad77681.c
 *   @brief  Implementation of AD7768-1 Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include <string.h>
#include "ad77681.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * Compute CRC8 checksum.
 * @param data - The data buffer.
 * @param data_size - The size of the data buffer.
 * @param init_val - CRC initial value.
 * @return CRC8 checksum.
 */
uint8_t ad77681_compute_crc8(uint8_t *data,
			     uint8_t data_size,
			     uint8_t init_val)
{
	uint8_t i;
	uint8_t crc = init_val;

	while (data_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			if (((crc & 0x80) != 0) != ((*data & i) != 0)) {
				crc <<= 1;
				crc ^= AD77681_CRC8_POLY;
			} else
				crc <<= 1;
		}
		data++;
		data_size--;
	}
	return crc;
}

/**
 * Compute XOR checksum.
 * @param data - The data buffer.
 * @param data_size - The size of the data buffer.
 * @param init_val - CRC initial value.
 * @return XOR checksum.
 */
uint8_t ad77681_compute_xor(uint8_t *data,
			    uint8_t data_size,
			    uint8_t init_val)
{
	uint8_t crc = init_val;
	uint8_t buf[3];
	uint8_t i;

	for (i = 0; i < data_size; i++) {
		buf[i] = *data;
		crc ^= buf[i];
		data++;
	}
	return crc;
}

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_reg_read(struct ad77681_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data)
{
	int32_t ret;
	uint8_t crc;
	uint8_t buf[3], crc_buf[2];
	uint8_t buf_len = (dev->crc_sel == AD77681_NO_CRC) ? 2 : 3;

	buf[0] = AD77681_REG_READ(reg_addr);
	buf[1] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, buf_len);
	if (ret < 0)
		return ret;

	/* XOR or CRC checksum for read transactions */
	if (dev->crc_sel != AD77681_NO_CRC) {
		crc_buf[0] = AD77681_REG_READ(reg_addr);
		crc_buf[1] = buf[1];

		if (dev->crc_sel == AD77681_XOR)
			/* INITIAL_CRC is 0, when ADC is not in continuous-read mode */
			crc = ad77681_compute_xor(crc_buf, 2, INITIAL_CRC);
		else if(dev->crc_sel == AD77681_CRC)
			/* INITIAL_CRC is 0, when ADC is not in continuous-read mode */
			crc = ad77681_compute_crc8(crc_buf, 2, INITIAL_CRC);

		/* In buf[2] is CRC from the ADC */
		if (crc != buf[2])
			ret = -1;
#ifdef CRC_DEBUG
		printf("\n%x\t%x\tCRC/XOR: %s\n", crc,
		       buf[2], ((crc !=  buf[2]) ? "FAULT" : "OK"));
#endif /* CRC_DEBUG */
	}

	reg_data[0] = AD77681_REG_READ(reg_addr);
	memcpy(reg_data + 1, buf + 1, sizeof(buf) - 1);

	return ret;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_reg_write(struct ad77681_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data)
{
	uint8_t buf[3];
	/* Buffer length in case of checksum usage */
	uint8_t buf_len = (dev->crc_sel == AD77681_NO_CRC) ? 2 : 3;

	buf[0] = AD77681_REG_WRITE(reg_addr);
	buf[1] = reg_data;

	/* CRC only for read transactions, CRC and XOR for write transactions*/
	if (dev->crc_sel != AD77681_NO_CRC)
		buf[2] = ad77681_compute_crc8(buf, 2, INITIAL_CRC);

	return no_os_spi_write_and_read(dev->spi_desc, buf, buf_len);
}

/**
 * SPI read from device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_read_mask(struct ad77681_dev *dev,
			      uint8_t reg_addr,
			      uint8_t mask,
			      uint8_t *data)
{
	uint8_t reg_data[3];
	int32_t ret;

	ret = ad77681_spi_reg_read(dev, reg_addr, reg_data);
	*data = (reg_data[1] & mask);

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
int32_t ad77681_spi_write_mask(struct ad77681_dev *dev,
			       uint8_t reg_addr,
			       uint8_t mask,
			       uint8_t data)
{
	uint8_t reg_data[3];
	int32_t ret;

	ret = ad77681_spi_reg_read(dev, reg_addr, reg_data);
	reg_data[1] &= ~mask;
	reg_data[1] |= data;
	ret |= ad77681_spi_reg_write(dev, reg_addr, reg_data[1]);

	return ret;
}

/**
 * Helper function to get the number of rx bytes
 * @param dev - The device structure.
 * @return rx_buf_len - the number of rx bytes
 */
uint8_t ad77681_get_rx_buf_len(struct ad77681_dev *dev)
{
	uint8_t rx_buf_len = 0;
	uint8_t data_len = 0;
	uint8_t crc = 0;
	uint8_t status_bit = 0;

	data_len = 3;
	crc = (dev->crc_sel == AD77681_NO_CRC) ? 0 : 1; // 1 byte for crc
	status_bit = dev->status_bit; // one byte for status

	rx_buf_len = data_len + crc + status_bit;

	return rx_buf_len;
}

/**
 * Helper function to get the number of SPI 16bit frames for INTERRUPT ADC DATA READ
 * @param dev - The device structure.
 * @return frame_16bit - the number of 16 bit SPI frames
 */
uint8_t ad77681_get_frame_byte(struct ad77681_dev *dev)
{
	/* number of 8bit frames */
	uint8_t frame_bytes;
	if (dev->conv_len == AD77681_CONV_24BIT)
		frame_bytes = 3;
	else
		frame_bytes = 2;
	if (dev->crc_sel != AD77681_NO_CRC)
		frame_bytes++;
	if (dev->status_bit)
		frame_bytes++;

	dev->data_frame_byte = frame_bytes;

	return frame_bytes;
}

/**
 * Read conversion result from device.
 * @param dev - The device structure.
 * @param adc_data - The conversion result data
 * @param mode - Data read mode
 * 		Accepted values: AD77681_REGISTER_DATA_READ
 *				 AD77681_CONTINUOUS_DATA_READ
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_read_adc_data(struct ad77681_dev *dev,
				  uint8_t *adc_data,
				  enum ad77681_data_read_mode mode)
{
	uint8_t buf[6], crc_xor, add_buff;
	int32_t ret;

	if (mode == AD77681_REGISTER_DATA_READ) {
		buf[0] = AD77681_REG_READ(AD77681_REG_ADC_DATA);
		add_buff = 1;
	} else {
		buf[0] = 0x00;
		add_buff = 0;
	}
	buf[1] = 0x00; /* added 2 more array places for max data length read */
	buf[2] = 0x00; /* For register data read */
	buf[3] = 0x00; /* register address + 3 bytes of data (24bit format) + Status bit + CRC */
	buf[4] = 0x00; /* For continuous data read */
	buf[5] = 0x00; /* 3 bytes of data (24bit format) + Status bit + CRC */


	ret = no_os_spi_write_and_read(dev->spi_desc, buf,
				       dev->data_frame_byte + add_buff);
	if (ret < 0)
		return ret;

	if (dev->crc_sel != AD77681_NO_CRC) {
		if (dev->crc_sel == AD77681_CRC)
			crc_xor = ad77681_compute_crc8(buf + add_buff, dev->data_frame_byte - 1,
						       INITIAL_CRC_CRC8);
		else
			crc_xor = ad77681_compute_xor(buf + add_buff, dev->data_frame_byte - 1,
						      INITIAL_CRC_XOR);

		if (crc_xor != buf[dev->data_frame_byte - (1 - add_buff)]) {
			printf("%s: CRC Error.\n", __func__);
			ret = -1;
		}
#ifdef CRC_DEBUG
		printf("\n%x\t%x\tCRC/XOR: %s\n", crc_xor,
		       buf[dev->data_frame_byte - (1 - add_buff)],
		       ((crc_xor != buf[dev->data_frame_byte - (1 - add_buff)]) ? "FAULT" : "OK"));
#endif /* CRC_DEBUG */
	}

	/* Fill the adc_data buffer */
	memcpy(adc_data, buf, sizeof(buf));

	return ret;
}

/**
 * CRC and status bit handling after each readout form the ADC
 * @param dev - The device structure.
 * @param *data_buffer - 16-bit buffer readed from the ADC containing the CRC,
 * data and the stattus bit.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_CRC_status_handling(struct ad77681_dev *dev,
				    uint16_t *data_buffer)
{
	int32_t ret = 0;
	uint8_t status_byte = 0, checksum = 0, checksum_byte = 0, checksum_buf[5],
		checksum_length = 0, i;
	char print_buf[50];

	/* Status bit handling */
	if (dev->status_bit) {
		/* 24bit ADC data + 8bit of status = 2 16bit frames */
		if (dev->conv_len == AD77681_CONV_24BIT)
			status_byte = data_buffer[1] & 0xFF;
		/* 16bit ADC data + 8bit of status = 2 16bit frames */
		else
			status_byte = data_buffer[1] >> 8;
	}

	/* Checksum bit handling */
	if (dev->crc_sel != AD77681_NO_CRC) {
		if ((dev->status_bit == true) & (dev->conv_len == AD77681_CONV_24BIT)) {
			/* 24bit ADC data + 8bit of status + 8bit of CRC = 3 16bit frames */
			checksum_byte = data_buffer[2] >> 8;
			checksum_length = 4;
		} else if ((dev->status_bit == true) & (dev->conv_len == AD77681_CONV_16BIT)) {
			/* 16bit ADC data + 8bit of status + 8bit of CRC = 2 16bit frames */
			checksum_byte = data_buffer[1] & 0xFF;
			checksum_length = 3;
		} else if ((dev->status_bit == false) & (dev->conv_len == AD77681_CONV_24BIT)) {
			/* 24bit ADC data + 8bit of CRC = 2 16bit frames */
			checksum_byte = data_buffer[1] & 0xFF;
			checksum_length = 3;
		} else if ((dev->status_bit == false) & (dev->conv_len == AD77681_CONV_16BIT)) {
			/* 16bit ADC data + 8bit of CRC = 2 16bit frames */
			checksum_byte = data_buffer[1] >> 8;
			checksum_length = 2;
		}

		for (i = 0; i < checksum_length; i++) {
			if (i % 2)
				checksum_buf[i] = data_buffer[i / 2] & 0xFF;
			else
				checksum_buf[i] = data_buffer[i / 2] >> 8;
		}

		if (dev->crc_sel == AD77681_CRC)
			checksum = ad77681_compute_crc8(checksum_buf, checksum_length,
							INITIAL_CRC_CRC8);
		else if (dev->crc_sel == AD77681_XOR)
			checksum = ad77681_compute_xor(checksum_buf, checksum_length, INITIAL_CRC_XOR);

		if (checksum != checksum_byte)
			ret = -1;

#ifdef CRC_DEBUG

		char ok[3] = { 'O', 'K' }, fault[6] = { 'F', 'A', 'U', 'L', 'T' };
		sprintf(print_buf, "\n%x\t%x\t%x\tCRC %s", checksum_byte, checksum, status_byte,
			((ret == -1) ? (fault) : (ok)));
		printf(print_buf);

#endif /* CRC_DEBUG */
	}

	return ret;
}

/**
 * Conversion from measured data to voltage
 * @param dev - The device structure.
 * @param raw_code - ADC raw code measurements
 * @param voltage - Converted ADC code to voltage
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_data_to_voltage(struct ad77681_dev *dev,
				uint32_t *raw_code,
				double *voltage)
{
	int32_t converted_data;

	if (*raw_code & 0x800000)
		converted_data = (int32_t)((0xFFU << 24) | *raw_code);
	else
		converted_data = (int32_t)((0x00 << 24) | *raw_code);

	/* ((2*Vref)*code)/2^24	*/
	*voltage = (double)(((2.0 * (((double)(dev->vref)) / 1000.0)) /
			     AD7768_FULL_SCALE) * converted_data);

	return 0;
}

/**
 * Update ADCs sample rate depending on MCLK, MCLK_DIV and filter settings
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_update_sample_rate(struct ad77681_dev *dev)
{
	uint8_t mclk_div;
	uint16_t osr;

	/* Finding out MCLK divider */
	switch (dev->mclk_div) {
	case AD77681_MCLK_DIV_16:
		mclk_div = 16;
		break;
	case AD77681_MCLK_DIV_8:
		mclk_div = 8;
		break;
	case AD77681_MCLK_DIV_4:
		mclk_div = 4;
		break;
	case AD77681_MCLK_DIV_2:
		mclk_div = 2;
		break;
	default:
		return -1;
	}

	/* Finding out decimation ratio */
	switch (dev->filter) {
	case (AD77681_SINC5 | AD77681_FIR):
		/* Decimation ratio of FIR or SINC5 (x32 to x1024) */
		switch (dev->decimate) {
		case AD77681_SINC5_FIR_DECx32:
			osr = 32;
			break;
		case AD77681_SINC5_FIR_DECx64:
			osr = 64;
			break;
		case AD77681_SINC5_FIR_DECx128:
			osr = 128;
			break;
		case AD77681_SINC5_FIR_DECx256:
			osr = 256;
			break;
		case AD77681_SINC5_FIR_DECx512:
			osr = 512;
			break;
		case AD77681_SINC5_FIR_DECx1024:
			osr = 1024;
			break;
		default:
			return -1;
			break;
		}
		break;
	/* Decimation ratio of SINC5 x8 */
	case AD77681_SINC5_DECx8:
		osr = 8;
		break;
	/* Decimation ratio of SINC5 x16 */
	case AD77681_SINC5_DECx16:
		osr = 16;
		break;
	/* Decimation ratio of SINC3 */
	case AD77681_SINC3:
		osr = (dev->sinc3_osr + 1) * 32;
		break;
	default:
		return -1;
	}

	/* Sample rate to Hz */
	dev->sample_rate = (dev->mclk / (osr*mclk_div)) * 1000;

	return 0;
}

/**
 * Get SINC3 filter oversampling ratio register value based on user's inserted
 * output data rate ODR
 * @param dev - The device structure.
 * @param sinc3_dec_reg - Returned closest value of SINC3 register
 * @param sinc3_odr - Desired output data rage
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_SINC3_ODR(struct ad77681_dev *dev,
			  uint16_t *sinc3_dec_reg,
			  float sinc3_odr)
{
	uint8_t mclk_div;
	float	odr;

	if (sinc3_odr < 0)
		return -1;

	switch (dev->mclk_div) {
	case AD77681_MCLK_DIV_16:
		mclk_div = 16;
		break;
	case AD77681_MCLK_DIV_8:
		mclk_div = 8;
		break;
	case AD77681_MCLK_DIV_4:
		mclk_div = 4;
		break;
	case AD77681_MCLK_DIV_2:
		mclk_div = 2;
		break;
	default:
		return -1;
	}

	odr = ((float)(dev->mclk * 1000.0) / (sinc3_odr * (float)(32 * mclk_div))) - 1;

	/* Sinc3 oversamplig register has 13 bits, biggest value = 8192 */
	if (odr < 8193)
		*sinc3_dec_reg = (uint16_t)(odr);
	else
		return -1;

	return 0;
}

/**
 * Set the power consumption mode of the ADC core.
 * @param dev - The device structure.
 * @param mode - The power mode.
 * 					Accepted values: AD77681_ECO
 *									 AD77681_MEDIAN
 *									 AD77681_FAST
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_power_mode(struct ad77681_dev *dev,
			       enum ad77681_power_mode mode)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_POWER_CLOCK,
				     AD77681_POWER_CLK_PWRMODE_MSK,
				     AD77681_POWER_CLK_PWRMODE(mode));

	if (ret == 0)
		dev->power_mode = mode;

	return ret;
}

/**
 * Set the MCLK divider.
 * @param dev - The device structure.
 * @param clk_div - The MCLK divider.
 * 					Accepted values: AD77681_MCLK_DIV_16
 *									 AD77681_MCLK_DIV_8
 *									 AD77681_MCLK_DIV_4
 *									 AD77681_MCLK_DIV_2
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_mclk_div(struct ad77681_dev *dev,
			     enum ad77681_mclk_div clk_div)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_POWER_CLOCK,
				     AD77681_POWER_CLK_MCLK_DIV_MSK,
				     AD77681_POWER_CLK_MCLK_DIV(clk_div));

	if (ret == 0)
		dev->mclk_div = clk_div;

	return ret;
}

/**
 * Set the VCM output.
 * @param dev - The device structure.
 * @param VCM_out - The VCM output voltage.
 * 			Accepted values: AD77681_VCM_HALF_VCC
 *					 AD77681_VCM_2_5V
 *	  				 AD77681_VCM_2_05V
 *					 AD77681_VCM_1_9V
 *					 AD77681_VCM_1_65V
 *					 AD77681_VCM_1_1V
 *					 AD77681_VCM_0_9V
 *					 AD77681_VCM_OFF
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_VCM_output(struct ad77681_dev *dev,
			       enum ad77681_VCM_out VCM_out)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_ANALOG2,
				     AD77681_ANALOG2_VCM_MSK,
				     AD77681_ANALOG2_VCM(VCM_out));

	if (ret == 0)
		dev->VCM_out = VCM_out;

	return ret;
}

/**
 * Set the AIN- precharge buffer.
 * @param dev - The device structure.
 * @param AINn - The negative analog input precharge buffer selector
 * 		 Accepted values: AD77681_AINn_ENABLED
 *				  AD77681_AINn_DISABLED
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_AINn_buffer(struct ad77681_dev *dev,
				enum ad77681_AINn_precharge AINn)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_ANALOG,
				     AD77681_ANALOG_AIN_BUF_NEG_OFF_MSK,
				     AD77681_ANALOG_AIN_BUF_NEG_OFF(AINn));

	if (ret == 0)
		dev->AINn = AINn;

	return ret;
}

/**
 * Set the AIN+ precharge buffer.
 * @param dev - The device structure.
 * @param AINp - The positive analog input precharge buffer selector
 * 		 Accepted values: AD77681_AINp_ENABLED
 *				  AD77681_AINp_DISABLED
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_AINp_buffer(struct ad77681_dev *dev,
				enum ad77681_AINp_precharge AINp)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_ANALOG,
				     AD77681_ANALOG_AIN_BUF_POS_OFF_MSK,
				     AD77681_ANALOG_AIN_BUF_POS_OFF(AINp));

	if (ret == 0)
		dev->AINp = AINp;

	return ret;
}

/**
 * Set the REF- reference buffer
 * @param dev - The device structure.
 * @param REFn - The negative reference buffer selector
 * 		 Accepted values: AD77681_BUFn_DISABLED
 *				  AD77681_BUFn_ENABLED
 *				  AD77681_BUFn_FULL_BUFFER_ON
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_REFn_buffer(struct ad77681_dev *dev,
				enum ad77681_REFn_buffer REFn)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_ANALOG,
				     AD77681_ANALOG_REF_BUF_NEG_MSK,
				     AD77681_ANALOG_REF_BUF_NEG(REFn));

	if (ret == 0)
		dev->REFn = REFn;

	return ret;
}

/**
 * Set the REF+ reference buffer
 * @param dev -  The device structure.
 * @param REFp - The positive reference buffer selector
 * 		 Accepted values: AD77681_BUFp_DISABLED
 *				  AD77681_BUFp_ENABLED
 *				  AD77681_BUFp_FULL_BUFFER_ON
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_REFp_buffer(struct ad77681_dev *dev,
				enum ad77681_REFp_buffer REFp)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_ANALOG,
				     AD77681_ANALOG_REF_BUF_POS_MSK,
				     AD77681_ANALOG_REF_BUF_POS(REFp));

	if (ret == 0)
		dev->REFp = REFp;
	else
		return -1;

	return ret;
}

/**
 * Set filter type and decimation ratio
 * @param dev -	The device structure.
 * @param decimate - Decimation ratio of filter
 *			Accepted values: AD77681_SINC5_FIR_DECx32
 *					 AD77681_SINC5_FIR_DECx64
 *					 AD77681_SINC5_FIR_DECx128
 *					 AD77681_SINC5_FIR_DECx256
 *					 AD77681_SINC5_FIR_DECx512
 *					 AD77681_SINC5_FIR_DECx1024
 * @param filter - Select filter type
 *			Accepted values: AD77681_SINC5
 *					 AD77681_SINC5_DECx8
 *					 AD77681_SINC5_DECx16
 *					 AD77681_SINC3
 *					 AD77681_FIR
 * @param sinc3_osr - Select decimation ratio for SINC3 filter separately as
 *			integer from 0 to 8192.
 *			See the AD7768-1 datasheet for more info
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_filter_type(struct ad77681_dev *dev,
				enum ad77681_sinc5_fir_decimate decimate,
				enum ad77681_filter_type filter,
				uint16_t sinc3_osr)
{
	int32_t ret;

	ret = ad77681_spi_reg_write(dev, AD77681_REG_DIGITAL_FILTER, 0x00);

	/* SINC5 for OSR 8x and 16x*/
	if ((filter == AD77681_SINC5_DECx8) || (filter == AD77681_SINC5_DECx16)) {
		ret |= ad77681_spi_write_mask(dev,
					      AD77681_REG_DIGITAL_FILTER,
					      AD77681_DIGI_FILTER_FILTER_MSK,
					      AD77681_DIGI_FILTER_FILTER(filter));
		/* SINC5 and FIR for osr 32x to 1024x */
	} else if ((filter == AD77681_SINC5) || (filter == AD77681_FIR)) {
		ret |= ad77681_spi_write_mask(dev,
					      AD77681_REG_DIGITAL_FILTER,
					      AD77681_DIGI_FILTER_FILTER_MSK,
					      AD77681_DIGI_FILTER_FILTER(filter));

		ret |= ad77681_spi_write_mask(dev,
					      AD77681_REG_DIGITAL_FILTER,
					      AD77681_DIGI_FILTER_DEC_RATE_MSK,
					      AD77681_DIGI_FILTER_DEC_RATE(decimate));
		/* SINC3*/
	} else {
		uint8_t sinc3_LSB = 0, sinc3_MSB = 0;

		sinc3_MSB = sinc3_osr >> 8;
		sinc3_LSB = sinc3_osr & 0x00FF;

		ret |= ad77681_spi_write_mask(dev,
					      AD77681_REG_DIGITAL_FILTER,
					      AD77681_DIGI_FILTER_FILTER_MSK,
					      AD77681_DIGI_FILTER_FILTER(filter));

		ret |= ad77681_spi_write_mask(dev,
					      AD77681_REG_SINC3_DEC_RATE_MSB,
					      AD77681_SINC3_DEC_RATE_MSB_MSK,
					      AD77681_SINC3_DEC_RATE_MSB(sinc3_MSB));

		ret |= ad77681_spi_write_mask(dev,
					      AD77681_REG_SINC3_DEC_RATE_LSB,
					      AD77681_SINC3_DEC_RATE_LSB_MSK,
					      AD77681_SINC3_DEC_RATE_LSB(sinc3_LSB));
	}

	if ( ret == 0) {
		dev->decimate = decimate;
		dev->filter = filter;
		/* Sync pulse after each filter change */
		ret |= ad77681_initiate_sync(dev);
	}

	return ret;
}

/**
 * Enable 50/60 Hz rejection
 * @param dev -	The device structure.
 * @param enable - The positive reference buffer selector
 * 		   Accepted values: true
 *				    false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_50HZ_rejection(struct ad77681_dev *dev,
				   uint8_t enable)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_DIGITAL_FILTER,
				     AD77681_DIGI_FILTER_60HZ_REJ_EN_MSK,
				     AD77681_DIGI_FILTER_60HZ_REJ_EN(enable));

	return ret;
}

/**
 * Set the REF- reference buffer
 * @param dev - The device structure.
 * @param continuous_enable - Continous read enable
 * 		 Accepted values: AD77681_CONTINUOUS_READ_ENABLE
 *				  AD77681_CONTINUOUS_READ_DISABLE
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_continuos_read(struct ad77681_dev *dev,
				   enum ad77681_continuous_read continuous_enable)
{
	int32_t ret;

	if (continuous_enable) {
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_INTERFACE_FORMAT,
					     AD77681_INTERFACE_CONT_READ_MSK,
					     AD77681_INTERFACE_CONT_READ_EN(continuous_enable));
	} else {
		/* To exit the continuous read mode, a key 0x6C must be
		written into the device over the SPI*/
		uint8_t end_key = EXIT_CONT_READ;
		ret = no_os_spi_write_and_read(dev->spi_desc, &end_key, 1);
	}

	return ret;
}

/**
 * Power down / power up the device
 * @param dev -	The device structure.
 * @param sleep_wake - Power down, or power up the ADC
 * 			Accepted values: AD77681_SLEEP
 *					 AD77681_WAKE
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_power_down(struct ad77681_dev *dev,
			   enum ad77681_sleep_wake sleep_wake)
{
	int32_t ret;

	if (sleep_wake == AD77681_SLEEP) {
		ret = ad77681_spi_reg_write(dev, AD77681_REG_POWER_CLOCK,
					    AD77681_POWER_CLK_POWER_DOWN);
	} else {
		/* Wake up the ADC over SPI, by sending a wake-up sequence:
		1 followed by 63 zeroes and CS hold low*/
		uint8_t wake_sequence[8] = { 0 };
		/* Insert '1' to the beginning of the wake_sequence*/
		wake_sequence[0] = 0x80;
		ret = no_os_spi_write_and_read(dev->spi_desc, wake_sequence,
					       sizeof(wake_sequence));
	}

	return ret;
}

/**
 * Conversion mode and source select
 * @param dev - The device structure.
 * @param conv_mode - Sets the conversion mode of the ADC
 * 					  Accepted values: AD77681_CONV_CONTINUOUS
 *									   AD77681_CONV_ONE_SHOT
 *									   AD77681_CONV_SINGLE
 *									   AD77681_CONV_PERIODIC
 * @param diag_mux_sel - Selects which signal to route through diagnostic mux
 * 					  Accepted values: AD77681_TEMP_SENSOR
 *									   AD77681_AIN_SHORT
 *									   AD77681_POSITIVE_FS
 *									   AD77681_NEGATIVE_FS
 * @param conv_diag_sel - Select the input for conversion as AIN or diagnostic mux
 * 					  Accepted values: true
 *									   false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_conv_mode(struct ad77681_dev *dev,
			      enum ad77681_conv_mode conv_mode,
			      enum ad77681_conv_diag_mux diag_mux_sel,
			      bool conv_diag_sel)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_CONVERSION,
				     AD77681_CONVERSION_MODE_MSK,
				     AD77681_CONVERSION_MODE(conv_mode));

	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_CONVERSION,
				      AD77681_CONVERSION_DIAG_MUX_MSK,
				      AD77681_CONVERSION_DIAG_MUX_SEL(diag_mux_sel));

	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_CONVERSION,
				      AD77681_CONVERSION_DIAG_SEL_MSK,
				      AD77681_CONVERSION_DIAG_SEL(conv_diag_sel));

	if (ret == 0) {
		dev->conv_mode = conv_mode;
		dev->diag_mux_sel = diag_mux_sel;
		dev->conv_diag_sel = conv_diag_sel;
	}

	return ret;
}

/**
 * Set the Conversion Result Output Length.
 * @param dev - The device structure.
 * @param conv_len - The MCLK divider.
 * 					Accepted values: AD77681_CONV_24BIT
 *									 AD77681_CONV_16BIT
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_convlen(struct ad77681_dev *dev,
			    enum ad77681_conv_len conv_len)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_INTERFACE_FORMAT,
				     AD77681_INTERFACE_CONVLEN_MSK,
				     AD77681_INTERFACE_CONVLEN(conv_len));

	if (ret == 0) {
		dev->conv_len = conv_len;
		ad77681_get_frame_byte(dev);
	}

	return ret;
}

/**
 * Activates CRC on all SPI transactions and
 * Selects CRC method as XOR or 8-bit polynomial
 * @param dev - The device structure.
 * @param crc_sel - The CRC type.
 * 					Accepted values: AD77681_CRC
 *									 AD77681_XOR
 *									 AD77681_NO_CRC
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_crc_sel(struct ad77681_dev *dev,
			    enum ad77681_crc_sel crc_sel)
{
	int32_t ret;

	if (crc_sel == AD77681_NO_CRC) {
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_INTERFACE_FORMAT,
					     AD77681_INTERFACE_CRC_EN_MSK,
					     AD77681_INTERFACE_CRC_EN(0));
	} else {
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_INTERFACE_FORMAT,
					     AD77681_INTERFACE_CRC_EN_MSK,
					     AD77681_INTERFACE_CRC_EN(1));

		ret |= ad77681_spi_write_mask(dev,
					      AD77681_REG_INTERFACE_FORMAT,
					      AD77681_INTERFACE_CRC_TYPE_MSK,
					      AD77681_INTERFACE_CRC_TYPE(crc_sel));
	}

	if (ret == 0) {
		dev->crc_sel = crc_sel;
		ad77681_get_frame_byte(dev);
	}

	return ret;
}

/**
 * Enables Status bits output
 * @param dev - The device structure.
 * @param status_bit - enable or disable status bit
 * 					Accepted values: true
 *									 false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_status_bit(struct ad77681_dev *dev,
			       bool status_bit)
{
	int32_t ret;

	// Set status bit
	ret = 	ad77681_spi_write_mask(dev,
				       AD77681_REG_INTERFACE_FORMAT,
				       AD77681_INTERFACE_STATUS_EN_MSK,
				       AD77681_INTERFACE_STATUS_EN(status_bit));

	if (ret == 0) {
		dev->status_bit = status_bit;
		ad77681_get_frame_byte(dev);
	}

	return ret;
}

/**
 * Device reset over SPI.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_soft_reset(struct ad77681_dev *dev)
{
	int32_t ret = 0;

	// Two writes are required to initialize the reset
	ret |= 	ad77681_spi_write_mask(dev,
				       AD77681_REG_SYNC_RESET,
				       AD77681_SYNC_RST_SPI_RESET_MSK,
				       AD77681_SYNC_RST_SPI_RESET(0x3));

	ret |= 	ad77681_spi_write_mask(dev,
				       AD77681_REG_SYNC_RESET,
				       AD77681_SYNC_RST_SPI_RESET_MSK,
				       AD77681_SYNC_RST_SPI_RESET(0x2));

	return ret;
}

/**
 * Initiate a SYNC_OUT pulse over spi
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_initiate_sync(struct ad77681_dev *dev)
{
	return ad77681_spi_write_mask(dev,
				      AD77681_REG_SYNC_RESET,
				      AD77681_SYNC_RST_SPI_STARTB_MSK,
				      AD77681_SYNC_RST_SPI_STARTB(0));
}

/**
 * Write to offset registers
 * @param dev 			The device structure.
 * @param value			The desired value of the whole 24-bit offset register
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_apply_offset(struct ad77681_dev *dev,
			     uint32_t value)
{
	int32_t ret;
	uint8_t offset_HI = 0, offset_MID = 0, offset_LO = 0;

	offset_HI = (value & 0x00FF0000) >> 16;
	offset_MID = (value & 0x0000FF00) >> 8;
	offset_LO = (value & 0x000000FF);

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_OFFSET_HI,
				     AD77681_OFFSET_HI_MSK,
				     AD77681_OFFSET_HI(offset_HI));

	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_OFFSET_MID,
				      AD77681_OFFSET_MID_MSK,
				      AD77681_OFFSET_MID(offset_MID));

	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_OFFSET_LO,
				      AD77681_OFFSET_LO_MSK,
				      AD77681_OFFSET_LO(offset_LO));

	return ret;
}

/**
 * Write to gain registers
 * @param dev - The device structure.
 * @param value - The desired value of the whole 24-bit gain register
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_apply_gain(struct ad77681_dev *dev,
			   uint32_t value)
{
	int32_t ret;
	uint8_t gain_HI = 0, gain_MID = 0, gain_LO = 0;

	gain_HI = (value & 0x00FF0000) >> 16;
	gain_MID = (value & 0x0000FF00) >> 8;
	gain_LO = (value & 0x000000FF);

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_GAIN_HI,
				     AD77681_GAIN_HI_MSK,
				     AD77681_GAIN_HI(gain_HI));

	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_GAIN_MID,
				      AD77681_GAIN_MID_MSK,
				      AD77681_GAIN_MID(gain_MID));

	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_GAIN_LO,
				      AD77681_GAIN_LOW_MSK,
				      AD77681_GAIN_LOW(gain_LO));

	return ret;
}

/**
 * Upload sequence for Programmamble FIR filter
 * @param dev - The device structure.
 * @param coeffs - Pointer to the desired filter coefficients array to be written
 * @param num_coeffs - Count of active filter coeffs
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_programmable_filter(struct ad77681_dev *dev,
				    const float *coeffs,
				    uint8_t num_coeffs)
{
	uint8_t coeffs_buf[4], coeffs_index, check_back = 0, i, address;
	uint32_t twait;
	int32_t twos_complement, ret;
	const uint8_t coeff_reg_length = 56;

	/* Specific keys in the upload sequence */
	const uint8_t key1 = 0xAC, key2 = 0x45, key3 = 0x55;
	/* Scaling factor for all coefficients 2^22 */
	const float coeff_scale_factor = (1 << 22);
	/* Wait time in uS necessary to access the COEFF_CONTROL and */
	/* COEFF_DATA registers. Twait = 512/MCLK */
	twait = (uint32_t)(((512.0) / ((float)(dev->mclk))) * 1000.0) + 1;

	/* Set Filter to FIR */
	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_DIGITAL_FILTER,
				     AD77681_DIGI_FILTER_FILTER_MSK,
				     AD77681_DIGI_FILTER_FILTER(AD77681_FIR));

	/* Check return value before proceeding */
	if (ret < 0)
		return ret;

	/* Write the first access key to the ACCESS_KEY register */
	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_ACCESS_KEY,
				     AD77681_ACCESS_KEY_MSK,
				     AD77681_ACCESS_KEY(key1));

	/* Check return value before proceeding */
	if (ret < 0)
		return ret;

	/* Write the second access key to the ACCESS_KEY register */
	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_ACCESS_KEY,
				     AD77681_ACCESS_KEY_MSK,
				     AD77681_ACCESS_KEY(key2));

	/* Check return value before proceeding */
	if (ret < 0)
		return ret;

	/* Read the the ACCESS_KEY register bit 0, the key bit */
	ret = ad77681_spi_read_mask(dev,
				    AD77681_REG_ACCESS_KEY,
				    AD77681_ACCESS_KEY_CHECK_MSK,
				    &check_back);

	/* Checks ret and key bit, return -1 in case key bit not equal to 1 */
	if ((ret < 0) || (check_back != 1))
		return -1;

	/* Set the initial adress to 0 and enable the  write and coefficient access bits */
	address = AD77681_COEF_CONTROL_COEFFACCESSEN_MSK
		  | AD77681_COEF_CONTROL_COEFFWRITEEN_MSK;

	/* The COEFF_DATA register has to be filled with 56 coeffs.*/
	/* In case the number of active filter coefficient is less */
	/* than 56, zeros will be padded before the desired coeff. */
	for (i = 0; i < coeff_reg_length; i++) {
		/* Set the coeff address */
		ret = ad77681_spi_reg_write(dev,
					    AD77681_REG_COEFF_CONTROL,
					    address);

		/* Check return value before proceeding */
		if (ret < 0)
			return ret;

		/* Wait for Twait uSeconds*/
		no_os_udelay(twait);

		/* Padding of zeros before the desired coef in case the coef count in less than 56 */
		if((num_coeffs + i) < coeff_reg_length) {
			/* wirte zeroes to COEFF_DATA, in case of less coeffs than 56*/
			coeffs_buf[0] = AD77681_REG_WRITE(AD77681_REG_COEFF_DATA);
			coeffs_buf[1] = 0;
			coeffs_buf[2] = 0;
			coeffs_buf[3] = 0;
		} else {/* Writting of desired filter coefficients */
			/* Computes the index of coefficients to be uploaded */
			coeffs_index = (num_coeffs + i) - coeff_reg_length;
			/* Scaling the coefficient value and converting it to 2's complement */
			twos_complement = (int32_t)(coeffs[coeffs_index] * coeff_scale_factor);

			/* Write coefficients to COEFF_DATA */
			coeffs_buf[0] = AD77681_REG_WRITE(AD77681_REG_COEFF_DATA);
			coeffs_buf[1] = (twos_complement & 0xFF0000) >> 16;
			coeffs_buf[2] = (twos_complement & 0x00FF00) >> 8;
			coeffs_buf[3] = (twos_complement & 0x0000FF);
		}

		ret = no_os_spi_write_and_read(dev->spi_desc, coeffs_buf, 4);

		/* Check return value before proceeding */
		if (ret < 0)
			return ret;

		/* Increment the address*/
		address++;
		/* Wait for Twait uSeconds*/
		no_os_udelay(twait);
	}

	/* Disable coefficient write */
	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_COEFF_CONTROL,
				     AD77681_COEF_CONTROL_COEFFWRITEEN_MSK,
				     AD77681_COEF_CONTROL_COEFFWRITEEN(0x00));

	/* Check return value before proceeding */
	if (ret < 0)
		return ret;

	no_os_udelay(twait);

	/* Disable coefficient access */
	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_COEFF_CONTROL,
				     AD77681_COEF_CONTROL_COEFFACCESSEN_MSK,
				     AD77681_COEF_CONTROL_COEFFACCESSEN(0x00));

	/* Check return value before proceeding */
	if (ret < 0)
		return ret;

	/* Toggle the synchronization pulse and to begin reading data */
	/* Write 0x800000 to COEFF_DATA */
	coeffs_buf[0] = AD77681_REG_WRITE(AD77681_REG_COEFF_DATA);
	coeffs_buf[1] = 0x80;
	coeffs_buf[2] = 0x00;
	coeffs_buf[3] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, coeffs_buf, 4);

	/* Check return value before proceeding */
	if (ret < 0)
		return ret;

	/* Exit filter upload by wirting specific access key 0x55*/
	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_ACCESS_KEY,
				     AD77681_ACCESS_KEY_MSK,
				     AD77681_ACCESS_KEY(key3));

	/* Check return value before proceeding */
	if (ret < 0)
		return ret;

	/* Send synchronization pulse */
	ad77681_initiate_sync(dev);

	return ret;
}

/**
 * Read value from GPIOs present in AD7768-1 separately, or all GPIOS at once.
 * @param dev - The device structure.
 * @param value	- Readed value.
 * @param gpio_number - Number of GPIO, the value will be written into
 *			Accepted values: AD77681_GPIO0
 *					 AD77681_GPIO1
 *					 AD77681_GPIO2
 *					 AD77681_GPIO3
 *					 AD77681_ALL_GPIOS
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_gpio_read(struct ad77681_dev *dev,
			  uint8_t *value,
			  enum ad77681_gpios gpio_number)
{
	int32_t ret;

	switch (gpio_number) {
	case AD77681_GPIO0:	/* Read to GPIO0 */
		ret = ad77681_spi_read_mask(dev,
					    AD77681_REG_GPIO_READ,
					    AD77681_GPIO_READ_0_MSK,
					    value);
		break;
	case AD77681_GPIO1:	/* Read to GPIO1 */
		ret = ad77681_spi_read_mask(dev,
					    AD77681_REG_GPIO_READ,
					    AD77681_GPIO_READ_1_MSK,
					    value);
		break;
	case AD77681_GPIO2:	/* Read to GPIO2 */
		ret = ad77681_spi_read_mask(dev,
					    AD77681_REG_GPIO_READ,
					    AD77681_GPIO_READ_2_MSK,
					    value);
		break;
	case AD77681_GPIO3:	/* Read to GPIO3 */
		ret = ad77681_spi_read_mask(dev,
					    AD77681_REG_GPIO_READ,
					    AD77681_GPIO_READ_3_MSK,
					    value);
		break;
	case AD77681_ALL_GPIOS: /* Read to all GPIOs */
		ret = ad77681_spi_read_mask(dev,
					    AD77681_REG_GPIO_READ,
					    AD77681_GPIO_READ_ALL_MSK,
					    value);
		break;
	default:
		return -1;
	}

	return ret;
}

/**
 * Write value to GPIOs present in AD7768-1 separately, or all GPIOS at once.
 * @param dev - The device structure.
 * @param value - Value to be written into GPIO
 *		  Accepted values: NO_OS_GPIO_HIGH
 *				   NO_OS_GPIO_LOW
 *				   4-bit value for all gpios
 * @param gpio_number - Number of GPIO, the value will be written into
 *			Accepted values: AD77681_GPIO0
 *					 AD77681_GPIO1
 *					 AD77681_GPIO2
 *					 AD77681_GPIO3
 *					 AD77681_ALL_GPIOS
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_gpio_write(struct ad77681_dev *dev,
			   uint8_t value,
			   enum ad77681_gpios gpio_number)
{
	int32_t ret;

	switch (gpio_number) {
	case AD77681_GPIO0:	/* Write to GPIO0 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_WRITE,
					     AD77681_GPIO_WRITE_0_MSK,
					     AD77681_GPIO_WRITE_0(value));
		break;
	case AD77681_GPIO1:	/* Write to GPIO1 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_WRITE,
					     AD77681_GPIO_WRITE_1_MSK,
					     AD77681_GPIO_WRITE_1(value));
		break;
	case AD77681_GPIO2: /* Write to GPIO2 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_WRITE,
					     AD77681_GPIO_WRITE_2_MSK,
					     AD77681_GPIO_WRITE_2(value));
		break;
	case AD77681_GPIO3: /* Write to GPIO3 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_WRITE,
					     AD77681_GPIO_WRITE_3_MSK,
					     AD77681_GPIO_WRITE_3(value));
		break;
	case AD77681_ALL_GPIOS: /* Write to all GPIOs */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_WRITE,
					     AD77681_GPIO_WRITE_ALL_MSK,
					     AD77681_GPIO_WRITE_ALL(value));
		break;
	default:
		return -1;
	}

	return ret;
}

/**
 * Set AD7768-1s GPIO as input or output.
 * @param dev - The device structure.
 * @param direction - Direction of the GPIO
 *			Accepted values: NO_OS_GPIO_IN
 *					 NO_OS_GPIO_OUT
 *					 4-bit value for all gpios
 * @param gpio_number - Number of GPIO, which will be affected
 *			Accepted values: AD77681_GPIO0
 *					 AD77681_GPIO1
 *					 AD77681_GPIO2
 *					 AD77681_GPIO3
 *					 AD77681_ALL_GPIOS
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_gpio_inout(struct ad77681_dev *dev,
			   uint8_t direction,
			   enum ad77681_gpios gpio_number)
{
	int32_t ret;

	switch (gpio_number) {
	case AD77681_GPIO0:	/* Set direction of GPIO0 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_CONTROL,
					     AD77681_GPIO_CNTRL_GPIO0_OP_EN_MSK,
					     AD77681_GPIO_CNTRL_GPIO0_OP_EN(direction));
		break;
	case AD77681_GPIO1:	/* Set direction of GPIO1 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_CONTROL,
					     AD77681_GPIO_CNTRL_GPIO1_OP_EN_MSK,
					     AD77681_GPIO_CNTRL_GPIO1_OP_EN(direction));
		break;
	case AD77681_GPIO2:	/* Set direction of GPIO2 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_CONTROL,
					     AD77681_GPIO_CNTRL_GPIO2_OP_EN_MSK,
					     AD77681_GPIO_CNTRL_GPIO2_OP_EN(direction));
		break;
	case AD77681_GPIO3:	/* Set direction of GPIO3 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_CONTROL,
					     AD77681_GPIO_CNTRL_GPIO3_OP_EN_MSK,
					     AD77681_GPIO_CNTRL_GPIO3_OP_EN(direction));
		break;
	case AD77681_ALL_GPIOS:	/* Set direction of all GPIOs */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_CONTROL,
					     AD77681_GPIO_CNTRL_ALL_GPIOS_OP_EN_MSK,
					     AD77681_GPIO_CNTRL_ALL_GPIOS_OP_EN(direction));
		break;
	default:
		return -1;
	}

	return ret;
}

/**
 * Enable global GPIO bit.
 * This bit must be set high to change GPIO settings.
 * @param dev - The device structure.
 * @param gpio_enable - Enable or diable the global GPIO pin
 *			Accepted values: AD77681_GLOBAL_GPIO_ENABLE
 *					 AD77681_GLOBAL_GPIO_DISABLE
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_global_gpio(struct ad77681_dev *dev,
			    enum ad77681_gobal_gpio_enable gpio_enable)
{
	return ad77681_spi_write_mask(dev,
				      AD77681_REG_GPIO_CONTROL,
				      AD77681_GPIO_CNTRL_UGPIO_EN_MSK,
				      AD77681_GPIO_CNTRL_UGPIO_EN(gpio_enable));
}

/**
 * Read and write from ADC scratchpad register to check SPI Communication in
 * the very beginning, during inicialization.
 * @param dev - The device structure.
 * @param sequence - The sequence which will be written into scratchpad and the
 * 		      readed sequence will be returned
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_scratchpad(struct ad77681_dev *dev,
			   uint8_t *sequence)
{
	int32_t ret;
	const uint8_t check = *sequence;/* Save the original sequence */
	uint8_t ret_sequence = 0;/* Return sequence */

	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_SCRATCH_PAD,
				     AD77681_SCRATCHPAD_MSK,
				     AD77681_SCRATCHPAD(check));

	ret |= ad77681_spi_read_mask(dev,
				     AD77681_REG_SCRATCH_PAD,
				     AD77681_SCRATCHPAD_MSK,
				     &ret_sequence);

	if (check != ret_sequence)/* Compare original an returned sequence */
		return -1;

	return ret;
}

/**
 * Set AD7768-1s GPIO output type between strong driver and open drain.
 * GPIO3 can not be accessed!
 * @param dev - The device structure.
 * @param gpio_number - AD7768-1s GPIO to be affected (Only GPIO0, GPIO1 and GPIO2)
 *			Accepted values: AD77681_GPIO0
 *					 AD77681_GPIO1
 *					 AD77681_GPIO2
 *					 AD77681_ALL_GPIOS
 *
 * @param output_type - Output type of the GPIO
 *			Accepted values: AD77681_GPIO_STRONG_DRIVER
 *					 AD77681_GPIO_OPEN_DRAIN
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_gpio_open_drain(struct ad77681_dev *dev,
				enum ad77681_gpios gpio_number,
				enum ad77681_gpio_output_type output_type)
{
	int32_t ret;

	switch (gpio_number) {
	case AD77681_GPIO0: /* Set ouptut type of GPIO0 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_CONTROL,
					     AD77681_GPIO_CNTRL_GPIO0_OD_EN_MSK,
					     AD77681_GPIO_CNTRL_GPIO0_OD_EN(output_type));
		break;
	case AD77681_GPIO1: /* Set ouptut type of GPIO1 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_CONTROL,
					     AD77681_GPIO_CNTRL_GPIO1_OD_EN_MSK,
					     AD77681_GPIO_CNTRL_GPIO1_OD_EN(output_type));
		break;
	case AD77681_GPIO2: /* Set ouptut type of GPIO2 */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_CONTROL,
					     AD77681_GPIO_CNTRL_GPIO2_OD_EN_MSK,
					     AD77681_GPIO_CNTRL_GPIO2_OD_EN(output_type));
		break;
	case AD77681_ALL_GPIOS: /* Set ouptut type of all GPIOs */
		ret = ad77681_spi_write_mask(dev,
					     AD77681_REG_GPIO_CONTROL,
					     AD77681_GPIO_CNTRL_ALL_GPIOS_OD_EN_MSK,
					     AD77681_GPIO_CNTRL_ALL_GPIOS_OD_EN(output_type));
		break;
	default:
		return -1;
	}

	return ret;
}

/**
 * Clear all error flags at once
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_clear_error_flags(struct ad77681_dev *dev)
{
	int32_t ret;

	/* SPI ignore error CLEAR */
	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_SPI_DIAG_STATUS,
				     AD77681_SPI_IGNORE_ERROR_MSK,
				     AD77681_SPI_IGNORE_ERROR_CLR(ENABLE));
	/* SPI read error CLEAR */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_SPI_DIAG_STATUS,
				      AD77681_SPI_READ_ERROR_MSK,
				      AD77681_SPI_READ_ERROR_CLR(ENABLE));
	/* SPI write error CLEAR */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_SPI_DIAG_STATUS,
				      AD77681_SPI_WRITE_ERROR_MSK,
				      AD77681_SPI_WRITE_ERROR_CLR(ENABLE));
	/* SPI CRC error CLEAR */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_SPI_DIAG_STATUS,
				      AD77681_SPI_CRC_ERROR_MSK,
				      AD77681_SPI_CRC_ERROR_CLR(ENABLE));

	return ret;
}

/**
 * Enabling error flags. All error flags enabled by default
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_error_flags_enabe(struct ad77681_dev *dev)
{
	int32_t ret;

	/* SPI ERRORS ENABLE */
	/* SPI ignore error enable */
	ret = ad77681_spi_write_mask(dev,
				     AD77681_REG_SPI_DIAG_ENABLE,
				     AD77681_SPI_DIAG_ERR_SPI_IGNORE_MSK,
				     AD77681_SPI_DIAG_ERR_SPI_IGNORE(ENABLE));
	/*  SPI Clock count error enable */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_SPI_DIAG_ENABLE,
				      AD77681_SPI_DIAG_ERR_SPI_CLK_CNT_MSK,
				      AD77681_SPI_DIAG_ERR_SPI_CLK_CNT(ENABLE));
	/* SPI Read error enable */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_SPI_DIAG_ENABLE,
				      AD77681_SPI_DIAG_ERR_SPI_RD_MSK,
				      AD77681_SPI_DIAG_ERR_SPI_RD(ENABLE));
	/* SPI Write error enable */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_SPI_DIAG_ENABLE,
				      AD77681_SPI_DIAG_ERR_SPI_WR_MSK,
				      AD77681_SPI_DIAG_ERR_SPI_WR(ENABLE));

	/* ADC DIAG ERRORS ENABLE */
	/* DLDO PSM error enable */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_ADC_DIAG_ENABLE,
				      AD77681_ADC_DIAG_ERR_DLDO_PSM_MSK,
				      AD77681_ADC_DIAG_ERR_DLDO_PSM(ENABLE));
	/* ALDO PSM error enable */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_ADC_DIAG_ENABLE,
				      AD77681_ADC_DIAG_ERR_ALDO_PSM_MSK,
				      AD77681_ADC_DIAG_ERR_ALDO_PSM(ENABLE));
	/* Filter saturated error enable */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_ADC_DIAG_ENABLE,
				      AD77681_ADC_DIAG_ERR_FILT_SAT_MSK,
				      AD77681_ADC_DIAG_ERR_FILT_SAT(ENABLE));
	/* Filter not settled error enable */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_ADC_DIAG_ENABLE,
				      AD77681_ADC_DIAG_ERR_FILT_NOT_SET_MSK,
				      AD77681_ADC_DIAG_ERR_FILT_NOT_SET(ENABLE));
	/* External clock check error enable */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_ADC_DIAG_ENABLE,
				      AD77681_ADC_DIAG_ERR_EXT_CLK_QUAL_MSK,
				      AD77681_ADC_DIAG_ERR_EXT_CLK_QUAL(ENABLE));

	/* DIG DIAG ENABLE */
	/* Memory map CRC error enabled */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_DIG_DIAG_ENABLE,
				      AD77681_DIG_DIAG_ERR_MEMMAP_CRC_MSK,
				      AD77681_DIG_DIAG_ERR_MEMMAP_CRC(ENABLE));
	/* RAM CRC error enabled */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_DIG_DIAG_ENABLE,
				      AD77681_DIG_DIAG_ERR_RAM_CRC_MSK,
				      AD77681_DIG_DIAG_ERR_RAM_CRC(ENABLE));
	/* FUSE CRC error enabled */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_DIG_DIAG_ENABLE,
				      AD77681_DIG_DIAG_ERR_FUSE_CRC_MSK,
				      AD77681_DIG_DIAG_ERR_FUSE_CRC(ENABLE));
	/* Enable MCLK Counter */
	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_DIG_DIAG_ENABLE,
				      AD77681_DIG_DIAG_FREQ_COUNT_EN_MSK,
				      AD77681_DIG_DIAG_FREQ_COUNT_EN(ENABLE));

	return ret;
}

/**
 * Read from all ADC status registers
 * @param dev - The device structure.
 * @param status - Structure with all satuts bits
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_status(struct ad77681_dev *dev,
		       struct ad77681_status_registers *status)
{
	int32_t ret;
	uint8_t buf[3];

	/* Master status register */
	ret = ad77681_spi_reg_read(dev, AD77681_REG_MASTER_STATUS,buf);
	status->master_error = buf[1] & AD77681_MASTER_ERROR_MSK;
	status->adc_error = buf[1] & AD77681_MASTER_ADC_ERROR_MSK;
	status->dig_error = buf[1] & AD77681_MASTER_DIG_ERROR_MSK;
	status->adc_err_ext_clk_qual = buf[1] & AD77681_MASTER_DIG_ERR_EXT_CLK_MSK;
	status->adc_filt_saturated = buf[1] & AD77681_MASTER_FILT_SAT_MSK;
	status->adc_filt_not_settled = buf[1] & AD77681_MASTER_FILT_NOT_SET_MSK;
	status->spi_error = buf[1] & AD77681_MASTER_SPI_ERROR_MSK;
	status->por_flag = buf[1] & AD77681_MASTER_POR_FLAG_MSK;
	/* SPI diag status register */
	ret |= ad77681_spi_reg_read(dev, AD77681_REG_SPI_DIAG_STATUS, buf);
	status->spi_ignore = buf[1] & AD77681_SPI_IGNORE_ERROR_MSK;
	status->spi_clock_count = buf[1] & AD77681_SPI_CLK_CNT_ERROR_MSK;
	status->spi_read_error = buf[1] & AD77681_SPI_READ_ERROR_MSK;
	status->spi_write_error = buf[1] & AD77681_SPI_WRITE_ERROR_MSK;
	status->spi_crc_error = buf[1] & AD77681_SPI_CRC_ERROR_MSK;
	/* ADC diag status register */
	ret |= ad77681_spi_reg_read(dev, AD77681_REG_ADC_DIAG_STATUS,buf);
	status->dldo_psm_error = buf[1] & AD77681_ADC_DLDO_PSM_ERROR_MSK;
	status->aldo_psm_error = buf[1] & AD77681_ADC_ALDO_PSM_ERROR_MSK;
	status->ref_det_error = buf[1] & AD77681_ADC_REF_DET_ERROR_MSK;
	status->filt_sat_error = buf[1] & AD77681_ADC_FILT_SAT_MSK;
	status->filt_not_set_error = buf[1] & AD77681_ADC_FILT_NOT_SET_MSK;
	status->ext_clk_qual_error = buf[1] & AD77681_ADC_DIG_ERR_EXT_CLK_MSK;
	/* DIG diag status register */
	ret |= ad77681_spi_reg_read(dev, AD77681_REG_DIG_DIAG_STATUS,buf);
	status->memoy_map_crc_error = buf[1] & AD77681_DIG_MEMMAP_CRC_ERROR_MSK;
	status->ram_crc_error = buf[1] & AD77681_DIG_RAM_CRC_ERROR_MSK;
	status->fuse_crc_error = buf[1] & AD77681_DIG_FUS_CRC_ERROR_MSK;

	return ret;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @param status - The structure that will contains the ADC status
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_setup(struct ad77681_dev **device,
		      struct ad77681_init_param init_param,
		      struct ad77681_status_registers **status)
{
	struct ad77681_dev *dev;
	struct ad77681_status_registers *stat;
	int32_t ret;
	uint8_t scratchpad_check = 0xAD;

	dev = (struct ad77681_dev *)no_os_malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	stat = (struct ad77681_status_registers *)no_os_malloc(sizeof(*stat));
	if (!stat) {
		no_os_free(dev);
		return -1;
	}

	dev->power_mode = init_param.power_mode;
	dev->mclk_div = init_param.mclk_div;
	dev->conv_diag_sel = init_param.conv_diag_sel;
	dev->conv_mode = init_param.conv_mode;
	dev->diag_mux_sel = init_param.diag_mux_sel;
	dev->conv_len = init_param.conv_len;
	dev->crc_sel = AD77681_NO_CRC;
	dev->status_bit = init_param.status_bit;
	dev->VCM_out = init_param.VCM_out;
	dev->AINn = init_param.AINn;
	dev->AINp = init_param.AINp;
	dev->REFn = init_param.REFn;
	dev->REFp = init_param.REFp;
	dev->filter = init_param.filter;
	dev->decimate = init_param.decimate;
	dev->sinc3_osr = init_param.sinc3_osr;
	dev->vref = init_param.vref;
	dev->mclk = init_param.mclk;
	dev->sample_rate = init_param.sample_rate;
	dev->data_frame_byte = init_param.data_frame_byte;

	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_eng_dev_init);
	if (ret < 0) {
		no_os_free(dev);
		no_os_free(stat);
		return ret;
	}

	ret |= ad77681_soft_reset(dev);

	no_os_udelay(200);

	/* Check physical connection using scratchpad*/
	if (ad77681_scratchpad(dev, &scratchpad_check) == -1) {
		scratchpad_check = 0xAD;/* If failure, second try */
		ret |= (ad77681_scratchpad(dev, &scratchpad_check));
		if(ret == -1) {
			no_os_free(dev);
			no_os_free(stat);
			return ret;
		}
	}
	ret |= ad77681_set_power_mode(dev, dev->power_mode);
	ret |= ad77681_set_mclk_div(dev, dev->mclk_div);
	ret |= ad77681_set_conv_mode(dev,
				     dev->conv_mode,
				     dev->diag_mux_sel,
				     dev->conv_diag_sel);
	ret |= ad77681_set_convlen(dev, dev->conv_len);
	ret |= ad77681_set_status_bit(dev, dev->status_bit);
	ret |= ad77681_set_crc_sel(dev, init_param.crc_sel);
	ret |= ad77681_set_VCM_output(dev, dev->VCM_out);
	ret |= ad77681_set_AINn_buffer(dev, dev->AINn);
	ret |= ad77681_set_AINp_buffer(dev, dev->AINp);
	ret |= ad77681_set_REFn_buffer(dev, dev->REFn);
	ret |= ad77681_set_REFp_buffer(dev, dev->REFp);
	ret |= ad77681_set_filter_type(dev, dev->decimate, dev->filter, dev->sinc3_osr);
	ret |= ad77681_error_flags_enabe(dev);
	ret |= ad77681_clear_error_flags(dev);
	ret |= ad77681_status(dev, stat);
	ad77681_get_frame_byte(dev);
	ad77681_update_sample_rate(dev);
	*status = stat;
	*device = dev;

	if (!ret)
		printf("ad77681 successfully initialized\n");

	return ret;
}

