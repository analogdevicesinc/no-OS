/***************************************************************************//**
 *   @file   AD9467.c
 *   @brief  Implementation of AD9467 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
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
#include "ad9467.h"
#include "no_os_alloc.h"

/***************************************************************************//**
 * @brief Configures the test mode and the output mode to a default state.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9467_setup(struct ad9467_dev **device,
		     struct ad9467_init_param init_param)
{
	int32_t ret = 0;
	struct ad9467_dev *dev;

	dev = (struct ad9467_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* Disable test mode. */
	ret = ad9467_write(dev, AD9467_REG_TEST_IO, 0x00);
	if(ret < 0)
		return ret;
	/* Enable digital output, disable output invert and set data format to
	   binary offset. */
	ret = ad9467_write(dev, AD9467_REG_OUT_MODE, 0x08);
	if(ret < 0)
		return ret;
	ret = ad9467_transfer(dev);
	if(ret < 0)
		return ret;

	*device = dev;

	return 0;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad9467_setup().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9467_remove(struct ad9467_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}

/**************************************************************************//**
* @brief Writes data into a register.
*
* @param dev      - The device structure.
* @param reg_addr - The address of the register to be written.
* @param reg_val  - The value to be written into the register.
*
* @return Returns 0 in case of success or negative error code.
******************************************************************************/
int32_t ad9467_write(struct ad9467_dev *dev,
		     uint16_t reg_addr,
		     uint8_t reg_val)
{
	int32_t ret;
	uint8_t write_buffer[3];

	reg_addr += AD9467_WRITE;

	write_buffer[0] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	write_buffer[1] = (uint8_t)(reg_addr & 0x00FF);
	write_buffer[2] = reg_val;

	ret = no_os_spi_write_and_read(dev->spi_desc, write_buffer, 3);

	return ret;
}

/**************************************************************************//**
* @brief Reads data from a register.
*
* @param dev      - The device structure.
* @param reg_addr - The address of the register to be read.
* @param reg_val  - The value read from the register.
*
* @return Returns the read data or negative error code.
******************************************************************************/
int32_t ad9467_read(struct ad9467_dev *dev,
		    uint16_t reg_addr,
		    uint8_t *reg_val)
{
	int32_t ret;
	uint8_t write_buffer[3];

	reg_addr += AD9467_READ;

	write_buffer[0] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	write_buffer[1] = (uint8_t)(reg_addr & 0x00FF);
	write_buffer[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, write_buffer, 3);

	*reg_val = write_buffer[2];

	return ret;
}

/***************************************************************************//**
 * @brief Sets a bit or a group of bits inside a register without modifying
 *        the other bits.
 *
 * @param dev              - The device structure.
 * @param register_address - The address of the register to be written.
 * @param bits_value       - The value of the bit/bits.
 * @param mask             - The bit/bits position in the register.
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
uint32_t ad9467_set_bits_to_reg(struct ad9467_dev *dev,
				uint16_t register_address,
				uint8_t bits_value,
				uint8_t mask)
{
	uint8_t reg_value = 0;
	int32_t ret = 0;

	ret = ad9467_read(dev, register_address, &reg_value);
	if(ret < 0)
		return ret;
	reg_value &= (~mask);
	reg_value |= bits_value;
	ret = ad9467_write(dev, register_address, reg_value);
	if(ret < 0)
		return ret;

	return 0;
}

/***************************************************************************//**
 * @brief Configures the power mode.
 *
 * @param dev      - The device structure.
 * @param mode     - The power mode.
 *			Example: 00 � normal operation(default);
 *			         01 � full power-down;
 * @param ret_mode - Return parameter of the set power mode.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_pwr_mode(struct ad9467_dev *dev,
			int32_t mode,
			int32_t *ret_mode)
{
	int32_t ret = 0;
	uint8_t read;

	if((mode == 0) || (mode == 1)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_MODES,
					     AD9467_MODES_INT_PD_MODE(mode),
					     AD9467_MODES_INT_PD_MODE(0x3));
		if(ret < 0) {
			return ret;
		}
		*ret_mode = mode;
	} else {
		ret = ad9467_read(dev, AD9467_REG_MODES, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_mode = (read & AD9467_MODES_INT_PD_MODE(0x3)) == 0x01;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the ADC's test mode.
 *
 * @param dev      - The device structure.
 * @param mode     - ADC test mode.
 *                   Example: 0 -> off(default)
 *                            1 -> midscale short
 *                            2 -> +FS short
 *                            3 -> -FS short
 *                            4 -> checkerboard output
 *                            5 -> PN 23 sequence
 *                            6 -> PN 9 sequence
 *                            7 -> one/zero word toggle
 * @param ret_mode - Return parameter of the set test mode.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_test_mode(struct ad9467_dev *dev,
			 int32_t mode,
			 int32_t *ret_mode)
{
	int32_t ret = 0;
	uint8_t read;

	if((mode >= 0) && (mode <= 7)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_TEST_IO,
					     AD9467_TEST_IO_OUT_TEST(mode),
					     AD9467_TEST_IO_OUT_TEST(0xF));
		if(ret < 0) {
			return ret;
		}
		*ret_mode = mode;
	} else {
		ret = ad9467_read(dev, AD9467_REG_TEST_IO, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_mode = (read & AD9467_TEST_IO_OUT_TEST(0xF)) >> 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets (1) or clears (0) the reset short PN sequence bit(PN9).
 *
 * @param dev      - The device structure.
 * @param rst      - Reset option.
 *                   Example: 1 - The PN sequence is held in reset;
 *                            0 - The PN sequence resumes from the seed
 *                                value(0x92).
 * @param ret_stat - Return parameter of the set PN9 status.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_reset_pn9(struct ad9467_dev *dev,
			 int32_t rst,
			 int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((rst == 0) || (rst == 1)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_TEST_IO,
					     rst * AD9467_TEST_IO_RST_PN_SHORT,
					     AD9467_TEST_IO_RST_PN_SHORT);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = rst;
	} else {
		ret = ad9467_read(dev, AD9467_REG_TEST_IO, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_TEST_IO_RST_PN_SHORT) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets (1) or clears (0) the reset long PN sequence bit(PN23).
 *
 * @param dev      - The device structure.
 * @param rst      - Reset option.
 *                   Example: 1 - The PN sequence is held in reset;
 *                            0 - The PN sequence resumes from the seed
 *                                value(0x3AFF).
 * @param ret_stat - Return parameter of the set PN23 status.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_reset_pn23(struct ad9467_dev *dev,
			  int32_t rst,
			  int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((rst == 0) || (rst == 1)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_TEST_IO,
					     rst * AD9467_TEST_IO_RST_PN_LONG,
					     AD9467_TEST_IO_RST_PN_LONG);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = rst;
	} else {
		ret = ad9467_read(dev, AD9467_REG_TEST_IO, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_TEST_IO_RST_PN_LONG) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Enables (1) or disables (0) the external voltage reference.
 *
 * @param dev      - The device structure.
 * @param en       - Enable option.
 *                   Example: 1 - Enables the external reference;
 *                            0 - Disables the external reference(default).
 * @param ret_stat - Return parameter of the set external reference enable
 *                   state.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_external_ref(struct ad9467_dev *dev,
			    int32_t en,
			    int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((en == 0) || (en == 1)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_ADC_INPUT,
					     en * AD9467_ADC_INPUT_XVREF,
					     AD9467_ADC_INPUT_XVREF);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = en;
	} else {
		ret = ad9467_read(dev, AD9467_REG_ADC_INPUT, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_ADC_INPUT_XVREF) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Disconnects (1) or connects (0) the analog input from or to the
 *        the ADC channel.
 *
 * @param dev      - The device structure.
 * @param en       - Enable option.
 *                   Example: 1 - Disconnects the analog input.
 *                            0 - Connects the analog input(default).
 * @param ret_stat - Return parameter of the set analog input disconnect status.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_analog_input_disconnect(struct ad9467_dev *dev,
				       int32_t en,
				       int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((en == 0) || (en == 1)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_ADC_INPUT,
					     en * AD9467_ADC_INPUT_ANALOG_DSCN,
					     AD9467_ADC_INPUT_ANALOG_DSCN);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = en;
	} else {
		ret = ad9467_read(dev, AD9467_REG_ADC_INPUT, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_ADC_INPUT_ANALOG_DSCN) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the offset adjustment.
 *
 * @param dev      - The device structure.
 * @param adj      - The offset adjust value in LSBs from +127 to -128.
 * @param ret_stat - Return parameter of the set offset adjustment.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_offset_adj(struct ad9467_dev *dev,
			  int32_t adj,
			  int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((adj >= -128) && (adj <= 127)) {
		ret = ad9467_write(dev, AD9467_REG_OFFSET, adj);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = adj;
	} else {
		ret = ad9467_read(dev, AD9467_REG_OFFSET, &read);
		*ret_stat = read;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Disables (1) or enables (0) the data output.
 *
 * @param dev      - The device structure.
 * @param en       - Enable option.
 *		     Example: 1 - Disables the data output;
 *			      0 - Enables the data output(default).
 * @param ret_stat - Return parameter of the set output disable state.
 *
 * @return  Error code.
*******************************************************************************/
int32_t ad9467_output_disable(struct ad9467_dev *dev,
			      int32_t en,
			      int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((en == 0) || (en == 1)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_OUT_MODE,
					     en * AD9467_OUT_MODE_DOUT_DISBL,
					     AD9467_OUT_MODE_DOUT_DISBL);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = en;
	} else {
		ret = ad9467_read(dev, AD9467_REG_OUT_MODE, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_OUT_MODE_DOUT_DISBL) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Activates the inverted (1) or normal (0) output mode.
 *
 * @param dev      - The device structure.
 * @param invert   - Invert option.
 *		     Example: 1 - Activates the inverted output mode;
 *			      0 - Activates the normal output mode(default).
 * @param ret_stat - Return parameter of the set output mode.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_output_invert(struct ad9467_dev *dev,
			     int32_t invert,
			     int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if( (invert == 0) || (invert == 1)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_OUT_MODE,
					     invert * AD9467_OUT_MODE_OUT_INV,
					     AD9467_OUT_MODE_OUT_INV);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = invert;
	} else {
		ret = ad9467_read(dev, AD9467_REG_OUT_MODE, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_OUT_MODE_OUT_INV) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Specifies the output format.
 *
 * @param dev      - The device structure.
 * @param format   - The output format.
 *		     Example: 0 � offset binary(default);
 *			      1 � two's complement;
 *			      2 � gray code;
 *			      3 � reserved.
 * @param ret_stat - Return parameter of the set output format.
 *
 * @return Negative error code or the set output format.
*******************************************************************************/
int32_t ad9467_output_format(struct ad9467_dev *dev,
			     int32_t format,
			     int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if( (format == 0) || (format == 1) || (format == 2)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_OUT_MODE,
					     AD9467_OUT_MODE_DATA_FORMAT(format),
					     AD9467_OUT_MODE_DATA_FORMAT(0x3));
		if(ret < 0) {
			return ret;
		}
		*ret_stat = format;
	} else {
		ret = ad9467_read(dev, AD9467_REG_OUT_MODE, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_OUT_MODE_DATA_FORMAT(0x3)) >> 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Determines LVDS output properties.
 *
 * @param dev      - The device structure.
 * @param lvds_adj - Coarse LVDS adjust.
 *                   Example: 0 - 3.0  mA output current level(default);
 *                            1 - 1.71 mA output current level.
 * @param ret_stat - Return parameter of the set LVDS adjust state.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_coarse_lvds_adj(struct ad9467_dev *dev,
			       int32_t lvds_adj,
			       int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((lvds_adj == 0) || (lvds_adj == 1)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_OUT_ADJ,
					     lvds_adj * AD9467_OUT_ADJ_LVDS,
					     AD9467_OUT_ADJ_LVDS);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = lvds_adj;
	} else {
		ret = ad9467_read(dev, AD9467_REG_OUT_ADJ, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_OUT_ADJ_LVDS) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the output current adjustment.
 *
 * @param dev      - The device structure.
 * @param adj      - The output current adjustment.
 *		     Example: 001 = 3.0  mA output drive current (default);
 *                            010 = 2.79 mA output drive current;
 *                            011 = 2.57 mA output drive current;
 *                            100 = 2.35 mA output drive current;
 *                            101 = 2.14 mA output drive current;
 *                            110 = 1.93 mA output drive current;
 *                            111 = 1.71 mA output drive current;
 * @param ret_stat - Return parameter of the set current adjustment.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_output_current_adj(struct ad9467_dev *dev,
				  int32_t adj,
				  int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((adj > 0) && (adj <= 7)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_OUT_ADJ,
					     AD9467_OUT_ADJ_OUT_CURRENT(adj),
					     AD9467_OUT_ADJ_OUT_CURRENT(0x7));
		if(ret < 0) {
			return ret;
		}
		*ret_stat = adj;
	} else {
		ret = ad9467_read(dev, AD9467_REG_OUT_ADJ, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_OUT_ADJ_OUT_CURRENT(0x7)) >> 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Activates the normal (0) or inverted (1) DCO clock.
 *
 * @param dev      - The device structure.
 * @param invert   - Invert option.
 *		     Example: 0 - Activates the normal DCO clock(default);
 *			      1 - Activates the inverted DCO clock.
 * @param ret_stat - Return parameter of the set DCO clock inversion status.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_dco_clock_invert(struct ad9467_dev *dev,
				int32_t invert,
				int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((invert == 0) || (invert == 1)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_OUT_PHASE,
					     invert * AD9467_OUT_PHASE_DCO_INV,
					     AD9467_OUT_PHASE_DCO_INV);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = invert;
	} else {
		ret = ad9467_read(dev, AD9467_REG_OUT_PHASE, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_OUT_PHASE_DCO_INV) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Configures the clock delay setting.
 *
 * @param dev      - The device structure.
 * @param delay    - The clock delay setting in ps {0, 100, 200, ..., 3200}.
 *                   Setting the delay to 0 disables the DCO output clock delay.
 * @param ret_stat - Return parameter of the set clock delay.
 *
 * @return Error code.
*******************************************************************************/
int32_t ad9467_dco_output_clock_delay(struct ad9467_dev *dev,
				      int32_t delay,
				      int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if(!delay) {
		ret = ad9467_write(dev, AD9467_REG_OUT_DELAY, 0x00);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = 0x00;
	} else if(delay <= 3200) {
		*ret_stat = delay;
		delay = (delay - 100) / 100;
		ret = ad9467_write(dev,
				   AD9467_REG_OUT_DELAY,
				   AD9467_OUT_DELAY_DCO_DLY_EN |
				   AD9467_OUT_DELAY_OUT_DLY(delay));
		if(ret < 0) {
			return ret;
		}
	} else {
		ret = ad9467_read(dev, AD9467_REG_OUT_DELAY, &read);
		if(ret < 0) {
			return ret;
		}
		if(read & AD9467_OUT_DELAY_DCO_DLY_EN) {
			*ret_stat = (read & AD9467_OUT_DELAY_OUT_DLY(0x1F)) * 100 + 100;
		} else {
			*ret_stat = 0;
		}
	}

	return ret;
}

/***************************************************************************//**
 * @brief Configures the full-scale input voltage selection.
 *
 * @param dev      - The device structure.
 * @param v_fs     - Full-scale input voltage selection.
 *		     Example: 2.0 V p-p
 *                            2.1 V p-p
 *                            2.2 V p-p
 *                            2.3 V p-p
 *                            2.4 V p-p
 *                            2.5 V p-p(default)
 * @param ret_stat - Return parameter of the set input voltage selection.
 *
 * @return  Error code.
*******************************************************************************/
int32_t ad9467_full_scale_range(struct ad9467_dev *dev,
				float v_fs,
				float *ret_stat)
{
	int32_t ret = 0;
	int8_t val = -1;
	uint8_t read;

	if(v_fs == 2.0)
		val = 0x00;
	else if((v_fs >= 2.1) && (v_fs <= 2.5))
		val = ((int8_t)(v_fs * 10) % 20) + 0x05;
	if(val != -1) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_V_REF,
					     AD9467_V_REF_IN_FS_RANGE(val),
					     AD9467_V_REF_IN_FS_RANGE(0xF));
		if(ret < 0) {
			return ret;
		}
		*ret_stat = v_fs;
	} else {
		ret = ad9467_read(dev, AD9467_REG_V_REF, &read);
		if(ret < 0) {
			return ret;
		}
		val = read & AD9467_V_REF_IN_FS_RANGE(0xF);
		if(val == 0x00) {
			*ret_stat = 2.0;
		} else {
			*ret_stat = (((float)(val - 0x05) / 10) + 2.0);
		}
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the AC coupling(0) or DC coupling(1) mode.
 *
 * @param dev           - The device structure.
 * @param coupling_mode - Input coupling mode selection.
 *                        Example: 0 - AC coupling;
 *                                 1 - DC coupling(default).
 * @param ret_stat      - Return parameter of the set coupling mode.
 *
 * @return  Error code.
*******************************************************************************/
int32_t ad9467_analog_input_coupling(struct ad9467_dev *dev,
				     int32_t coupling_mode,
				     int32_t *ret_stat)
{
	int32_t ret = 0;
	uint8_t read;

	if((coupling_mode == 0) || (coupling_mode == 7)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_ANALOG_INPUT,
					     coupling_mode *
					     AD9467_ANALOG_INPUT_COUPLING,
					     AD9467_ANALOG_INPUT_COUPLING);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = coupling_mode;
	} else {
		ret = ad9467_read(dev, AD9467_REG_ANALOG_INPUT, &read);
		if(ret < 0) {
			return ret;
		}
		*ret_stat = (read & AD9467_ANALOG_INPUT_COUPLING) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Changes the input buffer current(1).
 *
 * @param dev        - The device structure.
 * @param percentage - Buffer current select 1.
 *                     Example: +530%
 *                              +520%
 *                               ...
 *                              + 80%(default)
 *                               ...
 *                              + 20%
 *                              + 10%
 *                              +  0%
 *                              - 10%
 *                              - 20%
 *                               ...
 *                              - 90%
 *                              -100%
 * @param ret_stat   - Return parameter of the set buffer current.
 *
 * @return  Error code.
*******************************************************************************/
int32_t ad9467_buffer_current_1(struct ad9467_dev *dev,
				int32_t percentage,
				int32_t *ret_stat)
{
	int32_t ret = 0;
	int8_t val = 0;
	uint8_t read;

	if((percentage >= -100) && (percentage <= 530)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_BUFF_CURRENT_1,
					     AD9467_BUFF_CURRENT_1(percentage / 10),
					     AD9467_BUFF_CURRENT_1(0x3F));
		if(ret < 0) {
			return ret;
		}
		*ret_stat = percentage;
	} else {
		ret = ad9467_read(dev, AD9467_REG_BUFF_CURRENT_1, &read);
		if(ret < 0) {
			return ret;
		}
		val = (read & AD9467_BUFF_CURRENT_1(0x3F)) >> 2;
		if(val > 53) {
			val -= 0x40;
		}
		*ret_stat = (val * 10);
	}

	return ret;
}

/***************************************************************************//**
 * @brief Changes the input buffer current(2).
 * @param dev        - The device structure.
 * @param percentage - Buffer current select 2.
 *                     Example: +530%
 *                              +520%
 *                               ...
 *                              + 80%(default)
 *                               ...
 *                              + 20%
 *                              + 10%
 *                              +  0%
 *                              - 10%
 *                              - 20%
 *                               ...
 *                              - 90%
 *                              -100%
 * @param ret_stat   - Return parameter of the set buffer current.
 *
 * @return  Error code.
*******************************************************************************/
int32_t ad9467_buffer_current_2(struct ad9467_dev *dev,
				int32_t percentage,
				int32_t *ret_stat)
{
	int32_t ret = 0;
	int8_t val = 0;
	uint8_t read;

	if((percentage >= -100) && (percentage <= 530)) {
		ret = ad9467_set_bits_to_reg(dev,
					     AD9467_REG_BUFF_CURRENT_2,
					     AD9467_BUFF_CURRENT_2(percentage / 10),
					     AD9467_BUFF_CURRENT_2(0x3F));
		if(ret < 0) {
			return ret;
		}
		*ret_stat = percentage;
	} else {
		ret = ad9467_read(dev, AD9467_REG_BUFF_CURRENT_2, &read);
		if(ret < 0) {
			return ret;
		}
		val = (read & AD9467_BUFF_CURRENT_2(0x3F)) >> 2;
		if(val > 53) {
			val -= 0x40;
		}
		*ret_stat = (val * 10);
	}

	return ret;
}

/***************************************************************************//**
 * @brief Initiates a transfer and waits for the operation to end.
 *
 * @return  Negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9467_transfer(struct ad9467_dev *dev)
{
	int32_t ret = 0;
	int8_t sw_bit = 0;
	uint8_t read;

	ret = ad9467_write(dev,
			   AD9467_REG_DEVICE_UPDATE,
			   AD9467_DEVICE_UPDATE_SW);
	if(ret < 0) {
		return ret;
	}
	do {
		ret = ad9467_read(dev, AD9467_REG_DEVICE_UPDATE, &read);
		if(ret < 0) {
			return ret;
		}
		sw_bit = read & AD9467_REG_DEVICE_UPDATE;
	} while(sw_bit == 1);

	return 0;
}
