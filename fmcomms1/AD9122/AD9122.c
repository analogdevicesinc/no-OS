/***************************************************************************//**
 *   @file   AD9122.c
 *   @brief  Implementation of AD9122 Driver.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "spi_interface.h"
#include "AD9122.h"

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param registerAddress - The address of the register to write to.
 * @param registerValue - The value to write to the register.
 *
 * @return None.
*******************************************************************************/
int32_t ad9122_write(uint8_t registerAddress,
				     uint8_t registerValue)
{
	uint8_t regAddr = 0;
	
	regAddr = (0x7F & registerAddress);
	return SPI_Write(SPI_SEL_AD9122, regAddr, registerValue);
}

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param regAddress - The address of the register to read.
 *
 * @return registerValue - The register's value or negative error code.
*******************************************************************************/
int32_t ad9122_read(uint8_t registerAddress)
{
	uint8_t regAddr		= 0;
	uint32_t registerValue = 0;
	int32_t ret;

	regAddr = 0x80 + (0x7F & registerAddress);
    ret = SPI_Read(SPI_SEL_AD9122, regAddr, &registerValue);

	return (ret < 0 ? ret : (int32_t)registerValue);
}

/***************************************************************************//**
 * @brief Initializes the AD9643. 
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_setup()
{
	int32_t timeout;

	ad9122_reset();
	ad9122_powerdown_AUX_DAC(0);
	ad9122_write(AD9122_REG_DATA_FORMAT,
				 AD9122_DATA_FORMAT_BINARY);
	ad9122_write(AD9122_REG_CLK_REC_CTRL,
				 AD9122_CLK_REC_CTRL_DACCLK_CROSS_CORRECTION |
				 AD9122_CLK_REC_CTRL_REFCLK_CROSS_CORRECTION |
				 0xF);
	ad9122_write(AD9122_REG_PLL_CTRL_1,
				 AD9122_PLL_CTRL_1_PLL_MANUAL_EN);
	ad9122_write(AD9122_REG_PLL_CTRL_2,
				 AD9122_PLL_CTRL_2_PLL_LOOP_BANDWIDTH(3) |
				 AD9122_PLL_CTRL_2_PLL_CHARGE_PUMP_CURRENT(0x11));
	ad9122_write(AD9122_REG_PLL_CTRL_3,
				 AD9122_PLL_CTRL_3_N2(3) |
				 AD9122_PLL_CTRL_3_PLL_CROSS_CTRL_EN |
				 AD9122_PLL_CTRL_3_N0(1) |
				 AD9122_PLL_CTRL_3_N1(2));
	ad9122_write(AD9122_REG_SYNC_CTRL_1,
				 AD9122_SYNC_CTRL_1_DATA_FIFO_RATE_TOGGLE |
				 AD9122_SYNC_CTRL_1_RISING_EDGE_SYNC);
	ad9122_write(AD9122_REG_SYNC_CTRL_2, 0x00);
	ad9122_write(AD9122_REG_FIFO_CTRL,
				 AD9122_FIFO_CTRL_FIFO_PHA_OFFSET(4));
	ad9122_write(AD9122_REG_FIFO_STATUS_1, 0x00);
	ad9122_write(AD9122_REG_DATAPATH_CTRL,
				 AD9122_DATAPATH_CTRL_BYPASS_PREMOD |
				 AD9122_DATAPATH_CTRL_BYPASS_INV_SINC |
				 AD9122_DATAPATH_CTRL_BYPASS_NCO);
	ad9122_write(AD9122_REG_HB1_CTRL, AD9122_HB1_CTRL_BYPASS_HB1);
	ad9122_write(AD9122_REG_HB2_CTRL, AD9122_HB2_CTRL_BYPASS_HB2);
	ad9122_write(AD9122_REG_HB3_CTRL, AD9122_HB3_CTRL_BYPASS_HB3);
	ad9122_offset_I_DAC(0x00AE);
	ad9122_offset_Q_DAC(0x0000);
	ad9122_fs_adj_I_DAC(0x0000);
	ad9122_fs_adj_Q_DAC(0x0000);
	ad9122_write(AD9122_REG_DIE_TEMP_RANGE_CTRL,
				 AD9122_DIE_TEMP_RANGE_CTRL_REF_CURRENT(1));
	ad9122_write(AD9122_REG_FIFO_STATUS_1,
				 AD9122_FIFO_STATUS_1_FIFO_SOFT_ALIGN_REQ);

	timeout = 0xFFFF;
	while(((ad9122_read(AD9122_REG_FIFO_STATUS_1) &
		   AD9122_FIFO_STATUS_1_FIFO_SOFT_ALIGN_ACK) == 0) &&
		   --timeout);
	if(!timeout)
		return -1;

	ad9122_write(AD9122_REG_FIFO_STATUS_1, 0x00);
	ad9122_write(AD9122_REG_SYNC_CTRL_1,
				 AD9122_SYNC_CTRL_1_SYNC_EN |
				 AD9122_SYNC_CTRL_1_RISING_EDGE_SYNC);

	timeout = 0xFFFF;
	while(((ad9122_read(AD9122_REG_SYNC_STATUS_1) &
		  AD9122_SYNC_STATUS_1_SYNC_LOCKED) == 0) &&
		  --timeout);
	if(!timeout)
		return -1;

	return 0;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_reset(void)
{
	int32_t ret;

    ret = ad9122_write(AD9122_REG_COMM, AD9122_COMM_RESET);
    if(ret < 0)
        return ret;

	ret = ad9122_write(AD9122_REG_COMM, 0x00);

    return ret;
}

/***************************************************************************//**
 * @brief Sets the power state of the I DAC. 
 *
 * @param pd - Power state: 0 - powered, 1 - power down. For any other 
 *             value the function reads back the power state from the hardware.                   
 *
 * @return Returns the set power state.
*******************************************************************************/
int32_t ad9122_powerdown_I_DAC(int32_t pd)
{
	int32_t ret;
    int32_t regValue = 0;
	
	if(pd == 1)
	{
		regValue = ad9122_read(AD9122_REG_POWER_CTRL);
		regValue |= AD9122_POWER_CTRL_PD_I_DAC;
		ret = ad9122_write(AD9122_REG_POWER_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
    else if(pd == 0)
	{
		regValue = ad9122_read(AD9122_REG_POWER_CTRL);
		regValue &= ~AD9122_POWER_CTRL_PD_I_DAC;
		ret = ad9122_write(AD9122_REG_POWER_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_POWER_CTRL);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_POWER_CTRL_PD_I_DAC);
}

/***************************************************************************//**
 * @brief Sets the power state of the Q DAC. 
 *
 * @param pd - Power state: 0 - powered, 1 - power down. For any other 
 *             value the function reads back the power state from the hardware.                   
 *
 * @return Returns the set power state.
*******************************************************************************/
int32_t adD9122_powerdown_Q_DAC(int32_t pd)
{
	int32_t ret;
    int32_t regValue = 0;
	
	if(pd == 1)
	{
		regValue = ad9122_read(AD9122_REG_POWER_CTRL);
		regValue |= AD9122_POWER_CTRL_PD_Q_DAC;
		ret = ad9122_write(AD9122_REG_POWER_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
	else if(pd == 0)
	{
		regValue = ad9122_read(AD9122_REG_POWER_CTRL);
		regValue &= ~AD9122_POWER_CTRL_PD_Q_DAC;
		ret = ad9122_write(AD9122_REG_POWER_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_POWER_CTRL);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_POWER_CTRL_PD_Q_DAC);
}

/***************************************************************************//**
 * @brief Sets the power state of the data receiver. 
  *
 * @param pd - Pwer state: 0 - powered, 1 - power down. For any other 
 *             value the function reads back the power state from the hardware.                   
 *
 * @return Returns the set power state.
*******************************************************************************/
int32_t ad9122_powerdown_DATA_REC(int32_t pd)
{
	int32_t ret;
    uint32_t regValue = 0;
	
	if(pd == 1)
	{
		regValue = ad9122_read(AD9122_REG_POWER_CTRL);
		regValue |= AD9122_POWER_CTRL_PD_DATA_REC;
		ret = ad9122_write(AD9122_REG_POWER_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
	else if(pd == 0)
	{
		regValue = ad9122_read(AD9122_REG_POWER_CTRL);
		regValue &= ~AD9122_POWER_CTRL_PD_DATA_REC;
		ret = ad9122_write(AD9122_REG_POWER_CTRL, regValue);
        if(ret < 0)
            return ret;
    }
    else
    {
	    regValue = ad9122_read(AD9122_REG_POWER_CTRL);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_POWER_CTRL_PD_DATA_REC);
}

/***************************************************************************//**
 * @brief Sets the power state of the aux DAC.
  *
 * @param pd - Power state: 0 - powered, 1 - power down. For any other 
 *             value the function reads back the power state from the hardware.                   
 *
 * @return Returns the set power state.
*******************************************************************************/
int32_t ad9122_powerdown_AUX_DAC(int32_t pd)
{
	int32_t ret;
    uint32_t regValue = 0;
	
	if(pd == 1)
	{
		regValue = ad9122_read(AD9122_REG_POWER_CTRL);
		regValue |= AD9122_POWER_CTRL_PD_AUX_ADC;
		ret = ad9122_write(AD9122_REG_POWER_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
	else if(pd == 0)
	{
		regValue = ad9122_read(AD9122_REG_POWER_CTRL);
		regValue &= ~AD9122_POWER_CTRL_PD_AUX_ADC;
		ret = ad9122_write(AD9122_REG_POWER_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_POWER_CTRL);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_POWER_CTRL_PD_AUX_ADC);
}

/***************************************************************************//**
 * @brief Enables or disables the DACCLK duty correction. 
 *
 * @param en - Enable state: 0 - disabled, 1 - enabled. For any other 
 *             value the function reads back the state from the hardware.                   
 *
 * @return Returns duty correction state.
*******************************************************************************/
int32_t ad9122_duty_correction_DACCLK (int32_t en)
{
	int32_t ret;
    uint8_t regValue = 0;

	if(en == 1)
	{
		regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
		regValue |= AD9122_CLK_REC_CTRL_DACCLK_DUTY_CORRECTION;
		ret = ad9122_write(AD9122_REG_CLK_REC_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
	else if(en == 0)
	{
		regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
		regValue &= ~AD9122_CLK_REC_CTRL_DACCLK_DUTY_CORRECTION;
		ret = ad9122_write(AD9122_REG_CLK_REC_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_CLK_REC_CTRL_DACCLK_DUTY_CORRECTION);
}

/***************************************************************************//**
 * @brief Enables or disables the REFCLK duty correction. 
 *
 * @param en - Enable state: 0 - disabled, 1 - enabled. For any other 
 *             value the function reads back the state from the hardware.                   
 *
 * @return Returns duty correction state.
*******************************************************************************/
int32_t ad9122_duty_correction_REFCLK (int32_t en)
{
	int32_t ret;
    uint8_t regValue = 0;

	if(en == 1)
	{
		regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
		regValue |= AD9122_CLK_REC_CTRL_REFCLK_DUTY_CORRECTION;
		ret = ad9122_write(AD9122_REG_CLK_REC_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
	else if(en == 0)
	{
		regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
		regValue &= ~AD9122_CLK_REC_CTRL_REFCLK_DUTY_CORRECTION;
		ret = ad9122_write(AD9122_REG_CLK_REC_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_CLK_REC_CTRL_REFCLK_DUTY_CORRECTION);
}

/***************************************************************************//**
 * @brief Enables or disables the DACCLK cross correction. 
 *
 * @param en - Enable state: 0 - disabled, 1 - enabled. For any other 
 *             value the function reads back the state from the hardware.                   
 *
 * @return Returns cross correction state.
*******************************************************************************/
int32_t ad9122_cross_correction_DACCLK (int32_t en)
{
	int32_t ret;
    uint8_t regValue = 0;

	if(en == 1)
	{
		regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
		regValue |= AD9122_CLK_REC_CTRL_DACCLK_CROSS_CORRECTION;
		ret = ad9122_write(AD9122_REG_CLK_REC_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
	else if(en == 0)
	{
		regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
		regValue &= ~AD9122_CLK_REC_CTRL_DACCLK_CROSS_CORRECTION;
		ret = ad9122_write(AD9122_REG_CLK_REC_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_CLK_REC_CTRL_DACCLK_CROSS_CORRECTION);
}

/***************************************************************************//**
 * @brief Enables or disables the REFCLK cross correction. 
 *
 * @param en - Enable state: 0 - disabled, 1 - enabled. For any other 
 *             value the function reads back the state from the hardware.                   
 *
 * @return Returns cross correction state.
*******************************************************************************/
int32_t ad9122_cross_correction_REFCLK (int32_t en)
{
	int32_t ret;
    uint8_t regValue = 0;

	if(en == 1)
	{
		regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
		regValue |= AD9122_CLK_REC_CTRL_REFCLK_CROSS_CORRECTION;
		ret = ad9122_write(AD9122_REG_CLK_REC_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
	else if(en == 0)
	{
		regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
		regValue &= ~AD9122_CLK_REC_CTRL_REFCLK_CROSS_CORRECTION;
		ret = ad9122_write(AD9122_REG_CLK_REC_CTRL, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_CLK_REC_CTRL);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_CLK_REC_CTRL_REFCLK_CROSS_CORRECTION);
}

/***************************************************************************//**
 * @brief Enables or disables the PLL. 
 *
 * @param en - Enable state: 0 - disabled, 1 - enabled. For any other 
 *             value the function reads back the state from the hardware.                   
 *
 * @return Returns the enable PLL state.
*******************************************************************************/
int32_t ad9122_pll_enable(int32_t en)
{
	int32_t ret;
    uint8_t regValue = 0;

	if(en == 1)
	{
		regValue = ad9122_read(AD9122_REG_PLL_CTRL_1);
		regValue |= AD9122_PLL_CTRL_1_PLL_EN;
		ret = ad9122_write(AD9122_REG_PLL_CTRL_1, regValue);
        if(ret < 0)
            return ret;
	}
	else if(en == 0)
	{
		regValue = ad9122_read(AD9122_REG_PLL_CTRL_1);
		regValue &= ~AD9122_PLL_CTRL_1_PLL_EN;
		ret = ad9122_write(AD9122_REG_PLL_CTRL_1, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_PLL_CTRL_1);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_PLL_CTRL_1_PLL_EN);
}

/***************************************************************************//**
 * @brief Enables or disables the manual selection of the PLL band.
 *
 * @param en - Enable state: 0 - disabled, 1 - enabled. For any other 
 *             value the function reads back the state from the hardware.                   
 *
 * @return Returns the manual enable PLL state.
*******************************************************************************/
int32_t ad9122_pll_manual_enable(int32_t en)
{
	int32_t ret;
    uint8_t regValue = 0;

	if(en == 1)
	{
		regValue = ad9122_read(AD9122_REG_PLL_CTRL_1);
		regValue |= AD9122_PLL_CTRL_1_PLL_MANUAL_EN;
		ret = ad9122_write(AD9122_REG_PLL_CTRL_1, regValue);
        if(ret < 0)
            return ret;
	}
	else if(en == 0)
	{
		regValue = ad9122_read(AD9122_REG_PLL_CTRL_1);
		regValue &= ~AD9122_PLL_CTRL_1_PLL_MANUAL_EN;
		ret = ad9122_write(AD9122_REG_PLL_CTRL_1, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_PLL_CTRL_1);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_PLL_CTRL_1_PLL_MANUAL_EN);
}

/***************************************************************************//**
 * @brief Enables or disables the synchronization logic. 
 *
 * @param en - Enable state: 0 - disabled, 1 - enabled. For any other 
 *             value the function reads back the state from the hardware.                   
 *
 * @return Returns the enable synchronization logic state.
*******************************************************************************/
int32_t ad9122_sync_enable(int32_t en)
{
	int32_t ret;
    uint8_t regValue = 0;

	if(en == 1)
	{
		regValue = ad9122_read(AD9122_REG_SYNC_CTRL_1);
		regValue |= AD9122_SYNC_CTRL_1_SYNC_EN;
		ret = ad9122_write(AD9122_REG_SYNC_CTRL_1, regValue);
        if(ret < 0)
            return ret;
	}
	else if(en == 0)
	{
		regValue = ad9122_read(AD9122_REG_SYNC_CTRL_1);
		regValue &= ~AD9122_SYNC_CTRL_1_SYNC_EN;
		ret = ad9122_write(AD9122_REG_SYNC_CTRL_1, regValue);
        if(ret < 0)
            return ret;
	}
    else
    {
	    regValue = ad9122_read(AD9122_REG_SYNC_CTRL_1);
        if(regValue < 0)
            return regValue;
    }
	
	return (regValue & AD9122_SYNC_CTRL_1_SYNC_EN);
}

/***************************************************************************//**
 * @brief Sets the full-scale current for I DAC. 
 *
 * @param fs_adj - Full scale current value. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set full-scale current.
*******************************************************************************/
int32_t ad9122_fs_adj_I_DAC(int32_t fs_adj)
{
	uint8_t sleepBit	= 0;
	uint8_t regData1	= 0;
    uint8_t regData2	= 0;

	if(fs_adj != INT32_MAX)
	{
		/* Read the current state of the sleep bit */
		sleepBit = (ad9122_read(AD9122_REG_I_DAC_CTRL) &
					AD9122_I_DAC_CTRL_I_DAC_SLEEP);
		
        /* Set the full-scale value and keep the state of the sleep bit. */
		regData1 = AD9122_I_DAC_CTRL_I_DAC_FS_ADJ_9_8((fs_adj & 0x0300) >> 8);
		ad9122_write(AD9122_REG_I_DAC_CTRL, (sleepBit | regData1));
		regData2 = AD9122_I_DAC_FS_ADJ_I_DAC_FS_ADJ_7_0((fs_adj & 0x00FF) >> 0);
		ad9122_write(AD9122_REG_I_DAC_FS_ADJ, regData2);
        
        /* Compute the set full scale current */
        fs_adj = ((regData1 & 0x3) << 8) + (regData2 << 0);
	}
    else
    {
	    fs_adj = ((ad9122_read(AD9122_REG_I_DAC_CTRL) & 0x3) << 8) +
			     (ad9122_read(AD9122_REG_I_DAC_FS_ADJ) << 0);
    }
	
	return fs_adj;
}

/***************************************************************************//**
 * @brief Sets the full-scale current for Q DAC. 
 *
 * @param fs_adj - Full scale current value. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set full-scale current.
*******************************************************************************/
int32_t ad9122_fs_adj_Q_DAC(int32_t fs_adj)
{
	uint8_t sleepBit	= 0;
	uint8_t regData1	= 0;
    uint8_t regData2	= 0;
	
	if(fs_adj != INT32_MAX)
	{
		/* Read the current state of the sleep bit */
		sleepBit = (ad9122_read(AD9122_REG_Q_DAC_CTRL) &
					AD9122_Q_DAC_CTRL_Q_DAC_SLEEP);

		/* Set the full-scale value and keep the state of the sleep bit. */
		regData1 = AD9122_Q_DAC_CTRL_Q_DAC_FS_ADJ_9_8((fs_adj & 0x0300) >> 8);
		ad9122_write(AD9122_REG_Q_DAC_CTRL, (sleepBit | regData1));
		regData2 = AD9122_Q_DAC_FS_ADJ_Q_DAC_FS_ADJ_7_0((fs_adj & 0x00FF) >> 0);
		ad9122_write(AD9122_REG_Q_DAC_FS_ADJ, regData2);

        /* Compute the set full scale current */
        fs_adj = ((regData1 & 0x3) << 8) + (regData2 << 0);
	}
    else
    {
	    fs_adj = ((ad9122_read(AD9122_REG_Q_DAC_CTRL) & 0x3) << 8) +
			     (ad9122_read(AD9122_REG_Q_DAC_FS_ADJ) << 0);
    }
	
	return fs_adj;
}

/***************************************************************************//**
 * @brief Sets the offset of the I DAC. 
 *
 * @param offset - The value that is added directly to the samples written 
 *                 to the I DAC. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set offset.
*******************************************************************************/
int32_t ad9122_offset_I_DAC(int32_t offset)
{
	uint8_t regData1 = 0;
    uint8_t regData2 = 0;
	
	if(offset != INT32_MAX)
	{
		regData1 = (offset & 0x00FF) >> 0;
		ad9122_write(AD9122_REG_I_DAC_OFFSET_LSB, regData1);
		regData2 = (offset & 0xFF00) >> 8;
		ad9122_write(AD9122_REG_I_DAC_OFFSET_MSB, regData2);
	}
    else
    {
	    offset = (ad9122_read(AD9122_REG_I_DAC_OFFSET_MSB) << 8) +
			     (ad9122_read(AD9122_REG_I_DAC_OFFSET_LSB) << 0);
    }
	
	return offset;
}

/***************************************************************************//**
 * @brief Sets the offset of the Q DAC. 
 *		  
 * @param offset - The value that is added directly to the samples written 
 *                 to the Q DAC. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set offset.
*******************************************************************************/
int32_t ad9122_offset_Q_DAC(int32_t offset)
{
	uint8_t regData1 = 0;
    uint8_t regData2 = 0;
	
	if(offset != INT32_MAX)
	{
		regData1 = (offset & 0x00FF) >> 0;
		ad9122_write(AD9122_REG_Q_DAC_OFFSET_LSB, regData1);
		regData2 = (offset & 0xFF00) >> 8;
		ad9122_write(AD9122_REG_Q_DAC_OFFSET_MSB, regData2);
	}
    else
    {
	    offset = (ad9122_read(AD9122_REG_Q_DAC_OFFSET_MSB) << 8) +
			     (ad9122_read(AD9122_REG_Q_DAC_OFFSET_LSB) << 0);
    }
	
	return offset;
}

/***************************************************************************//**
 * @brief Sets the phase adjustment of the I DAC. 
 *
 * @param offset - The value that is added directly to the samples written 
 *                 to the I DAC. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set phase adjustment.
*******************************************************************************/
int32_t ad9122_phaseAdj_I_DAC(int32_t phaseAdj)
{
	uint8_t regData1 = 0;
    uint8_t regData2 = 0;
	
	if(phaseAdj != INT32_MAX)
	{
		regData1 = (phaseAdj & 0x00FF) >> 0;
		ad9122_write(AD9122_REG_I_PHA_ADJ_LSB, regData1);
		regData2 = (phaseAdj & 0x0300) >> 8;
		ad9122_write(AD9122_REG_I_PHA_ADJ_MSB, regData2);
        phaseAdj &= 0x03FF;
	}
    else
    {
	    phaseAdj = ((ad9122_read(AD9122_REG_I_PHA_ADJ_MSB) & 0x03) << 8) +
			       (ad9122_read(AD9122_REG_I_PHA_ADJ_LSB) << 0);
    }
	
	return phaseAdj;
}

/***************************************************************************//**
 * @brief Sets the phase adjustment of the Q DAC. 
 *		  
 * @param offset - The value that is added directly to the samples written 
 *                 to the Q DAC. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set phase adjustment.
*******************************************************************************/
int32_t ad9122_phaseAdj_Q_DAC(int32_t phaseAdj)
{
	uint8_t regData1 = 0;
    uint8_t regData2 = 0;
	
	if(phaseAdj != INT32_MAX)
	{
		regData1 = (phaseAdj & 0x00FF) >> 0;
		ad9122_write(AD9122_REG_Q_PHA_ADJ_LSB, regData1);
		regData2 = (phaseAdj & 0x0300) >> 8;
		ad9122_write(AD9122_REG_Q_PHA_ADJ_MSB, regData2);
        phaseAdj &= 0x03FF;
	}
    else
    {
	    phaseAdj = ((ad9122_read(AD9122_REG_Q_PHA_ADJ_MSB) & 0x03) << 8) +
			       (ad9122_read(AD9122_REG_Q_PHA_ADJ_LSB) << 0);
    }
	
	return phaseAdj;
}


/***************************************************************************//**
 * @brief Returns the status of the PLL lock.
 *******************************************************************************/
int32_t ad9122_status_pll_lock()
{
	uint8_t regValue	= 0;
	uint8_t status	= 0;
	
	regValue = ad9122_read(AD9122_REG_EVENT_FLAG_1);
	status = (regValue & AD9122_EVENT_FLAG_1_PLL_LOCKED) != 0;
	
	return status;
}

/***************************************************************************//**
 * @brief Returns the status of the sync lock.
 ******************************************************************************/
int32_t ad9122_status_sync_lock()
{
	uint8_t regValue	= 0;
	uint8_t status	= 0;
	
	regValue = ad9122_read(AD9122_REG_EVENT_FLAG_1);
	status = (regValue & AD9122_EVENT_FLAG_1_SYNC_SIGNAL_LOCKED) != 0;
	
	return status;
}

/***************************************************************************//**
 * @brief Returns the FIFO warning 1 status.
 ******************************************************************************/
int32_t ad9122_status_fifo_warn1()
{
	uint8_t regValue	= 0;
	uint8_t status	= 0;
	
	regValue = ad9122_read(AD9122_REG_EVENT_FLAG_1);
	status = (regValue & AD9122_EVENT_FLAG_1_FIFO_WARNING_1) != 0;
	
	return status;
}

/***************************************************************************//**
 * @brief Returns the FIFO warning 2 status.
 ******************************************************************************/
int32_t ad9122_status_fifo_warn2()
{
	uint8_t regValue	= 0;
	uint8_t status	= 0;
	
	regValue = ad9122_read(AD9122_REG_EVENT_FLAG_1);
	status = (regValue & AD9122_EVENT_FLAG_1_FIFO_WARNING_2) != 0;
	
	return status;
}
