/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include <stdlib.h>
#include <unistd.h>

/*
 * abort -- go out via exit...
 */
__attribute__((weak)) void abort(void)
{
  _exit(1);
}
