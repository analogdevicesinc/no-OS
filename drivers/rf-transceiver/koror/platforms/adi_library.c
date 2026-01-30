/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_library.c
* \brief Contains implementations of standard library functions
*
* ADRV904X API Version: 2.15.0.4
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

    void* duplicateString = ADI_LIBRARY_CALLOC(1, stringLength);

    if (duplicateString == NULL)
    {
        return NULL;
    }

    return (char*) ADI_LIBRARY_MEMCPY(duplicateString, stringPtr, stringLength);
}

/* ADI_LIBRARY_FORCE_FLOAT_FUNCS can be used to include these functions here,
 * while still removing other floating point code.
 */
#if !defined(ADI_LIBRARY_RM_FLOATS) || defined(ADI_LIBRARY_FORCE_FLOAT_FUNCS)

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


/**
* \brief Function to convert a scaled integer to bytes that represent a floating-point number.
*
* \param[in] scaledInt is the scaled integer to be converted.
* \param[in] scale is the scale factor, i.e., the unit size of the scaled integer is 1/scale.
* \param[in] fpBits is the number of bits in the floating-point representation (32 or 64).
* \param[out] fpNum is the resulting bytes that represent the floating-point number.
*
* \retval true if the conversion was successful, false otherwise (e.g. invalid argument).
*/
static bool adi_library_scaledIntToFp(int64_t scaledInt, unsigned scale, unsigned fpBits, uint64_t* fpNum)
{
    if (((fpBits != 32u) && (fpBits != 64u)) || scale == 0u || fpNum == NULL)
    {
        return false;  /* Invalid arguments */
    }

    if (scaledInt == 0)
    {
        *fpNum = 0u; /* Special case for 0.0 (note: returns +0.0, not -0.0) */
        return true;
    }

    /* Definitions for IEEE754 single and double precision formats */
    const unsigned MANTISSA_BITS = (fpBits == 32u) ? 23u : (fpBits == 64u) ? 52u : 0u;
    uint64_t exponent = (fpBits == 32u) ? 127u : (fpBits == 64u) ? 1023u : 0u;

    unsigned mantissaBitsRemaining = MANTISSA_BITS + 1u;  /* Add implied bit */
    uint64_t mantissa = 0u;
    uint64_t signBit = 0u;
    uint64_t scaledIntUnsigned = (uint64_t)scaledInt;

    /* Handle signedness */
    if (scaledInt < 0)
    {
        signBit = 1ull << (fpBits - 1u);
        scaledIntUnsigned = (uint64_t)-scaledInt;
    }

    uint64_t integer = scaledIntUnsigned / scale;
    uint64_t fraction = scaledIntUnsigned % scale;

    /* Deal with integer part of number (if any) */
    if (integer != 0u)
    {
        uint64_t temp = integer;
        do
        {
            mantissaBitsRemaining--;
        } while (temp >>= 1u);
        exponent += MANTISSA_BITS - mantissaBitsRemaining;
        /* Shift into upper part of mantissa, and clear the implied MSB */
        mantissa = (integer << mantissaBitsRemaining) & ((1ull << MANTISSA_BITS) - 1u);
    }

    /* If integer was 0: decrement exponent until a fractional bit is found */
    while ((mantissaBitsRemaining == MANTISSA_BITS + 1u) && (fraction != 0u))
    {
        exponent--;
        fraction <<= 1u;
        if (fraction >= scale)
        {
            fraction -= scale;
            mantissaBitsRemaining--;
        }
    }

    /* Deal with fractional part of number (if any) */
    while ((mantissaBitsRemaining != 0u) && (fraction != 0u))
    {
        mantissaBitsRemaining--;
        fraction <<= 1u;
        if (fraction >= scale)
        {
            fraction -= scale;
            mantissa |= 1ull << mantissaBitsRemaining;
        }
    }

    /* Assemble fields (exponent from scaled int cannot use too many bits) */
    *fpNum = signBit | (exponent << MANTISSA_BITS) | mantissa;
    return true;
}

ADI_API bool adi_library_scaledIntToFp32(int32_t scaledInt, unsigned scale, uint32_t* fpNum)
{
    if (fpNum == NULL)
    {
        return false;  /* Invalid argument */
    }
    uint64_t temp = 0u;
    bool status = adi_library_scaledIntToFp(scaledInt, scale, 32u, &temp);
    *fpNum = (uint32_t)temp;
    return status;
}

ADI_API bool adi_library_scaledIntToFp64(int64_t scaledInt, unsigned scale, uint64_t* fpNum)
{
    return adi_library_scaledIntToFp(scaledInt, scale, 64u, fpNum);
}

/**
 * \brief Structure to store a 128bit unsigned integer.
 */
typedef struct adi_library_u128 {
    uint64_t low;
    uint64_t high;
} adi_library_u128_t;

/**
* \brief Multiply two 64bit unsigned integers and return a 128bit result.
*
* \param[in] a is the first 64bit unsigned integer.
* \param[in] b is the second 64bit unsigned integer.
* \param[out] result is the pointer to the resulting 128bit unsigned integer.
*/
static void adi_library_u128_mul(uint64_t a, uint64_t b, adi_library_u128_t *result)
{
#ifdef __SIZEOF_INT128__
    __uint128_t prod = (__uint128_t)a * (__uint128_t)b;
    result->low = (uint64_t)prod;
    result->high = (uint64_t)(prod >> 64u);
#else
    /* Fallback for platforms without 128bit integer support */
    uint64_t aLow = (uint32_t)a;
    uint64_t aHigh = a >> 32u;
    uint64_t bLow = (uint32_t)b;
    uint64_t bHigh = b >> 32u;
 
    uint64_t lowLow = aLow * bLow;
    uint64_t lowHigh = aLow * bHigh;
    uint64_t highLow = aHigh * bLow;
    uint64_t highHigh = aHigh * bHigh;
 
    uint64_t mid = lowHigh + (lowLow >> 32u) + highLow;
 
    result->low = (lowLow & 0xFFFFFFFFu) | (mid << 32u);
    result->high = highHigh + (mid >> 32u);
#endif
}

/**
* \brief Divide a 64bit unsigned integer by a power of 2, rounding to nearest integer.
*
* \param[in] a is the 64bit unsigned integer to be divided.
* \param[in] pow2 is the power of 2 to divide by.
*
* \retval the result of the division.
*/
static uint64_t adi_library_u64_divPow2(uint64_t a, unsigned pow2)
{
    if (pow2 >= 64)
    {
        return 0;
    }
    if (pow2 == 0)
    {
        return a;
    }
    /* If the highest bit shifted out is 1, then add it back for rounding */
    return (a >> pow2) + ((a >> (pow2 - 1u)) & 1u);
}

/**
* \brief Divide a 128bit unsigned integer by a power of 2, rounding to nearest integer.
*
* \param[in] in is the 128bit unsigned integer to be divided.
* \param[in] pow2 is the power of 2 to divide by.
* \param[out] out is the result of the division.
*/
static void adi_library_u128_divPow2(adi_library_u128_t in, unsigned pow2, adi_library_u128_t *out)
{
    if (pow2 >= 64) {
        out->high = 0;
        out->low = adi_library_u64_divPow2(in.high, pow2 - 64u);
    } else {
        out->high = in.high >> pow2;
        out->low = (in.high << (64u - pow2)) | adi_library_u64_divPow2(in.low, pow2);
    }
}

/**
* \brief Function to convert bytes that represent a floating-point number to a scaled integer.
*
* \param[in] fpNum is the floating-point number to be converted.
* \param[in] scale is the scale factor (the unit size of the scaled integer is 1/scale).
* \param[in] fpBits is the number of bits in the floating-point representation (32 or 64).
* \param[out] scaledInt is the pointer to the resulting scaled integer.
*
* \retval true if the conversion was successful, false otherwise (e.g. outside range of scaled integer).
*/
static bool adi_library_fpToScaledInt(uint64_t fpNum, unsigned scale, unsigned fpBits, int64_t *scaledInt)
{
    if (((fpBits != 32u) && (fpBits != 64u)) || scale == 0u || scaledInt == NULL)
    {
        return false;  /* Invalid arguments */
    }

    /* Definitions for IEEE754 single and double precision formats */
    const unsigned MANTISSA_BITS = (fpBits == 32u) ? 23u : (fpBits == 64u) ? 52u : 0u;
    const int EXPONENT_OFFSET = (fpBits == 32u) ? 127u : (fpBits == 64u) ? 1023u : 0u;

    const int64_t RESULT_MAX = (fpBits == 32u) ? INT32_MAX : INT64_MAX;
    const int64_t RESULT_MIN = (fpBits == 32u) ? INT32_MIN : INT64_MIN;

    /* Handle and clear sign bit */
    int sign = (fpNum >> (fpBits - 1u)) ? -1 : 1;
    fpNum &= (1ull << (fpBits - 1u)) - 1u;

    /* Extract exponent */
    int exponent = (int)(fpNum >> MANTISSA_BITS) - EXPONENT_OFFSET;
    fpNum &= (1ull << MANTISSA_BITS) - 1u;

    /* Handle ±0.0 or subnormal numbers */
    if (exponent == -EXPONENT_OFFSET)
    {
        *scaledInt = 0;  /* subnormal floats will round to 0 */
        return true;
    }

    /* If exponent is too large it will cause overflow */
    if ((exponent - (int)MANTISSA_BITS) > ((int)fpBits - 1))
    {
        *scaledInt = (sign > 0) ? RESULT_MAX : RESULT_MIN;
        return false;  /* out of range, ±Infinity or NaN */
    }

    /* Extract mantissa (and set implied bit) */
    uint64_t mantissa = fpNum | (1ull << MANTISSA_BITS);

    /* Use 128 bit type to avoid overflows in multiplication. In the case where
     * we are going to do a right shift later then an overflow here is not
     * necessarily a problem.
     */
    adi_library_u128_t result128 = {0u, 0u};
    adi_library_u128_mul(mantissa, scale, &result128);
    int shift = (int)MANTISSA_BITS - exponent;
    /* negative = left shift; positive = right shift */
    if (shift <= 0)
    {
        unsigned lshift = (unsigned)-shift;
        /* Check that left shift won't cause overflow first */
        if ((result128.high != 0u) ||
            ((result128.low & (UINT64_MAX << (64u - lshift))) != 0u))
        {
            *scaledInt = (sign > 0) ? RESULT_MAX : RESULT_MIN;
            return false;  /* out of range */
        }
        result128.low <<= lshift;
    }
    else
    {
        adi_library_u128_divPow2(result128, (unsigned)shift, &result128);
    }

    /* Final check for overflow */
    if ((result128.high != 0u) || result128.low > (uint64_t)RESULT_MAX)
    {
        *scaledInt = (sign > 0) ? RESULT_MAX : RESULT_MIN;
        return false;  /* out of range */
    }

    *scaledInt = (int64_t)result128.low * sign;
    return true;
}

ADI_API bool adi_library_fp32ToScaledInt(uint32_t fpNum, unsigned scale, int32_t *scaledInt)
{
    if (scaledInt == NULL)
    {
        return false;  /* Invalid argument */
    }
    int64_t temp = 0u;
    bool status = adi_library_fpToScaledInt(fpNum, scale, 32, &temp);
    *scaledInt = (int32_t)temp;
    return status;
}

ADI_API bool adi_library_fp64ToScaledInt(uint64_t fpNum, unsigned scale, int64_t *scaledInt)
{
    return adi_library_fpToScaledInt(fpNum, scale, 64, scaledInt);
}
