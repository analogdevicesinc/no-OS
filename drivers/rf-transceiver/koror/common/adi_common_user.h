/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_common_user.h
* \brief Contains COMMON API macro definitions for user to override
* 
* ADI common lib Version: 0.0.2.1
*/
#ifndef _ADI_COMMON_USER_H_
#define _ADI_COMMON_USER_H_

#ifndef ADI_COMMON_VERBOSE
    #define ADI_COMMON_VERBOSE  1
#endif  /* Verbose Flag (i.e. String Enable Flag) */

#ifndef ADI_COMMON_VARIABLE_USAGE
    #define ADI_COMMON_VARIABLE_USAGE   1
#endif /* Variable Name String Conversion Flag */

#ifndef ADI_COMMON_ERROR_VARIABLE_TYPE
    #define ADI_COMMON_ERROR_VARIABLE_TYPE  (int64_t) (intptr_t)
#endif /* Error Reporting Variable Data Type */

#ifndef ADI_COMMON_LOG_ERR_INDENT1
    #define ADI_COMMON_LOG_ERR_INDENT1 "\t\t\t\t\t"
#endif /* Log Error Indentation 1 */

#ifndef ADI_COMMON_LOG_ERR_INDENT2
    #define ADI_COMMON_LOG_ERR_INDENT2 "\t\t\t\t\t\t"
#endif /* Log Error Indentation 2 */

#endif /* _ADI_COMMON_USER_H_ */