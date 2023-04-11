/***************************************************************************//**
 *   @file   projects/adf5902_sdz/src/adf5902_sdz.c
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
#include <xil_cache.h>
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "no_os_error.h"
#include "adf5902.h"
#include "parameters.h"
#include "no_os_util.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_adf5902.h"
#endif

#define NO_OS_LOG_LEVEL 6
#include "no_os_print_log.h"

int main(void)
{
	int32_t ret;
	struct adf5902_dev *dev;
	float temperature = 0;
	uint64_t freq = 0;

	struct xil_spi_init_param xil_spi_init = {
		.flags = 0,
		.type = SPI_PS
	};

	struct xil_gpio_init_param xil_gpio_init = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS
	};

	struct no_os_gpio_init_param gpio_ce_param = {
		.number = GPIO_CE,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_init
	};

	struct no_os_spi_init_param spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 2000000,
		.chip_select = SPI_ADF5902_CS,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_init
	};

	struct slope slopes_param[ADF5902_MAX_SLOPE_NO] = {
		{
			.dev_word = 455,
			.dev_offset = 9,
			.step_word = 144
		},
		{
			.dev_word = -7280,
			.dev_offset = 9,
			.step_word = 9
		},
		{
			.dev_word = 455,
			.dev_offset = 9,
			.step_word = 144
		},
		{
			.dev_word = -7280,
			.dev_offset = 9,
			.step_word = 9
		},
	};

	struct adf5902_init_param adf5902_param = {

		/* SPI Initialization Structure */
		.spi_init = &spi_init,

		/* GPIO Chip Enable Initialization Structure */
		.gpio_ce_param = &gpio_ce_param,

		/* Reference Frequency (Hz) */
		.ref_in = 100000000,

		/* VCO Frequency (Hz) */
		.rf_out = 24025000000,

		/* Reference Doubler */
		.ref_doubler_en = ADF5902_REF_DOUBLER_DISABLE,

		/* Ref / 2 */
		.ref_div2_en= ADF5902_R_DIV_2_DISABLE,

		/* ADC Average Output */
		.adc_avg = ADF5902_ADC_AVG_1,

		/* Transmitter Amplitude Calibration Reference Code */
		.tx_amp_cal_ref = ADF5902_TX_AMP_CAL_MAX_REF_CODE,

		/* Ramp Delay Enable/Disable */
		.ramp_delay_en = ADF5902_RAMP_DEL_DISABLE,

		/* TX Data Trigger */
		.tx_trig_en = ADF5902_TX_DATA_TRIG_DISABLE,

		/* Delay Words Number */
		.delay_words_no = ADF5902_MAX_DELAY_WORD_NO,

		/* Delay Words */
		.delay_wd = {0, 0, 0, 0},

		/* Number of Slopes*/
		.slopes_no = ADF5902_MAX_SLOPE_NO,

		/* Slopes Initialization Parameters (DEV/DEVoff/Steps) */
		.slopes = slopes_param,

		/* TX Ramp CLK */
		.tx_ramp_clk = ADF5902_TX_RAMP_CLK_DIV,

		/* TX Data invert */
		.tx_data_invert = AD5902_TX_DATA_INV_DISABLE,

		/* Ramp status */
		.ramp_status = ADF5902_RAMP_DOWN_TO_MUXOUT,

		/*Ramp  CLK1 */
		.clk1_div_ramp = 10,

		/* Number of Ramp Timing Parameters */
		.clk2_div_no = ADF5902_MAX_CLK2_DIV_NO,

		/* Timing Parameters Values */
		.clk2_div = {10, 10, 10, 10},

		/* Latch Enable Select */
		.le_sel = ADF5902_LE_FROM_PIN,

		/* Clock Divider Mode */
		.clk_div_mode = ADF5902_RAMP_DIV,

		/* Charge Pump Current */
		.cp_current = ADF5902_CP_CURRENT_2MA24,

		/* Charge Pump Tristate */
		.cp_tristate_en = ADF5902_CP_TRISTATE_DISABLE,

		/* Ramp Mode */
		.ramp_mode = ADF5902_CONTINUOUS_TRIANGULAR
	};

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	ret = adf5902_init(&dev, &adf5902_param);
	if (ret != 0) {
		pr_err("ADF5902 Initialization failed!\n");
		return -1;
	}

	ret = adf5902_recalibrate(dev);
	if (ret != 0) {
		pr_err("ADF5902 Recalibration failed!\n");
		return -1;
	}

	pr_info("ADF5902 Successfully initialized!\n");

	ret = adf5902f_compute_frequency(dev, &freq);
	if (ret != 0) {
		pr_err("ADF5902 Output Frequency not locked!\n");
		return -1;
	}

	pr_info("ADF5902 Locked Frequency: %llu Hz\n", freq);

	ret = adf5902_read_temp(dev, &temperature);
	if (ret != 0) {
		pr_err("ADF5902 Temperature read failed!\n");
		return -1;
	}

	pr_info("ADF5902 Temperature value: %.2f degC \n", temperature);

#ifdef IIO_SUPPORT
	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adf5902_dev", dev, &adf5902_iio_descriptor,
			       NULL, NULL, NULL),
	};
	return iio_app_run(NULL, 0, devices, NO_OS_ARRAY_SIZE(devices));
#endif

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	ret = adf5902_remove(dev);

	return ret;
}
