/******************************************************************************
* Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/



#ifndef _PROFILE_CONFIG_H
#define _PROFILE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define BINSIZE 4U
#define SAMPLE_FREQ_HZ 100000U
#define TIMER_CLK_TICKS 1000U

#define PROFILE_NO_FUNCPTR_FLAG 0

#define PROFILE_TIMER_BASEADDR 0x00608000U
#define PROFILE_TIMER_INTR_ID 0U

#define TIMER_CONNECT_INTC

#ifdef __cplusplus
}
#endif

#endif
