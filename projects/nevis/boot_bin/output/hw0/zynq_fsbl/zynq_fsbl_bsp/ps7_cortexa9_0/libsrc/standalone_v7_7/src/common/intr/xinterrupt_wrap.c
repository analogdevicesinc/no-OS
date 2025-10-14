/******************************************************************************
* Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xinterrupt_wrap.c
*
* The xinterrupt_wrap.c file contains interrupt related functions and macros.
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

#include "xinterrupt_wrap.h"

#ifdef XIL_INTERRUPT

#if defined (XPAR_SCUGIC) /* available in xscugic.h */
XScuGic XScuGicInstance;
#endif

#if defined (XPAR_AXI_INTC) /* available in xintc.h */
XIntc XIntcInstance ;
#endif

/*****************************************************************************/
/**
*
* @brief    Initializes the interrupt controller.
*
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   XST_SUCCESS if initialization was successful
* 	    XST_FAILURE in case of failure
*
* @note     None.
*
******************************************************************************/
int XConfigInterruptCntrl(UINTPTR IntcParent) {
	int Status = XST_FAILURE;
	UINTPTR BaseAddr = XGet_BaseAddr(IntcParent);

	if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC)
	{
	#if defined (XPAR_SCUGIC)
		XScuGic_Config *CfgPtr = NULL;
		if (XScuGicInstance.IsReady != XIL_COMPONENT_IS_READY) {
			CfgPtr = XScuGic_LookupConfigBaseAddr(BaseAddr);
			Status = XScuGic_CfgInitialize(&XScuGicInstance, CfgPtr, 0);
		} else {
			Status = XST_SUCCESS;
		}
		return Status;
	#else
		return XST_FAILURE;
	#endif
	} else {
	#if defined (XPAR_AXI_INTC)
		if (XIntcInstance.IsStarted != XIL_COMPONENT_IS_STARTED)
			Status = XIntc_Initialize(&XIntcInstance, BaseAddr);
		else
			Status = XST_SUCCESS;
		return Status;
	#else
		return XST_FAILURE;
	#endif
	}
}

/*****************************************************************************/
/**
*
* @brief    connects to the interrupt controller.
*
* @param    IntrId: Interrupt Id.
* @param    IntrHandler: Interrupt handler.
* @param    CallBackRef: Callback reference for handler.
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   XST_SUCCESS if initialization was successful
* 	    XST_FAILURE in case of failure
*
* @note     None.
*
******************************************************************************/
int XConnectToInterruptCntrl(u32 IntrId, void *IntrHandler, void *CallBackRef, UINTPTR IntcParent)
{
	int Status;

	if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC)
	{

	#if defined (XPAR_SCUGIC)
		u16 IntrNum = XGet_IntrId(IntrId);
		u16 Offset = XGet_IntrOffset(IntrId);

		IntrNum += Offset;
		Status = XScuGic_Connect(&XScuGicInstance, IntrNum,  \
			(Xil_ExceptionHandler) IntrHandler, CallBackRef);
		return Status;
	#else
		return XST_FAILURE;
	#endif
	} else {
	#if defined (XPAR_AXI_INTC)
		Status = XIntc_Connect(&XIntcInstance, IntrId, \
			(XInterruptHandler)IntrHandler, CallBackRef);
		return Status;
	#else
		return XST_FAILURE;
	#endif

	}
}

/*****************************************************************************/
/**
*
* @brief    disconnects the interrupt controller.
*
* @param    IntrId: Interrupt Id.
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   XST_SUCCESS if initialization was successful
* 	    XST_FAILURE in case of failure
*
* @note     None.
*
******************************************************************************/
int XDisconnectInterruptCntrl(u32 IntrId, UINTPTR IntcParent)
{
	if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC) {
        #if defined (XPAR_SCUGIC)
		u16 IntrNum = XGet_IntrId(IntrId);
		u16 Offset = XGet_IntrOffset(IntrId);

		IntrNum += Offset;
		XScuGic_Disconnect(&XScuGicInstance, IntrNum);
	#else
		return XST_FAILURE;
	#endif
	} else {
	#if defined (XPAR_AXI_INTC)
		XIntc_Disconnect(&XIntcInstance, IntrId);
	#else
		return XST_FAILURE;
	#endif
	}
	return XST_FAILURE;
}

/*****************************************************************************/
/**
*
* @brief    Starts the interrupt controller.
*
* @param    Mode: Interrupt controller mode type.
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   XST_SUCCESS if initialization was successful
* 	    XST_FAILURE in case of failure
*
* @note     None.
*
******************************************************************************/
int XStartInterruptCntrl(u32 Mode, UINTPTR IntcParent)
{
#if defined (XPAR_AXI_INTC)
	int Status = XST_FAILURE;
#else
	(void) Mode;
#endif
	if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC)
	{
		/*
		 * For XPAR_SCUGIC, XConfigInterruptCntrl starts controller
		 * hence returning without doing anything
		 */
		return 0;
	} else  {
	#if defined (XPAR_AXI_INTC)
		if (XIntcInstance.IsStarted != XIL_COMPONENT_IS_STARTED)
			Status = XIntc_Start(&XIntcInstance, Mode);
		else
			Status = XST_SUCCESS;
		return Status;
	#else
		return XST_FAILURE;
	#endif

	}

}

/*****************************************************************************/
/**
*
* @brief    Enable the interrupt id.
*
* @param    IntrId: Interrupt Id.
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void XEnableIntrId( u32 IntrId, UINTPTR IntcParent)
{
	if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC)
	{
	#if defined (XPAR_SCUGIC)
		u16 IntrNum = XGet_IntrId(IntrId);
		u16 Offset = XGet_IntrOffset(IntrId);
		IntrNum += Offset;
		XScuGic_Enable(&XScuGicInstance, IntrNum);
	#endif

	} else {
	#if defined (XPAR_AXI_INTC)
		XIntc_Enable(&XIntcInstance, IntrId);
	#endif
	}

}

/*****************************************************************************/
/**
*
* @brief    disable the interrupt id.
*
* @param    IntrId: Interrupt Id.
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void XDisableIntrId( u32 IntrId, UINTPTR IntcParent)
{
		if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC)
		{
		#if defined (XPAR_SCUGIC)
			u16 IntrNum = XGet_IntrId(IntrId);
			u16 Offset = XGet_IntrOffset(IntrId);

			IntrNum += Offset;
			XScuGic_Disable(&XScuGicInstance, IntrNum);
		#endif
		} else {
		#if defined (XPAR_AXI_INTC)
			XIntc_Disable(&XIntcInstance, IntrId);
		#endif
		}

}

/*****************************************************************************/
/**
*
* @brief    Configures the priority and trigger type.
*
* @param    IntrId: Interrupt Id.
* @param    Priority: Priority of the interrupt
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void XSetPriorityTriggerType( u32 IntrId, u8 Priority, UINTPTR IntcParent)
{
#if defined (XPAR_SCUGIC)
	u8 Trigger = (((XGet_TriggerType(IntrId) == 1) || (XGet_TriggerType(IntrId) == 2)) ? XINTR_IS_EDGE_TRIGGERED
                                                                                : XINTR_IS_LEVEL_TRIGGERED);
#else
	(void) Priority;
#endif
                u16 IntrNum = XGet_IntrId(IntrId);
                u16 Offset = XGet_IntrOffset(IntrId);

                IntrNum += Offset;
                if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC)
                {
                #if defined (XPAR_SCUGIC)
                                XScuGic_SetPriorityTriggerType(&XScuGicInstance, IntrNum, Priority, Trigger);
                #endif
                }


}

/*****************************************************************************/
/**
*
* @brief    Gets the priority of the interrupt controller.
*
* @param    IntrId: Interrupt Id.
* @param    Priority: Priority of the interrupt
* @param    Trigger: Trigger type of the interrupt
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void XGetPriorityTriggerType( u32 IntrId, u8 *Priority, u8 *Trigger,  UINTPTR IntcParent)
{
#if !defined (XPAR_SCUGIC)
	(void) IntrId;
	(void) Priority;
	(void) Trigger;
#endif
	if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC)
	{
		#if defined (XPAR_SCUGIC)
			XScuGic_GetPriorityTriggerType(&XScuGicInstance, IntrId, Priority, Trigger);
		#endif
	}
}

/*****************************************************************************/
/**
*
* @brief    stops the interrupt controller.
*
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void XStopInterruptCntrl( UINTPTR IntcParent)
{
	if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC)
	{
		#if defined (XPAR_SCUGIC)
		XScuGic_Stop(&XScuGicInstance);
		#endif
		} else {
		#if defined (XPAR_AXI_INTC)
		XIntc_Stop(&XIntcInstance);
		#endif

	}

}

/*****************************************************************************/
/**
*
* @brief    Registers the interrupt handler.
*
* @param    IntrHandler: Interrupt handler.
* @param    IntcParent: Interrupt controller baseaddress and type.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void XRegisterInterruptHandler(void *IntrHandler,  UINTPTR IntcParent)
{
	if (XGet_IntcType(IntcParent) == XINTC_TYPE_IS_SCUGIC)
	{
	#if defined (XPAR_SCUGIC)
		if (IntrHandler == NULL)
		{
			Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, \
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			&XScuGicInstance);
		} else {
			Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, \
				(Xil_ExceptionHandler) IntrHandler,
				&XScuGicInstance);

		}
	#endif
	} else {
	#if defined (XPAR_AXI_INTC)
		if (IntrHandler == NULL)
		{
			Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, \
				(Xil_ExceptionHandler) XIntc_InterruptHandler,
				&XIntcInstance);
		} else {
			Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, \
				(Xil_ExceptionHandler) IntrHandler,
				&XIntcInstance);
		}
	#endif
	}
}

/*****************************************************************************/
/**
*
* @brief    Setup the interrupt system.
*
* @param    DriverInstance: Driver instance pointer.
* @param    IntrHandler: Interrupt handler funtion pointer.
* @param    IntrId: Interrupt Id.
* @param    IntcParent: Interrupt controller baseaddress and type.
* @param    Priority: Interrupt priority.
*
* @return   XST_SUCCESS if initialization was successful
* 	    XST_FAILURE in case of failure
*
* @note     None.
*
******************************************************************************/
int XSetupInterruptSystem(void *DriverInstance, void *IntrHandler, u32 IntrId,  UINTPTR IntcParent, u16 Priority)
{
	int Status;

	Status = XConfigInterruptCntrl(IntcParent);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;
	XSetPriorityTriggerType( IntrId, Priority, IntcParent);
	Status = XConnectToInterruptCntrl( IntrId, (Xil_ExceptionHandler) IntrHandler, \
				DriverInstance, IntcParent);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;
	#if defined (XPAR_AXI_INTC)
	XStartInterruptCntrl(XIN_REAL_MODE, IntcParent);
	#endif
	XEnableIntrId(IntrId, IntcParent);
	XRegisterInterruptHandler(NULL, IntcParent);
	Xil_ExceptionInit();
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}
#endif
