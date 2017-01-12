// ***************************************************************************
// ***************************************************************************
// Copyright 2011(c) Analog Devices, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//     - Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     - Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in
//       the documentation and/or other materials provided with the
//       distribution.
//     - Neither the name of Analog Devices, Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//     - The use of this software may or may not infringe the patent rights
//       of one or more patent holders.  This license does not release you
//       from the requirement that you obtain separate licenses from these
//       patent holders to use this software.
//     - Use of the software either in source or binary form, must be run
//       on or directly connected to an Analog Devices Inc. component.
//
// THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED.
//
// IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
// RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ***************************************************************************
// ***************************************************************************

#include <stdlib.h>
#include "altxil_base.h"
#include "common.h"
#include "t_mykonos.h"
#include "mykonos.h"
#include "mykonos_gpio.h"
#include "ad9371_init.h"
#include "ad9371_bin.h"
#include "ad9528.h"
#include "adc_core.h"
#include "dac_core.h"

int main()
{
  uint8_t pll_status = 0;
  uint8_t mcs_status = 0;
  uint8_t tx_status = 0;
  uint8_t rx_status = 0;
  uint8_t rx_obs_status = 0;
  uint8_t cal_flag = 0;
  uint8_t cal_code = 0;
  uint32_t cal_mask = 0;

  spi_device  ad9528_spi_device;
  ad9528_channel_spec ad9528_channels[5];
  ad9528_platform_data  ad9528_init_param;

  // fpga-ref-clk (122.88)

  ad9528_channels[0].channel_num = 1;
  ad9528_channels[0].sync_ignore_en = 0;
  ad9528_channels[0].output_dis = 0;
  ad9528_channels[0].driver_mode = DRIVER_MODE_LVDS;
  ad9528_channels[0].signal_source = SOURCE_VCO;
  ad9528_channels[0].divider_phase = 0;
  ad9528_channels[0].channel_divider = 10;

  // fpga-aux-clk (122.88)

  ad9528_channels[1].channel_num = 2;
  ad9528_channels[1].sync_ignore_en = 0;
  ad9528_channels[1].output_dis = 0;
  ad9528_channels[1].driver_mode = DRIVER_MODE_LVDS;
  ad9528_channels[1].signal_source = SOURCE_VCO;
  ad9528_channels[1].divider_phase = 0;
  ad9528_channels[1].channel_divider = 10;

  // device-clk (122.88)

  ad9528_channels[2].channel_num = 13;
  ad9528_channels[2].sync_ignore_en = 0;
  ad9528_channels[2].output_dis = 0;
  ad9528_channels[2].driver_mode = DRIVER_MODE_LVDS;
  ad9528_channels[2].signal_source = SOURCE_VCO;
  ad9528_channels[2].divider_phase = 0;
  ad9528_channels[2].channel_divider = 10;

  // fpga-sysref (5.76)

  ad9528_channels[3].channel_num = 3;
  ad9528_channels[3].sync_ignore_en = 0;
  ad9528_channels[3].output_dis = 0;
  ad9528_channels[3].driver_mode = DRIVER_MODE_LVDS;
  ad9528_channels[3].signal_source = SOURCE_VCO;
  ad9528_channels[3].divider_phase = 0;
  ad9528_channels[3].channel_divider = 640;

  // device-sysref (5.76)

  ad9528_channels[4].channel_num = 12;
  ad9528_channels[4].sync_ignore_en = 0;
  ad9528_channels[4].output_dis = 0;
  ad9528_channels[4].driver_mode = DRIVER_MODE_LVDS;
  ad9528_channels[4].signal_source = SOURCE_VCO;
  ad9528_channels[4].divider_phase = 0;
  ad9528_channels[4].channel_divider = 640;

  // pllx settings

  ad9528_init_param.spi3wire = 0;
  ad9528_init_param.ref_mode = 0;
  ad9528_init_param.refa_en = 0;
  ad9528_init_param.refa_diff_rcv_en = 0;
  ad9528_init_param.refa_cmos_neg_inp_en = 0;
  ad9528_init_param.refa_r_div = 1;
  ad9528_init_param.refb_en = 0;
  ad9528_init_param.refb_diff_rcv_en = 0;
  ad9528_init_param.refb_cmos_neg_inp_en = 0;
  ad9528_init_param.refb_r_div = 0;
  ad9528_init_param.vcxo_freq = 122880000;
  ad9528_init_param.osc_in_diff_en = 0;
  ad9528_init_param.osc_in_cmos_neg_inp_en = 1;
  ad9528_init_param.pll1_feedback_div = 4;
  ad9528_init_param.pll1_feedback_src_vcxo = 1;
  ad9528_init_param.pll1_charge_pump_current_nA = 5000;
  ad9528_init_param.pll1_bypass_en = 0;
  ad9528_init_param.pll2_charge_pump_current_nA = 805000;
  ad9528_init_param.pll2_freq_doubler_en = 0;
  ad9528_init_param.pll2_r1_div = 1;
  ad9528_init_param.pll2_vco_diff_m1 = 3;
  ad9528_init_param.pll2_ndiv_a_cnt = 2;
  ad9528_init_param.pll2_ndiv_b_cnt = 7;
  ad9528_init_param.pll2_n2_div = 10;
  ad9528_init_param.sysref_src = SYSREF_SRC_INTERNAL;
  ad9528_init_param.sysref_k_div = 512;
  ad9528_init_param.rpole2 = RPOLE2_900_OHM;
  ad9528_init_param.rzero= RZERO_1850_OHM;
  ad9528_init_param.cpole1 = CPOLE1_16_PF;
  ad9528_init_param.rzero_bypass_en = 0;
  ad9528_init_param.num_channels = 5;
  ad9528_init_param.channels = &ad9528_channels[0];

  ad_gpio_set(AD9528_RESET_B,     0x0);
  ad_gpio_set(AD9371_RESET_B,     0x0);
  ad_gpio_set(AD9528_SYSREF_REQ,  0x0);
  ad_gpio_set(AD9371_TX1_ENABLE,  0x1);
  ad_gpio_set(AD9371_TX2_ENABLE,  0x1);
  ad_gpio_set(AD9371_RX1_ENABLE,  0x1);
  ad_gpio_set(AD9371_RX2_ENABLE,  0x1);
  ad_gpio_set(AD9371_TEST,        0x0);

  ad_gpio_set(AD9528_RESET_B,     0x1);
  ad_gpio_set(AD9371_RESET_B,     0x1);

  ad_spi_init(&ad9528_spi_device);
  ad9528_spi_device.chip_select = 0x6;
  ad9528_setup(&ad9528_spi_device, &ad9528_init_param);

  MYKONOS_resetDevice(&mykDevice);
  MYKONOS_initialize(&mykDevice);
  MYKONOS_checkPllsLockStatus(&mykDevice, &pll_status);

  if ((pll_status & 0x01) == 0)
  {
    xil_printf("ERROR: AD9371 PLL is NOT locked!\n");
    return(-1);
  }

  MYKONOS_enableMultichipSync(&mykDevice, 1, &mcs_status);
  mdelay(1);

  MYKONOS_enableMultichipSync(&mykDevice, 0, &mcs_status);
  mdelay(1);

  if ((mcs_status & 0x0b) != 0x0b)
  {
    xil_printf("ERROR: AD9371 MCS failed!\n");
    return(-1);
  }

  MYKONOS_initArm(&mykDevice);
  MYKONOS_loadArmFromBinary(&mykDevice, &ad9371_bin[0], ad9371_bin_len);

  MYKONOS_setRfPllFrequency(&mykDevice, RX_PLL, mykDevice.rx->rxPllLoFrequency_Hz);
  MYKONOS_setRfPllFrequency(&mykDevice, TX_PLL, mykDevice.tx->txPllLoFrequency_Hz);
  MYKONOS_setRfPllFrequency(&mykDevice, SNIFFER_PLL, mykDevice.obsRx->snifferPllLoFrequency_Hz);
  MYKONOS_checkPllsLockStatus(&mykDevice, &pll_status);

  if ((pll_status & 0x0f) != 0x0f)
  {
    xil_printf("ERROR: AD9371 PLLs are NOT locked!\n");
    return(-1);
  }

  MYKONOS_setRx1GainCtrlPin(&mykDevice, 0, 0, 0, 0, 0);
  MYKONOS_setRx2GainCtrlPin(&mykDevice, 0, 0, 0, 0, 0);
  MYKONOS_setTx1AttenCtrlPin(&mykDevice, 0, 0, 0, 0, 0);
  MYKONOS_setTx2AttenCtrlPin(&mykDevice, 0, 0, 0, 0);
  MYKONOS_setupGpio(&mykDevice);

  MYKONOS_setRx1ManualGain(&mykDevice, 255);
  MYKONOS_setRx2ManualGain(&mykDevice, 255);
  MYKONOS_setObsRxManualGain(&mykDevice, OBS_RX1_TXLO, 255);
  MYKONOS_setObsRxManualGain(&mykDevice, OBS_RX2_TXLO, 255);
  MYKONOS_setObsRxManualGain(&mykDevice, OBS_SNIFFER_A, 255);
  MYKONOS_setObsRxManualGain(&mykDevice, OBS_SNIFFER_B, 255);
  MYKONOS_setObsRxManualGain(&mykDevice, OBS_SNIFFER_C, 255);

  MYKONOS_setTx1Attenuation(&mykDevice, 0);
  MYKONOS_setTx2Attenuation(&mykDevice, 0);

  cal_mask = TX_BB_FILTER | ADC_TUNER | TIA_3DB_CORNER | DC_OFFSET |
    TX_ATTENUATION_DELAY | RX_GAIN_DELAY | FLASH_CAL |
    PATH_DELAY | TX_LO_LEAKAGE_INTERNAL | TX_QEC_INIT |
    LOOPBACK_RX_LO_DELAY | LOOPBACK_RX_RX_QEC_INIT |
    RX_LO_DELAY | RX_QEC_INIT;

  MYKONOS_runInitCals(&mykDevice, (cal_mask & ~TX_LO_LEAKAGE_EXTERNAL));
  MYKONOS_waitInitCals(&mykDevice, 60000, &cal_flag, &cal_code);

  if ((cal_flag != 0) || (cal_code != 0))
  {
    xil_printf("ERROR: AD9371 calibrations failed!\n");
    return(-1);
  }

  MYKONOS_enableSysrefToRxFramer(&mykDevice, 1);
  MYKONOS_enableSysrefToObsRxFramer(&mykDevice, 1);
  MYKONOS_enableSysrefToDeframer(&mykDevice, 1);

  MYKONOS_readRxFramerStatus(&mykDevice, &rx_status);
  MYKONOS_readOrxFramerStatus(&mykDevice, &rx_obs_status);
  MYKONOS_readDeframerStatus(&mykDevice, &tx_status);

  cal_mask = TRACK_RX1_QEC | TRACK_RX2_QEC |
    TRACK_TX1_QEC | TRACK_TX2_QEC |
    TRACK_ORX1_QEC | TRACK_ORX2_QEC;

  MYKONOS_enableTrackingCals(&mykDevice, cal_mask);
  MYKONOS_radioOn(&mykDevice);
  MYKONOS_setObsRxPathSource(&mykDevice, OBS_RXOFF);
  MYKONOS_setObsRxPathSource(&mykDevice, OBS_SNIFFER);

  xil_printf("done.\n");
  return(0);
}

// ***************************************************************************
// ***************************************************************************
