/***************************************************************************//**
 *   @file   main.c
 *   @brief  Implementation of Main Function.
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
#include <xparameters.h>
#include "spi.h"
#include "spi_extra.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "error.h"
#include "adf5902.h"
#include "parameters.h"

int main(void)
{
	int32_t ret;
	struct adf5902_dev *dev;
	float temperature = 0;

	struct xil_spi_init_param xil_spi_init = {
		.device_id = SPI_DEVICE_ID,
		.flags = 0,
		.type = SPI_PS
	};

	struct xil_gpio_init_param xil_gpio_init = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS
	};

	struct gpio_init_param gpio_ce_param = {
		.number = GPIO_CE,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_init
	};


	struct spi_init_param spi_init = {
		.max_speed_hz = 2000000,
		.chip_select = SPI_ADF5902_CS,
		.mode = SPI_MODE_0,
		.platform_ops = &xil_platform_ops,
		.extra = &xil_spi_init
	};

	struct freq_dev freq_deviation[ADF5902_MAX_FREQ_DEV_NO] = {
		{
			.dev_word = 455,
			.dev_offset = 9
		},
		{
			.dev_word = -1820,
			.dev_offset = 9
		},
		{
			.dev_word = 455,
			.dev_offset = 9
		},
		{
			.dev_word = -1820,
			.dev_offset = 9
		},
	};

	struct adf5902_init_param adf5902_param = {
		.spi_init = &spi_init,
		.gpio_ce_param = &gpio_ce_param,
		.ref_in = 100000000,
		.rf_out = 24125000000,
		.ref_div_factor = ADF5902_MIN_R_DIVIDER,
		.ref_doubler_en = ADF5902_REF_DOUBLER_DISABLE,
		.ref_div2_en= ADF5902_R_DIV_2_ENABLE,
		.adc_avg = ADF5902_ADC_AVG_1,
		.tx_amp_cal_ref = ADF5902_TX_AMP_CAL_MAX_REF_CODE,
		.ramp_delay_en = ADF5902_RAMP_DEL_DISABLE,
		.tx_trig_en = ADF5902_TX_DATA_TRIG_DISABLE,
		.delay_words_no = ADF5902_MAX_DELAY_WORD_NO,
		.delay_wd = {0, 0, 0, 0},
		.step_words_no = ADF5902_MAX_STEP_WORD_NO,
		.step_words = {144, 9, 144, 9},
		.freq_dev_no = ADF5902_MAX_FREQ_DEV_NO,
		.freq_dev = freq_deviation,
		.tx_ramp_clk = ADF5902_TX_RAMP_CLK_DIV,
		.tx_data_invert = AD5902_TX_DATA_INV_DISABLE,
		.clk2_div_no = ADF5902_MAX_CLK2_DIV_NO,
		.clk2_div = {10, 10, 10, 10},
		.le_sel = ADF5902_LE_FROM_PIN,
		.clk_div_mode = ADF5902_RAMP_DIV,
		.cp_current = ADF5902_CP_CURRENT_2MA24,
		.cp_tristate_en = ADF5902_CP_TRISTATE_DISABLE
	};

	ret = adf5902_init(&dev, &adf5902_param);
	if (ret != SUCCESS)
		return FAILURE;

	ret = adf5902_read_temp(dev, &temperature);
	if (ret != SUCCESS)
		return FAILURE;
	return SUCCESS;

}
