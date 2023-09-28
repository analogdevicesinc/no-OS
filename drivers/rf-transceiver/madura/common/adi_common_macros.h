/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_common_macros.h
* \brief Contains ADI Transceiver general purpose macros.
*
* ADI common lib Version: 0.0.1.1
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_COMMON_MACROS_H_
#define _ADI_COMMON_MACROS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ADI_HARDRESET 1U
#define ADI_SOFTRESET 0U

#define ADI_TRUE  1U
#define ADI_FALSE 0U

#define ADI_ENABLE  1U
#define ADI_DISABLE 0U

#define ADI_ON  1U
#define ADI_OFF 0U

#define ADI_SUCCESS 1U
#define ADI_FAILURE 0U

#define ADI_COMMON_VERBOSE        1
#define ADI_COMMON_VARIABLE_USAGE 1
#define ADI_MAX_PATH              100
#define ADI_MAX_ERR_STR           150
#define ADI_ITER_BEGIN            0
#define ADI_ITER_END UINT32_MAX

#ifdef __cplusplus
}
#endif
#endif  /* _ADI_COMMON_MACROS_H_ */
