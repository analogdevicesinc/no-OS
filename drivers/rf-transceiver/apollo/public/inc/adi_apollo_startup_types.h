/*!
 * \brief     Apollo API startup sequence types
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_STARTUP
 * @{
 */
#ifndef __ADI_APOLLO_STARTUP_TYPES_H__
#define __ADI_APOLLO_STARTUP_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_common.h"
#include "adi_apollo_device_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Enumerates startup sequence types
 */
typedef enum {
    ADI_APOLLO_STARTUP_SEQ_FW_LOAD          = 0x00000001,
    ADI_APOLLO_STARTUP_SEQ_PROFILE_LOAD     = 0x00000002,
    ADI_APOLLO_STARTUP_SEQ_TMU_EN           = 0x00000004,
    ADI_APOLLO_STARTUP_SEQ_DP_CONFIG        = 0x00000008,
    ADI_APOLLO_STARTUP_SEQ_LINK_EN          = 0x00000010,
    ADI_APOLLO_STARTUP_SEQ_TXRX_ACTIVATE    = 0x00000020,
    ADI_APOLLO_STARTUP_SEQ_SYNC             = 0x00000040,

    ADI_APOLLO_STARTUP_SEQ_DEFAULT          = 0x0000007F,
} adi_apollo_startup_seq_type_e;

/*!
* \brief Enumerates FW image types
*/
typedef enum {
    ADI_APOLLO_FW_ID_CORE_0_STD_FW_BIN      = 0,
    ADI_APOLLO_FW_ID_CORE_1_STD_FW_BIN      = 1,

    ADI_APOLLO_FW_ID_SECR_BOOT_HDR_BIN      = 2,
    ADI_APOLLO_FW_ID_CORE_0_TYE_FW_BIN      = 3,
    ADI_APOLLO_FW_ID_CORE_1_TYE_FW_BIN      = 4,
    ADI_APOLLO_FW_ID_TYE_OPER_FW_BIN        = 5,

    ADI_APOLLO_FW_ID_PROD_SECR_BOOT_HDR_BIN = 6,
    ADI_APOLLO_FW_ID_PROD_CORE_0_TYE_FW_BIN = 7,
    ADI_APOLLO_FW_ID_PROD_CORE_1_TYE_FW_BIN = 8,
    ADI_APOLLO_FW_ID_PROD_TYE_OPER_FW_BIN   = 9,

    ADI_APOLLO_FW_ID_MAX

} adi_apollo_startup_fw_id_e;


#ifndef CLIENT_IGNORE

/*!
 * \brief Apollo FW image provider structure
 */
typedef struct adi_apollo_fw_provider_t {
    char *desc;                 /*!< User specific text description */

    void *tag;                  /*!< Implementation specific struct/obj */
} adi_apollo_fw_provider_t;

/*!
 * \brief Apollo startup sequence - open. Called before a FW image load.
 */
typedef int32_t (*adi_apollo_fw_provider_open_t)     (adi_apollo_fw_provider_t *obj, adi_apollo_startup_fw_id_e fw_id);

/*!
 * \brief Apollo startup sequence - close. Called after a FW image load.
 */
typedef int32_t (*adi_apollo_fw_provider_close_t)    (adi_apollo_fw_provider_t *obj, adi_apollo_startup_fw_id_e fw_id);

/*!
 * \brief Apollo startup sequence - get. Called during FW image load to get raw FW bytes
 */
typedef int32_t (*adi_apollo_fw_provider_get_t)      (adi_apollo_fw_provider_t *obj, adi_apollo_startup_fw_id_e fw_id, uint8_t **byte_arr, uint32_t* bytes_read);

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_STARTUP_TYPES_H__ */
/*! @} */
