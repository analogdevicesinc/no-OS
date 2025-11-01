/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_common_macros.h
* \brief Contains ADI Transceiver general purpose macros.
* 
* ADI common lib Version: 0.0.2.1
*/

#ifndef _ADI_COMMON_MACROS_H_
#define _ADI_COMMON_MACROS_H_

#define ADI_HARDRESET    1U
#define ADI_SOFTRESET    0U

#define ADI_TRUE    1U
#define ADI_FALSE   0U

#define ADI_ENABLE  1U
#define ADI_DISABLE 0U

#define ADI_ON 1U
#define ADI_OFF 0U

#define ADI_SUCCESS 1U
#define ADI_FAILURE 0U

#define ADI_NO_VARIABLE 0
#define ADI_NO_ERROR_MESSAGE "No Error Message"

#define ADI_VERBOSE_OFF_MESSAGE "Verbose OFF: No String Messages Built in API Code"

#define ADI_LOG_INDENT 4U

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C" 
  #else
    #define ADI_API
  #endif
#endif

#ifndef ADI_API_EX
  #ifdef __cplusplus
    #define ADI_API_EX ADI_API
  #else
    #define ADI_API_EX ADI_API extern
  #endif
#endif

#endif  /* _ADI_COMMON_MACROS_H_ */
