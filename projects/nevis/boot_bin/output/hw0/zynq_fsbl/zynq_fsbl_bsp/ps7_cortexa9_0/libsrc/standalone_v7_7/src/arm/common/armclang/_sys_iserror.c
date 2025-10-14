/******************************************************************************
* Copyright (c) 2020 Xilinx, Inc.  All rights reserved.
*
* SPDX-License-Identifier: MIT
*
******************************************************************************/
#include "xil_types.h"

/* Stub for iserror() function */
__attribute__((weak)) s32 _sys_iserror(s32 status)
{
	if (status < 0) {
		return 1;
	}

	return 0;
}
