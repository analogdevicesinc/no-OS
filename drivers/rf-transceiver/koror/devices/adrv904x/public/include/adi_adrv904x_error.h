/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_error.h
*
* \brief Device Specific Abstractions for Common Error Declarations
*
* ADRV904X API Version: 2.9.0.4
*/

#ifndef _ADI_ADRV904X_ERROR_H_
#define _ADI_ADRV904X_ERROR_H_

#include "adi_adrv904x_types.h"
#include "adi_adrv904x_error_types.h"

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
 * \retval ADI_ADRV904X_ERR_ACT_NONE if Lookup was Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ErrInfoGet(const adi_adrv904x_ErrSource_e      errSrc,
                                                         const int64_t                       errCode,
                                                         const char** const                  errMsgPtr,
                                                         const char** const                  errCausePtr,
                                                         adi_adrv904x_ErrAction_e* const     actionCodePtr,
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
 * \retval ADI_ADRV904X_ERR_ACT_NONE if Lookup was Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ErrDataGet(const adi_adrv904x_Device_t* const  device,
                                                         const adi_common_ErrFrameId_e       frameId,
                                                         adi_adrv904x_ErrSource_e* const     errSrcPtr,
                                                         int64_t* const                      errCodePtr,
                                                         const char** const                  errMsgPtr,
                                                         const char** const                  errCausePtr,
                                                         adi_adrv904x_ErrAction_e* const     actionCodePtr,
                                                         const char** const                  actionMsgPtr);

#endif /* _ADI_ADRV904X_ERROR_H_ */
