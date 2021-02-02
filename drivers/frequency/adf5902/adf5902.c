/***************************************************************************//**
 *   @file   adf5902.c
 *   @brief  Implementation of adf5902 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "adf5902.h"
#include "error.h"
#include "delay.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Writes 4 bytes of data to ADF5902.
 * @param device - The device structure.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf5902_write(struct adf5902_dev *device, uint32_t data)
{
	uint8_t buff[4];
	uint8_t buff_size = 4;

	buff[0] = data >> 24;
	buff[1] = data >> 16;
	buff[2] = data >> 8;
	buff[3] = data;

	return spi_write_and_read(device->spi_desc, buff, buff_size);
}

/**
 * @brief Readback data from ADF5902.
 * @param device - The device structure.
 * @param data - Data to be read.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf5902_readback(struct adf5902_dev *device, uint32_t *data)
{
	int32_t ret;
	uint32_t i;
	uint8_t buff[4];
	uint8_t buff_size = 4;

	buff[0] = *data >> 24;
	buff[1] = *data >> 16;
	buff[2] = *data >> 8;
	buff[3] = *data;

	ret = spi_write_and_read(device->spi_desc, buff, buff_size);
	if (ret != SUCCESS)
		return ret;

	for (i = 0; i < buff_size; i++)
		*data = (*data << 8) | buff[i];

	return ret;
}

/**
 * @brief Sets the ADF4350 RF VCO frequency.
 * @param device - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902_set_vco_freq(struct adf5902_dev *device)
{

	device->f_pfd = (device->ref_in * (1 + device->ref_doubler_en) /
			 (device->ref_div_factor *(1 + device->ref_div2_en)));

	if (device->f_pfd > ADF5902_MAX_FREQ_PFD)
		return FAILURE;

	device->int_div = (uint16_t)(device->rf_out / (device->f_pfd * 2));

	if ((device->int_div > ADF5902_MAX_INT_MSB_WORD)
	    || (device->int_div < ADF5902_MIN_INT_MSB_WORD))
		return FAILURE;

	device->frac_msb = (uint16_t)(((device->rf_out / (device->f_pfd * 2)) -
				       device->int_div) * pow(2, 12));

	if ((device->frac_msb > ADF5902_MAX_FRAC_MSB_WORD)
	    || (device->frac_msb < ADF5902_MIN_FRAC_MSB_WORD))
		return FAILURE;

	device->frac_lsb = (uint16_t)(((((device->rf_out / (device->f_pfd * 2)) -
					 device->int_div) * pow(2, 12)) -
				       device->frac_msb) * pow(2, 13));

	if ((device->frac_lsb > ADF5902_MAX_FRAC_LSB_WORD)
	    || (device->frac_lsb < ADF5902_MIN_FRAC_LSB_WORD))
		return FAILURE;

	return SUCCESS;
}

/**
 * @brief Initializes the ADF5902.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902_init(struct adf5902_dev **device,
		     struct adf5902_init_param *init_param)
{
	int32_t ret;
	struct adf5902_dev *dev;

	dev = (struct adf5902_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return FAILURE;

	/* SPI */
	ret = spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret != SUCCESS)
		return ret;

	if ((init_param->rf_out > ADF5902_MAX_VCO_FREQ)
	    || (init_param->rf_out < ADF5902_MIN_VCO_FREQ))
		return FAILURE;

	dev->rf_out = init_param->rf_out;

	if ((init_param->ref_in > ADF5902_MAX_REFIN_FREQ)
	    || (init_param->ref_in < ADF5902_MIN_REFIN_FREQ))
		return FAILURE;

	dev->ref_in = init_param->ref_in;

	if ((init_param->ref_div_factor > ADF5902_MAX_R_DIVIDER)
	    || (init_param->ref_div_factor < ADF5902_MIN_R_DIVIDER))
		return FAILURE;

	dev->ref_div_factor = init_param->ref_div_factor;

	if ((init_param->ref_doubler_en != ADF5902_REF_DOUBLER_DISABLE)
	    && (init_param->ref_doubler_en != ADF5902_REF_DOUBLER_ENABLE))
		return FAILURE;

	dev->ref_doubler_en = init_param->ref_doubler_en;

	if ((init_param->ref_div2_en != ADF5902_R_DIV_2_DISABLE)
	    && (init_param->ref_div2_en != ADF5902_R_DIV_2_ENABLE))
		return FAILURE;

	dev->ref_div2_en = init_param->ref_div2_en;

	if ((init_param->adc_avg > ADF5902_ADC_AVG_4) ||
	    (init_param->adc_avg < ADF5902_ADC_AVG_1))
		return FAILURE;

	dev->adc_avg = init_param->adc_avg;

	if ((init_param->tx_amp_cal_ref > ADF5902_TX_AMP_CAL_MAX_REF_CODE) ||
	    (init_param->tx_amp_cal_ref < ADF5902_TX_AMP_CAL_MIN_REF_CODE))
		return FAILURE;

	dev->tx_amp_cal_ref = init_param->tx_amp_cal_ref;

	/* Set device specific parameters for obtaining the VCO frequency */
	ret = adf5902_set_vco_freq(dev);
	if (ret != SUCCESS)
		return ret;

	/* Set frequency calibration divider value */
	dev->freq_cal_div = (uint16_t)(dev->f_pfd / 100000);
	/* f_PFD / 100kHz <= freq_cal_div */
	if(dev->f_pfd % 100000)
		dev->freq_cal_div += 1;

	if(dev->freq_cal_div > ADF5902_MAX_FREQ_CAL_DIV ||
	    dev->freq_cal_div < ADF5902_MIN_FREQ_CAL_DIV)
		return FAILURE;

	/* Set clock divider value */
	dev->clk1_div = dev->f_pfd / 25000;

	/* f_PFD/25kHz <= clk1_div */
	if (dev->f_pfd % 25000)
		dev->clk1_div += 1;

	if (dev->clk1_div > ADF5902_MAX_CLK_DIVIDER ||
	    dev->clk1_div < ADF5902_MIN_CLK_DIVIDER)
		return FAILURE;

	/* Set ADC clock divider value for 1MHz ADC clock */
	dev->adc_clk_div = dev->f_pfd / 1000000;

	if (dev->adc_clk_div > ADF5902_ADC_MAX_CLK_DIVIDER ||
	    dev->adc_clk_div < ADF5902_ADC_MIN_CLK_DIVIDER)
		return FAILURE;

	/* Master Reset */
	ret = adf5902_write(dev, ADF5902_REG7 | ADF5902_REG7_RESERVED |
			    ADF5902_REG7_MASTER_RESET(ADF5902_MASTER_RESET_ENABLE));
	if (ret != SUCCESS)
		return ret;

	/* Reset Counters */
	ret = adf5902_write(dev, ADF5902_REG11 | ADF5902_REG11_RESERVED |
			    ADF5902_REG11_CNTR_RESET(ADF5902_CNTR_RESET_ENABLE));
	if (ret != SUCCESS)
		return ret;

	/* Enable Counters */
	ret = adf5902_write(dev, ADF5902_REG11 | ADF5902_REG11_RESERVED |
			    ADF5902_REG11_CNTR_RESET(ADF5902_CNTR_RESET_DISABLE));
	if (ret != SUCCESS)
		return ret;

	/* Set clock divider mode to Ramp Divider */
	ret = adf5902_write(dev, ADF5902_REG13 | ADF5902_REG13_RESERVED |
			    ADF5902_REG13_CLK_DIV_MODE(ADF5902_RAMP_DIV));
	if (ret != SUCCESS)
		return ret;

	/* Register 10 Reserved */
	ret = adf5902_write(dev, ADF5902_REG10 | ADF5902_REG10_RESERVED);
	if (ret != SUCCESS)
		return ret;

	/* VCO Calibration Setup */
	ret = adf5902_write(dev, ADF5902_REG9 | ADF5902_REG9_RESERVED_CALIB);
	if (ret != SUCCESS)
		return ret;

	/* Set VCO frequency calibration divider clock to 100kHz */
	ret = adf5902_write(dev, ADF5902_REG8 | ADF5902_REG8_RESERVED |
			    ADF5902_REG8_FREQ_CAL_DIV(dev->freq_cal_div));
	if (ret != SUCCESS)
		return ret;

	/* Power up the device and LO */
	ret = adf5902_write(dev, ADF5902_REG0 | ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != SUCCESS)
		return ret;

	udelay(10);

	/* Set Register 7 */
	ret = adf5902_write(dev, ADF5902_REG7 | ADF5902_REG7_RESERVED |
			    ADF5902_REG7_R_DIVIDER(dev->ref_div_factor) |
			    ADF5902_REG7_REF_DOUBLER(dev->ref_doubler_en) |
			    ADF5902_REG7_R_DIV_2(dev->ref_div2_en) |
			    ADF5902_REG7_CLK_DIV(dev->clk1_div) |
			    ADF5902_REG7_MASTER_RESET(ADF5902_MASTER_RESET_DISABLE));
	if (ret != SUCCESS)
		return ret;

	/* Set LSB FRAC */
	ret = adf5902_write(dev, ADF5902_REG6 | ADF5902_REG6_RESERVED |
			    ADF5902_REG6_FRAC_LSB_WORD(dev->frac_lsb));
	if (ret != SUCCESS)
		return ret;

	/* Set MSB FRAC and INT */
	ret = adf5902_write(dev, ADF5902_REG5 | ADF5902_REG5_RESERVED |
			    ADF5902_REG5_FRAC_MSB_WORD(dev->frac_msb) |
			    ADF5902_REG5_INTEGER_WORD(dev->int_div));
	if (ret != SUCCESS)
		return ret;

	/* Set ATEST to high Impedance */
	ret = adf5902_write(dev, ADF5902_REG4 | ADF5902_REG4_RESERVED |
			    ADF5902_REG4_TEST_BUS(ADF5902_TEST_BUS_NONE));
	if (ret != SUCCESS)
		return ret;

	/* Set IO level to 3.3V, CAL_BUSY to MUXOUT */
	ret = adf5902_write(dev, ADF5902_REG3 | ADF5902_REG3_RESERVED |
			    ADF5902_REG3_READBACK_CTRL(ADF5902_REG_RB_NONE) |
			    ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
			    ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_CAL_BUSY));
	if (ret != SUCCESS)
		return ret;

	/* Set ADC Clock to 1MHz */
	ret = adf5902_write(dev, ADF5902_REG2 | ADF5902_REG2_RESERVED |
			    ADF5902_REG2_ADC_CLK_DIV(dev->adc_clk_div) |
			    ADF5902_REG2_ADC_AVG(dev->adc_avg) |
			    ADF5902_REG2_ADC_START(ADF5902_ADC_NORMAL_OP));
	if (ret != SUCCESS)
		return ret;

	/* Set Transmitter amplitude level */
	ret = adf5902_write(dev, ADF5902_REG1 | ADF5902_REG1_RESERVED |
			    ADF5902_REG1_TX_AMP_CAL_REF(dev->tx_amp_cal_ref));
	if (ret != SUCCESS)
		return ret;

	/* Start VCO frequency calibration */
	ret = adf5902_write(dev, ADF5902_REG0 | ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_FULL_CAL) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != SUCCESS)
		return ret;

	udelay(1200);

	/* Tx1 on, Tx2 off, LO on */
	ret = adf5902_write(dev, ADF5902_REG0 | ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_UP_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != SUCCESS)
		return ret;

	/* Tx1 amplitude calibration */
	ret = adf5902_write(dev, ADF5902_REG0 | ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_UP_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_AMP_CAL) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != SUCCESS)
		return ret;

	udelay(500);

	/* Tx1 of, Tx2 on, LO on */
	ret = adf5902_write(dev, ADF5902_REG0 | ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_UP_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != SUCCESS)
		return ret;

	/* Tx2 amplitude calibration */
	ret = adf5902_write(dev, ADF5902_REG0 | ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_AMP_CAL));
	if (ret != SUCCESS)
		return ret;

	udelay(500);

	*device = dev;

	return ret;
}

/**
 * @brief Free resoulces allocated for ADF5902
 * @param device - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902_remove(struct adf5902_dev *device)
{
	int32_t ret;

	ret = spi_remove(device->spi_desc);

	free(device);

	return ret;
}