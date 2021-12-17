/*!
 * @brief     APIs to call HAL functions
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9081_HAL_API
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

/**
 * \brief Open and initialize resources and peripherals required for the TxFE device.
 *
 * \param[in]  device	         Pointer to device handler structure.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, API_CMS_ERROR_HW_OPEN failure code.
 */
int32_t adi_ad9081_hal_hw_open(adi_ad9081_device_t *device);

/**
 * \brief Shutdown and close any resources opened by adi_aegir_hal_hw_open.
 *
 * \param[in]  device	         Pointer to device handler structure.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, API_CMS_ERROR_HW_CLOSE failure code.
 */
int32_t adi_ad9081_hal_hw_close(adi_ad9081_device_t *device);

/**
 * \brief Perform a wait/thread sleep in units of microseconds.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  us                Delay duration in microseconds.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, API_CMS_ERROR_DELAY_US failure code.
 */
int32_t adi_ad9081_hal_delay_us(adi_ad9081_device_t *device, uint32_t us);

/**
 * \brief Set ADI device RESETB pin high or low.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  enable            0: set RESETB pin low, 1: set RESETB pin high.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, API_CMS_ERROR_RESET_PIN_CTRL failure code.
 */
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

int32_t adi_ad9081_hal_2bf_get(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint8_t *value0, uint32_t info1,
			       uint8_t *value1, uint8_t value_size_bytes);
int32_t adi_ad9081_hal_3bf_get(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint8_t *value0, uint32_t info1,
			       uint8_t *value1, uint32_t info2, uint8_t *value2,
			       uint8_t value_size_bytes);
int32_t adi_ad9081_hal_4bf_get(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint8_t *value0, uint32_t info1,
			       uint8_t *value1, uint32_t info2, uint8_t *value2,
			       uint32_t info3, uint8_t *value3,
			       uint8_t value_size_bytes);
int32_t adi_ad9081_hal_5bf_get(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint8_t *value0, uint32_t info1,
			       uint8_t *value1, uint32_t info2, uint8_t *value2,
			       uint32_t info3, uint8_t *value3, uint32_t info4,
			       uint8_t *value4, uint8_t value_size_bytes);
int32_t adi_ad9081_hal_6bf_get(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint8_t *value0, uint32_t info1,
			       uint8_t *value1, uint32_t info2, uint8_t *value2,
			       uint32_t info3, uint8_t *value3, uint32_t info4,
			       uint8_t *value4, uint32_t info5, uint8_t *value5,
			       uint8_t value_size_bytes);
int32_t adi_ad9081_hal_7bf_get(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint8_t *value0, uint32_t info1,
			       uint8_t *value1, uint32_t info2, uint8_t *value2,
			       uint32_t info3, uint8_t *value3, uint32_t info4,
			       uint8_t *value4, uint32_t info5, uint8_t *value5,
			       uint32_t info6, uint8_t *value6,
			       uint8_t value_size_bytes);
int32_t adi_ad9081_hal_8bf_get(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint8_t *value0, uint32_t info1,
			       uint8_t *value1, uint32_t info2, uint8_t *value2,
			       uint32_t info3, uint8_t *value3, uint32_t info4,
			       uint8_t *value4, uint32_t info5, uint8_t *value5,
			       uint32_t info6, uint8_t *value6, uint32_t info7,
			       uint8_t *value7, uint8_t value_size_bytes);

int32_t adi_ad9081_hal_2bf_set(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint64_t value0, uint32_t info1,
			       uint64_t value1);
int32_t adi_ad9081_hal_3bf_set(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint64_t value0, uint32_t info1,
			       uint64_t value1, uint32_t info2,
			       uint64_t value2);
int32_t adi_ad9081_hal_4bf_set(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint64_t value0, uint32_t info1,
			       uint64_t value1, uint32_t info2, uint64_t value2,
			       uint32_t info3, uint64_t value3);
int32_t adi_ad9081_hal_5bf_set(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint64_t value0, uint32_t info1,
			       uint64_t value1, uint32_t info2, uint64_t value2,
			       uint32_t info3, uint64_t value3, uint32_t info4,
			       uint64_t value4);
int32_t adi_ad9081_hal_6bf_set(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint64_t value0, uint32_t info1,
			       uint64_t value1, uint32_t info2, uint64_t value2,
			       uint32_t info3, uint64_t value3, uint32_t info4,
			       uint64_t value4, uint32_t info5,
			       uint64_t value5);
int32_t adi_ad9081_hal_7bf_set(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint64_t value0, uint32_t info1,
			       uint64_t value1, uint32_t info2, uint64_t value2,
			       uint32_t info3, uint64_t value3, uint32_t info4,
			       uint64_t value4, uint32_t info5, uint64_t value5,
			       uint32_t info6, uint64_t value6);
int32_t adi_ad9081_hal_8bf_set(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t info0, uint64_t value0, uint32_t info1,
			       uint64_t value1, uint32_t info2, uint64_t value2,
			       uint32_t info3, uint64_t value3, uint32_t info4,
			       uint64_t value4, uint32_t info5, uint64_t value5,
			       uint32_t info6, uint64_t value6, uint32_t info7,
			       uint64_t value7);

int32_t adi_ad9081_hal_multi_bf_get(adi_ad9081_device_t *device, uint32_t reg,
				    uint32_t *info, uint8_t **value,
				    uint8_t value_size_bytes, uint8_t num_bfs);
int32_t adi_ad9081_hal_multi_bf_set(adi_ad9081_device_t *device, uint32_t reg,
				    uint32_t *info, uint64_t *value,
				    uint8_t num_bfs);

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

int32_t adi_ad9081_hal_calc_nco_ftw(adi_ad9081_device_t *device, uint64_t freq,
				    int64_t nco_shift, uint64_t *ftw,
				    uint64_t *a, uint64_t *b);
#if AD9081_USE_FLOATING_TYPE > 0
int32_t adi_ad9081_hal_calc_nco_ftw_f(adi_ad9081_device_t *device, double freq,
				      double nco_shift, uint64_t *ftw,
				      uint64_t *a, uint64_t *b);
#endif
int32_t adi_ad9081_hal_calc_rx_nco_ftw(adi_ad9081_device_t *device,
				       uint64_t adc_freq, int64_t nco_shift,
				       uint64_t *ftw);
int32_t adi_ad9081_hal_calc_tx_nco_ftw(adi_ad9081_device_t *device,
				       uint64_t dac_freq, int64_t nco_shift,
				       uint64_t *ftw);
int32_t adi_ad9081_hal_calc_rx_nco_ftw32(adi_ad9081_device_t *device,
					 uint64_t adc_freq, int64_t nco_shift,
					 uint64_t *ftw);
int32_t adi_ad9081_hal_calc_tx_nco_ftw32(adi_ad9081_device_t *device,
					 uint64_t dac_freq, int64_t nco_shift,
					 uint64_t *ftw);

#ifdef __cplusplus
}
#endif

#endif /* __AD9081_REG_H__ */

/*! @} */