/*!
 * @brief     APIs to call HAL functions
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup __AD9081_HAL__
 * @{
 */
#ifndef __AD9081_HAL_H__
#define __AD9081_HAL_H__

/*============= I N C L U D E S ============*/
#include "adi_ad9081_config.h"
#ifdef __KERNEL__
#include <linux/math64.h>
#endif

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

int32_t adi_ad9081_hal_hw_open(adi_ad9081_device_t *device);
int32_t adi_ad9081_hal_hw_close(adi_ad9081_device_t *device);
int32_t adi_ad9081_hal_delay_us(adi_ad9081_device_t *device, uint32_t us);
int32_t adi_ad9081_hal_reset_pin_ctrl(adi_ad9081_device_t *device,
				      uint8_t enable);
int32_t adi_ad9081_hal_log_write(adi_ad9081_device_t *device,
				 adi_cms_log_type_e type, const char *comment,
				 ...);

int32_t adi_ad9081_hal_bf_get(adi_ad9081_device_t *device, uint32_t reg,
			      uint32_t info, uint8_t *value,
			      uint8_t value_size_bytes);
int32_t adi_ad9081_hal_bf_set(adi_ad9081_device_t *device, uint32_t reg,
			      uint32_t info, uint64_t value);

int32_t adi_ad9081_hal_reg_get(adi_ad9081_device_t *device, uint32_t reg,
			       uint8_t *data);
int32_t adi_ad9081_hal_reg_set(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t data);

int32_t adi_ad9081_hal_cbusjrx_reg_get(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t *data,
				       uint8_t lane);
int32_t adi_ad9081_hal_cbusjrx_reg_set(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t data,
				       uint8_t lane);

int32_t adi_ad9081_hal_cbusjtx_reg_get(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t *data,
				       uint8_t lane);
int32_t adi_ad9081_hal_cbusjtx_reg_set(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t data,
				       uint8_t lane);

int32_t adi_ad9081_hal_cbuspll_reg_get(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t *data);
int32_t adi_ad9081_hal_cbuspll_reg_set(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t data);

int32_t adi_ad9081_hal_bf_wait_to_clear(adi_ad9081_device_t *device,
					uint32_t reg, uint32_t info);
int32_t adi_ad9081_hal_bf_wait_to_set(adi_ad9081_device_t *device, uint32_t reg,
				      uint32_t info);

int32_t adi_ad9081_hal_error_report(adi_ad9081_device_t *device,
				    adi_cms_log_type_e log_type, int32_t error,
				    const char *file_name,
				    const char *func_name, uint32_t line_num,
				    const char *var_name, const char *comment);

int32_t adi_ad9081_hal_calc_rx_nco_ftw(adi_ad9081_device_t *device,
				       uint64_t adc_freq, int64_t nco_shift,
				       uint64_t *ftw);
int32_t adi_ad9081_hal_calc_tx_nco_ftw(adi_ad9081_device_t *device,
				       uint64_t dac_freq, int64_t nco_shift,
				       uint64_t *ftw);
int32_t adi_ad9081_hal_calc_tx_nco_ftw32(adi_ad9081_device_t *device,
					 uint64_t dac_freq, int64_t nco_shift,
					 uint64_t *ftw);

#ifdef __cplusplus
}
#endif

#endif /* __AD9081_REG_H__ */

/*! @} */