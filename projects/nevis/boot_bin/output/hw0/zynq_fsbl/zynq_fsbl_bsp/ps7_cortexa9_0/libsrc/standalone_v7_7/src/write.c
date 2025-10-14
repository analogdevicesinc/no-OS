/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/* write.c -- write bytes to an output device.
 */
#ifndef UNDEFINE_FILE_OPS
#include "xil_printf.h"
#include "xparameters.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) sint32 _write (sint32 fd, char8* buf, sint32 nbytes);
__attribute__((weak)) sint32 write (sint32 fd, char8* buf, sint32 nbytes);

#ifdef __cplusplus
}
#endif

/*
 * write -- write bytes to the serial port. Ignore fd, since
 *          stdout and stderr are the same. Since we have no filesystem,
 *          open will only return an error.
 */
__attribute__((weak)) sint32
write (sint32 fd, char8* buf, sint32 nbytes)

{
#ifdef STDOUT_BASEADDRESS
  s32 i;
  char8* LocalBuf = buf;

  (void)fd;
  for (i = 0; i < nbytes; i++) {
	if(LocalBuf != NULL) {
		LocalBuf += i;
	}
	if(LocalBuf != NULL) {
	    if (*LocalBuf == '\n') {
	      outbyte ('\r');
	    }
	    outbyte (*LocalBuf);
	}
	if(LocalBuf != NULL) {
		LocalBuf -= i;
	}
  }
  return (nbytes);
#else
  (void)fd;
  (void)buf;
  (void)nbytes;
  return 0;
#endif
}

__attribute__((weak)) sint32
_write (sint32 fd, char8* buf, sint32 nbytes)
{
#if defined (__aarch64__) && (HYP_GUEST == 1) && (EL1_NONSECURE == 1) && defined (XEN_USE_PV_CONSOLE)
	sint32 length;

	(void)fd;
	(void)nbytes;
	length = XPVXenConsole_Write(buf);
	return length;
#else
#ifdef STDOUT_BASEADDRESS
  s32 i;
  char8* LocalBuf = buf;

  (void)fd;
  for (i = 0; i < nbytes; i++) {
	if(LocalBuf != NULL) {
		LocalBuf += i;
	}
	if(LocalBuf != NULL) {
	    if (*LocalBuf == '\n') {
	      outbyte ('\r');
	    }
	    outbyte (*LocalBuf);
	}
	if(LocalBuf != NULL) {
		LocalBuf -= i;
	}
  }
  return (nbytes);
#else
  (void)fd;
  (void)buf;
  (void)nbytes;
  return 0;
#endif
#endif
}
#endif
