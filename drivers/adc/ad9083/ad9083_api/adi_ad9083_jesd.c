// SPDX-License-Identifier: GPL-2.0
/*!
 * @brief     APIs for JESD configuration and control
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9083_JESD_API
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9083_config.h"
#include "adi_ad9083_hal.h"

/*============= D E F I N E S ==============*/

/*============= C O D E ====================*/
int32_t adi_ad9083_jesd_powerup_enable_set(adi_ad9083_device_t *device,
                                           uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_PWRUP_LCPLL_INFO, enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_link_reset(adi_ad9083_device_t *device,
                                      uint8_t reset) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_FORCE_LINK_RESET_INFO, reset);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_link_digital_reset(adi_ad9083_device_t *device,
                                              uint8_t reset) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_FORCE_LINK_DIGITAL_RESET_INFO, reset);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_link_enable_set(adi_ad9083_device_t *device,
                                           uint8_t reset) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_JTX_LINK_EN_INFO, reset);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_cbus_default_set(adi_ad9083_device_t *device) {
  int32_t err;
  uint8_t pll_reg;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_RSTB_LCPLL_RC_INFO, 1);
  AD9083_ERROR_RETURN(err);
  /* The HPC+ change to var_dac_offset is 19*32=608, 0x260,
   * pll/vcovardacinovd_lcpll_rc */
  err = adi_ad9083_hal_cbuspll_reg_set(device, 0xB1, 0x20);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_cbuspll_reg_set(device, 0xB2, 0x02);
  AD9083_ERROR_RETURN(err);
  /* increase the regulator voltage level from 3 to 4, pll/sel_regref_lcpll_rc
   */
  err = adi_ad9083_hal_cbuspll_reg_get(device, 0x8D, &pll_reg);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_cbuspll_reg_set(device, 0x8D, (pll_reg & 0xF8) | 0x4);
  AD9083_ERROR_RETURN(err);
  /* pll/ptatslope_lcpll_rc */
  err = adi_ad9083_hal_cbuspll_reg_get(device, 0xB6, &pll_reg);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_cbuspll_reg_set(device, 0xB6, (pll_reg & 0x0F) | 0x70);
  AD9083_ERROR_RETURN(err);
  /* pll/vcobiastcf_lcpll_rc */
  err = adi_ad9083_hal_cbuspll_reg_get(device, 0x93, &pll_reg);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_cbuspll_reg_set(device, 0x93, (pll_reg & 0x8F) | 0x50);
  AD9083_ERROR_RETURN(err);
  /* changed from 2 -> 1 (48 to 24), pll/vco_band_init_lcpll_rc */
  err = adi_ad9083_hal_cbuspll_reg_get(device, 0xD3, &pll_reg);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_cbuspll_reg_set(device, 0xD3, (pll_reg & 0xCF) | 0x10);
  AD9083_ERROR_RETURN(err);
  /* bits[7:6] changed from 1 -> 2 (2 % to 4 % ), pll/spare2_lcpll_rc */
  err = adi_ad9083_hal_cbuspll_reg_get(device, 0xB5, &pll_reg);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_cbuspll_reg_set(device, 0xB5, (pll_reg & 0xBF) | 0x80);
  AD9083_ERROR_RETURN(err);
  /* reduced default from 52 to 44, pll/regthresh_lcpll_rc */
  err = adi_ad9083_hal_cbuspll_reg_get(device, 0xBE, &pll_reg);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_cbuspll_reg_set(device, 0xBE, (pll_reg & 0xC0) | 0x2C);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_RSTB_LCPLL_RC_INFO, 0);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_link_config_set(adi_ad9083_device_t *device,
                                           adi_cms_jesd_param_t *jesd_param) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_INVALID_PARAM_RETURN(jesd_param->jesd_l > 4);

  err =
      adi_ad9083_hal_bf_set(device, BF_JTX_M_CFG_INFO, jesd_param->jesd_m - 1);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_JTX_F_CFG_INFO, jesd_param->jesd_f - 1);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_JTX_K_CFG_INFO, jesd_param->jesd_k - 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_CS_CFG_INFO, jesd_param->jesd_cs);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_JTX_N_CFG_INFO, jesd_param->jesd_n - 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_NP_CFG_INFO,
                              jesd_param->jesd_np - 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_SUBCLASSV_CFG_INFO,
                              jesd_param->jesd_subclass);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_JTX_S_CFG_INFO, jesd_param->jesd_s - 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_NS_CFG_INFO, 0);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_JTX_L_CFG_INFO, jesd_param->jesd_l - 1);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_JTX_SCR_CFG_INFO, jesd_param->jesd_scr);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_JESDV_CFG_INFO,
                              1); /* always JESD204B */
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_HD_CFG_INFO, jesd_param->jesd_hd);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_JTX_DID_CFG_INFO, jesd_param->jesd_did);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_JTX_BID_CFG_INFO, jesd_param->jesd_bid);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_MODE_INFO, 0); /* not used */
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000300, 0x00000100, 0); /* not used */
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_MODE_S_SEL_INFO,
                              0); /* disable quick config */
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_lan_xbar_set(adi_ad9083_device_t *device,
                                        uint8_t physical_lane,
                                        uint8_t logical_lane) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_JTX_LANE_SEL_INFO(physical_lane),
                              logical_lane);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_lan_power_down_set(adi_ad9083_device_t *device,
                                              uint8_t physical_lane,
                                              uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_JTX_FORCE_LANE_PD_INFO(physical_lane),
                              enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_phy_enable_set(adi_ad9083_device_t *device,
                                          uint8_t lane_num) {
  int32_t err;
  uint16_t mask;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  switch (lane_num) {
  case 1:
    mask = 0xfffe;
    break;
  case 2:
    mask = 0xfffc;
    break;
  case 3:
    mask = 0xfff8;
    break;
  case 4:
    mask = 0xfff0;
    break;
  default:
    return API_CMS_ERROR_INVALID_PARAM;
  }
  err = adi_ad9083_hal_bf_set(device, BF_PD_SER_INFO, mask);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_clk_set(adi_ad9083_device_t *device) {
  int32_t err;
  uint8_t jesd_m, jesd_np, jesd_l, refindiv_lcpll;
  uint8_t bit_repeat = 1, bit_repeat_exponent = 0, lcpll_exponent = 1;
  uint64_t adc_clk, lane_rate, vco_clk, fRefClk, fPfd, clk_divp;
  uint8_t spi_div;
  uint8_t b_lcpll_integer, b_lcpll_numerator, b_lcpll_denominator;
  uint8_t pclk_div_integer, pclk_div_numerator, pclk_div_denominator;
  uint8_t ifx_pclk_div_integer, ifx_pclk_div_numerator,
      ifx_pclk_div_denominator;
  uint8_t spi_div_integer, spi_div_numerator, spi_div_denominator;
  uint8_t async_pclk_ratio, divm_lcpll_rc_rx = 0, i;
  uint8_t async_pclk_div_lookup[6] = {5, 10, 20, 40, 80, 160};
  uint16_t total_dcm;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  /* get configuration */
  err = adi_ad9083_hal_bf_get(device, BF_JTX_L_CFG_INFO, &jesd_l, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_get(device, BF_JTX_M_CFG_INFO, &jesd_m, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_get(device, BF_JTX_NP_CFG_INFO, &jesd_np, 1);
  AD9083_ERROR_RETURN(err);
  jesd_l = jesd_l + 1;
  jesd_m = jesd_m + 1;
  jesd_np = jesd_np + 1;

  /* get ADC clk and total_dcm, from user settings */
  adc_clk = device->dev_info.adc_freq_hz;
  adi_ad9083_rx_datapath_total_dec_get(device, &total_dcm);
  AD9083_ERROR_RETURN(err);

  /* cal jtx bitrepeat */
  lane_rate = jesd_np * jesd_m * adc_clk * 10;
#ifdef __KERNEL__
  lane_rate = no_os_div_u64(lane_rate, jesd_l * total_dcm * 8);
#else
  lane_rate = lane_rate / (jesd_l * total_dcm * 8);
#endif
  vco_clk = lane_rate * bit_repeat;

  while (((vco_clk < 8000000000ul) || (vco_clk > 16000000000ul)) &&
         (bit_repeat < 33)) {
    bit_repeat = bit_repeat * 2;
    vco_clk = lane_rate * bit_repeat;
    bit_repeat_exponent++;
  }
  /* Calculate JTX PLL loop dividers */
#ifdef __KERNEL__
  spi_div = no_os_div_u64(lane_rate * 2, adc_clk);
#else
  spi_div = (lane_rate * 2) / (adc_clk);
#endif
  err = adi_ad9083_hal_div_nume_deno(device, lane_rate * 2, adc_clk,
                                     &spi_div_integer, &spi_div_numerator,
                                     &spi_div_denominator);
  AD9083_ERROR_RETURN(err);
  spi_div = spi_div_denominator;
  spi_div = (spi_div == 1) ? 2 : spi_div;

  refindiv_lcpll = 2;
#ifdef __KERNEL__
  fRefClk = no_os_div_u64(adc_clk / 2, spi_div);
  fPfd = no_os_div_u64(fRefClk, refindiv_lcpll);
#else
  fRefClk = (adc_clk / 2) / spi_div;
  fPfd = fRefClk / refindiv_lcpll;
#endif
  err =
      adi_ad9083_hal_div_nume_deno(device, vco_clk, fPfd * 8, &b_lcpll_integer,
                                   &b_lcpll_numerator, &b_lcpll_denominator);
  AD9083_ERROR_RETURN(err);

  while (((fPfd < 40000000ul) || (fPfd > 400000000ul) ||
          (b_lcpll_numerator > 0)) &&
         (refindiv_lcpll < 15)) {
    refindiv_lcpll = refindiv_lcpll * 2;
#ifdef __KERNEL__
    fPfd = no_os_div_u64(fRefClk, refindiv_lcpll);
#else
    fPfd = fRefClk / refindiv_lcpll;
#endif
    err = adi_ad9083_hal_div_nume_deno(device, vco_clk, fPfd * 8,
                                       &b_lcpll_integer, &b_lcpll_numerator,
                                       &b_lcpll_denominator);
    AD9083_ERROR_RETURN(err);
    lcpll_exponent++;
  }

  /* Calculate Controls for JTX Internal Clocks */
  clk_divp = vco_clk >> 3;
  err = adi_ad9083_hal_div_nume_deno(device, adc_clk / 2, lane_rate / 40,
                                     &pclk_div_integer, &pclk_div_numerator,
                                     &pclk_div_denominator);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_div_nume_deno(
      device, adc_clk / 2, (lane_rate * bit_repeat) / 40, &ifx_pclk_div_integer,
      &ifx_pclk_div_numerator, &ifx_pclk_div_denominator);
  AD9083_ERROR_RETURN(err);
#ifdef __KERNEL__
  async_pclk_ratio = no_os_div_u64(clk_divp, lane_rate / 40);
#else
  async_pclk_ratio = clk_divp / (lane_rate / 40);
#endif
  for (i = 0; i < 6; i++) {
    if (async_pclk_div_lookup[i] == async_pclk_ratio) {
      divm_lcpll_rc_rx = i;
      break;
    }
  }

  err = adi_ad9083_hal_bf_set(device, BF_JTX_BR_LOG2_RATIO_INFO(0),
                              bit_repeat_exponent); /* lane 1 */
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_BR_LOG2_RATIO_INFO(1),
                              bit_repeat_exponent); /* lane 2 */
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_BR_LOG2_RATIO_INFO(2),
                              bit_repeat_exponent); /* lane 3 */
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_BR_LOG2_RATIO_INFO(3),
                              bit_repeat_exponent); /* lane 4 */
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_SPI_DIV_SERDES_INFO, spi_div);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_DIVP_LCPLL_RC_INFO,
                              0); /* multiplication factor is 8 for B divider */
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_REFINDIV_LCPLL_RC_INFO, lcpll_exponent);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_B_LCPLL_RC_INFO, b_lcpll_integer);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_hal_bf_set(device, BF_JTX_PCLK_DIV_INTEGER_INFO,
                              pclk_div_integer - 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_PCLK_DIV_FRAC_NUM_INFO,
                              pclk_div_numerator);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_PCLK_DIV_FRAC_DEN_INFO,
                              pclk_div_denominator);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_IFX_PCLK_DIV_INTEGER_INFO,
                              ifx_pclk_div_integer - 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_IFX_PCLK_DIV_FRAC_NUM_INFO,
                              ifx_pclk_div_numerator);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_IFX_PCLK_DIV_FRAC_DEN_INFO,
                              ifx_pclk_div_denominator);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_hal_bf_set(device, BF_DIVM_LCPLL_RC_INFO, 1);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_bf_set(device, BF_DIVM_LCPLL_RC_RX_INFO, divm_lcpll_rc_rx);
  AD9083_ERROR_RETURN(err);

  if ((pclk_div_integer % bit_repeat) > 0) {
    err = adi_ad9083_hal_bf_set(device, BF_ASYNC_LANE_CLK_SEL_INFO, 1);
    AD9083_ERROR_RETURN(err);
    err = adi_ad9083_hal_bf_set(device, BF_ASYNC_LANE_DOUT_SEL_INFO, 1);
    AD9083_ERROR_RETURN(err);
  } else {
    err = adi_ad9083_hal_bf_set(device, BF_ASYNC_LANE_CLK_SEL_INFO, 0);
    AD9083_ERROR_RETURN(err);
    err = adi_ad9083_hal_bf_set(device, BF_ASYNC_LANE_DOUT_SEL_INFO, 0);
    AD9083_ERROR_RETURN(err);
  }
  return API_CMS_ERROR_OK;
}

int32_t
adi_ad9083_jesd_tx_gen_test(adi_ad9083_device_t *device,
                            adi_ad9083_jesd_tx_test_data_src_e data_source,
                            adi_ad9083_jesd_tx_test_mode_e test_mode) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  /* stop test */
  err = adi_ad9083_hal_bf_set(device, BF_JTX_TEST_USER_GO_INFO, 0);
  AD9083_ERROR_RETURN(err);

  /* set data source */
  err = adi_ad9083_hal_bf_set(device, BF_JTX_TEST_GEN_SEL_INFO, data_source);
  AD9083_ERROR_RETURN(err);

  /* set test mode */
  err = adi_ad9083_hal_bf_set(device, BF_JTX_TEST_GEN_MODE_INFO, test_mode);
  AD9083_ERROR_RETURN(err);

  /* update data */
  err = adi_ad9083_hal_bf_set(device, BF_JTX_TEST_USER_GO_INFO, 1);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t
adi_ad9083_jesd_tx_continuous_d215_enable_set(adi_ad9083_device_t *device,
                                              uint8_t lane_id, uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(
      device, BF_JTX_DL_204B_SCR_DATA_SEL_CFG_INFO(lane_id), enable);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t
adi_ad9083_jesd_tx_phy_prbs_test(adi_ad9083_device_t *device,
                                 adi_cms_jesd_prbs_pattern_e prbs_pattern) {
  int32_t err;
  uint8_t pattern;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  /* decode prbs pattern */
  switch (prbs_pattern) {
  case PRBS7:
    pattern = AD9083_JESD_TX_TEST_MODE_PN7;
    break;
  case PRBS15:
    pattern = AD9083_JESD_TX_TEST_MODE_PN15;
    break;
  case PRBS31:
    pattern = AD9083_JESD_TX_TEST_MODE_PN31;
    break;
  default:
    return API_CMS_ERROR_INVALID_PARAM;
  }
//  pattern = AD9083_JESD_TX_TEST_MODE_RAMP;
  err = adi_ad9083_jesd_tx_gen_test(device, AD9083_JESD_TX_TEST_DATA_PHY,
                                    pattern);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_lanes_enable_set(adi_ad9083_device_t *device,
                                            uint8_t lane_num) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_jesd_tx_lan_power_down_set(device, 0, 0);
  AD9083_ERROR_RETURN(err);

  if (lane_num > 1) {
    err = adi_ad9083_jesd_tx_lan_power_down_set(device, 1, 0);
    AD9083_ERROR_RETURN(err);
  } else {
    err = adi_ad9083_jesd_tx_lan_power_down_set(device, 1, 1);
    AD9083_ERROR_RETURN(err);
  }
  if (lane_num > 2) {
    err = adi_ad9083_jesd_tx_lan_power_down_set(device, 2, 0);
    AD9083_ERROR_RETURN(err);
  } else {
    err = adi_ad9083_jesd_tx_lan_power_down_set(device, 2, 1);
    AD9083_ERROR_RETURN(err);
  }
  if (lane_num > 3) {
    err = adi_ad9083_jesd_tx_lan_power_down_set(device, 3, 0);
    AD9083_ERROR_RETURN(err);
  } else {
    err = adi_ad9083_jesd_tx_lan_power_down_set(device, 3, 1);
    AD9083_ERROR_RETURN(err);
  }
  if (lane_num > 4) {
    return API_CMS_ERROR_INVALID_PARAM;
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_lanes_disable_all_set(adi_ad9083_device_t *device) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_jesd_tx_lan_power_down_set(device, 0, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_jesd_tx_lan_power_down_set(device, 1, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_jesd_tx_lan_power_down_set(device, 2, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_jesd_tx_lan_power_down_set(device, 3, 1);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_lanes_xbar_set(adi_ad9083_device_t *device,
                                          uint8_t logic_lanes[4]) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_jesd_tx_lan_xbar_set(device, 0, logic_lanes[0]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_jesd_tx_lan_xbar_set(device, 1, logic_lanes[1]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_jesd_tx_lan_xbar_set(device, 2, logic_lanes[2]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_jesd_tx_lan_xbar_set(device, 3, logic_lanes[3]);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_lane_id_set(adi_ad9083_device_t *device,
                                       uint8_t lid[4]) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_JTX_LID_CFG_INFO(0), lid[0]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_LID_CFG_INFO(1), lid[1]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_LID_CFG_INFO(2), lid[2]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_JTX_LID_CFG_INFO(3), lid[3]);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_octave_cal_disable_set(adi_ad9083_device_t *device) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_EN_OCTAVECAL_LCPLL_RC_INFO, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_BOVD_LCPLL_RC_INFO, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_DIVMOVD_LCPLL_RC_INFO, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_REFINDIVOVD_LCPLL_RC_INFO, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_RXDIVRATEOVD_LCPLL_RC_INFO, 1);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_check_jtx_m(adi_ad9083_device_t *device, uint8_t m) {
  int32_t err;
  uint8_t num_tones, nco_bypass, total_channels;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_get(device, BF_NO_DDC_MODE_INFO, &nco_bypass, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_get(device, BF_NUM_TONES_INFO, &num_tones, 1);
  AD9083_ERROR_RETURN(err);

  total_channels = (nco_bypass > 0) ? num_tones * 16 : num_tones * 32;
  if (m != total_channels) {
    err = adi_ad9083_hal_log_write(device, ADI_CMS_LOG_WARN,
                                   "jtx_m NOT match non-zero NCO number.");
    AD9083_ERROR_RETURN(err);
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jtx_startup(adi_ad9083_device_t *device,
                               adi_cms_jesd_param_t *jtx_param) {
  int32_t err;
  uint8_t logic_lanes[4] = {0, 1, 2, 3};
  uint8_t lane_id[4] = {0, 1, 2, 3};
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  /* check if jtx_m matches non_zero_nco number */
  err = adi_ad9083_jesd_tx_check_jtx_m(device, jtx_param->jesd_m);
  AD9083_ERROR_RETURN(err);

  /* power down */
  err = adi_ad9083_jesd_powerup_enable_set(device, 0);
  AD9083_ERROR_RETURN(err);

  /* disable jtx link */
  err = adi_ad9083_jesd_tx_link_enable_set(device, 0);
  AD9083_ERROR_RETURN(err);

  /* force jtx link reset */
  err = adi_ad9083_jesd_tx_link_reset(device, 1);
  AD9083_ERROR_RETURN(err);

  /* write CBUS default changes */
  err = adi_ad9083_jesd_tx_cbus_default_set(device);
  AD9083_ERROR_RETURN(err);

  /* disable octave cal */
  err = adi_ad9083_jesd_tx_octave_cal_disable_set(device);
  AD9083_ERROR_RETURN(err);

  /* config jtx parameters */
  err = adi_ad9083_jesd_tx_link_config_set(device, jtx_param);
  AD9083_ERROR_RETURN(err);

  /* config jtx pll and internal clock */
  err = adi_ad9083_jesd_tx_clk_set(device);
  AD9083_ERROR_RETURN(err);

  /* assign default jtx lanes 0 = 0， 1 - 1， 2 - 2， 3 - 3 */
  err = adi_ad9083_jesd_tx_lanes_xbar_set(device, logic_lanes);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_jesd_tx_lanes_enable_set(device, jtx_param->jesd_l);
  AD9083_ERROR_RETURN(err);

  /* power up */
  err = adi_ad9083_jesd_powerup_enable_set(device, 1);
  AD9083_ERROR_RETURN(err);

  /* enable jtx phys */
  err = adi_ad9083_jesd_tx_phy_enable_set(device, jtx_param->jesd_l);
  AD9083_ERROR_RETURN(err);

  /* assign default lane id: 0 1 2 3 */
  err = adi_ad9083_jesd_tx_lane_id_set(device, lane_id);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_hal_bf_set(device, BF_RSTB_SER_INFO, 0xFFFF);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_PD_SYNCA_RX_RC_INFO, 0);
  AD9083_ERROR_RETURN(err);

  /* enable jtx link */
  err = adi_ad9083_jesd_tx_link_enable_set(device, 1);
  AD9083_ERROR_RETURN(err);

  /* force jtx link reset */
  err = adi_ad9083_jesd_tx_link_reset(device, 0);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_hal_delay_us(device, 1000000);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_jesd_tx_lanes_disable_all_set(device);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_jesd_tx_lanes_enable_set(device, jtx_param->jesd_l);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_SYNCA_RX_MODE_RC_INFO, 1);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_jesd_tx_link_digital_reset(device, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 1000);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_jesd_tx_link_digital_reset(device, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 1000);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_lcpll_status_get(adi_ad9083_device_t *device,
                                            uint8_t *pll_locked) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(pll_locked);

  err =
      adi_ad9083_hal_bf_get(device, BF_LCPLLLOCK_LCPLL_RS_INFO, pll_locked, 1);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_link_status_get(adi_ad9083_device_t *device,
                                           uint16_t *status) {
  int32_t err;
  uint16_t res;
  uint8_t pll_locked, phase_establish, invalid_mode, sync_204b, state_204b;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(status);

  err =
      adi_ad9083_hal_bf_get(device, BF_JTX_DL_204B_STATE_INFO, &state_204b, 1);
  AD9083_ERROR_RETURN(err);
  res = state_204b & 0x0F;

  err =
      adi_ad9083_hal_bf_get(device, BF_JTX_DL_204B_SYNC_N_INFO, &sync_204b, 1);
  AD9083_ERROR_RETURN(err);
  res += (sync_204b & 0x1) << 4;

  err = adi_ad9083_hal_bf_get(device, BF_JTX_PLL_LOCKED_INFO, &pll_locked, 1);
  AD9083_ERROR_RETURN(err);
  res += (pll_locked & 0x1) << 5;

  err = adi_ad9083_hal_bf_get(device, BF_JTX_PHASE_ESTABLISHED_INFO,
                              &phase_establish, 1);
  AD9083_ERROR_RETURN(err);
  res += (phase_establish & 0x1) << 6;

  err =
      adi_ad9083_hal_bf_get(device, BF_JTX_INVALID_MODE_INFO, &invalid_mode, 1);
  AD9083_ERROR_RETURN(err);
  res += (invalid_mode & 0x1) << 7;
  *status = res;

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_set_swing(adi_ad9083_device_t *device, uint8_t lane,
                                     adi_ad9083_ser_swing_e swing) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_INVALID_PARAM_RETURN(lane >= AD9083_JESD_SER_COUNT);
  AD9083_LOG_FUNC();

  if (lane == 0) {
    err = adi_ad9083_hal_bf_set(device, BF_DRVSWING_CH0_SER_RC_INFO, swing);
  } else if (lane == 1) {
    err = adi_ad9083_hal_bf_set(device, BF_DRVSWING_CH1_SER_RC_INFO, swing);
  } else if (lane == 2) {
    err = adi_ad9083_hal_bf_set(device, BF_DRVSWING_CH2_SER_RC_INFO, swing);
  } else {
    err = adi_ad9083_hal_bf_set(device, BF_DRVSWING_CH3_SER_RC_INFO, swing);
  }
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_set_pre_emp(adi_ad9083_device_t *device,
                                       uint8_t lane,
                                       adi_ad9083_ser_pre_emp_e pre_emp) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_INVALID_PARAM_RETURN(lane >= AD9083_JESD_SER_COUNT);
  AD9083_LOG_FUNC();

  if (lane == 0) {
    err = adi_ad9083_hal_bf_set(device, BF_DRVPREEM_CH0_SER_RC_INFO, pre_emp);
  } else if (lane == 1) {
    err = adi_ad9083_hal_bf_set(device, BF_DRVPREEM_CH1_SER_RC_INFO, pre_emp);
  } else if (lane == 2) {
    err = adi_ad9083_hal_bf_set(device, BF_DRVPREEM_CH2_SER_RC_INFO, pre_emp);
  } else {
    err = adi_ad9083_hal_bf_set(device, BF_DRVPREEM_CH3_SER_RC_INFO, pre_emp);
  }
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_jesd_tx_set_post_emp(adi_ad9083_device_t *device,
                                        uint8_t lane,
                                        adi_ad9083_ser_post_emp_e post_emp) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_INVALID_PARAM_RETURN(lane >= AD9083_JESD_SER_COUNT);
  AD9083_LOG_FUNC();

  if (lane == 0) {
    err = adi_ad9083_hal_bf_set(device, BF_DRVPOSTEM_CH0_SER_RC_INFO, post_emp);
  } else if (lane == 1) {
    err = adi_ad9083_hal_bf_set(device, BF_DRVPOSTEM_CH1_SER_RC_INFO, post_emp);
  } else if (lane == 2) {
    err = adi_ad9083_hal_bf_set(device, BF_DRVPOSTEM_CH2_SER_RC_INFO, post_emp);
  } else {
    err = adi_ad9083_hal_bf_set(device, BF_DRVPOSTEM_CH3_SER_RC_INFO, post_emp);
  }
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t
adi_ad9083_jead_tx_ser_settings_set(adi_ad9083_device_t *device,
                                    adi_ad9083_ser_settings_t *ser_settings) {
  int32_t err;
  uint8_t i;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(ser_settings);
  AD9083_LOG_FUNC();

  /* swing - pre - post */
  for (i = 0; i < AD9083_JESD_SER_COUNT; ++i) {
    err = adi_ad9083_jesd_tx_set_swing(
        device, i, ser_settings->indv_ser_lane_settings[i].swing_setting);
    AD9083_ERROR_RETURN(err);
    err = adi_ad9083_jesd_tx_set_pre_emp(
        device, i, ser_settings->indv_ser_lane_settings[i].pre_emp_setting);
    AD9083_ERROR_RETURN(err);
    err = adi_ad9083_jesd_tx_set_post_emp(
        device, i, ser_settings->indv_ser_lane_settings[i].post_emp_setting);
    AD9083_ERROR_RETURN(err);
  }

  /* tx invert */
  err = adi_ad9083_hal_reg_set(device, REG_MAIN_DATA_INV_ADDR,
                               ser_settings->tx_invert_mask);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

/*! @} */
