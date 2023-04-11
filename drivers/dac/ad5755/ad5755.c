/***************************************************************************//**
 *   @file   AD5755.c
 *   @brief  Implementation of AD5755 Driver.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#include "ad5755.h"         // AD5755 definitions.
#include "ad5755_cfg.h"     // AD5755_cfg definitions.
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the device and powers-up all channels. The device is
 *        initialized with the values held by AD5755_InitialSettings structure.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *          Example: -1 - SPI peripheral was not initialized.
 *                    0 - SPI peripheral is initialized.
*******************************************************************************/
int8_t ad5755_init(struct ad5755_dev **device,
		   struct ad5755_init_param init_param)
{
	struct ad5755_dev *dev;
	uint8_t status;
	uint8_t channel = 0;
	uint16_t dac_control_buff[4] = {0, 0, 0, 0};

	dev = (struct ad5755_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->p_ad5755_st = &AD5755_st;
	dev->this_device = init_param.this_device;

	/* GPIO */
	status = no_os_gpio_get(&dev->gpio_ldac, &init_param.gpio_ldac);
	status |= no_os_gpio_get(&dev->gpio_rst, &init_param.gpio_rst);
	status |= no_os_gpio_get(&dev->gpio_clr, &init_param.gpio_clr);
	status |= no_os_gpio_get(&dev->gpio_poc, &init_param.gpio_poc);

	/* GPIO configuration. */
	AD5755_LDAC_OUT;
	AD5755_LDAC_LOW;
	AD5755_RESET_OUT;
	AD5755_RESET_HIGH;
	AD5755_CLEAR_OUT;
	AD5755_CLEAR_LOW;
	AD5755_POC_OUT;
	AD5755_POC_LOW;

	status |= no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	/* Device Setup. */
	/* Configure the POC bit, STATREAD bit and ShtCcLim bit. */
	ad5755_set_control_registers(dev,
				     AD5755_CREG_MAIN,
				     0,
				     (dev->p_ad5755_st->poc_bit * AD5755_MAIN_POC) |
				     (dev->p_ad5755_st->stat_readbit * AD5755_MAIN_STATREAD) |
				     AD5755_MAIN_SHTCCTLIM(dev->p_ad5755_st->sht_cc_lim_bit));

	ad5755_software_reset(dev);
	no_os_mdelay(100);
	/* DC-to-DC configuration. */
	ad5755_set_control_registers(dev,
				     AD5755_CREG_DC_DC,
				     0,
				     (dev->p_ad5755_st->dc_dc_comp_bit * AD5755_DC_DC_COMP) |
				     (AD5755_DC_DC_FREQ(dev->p_ad5755_st->dc_dc_freq_bit)) |
				     (AD5755_DC_DC_PHASE(dev->p_ad5755_st->dc_dc_phase_bit)) |
				     (AD5755_DC_DC_MAX_V(dev->p_ad5755_st->dc_dc_max_vbit)));
	/* Configure the DAC control register on a per channel basis. */
	for(channel = AD5755_DAC_A; channel <= AD5755_DAC_D; channel++) {
		if((dev->this_device == ID_AD5755) || (dev->this_device == ID_AD5755_1)) {
			dac_control_buff[channel] = AD5755_DAC_INT_ENABLE |
						    AD5755_DAC_CLR_EN |
						    dev->p_ad5755_st->rset_bits[channel] |
						    AD5755_DAC_DC_DC |
						    dev->p_ad5755_st->ovrng_bits[channel] |
						    AD5755_DAC_R(AD5755_R_0_5_V);
		} else {
			dac_control_buff[channel] = AD5755_DAC_INT_ENABLE |
						    AD5755_DAC_CLR_EN |
						    dev->p_ad5755_st->rset_bits[channel] |
						    AD5755_DAC_DC_DC |
						    dev->p_ad5755_st->ovrng_bits[channel] |
						    AD5755_DAC_R(AD5755_R_4_20_MA);
		}
		ad5755_set_control_registers(dev,
					     AD5755_CREG_DAC,
					     channel,
					     dac_control_buff[channel]);
	}
	/* Allow at least 200us before enabling the channel output. */
	no_os_mdelay(200);
	/* Enable the channel output. */
	for(channel = AD5755_DAC_A; channel <= AD5755_DAC_D; channel++) {
		/* Write to each DAC data register*/
		ad5755_set_register_value(dev,
					  AD5755_DREG_WR_DAC,
					  channel,
					  0x0000);
		ad5755_set_control_registers(dev,
					     AD5755_CREG_DAC,
					     channel,
					     dac_control_buff[channel] | AD5755_DAC_OUTEN);
	}

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5755_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad5755_remove(struct ad5755_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_ldac);
	ret |= no_os_gpio_remove(dev->gpio_rst);
	ret |= no_os_gpio_remove(dev->gpio_clr);
	ret |= no_os_gpio_remove(dev->gpio_poc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 *                          Example:
 *                           AD5755_RD_DATA_REG(x)
 *                           AD5755_RD_CTRL_REG(x)
 *                           AD5755_RD_GAIN_REG(x)
 *                           AD5755_RD_OFFSET_REG(x)
 *                           AD5755_RD_CODE_REG(x)
 *                           AD5755_RD_SR_CTRL_REG(x)
 *                           AD5755_RD_STATUS_REG
 *                           AD5755_RD_MAIN_CTRL_REG
 *                           AD5755_RD_Dc_DC_CTRL_REG
 *                           x = any of AD5755_DAC_A, .. AD5755_DAC_D
 *
 * @return regValue - Value of the register.
*******************************************************************************/
int32_t ad5755_get_register_value(struct ad5755_dev *dev,
				  uint8_t register_address)
{
	uint8_t buffer[4] = {0, 0, 0, 0};
	uint32_t command = 0;
	int32_t reg_value = 0;
	uint8_t crc = 0;

	command = AD5755_ISR_READ |
		  AD5755_ISR_DUT_AD1(dev->p_ad5755_st->pin_ad1state) |
		  AD5755_ISR_DUT_AD0(dev->p_ad5755_st->pin_ad0state) |
		  AD5755_ISR_RD(register_address);
	buffer[0] = (command & 0xFF0000) >> 16;
	buffer[1] = (command & 0x00FF00) >> 8;
	buffer[2] = (command & 0x0000FF) >> 0;
	if(dev->p_ad5755_st->enable_packet_error_check) {
		buffer[3] = ad5755_check_crc(buffer, 3);
	}
	no_os_spi_write_and_read(dev->spi_desc,
				 buffer,
				 3 + dev->p_ad5755_st->enable_packet_error_check);
	command = AD5755_ISR_WRITE |
		  AD5755_ISR_DUT_AD1(dev->p_ad5755_st->pin_ad1state) |
		  AD5755_ISR_DUT_AD0(dev->p_ad5755_st->pin_ad0state) |
		  AD5755_ISR_NOP;
	buffer[0] = (command & 0xFF0000) >> 16;
	buffer[1] = (command & 0x00FF00) >> 8;
	buffer[2] = (command & 0x0000FF) >> 0;
	if(dev->p_ad5755_st->enable_packet_error_check) {
		buffer[3] = ad5755_check_crc(buffer, 3);
	}
	no_os_spi_write_and_read(dev->spi_desc,
				 buffer,
				 3 + dev->p_ad5755_st->enable_packet_error_check);
	reg_value = ((uint16_t)buffer[1] << 8) + buffer[2];
	/* Check the CRC. */
	if(dev->p_ad5755_st->enable_packet_error_check) {
		crc = ad5755_check_crc(&buffer[1], 3);
		if(crc != AD5755_CRC_CHECK_CODE) {
			reg_value = -1;
		}
	}
	return reg_value;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 *                          Example:
 *                          AD5755_DREG_WR_DAC
 *                          AD5755_DREG_WR_GAIN
 *                          AD5755_DREG_WR_GAIN_ALL
 *                          AD5755_DREG_WR_OFFSET
 *                          AD5755_DREG_WR_OFFSET_ALL
 *                          AD5755_DREG_WR_CLR_CODE
 *                          AD5755_DREG_WR_CTRL_REG
 *
 * @param channel         -  Channel option.
 *                          Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param register_value - Data value to write.
 *
 * @return None.
*******************************************************************************/
uint16_t ad5755_set_register_value(struct ad5755_dev *dev,
				   uint8_t register_address,
				   uint8_t channel,
				   uint16_t register_value)
{
	uint8_t buff[4] = {0, 0, 0, 0};
	uint32_t command = 0;
	uint16_t status_reg = 0;

	command = AD5755_ISR_WRITE |
		  AD5755_ISR_DUT_AD1(dev->p_ad5755_st->pin_ad1state) |
		  AD5755_ISR_DUT_AD0(dev->p_ad5755_st->pin_ad0state) |
		  AD5755_ISR_DREG(register_address) |
		  AD5755_ISR_DAC_AD(channel)|
		  AD5755_ISR_DATA(register_value);
	buff[0] = (command & 0xFF0000) >> 16;
	buff[1] = (command & 0x00FF00) >> 8;
	buff[2] = (command & 0x0000FF) >> 0;
	if(dev->p_ad5755_st->enable_packet_error_check) {
		buff[3] = ad5755_check_crc(buff, 3);
	}
	if(dev->p_ad5755_st->stat_readbit == 0) {
		no_os_spi_write_and_read(dev->spi_desc,
					 buff,
					 3 + dev->p_ad5755_st->enable_packet_error_check);
	} else {
		no_os_spi_write_and_read(dev->spi_desc,
					 buff,
					 3 + dev->p_ad5755_st->enable_packet_error_check);
		status_reg = (buff[1] << 8) + buff[2];
	}

	return status_reg;
}


/***************************************************************************//**
 * @brief Performs a software reset to the device.
 *
 * @param dev - The device structure.
 *
 * @return None.
*******************************************************************************/
void ad5755_software_reset(struct ad5755_dev *dev)
{
	ad5755_set_control_registers(dev,
				     AD5755_CREG_SOFT,
				     0,
				     AD5755_RESET_CODE);
}

/***************************************************************************//**
 * @brief Enables/Disables watchdog timer and sets the timeout period.
 *
 * @param dev        - The device structure.
 * @param wtd_enable - Watchdog enable option.
 *                    Example: 0 - disables watchdog timer
 *                             1 - enabled watchdog timer
 * @param timeout    - Timeout period for the watchdog timer.
 *                    Example: AD5755_WD_5MS
 *                             AD5755_WD_10MS
 *                             AD5755_WD_100MS
 *                             AD5755_WD_200MS
 *
 * @return None.
*******************************************************************************/
void ad5755_watch_dog_setup(struct ad5755_dev *dev,
			    uint8_t wtd_enable,
			    uint8_t timeout)
{
	uint32_t old_main_ctrl_reg = 0;
	uint32_t new_main_ctrl_reg = 0;

	old_main_ctrl_reg = ad5755_get_register_value(dev,
			    AD5755_RD_MAIN_CTRL_REG);
	old_main_ctrl_reg &= ~(AD5755_MAIN_EWD | AD5755_MAIN_WD(3));
	new_main_ctrl_reg = old_main_ctrl_reg |
			    (AD5755_MAIN_EWD * wtd_enable) |
			    AD5755_MAIN_WD(timeout);
	ad5755_set_control_registers(dev,
				     AD5755_CREG_MAIN,
				     0,
				     new_main_ctrl_reg);
}

/***************************************************************************//**
 * @brief Write a "service pulse" to the AD5755 watchdog timer when enabled.
 *
 * @param dev - The device structure.
 *
 * @return None.
*******************************************************************************/
void ad5755_feed_watch_dog_timer(struct ad5755_dev *dev)
{
	ad5755_set_control_registers(dev,
				     AD5755_CREG_SOFT,
				     0,
				     AD5755_SPI_CODE);
}

/***************************************************************************//**
 * @brief Configures one of the control registers.
 *
 * @param dev              - The device structure.
 * @param ctrl_reg_address - Control Register Address.
 *                         Example:
 *                         AD5755_CREG_SLEW
 *                         AD5755_CREG_MAIN
 *                         AD5755_CREG_DAC
 *                         AD5755_CREG_DC_DC
 *                         AD5755_CREG_SOFT
 *
 * @param channel          - Channel option.
 *                         Example: AD5755_DAC_A
 *                                  AD5755_DAC_B
 *                                  AD5755_DAC_C
 *                                  AD5755_DAC_D
 * @param reg_value - Value to be written to the selected Control Register.
 *
 * @return None.
*******************************************************************************/
void ad5755_set_control_registers(struct ad5755_dev *dev,
				  uint8_t  ctrl_reg_address,
				  uint8_t  channel,
				  uint16_t reg_value)
{
	ad5755_set_register_value(dev,
				  AD5755_DREG_WR_CTRL_REG,
				  channel,
				  AD5755_CTRL_CREG(ctrl_reg_address) | reg_value);
}

/***************************************************************************//**
 * @brief Computes the CRC for a data buffer.
 *
 * @param data         - Data buffer.
 * @param bytes_number - Data buffer size in bytes.
 *
 * @return The computed CRC.
*******************************************************************************/
uint8_t ad5755_check_crc(uint8_t* data,
			 uint8_t bytes_number)
{
	uint8_t crc = 0x00;
	uint8_t byte = 0;
	uint8_t bit = 0;

	/* Calculates 8-Bit checksum with given polynomial. */
	for(byte = 0; byte < bytes_number; byte++) {
		crc ^= (data[byte]);
		for(bit = 8; bit > 0; bit--) {
			if (crc & 0x80) {
				crc = (crc << 1) ^ AD5755_CRC_POLYNOMIAL;
			} else {
				crc = (crc << 1);
			}
		}
	}

	return crc;
}

/***************************************************************************//**
 * @brief Allows power-up/down of the dc-to-dc converter, DAC and internal
 *        amplifiers for the selected channel.
 *
 * @param dev        - The device structure.
 * @param channel    - Channel option.
 *                    Example: AD5755_DAC_A
 *                             AD5755_DAC_B
 *                             AD5755_DAC_C
 *                             AD5755_DAC_D
 * @param pwr_status - Power mode.
 *                    Example: 0 - power-down the channel;
 *                             1 - power-up the channel.
 *
 * @return None.
*******************************************************************************/
void ad5755_set_channel_power(struct ad5755_dev *dev,
			      uint8_t channel,
			      uint8_t pwr_status)
{
	uint32_t old_dac_ctrl_reg = 0;
	uint32_t new_dac_ctrl_reg = 0;

	/* Read the content of the DAC Control Register of the selected channel. */
	old_dac_ctrl_reg = ad5755_get_register_value(dev,
			   AD5755_RD_CTRL_REG(channel));
	old_dac_ctrl_reg &= ~(AD5755_DAC_INT_ENABLE |
			      AD5755_DAC_DC_DC |
			      AD5755_DAC_OUTEN);
	new_dac_ctrl_reg = old_dac_ctrl_reg | (pwr_status * (AD5755_DAC_INT_ENABLE |
					       AD5755_DAC_DC_DC |
					       AD5755_DAC_OUTEN));
	ad5755_set_control_registers(dev,
				     AD5755_CREG_DAC,
				     AD5755_DAC_A,
				     new_dac_ctrl_reg);
}

/***************************************************************************//**
 * @brief Sets the range of a channel.
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param range   - Range option.
 *                  Example:
 *                  AD5755_R_0_5_V      - 0 V to 5 V voltage range (default)
 *                  AD5755_R_0_10_V     - 0 V to 10 V voltage range
 *                  AD5755_R_M5_P5_V    - -5 V to +5 V voltage range
 *                  AD5755_R_M10_P10_V  - -10 V to 10 V voltage range
 *                  AD5755_R_4_20_MA    - 4 mA to 20 mA current range
 *                  AD5755_R_0_20_MA    - 0 mA to 20 mA current range
 *                  AD5755_R_0_24_MA    - 0 mA to 24 mA current range
 *
 * @return None.
*******************************************************************************/
void ad5755_set_channel_range(struct ad5755_dev *dev,
			      uint8_t channel,
			      uint8_t range)
{
	uint16_t output_code = 0x0000;
	uint32_t old_dac_ctrl_reg = 0;
	uint32_t new_dac_ctrl_reg = 0;

	/* Read the content of the DAC Control Register of the selected channel. */
	old_dac_ctrl_reg = ad5755_get_register_value(dev,
			   AD5755_RD_CTRL_REG(channel));
	/* Clear the bits that will be modified by this function. */
	old_dac_ctrl_reg &= ~(AD5755_DAC_INT_ENABLE |
			      AD5755_DAC_OUTEN |
			      AD5755_DAC_DC_DC |
			      AD5755_DAC_R(7));
	/* Select the output code before changing the range. */
	if((range == AD5755_R_M5_P5_V) || (range == AD5755_R_M10_P10_V)) {
		output_code = 0x8000;
	}
	/* Set the output code to zero or midscale. */
	ad5755_set_register_value(dev,
				  AD5755_DREG_WR_DAC,
				  channel,
				  output_code);
	/* Set range. */
	new_dac_ctrl_reg = old_dac_ctrl_reg |
			   AD5755_DAC_INT_ENABLE |
			   AD5755_DAC_DC_DC |
			   AD5755_DAC_R(range);
	ad5755_set_control_registers(dev,
				     AD5755_CREG_DAC,
				     channel,
				     new_dac_ctrl_reg);
	/* Set the output code to zero or midscale. */
	ad5755_set_register_value(dev,
				  AD5755_DREG_WR_DAC,
				  channel,
				  output_code);
	/* Enable the output of the channel. */
	new_dac_ctrl_reg |= AD5755_DAC_OUTEN;
	ad5755_set_control_registers(dev,
				     AD5755_CREG_DAC,
				     channel,
				     new_dac_ctrl_reg);
}

/***************************************************************************//**
 * @brief Selects if the channel clears when CLEAR pin is activated.
 *
 * @param dev      - The device structure.
 * @param channel  - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param clear_en - Clear Enable option.
 *                  Example:
 *                   1 - channel clears when the part is cleared;
 *                   0 - channel does not clear when the part is cleared.
 *
 * @return None.
*******************************************************************************/
void ad5755_channel_clear_enable(struct ad5755_dev *dev,
				 uint8_t channel,
				 uint8_t clear_en)
{
	uint32_t old_dac_ctrl_reg = 0;
	uint32_t new_dac_ctrl_reg = 0;

	/* Read the content of the DAC Control Register of the selected channel. */
	old_dac_ctrl_reg = ad5755_get_register_value(dev,
			   AD5755_RD_CTRL_REG(channel));
	/* Clear the CLR_EN bit. */
	old_dac_ctrl_reg &= ~(AD5755_DAC_CLR_EN);
	new_dac_ctrl_reg |= old_dac_ctrl_reg | (clear_en * AD5755_DAC_CLR_EN);
	ad5755_set_control_registers(dev,
				     AD5755_CREG_DAC,
				     channel,
				     new_dac_ctrl_reg);
}

/***************************************************************************//**
 * @brief Configures the Digital Slew Rate Control.
 *
 * @param dev       - The device structure.
 * @param channel   - Channel option.
 *                   Example: AD5755_DAC_A
 *                            AD5755_DAC_B
 *                            AD5755_DAC_C
 *                            AD5755_DAC_D
 * @param sr_en     - Enable/Disable the Slew Rate Control.
 *                   Example: 0 - disable feature;
 *                            1 - enable feature.
 * @param updt_freq - Update Clock Frequency(Hz).
 *                   Example: AD5755_SR_CLK_64K
 *                            AD5755_SR_CLK_32k
 *                            ...
 *                            AD5755_SR_CLK_8
 *                            AD5755_SR_CLK_4
 *                            AD5755_SR_CLK_0_5
 * @param step_size - Step Size (LSBs).
 *                   Example: AD5755_STEP_1
 *                            AD5755_STEP_2
 *                            ...
 *                            AD5755_STEP_128
 *                            AD5755_STEP_256
 *
 * @return None.
*******************************************************************************/
void ad5755_slew_rate_ctrl(struct ad5755_dev *dev,
			   int8_t channel,
			   int8_t sr_en,
			   int8_t updt_freq,
			   int8_t step_size)
{
	ad5755_set_control_registers(dev,
				     AD5755_CREG_SLEW,
				     channel,
				     sr_en * (AD5755_SLEW_SREN) |
				     AD5755_SLEW_SR_CLOCK(updt_freq) |
				     AD5755_SLEW_SR_STEP(step_size));
}

/***************************************************************************//**
 * @brief Sets the output voltage of a channel.
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param voltage - Value to be outputted by the DAC(Volts).
 *
 * @return The actual voltage value that can be outputted by the channel.
*******************************************************************************/
float ad5755_set_voltage(struct ad5755_dev *dev,
			 uint8_t channel,
			 float voltage)
{
	uint32_t offset = 0;
	uint32_t gain = 0;
	uint32_t dac_val = 0;
	int32_t code = 0;
	uint8_t range = 0;
	uint8_t resolution = 0;
	uint32_t range_offset = 0;
	float v_ref = 0;
	float real_voltage = 0;

	if((dev->this_device == ID_AD5755) || (dev->this_device == ID_AD5755_1)) {
		/* Get the offset, gain and range of the selected channel. */
		offset = ad5755_get_register_value(dev,
						   AD5755_RD_OFFSET_REG(channel));
		gain = ad5755_get_register_value(dev,
						 AD5755_RD_GAIN_REG(channel));
		range = ad5755_get_register_value(dev,
						  AD5755_RD_CTRL_REG(channel)) & 0x7;
		switch(range) {
		case AD5755_R_0_5_V : {
			range_offset = 0;
			v_ref = 5.0;
			resolution = 16;
			break;
		}
		case AD5755_R_0_10_V : {
			range_offset = 0;
			v_ref = 10.0;
			resolution = 16;
			break;
		}
		case AD5755_R_M5_P5_V : {
			range_offset = 0x8000;
			v_ref = 5.0;
			resolution = 15;
			break;
		}
		case AD5755_R_M10_P10_V : {
			range_offset = 0x8000;
			v_ref = 10.0;
			resolution = 15;
			break;
		}
		default : {
			range_offset = 0;
			v_ref = 0;
			resolution = 0;
			break;
		}
		}
		/* Compute the binary code from the users voltage value. */
		code = (int32_t)(voltage * (1l << resolution) / v_ref) + range_offset;
		if(code > 0xFFFF) {
			code = 0xFFFF;
		}
		/* Offset and Gain are used to obtain the correct value to be written
		 to the DAC register in order to output the voltage desired by the user.
		*/
		if((int32_t)(code + (1l << 15) - offset) > 0) { // Avoid negative values
			dac_val = (code + (1l << 15) - offset) * (1l << 16) / (gain + 1);
		} else {
			dac_val = 0;
		}

		/* Write to the Data Register of the DAC. */
		ad5755_set_register_value(dev,
					  AD5755_DREG_WR_DAC,
					  channel,
					  dac_val);
		real_voltage = ((int32_t)(dac_val - range_offset) * v_ref) /
			       (1l << resolution);
	}
	return real_voltage;
}

/***************************************************************************//**
 * @brief Sets the output current of a channel.
 *
 * @param dev        - The device structure.
 * @param channel    - Channel option.
 *                    Example: AD5755_DAC_A
 *                             AD5755_DAC_B
 *                             AD5755_DAC_C
 *                             AD5755_DAC_D
 * @param m_acurrent - Value to be outputted by the DAC(milliampere).
 *
 * @return The actual current value that can be outputted by the channel.
*******************************************************************************/
float ad5755_set_current(struct ad5755_dev *dev,
			 uint8_t channel,
			 float m_acurrent)
{
	int32_t offset = 0;
	int32_t gain = 0;
	int32_t range = 0;
	int32_t dac_val = 0;
	int32_t code = 0;
	int8_t range_offset = 0;
	float i_ref = 0;
	float real_current = 0;

	/* Get the offset, gain and range of the selected channel. */
	offset = ad5755_get_register_value(dev,
					   AD5755_RD_OFFSET_REG(channel));
	gain = ad5755_get_register_value(dev,
					 AD5755_RD_GAIN_REG(channel));
	range = ad5755_get_register_value(dev,
					  AD5755_RD_CTRL_REG(channel)) & 0x7;
	switch(range) {
	case AD5755_R_4_20_MA : {
		i_ref = 16.0;        // mA
		range_offset = 4;    // mA
		break;
	}
	case AD5755_R_0_20_MA : {
		i_ref = 20.0;        // mA
		range_offset = 0;    // mA
		break;
	}
	case AD5755_R_0_24_MA : {
		i_ref = 24.0;        // mA
		range_offset = 0;    // mA
		break;
	}
	default : {
		i_ref = 1;
		range_offset = 0;
		break;
	}
	}
	/* Compute the binary code from the value(mA) provided by user. */
	code = (int32_t)((m_acurrent - range_offset) * (1l << 16) / i_ref);
	if(code > 0xFFFF) {
		code = 0xFFFF;
	}
	/* Offset and Gain are used to obtain the correct value to be written to the
	   DAC register in order to output the current desired by the user. */
	if((code + (1l << 15) - offset) > 0) {  // Avoid negative values
		dac_val = (code + (1l << 15) - offset) * (1l << 16) / (gain + 1);
	} else {
		dac_val = 0;
	}
	/* Write to the Data Register of the DAC. */
	ad5755_set_register_value(dev,
				  AD5755_DREG_WR_DAC,
				  channel,
				  dac_val);

	real_current = (code * i_ref / (float)(1l << 16)) + range_offset;

	return real_current;
}
