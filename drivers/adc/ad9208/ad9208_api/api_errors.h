/**
 * \file ad9208_api/api_errors.h
 *
 * \brief API error codes header file.
 *
 * This file contains all the error codes the API can possibly return.
 * This is a common header file, used across different APIs.
 *
 * Copyright(c) 2016 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#ifndef __API_ERRORS_H__
#define __API_ERRORS_H__

/** \addtogroup Errors
 *  @{
 */

#include <errno.h>

/** No Error */
#define API_ERROR_OK 0
/** Wrong value assigned to the ref ad9172_handle_t.sdo */
#define API_ERROR_SPI_SDO (-EINVAL)
/** Handler pointer is invalid */
#define API_ERROR_INVALID_HANDLE_PTR (-EINVAL)
/** Invalid pointer to the SPI xfer function assigned */
#define API_ERROR_INVALID_XFER_PTR (-EINVAL)
/** Invalid pointer to the delayus function assigned */
#define API_ERROR_INVALID_DELAYUS_PTR (-EINVAL)
/** Invalid parameter passed */
#define API_ERROR_INVALID_PARAM (-EINVAL)
/** Not supported */
#define API_ERROR_NOT_SUPPORTED (-ENOTSUP)
/** The VCO is out of range */
#define API_ERROR_VCO_OUT_OF_RANGE (-EINVAL)
/** FTW Acknowledge not received */
#define API_ERROR_FTW_LOAD_ACK (-EFAULT)
/** The NCO is not enabled */
#define API_ERROR_NCO_NOT_ENABLED (-ENODEV)
/** Initialization sequence failed */
#define API_ERROR_INIT_SEQ_FAIL (-EFAULT)
/** Test failed */
#define API_ERROR_TEST_FAILED -(ENODEV)
/** SPI transfer error */
#define API_ERROR_SPI_XFER	-(ENODEV)
/** delay function error */
#define API_ERROR_US_DELAY	-(ENODEV)
/** TX enable function error */
#define API_ERROR_TX_EN_PIN_CTRL -(ENODEV)
/** HW reset function error */
#define API_ERROR_RESET_PIN_CTRL -(ENODEV)
/** Event handling error */
#define API_ERROR_EVENT_HNDL  -(ENODEV)
/** HW Open function error */
#define API_ERROR_HW_OPEN  -(ENODEV)
/** HW Close function error */
#define API_ERROR_HW_CLOSE  -(ENODEV)

/** @} */
#endif /* !__API_ERRORS_H__ */
