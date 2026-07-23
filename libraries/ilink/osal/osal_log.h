/* Logging macros for the i-link IO-Link stack.
 * Maps rt-labs LOG_* macros to printf with compile-time level filtering.
 * Set LOG_LEVEL in iolink_options.h to control verbosity. */

#ifndef OSAL_LOG_H
#define OSAL_LOG_H

#include <stdio.h>
#include "iolink_options.h"

#define LOG_STATE_ON  1
#define LOG_STATE_OFF 0

#define LOG_LEVEL_DEBUG   0
#define LOG_LEVEL_INFO    1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR   3

#define LOG_DEBUG(flag, ...) \
   do { if ((flag) && LOG_LEVEL <= LOG_LEVEL_DEBUG) printf (__VA_ARGS__); } while (0)

#define LOG_INFO(flag, ...) \
   do { if ((flag) && LOG_LEVEL <= LOG_LEVEL_INFO) printf (__VA_ARGS__); } while (0)

#define LOG_WARNING(flag, ...) \
   do { if ((flag) && LOG_LEVEL <= LOG_LEVEL_WARNING) printf (__VA_ARGS__); } while (0)

#define LOG_ERROR(flag, ...) \
   do { if ((flag) && LOG_LEVEL <= LOG_LEVEL_ERROR) printf (__VA_ARGS__); } while (0)

#endif /* OSAL_LOG_H */
