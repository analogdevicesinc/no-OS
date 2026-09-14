/*!
 * \brief     Rx Crossbar Mux functional block local API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXMUX
 * @{
 */
#ifndef __ADI_APOLLO_RXMUX_LOCAL_H__
#define __ADI_APOLLO_RXMUX_LOCAL_H__

/*============= I N C L U D E S ============*/

/*============= D E F I N E S ==============*/

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

uint32_t calc_rx_datin_base(int32_t adc_mux_index);

#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_RXMUX_LOCAL_H__ */
/*! @} */
