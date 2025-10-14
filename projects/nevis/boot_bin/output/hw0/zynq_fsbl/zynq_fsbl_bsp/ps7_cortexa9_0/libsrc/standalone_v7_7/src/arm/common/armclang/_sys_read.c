/******************************************************************************
* Copyright (c) 2020 Xilinx, Inc.  All rights reserved.
*
* SPDX-License-Identifier: MIT
*
******************************************************************************/
#ifndef UNDEFINE_FILE_OPS
#include "xil_printf.h"
#include "xparameters.h"

/* Stub for read() sys-call */
__attribute__((weak)) s32 _sys_read(__attribute__((unused)) u32 fh,
					__attribute__((unused)) u8 *buf,
					u32 len,
					__attribute__((unused)) s32 mode)
{
#ifdef STDIN_BASEADDRESS
	u32 i;
	s32 numbytes = 0;
	char8* LocalBuf = (char8 *)buf;

	if(LocalBuf != NULL) {
		for (i = 0; i < len; i++) {
			numbytes++;
			*(LocalBuf + i) = inbyte();
			if ((*(LocalBuf + i) == '\n' ) ||
				(*(LocalBuf + i) == '\r')) {
				break;
			}
		}
	}

	return numbytes;
#endif
	/* Return the number of character read */
	return 0;
}
#endif
