// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_error.h
 * \brief Contains Talise API error handling function prototypes and data types for talise_error.c
 *        These functions are public to the customer for getting more details on
 *        errors and debugging.
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_ERROR_H_
#define TALISE_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define IF_ERR_RETURN_U32(a) if(a > TALACT_ERR_CHECK_TIMER) { return (uint32_t)a; }
#define IF_ERR_RETURN(a) if(a > TALACT_ERR_CHECK_TIMER) { return a; }

#include "talise_types.h"
#include "talise_error_types.h"

/**
 * \brief Helper function for return of current Error code
 *
 * Returns an error code containing root source of the error and an error code
 * from that source. The sources are defined by the enumeration taliseErrSources_t
 * Each source has its own enumerated list of error codes.
 * HAL layer error codes are defined by  adiHalErr_t in adi_hal.h
 * and API errors are defined by  taliseErr_t.
 *
 * \param device Pointer to device data structure identifying desired instance of Talise device
 * \param errSrc Return Source of error at the pointer address
 * \param errCode Returns Enumerated error code value at the pointer address
 *
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 * \retval TALACT_ERR_CHECK_PARAM if device reference unknown or invalid parameter
*/
uint32_t TALISE_getErrCode(taliseDevice_t *device, uint32_t *errSrc,
			   uint32_t *errCode);

/**
 * \brief Helper function to return a character string based on error code.
 *
 * A helper function to return a string message for each error code.
 * The error source and error code is available from the read only devStateInfo
 * member of the Talise device reference.
 *
 * This debug feature is enabled when the MACRO TALISE_VERBOSE is set to 1 in
 * talise_user.h configuration file.
 *
 * \param errSrc  A value representing the source of error.
 *
 * \param errCode Error code. There is an enumerated list of
 *        error codes per source.
 *
 * \return Returns character string based on errSrc and errCode values
 */
const char* TALISE_getErrorMessage(uint32_t errSrc, uint32_t errCode);

/**
 * \brief Helper function to return a character string based on error code.
 *
 * A helper function to return a string message for each error code.
 * The error source and error code is available from the read only devStateInfo
 * member of the Talise device reference.
 *
 * This debug feature is enabled when the MACRO TALISE_VERBOSE is set to 1 in
 * talise_user.h configuration file.
 *
 * \param device Pointer to device data structure identifying desired instance of Talise device
 *
 * \param errSrc  A value representing the source of error.
 *
 * \param errCode Error code. There is an enumerated list of
 *        error codes per source.
 *
 * \return Returns character string based on errSrc and errCode values
 */
const char* TALISE_getRegisteredErrorMessage(taliseDevice_t *device,
		uint32_t errSrc, uint32_t errCode);

/**
 * \brief Private helper function to register error message callback functions.
 *
 * \param device Pointer to device data structure identifying desired instance of Talise device
 *
 * \param errSrc            A value representing the source of error.
 *
 * \param callbackFunction  pointer to callback function which processes Error Messages.
 *                          const char* (*callbackFunction)(uint32_t errSrc, uint32_t errCode).
 *                          The callback function params are (unit32_t errSrc, uint32_t errCode).
 *                          The callback function return a (const char *).
 *
 * \retval uint32_t         0 - Success ; 1 - Failure
 */
uint32_t talRegisterErrorMessage(taliseDevice_t *device, uint32_t errSrc,
				 const char* (*callbackFunction)(uint32_t, uint32_t));

/**
 * \brief Private Helper function to process detected errors  and determine if
 *        a new recovery action should be recommended.
 *
 *
 * \param device Pointer to device data structure identifying desired device instance
 * \param errHdl Error Handler type
 * \param detErr Error detected to be processed by handler
 * \param retVal current Recovery Action,
 * \param recAction new Recovery Action to be returned should error handler determine an error
 *
 * \retval uint32_t Value representing the latest recovery Action following processing of detected error.
*/
talRecoveryActions_t talApiErrHandler(taliseDevice_t *device,
				      taliseErrHdls_t errHdl, uint32_t detErr, talRecoveryActions_t retVal,
				      talRecoveryActions_t recAction);

#ifdef __cplusplus
}
#endif

#endif /* TALISE_ERROR_H_ */
