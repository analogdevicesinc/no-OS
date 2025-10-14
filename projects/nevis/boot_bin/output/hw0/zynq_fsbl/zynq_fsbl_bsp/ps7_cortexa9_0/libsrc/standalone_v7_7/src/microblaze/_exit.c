/******************************************************************************
* Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include <unistd.h>
#include "xil_types.h"
void _exit (sint32 status);

/* _exit - Simple implementation. Does not return.
*/
void _exit (sint32 status)
{
  (void) status;
  while (1)
    {
		;
    }
}
