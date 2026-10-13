/***************************************************************************//**
 *   @file   mxc_sys.h
 *   @brief  Host test stub for the MSDK system API.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef TEST_MXC_SYS_H_
#define TEST_MXC_SYS_H_
#include <stdint.h>
#define ISO_FREQ 60000000U
uint32_t MXC_SYS_RiscVClockRate(void);
#endif
