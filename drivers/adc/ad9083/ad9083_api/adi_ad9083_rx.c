// SPDX-License-Identifier: GPL-2.0
/*!
 * @brief     APIs for RX configuration and control
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9083_RX_API
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9083_config.h"
#include "adi_ad9083_hal.h"

/*============= C O D E ====================*/
#define AD9083_FIXED_INT2Q(x, fb) ((uint32_t)((x) << (fb)))
#define AD9083_FIXED_QMUL(x, y, xFb, yFb, resFb)                               \
  ((uint32_t)(((uint64_t)(x) * (uint64_t)(y)) >> ((xFb) + (yFb) - (resFb))))
#define AD9083_FIXED_QDIV(x, y, xFb, yFb, resFb)                               \
  ((uint32_t)((((uint64_t)(x)) << ((resFb) + (yFb) - (xFb))) / (y)))

int32_t adi_ad9083_rx_cic_dec_rate_set(adi_ad9083_device_t *device,
                                       adi_ad9083_cic_dec_rate_e dec_rate) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_CIC_DEC_RATE_INFO, dec_rate);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_cic_acc_clear_set(adi_ad9083_device_t *device,
                                        uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_CIC_ACC_CLR_INFO, enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_bypass_cic_set(adi_ad9083_device_t *device,
                                     uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_DECI_ADC_DATA_INFO, enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_j_dec_rate_set(adi_ad9083_device_t *device,
                                     adi_ad9083_j_dec_rate_e dec_j) {
  int32_t err;
  /* Bypassing CIC decimation */
  uint8_t valid_j_dec_1[7] = {AD9083_J_DEC_8,  AD9083_J_DEC_12, AD9083_J_DEC_16,
                              AD9083_J_DEC_20, AD9083_J_DEC_24, AD9083_J_DEC_40,
                              AD9083_J_DEC_60};
  /* Using CIC deciamtion, bypassing mixer */
  uint8_t valid_j_dec_2[10] = {
      AD9083_J_DEC_1,  AD9083_J_DEC_4,  AD9083_J_DEC_8,  AD9083_J_DEC_10,
      AD9083_J_DEC_12, AD9083_J_DEC_16, AD9083_J_DEC_20, AD9083_J_DEC_24,
      AD9083_J_DEC_40, AD9083_J_DEC_60};
  /* Using CIC decimation and mixer */
  uint8_t valid_j_dec_3[4] = {AD9083_J_DEC_1, AD9083_J_DEC_4, AD9083_J_DEC_8,
                              AD9083_J_DEC_16};
  uint8_t cic_bypass, mixer_bypass, invalid, i;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_get(device, BF_DECI_ADC_DATA_INFO, &cic_bypass, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_get(device, BF_NO_DDC_MODE_INFO, &mixer_bypass, 1);
  AD9083_ERROR_RETURN(err);

  invalid = 1;
  if (cic_bypass == 1) {
    for (i = 0; i < 7; i++) {
      if (dec_j == valid_j_dec_1[i]) {
        invalid = 0;
        break;
      }
    }
  } else if (mixer_bypass == 1) {
    for (i = 0; i < 10; i++) {
      if (dec_j == valid_j_dec_2[i]) {
        invalid = 0;
        break;
      }
    }
  } else {
    for (i = 0; i < 4; i++) {
      if (dec_j == valid_j_dec_3[i]) {
        invalid = 0;
        break;
      }
    }
  }
  AD9083_INVALID_PARAM_RETURN(invalid == 1);
  err = adi_ad9083_hal_bf_set(device, BF_DEC_J_INFO, dec_j);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_g_value_set(adi_ad9083_device_t *device, uint8_t dec_g) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_G_VALUE_INFO, dec_g);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_h_value_set(adi_ad9083_device_t *device, uint8_t dec_h) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_H_VALUE_INFO, dec_h);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_datapath_total_dec_get(adi_ad9083_device_t *device,
                                             uint16_t *total_dec) {
  int32_t err;
  uint8_t cic, j, h;
  uint8_t deci_adc_data, burst_mode;
  uint8_t fbw_sel, cic_sel;
  uint8_t cic_rate[] = {4, 8, 16};
  uint8_t j_rate[] = {1, 4, 8, 16, 0, 0, 12, 24, 0, 10, 20, 40, 0, 0, 0, 60};
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(total_dec);

  err = adi_ad9083_hal_bf_get(device, BF_CIC_DEC_RATE_INFO, &cic, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_get(device, BF_DEC_J_INFO, &j, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_get(device, BF_H_VALUE_INFO, &h, 1);
  AD9083_ERROR_RETURN(err);
  AD9083_INVALID_PARAM_RETURN(cic > 2);
  AD9083_INVALID_PARAM_RETURN(j_rate[j] == 0);

  err = adi_ad9083_hal_bf_get(device, BF_BURST_MODE_INFO, &burst_mode, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_get(device, BF_DECI_ADC_DATA_INFO, &deci_adc_data, 1);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_hal_reg_get(device, 0x00000171, &fbw_sel);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_get(device, 0x00000171, &cic_sel);
  AD9083_ERROR_RETURN(err);

  if (fbw_sel > 0) {
    *total_dec = 4;
  } else if (cic_sel > 0) {
    *total_dec = cic_rate[cic]; /* To be checked */
  } else {
    if (burst_mode == 0) {
      if (deci_adc_data == 0) {
        *total_dec = cic_rate[cic] * j_rate[j];
      } else {
        *total_dec = j_rate[j];
      }
    } else {
      h = (h == 14) ? 8 : h;
      h = ((h == 18) || (h == 28)) ? 16 : h;
      h = (h == 36) ? 32 : h;
      *total_dec = cic_rate[cic] * h;
    }
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_nco_ftw_set(adi_ad9083_device_t *device, uint8_t index,
                                  uint8_t ftw) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  switch (index) {
  case 0:
    err = adi_ad9083_hal_bf_set(device, BF_NCO0_FTW_INFO, ftw);
    AD9083_ERROR_RETURN(err);
    break;
  case 1:
    err = adi_ad9083_hal_bf_set(device, BF_NCO1_FTW_INFO, ftw);
    AD9083_ERROR_RETURN(err);
    break;
  case 2:
    err = adi_ad9083_hal_bf_set(device, BF_NCO2_FTW_INFO, ftw);
    AD9083_ERROR_RETURN(err);
    break;
  default:
    return API_CMS_ERROR_INVALID_PARAM;
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_nco_phase_offset_set(adi_ad9083_device_t *device,
                                           uint8_t index,
                                           uint8_t phase_offset) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  switch (index) {
  case 0:
    err = adi_ad9083_hal_bf_set(device, BF_NCO0_PHOFF_INFO, phase_offset);
    AD9083_ERROR_RETURN(err);
    break;
  case 1:
    err = adi_ad9083_hal_bf_set(device, BF_NCO1_PHOFF_INFO, phase_offset);
    AD9083_ERROR_RETURN(err);
    break;
  case 2:
    err = adi_ad9083_hal_bf_set(device, BF_NCO2_PHOFF_INFO, phase_offset);
    AD9083_ERROR_RETURN(err);
    break;
  default:
    return API_CMS_ERROR_INVALID_PARAM;
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_nco_adither_enable_set(adi_ad9083_device_t *device,
                                             uint8_t index, uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  switch (index) {
  case 0:
    err = adi_ad9083_hal_bf_set(device, BF_ADITHER_EN0_INFO, enable);
    AD9083_ERROR_RETURN(err);
    break;
  case 1:
    err = adi_ad9083_hal_bf_set(device, BF_ADITHER_EN1_INFO, enable);
    AD9083_ERROR_RETURN(err);
    break;
  case 2:
    err = adi_ad9083_hal_bf_set(device, BF_ADITHER_EN2_INFO, enable);
    AD9083_ERROR_RETURN(err);
    break;
  default:
    return API_CMS_ERROR_INVALID_PARAM;
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_nco_6db_gain_enable_set(adi_ad9083_device_t *device,
                                              uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_NCO_6DB_GAIN_INFO, enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_bypass_mixer_set(adi_ad9083_device_t *device,
                                       uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_NO_DDC_MODE_INFO, enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_burst_mode_set(adi_ad9083_device_t *device, uint8_t sel) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_BURST_MODE_INFO, sel);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_datapath_enable_set(adi_ad9083_device_t *device,
                                          uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_DATAPATH_EN_INFO, enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_num_tones_set(adi_ad9083_device_t *device,
                                    uint8_t num_tones) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_NUM_TONES_INFO, num_tones);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_zero_stuff_enable_set(adi_ad9083_device_t *device,
                                            uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x0000011C, 0x00000100, enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_clk_sample_order_sel(adi_ad9083_device_t *device,
                                           uint8_t sel) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x0000011D, 0x00000101, sel);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_clk_sample_polarity_set(adi_ad9083_device_t *device,
                                              uint8_t pol) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x0000011D, 0x00000100, pol);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_test_mode_enable_set(adi_ad9083_device_t *device,
                                           uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x000001E0, 0x00000100, enable);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t
adi_ad9083_rx_test_mode_res_set(adi_ad9083_device_t *device,
                                adi_ad9083_rx_tmode_resolution_e i_resolution,
                                adi_ad9083_rx_tmode_resolution_e q_resolution) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x0000012F, 0x00000400, i_resolution);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000139, 0x00000400, q_resolution);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_test_mode_type_set(adi_ad9083_device_t *device,
                                         uint8_t i_type, uint8_t q_type) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x0000012E, 0x00000404, i_type);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000138, 0x00000404, q_type);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_test_mode_usr_pattern_set(adi_ad9083_device_t *device,
                                                uint16_t i_pattern[8],
                                                uint16_t q_pattern[8]) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x00000130, 0x00001000, i_pattern[0]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000132, 0x00001000, i_pattern[1]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000134, 0x00001000, i_pattern[2]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000136, 0x00001000, i_pattern[3]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000142, 0x00001000, i_pattern[4]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000144, 0x00001000, i_pattern[5]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000146, 0x00001000, i_pattern[6]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000148, 0x00001000, i_pattern[7]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x0000013A, 0x00001000, q_pattern[0]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x0000013C, 0x00001000, q_pattern[1]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x0000013E, 0x00001000, q_pattern[2]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000140, 0x00001000, q_pattern[3]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x0000014A, 0x00001000, q_pattern[4]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x0000014C, 0x00001000, q_pattern[5]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x0000014E, 0x00001000, q_pattern[6]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000150, 0x00001000, q_pattern[7]);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_test_mode_sel_set(adi_ad9083_device_t *device,
                                        uint8_t tmode_sel[12]) {
  int32_t i;
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  for (i = 0; i < 12; i++) {
    err = adi_ad9083_hal_reg_set(device, 0x00000122 + i, tmode_sel[i]);
    AD9083_ERROR_RETURN(err);
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_test_mode_sel_all_set(adi_ad9083_device_t *device,
                                            uint8_t enable) {
  int32_t i;
  int32_t err;
  uint8_t sel[12];
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  for (i = 0; i < 4; i++) {
    sel[i] = (enable > 0) ? 0xff : 0;
  }
  err = adi_ad9083_rx_test_mode_sel_set(device, sel);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_res_set(adi_ad9083_device_t *device,
                                      uint8_t res) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_DFORMAT_RES_INFO, res);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_fbw_dither_enable_set(adi_ad9083_device_t *device,
                                                    uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x00000189, 0x00000104, enable);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_ddc_dither_enable_set(adi_ad9083_device_t *device,
                                                    uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_DFORMAT_DDC_DITHER_EN_INFO, enable);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_cic_sel_set(adi_ad9083_device_t *device,
                                          uint8_t sel[12]) {
  uint8_t i;
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  for (i = 0; i < 12; i++) {
    err = adi_ad9083_hal_reg_set(device, 0x0000017D + i, sel[i]);
    AD9083_ERROR_RETURN(err);
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_cic_sel_all_set(adi_ad9083_device_t *device,
                                              uint8_t enable) {
  int32_t err;
  uint8_t i, sel[12];
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  for (i = 0; i < 12; i++) {
    sel[i] = 0;
  }

  sel[0] = enable ? 0x0F : 0;
  err = adi_ad9083_rx_dformat_cic_sel_set(device, sel);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_fbw_sel_set(adi_ad9083_device_t *device,
                                          uint8_t sel[12]) {
  uint8_t i;
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  for (i = 0; i < 12; i++) {
    err = adi_ad9083_hal_reg_set(device, 0x00000171 + i, sel[i]);
    AD9083_ERROR_RETURN(err);
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_fbw_sel_all_set(adi_ad9083_device_t *device,
                                              uint8_t enable) {
  int32_t err;
  uint8_t i, sel[12];
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  for (i = 0; i < 12; i++) {
    sel[i] = 0;
  }

  sel[0] = enable ? 0x0F : 0;
  err = adi_ad9083_rx_dformat_fbw_sel_set(device, sel);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_average_sel_set(adi_ad9083_device_t *device,
                                              uint8_t sel[12]) {
  uint8_t i;
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  for (i = 0; i < 12; i++) {
    err = adi_ad9083_hal_reg_set(device, 0x000001E1 + i, sel[i]);
    AD9083_ERROR_RETURN(err);
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_average_sel_all_set(adi_ad9083_device_t *device,
                                                  uint8_t enable) {
  int32_t err;
  uint8_t i, sel[12];
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  for (i = 0; i < 12; i++) {
    sel[i] = 0;
  }

  sel[0] = enable ? 0x0F : 0;
  err = adi_ad9083_rx_dformat_average_sel_set(device, sel);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_cic_div_sel_set(adi_ad9083_device_t *device,
                                              uint8_t sel) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x000001ED, 0x00000101, sel);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_fbw_div_sel_set(adi_ad9083_device_t *device,
                                              uint8_t sel) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x000001ED, 0x00000100, sel);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_dformat_out_div_sel_set(adi_ad9083_device_t *device,
                                              uint8_t sel) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x000001ED, 0x00000102, sel);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_enable_set(adi_ad9083_device_t *device,
                                     uint16_t adcs) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_reg_set(device, REG_ENABLE_CH7_0_REG_ADDR, adcs & 0xFF);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_ENABLE_CH15_8_REG_ADDR, adcs >> 8);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_pd_pin_mode_set(adi_ad9083_device_t *device,
                                          adi_ad9083_adc_pd_pin_mode_e mode) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_PIN_PD_MODE_INFO, mode);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_reset_enable_set(adi_ad9083_device_t *device,
                                           uint8_t enable) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_reg_set(device, 0x00000B93, enable);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_term_res_set(adi_ad9083_device_t *device,
                                       adi_ad9083_adc_term_res_e res) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, BF_RTERM_INFO, res);
  AD9083_ERROR_RETURN(err);
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_clk_freq_get(adi_ad9083_device_t *device,
                                       uint64_t *adc_clk_hz) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(adc_clk_hz);

  *adc_clk_hz = device->dev_info.adc_freq_hz;

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_reset_cal(adi_ad9083_device_t *device) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  err = adi_ad9083_hal_bf_set(device, 0x00000BB3, 0x00000106, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000BB3, 0x00000106, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 1000);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_kvti_factor_fixed_get(adi_ad9083_device_t *device,
                                                uint32_t orc1mode,
                                                uint32_t backoff,
                                                uint64_t finmax,
                                                uint32_t *kvti_factor) {
  uint32_t adc_clk_hz, qorc1mode, factor, qresult1, qresult2;
  uint32_t qbase = 0x3fed25e8;        /* 10^(-0.0005) in Q30 format */
  uint32_t onethird_q30 = 0x15555555; /* 1 / 3 in Q30 format */
  uint32_t one_q30 = 0x40000000;      /* 1 in Q30 format */

  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(kvti_factor);

  /* kvti_factor=44 * no_os_min(no_os_max(no_os_min(10**(-backoff/20), no_os_max(orc1mode,
   * (clkfreq/2e3)*(100e6/finmax))), 0.333),1) */
  adc_clk_hz = device->dev_info.adc_freq_hz;
  finmax = (finmax < adc_clk_hz / 20) ? 100000000ul : finmax;

  /* qresult1 = (clkfreq/2e3)*(100e6/finmax) */
  qorc1mode = AD9083_FIXED_INT2Q(orc1mode, 30);
  qresult1 = AD9083_FIXED_QDIV(adc_clk_hz, finmax * 20, 0, 0, 30);
  qresult1 = (qorc1mode > qresult1) ? qorc1mode : qresult1;

  /* qresult2 = 10**(-backoff/20) */
  qresult2 = qbase;
  if (backoff == 0) {
    qresult2 = 1073741824ul;
  } else {
    for (backoff -= 1; backoff != 0; backoff--) {
      qresult2 = AD9083_FIXED_QMUL(qresult2, qbase, 30, 30, 30);
    }
  }

  factor = (qresult1 > qresult2) ? qresult2 : qresult1;
  factor = (factor > onethird_q30) ? factor : onethird_q30;
  factor = (factor > one_q30) ? one_q30 : factor; /* 0.333333 ~ 1 */
  factor = AD9083_FIXED_QMUL(factor, 44, 30, 0,
                             26); /* kvti_factor: 14.52 ~ 44  QQ26 */

  *kvti_factor = factor;
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_fscale_fixed_get(adi_ad9083_device_t *device,
                                           uint8_t bcenter_os,
                                           uint32_t *fscale) {
  uint32_t adc_clk_hz;
  uint32_t qresult, qresult1, qresult2;
  uint32_t zerop3_q30 = 0x13333332;
  uint32_t zerop1_q30 = 0x6666666;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_NULL_POINTER_RETURN(fscale);

  adc_clk_hz = device->dev_info.adc_freq_hz;
  qresult = AD9083_FIXED_QDIV(adc_clk_hz, 1100000000ul, 0, 0, 30);
  qresult1 = AD9083_FIXED_QDIV(bcenter_os, 112, 0, 0, 30);
  qresult1 = 0x40000000 + qresult1;
  qresult = AD9083_FIXED_QMUL(qresult, qresult1, 30, 30, 30);

  qresult1 = AD9083_FIXED_QMUL(qresult, zerop3_q30, 30, 30, 30);
  qresult2 = AD9083_FIXED_QMUL(qresult, qresult, 30, 30, 30);
  qresult2 = AD9083_FIXED_QMUL(qresult2, zerop1_q30, 30, 30, 30);
  *fscale = qresult1 + qresult2; /* fscale: 0.35 ~ 0.87   Q30 */
  return 0;
}

int32_t adi_ad9083_rx_adc_vti_set(adi_ad9083_device_t *device, uint32_t fc,
                                  uint32_t vmax, uint8_t en_hp,
                                  uint32_t orc1mode, uint32_t backoff,
                                  uint64_t finmax, uint32_t *kgain,
                                  uint8_t *bcenter_os) {
  int32_t err;
  uint8_t kvti, kcap, bcenter;
  uint32_t kgain_value, kvti_factor, kvti_fixed, fscale, qresult;
  uint64_t temp, kcap_temp;
#ifdef R0
  uint64_t temp_h, temp_l, kgain_temp;
  uint64_t adc_clk_khz = device->dev_info.adc_freq_hz / 1000;
#endif
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  /* fscale = 0.3*(adc_clk_hz/1100.0) + 0.1*(adc_clk_hz/1100.0)**2.0 */
  /* Kvti=no_os_min(47,(floor(fscale*(1 + en_hp)*kvti_factor)/vmax)) */
  /* c_unit=11.5f */
  /* c_fix=70f */
  /* Kcap=no_os_max(no_os_min(63,round((1/(2*pi*lpf_freq*2000*c_unit)-(c_fix/c_unit)))),0)
   */
  /* kgain = (fscale*1.4)*(2^25) / (vmax * Kvti / (1 + en_hp)) */

#ifdef R0
  kvti_factor = 0x76000000; /* 29.5 in Q26 */
  bcenter = 10;
#endif
#ifdef R1
  err = adi_ad9083_rx_adc_kvti_factor_fixed_get(device, orc1mode, backoff,
                                                finmax, &kvti_factor);
  AD9083_ERROR_RETURN(err);
  qresult = AD9083_FIXED_QDIV(kvti_factor, 44, 26, 0, 26);
  qresult = AD9083_FIXED_QMUL(qresult, 10, 26, 0, 26);
  qresult = qresult + 0x2000000;
  bcenter = (uint8_t)(qresult >> 26);
#endif
  err = adi_ad9083_rx_adc_fscale_fixed_get(device, 0, &fscale);
  AD9083_ERROR_RETURN(err);

  kvti_fixed = AD9083_FIXED_QMUL(fscale, kvti_factor, 30, 26,
                                 26); /* Q26  max is 38.28 */
  qresult = AD9083_FIXED_QDIV((1 + en_hp) * 1000, vmax, 0, 0,
                              29); /* Q29  max is 4     */
  kvti_fixed =
      AD9083_FIXED_QMUL(kvti_fixed, qresult, 26, 29, 24); /* max is 153.12 */
  kvti = ((kvti_fixed >> 24) > 47) ? 47 : (kvti_fixed >> 24);

  temp = fc;
  temp = 2 * 314 * 2 * temp;
  kcap_temp = (uint64_t)1000000000000000ul - temp * 700;
#ifdef __KERNEL__
  kcap_temp = no_os_div_u64(kcap_temp + (temp / 2) * 115, temp * 115);
#else
  kcap_temp = (kcap_temp + (temp / 2) * 115) / (temp * 115);
#endif
  kcap = (kcap_temp > 63) ? 63 : (uint8_t)kcap_temp;

#ifdef R0
  kgain_temp =
      140928614 * adc_clk_khz * 1100000; /* 1.4 * (1 << 25) * 0.3 = 140928614 */
  temp = 4697620 *
         (adc_clk_khz * adc_clk_khz); /* 1.4 * (1 << 25) * 0.1 = 4697620 */
  adi_ad9083_hal_add_128((uint64_t)0, kgain_temp, (uint64_t)0, temp, &temp_h,
                         &temp_l);
  kgain_temp = ((uint64_t)1100 * 1100000 * (uint64_t)vmax * (uint64_t)kvti) /
               (1 + en_hp);
  adi_ad9083_hal_add_128(temp_h, temp_l, (uint64_t)0, kgain_temp / 2, &temp_h,
                         &temp_l);
  /*kgain_temp = (kgain_temp) * 1000 / (1100000 * 1100000 * vmax * kvti / (1 +
   * en_hp))*/
  adi_ad9083_hal_div_128(temp_h, temp_l, (uint64_t)0, kgain_temp, &temp_h,
                         &temp_l);
  kgain_value = (uint32_t)(temp_l & 0xffffffff);
#endif
#ifdef R1
  err = adi_ad9083_rx_adc_fscale_fixed_get(device, bcenter, &fscale);
  AD9083_ERROR_RETURN(err);
  qresult = 41943040; /* 1.25 * 33554432 */
  qresult = AD9083_FIXED_QDIV(qresult, vmax * kvti / (1 + en_hp), 0, 0, 12);
  qresult = AD9083_FIXED_QMUL(qresult, 1000, 12, 0, 8);
  qresult = AD9083_FIXED_QMUL(fscale, qresult, 30, 8, 8);
  kgain_value = (qresult >> 8);
#endif

  err = adi_ad9083_hal_bf_set(device, BF_BGAIN_INFO, kvti);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_BCAP_INFO, kcap);
  AD9083_ERROR_RETURN(err);
  *kgain = kgain_value;
  *bcenter_os = bcenter;

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_cal_set(adi_ad9083_device_t *device, uint16_t gcenter,
                                  uint32_t kgain, uint8_t bcenter_os) {
  int32_t err;
  uint32_t adc_clk_hz;
  uint8_t div_rc;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  adc_clk_hz = device->dev_info.adc_freq_hz;
  /* div_rc = round(adc_clk_hz / (8e6)) */
  div_rc = (adc_clk_hz + 4000000) / 8000000;

  err = adi_ad9083_hal_reg_set(device, 0x00000BCF, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BCE, 0x80);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BCB, 0x40);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BC9, 0x48);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_DIVM_RC_ADDR, div_rc);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BC7, 0x3F);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000BC4, 0x00001500, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_BCENTER_OFFSET_INFO, bcenter_os);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BBE, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BBD, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_KGAIN_VAL_INFO, kgain);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BB9, 0x83);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BB8, 0x83);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BB6, 0x04);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BB2, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000BB0, 0x00000A00, gcenter);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000BB1, 0x00000302, 7);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_CAL_EN_ADDR, 0x1F);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_POWER_DOWN_REG_ADDR, 0xDF);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 100);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000BC9, 0x00000100, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_ENABLE_CLOCK_INFO, 1);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_adc_config_set(adi_ad9083_device_t *device, uint32_t vmax,
                                     uint32_t fc, uint8_t rterm, uint8_t en_hp,
                                     uint32_t backoff, uint64_t finmax) {
  int32_t err;
  uint8_t gdith1, gdith2, bcenter_os;
  uint16_t gcenter;
  uint32_t fscale, gdith1_fixed, gdith2_fixed, gcenter_fixed, kgain;
  uint8_t orc1mode = 0;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_INVALID_PARAM_RETURN((vmax > 2000) || (vmax < 500));
  AD9083_INVALID_PARAM_RETURN(en_hp > 1);
  AD9083_INVALID_PARAM_RETURN(backoff > 18000);

  err = adi_ad9083_rx_adc_fscale_fixed_get(device, 0, &fscale);
  AD9083_ERROR_RETURN(err);
  gdith1_fixed = AD9083_FIXED_QMUL(fscale, 90, 30, 0, 24);
  gdith1_fixed = gdith1_fixed + 0x800000;
  gdith2_fixed = AD9083_FIXED_QMUL(fscale, 125, 30, 0, 24);
  gdith2_fixed = gdith2_fixed + 0x800000;
  gcenter_fixed = AD9083_FIXED_QMUL(fscale, 590, 30, 0, 22);
  gcenter_fixed = gcenter_fixed + 0x200000;

  gdith1 = gdith1_fixed >> 24;
  gdith2 = gdith2_fixed >> 24;
  gcenter = gcenter_fixed >> 22;

  /* Power down and disable all channels */
  err = adi_ad9083_hal_reg_set(device, REG_POWER_DOWN_REG_ADDR, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_adc_enable_set(device, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_adc_reset_enable_set(device, 0);
  AD9083_ERROR_RETURN(err);

  /* cal cap and gain */
  err = adi_ad9083_rx_adc_vti_set(device, fc, vmax, en_hp, orc1mode, backoff,
                                  finmax, &kgain, &bcenter_os);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_hal_reg_set(device, 0x00000B96, 0x02);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000B97, 0x35);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000B98, 0x02);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_reg_set(device, REG_DITHER_DAC_CURRENT1_REG_ADDR, gdith1);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_reg_set(device, REG_DITHER_DAC_CURRENT2_REG_ADDR, gdith2);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000B9B, 0x00000A00, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000B9D, 0xEB);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000B9E, 0x80);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000B9F, 0x80);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BA0, 0x80);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BA1, 0x00);
  AD9083_ERROR_RETURN(err);
  err =
      adi_ad9083_hal_reg_set(device, REG_VTI_SHIFT_CURRENT_MSB_REG_ADDR, 0x20);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, 0x00000BA3, 0x00000A00, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BA4, 0x10);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000BA5, 0x02);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_adc_term_res_set(device, rterm);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_bf_set(device, BF_EN_34_INFO, en_hp);
  AD9083_ERROR_RETURN(err);

  /* Config calibration */
  err = adi_ad9083_rx_adc_cal_set(device, gcenter, kgain, bcenter_os);
  AD9083_ERROR_RETURN(err);

  /* Enable all channels */
  err = adi_ad9083_rx_adc_enable_set(device, 0xFFFF);
  AD9083_ERROR_RETURN(err);

  /* Reset digital calibration */
  err = adi_ad9083_rx_adc_reset_cal(device);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_get_num_tones(adi_ad9083_device_t *device,
                                    uint64_t nco_freq_hz[3],
                                    uint8_t *tone_nums) {
  uint8_t num = 0;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();

  for (int i = 0; i < 3; i++) {
    if (nco_freq_hz[i] > 0) {
      num++;
    }
  }
  *tone_nums = num;

  if (((nco_freq_hz[0] == 0) &&
       ((nco_freq_hz[1] > 0) || (nco_freq_hz[2] > 0))) ||
      ((nco_freq_hz[1] == 0) && (nco_freq_hz[2] > 0))) {
    return API_CMS_ERROR_INVALID_PARAM;
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_rx_datapath_config_set(adi_ad9083_device_t *device,
                                          adi_ad9083_datapath_mode_e mode,
                                          uint8_t dec[4],
                                          uint64_t nco_freq_hz[3]) {
  int32_t err;
  uint8_t fbw_sel, cic_sel, average_sel, fbw_div_sel, cic_div_sel, out_div_sel;
  uint8_t sample_order, deci_adc_data, no_ddc_mode, burst_mode, zstuff_en,
      num_tones, nco_6db_gain;
  uint8_t i, cic_dec, non_zero_tones;
  uint64_t ftw, adc_clk_hz;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_LOG_FUNC();
  AD9083_INVALID_PARAM_RETURN(mode > AD9083_DATAPATH_ADC_CIC_NCO_G_H);
  AD9083_INVALID_PARAM_RETURN(dec[0] > AD9083_CIC_DEC_16);

  fbw_sel = 0;
  cic_sel = 0;
  average_sel = 0;
  fbw_div_sel = 0;
  cic_div_sel = 0;
  out_div_sel = 0;
  sample_order = 1;
  deci_adc_data = 0;
  no_ddc_mode = 0;
  burst_mode = 0;
  zstuff_en = 0;
  num_tones = 1;
  nco_6db_gain = 0;

  err = adi_ad9083_rx_get_num_tones(device, nco_freq_hz, &non_zero_tones);
  AD9083_ERROR_RETURN(err);

  if ((mode != AD9083_DATAPATH_ADC_CIC_NCO_G_H) &&
      ((mode != AD9083_DATAPATH_ADC_CIC_NCO_G))) {
    if (non_zero_tones > 1) {
      err = adi_ad9083_hal_log_write(
          device, ADI_CMS_LOG_WARN,
          "datapath mode not match non-zero NCO shift values.");
      AD9083_ERROR_RETURN(err);
    }
  }

  switch (mode) {
  case AD9083_DATAPATH_ADC_CIC:
    cic_sel = 1;
    cic_div_sel = 1;
    break;
  case AD9083_DATAPATH_ADC_CIC_NCO_J:
    nco_6db_gain = 1;
    /* not support J decimations > 16 when using the mixer */
    if (dec[1] > 16) {
      return API_CMS_ERROR_INVALID_PARAM;
    }
    break;
  case AD9083_DATAPATH_ADC_CIC_J:
    no_ddc_mode = 1;
    break;
  case AD9083_DATAPATH_ADC_J:
    deci_adc_data = 1;
    sample_order = 0;
    break;
  case AD9083_DATAPATH_ADC_CIC_NCO_G:
    nco_6db_gain = 1;
    average_sel = 1;
    break;
  case AD9083_DATAPATH_ADC_CIC_NCO_G_H:
    nco_6db_gain = 1;
    burst_mode = 1;
    num_tones = non_zero_tones;
    break;
  default:
    return API_CMS_ERROR_INVALID_PARAM;
  }
  /* disable datapath */
  err = adi_ad9083_rx_datapath_enable_set(device, 0);
  AD9083_ERROR_RETURN(err);

  /* diable Test Mode */
  err = adi_ad9083_rx_test_mode_res_set(device, 0, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_test_mode_type_set(device, 0, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_test_mode_sel_all_set(device, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_test_mode_enable_set(device, 0);
  AD9083_ERROR_RETURN(err);

  /* default setings */
  err = adi_ad9083_rx_dformat_res_set(device, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_nco_adither_enable_set(device, 0, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_nco_adither_enable_set(device, 1, 1);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_nco_adither_enable_set(device, 2, 1);
  AD9083_ERROR_RETURN(err);

  /* set mux based on user */
  err = adi_ad9083_rx_clk_sample_order_sel(device, sample_order);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_dformat_fbw_sel_all_set(device, fbw_sel);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_dformat_cic_sel_all_set(device, cic_sel);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_dformat_average_sel_all_set(device, average_sel);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_dformat_fbw_div_sel_set(device, fbw_div_sel);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_dformat_cic_div_sel_set(device, cic_div_sel);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_dformat_out_div_sel_set(device, out_div_sel);
  AD9083_ERROR_RETURN(err);

  err = adi_ad9083_rx_nco_6db_gain_enable_set(device, nco_6db_gain);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_bypass_cic_set(device, deci_adc_data);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_bypass_mixer_set(device, no_ddc_mode);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_burst_mode_set(device, burst_mode);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_num_tones_set(device, num_tones);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_zero_stuff_enable_set(device, zstuff_en);
  AD9083_ERROR_RETURN(err);

  /* set decimation */
  err = adi_ad9083_rx_cic_dec_rate_set(device, dec[0]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_j_dec_rate_set(device, dec[1]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_g_value_set(device, dec[2]);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_rx_h_value_set(device, dec[3]);
  AD9083_ERROR_RETURN(err);

  /* set ftw */
  cic_dec = 4 << dec[0];
  adc_clk_hz = device->dev_info.adc_freq_hz;

  for (i = 0; i < 3; i++) {
    ftw = 128 * nco_freq_hz[i] * cic_dec;
#ifdef __KERNEL__
    ftw = (uint8_t)div64_u64(ftw + adc_clk_hz / 2, adc_clk_hz);
#else
    ftw = (ftw + adc_clk_hz / 2) / (adc_clk_hz);
#endif
    err = adi_ad9083_rx_nco_ftw_set(device, i, (uint8_t)ftw);
    AD9083_ERROR_RETURN(err);
  }

  err = adi_ad9083_hal_reg_set(device, REG_CIC_GAIN_ADJ_VALUE_0_ADDR, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000153, 0);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, 0x00000154, 2);
  AD9083_ERROR_RETURN(err);

  /* enable datapath */
  err = adi_ad9083_rx_datapath_enable_set(device, 1);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

/*! @} */