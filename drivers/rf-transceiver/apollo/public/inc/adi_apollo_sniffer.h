/*!
 * \brief     Spectrum Sniffer definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SNIFFER
 * @{
 */
#ifndef __ADI_APOLLO_SNIFFER_H__
#define __ADI_APOLLO_SNIFFER_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_sniffer_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Enable/disable sniffer
 *
 * \param[in] device             Context variable - Pointer to the APOLLO device data structure
 * \param[in] sniffers           Target sniffer selector \ref adi_apollo_sniffer_select_e
 * \param[in] enable             1 to enable, 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sniffer_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint8_t enable);

/**
 * \brief  Set ADC mux for sniffer
 *
 * \param[in] device             Context variable - Pointer to the APOLLO device data structure
 * \param[in] sniffers           Target sniffer selector \ref adi_apollo_sniffer_select_e
 * \param[in] adc                Single ADC to input to sniffer \ref adi_apollo_adc_idx_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sniffer_adc_mux_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint16_t adc);

/**
 * \brief  Setup FFT Sniffer (before oneshot sync) or disable
 *
 * \param[in] device             Context variable - Pointer to the APOLLO device data structure
 * \param[in] sniffers           Target sniffer selector \ref adi_apollo_sniffer_select_e
 * \param[in] config             Sniffer init parameters \ref adi_apollo_sniffer_init_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sniffer_init(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, adi_apollo_sniffer_init_t *config);

/**
 * \brief  Program FFT Sniffer
 *
 * \param[in] device             Context variable - Pointer to the APOLLO device data structure
 * \param[in] sniffers           Target sniffer selector \ref adi_apollo_sniffer_select_e
 * \param[in] config             Sniffer programming parameters \ref adi_apollo_sniffer_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sniffer_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, adi_apollo_sniffer_pgm_t *config);

/**
 * \brief  Get Sniffer FFT data using regmap data request sequence
 *
 * \param[in]  device            Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sniffers          Single target sniffer selector \ref adi_apollo_sniffer_select_e
 * \param[in]  config            Sniffer programming parameters \ref adi_apollo_sniffer_pgm_t
 * \param[out] fft_data          FFT data output \ref adi_apollo_sniffer_fft_data_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sniffer_data_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, adi_apollo_sniffer_param_t *config, adi_apollo_sniffer_fft_data_t *fft_data_params);

/**
 * \brief  Set sniffer fft enable value in data request sequence. Can be mapped to GPIO
 *
 * \param[in]  device            Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sniffers          Sniffer selector mask \ref adi_apollo_sniffer_select_e
 * \param[in]  enable            1 to enable fft calculations, 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sniffer_fft_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint8_t enable);

/**
 * \brief  Set sniffer fft hold value in data request sequence. Can be mapped to GPIO
 *
 * \param[in]  device            Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sniffers          Sniffer selector mask \ref adi_apollo_sniffer_select_e
 * \param[in]  hold              Falling edge (1->0) requests new data to read. When fft_done goes high, set hold to 1 and read data. 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sniffer_fft_hold_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint8_t hold);

/**
 * \brief  Get status of sniffer fft calculation in data request sequence. Can be mapped to GPIO
 *
 * \param[in]  device            Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sniffers          Single target sniffer selector \ref adi_apollo_sniffer_select_e
 * \param[out] done              1: FFT calculation/load done 0: FFT calculation/load not done
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sniffer_fft_done_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint8_t *done);

/**
 * \brief  Read back sniffer FFT data
 *
 * \param[in]  device            Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sniffers          Single target sniffer selector \ref adi_apollo_sniffer_select_e
 * \param[in]  config            Sniffer programming parameters \ref adi_apollo_sniffer_pgm_t
 * \param[out] fft_data          FFT data output \ref adi_apollo_sniffer_fft_data_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sniffer_fft_data_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, adi_apollo_sniffer_param_t *config, adi_apollo_sniffer_fft_data_t *fft_data_params);
 

#ifdef __cplusplus
}
#endif
    
#endif /*__ADI_APOLLO_SNIFFER_H__ */
/*! @} */