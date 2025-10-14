/******************************************************************************
* Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xinterrupt_wrap.h
*
* The xinterrupt_wrap.h file contains interrupt related functions and macros.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 7.2   mus  22/11/21 First release of xil interrupt support
* </pre>
*
******************************************************************************/

#ifndef XINTERRUPT_WRAP_H		/* prevent circular inclusions */
#define XINTERRUPT_WRAP_H		/* by using protection macros */

#include "xil_types.h"
#include "xstatus.h"
#include "xparameters.h"
#include "xil_exception.h"

#ifdef XIL_INTERRUPT
#if defined(XPAR_AXI_INTC)
#include "xintc.h"
#endif

#if defined(XPAR_SCUGIC)
#include "xscugic.h"
#define XSPI_INTR_OFFSET		32U
#endif

#define XINTERRUPT_DEFAULT_PRIORITY     0xA0U /* AXI INTC doesnt support priority setting, it is default priority for GIC interrupts */
#define XINTC_TYPE_IS_SCUGIC		0U
#define XINTC_TYPE_IS_INTC		1U
#define XINTR_IS_EDGE_TRIGGERED		3U
#define XINTR_IS_LEVEL_TRIGGERED	1U

#define XINTC_TYPE_MASK		0x1
#define XINTC_INTR_TYPE_MASK		0x100000
#define XINTC_BASEADDR_MASK		0xFFFFFFFFFFFFFFFE
#define XINTC_INTRID_MASK		0xFFF
#define XINTC_TRIGGER_MASK		0xF000
#define XINTC_TRIGGER_SHIFT		12
#define XINTC_INTR_TYPE_SHIFT		20U
#define XGet_IntcType(IntParent)	(IntParent & XINTC_TYPE_MASK)
#define XGet_IntrType(IntId)		((IntId & XINTC_INTR_TYPE_MASK) >> XINTC_INTR_TYPE_SHIFT)
#define XGet_BaseAddr(IntParent)	(IntParent & XINTC_BASEADDR_MASK)
#define XGet_IntrId(IntId)		(IntId & XINTC_INTRID_MASK)
#define XGet_TriggerType(IntId)		((IntId & XINTC_TRIGGER_MASK) >> XINTC_TRIGGER_SHIFT)
#define XGet_IntrOffset(IntId)		(( XGet_IntrType(IntId) == 1) ? 16 : 32) /* For PPI offset is 16 and for SPI it is 32 */

extern int XConfigInterruptCntrl(UINTPTR IntcParent);
extern int XConnectToInterruptCntrl(u32 IntrId, void *IntrHandler, void *CallBackRef, UINTPTR IntcParent);
extern int XDisconnectInterruptCntrl(u32 IntrId, UINTPTR IntcParent);
extern int XStartInterruptCntrl(u32 Mode, UINTPTR IntcParent);
extern void XEnableIntrId( u32 IntrId, UINTPTR IntcParent);
extern void XDisableIntrId( u32 IntrId, UINTPTR IntcParent);
extern void XSetPriorityTriggerType( u32 IntrId, u8 Priority, UINTPTR IntcParent);
extern void XGetPriorityTriggerType( u32 IntrId, u8 *Priority, u8 *Trigger, UINTPTR IntcParent);
extern void XStopInterruptCntrl( UINTPTR IntcParent);
extern void XRegisterInterruptHandler( void *IntrHandler, UINTPTR IntcParent);
extern int XSetupInterruptSystem(void *DriverInstance, void *IntrHandler, u32 IntrId,  UINTPTR IntcParent, u16 Priority);
#endif

#endif  /* end of protection macro */
