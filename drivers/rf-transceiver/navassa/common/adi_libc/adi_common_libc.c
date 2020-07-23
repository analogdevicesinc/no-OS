/**
* \file
* \brief Contains features related function implementation defined in
* adi_common0_libc.h
*
* API Version: $ADI_COMMON_LIB_VERSION$
*/
/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_user.h"
#include "adi_common_libc.h"

void * adi_common_memset(void *dst, int c, size_t len)
{
	uint8_t *target;
	
	target = (uint8_t *) dst;

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

void * adi_common_memcpy(void *dst, const void *src, size_t len)
{
	uint8_t *target;
	const uint8_t *source;
	
	target = (uint8_t *) dst;
	source = (const uint8_t *) src;
	
	if ((dst == NULL) || (src == NULL) || (len <= 0))
	{
		return (NULL);
	}

	for (; len; len--) 
	{
		*target++ = *source++;
	}
	
	return dst;
}

size_t adi_common_strlen(const char *src)
{
	uint8_t *target;
	const uint8_t *source;
	
	target = (uint8_t *) src;
	source = (const uint8_t *) src;
	
	if (src == NULL)
	{
		return (0);
	}
	
	/* Find NULL character in the src string */
	for (; *target != '\0'; target++) 
	{
		// Do nothing
	}
	
	/* The delta is the length */
	return (target - source);
}

size_t adi_common_strnlen(const char *src, size_t maxlen)
{
	uint8_t *target;
	const uint8_t *source;
    
	target = (uint8_t *) src;
	source = (const uint8_t *) src;
	
	if ((src == NULL) || (maxlen <= 0)) 
	{
		return (0);
	}
	
	for (; *target != '\0'; target++)
	{
		if ((size_t)(target - source) > maxlen)
		{
			return (maxlen);
		}
	}	
    
	return (target - source);
}

char * adi_common_strchr(const char *src, int c)
{
	char *target;
    
	target = (char *) src;
	
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

char * adi_common_strcat(char *dst, const char *src)
{
	char *target = dst;
	char *source;
	source = (char *)src;

	if ((dst == NULL) || (src == NULL))
	{
		return (NULL);
	}
           
	/* Move target pointer to the end of target string. */
	for (; *target != '\0'; target++)
	{
		// do nothing.
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

char * adi_common_strncat(char *dst, const char *src, size_t n)
{
	char *target = dst;
	char *source;
	source = (char *)src;

	if ((dst == NULL) || (src == NULL) || (n <= 0))
	{
		return (NULL);
	}

	/* Move target pointer to the end of target string. */
	for (; *target != '\0'; target++)
	{
		// do nothing.
	}
        
	/* Copy every byte from source string to target */
	for (; *source != '\0'; source++, target++)
	{
		if (n != 0)
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

int adi_common_atoi32(const char *src)
{
	int num = 0, negative = 0;
	int errorCode = 0;
	int maxDigits = 10; 
	int digits = 0;
	char *source;
	source = (char *) src;
   
	if (src == NULL)
	{
		return (errorCode);
	}
   
	if (*source == '-')
	{
		negative = 1;
		source++;
	}
      
	for (digits = 0; (digits < maxDigits) && (*source != '\0'); digits++, source++)
	{
		if ((*source < '0') || (*source > '9'))
		{
			return (errorCode);
		}
         
		num = (num * 10) + (*source - 48); /* 48 is value offset of 0 and '0' in ASCII table */
	}
   
	if (negative)
	{
		num = -num;
	}
      
	return (num);
   
}

void adi_common_i64toa(char *buf, long long int num2convert)
{
	int curIdx = 39, lastDigit = 0;
	char numStr[40] = { 0 }; /* big enough to hold a long 64 bits values in char */
   
	if (buf == NULL)
	{
		return;
	}
   
	numStr[39] = '\0';
   
	/* convert each digit from least significant byte to MSB */
	for (curIdx = 38; curIdx && num2convert; curIdx--)
	{
		lastDigit = num2convert % 10;
		numStr[curIdx] = lastDigit + 48; /* 48 is value offset of 0 and '0' in ASCII table */
		num2convert = num2convert / 10;
	}
   
	curIdx++;
	/* copy asscii numStr to the buffer */
	for (; curIdx < 39; curIdx++) 
	{
		*buf++ = numStr[curIdx];
	}
}

int adi_common_sprintf(char *buf, const char *formatStr, ...)
{
	va_list argp;
	char *formatPtr = NULL, *targetPtr = NULL, *holderPtr = NULL;
	char numStr[45] = { 0 };
	long long int longArg = 0;
	int intArg = 0, holderLen = 0;
	int longFound = 0;
    
	formatPtr = (char *) formatStr;
	targetPtr = buf;
    
	if ((buf == NULL) || (formatStr == NULL))
	{
		return (-1);
	}
    
	va_start(argp, formatStr);
    
	for (; *formatPtr != '\0'; formatPtr++)
	{
		holderPtr = NULL;
		
		/* if it is not % char then copy to target string */
		if (*formatPtr != '%')
		{
			*targetPtr++ = *formatPtr;
			continue; /* process next char */
		}
       
		/* Percent sign found, advance to next char */
		formatPtr++;
		
		/* Here is where to process the key char format */
		if (*formatPtr == 'l') 
		{
			longFound = 1;
			formatPtr++; /* advance to next character */
		}
       
		/* Checking if key char is supported */
		switch (*formatPtr)
		{
		case 'c':
			intArg = va_arg(argp, int);
			*targetPtr++ = (char) intArg;
			continue; /* process next char */
			break;
           
		case 's':
			holderPtr = va_arg(argp, char *);
			break;
           
		case 'd':
			/* fall through */
		case 'u':
			if (longFound) 
			{
				longArg = va_arg(argp, long long);
				longFound = 0;
				adi_common_i64toa(numStr, longArg);
				holderPtr = numStr;
			}
			else 
			{
				intArg = va_arg(argp, int);
				adi_common_i64toa(numStr, intArg);
				holderPtr = numStr;
			}
			break;
           
		default:
			/* unsupport key word */
			va_end(argp);
			return (-2);
			break;
		}
     
		if (longFound || (holderPtr == NULL)) 
		{
			va_end(argp);
			return (-3);
		}  
 
		/* copy resulted string to target string */
		holderLen = adi_common_strlen(holderPtr);
		adi_common_strncat(targetPtr, holderPtr, holderLen);
		targetPtr += holderLen;
		adi_common_memset(numStr,0,	sizeof(numStr));
       
	} /* for loop */
 
	*targetPtr = '\0';
	va_end(argp);
	return (targetPtr - buf);
}

