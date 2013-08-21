/**************************************************************************//**
*   @file   axi_interrupts.c
*   @brief  AXI Interrupts handling functions driver file.
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/
#include "system_config.h"
#include <stdio.h>
#include "xil_io.h"

#if(USE_PS7 == 0)
#include "axi_interrupts.h"

extern volatile int  rxData;
extern volatile int  timeStepValue;

/******************************************************************************
* @brief Enable Interrupt Controller.
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiIntcInit(void)
{
	// Enable Interrupt Controller
	XIntc_MasterEnable(INTC_BASEADDR);
}

/******************************************************************************
* @brief Enable Uart Interrupts
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiUartIntrEnable(void)
{
	int prevIntrMask = 0;
	// Register UART Interrupt Service Routine
	XIntc_RegisterHandler(INTC_BASEADDR, UART_INTERRUPT, (XInterruptHandler)AxiUartIntrHandler, (void *) 0);
	prevIntrMask = Xil_In32(INTC_BASEADDR + XIN_IER_OFFSET);
	XIntc_EnableIntr(INTC_BASEADDR, prevIntrMask | UART_INTERRUPT_MASK);
	// Enable UART Interrupts
	Xil_Out32(UART_BASEADDR + 0x0C, (1 << 4));
	// Enable MicroBlaze Interrupts
	microblaze_enable_interrupts();
}

/******************************************************************************
* @brief UART Interrupt Handler.
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiUartIntrHandler(void)
{
	// If UART has triggered a RX DATA Interrupt
	// store the read value in rxData
	if(Xil_In32(UART_BASEADDR + 0x08) & 0x01)
	{
		// Save read data from UART
		rxData = Xil_In32(UART_BASEADDR);
	}
}

/******************************************************************************
* @brief Enable Timer Interrupts
*
* @param None.
*
* @return None.
******************************************************************************/
#if(USE_TIMER == 1)
void AxiTimerIntrEnable(void)
{
	int prevIntrMask = 0;
	// Register UART Interrupt Service Routine
	XIntc_RegisterHandler(INTC_BASEADDR, TIMER_INTERRUPT, (XInterruptHandler)AxiTimerIntrHandler, (void *) 0);
	prevIntrMask = Xil_In32(INTC_BASEADDR + XIN_IER_OFFSET);
	XIntc_EnableIntr(INTC_BASEADDR, prevIntrMask | TIMER_INTERRUPT_MASK);
	// Enable TIMER Initial Settings
	Xil_Out32(TIMER_BASEADDR + 0x04, timeStepValue);
	Xil_Out32(TIMER_BASEADDR + 0x00, 0x00000060);
	// Enable MicroBlaze Interrupts
	microblaze_enable_interrupts();
}
#endif

/******************************************************************************
* @brief Timer Interrupt Handler
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiTimerIntrHandler(void)
{
	TimerIntrFunction();
}

#if(USE_EXTERNAL == 1)
/******************************************************************************
* @brief Enable External Interrupts
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiExtIntrEnable(void)
{
	int prevIntrMask = 0;
	// Register UART Interrupt Service Routine
	XIntc_RegisterHandler(INTC_BASEADDR, EXT_INTERRUPT, (XInterruptHandler)AxiExtIntrHandler, (void *) 0);
	prevIntrMask = Xil_In32(INTC_BASEADDR + XIN_IER_OFFSET);
	XIntc_EnableIntr(INTC_BASEADDR, prevIntrMask | EXT_INTERRUPT_MASK);
	// Set GPIO as Inputs
	Xil_Out32(EXT_BASEADDR + 0x04, 0x0F);
	// Enable GPIO Interrupts
	Xil_Out32(EXT_BASEADDR + 0x11C, 0xFFFFFFFF);
	Xil_Out32(EXT_BASEADDR + 0x128, 0x01);
	// Enable MicroBlaze Interrupts
	microblaze_enable_interrupts();
}
#endif
/******************************************************************************
* @brief External Interrupt Handler
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiExtIntrHandler(void)
{
	ExtIntrFunction();
}

/******************************************************************************
* @brief Enable Interrupts System
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiIntrInit(void)
{
	// Initialize interrupt controller
	AxiIntcInit();

	// Enable UART Interrupts
	AxiUartIntrEnable();

	if(USE_TIMER == 1)
	{
		// Enable Timer Interrupts
		AxiTimerIntrEnable();
	}

	if(USE_EXTERNAL == 1)
	{
		// Enable External Interrupts
		AxiExtIntrEnable();
	}
}

/******************************************************************************
* @brief Enable Interrupts
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiIntrEnable(int intrNr)
{
	// Enable interrupts
	microblaze_enable_interrupts();
}

/******************************************************************************
* @brief Disable Interrupts
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiIntrDisable(int intrNr)
{
	// Disable interrupts in order to use normal polling
	microblaze_disable_interrupts();
}

/******************************************************************************
* @brief Read a single char from Uart
*
* @param None.
*
* @return None.
******************************************************************************/
char AxiUartReadChar(void)
{
	char rx;

	// Check if data is available on the UART
	while (!((Xil_In32(UART_BASEADDR + 0x08)) & 0x01));
	rx = Xil_In32(UART_BASEADDR);

	return(rx);
}
#endif
