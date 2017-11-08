/***************************************************************************//**
 *   @file   AD9517.c
 *   @brief  Implementation of AD9517 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "AD9517.h"
#include "AD9517_cfg.h"
#include "spi.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct ad9517_state
{
	struct ad9517_platform_data				*pdata;
	struct ad9517_lvpecl_channel_spec		*lvpecl_channels;
	struct ad9517_lvds_cmos_channel_spec	*lvds_cmos_channels;
	uint32_t								r_counter;
	uint8_t									a_counter;
	uint16_t								b_counter;
	uint8_t									vco_divider;
	uint8_t									prescaler_p;
	uint8_t									antibacklash_pulse_width;
}ad9517_st;
#ifdef OLD_VERSION
int32_t spiBaseAddress = 0;
int32_t spiSlaveSelect = 0;
#else
static uint8_t ad9517_slave_select;
#endif

/***************************************************************************//**
 * @brief Initializes the AD9517.
 *
 * @param spiBaseAddr - SPI peripheral AXI base address.
 * @param ssNo        - Slave select line on which the slave is connected.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
#ifdef OLD_VERSION
int32_t ad9517_setup(int32_t spiBaseAddr, int32_t ssNo)
#else
int32_t ad9517_setup(uint32_t spi_device_id, uint8_t slave_select)
#endif
{
	struct ad9517_state *st 		= &ad9517_st;
	int32_t				ret			= 0;
	int8_t				index		= 0;
	uint16_t 			regAddress	= 0;
	char     			regValue    = 0;

	st->pdata = &ad9517_pdata_lpc;
	st->lvpecl_channels = &ad9517_lvpecl_channels[0];
	st->lvds_cmos_channels = &ad9517_lvds_cmos_channels[0];

#ifdef OLD_VERSION
	spiBaseAddress = spiBaseAddr;
	spiSlaveSelect = ssNo;
	
    /* Initializes the SPI peripheral */
    ret = SPI_Init(spiBaseAddress, 0, 0, 0);
	if(ret < 0)
	{
		return ret;
	}
#else
	ad9517_slave_select = slave_select;
	spi_init(spi_device_id, 0, 0);
#endif
	/* Configure serial port for long instructions and reset the serial interface. */
	ret = ad9517_write(AD9517_REG_SERIAL_PORT_CONFIG, AD9517_SOFT_RESET | AD9517_LONG_INSTRUCTION);
	if(ret < 0)
	{
		return ret;
	}
	ret = ad9517_update();
	if(ret < 0)
	{
		return ret;
	}
	/* Clear AD9517_SOFT_RESET bit to complete reset operation. */
	ret = ad9517_write(AD9517_REG_SERIAL_PORT_CONFIG, AD9517_LONG_INSTRUCTION);
	if(ret < 0)
	{
		return ret;
	}
	ret = ad9517_update();
	if(ret < 0)
	{
		return ret;
	}
	/* Selects the PLL reference mode. */
	regValue = st->pdata->diff_ref_en * AD9517_DIFF_REF |
			   st->pdata->ref_1_power_on * AD9517_REF1_POWER_ON |
			   st->pdata->ref_2_power_on * AD9517_REF2_POWER_ON |
			   st->pdata->ref_sel_pin_en * AD9517_USE_REF_SEL_PIN |
			   st->pdata->ref_2_en * AD9517_SELECT_REF2;
	ret = ad9517_write(AD9517_REG_PLL_CTRL_7, regValue);
	if(ret < 0)
	{
		return ret;
	}
	/* Select CLK input. */
	regValue = st->pdata->vco_clk_sel * AD9517_SEL_VCO_CLK |
			   st->pdata->power_down_vco_clk * AD9517_POWER_DOWN_VCO_CLK;
	ret = ad9517_write(AD9517_REG_INPUT_CLKS, regValue);
	if(ret < 0)
	{
		return ret;
	}
	/* Update the device with user settings for the LVPECL output channels. */
	for(index = 0; index < 4; index++)
	{
		regAddress = AD9517_REG_LVPECL_OUT0 + index;
		regValue = st->lvpecl_channels[index].out_invert_en * AD9517_OUT_LVPECL_INVERT |
				   AD9517_OUT_LVPECL_DIFF_VOLTAGE(st->lvpecl_channels[index].out_diff_voltage);
		ret = ad9517_write(regAddress, regValue);
		if(ret < 0)
		{
			return ret;
		}
	}
	/* Update the device with user settings for the LVDS/CMOS output channels. */
	for(index = 0; index < 4; index++)
	{
		regAddress = AD9517_REG_LVDS_CMOS_OUT4 + index;
		regValue = AD9517_OUT_LVDS_CMOS_INVERT(st->lvds_cmos_channels[index].out_invert) |
				   st->lvds_cmos_channels[index].logic_level * AD9517_OUT_LVDS_CMOS |
				   st->lvds_cmos_channels[index].cmos_b_en * AD9517_OUT_CMOS_B |
				   AD9517_OUT_LVDS_OUTPUT_CURRENT(st->lvds_cmos_channels[index].out_lvds_current);
		ret = ad9517_write (regAddress, regValue);
		if(ret < 0)
		{
			return ret;
		}
	}
	/* Check if VCO is selected as input. */
	if(st->pdata->vco_clk_sel)
	{
		/* Sets the VCO frequency. */
		ad9517_vco_frequency(st->pdata->int_vco_freq);
	}

	return ret;
}

/**************************************************************************//**
* @brief Writes data into a register.
*
* @param regAddr - The address of the register to be written.
* @param regVal  - The value to be written into the register.
*
* @return Returns 0 in case of success or negative error code.
******************************************************************************/
int32_t ad9517_write(uint32_t regAddr, uint16_t regVal)
{
	uint8_t  i           = 0;
    int32_t  ret         = 0;
	uint16_t regAddress  = 0;
#ifdef OLD_VERSION
	char     regValue    = 0;
	char     txBuffer[3] = {0, 0, 0};
#else
	uint8_t regValue	 = 0;
	uint8_t txBuffer[3]  = {0, 0, 0};
#endif

	regAddress = AD9517_WRITE + AD9517_ADDR(regAddr);
	for(i = 0; i < AD9517_TRANSF_LEN(regAddr); i++)
	{
		regValue    = (regVal >> ((AD9517_TRANSF_LEN(regAddr) - i - 1) * 8)) & 0xFF;
		txBuffer[0] = (regAddress & 0xFF00) >> 8;
		txBuffer[1] = regAddress & 0x00FF;
		txBuffer[2] = regValue;
#ifdef OLD_VERSION
		ret         = SPI_TransferData(spiBaseAddress, 3, txBuffer, 0, NULL, spiSlaveSelect);
#else
		ret = spi_write_and_read(ad9517_slave_select, txBuffer, 3);
#endif
		if(ret < 0)
		{
			return ret;
		}
		regAddress--;
	}

    return ret;
}

/**************************************************************************//**
* @brief Reads data from a register.
*
* @param regAddr - The address of the register to be read.
*
* @return Returns the read data or negative error code.
******************************************************************************/
int32_t ad9517_read(uint32_t regAddr)
{
	uint32_t regAddress  = 0;
#ifdef OLD_VERSION
	uint8_t  rxBuffer[3] = {0, 0, 0};
	int32_t  ret         = 0;
#endif
	uint8_t  txBuffer[3] = {0, 0, 0};
	uint32_t regValue    = 0;
	uint8_t  i           = 0;

	regAddress = AD9517_READ + AD9517_ADDR(regAddr);
	for(i = 0; i < AD9517_TRANSF_LEN(regAddr); i++)
	{
		txBuffer[0] = (regAddress & 0xFF00) >> 8;
		txBuffer[1] = regAddress & 0x00FF;
		txBuffer[2] = 0;
#ifdef OLD_VERSION
		ret         = SPI_TransferData(spiBaseAddress, 3, (char*)txBuffer, 3, (char*)rxBuffer, spiSlaveSelect);
		if(ret < 0)
		{
			return ret;
		}
#else
		spi_write_and_read(ad9517_slave_select, txBuffer, 3);
#endif
		regAddress--;
		regValue <<= 8;
#ifdef OLD_VERSION
		regValue |= rxBuffer[2];
#else
		regValue |= txBuffer[2];
#endif
	}

	return regValue;
}

/***************************************************************************//**
 * @brief Transfers the contents of the buffer registers into the active
 *        registers.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9517_update(void)
{
	return ad9517_write(AD9517_REG_UPDATE_ALL_REGS, AD9517_UPDATE_ALL_REGS);
}


/***************************************************************************//**
 * @brief Sets the VCO frequency.
 *
 * @param frequency - The desired frequency value.
 *
 * @return vco_freq - The actual frequency value that was set.
*******************************************************************************/
int64_t ad9517_vco_frequency(int64_t frequency)
{
	struct ad9517_state *st	   = &ad9517_st;
	int32_t ref_freq 		   = 0;
	int32_t pfd_freq 		   = 0;
	int32_t n_divider		   = 0;
	int32_t prescaler_value[5] = {2, 4, 8, 16, 32};
	int64_t prescaler_limit[5] = {200000000ul, 1000000000ul, 2400000000ul, 3000000000ul, 3000000000ul};
	int32_t index			   = 0;
	int32_t vco_freq		   = 0;
	int32_t good_values		   = 0;
	uint8_t prescaler		   = 0;
	int32_t reg_value		   = 0;

	if((frequency < AD9517_MIN_VCO_FREQ) || (frequency > AD9517_MAX_VCO_FREQ))
	{
		return -1;
	}
	if(st->pdata->ref_sel_pin_en)
	{
		/* Reference selection is made using REF_SEL pin. */
		ref_freq = st->pdata->ref_sel_pin ? st->pdata->ref_2_freq : st->pdata->ref_1_freq;
	}
	else
	{
		/* Reference selection is made using bit AD9517_SELECT_REF2 from AD9517_REG_PLL_CTRL_7. */
		ref_freq = st->pdata->ref_2_en ? st->pdata->ref_2_freq : st->pdata->ref_1_freq;
	}
	st->r_counter = 1;
	pfd_freq = ref_freq / st->r_counter;
	while(pfd_freq > AD9517_MAX_PFD_FREQ)
	{
		st->r_counter++;
		pfd_freq = ref_freq / st->r_counter;
	}
	/* Dual Modulus Mode */
	while(good_values == 0)
	{
		for(index = 0; index < 5; index++)
		{
			if(frequency <= prescaler_limit[index])
			{
				n_divider = (int32_t)(frequency / pfd_freq);
				st->prescaler_p = prescaler_value[index];
				prescaler = index + 2;
				st->b_counter = n_divider / st->prescaler_p;
				st->a_counter = n_divider % st->prescaler_p;
				if((st->b_counter >= 3) && ((st->b_counter > st->a_counter)))
				{
					good_values = 1;
					break;
				}
			}
		}
		if(good_values == 0)
		{
			st->r_counter++;
			pfd_freq = ref_freq / st->r_counter;
		}
	}
	if(pfd_freq > 50000000)
	{
		/* This setting may be necessary if the PFD frequency > 50 MHz. */
		st->antibacklash_pulse_width = 1;
	}
	reg_value = ad9517_read(AD9517_REG_PLL_CTRL_1);
	if(reg_value < 0)
	{
		return reg_value;
	}
	reg_value &= ~AD9517_PRESCALER_P(0x7);
	reg_value |= AD9517_PRESCALER_P(prescaler);
	ad9517_write(AD9517_REG_PLL_CTRL_1, reg_value);
	ad9517_write(AD9517_REG_A_COUNTER, AD9517_A_COUNTER(st->a_counter));
	ad9517_write(AD9517_REG_B_COUNTER, AD9517_B_COUNTER(st->b_counter));
	ad9517_write(AD9517_REG_R_COUNTER, AD9517_R_COUNTER(st->r_counter));

	/* Compute the frequency obtained with the calculated values. */
	vco_freq = (ref_freq / st->r_counter) * (st->prescaler_p * st->b_counter + st->a_counter);
	
	/* Update vco_freq value. */
	st->pdata->int_vco_freq = vco_freq;

	return vco_freq;
}

/***************************************************************************//**
 * @brief Checks if the number can be decomposed into a product of two numbers
 *        smaller or equal to 32 each one.
 *
 * @param number - The number.
 *
 * @return Returns 1 if the number can't be decomposed or 0 otherwise.
*******************************************************************************/
int8_t DividersChecker(int32_t number)
{
	int32_t i = 0;
	
	for(i = 1; i*i <= number; i++)
	{
		if(number % i == 0)
		{
			if(i <= 32)
			{
				if((number / i) <= 32)
				{
					return 0;
				}
			}
		}
    }
	
    return 1;
}

/***************************************************************************//**
 * @brief Sets the frequency on the specified channel.
 *
 * @param channel   - The channel.
 * @param frequency - The desired frequency value.
 *
 * @return set_freq - The actual frequency value that was set.
*******************************************************************************/
int64_t ad9517_frequency(int32_t channel, int64_t frequency)
{
	struct   ad9517_state *st        = &ad9517_st;
	int64_t  freq_to_chan_div        = 0;			// The frequency that is applied to the channel dividers.
	int64_t  freq_to_chan_div_backup = 0;
	int64_t  freq_0_value            = 0;
	int32_t  freq_0_divider          = 0;
	int64_t  freq_1_value            = 0;
	int32_t  freq_1_divider          = 0;
	int32_t  divider                 = 0;
	int32_t  divider_max             = 0;
	int32_t  divider_1               = 0;
	int32_t  divider_2               = 0;
	int64_t  set_freq                = 0;
	int16_t  reg_value               = 0;
	int32_t  reg_address             = 0;
	uint32_t ret                     = 0;

	if(st->pdata->vco_clk_sel)
	{
		/* VCO is selected as input. */
		freq_to_chan_div = st->pdata->int_vco_freq;
		freq_to_chan_div_backup = freq_to_chan_div;
		/* VCO divider cannot be bypassed when VCO is selected as input. */
		st->vco_divider = 2;
		freq_to_chan_div >>= 1;
	}
	else
	{
		/* External Clock is selected as input. */
		freq_to_chan_div = st->pdata->ext_clk_freq;
		freq_to_chan_div_backup = freq_to_chan_div;
		st->vco_divider = 1;
	}
	/* The maximum frequency that can be applied to the channel dividers is 1600 MHz. */
	while(freq_to_chan_div > 1600000000)
	{
		st->vco_divider++;
		freq_to_chan_div = freq_to_chan_div_backup / st->vco_divider;
	}
	/* The range of division for the LVPECL outputs is 1 to 32. */
	if((channel >= 0) && (channel <= 3))
	{
		divider_max = 32;
	}
	/* The LVDS/CMOS outputs allow a range of divisions up to a maximum of 1024. */
	if((channel >= 4) && (channel <= 7))
	{
		divider_max = 1024;
	}
	/* Increase vco_divider if the divider is greater than divider_max. */
	while(st->vco_divider < 6)
	{
		divider = freq_to_chan_div / frequency;
		if(divider > divider_max)
		{
			st->vco_divider++;
			freq_to_chan_div = freq_to_chan_div_backup / st->vco_divider;
		}
		else
		{
			break;
		}
	}
	divider = freq_to_chan_div / frequency;
	/* If the divider is still greater than divider_max assign it the divider_max value.  */
	if(divider > divider_max)
	{
		divider = divider_max;
		set_freq = freq_to_chan_div / divider;
	}
	else
	{
		divider = 0;
	}
	/* Write the VCO divider value. */
	reg_value = ad9517_read(AD9517_REG_INPUT_CLKS);
	if(reg_value < 0)
	{
		return reg_value;
	}
	if((st->vco_divider == 1) && ((reg_value & AD9517_SEL_VCO_CLK) == 0))
	{
		reg_value |= AD9517_BYPASS_VCO_DIVIDER;
		ret = ad9517_write(AD9517_REG_INPUT_CLKS, reg_value);
		if(ret < 0)
		{
			return ret;
		}
	}
	else
	{
		ret = ad9517_write(AD9517_REG_VCO_DIVIDER, AD9517_VCO_DIVIDER((st->vco_divider - 2)));
		if(ret < 0)
		{
			return ret;
		}
	}
	freq_to_chan_div_backup = freq_to_chan_div;
	if((channel >= 0) && (channel <= 7))
	{
		if(divider == 0)
		{
			divider = 1;
			freq_0_divider = divider;
			freq_0_value = freq_to_chan_div_backup;			
			while(freq_to_chan_div >= frequency)
			{
				freq_0_value = freq_to_chan_div;
				freq_0_divider = divider;
				if(divider < 32)
				{
					divider++;
					freq_to_chan_div = freq_to_chan_div_backup / divider;
				}
				else
				{
					divider++;
					freq_to_chan_div = freq_to_chan_div_backup / divider;
					while(DividersChecker(divider))
					{
						divider++;
						freq_to_chan_div = freq_to_chan_div_backup / divider;
					}
				}
			}
			freq_1_value = freq_to_chan_div;
			freq_1_divider = divider;
			/* Choose the frequency closer to desired frequency */
			if((frequency - freq_1_value) > (freq_0_value - frequency))
			{
				divider = freq_0_divider;
				set_freq = freq_0_value;
			}
			else
			{
				divider = freq_1_divider;
				set_freq = freq_1_value;
			}
		}
		if((channel >= 0) && (channel <= 3))
		{
			/* LVPECL Channels. */
			if(divider == 1)
			{
				if(channel / 2)
				{
					reg_address = AD9517_REG_DIVIDER_1_1;
				}
				else
				{
					reg_address = AD9517_REG_DIVIDER_0_1;
				}
				reg_value = ad9517_read(reg_address);
				if(reg_value < 0)
				{
					return reg_value;
				}
				reg_value |= AD9517_DIVIDER_BYPASS;
			}
			else
			{
				if(channel / 2)
				{
					reg_address = AD9517_REG_DIVIDER_1_0;
				}
				else
				{
					reg_address = AD9517_REG_DIVIDER_0_0;	
				}
				/* The duty cycle closest to 50% is selected. */
				reg_value = AD9517_DIVIDER_LOW_CYCLES(((divider / 2) - 1)) |
							AD9517_DIVIDER_HIGH_CYCLES(((divider / 2) + (divider % 2) - 1));
			}
			ret = ad9517_write(reg_address, reg_value);
			if(ret < 0)
			{
				return ret;
			}
		}
		else
		{
			/* LVDS/CMOS Channels. */
			if(divider == 1)
			{
				/* Bypass the dividers. */
				if(channel / 6)
				{
					reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_3_3;
				}
				else
				{
					reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_2_3;	
				}
				reg_value = ad9517_read(reg_address);
				if(reg_value < 0)
				{
					return reg_value;
				}
				reg_value |= (AD9517_BYPASS_DIVIDER_2 | AD9517_BYPASS_DIVIDER_1);
				ret = ad9517_write(reg_address, reg_value);
				if(ret < 0)
				{
					return ret;
				}
			}
			else
			{
				if(divider <= 32)
				{
					/* Bypass the divider 2. */
					if(channel / 6)
					{
						reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_3_3;
					}
					else
					{
						reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_2_3;	
					}
					reg_value = ad9517_read(reg_address);
					if(reg_value < 0)
					{
						return reg_value;
					}
					reg_value |= AD9517_BYPASS_DIVIDER_2;
					ret = ad9517_write(reg_address, reg_value);
					if(ret < 0)
					{
						return ret;
					}
					if(channel / 6)
					{
						reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_3_0;
					}
					else
					{
						reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_2_0;
					}
					/* The duty cycle closest to 50% is selected. */
					reg_value = AD9517_LOW_CYCLES_DIVIDER_1(((divider / 2) - 1)) |
								AD9517_HIGH_CYCLES_DIVIDER_1(((divider / 2) + (divider % 2) - 1));
					ret = ad9517_write(reg_address, reg_value);
					if(ret < 0)
					{
						return ret;
					}
				}
				else
				{
					/* Find a good value smaller or equal to 32 for divider_2. */
					divider_2 = 32;
					do
					{
						divider_1 = divider;
						if((divider_1 % divider_2))
						{
							divider_2--;
						}
						else
						{
							divider_1 /= divider_2;
						}
					}
					while(divider_1 > 32);
					if(channel / 6)
					{
						reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_3_0;
					}
					else
					{
						reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_2_0;
					}
					/* The duty cycle closest to 50% is selected. */
					reg_value = AD9517_LOW_CYCLES_DIVIDER_1(((divider_1 / 2) - 1)) |
								AD9517_HIGH_CYCLES_DIVIDER_1(((divider_1 / 2) + (divider_1 % 2) - 1));
					ret = ad9517_write(reg_address, reg_value);
					if(ret < 0)
					{
						return ret;
					}
					if(channel / 6)
					{
						reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_3_2;
					}
					else
					{
						reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_2_2;
					}
					/* The duty cycle closest to 50% is selected. */
					reg_value = AD9517_LOW_CYCLES_DIVIDER_2(((divider_2 / 2) - 1)) |
								AD9517_HIGH_CYCLES_DIVIDER_2(((divider_2 / 2) + (divider_2 % 2) - 1));
					ret = ad9517_write(reg_address, reg_value);
					if(ret < 0)
					{
						return ret;
					}
				}
			}
		}
	}
	else
	{
		/* Invalid channel number. */
		return -1;
	}

	return set_freq;
}

/***************************************************************************//**
 * @brief Sets the phase on the specified channel.
 *
 * @param channel - The channel.
 * @param phase   - The desired phase value.
 *
 * @return Returns the phase or negative error code.
*******************************************************************************/
int32_t ad9517_phase(int32_t channel, int32_t phase)
{
	uint8_t  reg_value   = 0;
	int32_t  reg_address = 0;
	uint32_t ret         = 0;

	if((channel >= 0) && (channel <= 7))
	{
		if((channel >= 0) && (channel <= 3))
		{
			if(channel / 2)
			{
				reg_address = AD9517_REG_DIVIDER_1_1;
			}
			else
			{
				reg_address = AD9517_REG_DIVIDER_0_1;
			}

			reg_value = ad9517_read(reg_address);
			if(reg_value < 0)
			{
				return reg_value;
			}
			reg_value &= ~AD9517_DIVIDER_PHASE_OFFSET(0xF);
			reg_value |= AD9517_DIVIDER_PHASE_OFFSET(phase);
		}
		else
		{
			if(channel / 6)
			{
				reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_3_1;
			}
			else
			{
				reg_address = AD9517_REG_LVDS_CMOS_DIVIDER_2_1;	
			}
			reg_value = AD9517_PHASE_OFFSET_DIVIDER_2(((phase / 2) + (phase % 2))) |
						AD9517_PHASE_OFFSET_DIVIDER_1(phase / 2);
		}
		ret = ad9517_write(reg_address, reg_value);
		if(ret < 0)
		{
			return ret;
		}
	}

	return phase;
}

/***************************************************************************//**
 * @brief Sets the power mode of the specified channel.
 *
 * @param channel - The channel.
 * @param mode    - Power mode.
 *
 * @return Returns the mode or negative error code.
*******************************************************************************/
int32_t ad9517_power_mode(int32_t channel, int32_t mode)
{
	struct ad9517_state 				 *st				= &ad9517_st;
	struct ad9517_lvpecl_channel_spec 	 *lvpecl_channel;
	struct ad9517_lvds_cmos_channel_spec *lvds_cmos_channel;
	uint8_t 							 reg_value			= 0;
	int32_t 							 reg_address		= 0;
	uint32_t 							 ret				= 0;

	if((channel >= 0) && (channel <= 3))
	{
		lvpecl_channel = &st->lvpecl_channels[channel];
		switch(channel)
		{
		case 0:
			reg_address = AD9517_REG_LVPECL_OUT0;
			break;
		case 1:
			reg_address = AD9517_REG_LVPECL_OUT1;
			break;
		case 2:
			reg_address = AD9517_REG_LVPECL_OUT2;
			break;
		default:
			reg_address = AD9517_REG_LVPECL_OUT3;
		}
		if((mode >= 0) && (mode <= 3))
		{
			reg_value = lvpecl_channel->out_invert_en * AD9517_OUT_LVPECL_INVERT |
						AD9517_OUT_LVPECL_DIFF_VOLTAGE(lvpecl_channel->out_diff_voltage) |
						AD9517_OUT_LVPECL_POWER_DOWN(mode);
			ret = ad9517_write(reg_address, reg_value);
			if(ret < 0)
			{
				return ret;
			}
			return mode;
		}
		else
		{
			ret = ad9517_read(reg_address);
			if(ret < 0)
			{
				return ret;
			}
			return (ret & AD9517_OUT_LVPECL_POWER_DOWN(0x3));
		}
	}
	else
	{
		if((channel >= 4) && (channel <= 7))
		{
			lvds_cmos_channel = &st->lvds_cmos_channels[0];
			switch(channel)
			{
			case 4:
				reg_address = AD9517_REG_LVDS_CMOS_OUT4;
				break;
			case 5:
				reg_address = AD9517_REG_LVDS_CMOS_OUT5;
				break;
			case 6:
				reg_address = AD9517_REG_LVDS_CMOS_OUT6;
				break;
			default:
				reg_address = AD9517_REG_LVDS_CMOS_OUT7;
			}
			if((mode >= 0) && (mode <= 1))
			{
				reg_value = AD9517_OUT_LVDS_CMOS_INVERT(lvds_cmos_channel->out_invert) |
							lvds_cmos_channel->cmos_b_en * AD9517_OUT_CMOS_B |
							lvds_cmos_channel->logic_level * AD9517_OUT_LVDS_CMOS |
							AD9517_OUT_LVDS_OUTPUT_CURRENT(lvds_cmos_channel->out_lvds_current) |
							mode * AD9517_OUT_LVDS_CMOS_POWER_DOWN;
				ret = ad9517_write(reg_address, reg_value);
				if(ret < 0)
				{
					return ret;
				}
				return mode;
			}
			else
			{
				ret = ad9517_read(reg_address);
				if(ret < 0)
				{
					return ret;
				}
				return (ret & AD9517_OUT_LVDS_CMOS_POWER_DOWN);
			}
		}
		else
		{
			/* Invalid channel number. */
			return -1;
		}
	}
}
