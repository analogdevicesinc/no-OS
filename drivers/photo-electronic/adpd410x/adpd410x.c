/***************************************************************************//**
 *   @file   adpd410x.c
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include <stdint.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "adpd410x.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param dev - Device handler.
 * @param address - Register address.
 * @param data - Pointer to the register value container.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_reg_read(struct adpd410x_dev *dev, uint16_t address,
			  uint16_t *data)
{
	int32_t ret;
	uint8_t buff[] = {0, 0};

	ret = adpd410x_reg_read_bytes(dev, address, (uint8_t *) buff, 2);
	if (ret != 0) {
		return ret;
	}

	*data = ((uint16_t)buff[0] << 8) & 0xff00;
	*data |= buff[1] & 0xff;

	return 0;
}

/**
 * @brief Read a specified number of bytes from device register.
 * @param dev - Device handler.
 * @param address - Register address.
 * @param data - Pointer to the register value container.
 * @param num_bytes - number of bytes to read. Max 255 for ADPD4101 (I2C).
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_reg_read_bytes(struct adpd410x_dev *dev, uint16_t address,
				uint8_t *data, uint16_t num_bytes)
{
	int32_t ret;
	uint8_t i;
	uint8_t *buff;

	switch (dev->dev_type) {
	case ADPD4100:
		buff = (uint8_t *) no_os_calloc(num_bytes + 2, sizeof(*buff));
		buff[0] = no_os_field_get(ADPD410X_UPPDER_BYTE_SPI_MASK, address);
		buff[1] = (address << 1) & ADPD410X_LOWER_BYTE_SPI_MASK;

		ret = no_os_spi_write_and_read(dev->dev_ops.spi_phy_dev, buff, num_bytes + 2);
		if(ret != 0) {
			no_os_free(buff);
			return ret;
		}
		for (i = 0; i < num_bytes; i++) {
			data[i] = buff[i + 2];
		}
		break;
	case ADPD4101:
		// Number of bytes for an I2C read is an 8-bit number, or at most 255
		if (num_bytes > 255)
			return -1;
		buff = (uint8_t *) no_os_calloc(2, sizeof(*buff));
		buff[0] = no_os_field_get(ADPD410X_UPPDER_BYTE_I2C_MASK, address);
		buff[0] |= 0x80;
		buff[1] = address & ADPD410X_LOWER_BYTE_I2C_MASK;

		/* No stop bit */
		ret = no_os_i2c_write(dev->dev_ops.i2c_phy_dev, buff, 2, 0);
		if(ret != 0) {
			no_os_free(buff);
			return ret;
		}
		ret = no_os_i2c_read(dev->dev_ops.i2c_phy_dev, data, (uint8_t) num_bytes, 1);
		if(ret != 0) {
			no_os_free(buff);
			return ret;
		}
		break;
	default:
		return -1;
	}
	no_os_free(buff);
	return 0;
}

/**
 * @brief Write device register.
 * @param dev - Device handler.
 * @param address - Register address.
 * @param data - New register value.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_reg_write(struct adpd410x_dev *dev, uint16_t address,
			   uint16_t data)
{
	uint8_t buff[] = {0, 0, 0, 0};

	switch (dev->dev_type) {
	case ADPD4100:
		buff[0] = no_os_field_get(ADPD410X_UPPDER_BYTE_SPI_MASK, address);
		buff[1] = ((address << 1) & ADPD410X_LOWER_BYTE_SPI_MASK) | 0x1 ;
		buff[2] = no_os_field_get(0xff00, data);
		buff[3] = data & 0xff;

		return no_os_spi_write_and_read(dev->dev_ops.spi_phy_dev, buff, 4);
	case ADPD4101:
		buff[0] = no_os_field_get(ADPD410X_UPPDER_BYTE_I2C_MASK, address);
		buff[0] |= 0x80;
		buff[1] = address & ADPD410X_LOWER_BYTE_I2C_MASK;
		buff[2] = no_os_field_get(0xff00, data);
		buff[3] = data & 0xff;

		return no_os_i2c_write(dev->dev_ops.i2c_phy_dev, buff, 4, 1);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Do a read and write of a register to update only part of a register.
 * @param dev - Device handler.
 * @param address - Address of the register.
 * @param data - Value to be written to the device.
 * @param mask - Mask of the bit field to update.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_reg_write_mask(struct adpd410x_dev *dev, uint16_t address,
				uint16_t data, uint16_t mask)
{
	int32_t ret;
	uint16_t reg_val;
	uint32_t bit_pos;

	ret = adpd410x_reg_read(dev, address, &reg_val);
	if (ret != 0)
		return -1;
	reg_val &= ~mask;
	bit_pos = no_os_find_first_set_bit((uint32_t)mask);
	reg_val |= (data << bit_pos) & mask;

	return adpd410x_reg_write(dev, address, reg_val);
}

/**
 * @brief Update the clocking option of the device.
 * @param dev - Device handler.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t adpd410x_get_clk_opt(struct adpd410x_dev *dev)
{
	int32_t ret;
	uint16_t reg_temp;

	ret = adpd410x_reg_read(dev, ADPD410X_REG_SYS_CTL, &reg_temp);
	if(ret != 0)
		return ret;
	dev->clk_opt = (reg_temp & BITM_SYS_CTL_ALT_CLOCKS) >>
		       BITP_SYS_CTL_ALT_CLOCKS;

	return 0;
}

/**
 * @brief Do a software reset.
 * @param dev - Device handler.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_reset(struct adpd410x_dev *dev)
{
	int32_t ret;

	ret = adpd410x_reg_write_mask(dev, ADPD410X_REG_SYS_CTL, 1,
				      BITM_SYS_CTL_SW_RESET);
	if(ret != 0)
		return ret;

	return adpd410x_get_clk_opt(dev);
}

/**
 * @brief Set operation mode.
 * @param dev - Device handler.
 * @param mode - New operation mode.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_set_opmode(struct adpd410x_dev *dev,
			    enum adpd410x_opmode mode)
{
	return adpd410x_reg_write_mask(dev, ADPD410X_REG_OPMODE, mode,
				       BITM_OPMODE_OP_MODE);
}

/**
 * @brief Get operation mode.
 * @param dev - Device handler.
 * @param mode - Operation mode.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_get_opmode(struct adpd410x_dev *dev,
			    enum adpd410x_opmode *mode)
{
	int32_t ret;
	uint16_t data;

	ret = adpd410x_reg_read(dev, ADPD410X_REG_OPMODE, &data);
	if (ret != 0)
		return ret;

	*mode = data & BITM_OPMODE_OP_MODE;

	return ret;
}

/**
 * @brief Set number of active time slots.
 * @param dev - Device handler.
 * @param timeslot_no - Last time slot to be enabled.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_set_last_timeslot(struct adpd410x_dev *dev,
				   enum adpd410x_timeslots timeslot_no)
{
	return adpd410x_reg_write_mask(dev, ADPD410X_REG_OPMODE,
				       timeslot_no, BITM_OPMODE_TIMESLOT_EN);
}

/**
 * @brief Get number of active time slots.
 * @param dev - Device handler.
 * @param timeslot_no - Last time slot enabled.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_get_last_timeslot(struct adpd410x_dev *dev,
				   enum adpd410x_timeslots *timeslot_no)
{
	int32_t ret;
	uint16_t data;

	ret = adpd410x_reg_read(dev, ADPD410X_REG_OPMODE, &data);
	if (ret != 0)
		return ret;

	*timeslot_no = (data & BITM_OPMODE_TIMESLOT_EN) >> 8;

	return ret;
}

/**
 * @brief Set device sampling frequency.
 * @param dev - Device handler.
 * @param sampling_freq - New sampling frequency.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_set_sampling_freq(struct adpd410x_dev *dev,
				   uint32_t sampling_freq)
{
	int32_t ret;
	uint32_t reg_load;
	uint16_t reg_temp;

	if((dev->clk_opt == ADPD410X_INTLFO_INTHFO) ||
	    (dev->clk_opt == ADPD410X_INTLFO_EXTHFO)) {
		ret = adpd410x_reg_read(dev, ADPD410X_REG_SYS_CTL, &reg_temp);
		if(ret != 0)
			return ret;
		if(reg_temp & BITP_SYS_CTL_LFOSC_SEL)
			reg_load = ADPD410X_LOW_FREQ_OSCILLATOR_FREQ1;
		else
			reg_load = ADPD410X_LOW_FREQ_OSCILLATOR_FREQ2;
	} else {
		reg_load = dev->ext_lfo_freq;
	}

	reg_load /= sampling_freq;
	ret = adpd410x_reg_write(dev, ADPD410X_REG_TS_FREQ,
				 (reg_load & 0xFFFF));
	if(ret != 0)
		return ret;
	return adpd410x_reg_write(dev, ADPD410X_REG_TS_FREQH,
				  ((reg_load & 0x7F0000) >> 16));
}

/**
 * @brief Get device sampling frequency.
 * @param dev - Device handler.
 * @param sampling_freq - New sampling frequency.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_get_sampling_freq(struct adpd410x_dev *dev,
				   uint32_t *sampling_freq)
{
	int32_t ret;
	uint32_t reg_load;
	uint16_t reg_temp;

	if ((dev->clk_opt == ADPD410X_INTLFO_INTHFO) ||
	    (dev->clk_opt == ADPD410X_INTLFO_EXTHFO)) {
		ret = adpd410x_reg_read(dev, ADPD410X_REG_SYS_CTL, &reg_temp);
		if (ret != 0)
			return ret;

		if (reg_temp & BITP_SYS_CTL_LFOSC_SEL)
			reg_load = ADPD410X_LOW_FREQ_OSCILLATOR_FREQ1;
		else
			reg_load = ADPD410X_LOW_FREQ_OSCILLATOR_FREQ2;
	} else {
		reg_load = dev->ext_lfo_freq;
	}

	ret = adpd410x_reg_read(dev, ADPD410X_REG_TS_FREQ, &reg_temp);
	if (ret != 0)
		return ret;

	*sampling_freq = reg_temp;

	ret = adpd410x_reg_read(dev, ADPD410X_REG_TS_FREQH, &reg_temp);
	if (ret != 0)
		return ret;

	*sampling_freq = reg_load / (*sampling_freq | ((reg_temp & 0x7F) << 16));

	return ret;
}

/**
 * @brief Setup an active time slot.
 * @param dev - Device handler.
 * @param timeslot_no - Time slot ID to setup.
 * @param init - Pointer to the time slot initialization structure.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_timeslot_setup(struct adpd410x_dev *dev,
				enum adpd410x_timeslots timeslot_no,
				struct adpd410x_timeslot_init *init)
{
	int32_t ret;
	uint16_t data;

	ret = adpd410x_reg_read(dev, ADPD410X_REG_OPMODE, &data);
	if(ret != 0)
		return ret;
	if(((data & BITM_OPMODE_TIMESLOT_EN) >> BITP_OPMODE_TIMESLOT_EN) <
	    timeslot_no)
		return -EINVAL;

	/* Enable channel 2 */
	ret = adpd410x_reg_read(dev, ADPD410X_REG_TS_CTRL(timeslot_no), &data);
	if(ret != 0)
		return ret;
	if(init->enable_ch2)
		data |= BITM_TS_CTRL_A_CH2_EN;
	else
		data &= ~BITM_TS_CTRL_A_CH2_EN;
	ret = adpd410x_reg_write(dev, ADPD410X_REG_TS_CTRL(timeslot_no), data);
	if(ret != 0)
		return ret;

	/* Setup inputs */
	data = init->ts_inputs.option << (init->ts_inputs.pair * 4);
	ret = adpd410x_reg_write(dev, ADPD410X_REG_INPUTS(timeslot_no), data);
	if(ret != 0)
		return ret;

	/* Set precondition PD */
	ret = adpd410x_reg_write_mask(dev, ADPD410X_REG_CATHODE(timeslot_no),
				      init->precon_option,
				      BITM_CATHODE_A_PRECON);
	if(ret != 0)
		return ret;

	/**
	 *  Set TIA VREF and TRIM options. The 0xE000 is writing reserved bits
	 *  as specified in the datasheet.
	 */
	data = init->afe_trim_opt << BITP_AFE_TRIM_A_AFE_TRIM_VREF |
	       init->vref_pulse_opt << BITP_AFE_TRIM_A_VREF_PULSE_VAL |
	       init->chan2 << BITP_AFE_TRIM_A_TIA_GAIN_CH2 |
	       init->chan1 << BITP_AFE_TRIM_A_TIA_GAIN_CH1;
	ret = adpd410x_reg_write(dev, ADPD410X_REG_AFE_TRIM(timeslot_no), data);
	if(ret != 0)
		return ret;

	/* Set LED pattern */
	data = init->pulse4_subtract << BITP_PATTERN_A_SUBTRACT |
	       init->pulse4_reverse << BITP_PATTERN_A_REVERSE_INTEG;
	ret = adpd410x_reg_write(dev, ADPD410X_REG_PATTERN(timeslot_no), data);
	if(ret != 0)
		return ret;

	/* Set bytes number for time slot */
	data = (init->byte_no << BITP_DATA1_A_SIGNAL_SIZE) &
	       BITM_DATA1_A_SIGNAL_SIZE;
	ret = adpd410x_reg_write(dev, ADPD410X_REG_DATA1(timeslot_no), data);
	if(ret != 0)
		return ret;

	/* Set decimate factor */
	data = (init->dec_factor << BITP_DECIMATE_A_DECIMATE_FACTOR) &
	       BITM_DECIMATE_A_DECIMATE_FACTOR;
	ret = adpd410x_reg_write(dev, ADPD410X_REG_DECIMATE(timeslot_no), data);
	if(ret != 0)
		return ret;

	/* Set LED power */
	data = init->led1.value |
	       (init->led2.value << BITP_LED_POW12_A_LED_CURRENT2);
	ret = adpd410x_reg_write(dev, ADPD410X_REG_LED_POW12(timeslot_no),
				 data);
	if(ret != 0)
		return ret;
	data = init->led3.value |
	       (init->led4.value << BITP_LED_POW34_A_LED_CURRENT4);
	ret = adpd410x_reg_write(dev, ADPD410X_REG_LED_POW34(timeslot_no),
				 data);
	if(ret != 0)
		return ret;

	/* Set ADC cycle and repeat */
	if(init->adc_cycles == 0)
		init->adc_cycles = 1;
	if(init->repeats_no == 0)
		init->repeats_no = 1;
	data = init->repeats_no | (init->adc_cycles << BITP_COUNTS_A_NUM_INT);
	return adpd410x_reg_write(dev, ADPD410X_REG_COUNTS(timeslot_no),
				  data);
}

/**
 * @brief Get number of bytes in the device FIFO.
 * @param dev - Device handler.
 * @param bytes - Pointer to the byte count container.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_get_fifo_bytecount(struct adpd410x_dev *dev, uint16_t *bytes)
{
	int32_t ret;

	ret = adpd410x_reg_read(dev, ADPD410X_REG_FIFO_STATUS, bytes);
	if(ret != 0)
		return ret;

	*bytes &= BITM_INT_STATUS_FIFO_FIFO_BYTE_COUNT;

	return 0;
}

/**
 * @brief Get a data packet containing data from all active time slots and
 *        channels. adpd410x_get_data() helper function.
 * @param dev - Device handler.
 * @param data - Pointer to the data container.
 * @param no_slots - Number of active time slots.
 * @param dual_chan - Mask showing which of the active time slots are dual
 *                    channeled and which are single channeled.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t adpd410x_get_data_packet(struct adpd410x_dev *dev,
					uint32_t *data, uint8_t no_slots,
					uint16_t dual_chan)
{
	int32_t ret;
	uint16_t temp_data = 0;
	uint16_t sample_index;
	int8_t i, got_one = 0;
	uint8_t *slot_bytecount_tab;

	slot_bytecount_tab = (uint8_t *)no_os_calloc(no_slots,
			     sizeof (*slot_bytecount_tab));
	if (!slot_bytecount_tab)
		return -ENOMEM;
	for(i = 0; i < no_slots; i++) {
		ret = adpd410x_reg_read(dev, ADPD410X_REG_DATA1(i), &temp_data);
		if(ret != 0)
			goto error_slot;
		slot_bytecount_tab[i] = (temp_data & BITM_DATA1_A_SIGNAL_SIZE);
	}

	i = 0;
	sample_index = 0;
	do {
		ret = adpd410x_read_fifo(dev, data + sample_index, 1, slot_bytecount_tab[i]);
		if (ret != 0)
			goto error_slot;

		sample_index++;
		if(((dual_chan & (1 << i)) != 0) && (got_one == 0)) {
			got_one = 1;
		} else {
			i++;
			got_one = 0;
		}
	} while(i < no_slots);

	no_os_free(slot_bytecount_tab);
	return 0;

error_slot:
	no_os_free(slot_bytecount_tab);

	return ret;
}

/**
 * @brief Reads a certain number of bytes from the fifo and stores in data
 *        Used to read a large amount of data from the fifo efficiently (using
 *        as few register reads as possible.)
 * @param dev - Device handler.
 * @param data - Pointer to the data container.
 * @param num_samples - number of samples to read
 * @param datawidth - number of bytes per sample
 * @return 0 in case of success, -1 or an error code otherwise.
 */
int32_t adpd410x_read_fifo(struct adpd410x_dev *dev, uint32_t *data,
			   uint16_t num_samples,
			   uint8_t datawidth)
{
	int32_t ret = 0;
	uint8_t *data_byte_buff, i, j;
	uint16_t next_packet_size, bytes_read = 0,
				   total_bytes = num_samples * datawidth;
	if (datawidth > 4 || total_bytes > ADPD410X_FIFO_DEPTH || data == NULL)
		return -1;

	data_byte_buff = (uint8_t *) no_os_calloc(total_bytes,
			 sizeof (*data_byte_buff));
	if (!data_byte_buff)
		return -ENOMEM;

	while (bytes_read < total_bytes) {
		next_packet_size = total_bytes - bytes_read;
		// Can read a maximum of 255 bytes at once for i2c
		if (dev->dev_type == ADPD4101 && next_packet_size > 255)
			next_packet_size = 255;
		ret = adpd410x_reg_read_bytes(dev, ADPD410X_REG_FIFO_DATA,
					      data_byte_buff + bytes_read,
					      next_packet_size);
		if(ret != 0)
			goto fifo_free_return;

		bytes_read += next_packet_size;
	}

	i = 0;
	for (j = 0; j < num_samples; j++) {
		switch(datawidth) {
		case 0:
			break;
		case 1:
			data[j] = data_byte_buff[i++];
			break;
		case 2:
			data[j]  = data_byte_buff[i++] << 8;
			data[j] |= data_byte_buff[i++];
			break;
		case 3:
			data[j]  = data_byte_buff[i++] << 8;
			data[j] |= data_byte_buff[i++];
			data[j] |= data_byte_buff[i++] << 16;
			break;
		case 4:
			data[j]  = data_byte_buff[i++] << 8;
			data[j] |= data_byte_buff[i++];
			data[j] |= data_byte_buff[i++] << 24;
			data[j] |= data_byte_buff[i++] << 16;
			break;
		}
	}

fifo_free_return:
	no_os_free(data_byte_buff);
	return ret;
}

/**
 * @brief Get a full data packet from the device containing data from all active
 *        time slots.
 * @param dev - Device handler.
 * @param data - Pointer to the data container.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_get_data(struct adpd410x_dev *dev, uint32_t *data)
{
	int32_t ret;
	int8_t i;
	uint16_t temp_data, dual_chan = 0;
	uint8_t ts_no;

	ret = adpd410x_reg_read(dev, ADPD410X_REG_OPMODE, &temp_data);
	if(ret != 0)
		return ret;
	ts_no = ((temp_data & BITM_OPMODE_TIMESLOT_EN) >>
		 BITP_OPMODE_TIMESLOT_EN) + 1;

	for(i = 0; i < ts_no; i++) {
		ret = adpd410x_reg_read(dev, ADPD410X_REG_TS_CTRL(i),
					&temp_data);
		if(ret != 0)
			return ret;
		if((temp_data & BITM_TS_CTRL_A_CH2_EN) != 0)
			dual_chan |= (1 << i);
	}

	return adpd410x_get_data_packet(dev, data, ts_no, dual_chan);
}

/**
 * @brief Setup the device and the driver.
 * @param device - Pointer to the device handler.
 * @param init_param - Pointer to the initialization structure.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_setup(struct adpd410x_dev **device,
		       struct adpd410x_init_param *init_param)
{
	int32_t ret;
	struct adpd410x_dev *dev;
	uint16_t reg_temp;

	dev = no_os_calloc(1, sizeof *dev);
	if(!dev)
		return -ENOMEM;

	dev->dev_type = init_param->dev_type;
	dev->ext_lfo_freq = init_param->ext_lfo_freq;

	if(dev->dev_type == ADPD4100)
		ret = no_os_spi_init(&dev->dev_ops.spi_phy_dev,
				     &init_param->dev_ops_init.spi_phy_init);
	else
		ret = no_os_i2c_init(&dev->dev_ops.i2c_phy_dev,
				     &init_param->dev_ops_init.i2c_phy_init);
	if(ret != 0)
		goto error_dev;

	ret = no_os_gpio_get(&dev->gpio0, &init_param->gpio0);
	if(ret != 0)
		goto error_phy;
	ret = no_os_gpio_get(&dev->gpio1, &init_param->gpio1);
	if(ret != 0)
		goto error_gpio0;
	ret = no_os_gpio_get(&dev->gpio2, &init_param->gpio2);
	if(ret != 0)
		goto error_gpio1;
	ret = no_os_gpio_get(&dev->gpio3, &init_param->gpio3);
	if(ret != 0)
		goto error_gpio2;

	ret = adpd410x_reset(dev);
	if(ret != 0)
		goto error_gpio3;

	/* Do power-up sequence described in errata. */
	ret = no_os_gpio_direction_output(dev->gpio0, NO_OS_GPIO_HIGH);
	if(ret != 0)
		goto error_gpio3;
	ret = adpd410x_reg_write(dev, 0xB5, 0x04);
	if(ret != 0)
		goto error_gpio3;
	ret = adpd410x_reg_write(dev, 0xB5, 0x00);
	if(ret != 0)
		goto error_gpio3;
	ret = no_os_gpio_set_value(dev->gpio0, NO_OS_GPIO_HIGH_Z);
	if(ret != 0)
		goto error_gpio3;

	ret = adpd410x_reg_read(dev, ADPD410X_REG_CHIP_ID, &reg_temp);
	if(ret != 0)
		goto error_gpio3;
	if((reg_temp & BITM_CHIP_ID) != ADPD410X_CHIP_ID)
		goto error_gpio3;

	dev->clk_opt = init_param->clk_opt;


	ret = adpd410x_reg_write_mask(dev, ADPD410X_REG_SYS_CTL, dev->clk_opt,
				      BITM_SYS_CTL_ALT_CLOCKS);
	if(ret != 0)
		goto error_gpio3;

	/**
	 * Enable the 1MHz oscillator if the internal low frequency oscillator
	 * is used.
	 */
	if((dev->clk_opt == ADPD410X_INTLFO_INTHFO) ||
	    (dev->clk_opt == ADPD410X_INTLFO_EXTHFO)) {
		ret = adpd410x_reg_read(dev, ADPD410X_REG_SYS_CTL, &reg_temp);
		if(ret != 0)
			goto error_gpio3;
		reg_temp |= (BITM_SYS_CTL_OSC_1M_EN | BITM_SYS_CTL_LFOSC_SEL);
		ret = adpd410x_reg_write(dev, ADPD410X_REG_SYS_CTL, reg_temp);
		if(ret != 0)
			goto error_gpio3;
	}

	*device = dev;

	return 0;

error_gpio3:
	no_os_gpio_remove(dev->gpio3);
error_gpio2:
	no_os_gpio_remove(dev->gpio2);
error_gpio1:
	no_os_gpio_remove(dev->gpio1);
error_gpio0:
	no_os_gpio_remove(dev->gpio0);
error_phy:
	if(dev->dev_type == ADPD4100)
		no_os_spi_remove(dev->dev_ops.spi_phy_dev);
	else
		no_os_i2c_remove(dev->dev_ops.i2c_phy_dev);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free memory allocated by adpd410x_setup().
 * @param dev - Device handler.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd410x_remove(struct adpd410x_dev *dev)
{
	int32_t ret;

	if(!dev)
		return -EINVAL;

	if(dev->dev_type == ADPD4100)
		ret = no_os_spi_remove(dev->dev_ops.spi_phy_dev);
	else
		ret = no_os_i2c_remove(dev->dev_ops.i2c_phy_dev);
	if(ret != 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio0);
	if(ret != 0)
		return ret;
	ret = no_os_gpio_remove(dev->gpio1);
	if(ret != 0)
		return ret;
	ret = no_os_gpio_remove(dev->gpio2);
	if(ret != 0)
		return ret;
	ret = no_os_gpio_remove(dev->gpio3);
	if(ret != 0)
		return ret;

	no_os_free(dev);

	return 0;
}
