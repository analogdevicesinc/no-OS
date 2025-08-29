/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Proprietary data transfer server sample application.
 *
 *  Copyright (c) 2012-2018 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019 Packetcraft, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/
#ifndef EXAMPLES_MAX32655_BLE_PERIPH_PERIPH_API_H_
#define EXAMPLES_MAX32655_BLE_PERIPH_PERIPH_API_H_

#include "wsf_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************************************/
/*!
 *  \brief  Peripheral RX callback function.
 *
 *  \param  data     Pointer to received data.
 *  \param  len      Number of bytes received.
 */
/*************************************************************************************************/
typedef void (*periphRxCb_t)(uint8_t *data, uint16_t len);

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Register the callback for received data.
 *
 *  \param  cb    Callback function for when data is received.
 *
 *  \return None.
 */
/*************************************************************************************************/
void PeriphRegisterRXCallback(periphRxCb_t cb);

/*************************************************************************************************/
/*!
 *  \brief  Send data to the peer.
 *
 *  \param  data     Pointer to received data.
 *  \param  len      Number of bytes received.
 *
 *  \return TRUE if data send, FALSE if error.
 */
/*************************************************************************************************/
bool_t PeriphTXData(uint8_t *data, uint16_t len);

/*************************************************************************************************/
/*!
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
void PeriphStart(void);

/*************************************************************************************************/
/*!
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID for App.
 *
 *  \return None.
 */
/*************************************************************************************************/
void PeriphHandlerInit(wsfHandlerId_t handlerId);

/*************************************************************************************************/
/*!
 *  \brief  WSF event handler for the application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void PeriphHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);

#ifdef __cplusplus
};
#endif

#endif // EXAMPLES_MAX32655_BLE_PERIPH_PERIPH_API_H_
