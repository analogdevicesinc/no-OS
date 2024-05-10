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

extern int fseekx (FILE * stream, long int offset, int origin);
#define fseek( stream, offset, origin) fseekx( stream, offset, origin)
extern long int ftell (FILE *stream);

extern int __fclose (FILE *stream);
#define fclose(filename) __fclose(filename)
extern char * fgets(char *dst, int num, FILE *stream);
extern size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
extern size_t fwrite (const void * ptr, size_t size, size_t count, FILE *stream);

extern FILE* __fopen(const char * filename, const char *mode);
#define fopen(filename, mode) __fopen(filename, mode)

extern int32_t no_os_log_base_2(uint32_t x);
#undef log2
#define log2(x)  no_os_log_base_2(x);

extern long int_20db_to_mag(long a, int mdB);

#endif

