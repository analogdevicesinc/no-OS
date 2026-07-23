/* Compiler compatibility macros for the i-link IO-Link stack.
 * Maps rt-labs OSAL compiler abstractions to GCC equivalents. */

#ifndef OSAL_CC_H
#define OSAL_CC_H

#include <assert.h>

#define CC_PACKED          __attribute__((packed))
#define CC_PACKED_BEGIN
#define CC_PACKED_END

#define CC_ASSERT(expr)    assert(expr)

#ifndef BIT
#define BIT(n)             (1u << (n))
#endif

#define NELEMENTS(arr)     (sizeof(arr) / sizeof((arr)[0]))

#endif /* OSAL_CC_H */
