/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include <unistd.h>
#include "xil_types.h"

/* _exit - Simple implementation. Does not return.
*/
__attribute__((weak)) void _exit (sint32 status)
{
  (void)status;
  while (1) {
	;
  }
}
