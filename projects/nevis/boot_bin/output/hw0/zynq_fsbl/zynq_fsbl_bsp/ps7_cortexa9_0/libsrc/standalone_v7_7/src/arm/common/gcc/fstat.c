/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include <sys/stat.h>
#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) s32 _fstat(s32 fd, struct stat *buf);

#ifdef __cplusplus
}
#endif
/*
 * fstat -- Since we have no file system, we just return an error.
 */
__attribute__((weak)) s32 _fstat(s32 fd, struct stat *buf)
{
  (void)fd;
  buf->st_mode = S_IFCHR; /* Always pretend to be a tty */

  return (0);
}
