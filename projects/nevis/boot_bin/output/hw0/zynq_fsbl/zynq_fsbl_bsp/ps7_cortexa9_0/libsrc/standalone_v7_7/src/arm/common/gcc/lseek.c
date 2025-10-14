/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include <sys/types.h>
#include <errno.h>
#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) off_t _lseek(s32 fd, off_t offset, s32 whence);
__attribute__((weak)) off_t lseek(s32 fd, off_t offset, s32 whence);

#ifdef __cplusplus
}
#endif
/*
 * lseek --  Since a serial port is non-seekable, we return an error.
 */
__attribute__((weak)) off_t lseek(s32 fd, off_t offset, s32 whence)
{
  (void)fd;
  (void)offset;
  (void)whence;
  errno = ESPIPE;
  return ((off_t)-1);
}

__attribute__((weak)) off_t _lseek(s32 fd, off_t offset, s32 whence)
{
  (void)fd;
  (void)offset;
  (void)whence;
  errno = ESPIPE;
  return ((off_t)-1);
}
