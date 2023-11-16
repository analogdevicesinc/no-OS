/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/
/**
* \file adi_library.h
*
* ADRV904X API Version: 2.9.0.4
*/
#ifndef _ADI_LIBRARY_H_
#define _ADI_LIBRARY_H_

#include "adi_library_types.h"
#include "adi_platform.h"

/**
* \brief A function to set a n byte of memory to a specific character.
*
* \param dst is a pointer to the chunk of memory
* \param c value to be set is passed as an int but function fills memory using unsigned char conversion
* \param len is the size of the chunk of memory.
*
* \retval Returns pointer to the dst when successful, NULL when error occurred.
*/
ADI_API void* adi_library_memset(void* dst, int c, size_t len);

/**
* \brief A function to copy len bytes from memory area src to memory area dst.
*
* \param dst destination memory (copy to).
* \param src source memory (copy from).
* \param len is the size of the chunk of dst when successful, NULL when error occurred.
*
* \retval Returns pointer to the dst.
*/
ADI_API void* adi_library_memcpy(void* dst, const void* src, size_t len);

/**
* \brief A function to determine the length of a string.
*
* \param src points to a string.
*
* \retval Returns the length of the string, 0 when error occurred.
*/
ADI_API size_t adi_library_strlen(const char* src);

/**
* \brief A function to determine the length of a fixed-size string.
*
* \param src points to a string.
* \param maxlen is at most of the string length.
*
* \retval Returns the length of the string or maxlen if '\0' is not found in the string, or 0 when error occurred.
*/
ADI_API size_t adi_library_strnlen(const char* src, size_t maxlen);

/**
* \brief A function to return a pointer to the first occurrence of the character c in the string src.
*
* \param src points to a string.
* \param c is a character to look for in the src string.
*
* \retval Return a pointer to the matched character or NULL if the character is not found or error occurred.
*/
ADI_API char* adi_library_strchr(const char* src, int c);

/**
* \brief A function appends the src string to the dst string. dst string must have enough room for both strings.
*
* \param dst points to a string
* \param src points to a string
*
* \retval Return a pointer to the resulting string dst, NULL when error occurred.
*/
ADI_API char* adi_library_strcat(char* dst, const char* src);

/**
* \brief A function appends n bytes the src string to the dst string. dst string must have enough room for dst string and n bytes.
*
* \param dst points to a string.
* \param src points to a string.
* \param n number of bytes to use from src string.
*
* \retval Return a pointer to the resulting string dst, NULL when error occurred.
*/
ADI_API char* adi_library_strncat(char* dst, const char* src, size_t n);

/**
* \brief A function to duplicate a string
*
*   NOTE: Caller is responsible for managing/freeing duplicate string (i.e. Duplication done using malloc())
*
* \param[in] stringPtr points to a string to be duplicated
*
* \retval Pointer to duplicated string or NULL for error
*/
ADI_API char* adi_library_strdup(const char* stringPtr);

#endif  /*_ADI_LIBRARY_H_ */
