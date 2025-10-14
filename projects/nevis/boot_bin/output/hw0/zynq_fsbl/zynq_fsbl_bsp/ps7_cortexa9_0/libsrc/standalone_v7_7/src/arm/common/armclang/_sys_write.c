/******************************************************************************
* Copyright (c) 2020 Xilinx, Inc.  All rights reserved.
*
* SPDX-License-Identifier: MIT
*
******************************************************************************/
#include "xil_printf.h"
#include "xparameters.h"

__attribute__((weak)) s32 _sys_write(__attribute__((unused)) u32 fh,
					const u8 *buf, u32 len,
					__attribute__((unused)) s32 mode)
{
#if HYP_GUEST && EL1_NONSECURE && XEN_USE_PV_CONSOLE
	return XPVXenConsole_Write(buf);
#else
#ifdef STDOUT_BASEADDRESS
	char8* LocalBuf = (char8 *)buf;
	u32 i;

	for (i = 0; i < len; i++) {
		/* wait if TNFUL */
		if(LocalBuf != NULL) {
			LocalBuf += i;
		}
		if(LocalBuf != NULL) {
			if (*LocalBuf == '\n') {
				outbyte('\r');
			}
			outbyte(*LocalBuf);
		}
		if(LocalBuf != NULL) {
			LocalBuf -= i;
		}
	}

	return len;
#endif
#endif
	return 0;
}
