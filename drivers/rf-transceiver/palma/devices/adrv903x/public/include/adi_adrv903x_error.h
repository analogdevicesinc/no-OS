/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_error.h
*
* \brief Device Specific Abstractions for Common Error Declarations
*
* ADRV903X API Version: 2.12.1.4
*/

#ifndef _ADI_ADRV903X_ERROR_H_
#define _ADI_ADRV903X_ERROR_H_

#include "adi_adrv903x_types.h"
#include "adi_adrv903x_error_types.h"

/**
 * \brief Service to Lookup the Error Message and Recovery Action for an Error Code
 * 
 * \param[in]   errSrc          Error Source for the Device Type
 * \param[in]   errCode         Error Code to be Looked Up
 * \param[out]  errMsgPtr       Associated Error Message
 * \param[out]  errCausePtr     Associated Error Cause
 * \param[out]  actionCodePtr   Associated Recovery Action for the Error
 * \param[out]  actionMsgPtr    Associated Action Message
 * 
 * \retval ADI_ADRV903X_ERR_ACT_NONE if Lookup was Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_ErrInfoGet(const adi_adrv903x_ErrSource_e      errSrc,
                                                         const int64_t                       errCode,
                                                         const char** const                  errMsgPtr,
                                                         const char** const                  errCausePtr,
                                                         adi_adrv903x_ErrAction_e* const     actionCodePtr,
                                                         const char** const                  actionMsgPtr);

/**
 * \brief   Service to Parse Error Data from the Error Structure
 * 
 * \param[in]   device          Pointer to the device
 * \param[in]   frameId         Stack Trace Frame ID
 * \param[out]  errSrcPtr       Error Source for the Device Type
 * \param[out]  errCodePtr      Associated Error Code
 * \param[out]  errCausePtr     Associated Error Message
 * \param[out]  errMsgPtr       Associated Error Cause
 * \param[out]  actionCodePtr   Associated Recovery Action for the Error
 * \param[out]  actionMsgPtr    Associated Action Message
 * 
 * \retval ADI_ADRV903X_ERR_ACT_NONE if Lookup was Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_ErrDataGet(const adi_adrv903x_Device_t* const  device,
                                                         const adi_common_ErrFrameId_e       frameId,
                                                         adi_adrv903x_ErrSource_e* const     errSrcPtr,
                                                         int64_t* const                      errCodePtr,
                                                         const char** const                  errMsgPtr,
                                                         const char** const                  errCausePtr,
                                                         adi_adrv903x_ErrAction_e* const     actionCodePtr,
                                                         const char** const                  actionMsgPtr);

#endif /* _ADI_ADRV903X_ERROR_H_ */
