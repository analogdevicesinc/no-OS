// SPDX-License-Identifier: GPL-2.0
/*!
 * @brief     APIs to call HAL functions
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9083_HAL
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9083_hal.h"

/*============= C O D E ====================*/
int32_t adi_ad9083_hal_hw_open(adi_ad9083_device_t *device) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(device->hal_info.hw_open);
  if (API_CMS_ERROR_OK !=
      device->hal_info.hw_open(device->hal_info.user_data)) {
    return API_CMS_ERROR_HW_OPEN;
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_hw_close(adi_ad9083_device_t *device) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(device->hal_info.hw_close);
  if (API_CMS_ERROR_OK !=
      device->hal_info.hw_close(device->hal_info.user_data)) {
    return API_CMS_ERROR_HW_CLOSE;
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_delay_us(adi_ad9083_device_t *device, uint32_t us) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(device->hal_info.delay_us);
  if (API_CMS_ERROR_OK !=
      device->hal_info.delay_us(device->hal_info.user_data, us)) {
    return API_CMS_ERROR_DELAY_US;
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_reset_pin_ctrl(adi_ad9083_device_t *device,
                                      uint8_t enable) {
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(device->hal_info.reset_pin_ctrl);
  if (API_CMS_ERROR_OK !=
      device->hal_info.reset_pin_ctrl(device->hal_info.user_data, enable)) {
    return API_CMS_ERROR_RESET_PIN_CTRL;
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_log_write(adi_ad9083_device_t *device,
                                 adi_cms_log_type_e log_type,
                                 const char *comment, ...) {
  int32_t err;
  va_list argp;
  AD9083_NULL_POINTER_RETURN(device);
  if (device->hal_info.log_write == NULL) {
    return API_CMS_ERROR_NULL_PARAM;
  }
  va_start(argp, comment);
  err = device->hal_info.log_write(device->hal_info.user_data, log_type,
                                   comment, argp);
  va_end(argp);

  return err;
}

int32_t adi_ad9083_hal_bf_get(adi_ad9083_device_t *device, uint32_t reg,
                              uint32_t info, uint8_t *value,
                              uint8_t value_size_bytes) {
  int32_t err;
  uint8_t reg_offset = 0, data8 = 0;
  uint8_t offset = (uint8_t)(info >> 0), width = (uint8_t)(info >> 8);
  uint32_t mask = 0, endian_test_val = 0x11223344;
  uint64_t bf_val = 0;
  uint8_t reg_bytes =
      ((width + offset) >> 3) + (((width + offset) & 7) == 0 ? 0 : 1);
  uint8_t i = 0, j = 0, filled_bits = 0;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(value);
  AD9083_INVALID_PARAM_RETURN(width > 64);
  AD9083_INVALID_PARAM_RETURN(width < 1);
  AD9083_INVALID_PARAM_RETURN(value_size_bytes > 8);

  if (reg < 0x1000) {
    for (reg_offset = 0; reg_offset < reg_bytes; reg_offset++) {
      err = adi_ad9083_hal_reg_get(device, reg + reg_offset, &data8);
      AD9083_ERROR_RETURN(err);
      if ((offset + width) <= 8) { /* last 8bits */
        mask = (1 << width) - 1;
        data8 = (data8 >> offset) & mask;
        bf_val = bf_val + ((uint64_t)data8 << filled_bits);
        filled_bits = filled_bits + width;
      } else {
        mask = (1 << (8 - offset)) - 1;
        data8 = (data8 >> offset) & mask;
        bf_val = bf_val + ((uint64_t)data8 << filled_bits);
        width = offset + width - 8;
        filled_bits = filled_bits + (8 - offset);
        offset = 0;
      }
    }
  }

  /* save bitfield value to buffer */
  for (i = 0; i < value_size_bytes; i++) {
    j = (*(uint8_t *)&endian_test_val == 0x44) ? (i)
                                               : (value_size_bytes - 1 - i);
    value[j] = (uint8_t)(bf_val >> (i << 3));
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_bf_set(adi_ad9083_device_t *device, uint32_t reg,
                              uint32_t info, uint64_t value) {
  int32_t err;
  uint8_t reg_offset = 0, data8 = 0;
  uint8_t offset = (uint8_t)(info >> 0), width = (uint8_t)(info >> 8);
  uint32_t mask = 0;
  uint8_t reg_bytes =
      ((width + offset) >> 3) + (((width + offset) & 7) == 0 ? 0 : 1);
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_INVALID_PARAM_RETURN(width > 64);
  AD9083_INVALID_PARAM_RETURN(width < 1);

  if (reg < 0x1000) {
    for (reg_offset = 0; reg_offset < reg_bytes; reg_offset++) {
      if ((offset + width) <= 8) { /* last 8bits */
        if ((offset > 0) || ((offset + width) < 8)) {
          err = adi_ad9083_hal_reg_get(device, reg + reg_offset, &data8);
          AD9083_ERROR_RETURN(err);
        }
        mask = (1 << width) - 1;
        data8 = data8 & (~(mask << offset));
        data8 = data8 | ((value & mask) << offset);
      } else {
        if (offset > 0) {
          err = adi_ad9083_hal_reg_get(device, reg + reg_offset, &data8);
          AD9083_ERROR_RETURN(err);
        }
        mask = (1 << (8 - offset)) - 1;
        data8 = data8 & (~(mask << offset));
        data8 = data8 | ((value & mask) << offset);
        value = value >> (8 - offset);
        width = offset + width - 8;
        offset = 0;
      }
      err = adi_ad9083_hal_reg_set(device, reg + reg_offset, data8);
      AD9083_ERROR_RETURN(err);
    }
  }

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_reg_get(adi_ad9083_device_t *device, uint32_t reg,
                               uint8_t *data) {
  uint8_t in_data[SPI_IN_OUT_BUFF_SZ] = {0};
  uint8_t out_data[SPI_IN_OUT_BUFF_SZ] = {0};
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(device->hal_info.spi_xfer);
  AD9083_NULL_POINTER_RETURN(data);

  if (reg < 0x1000) {
    in_data[0] = (((reg >> 8) | 0x80) & 0xFF);
    in_data[1] = (reg & 0xFF);
    if (API_CMS_ERROR_OK !=
        device->hal_info.spi_xfer(device->hal_info.user_data, in_data, out_data,
                                  SPI_IN_OUT_BUFF_SZ)) {
      return API_CMS_ERROR_SPI_XFER;
    }
    *data = out_data[2];
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_reg_set(adi_ad9083_device_t *device, uint32_t reg,
                               uint8_t data) {
  uint8_t in_data[SPI_IN_OUT_BUFF_SZ] = {0};
  uint8_t out_data[SPI_IN_OUT_BUFF_SZ] = {0};
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(device->hal_info.spi_xfer);

  if (reg < 0x1000) {
    in_data[0] = ((reg >> 8) & 0xFF);
    in_data[1] = (reg & 0xFF);
    in_data[2] = data;
    if (API_CMS_ERROR_OK !=
        device->hal_info.spi_xfer(device->hal_info.user_data, in_data, out_data,
                                  SPI_IN_OUT_BUFF_SZ)) {
      return API_CMS_ERROR_SPI_XFER;
    }
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_error_report(adi_ad9083_device_t *device,
                                    adi_cms_log_type_e log_type, int32_t error,
                                    const char *file_name,
                                    const char *func_name, uint32_t line_num,
                                    const char *var_name, const char *comment) {
  if (device == NULL) {
    return API_CMS_ERROR_NULL_PARAM;
  }
  if (API_CMS_ERROR_OK !=
      adi_ad9083_hal_log_write(device, log_type,
                               "%s, \"%s\" in %s(...), line%d in %s", comment,
                               var_name, func_name, line_num, file_name)) {
    return API_CMS_ERROR_LOG_WRITE;
  }
  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_cbuspll_reg_get(adi_ad9083_device_t *device,
                                       uint32_t reg, uint8_t *data) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(data);

  err = adi_ad9083_hal_reg_set(device, REG_CBUS_REN_LCPLL_ADDR, 0x00);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_CBUS_ADDR_LCPLL_ADDR, reg);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_CBUS_REN_LCPLL_ADDR, 0x01);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 100000);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_get(device, REG_CBUS_RDATA_LCPLL_ADDR, data);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_CBUS_REN_LCPLL_ADDR, 0x00);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_cbuspll_reg_set(adi_ad9083_device_t *device,
                                       uint32_t reg, uint8_t data) {
  int32_t err;
  AD9083_NULL_POINTER_RETURN(device);

  err = adi_ad9083_hal_reg_set(device, REG_CBUS_WSTROBE_LCPLL_ADDR, 0x00);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_CBUS_ADDR_LCPLL_ADDR, reg);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_CBUS_WDATA_LCPLL_ADDR, data);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 100000);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_CBUS_WSTROBE_LCPLL_ADDR, 0x01);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_reg_set(device, REG_CBUS_WSTROBE_LCPLL_ADDR, 0x00);
  AD9083_ERROR_RETURN(err);
  err = adi_ad9083_hal_delay_us(device, 100000);
  AD9083_ERROR_RETURN(err);

  return API_CMS_ERROR_OK;
}

int32_t adi_ad9083_hal_div_nume_deno(adi_ad9083_device_t *device, uint64_t a,
                                     uint64_t b, uint8_t *integer,
                                     uint8_t *nume, uint8_t *deno) {
  uint8_t integer_temp, nume_index, deno_index;
  uint64_t frac, min_diff;
  int64_t diff;
  uint8_t golden_nume, golden_deno;
  AD9083_NULL_POINTER_RETURN(device);
  AD9083_NULL_POINTER_RETURN(integer);
  AD9083_NULL_POINTER_RETURN(nume);
  AD9083_NULL_POINTER_RETURN(deno);
  AD9083_INVALID_PARAM_RETURN(b == 0);

#ifdef __KERNEL__
  integer_temp = div64_u64(a, b);
#else
  integer_temp = a / b;
#endif
  frac = a - integer_temp * b;

  if (frac == 0) {
    *nume = 0;
    *deno = 1;
  } else {
    min_diff = frac * 1000;
    for (deno_index = 2; deno_index < 32; deno_index++)
      for (nume_index = 0; nume_index < deno_index; nume_index++) {
        diff = frac * 1000 - (b * 1000 * nume_index) / deno_index;
        if (diff < 0) {
          diff = -diff;
        }
        if (min_diff > diff) {
          min_diff = diff;
          golden_deno = deno_index;
          golden_nume = nume_index;
        }
      }
    *nume = golden_nume;
    *deno = golden_deno;
  }
  *integer = integer_temp;

  return API_CMS_ERROR_OK;
}

void adi_ad9083_hal_add_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
                            uint64_t *hi, uint64_t *lo) {
  uint64_t rl = al + bl, rh = ah + bh;
  if (rl < al)
    rh++;
  *lo = rl;
  *hi = rh;
}

void adi_ad9083_hal_subt_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
                             uint64_t *hi, uint64_t *lo) {
  uint64_t rl, rh;
  if (bl <= al) {
    rl = al - bl;
    rh = ah - bh;
  } else {
    rl = bl - al - 1;
    rl = 0xffffffffffffffffull - rl;
    ah--;
    rh = ah - bh;
  }
  *lo = rl;
  *hi = rh;
}

void adi_ad9083_hal_mult_128(uint64_t a, uint64_t b, uint64_t *hi,
                             uint64_t *lo) {
  uint64_t ah = a >> 32, al = a & 0xffffffff, bh = b >> 32, bl = b & 0xffffffff,
           rh = ah * bh, rl = al * bl, rm1 = ah * bl, rm2 = al * bh,
           rm1h = rm1 >> 32, rm2h = rm2 >> 32, rm1l = rm1 & 0xffffffff,
           rm2l = rm2 & 0xffffffff, rmh = rm1h + rm2h, rml = rm1l + rm2l,
           c = ((rl >> 32) + rml) >> 32;
  rl = rl + (rml << 32);
  rh = rh + rmh + c;
  *lo = rl;
  *hi = rh;
}

void adi_ad9083_hal_lshift_128(uint64_t *hi, uint64_t *lo) {
  *hi <<= 1;
  if (*lo & 0x8000000000000000ull)
    *hi |= 1ull;
  *lo <<= 1;
}

void adi_ad9083_hal_rshift_128(uint64_t *hi, uint64_t *lo) {
  *lo >>= 1;
  if (*hi & 1ull)
    *lo |= 0x8000000000000000ull;
  *hi >>= 1;
}

void adi_ad9083_hal_div_128(uint64_t a_hi, uint64_t a_lo, uint64_t b_hi,
                            uint64_t b_lo, uint64_t *hi, uint64_t *lo) {
  uint64_t remain_lo = a_lo, remain_hi = a_hi, part1_lo = b_lo, part1_hi = b_hi;
  uint64_t result_lo = 0, result_hi = 0, mask_lo = 1, mask_hi = 0;

  while (!(part1_hi & 0x8000000000000000ull)) {
    adi_ad9083_hal_lshift_128(&part1_hi, &part1_lo);
    adi_ad9083_hal_lshift_128(&mask_hi, &mask_lo);
  }

  do {
    if ((remain_hi > part1_hi) ||
        ((remain_hi == part1_hi) && (remain_lo >= part1_lo))) {
      adi_ad9083_hal_subt_128(remain_hi, remain_lo, part1_hi, part1_lo,
                              &remain_hi, &remain_lo);
      adi_ad9083_hal_add_128(result_hi, result_lo, mask_hi, mask_lo, &result_hi,
                             &result_lo);
    }
    adi_ad9083_hal_rshift_128(&part1_hi, &part1_lo);
    adi_ad9083_hal_rshift_128(&mask_hi, &mask_lo);
  } while ((mask_hi != 0) || (mask_lo != 0));
  *lo = result_lo;
  *hi = result_hi;
}

/*! @} */