/*!
 * \brief     SMON Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SMON
 * @{
 */
#ifndef __ADI_APOLLO_SMON_H__
#define __ADI_APOLLO_SMON_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_smon_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configure SMON parameters
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons               Target SMON  \ref adi_apollo_smon_sel_e
 * \param[in]  config              \ref adi_apollo_smon_pgm_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, adi_apollo_smon_pgm_t *config);

/**
 * \brief  Configure SMON DFOR parameters
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links               Target links
 * \param[in]  fddcs               Target FDDC \ref adi_apollo_fine_ddc_select_e
 * \param[in]  config              \ref adi_apollo_smon_dfor_pgm_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_dfor_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t links, adi_apollo_blk_sel_t fddcs, adi_apollo_smon_dfor_pgm_t *config);

/**
 * \brief  Update SMON STATUS
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons               Target SMON  \ref adi_apollo_smon_sel_e
 * \param[in]  update              A pulse on this signal will cause the status value to change in the regmap. 0: Status not updated, 1: Status updated.
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_status_update(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, uint8_t update);

/**
 * \brief  Read SMON STATUS
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  smons               Target SMON  \ref adi_apollo_smon_sel_e
 * \param[out] data                \ref adi_apollo_smon_read_t 
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_smon_read(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, adi_apollo_smon_read_t *data);


#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_SMON_H__ */
/*! @} */
