/*!
 * \brief     MAILBOX Handler local defines and prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_MAILBOX_HANDLER_LOCAL
 * @{
 */
#ifndef __ADI_APOLLO_MAILBOX_HANDLER_LOCAL_H__
#define __ADI_APOLLO_MAILBOX_HANDLER_LOCAL_H__

/*============= I N C L U D E S ============*/

/*============= D E F I N E S ==============*/

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

int32_t adi_apollo_mailbox_busy_wait(adi_apollo_device_t *device);
int32_t adi_apollo_mailbox_read(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_id_e cmd_id, uint8_t* ptr_resp, const size_t size_resp);
int32_t adi_apollo_mailbox_write(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_id_e cmd_id, uint8_t* ptr_cmd, const size_t size_cmd);
int32_t adi_apollo_mailbox_xfer(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_id_e cmd_id, uint8_t* ptr_cmd, const size_t size_cmd, uint8_t* ptr_resp, const size_t size_resp);

#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_MAILBOX_HANDLER_LOCAL_H__ */
/*! @} */
