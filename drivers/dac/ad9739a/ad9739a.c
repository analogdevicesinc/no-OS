/***************************************************************************//**
 *   @file   AD9739A.c
 *   @brief  Implementation of AD9739A Driver.
 *   @author 1 Bancisor Mihai
 *   @author 2 Grozav Andrei
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
#include <stdlib.h>
#include <stdio.h>
#include "ad9739a.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define FDATA 2500	// for 2.5 GSPS

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param dev              - The device structure.
 * @param register_address - The address of the register to write to.
 * @param register_value   - The value to write to the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9739a_write(struct ad9739a_dev *dev,
		      uint8_t register_address,
		      uint8_t register_value)
{
	uint8_t data[2] = {0, 0};
	int32_t ret;

	data[0] = AD9739A_WRITE | (0x7F & register_address);
	data[1] = register_value;

	ret = no_os_spi_write_and_read(dev->spi_desc, data, 2);

	return ret;
}

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param dev              - The device structure.
 * @param register_address - The address of the register to read.
 * @param register_value   - The value read from the register.
 *
 * @return registerValue - The register's value or negative error code.
*******************************************************************************/
int32_t ad9739a_read(struct ad9739a_dev *dev,
		     uint8_t register_address,
		     uint8_t *register_value)
{
	uint8_t tx_data[2] = {0, 0};
	int32_t ret;

	tx_data[0] = AD9739A_READ | (0x7F & register_address);

	ret = no_os_spi_write_and_read(dev->spi_desc, tx_data, 2);
	*register_value = tx_data[1];

	return ret;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @param dev - The device structure.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9739a_reset(struct ad9739a_dev *dev)
{
	int32_t ret;

	/* Software reset to default SPI values. */
	ret = ad9739a_write(dev,
			    AD9739A_REG_MODE,
			    AD9739A_MODE_RESET);
	if(ret < 0) {
		return ret;
	}
	/* Clear the reset bit. */
	ret = ad9739a_write(dev, AD9739A_REG_MODE, 0x00);

	return ret;
}

/***************************************************************************//**
 * @brief Powers down LVDS interface and TxDAC.
 *
 * @param dev       - The device structure.
 * @param pwr_config - Selects the modules to be powered-down.
 * 		      Example:
 * 			AD9739A_POWER_DOWN_LVDS_DRVR_PD |
 * 			AD9739A_POWER_DOWN_LVDS_RCVR_PD
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9739a_power_down(struct ad9739a_dev *dev,
			   uint8_t pwr_config)
{
	int32_t ret;
	uint8_t reg_data;

	/* Write to register if pwrConfig contains valid parameters. Else read
	 * and return the register value.
	 * */
	if((pwr_config & ((1 << 7) | (1 << 6) | (1 << 3) | (1 << 2))) == 0) {
		ret = ad9739a_write(dev, AD9739A_REG_POWER_DOWN, pwr_config);
	} else {
		ad9739a_read(dev, AD9739A_REG_POWER_DOWN, &reg_data);
		return reg_data;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the normal baseband mode or mix-mode.
 *
 * @param dev  - The device structure.
 * @param mode - mode of operation.
 * 		 Example:
 * 			AD9739A_DAC_DEC_NORMAL_BASEBAND - normal baseband mode;
 *			AD9739A_DAC_DEC_MIX_MODE - mix-mode.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad_serdes_clk(struct ad9739a_dev *dev,
		      uint8_t mode)
{
	int32_t ret;
	uint8_t reg_data;

	if((mode == AD9739A_DAC_DEC_NORMAL_BASEBAND) |
	    (mode == AD9739A_DAC_DEC_MIX_MODE)) {
		ret = ad9739a_write(dev, AD9739A_REG_DEC_CNT,
				    AD9739A_DEC_CNT_DAC_DEC(mode));
	} else {
		ad9739a_read(dev, AD9739A_REG_DEC_CNT, &reg_data);
		return reg_data;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the full-scale output current for the DAC.
 *
 * @param dev    - The device structure.
 * @param fs_val - The desired full-scale output current. Accepted values:
 *		   8.7 to 32.7 (mA) and 0. When fs_val is set to 0 the DAC
 *		   output is disabled(sleep).
 *
 * @return Returns the actual set full-scale current or negative error code.
*******************************************************************************/
float ad9739a_dac_fs_current(struct ad9739a_dev *dev,
			     float fs_val)
{
	float actual_fs = 0;
	uint8_t reg_data_r = 0;
	int16_t reg_data = 0;
	int32_t ret = 0;

	if((fs_val >= 8.7) && (fs_val <= 31.7)) {
		reg_data = (int16_t)((fs_val - 8.58) / 0.0226);
		ret = ad9739a_write(dev, AD9739A_REG_FSC_1, (reg_data & 0xFF));
		if(ret < 0) {
			return (float)ret;
		}
		ret = ad9739a_write(dev, AD9739A_REG_FSC_2,
				    ((reg_data & 0x300) >> 8));
		if(ret < 0) {
			return (float)ret;
		}
	} else if(fs_val == 0) {
		ret = ad9739a_write(dev, AD9739A_REG_FSC_2,
				    AD9739A_FSC_2_Sleep);
		if(ret < 0) {
			return (float)ret;
		}
	} else {
		ad9739a_read(dev, AD9739A_REG_FSC_1, &reg_data_r);
		if((int8_t)reg_data_r < 0) {
			return -1;
		}
		ad9739a_read(dev, AD9739A_REG_FSC_2, &reg_data_r);
		if((int8_t)reg_data_r < 0) {
			return (float)reg_data_r;
		}
		reg_data |= (reg_data_r & 0x3) << 8;
		actual_fs = (float)(reg_data * 0.0226  + 8.58);
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
	uint32_t us;
	/* 10 ns */
	uint8_t  cpu_clk_period = 10;
	/* 100 cpu clocks in a microsecond */
	uint32_t cpu_clks_in_a_us = 1000 / cpu_clk_period;
	uint32_t delay;

	/* convert DAC cycles to microseconds delay */
	us = (cycles/FDATA);
	/* add 9-10 microseconds to the delay as a precaution */
	delay = ((us + 9) * cpu_clks_in_a_us);
	for (count = 0; count < delay; count++) {
		asm("nop");
	}

	return 0;
}

/***************************************************************************//**
 * @brief Initializes the AD9739A.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9739a_setup(struct ad9739a_dev **device,
		      struct ad9739a_init_param init_param)
{
	int32_t ret = 0;
	float fret = 0;
	uint8_t chip_id = 0;
	uint8_t dll_loop_lock_counter = 0;
	uint8_t dll_loop_locked = 0;
	uint8_t ad9739a_reg_mu_stat1_buf = 0;
	uint8_t ad9739a_reg_lvds_rec_stat9_buf;
	struct ad9739a_dev *dev;

	dev = (struct ad9739a_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* Device ID */
	ad9739a_read(dev, AD9739A_REG_PART_ID, &chip_id);
	if(chip_id != AD9739A_CHIP_ID) {
		printf("Error: Invalid CHIP ID (0x%x).\n", chip_id);
		return -1;
	}

	/* Set 4-wire SPI, MSB first. */
	ret |= ad9739a_write(dev, AD9739A_REG_MODE, 0x00);
	if(ret < 0) {
		return ret;
	}
	ret = ad9739a_reset(dev);
	if(ret < 0) {
		return ret;
	}
	/* Set the common-mode voltage of DACCLK_P and DACCLK_N inputs. */
	ret = ad9739a_write(dev,
			    AD9739A_REG_CROSS_CNT1,
			    AD9739A_CROSS_CNT1_CLKP_OFFSET(init_param.
					    common_mode_voltage_dacclk_p));
	if(ret < 0) {
		return ret;
	}
	ret = ad9739a_write(dev,
			    AD9739A_REG_CROSS_CNT2,
			    AD9739A_CROSS_CNT2_CLKN_OFFSET(init_param.
					    common_mode_voltage_dacclk_n));
	if(ret < 0) {
		return ret;
	}

	/* MU CONTROLLER Setup*/
	/* Phase detector enable and boost bias bits. */
	ret = ad9739a_write(dev,
			    AD9739A_REG_PHS_DET,
			    AD9739A_PHS_DET_CMP_BST |
			    AD9739A_PHS_DET_PHS_DET_AUTO_EN);
	if(ret < 0) {
		return ret;
	}
	ret = ad9739a_write(dev,
			    AD9739A_REG_MU_DUTY,
			    AD9739A_MU_DUTY_MU_DUTY_AUTO_EN);
	if(ret < 0) {
		return ret;
	}
	/* Set the MU Controller search direction to UP and the target phase
	 * to 4.
	 * */
	ret = ad9739a_write(dev,
			    AD9739A_REG_MU_CNT2,
			    AD9739A_MU_CNT2_SRCH_MODE(1) |
			    AD9739A_MU_CNT2_SET_PHS(4));
	if(ret < 0) {
		return ret;
	}
	/* Set the MU delay value at witch the controller begins its search
	 * to 216.
	 * */
	ret = ad9739a_write(dev,
			    AD9739A_REG_MU_CNT3,
			    AD9739A_MU_CNT3_MUDEL(0x6C));
	if(ret < 0) {
		return ret;
	}
	do {
		/* Set: find the exact targeted phase, retry the search until
		 * correct value is found, continue if desired phase is not
		 * found, guard band set to optimal value: 0xB.
		 * */
		ret = ad9739a_write(dev,
				    AD9739A_REG_MU_CNT4,
				    AD9739A_MU_CNT4_SEARCH_TOL |
				    AD9739A_MU_CNT4_RETRY |
				    AD9739A_MU_CNT4_GUARD(0xB));
		if(ret < 0) {
			return ret;
		}
		/* Set the MU controller tracking gain to the recommended value
		 * of 0x1.
		 * */
		ret = ad9739a_write(dev,
				    AD9739A_REG_MU_CNT1,
				    AD9739A_MU_CNT1_GAIN(0x1));
		if(ret < 0) {
			return ret;
		}
		/* Enable the MU controller. */
		ret = ad9739a_write(dev,
				    AD9739A_REG_MU_CNT1,
				    AD9739A_MU_CNT1_GAIN(0x1) |
				    AD9739A_MU_CNT1_ENABLE);
		delay_fdata_cycles(180000);
		dll_loop_lock_counter++;
		ad9739a_read(dev, AD9739A_REG_MU_STAT1,
			     &ad9739a_reg_mu_stat1_buf);
		if(ad9739a_reg_mu_stat1_buf == AD9739A_MU_STAT1_MU_LKD) {
			dll_loop_locked = 1;
		}
	} while((dll_loop_lock_counter <= 3) && (dll_loop_locked == 0));
	if(dll_loop_locked == 0) {
		return -1;
	}
	/* Set FINE_DEL_SKEW to 2 for a larger DCI sampling window. */
	ret = ad9739a_write(dev,
			    AD9739A_REG_LVDS_REC_CNT4,
			    AD9739A_LVDS_REC_CNT4_FINE_DEL_SKEW(2) |
			    AD9739A_LVDS_REC_CNT4_DCI_DEL(0x7));
	if(ret < 0) {
		return ret;
	}
	dll_loop_lock_counter = 0;
	dll_loop_locked = 0;
	do {
		/* Disable the data Rx controller before enabling it. */
		ret = ad9739a_write(dev, AD9739A_REG_LVDS_REC_CNT1, 0x00);
		if(ret < 0) {
			return ret;
		}
		/* Enable the data Rx controller for loop and IRQ. */
		ret = ad9739a_write(dev, AD9739A_REG_LVDS_REC_CNT1,
				    AD9739A_LVDS_REC_CNT1_RCVR_LOOP_ON);
		if(ret < 0) {
			return ret;
		}
		/* Enable the data Rx controller for search and track mode. */
		ret = ad9739a_write(dev, AD9739A_REG_LVDS_REC_CNT1,
				    AD9739A_LVDS_REC_CNT1_RCVR_LOOP_ON |
				    AD9739A_LVDS_REC_CNT1_RCVR_CNT_ENA);
		if(ret < 0) {
			return ret;
		}
		delay_fdata_cycles(135000);
		dll_loop_lock_counter++;
		ad9739a_read(dev, AD9739A_REG_LVDS_REC_STAT9,
			     &ad9739a_reg_lvds_rec_stat9_buf);
		if(ad9739a_reg_lvds_rec_stat9_buf ==
		    (AD9739A_LVDS_REC_STAT9_RCVR_LCK |
		     AD9739A_LVDS_REC_STAT9_RCVR_TRK_ON)) {
			dll_loop_locked = 1;
		}
	} while((dll_loop_lock_counter <= 3) && (dll_loop_locked == 0));
	if(dll_loop_locked == 0) {
		printf("AD9739A error: DLL unlocked.\n");
		return -1;
	}
	fret = ad9739a_dac_fs_current(dev, init_param.full_scale_current);
	if(fret < 0) {
		return (int32_t)fret;
	}

	printf("AD9739A successfully initialized.\n");

	*device = dev;

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad9739a_setup().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9739a_remove(struct ad9739a_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}
