/******************************************************************************
* Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include "xil_types.h"
/* Stub for read() sys-call */
__weak s32 _sys_read(u32 fh, u8 *buf, u32 len, s32 mode)
{
   /* Return the number of character NOT read */
   return len;
}
