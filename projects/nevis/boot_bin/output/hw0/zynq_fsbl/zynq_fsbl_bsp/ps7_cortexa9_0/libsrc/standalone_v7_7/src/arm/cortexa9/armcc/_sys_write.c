/******************************************************************************
* Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include "xil_types.h"
#include "xparameters.h"

__weak s32 _sys_write(u32 fh, const u8 *buf, u32 len, s32 mode)
{
#ifdef STDOUT_BASEADDRESS
  u32 volatile *uart_base = (u32 *)STDOUT_BASEADDRESS;
  s32 i;

  for (i =0; i < len;i++) {
    /* wait if TNFUL */
    while (*(uart_base + 11U) & (1U << 14U)) {
		;
	}
    *(uart_base + 12U) = buf[i];
  }
#endif
  return 0;
}
