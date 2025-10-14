/******************************************************************************
* Copyright (C) 2015 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xcoresightpsdcc.h
* @addtogroup coresightps_dcc_v1_8
* @{
* @details
*
* CoreSight driver component.
*
* The coresight is a part of debug communication channel (DCC) group. Jtag UART
* for ARM uses DCC. Each ARM core has its own DCC, so one need to select an
* ARM target in XSDB console before running the jtag terminal command. Using the
* coresight driver component, the output stream can be directed to a log file.
*
* @note 	None.
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date		Changes
* ----- -----  -------- -----------------------------------------------
* 1.00  kvn    02/14/15 First release
* 1.1   kvn    06/12/15 Add support for Zynq Ultrascale+ MP.
*       kvn    08/18/15 Modified Makefile according to compiler changes.
* 1.3   asa    07/01/16 Made changes to ensure that the file does not compile
*                       for MB BSPs. Instead it throws up a warning. This
*                       fixes the CR#953056.
* 1.5   sne    01/19/19 Fixed MISRA-C Violations CR#1025101.
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#ifndef XCORESIGHTPSDCC_H                /* prevent circular inclusions */
#define XCORESIGHTPSDCC_H                /* by using protection macros */
#ifndef __MICROBLAZE__
#include <xil_types.h>

void XCoresightPs_DccSendByte(u32 BaseAddress, u8 Data);

u8 XCoresightPs_DccRecvByte(u32 BaseAddress);
#endif
#endif
/** @} */
