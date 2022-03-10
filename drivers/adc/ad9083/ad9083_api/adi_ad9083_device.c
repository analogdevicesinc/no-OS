// SPDX-License-Identifier: GPL-2.0
/*!
 * @brief     device level API implementation
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9083_DEVICE_API
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9083_config.h"
#include "adi_ad9083_hal.h"

/*============= D E F I N E S ==============*/
static uint8_t ad9083_api_revision[3] = {1, 0, 1};

/*============= C O D E ====================*/
int32_t adi_ad9083_device_hw_open(adi_ad9083_device_t *device) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  if (err = adi_ad9083_hal_hw_open(device), err != API_CMS_ERROR_OK) {
    return err;
  }
  AD9083_LOG_FUNC();
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_hw_close(adi_ad9083_device_t *device) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  return adi_ad9083_hal_hw_close(device);
}

int32_t adi_ad9083_device_init(adi_ad9083_device_t *device) {
  int32_t err;
  uint32_t endian_test_val = 0x11223344;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

#ifdef R0
  err = adi_ad9083_hal_log_write(device, ADI_CMS_LOG_MSG,
                                 "api v%d.%d.%d commit %s for ad%x r0",
                                 ad9083_api_revision[0], ad9083_api_revision[1],
                                 ad9083_api_revision[2], "0000000", AD9083_ID);
  AD9083_ERROR_RETURN(err);
#endif
#ifdef R1
  err = adi_ad9083_hal_log_write(device, ADI_CMS_LOG_MSG,
                                 "api v%d.%d.%d commit %s for ad%x r1",
                                 ad9083_api_revision[0], ad9083_api_revision[1],
                                 ad9083_api_revision[2], "0000000", AD9083_ID);
  AD9083_ERROR_RETURN(err);
#endif
  /* get host cpu endian mode */
  if (*(uint8_t *)&endian_test_val == 0x44)
    AD9083_LOG_MSG("host is using little endian mode.");
  else
    AD9083_LOG_MSG("host is using big endian mode.");

  /* get host cpu type settings */
  err = adi_ad9083_hal_log_write(
      device, ADI_CMS_LOG_MSG, "sizeof(uint8/16/32/64_t) = %d/%d/%d/%d bytes",
      sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t), sizeof(uint64_t));
  AD9083_ERROR_RETURN(err);

  /* configure spi */
  err = adi_ad9083_device_spi_config(device);
  AD9083_ERROR_RETURN(err);

  /* 8bit reg read/write test */
  err = adi_ad9083_device_reg8_access_check(device);
  AD9083_ERROR_RETURN(err);

  /* check power status */
  err = adi_ad9083_device_power_status_check(device);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_deinit(adi_ad9083_device_t *device) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  /* hardware reset */
  err = adi_ad9083_device_reset(device, AD9083_HARD_RESET);
  AD9083_ERROR_RETURN(err);

  /* software reset */
  err = adi_ad9083_device_reset(device, AD9083_SOFT_RESET);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_spi_config(adi_ad9083_device_t *device) {
  uint8_t reg_val = 0x00;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  reg_val |= ((device->hal_info.sdo == SPI_SDIO) ? 0x00 : 0x18);
  reg_val |= ((device->hal_info.msb == SPI_MSB_FIRST) ? 0x00 : 0x42);
  reg_val |= ((device->hal_info.addr_inc == SPI_ADDR_DEC_AUTO) ? 0x00 : 0x24);

  return adi_ad9083_hal_reg_set(device, REG_SPI_INTERFACE_CONFIG_A_ADDR,
                                reg_val);
}

int32_t adi_ad9083_device_spi_register_set(adi_ad9083_device_t *device,
                                           uint16_t addr, uint8_t data) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  return adi_ad9083_hal_reg_set(device, addr, data);
  ;
}

int32_t adi_ad9083_device_spi_register_get(adi_ad9083_device_t *device,
                                           uint16_t addr, uint8_t *data) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(data);

  return adi_ad9083_hal_reg_get(device, addr, data);
  ;
}

int32_t adi_ad9083_device_cbuspll_register_get(adi_ad9083_device_t *device,
                                               uint8_t addr, uint8_t *data) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(data);

  return adi_ad9083_hal_cbuspll_reg_get(device, addr, data);
}

int32_t adi_ad9083_device_cbuspll_register_set(adi_ad9083_device_t *device,
                                               uint8_t addr, uint8_t data) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  return adi_ad9083_hal_cbuspll_reg_set(device, addr, data);
}

int32_t adi_ad9083_device_reg8_access_check(adi_ad9083_device_t *device) {
  int32_t err;
  uint8_t reg8, data8;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  /* 8bit reg read/write test */
  err = adi_ad9083_hal_reg_get(device, REG_DECIMATE_H_ADDR, &reg8);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_DECIMATE_H_ADDR, 0x5a);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_get(device, REG_DECIMATE_H_ADDR, &data8);
  AD9083_ERROR_RETURN(err);
  if (data8 != 0x5a) {
    err = adi_ad9083_hal_log_write(
        device, ADI_CMS_LOG_ERR,
        "8bit r/w test failed. Write %.2x but readback is %.2x.", 0x5a, data8);
    AD9083_ERROR_RETURN(err);
    return API_CMS_ERROR_TEST_FAILED;
  }
  err = adi_ad9083_hal_reg_set(device, REG_DECIMATE_H_ADDR, 0xa5);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_get(device, REG_DECIMATE_H_ADDR, &data8);
  AD9083_ERROR_RETURN(err);
  if (data8 != 0xa5) {
    err = adi_ad9083_hal_log_write(
        device, ADI_CMS_LOG_ERR,
        "8bit r/w test failed. Write %.2x but readback is %.2x.", 0xa5, data8);
    AD9083_ERROR_RETURN(err);
    return API_CMS_ERROR_TEST_FAILED;
  }
  err = adi_ad9083_hal_reg_set(device, REG_DECIMATE_H_ADDR, reg8);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_reset(adi_ad9083_device_t *device,
                                adi_ad9083_reset_e operation) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_INVALID_PARAM_RETURN(operation > AD9083_HARD_RESET_AND_INIT);

  /* do reset */
  if ((operation == AD9083_SOFT_RESET) ||
      (operation == AD9083_SOFT_RESET_AND_INIT)) {
    err = adi_ad9083_hal_reg_set(device, REG_SPI_INTERFACE_CONFIG_A_ADDR, 0x81);
    AD9083_ERROR_RETURN(err);
    err = adi_ad9083_hal_delay_us(device, 200);
    AD9083_ERROR_RETURN(err);
  } else if ((operation == AD9083_HARD_RESET) ||
             (operation == AD9083_HARD_RESET_AND_INIT)) {
    err = adi_ad9083_hal_reset_pin_ctrl(device, 0);
    AD9083_ERROR_RETURN(err);
    err = adi_ad9083_hal_delay_us(device, 100000);
    AD9083_ERROR_RETURN(err);
    err = adi_ad9083_hal_reset_pin_ctrl(device, 1);
    AD9083_ERROR_RETURN(err);
    err = adi_ad9083_hal_delay_us(device, 100000);
    AD9083_ERROR_RETURN(err);
  }

  /* do init */
  if ((operation == AD9083_SOFT_RESET_AND_INIT) ||
      (operation == AD9083_HARD_RESET_AND_INIT)) {
    err = adi_ad9083_device_init(device);
    AD9083_ERROR_RETURN(err);
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_chip_id_get(adi_ad9083_device_t *device,
                                      adi_cms_chip_id_t *chip_id) {
  int32_t err;
  uint8_t reg_val = 0x0;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(chip_id);

  err = adi_ad9083_hal_reg_get(device, REG_CHIP_TYPE_ADDR, &reg_val);
  AD9083_ERROR_RETURN(err);
  chip_id->chip_type = reg_val;

  err = adi_ad9083_hal_reg_get(device, REG_PROD_ID_LSB_ADDR, &reg_val);
  AD9083_ERROR_RETURN(err);
  chip_id->prod_id = reg_val;
  err = adi_ad9083_hal_reg_get(device, 0x00000005, &reg_val);
  AD9083_ERROR_RETURN(err);
  chip_id->prod_id |= (reg_val << 8);

  err = adi_ad9083_hal_reg_get(device, REG_CHIP_GRADE_ADDR, &reg_val);
  AD9083_ERROR_RETURN(err);
  chip_id->prod_grade = (reg_val >> 4);
  chip_id->dev_revision = (reg_val & 0x0F);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_die_id_get(adi_ad9083_device_t *device, uint8_t *id) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(id);

  return adi_ad9083_hal_reg_get(device, REG_DIE_ID_ADDR, (uint8_t *)id);
}

int32_t adi_ad9083_device_api_revision_get(adi_ad9083_device_t *device,
                                           uint8_t *rev_major,
                                           uint8_t *rev_minor,
                                           uint8_t *rev_rc) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(rev_major);
  AD9083_NULL_POINTER_RETURN(rev_minor);
  AD9083_NULL_POINTER_RETURN(rev_rc);

  *rev_major = ad9083_api_revision[0];
  *rev_minor = ad9083_api_revision[1];
  *rev_rc = ad9083_api_revision[2];

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_clock_config_set(adi_ad9083_device_t *device,
                                           uint64_t adc_clk_hz,
                                           uint64_t ref_clk_hz) {
  int32_t err;
  uint8_t i, pll_lock, valid_cfg = 0;
  uint8_t ref_div = 1, m_div = 1, n_div = 1, pll_div = 1;
  uint64_t pfd_clk_hz, vco_freq, adc_sample_rate;
  uint8_t n_div_buff[4] = {5, 7, 8, 11};
  uint8_t pll_div_buff[3] = {6, 8, 10};

  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_INVALID_PARAM_RETURN(adc_clk_hz > AD9083_ADC_CLK_FREQ_HZ_MAX);
  AD9083_INVALID_PARAM_RETURN(adc_clk_hz < AD9083_ADC_CLK_FREQ_HZ_MIN);

  /* save clock settings */
  device->dev_info.dev_freq_hz = ref_clk_hz;
  device->dev_info.adc_freq_hz = adc_clk_hz;

  if (ref_clk_hz >= AD9083_ADC_CLK_FREQ_HZ_MIN) {
    AD9083_INVALID_PARAM_RETURN(ref_clk_hz > AD9083_ADC_CLK_FREQ_HZ_MAX);
    err = adi_ad9083_hal_bf_set(device, BF_SPI_PLL_BYP_INFO, 0x1);
    AD9083_ERROR_RETURN(err);
    return API_CMS_ERROR_OK;
  } else {
    AD9083_INVALID_PARAM_RETURN(ref_clk_hz > AD9083_REF_CLK_FREQ_HZ_MAX);
    AD9083_INVALID_PARAM_RETURN(ref_clk_hz < AD9083_REF_CLK_FREQ_HZ_MIN);
  }

  /* calculate m/n div */
  for (ref_div = 1; ref_div < 6; ref_div++) {
    pfd_clk_hz = ref_clk_hz / ref_div;
    for (m_div = 2; m_div < 52; m_div++) {
      for (n_div = 0; n_div < 4; n_div++) {
        vco_freq = (ref_clk_hz * m_div * n_div_buff[n_div]) / ref_div;
        for (pll_div = 0; pll_div < 3; pll_div++) {
#ifdef __KERNEL__
          adc_sample_rate = no_os_div_u64(vco_freq, pll_div_buff[pll_div]);
#else
          adc_sample_rate = vco_freq / pll_div_buff[pll_div];
#endif
          if ((vco_freq > 8800000000ul) && (vco_freq < 12400000000ul) &&
              (adc_sample_rate == adc_clk_hz) && (pfd_clk_hz <= 500000000ul)) {
            valid_cfg = 1;
            break;
          }
        }
        if (valid_cfg == 1) {
          break;
        }
      }
      if (valid_cfg == 1) {
        break;
      }
    }
    if (valid_cfg == 1) {
      break;
    }
  }

  if ((vco_freq < 8800000000ul) || (vco_freq > 12400000000ul) ||
      (pfd_clk_hz < 25000000ul) || (pfd_clk_hz > 500000000ul)) {
    return API_CMS_ERROR_INVALID_PARAM;
  }

  /* set filter */
  err = adi_ad9083_hal_reg_set(device, 0x00000D4C, 0x05);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_IMPALA_FILT_MAIN_0_ADDR, 0x20);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_IMPALA_FILT_MAIN_2_ADDR, 0x22);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_IMPALA_FILT_MAIN_1_ADDR, 0xFA);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 1000);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_hal_bf_set(device, 0x00000D40, 0x00000102, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000D40, 0x00000101, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000D40, 0x00000105, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000D40, 0x00000105, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000D08, 0x00000106, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 1000);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_SPI_PLL_BYP_INFO, 0x0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000D08, 0x1C);
  AD9083_ERROR_RETURN(err);

  /* set clk divider */
  err = adi_ad9083_hal_bf_set(device, BF_D_REFIN_DIV_INFO, ref_div - 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_D_DIVIDE_CONTROL_INFO, m_div);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_D_CP_CURRENT_INFO, 0x13);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_D_CONTROL_HS_FB_DIV_INFO, n_div);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_SPI_CNTRL_HS_DIV_INFO, pll_div);
  AD9083_ERROR_RETURN(err);

  /* reset cal, try to lock pll */
  err = adi_ad9083_hal_bf_set(device, 0x00000D4C, 0x00000101, 0x0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000D4C, 0x00000101, 0x1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_SPI_LOCK_VALID_RST_INFO, 0x1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_SPI_OUTOFLOCK_RST_INFO, 0x1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000D44, 0x00000105, 0x1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_SPI_LOCK_VALID_RST_INFO, 0x0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_SPI_OUTOFLOCK_RST_INFO, 0x0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000D44, 0x00000105, 0x0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_D_CAL_RESET_INFO, 0x1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_D_CAL_RESET_INFO, 0x0);
  AD9083_ERROR_RETURN(err);

  /* check pll lock status */
  for (i = 0; i < 50; i++) {
    err = adi_ad9083_hal_delay_us(device, 10000);
    AD9083_ERROR_RETURN(err);
    err = adi_ad9083_device_pll_lock_status_get(device, &pll_lock);
    AD9083_ERROR_RETURN(err);
    if (pll_lock == 0x1)
      break;
  }
  if (pll_lock != 0x1) {
    AD9083_ERROR_REPORT(API_CMS_ERROR_PLL_NOT_LOCKED, pll_lock,
                        "PLL not locked");
    return API_CMS_ERROR_PLL_NOT_LOCKED;
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_pll_lock_status_get(adi_ad9083_device_t *device,
                                              uint8_t *status) {
  int32_t err;
  uint8_t pll_lock;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_get(device, BF_PLL_LOCK_VALID_INFO, &pll_lock, 1);
  AD9083_ERROR_RETURN(err);
  *status = pll_lock;

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_pll_test_enable_set(adi_ad9083_device_t *device,
                                              uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x00000D44, 0x00000106, enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_power_status_check(adi_ad9083_device_t *device) {
  int32_t err;
  uint8_t reg8, power_on;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  /* check power status */
  power_on = 1;
  err = adi_ad9083_hal_bf_get(device, BF_PORB_VDD_ANA_INFO, &reg8, 1);
  AD9083_ERROR_RETURN(err);
  power_on &= reg8;
  err = adi_ad9083_hal_bf_get(device, BF_PORB_VDD_DIG_1P0_INFO, &reg8, 1);
  AD9083_ERROR_RETURN(err);
  power_on &= reg8;
  err = adi_ad9083_hal_bf_get(device, BF_PORB_VDD_SYNCRX_1P8_INFO, &reg8, 1);
  AD9083_ERROR_RETURN(err);
  power_on &= reg8;
  err = adi_ad9083_hal_bf_get(device, BF_PORB_VDDCP_LCPLL_1P0_INFO, &reg8, 1);
  AD9083_ERROR_RETURN(err);
  power_on &= reg8;
  err = adi_ad9083_hal_bf_get(device, BF_PORB_VDDLDO_LCPLL_1P8_INFO, &reg8, 1);
  AD9083_ERROR_RETURN(err);
  power_on &= reg8;
  err = adi_ad9083_hal_bf_get(device, BF_PORB_VDDPHY_SER_1P0_INFO, &reg8, 1);
  AD9083_ERROR_RETURN(err);
  power_on &= reg8;
  err =
      adi_ad9083_hal_bf_get(device, BF_PORB_VDDSYNTH_LCPLL_1P0_INFO, &reg8, 1);
  AD9083_ERROR_RETURN(err);
  power_on &= reg8;

  if (power_on == 0) {
    err = adi_ad9083_hal_log_write(device, ADI_CMS_LOG_ERR,
                                   "some power supplies are not on.");
    AD9083_ERROR_RETURN(err);
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_temp_sensor_enable_set(adi_ad9083_device_t *device,
                                                 uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_EN_TEMP_DIODE_INFO,
                              (enable > 0) ? 0x03 : 0);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_temp_sensor_sel_set(adi_ad9083_device_t *device,
                                              uint8_t sel) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_TEMP_DIODE_SEL_INFO, sel);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_temp_get(adi_ad9083_device_t *device, int32_t vbe_1,
                                   int32_t vbe_2, int32_t *temp) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(temp);

  int32_t constant = 3874; /* q * 1000 / (k * ln20): q = 1.602e-19, k
                              = 1.3806e-23, ln20 = 2.9957 */
  int32_t temp_mC =
      constant * (vbe_1 - vbe_2) -
      273150; /* unit: mili-degree, vbe_1 & vbe_2 must be in unit mV */
  *temp = temp_mC;

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_low_power_pin_ctrl_set(adi_ad9083_device_t *device,
                                                 uint8_t sel) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_reg_set(device, REG_LOW_PWR_PIN_CTRL_ADDR, sel);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_low_power_cfg_set(adi_ad9083_device_t *device,
                                            uint8_t cfg) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_reg_set(device, REG_LOW_PWR_CONFIG_ADDR, cfg);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_low_power_status_get(adi_ad9083_device_t *device,
                                               uint8_t *status) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(status);

  err = adi_ad9083_hal_reg_get(device, REG_LOW_PWR_PIN_STAT_ADDR, status);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_low_power_pol_set(adi_ad9083_device_t *device,
                                            uint8_t pol) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_POL_INFO, pol);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_low_power_mask_set(adi_ad9083_device_t *device,
                                             uint8_t mask) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_reg_set(device, REG_LOW_PWR_PIN_MASK_ADDR, mask);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_device_low_power_mode_set(adi_ad9083_device_t *device,
                                             adi_ad9083_low_power_mode_e mode) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_INVALID_PARAM_RETURN(mode > AD9083_FULL_POWER_DOWN);

  err = adi_ad9083_device_low_power_pin_ctrl_set(device, 0);
  AD9083_ERROR_RETURN(err);
  switch (mode) {
  case AD9083_POWER_ON:
    err = adi_ad9083_device_low_power_cfg_set(device, 0);
    AD9083_ERROR_RETURN(err);
    break;
  case AD9083_STANDBY:
    err = adi_ad9083_device_low_power_cfg_set(device, 0x2C);
    AD9083_ERROR_RETURN(err);
    break;
  case AD9083_FULL_POWER_DOWN:
    err = adi_ad9083_device_low_power_cfg_set(device, 0x2D);
    AD9083_ERROR_RETURN(err);
    break;
  }

  return API_CMS_ERROR_OK;
}

/*! @} */