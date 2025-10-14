/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include <unistd.h>
#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) sint32 _isatty(sint32 fd);

#ifdef __cplusplus
}
#endif

/*
 * isatty -- returns 1 if connected to a terminal device,
 *           returns 0 if not. Since we're hooked up to a
 *           serial port, we'll say yes _AND return a 1.
 */
__attribute__((weak)) sint32 isatty(sint32 fd)
{
  (void)fd;
  return (1);
}

__attribute__((weak)) sint32 _isatty(sint32 fd)
{
  (void)fd;
  return (1);
}
