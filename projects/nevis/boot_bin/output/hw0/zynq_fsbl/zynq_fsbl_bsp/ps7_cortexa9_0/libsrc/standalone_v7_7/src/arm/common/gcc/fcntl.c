/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include <stdio.h>
#include "xil_types.h"

__attribute__((weak)) sint32 fcntl (sint32 fd, sint32 cmd, LONG arg);

/*
 * fcntl -- Manipulate a file descriptor.
 *          We don't have a filesystem, so we do nothing.
 */
__attribute__((weak)) sint32 fcntl (sint32 fd, sint32 cmd, LONG arg)
{
  (void)fd;
  (void)cmd;
  (void)arg;
  return 0;
}
