/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*
 *
 * This is a template implementation of the "__lseek" function used by
 * the standard library.  Replace it with a system-specific
 * implementation.
 *
 * The "__lseek" function makes the next file operation (__read or
 * __write) act on a new location.  The parameter "whence" specifies
 * how the "offset" parameter should be interpreted according to the
 * following table:
 *
 *  0 (=SEEK_SET) - Goto location "offset".
 *  1 (=SEEK_CUR) - Go "offset" bytes from the current location.
 *  2 (=SEEK_END) - Go to "offset" bytes from the end.
 *
 * This function should return the current file position, or -1 on
 * failure.
 */

#include <stdio.h>
#include <yfuns.h>
#include "xil_types.h"
LONG __lseek(sint32 handle, LONG offset, sint32 whence);

LONG __lseek(sint32 handle, LONG offset, sint32 whence)
{
  return (-1);
}
