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
#include <malloc.h>
#include "adf5902.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Writes 4 bytes of data to ADF5902.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf5902_write(struct adf5902_dev *dev, uint8_t reg_addr, uint32_t data)
{
	uint8_t buff[ADF5902_BUFF_SIZE_BYTES];

	data = data | reg_addr;

	buff[0] = data >> 24;
	buff[1] = data >> 16;
	buff[2] = data >> 8;
	buff[3] = data;

	return no_os_spi_write_and_read(dev->spi_desc, buff, ADF5902_BUFF_SIZE_BYTES);
}

/**
 * @brief Readback data from ADF5902.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Store the read data.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf5902_readback(struct adf5902_dev *dev, uint8_t reg_addr,
			 uint32_t *data)
{
	int32_t ret;
	uint32_t i;
	uint8_t buff[ADF5902_BUFF_SIZE_BYTES];

	buff[0] = *data >> 24;
	buff[1] = *data >> 16;
	buff[2] = (*data >> 8) | (reg_addr >> 3);
	buff[3] =  ADF5902_REG3 | (reg_addr << 5);

	/* Write command */
	ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADF5902_BUFF_SIZE_BYTES);
	if (ret != 0)
		return ret;

	/* Send dummy value to get the data on DOUT */
	buff[0] = ADF5902_SPI_DUMMY_DATA;
	buff[1] = ADF5902_SPI_DUMMY_DATA;
	buff[2] = ADF5902_SPI_DUMMY_DATA;
	buff[3] = ADF5902_SPI_DUMMY_DATA;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADF5902_BUFF_SIZE_BYTES);
	if (ret != 0)
		return ret;

	/* Concatenate received data Bytes */
	for (i = 0; i < ADF5902_BUFF_SIZE_BYTES; i++)
		*data = (*data << 8) | buff[i];

	return ret;
}

/**
 * @brief Compute ADF4350 RF VCO frequency parameters.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
static int32_t adf5902_vco_freq_param(struct adf5902_dev *dev)
{
	uint32_t tmp;

	dev->ref_div_factor = 0;

	do {
		dev->ref_div_factor++;
		dev->f_pfd = (dev->ref_in * (1 + dev->ref_doubler_en) /
			      (dev->ref_div_factor *(1 + dev->ref_div2_en)));
	} while (dev->f_pfd > ADF5902_MAX_FREQ_PFD);

	dev->int_div = (uint16_t)(dev->rf_out / (dev->f_pfd * 2));

	if (dev->int_div > ADF5902_MAX_INT_MSB_WORD)
		return -1;

	tmp = (((dev->rf_out) * (1 << 25) / (dev->f_pfd * 2)) -
	       (dev->int_div * (1 << 25)));

	dev->frac_msb = (tmp >> 13) & ADF5902_FRAC_MSB_MSK;
	dev->frac_lsb = tmp & ADF5902_FRAC_LSB_MSK;

	/* Set frequency calibration divider value */
	dev->freq_cal_div = NO_OS_DIV_ROUND_UP(dev->f_pfd, ADF5902_FREQ_CAL_DIV_100KHZ);

	if(dev->freq_cal_div > ADF5902_MAX_FREQ_CAL_DIV)
		return -1;

	/* Set clock divider value */
	dev->clk1_div = NO_OS_DIV_ROUND_UP(dev->f_pfd, ADF5902_CLK1_DIV_25KHZ);

	if (dev->clk1_div > ADF5902_MAX_CLK_DIVIDER)
		return -1;

	/* Set ADC clock divider value for 1MHz ADC clock */
	dev->adc_clk_div = dev->f_pfd / ADF5902_ADC_CLK_DIV_1MHZ;

	if (dev->adc_clk_div > ADF5902_ADC_MAX_CLK_DIVIDER)
		return -1;

	return 0;
}

/**
 * @brief Check if initialization parameters contain valid data
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
static int32_t adf5902_check_init_param(struct adf5902_init_param *init_param)
{
	uint32_t i;

	if ((init_param->rf_out > ADF5902_MAX_VCO_FREQ)
	    || (init_param->rf_out < ADF5902_MIN_VCO_FREQ))
		return -EINVAL;

	if ((init_param->ref_in > ADF5902_MAX_REFIN_FREQ)
	    || (init_param->ref_in < ADF5902_MIN_REFIN_FREQ))
		return -EINVAL;


	if ((init_param->ref_doubler_en != ADF5902_REF_DOUBLER_DISABLE)
	    && (init_param->ref_doubler_en != ADF5902_REF_DOUBLER_ENABLE))
		return -EINVAL;

	if ((init_param->ref_div2_en != ADF5902_R_DIV_2_DISABLE)
	    && (init_param->ref_div2_en != ADF5902_R_DIV_2_ENABLE))
		return -EINVAL;

	if (init_param->adc_avg > ADF5902_ADC_AVG_4)
		return -EINVAL;

	if (init_param->delay_words_no > (ADF5902_DEL_SEL_3 + 1))
		return -EINVAL;

	for (i = 0; i < init_param->delay_words_no; i++)
		if (init_param->delay_wd[i] > ADF5902_MAX_DELAY_START_WRD)
			return -EINVAL;

	if ((init_param->ramp_delay_en != ADF5902_RAMP_DEL_DISABLE)
	    && (init_param->ramp_delay_en != ADF5902_RAMP_DEL_ENABLE))
		return -EINVAL;

	if ((init_param->tx_trig_en != ADF5902_TX_DATA_TRIG_DISABLE)
	    && (init_param->tx_trig_en != ADF5902_TX_DATA_TRIG_ENABLE))
		return -EINVAL;

	if (init_param->slopes_no > (ADF5902_STEP_SEL_3 + 1))
		return -EINVAL;

	for (i = 0; i < init_param->slopes_no; i++)
		if((init_param->slopes[i].step_word > ADF5902_MAX_STEP_WORD) ||
		    (init_param->slopes[i].dev_offset > ADF5902_MAX_DEV_OFFSET))
			return -EINVAL;

	if ((init_param->tx_ramp_clk != ADF5902_TX_RAMP_CLK_DIV)
	    && (init_param->tx_ramp_clk != ADF5902_TX_RAMP_TX_DATA_PIN))
		return -EINVAL;

	if ((init_param->tx_data_invert != AD5902_TX_DATA_INV_DISABLE)
	    && (init_param->tx_data_invert != AD5902_TX_DATA_INV_ENABLE))
		return -EINVAL;

	if (init_param->clk2_div_no > (ADF5902_CLK_DIV_SEL_3 + 1))
		return -EINVAL;

	for (i = 0; i < init_param->clk2_div_no; i++)
		if(init_param->clk2_div[i] > ADF5902_MAX_CLK_DIV_2)
			return -EINVAL;

	if((init_param->clk_div_mode != ADF5902_CLK_DIV_OFF) &&
	    (init_param->clk_div_mode != ADF5902_FREQ_MEASURE) &&
	    (init_param->clk_div_mode != ADF5902_RAMP_DIV))
		return -EINVAL;

	if((init_param->le_sel != ADF5902_LE_FROM_PIN) &&
	    (init_param->le_sel != ADF5902_LE_SYNC_REFIN))
		return -EINVAL;

	if (init_param->cp_current > ADF5902_CP_CURRENT_4MA48)
		return -EINVAL;

	if ((init_param->cp_tristate_en != ADF5902_CP_TRISTATE_DISABLE)
	    && (init_param->cp_tristate_en != ADF5902_CP_TRISTATE_ENABLE))
		return -EINVAL;

	if(init_param->ramp_status != ADF5902_TEST_BUS_NONE &&
	    init_param->ramp_status != ADF5902_RAMP_COMPL_TO_MUXOUT &&
	    init_param->ramp_status != ADF5902_RAMP_DOWN_TO_MUXOUT &&
	    init_param->ramp_status != ADF5902_TEMP_SENS_TO_ATEST &&
	    init_param->ramp_status != ADF5902_TEMP_SENS_TO_ADC)
		return -EINVAL;

	if(init_param->ramp_mode != ADF5902_CONT_SAWTOOTH &&
	    init_param->ramp_mode != ADF5902_SAWTOOTH_BURST &&
	    init_param->ramp_mode != ADF5902_CONTINUOUS_TRIANGULAR &&
	    init_param->ramp_mode != ADF5902_SINGLE_RAMP_BURST)
		return -EINVAL;

	if (init_param->clk1_div_ramp > ADF5902_MAX_CLK_DIVIDER)
		return -EINVAL;

	return 0;
}

/**
 * @brief VCO Frequency Calibration
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
static int32_t adf5902_vco_freq_calib(struct adf5902_dev *dev)
{
	int32_t ret;

	/* Set PFD, CLK1 */
	ret = adf5902_write(dev, ADF5902_REG7, ADF5902_REG7_RESERVED |
			    ADF5902_REG7_R_DIVIDER(dev->ref_div_factor) |
			    ADF5902_REG7_REF_DOUBLER(dev->ref_doubler_en) |
			    ADF5902_REG7_R_DIV_2(dev->ref_div2_en) |
			    ADF5902_REG7_CLK_DIV(dev->clk1_div) |
			    ADF5902_REG7_MASTER_RESET(ADF5902_MASTER_RESET_DISABLE));
	if (ret != 0)
		return ret;

	/* Set LSB FRAC */
	ret = adf5902_write(dev, ADF5902_REG6, ADF5902_REG6_RESERVED |
			    ADF5902_REG6_FRAC_LSB_WORD(dev->frac_lsb));
	if (ret != 0)
		return ret;

	/* Set MSB FRAC and INT */
	ret = adf5902_write(dev, ADF5902_REG5, ADF5902_REG5_RESERVED |
			    ADF5902_REG5_FRAC_MSB_WORD(dev->frac_msb) |
			    ADF5902_REG5_INTEGER_WORD(dev->int_div) |
			    ADF5902_REG5_RAMP_ON(ADF5902_RAMP_ON_DISABLED));
	if (ret != 0)
		return ret;

	/* Set ATEST to high Impedance */
	ret = adf5902_write(dev, ADF5902_REG4, ADF5902_REG4_RESERVED |
			    ADF5902_REG4_TEST_BUS(ADF5902_TEST_BUS_NONE));
	if (ret != 0)
		return ret;

	/* Set IO level to 3.3V, CAL_BUSY to MUXOUT */
	ret = adf5902_write(dev, ADF5902_REG3, ADF5902_REG3_RESERVED |
			    ADF5902_REG3_READBACK_CTRL(ADF5902_REG_RB_NONE) |
			    ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
			    ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_CAL_BUSY));
	if (ret != 0)
		return ret;

	/* Set ADC Clock to 1MHz */
	ret = adf5902_write(dev, ADF5902_REG2, ADF5902_REG2_RESERVED |
			    ADF5902_REG2_ADC_CLK_DIV(dev->adc_clk_div) |
			    ADF5902_REG2_ADC_AVG(dev->adc_avg) |
			    ADF5902_REG2_ADC_START(ADF5902_ADC_NORMAL_OP));
	if (ret != 0)
		return ret;

	/* Set Transmitter amplitude level */
	ret = adf5902_write(dev, ADF5902_REG1, ADF5902_REG1_RESERVED |
			    ADF5902_REG1_TX_AMP_CAL_REF(dev->tx_amp_cal_ref));
	if (ret != 0)
		return ret;

	/* Start VCO frequency calibration */
	ret = adf5902_write(dev, ADF5902_REG0, ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_FULL_CAL) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != 0)
		return ret;

	/* Required 1200us delay */
	no_os_udelay(1200);

	/* Tx1 on, Tx2 off, LO on */
	ret = adf5902_write(dev, ADF5902_REG0, ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_UP_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != 0)
		return ret;

	/* Tx1 amplitude calibration */
	ret = adf5902_write(dev, ADF5902_REG0, ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_UP_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_AMP_CAL) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != 0)
		return ret;

	/* Required 500us delay */
	no_os_udelay(500);

	/* Tx1 off, Tx2 on, LO on */
	ret = adf5902_write(dev, ADF5902_REG0, ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_UP_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != 0)
		return ret;

	/* Tx2 amplitude calibration */
	ret = adf5902_write(dev, ADF5902_REG0, ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_UP_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_AMP_CAL));
	if (ret != 0)
		return ret;

	/* Required 500us delay */
	no_os_udelay(500);

	return ret;
}

/**
 * @brief VCO Normal Operation
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
static int32_t adf5902_vco_normal_op(struct adf5902_dev *dev)
{
	int32_t ret;

	/* VCO Normal Operation */
	ret = adf5902_write(dev, ADF5902_REG9, ADF5902_REG9_RESERVED_NORMAL);
	if (ret != 0)
		return ret;

	/* Compute new f_pfd */
	ret = adf5902_vco_freq_param(dev);
	if (ret != 0)
		return ret;

	ret = adf5902_write(dev, ADF5902_REG7, ADF5902_REG7_RESERVED |
			    ADF5902_REG7_R_DIVIDER(dev->ref_div_factor) |
			    ADF5902_REG7_REF_DOUBLER(dev->ref_doubler_en) |
			    ADF5902_REG7_R_DIV_2(ADF5902_R_DIV_2_DISABLE) |
			    ADF5902_REG7_CLK_DIV(dev->clk1_div_ramp) |
			    ADF5902_REG7_MASTER_RESET(ADF5902_MASTER_RESET_DISABLE));
	if (ret != 0)
		return ret;

	/* Set LSB FRAC */
	ret = adf5902_write(dev, ADF5902_REG6, ADF5902_REG6_RESERVED |
			    ADF5902_REG6_FRAC_LSB_WORD(dev->frac_lsb));
	if (ret != 0)
		return ret;

	/* Set MSB FRAC and INT */
	ret = adf5902_write(dev, ADF5902_REG5, ADF5902_REG5_RESERVED |
			    ADF5902_REG5_FRAC_MSB_WORD(dev->frac_msb) |
			    ADF5902_REG5_INTEGER_WORD(dev->int_div));
	if (ret != 0)
		return ret;

	/* Set Ramp Status */
	ret = adf5902_write(dev, ADF5902_REG4, ADF5902_REG4_RESERVED |
			    ADF5902_REG4_TEST_BUS(dev->ramp_status));
	if (ret != 0)
		return ret;

	/* Set I/O Voltage to 3.3V */
	ret = adf5902_write(dev, ADF5902_REG3, ADF5902_REG3_RESERVED |
			    ADF5902_REG3_READBACK_CTRL(ADF5902_REG_RB_NONE) |
			    ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
			    ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_RAMP_STATUS));
	if (ret != 0)
		return ret;

	no_os_udelay(100);

	/* Set Ramp Mode */
	ret = adf5902_write(dev, ADF5902_REG11, ADF5902_REG11_RESERVED |
			    ADF5902_REG11_RAMP_MODE(dev->ramp_mode));
	if (ret != 0)
		return ret;

	return ret;
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

	/* Check ADF5902 Initialization Paramteres */
	ret = adf5902_check_init_param(init_param);
	if(ret != 0)
		return ret;

	dev = (struct adf5902_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* GPIO Chip Enable */
	ret = no_os_gpio_get(&dev->gpio_ce, init_param->gpio_ce_param);
	if (ret != 0)
		goto error_dev;

	ret = no_os_gpio_direction_output(dev->gpio_ce, NO_OS_GPIO_HIGH);
	if (ret != 0)
		goto error_dev;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret != 0)
		goto error_gpio;

	/* Initialization Parameters */
	dev->rf_out = init_param->rf_out;
	dev->ref_in = init_param->ref_in;
	dev->ref_doubler_en = init_param->ref_doubler_en;
	dev->adc_avg = init_param->adc_avg;
	dev->tx_amp_cal_ref = init_param->tx_amp_cal_ref;
	dev->delay_words_no = init_param->delay_words_no;
	dev->delay_wd = init_param->delay_wd;
	dev->ramp_delay_en = init_param->ramp_delay_en;
	dev->tx_trig_en = init_param->tx_trig_en;
	dev->slopes_no = init_param->slopes_no;
	dev->slopes = init_param->slopes;
	dev->tx_ramp_clk = init_param->tx_ramp_clk;
	dev->tx_data_invert = init_param->tx_data_invert;
	dev->clk2_div_no = init_param->clk2_div_no;
	dev->clk2_div = init_param->clk2_div;
	dev->clk_div_mode = init_param->clk_div_mode;
	dev->le_sel = init_param->le_sel;
	dev->cp_current = init_param->cp_current;
	dev->cp_tristate_en = init_param->cp_tristate_en;
	dev->ramp_status = init_param->ramp_status;
	dev->ramp_mode = init_param->ramp_mode;
	dev->clk1_div_ramp = init_param->clk1_div_ramp;

	/* Enable R/2 Div for the VCO Calibration procedure */
	dev->ref_div2_en = ADF5902_R_DIV_2_ENABLE;

	/* Set device specific parameters for obtaining the VCO frequency */
	ret = adf5902_vco_freq_param(dev);
	if (ret != 0)
		goto error_spi;

	/* Master Reset */
	ret = adf5902_write(dev, ADF5902_REG7, ADF5902_REG7_RESERVED |
			    ADF5902_REG7_MASTER_RESET(ADF5902_MASTER_RESET_ENABLE));
	if (ret != 0)
		goto error_spi;

	/* Reset Counters */
	ret = adf5902_write(dev, ADF5902_REG11, ADF5902_REG11_RESERVED |
			    ADF5902_REG11_CNTR_RESET(ADF5902_CNTR_RESET_ENABLE));
	if (ret != 0)
		goto error_spi;

	/* Enable Counters */
	ret = adf5902_write(dev, ADF5902_REG11, ADF5902_REG11_RESERVED |
			    ADF5902_REG11_CNTR_RESET(ADF5902_CNTR_RESET_DISABLE));
	if (ret != 0)
		goto error_spi;

	/* Set clock divider mode to Ramp Divider */
	ret = adf5902_write(dev, ADF5902_REG13, ADF5902_REG13_RESERVED |
			    ADF5902_REG13_CLK_DIV_MODE(dev->clk_div_mode) |
			    ADF5902_REG13_LE_SEL(dev->le_sel));
	if (ret != 0)
		goto error_spi;

	/* Register 10 Reserved */
	ret = adf5902_write(dev, ADF5902_REG10, ADF5902_REG10_RESERVED);
	if (ret != 0)
		goto error_spi;

	/* VCO Calibration Setup */
	ret = adf5902_write(dev, ADF5902_REG9, ADF5902_REG9_RESERVED_CALIB);
	if (ret != 0)
		goto error_spi;

	/* Set VCO frequency calibration divider clock to 100kHz */
	ret = adf5902_write(dev, ADF5902_REG8, ADF5902_REG8_RESERVED |
			    ADF5902_REG8_FREQ_CAL_DIV(dev->freq_cal_div));
	if (ret != 0)
		goto error_spi;

	/* Power up the device and LO */
	ret = adf5902_write(dev, ADF5902_REG0, ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_UP_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != 0)
		goto error_spi;

	/* Required delay */
	no_os_udelay(10);

	/* Start VCO Frequency Calibration */
	ret = adf5902_vco_freq_calib(dev);
	if (ret != 0)
		goto error_spi;

	/* Set R17 Reserved */
	ret = adf5902_write(dev, ADF5902_REG17, ADF5902_REG17_RESERVED);
	if (ret != 0)
		goto error_spi;

	/* Ramp delay register */
	for (i = 0; i < dev->delay_words_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG16, ADF5902_REG16_RESERVED |
				    ADF5902_REG16_DEL_START_WORD(dev->delay_wd[i]) |
				    ADF5902_REG16_RAMP_DEL(dev->ramp_delay_en) |
				    ADF5902_REG16_TX_DATA_TRIG(dev->tx_trig_en) |
				    ADF5902_REG16_DEL_SEL(i));
		if (ret != 0)
			goto error_spi;
	}

	/* Load Slope Parameters */
	for (i = 0; i < dev->slopes_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG15, ADF5902_REG15_RESERVED |
				    ADF5902_REG15_STEP_WORD(dev->slopes[i].step_word) |
				    ADF5902_REG15_STEP_SEL(i));
		if (ret != 0)
			goto error_spi;
		ret = adf5902_write(dev, ADF5902_REG14, ADF5902_REG14_RESERVED |
				    ADF5902_REG14_DEV_WORD(dev->slopes[i].dev_word) |
				    ADF5902_REG14_DEV_OFFSET(dev->slopes[i].dev_offset) |
				    ADF5902_REG14_DEV_SEL(i));
		if (ret != 0)
			goto error_spi;
	}

	/* Load Clock Register */
	for (i = 0; i < dev->clk2_div_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG13, ADF5902_REG13_RESERVED |
				    ADF5902_REG13_CLK_DIV_2(dev->clk2_div[i]) |
				    ADF5902_REG13_CLK_DIV_SEL(i) |
				    ADF5902_REG13_CLK_DIV_MODE(dev->clk_div_mode) |
				    ADF5902_REG13_LE_SEL(dev->le_sel));
		if (ret != 0)
			goto error_spi;
	}

	/* Set Charge Pump Current */
	ret = adf5902_write(dev, ADF5902_REG12, ADF5902_REG12_RESERVED |
			    ADF5902_REG12_CP_TRISTATE(dev->cp_tristate_en) |
			    ADF5902_REG12_CHARGE_PUMP(dev->cp_current));
	if (ret != 0)
		goto error_spi;

	/* Init R/2 Div with the initialization parameter */
	dev->ref_div2_en = init_param->ref_div2_en;

	ret = adf5902_vco_freq_param(dev);
	if (ret != 0)
		goto error_spi;

	/* Start VCO Normal Operation */
	ret = adf5902_vco_normal_op(dev);
	if (ret != 0)
		goto error_spi;

	*device = dev;

	return ret;

error_spi:
	no_os_spi_remove(dev->spi_desc);

error_gpio:
	no_os_gpio_remove(dev->gpio_ce);

error_dev:
	no_os_free(dev);

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

	/* Store R/2 Div value before Recalibration Procedure */
	uint8_t temp = dev->ref_div2_en;

	/* Compute VCO parameters for the calibration procedure */
	ret = adf5902_vco_freq_param(dev);
	if (ret != 0)
		return ret;

	/* Turn Tx1 off, Tx2 off, LO off */
	ret = adf5902_write(dev, ADF5902_REG0, ADF5902_REG0_RESERVED |
			    ADF5902_REG0_PLO(ADF5902_POWER_DOWN_LO) |
			    ADF5902_REG0_PTX1(ADF5902_POWER_DOWN_TX1) |
			    ADF5902_REG0_PTX2(ADF5902_POWER_DOWN_TX2) |
			    ADF5902_REG0_PADC(ADF5902_POWER_UP_ADC) |
			    ADF5902_REG0_VCAL(ADF5902_VCO_NORMAL_OP) |
			    ADF5902_REG0_PVCO(ADF5902_POWER_UP_VCO) |
			    ADF5902_REG0_TX1C(ADF5902_TX1_NORMAL_OP) |
			    ADF5902_REG0_TX2C(ADF5902_TX2_NORMAL_OP));
	if (ret != 0)
		return ret;

	/* VCO Calibration Setup */
	ret = adf5902_write(dev, ADF5902_REG9, ADF5902_REG9_RESERVED_CALIB);
	if (ret != 0)
		return ret;

	/* Start VCO Calibration */
	ret = adf5902_vco_freq_calib(dev);
	if (ret != 0)
		return ret;

	/* VCO Normal Operation */
	ret = adf5902_write(dev, ADF5902_REG9, ADF5902_REG9_RESERVED_NORMAL);
	if (ret != 0)
		return ret;

	/* Get back the initial value for R/2 Div */
	dev->ref_div2_en = temp;

	/* Compute VCO parameters for the calibration procedure */
	ret = adf5902_vco_freq_param(dev);
	if (ret != 0)
		return ret;

	/* Start VCO Normal Operation */
	ret = adf5902_vco_normal_op(dev);
	if (ret != 0)
		return ret;

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
	ret = adf5902_write(dev, ADF5902_REG4, ADF5902_REG4_RESERVED |
			    ADF5902_REG4_TEST_BUS(ADF5902_TEMP_SENS_TO_ADC));
	if (ret != 0)
		return ret;

	/* Start ADC Conversion */
	ret = adf5902_write(dev, ADF5902_REG2, ADF5902_REG2_RESERVED |
			    ADF5902_REG2_ADC_CLK_DIV(dev->adc_clk_div) |
			    ADF5902_REG2_ADC_AVG(dev->adc_avg) |
			    ADF5902_REG2_ADC_START(ADF5902_START_ADC_CONV));
	if (ret != 0)
		return ret;

	/* Make sure ADC conversion is finished */
	no_os_udelay(1200);

	/* Read ADC Data */
	reg_data = ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
		   ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_RAMP_STATUS);

	ret = adf5902_readback(dev, ADF5902_ADC_RB, &reg_data);
	if (ret != 0)
		return ret;

	/* Set Register 4 to initial value */
	ret = adf5902_write(dev, ADF5902_REG4, ADF5902_REG4_RESERVED |
			    ADF5902_REG4_TEST_BUS(ADF5902_RAMP_DOWN_TO_MUXOUT));
	if (ret != 0)
		return ret;

	/* Set Register 2 to initial value */
	ret = adf5902_write(dev, ADF5902_REG2, ADF5902_REG2_RESERVED |
			    ADF5902_REG2_ADC_CLK_DIV(dev->adc_clk_div) |
			    ADF5902_REG2_ADC_AVG(dev->adc_avg) |
			    ADF5902_REG2_ADC_START(ADF5902_ADC_NORMAL_OP));
	if (ret != 0)
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
int32_t adf5902f_compute_frequency(struct adf5902_dev *dev, uint64_t *freq)
{
	int32_t ret;
	uint16_t clk1_div = 1808;
	uint16_t clk2_div = 10;
	uint32_t i;
	uint32_t freq1, freq2;
	uint64_t delta_freq = 0, clk_div, delay;

	/* Compute CLK_DIV */
	clk_div = (clk2_div * (1 << 12)) + clk1_div;

	/* Delay conversion to us */
	delay = ((clk_div * 1000000) / dev->f_pfd) + 1;

	/* Read Frequency Data 1 */
	freq1 = ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
		ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_RAMP_STATUS);

	ret = adf5902_readback(dev, ADF5902_FREQ_RB, &freq1);
	if (ret != 0)
		return ret;

	/* Set CLK2 to 10 */
	for (i = 0; i < dev->clk2_div_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG13, ADF5902_REG13_RESERVED |
				    ADF5902_REG13_CLK_DIV_2(clk2_div) |
				    ADF5902_REG13_CLK_DIV_SEL(i) |
				    ADF5902_REG13_CLK_DIV_MODE(ADF5902_CLK_DIV_OFF) |
				    ADF5902_REG13_LE_SEL(dev->le_sel));
		if (ret != 0)
			return ret;
	}

	/* Set CLK1 to 1808 */
	ret = adf5902_write(dev, ADF5902_REG7, ADF5902_REG7_RESERVED |
			    ADF5902_REG7_R_DIVIDER(dev->ref_div_factor) |
			    ADF5902_REG7_REF_DOUBLER(dev->ref_doubler_en) |
			    ADF5902_REG7_R_DIV_2(dev->ref_div2_en) |
			    ADF5902_REG7_CLK_DIV(clk1_div) |
			    ADF5902_REG7_MASTER_RESET(ADF5902_MASTER_RESET_DISABLE));
	if (ret != 0)
		return ret;

	/* Ramp Mode Disabled */
	ret = adf5902_write(dev, ADF5902_REG5, ADF5902_REG5_RESERVED |
			    ADF5902_REG5_FRAC_MSB_WORD(dev->frac_msb) |
			    ADF5902_REG5_INTEGER_WORD(dev->int_div) |
			    ADF5902_REG5_RAMP_ON(ADF5902_RAMP_ON_DISABLED));

	/* Set CLK DIV MODE to Frequency Measure */
	for (i = 0; i < dev->clk2_div_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG13, ADF5902_REG13_RESERVED |
				    ADF5902_REG13_CLK_DIV_2(clk2_div) |
				    ADF5902_REG13_CLK_DIV_SEL(i) |
				    ADF5902_REG13_CLK_DIV_MODE(ADF5902_FREQ_MEASURE) |
				    ADF5902_REG13_LE_SEL(dev->le_sel));
		if (ret != 0)
			return ret;
	}

	/* Add minumum required delay */
	no_os_udelay(delay);

	/* Read Frequency Data 2 */
	freq2 = ADF5902_REG3_IO_LVL(ADF5902_IO_LVL_3V3) |
		ADF5902_REG3_MUXOUT(ADF5902_MUXOUT_RAMP_STATUS);

	ret = adf5902_readback(dev, ADF5902_FREQ_RB, &freq2);
	if (ret != 0)
		return ret;

	/* Set Register 7 to initial values */
	ret = adf5902_write(dev, ADF5902_REG7, ADF5902_REG7_RESERVED |
			    ADF5902_REG7_R_DIVIDER(dev->ref_div_factor) |
			    ADF5902_REG7_REF_DOUBLER(dev->ref_doubler_en) |
			    ADF5902_REG7_R_DIV_2(ADF5902_R_DIV_2_DISABLE) |
			    ADF5902_REG7_CLK_DIV(dev->clk1_div_ramp) |
			    ADF5902_REG7_MASTER_RESET(ADF5902_MASTER_RESET_DISABLE));
	if (ret != 0)
		return ret;

	/* Set Register 13 to initial values */
	for (i = 0; i < dev->clk2_div_no; i++) {
		ret = adf5902_write(dev, ADF5902_REG13, ADF5902_REG13_RESERVED |
				    ADF5902_REG13_CLK_DIV_2(dev->clk2_div[i]) |
				    ADF5902_REG13_CLK_DIV_SEL(i) |
				    ADF5902_REG13_CLK_DIV_MODE(dev->clk_div_mode) |
				    ADF5902_REG13_LE_SEL(dev->le_sel));
		if (ret != 0)
			return ret;
	}

	/* Compute Output Frequency */
	if (freq1 > freq2)
		delta_freq = ((1 << 16) - freq1) + freq2;

	if (freq1 < freq2)
		delta_freq = (freq2 - freq1);

	*freq = ((float)delta_freq / clk_div) * dev->rf_out;

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

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_ce);

	no_os_free(dev);

	return ret;
}
