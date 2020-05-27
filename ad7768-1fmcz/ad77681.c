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
#include <stdint.h>
#include <string.h>
#include "ad77681.h"

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

	ret = spi_write_and_read(dev->spi_desc, buf, buf_len);
	if (ret < 0)
		return ret;

	if (dev->crc_sel != AD77681_NO_CRC)														// XOR or CRC checksum for read transactions
	{
		crc_buf[0] = AD77681_REG_READ(reg_addr);
		crc_buf[1] = buf[1];
		
		if (dev->crc_sel == AD77681_XOR)
			crc = ad77681_compute_xor(crc_buf, 2, INITIAL_CRC);								// INITIAL_CRC is 0, wen ADC is not in continuous-read mode
		
		else if(dev->crc_sel == AD77681_CRC)
			crc = ad77681_compute_crc8(crc_buf, 2, INITIAL_CRC);							// INITIAL_CRC is 0, wen ADC is not in continuous-read mode
		
		if (crc != buf[2])																	// In buf[2] is CRC from the ADC
			ret = -1;
	}
	
	memcpy(reg_data, buf, ARRAY_SIZE(buf));

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
	uint8_t buf_len = (dev->crc_sel == AD77681_NO_CRC) ? 2 : 3; 							// Buffer length in case of checksum usage

	buf[0] = AD77681_REG_WRITE(reg_addr);
	buf[1] = reg_data;
	
	if (dev->crc_sel != AD77681_NO_CRC)														// CRC only for read transactions, CRC and XOR for write transactions
		buf[2] = ad77681_compute_crc8(buf, 2, INITIAL_CRC);									// If you choose XOR as checksum, you must apply CRC to write transaction

	return spi_write_and_read(dev->spi_desc, buf, buf_len);
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
 * Helper function to get the number of SPI bytes for INTERRUPT ADC DATA READ
 * @param dev - The device structure.
 * @return frame_bytes - the number of rx bytes
 */
uint8_t ad77681_get_frame_bytes(struct ad77681_dev *dev)
{
	uint8_t frame_bytes = 0;																// number of 8bit frames
	
	if (dev->conv_len == AD77681_CONV_24BIT)
		frame_bytes += 3;
	else
		frame_bytes += 2;
	
	if (dev->crc_sel != AD77681_NO_CRC)
		frame_bytes++;
	
	if (dev->status_bit)
		frame_bytes++;
	
	if (frame_bytes % 2)																	// Conversion from number of 8bit frames to number of 16bit frames
		frame_bytes = (frame_bytes / 2) + 1;
	else
		frame_bytes /= 2;
	
	dev->data_frame_bytes = frame_bytes;
	
	return frame_bytes;
}

/**
 * This function reads the ADC data register by demand from the user in 8 bit frames and converts it to voltage
 * Read conversion result from device.
 * @param dev - The device structure.
 * @param adc_data - The conversion result data
 * @param voltage  - Measured data transfered to voltage
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_read_adc_data(struct ad77681_dev *dev,
				uint8_t *adc_data,
				double *voltage)
{
	uint8_t crc_calc_buf[4], buf[6], crc, frames_8byte = 0;
	int32_t ret, converted_data;
	double Vref;

	if (dev->conv_len == AD77681_CONV_24BIT)
		frames_8byte += 3;
	else
		frames_8byte += 2;
	
	if (dev->crc_sel != AD77681_NO_CRC)
		frames_8byte++;
	
	if (dev->status_bit)
		frames_8byte++;	

	buf[0] = AD77681_REG_READ(AD77681_REG_ADC_DATA);
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0x00;																				// added 2 more array places for max data length read 
	buf[5] = 0x00;																				// register address + 3 bytes of data (24bit format) + CRC + Status bit

	ret = spi_write_and_read(dev->spi_desc, buf, frames_8byte + 1);
	if (ret < 0)
		return ret;

	if (dev->crc_sel == AD77681_CRC) {
		memcpy(crc_calc_buf, buf, ARRAY_SIZE(buf));
		crc = ad77681_compute_crc8(crc_calc_buf, 4, INITIAL_CRC);
		if (crc != buf[3]) {
			printf("%s: CRC Error.\n", __func__);
			ret = -1;
		}
	}

	/* Fill the adc_data buffer */
	memcpy(adc_data, buf, ARRAY_SIZE(buf));
	
	if (buf[1] & 0x80)
		converted_data = (int32_t)(0xFF << 24 | (((buf[1]  &   0xFF)  << 16) |	
												 ((buf[2]  &   0xFF)  << 8) |	
												 ((buf[3]  &   0xFF)  << 0)));
	else
		converted_data = (int32_t)(0x00 << 24 | (((buf[1]  &   0xFF)  << 16) |	
												 ((buf[2]  &   0xFF)  << 8) |	
												 ((buf[3]  &   0xFF)  << 0)));
	
	Vref = ((double)(dev->vref)) / 1000.0;
	*voltage = (double)(((2.0 * Vref) / AD7768_FULL_SCALE ) * converted_data);					// ((2*Vref)*code)/2^24	
	
	if ((*voltage > Vref) || (*voltage < Vref * (-1.0)))										// Overflow protection, voltage can only fit between +- Vref
		ret = -1;

	return ret;
}

/**
 * This function uses continuous read mode, which has to be enabled in advance
 * Read conversion result from device using GPIO DRDY interrupt in 16bit frames
 *
 *	    /-------------\       /-------------\       /-------------\
 *------  24-bit data  -------  8-bit staus  -------   8-bit CRC   -------
 *      \-------------/       \-------------/       \-------------/
 *
 *                THIS FUNCTION USES 16bit SPI FRAMES
 *                
 * Firtst 24bits or 16bits of data	dev->conv_length   followed by
 * 8bits of status bit, if enabled	dev->sataus_bit    followed by
 * 8bits of CRC, if enabled			dev->crc_sel
 *
 * @param dev - The device structure.
 * @param measured_data - structure carying ADC measurement info
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_read_interrupt_adc_data(struct ad77681_dev *dev,
	struct adc_data *measured_data)
{
	int32_t ret;
	uint16_t buf[3];
	
	buf[0] = 0;																					// 16bit buffers! Used only for ADC data read
	buf[1] = 0;
	buf[2] = 0;
	
	ret = spi_read_data(dev->spi_desc, buf, dev->data_frame_bytes);
	if (ret < 0)
		return ret;
		
	if (dev->conv_len == AD77681_CONV_24BIT)													// 24bit format
		measured_data->raw_data[measured_data->count] = (buf[0] << 8 | buf[1] >> 8);			// Cuts first 8 bits from buf[1] and paste it after 16bits of buf[0]
	else
		measured_data->raw_data[measured_data->count] = (buf[0] << 8);							// 16bit format


	if(dev->status_bit || (dev->crc_sel != AD77681_NO_CRC))										// If status bit or CRC is enabled, handle it
		ret |= ad77681_CRC_status_handling(dev, buf);
	
	return ret;			
}

/**
 * CRC and status bit handling after each readout form the ADC
 * @param dev - The device structure.
 * @param *data_buffer - 16-bit buffer readed from the ADC containing the CRC, data and the stattus bit
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_CRC_status_handling(struct ad77681_dev *dev,
	uint16_t *data_buffer)
{
	int32_t ret = 0;
	uint8_t status_byte = 0, checksum, checksum_byte, checksum_buf[5], checksum_length, i;
	char print_buf[50];
	
	// Status bit handling
	if(dev->status_bit)																			// Status bit attached to SPI data frame
	{
		if (dev->conv_len == AD77681_CONV_24BIT)												// 24bit ADC data + 8bit of status = 2 16bit frames
			status_byte = data_buffer[1] & 0xFF;															
		else																					// 16bit ADC data + 8bit of status = 2 16bit frames
			status_byte = data_buffer[1] >> 8;															
	}
	
	// Chcecksum bit handling
	if(dev->crc_sel != AD77681_NO_CRC)
	{
		if (dev->status_bit)
		{
			if (dev->conv_len == AD77681_CONV_24BIT)											// 24bit ADC data + 8bit of status + 8bit of CRC = 3 16bit frames
				{
					checksum_byte = data_buffer[2] >> 8;
					checksum_length = 4;
				}		
			else																				// 16bit ADC data + 8bit of status + 8bit of CRC = 2 16bit frames
				{
					checksum_byte = data_buffer[1] & 0xFF;
					checksum_length = 3;
				}	
		}
		
		else																					// without status bit
			{
				if (dev->conv_len == AD77681_CONV_24BIT)											// 24bit ADC data + 8bit of CRC = 2 16bit frames
					{
						checksum_byte = data_buffer[1] & 0xFF;
						checksum_length = 3;
					}
				
				else																				// 16bit ADC data + 8bit of CRC = 2 16bit frames
					{
						checksum_byte = data_buffer[1] >> 8;
						checksum_length = 2;
					}	
			}
		
		for (i = 0; i < checksum_length; i++)
		{
			if (i % 2)
				checksum_buf[i] = data_buffer[i / 2] & 0xFF;
			else
				checksum_buf[i] = data_buffer[i / 2] >> 8;
		}
		
		if (dev->crc_sel == AD77681_CRC)
			checksum = ad77681_compute_crc8(checksum_buf, checksum_length, INITIAL_CRC_CRC8);
		
		else if (dev->crc_sel == AD77681_XOR)
			checksum = ad77681_compute_xor(checksum_buf, checksum_length, INITIAL_CRC_XOR);
		
		
		if (checksum != checksum_byte)	
			ret = -1;
		
#ifdef CRC_DEBUG
		
		char ok[3] = { 'O', 'K' }, fault[6] = { 'F', 'A', 'U', 'L', 'T' };
		sprintf(print_buf, "\n%x\t%x\t%x\tCRC %s", checksum_byte, checksum, status_byte, ((ret == -1) ? (fault) : (ok)));
		print_debug(print_buf);	
		
#endif // CRC_DEBUG
		
	}	
	
	return ret;
}

/**
 * Conversion from measured data to voltage
 * @param dev - The device structure.
 * @param measured_data - structure carying ADC measurement info
 * @param voltage		- Array of voltage values
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_data_to_voltage(struct ad77681_dev *dev,
	struct adc_data *measured_data,
	double *voltage)
{
	int32_t converted_data = 0;
	
	for (measured_data->count = 0; measured_data->count < measured_data->samples; measured_data->count++)
	{

		if (measured_data->raw_data[measured_data->count] & 0x800000)
			converted_data = (int32_t)((0xFF << 24) | measured_data->raw_data[measured_data->count]);
		else
			converted_data = (int32_t)((0x00 << 24) | measured_data->raw_data[measured_data->count]);
				
		voltage[measured_data->count] = (double)(((2.0 * (((double)(dev->vref)) / 1000.0)) / AD7768_FULL_SCALE)*converted_data); 	//       ((2*Vref)*code)/2^24	
	}
	return 0;	
}

/**
 * Update ADCs sample rate depending on MCLK, MCLK_DIV and filter settings
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_update_sample_rate(struct ad77681_dev *dev)
{
	uint32_t sample_rate;
	uint8_t mclk_div;
	uint16_t osr;
	
	// Finding out MCLK divide
	switch (dev->mclk_div)
	{
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
		break;
	}
	
	
	// Finding out decimation ratio
	switch (dev->filter)
	{
	case (AD77681_SINC5 | AD77681_FIR):
		
		// Decimation ratio of FIR or SINC5 (x32 to x1024)
		switch (dev->decimate)
		{
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
		
	// Decimation ratio of SINC5 x8
	case AD77681_SINC5_DECx8:
		osr = 8;
		break;
	
	// Decimation ratio of SINC5 x16
	case AD77681_SINC5_DECx16:
		osr = 16;
		break;

	// Decimation ratio of SINC3	
	case AD77681_SINC3:
		osr = (dev->sinc3_osr + 1) * 32;
		break;
		
	default:
		return -1;
		break;
	}
	
	dev->sample_rate = (dev->mclk / (osr*mclk_div)) * 1000;					// Sample rate to Hz
	
	return 0;
}


/**
 * Get SINC3 filter oversampling ratio register value based on user's inserted output data rate ODR
 * @param dev - The device structure.
 * @param sinc3_dec_reg - Returned closest value of SINC3 register
 * @param sinc3_osr	- Desired output data rage 
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_SINC3_ODR(struct ad77681_dev *dev, 
	uint16_t *sinc3_dec_reg, 
	float sinc3_odr)
{
	int32_t ret;
	uint8_t mclk_div;
	float	odr;
	
	if (sinc3_odr < 0)
		return -1;
	
	switch (dev->mclk_div)
	{
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
		break;
	}
	
	odr = ((float)(dev->mclk * 1000.0) / (sinc3_odr * (float)(32 * mclk_div))) - 1;
	
	if (odr < 8192)																			// Sinc3 oversamplig register has 13 bits, biggest value = 8192
		{
			*sinc3_dec_reg = (uint16_t)(odr);
			ret = 0;
		} 
	
	else
		ret = -1;
	
	return ret;
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
	int32_t ret = 0;

	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_POWER_CLOCK,
				      AD77681_POWER_CLK_PWRMODE_MSK,
				      AD77681_POWER_CLK_PWRMODE(mode));

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
	int32_t ret = 0;

	ret |= ad77681_spi_write_mask(dev,
			       AD77681_REG_POWER_CLOCK,
			       AD77681_POWER_CLK_MCLK_DIV_MSK,
			       AD77681_POWER_CLK_MCLK_DIV(clk_div));

	dev->mclk_div = clk_div;

	return ret;
}

/**
 * Set the VCM output.
 * @param dev - The device structure.
 * @param VCM_out - The VCM output voltage.
 * 					Accepted values: AD77681_VCM_HALF_VCC
 *									 AD77681_VCM_2_5V
	  								 AD77681_VCM_2_05V
									 AD77681_VCM_1_9V
									 AD77681_VCM_1_65V
									 AD77681_VCM_1_1V
									 AD77681_VCM_0_9V
									 AD77681_VCM_OFF
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

	dev->VCM_out = VCM_out;

	return ret;
}

/**
 * Set the AIN- precharge buffer.
 * @param dev - The device structure.
 * @param AINn - The negative analog input precharge buffer selector
 * 					Accepted values: AD77681_AINn_ENABLED
 *									 AD77681_AINn_DISABLED
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

	dev->AINn = AINn;

	return ret;
}

/**
 * Set the AIN+ precharge buffer.
 * @param dev - The device structure.
 * @param AINp - The positive analog input precharge buffer selector
 * 					Accepted values: AD77681_AINp_ENABLED
 *									 AD77681_AINp_DISABLED
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

	dev->AINp = AINp;

	return ret;
}

/**
 * Set the REF- reference buffer
 * @param dev - The device structure.
 * @param REFn - The negative reference buffer selector
 * 					Accepted values: AD77681_BUFn_DISABLED
 *									 AD77681_BUFn_ENABLED
 *									 AD77681_BUFn_FULL_BUFFER_ON
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

	dev->REFn = REFn;

	return ret;
}

/**
 * Set the REF+ reference buffer
 * @param dev -  The device structure.
 * @param REFp - The positive reference buffer selector
 * 					Accepted values: AD77681_BUFp_DISABLED
 *									 AD77681_BUFp_ENABLED
 *									 AD77681_BUFp_FULL_BUFFER_ON
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

	dev->REFp = REFp;

	return ret;
}

/**
 * Set filter type and decimation ratio
 * @param dev 			The device structure.
 * @param decimate		Decimation ratio of filter
 *						Accepted values: AD77681_SINC5_FIR_DECx32
 *										 AD77681_SINC5_FIR_DECx64
 *										 AD77681_SINC5_FIR_DECx128
 *										 AD77681_SINC5_FIR_DECx256
 *										 AD77681_SINC5_FIR_DECx512
 *										 AD77681_SINC5_FIR_DECx1024
 * @param filter    	Select filter type
 *						Accepted values: AD77681_SINC5
 *										 AD77681_SINC5_DECx8
 *										 AD77681_SINC5_DECx16
 *										 AD77681_SINC3
 *										 AD77681_FIR
 * @param sinc3_osr		Select decimation ratio for SINC3 filter separately as integer
 *						from 0 to 8192.
 *						See the AD7768-1 datasheet for more info
 * @return 0 in case of success, negative error code otherwise.
 */

int32_t ad77681_set_filter_type(struct ad77681_dev *dev,
	enum ad77681_sinc5_fir_decimate decimate,
	enum ad77681_filter_type filter,
	uint16_t sinc3_osr)
{
	int32_t ret = 0;
	
	ret |= ad77681_spi_reg_write(dev, AD77681_REG_DIGITAL_FILTER, 0x00);
	
	if ((filter == AD77681_SINC5_DECx8) || (filter == AD77681_SINC5_DECx16)) {			// SINC5 for OSR 8x and 16x
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_DIGITAL_FILTER,
			AD77681_DIGI_FILTER_FILTER_MSK,
			AD77681_DIGI_FILTER_FILTER(filter));
	}
	else if ((filter == AD77681_SINC5) || (filter == AD77681_FIR)) {					// SINC5 and FIR for osr 32x to 1024x
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_DIGITAL_FILTER,
			AD77681_DIGI_FILTER_FILTER_MSK,
			AD77681_DIGI_FILTER_FILTER(filter));
		
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_DIGITAL_FILTER,
			AD77681_DIGI_FILTER_DEC_RATE_MSK,
			AD77681_DIGI_FILTER_DEC_RATE(decimate));
	}

	else {																				// SINC3
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
	
	dev->decimate = decimate;
	dev->filter = filter;
	ret |= ad77681_initiate_sync(dev);															// Sync pulse after each filter change
	return ret;
}

/**
 * Enable 50/60 Hz rejection
 * @param dev   -  The device structure.
 * @param enable - The positive reference buffer selector
 * 					Accepted values: true
 *									 false
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
 * @param REFn - The negative reference buffer selector
 * 					Accepted values: AD77681_BUFn_DISABLED
 *									 AD77681_BUFn_ENABLED
 *									 AD77681_BUFn_FULL_BUFFER_ON
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_continuos_read(struct ad77681_dev *dev,
	enum ad77681_continuous_read continuous_enable)
{
	int32_t ret;
	
	if (continuous_enable)
		ret = ad77681_spi_write_mask(dev,
			AD77681_REG_INTERFACE_FORMAT,
			AD77681_INTERFACE_CONT_READ_MSK,
			AD77681_INTERFACE_CONT_READ_EN(continuous_enable));
	else																			// To exit the continuous read mode, a key 0x6C must be written into the device over the SPI
	{
		uint8_t end_key = EXIT_CONT_READ;
		ret = spi_write_and_read(dev->spi_desc, &end_key, 1);	
	}

	return ret;
}

/**
 * Power down / power up the device
 * @param dev -			The device structure.
 * @param sleep_wake	Power down, or power up the ADC
 * 						Accepted values: AD77681_SLEEP
 *										 AD77681_WAKE
 * @return 0 in case of success, negative error code otherwise.
 */

int32_t ad77681_power_down(struct ad77681_dev *dev,
	enum ad77681_sleep_wake sleep_wake)
{
	int32_t ret;
	
	if (sleep_wake == AD77681_SLEEP)
	{
		ret = ad77681_spi_reg_write(dev, AD77681_REG_POWER_CLOCK, AD77681_POWER_CLK_POWER_DOWN);
	}
	else {											// Wake up the ADC over SPI, by sending a wake-up sequence: 1 followed by 63 zeroes and CS hold low
		uint8_t wake_sequence[8] = { 0 };
		wake_sequence[0] = 0x80;					// Insert '1' to the beginning of the wake_sequence
		ret = spi_write_and_read(dev->spi_desc, wake_sequence, ARRAY_SIZE(wake_sequence));	
	}
	
	return 0;
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

	dev->conv_mode = conv_mode;
	dev->diag_mux_sel = diag_mux_sel;
	dev->conv_diag_sel = conv_diag_sel;

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
	int32_t ret = 0;

	ret |= 	ad77681_spi_write_mask(dev,
				       AD77681_REG_INTERFACE_FORMAT,
				       AD77681_INTERFACE_CONVLEN_MSK,
				       AD77681_INTERFACE_CONVLEN(conv_len));


	dev->conv_len = conv_len;

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
	int32_t ret = 0;

	if (crc_sel == AD77681_NO_CRC) {
		ret |= 	ad77681_spi_write_mask(dev,
					       AD77681_REG_INTERFACE_FORMAT,
					       AD77681_INTERFACE_CRC_EN_MSK,
					       AD77681_INTERFACE_CRC_EN(0));
	} else {
		ret |= 	ad77681_spi_write_mask(dev,
					       AD77681_REG_INTERFACE_FORMAT,
					       AD77681_INTERFACE_CRC_EN_MSK,
					       AD77681_INTERFACE_CRC_EN(1));

		ret |= 	ad77681_spi_write_mask(dev,
					       AD77681_REG_INTERFACE_FORMAT,
					       AD77681_INTERFACE_CRC_TYPE_MSK,
					       AD77681_INTERFACE_CRC_TYPE(crc_sel));
	}

	dev->crc_sel = crc_sel;

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
	int32_t ret = 0;

	// Set status bit
	ret |= 	ad77681_spi_write_mask(dev,
				       AD77681_REG_INTERFACE_FORMAT,
				       AD77681_INTERFACE_STATUS_EN_MSK,
				       AD77681_INTERFACE_STATUS_EN(status_bit));

	dev->status_bit = status_bit;

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
 * Device reset over SPI.
 * @param dev 			The device structure.
 * @param rst_option	Choose between soft and hard reset
 *						Accepted values: AD77681_SOFT_RESET
 *										 AD77681_HARD_RESET
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_reset(struct ad77681_dev *dev,
	enum ad7761_reset_option rst_option)
{
	int32_t ret = 0;

	if (rst_option == AD77681_SOFT_RESET)							// Soft reset over SPI
	{		
		ret |= 	ad77681_spi_write_mask(dev,							// Two writes are required to initialize the reset
			AD77681_REG_SYNC_RESET,
			AD77681_SYNC_RST_SPI_RESET_MSK,
			AD77681_SYNC_RST_SPI_RESET(0x3));

		ret |= 	ad77681_spi_write_mask(dev,
			AD77681_REG_SYNC_RESET,
			AD77681_SYNC_RST_SPI_RESET_MSK,
			AD77681_SYNC_RST_SPI_RESET(0x2));		
	}	
	
	else															// Hard reset using RST pin on ADC
	{		
		ret |= gpio_set_value(dev->gpio_reset, GPIO_LOW);
		_delay_ms(20);
		ret |= gpio_set_value(dev->gpio_reset, GPIO_HIGH);
	}
			
	return ret;
}

/**
 * Initiate a SYNC_OUT pulse over spi
 * @param dev 			The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_initiate_sync(struct ad77681_dev *dev)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
		AD77681_REG_SYNC_RESET,
		AD77681_SYNC_RST_SPI_STARTB_MSK,
		AD77681_SYNC_RST_SPI_STARTB(0));	
	
	return ret;
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
 * @param dev 			The device structure.
 * @param value			The desired value of the whole 24-bit gain register
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
 * Programmamble fir filter
 * @param dev 				The device structure.
 * @param *coefs			Desired coefficients in array of the filter to be written
 * @param count_of_coeffs	Count of active coeffs
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_programmable_filter(struct ad77681_dev *dev,
	const float *coeffs,
	uint8_t num_coeffs)
{
	uint8_t coeffs_buf[4], check_back = 0, i, address = 0;
	uint16_t twait;
	int32_t twos_complement, ret = 0;
	
	const uint8_t coeff_reg_length = 56;
	const uint8_t key1 = 0xAC, key2 = 0x45, key3 = 0x55;						// Specifice keys taken from the AD7768-1 datasheet as a part of the upload sequence
	const float coeff_scale_factor = (1 << 22);									// Scaling factor for all coefficients 2^22
	
	twait = (uint16_t)(((512.0) / ((float)(dev->mclk))) * 1000.0) + 1; 			// Wait time in uS necessary for access the COEFF_CONTROL and COEFF_DATA registers
																				// Equation taken form the AD7768-1 datasheet => Twait = 512/MCLK
	
	// 9 "UPLOAD SEQUENCE" steps taken from the AD7768-1 datasheet
	
	// 1. Set the FIR filter
	ret |= ad77681_spi_write_mask(dev,
		AD77681_REG_DIGITAL_FILTER,
		AD77681_DIGI_FILTER_FILTER_MSK,
		AD77681_DIGI_FILTER_FILTER(AD77681_FIR));
	
	// 2. write specific key to the ACCESS_KEY register
	ret |= ad77681_spi_write_mask(dev,
		AD77681_REG_ACCESS_KEY,
		AD77681_ACCESS_KEY_MSK,
		AD77681_ACCESS_KEY(key1));
	
	ret |= ad77681_spi_write_mask(dev,
		AD77681_REG_ACCESS_KEY,
		AD77681_ACCESS_KEY_MSK,
		AD77681_ACCESS_KEY(key2));
	
	ret |= ad77681_spi_read_mask(dev,
		AD77681_REG_ACCESS_KEY,
		AD77681_ACCESS_KEY_CHECK_MSK,
		&check_back);

	(check_back == 1) ? (ret = 0) : (ret = -1);
	
	// 3. Write and increment address to COEF_CONTROL
	//	  Write coefficient to the COEF_DATA register
	
	address |= AD77681_COEF_CONTROL_COEFFACCESSEN_MSK;
	address |= AD77681_COEF_CONTROL_COEFFWRITEEN_MSK;
	
	for (i = 0; i < coeff_reg_length; i++)
	{
		// Set the coeff address, write 0xC0 to COEF_CONTROLL
		ret |= ad77681_spi_reg_write(dev,
			AD77681_REG_COEFF_CONTROL,
			address);
		
		// 4. the address is set, now wait 
		_delay_us(twait);		
																				// The COEFF_DATA register has to be filled with 56 coeffs!
																				// In case of usage less than 56 active coeffs, the remaining
		if(i <= num_coeffs)														// adressess of the COEFF_DATA register will be filled with zeroes																																																			
		{
										
			twos_complement = (int32_t)(coeffs[i] * coeff_scale_factor);		// Scaling value of each coefficient * 2^22		
			
			// 5. wirte coefficients to COEFF_DATA
			coeffs_buf[0] = AD77681_REG_WRITE(AD77681_REG_COEFF_DATA);
			coeffs_buf[1] = (twos_complement & 0xFF0000) >> 16;
			coeffs_buf[2] = (twos_complement & 0x00FF00) >> 8;
			coeffs_buf[3] = (twos_complement & 0x0000FF);
		}
		
		else
		{
			// 5. wirte zeroes to COEFF_DATA, in case of less coeffs than 56
			coeffs_buf[0] = AD77681_REG_WRITE(AD77681_REG_COEFF_DATA);
			coeffs_buf[1] = 0;
			coeffs_buf[2] = 0;
			coeffs_buf[3] = 0;
		}
		
		ret |= spi_write_and_read(dev->spi_desc, coeffs_buf, 4);	
		
		// Increment the address
		address++;	
		_delay_us(twait);	
	}
	
	// 6. Disable coefficient write
	ret |= ad77681_spi_write_mask(dev,
		AD77681_REG_COEFF_CONTROL,
		AD77681_COEF_CONTROL_COEFFWRITEEN_MSK,
		AD77681_COEF_CONTROL_COEFFWRITEEN(0x00));
	
	_delay_ms(1);	
	
	// 6. Disable coefficient access
	ret |= ad77681_spi_write_mask(dev,
		AD77681_REG_COEFF_CONTROL,
		AD77681_COEF_CONTROL_COEFFACCESSEN_MSK,
		AD77681_COEF_CONTROL_COEFFACCESSEN(0x00));
	
	// 7. Allowing to toggle sync pulse
	// Write 0x800000 to COEFF_DATA
	coeffs_buf[0] = AD77681_REG_WRITE(AD77681_REG_COEFF_DATA);
	coeffs_buf[1] = 0x80;
	coeffs_buf[2] = 0x00;
	coeffs_buf[3] = 0x00;
	ret |= spi_write_and_read(dev->spi_desc, coeffs_buf, 4);
	
	// 8. Exit filter upload by wirting specific access key 0x55
	ret |= ad77681_spi_write_mask(dev,
		AD77681_REG_ACCESS_KEY,
		AD77681_ACCESS_KEY_MSK,
		AD77681_ACCESS_KEY(key3));
	
	// 9. Send synchronization pulse
	ad77681_initiate_sync(dev);
	
	return ret;
}



/**
 * Read value from GPIOs present in AD7768-1 separately, or all GPIOS at once.
 * @param dev 			The device structure.
 * @param value			Readed value.
 * @param gpio_number	Number of GPIO, the value will be written into
 *						Accepted values: AD77681_GPIO0
 *										 AD77681_GPIO1
 *										 AD77681_GPIO2
 *										 AD77681_GPIO3
 *										 AD77681_ALL_GPIOS
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_gpio_read(struct ad77681_dev *dev,
	uint8_t *value,
	enum ad77681_gpios gpio_number)
{
	int32_t ret = 0;
	
	switch (gpio_number)
	{
	case AD77681_GPIO0:
		
		ret |= ad77681_spi_read_mask(dev,
			AD77681_REG_GPIO_READ,
			AD77681_GPIO_READ_0_MSK,
			value);
		break;
		
	case AD77681_GPIO1:
		ret |= ad77681_spi_read_mask(dev,
			AD77681_REG_GPIO_READ,
			AD77681_GPIO_READ_1_MSK,
			value);
		break;
		
	case AD77681_GPIO2:
		ret |= ad77681_spi_read_mask(dev,
			AD77681_REG_GPIO_READ,
			AD77681_GPIO_READ_2_MSK,
			value);
		break;
		
	case AD77681_GPIO3:
		ret |= ad77681_spi_read_mask(dev,
			AD77681_REG_GPIO_READ,
			AD77681_GPIO_READ_3_MSK,
			value);
		break;
		
	case AD77681_ALL_GPIOS:
		ret |= ad77681_spi_read_mask(dev,
			AD77681_REG_GPIO_READ,
			AD77681_GPIO_READ_ALL_MSK,
			value);
		break;
	
	default:
		return -1;
		break;
	}
	
	return ret;
}



/**
 * Write value to GPIOs present in AD7768-1 separately, or all GPIOS at once.
 * @param dev 			The device structure.
 * @param value			Value to be written into GPIO
 *						Accepted values: GPIO_HIGH
 *										 GPIO_LOW
 *										 4-bit value for all gpios
 * @param gpio_number	Number of GPIO, the value will be written into
 *						Accepted values: AD77681_GPIO0
 *										 AD77681_GPIO1
 *										 AD77681_GPIO2
 *										 AD77681_GPIO3
 *										 AD77681_ALL_GPIOS
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_gpio_write(struct ad77681_dev *dev,
	uint8_t value,
	enum ad77681_gpios gpio_number)
{
	int32_t ret = 0;

	switch (gpio_number)
	{
	case AD77681_GPIO0:											// Write to GPIO0
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_WRITE,
			AD77681_GPIO_WRITE_0_MSK,
			AD77681_GPIO_WRITE_0(value));	
	
		break;
	case AD77681_GPIO1:											// Write to GPIO1
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_WRITE,
			AD77681_GPIO_WRITE_1_MSK,
			AD77681_GPIO_WRITE_1(value));
		
		break;
	case AD77681_GPIO2:											// Write to GPIO2	
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_WRITE,
			AD77681_GPIO_WRITE_2_MSK,
			AD77681_GPIO_WRITE_2(value));
		
		break;
	case AD77681_GPIO3:											// Write to GPIO3
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_WRITE,
			AD77681_GPIO_WRITE_3_MSK,
			AD77681_GPIO_WRITE_3(value));
		
		break;
	case AD77681_ALL_GPIOS:										// Write to all GPIOs
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_WRITE,
			AD77681_GPIO_WRITE_ALL_MSK,
			AD77681_GPIO_WRITE_ALL(value));	
		
		break;
	default:
		return -1;
		break;
	}
	
	return ret;
}

/**
 * Set AD7768-1s GPIO as input or output.
 * @param dev 			The device structure.
 * @param direction		Direction of the GPIO
 *						Accepted values: GPIO_INPUT
 *										 GPIO_OUTPUT
 *										 4-bit value for all gpios
 * @param gpio_number	Number of GPIO, which will be affected
 *						Accepted values: AD77681_GPIO0
 *										 AD77681_GPIO1
 *										 AD77681_GPIO2
 *										 AD77681_GPIO3
 *										 AD77681_ALL_GPIOS
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_gpio_inout(struct ad77681_dev *dev,
	uint8_t direction,
	enum ad77681_gpios gpio_number)
{
	int32_t ret = 0;

	switch (gpio_number)
	{			
	case AD77681_GPIO0:											// Set direction of GPIO0
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_CONTROL,
			AD77681_GPIO_CNTRL_GPIO0_OP_EN_MSK,
			AD77681_GPIO_CNTRL_GPIO0_OP_EN(direction));
		break;
		
	case AD77681_GPIO1:											// Set direction of GPIO1
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_CONTROL,
			AD77681_GPIO_CNTRL_GPIO1_OP_EN_MSK,
			AD77681_GPIO_CNTRL_GPIO1_OP_EN(direction));
		break;
		
	case AD77681_GPIO2:											// Set direction of GPIO2
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_CONTROL,
			AD77681_GPIO_CNTRL_GPIO2_OP_EN_MSK,
			AD77681_GPIO_CNTRL_GPIO2_OP_EN(direction));
		break;
			
	case AD77681_GPIO3:											// Set direction of GPIO3
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_CONTROL,
			AD77681_GPIO_CNTRL_GPIO3_OP_EN_MSK,
			AD77681_GPIO_CNTRL_GPIO3_OP_EN(direction));
		break;
		
	case AD77681_ALL_GPIOS:										// Set direction of all GPIOs		
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_CONTROL,
			AD77681_GPIO_CNTRL_ALL_GPIOS_OP_EN_MSK,
			AD77681_GPIO_CNTRL_ALL_GPIOS_OP_EN(direction));	
		break;
		
	default:
		return -1;
		break;
	}
	
	return ret;
}

/**
 * Enable global GPIO bit.
 * This bit must be set high to change GPIO settings.
 * @param dev 			The device structure.
 * @param gpio_enable	Enable or diable the global GPIO pin
 *						Accepted values: AD77681_GLOBAL_GPIO_ENABLE
 *										 AD77681_GLOBAL_GPIO_DISABLE
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_global_gpio(struct ad77681_dev *dev,
	enum ad77681_gobal_gpio_enable gpio_enable)
{
	int32_t ret;

	ret = ad77681_spi_write_mask(dev,
		AD77681_REG_GPIO_CONTROL,
		AD77681_GPIO_CNTRL_UGPIO_EN_MSK,
		AD77681_GPIO_CNTRL_UGPIO_EN(gpio_enable));

	return ret;	
}

/**
 * Read and write from ADC scratchpad register to check SPI Communication in the very beginning, during inicialization.
 * @param dev 			The device structure.
 * @param *sequence		The sequence which will be written into scratchpad and the readed sequence will be returned
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_scratchpad(struct ad77681_dev *dev,
	uint8_t *sequence)
{
	int32_t ret;
	const uint8_t check = *sequence;											// Save the original sequence
	
	ret = ad77681_spi_write_mask(dev,
		AD77681_REG_SCRATCH_PAD,
		AD77681_SCRATCHPAD_MSK,
		AD77681_SCRATCHPAD(*sequence));
	
	ret |= ad77681_spi_read_mask(dev,
		AD77681_REG_SCRATCH_PAD,
		AD77681_SCRATCHPAD_MSK,
		sequence);
	
	if (check == *sequence)														// Compare original an returned sequence
		ret = 0;
	else
		ret = -1;
	
	return ret;	
}

/**
 * Set AD7768-1s GPIO output type between strong driver and open drain.
 * GPIO3 can not be accessed!
 * @param dev 			The device structure.
 * @param gpio_number	AD7768-1s GPIO to be affected (Only GPIO0, GPIO1 and GPIO2)
 *						Accepted values: AD77681_GPIO0
 *										 AD77681_GPIO1
 *										 AD77681_GPIO2
 *										 AD77681_ALL_GPIOS
 *										 
 * @param output_type	Otuput type of the GPIO
 *						Accepted values: AD77681_GPIO_STRONG_DRIVER
 *										 AD77681_GPIO_OPEN_DRAIN
 * @param 
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_gpio_open_drain(struct ad77681_dev *dev,
	enum ad77681_gpios gpio_number,
	enum ad77681_gpio_output_type output_type)
{
	int32_t ret = 0;
	
	switch (gpio_number)
	{			
	case AD77681_GPIO0:											// Set ouptut type of GPIO0
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_CONTROL,
			AD77681_GPIO_CNTRL_GPIO0_OD_EN_MSK,
			AD77681_GPIO_CNTRL_GPIO0_OD_EN(output_type));
		break;
		
	case AD77681_GPIO1:											// Set ouptut type of GPIO1
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_CONTROL,
			AD77681_GPIO_CNTRL_GPIO1_OD_EN_MSK,
			AD77681_GPIO_CNTRL_GPIO1_OD_EN(output_type));
		break;
		
	case AD77681_GPIO2:											// Set ouptut type of GPIO2
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_CONTROL,
			AD77681_GPIO_CNTRL_GPIO2_OD_EN_MSK,
			AD77681_GPIO_CNTRL_GPIO2_OD_EN(output_type));
		break;
		
	case AD77681_ALL_GPIOS:										// Set ouptut type of all GPIOs		
		ret |= ad77681_spi_write_mask(dev,
			AD77681_REG_GPIO_CONTROL,
			AD77681_GPIO_CNTRL_ALL_GPIOS_OD_EN_MSK,
			AD77681_GPIO_CNTRL_ALL_GPIOS_OD_EN(output_type));	
		break;
		
	default:
		return -1;
		break;
	}
	
	return ret;
}

/**
 * Clear all error flags at once
 * @param dev 			The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_clear_error_flags(struct ad77681_dev *dev)
{
	int32_t ret;
	
	ret = ad77681_spi_write_mask(dev,									//  SPI ignore error CLEAR
		AD77681_REG_SPI_DIAG_STATUS,
		AD77681_SPI_IGNORE_ERROR_MSK,
		AD77681_SPI_IGNORE_ERROR_CLR(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									//  SPI read error CLEAR
		AD77681_REG_SPI_DIAG_STATUS,
		AD77681_SPI_READ_ERROR_MSK,
		AD77681_SPI_READ_ERROR_CLR(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									//  SPI write error CLEAR
		AD77681_REG_SPI_DIAG_STATUS,
		AD77681_SPI_WRITE_ERROR_MSK,
		AD77681_SPI_WRITE_ERROR_CLR(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									//  SPI CRC error CLEAR
		AD77681_REG_SPI_DIAG_STATUS,
		AD77681_SPI_CRC_ERROR_MSK,
		AD77681_SPI_CRC_ERROR_CLR(ENABLE));
	
	return ret;
}

/**
 * Enabling error flags. All error flags enabled by default
 * @param dev 			The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_error_flags_enabe(struct ad77681_dev *dev)
{
	int32_t ret;
	
	//			SPI ERRORS ENABLE
	
	ret = ad77681_spi_write_mask(dev,									// SPI ignore error enable
		AD77681_REG_SPI_DIAG_ENABLE,
		AD77681_SPI_DIAG_ERR_SPI_IGNORE_MSK,
		AD77681_SPI_DIAG_ERR_SPI_IGNORE(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									//  SPI Clock count error enable 
		AD77681_REG_SPI_DIAG_ENABLE,
		AD77681_SPI_DIAG_ERR_SPI_CLK_CNT_MSK,
		AD77681_SPI_DIAG_ERR_SPI_CLK_CNT(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									// SPI Read error enable
		AD77681_REG_SPI_DIAG_ENABLE,
		AD77681_SPI_DIAG_ERR_SPI_RD_MSK,
		AD77681_SPI_DIAG_ERR_SPI_RD(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									// SPI Write error enable
		AD77681_REG_SPI_DIAG_ENABLE,
		AD77681_SPI_DIAG_ERR_SPI_WR_MSK,
		AD77681_SPI_DIAG_ERR_SPI_WR(ENABLE));
	
	//		ADC DIAG ERRORS ENABLE
	
	ret |= ad77681_spi_write_mask(dev,									// DLDO PSM error enable
		AD77681_REG_ADC_DIAG_ENABLE,
		AD77681_ADC_DIAG_ERR_DLDO_PSM_MSK,
		AD77681_ADC_DIAG_ERR_DLDO_PSM(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									// ALDO PSM error enable
		AD77681_REG_ADC_DIAG_ENABLE,
		AD77681_ADC_DIAG_ERR_ALDO_PSM_MSK,
		AD77681_ADC_DIAG_ERR_ALDO_PSM(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									// Filter saturated error enable
		AD77681_REG_ADC_DIAG_ENABLE,
		AD77681_ADC_DIAG_ERR_FILT_SAT_MSK,
		AD77681_ADC_DIAG_ERR_FILT_SAT(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									// Filter not settled error enable
		AD77681_REG_ADC_DIAG_ENABLE,
		AD77681_ADC_DIAG_ERR_FILT_NOT_SET_MSK,
		AD77681_ADC_DIAG_ERR_FILT_NOT_SET(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									// External clock check error enable
		AD77681_REG_ADC_DIAG_ENABLE,
		AD77681_ADC_DIAG_ERR_EXT_CLK_QUAL_MSK,
		AD77681_ADC_DIAG_ERR_EXT_CLK_QUAL(ENABLE));
	
	//	    DIG DIAG ENABLE
	
	ret |= ad77681_spi_write_mask(dev,									// Memory map CRC error enabled
		AD77681_REG_DIG_DIAG_ENABLE,
		AD77681_DIG_DIAG_ERR_MEMMAP_CRC_MSK,
		AD77681_DIG_DIAG_ERR_MEMMAP_CRC(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									// RAM CRC error enabled
		AD77681_REG_DIG_DIAG_ENABLE,
		AD77681_DIG_DIAG_ERR_RAM_CRC_MSK,
		AD77681_DIG_DIAG_ERR_RAM_CRC(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									// FUSE CRC error enabled
		AD77681_REG_DIG_DIAG_ENABLE,
		AD77681_DIG_DIAG_ERR_FUSE_CRC_MSK,
		AD77681_DIG_DIAG_ERR_FUSE_CRC(ENABLE));
	
	ret |= ad77681_spi_write_mask(dev,									// Enable MCLK Counter
		AD77681_REG_DIG_DIAG_ENABLE,
		AD77681_DIG_DIAG_FREQ_COUNT_EN_MSK,
		AD77681_DIG_DIAG_FREQ_COUNT_EN(ENABLE));	
	
	return ret;
}

/**
 * Read from all ADC status registers
 * @param dev 			The device structure.
 * @param status		Structure with all satuts bits
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_status(struct ad77681_dev *dev,
	struct ad77681_status_registers *status)
{
	int32_t ret;
	uint8_t buf[3];
	
	ret = ad77681_spi_reg_read(dev, AD77681_REG_MASTER_STATUS, buf);						// Master status register
	
	status->master_error = buf[1] & AD77681_MASTER_ERROR_MSK;
	status->adc_error = buf[1] & AD77681_MASTER_ADC_ERROR_MSK;
	status->dig_error = buf[1] & AD77681_MASTER_DIG_ERROR_MSK;
	status->adc_err_ext_clk_qual = buf[1] & AD77681_MASTER_DIG_ERR_EXT_CLK_MSK;
	status->adc_filt_saturated = buf[1] & AD77681_MASTER_FILT_SAT_MSK;
	status->adc_filt_not_settled = buf[1] & AD77681_MASTER_FILT_NOT_SET_MSK;
	status->spi_error = buf[1] & AD77681_MASTER_SPI_ERROR_MSK;
	status->por_flag = buf[1] & AD77681_MASTER_POR_FLAG_MSK;
	
	ret |= ad77681_spi_reg_read(dev, AD77681_REG_SPI_DIAG_STATUS, buf);						// SPI diag status register
	
	status->spi_ignore = buf[1] & AD77681_SPI_IGNORE_ERROR_MSK;
	status->spi_clock_count = buf[1] & AD77681_SPI_CLK_CNT_ERROR_MSK;
	status->spi_read_error = buf[1] & AD77681_SPI_READ_ERROR_MSK;
	status->spi_write_error = buf[1] & AD77681_SPI_WRITE_ERROR_MSK;
	status->spi_crc_error = buf[1] & AD77681_SPI_CRC_ERROR_MSK;
	
	ret |= ad77681_spi_reg_read(dev, AD77681_REG_ADC_DIAG_STATUS, buf);  					// ADC diag status register
	
	status->dldo_psm_error = buf[1] & AD77681_ADC_DLDO_PSM_ERROR_MSK;
	status->aldo_psm_error = buf[1] & AD77681_ADC_ALDO_PSM_ERROR_MSK;
    status->ref_det_error = buf[1] & AD77681_ADC_REF_DET_ERROR_MSK;
    status->filt_sat_error = buf[1] & AD77681_ADC_FILT_SAT_MSK;
    status->filt_not_set_error = buf[1] & AD77681_ADC_FILT_NOT_SET_MSK;
    status->ext_clk_qual_error = buf[1] & AD77681_ADC_DIG_ERR_EXT_CLK_MSK;
	
	ret |= ad77681_spi_reg_read(dev, AD77681_REG_DIG_DIAG_STATUS, buf);    					// DIG diag status register
	
	status->memoy_map_crc_error = buf[1] & AD77681_DIG_MEMMAP_CRC_ERROR_MSK;
	status->ram_crc_error = buf[1] & AD77681_DIG_RAM_CRC_ERROR_MSK;
	status->fuse_crc_error = buf[1] & AD77681_DIG_FUS_CRC_ERROR_MSK;
	
	return ret;
}

/**
 * Initialize the device and ADC status structure.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @param status	 - First status read from ADC
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_setup(struct ad77681_dev **device,
		    struct ad77681_init_param init_param,
			struct ad77681_status_registers **status)
{
	struct ad77681_dev *dev;
	struct ad77681_status_registers *stat;
	int32_t ret = 0;
	uint8_t scratchpad_check = 0xAD;
	
	dev = (struct ad77681_dev *)malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

    stat = (struct ad77681_status_registers *)malloc(sizeof(*stat));
    if (!stat){
        return -1;
    }

	dev->power_mode = init_param.power_mode;
	dev->mclk_div = init_param.mclk_div;
	dev->conv_diag_sel = init_param.conv_diag_sel;
	dev->conv_mode = init_param.conv_mode;
	dev->diag_mux_sel = init_param.diag_mux_sel;
	dev->conv_len = init_param.conv_len;
	dev->crc_sel = init_param.crc_sel;
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
	dev->data_frame_bytes = init_param.data_frame_bytes;

	spi_init(&dev->spi_desc, &init_param.spi_eng_dev_init);
		
	ret |= gpio_get(&dev->gpio_drdy, &init_param.gpio_drdy);
	ret |= drdy_interrupt_enable(dev->gpio_drdy, DISABLE);						// initialize and disable interrupt on DRDY pin
	ret |= gpio_get(&dev->gpio_reset, &init_param.gpio_reset);
	ret |= gpio_direction_output(dev->gpio_reset, GPIO_HIGH);

	ret |= ad77681_reset(dev, AD77681_HARD_RESET);								// Hard reset in case of ADC's SPI is jammed
	ret |= ad77681_reset_spi_interface();										// SPI reset by toggling CS
	
	if (ad77681_scratchpad(dev, &scratchpad_check) == -1) {				// Chceck physical connection using scratchpad
							
		scratchpad_check = 0xAD;												
		ret |= (ad77681_scratchpad(dev, &scratchpad_check));					// If failure, second try
		
		if(ret == -1)
			return -1;
	}
	else
	{
		ret |= ad77681_set_power_mode(dev, dev->power_mode);
		ret |= ad77681_set_mclk_div(dev, dev->mclk_div);
		ret |= ad77681_set_conv_mode(dev,
						 dev->conv_mode,
						 dev->diag_mux_sel,
						 dev->conv_diag_sel);
		ret |= ad77681_set_convlen(dev, dev->conv_len);
		ret |= ad77681_set_status_bit(dev, dev->status_bit);
		ret |= ad77681_set_crc_sel(dev, dev->crc_sel);
		ret |= ad77681_set_VCM_output(dev, dev->VCM_out);
		ret |= ad77681_set_AINn_buffer(dev, dev->AINn);
		ret |= ad77681_set_AINp_buffer(dev, dev->AINp);
		ret |= ad77681_set_REFn_buffer(dev, dev->REFn);
		ret |= ad77681_set_REFp_buffer(dev, dev->REFp);
		ret |= ad77681_set_filter_type(dev, dev->decimate, dev->filter, dev->sinc3_osr);
		ret |= ad77681_error_flags_enabe(dev);
		ret |= ad77681_clear_error_flags(dev);
        ret |= ad77681_status(dev, stat);
        *status = stat;	
		ad77681_get_frame_bytes(dev);
		ad77681_update_sample_rate(dev);
		*device = dev;
	}
	if (!ret)
		printf("ad77681 successfully initialized\n");

	return ret;
}

int32_t ad77681_reset_spi_interface(void)
{
	int32_t ret;
	
	ret = spi_slave_select(GPIO_LOW);
	ret |= spi_slave_select(GPIO_HIGH);
	
	return ret;
}
