/**
* \file
* \brief Contains LIB C declarations for platform dependent.
*
* API Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_COMMON_LIBC_H_
#define _ADI_COMMON_LIBC_H_

#include "adi_common_libc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	* \brief A function to set a specified number of bytes of memory to a specific character.
	*
	* \param dst is a pointer to the chunk of memory
	* \param c a unsigned char used to set the memory
	* \param len is the size of the chunk of memory.
	*
	* \retval Returns pointer to the dst when successful, NULL when error occured.
	*/
	void * adi_common_memset(void *dst, int c, size_t len);

	/**
	* \brief A function to copy len bytes from memory area src to memory area dst.
	*
	* \param dst destination memory (copy to).
	* \param src source memory (copy from).
	* \param len is the size of the chunk of dst when successful, NULL when error occured.
	*
	* \retval Returns pointer to the dst.
	*/
	void * adi_common_memcpy(void *dst, const void *src, size_t len);

	/**
	* \brief A function to determine the length of a string.
	*
	* \param src points to a string.
	*
	* \retval Returns the length of the string, 0 when error occured.
	*/
	size_t adi_common_strlen(const char *src);

	/**
	* \brief A function to determine the length of a fixed-size string.
	*
	* \param src points to a string.
	* \param mmaxlen is the maximum allowed length of the string.
	*
	* \retval Returns the length of the string or maxlen if '\0' is not found in the string, or 0 when error occured.
	*/
	size_t adi_common_strnlen(const char *src, size_t maxlen);

	/**
	* \brief A function to return a pointer to the first occurrence of the character c in the string src.
	*
	* \param src points to a string.
	* \param c is a character to look for in the src string.
	*
	* \retval Return a pointer to the matched character or NULL if the character is not found or error occured.
	*/
	char * adi_common_strchr(const char *src, int c);

	/**
	* \brief A function appends the src string to the dst string. dst string must have enough room for both strings.
	*
	* \param dst points to a string.
	* \param src points to a string.
	*
	* \retval Return a pointer to the resulting string dst, NULL when error occurred.
	*/
	char * adi_common_strcat(char *dst, const char *src);

	/**
	* \brief A function that appends the first n bytes of the src string to the dst string. dst string must have enough room for dst string and n bytes.
	*
	* \param dst points to a string.
	* \param src points to a string.
	* \param n number of bytes to use from src string.
	*
	* \retval Return a pointer to the resulting string dst, NULL when error occured.
	*/
	char * adi_common_strncat(char *dst, const char *src, size_t n);

	/**
    * \brief A function to convert number in ascii string to numerical number. This is for 32 bits number.
    *
    * \param src points to a ascii number string.
    *
    * \retval Return integer value corresponding to the ascii number string.
    */
	int adi_common_atoi32(const char *src);

	/**
    * \brief A function to convert a int (32bits) or long int (64 bits) to equivalent ascii number string.
    *
    * \param buf points to a string to hold the resulted ascii number string.
    * \param num2convert is a number to convert to ascii.
    *
    * \retval Return no error.
    */
	void adi_common_i64toa(char *buf, long long int num2convert);

	/**
    * \brief A function for a variable number of parameters into a formatted string.
    *  This is a simple version and only supports a limited format options: %c, %s, %d, %u, %ld, %lu. 
    *  It does not support padding or left/right justification.
    * \param buf points to a string to hold the resulted string.
    * \param formatStr is a formatted string.
    * \param ...  contains a variable number parameters.
    *
    * \retval Return integer value corresponding to the ascii number string.
    */
	int adi_common_sprintf(char *buf, const char *formatStr, ...);
	
#ifdef __cplusplus
}
#endif
#endif // _ADI_COMMON_LIBC_H_
