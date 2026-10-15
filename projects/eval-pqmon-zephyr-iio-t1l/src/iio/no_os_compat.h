/*
 * Compatibility shim for no-OS macros used by iiod.c
 * Provides NO_OS_IS_ERR_VALUE, NO_OS_ARRAY_SIZE, no_os_min
 */

#ifndef NO_OS_COMPAT_H
#define NO_OS_COMPAT_H

#include <errno.h>
#include <stdint.h>

#define NO_OS_IS_ERR_VALUE(x) ((x) < 0)

#define NO_OS_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define no_os_min(x, y) (((x) < (y)) ? (x) : (y))

#endif /* NO_OS_COMPAT_H */
