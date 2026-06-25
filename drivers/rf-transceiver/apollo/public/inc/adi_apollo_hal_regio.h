/*!
 * \brief     Contains abstract hal register io definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_HAL
 * @{
 */

#ifndef __ADI_APOLLO_HAL_REGIO_H__
#define __ADI_APOLLO_HAL_REGIO_H__

#include "adi_cms_api_common.h"
#include "adi_apollo_hal_types.h"

#define ADI_APOLLO_HAL_SWAP32(w)       (((w)>>24)&0xff) | (((w)<<8)&0xff0000) | (((w)>>8)&0xff00) | (((w)<<24)&0xff000000)

#ifndef CLIENT_IGNORE
/*!
 * \brief Abstract register I/O struct def.
 */
typedef struct {
    adi_apollo_hal_protocol_e protocol_id;          /*!< Indicates protocol type \ref adi_apollo_hal_protocol_e */
    uint8_t     supports_paging;                    /*!< Indicates of paging is supported */
    uint8_t     rmw_enabled;                        /*!< Indicates if read-modify-write feature should be used */
    uint8_t     poll_read_en;                       /*!< Indicates if poll reads are enabled  */
    uint8_t     poll_read_returns_val;              /*!< Indicates if a polling read returns an actual value. */

    void        *child_desc;                        /*!< Pointer to specific protocol type descriptor (e.g. spi0, spi1, hsci) */
    void        *ops;                               /*!< Container for protocol specific regio implementations */
} adi_apollo_hal_regio_t;
#endif /* CLIENT_IGNORE*/

#endif /* __ADI_APOLLO_HAL_REGIO_H__ */

/*! @} */