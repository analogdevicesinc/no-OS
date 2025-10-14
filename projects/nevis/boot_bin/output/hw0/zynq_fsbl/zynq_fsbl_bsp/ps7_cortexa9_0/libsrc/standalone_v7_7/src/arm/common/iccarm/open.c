/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*
 *
 * This is a template implementation of the "__open" function used by
 * the standard library.  Replace it with a system-specific
 * implementation.
 *
 * The "__open" function opens the file named "filename" as specified
 * by "mode".
 * open -- open a file descriptor. We don't have a filesystem, so
 *         we return an error.
 *
 */

#include <yfuns.h>
#include "xil_types.h"

sint32 __open(const char8 * filename, sint32 mode);

sint32 __open(const char8 * filename, sint32 mode)
{
return 0;
}
