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
#ifndef __AD9083_HAL_H__
#define __AD9083_HAL_H__

/*============= I N C L U D E S ============*/
#include "adi_ad9083_config.h"
#ifdef __KERNEL__
#include <linux/math64.h>
#endif

/*============= D E F I N E S ==============*/
#define SPI_IN_OUT_BUFF_SZ 0x3

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

int32_t adi_ad9083_hal_hw_open(adi_ad9083_device_t *device);
int32_t adi_ad9083_hal_hw_close(adi_ad9083_device_t *device);
int32_t adi_ad9083_hal_delay_us(adi_ad9083_device_t *device, uint32_t us);
int32_t adi_ad9083_hal_reset_pin_ctrl(adi_ad9083_device_t *device,
                                      uint8_t enable);
int32_t adi_ad9083_hal_log_write(adi_ad9083_device_t *device,
                                 adi_cms_log_type_e type, const char *comment,
                                 ...);

int32_t adi_ad9083_hal_bf_get(adi_ad9083_device_t *device, uint32_t reg,
                              uint32_t info, uint8_t *value,
                              uint8_t value_size_bytes);
int32_t adi_ad9083_hal_bf_set(adi_ad9083_device_t *device, uint32_t reg,
                              uint32_t info, uint64_t value);

int32_t adi_ad9083_hal_reg_get(adi_ad9083_device_t *device, uint32_t reg,
                               uint8_t *data);
int32_t adi_ad9083_hal_reg_set(adi_ad9083_device_t *device, uint32_t reg,
                               uint8_t data);

int32_t adi_ad9083_hal_error_report(adi_ad9083_device_t *device,
                                    adi_cms_log_type_e log_type, int32_t error,
                                    const char *file_name,
                                    const char *func_name, uint32_t line_num,
                                    const char *var_name, const char *comment);

int32_t adi_ad9083_hal_cbuspll_reg_set(adi_ad9083_device_t *device,
                                       uint32_t reg, uint8_t data);
int32_t adi_ad9083_hal_cbuspll_reg_get(adi_ad9083_device_t *device,
                                       uint32_t reg, uint8_t *data);

int32_t adi_ad9083_hal_div_nume_deno(adi_ad9083_device_t *device, uint64_t a,
                                     uint64_t b, uint8_t *integer,
                                     uint8_t *nume, uint8_t *deno);

void adi_ad9083_hal_add_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
                            uint64_t *hi, uint64_t *lo);
void adi_ad9083_hal_subt_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
                             uint64_t *hi, uint64_t *lo);
void adi_ad9083_hal_mult_128(uint64_t a, uint64_t b, uint64_t *hi,
                             uint64_t *lo);
void adi_ad9083_hal_lshift_128(uint64_t *hi, uint64_t *lo);
void adi_ad9083_hal_rshift_128(uint64_t *hi, uint64_t *lo);
void adi_ad9083_hal_div_128(uint64_t a_hi, uint64_t a_lo, uint64_t b_hi,
                            uint64_t b_lo, uint64_t *hi, uint64_t *lo);

#ifdef __cplusplus
}
#endif

#endif /* __AD9083_HAL__ */

/*! @} */