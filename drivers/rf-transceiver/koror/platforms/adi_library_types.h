/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_library_types.h
*
* ADRV904X API Version: 2.10.0.4
*/
#ifndef _ADI_LIBRARY_TYPES_H_
#define _ADI_LIBRARY_TYPES_H_

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>

/* stddef.h */
#define ADI_LIBRARY_OFFSETOF                            offsetof

/* stdio.h */
#define ADI_LIBRARY_PRINTF                              printf
#define ADI_LIBRARY_FPRINTF                             fprintf
#define ADI_LIBRARY_SPRINTF                             sprintf
#define ADI_LIBRARY_SNPRINTF                            snprintf
#define ADI_LIBRARY_VPRINTF                             vprintf
#define ADI_LIBRARY_VSNPRINTF                           vsnprintf
#define ADI_LIBRARY_FFLUSH                              fflush
#define ADI_LIBRARY_FSEEK                               fseek
#define ADI_LIBRARY_FREAD                               fread
#define ADI_LIBRARY_FWRITE                              fwrite
#define ADI_LIBRARY_FOPEN                               fopen
#define ADI_LIBRARY_FOPEN_S                             fopen_s
#define ADI_LIBRARY_FCLOSE                              fclose
#define ADI_LIBRARY_FTELL                               ftell
#define ADI_LIBRARY_FERROR                              ferror
#define ADI_LIBRARY_SETVBUF                             setvbuf

/* stdlib.h */
#define ADI_LIBRARY_CALLOC                              calloc
#define ADI_LIBRARY_FREE                                free
#define ADI_LIBRARY_RAND                                rand
#define ADI_LIBRARY_EXIT                                exit
#define ADI_LIBRARY_ABS                                 abs

/* stdarg.h */
#define ADI_LIBRARY_VA_START                            va_start
#define ADI_LIBRARY_VA_END                              va_end

/* math.h*/
#define ADI_LIBRARY_CEIL                                ceil

/* string.h */
#define ADI_LIBRARY_MEMSET                              memset
#define ADI_LIBRARY_MEMCPY                              memcpy
#define ADI_LIBRARY_STRLEN                              strlen
#define ADI_LIBRARY_MEMCMP                              memcmp
#define ADI_LIBRARY_STRCMP                              strcmp
#define ADI_LIBRARY_STRTOK                              strtok
#define ADI_LIBRARY_STRTOK_R                            strtok_r

#if __STDC_VERSION__ >= 199901L     /* C99 */
    #define ADI_LIBRARY_STRNLEN                         adi_library_strnlen     /* ADI Implementation Required for C99 Make File Test */
#else
    #define ADI_LIBRARY_STRNLEN                         strnlen
#endif

#define ADI_LIBRARY_STRCHR                              strchr
#define ADI_LIBRARY_STRCAT                              strcat
#define ADI_LIBRARY_STRNCAT                             strncat
#define ADI_LIBRARY_STRNCPY                             strncpy

#if __STDC_VERSION__ >= 199901L     /* C99 */
    #define ADI_LIBRARY_STRDUP                          adi_library_strdup      /* ADI Implementation Required for C99 Make File Test */
#else
    #define ADI_LIBRARY_STRDUP                          strdup
#endif

#define ADI_LIBRARY_STRSTR                              strstr

/* time.h */

#define ADI_LIBRARY_TIME                                time
#define ADI_LIBRARY_LOCALTIME_R                         localtime_r
#define ADI_LIBRARY_MKTIME                              mktime
#define ADI_LIBRARY_CTIME                               ctime
#define ADI_LIBRARY_NANOSLEEP                           nanosleep
#define ADI_LIBRARY_CLOCK                               clock
#define ADI_LIBRARY_CLOCKS_PER_SEC                      CLOCKS_PER_SEC
#define ADI_LIBRARY_GMTIME                              gmtime

/* ctype.h */
#define ADI_LIBRARY_TOUPPER                             toupper
#define ADI_LIBRARY_TOLOWER                             tolower


#endif  /* _ADI_LIBRARY_TYPES_H_ */
