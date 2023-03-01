/**************************************************************************//**
*   @file   ad9833.c
*   @brief  Implementation of ad9833 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

/******************************************************************************/
/******************************* Include Files ********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad9833.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Constants Definitions *****************************/
/******************************************************************************/
float phase_const = 651.8986469f;

static const struct ad9833_chip_info chip_info[] = {
	[ID_AD9833] = {
		.mclk       = 25000000,
		.freq_const = 10.7374182f,
	},
	[ID_AD9834]  = {
		.mclk       = 75000000,
		.freq_const = 3.5791394f,
	},
	[ID_AD9837] = {
		.mclk       = 16000000,
		.freq_const = 16.777216f,
	},
	[ID_AD9838]  = {
		.mclk       = 16000000,
		.freq_const = 16.777216f,
	}
};

/******************************************************************************/
/************************** Functions Definitions *****************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initialize the SPI communication with the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
int8_t ad9833_init(struct ad9833_dev **device,
		   struct ad9833_init_param init_param)
{
	struct ad9833_dev *dev;
	uint16_t spi_data = 0;
	int8_t status = -1;

	dev = (struct ad9833_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* Setup GPIO pads. */
	status |= no_os_gpio_get(&dev->gpio_psel,
				 &init_param.gpio_psel);
	status |= no_os_gpio_get(&dev->gpio_fsel,
				 &init_param.gpio_fsel);
	status |= no_os_gpio_get(&dev->gpio_reset,
				 &init_param.gpio_reset);
	status |= no_os_gpio_get(&dev->gpio_sleep,
				 &init_param.gpio_sleep);

	AD9834_PSEL_OUT;
	AD9834_PSEL_LOW;
	AD9834_FSEL_OUT;
	AD9834_FSEL_LOW;
	AD9834_RESET_OUT;
	AD9834_RESET_LOW;
	AD9834_SLEEP_OUT;
	AD9834_SLEEP_LOW;

	dev->act_device = init_param.act_device;
	dev->prog_method = 0;
	dev->ctrl_reg_value = 0;
	dev->test_opbiten = 0;

	/* Setup SPI interface. */
	status = no_os_spi_init(&dev->spi_desc,
				&init_param.spi_init);
	/* Initialize board. */
	spi_data |= AD9833_CTRLRESET;
	ad9833_tx_spi(dev,
		      spi_data);
	no_os_mdelay(10);
	spi_data &= ~AD9833_CTRLRESET;
	ad9833_tx_spi(dev,
		      spi_data);

	ad9833_set_freq(dev, 0, 0);
	ad9833_set_freq(dev, 1, 0);
	ad9833_set_phase(dev, 0, 0);
	ad9833_set_phase(dev, 1, 0);

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad9833_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9833_remove(struct ad9833_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_psel);
	ret |= no_os_gpio_remove(dev->gpio_fsel);
	ret |= no_os_gpio_remove(dev->gpio_reset);
	ret |= no_os_gpio_remove(dev->gpio_sleep);

	no_os_free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Transmits 16 bits on SPI.
 *
 * @param dev   - The device structure.
 * @param value - Data which will be transmitted.
 *
 * @return none.
******************************************************************************/
void ad9833_tx_spi(struct ad9833_dev *dev,
		   int16_t value)
{
	uint16_t spi_data = 0;
	uint8_t tx_buffer[4]  = {0, 0, 0, 0};// Tx SPI Buffer.

	tx_buffer[0] = (uint8_t) ((value & 0x00ff00) >> 8); // data to be sent
	tx_buffer[1] = (uint8_t) (value & 0x0000ff);        // in 8 bit packets
	if (no_os_spi_write_and_read(dev->spi_desc, tx_buffer, 2) != 0) {
		/* Initialize board. */
		spi_data |= AD9833_CTRLRESET;
		ad9833_tx_spi(dev,
			      spi_data);
		no_os_mdelay(10);
		spi_data &=~ AD9833_CTRLRESET;
		ad9833_tx_spi(dev,
			      spi_data);
	}
}

/**************************************************************************//**
 * @brief Selects the type of output.
 *
 * @param dev      - The device structure.
 * @param out_mode - type of output
 *                   Example AD9833&AD9837: 0 - Sinusoid.
 *                                          1 - Triangle.
 *                                          2 - DAC Data MSB/2.
 *                                          3 - DAC Data MSB.
 *                   Example AD9834&AD9838: 0 - Sinusoid.
 *                                          1 - Triangle.
 *
 * @return status - output type could / couldn't be selected.
 *                  Example: 0 - output type possible for the given configuration.
 *                          -1 - output type not possible for the given configuration.
******************************************************************************/
int8_t ad9833_out_mode(struct ad9833_dev *dev,
		       uint8_t out_mode)
{
	uint16_t spi_data = 0;
	int8_t status = 0;


	if ((dev->act_device == ID_AD9833) || (dev->act_device == ID_AD9837)) {
		spi_data = (dev->ctrl_reg_value & ~(AD9833_CTRLMODE    |
						    AD9833_CTRLOPBITEN |
						    AD9833_CTRLDIV2));
		switch(out_mode) {
		case 1:     // Triangle
			spi_data += AD9833_CTRLMODE;
			break;
		case 2:     // DAC Data MSB/2
			spi_data += AD9833_CTRLOPBITEN;
			break;
		case 3:     // DAC Data MSB
			spi_data += AD9833_CTRLOPBITEN + AD9833_CTRLDIV2;
			break;
		default:    // Sinusoid
			break;
		}
		ad9833_tx_spi(dev,
			      spi_data);
		dev->ctrl_reg_value = spi_data;
	} else {
		if ((dev->act_device == ID_AD9834) || (dev->act_device == ID_AD9838)) {
			spi_data = (dev->ctrl_reg_value & ~AD9833_CTRLMODE);
			dev->test_opbiten = dev->ctrl_reg_value & AD9833_CTRLOPBITEN;

			switch(out_mode) {
			case 1:     // Triangle
				if (dev->test_opbiten == 0) {
					spi_data += AD9833_CTRLMODE;
				} else {
					status = -1;
				}
				break;
			default:    // Sinusoid
				break;
			}
			ad9833_tx_spi(dev,
				      spi_data);
			dev->ctrl_reg_value = spi_data;
		}
	}

	return status;
}

/**************************************************************************//**
 * @brief Enable / Disable the sleep function.
 *
 * @param dev        - The device structure.
 * @param sleep_mode - type of sleep
 *                    Example soft method(all devices):
 *                              0 - No power-down.
 *                              1 - DAC powered down.
 *                              2 - Internal clock disabled.
 *                              3 - DAC powered down and Internal
 *                                  clock disabled.
 *                    Example hard method(AD9834 & AD9838):
 *                              0 - No power-down.
 *                              1 - DAC powered down.
 *
 * @return None.
******************************************************************************/
void ad9833_sleep_mode(struct ad9833_dev *dev,
		       uint8_t sleep_mode)
{
	uint16_t spi_data = 0;

	if (dev->prog_method == 0) {
		spi_data = (dev->ctrl_reg_value & ~(AD9833_CTRLSLEEP12 | AD9833_CTRLSLEEP1));
		switch(sleep_mode) {
		case 1:     // DAC powered down
			spi_data += AD9833_CTRLSLEEP12;
			break;
		case 2:     // Internal clock disabled
			spi_data += AD9833_CTRLSLEEP1;
			break;
		case 3:     // DAC powered down and Internal clock disabled
			spi_data += AD9833_CTRLSLEEP1 + AD9833_CTRLSLEEP12;
			break;
		default:    // No power-down
			break;
		}
		ad9833_tx_spi(dev,
			      spi_data);
		dev->ctrl_reg_value = spi_data;
	} else {
		if (dev->prog_method == 1) {
			switch(sleep_mode) {
			case 0:     // No power-down
				AD9834_SLEEP_LOW;
				break;
			case 1:     // DAC powered down
				AD9834_SLEEP_HIGH;
				break;
			default:    // Do nothing
				break;
			}
		}
	}

}

/**************************************************************************//**
 * @brief Loads a frequency value in a register.
 *
 * @param dev             - The device structure.
 * @param register_number - Number of the register (0 / 1).
 * @param frequency_value - Frequency value.
 *
 * @return None.
******************************************************************************/
void ad9833_set_freq(struct ad9833_dev *dev,
		     uint8_t register_number,
		     uint32_t frequency_value)
{
	uint32_t ul_freq_register;
	uint16_t i_freq_lsb, i_freq_msb;

	ul_freq_register = (uint32_t)(frequency_value *
				      chip_info[dev->act_device].freq_const);
	i_freq_lsb = (ul_freq_register & 0x0003FFF);
	i_freq_msb = ((ul_freq_register & 0xFFFC000) >> 14);
	dev->ctrl_reg_value |= AD9833_CTRLB28;
	ad9833_tx_spi(dev,
		      dev->ctrl_reg_value);
	if(register_number == 0) {
		ad9833_tx_spi(dev,
			      BIT_F0ADDRESS + i_freq_lsb);
		ad9833_tx_spi(dev,
			      BIT_F0ADDRESS + i_freq_msb);
	} else {
		ad9833_tx_spi(dev,
			      BIT_F1ADDRESS + i_freq_lsb);
		ad9833_tx_spi(dev,
			      BIT_F1ADDRESS + i_freq_msb);
	}
}

/**************************************************************************//**
 * @brief Loads a phase value in a register.
 *
 * @param dev             - The device structure.
 * @param register_number - Number of the register (0 / 1).
 * @param phase_value     - Phase value.
 *
 * @return none
******************************************************************************/
void ad9833_set_phase(struct ad9833_dev *dev,
		      uint8_t register_number,
		      float phase_value)
{
	uint16_t phase_calc;

	phase_calc = (uint16_t)(phase_value * phase_const);
	if(register_number == 0) {
		ad9833_tx_spi(dev,
			      BIT_P0ADDRESS + phase_calc);
	} else {
		ad9833_tx_spi(dev,
			      BIT_P1ADDRESS + phase_calc);
	}
}

/**************************************************************************//**
 * @brief Select the frequency register to be used.
 *
 * @param dev      - The device structure.
 * @param freq_reg - Number of frequency register. (0 / 1)
 *
 * @return None.
******************************************************************************/
void ad9833_select_freq_reg(struct ad9833_dev *dev,
			    uint8_t freq_reg)
{
	uint16_t spi_data = 0;

	if (dev->prog_method == 0) {
		spi_data = (dev->ctrl_reg_value & ~AD9833_CTRLFSEL);
		// Select soft the working frequency register according to parameter
		if(freq_reg == 1) {
			spi_data += AD9833_CTRLFSEL;
		}
		ad9833_tx_spi(dev,
			      spi_data);
		dev->ctrl_reg_value = spi_data;
	} else {
		if (dev->prog_method == 1) {
			// Select hard the working frequency register according to parameter
			if (freq_reg == 1) {
				AD9834_FSEL_HIGH;
			} else {
				if (freq_reg == 0) {
					AD9834_FSEL_LOW;
				}
			}
		}
	}
}

/**************************************************************************//**
 * @brief Select the phase register to be used.
 *
 * @param dev       - The device structure.
 * @param phase_reg - Number of phase register. (0 / 1)
 *
 * @return None.
******************************************************************************/
void ad9833_select_phase_reg(struct ad9833_dev *dev,
			     uint8_t phase_reg)
{
	uint16_t spi_data = 0;

	if (dev->prog_method == 0) {
		spi_data = (dev->ctrl_reg_value & ~AD9833_CTRLPSEL);
		// Select soft the working phase register according to parameter
		if(phase_reg == 1) {
			spi_data += AD9833_CTRLPSEL;
		}
		ad9833_tx_spi(dev,
			      spi_data);
		dev->ctrl_reg_value = spi_data;
	} else {
		if (dev->prog_method == 1) {
			// Select hard the working phase register according to parameter
			if (phase_reg == 1) {
				AD9834_PSEL_HIGH;
			} else {
				if (phase_reg == 0) {
					AD9834_PSEL_LOW;
				}
			}
		}
	}
}

/**************************************************************************//**
 * @brief Sets the programming method. (only for AD9834 & AD9838)
 *
 * @param dev   - The device structure.
 * @param value - soft or hard method. (0 / 1)
 *
 * @return None.
******************************************************************************/
void ad9834_select_prog_method(struct ad9833_dev *dev,
			       uint8_t value)
{
	uint16_t spi_data = (dev->ctrl_reg_value & ~AD9834_CTRLPINSW);

	dev->prog_method = 0;        // software method
	if (value == 1) {
		spi_data += AD9834_CTRLPINSW;
		dev->prog_method = 1;    // hardware method
	}
	ad9833_tx_spi(dev,
		      spi_data);
	dev->ctrl_reg_value = spi_data;
}

/**************************************************************************//**
 * @brief Configures the control register for logic output.
 *        (only for AD9834 & AD9838)
 *
 * @param dev     - The device structure.
 * @param opbiten - Enables / disables the logic output.
 * @param signpib - Connects comparator / MSB to the SIGN BIT OUT pin.
 * @param div2    - MSB / MSB/2
 *
 * @return None.
******************************************************************************/
void ad9834_logic_output(struct ad9833_dev *dev,
			 uint8_t opbiten,
			 uint8_t signpib,
			 uint8_t div2)
{
	uint16_t spi_data = 0;

	spi_data = (dev->ctrl_reg_value & ~(AD9833_CTRLOPBITEN |
					    AD9833_CTRLMODE    |
					    AD9834_CTRLSIGNPIB |
					    AD9833_CTRLDIV2));


	if(opbiten == 1) {
		spi_data |= AD9833_CTRLOPBITEN;

		if (signpib == 1) {
			spi_data |= AD9834_CTRLSIGNPIB;
		} else {
			if (signpib == 0) {
				spi_data &= ~AD9834_CTRLSIGNPIB;
			}
		}
		if (div2 == 1) {
			spi_data |= AD9833_CTRLDIV2;
		} else {
			if (div2 == 0) {
				spi_data &= ~AD9833_CTRLDIV2;
			}
		}
	} else {
		if (opbiten == 0) {
			spi_data &= ~AD9833_CTRLOPBITEN;
		}
	}
	ad9833_tx_spi(dev,
		      spi_data);
	dev->ctrl_reg_value = spi_data;
}


