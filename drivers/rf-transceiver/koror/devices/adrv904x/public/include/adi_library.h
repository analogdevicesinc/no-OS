/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/
/**
* \file adi_library.h
*
* ADRV904X API Version: 2.15.0.4
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

/**
* \brief Convert a fixed point Q0.36 value to a dB value
*
* \param[in] val   Q0.36 value
* \param[in] scale Scale factor (e.g. 1000 to get milli-dB returned)
*
* \retval Scaled dB value
*/
ADI_API int32_t adi_library_q36ToDB(uint32_t val, int32_t scale);

/**
* \brief Convert a linear value to a milli-dB value
*
* \param[in] val   linear value
* \param[in] scale Scale factor
*
* \retval milli-dB value
*/
ADI_API int32_t adi_library_linearToMillidBVolt(uint32_t val, uint32_t scale);

/**
* \brief Convert a milli-dB value to a linear field value
*
* \param[in] millidB milli-dB value
* \param[in] scale   Scale factor
*
* \retval linear value
*/
ADI_API int32_t adi_library_millidBVoltToLinear(int32_t millidB, uint32_t scale);

/**
* \brief Function to convert a 32bit scaled integer to bytes that represent a 32bit floating-point number.
*
* \param[in] scaledInt is the scaled integer to be converted.
* \param[in] scale is the scale factor, i.e., the unit size of the scaled integer is 1/scale.
* \param[out] fpNum is the resulting bytes 4 that represent the floating-point number.
*
* \retval true if the conversion was successful, false otherwise (e.g. invalid argument).
*/
ADI_API bool adi_library_scaledIntToFp32(int32_t scaledInt, unsigned scale, uint32_t* fpNum);

/**
* \brief Function to convert a 64bit scaled integer to bytes that represent a 64bit floating-point number.
*
* \param[in] scaledInt is the scaled integer to be converted.
* \param[in] scale is the scale factor, i.e., the unit size of the scaled integer is 1/scale.
* \param[out] fpNum is the resulting 8 bytes that represent the floating-point number.
*
* \retval true if the conversion was successful, false otherwise (e.g. invalid argument).
*/
ADI_API bool adi_library_scaledIntToFp64(int64_t scaledInt, unsigned scale, uint64_t* fpNum);

/**
* \brief Function to convert bytes that represent a 32bit floating-point number to a 32bit scaled integer.
*
* \param[in] fpNum is the floating-point number to be converted.
* \param[in] scale is the scale factor (the unit size of the scaled integer is 1/scale).
* \param[out] scaledInt is the pointer to the resulting scaled integer.
*
* \retval true if the conversion was successful, false otherwise (e.g. outside range of scaled integer).
*/
ADI_API bool adi_library_fp32ToScaledInt(uint32_t fpNum, unsigned scale, int32_t *scaledInt);

/**
* \brief Function to convert bytes that represent a 64bit floating-point number to a 64bit scaled integer.
*
* \param[in] fpNum is the floating-point number to be converted.
* \param[in] scale is the scale factor (the unit size of the scaled integer is 1/scale).
* \param[out] scaledInt is the pointer to the resulting scaled integer.
*
* \retval true if the conversion was successful, false otherwise (e.g. outside range of scaled integer).
*/
ADI_API bool adi_library_fp64ToScaledInt(uint64_t fpNum, unsigned scale, int64_t *scaledInt);

#endif  /*_ADI_LIBRARY_H_ */
