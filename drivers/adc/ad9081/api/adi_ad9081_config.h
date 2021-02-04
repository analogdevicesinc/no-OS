/*!
 * @brief     device configuration header
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup ADI_AD9081_INTERNAL_CONFIG
 * @{
 */
#ifndef __ADI_AD9081_CONFIG_H__
#define __ADI_AD9081_CONFIG_H__

/*============= D E F I N E S ==============*/

/*============= I N C L U D E S ============*/
#include "adi_ad9081.h"
#include "adi_ad9081_bf_impala_tc.h"
#include "adi_ad9081_bf_jrxa_des.h"
#include "adi_ad9081_bf_jtx_dual_link.h"
#include "adi_ad9081_bf_jtx_qbf_ad9081.h"
#include "adi_ad9081_bf_lcpll_28nm.h"
#include "adi_ad9081_bf_main.h"
#include "adi_ad9081_bf_nb_coarse_nco.h"
#include "adi_ad9081_bf_nb_ddc_dformat.h"
#include "adi_ad9081_bf_nb_fine_nco.h"
#include "adi_ad9081_bf_rx_paging.h"
#include "adi_ad9081_bf_ser_phy.h"
#include "adi_ad9081_bf_spi_only_up.h"
#include "adi_ad9081_bf_ad9081.h"

/*============= D E F I N E S ==============*/
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ == 199901L)
#define __FUNCTION_NAME__ __func__
#else
#define __FUNCTION_NAME__ __FUNCTION__
#endif

#define AD9081_API_REV 0x00010007

/* var error report */
#define AD9081_MSG_REPORT(var, comment)                                        \
	adi_ad9081_hal_error_report(device, ADI_CMS_LOG_MSG, API_CMS_ERROR_OK, \
				    __FILE__, __FUNCTION_NAME__, __LINE__,     \
				    #var, comment)
#define AD9081_WARN_REPORT(var, comment)                                       \
	adi_ad9081_hal_error_report(device, ADI_CMS_LOG_WARN,                  \
				    API_CMS_ERROR_OK, __FILE__,                \
				    __FUNCTION_NAME__, __LINE__, #var,         \
				    comment)
#define AD9081_ERROR_REPORT(error, var, comment)                               \
	adi_ad9081_hal_error_report(device, ADI_CMS_LOG_ERR, error, __FILE__,  \
				    __FUNCTION_NAME__, __LINE__, #var,         \
				    comment)

/* log report */
#define AD9081_LOG_FUNC()                                                      \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_API, "%s(...)",           \
				 __FUNCTION_NAME__)
#define AD9081_LOG_SPIR(addr, data)                                            \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_SPI,                      \
				 "ad9081: r@%.4x = %.2x", addr, data)
#define AD9081_LOG_SPIW(addr, data)                                            \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_SPI,                      \
				 "ad9081: w@%.4x = %.2x", addr, data)
#define AD9081_LOG_SPIR32(addr, data)                                          \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_SPI,                      \
				 "ad9081: r32@%.4x = %.8x", addr, data)
#define AD9081_LOG_SPIW32(addr, data)                                          \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_SPI,                      \
				 "ad9081: w32@%.4x = %.8x", addr, data)
#define AD9081_LOG_VAR(type, msg, ...)                                         \
	adi_ad9081_hal_log_write(device, type, msg, ##__VA_ARGS__)
#define AD9081_LOG_MSG(msg)                                                    \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_MSG, msg)
#define AD9081_LOG_WARN(msg)                                                   \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_WARN, msg)
#define AD9081_LOG_ERR(msg)                                                    \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_ERR, msg)

/* var error check */
#define AD9081_ERROR_RETURN(r)                                                 \
	{                                                                      \
		if (r != API_CMS_ERROR_OK) {                                   \
			return r;                                              \
		}                                                              \
	}
#define AD9081_NULL_POINTER_RETURN(p)                                          \
	{                                                                      \
		if (p == NULL) {                                               \
			AD9081_ERROR_REPORT(API_CMS_ERROR_NULL_PARAM, p,       \
					    "Null pointer passed.");           \
			return API_CMS_ERROR_NULL_PARAM;                       \
		}                                                              \
	}
#define AD9081_INVALID_PARAM_RETURN(r)                                         \
	{                                                                      \
		if (r) {                                                       \
			AD9081_ERROR_REPORT(API_CMS_ERROR_INVALID_PARAM, r,    \
					    "Invalid param passed.");          \
			return API_CMS_ERROR_INVALID_PARAM;                    \
		}                                                              \
	}
#define AD9081_INVALID_PARAM_WARN(r)                                           \
	{                                                                      \
		if (r) {                                                       \
			AD9081_WARN_REPORT(r, "Invalid param passed.");        \
		}                                                              \
	}

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

int32_t adi_ad9081_device_clk_pll_enable_set(adi_ad9081_device_t *device,
					     uint8_t pll_en);
int32_t adi_ad9081_device_clk_pll_div_set(adi_ad9081_device_t *device,
					  uint8_t ref_div, uint8_t m_div,
					  uint8_t pll_div, uint8_t fb_div);
int32_t adi_ad9081_device_clk_pll_startup(adi_ad9081_device_t *device,
					  uint64_t dac_clk_hz,
					  uint64_t ref_clk_hz);
int32_t adi_ad9081_device_clk_up_div_set(adi_ad9081_device_t *device,
					 uint64_t dac_clk_hz);
int32_t adi_ad9081_device_laminate_id_get(adi_ad9081_device_t *device,
					  uint8_t *id);
int32_t adi_ad9081_device_die_id_get(adi_ad9081_device_t *device, uint8_t *id);
int32_t adi_ad9081_device_power_status_check(adi_ad9081_device_t *device);
int32_t adi_ad9081_device_reg8_access_check(adi_ad9081_device_t *device);
int32_t adi_ad9081_device_reg32_access_check(adi_ad9081_device_t *device);
int32_t adi_ad9081_device_boot_pre_clock(adi_ad9081_device_t *device);
int32_t adi_ad9081_device_boot_post_clock(adi_ad9081_device_t *device);
int32_t adi_ad9081_device_nco_sync_mode_set(adi_ad9081_device_t *device,
					    uint8_t mode);
int32_t
adi_ad9081_device_nco_sync_trigger_source_set(adi_ad9081_device_t *device,
					      uint8_t source);
int32_t adi_ad9081_device_nco_sync_gpio_set(adi_ad9081_device_t *device,
					    uint8_t gpio_index, uint8_t output);
int32_t
adi_ad9081_device_nco_sync_extra_lmfc_num_set(adi_ad9081_device_t *device,
					      uint8_t num);
int32_t adi_ad9081_device_nco_sync_sysref_mode_set(adi_ad9081_device_t *device,
						   uint8_t mode);
int32_t
adi_ad9081_device_nco_sync_reset_via_sysref_set(adi_ad9081_device_t *device,
						uint8_t enable);
int32_t adi_ad9081_device_nco_sync_trigger_set(adi_ad9081_device_t *device);

int32_t adi_ad9081_dac_d2a_dual_spi_enable_set(adi_ad9081_device_t *device,
					       uint8_t duals, uint8_t enable);
int32_t adi_ad9081_dac_dll_startup(adi_ad9081_device_t *device, uint8_t dacs);
int32_t adi_ad9081_dac_digital_logic_enable_set(adi_ad9081_device_t *device,
						uint8_t enable);
int32_t adi_ad9081_dac_duc_nco_ftw0_set(adi_ad9081_device_t *device,
					uint8_t dacs, uint8_t channels,
					uint64_t ftw);
int32_t adi_ad9081_dac_soft_off_gain_enable_set(adi_ad9081_device_t *device,
						uint8_t dacs, uint8_t enable);
int32_t adi_ad9081_dac_shuffle_enable_set(adi_ad9081_device_t *device,
					  uint8_t dacs, uint8_t enable);
int32_t adi_ad9081_dac_data_xor_set(adi_ad9081_device_t *device, uint8_t dacs,
				    uint8_t enable);

int32_t adi_ad9081_adc_select_set(adi_ad9081_device_t *device, uint8_t adcs);
int32_t adi_ad9081_adc_core_analog_regs_enable_set(adi_ad9081_device_t *device,
						   uint8_t adc_cores,
						   uint8_t enable);
int32_t adi_ad9081_adc_core_setup(adi_ad9081_device_t *device,
				  uint8_t adc_cores);
int32_t adi_ad9081_adc_power_up_set(adi_ad9081_device_t *device, uint8_t adcs,
				    uint8_t enable);
uint8_t
adi_ad9081_adc_ddc_coarse_dcm_decode(adi_ad9081_adc_coarse_ddc_dcm_e cddc_dcm);
uint8_t
adi_ad9081_adc_ddc_fine_dcm_decode(adi_ad9081_adc_fine_ddc_dcm_e fddc_dcm);
int32_t adi_ad9081_adc_fdelay_cdelay_pfir_sel_to_gpio_mapping_set(
	adi_ad9081_device_t *device, uint8_t sel0, uint8_t sel1);
int32_t adi_ad9081_adc_common_hop_en_set(adi_ad9081_device_t *device,
					 uint8_t enable);
int32_t adi_ad9081_adc_ddc_coarse_trig_nco_reset_enable_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t enable);
int32_t adi_ad9081_adc_ddc_coarse_nco_channel_update_mode_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t mode);
int32_t
adi_ad9081_adc_ddc_coarse_gpio_chip_xfer_mode_set(adi_ad9081_device_t *device,
						  uint8_t cddcs, uint8_t mode);
int32_t adi_ad9081_adc_ddc_coarse_trig_hop_en_set(adi_ad9081_device_t *device,
						  uint8_t cddcs,
						  uint8_t enable);
int32_t adi_ad9081_adc_ddc_coarse_dither_en_set(adi_ad9081_device_t *device,
						uint8_t cddcs,
						uint8_t amp_dither_en,
						uint8_t phase_dither_en);
int32_t adi_ad9081_adc_ddc_coarse_chip_xfer_set(adi_ad9081_device_t *device,
						uint8_t cddcs);
int32_t
adi_ad9081_adc_ddc_coarse_chip_xfer_status_get(adi_ad9081_device_t *device,
					       uint8_t cddcs, uint8_t *status);
int32_t adi_ad9081_adc_ddc_coarse_psw_set(adi_ad9081_device_t *device,
					  uint8_t cddcs, uint64_t psw);
int32_t adi_ad9081_adc_ddc_fine_trig_nco_reset_enable_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t enable);
int32_t adi_ad9081_adc_ddc_fine_nco_channel_update_mode_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t mode);
int32_t
adi_ad9081_adc_ddc_fine_gpio_chip_xfer_mode_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t mode);
int32_t adi_ad9081_adc_ddc_fine_trig_hop_en_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t enable);
int32_t adi_ad9081_adc_ddc_fine_dither_en_set(adi_ad9081_device_t *device,
					      uint8_t fddcs,
					      uint8_t amp_dither_en,
					      uint8_t phase_dither_en);
int32_t adi_ad9081_adc_ddc_fine_chip_xfer_set(adi_ad9081_device_t *device,
					      uint8_t fddcs);

int32_t adi_ad9081_jesd_tx_conv_mask_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t conv_index, uint8_t val);
int32_t adi_ad9081_jesd_tx_link_conv_sel_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2], uint8_t jesd_m[2]);
int32_t adi_ad9081_jesd_tx_pll_status_get(adi_ad9081_device_t *device,
					  uint8_t *pll_locked);
int32_t adi_ad9081_jesd_rx_startup_des(adi_ad9081_device_t *device,
				       adi_ad9081_deser_mode_e deser_mode);
int32_t adi_ad9081_jesd_pll_lock_status_get(adi_ad9081_device_t *device,
					    uint8_t *locked);
uint16_t adi_ad9081_jesd_find_dformat_out_nc(
	adi_ad9081_jtx_conv_sel_t const *jesd_conv_sel, uint8_t jesd_m);
uint8_t
adi_ad9081_jesd_determine_common_nc(adi_ad9081_jesd_link_select_e links,
				    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2],
				    uint8_t jesd_m[2]);

#if AD9081_USE_FLOATING_TYPE > 0
int32_t adi_ad9081_hal_calc_nco_ftw_f(adi_ad9081_device_t *device, double freq,
				      double nco_shift, uint64_t *ftw,
				      uint64_t *a, uint64_t *b);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ADI_AD9081_CONFIG_H__ */

/*! @} */