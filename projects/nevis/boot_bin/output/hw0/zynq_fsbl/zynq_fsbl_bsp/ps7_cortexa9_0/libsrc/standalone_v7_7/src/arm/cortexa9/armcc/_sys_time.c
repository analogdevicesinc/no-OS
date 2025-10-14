/******************************************************************************
* Copyright (C) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
#include <time.h>

/*
 * clock -- It supposed to return processor time. We are not implementing
 *          this function, as timekeeping is tightly coupled with system, hence
 *          always returning 0. Users can override this with their system
 *          specific implementation.
 *
 */
__weak clock_t clock(void)
{
    return (0);
}
