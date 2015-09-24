/***************************************************************************//**
 *   @file   AD9739A.c
 *   @brief  Implementation of AD9739A Driver.
 *   @author Bancisor Mihai
********************************************************************************
 * Copyright 2012-2015(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <malloc.h>
#include <stdio.h>
#include "platform_drivers.h"
#include "ad9739a.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define FDATA 2500	// for 2.5 GSPS

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
uint8_t ad9739a_slave_select;
struct ad9739a_state
{
	struct ad9739a_platform_data *pdata;
}ad9739a_st;

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param registerAddress - The address of the register to write to.
 * @param registerValue   - The value to write to the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9739a_write(unsigned char registerAddress,
					unsigned char registerValue)
{
	uint8_t data[2] = {0, 0};

	data[0] = AD9739A_WRITE | (0x7F & registerAddress);
	data[1] = registerValue;

	return spi_write_and_read(ad9739a_slave_select, data, 2);
}

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param regAddress	 - The address of the register to read.
 *
 * @return registerValue - The register's value or negative error code.
*******************************************************************************/
int32_t ad9739a_read(unsigned char registerAddress)
{
	uint8_t txData[2] = {0, 0};
	int32_t ret;

	txData[0] = AD9739A_READ | (0x7F & registerAddress);

	ret = spi_write_and_read(ad9739a_slave_select, txData, 2);

	return (ret < 0 ? ret : (int32_t)txData[1]);
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9739a_reset(void)
{
	int32_t ret;

	/* Software reset to default SPI values. */
	ret = ad9739a_write(AD9739A_REG_MODE,
					   AD9739A_MODE_RESET);
	if(ret < 0)
	{
		return ret;
	}
	ret = ad9739a_write(AD9739A_REG_MODE, 0x00);	// Clear the reset bit.

	return ret;
}

/***************************************************************************//**
 * @brief Powers down LVDS interface and TxDAC.
 *
 * @param pwrConfig - Selects the modules to be powered-down.
 *					  Example:
 *			   AD9739A_POWER_DOWN_LVDS_DRVR_PD | AD9739A_POWER_DOWN_LVDS_RCVR_PD
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9739a_power_down(unsigned char pwrConfig)
{
	int32_t ret;
	int32_t regData;

	/* Write to register if pwrConfig contains valid parameters. Else read and
	return the register value. */
	if((pwrConfig & ((1 << 7) | (1 << 6) | (1 << 3) | (1 << 2))) == 0)
	{
		ret = ad9739a_write(AD9739A_REG_POWER_DOWN, pwrConfig);
	}
	else
	{
		regData = ad9739a_read(AD9739A_REG_POWER_DOWN);
		return regData;
	}

	return ret;	
}

/***************************************************************************//**
 * @brief Sets the normal baseband mode or mix-mode.
 *
 * @param mode - mode of operation.
 *				 Example: AD9739A_DAC_DEC_NORMAL_BASEBAND - normal baseband mode;
 *						  AD9739A_DAC_DEC_MIX_MODE - mix-mode.  
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9739a_operation_mode(unsigned char mode)
{
	int32_t ret;
	int32_t regData;

	if((mode == AD9739A_DAC_DEC_NORMAL_BASEBAND) |
	  (mode == AD9739A_DAC_DEC_MIX_MODE))
	{
		ret = ad9739a_write(AD9739A_REG_DEC_CNT, AD9739A_DEC_CNT_DAC_DEC(mode));
	}
	else
	{
		regData = ad9739a_read(AD9739A_REG_DEC_CNT);
		return regData;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the full-scale output current for the DAC. 
 *
 * @param fs_val - The desired full-scale output current. Accepted values:
 *				   8.7 to 32.7 (mA) and 0. When fs_val is set to 0 the DAC
 *				   output is disabled(sleep).
 *
 * @return Returns the actual set full-scale current or negative error code.
*******************************************************************************/
float ad9739a_DAC_fs_current(float fs_val)
{
	float   actual_fs = 0;
	int16_t regData   = 0;
	int32_t ret	   = 0;

	if((fs_val >= 8.7) && (fs_val <= 31.7))
	{
		regData = (int16_t)((fs_val - 8.58) / 0.0226); 
		ret = ad9739a_write(AD9739A_REG_FSC_1, (regData & 0xFF));
		if(ret < 0)
		{
			return (float)ret;
		}
		ret = ad9739a_write(AD9739A_REG_FSC_2, ((regData & 0x300) >> 8));
		if(ret < 0)
		{
			return (float)ret;
		}
	}
	else if(fs_val == 0)
	{
		ret = ad9739a_write(AD9739A_REG_FSC_2, AD9739A_FSC_2_Sleep);
		if(ret < 0)
		{
			return (float)ret;
		}
	}
	else
	{
		regData = ad9739a_read(AD9739A_REG_FSC_1) & 0xFF;
		if(regData < 0)
		{
			return -1;
		}
		ret = ad9739a_read(AD9739A_REG_FSC_2);
		if(ret < 0)
		{
			return (float)ret;
		}
		regData |= (ret & 0x3) << 8;
		actual_fs = (float)(regData * 0.0226  + 8.58);
		return actual_fs;
	}

	return (float)ret;
}

/***************************************************************************//**
 * @brief Delay for a number of fdata clock cycles. 
 *
 * @param cycles - Number of cycles to wait for. 
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t delay_fdata_cycles(uint32_t cycles)
{
	volatile uint32_t count;
	uint32_t		  us;
	uint8_t		   cpu_clk_period = 10;  					// 10 ns
	uint32_t		  cpu_clks_in_a_us = 1000 / cpu_clk_period;	// 100 cpu clocks in a microsecond
	uint32_t		  delay;

	us = (cycles/FDATA);					// convert DAC cycles to microseconds delay
	delay = ((us + 9) * cpu_clks_in_a_us);	// add 9-10 microseconds to the delay as a precaution
	for (count = 0; count < delay; count++)
	{
		asm("nop");
	}

	return 0;
}

/***************************************************************************//**
 * @brief Initializes the AD9739A. 
 *
 * @param spiBaseAddr - SPI peripheral AXI base address.
 * @param ssNo		  - Slave select line on which the slave is connected.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9739a_setup(uint32_t spi_device_id, uint8_t slave_select,
		ad9739a_init_param init_param)
{
	struct  ad9739a_state *st = &ad9739a_st;
	int32_t ret = 0;
	float   fret = 0;
	uint8_t dll_loop_lock_counter = 0;
	uint8_t dll_loop_locked = 0;

	ad9739a_slave_select = slave_select;
	spi_init(spi_device_id, 0, 0);

	st->pdata = (struct ad9739a_platform_data *)malloc(sizeof(*st->pdata));
	if (!st->pdata)
		return -1;

	st->pdata->common_mode_voltage_dacclk_p =
			init_param.common_mode_voltage_dacclk_p;
	st->pdata->common_mode_voltage_dacclk_n =
			init_param.common_mode_voltage_dacclk_n;
	st->pdata->full_scale_current = init_param.full_scale_current;

	/* Set 4-wire SPI, MSB first. */
	ret = ad9739a_write(AD9739A_REG_MODE, 0x00);
	if(ret < 0)
	{
		return ret;
	}
	ret = ad9739a_reset();
	if(ret < 0)
	{
		return ret;
	}
	/* Set the common-mode voltage of DACCLK_P and DACCLK_N inputs. */
	ret = ad9739a_write(AD9739A_REG_CROSS_CNT1,
			AD9739A_CROSS_CNT1_CLKP_OFFSET(st->pdata->common_mode_voltage_dacclk_p));
	if(ret < 0)
	{
		return ret;
	}
	ret = ad9739a_write(AD9739A_REG_CROSS_CNT2,
			AD9739A_CROSS_CNT2_CLKN_OFFSET(st->pdata->common_mode_voltage_dacclk_n));
	if(ret < 0)
	{
		return ret;
	}

	/* MU CONTROLLER Setup*/
	/* Phase detector enable and boost bias bits. */
	ret = ad9739a_write(AD9739A_REG_PHS_DET,
			AD9739A_PHS_DET_CMP_BST | AD9739A_PHS_DET_PHS_DET_AUTO_EN);
	if(ret < 0)
	{
		return ret;
	}
	ret = ad9739a_write(AD9739A_REG_MU_DUTY, AD9739A_MU_DUTY_MU_DUTY_AUTO_EN);
	if(ret < 0)
	{
		return ret;
	}
	/* Set the MU Controller search direction to UP and the target phase to 4.*/
	ret = ad9739a_write(AD9739A_REG_MU_CNT2,
					  AD9739A_MU_CNT2_SRCH_MODE(1) | AD9739A_MU_CNT2_SET_PHS(4));
	if(ret < 0)
	{
		return ret;
	}
	/* Set the MU delay value at witch the controller begins its search to 216. */
	ret = ad9739a_write(AD9739A_REG_MU_CNT3, AD9739A_MU_CNT3_MUDEL(0x6C));
	if(ret < 0)
	{
		return ret;
	}
	do
	{
		/* Set: find the exact targeted phase, retry the search until correct value is found,
				continue if desired phase is not found, guard band set to optimal value: 0xB. */
		ret = ad9739a_write(AD9739A_REG_MU_CNT4, AD9739A_MU_CNT4_SEARCH_TOL |
						   AD9739A_MU_CNT4_RETRY | AD9739A_MU_CNT4_GUARD(0xB));
		if(ret < 0)
		{
			return ret;
		}
		/* Set the MU controller tracking gain to the recommended value of 0x1. */
		ret = ad9739a_write(AD9739A_REG_MU_CNT1, AD9739A_MU_CNT1_GAIN(0x1));
		if(ret < 0)
		{
			return ret;
		}
		/* Enable the MU controller. */
		ret = ad9739a_write(AD9739A_REG_MU_CNT1,
				AD9739A_MU_CNT1_GAIN(0x1) | AD9739A_MU_CNT1_ENABLE);
		delay_fdata_cycles(180000);
		dll_loop_lock_counter++;
		if(ad9739a_read(AD9739A_REG_MU_STAT1) == AD9739A_MU_STAT1_MU_LKD)
		{
			dll_loop_locked = 1;
		}
	}
	while((dll_loop_lock_counter <= 3) && (dll_loop_locked == 0));
	if(dll_loop_locked == 0)
	{
		return -1;
	}
	/* Set FINE_DEL_SKEW to 2 for a larger DCI sampling window. */
	ret = ad9739a_write(AD9739A_REG_LVDS_REC_CNT4,
					   AD9739A_LVDS_REC_CNT4_FINE_DEL_SKEW(2) |
					   AD9739A_LVDS_REC_CNT4_DCI_DEL(0x7));
	if(ret < 0)
	{
		return ret;
	}
	dll_loop_lock_counter = 0;
	dll_loop_locked = 0;
	do
	{
		/* Disable the data Rx controller before enabling it. */
		ret = ad9739a_write(AD9739A_REG_LVDS_REC_CNT1, 0x00);
		if(ret < 0)
		{
			return ret;
		}
		/* Enable the data Rx controller for loop and IRQ. */
		ret = ad9739a_write(AD9739A_REG_LVDS_REC_CNT1,
				AD9739A_LVDS_REC_CNT1_RCVR_LOOP_ON);
		if(ret < 0)
		{
			return ret;
		}
		/* Enable the data Rx controller for search and track mode. */
		ret = ad9739a_write(AD9739A_REG_LVDS_REC_CNT1,
				AD9739A_LVDS_REC_CNT1_RCVR_LOOP_ON | AD9739A_LVDS_REC_CNT1_RCVR_CNT_ENA);
		if(ret < 0)
		{
			return ret;
		}
		delay_fdata_cycles(135000);
		dll_loop_lock_counter++;
		if(ad9739a_read(AD9739A_REG_LVDS_REC_STAT9) ==
				(AD9739A_LVDS_REC_STAT9_RCVR_LCK | AD9739A_LVDS_REC_STAT9_RCVR_TRK_ON))
		{
			dll_loop_locked = 1;
		}
	}
	while((dll_loop_lock_counter <= 3) && (dll_loop_locked == 0));
	if(dll_loop_locked == 0)
	{
		printf("AD9739A error: DLL unlocked.\n");
		return -1;
	}
	fret = ad9739a_DAC_fs_current(st->pdata->full_scale_current);
	if(fret < 0)
	{
		return (int32_t)fret;
	}

	printf("AD9739A successfully initialized.\n");

	return 0;
}
