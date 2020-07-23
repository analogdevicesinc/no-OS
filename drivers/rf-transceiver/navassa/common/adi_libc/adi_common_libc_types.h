/**
* \file
* \brief Contains API LIB C declarations for platform dependent.
*
*  API Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/* User must support stdarg.h */
#include <stdarg.h>

#ifndef _ADI_COMMON_LIBC__TYPES_H_
#define _ADI_COMMON_LIBC__TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ADI_COMMON_LIBC_MEMSET
#define ADI_COMMON_LIBC_MEMSET                              adi_common_memset
#endif

#ifndef ADI_COMMON_LIBC_MEMCPY
#define ADI_COMMON_LIBC_MEMCPY                              adi_common_memcpy
#endif

#ifndef ADI_COMMON_LIBC_STRLEN
#define ADI_COMMON_LIBC_STRLEN                              adi_common_strlen
#endif

#ifndef ADI_COMMON_LIBC_STRNLEN
#define ADI_COMMON_LIBC_STRNLEN                             adi_common_strnlen
#endif

#ifndef ADI_COMMON_LIBC_STRCHR
#define ADI_COMMON_LIBC_STRCHR                              adi_common_strchr
#endif

#ifndef ADI_COMMON_LIBC_STRCAT
#define ADI_COMMON_LIBC_STRCAT                              adi_common_strcat
#endif

#ifndef ADI_COMMON_LIBC_STRNCAT
#define ADI_COMMON_LIBC_STRNCAT                             adi_common_strncat
#endif

#ifndef ADI_COMMON_LIBC_ATOI
#define ADI_COMMON_LIBC_ATOI                                adi_common_atoi32
#endif

#ifndef ADI_COMMON_LIBC_ITOA
#define ADI_COMMON_LIBC_ITOA                                adi_common_i64toa
#endif
	
#ifndef ADI_COMMON_LIBC_SPRINTF
#define ADI_COMMON_LIBC_SPRINTF                             adi_common_sprintf
#endif
	
#ifdef __cplusplus
}
#endif
#endif // _ADI_COMMON_LIBC__TYPES_H_
