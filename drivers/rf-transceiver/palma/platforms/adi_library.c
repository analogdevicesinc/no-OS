/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_library.c
* \brief Contains implementations of standard library functions
*
* ADRV903X API Version: 2.12.1.4
*/
#include "adi_library.h"

ADI_API void* adi_library_memset(void* dst, int c, size_t len)
{
    uint8_t* target = NULL;

    target = (uint8_t*) dst;

    if ((dst == NULL) || (len <= 0))
    {
        return (NULL);
    }

    for (; len; len--)
    {
        *target++ = (uint8_t) c;
    }

    return target;
}

ADI_API void* adi_library_memcpy(void* dst, const void* src, size_t len)
{
    uint8_t* target = NULL;
    const uint8_t* source = NULL;

    target = (uint8_t*) dst;
    source = (const uint8_t*) src;

    if ((dst == NULL) || (src == NULL) || (len <= 0U))
        return (NULL);

    for (; len; len--)
    {
        *target++ = *source++;
    }

    return dst;
}

ADI_API size_t adi_library_strlen(const char* src)
{
    uint8_t* target = NULL;
    const uint8_t* source = NULL;

    target = (uint8_t*) src;
    source = (const uint8_t*) src;

    if (src == NULL)
    {
        return (0U);
    }

    /* Find NULL character in the src string */
    for (; *target != '\0'; target++) 
    {
        /* Do nothing */
    }

    /* The delta is the length */
    return (target - source);
}

ADI_API size_t adi_library_strnlen(const char* src, size_t maxlen)
{
    uint8_t* target = NULL;
    const uint8_t* source = NULL;

    target = (uint8_t*) src;
    source = (const uint8_t*) src;

    if ((src == NULL) || (maxlen <= 0U))
    {
        return (0U);
    }

    for (; *target != '\0'; target++)
    {
        if ((size_t) (target - source) > maxlen)
        {
            return (maxlen);
        }
    }

    return (target - source);
}

ADI_API char* adi_library_strchr(const char* src, int c)
{
    char* target = NULL;

    target = (char*) src;

    if (target == NULL) 
    {
        return (NULL);
    }

    for (; *target != '\0'; target++)
    {
        if (*target == (char) c)
        {
            return (target);
        }
    }

    return (NULL);
}

ADI_API char* adi_library_strcat(char* dst, const char* src)
{
    char* target = dst;
    char* source = NULL;
    source = (char*) src;

    if ((dst == NULL) || (src == NULL))
    {
        return (NULL);
    }

    /* Move target pointer to the end of target string. */
    for (; *target != '\0'; target++)
    {
        /* Do Nothing */
    }

    /* Copy every byte from source string to target */
    for (; *source != '\0'; source++, target++)
    {
        *target = *source;
    }

    /* set last byte as NULL */
    *target = '\0';

    return (dst);
}

ADI_API char* adi_library_strncat(char* dst, const char* src, size_t n)
{
    char* target = dst;
    char* source = NULL;

    source = (char*) src;

    if ((dst == NULL) || (src == NULL) || (n <= 0U))
    {
        return (NULL);
    }

    /* Move target pointer to the end of target string. */
    for (; *target != '\0'; target++)
    {
        /* Do Nothing */
    }

    /* Copy every byte from source string to target */
    for (; *source != '\0'; source++, target++)
    {
        if (n != 0U)
        {
            *target = *source;
            n--;
        }
        else
        {
            break;
        }
    }

    /* set last byte as NULL */
    *target = '\0';

    return (dst);
}

ADI_API char* adi_library_strdup(const char* stringPtr)
{
    size_t stringLength = adi_library_strlen(stringPtr) + 1;

    if (stringPtr == NULL)
    {
        return NULL;
    }

    void* duplicateString = malloc(stringLength);

    if (duplicateString == NULL)
    {
        return NULL;
    }

    return (char*) ADI_LIBRARY_MEMCPY(duplicateString, stringPtr, stringLength);
}

#ifndef ADI_LIBRARY_RM_FLOATS

ADI_API int32_t adi_library_q36ToDB(uint32_t val, int32_t scale)
{
    return (int32_t)(10 * log10(val / pow(2,36)) * scale);
}

ADI_API int32_t adi_library_linearToMillidBVolt(uint32_t val, uint32_t scale)
{
    return (int32_t)(1000 * 20 * log10((double)val / scale));
}

ADI_API int32_t adi_library_millidBVoltToLinear(int32_t millidB, uint32_t scale)
{
    return (uint32_t)(pow(10, (double)millidB / 1000 / 20) * scale);
}
#elif defined (__KERNEL__)

ADI_API int32_t adi_library_q36ToDB(uint32_t val, int32_t scale)
{
    /* 181816029 = (1 << 24) * log10(1 << 36) */
    return (10 * (int64_t)(intlog10(val) - 181816029ULL) * scale) >> 24;
}

ADI_API int32_t adi_library_linearToMillidBVolt(uint32_t val, uint32_t scale)
{
    return (1000 * 20 * ((uint64_t)intlog10(val) - intlog10(scale))) >> 24U;
}

#define COMPUTE(n, d) if (neg) {a *= d; a /= n;} else {a *= n; a /= d;};
ADI_API int32_t adi_library_millidBVoltToLinear(int32_t millidB, uint32_t scale)
{
    unsigned neg = 0;
    uint64_t a = scale;

    if (millidB < 0) {
        neg = 1;
        millidB *= -1;
    }

    while (millidB > 0) {
        if (millidB >= 20000) {
            millidB -= 20000;
            COMPUTE(10, 1); /* 10^(20/20) */
            continue;
        }
        if (millidB >= 6000) {
            millidB -= 6000;
            COMPUTE(199526, 100000); /* 10^(6/20) */
            continue;
        }
        if (millidB >= 1000) {
            millidB -= 1000;
            COMPUTE(112202, 100000); /* 10^(1/20) */
            continue;
        }
        if (millidB >= 100) {
            millidB -= 100;
            COMPUTE(101158, 100000); /* 10^(0.1/20) */
            continue;
        }
        if (millidB >= 10) {
            millidB -= 10;
            COMPUTE(100115, 100000); /* 10^(0.01/20) */
            continue;
        }
        if (millidB >= 1) {
            millidB -= 1;
            COMPUTE(100012, 100000); /* 10^(0.001/20) */
            continue;
        }
    }

    return a;
}

#else
#error "log(), pow() replacement functions using integers must be implemented"
#endif