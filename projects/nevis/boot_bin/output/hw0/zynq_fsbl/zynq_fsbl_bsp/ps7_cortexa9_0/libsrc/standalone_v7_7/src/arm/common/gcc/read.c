/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/* read.c -- read bytes from a input device.
 */
#ifndef UNDEFINE_FILE_OPS
#include "xil_printf.h"
#include "xparameters.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) s32 _read (s32 fd, char8* buf, s32 nbytes);
__attribute__((weak)) s32 read (s32 fd, char8* buf, s32 nbytes);

#ifdef __cplusplus
}
#endif

/*
 * read  -- read bytes from the serial port. Ignore fd, since
 *          we only have stdin.
 */
__attribute__((weak)) s32
read (s32 fd, char8* buf, s32 nbytes)
{
#ifdef STDIN_BASEADDRESS
  s32 i;
  s32 numbytes = 0;
  char8* LocalBuf = buf;

  (void)fd;
  if(LocalBuf != NULL) {
	for (i = 0; i < nbytes; i++) {
		numbytes++;
		LocalBuf[i] = inbyte();
		if ((LocalBuf[i] == '\n' )|| (LocalBuf[i] == '\r')) {
			break;
		}
	}
  }

  return numbytes;
#else
  (void)fd;
  (void)buf;
  (void)nbytes;
  return 0;
#endif
}

__attribute__((weak)) s32
_read (s32 fd, char8* buf, s32 nbytes)
{
#ifdef STDIN_BASEADDRESS
  s32 i;
  s32 numbytes = 0;
  char8* LocalBuf = buf;

  (void)fd;
  if(LocalBuf != NULL) {
	for (i = 0; i < nbytes; i++) {
		numbytes++;
		LocalBuf[i] = inbyte();
		if ((LocalBuf[i] == '\n' )|| (LocalBuf[i] == '\r')) {
			break;
		}
	}
  }

  return numbytes;
#else
  (void)fd;
  (void)buf;
  (void)nbytes;
  return 0;
#endif
}
#endif
