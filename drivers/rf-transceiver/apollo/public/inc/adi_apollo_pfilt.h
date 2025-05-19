/*!
 * \brief     PFILT Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_PFILT
 * @{
 */
#ifndef __ADI_APOLLO_PFILT_H__
#define __ADI_APOLLO_PFILT_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_pfilt_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configure PFILT MODE parameters
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts       Target PFILT \ref adi_apollo_pfilt_sel_e
 * \param[in]  config       \ref adi_apollo_pfilt_mode_pgm_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_mode_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, adi_apollo_pfilt_mode_pgm_t *config);

/**
 * \brief  Configure PFILT gain and delay parameters
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal     Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts       Target PFILT \ref adi_apollo_pfilt_sel_e
 * \param[in]  pfilt_banks  Target PFILT coeff banks \ref adi_apollo_pfilt_bank_sel_e 
 * \param[in]  config       \ref adi_apollo_pfilt_gain_dly_pgm_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_gain_dly_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks, adi_apollo_pfilt_gain_dly_pgm_t *config);

/**
 * \brief  Configure PFILT coeff parameters
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal            Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts              Target PFILT \ref adi_apollo_pfilt_sel_e
 * \param[in]  pfilt_banks         Target PFILT coeff banks \ref adi_apollo_pfilt_bank_sel_e 
 * \param[in]  pfilt_coeff         Coefficient values for the PFILT bank
 * \param[in]  length              Coefficient values for the PFILT bank array size
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_coeff_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks, uint16_t pfilt_coeff[], uint32_t length);

/**
 * \brief  Set PFILT coeffs for Real-N, Real-N/2 or Real-N/4 modes
 *
 * \note Real-N = 32 taps, Real-N/2 = 16 taps, Real-N/4 = 8 taps
 * 
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts           Target PFILT (ADI_APOLLO_PFILT_A0, A1, B0, A1) \ref adi_apollo_pfilt_sel_e
 * \param[in]  pfilt_banks      Target PFILT coeff banks (ADI_APOLLO_PFILT_BANK0, 1, 2, 3) \ref adi_apollo_pfilt_bank_sel_e 
 * \param[in]  streams          Streams to set coeffs for (ADI_APOLLO_PFILT_STREAM_0, _1) \ref adi_apollo_pfilt_stream_sel_e
 * \param[in]  ntap_mode        Num real taps (ADI_APOLLO_PFILT_MODE_N_REAL, _N_DIV_BY_2_REAL, _N_DIV_BY_4_REAL) \ref adi_apollo_pfilt_mode_e
 * \param[in]  coeffs           Coefficient values for the PFILT bank
 * \param[in]  coeffs_len       Length of coeffs array
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_coeff_ntap_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks,
                                        uint16_t streams, adi_apollo_pfilt_mode_e ntap_mode, int16_t coeffs[], uint32_t coeffs_len);

/**
 * \brief  Set PFILT coeffs for half-complex mode
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts           Target PFILT (ADI_APOLLO_PFILT_A0, A1, B0, A1) \ref adi_apollo_pfilt_sel_e
 * \param[in]  pfilt_banks      Target PFILT coeff banks (ADI_APOLLO_PFILT_BANK0, 1, 2, 3) \ref adi_apollo_pfilt_bank_sel_e 
 * \param[in]  coeffs0          Coefficient values for first stream
 * \param[in]  coeffs1          Coefficient values for second stream
 * \param[in]  coeffs_len       Length of each coeff array
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_coeff_half_complex_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks, 
                                               int16_t coeffs0[], int16_t coeffs1[], uint32_t coeffs_len);

/**
 * \brief  Set PFILT coeffs for full-matrix mode
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts           Target PFILT (ADI_APOLLO_PFILT_A0, A1, B0, A1) \ref adi_apollo_pfilt_sel_e
 * \param[in]  pfilt_banks      Target PFILT coeff banks (ADI_APOLLO_PFILT_BANK0, 1, 2, 3) \ref adi_apollo_pfilt_bank_sel_e 
 * \param[in]  coeffs0          Coefficient values
 * \param[in]  coeffs1          Coefficient values
 * \param[in]  coeffs2          Coefficient values
 * \param[in]  coeffs3          Coefficient values
 * \param[in]  coeffs_len       Length of each coefficient array (N/4, 8 taps)
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_coeff_full_matrix_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks,
                                               int16_t coeffs0[], int16_t coeffs1[], int16_t coeffs2[], int16_t coeffs3[], uint32_t coeffs_len);

/**
 * \brief  Set PFILT delay fir half-complex mode
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts           Target PFILT (ADI_APOLLO_PFILT_A0, A1, B0, A1) \ref adi_apollo_pfilt_sel_e
 * \param[in]  pfilt_banks      Target PFILT coeff banks (ADI_APOLLO_PFILT_BANK0, 1, 2, 3) \ref adi_apollo_pfilt_bank_sel_e 
 * \param[in]  delay            Delay stream value (in terms of samples)
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_half_complex_delay_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks, uint8_t delay);

/**
 * \brief Select filter bank to transfer to active
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal            Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts              Target PFILT select mask \ref adi_apollo_pfilt_sel_e
 * \param[in]  bank_sel            Coeff bank to transfer to active (single selection) \ref adi_apollo_pfilt_bank_sel_e 
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_coeff_transfer(adi_apollo_device_t* device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t bank_sel);

/**
 * \brief  Inspect RX PFILT parameters
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal             Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilt                Target PFILT \ref adi_apollo_pfilt_sel_e
 * \param[out]  pfilt_inspect       Pointer to pfilt inspect struct \ref adi_apollo_pfilt_inspect_t
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilt, adi_apollo_pfilt_inspect_t *pfilt_inspect);

/**
 * \brief  Set the PFILT mode only, can be used to enable or bypass the filter
 * 
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal   Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts     Target PFILT \ref adi_apollo_pfilt_sel_e
 * \param[in]  iq_sel     Bit-or select of I(stream 0) and Q(stream 1) \ref adi_apollo_pfilt_stream_sel_e
 * \param[in]  mode       PFILT mode or disable \ref adi_apollo_pfilt_mode_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_mode_enable_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t iq_sel, adi_apollo_pfilt_mode_e mode);

/**
 * \brief  Set the PFILT profile selection and hopping mode
 * 
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts           Target PFILT (ADI_APOLLO_FILT_A0, A1, B0, B1) \ref adi_apollo_pfilt_sel_e
 * \param[in]  prof_sel_mode    Profile select mode. Can be direct spi, regmap and trig based. \ref adi_apollo_pfilt_profile_sel_mode_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_profile_sel_mode_set(adi_apollo_device_t* device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, const adi_apollo_pfilt_profile_sel_mode_e prof_sel_mode);

/**
 * \brief  Set the next PFILT profile selection or hop
 * 
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts           Target PFILT (ADI_APOLLO_PFILT_A0, A1, B0, B1) \ref adi_apollo_pfilt_sel_e
 * \param[in]  hop_num          Profile (e.g. coeff set) to hop to. Will be immediate if in direct regmap mode, or scheduled for next trigger.
 * 
 * The behavior of the function is influenced by adi_apollo_pfilt_profile_sel_mode_set().
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_next_hop_num_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, int16_t hop_num);

/**
 * \brief  Set the Rx PFILT ADC averaging mode
 * 
 * \note PFILT averaging only applicable to Rx PFILT
 * 
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  pfilts           Target PFILT (ADI_APOLLO_PFILT_A0, A1, B0, B1) \ref adi_apollo_pfilt_sel_e
 * \param[in]  ave_mode         Average mode (ADI_APOLLO_PFILT_AVE_DISABLE, ENABLE_AVE, ENABLE_SUB) \ref adi_apollo_pfilt_ave_mode_e
 * 
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_ave_mode_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t pfilts, adi_apollo_pfilt_ave_mode_e ave_mode);

/**
 * \brief  Set the PFILT data type for real or complex processing
 * 
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal \ref adi_apollo_terminal_e
 * \param[in]  pfilts           Target PFILT (ADI_APOLLO_PFILT_A0, A1, B0, B1) \ref adi_apollo_pfilt_sel_e
 * \param[in]  data_type        PFILT input data type (ADI_APOLLO_PFILT_COMPLEX_DATA, _REAL_DATA) \ref adi_apollo_pfilt_data_e
 * 
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_pfilt_data_type_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, adi_apollo_pfilt_data_e data_type);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_PFILT_H__ */
/*! @} */
