/***************************************************************************//**
 *   @file   main.c
 *   @brief  Implementation of Main Function.
 *   @author Cristian Pop (cristianpop@analog.com)
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
#include "hmc7044.h"
#include "ad9172.h"
#include "axi_adxcvr.h"
#include "axi_jesd204_tx.h"
#include "axi_dac_core.h"
#include "parameters.h"
#include "inttypes.h"
#include "autoconfig.h"
#define DAC_DMA_EXAMPLE
#ifdef DAC_DMA_EXAMPLE
#include "axi_dmac.h"
#endif /* DAC_DMA_EXAMPLE */

#define AD9172_MAX_MODES 22

jesd_param_t ad9172_modes[AD9172_MAX_MODES] = {
	{.jesd_L = 1, .jesd_M = 2, .jesd_F = 4, .jesd_S = 1, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 0 */
	{.jesd_L = 2, .jesd_M = 4, .jesd_F = 4, .jesd_S = 1, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 1 */
	{.jesd_L = 3, .jesd_M = 6, .jesd_F = 4, .jesd_S = 1, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 2 */
	{.jesd_L = 2, .jesd_M = 2, .jesd_F = 2, .jesd_S = 1, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 3 */
	{.jesd_L = 4, .jesd_M = 4, .jesd_F = 2, .jesd_S = 1, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 4 */
	{.jesd_L = 1, .jesd_M = 2, .jesd_F = 3, .jesd_S = 1, .jesd_NP = 12, .jesd_N = 12, .jesd_K = 32, .jesd_HD = 1}, /* mode 5 */
	{.jesd_L = 2, .jesd_M = 4, .jesd_F = 3, .jesd_S = 1, .jesd_NP = 12, .jesd_N = 12, .jesd_K = 32, .jesd_HD = 1}, /* mode 6 */
	{.jesd_L = 1, .jesd_M = 4, .jesd_F = 8, .jesd_S = 1, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 7 */
	{.jesd_L = 4, .jesd_M = 2, .jesd_F = 1, .jesd_S = 1, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 8 */
	{.jesd_L = 4, .jesd_M = 2, .jesd_F = 2, .jesd_S = 2, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 9 */
	{.jesd_L = 8, .jesd_M = 2, .jesd_F = 1, .jesd_S = 2, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 10 */
	{.jesd_L = 8, .jesd_M = 2, .jesd_F = 2, .jesd_S = 4, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 11 */
	{.jesd_L = 8, .jesd_M = 2, .jesd_F = 3, .jesd_S = 8, .jesd_NP = 12, .jesd_N = 12, .jesd_K = 32, .jesd_HD = 1}, /* mode 12 */
	{.jesd_L = 0, .jesd_M = 0, .jesd_F = 0, .jesd_S = 0, .jesd_NP = 0, .jesd_N = 0, .jesd_K = 32, .jesd_HD = 1}, /* mode 13 invalid */
	{.jesd_L = 0, .jesd_M = 0, .jesd_F = 0, .jesd_S = 0, .jesd_NP = 0, .jesd_N = 0, .jesd_K = 32, .jesd_HD = 1}, /* mode 14 invalid */
	{.jesd_L = 0, .jesd_M = 0, .jesd_F = 0, .jesd_S = 0, .jesd_NP = 0, .jesd_N = 0, .jesd_K = 32, .jesd_HD = 1}, /* mode 15 invalid */
	{.jesd_L = 0, .jesd_M = 0, .jesd_F = 0, .jesd_S = 0, .jesd_NP = 0, .jesd_N = 0, .jesd_K = 32, .jesd_HD = 1}, /* mode 16 invalid */
	{.jesd_L = 0, .jesd_M = 0, .jesd_F = 0, .jesd_S = 0, .jesd_NP = 0, .jesd_N = 0, .jesd_K = 32, .jesd_HD = 1}, /* mode 17 invalid */
	{.jesd_L = 4, .jesd_M = 1, .jesd_F = 1, .jesd_S = 2, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 18 */
	{.jesd_L = 4, .jesd_M = 1, .jesd_F = 2, .jesd_S = 4, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 19 */
	{.jesd_L = 8, .jesd_M = 1, .jesd_F = 1, .jesd_S = 4, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 20 */
	{.jesd_L = 8, .jesd_M = 1, .jesd_F = 2, .jesd_S = 8, .jesd_NP = 16, .jesd_N = 16, .jesd_K = 32, .jesd_HD = 1}, /* mode 21 */
};



int32_t ad9172_system_init(uint8_t mode) {

	struct spi_init_param hmc7044_spi_param = {
		.id = SPI_DEVICE_ID,
		.max_speed_hz = 10000000,
		.mode = SPI_MODE_0,
		.chip_select = SPI_HMC7044_CS,
		.flags = 0
	};

	struct hmc7044_chan_spec chan_spec[4] = {
		{.disable = 0, .num = 2, .divider = 25/* 8 */, .driver_mode = 1},		/* DAC_CLK */
		{.disable = 0, .num = 3, .divider = 512/* 2*64 */, .driver_mode = 1},		/* DAC_SYSREF */
		{.disable = 0, .num = 12, .divider = 5/* 8 */, .driver_mode = 2},		/* FPGA_CLK */
		{.disable = 0, .num = 13, .divider = 512/* 512 */, .driver_mode = 2},	/* FPGA_SYSREF */
	};

	struct hmc7044_init_param hmc7044_param = {
		.spi_init = &hmc7044_spi_param,
		.clkin_freq = {122880000, 0, 0, 0},
		.vcxo_freq = 122880000,
		.pll2_freq = 2304000000,
		.pll1_loop_bw = 200,
		.sysref_timer_div = 1024,
		.pulse_gen_mode = 0,
		.in_buf_mode = {0x15, 0, 0, 0, 0x15},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x00},
		.gpo_ctrl = {0x1f, 0x2b, 0x00, 0x00},
		.num_channels = 4,
		.channels = chan_spec,
	};

	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = ad9172_modes[mode].jesd_F,
		.frames_per_multiframe = ad9172_modes[mode].jesd_K,
		.converters_per_device = ad9172_modes[mode].jesd_M,
		.converter_resolution = ad9172_modes[mode].jesd_N,
		.bits_per_sample = ad9172_modes[mode].jesd_NP,
		.high_density = ad9172_modes[mode].jesd_HD, //false,
		.control_bits_per_sample = 0,
		.subclass = 1,
		.device_clk_khz = 7372800 / 40,	/* (lane_clk_khz / 40) */
		.lane_clk_khz = 7372800,	/* LaneRate = ( M/L)*NP*(10/8)*DataRate */
	};

	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = 3,
		.out_clk_sel = 4,
		.cpll_enable = 0,
		.lpm_enable = 1,			/* lane rates up to 11.2 Gb/s for short reach */
		.lane_rate_khz = 7372800,   /* desired lane rate */
		.ref_rate_khz = 460800,		/* FPGA_CLK, output 12 of HMC 7044 */
	};

	struct spi_init_param ad9172_spi_param = {
		.id = SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.mode = SPI_MODE_0,
		.chip_select = SPI_AD9172_CS,
		.flags = 0
	};

	struct ad9172_init_param ad9172_param = {
		.spi_init = &ad9172_spi_param,	/* spi_init_param */
		.gpio_txen0 = 54 + 22,
		.gpio_txen1 = 54 + 23,
		.dac_rate_khz = 2949120,		/* or sample rate */
		.dac_clkin_Hz = 92160000,		/* DAC_CLK, output 2 of HMC 7044 */
		.jesd_link_mode = 20,
		.jesd_subclass = 1,
		.dac_interpolation = 1,
		.channel_interpolation = 1,
		.clock_output_config = 4,
		.syncoutb_type = SIGNAL_LVDS,
		.sysref_coupling = COUPLING_AC,
	};

	struct hmc7044_dev *hmc7044_device;
	struct ad9172_dev *ad9172_device;
	struct axi_jesd204_tx *tx_jesd;
	struct adxcvr *tx_adxcvr;

	int32_t status;

	status = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_init() failed\n", tx_adxcvr_init.name);
		goto error_2;
	}

	status = hmc7044_init(&hmc7044_device, &hmc7044_param);

	if (status != SUCCESS) {
		printf("hmc7044_init() error: %"PRIi32"\n", status);
	//	continue;
		//goto error_1;
	}

	uint32_t lane_rate_kHz = 7372800;
	//uint32_t lane_rate_kHz =   3686400;
	//uint32_t lane_rate_kHz = 7372880;
	uint32_t data_rate_kHz = (lane_rate_kHz * ad9172_modes[mode].jesd_L * 8) / (ad9172_modes[mode].jesd_M * ad9172_modes[mode].jesd_NP * 10);
	uint32_t dac_rate_kHz = data_rate_kHz * 1;
	uint32_t ad9172_ref_rate_kHz = 0;
	uint32_t fpga_ch_divider;
	uint32_t dac_ch_divider;
	uint32_t pll2_freq_khz;
	status = -1;
	printf("------------------------------------------\n");
	while(1 /*status < 0*/) {
		//get FPGA ref
		status = autoconfig(tx_adxcvr, lane_rate_kHz); // get tx_adxcvr->ref_rate_khz

		if (status != SUCCESS) {
			printf("autoconfig() error: lane_rate_kHz %"PRIi32":\n", lane_rate_kHz);
			return FAILURE;
			//goto error_1;
		}
		while(1) {
			// check HMC can calc ref_rate_khz, and get a pll2_freq
			status = hmc7044_auto_init(hmc7044_device, tx_adxcvr->ref_rate_khz, &pll2_freq_khz, &fpga_ch_divider);
			if(status < 0)
				break; // try another ref_rate_khz

			while(1) {
				status = ad9172_autoconfig(dac_rate_kHz, pll2_freq_khz, &ad9172_ref_rate_kHz, &dac_ch_divider);
				if(status < 0)
					break; // try another HMC config
				if(status == SUCCESS) {
					printf("lane_rate_kHz = %"PRIi32" FPGA ref_rate_kHz = %"PRIi32"", lane_rate_kHz, tx_adxcvr->ref_rate_khz);
					printf(" HMC7044 pll2_freq = %"PRIi32" ", pll2_freq_khz);
					printf(" AD9172 ad9172_ref_rate_kHz = %"PRIi32" \n", ad9172_ref_rate_kHz);
				}
			}
		}
	}
	tx_jesd_init.lane_clk_khz = lane_rate_kHz;
	tx_jesd_init.device_clk_khz = lane_rate_kHz / 40;

	tx_adxcvr_init.lane_rate_khz = lane_rate_kHz;
	tx_adxcvr_init.ref_rate_khz = lane_rate_kHz;

	ad9172_param.dac_rate_khz = dac_rate_kHz;
	ad9172_param.dac_clkin_Hz = ad9172_ref_rate_kHz * 1000;

	hmc7044_device->channels[0].divider = dac_ch_divider;
	hmc7044_device->channels[2].divider = fpga_ch_divider;
	hmc7044_device->pll2_freq = pll2_freq_khz * 1000;







	status = hmc7044_setup(hmc7044_device);
	if (status != SUCCESS) {
		printf("hmc7044_init() error: %"PRIi32"\n", status);
		goto error_1;
	}

	status = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_rx_init() failed\n", tx_jesd_init.name);
		goto error_2;
	}

//	status = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
//	if (status != SUCCESS) {
//		printf("error: %s: adxcvr_init() failed\n", tx_adxcvr_init.name);
//		goto error_3;
//	}


	status = adxcvr_clk_set_rate(tx_adxcvr, tx_adxcvr->lane_rate_khz, tx_adxcvr->ref_rate_khz);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_clk_set_rate() failed\n", tx_adxcvr->name);
		goto error_3;
	}

	status = adxcvr_clk_enable(tx_adxcvr);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_clk_enable() failed\n", tx_adxcvr->name);
		goto error_3;
	}

	status = axi_jesd204_tx_lane_clk_enable(tx_jesd);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n", tx_jesd->name);
		goto error_3;
	}

	status = ad9172_init(&ad9172_device, &ad9172_param);
	if (status != SUCCESS) {
		printf("ad9172_init() error: %"PRIi32"\n", status);
		goto error_4;
	}

	status = axi_jesd204_tx_status_read(tx_jesd);
	if (status != SUCCESS) {
		printf("axi_jesd204_tx_status_read() error: %"PRIi32"\n", status);
		goto error_4;
	}

	return 0;

	error_4:
		ad9172_remove(ad9172_device);
	error_3:
		axi_jesd204_tx_remove(tx_jesd);
	error_2:
		adxcvr_remove(tx_adxcvr);
	error_1:
		hmc7044_remove(hmc7044_device);

	return status;

}

int main(void)
{

	struct axi_dac_init tx_dac_init = {
		"tx_dac",
		TX_CORE_BASEADDR,
		1,
	};
#ifdef DAC_DMA_EXAMPLE
	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		DMA_MEM_TO_DEV,
		DMA_LAST,
	};
	struct axi_dmac *tx_dmac;
#endif /* DAC_DMA_EXAMPLE */

	struct axi_dac *tx_dac;
	int32_t status;

	status = ad9172_system_init(20);
	if (status != SUCCESS) {
		printf("ad9172_system_init() error: %"PRIi32"\n", status);
		goto error_5;
	}
	status = axi_dac_init(&tx_dac, &tx_dac_init);
	if (status != SUCCESS) {
		printf("axi_dac_init() error: %"PRIi32"\n", status);
		goto error_5;
	}



#ifdef DAC_DMA_EXAMPLE
	extern const uint32_t sine_lut_iq[1024];
	axi_dac_load_custom_data(tx_dac, sine_lut_iq,
				 ARRAY_SIZE(sine_lut_iq),
				 DDR_MEM_BASEADDR);
	axi_dmac_init(&tx_dmac, &tx_dmac_init);
	axi_dmac_transfer(tx_dmac, DDR_MEM_BASEADDR,
			  sizeof(sine_lut_iq) * (tx_dac->num_channels / 2));
#else /* DAC_DMA_EXAMPLE */
	printf("Set dds frequency at 40MHz\n");
	uint32_t ch1_freq_hz = 1200000000;
	uint32_t ch2_freq_hz = 400000000;
	axi_dac_dds_set_frequency(tx_dac, 0, ch1_freq_hz);	/* TX1_I_F1 */
	axi_dac_dds_set_frequency(tx_dac, 1, ch1_freq_hz);	/* TX1_I_F2 */
	axi_dac_dds_set_frequency(tx_dac, 2, ch2_freq_hz);	/* TX2_Q_F1 */
	axi_dac_dds_set_frequency(tx_dac, 3, ch2_freq_hz);	/* TX2_Q_F2 */

	axi_dac_dds_set_frequency(tx_dac, 4, ch1_freq_hz); /* TX1_I_F1 */
	axi_dac_dds_set_frequency(tx_dac, 5, ch1_freq_hz); /* TX1_I_F2 */
	axi_dac_dds_set_frequency(tx_dac, 6, ch2_freq_hz); /* TX2_Q_F1 */
	axi_dac_dds_set_frequency(tx_dac, 7, ch2_freq_hz); /* TX2_Q_F2 */


	axi_dac_dds_set_scale(tx_dac, 0, 250000);
	axi_dac_dds_set_scale(tx_dac, 1, 250000);
	axi_dac_dds_set_scale(tx_dac, 2, 250000);
	axi_dac_dds_set_scale(tx_dac, 3, 250000);

	axi_dac_dds_set_scale(tx_dac, 4, 250000);
	axi_dac_dds_set_scale(tx_dac, 5, 250000);
	axi_dac_dds_set_scale(tx_dac, 6, 250000);
	axi_dac_dds_set_scale(tx_dac, 7, 250000);


	axi_dac_dds_set_phase(tx_dac, 0, 90000);
	axi_dac_dds_set_phase(tx_dac, 1, 90000);
	axi_dac_dds_set_phase(tx_dac, 2, 0);
	axi_dac_dds_set_phase(tx_dac, 3, 0);

	axi_dac_dds_set_phase(tx_dac, 4, 90000);
	axi_dac_dds_set_phase(tx_dac, 5, 90000);
	axi_dac_dds_set_phase(tx_dac, 6, 0);
	axi_dac_dds_set_phase(tx_dac, 7, 0);

	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
#endif /* DAC_DMA_EXAMPLE */

	printf("Bye\n");

	while(1)
	{}

	error_5:
		axi_dac_remove(tx_dac);

	return 0;
}
