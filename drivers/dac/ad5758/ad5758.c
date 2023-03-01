/***************************************************************************//**
 *   @file   ad5758.c
 *   @brief  Implementation of ad5758 Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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

#include "ad5758.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "inttypes.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_alloc.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

/**
 * Compute CRC8 checksum.
 * @param data - The data buffer.
 * @param data_size - The size of the data buffer.
 * @return CRC8 checksum.
 */
static uint8_t ad5758_compute_crc8(uint8_t *data,
				   uint8_t data_size)
{
	uint8_t i;
	uint8_t crc = 0;

	while (data_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			if (((crc & 0x80) != 0) != ((*data & i) != 0)) {
				crc <<= 1;
				crc ^= AD5758_CRC8_POLY;
			} else {
				crc <<= 1;
			}
		}
		data++;
		data_size--;
	}

	return crc;
}

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_spi_reg_read(struct ad5758_dev *dev,
			    uint8_t reg_addr,
			    uint16_t *reg_data)
{
	uint8_t buf[4];
	int32_t ret;

	buf[0] = AD5758_REG_WRITE(AD5758_REG_TWO_STAGE_READBACK_SELECT);
	buf[1] = 0x00;
	buf[2] = reg_addr;

	if (dev->crc_en)
		buf[3] = ad5758_compute_crc8(buf, 3);
	else
		buf[3] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 4);
	if (ret < 0)
		goto spi_err;

	buf[0] = AD5758_REG_WRITE(AD5758_REG_NOP);
	buf[1] = 0x00;
	buf[2] = 0x00;

	if (dev->crc_en)
		buf[3] = ad5758_compute_crc8(buf, 3);
	else
		buf[3] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 4);
	if (ret < 0)
		goto spi_err;

	if ((dev->crc_en) &&
	    (ad5758_compute_crc8(buf, 3) != buf[3]))
		goto error;

	*reg_data = (buf[1] << 8) | buf[2];

	return 0;

spi_err:
	pr_err("%s: Failed spi comm with code: %"PRIi32".\n", __func__, ret);
	return -1;
error:
	pr_err("%s: Failed CRC with code: %"PRIi32".\n", __func__, ret);
	return -1;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_spi_reg_write(struct ad5758_dev *dev,
			     uint8_t reg_addr,
			     uint16_t reg_data)
{
	uint8_t buf[4];

	buf[0] = AD5758_REG_WRITE(reg_addr);
	buf[1] = (reg_data >> 8);
	buf[2] = (reg_data & 0xFF);
	buf[3] = ad5758_compute_crc8(buf, 3);

	return no_os_spi_write_and_read(dev->spi_desc, buf, 4);
}

/**
 * SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad5758_spi_write_mask(struct ad5758_dev *dev,
				     uint8_t reg_addr,
				     uint32_t mask,
				     uint16_t data)
{
	uint16_t reg_data;
	int32_t ret;

	ret = ad5758_spi_reg_read(dev, reg_addr, &reg_data);
	if (ret < 0)
		return -1;

	reg_data &= ~mask;
	reg_data |= data;

	return ad5758_spi_reg_write(dev, reg_addr, reg_data);
}

/**
 * Enable/disable SPI CRC function.
 * @param dev - The device structure.
 * @param crc_en - CRC status
 * Accepted values: 0 - disabled
 *					1 - enabled
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_set_crc(struct ad5758_dev *dev, uint8_t crc_en)
{
	int32_t ret;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_DIGITAL_DIAG_CONFIG,
				    AD5758_DIG_DIAG_CONFIG_SPI_CRC_EN_MSK,
				    AD5758_DIG_DIAG_CONFIG_SPI_CRC_EN_MODE(crc_en));

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}
	dev->crc_en = crc_en;

	return 0;
}

/**
 * Busy wait until CAL_MEM_UNREFRESHED bit in the DIGITAL_DIAG_RESULTS clears
 * @param dev - The device structure.
 * @return 0 in case of success
 */
int32_t ad5758_wait_for_refresh_cycle(struct ad5758_dev *dev)
{
	uint16_t reg_data;
	/*
	 * Wait until the CAL_MEM_UNREFRESHED bit in the DIGITAL_DIAG_RESULTS
	 * register returns to 0.
	 */
	do {
		ad5758_spi_reg_read(dev, AD5758_REG_DIGITAL_DIAG_RESULTS,
				    &reg_data);
	} while (reg_data & AD5758_DIG_DIAG_RESULTS_CAL_MEM_UNREFRESHED_MSK);

	return 0;
}

/**
 * Initiate a software reset
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_soft_reset(struct ad5758_dev *dev)
{
	int32_t ret;

	ret = ad5758_spi_reg_write(dev, AD5758_REG_KEY,
				   AD5758_KEY_CODE_RESET_1);
	if (ret < 0)
		goto error;

	ret = ad5758_spi_reg_write(dev, AD5758_REG_KEY,
				   AD5758_KEY_CODE_RESET_2);
	if (ret < 0)
		goto error;
	/* Wait 100 us */
	no_os_udelay(100);

	return 0;

error:
	pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
	return -1;
}

/**
 * Initiate a calibration memory refresh to the shadow registers
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_calib_mem_refresh(struct ad5758_dev *dev)
{
	int32_t ret;

	ret = ad5758_spi_reg_write(dev, AD5758_REG_KEY,
				   AD5758_KEY_CODE_CALIB_MEM_REFRESH);
	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}

	/* Wait to allow time for the internal calibrations to complete */
	return ad5758_wait_for_refresh_cycle(dev);
}

/**
 * Configure the dc-to-dc controller mode
 * @param dev - The device structure.
 * @param mode - Mode[1:0] bits.
 * Accepted values: DC_DC_POWER_OFF
 *		    DPC_CURRENT_MODE
 *		    DPC_VOLTAGE_MODE
 *		    PPC_CURRENT_MODE
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_set_dc_dc_conv_mode(struct ad5758_dev *dev,
				   enum ad5758_dc_dc_mode mode)
{
	uint16_t reg_data;
	int32_t ret;

	/*
	 * The ENABLE_PPC_BUFFERS bit must be set prior to enabling PPC current
	 * mode.
	 */
	if (mode == PPC_CURRENT_MODE) {
		ret  = ad5758_spi_write_mask(dev, AD5758_REG_ADC_CONFIG,
					     AD5758_ADC_CONFIG_PPC_BUF_MSK,
					     AD5758_ADC_CONFIG_PPC_BUF_EN(1));
		if (ret < 0)
			goto error;
	}

	ret = ad5758_spi_write_mask(dev, AD5758_REG_DCDC_CONFIG1,
				    AD5758_DCDC_CONFIG1_DCDC_MODE_MSK,
				    AD5758_DCDC_CONFIG1_DCDC_MODE_MODE(mode));

	if (ret < 0)
		goto error;
	/*
	 * Poll the BUSY_3WI bit in the DCDC_CONFIG2 register until it is 0.
	 * This allows the 3-wire interface communication to complete.
	 */
	do {
		ad5758_spi_reg_read(dev, AD5758_REG_DCDC_CONFIG2, &reg_data);
	} while (reg_data & AD5758_DCDC_CONFIG2_BUSY_3WI_MSK);
	dev->dc_dc_mode = mode;

	return 0;

error:
	pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
	return -1;
}

/**
 * Set the dc-to-dc converter current limit.
 * @param dev - The device structure.
 * @param ilimit - current limit in mA
 * Accepted values: ILIMIT_150_mA
 *		    ILIMIT_200_mA
 *		    ILIMIT_250_mA
 *		    ILIMIT_300_mA
 *		    ILIMIT_350_mA
 *		    ILIMIT_400_mA
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_set_dc_dc_ilimit(struct ad5758_dev *dev,
				enum ad5758_dc_dc_ilimt ilimit)
{
	uint16_t reg_data;
	int32_t ret;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_DCDC_CONFIG2,
				    AD5758_DCDC_CONFIG2_ILIMIT_MSK,
				    AD5758_DCDC_CONFIG2_ILIMIT_MODE(ilimit));

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}

	/*
	 * Poll the BUSY_3WI bit in the DCDC_CONFIG2 register until it is 0.
	 * This allows the 3-wire interface communication to complete.
	 */
	do {
		ad5758_spi_reg_read(dev, AD5758_REG_DCDC_CONFIG2, &reg_data);
	} while (reg_data & AD5758_DCDC_CONFIG2_BUSY_3WI_MSK);
	dev->dc_dc_ilimit = ilimit;

	return 0;
}

/**
 * Enable/disable Enable Internal Buffers.
 * @param dev - The device structure.
 * @param enable - enable or disable
 * Accepted values: 0: disable
 * 		    1: enable
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_internal_buffers_en(struct ad5758_dev *dev, uint8_t enable)
{
	int32_t ret;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_DAC_CONFIG,
				    AD5758_DAC_CONFIG_INT_EN_MSK,
				    AD5758_DAC_CONFIG_INT_EN_MODE(enable));
	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}

	/* Wait to allow time for the internal calibrations to complete */
	return ad5758_wait_for_refresh_cycle(dev);
}

/**
 * Select Output Range.
 * @param dev - The device structure.
 * @param range - output range
 * Accepted values: RANGE_0V_5V
 *		    RANGE_0V_10V
 *		    RANGE_M5V_5V
 *		    RANGE_M10V_10V
 *		    RANGE_0mA_20mA
 *		    RANGE_0mA_24mA
 *		    RANGE_4mA_24mA
 *		    RANGE_M20mA_20mA
 *		    RANGE_M24mA_24mA
 *		    RANGE_M1mA_22mA
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_set_out_range(struct ad5758_dev *dev,
			     enum ad5758_output_range range)
{
	int32_t ret;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_DAC_CONFIG,
				    AD5758_DAC_CONFIG_RANGE_MSK,
				    AD5758_DAC_CONFIG_RANGE_MODE(range));

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}
	dev->output_range = range;

	/* Modifying the RANGE bits in the DAC_CONFIG register also initiates
	 * a calibration memory refresh and, therefore, a subsequent SPI write
	 * must not be performed until the CAL_MEM_UNREFRESHED bit in the
	 * DIGITAL_DIAG_RESULTS register returns to 0.
	 */
	return ad5758_wait_for_refresh_cycle(dev);
}

/**
 * Configure the slew rate by setting the clock and enable/disable the control
 * @param dev - The device structure.
 * @param clk - Slew rate clock.
 * Accepted values: SR_CLOCK_240_KHZ
 *		    SR_CLOCK_200_KHZ
 *		    SR_CLOCK_150_KHZ
 *		    SR_CLOCK_128_KHZ
 *		    SR_CLOCK_64_KHZ
 *		    SR_CLOCK_32_KHZ
 *		    SR_CLOCK_16_KHZ
 *		    SR_CLOCK_8_KHZ
 *		    SR_CLOCK_4_KHZ
 *		    SR_CLOCK_2_KHZ
 *		    SR_CLOCK_1_KHZ
 *		    SR_CLOCK_512_HZ
 *		    SR_CLOCK_256_HZ
 *		    SR_CLOCK_128_HZ
 *		    SR_CLOCK_64_HZ
 *		    SR_CLOCK_16_HZ
 * @param enable - enable or disable the sr coontrol
 * Accepted values: 0: disable
 * 		    1: enable
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_slew_rate_config(struct ad5758_dev *dev,
				enum ad5758_slew_rate_clk clk,
				uint8_t enable)
{
	int32_t ret;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_DAC_CONFIG,
				    AD5758_DAC_CONFIG_SR_EN_MSK,
				    AD5758_DAC_CONFIG_SR_EN_MODE(enable));
	if(ret)
		goto error;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_DAC_CONFIG,
				    AD5758_DAC_CONFIG_SR_CLOCK_MSK,
				    AD5758_DAC_CONFIG_SR_CLOCK_MODE(clk));

	if (ret < 0)
		goto error;

	dev->slew_rate_clk = clk;

	/* Wait to allow time for the internal calibrations to complete */
	return ad5758_wait_for_refresh_cycle(dev);

error:
	pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
	return -1;
}

/**
 * Write DAC data to the input register
 * @param dev - The device structure.
 * @param code - DAC input data of 16 bits
 * Accepted values: 0x00 to 0xFFFF
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_dac_input_write(struct ad5758_dev *dev, uint16_t code)
{
	int32_t ret;

	ret = ad5758_spi_reg_write(dev, AD5758_REG_DAC_INPUT, code);

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}

	return 0;
}

/**
 * Enable/disable VIOUT.
 * @param dev - The device structure.
 * @param enable - enable or disable VIOUT output
 * Accepted values: 0: disable
 * 		    1: enable
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_dac_output_en(struct ad5758_dev *dev, uint8_t enable)
{
	int32_t ret;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_DAC_CONFIG,
				    AD5758_DAC_CONFIG_OUT_EN_MSK,
				    AD5758_DAC_CONFIG_OUT_EN_MODE(enable));

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}
	no_os_mdelay(1);

	return 0;
}

/**
 * Clear the error flags for the on-chip digital diagnostic features
 * @param dev - The device structure.
 * @param flag - which flag to clear
 * Accepted values: DIAG_SPI_CRC_ERR
 * 		    DIAG_SLIPBIT_ERR
 * 		    DIAG_SCLK_COUNT_ERR
 * 		    DIAG_INVALID_SPI_ACCESS_ERR
 * 		    DIAG_CAL_MEM_CRC_ERR
 * 		    DIAG_INVERSE_DAC_CHECK_ERR
 * 		    DIAG_DAC_LATCH_MON_ERR
 * 		    DIAG_THREE_WI_RC_ERR
 * 		    DIAG_WDT_ERR
 * 		    DIAG_ERR_3WI
 * 		    DIAG_RESET_OCCURRED
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_clear_dig_diag_flag(struct ad5758_dev *dev,
				   enum ad5758_dig_diag_flags flag)
{
	int32_t ret;

	/*
	 * Flags require a 1 to be written to them to update them to their
	 * current value
	 */
	ret = ad5758_spi_write_mask(dev, AD5758_REG_DIGITAL_DIAG_RESULTS,
				    NO_OS_BIT(flag), NO_OS_BIT(flag));

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}

	return 0;
}

/**
 * Configure CLKOUT by setting the frequency and enabling/disabling the option
 * @param dev - The device structure.
 * @param config - Enable or disable
 * Accepted values: CLKOUT_DISABLE
 * 		    CLKOUT_ENABLE
 * @param freq - configure the frequency of CLKOUT.
 * Accepted values: CLKOUT_FREQ_416_KHZ
 * 		    CLKOUT_FREQ_435_KHZ
 * 		    CLKOUT_FREQ_454_KHZ
 * 		    CLKOUT_FREQ_476_KHZ
 * 		    CLKOUT_FREQ_500_KHZ
 * 		    CLKOUT_FREQ_526_KHZ
 * 		    CLKOUT_FREQ_555_KHZ
 * 		    CLKOUT_FREQ_588_KHZ
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_set_clkout_config(struct ad5758_dev *dev,
				 enum ad5758_clkout_config config,
				 enum ad5758_clkout_freq freq)
{
	int32_t ret;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_GP_CONFIG1,
				    AD5758_GP_CONFIG1_CLKOUT_FREQ_MSK,
				    AD5758_GP_CONFIG1_CLKOUT_FREQ_MODE(freq));
	if(ret < 0)
		goto error;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_GP_CONFIG1,
				    AD5758_GP_CONFIG1_CLKOUT_CONFIG_MSK,
				    AD5758_GP_CONFIG1_CLKOUT_CONFIG_MODE(config));
	if(ret < 0)
		goto error;

	dev->clkout_config = config;
	dev->clkout_freq = freq;

	return 0;

error:
	pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
	return -1;
}

/**
 * Select which node to multiplex to the ADC.
 * @param dev - The device structure.
 * @param adc_ip_sel - diagnostic select
 * Accepted values: ADC_IP_MAIN_DIE_TEMP
 * 		    ADC_IP_DCDC_DIE_TEMP
 * 		    ADC_IP_REFIN
 * 		    ADC_IP_REF2
 * 		    ADC_IP_VSENSE
 * 		    ADC_IP_MVSENSE
 * 		    ADC_IP_INT_AVCC
 * 		    ADC_IP_REGOUT
 * 		    ADC_IP_VLOGIC
 * 		    ADC_IP_INT_CURR_MON_VOUT
 * 		    ADC_IP_REFGND
 * 		    ADC_IP_AGND
 * 		    ADC_IP_DGND
 * 		    ADC_IP_VDPC
 * 		    ADC_IP_AVDD2
 * 		    ADC_IP_AVSS
 * 		    ADC_IP_DCDC_DIE_NODE
 * 		    ADC_IP_REFOUT
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_select_adc_ip(struct ad5758_dev *dev,
			     enum ad5758_adc_ip adc_ip_sel)
{
	int32_t ret;

	ret = ad5758_spi_write_mask(dev, AD5758_REG_ADC_CONFIG,
				    AD5758_ADC_CONFIG_ADC_IP_SELECT_MSK,
				    AD5758_ADC_CONFIG_ADC_IP_SELECT_MODE(adc_ip_sel));

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}

	return 0;
}

/**
 * Set depth of the sequencer.
 * @param dev - The device structure.
 * @param num_of_channels - depth of the sequencer 1 to 8 channels
 * Accepted values: 1 channel, 2 channels ... 8 channels
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_select_adc_depth(struct ad5758_dev *dev,
				uint8_t num_of_channels)
{
	int32_t ret = -1;

	if ((num_of_channels == 0) || (num_of_channels > 8)) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);

		return ret;
	}

	num_of_channels -= 1;
	ret = ad5758_spi_reg_write(dev, AD5758_REG_ADC_CONFIG,
				   AD5758_ADC_CONFIG_SEQUENCE_DATA_MODE(num_of_channels));

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return ret;
	}

	return 0;
}

/**
 * Load the desired channel into the sequencer with the adc input
 * @param dev - The device structure.
 * @param channel - Desired channel
 * Accepted values: 0 = channel 1, 1 = channel 2... 7 = channel 8
 * @param adc_ip_sel - diagnostic select
 * Accepted values: ADC_IP_MAIN_DIE_TEMP
 * 		    ADC_IP_DCDC_DIE_TEMP
 * 		    ADC_IP_REFIN
 * 		    ADC_IP_REF2
 * 		    ADC_IP_VSENSE
 * 		    ADC_IP_MVSENSE
 * 		    ADC_IP_INT_AVCC
 * 		    ADC_IP_REGOUT
 * 		    ADC_IP_VLOGIC
 * 		    ADC_IP_INT_CURR_MON_VOUT
 * 		    ADC_IP_REFGND
 * 		    ADC_IP_AGND
 * 		    ADC_IP_DGND
 * 		    ADC_IP_VDPC
 * 		    ADC_IP_AVDD2
 * 		    ADC_IP_AVSS
 * 		    ADC_IP_DCDC_DIE_NODE
 * 		    ADC_IP_REFOUT
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_set_adc_channel_input(struct ad5758_dev *dev,
				     uint8_t channel,
				     enum ad5758_adc_ip adc_ip_sel)
{
	uint16_t cmd;
	int32_t ret;

	cmd = (AD5758_ADC_CONFIG_SEQUENCE_COMMAND_MODE(0x01) |
	       AD5758_ADC_CONFIG_SEQUENCE_DATA_MODE(channel) |
	       AD5758_ADC_CONFIG_ADC_IP_SELECT_MODE(adc_ip_sel));

	ret = ad5758_spi_reg_write(dev, AD5758_REG_ADC_CONFIG, cmd);

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}

	return 0;
}

/**
 * Configure the ADC into one of four modes of operation
 * @param dev - The device structure.
 * @param adc_mode - ADC mode of operation
 * Accepted values: ADC_MODE_KEY_SEQ
 * 		    ADC_MODE_AUTO_SEQ
 * 		    ADC_MODE_SINGLE_CONV
 * 		    ADC_MODE_SINGLE_KEY_CONV
 * @param enable - enable or disable the selected mode
 * Accepted values: 0: disable
 * 		    1: enable
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_set_adc_mode(struct ad5758_dev *dev,
			    enum ad5758_adc_mode adc_mode,
			    uint8_t enable)
{
	uint16_t cmd;
	int32_t ret;

	cmd = (AD5758_ADC_CONFIG_SEQUENCE_COMMAND_MODE(adc_mode) |
	       AD5758_ADC_CONFIG_SEQUENCE_DATA_MODE(enable));

	ret = ad5758_spi_reg_write(dev, AD5758_REG_ADC_CONFIG, cmd);

	if (ret < 0) {
		pr_err("%s: Failed with code: %"PRIi32".\n", __func__, ret);
		return -1;
	}

	return 0;
}

/**
 * Set up the device from power-on or reset condition with the correct
 * programming sequence to enable the output
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 			parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5758_init(struct ad5758_dev **device,
		    struct ad5758_init_param *init_param)
{
	struct ad5758_dev *dev;
	int32_t ret;

	dev = (struct ad5758_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->crc_en = true;

	/* Initialize the SPI communication */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if(ret)
		goto error_init;

	/* GPIO */
	ret = no_os_gpio_get(&dev->reset_n, &init_param->reset_n);
	if(ret)
		goto error_init;
	ret = no_os_gpio_get(&dev->ldac_n, &init_param->ldac_n);
	if(ret)
		goto error_gpio_ldac;

	/* Get the DAC out of reset */
	ret = no_os_gpio_direction_output(dev->reset_n, NO_OS_GPIO_HIGH);
	if(ret)
		goto err;

	/* Tie the LDAC pin low */
	ret = no_os_gpio_direction_output(dev->ldac_n, NO_OS_GPIO_LOW);
	if(ret)
		goto err;

	/* Perform a software reset */
	ret = ad5758_soft_reset(dev);
	if(ret)
		goto err;

	/* Perform a calibration memory refresh */
	ret = ad5758_calib_mem_refresh(dev);
	if(ret)
		goto err;

	/* Clear the RESET_OCCURRED flag */
	ret = ad5758_clear_dig_diag_flag(dev, DIAG_RESET_OCCURRED);
	if(ret)
		goto err;

	/* Configure CLKOUT before enabling the dc-to-dc converter */
	ret = ad5758_set_clkout_config(dev, init_param->clkout_config,
				       init_param->clkout_freq);
	if(ret)
		goto err;

	/* Set the dc-to-dc current limit */
	ret = ad5758_set_dc_dc_ilimit(dev, init_param->dc_dc_ilimit);
	if(ret)
		goto err;

	/* Set up the dc-to-dc converter mode */
	ret = ad5758_set_dc_dc_conv_mode(dev, init_param->dc_dc_mode);
	if(ret)
		goto err;

	/* Power up the DAC and internal (INT) amplifiers */
	ret = ad5758_internal_buffers_en(dev, 1);
	if(ret)
		goto err;

	/* Configure the output range */
	ret = ad5758_set_out_range(dev, init_param->output_range);
	if(ret)
		goto err;

	/* Enable Slew Rate Control and set the slew rate clock */
	ret = ad5758_slew_rate_config(dev, init_param->slew_rate_clk, 1);
	if(ret)
		goto err;

	/* Enable VIOUT */
	ret = ad5758_dac_output_en(dev, 1);
	if(ret)
		goto err;

	ret = ad5758_set_crc(dev, init_param->crc_en);
	if(ret)
		goto err;

	*device = dev;
	pr_info("ad5758 successfully initialized\n");
	no_os_mdelay(1000);

	return 0;

err:
	no_os_gpio_remove(dev->ldac_n);
error_gpio_ldac:
	no_os_gpio_remove(dev->reset_n);
error_init:
	pr_err("ad5758 could not be initialized\n");
	no_os_free(dev);
	return ret;
}
