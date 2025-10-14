/******************************************************************************
* Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include "xil_types.h"
/* Stuv for close() sys-call */
__weak s32 _sys_close(s32 fh)
{
   return -1;
}
