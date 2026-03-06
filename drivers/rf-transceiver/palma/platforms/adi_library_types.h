/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_library_types.h
*
* ADRV903X API Version: 2.12.1.4
*/
#ifndef _ADI_LIBRARY_TYPES_H_
#define _ADI_LIBRARY_TYPES_H_

#ifdef __KERNEL__
#include <linux/int_log.h>
#include <linux/kernel.h>
#include <linux/limits.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/fs.h>
typedef time64_t time_t;
#define INT16_MAX                                       S16_MAX
#define INT16_MIN                                       S16_MIN
#define UINT16_MAX                                      U16_MAX
#define UINT32_MAX                                      U32_MAX
#define PRIX32                                          "lX"
#define PRIu32                                          "u"
#define PRId64                                          "d"
#define PRIX64                                          "llX"
#define time_t                                          time64_t
/** Define to disable any code that uses floating point types */
#define ADI_LIBRARY_RM_FLOATS 1
#define ADI_PLATFORM_LARGE_VARS_ON_HEAP /* Use heap instead of stack for large variables */
#else

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
#include <stdbool.h>

#endif

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
#define ADI_LIBRARY_FGETS                               fgets

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


/* When ADI_PLATFORM_LARGE_VARS_ON_HEAP is defined, the memory per function frame is limited to 2k,
 * and the heap is used instead.
 *
 * This works by using the ADI_PLATFORM_LARGE_VAR_ALLOC(), and ADI_PLATFORM_LARGE_ARRAY_ALLOC()
 * macros within the code for large variables/arrays. Memory allocated using these macros is
 * automatically freed when the variable goes out of scope, and is zeroed out. When used for arrays
 * it gives a pointer to an array and not an actual array so be careful using sizeof() with it, or
 * taking its address!
 */
/*#define ADI_PLATFORM_LARGE_VARS_ON_HEAP*/

#ifdef ADI_PLATFORM_LARGE_VARS_ON_HEAP

#ifndef __GNUC__
#error "Compiler does not support ADI_PLATFORM_LARGE_VARS_ON_HEAP"
#endif

static inline void adi_platform_free_ptr(void *ptrPtr) {
    ADI_LIBRARY_FREE(*(void**)(ptrPtr));
}

#define ADI_PLATFORM_LARGE_ARRAY_ALLOC(type, name, count) \
    type *name __attribute__((__cleanup__(adi_platform_free_ptr))) = (type*)ADI_LIBRARY_CALLOC(count, sizeof(type))

#else /* ADI_PLATFORM_LARGE_VARS_ON_HEAP */

#define ADI_PLATFORM_LARGE_ARRAY_ALLOC(type, name, count) \
    type name##_[count]; \
    type *name = name##_; \
    ADI_LIBRARY_MEMSET(name, 0, sizeof(name##_));

#endif

#define ADI_PLATFORM_LARGE_VAR_ALLOC(type, name) ADI_PLATFORM_LARGE_ARRAY_ALLOC(type, name, 1)

#endif  /* _ADI_LIBRARY_TYPES_H_ */
