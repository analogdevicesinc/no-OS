/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include <sys/types.h>
#include "xil_types.h"

extern u8 _heap_start[];
extern u8 _heap_end[];

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) caddr_t _sbrk ( s32 incr );

#ifdef __cplusplus
}
#endif

__attribute__((weak)) caddr_t _sbrk ( s32 incr )
{
  static u8 *heap = NULL;
  u8 *prev_heap;
  static u8 *HeapEndPtr = (u8 *)&_heap_end;
  caddr_t Status;

  if (heap == NULL) {
    heap = (u8 *)&_heap_start;
  }
  prev_heap = heap;

	if (((heap + incr) <= HeapEndPtr) && (prev_heap != NULL)) {
  heap += incr;
	  Status = (caddr_t) ((void *)prev_heap);
	} else {
	  Status = (caddr_t) -1;
  }

  return Status;
}
