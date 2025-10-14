/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) clock_t clock(void);

#ifdef __cplusplus
}
#endif
/*
 * clock -- It supposed to return processor time. We are not implementing
 *          this function, as timekeeping is tightly coupled with system, hence
 *          always returning 0. Users can override this with their system
 *          specific implementation.
 *
 */
__attribute__((weak)) clock_t clock(void)
{
    return (0);
}
