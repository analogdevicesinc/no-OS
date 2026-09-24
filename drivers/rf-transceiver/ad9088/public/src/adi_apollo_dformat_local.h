/*!
 * \brief     Apollo DFFORMAT functional block local defines and prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_DFORMAT
 * @{
 */
#ifndef __ADI_APOLLO_DFORMAT_LOCAL_H__
#define __ADI_APOLLO_DFORMAT_LOCAL_H__


/*============= I N C L U D E S ============*/

/*============= D E F I N E S ==============*/

/*============= E X P O R T S ==============*/

/**
 * \brief  Enable virtual converter test mode
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link select
 * \param[in]  enable          Enable test mode, bit0 - virtual converter0, bit1 - virtual converter1, ...
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */ 
int32_t adi_apollo_dformat_conv_test_mode_enable_set(adi_apollo_device_t *device, 
    const uint16_t links, 
    uint8_t enable);


#ifdef __cplusplus
extern "C" {
#endif

uint32_t calc_jtx_dformat_base(int32_t link);


#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_DFORMAT_LOCAL_H__ */
/*! @} */
