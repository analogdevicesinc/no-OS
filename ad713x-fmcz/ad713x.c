/***************************************************************************//**
 *   @file   ad713x.c
 *   @brief  Implementation of ad713x Driver.
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
#include "platform_drivers.h"
#include "ad713x.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad713x_spi_reg_read(struct ad713x_dev *dev,
			    uint8_t reg_addr,
			    uint8_t *reg_data)
{
	int32_t ret;
	uint8_t buf[2];

	buf[0] = AD713X_REG_READ(reg_addr);
	buf[1] = 0x00;

	ret = spi_write_and_read(dev->spi_desc, buf, 2);
	*reg_data = buf[1];

	return ret;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad713x_spi_reg_write(struct ad713x_dev *dev,
			     uint8_t reg_addr,
			     uint8_t reg_data)
{
	int32_t ret;
	uint8_t buf[2];

	buf[0] = reg_addr;
	buf[1] = reg_data;

	ret = spi_write_and_read(dev->spi_desc, buf, 2);

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
int32_t ad713x_spi_write_mask(struct ad713x_dev *dev,
			      uint8_t reg_addr,
			      uint32_t mask,
			      uint8_t data)
{
	uint8_t reg_data;
	int32_t ret;

	ret = ad713x_spi_reg_read(dev, reg_addr, &reg_data);
	reg_data &= ~mask;
	reg_data |= data;
	ret |= ad713x_spi_reg_write(dev, reg_addr, reg_data);

	return ret;
}

/**
 * Device power mode control.
 * @param dev - The device structure.
 * @param mode - Type of power mode
 * 			Accepted values: LOW_POWER
 * 					 HIGH_POWER
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad713x_set_power_mode(struct ad713x_dev *dev,
			      enum ad713x_power_mode mode)
{
	int32_t ret;

	ret = ad713x_spi_write_mask(dev,
				    AD713X_REG_DEVICE_CONFIG,
				    AD713X_DEV_CONFIG_PWR_MODE_MSK,
				    AD713X_DEV_CONFIG_PWR_MODE(mode));

	return ret;
}

/**
 * ADC conversion data output frame control.
 * @param dev - The device structure.
 * @param adc_data_len - Data conversion length
 * 				Accepted values: ADC_16_BIT_DATA
 * 						 ADC_24_BIT_DATA
 * 						 ADC_32_BIT_DATA
 * @param crc_header - CRC header
 * 				Accepted values: NO_CRC
 * 						 CRC_6
 * 						 CRC_8
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad713x_set_out_data_frame(struct ad713x_dev *dev,
				  enum ad713x_adc_data_len adc_data_len,
				  enum ad713x_crc_header crc_header)
{
	int32_t ret;
	uint8_t id;
	uint8_t i = 0;

	id = dev->dev_id;

	while (ad713x_output_data_frame[id][i][0] != INVALID) {
		if((adc_data_len == ad713x_output_data_frame[id][i][0]) &&
		    (crc_header == ad713x_output_data_frame[id][i][1])) {
			ret = ad713x_spi_write_mask(dev,
						    AD713X_REG_DATA_PACKET_CONFIG,
						    AD713X_DATA_PACKET_CONFIG_FRAME_MSK,
						    AD713X_DATA_PACKET_CONFIG_FRAME_MODE(i));
			return ret;
		}
		i++;
	}

	return FAILURE;
}

/**
 * DOUTx output format configuration.
 * @param dev - The device structure.
 * @param format - Single channel daisy chain mode. Dual channel daisy chain mode.
 * 		   Quad channel parallel output mode. Channel data averaging mode.
 * 			Accepted values: SINGLE_CH_DC
 * 					 DUAL_CH_DC
 * 					 QUAD_CH_PO
 * 					 CH_AVG_MODE
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad713x_dout_format_config(struct ad713x_dev *dev,
				  enum ad713x_doutx_format format)
{
	int32_t ret;

	ret = ad713x_spi_write_mask(dev,
				    AD713X_REG_DIGITAL_INTERFACE_CONFIG,
				    AD713X_DIG_INT_CONFIG_FORMAT_MSK,
				    AD713X_DIG_INT_CONFIG_FORMAT_MODE(format));

	return ret;
}

/**
 * Magnitude and phase matching calibration clock delay enable for all channels
 * at 2 clock delay.
 *
 * This function is kept for backwards compatibility with the current
 * application source, but it is deprecated. Use
 * ad713x_mag_phase_clk_delay_chan().
 *
 * @param dev - The device structure.
 * @param clk_delay_en - Enable or disable Mag/Phase clock delay.
 * 				Accepted values: true
 * 						 false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad713x_mag_phase_clk_delay(struct ad713x_dev *dev,
				   bool clk_delay_en)
{
	int32_t ret;
	int8_t i;
	int8_t temp_clk_delay;

	if(clk_delay_en)
		temp_clk_delay = DELAY_2_CLOCKS;
	else
		temp_clk_delay = DELAY_NONE;
	for(i = CH3; i >= 0; i--) {
		ret = ad713x_spi_write_mask(dev,
					    AD713X_REG_MPC_CONFIG,
					    AD713X_MPC_CLKDEL_EN_CH_MSK(i),
					    AD713X_MPC_CLKDEL_EN_CH_MODE(temp_clk_delay, i));
		if(ret != 0)
			break;
	}

	return ret;
}

/**
 * Change magnitude and phase calibration clock delay mode for a specific
 * channel.
 *
 * @param dev - The device structure.
 * @param chan - ID of the channel to be changed.
 * 				Accepted values: CH0, CH1, CH2, CH3
 * @param mode - Delay in clock periods.
 * 				Accepted values: DELAY_NONE,
 * 						 DELAY_1_CLOCKS,
 *						 DELAY_2_CLOCKS
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad713x_mag_phase_clk_delay_chan(struct ad713x_dev *dev,
					enum ad713x_channels chan,
					enum ad717x_mpc_clkdel mode)
{
	return ad713x_spi_write_mask(dev,
				     AD713X_REG_MPC_CONFIG,
				     AD713X_MPC_CLKDEL_EN_CH_MSK(chan),
				     AD713X_MPC_CLKDEL_EN_CH_MODE(mode, chan));
}

/**
 * Digital filter type selection for each channel
 * @param dev - The device structure.
 * @param filter - Type of filter: Wideband, Sinc6, Sinc3,
 * 				   Sinc3 filter with simultaneous 50Hz and 60Hz rejection.
 * 				   	Accepted values: FIR
 * 							 SINC6
 * 							 SINC3
 * 							 SINC3_50_60_REJ
 * @param ch - Channel to apply the filter to
 * 					Accepted values: CH0
 * 							 CH1
 * 							 CH2
 * 							 CH3
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad713x_dig_filter_sel_ch(struct ad713x_dev *dev,
				 enum ad713x_dig_filter_sel filter,
				 enum ad713x_channels ch)
{
	int32_t ret;

	ret = ad713x_spi_write_mask(dev,
				    AD713X_REG_CHAN_DIG_FILTER_SEL,
				    AD713X_DIGFILTER_SEL_CH_MSK(ch),
				    AD713X_DIGFILTER_SEL_CH_MODE(filter, ch));

	return ret;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad713x_init(struct ad713x_dev **device,
		    struct ad713x_init_param init_param)
{
	struct ad713x_dev *dev;
	uint8_t buf[3];
	int32_t ret;

	dev = (ad713x_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = spi_init(&dev->spi_desc, &init_param.spi_init);
	/* GPIO */
	ret |= gpio_get(&dev->gpio_mode1, init_param.gpio_mode1);
	ret |= gpio_get(&dev->gpio_mode2, init_param.gpio_mode2);
	ret |= gpio_get(&dev->gpio_dclkmode, init_param.gpio_dclkmode);
	ret |= gpio_get(&dev->gpio_dclkio1, init_param.gpio_dclkio1);
	ret |= gpio_get(&dev->gpio_dclkio2, init_param.gpio_dclkio2);
	ret |= gpio_get(&dev->gpio_resetn1, init_param.gpio_resetn1);
	ret |= gpio_get(&dev->gpio_resetn2, init_param.gpio_resetn2);
	ret |= gpio_get(&dev->gpio_pnd1, init_param.gpio_pnd1);
	ret |= gpio_get(&dev->gpio_pnd2, init_param.gpio_pnd2);
	/*
	 * Tie this pin to IOVDD for master mode operation,
	 * tie this pin to IOGND for slave mode operation.
	 */
	gpio_set_value(dev->gpio_mode1, GPIO_HIGH);
	gpio_set_value(dev->gpio_mode1, GPIO_LOW);
	/* Tie this pin low to ground to make DLCK operating in gated mode */
	gpio_set_value(dev->gpio_dclkmode, GPIO_LOW);
	/*
	 * Tie this pin high to make DCLK an output,
	 * tie this pin low to make DLCK an input.
	 */
	gpio_set_value(dev->gpio_dclkio1, GPIO_LOW);
	gpio_set_value(dev->gpio_dclkio2, GPIO_LOW);

	/* Get the ADCs out of reset state */
	gpio_set_value(dev->gpio_resetn1, GPIO_HIGH);
	gpio_set_value(dev->gpio_resetn2, GPIO_HIGH);

	/* Get the ADCs out of power down state */
	gpio_set_value(dev->gpio_pnd1, GPIO_HIGH);
	gpio_set_value(dev->gpio_pnd2, GPIO_HIGH);

	mdelay(100);

	dev->dev_id = init_param.dev_id;

	/* Disable CRC checks */
	buf[0] = AD713X_REG_DIAGNOSTIC_CONTROL;
	buf[1] = 0x00;
	buf[2] = 0x9C;
	ret |= spi_write_and_read(dev->spi_desc, buf, 3);

	ret |= ad713x_set_power_mode(dev, init_param.power_mode);
	ret |= ad713x_set_out_data_frame(dev,
					 init_param.adc_data_len,
					 init_param.crc_header);
	ret |= ad713x_dout_format_config(dev,
					 init_param.format);
	ret |= ad713x_mag_phase_clk_delay(dev, init_param.clk_delay_en);

	/*
	 * ODR/MCLK ratio. In master mode where ODR pin is configured as output,
	 * user can program this register to set the ODR output frequency based on
	 * MCLK frequency.
	 */
	ad713x_spi_reg_write(dev, AD713X_REG_ODR_MCLK_RATIO_MSB, 0x10);
	/*
	 * Master slave transfer bit. When this bit is set, data that has been
	 * entered into the master registers will be transferred to the slave.
	 */
	ad713x_spi_reg_write(dev, AD713X_REG_TRANSFER_REGISTER, 0x01);

	*device = dev;

	if (!ret)
		printf("ad713x successfully initialized\n");
	mdelay(1000);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad713x_init().
 * @param dev - The device structure.
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad713x_remove(struct ad713x_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);
	ret |= gpio_remove(dev->gpio_dclkio1);
	ret |= gpio_remove(dev->gpio_dclkio2);
	ret |= gpio_remove(dev->gpio_dclkmode);
	ret |= gpio_remove(dev->gpio_mode1);
	ret |= gpio_remove(dev->gpio_mode2);
	ret |= gpio_remove(dev->gpio_pnd1);
	ret |= gpio_remove(dev->gpio_pnd2);
	ret |= gpio_remove(dev->gpio_resetn1);
	ret |= gpio_remove(dev->gpio_resetn2);

	free(dev);

	return ret;
}

