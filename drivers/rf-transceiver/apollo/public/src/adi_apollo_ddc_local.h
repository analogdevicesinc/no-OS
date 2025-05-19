/*!
 * \brief     DDC functional block local API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CDDC
 * @{
 */
#ifndef __ADI_APOLLO_DDC_LOCAL_H__
#define __ADI_APOLLO_DDC_LOCAL_H__

/*============= I N C L U D E S ============*/

/*============= D E F I N E S ==============*/

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

uint32_t calc_rx_cddc_base(int32_t cddc_index);
uint32_t calc_rx_fddc_base(int32_t fddc_index);

#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_DDC_LOCAL_H__ */
/*! @} */
