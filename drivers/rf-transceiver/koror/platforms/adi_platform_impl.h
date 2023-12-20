/*
 * HAL implementors/porters may modify this file to suit the underlying OS
 * and hardware.  For instance to map abstract HAL types to concrete
 * platform/OS-dependent types.
 */

#ifndef __ADI_PLATFORM_IMPL_H__
#define __ADI_PLATFORM_IMPL_H__

#include "adi_platform_impl_types.h"

/* Add custom platform functions here */
FILE* fopen(const char * filename, const char *mode);

extern int fseek (FILE * stream, long int offset, int origin);
extern long int ftell (FILE *stream);

extern int __fclose (FILE *stream);
#define fclose(filename) __fclose(filename)
extern char * fgets(char *dst, int num, FILE *stream);
extern size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
extern size_t fwrite (const void * ptr, size_t size, size_t count, FILE *stream);

extern FILE* __fopen(const char * filename, const char *mode);
#define fopen(filename, mode) __fopen(filename, mode)

#endif

