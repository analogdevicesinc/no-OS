/******************************************************************************
* Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include "profile.h"
#include "_profile_timer_hw.h"
#include "xil_exception.h"

void _profile_clean( void );

/*
 * This function is the exit routine and is called by the crtinit, when the
 * program terminates. The name needs to be changed later..
 */
void _profile_clean( void )
{
	Xil_ExceptionDisable();
	disable_timer();
}
