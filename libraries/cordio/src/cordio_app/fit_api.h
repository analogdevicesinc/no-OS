/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Fitness sample application interface.
 *
 *  Copyright (c) 2011-2018 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019 Packetcraft, Inc.
 *
 *  Portions Copyright (c) 2022-2023 Analog Devices, Inc.
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

#ifndef EXAMPLES_MAX32655_BLUETOOTH_BLE_FIT_FIT_API_H_
#define EXAMPLES_MAX32655_BLUETOOTH_BLE_FIT_FIT_API_H_

#include "wsf_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

#ifndef FIT_CONN_MAX
#define FIT_CONN_MAX 1
#endif

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/
/*************************************************************************************************/
/*!
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
void FitStart(void);

/*************************************************************************************************/
/*!
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID for App.
 *
 *  \return None.
 */
/*************************************************************************************************/
void FitHandlerInit(wsfHandlerId_t handlerId);

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
void FitHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);

#ifdef __cplusplus
};
#endif

#endif // EXAMPLES_MAX32655_BLUETOOTH_BLE_FIT_FIT_API_H_
