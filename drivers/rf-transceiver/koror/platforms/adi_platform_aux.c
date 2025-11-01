/*
 * Functions to read I2C EEPROM info based on FMC EEPROM format
 */

#include "adi_platform_aux.h"

#ifdef __GNUC__
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <strings.h>

ADI_API void __gcov_flush();
#else
#ifdef _WIN64
#define ssize_t __int64
#else
#define ssize_t long
#endif
#endif

ADI_API int32_t adi_platform_gcov_flush()
{
#ifdef __GNUC__
    (void) ADI_LIBRARY_PRINTF("Flushing gcov files ... OK\n"); /*not resolved*/
    ADI_LIBRARY_FFLUSH(stdout);
//    __gcov_flush();
#endif
    return (0);
}

