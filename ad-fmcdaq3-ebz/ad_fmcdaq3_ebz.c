/***************************************************************************//**
 *   @file   ad_fmcdaq3_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
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

#include "platform_drivers.h"
#include "ad9152.h"
#include "ad9528.h"
#include "ad9680.h"
#include "adc_core.h"
#include "dac_core.h"
#include "xcvr_core.h"
#include "jesd_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define GPIO_CLKD_STATUS_0      32
#define GPIO_CLKD_STATUS_1      33
#define GPIO_DAC_IRQ            34
#define GPIO_ADC_FDA            35
#define GPIO_ADC_FDB            36
#define GPIO_DAC_TXEN           37
#define GPIO_ADC_PD             38
#define GPIO_TRIG               39

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
  spi_device ad9528_spi_device;
  spi_device ad9152_spi_device;
  spi_device ad9680_spi_device;
  ad9528_channel_spec ad9528_channels[8];
  ad9528_platform_data ad9528_param;
  ad9152_init_param ad9152_param;
  ad9680_init_param ad9680_param;
  xcvr_core ad9152_xcvr;
  jesd_core ad9152_jesd;
  dac_channel ad9152_channels[2];
  dac_core ad9152_core;
  xcvr_core ad9680_xcvr;
  jesd_core ad9680_jesd;
  adc_core ad9680_core;

  ad_spi_init(&ad9528_spi_device);
  ad_spi_init(&ad9152_spi_device);
  ad_spi_init(&ad9680_spi_device);

  ad9528_spi_device.chip_select = 0x6;
  ad9152_spi_device.chip_select = 0x5;
  ad9680_spi_device.chip_select = 0x3;

  // ad9528 defaults

  ad9528_param.num_channels = 8;
  ad9528_param.channels = &ad9528_channels[0];
  ad9528_init(&ad9528_param);

  // dac-device-clock (1.25G) & sysref (2.44M)

  ad9528_channels[0].channel_num = 2;
  ad9528_channels[0].channel_divider = 1;
  ad9528_channels[1].channel_num = 8;
  ad9528_channels[1].channel_divider = 512;

  // dac-fpga-clock (625M) & sysref (2.44M)

  ad9528_channels[2].channel_num = 9;
  ad9528_channels[2].channel_divider = 2;
  ad9528_channels[3].channel_num = 7;
  ad9528_channels[3].channel_divider = 512;

  // adc-device-clock (1.2G) & sysref (2.44M)

  ad9528_channels[4].channel_num = 13;
  ad9528_channels[4].channel_divider = 1;
  ad9528_channels[5].channel_num = 5;
  ad9528_channels[5].channel_divider = 512;

  // adc-fpga-clock (625M) & sysref (2.44M)

  ad9528_channels[6].channel_num = 4;
  ad9528_channels[6].channel_divider = 2;
  ad9528_channels[7].channel_num = 6;
  ad9528_channels[7].channel_divider = 512;

  // pllx settings

  ad9528_param.spi3wire = 1;
  ad9528_param.vcxo_freq = 125000000;
  ad9528_param.osc_in_diff_en = 1;
  ad9528_param.pll2_charge_pump_current_nA = 805000;
  ad9528_param.pll2_vco_diff_m1 = 3;
  ad9528_param.pll2_ndiv_a_cnt = 2;
  ad9528_param.pll2_ndiv_b_cnt = 7;
  ad9528_param.pll2_n2_div = 10;
  ad9528_param.sysref_k_div = 512;
  ad9528_param.rpole2 = RPOLE2_900_OHM;
  ad9528_param.rzero= RZERO_3250_OHM;
  ad9528_param.cpole1 = CPOLE1_16_PF;

  // dac settings

  ad9152_param.lane_rate_kbps = 10000000;

  ad9152_xcvr.mmcm_present = 0;
  ad9152_xcvr.reconfig_bypass = 1;
  ad9152_xcvr.rx_tx_n = 0;
  ad9152_xcvr.no_of_lanes = 4;
  ad9152_xcvr.lane_rate_kbps = 10*1000*1000;

  ad9152_jesd.rx_tx_n = 0;
  ad9152_jesd.scramble_enable = 1;
  ad9152_jesd.octets_per_frame = 1;
  ad9152_jesd.frames_per_multiframe = 32;
  ad9152_jesd.subclass_mode = 1;

  ad9152_channels[0].dds_dual_tone = 0;
  ad9152_channels[0].dds_frequency_0 = 33*1000*1000;
  ad9152_channels[0].dds_phase_0 = 0;
  ad9152_channels[0].dds_scale_0 = 500000;
  ad9152_channels[0].sel = DAC_SRC_DDS;
  ad9152_channels[1].dds_dual_tone = 0;
  ad9152_channels[1].dds_frequency_0 = 11*1000*1000;
  ad9152_channels[1].dds_phase_0 = 0;
  ad9152_channels[1].dds_scale_0 = 5000000;
  ad9152_channels[1].sel = DAC_SRC_DDS;

  ad9152_core.no_of_channels = 2;
  ad9152_core.resolution = 16;
  ad9152_core.fifo_present = 1;
  ad9152_core.channels = &ad9152_channels[0];

  // adc settings

  ad9680_param.lane_rate_kbps = 10000000;

  ad9680_xcvr.mmcm_present = 0;
  ad9680_xcvr.reconfig_bypass = 1;
  ad9680_xcvr.rx_tx_n = 1;
  ad9680_xcvr.no_of_lanes = 4;
  ad9680_xcvr.lane_rate_kbps = 10*1000*1000;

  ad9680_jesd.rx_tx_n = 1;
  ad9680_jesd.scramble_enable = 1;
  ad9680_jesd.octets_per_frame = 1;
  ad9680_jesd.frames_per_multiframe = 32;
  ad9680_jesd.subclass_mode = 1;

  ad9680_core.no_of_channels = 2;
  ad9680_core.resolution = 14;

  // base addresses

#ifdef XILINX
  ad9152_xcvr.base_address = XPAR_AXI_AD9152_XCVR_BASEADDR;
  ad9152_jesd.base_address = XPAR_AXI_AD9152_JESD_BASEADDR;
  ad9152_core.base_address = XPAR_AXI_AD9152_CORE_BASEADDR;
  ad9680_xcvr.base_address = XPAR_AXI_AD9680_XCVR_BASEADDR;
  ad9680_jesd.base_address = XPAR_AXI_AD9680_JESD_BASEADDR;
  ad9680_core.base_address = XPAR_AXI_AD9680_CORE_BASEADDR;
#endif

  // functions (do not modify below)

  ad_gpio_set(GPIO_DAC_TXEN, 0x1);
  ad_gpio_set(GPIO_ADC_PD, 0x0);

  ad9528_setup(&ad9528_spi_device, &ad9528_param);

  ad9152_setup(&ad9152_spi_device, ad9152_param);
  jesd_setup(ad9152_jesd);
  xcvr_setup(ad9152_xcvr);
  jesd_status(ad9152_jesd);
  dac_setup(ad9152_core);
  ad9152_status(&ad9152_spi_device);

  ad9680_setup(&ad9680_spi_device, ad9680_param);
  jesd_setup(ad9680_jesd);
  xcvr_setup(ad9680_xcvr);
  jesd_status(ad9680_jesd);
  adc_setup(ad9680_core);

  ad9680_test(&ad9680_spi_device, AD9680_TEST_PN9);
  adc_pn_mon(ad9680_core, ADC_PN9);
  ad9680_test(&ad9680_spi_device, AD9680_TEST_PN23);
  adc_pn_mon(ad9680_core, ADC_PN23A);

  // default data

  ad9152_channels[0].sel = DAC_SRC_DDS;
  ad9152_channels[1].sel = DAC_SRC_DDS;
  dac_data_setup(ad9152_core);
  ad9680_test(&ad9680_spi_device, AD9680_TEST_OFF);
  ad_printf("daq3: done\n");
  return(0);
}
