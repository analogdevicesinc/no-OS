/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include <errno.h>
#include "xil_types.h"
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) char8 *sbrk (s32 nbytes);

#ifdef __cplusplus
}
#endif

extern u8 _heap_start[];
extern u8 _heap_end[];
extern char8 HeapBase[];
extern char8 HeapLimit[];



__attribute__((weak)) char8 *sbrk (s32 nbytes)
{
  char8 *base;
  static char8 *heap_ptr = HeapBase;

  base = heap_ptr;
	if((heap_ptr != NULL) && ((heap_ptr + nbytes) <= ((char8 *)&HeapLimit + 1))) {
	heap_ptr += nbytes;
    return base;
  }	else {
    errno = ENOMEM;
    return ((char8 *)-1);
  }
}
