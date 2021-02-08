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
 * @param dev - The device structure.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf5902_write(struct adf5902_dev *dev, uint32_t data)
{
	uint8_t buff[4];
	uint8_t buff_size = 4;

	buff[0] = data >> 24;
	buff[1] = data >> 16;
	buff[2] = data >> 8;
	buff[3] = data;

	return spi_write_and_read(dev->spi_desc, buff, buff_size);
}

/**
 * @brief Readback data from ADF5902.
 * @param dev - The device structure.
 * @param data - Data to be read.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf5902_readback(struct adf5902_dev *dev, uint32_t *data)
{
	int32_t ret;
	uint32_t i;
	uint8_t buff[4];
	uint8_t buff_size = 4;

	buff[0] = *data >> 24;
	buff[1] = *data >> 16;
	buff[2] = *data >> 8;
	buff[3] = *data;

	/* Write command */
	ret = spi_write_and_read(dev->spi_desc, buff, buff_size);
	if (ret != SUCCESS)
		return ret;

	/* Send dummy value to get the data on DOUT */
	buff[0] = ADF5902_SPI_DUMMY_DATA;
	buff[1] = ADF5902_SPI_DUMMY_DATA;
	buff[2] = ADF5902_SPI_DUMMY_DATA;
	buff[3] = ADF5902_SPI_DUMMY_DATA;

	ret = spi_write_and_read(dev->spi_desc, buff, buff_size);
	if (ret != SUCCESS)
		return ret;

	for (i = 0; i < buff_size; i++)
		*data = (*data << 8) | buff[i];

	return ret;
}

/**
 * @brief Sets the ADF4350 RF VCO frequency.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902_set_vco_freq(struct adf5902_dev *dev)
{

	dev->f_pfd = (dev->ref_in * (1 + dev->ref_doubler_en) /
		      (dev->ref_div_factor *(1 + dev->ref_div2_en)));

	if (dev->f_pfd > ADF5902_MAX_FREQ_PFD)
		return FAILURE;

	dev->int_div = (uint16_t)(dev->rf_out / (dev->f_pfd * 2));

	if ((dev->int_div > ADF5902_MAX_INT_MSB_WORD)
	    || (dev->int_div < ADF5902_MIN_INT_MSB_WORD))
		return FAILURE;

	dev->frac_msb = (uint16_t)(((dev->rf_out / (dev->f_pfd * 2)) -
				    dev->int_div) * pow(2, 12));

	if ((dev->frac_msb > ADF5902_MAX_FRAC_MSB_WORD)
	    || (dev->frac_msb < ADF5902_MIN_FRAC_MSB_WORD))
		return FAILURE;

	dev->frac_lsb = (uint16_t)(((((dev->rf_out / (dev->f_pfd * 2)) -
				      dev->int_div) * pow(2, 12)) -
				    dev->frac_msb) * pow(2, 13));

	if ((dev->frac_lsb > ADF5902_MAX_FRAC_LSB_WORD)
	    || (dev->frac_lsb < ADF5902_MIN_FRAC_LSB_WORD))
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
	uint32_t i;
	struct adf5902_dev *dev;

	dev = (struct adf5902_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return FAILURE;

	/* Chip Enable */
	ret = gpio_get(&dev->gpio_ce, init_param->gpio_ce_param);
	if (ret != SUCCESS)
		return ret;

	ret = gpio_direction_output(dev->gpio_ce, GPIO_HIGH);
	if (ret != SUCCESS)
		return ret;

	/* SPI */
	ret = spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret != SUCCESS)
		return ret;

	/* Initialization Parameters */
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

	if ((init_param->delay_words_no > (ADF5902_DEL_SEL_3 + 1)) ||
	    (init_param->delay_words_no < ADF5902_DEL_SEL_0))
		return FAILURE;

	dev->delay_words_no = init_param->delay_words_no;

	for (i = 0; i < dev->delay_words_no; i++)
		if ((init_param->delay_wd[i] > ADF5902_MAX_DELAY_START_WRD) ||
		    (init_param->delay_wd[i] < ADF5902_MIN_DELAY_START_WRD))
			return FAILURE;

	dev->delay_wd = init_param->delay_wd;

	if ((init_param->ramp_delay_en != ADF5902_RAMP_DEL_DISABLE)
	    && (init_param->ramp_delay_en != ADF5902_RAMP_DEL_ENABLE))
		return FAILURE;

	dev->ramp_delay_en = init_param->ramp_delay_en;

	if ((init_param->tx_trig_en != ADF5902_TX_DATA_TRIG_DISABLE)
	    && (init_param->tx_trig_en != ADF5902_TX_DATA_TRIG_ENABLE))
		return FAILURE;

	dev->tx_trig_en = init_param->tx_trig_en;

	if ((init_param->step_words_no > (ADF5902_STEP_SEL_3 + 1)) ||
	    (init_param->step_words_no < ADF5902_STEP_SEL_0))
		return FAILURE;

	dev->step_words_no = init_param->step_words_no;

	for (i = 0; i < dev->step_words_no; i++)
		if((init_param->step_words[i] < ADF5902_MIN_STEP_WORD) ||
		    (init_param->step_words[i] > ADF5902_MAX_STEP_WORD))
			return FAILURE;

	dev->step_words = init_param->step_words;

	if ((init_param->freq_dev_no > (ADF5902_DEV_SEL_3 + 1)) ||
	    (init_param->freq_dev_no < ADF5902_DEV_SEL_0))
		return FAILURE;

	dev->freq_dev_no = init_param->freq_dev_no;

	for (i = 0; i < dev->freq_dev_no; i++)
		if((init_param->freq_dev[i].dev_word < ADF5902_MIN_DEV_WORD) ||
		    (init_param->freq_dev[i].dev_word > ADF5902_MAX_DEV_WORD) ||
		    (init_param->freq_dev[i].dev_offset < ADF5902_MIN_DEV_OFFSET) ||
		    (init_param->freq_dev[i].dev_offset > ADF5902_MAX_DEV_OFFSET))
			return FAILURE;

	dev->freq_dev = init_param->freq_dev;

	if ((init_param->tx_ramp_clk != ADF5902_TX_RAMP_CLK_DIV)
	    && (init_param->tx_ramp_clk != ADF5902_TX_RAMP_TX_DATA_PIN))
		return FAILURE;

	dev->tx_ramp_clk = init_param->tx_ramp_clk;

	if ((init_param->tx_data_invert != AD5902_TX_DATA_INV_DISABLE)
	    && (init_param->tx_data_invert != AD5902_TX_DATA_INV_ENABLE))
		return FAILURE;

	dev->tx_data_invert = init_param->tx_data_invert;

	if ((init_param->clk2_div_no > (ADF5902_CLK_DIV_SEL_3 + 1)) ||
	    (init_param->clk2_div_no < ADF5902_CLK_DIV_SEL_0))
		return FAILURE;

	dev->clk2_div_no = init_param->clk2_div_no;

	for (i = 0; i < dev->clk2_div_no; i++)
		if((init_param->clk2_div[i] < ADF5902_MIN_CLK_DIV_2) ||
		    (init_param->clk2_div[i] > ADF5902_MAX_CLK_DIV_2))
			return FAILURE;

	dev->clk2_div = init_param->clk2_div;

	if ((init_param->cp_current > ADF5902_CP_CURRENT_4MA48) ||
	    (init_param->cp_current < ADF5902_CP_CURRENT_280UA))
		return FAILURE;

	dev->cp_current = init_param->cp_current;

	if ((init_param->cp_tristate_en != ADF5902_CP_TRISTATE_DISABLE)
	    && (init_param->cp_tristate_en != ADF5902_CP_TRISTATE_ENABLE))
		return FAILURE;

	dev->cp_tristate_en = init_param->cp_tristate_en;

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
			    ADF5902_REG13_CLK_DIV_MODE(dev->clk_div_mode));
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
			    ADF5902_REG5_INTEGER_WORD(dev->int_div) |
			    ADF5902_REG5_RAMP_ON(ADF5902_RAMP_ON_DISABLED));
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

	/* R17 Reserved */
	ret = adf5902_write(dev, ADF5902_REG17 | ADF5902_REG17_RESERVED);
	if (ret != SUCCESS)
		return ret;

	/* Ramp delay register */
	for (i = 0; i < dev->step_words_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG16 | ADF5902_REG16_RESERVED |
				    ADF5902_REG16_DEL_START_WORD(dev->delay_wd[i]) |
				    ADF5902_REG16_RAMP_DEL(dev->ramp_delay_en) |
				    ADF5902_REG16_TX_DATA_TRIG(dev->tx_trig_en) |
				    ADF5902_REG16_DEL_SEL(i));
		if (ret != SUCCESS)
			return ret;
	}

	/* Load Step Register */
	for (i = 0; i < dev->step_words_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG15 | ADF5902_REG15_RESERVED |
				    ADF5902_REG15_STEP_WORD(dev->step_words[i]) |
				    ADF5902_REG15_STEP_SEL(i));
		if (ret != SUCCESS)
			return ret;
	}

	/* Load Deviation Register */
	for (i = 0; i < dev->freq_dev_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG15 | ADF5902_REG15_RESERVED |
				    ADF5902_REG14_DEV_WORD(dev->freq_dev[i].dev_word) |
				    ADF5902_REG14_DEV_OFFSET(dev->freq_dev[i].dev_offset) |
				    ADF5902_REG14_DEV_SEL(i));
		if (ret != SUCCESS)
			return ret;
	}

	/* Load Clock Register */
	for (i = 0; i < dev->clk2_div_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG13 | ADF5902_REG13_RESERVED |
				    ADF5902_REG13_CLK_DIV_2(dev->clk2_div[i]) |
				    ADF5902_REG13_CLK_DIV_SEL(i) |
				    ADF5902_REG13_CLK_DIV_MODE(dev->clk_div_mode) |
				    ADF5902_REG13_LE_SEL(dev->le_sel));
		if (ret != SUCCESS)
			return ret;
	}

	ret = adf5902_write(dev, ADF5902_REG12 | ADF5902_REG12_RESERVED |
			    ADF5902_REG12_CP_TRISTATE(dev->cp_tristate_en) |
			    ADF5902_REG12_CHARGE_PUMP(dev->cp_current));
	if (ret != SUCCESS)
		return ret;

	/* VCO Normal Operation */
	ret = adf5902_write(dev, ADF5902_REG9 | ADF5902_REG9_RESERVED_NORMAL);
	if (ret != SUCCESS)
		return ret;

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

	ret = adf5902_write(dev, ADF5902_REG4 | ADF5902_REG4_RESERVED |
			    ADF5902_REG4_TEST_BUS(ADF5902_RAMP_DOWN_TO_MUXOUT));
	if (ret != SUCCESS)
		return ret;

	ret = adf5902_write(dev, ADF5902_REG3 | ADF5902_REG3_RESERVED |
			    ADF5902_REG3_READBACK_CTRL(ADF5902_REG_RB_NONE) |
			    ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
			    ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_RAMP_STATUS));
	if (ret != SUCCESS)
		return ret;

	udelay(100);

	ret = adf5902_write(dev, ADF5902_REG11 | ADF5902_REG11_RESERVED |
			    ADF5902_REG11_RAMP_MODE(ADF5902_CONTINUOUS_TRIANGULAR));

	if (ret != SUCCESS)
		return ret;

	*device = dev;

	return ret;
}

/**
 * @brief Recalibration procedure
 * @param dev - The device structure
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902_recalibrate(struct adf5902_dev *dev)
{
	int32_t ret;

	/* Turn Tx1 off, Tx2 off, LO off */
	ret = adf5902_write(dev, ADF5902_REG0 | ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_DOWN_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != SUCCESS)
		return ret;

	/* VCO Calibration Setup */
	ret = adf5902_write(dev, ADF5902_REG9 | ADF5902_REG9_RESERVED_CALIB);
	if (ret != SUCCESS)
		return ret;

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

	/* VCO Normal Operation */
	ret = adf5902_write(dev, ADF5902_REG9 | ADF5902_REG9_RESERVED_NORMAL);
	if (ret != SUCCESS)
		return ret;

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
			    ADF5902_REG5_INTEGER_WORD(dev->int_div) |
			    ADF5902_REG5_RAMP_ON(ADF5902_RAMP_ON_DISABLED));
	if (ret != SUCCESS)
		return ret;

	ret = adf5902_write(dev, ADF5902_REG4 | ADF5902_REG4_RESERVED |
			    ADF5902_REG4_TEST_BUS(ADF5902_RAMP_DOWN_TO_MUXOUT));
	if (ret != SUCCESS)
		return ret;

	ret = adf5902_write(dev, ADF5902_REG3 | ADF5902_REG3_RESERVED |
			    ADF5902_REG3_READBACK_CTRL(ADF5902_REG_RB_NONE) |
			    ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
			    ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_RAMP_STATUS));
	if (ret != SUCCESS)
		return ret;

	udelay(100);

	ret = adf5902_write(dev, ADF5902_REG11 | ADF5902_REG11_RESERVED |
			    ADF5902_REG11_RAMP_MODE(ADF5902_CONTINUOUS_TRIANGULAR));

	return ret;
}

/**
 * @brief Free resoulces allocated for ADF5902
 * @param dev - The device structure.
 * @param temp - The temperature value.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902_read_temp(struct adf5902_dev *dev, float *temp)
{
	int32_t ret;
	uint32_t reg_data;

	/* Connect ATEST to ADC and Temperature sensor to ATEST */
	ret = adf5902_write(dev, ADF5902_REG4 | ADF5902_REG4_RESERVED |
			    ADF5902_REG4_TEST_BUS(ADF5902_TEMP_SENS_TO_ADC));
	if (ret != SUCCESS)
		return ret;

	/* Start ADC Conversion */
	ret = adf5902_write(dev, ADF5902_REG2 | ADF5902_REG2_RESERVED |
			    ADF5902_REG2_ADC_CLK_DIV(dev->adc_clk_div) |
			    ADF5902_REG2_ADC_AVG(dev->adc_avg) |
			    ADF5902_REG2_ADC_START(ADF5902_START_ADC_CONV));
	if (ret != SUCCESS)
		return ret;

	/* Make sure ADC conversion is finished */
	mdelay(10);

	/* Read ADC Data */
	reg_data = ADF5902_REG3 | ADF5902_REG3_RESERVED |
		   ADF5902_REG3_READBACK_CTRL(ADF5902_ADC_RB) |
		   ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
		   ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_RAMP_STATUS);

	ret = adf5902_readback(dev, &reg_data);
	if (ret != SUCCESS)
		return ret;

	/* Set Register 4 to initial value */
	ret = adf5902_write(dev, ADF5902_REG4 | ADF5902_REG4_RESERVED |
			    ADF5902_REG4_TEST_BUS(ADF5902_RAMP_DOWN_TO_MUXOUT));
	if (ret != SUCCESS)
		return ret;

	/* Set Register 2 to initial value */
	ret = adf5902_write(dev, ADF5902_REG2 | ADF5902_REG2_RESERVED |
			    ADF5902_REG2_ADC_CLK_DIV(dev->adc_clk_div) |
			    ADF5902_REG2_ADC_AVG(dev->adc_avg) |
			    ADF5902_REG2_ADC_START(ADF5902_ADC_NORMAL_OP));
	if (ret != SUCCESS)
		return ret;

	/* Compute temperature */
	*temp = ((((uint8_t)reg_data * ADF5902_VLSB) - ADF5902_VOFF) / ADF5902_VGAIN);

	return ret;
}

/**
 * @brief Measure output locked frequency
 * @param dev - The device structure.
 * @param freq - Measured frequency.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902f_compute_frequency(struct adf5902_dev *dev, float freq)
{
	int32_t ret;
	uint32_t freq1, freq2, delay, clk_div, i;
	uint64_t delta_freq;

	clk_div = (dev->clk2_div[0] * pow(2, 12)) + dev->clk1_div;
	/* Delay conversion to us */
	delay = clk_div * 10000000 / dev->f_pfd;

	/* Read ADC Data */
	freq1 = ADF5902_REG3 | ADF5902_REG3_RESERVED |
		ADF5902_REG3_READBACK_CTRL(ADF5902_FREQ_RB) |
		ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
		ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_RAMP_STATUS);

	ret = adf5902_readback(dev, &freq1);
	if (ret != SUCCESS)
		return ret;

	ret = adf5902_write(dev, ADF5902_REG7 | ADF5902_REG7_RESERVED |
			    ADF5902_REG7_R_DIVIDER(dev->ref_div_factor) |
			    ADF5902_REG7_REF_DOUBLER(dev->ref_doubler_en) |
			    ADF5902_REG7_R_DIV_2(dev->ref_div2_en) |
			    ADF5902_REG7_CLK_DIV(1808) |
			    ADF5902_REG7_MASTER_RESET(ADF5902_MASTER_RESET_DISABLE));
	if (ret != SUCCESS)
		return ret;

	for (i = 0; i < dev->clk2_div_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG13 | ADF5902_REG13_RESERVED |
				    ADF5902_REG13_CLK_DIV_2(10) |
				    ADF5902_REG13_CLK_DIV_SEL(i) |
				    ADF5902_REG13_CLK_DIV_MODE(dev->clk_div_mode) |
				    ADF5902_REG13_LE_SEL(dev->le_sel));
		if (ret != SUCCESS)
			return ret;
	}

	/* Ramp Mode Disabled */
	ret = adf5902_write(dev, ADF5902_REG5 | ADF5902_REG5_RESERVED |
			    ADF5902_REG5_FRAC_MSB_WORD(dev->frac_msb) |
			    ADF5902_REG5_INTEGER_WORD(dev->int_div) |
			    ADF5902_REG5_RAMP_ON(ADF5902_RAMP_ON_DISABLED));

	for (i = 0; i < dev->clk2_div_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG13 | ADF5902_REG13_RESERVED |
				    ADF5902_REG13_CLK_DIV_2(10) |
				    ADF5902_REG13_CLK_DIV_SEL(i) |
				    ADF5902_REG13_CLK_DIV_MODE(ADF5902_FREQ_MEASURE) |
				    ADF5902_REG13_LE_SEL(dev->le_sel));
		if (ret != SUCCESS)
			return ret;
	}

	udelay(delay);

	freq2 = ADF5902_REG3 | ADF5902_REG3_RESERVED |
		ADF5902_REG3_READBACK_CTRL(ADF5902_FREQ_RB) |
		ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
		ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_RAMP_STATUS);

	ret = adf5902_readback(dev, &freq2);
	if (ret != SUCCESS)
		return ret;

	ret = adf5902_write(dev, ADF5902_REG7 | ADF5902_REG7_RESERVED |
			    ADF5902_REG7_R_DIVIDER(dev->ref_div_factor) |
			    ADF5902_REG7_REF_DOUBLER(dev->ref_doubler_en) |
			    ADF5902_REG7_R_DIV_2(dev->ref_div2_en) |
			    ADF5902_REG7_CLK_DIV(dev->clk1_div) |
			    ADF5902_REG7_MASTER_RESET(ADF5902_MASTER_RESET_DISABLE));
	if (ret != SUCCESS)
		return ret;

	for (i = 0; i < dev->clk2_div_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG13 | ADF5902_REG13_RESERVED |
				    ADF5902_REG13_CLK_DIV_2(dev->clk2_div[i]) |
				    ADF5902_REG13_CLK_DIV_SEL(i) |
				    ADF5902_REG13_CLK_DIV_MODE(dev->clk_div_mode) |
				    ADF5902_REG13_LE_SEL(dev->le_sel));
		if (ret != SUCCESS)
			return ret;
	}

	if (freq1 < freq2)
		delta_freq = (pow(2, 16) - freq1) + freq2;

	if (freq1 > freq2)
		delta_freq = (freq2 - freq1);

	freq = ((float)delta_freq / clk_div) * dev->f_pfd * ((float)dev->rf_out /
			(dev->f_pfd * 2)) * 2;

	return ret;
}

/**
 * @brief Free resoulces allocated for ADF5902
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902_remove(struct adf5902_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	free(dev);

	return ret;
}
