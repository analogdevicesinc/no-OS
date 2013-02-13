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
#include "dac_core.h"
#include "cf_axi_dds.h"
#include "AD9122.h"

extern void delay_us(uint32_t us_count);
extern struct cf_axi_dds_state dds_state;
extern struct cf_axi_dds_converter dds_conv;

int64_t (*pfnSetDataClk)(int64_t Hz);
int64_t (*pfnSetDacClk)(int64_t Hz);

/******************************************************************************/
/***************************** Local Types and Variables***********************/
/******************************************************************************/
static const uint32_t ad9122_reg_defaults[][2] =
{
	{AD9122_REG_COMM, 0x00},
	{AD9122_REG_COMM, AD9122_COMM_RESET},
	{AD9122_REG_COMM, 0x00},
	{AD9122_REG_POWER_CTRL, AD9122_POWER_CTRL_PD_AUX_ADC},
	{AD9122_REG_DATA_FORMAT, AD9122_DATA_FORMAT_BINARY},
	{AD9122_REG_INTERRUPT_EN_1, 0x00},
	{AD9122_REG_INTERRUPT_EN_2, 0x00},
	{AD9122_REG_CLK_REC_CTRL, AD9122_CLK_REC_CTRL_DACCLK_CROSS_CORRECTION |
				  AD9122_CLK_REC_CTRL_REFCLK_CROSS_CORRECTION |
				  0xF},
	{AD9122_REG_PLL_CTRL_1, AD9122_PLL_CTRL_1_PLL_MANUAL_EN},
	{AD9122_REG_PLL_CTRL_2, AD9122_PLL_CTRL_2_PLL_LOOP_BANDWIDTH(3) |
				AD9122_PLL_CTRL_2_PLL_CHARGE_PUMP_CURRENT(0x11)},
	{AD9122_REG_PLL_CTRL_3, AD9122_PLL_CTRL_3_N2(3) |
				AD9122_PLL_CTRL_3_PLL_CROSS_CTRL_EN |
				AD9122_PLL_CTRL_3_N0(1) |
				AD9122_PLL_CTRL_3_N1(2)},
	{AD9122_REG_SYNC_CTRL_1, AD9122_SYNC_CTRL_1_DATA_FIFO_RATE_TOGGLE |
				 AD9122_SYNC_CTRL_1_RISING_EDGE_SYNC},
	{AD9122_REG_SYNC_CTRL_2, 0x00},
	{AD9122_REG_DCI_DELAY, 0x00},
	{AD9122_REG_FIFO_CTRL, AD9122_FIFO_CTRL_FIFO_PHA_OFFSET(4)},
	{AD9122_REG_FIFO_STATUS_1, 0x00},
	{AD9122_REG_DATAPATH_CTRL, AD9122_DATAPATH_CTRL_BYPASS_PREMOD |
				   AD9122_DATAPATH_CTRL_BYPASS_INV_SINC |
				   AD9122_DATAPATH_CTRL_BYPASS_NCO},
	{AD9122_REG_HB1_CTRL, AD9122_HB1_CTRL_BYPASS_HB1},
	{AD9122_REG_HB2_CTRL, AD9122_HB2_CTRL_BYPASS_HB2},
	{AD9122_REG_HB3_CTRL, AD9122_HB3_CTRL_BYPASS_HB3},
	{AD9122_REG_FTW_7_0, 0x00},
	{AD9122_REG_FTW_15_8, 0x00},
	{AD9122_REG_FTW_23_16, 0x00},
	{AD9122_REG_FTW_31_24, 0x00},
	{AD9122_REG_NCO_PHA_OFFSET_LSB, 0x00},
	{AD9122_REG_NCO_PHA_OFFSET_MSB, 0x00},
	{AD9122_REG_NCO_FTW_UPDATE, 0x00},
	{AD9122_REG_I_PHA_ADJ_LSB, 0x00},
	{AD9122_REG_I_PHA_ADJ_MSB, 0x00},
	{AD9122_REG_Q_PHA_ADJ_LSB, 0x00},
	{AD9122_REG_Q_PHA_ADJ_MSB, 0x00},
	{AD9122_REG_I_DAC_OFFSET_LSB, 0x00},
	{AD9122_REG_I_DAC_OFFSET_MSB, 0x00},
	{AD9122_REG_Q_DAC_OFFSET_LSB, 0x00},
	{AD9122_REG_Q_DAC_OFFSET_MSB, 0x00},
	{AD9122_REG_I_DAC_FS_ADJ, 0xF9},
	{AD9122_REG_I_DAC_CTRL, 0x01},
	{AD9122_REG_I_AUX_DAC_DATA, 0x00},
	{AD9122_REG_I_AUX_DAC_CTRL, 0x00},
	{AD9122_REG_Q_DAC_FS_ADJ, 0xF9},
	{AD9122_REG_Q_DAC_CTRL, 0x01},
	{AD9122_REG_Q_AUX_DAC_DATA, 0x00},
	{AD9122_REG_Q_AUX_DAC_CTRL, 0x00},
	{AD9122_REG_DIE_TEMP_RANGE_CTRL, AD9122_DIE_TEMP_RANGE_CTRL_REF_CURRENT(1)},
	{AD9122_REG_FIFO_STATUS_1, AD9122_FIFO_STATUS_1_FIFO_SOFT_ALIGN_REQ},
};

#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])

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
	uint8_t regAddr			= 0;
	uint32_t registerValue 	= 0;
	int32_t ret;

	regAddr = 0x80 + (0x7F & registerAddress);
    ret = SPI_Read(SPI_SEL_AD9122, regAddr, &registerValue);

	return (ret < 0 ? ret : (int32_t)registerValue);
}

/***************************************************************************//**
 * @brief Waits for the AD9122 to sync.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_sync()
{
	int32_t ret, timeout;

	timeout = 255;
	do
	{
		delay_us(1000);
		ret = ad9122_read(AD9122_REG_FIFO_STATUS_1);
		if (ret < 0)
			return ret;

	} while (timeout-- && !(ret & AD9122_FIFO_STATUS_1_FIFO_SOFT_ALIGN_ACK));

	ad9122_write(AD9122_REG_FIFO_STATUS_1, 0x0);
	ad9122_write(AD9122_REG_SYNC_CTRL_1,
		     	 AD9122_SYNC_CTRL_1_SYNC_EN |
		     	 AD9122_SYNC_CTRL_1_RISING_EDGE_SYNC);

	timeout = 255;
	do
	{
		delay_us(1000);
		ret = ad9122_read(AD9122_REG_SYNC_STATUS_1);
		if (ret < 0)
			return ret;

	} while (timeout-- && !(ret & AD9122_SYNC_STATUS_1_SYNC_LOCKED));

	return 0;
}

/***************************************************************************//**
 * @brief Returns the value of the data clock.
 *
 * @param conv - Pointer to a cf_axi_dds_converter struct which contains the
 * 				 data clock value
 *
 * @return eturns the value of the data clock.
*******************************************************************************/
static uint32_t ad9122_get_data_clk(struct cf_axi_dds_converter *conv)
{
	return conv->clk[CLK_DATA];
}

/***************************************************************************//**
 * @brief Sets the value of the data clock.
 *
 * @param conv - Pointer to a cf_axi_dds_converter struct
 * @param freq - Data clock value
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
static int32_t ad9122_set_data_clk(struct cf_axi_dds_converter *conv, uint32_t freq)
{
	int64_t ret;
	uint32_t dac_freq;

	dac_freq = freq * conv->interp_factor;
	if (dac_freq > AD9122_MAX_DAC_RATE)
	{
		return -1;
	}

	ret = pfnSetDataClk(freq);
	if(ret < 0)
		return -1;
	conv->clk[CLK_DATA] = (uint32_t)ret;

	ret = pfnSetDacClk(dac_freq);
	if(ret < 0)
		return -1;
	conv->clk[CLK_DAC]  = (uint32_t)ret;

	return 0;
}

/***************************************************************************//**
 * @brief Validates the interpolation factor
 *
 * @return Returns a valid interpolation factor.
*******************************************************************************/
static uint32_t ad9122_validate_interp_factor(unsigned fact)
{
	switch (fact)
	{
		case 1:
		case 2:
		case 4:
		case 8:
			return fact;
		default:
			return 1;
	}
}

/***************************************************************************//**
 * @brief Sets the interpolation factor.
 *
 * @param conv - Pointer to a cf_axi_dds_converter struct.
 * @param interp - Interpolation factor
 * @param fcent_shift - Center frequency shift in Hz
 * @param data_rate - Data rate in Hz
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
static int32_t ad9122_set_interpol(struct cf_axi_dds_converter *conv,
								   uint32_t interp,
								   uint32_t fcent_shift,
								   uint32_t data_rate)
{
	uint32_t hb1, hb2, hb3, tmp;
	int32_t ret, cached;

	hb1 = AD9122_HB1_CTRL_BYPASS_HB1;
	hb2 = AD9122_HB2_CTRL_BYPASS_HB2;
	hb3 = AD9122_HB3_CTRL_BYPASS_HB3;

	switch (interp) {
		case 1:
			break;
		case 2:
			if (fcent_shift > 3)
				return -1;
			hb1 = AD9122_HB1_INTERP(fcent_shift);
			break;
		case 4:
			if (fcent_shift > 7)
				return -1;
			hb1 = AD9122_HB1_INTERP(fcent_shift % 4);
			hb2 = AD9122_HB23_INTERP(fcent_shift);
			break;
		case 8:
			if (fcent_shift > 15)
				return -1;
			hb1 = AD9122_HB1_INTERP(fcent_shift % 4);
			hb2 = AD9122_HB23_INTERP(fcent_shift % 8);
			hb3 = AD9122_HB23_INTERP(fcent_shift / 2);
			break;
		default:
			return -1;
	}

	cached = conv->interp_factor;
	conv->interp_factor = interp;
	ret = ad9122_set_data_clk(conv, data_rate ?
				 data_rate : ad9122_get_data_clk(conv));

	if (ret < 0) {
		conv->interp_factor = cached;

		return ret;
	}

	tmp = ad9122_read(AD9122_REG_DATAPATH_CTRL);
	switch (hb1) {
		case AD9122_HB1_INTERP(1):
		case AD9122_HB1_INTERP(3):
			tmp &= ~AD9122_DATAPATH_CTRL_BYPASS_PREMOD;
			break;
		default:
			tmp |= AD9122_DATAPATH_CTRL_BYPASS_PREMOD;
	}

	ad9122_write(AD9122_REG_DATAPATH_CTRL, tmp);
	ad9122_write(AD9122_REG_HB1_CTRL, hb1);
	ad9122_write(AD9122_REG_HB2_CTRL, hb2);
	ad9122_write(AD9122_REG_HB3_CTRL, hb3);
	conv->fcenter_shift = fcent_shift;

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
 * @brief Initializes the AD9122.
 *
 * @param pfnSetDataClock - Pointer to a function which sets the DAC data clock
 * @param pfnSetDacClock - Pointer to a function which sets the DAC clock
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_setup(void* pfnSetDataClock, void* pfnSetDacClock)
{
	int32_t ret;
	int32_t i;
	uint32_t datapath_ctrl, rate;
	struct cf_axi_dds_converter *conv = &dds_conv;

	if(ad9122_reset() < 0)
		return -1;

	pfnSetDataClk = pfnSetDataClock;
	pfnSetDacClk  = pfnSetDacClock;

	conv->write 		= ad9122_write;
	conv->read 			= ad9122_read;
	conv->setup 		= ad9122_setup;
	conv->get_data_clk 	= ad9122_get_data_clk;
	conv->set_data_clk 	= ad9122_set_data_clk;
	conv->set_interpol 	= ad9122_set_interpol;

	for (i = 0; i < ARRAY_SIZE(ad9122_reg_defaults); i++)
	{
		ad9122_write(ad9122_reg_defaults[i][0],
					 ad9122_reg_defaults[i][1]);
	}

	if(ad9122_sync() < 0)
		return -1;

	conv->interp_factor = 1;
	conv->interp_factor = ad9122_validate_interp_factor(conv->interp_factor);

	conv->fcenter_shift = 0;

	datapath_ctrl = AD9122_DATAPATH_CTRL_BYPASS_PREMOD |
					AD9122_DATAPATH_CTRL_BYPASS_NCO |
					AD9122_DATAPATH_CTRL_BYPASS_INV_SINC;
	ad9122_write(AD9122_REG_DATAPATH_CTRL, datapath_ctrl);

	rate = 491520000;
	ret = ad9122_set_interpol(conv, conv->interp_factor,
			  	  	  	  	  conv->fcenter_shift, rate);

	cf_axi_dds_of_probe();

	return ret;
}

/***************************************************************************//**
 * @brief Calibrates the AD9122 DCI.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_dci_calibrate()
{
	return cf_axi_dds_tune_dci(0);
}

/***************************************************************************//**
 * @brief Sets the AD9122 data rate.
 *
 * @param rate - desired rate in Hz
 *
 * @return Returns the set data rate.
*******************************************************************************/
int32_t ad9122_set_data_rate(uint32_t rate)
{
	int32_t ret = 0;

	ret = cf_axi_dds_write_raw(0, 0,
							  (int32_t)rate, 0,
							  IIO_CHAN_INFO_SAMP_FREQ);
	if(ret < 0)
		return ret;

	return dds_state.dac_clk;
}

/***************************************************************************//**
 * @brief Sets the phase adjustment of the I DAC.
 *
 * @param val - Phase adjustment value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage0_phase(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_dds_store(AD9122_REG_I_PHA_ADJ_MSB, val);
	}

	ret = ad9122_dds_show(AD9122_REG_I_PHA_ADJ_MSB, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the phase adjustment of the Q DAC.
 *
 * @param val - Phase adjustment value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage1_phase(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_dds_store(AD9122_REG_Q_PHA_ADJ_MSB, val);
	}

	ret = ad9122_dds_show(AD9122_REG_Q_PHA_ADJ_MSB, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the calibration bias of the I DAC.
 *
 * @param val - Calibration bias value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage0_calibbias(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_dds_store(AD9122_REG_I_DAC_OFFSET_MSB, val);
	}

	ret = ad9122_dds_show(AD9122_REG_I_DAC_OFFSET_MSB, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the calibration bias of the Q DAC.
 *
 * @param val - Calibration bias value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage1_calibbias(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_dds_store(AD9122_REG_Q_DAC_OFFSET_MSB, val);
	}

	ret = ad9122_dds_show(AD9122_REG_Q_DAC_OFFSET_MSB, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the calibration scale of the I DAC.
 *
 * @param val - Calibration scale value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage0_calibscale(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_dds_store(AD9122_REG_I_DAC_CTRL, val);
	}

	ret = ad9122_dds_show(AD9122_REG_I_DAC_CTRL, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the calibration scale of the Q DAC.
 *
 * @param val - Calibration scale value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage1_calibscale(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_dds_store(AD9122_REG_Q_DAC_CTRL, val);
	}

	ret = ad9122_dds_show(AD9122_REG_Q_DAC_CTRL, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the interpolation factor.
 *
 * @param val - Interpolation value. If the value equals INT32_MAX then
 *             the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_altvoltage_interpolation(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_dds_interpolation_store(0, val);
	}

	ret = ad9122_dds_interpolation_show(0, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the center frequency shift value.
 *
 * @param val - Center shift value. If the value equals INT32_MAX then
 *             the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_altvoltage_interpolation_center_shift(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_dds_interpolation_store(1, val);
	}

	ret = ad9122_dds_interpolation_show(1, &val);

	return ret > 0 ? val : ret;
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

