/***************************************************************************//**
 *   @file   AD9250.c
 *   @brief  Implementation of AD9250 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
 ********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "AD9250.h"
#include "AD9250_cfg.h"

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/
const int32_t shadowRegs[SHADOW_REGISTER_COUNT] = {
	0,
	0x01, // AD9250_SHD_REG_CLOCK
	0x00, // AD9250_SHD_REG_CLOCK_DIV
	0x00, // AD9250_SHD_REG_TEST
	0x00, // AD9250_SHD_REG_BIST
	0x00, // AD9250_SHD_REG_OFFSET
	0x01, // AD9250_SHD_REG_OUT_MODE
	0x00, // AD9250_SHD_REG_VREF
	0x00, // AD9250_SHD_REG_SYS_CTRL
	0x00, // AD9250_SHD_REG_DCC_CTRL
	0x00, // AD9250_SHD_REG_DCC_VAL
	0x00, // AD9250_SHD_REG_FAST_DETECT
	0x00, // AD9250_SHD_REG_FD_UPPER_THD
	0x00, // AD9250_SHD_REG_FD_LOWER_THD
	0x00  // AD9250_SHD_REG_FD_DWELL_TIME
};

/******************************************************************************/
/************************ Private Functions Prototypes ************************/
/******************************************************************************/
int32_t ad9250_set_bits_to_reg(ad9250_dev *dev,
			       uint32_t registerAddress,
			       uint8_t  bitsValue,
			       uint8_t  mask);

int32_t ad9250_is_shadow_register(int32_t registerAddress);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Configures the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return Returns negative error code or 0 in case of success.
 *******************************************************************************/
int32_t ad9250_setup(ad9250_dev **device,
		     ad9250_init_param init_param)
{
	ad9250_dev *dev;
	int32_t ret, i;

	dev = (ad9250_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* Initializes registers */
	for(i = 0; i < SHADOW_REGISTER_COUNT; i++)
		dev->shadowRegs[i] = shadowRegs[i];

	/* Initializes the SPI peripheral */
	ret = spi_init(&dev->spi_desc, init_param.spi_init);
	if(ret != SUCCESS)
	{
		return ret;
	}

	/* Reset ad9250 registers to their default values. */
	ad9250_soft_reset(dev);

	/* Configure the AD9250 device. */
	dev->ad9250_st.pdata = &ad9250_pdata_lpc;
	ret = ad9250_write(dev,
			   AD9250_REG_SPI_CFG,
			   AD9250_SPI_CFG_SOFT_RST);
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_PDWN,
				     dev->ad9250_st.pdata->extrnPDWNmode * AD9250_PDWN_EXTERN,
				     AD9250_PDWN_EXTERN);
	if(ret != SUCCESS)
	{
		return ret;
	}

	ret = ad9250_write(dev,
			   AD9250_REG_CLOCK,
			   dev->ad9250_st.pdata->enClkDCS * AD9250_CLOCK_DUTY_CYCLE |
			   AD9250_CLOCK_SELECTION(dev->ad9250_st.pdata->clkSelection));
	if(ret != SUCCESS)
	{
		return ret;
	}

	ret = ad9250_write(dev,
			   AD9250_REG_CLOCK_DIV,
			   AD9250_CLOCK_DIV_RATIO(dev->ad9250_st.pdata->clkDivRatio) |
			   AD9250_CLOCK_DIV_PHASE(dev->ad9250_st.pdata->clkDivPhase));
	if(ret != SUCCESS)
	{
		return ret;
	}

	ret = ad9250_write(dev,
			   AD9250_REG_VREF,
			   AD9250_VREF_FS_ADJUST(dev->ad9250_st.pdata->adcVref));
	if(ret != SUCCESS)
	{
		return ret;
	}

	ret = ad9250_write(dev,
			   AD9250_REG_PLL_ENCODE,
			   AD9250_PLL_ENCODE(dev->ad9250_st.pdata->pllLowEncode));
	if(ret != SUCCESS)
	{
		return ret;
	}

	/* Synchronously update registers. */
	ret = ad9250_transfer(dev);
	if(ret != SUCCESS)
	{
		return ret;
	}

	/* Configure the JESD204B interface. */
	ret = ad9250_jesd204b_setup(dev);
	if(ret != SUCCESS)
	{
		return ret;
	}

	/* Configure the Fast-detect circuit. */
	ret = ad9250_fast_detect_setup(dev);
	if(ret != SUCCESS)
	{
		return ret;
	}

	/* Synchronously update registers. */
	ret = ad9250_transfer(dev);

	*device = dev;

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad9250_setup().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9250_remove(ad9250_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param dev             - The device structure.
 * @param registerAddress - The address of the register to read.
 *
 * @return registerValue  - The register's value or negative error code.
 *******************************************************************************/
int32_t ad9250_read(ad9250_dev *dev,
		    int32_t registerAddress)
{
	uint32_t regAddress  = 0;
	uint8_t  Buffer[3]   = {0, 0, 0};
	uint32_t regValue    = 0;
	uint8_t  i	     = 0;
	int32_t  ret	     = 0;

	regAddress = AD9250_READ + AD9250_ADDR(registerAddress);
	for(i = 0; i < AD9250_TRANSF_LEN(registerAddress); i++)
	{
		Buffer[0] = (regAddress & 0xFF00) >> 8;
		Buffer[1] = regAddress & 0x00FF;
		Buffer[2] = 0;
		ret = spi_write_and_read(dev->spi_desc,
					 Buffer,
					 3);
		if(ret != SUCCESS)
		{
			return ret;
		}
		regAddress--;
		regValue <<= 8;
		regValue |= Buffer[2];
	}

	return regValue;
}

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param dev             - The device structure.
 * @param registerAddress - The address of the register to write to.
 * @param registerValue   - The value to write to the register.
 *
 * @return Returns 0 in case of success or negative error code.
 *******************************************************************************/
int32_t ad9250_write(ad9250_dev *dev,
		     int32_t registerAddress,
		     int32_t registerValue)
{
	uint8_t  i	     = 0;
	int32_t  ret	     = 0;
	uint16_t regAddress  = 0;
	char	 regValue    = 0;
	uint8_t	 txBuffer[3] = {0, 0, 0};

	/* Check if the register is shadowed. */
	ret = ad9250_is_shadow_register(registerAddress);
	/* Synchronize shadow register with on-chip register. */
	if(ret > 0)
	{
		dev->shadowRegs[ret] = registerValue;
	}
	regAddress = AD9250_WRITE + AD9250_ADDR(registerAddress);
	for(i = 0; i < AD9250_TRANSF_LEN(registerAddress); i++)
	{
		regValue = (registerValue >>
				((AD9250_TRANSF_LEN(registerAddress) - i - 1) * 8)) & 0xFF;
		txBuffer[0] = (regAddress & 0xFF00) >> 8;
		txBuffer[1] = regAddress & 0x00FF;
		txBuffer[2] = regValue;
		ret = spi_write_and_read(dev->spi_desc,
					 txBuffer,
					 3);
		if(ret != SUCCESS)
		{
			return ret;
		}
		regAddress--;
	}

	return (ret - 1);
}

/***************************************************************************//**
 * @brief Initiates a transfer and waits for the operation to end.
 *	  Note: This function may be called after a shadowed register was written,
 *		so that the internal update can actually take place.
 *
 * @param dev - The device structure.
 *
 * @return  Negative error code or 0 in case of success.
 *******************************************************************************/
int32_t ad9250_transfer(ad9250_dev *dev)
{
	int32_t timeout = 0xFFFF;
	int32_t ret	= 0;
	int8_t	sw_bit	= 0;

	ret = ad9250_write(dev,
			   AD9250_REG_DEVICE_UPDATE,
			   AD9250_DEVICE_UPDATE_SW);
	if(ret != SUCCESS)
	{
		return ret;
	}
	do
	{
		ret = ad9250_read(dev,
				  AD9250_REG_DEVICE_UPDATE);
		if(ret != SUCCESS)
		{
			return ret;
		}
		sw_bit = ret & AD9250_REG_DEVICE_UPDATE;
		timeout--;
	}while((sw_bit == 1) && (timeout != 0));

	return 0;
}

/***************************************************************************//**
 * @brief Resets all registers to their default values.
 *
 * @param dev - The device structure.
 *
 * @return Returns negative error code or 0 in case of success.
 *******************************************************************************/
int32_t ad9250_soft_reset(ad9250_dev *dev)
{
	int32_t timeout = 0xFFFF;
	int32_t ret	= 0;

	/* Software reset to default SPI values. */
	ret = ad9250_write(dev,
			   AD9250_REG_SPI_CFG,
			   AD9250_SPI_CFG_SOFT_RST);
	if(ret != SUCCESS)
	{
		return ret;
	}
	do
	{
		ret = ad9250_read(dev,
				  AD9250_REG_SPI_CFG);
		if(ret != SUCCESS)
		{
			return ret;
		}
		timeout--;
	}while(((ret & AD9250_SPI_CFG_SOFT_RST) != 0) && (timeout != 0));

	return ret;
}

/***************************************************************************//**
 * @brief Sets a bit/group of bits inside a register without modifying other
 *	  bits.
 *
 * @param dev             - The device structure.
 * @param registerAddress - The address of the register to be written.
 * @param bitsValue	  - The value of the bit/bits.
 * @param mask		  - The bit/bits position in the register.
 *
 * @return Returns negative error code or 0 in case of success.
 *******************************************************************************/
int32_t ad9250_set_bits_to_reg(ad9250_dev *dev,
			       uint32_t registerAddress,
			       uint8_t  bitsValue,
			       uint8_t  mask)
{
	uint8_t regValue = 0;
	int32_t ret	 = 0;

	/* Read from the shadow register instead of the on-chip register when
	   shadowed register is discovered. */
	ret = ad9250_is_shadow_register(registerAddress);
	if(ret > 0)
	{
		regValue = dev->shadowRegs[ret];
	}
	else
	{
		ret = ad9250_read(dev,
				  registerAddress);
		if(ret != SUCCESS)
		{
			return ret;
		}
		regValue = ret;
	}
	regValue &= (~mask);
	regValue |= bitsValue;
	ret = ad9250_write(dev,
			   registerAddress,
			   regValue);
	if(ret != SUCCESS)
	{
		return ret;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Checks if the register is shadowed.
 *
 * @param registerAddress - The address of the register to be checked.
 *
 * @return Returns the index of the shadow register or 0 if the register is not
 *	   shadowed.
 *******************************************************************************/
int32_t ad9250_is_shadow_register(int32_t registerAddress)
{
	return ((SHADOW(0xFF) & registerAddress) >> 16);
}

/***************************************************************************//**
 * @brief Configures the power mode of the chip.
 *
 * @param dev  - The device structure.
 * @param mode - The power mode.
 *		   Example: 00 � normal operation(default);
 *			    01 � power-down;
 *			    10 - standby.
 *
 * @return Returns negative error code or the set power mode.
 *******************************************************************************/
int32_t ad9250_chip_pwr_mode(ad9250_dev *dev,
			     int32_t mode)
{
	uint32_t ret = 0;

	if((mode >= 0) && (mode < 3))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_PDWN,
					     AD9250_PDWN_CHIP(mode),
					     AD9250_PDWN_CHIP(0x3));
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_PDWN);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_PDWN_CHIP(0x3)) >> 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Selects a channel as the current channel for further configurations.
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *		    Example 1 - channel A
 *			    2 - channel B
 *			    3 - channel A and channel B
 *
 * @return Returns negative error code or the selected channel.
 *******************************************************************************/
int32_t ad9250_select_channel_for_config(ad9250_dev *dev,
					 int32_t channel)
{
	int32_t ret = 0;

	if((channel > 0) && (channel <= 3))
	{
		ret = ad9250_write(dev,
				   AD9250_REG_CH_INDEX,
				   channel);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_CH_INDEX);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return ret & (AD9250_CH_INDEX_ADC_A | AD9250_CH_INDEX_ADC_B);
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the ADC's test mode.
 *
 * @param dev  - The device structure.
 * @param mode - ADC test mode.
 *		 Example: 0 -> off(default)
 *			  1 -> midscale short
 *			  2 -> +FS short
 *			  3 -> -FS short
 *			  4 -> checkerboard output
 *			  5 -> PN sequence long
 *			  6 -> PN 9 short
 *			  7 -> one/zero word toggle
 *			  8 -> user test mode
 *		    9 to 14 -> unused
 *			 15 -> ramp output
 *
 * @return Returns the set test mode or negative error code.
 *******************************************************************************/
int32_t ad9250_test_mode(ad9250_dev *dev,
			 int32_t mode)
{
	int32_t ret = 0;

	if((mode >= 0) && (mode < 16))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_TEST,
					     AD9250_TEST_OUTPUT_TEST(mode),
					     AD9250_TEST_OUTPUT_TEST(0xF));
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_TEST);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_TEST_OUTPUT_TEST(0xF)) >> 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the offset adjustment.
 *
 * @param dev - The device structure.
 * @param adj - The offset adjust value in LSBs from +31 to -32.
 *
 * @return Returns negative error code or the set offset adjustment.
 *******************************************************************************/
int32_t ad9250_offset_adj(ad9250_dev *dev,
			  int32_t adj)
{
	int32_t ret = 0;

	if((adj >= -32) && (adj <= 31))
	{
		ret = ad9250_write(dev,
				   AD9250_REG_OFFSET,
				   AD9250_REG_OFFSET_ADJUST(adj));
	}
	else
	{
		return (ad9250_read(dev,
				    AD9250_REG_OFFSET));
	}

	return ret;
}

/***************************************************************************//**
 * @brief Disables (1) or enables (0) the data output.
 *	  Note: This function modifies a shadowed register, therefore a call of
 *		ad9250_transfer() is required for the internal update to take
 *		place.
 *
 * @param dev - The device structure.
 * @param en  - Enable option.
 *		 Example: 1 - Disables the data output;
 *			  0 - Enables the data output(default).
 *
 * @return  Returns negative error code or the output disable state.
 *******************************************************************************/
int32_t ad9250_output_disable(ad9250_dev *dev,
			      int32_t en)
{
	uint32_t ret = 0;

	if((en == 0) || (en == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_OUT_MODE,
					     en * AD9250_OUT_MODE_DISABLE,
					     AD9250_OUT_MODE_DISABLE);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_OUT_MODE);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_OUT_MODE_DISABLE) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Activates the inverted (1) or normal (0) output mode.
 *	  Note: This function modifies a shadowed register, therefore a call of
 *		ad9250_transfer() is required for the internal update to take
 *		place.
 *
 * @param dev    - The device structure.
 * @param invert - Invert option.
 *		     Example: 1 - Activates the inverted output mode;
 *			      0 - Activates the normal output mode(default).
 *
 * @return Returns negative error code or the set output mode.
 *******************************************************************************/
int32_t ad9250_output_invert(ad9250_dev *dev,
			     int32_t invert)
{
	uint32_t ret = 0;

	if( (invert == 0) || (invert == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_OUT_MODE,
					     invert * AD9250_OUT_MODE_INVERT_DATA,
					     AD9250_OUT_MODE_INVERT_DATA);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_OUT_MODE);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_OUT_MODE_INVERT_DATA) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Specifies the output format.
 *	  Note: This function modifies a shadowed register, therefore a call of
 *		ad9250_transfer() is required for the internal update to take
 *		place.
 *
 * @param dev    - The device structure.
 * @param format - The output format.
 *		     Example: 0 � offset binary(default);
 *			      1 � two's complement;
 *
 * @return Returns negative error code or the set output format.
 *******************************************************************************/
int32_t ad9250_output_format(ad9250_dev *dev,
			     int32_t format)
{
	uint32_t ret = 0;

	if( (format == 0) || (format == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_OUT_MODE,
					     AD9250_OUT_MODE_DATA_FORMAT(format),
					     AD9250_OUT_MODE_DATA_FORMAT(1));
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_OUT_MODE);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_OUT_MODE_DATA_FORMAT(1)) >> 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets (1) or clears (0) the reset short PN sequence bit(PN9).
 *
 * @param dev - The device structure.
 * @param en  - Enable option.
 *	       Example: 1 - The PN sequence is held in reset;
 *			0 - The PN sequence resumes from the seed value(0x92).
 *
 * @return Returns negative error code or the set PN9 status.
 *******************************************************************************/
int32_t ad9250_reset_PN9(ad9250_dev *dev,
			 int32_t rst)
{
	int32_t ret = 0;

	if((rst == 0) || (rst == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_TEST,
					     rst * AD9250_TEST_RST_PN_SHOR,
					     AD9250_TEST_RST_PN_SHOR);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_TEST);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_TEST_RST_PN_SHOR) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets (1) or clears (0) the reset long PN sequence bit(PN23).
 *
 * @param dev - The device structure.
 * @param en  - Enable option.
 *	       Example: 1 - The PN sequence is held in reset;
 *			0 - The PN sequence resumes from the seed value(0x3AFF).
 *
 * @return Returns negative error code or the set PN23 status.
 *******************************************************************************/
int32_t ad9250_reset_PN23(ad9250_dev *dev,
			  int32_t rst)
{
	int32_t ret = 0;

	if((rst == 0) || (rst == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_TEST,
					     rst * AD9250_TEST_RST_PN_LONG,
					     AD9250_TEST_RST_PN_LONG);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_TEST);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_TEST_RST_PN_LONG) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Configures a User Test Pattern.
 *
 * @param dev          - The device structure.
 * @param patternNo    - Selects the patterns to be configured. Range 1..4.
 * @param user_pattern - Users's pattern.
 *
 * @return Returns negative error code or the selected user pattern.
 *******************************************************************************/
int32_t ad9250_set_user_pattern(ad9250_dev *dev,
				int32_t patternNo,
				int32_t user_pattern)
{
	int32_t patternAddress = 0;;
	int32_t ret	       = 0;

	patternAddress = AD9250_REG_USER_TEST1 + (2 * patternNo);
	ret = ad9250_write(dev,
			   patternAddress,
			   user_pattern);

	return ret;
}

/***************************************************************************//**
 * @brief Enables the Build-In-Self-Test.
 *
 * @param dev    - The device structure.
 * @param enable - enable option.
 *
 * @return Returns negative error code or the state of the enable bit.
 *******************************************************************************/
int32_t ad9250_bist_enable(ad9250_dev *dev,
			   int32_t enable)
{
	int32_t ret = 0;

	if((enable == 0) || (enable == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_BIST,
					     enable * AD9250_BIST_ENABLE,
					     AD9250_BIST_ENABLE);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_BIST);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_BIST_ENABLE) >> 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Resets the Build-In-Self-Test.
 *
 * @param dev    - The device structure.
 * @param enable - reset option.
 *
 * @return Returns negative error code or the state of the reset bit.
 *******************************************************************************/
int32_t ad9250_bist_reset(ad9250_dev *dev,
			  int32_t reset)
{
	int32_t ret = 0;

	if((reset == 0) || (reset == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_BIST,
					     reset * AD9250_BIST_RESET,
					     AD9250_BIST_RESET);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_BIST);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_BIST_RESET) >> 2;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets number of frames per multiframe (K).
 *
 * @param dev      - The device structure.
 * @param k_frames - Number of frames. Must be a multiple of 4.
 *		     Range: Ceil(17/F) to 32,
 *			    where F - is the number of octets per frame.
 *
 * @return Returns negative error code or actual number of frames that can be set.
 *******************************************************************************/
int32_t ad9250_jesd204b_set_frames(ad9250_dev *dev,
				   int32_t k_frames)
{
	int32_t kRegVal = k_frames;
	int32_t mod4	= 0;
	int32_t ret	= 0;

	if(k_frames >=0 && k_frames <= 32)
	{
		mod4 = k_frames % 4;
		if(mod4 != 0)
		{
			if(mod4 < 2)
			{
				kRegVal -= mod4;
			}
			else
			{
				kRegVal += 4 - mod4;
			}
		}
		ret = ad9250_write(dev,
				   AD9250_REG_204B_PARAM_K,
				   kRegVal - 1);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return kRegVal;
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_204B_PARAM_K);
		return ret;
	}
}

/***************************************************************************//**
 * @brief Configures the JESD204B interface.
 *
 * @param dev - The device structure.
 *
 * @return Returns negative error code or 0 in case of success.
 *******************************************************************************/
int32_t ad9250_jesd204b_setup(ad9250_dev *dev)
{
	int32_t		    ret = 0;

	dev->ad9250_st.pJesd204b = &ad9250_jesd204b_interface;

	/* Disable lanes before changing configuration */
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_204B_CTRL1,
				     AD9250_204B_CTRL1_POWER_DOWN,
				     AD9250_204B_CTRL1_POWER_DOWN);
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Select quick configuration option */
	ret = ad9250_write(dev,
			   AD9250_REG_204B_QUICK_CFG,
			   AD9250_204B_QUICK_CFG(dev->ad9250_st.pJesd204b->quickCfgOption));
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Configure detailed options */
	/* CML differential output drive level adjustment */
	ret = ad9250_write(dev,
			   AD9250_REG_CML,
			   AD9250_CML_DIFF_OUT_LEVEL(dev->ad9250_st.pJesd204b->cmlLevel));
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Select the behavioral of the 204B core when in standby. */
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_PDWN,
				     dev->ad9250_st.pJesd204b->jtxInStandBy * AD9250_PDWN_JTX,
				     AD9250_PDWN_JTX);
	if(ret != SUCCESS)
	{
		return ret;
	}

	/* Select subclass. */
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_204B_PARAM_NP,
				     AD9250_204B_PARAM_NP_JESD_SUBCLASS(dev->ad9250_st.pJesd204b->subclass),
				     AD9250_204B_PARAM_NP_JESD_SUBCLASS(-1));
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Configure the tail bits and control bits. */
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_204B_PARAM_CS_N,
				     AD9250_204B_PARAM_CS_N_NR_CTRL_BITS(dev->ad9250_st.pJesd204b->ctrlBitsNo),
				     AD9250_204B_PARAM_CS_N_NR_CTRL_BITS(-1));
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_OUT_MODE,
				     AD9250_OUT_MODE_JTX_BIT_ASSIGN(dev->ad9250_st.pJesd204b->ctrlBitsAssign),
				     AD9250_OUT_MODE_JTX_BIT_ASSIGN(-1));
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* A transfer operation is needed because AD9250_REG_OUT_MODE is a shadowed register. */
	ret = ad9250_transfer(dev);
	if(ret != SUCCESS)
	{
		return ret;
	}
	if(dev->ad9250_st.pJesd204b->ctrlBitsNo == 0)
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_204B_CTRL1,
					     AD9250_204B_CTRL1_TAIL_BITS * dev->ad9250_st.pJesd204b->tailBitsMode,
					     AD9250_204B_CTRL1_TAIL_BITS);
		if(ret != SUCCESS)
		{
			return ret;
		}
	}
	/* Set lane identification values. */
	ret = ad9250_write(dev,
			   AD9250_REG_204B_DID_CFG,
			   dev->ad9250_st.pJesd204b->did);
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_write(dev,
			   AD9250_REG_204B_BID_CFG,
			   dev->ad9250_st.pJesd204b->bid);
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_write(dev,
			   AD9250_REG_204B_LID_CFG1,
			   dev->ad9250_st.pJesd204b->lid0);
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_write(dev,
			   AD9250_REG_204B_LID_CFG2,
			   dev->ad9250_st.pJesd204b->lid1);
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Set number of frames per multiframe, K */
	ret = ad9250_jesd204b_set_frames(dev,
					 dev->ad9250_st.pJesd204b->k);
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Scramble, SCR. */
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_204B_PARAM_SCR_L,
				     AD9250_204B_PARAM_SCR_L_SCRAMBLING * dev->ad9250_st.pJesd204b->scrambling,
				     AD9250_204B_PARAM_SCR_L_SCRAMBLING);
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Select lane synchronization options */
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_204B_CTRL1,
				     AD9250_204B_CTRL1_ILAS_MODE(dev->ad9250_st.pJesd204b->ilasMode),
				     AD9250_204B_CTRL1_ILAS_MODE(-1));
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_204B_CTRL1,
				     dev->ad9250_st.pJesd204b->enIlasTest * AD9250_204B_CTRL1_TEST_SAMPLE_EN,
				     AD9250_204B_CTRL1_TEST_SAMPLE_EN);
	if(ret != SUCCESS)
	{
		return ret;
	}

	/* Set additional digital output configuration options */
	/* Set polarity of serial output data */
	ret = ad9250_jesd204b_invert_logic(dev,
					   dev->ad9250_st.pJesd204b->invertLogicBits);
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Options for interpreting single on SYSREF+- and SYNCINB+- */
	ret = ad9250_write(dev,
			   AD9250_REG_SYS_CTRL,
			   dev->ad9250_st.pJesd204b->enSyncInB * AD9250_SYS_CTRL_SYNCINB_EN |
			   dev->ad9250_st.pJesd204b->enSysRef * AD9250_SYS_CTRL_SYSREF_EN |
			   dev->ad9250_st.pJesd204b->sysRefMode * AD9250_SYS_CTRL_SYSREF_MODE |
			   dev->ad9250_st.pJesd204b->alignSysRef * AD9250_SYS_CTRL_REALIGN_ON_SYSREF |
			   dev->ad9250_st.pJesd204b->alignSyncInB * AD9250_SYS_CTRL_REALIGN_ON_SYNCINB);
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* A transfer operation is needed, because AD9250_REG_SYS_CTRL is a shadowed register. */
	ret = ad9250_transfer(dev);
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Option to remap converter and lane assignments */
	ret = ad9250_write(dev,
			   AD9250_REG_204B_LANE_ASSGN1,
			   AD9250_204B_LANE_ASSGN1(dev->ad9250_st.pJesd204b->lane0Assign) |
			   0x02);
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_write(dev,
			   AD9250_REG_204B_LANE_ASSGN2,
			   AD9250_204B_LANE_ASSGN2(dev->ad9250_st.pJesd204b->lane1Assign) |
			   0x30);
	if(ret != SUCCESS)
	{
		return ret;
	}
	/* Re-enable lane(s) */
	ret = ad9250_set_bits_to_reg(dev,
				     AD9250_REG_204B_CTRL1,
				     0,
				     AD9250_204B_CTRL1_POWER_DOWN);
	if(ret != SUCCESS)
	{
		return ret;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Configures the power mode of the JESD204B data transmit block.
 *
 * @param dev  - The device structure.
 * @param mode - The power mode.
 *		   Example: 00 � normal operation(default);
 *			    01 � power-down;
 *			    10 - standby.
 *
 * @return Returns negative error code or the set power mode.
 *******************************************************************************/
int32_t ad9250_jesd204b_pwr_mode(ad9250_dev *dev,
				 int32_t mode)
{
	uint32_t ret = 0;

	if((mode >= 0) && (mode < 3))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_PDWN,
					     AD9250_PDWN_JESD204B(mode),
					     AD9250_PDWN_JESD204B(0x3));
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_PDWN);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_PDWN_JESD204B(0x3)) >> 2;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Selects the point in the processing path of a lane, where the test
 *	  data will be inserted.
 *
 * @param dev      - The device structure.
 * @param injPoint - The point in the processing path of a lane.
 *		     Example:
 *			  1 - a 10-bit data is inserted at 8B/10B ENCODER output
 *			  2 - a 8-bit at scrambler input
 *
 * @return Returns negative error code or the status of the data injection point bit.
 *******************************************************************************/
int32_t ad9250_jesd204b_select_test_injection_point(ad9250_dev *dev,
						    int32_t injPoint)
{
	int32_t ret = 0;

	if((injPoint == 1) || (injPoint == 2))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_204B_CTRL3,
					     AD9250_204B_CTRL3_TEST_DATA_INJ_PT(injPoint),
					     AD9250_204B_CTRL3_TEST_DATA_INJ_PT(-1));
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_204B_CTRL3);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret &  AD9250_204B_CTRL3_TEST_DATA_INJ_PT(-1)) >> 4;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Selects a JESD204B test mode.
 *
 * @param dev      - The device structure.
 * @param testMode - mode option.
 *		     Example: 0 - test mode disabled
 *			      1 - alternating checker board
 *			      2 - 1/0 word toggle
 *			      3 - PN23 sequence
 *			      4 - PN9 sequence
 *			      5 - continuous/repeat user test mode
 *			      6 - single user test mode
 *			      7 - reserved
 *			      8 - modified RPAT test sequence
 *			     12 - PN7 sequence
 *			     13 - PN15 sequence
 *
 * @return Returns the set test mode or negative error code.
 *******************************************************************************/
int32_t ad9250_jesd204b_test_mode(ad9250_dev *dev,
				  int32_t testMode)
{
	int32_t ret = 0;

	if((testMode >= 0) && (testMode < 14))
	{
		ad9250_set_bits_to_reg(dev,
				       AD9250_REG_204B_CTRL3,
				       AD9250_204B_CTRL3_JESD_TEST_MODE(testMode),
				       AD9250_204B_CTRL3_JESD_TEST_MODE(-1));
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_204B_CTRL3);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret &  AD9250_204B_CTRL3_JESD_TEST_MODE(-1)) >> 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Inverts the logic of JESD204B bits.
 *
 * @param dev    - The device structure.
 * @param invert - Invert option.
 *		     Example: 1 - Activates the inverted mode
 *			      0 - Activates the normal mode
 *
 * @return Returns negative error code or the set mode.
 *******************************************************************************/
int32_t ad9250_jesd204b_invert_logic(ad9250_dev *dev,
				     int32_t invert)
{
	uint32_t ret = 0;

	if( (invert == 0) || (invert == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_204B_CTRL2,
					     invert * AD9250_204B_CTRL2_INVERT_JESD_BITS,
					     AD9250_204B_CTRL2_INVERT_JESD_BITS);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_204B_CTRL2);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_204B_CTRL2_INVERT_JESD_BITS) != 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Configures the Fast-Detect module.
 *
 * @param dev - The device structure.
 *
 * @return Returns negative error code or 0 in case of success.
 *******************************************************************************/
int32_t ad9250_fast_detect_setup(ad9250_dev *dev)
{
	int32_t		    ret = 0;

	dev->ad9250_st.pFd = &ad9250_fast_detect;

	ret = ad9250_write(dev,
			   AD9250_REG_FAST_DETECT,
			   AD9250_FAST_DETECT_OUTPUT_ENABLE * dev->ad9250_st.pFd->enFd |
			   AD9250_FAST_DETECT_FORCE_FDA_FDB_VAL * dev->ad9250_st.pFd->pinForceValue |
			   AD9250_FAST_DETECT_FORCE_FDA_FDB_PIN * dev->ad9250_st.pFd->forcePins |
			   AD9250_FAST_DETECT_PIN_FCT * dev->ad9250_st.pFd->pinFunction );
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_write(dev,
			   AD9250_REG_FD_UPPER_THD,
			   dev->ad9250_st.pFd->fdUpperTresh);
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_write(dev,
			   AD9250_REG_FD_LOWER_THD,
			   dev->ad9250_st.pFd->fdLowerTresh);
	if(ret != SUCCESS)
	{
		return ret;
	}
	ret = ad9250_write(dev,
			   AD9250_REG_FD_DWELL_TIME,
			   dev->ad9250_st.pFd->dfDwellTime);
	if(ret != SUCCESS)
	{
		return ret;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Enables DC correction for use in the output data signal path.
 *
 * @param dev    - The device structure.
 * @param enable - enable option.
 *		   Example: 0 - correction off
 *			    1 - correction on
 *
 * @return Returns negative error code or the status of the enable bit.
 *******************************************************************************/
int32_t ad9250_dcc_enable(ad9250_dev *dev,
			  int32_t enable)
{
	int32_t ret = 0;

	if((enable == 0) || (enable == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_DCC_CTRL,
					     AD9250_DCC_CTRL_DCC_EN * enable,
					     AD9250_DCC_CTRL_DCC_EN);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_DCC_CTRL);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_DCC_CTRL_DCC_EN) >> 0;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Selects the bandwidth value for the DC correction circuit.
 *
 * @param dev - The device structure.
 * @param bw  - DC correction bandwidth.
 *	       Example: 0 - 2387.32 Hz at Sample Rate of 245.76 MSPS
 *			1 - 1193.66 Hz at Sample Rate of 245.76 MSPS
 *			...
 *			13 - 0.29 Hz at Sample Rate of 245.76 MSPS
 *
 * @return Returns negative error code or the state of the bandwidth bits.
 *******************************************************************************/
int32_t ad9250_dcc_bandwidth(ad9250_dev *dev,
			     int32_t bw)
{
	int32_t ret = 0;

	if((bw >= 0) && (bw <= 13))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_DCC_CTRL,
					     AD9250_DCC_CTRL_DCC_BW(bw),
					     AD9250_DCC_CTRL_DCC_BW(-1));
	}
	else
	{
		ad9250_read(dev,
			    AD9250_REG_DCC_CTRL);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_DCC_CTRL_DCC_BW(-1)) >> 2;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Freezes DC correction value.
 *
 * @param dev    - The device structure.
 * @param freeze - freeze option.
 *		   Example: 0 - calculates the correction value
 *			    1 - freezes the DC correction at its current state
 *
 * @return Returns negative error code or the status of the freeze bit.
 *******************************************************************************/
int32_t ad9250_dcc_freeze(ad9250_dev *dev,
			  int32_t freeze)
{
	int32_t ret = 0;

	if((freeze == 0) || (freeze == 1))
	{
		ret = ad9250_set_bits_to_reg(dev,
					     AD9250_REG_DCC_CTRL,
					     AD9250_DCC_CTRL_FREEZE_DCC * freeze,
					     AD9250_DCC_CTRL_FREEZE_DCC);
	}
	else
	{
		ret = ad9250_read(dev,
				  AD9250_REG_DCC_CTRL);
		if(ret != SUCCESS)
		{
			return ret;
		}
		return (ret & AD9250_DCC_CTRL_FREEZE_DCC) >> 6;
	}

	return ret;
}
