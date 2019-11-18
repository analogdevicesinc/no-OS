/***************************************************************************//**
 *   @file   main.c
 *   @brief  Implementation of Main Function.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include "hmc7044.h"
#include "ad9208.h"
#include "axi_adxcvr.h"
#include "axi_adc_core.h"
#include "axi_jesd204_rx.h"
#include "axi_dmac.h"
#include "parameters.h"
#include "xil_printf.h"
#include <xparameters.h>
#include "xilinx_platform_drivers.h"
#include "spi.h"
#include "spi_extra.h"
#include "error.h"
#include "delay.h"

int main(void)
{
	struct xil_spi_init_param xil_spi_param = {	
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.device_id = SPI_DEVICE_ID, 
		.flags = 0};

	struct spi_init_param hmc7044_spi_param = {
		.max_speed_hz = 10000000,
		.mode = SPI_MODE_0,
		.chip_select = SPI_HMC7044_CS,
		.extra = &xil_spi_param
	};

	struct hmc7044_chan_spec chan_spec[8] = {
		/* ADC_B_CLK */
		{
			.disable = 0, .num = 0, .divider = 1, .driver_mode = 1,
			.start_up_mode_dynamic_enable = false,
			.high_performance_mode_dis = false
		},
		/* ADC_B_SYSREF */
		{
			.disable = 0, .num = 1, .divider = 512, .driver_mode = 2,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true
		},
		/* ADC_A_CLK */
		{
			.disable = 0, .num = 2,  .divider = 1, .driver_mode = 1,
			.start_up_mode_dynamic_enable = false,
			.high_performance_mode_dis = false
		},
		/* ADC_A_SYSREF */
		{
			.disable = 0, .num = 3,  .divider = 512, .driver_mode = 2,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true
		},
		/* FMC_REFCLK0 */
		{
			.disable = 0, .num = 8,  .divider = 4, .driver_mode = 1,
			.start_up_mode_dynamic_enable = false,
			.high_performance_mode_dis = false
		},
		/* FMC_REFCLK1 */
		{
			.disable = 0, .num = 9,  .divider = 8, .driver_mode = 1,
			.start_up_mode_dynamic_enable = false,
			.high_performance_mode_dis = false
		},
		/* FMC_0_SYSREF */
		{
			.disable = 0, .num = 10, .divider = 512, .driver_mode = 2,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true
		},
		/* FMC_1_SYSREF */
		{
			.disable = 0, .num = 11, .divider = 512, .driver_mode = 2,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true
		},
	};

	struct hmc7044_init_param hmc7044_param = {
		.spi_init = &hmc7044_spi_param,
		.clkin_freq = {125000000, 0, 0, 0},
		.vcxo_freq = 125000000,
		.pll2_freq = 3000000000,
		.pll1_loop_bw = 200,
		.sysref_timer_div = 1024,
		.pulse_gen_mode = 7, /* HMC7044_PULSE_GEN_CONT_PULSE */
		.in_buf_mode = {0x07, 0, 0, 0, 0x03},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x00},
		.gpo_ctrl = {0x1f, 0x2b, 0x00, 0x00},
		.num_channels = 8,
		.channels = chan_spec,
	};

	struct spi_init_param ad9208_0_spi_param = {
		.max_speed_hz = 10000000,
		.mode = SPI_MODE_3,
		.chip_select = SPI_AD9208_0_CS,
		.extra = &xil_spi_param
	};

	struct spi_init_param ad9208_1_spi_param = {
		.max_speed_hz = 10000000,
		.mode = SPI_MODE_3,
		.chip_select = SPI_AD9208_CS,
		.extra = &xil_spi_param
	};

	struct ad9208_ddc ad9208_0_ddc_init[1] = {
		{
			.decimation = 2,
			.nco_mode = AD9208_NCO_MODE_VIF,
			.carrier_freq_hz = 70000000,
			.po = 0,
			.gain_db = true,
		},
	};

	struct ad9208_ddc ad9208_1_ddc_init[1] = {
		{
			.decimation = 2,
			.nco_mode = AD9208_NCO_MODE_VIF,
			.carrier_freq_hz = 70000000,
			.po = 0,
			.gain_db = true,
		},
	};

	jesd_param_t jesd_param_init = {
		.jesd_F = 1, /* octets per frame */
		.jesd_K = 32, /* frames per multiframe */
		.jesd_HD = false, /* high density */
		.jesd_N = 16, /* converter resolution */
		.jesd_NP = 16, /* bits per sample */
		.jesd_M = 2, /* converters per device */
		.jesd_CS = 0, /* control bits per sample */
		.jesd_L = 8, /* lanes per device */
	};

	struct adxcvr_init rx_0_adxcvr_init = {
		.name = "rx_0_adxcvr",
		.base = RX_0_XCVR_BASEADDR,
		.sys_clk_sel = 3,
		.out_clk_sel = 3,
		.cpll_enable = 0,
		.lpm_enable = 1,
		/* LaneRate = (M/L)*NP*(10/8)*DataRate */
		.lane_rate_khz = 15000000,
		/* FMC_REFCLK0, output 8 of HMC 7044 */
		.ref_rate_khz = 750000,
	};

	struct adxcvr_init rx_1_adxcvr_init = {
		.name = "rx_1_adxcvr",
		.base = RX_1_XCVR_BASEADDR,
		.sys_clk_sel = 3,
		.out_clk_sel = 3,
		.cpll_enable = 0,
		.lpm_enable = 1,
		/* LaneRate = (M/L)*NP*(10/8)*DataRate */
		.lane_rate_khz = 15000000,
		/* FMC_REFCLK0, output 8 of HMC 7044 */
		.ref_rate_khz = 750000,
	};

	struct jesd204_rx_init rx_0_jesd_init = {
		.name = "rx_0_jesd",
		.base = RX_0_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.subclass = 1,
		/* (lane_clk_khz / 40) output 9 of HMC 7044*/
		.device_clk_khz = 375000,
		/* LaneRate = (M/L)*NP*(10/8)*DataRate */
		.lane_clk_khz = 15000000,
	};

	struct jesd204_rx_init rx_1_jesd_init = {
		.name = "rx_1_jesd",
		.base = RX_1_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.subclass = 1,
		/* (lane_clk_khz / 40) output 9 of HMC 7044*/
		.device_clk_khz = 375000,
		/* LaneRate = (M/L)*NP*(10/8)*DataRate */
		.lane_clk_khz = 15000000,
	};

	struct axi_adc_init rx_0_adc_init = {
		.name = "rx_0_adc",
		.base = RX_0_CORE_BASEADDR,
		.num_channels = 2,
	};

	struct axi_adc_init rx_1_adc_init = {
		.name = "rx_1_adc",
		.base = RX_1_CORE_BASEADDR,
		.num_channels = 2,
	};

	struct axi_dmac_init rx_dmac_init = {
		.name = "rx_dmac",
		.base = RX_DMA_BASEADDR,
		.direction = DMA_DEV_TO_MEM,
		.flags = 0,
	};

	struct ad9208_init_param ad9208_0_param = {
		.spi_init = &ad9208_0_spi_param,
		.gpio_powerdown = 54 + 34, /* adc0_pwdn */
		.sampling_frequency_hz = 3000000000,
		.input_div = 1,
		.gpio_powerdown = false,
		.powerdown_mode = AD9208_POWERDOWN, /* Full Powerdown Mode */
		/* duty cycle stabilizer enable */
		.duty_cycle_stabilizer_en = true,
		.current_scale = AD9208_ADC_SCALE_1P7_VPP,
		/* analog input dc coupling enable */
		.analog_input_mode = COUPLING_AC,
		.ext_vref_en = false,
		/* analog input neg buffer current */
		.buff_curr_n = AD9208_BUFF_CURR_600_UA,
		/* analog input pos buffer current */
		.buff_curr_p = AD9208_BUFF_CURR_600_UA,
		/* Number of Carrier Frequencies */
		.fc_ch = AD9208_FULL_BANDWIDTH_MODE,
		.ddc = ad9208_0_ddc_init,
		.ddc_cnt = ARRAY_SIZE(ad9208_0_ddc_init),
		/* ddc complex to real enable */
		.ddc_output_format_real_en = false,
		/* ddc mixer real enable */
		.ddc_input_format_real_en = false,
		.test_mode_ch0 = AD9208_TESTMODE_OFF,
		.test_mode_ch1 = AD9208_TESTMODE_RAMP,

		/* SYSREF Config */
		.sysref_lmfc_offset = 0,
		.sysref_edge_sel = false,
		.sysref_clk_edge_sel = false,
		.sysref_neg_window_skew = 0,
		.sysref_pos_window_skew = 0,
		.sysref_mode = AD9208_SYSREF_ONESHOT,
		.sysref_count = 15,
		.jesd_param = &jesd_param_init,
		.jesd_subclass = 1,
	};

	struct ad9208_init_param ad9208_1_param = {
		.spi_init = &ad9208_1_spi_param,
		.gpio_powerdown = 54 + 33, /* adc1_pwdn */
		.sampling_frequency_hz = 3000000000,
		.input_div = 1,
		.gpio_powerdown = false,
		.powerdown_mode = AD9208_POWERDOWN, /* Full Powerdown Mode */
		.duty_cycle_stabilizer_en = true,
		/* duty cycle stabilizer enable */
		.current_scale = AD9208_ADC_SCALE_1P7_VPP,
		/* analog input dc coupling enable */
		.analog_input_mode = COUPLING_AC,
		.ext_vref_en = false,
		/* analog input neg buffer current */
		.buff_curr_n = AD9208_BUFF_CURR_600_UA,
		/* analog input pos buffer current */
		.buff_curr_p = AD9208_BUFF_CURR_600_UA,
		/* Number of Carrier Frequencies */
		.fc_ch = AD9208_FULL_BANDWIDTH_MODE,
		.ddc = ad9208_1_ddc_init,
		.ddc_cnt = ARRAY_SIZE(ad9208_1_ddc_init),
		/* ddc complex to real enable */
		.ddc_output_format_real_en = false,
		/* ddc mixer real enable */
		.ddc_input_format_real_en = false,
		.test_mode_ch0 = AD9208_TESTMODE_OFF,
		.test_mode_ch1 = AD9208_TESTMODE_ONE_ZERO_TOGGLE,

		/* SYSREF Config */
		.sysref_lmfc_offset = 0,
		.sysref_edge_sel = false,
		.sysref_clk_edge_sel = false,
		.sysref_neg_window_skew = 0,
		.sysref_pos_window_skew = 0,
		.sysref_mode = AD9208_SYSREF_ONESHOT,
		.sysref_count = 15,
		.jesd_param = &jesd_param_init,
		.jesd_subclass = 1,
	};

	struct hmc7044_dev *hmc7044_device;
	struct ad9208_dev *ad9208_0_device, *ad9208_1_device;
	struct axi_jesd204_rx *rx_0_jesd, *rx_1_jesd;
	struct adxcvr *rx_0_adxcvr, *rx_1_adxcvr;
	struct axi_adc *rx_0_adc, *rx_1_adc;
	struct axi_dmac *rx_dmac;
	int32_t status;
	uint32_t size;

	status = hmc7044_init(&hmc7044_device, &hmc7044_param);
	if (status != SUCCESS) {
		xil_printf("hmc7044_init() error: %"PRIi32"\n", status);
		return status;
	}

	status = ad9208_initialize(&ad9208_0_device, &ad9208_0_param);
	if (status != SUCCESS) {
		xil_printf("ad9208_0_init() error: %"PRIi32"\n", status);
		goto error_1;
	}

	status = ad9208_remove(ad9208_0_device);
	if (status != SUCCESS) {
		xil_printf("ad9208_remove() error: %"PRIi32"\n", status);
		goto error_1;
	}

	mdelay(100);

	status = ad9208_initialize(&ad9208_1_device, &ad9208_1_param);
	if (status != SUCCESS) {
		xil_printf("ad9208_1_init() error: %"PRIi32"\n", status);
		goto error_1;
	}

	status = ad9208_remove(ad9208_1_device);
	if (status != SUCCESS) {
		xil_printf("ad9208_remove() error: %"PRIi32"\n", status);
		goto error_1;
	}

	status = adxcvr_init(&rx_0_adxcvr, &rx_0_adxcvr_init);
	if (status != SUCCESS) {
		xil_printf("error: %s: adxcvr_init() failed\n",
			   rx_0_adxcvr_init.name);
		goto error_1;
	}

	status = adxcvr_clk_enable(rx_0_adxcvr);
	if (status != SUCCESS) {
		xil_printf("error: %s: adxcvr_clk_enable() failed\n",
			   rx_0_adxcvr->name);
		goto error_2;
	}

	status = axi_jesd204_rx_init(&rx_0_jesd, &rx_0_jesd_init);
	if (status != SUCCESS) {
		xil_printf("error: %s: axi_jesd204_rx_init() failed\n",
			   rx_0_jesd_init.name);
		goto error_3;
	}

	status = axi_jesd204_rx_lane_clk_enable(rx_0_jesd);
	if (status != SUCCESS) {
		xil_printf("error: %s: axi_jesd204_rx_lane_clk_enable() err\n",
			   rx_0_jesd->name);
		goto error_4;
	}

	status = axi_jesd204_rx_status_read(rx_0_jesd);
	if (status != SUCCESS) {
		xil_printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n",
			   status);
		goto error_5;
	}

	/* Initialize the ADC core */
	status = axi_adc_init(&rx_0_adc, &rx_0_adc_init);
	if (status != SUCCESS) {
		xil_printf("axi_adc_init() error: %"PRIi32"\n", status);
		goto error_5;
	}

	status = adxcvr_init(&rx_1_adxcvr, &rx_1_adxcvr_init);
	if (status != SUCCESS) {
		xil_printf("error: %s: adxcvr_init() failed\n",
			   rx_1_adxcvr_init.name);
		goto error_6;
	}

	status = adxcvr_clk_enable(rx_1_adxcvr);
	if (status != SUCCESS) {
		xil_printf("error: %s: adxcvr_clk_enable() failed\n",
			   rx_1_adxcvr->name);
		goto error_7;
	}

	status = axi_jesd204_rx_init(&rx_1_jesd, &rx_1_jesd_init);
	if (status != SUCCESS) {
		xil_printf("error: %s: axi_jesd204_rx_init() failed\n",
			   rx_1_jesd_init.name);
		goto error_8;
	}

	status = axi_jesd204_rx_lane_clk_enable(rx_1_jesd);
	if (status != SUCCESS) {
		xil_printf("error: %s: axi_jesd204_rx_lane_clk_enable() err\n",
			   rx_1_jesd->name);
		goto error_9;
	}

	status = axi_jesd204_rx_status_read(rx_1_jesd);
	if (status != SUCCESS) {
		xil_printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n",
			   status);
		goto error_10;
	}

	/* Initialize the ADC core */
	status = axi_adc_init(&rx_1_adc, &rx_1_adc_init);
	if (status != SUCCESS) {
		xil_printf("axi_adc_init() error: %"PRIi32"\n", status);
		goto error_10;
	}

	/* Initialize the DMAC and transfer 16384 samples from ADC to MEM */
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status != SUCCESS) {
		xil_printf("axi_dmac_init() error: %"PRIi32"\n", status);
		goto error_11;
	}

	size = (rx_1_adc->num_channels + rx_0_adc->num_channels) * sizeof(uint16_t);
	status = axi_dmac_transfer(rx_dmac,
				   DDR_MEM_BASEADDR + 0x800000, 16384 * size);
	if (status != SUCCESS)
		xil_printf("axi_dmac_transfer() error: %"PRIi32"\n", status);

error_11:
	axi_adc_remove(rx_1_adc);
error_10:
	axi_jesd204_rx_lane_clk_disable(rx_1_jesd);
error_9:
	axi_jesd204_rx_remove(rx_1_jesd);
error_8:
	adxcvr_clk_disable(rx_1_adxcvr);
error_7:
	adxcvr_remove(rx_1_adxcvr);
error_6:
	axi_adc_remove(rx_0_adc);
error_5:
	axi_jesd204_rx_lane_clk_disable(rx_0_jesd);
error_4:
	axi_jesd204_rx_remove(rx_0_jesd);
error_3:
	adxcvr_clk_disable(rx_0_adxcvr);
error_2:
	adxcvr_remove(rx_0_adxcvr);
error_1:
	hmc7044_remove(hmc7044_device);

	xil_printf("Bye\n");

	return 0;
}
