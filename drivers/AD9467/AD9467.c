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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "AD9467.h"
#include "spi.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
#ifdef OLD_VERSION
 static int32_t spiBaseAddress;
 static int32_t spiSlaveSelect;
#else
static uint8_t ad9467_slave_select;
#endif

/***************************************************************************//**
 * @brief Configures the test mode and the output mode to a default state.
 *
 * @param spiBaseAddr - SPI peripheral AXI base address.
 * @param ssNo        - Slave select line on which the slave is connected.
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
#ifdef OLD_VERSION
int32_t ad9467_setup(int32_t spiBaseAddr, int32_t ssNo)
#else
int32_t ad9467_setup(uint32_t spi_device_id, uint8_t slave_select)
#endif
{
    int32_t ret = 0;

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
	ad9467_slave_select = slave_select;
	spi_init(spi_device_id, 0, 0);
#endif
	/* Disable test mode. */
    ret = ad9467_write(AD9467_REG_TEST_IO, 0x00);
    if(ret < 0)
    {
        return ret;
    }
	/* Enable digital output, disable output invert and set data format to
	   binary offset. */
    ret = ad9467_write(AD9467_REG_OUT_MODE, 0x08);
    if(ret < 0)
    {
        return ret;
    }
    ret = ad9467_transfer();
    if(ret < 0)
    {
        return ret;
    }

    return 0;
}

/**************************************************************************//**
* @brief Writes data into a register.
*
* @param regAddr - The address of the register to be written.
* @param regVal  - The value to be written into the register.
*
* @return Returns 0 in case of success or negative error code.
******************************************************************************/
int32_t ad9467_write(uint16_t regAddr, uint8_t regVal)
{
	int32_t ret;
	uint8_t	write_buffer[3];

    regAddr += AD9467_WRITE;

	write_buffer[0] = (uint8_t)((regAddr & 0xFF00) >> 8);
	write_buffer[1] = (uint8_t)(regAddr & 0x00FF);
	write_buffer[2] = regVal;

#ifdef OLD_VERSION
	ret = SPI_TransferData(spiBaseAddress, 3, (char*)write_buffer, 0, NULL, 
			spiSlaveSelect);
#else
	ret = spi_write_and_read(ad9467_slave_select, write_buffer, 3);
#endif

    return ret;
}

/**************************************************************************//**
* @brief Reads data from a register.
*
* @param regAddr - The address of the register to be read.
*
* @return Returns the read data or negative error code.
******************************************************************************/
int32_t ad9467_read(uint16_t regAddr)
{
#ifdef OLD_VERSION
	int32_t ret;
	uint8_t	read_buffer[3] = {0, 0, 0};
#endif
	uint8_t	write_buffer[3];

	regAddr += AD9467_READ;

	write_buffer[0] = (uint8_t)((regAddr & 0xFF00) >> 8);
	write_buffer[1] = (uint8_t)(regAddr & 0x00FF);
	write_buffer[2] = 0;

#ifdef OLD_VERSION
	ret = SPI_TransferData(spiBaseAddress, 3, (char*)write_buffer, 3, (char*)read_buffer, 
			spiSlaveSelect);
	if(ret < 0)
	{
		return ret;
	}

	return (int32_t)read_buffer[2];
#else
	spi_write_and_read(ad9467_slave_select, write_buffer, 3);

	return (int32_t)write_buffer[2];
#endif
}

/***************************************************************************//**
 * @brief Sets a bit or a group of bits inside a register without modifying
 *        the other bits.
 *
 * @param registerAddress - The address of the register to be written.
 * @param bitsValue       - The value of the bit/bits.
 * @param mask            - The bit/bits position in the register.
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
uint32_t ad9467_set_bits_to_reg(uint16_t registerAddress,
                                uint8_t bitsValue,
                                uint8_t mask)
{
    uint8_t regValue = 0;
    int32_t ret      = 0;

    ret = ad9467_read(registerAddress);
    if(ret < 0)
    {
        return ret;
    }
    regValue = ret & (~mask);
    regValue |= bitsValue;
    ret = ad9467_write(registerAddress, regValue);
    if(ret < 0)
    {
        return ret;
    }

    return 0;
}

/***************************************************************************//**
 * @brief Configures the power mode.
 *
 * @param mode - The power mode.
 *				 Example: 00 – normal operation(default);
 *						  01 – full power-down.
 *
 * @return Negative error code or the set power mode.
*******************************************************************************/
int32_t ad9467_pwr_mode(int32_t mode)
{
    uint32_t ret = 0;

    if((mode == 0) || (mode == 1))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_MODES,
                                     AD9467_MODES_INT_PD_MODE(mode),
                                     AD9467_MODES_INT_PD_MODE(0x3));
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_MODES);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_MODES_INT_PD_MODE(0x3)) == 0x01;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Sets the ADC's test mode.
 *
 * @param mode - ADC test mode.
 *               Example: 0 -> off(default)
 *                        1 -> midscale short
 *                        2 -> +FS short
 *                        3 -> -FS short
 *                        4 -> checkerboard output
 *                        5 -> PN 23 sequence
 *                        6 -> PN 9 sequence
 *                        7 -> one/zero word toggle
 *
 * @return Returns the set test mode or negative error code
*******************************************************************************/
int32_t ad9467_test_mode(int32_t mode)
{
    int32_t ret = 0;

    if((mode >= 0) && (mode <= 7))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_TEST_IO,
                                     AD9467_TEST_IO_OUT_TEST(mode),
                                     AD9467_TEST_IO_OUT_TEST(0xF));
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_TEST_IO);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_TEST_IO_OUT_TEST(0xF)) >> 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Sets (1) or clears (0) the reset short PN sequence bit(PN9).
 *
 * @param en - Enable option.
 *             Example: 1 - The PN sequence is held in reset;
 *                      0 - The PN sequence resumes from the seed value(0x92).
 *
 * @return Negative error code or the set PN9 status.
*******************************************************************************/
int32_t ad9467_reset_PN9(int32_t rst)
{
    int32_t ret = 0;

    if((rst == 0) || (rst == 1))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_TEST_IO,
                                     rst * AD9467_TEST_IO_RST_PN_SHORT,
                                     AD9467_TEST_IO_RST_PN_SHORT);
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_TEST_IO);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_TEST_IO_RST_PN_SHORT) != 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Sets (1) or clears (0) the reset long PN sequence bit(PN23).
 *
 * @param en - Enable option.
 *             Example: 1 - The PN sequence is held in reset;
 *                      0 - The PN sequence resumes from the seed value(0x3AFF).
 *
 * @return Negative error code or the set PN23 status.
*******************************************************************************/
int32_t ad9467_reset_PN23(int32_t rst)
{
    int32_t ret = 0;

    if((rst == 0) || (rst == 1))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_TEST_IO,
                                     rst * AD9467_TEST_IO_RST_PN_LONG,
                                     AD9467_TEST_IO_RST_PN_LONG);
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_TEST_IO);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_TEST_IO_RST_PN_LONG) != 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Enables (1) or disables (0) the external voltage reference.
 *
 * @param en - Enable option.
 *             Example: 1 - Enables the external reference;
 *                      0 - Disables the external reference(default).
 *
 * @return Negative error code or the external reference state.
*******************************************************************************/
int32_t ad9467_external_ref(int32_t en)
{
    uint32_t ret = 0;

    if((en == 0) || (en == 1))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_ADC_INPUT,
                                     en * AD9467_ADC_INPUT_XVREF,
                                     AD9467_ADC_INPUT_XVREF);
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_ADC_INPUT);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_ADC_INPUT_XVREF) != 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Disconnects (1) or connects (0) the analog input from or to the
 *        the ADC channel.
 *
 * @param en - Enable option.
 *             Example: 1 - Disconnects the analog input.
 *                      0 - Connects the analog input(default).
 *
 * @return Negative error code or the analog disconnect status.
*******************************************************************************/
int32_t ad9467_analog_input_disconnect(int32_t en)
{
    uint32_t ret = 0;

    if((en == 0) || (en == 1))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_ADC_INPUT,
                                     en * AD9467_ADC_INPUT_ANALOG_DSCN,
                                     AD9467_ADC_INPUT_ANALOG_DSCN);
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_ADC_INPUT);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_ADC_INPUT_ANALOG_DSCN) != 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Sets the offset adjustment.
 *
 * @param adj - The offset adjust value in LSBs from +127 to -128.
 *
 * @return Negative error code or the set offset adjustment.
*******************************************************************************/
int32_t ad9467_offset_adj(int32_t adj)
{
    int32_t ret = 0;

    if((adj >= -128) && (adj <= 127))
    {
        ret = ad9467_write(AD9467_REG_OFFSET, adj);
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        return (ad9467_read(AD9467_REG_OFFSET));
    }

    return ret;
}

/***************************************************************************//**
 * @brief Disables (1) or enables (0) the data output.
 *
 * @param en - Enable option.
 *			   Example: 1 - Disables the data output;
 *						0 - Enables the data output(default).
 *
 * @return  Negative error code or the output disable state.
*******************************************************************************/
int32_t ad9467_output_disable(int32_t en)
{
    uint32_t ret = 0;

    if((en == 0) || (en == 1))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_OUT_MODE,
                                     en * AD9467_OUT_MODE_DOUT_DISBL,
                                     AD9467_OUT_MODE_DOUT_DISBL);
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_OUT_MODE);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_OUT_MODE_DOUT_DISBL) != 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Activates the inverted (1) or normal (0) output mode.
 *
 * @param invert - Invert option.
 *				   Example: 1 - Activates the inverted output mode;
 *							0 - Activates the normal output mode(default).
 *
 * @return Negative error code or the set output mode.
*******************************************************************************/
int32_t ad9467_output_invert(int32_t invert)
{
    uint32_t ret = 0;

    if( (invert == 0) || (invert == 1))
	{
        ret = ad9467_set_bits_to_reg(AD9467_REG_OUT_MODE,
                                     invert * AD9467_OUT_MODE_OUT_INV,
                                     AD9467_OUT_MODE_OUT_INV);
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_OUT_MODE);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_OUT_MODE_OUT_INV) != 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Specifies the output format.
 *
 * @param format - The output format.
 *				   Example: 0 – offset binary(default);
 *							1 – two's complement;
 *							2 – gray code;
 *							3 – reserved.
 *
 * @return Negative error code or the set output format.
*******************************************************************************/
int32_t ad9467_output_format(int32_t format)
{
    uint32_t ret = 0;

    if( (format == 0) || (format == 1) || (format == 2))
	{
        ret = ad9467_set_bits_to_reg(AD9467_REG_OUT_MODE,
                                     AD9467_OUT_MODE_DATA_FORMAT(format),
                                     AD9467_OUT_MODE_DATA_FORMAT(0x3));
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_OUT_MODE);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_OUT_MODE_DATA_FORMAT(0x3)) >> 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Determines LVDS output properties.
 *
 * @param lvds_ajd - Coarse LVDS adjust.
 *                   Example: 0 - 3.0  mA output current level(default);
 *                            1 - 1.71 mA output current level.
 *
 * @return Negative error code or LVDS adjust state.
*******************************************************************************/
int32_t ad9467_coarse_LVDS_adj(int32_t lvds_adj)
{
    uint32_t ret = 0;

    if((lvds_adj == 0) || (lvds_adj == 1))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_OUT_ADJ,
                                     lvds_adj * AD9467_OUT_ADJ_LVDS,
                                     AD9467_OUT_ADJ_LVDS);
        if(ret < 0)
            {
                return ret;
            }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_OUT_ADJ);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_OUT_ADJ_LVDS) != 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Sets the output current adjustment.
 *
 * @param adj - The output current adjustment.
 *				Example: 001 = 3.0  mA output drive current (default);
 *                       010 = 2.79 mA output drive current;
 *                       011 = 2.57 mA output drive current;
 *                       100 = 2.35 mA output drive current;
 *                       101 = 2.14 mA output drive current;
 *                       110 = 1.93 mA output drive current;
 *                       111 = 1.71 mA output drive current;
 *
 * @return Negative error code or the set current adjustment.
*******************************************************************************/
int32_t ad9467_output_current_adj(int32_t adj)
{
    uint32_t ret = 0;

    if((adj > 0) || (adj <= 7))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_OUT_ADJ,
                                     AD9467_OUT_ADJ_OUT_CURRENT(adj),
                                     AD9467_OUT_ADJ_OUT_CURRENT(0x7));
        if(ret < 0)
            {
                return ret;
            }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_OUT_ADJ);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_OUT_ADJ_OUT_CURRENT(0x7)) >> 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Activates the normal (0) or inverted (1) DCO clock.
 *
 * @param invert - Invert option.
 *				   Example: 0 - Activates the normal DCO clock(default);
 *							1 - Activates the inverted DCO clock.
 *
 * @return Negative error code or the DCO clock inversion status.
*******************************************************************************/
int32_t ad9467_dco_clock_invert(int32_t invert)
{
    int32_t ret = 0;

    if((invert == 0) || (invert == 1))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_OUT_PHASE,
                                     invert * AD9467_OUT_PHASE_DCO_INV,
                                     AD9467_OUT_PHASE_DCO_INV);
        if(ret < 0)
            {
                return ret;
            }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_OUT_PHASE);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_OUT_PHASE_DCO_INV) != 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Configures the clock delay setting.
 *
 * @param delay - The clock delay setting in ps {0, 100, 200, ..., 3100, 3200}.
                  Setting the delay to 0 disables the DCO output clock delay.
 *
 * @return Negative error code or the set clock delay.
*******************************************************************************/
int32_t ad9467_dco_output_clock_delay(int32_t delay)
{
    int32_t ret = 0;

    if(!delay)
    {
        ret = ad9467_write(AD9467_REG_OUT_DELAY, 0x00);
        if(ret < 0)
        {
            return ret;
        }
    }
    else if( delay <= 3200)
    {
        delay = (delay - 100) / 100;
        ret = ad9467_write(AD9467_REG_OUT_DELAY,
                           AD9467_OUT_DELAY_DCO_DLY_EN |
                           AD9467_OUT_DELAY_OUT_DLY(delay));
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_OUT_DELAY);
        if(ret < 0)
        {
            return ret;
        }
        if(ret & AD9467_OUT_DELAY_DCO_DLY_EN)
        {
            ret  = (ret & AD9467_OUT_DELAY_OUT_DLY(0x1F)) * 100 + 100;
            return ret;
        }
        else
        {
            return 0;
        }
    }

    return ret;
}

/***************************************************************************//**
 * @brief Configures the full-scale input voltage selection.
 *
 * @param span - Full-scale input voltage selection.
 *				 Example: 2.0 V p-p
 *                        2.1 V p-p
 *                        2.2 V p-p
 *                        2.3 V p-p
 *                        2.4 V p-p
 *                        2.5 V p-p(default)
 *
 * @return  Negative error code or the set input voltage selection.
*******************************************************************************/
float ad9467_full_scale_range(float v_fs)
{
    int32_t ret = 0;
    int8_t  val = -1;

    if(v_fs == 2.0)
    {
        val = 0x00;
    }
    else if((v_fs >= 2.1) && (v_fs <= 2.5))
    {
        val = ((int8_t)(v_fs * 10) % 20) + 0x05;
    }
    if(val != -1)
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_V_REF,
                                     AD9467_V_REF_IN_FS_RANGE(val),
                                     AD9467_V_REF_IN_FS_RANGE(0xF));
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_V_REF);
        if(ret < 0)
        {
            return ret;
        }
        val = ret & AD9467_V_REF_IN_FS_RANGE(0xF);
        if(val == 0x00)
        {
            return 2.0;
        }
        else
        {
            return (((float)(val - 0x05) / 10) + 2.0);
        }
    }

    return ret;
}

/***************************************************************************//**
 * @brief Sets the AC coupling(0) or DC coupling(1) mode.
 *
 * @param coupling_mode - Input coupling mode selection.
 *                        Example: 0 - AC coupling;
 *                                 1 - DC coupling(default).
 *
 * @return  Negative error code or the set coupling mode.
*******************************************************************************/
int32_t ad9467_analog_input_coupling(int32_t coupling_mode)
{
    int32_t ret = 0;

    if((coupling_mode == 0) || (coupling_mode == 7))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_ANALOG_INPUT,
                                   coupling_mode * AD9467_ANALOG_INPUT_COUPLING,
                                   AD9467_ANALOG_INPUT_COUPLING);
        if(ret < 0)
            {
                return ret;
            }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_ANALOG_INPUT);
        if(ret < 0)
        {
            return ret;
        }
        return (ret & AD9467_ANALOG_INPUT_COUPLING) != 0;
    }

    return ret;
}

/***************************************************************************//**
 * @brief Changes the input buffer current(1).
 *
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
 *
 * @return  Negative error code or the set buffer current.
*******************************************************************************/
int32_t ad9467_buffer_current_1(int32_t percentage)
{
    int32_t ret = 0;
    int8_t  val = 0;

    if((percentage >= -100) && (percentage <= 530))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_BUFF_CURRENT_1,
                                     AD9467_BUFF_CURRENT_1(percentage / 10),
                                     AD9467_BUFF_CURRENT_1(0x3F));
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_BUFF_CURRENT_1);
        if(ret < 0)
        {
            return ret;
        }
        val = (ret & AD9467_BUFF_CURRENT_1(0x3F)) >> 2;
        if(val > 53)
        {
            val -= 0x40;
        }
        return (val * 10);
    }

    return ret;
}

/***************************************************************************//**
 * @brief Changes the input buffer current(2).
 *
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
 *
 * @return  Negative error code or the set buffer current.
*******************************************************************************/
int32_t ad9467_buffer_current_2(int32_t percentage)
{
    int32_t ret = 0;
    int8_t  val = 0;

    if((percentage >= -100) && (percentage <= 530))
    {
        ret = ad9467_set_bits_to_reg(AD9467_REG_BUFF_CURRENT_2,
                                     AD9467_BUFF_CURRENT_2(percentage / 10),
                                     AD9467_BUFF_CURRENT_2(0x3F));
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        ret = ad9467_read(AD9467_REG_BUFF_CURRENT_2);
        if(ret < 0)
        {
            return ret;
        }
        val = (ret & AD9467_BUFF_CURRENT_2(0x3F)) >> 2;
        if(val > 53)
        {
            val -= 0x40;
        }
        return (val * 10);
    }

    return ret;
}

/***************************************************************************//**
 * @brief Initiates a transfer and waits for the operation to end.
 *
 * @return  Negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9467_transfer(void)
{
    int32_t ret    = 0;
    int8_t  sw_bit = 0;

    ret = ad9467_write(AD9467_REG_DEVICE_UPDATE, AD9467_DEVICE_UPDATE_SW);
    if(ret < 0)
    {
        return ret;
    }
    do
    {
        ret = ad9467_read(AD9467_REG_DEVICE_UPDATE);
        if(ret < 0)
        {
            return ret;
        }
        sw_bit = ret & AD9467_REG_DEVICE_UPDATE;
    }while(sw_bit == 1);

    return 0;
}
