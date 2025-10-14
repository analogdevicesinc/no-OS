/******************************************************************************
* Copyright (c) 2014 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include <signal.h>
#include <unistd.h>
#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) s32 _kill(pid_t pid, s32 sig);

#ifdef __cplusplus
}
#endif

/*
 * kill -- go out via exit...
 */

__attribute__((weak)) int kill(pid_t pid, int sig)
{
  if(pid == 1) {
    _exit(sig);
  }
  return 0;
}

__attribute__((weak)) s32 _kill(pid_t pid, s32 sig)
{
  if(pid == 1) {
    _exit(sig);
  }
  return 0;
}
