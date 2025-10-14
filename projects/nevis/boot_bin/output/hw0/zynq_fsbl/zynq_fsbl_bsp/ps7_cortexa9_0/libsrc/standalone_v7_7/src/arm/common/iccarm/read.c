/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*
 *
 *
 * The "__read" function reads a number of bytes, at most "size" into
 * the memory area pointed to by "buffer".  It returns the number of
 * bytes read, 0 at the end of the file, or _LLIO_ERROR if failure
 * occurs.
 *
 * The template implementation below should return a
 * character value, or -1 on failure.
 *
 */

#include <yfuns.h>
#include "xparameters.h"
#include "xil_types.h"

size_t __read(sint32 handle, u8 * buffer, size_t size);

size_t __read(sint32 handle, u8 * buffer, size_t size)
{

#ifdef STDIN_BASEADDRESS
  s32 i;
  s32 numbytes = 0;
  char8* LocalBuf = buffer;

  (void)handle;
  if(LocalBuf != NULL) {
	for (i = 0; i < size; i++) {
		numbytes++;
		*(LocalBuf + i) = inbyte();
		if ((*(LocalBuf + i) == '\n' )|| (*(LocalBuf + i) == '\r')) {
			break;
		}
	}
  }

  return numbytes;
#else
  (void)handle;
  (void)buffer;
  (void)size;
  return 0;
#endif
}
