/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_library.c
* \brief Contains implementations of standard library functions
*
* ADRV904X API Version: 2.10.0.4
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