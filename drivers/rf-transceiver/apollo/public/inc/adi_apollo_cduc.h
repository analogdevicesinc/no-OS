/*!
 * \brief     Coarse DUC functional block API prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CDUC
 * @{
 */
#ifndef __ADI_APOLLO_CDUC_H__
#define __ADI_APOLLO_CDUC_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_cduc_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief  Sets coarse DUC interpolation value
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cducs        Coarse DUC selection. ADI_APOLLO_CDUC_A0, _A0 to _A3, _B0 to _B3, _ALL \ref adi_apollo_cduc_select_e
 * \param[in]  interp       Interpolation enum.  ADI_APOLLO_CDUC_INTERP_1, _2, _3, _4, _6, _8, _12 \ref adi_apollo_coarse_duc_dcm_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cduc_interp_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cducs, uint8_t interp);

/**
 * \brief  Sets coarse DUC interpolator block parameters
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cducs        Coarse DUC selection. \ref adi_apollo_cduc_select_e
 * \param[in]  config     Pointer to coarse interpolation parameter struct. \ref adi_apollo_cduc_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cduc_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t cducs, const adi_apollo_cduc_pgm_t *config);

/**
 * \brief Enable or disable regsiter control for setting a CDUC's active state
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cducs            Coarse DUC selection. \ref adi_apollo_cduc_select_e
 * \param[in]  cduc_spien_en    Enables(1) or disables(0) register control of CDUC active state. Bypasses auto decode. (Typically disabled)
 * \param[in]  cduc_spi_en      If cduc_spien_en is true, then a 1 will activate and 0 deactivate the selected DUCs
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cduc_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cducs, uint8_t cduc_spien_en, uint8_t cduc_spi_en);

/**
 * \brief Inspect CDUC parameters
 *
 * \param[in]   device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]   cduc             Single Target CDUC \ref adi_apollo_cduc_select_e
 * \param[out]  cduc_inspect     Pointer to cduc inspect structure. \ref adi_apollo_cduc_inspect_t
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cduc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t cduc, adi_apollo_cduc_inspect_t *cduc_inspect);

/**
 * \brief  Determine the numeric CDUC interpolation value from the associated enum. Enum vals are HW specific
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bf_enum          Enum that contains register bitfield value for decimation.
 * \param[out] val              Numeric value of decimation bf enum.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cduc_interp_bf_to_val(adi_apollo_device_t* device, adi_apollo_coarse_duc_dcm_e bf_enum, uint32_t* val);

/**
 * \brief  Enables/disables coarse DUC IRQ generation
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  cducs        Coarse DUC selection. ADI_APOLLO_CDUC_A0, _A0 to _A3, _B0 to _B3, _ALL \ref adi_apollo_cduc_select_e
 * \param[in]  enable       Interpolation enum.  1 to enable, 0 to disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_cduc_irq_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cducs, uint8_t enable);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_CDUC_H__ */
/*! @} */
