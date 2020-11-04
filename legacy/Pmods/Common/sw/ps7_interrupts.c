/**************************************************************************//**
*   @file   ps7_interrupts.c
*   @brief  PS7 interrupts configuration driver file
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
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
*   SVN Revision: 468
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "system_config.h"

#if(USE_PS7 == 1)
#include "xparameters.h"
#include "xil_io.h"
#include "xil_exception.h"

XScuGic IntcInstance;               // Interrupt controller instance
extern volatile int rxData;         // Data read from UART
extern volatile int timeStepValue;  // Value to be loaded in timer

/******************************************************************************
* @brief Initialize Interrupt Controller
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7IntcInit(void)
{
	XScuGic_Config *IntcConfig;
	/* Initialize the interrupt controller driver */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);

	XScuGic_CfgInitialize(&IntcInstance, IntcConfig, IntcConfig->CpuBaseAddress);

	Xil_ExceptionInit();

	/* Connect the interrupt controller interrupt handler to the hardware interrupt logic */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &IntcInstance);
}

/******************************************************************************
* @brief Enable Uart Interrupts
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7UartIntrEnable(void)
{
	/* Connect device driver handler to be called when interrupt occures */
	XScuGic_Connect(&IntcInstance, UART_INTERRUPT, (Xil_ExceptionHandler)Ps7UartIntrHandler, (void *)0);

	/* Enable interrupts for the device */
	XScuGic_Enable(&IntcInstance, UART_INTERRUPT);

	/* Enable UART RX Empty interrupt */
	Xil_Out32(UART_BASEADDR + 0x08, 0x01);
	Xil_Out32(UART_BASEADDR + 0x0C, 0xFE);

	/* Clear Interrupt Flags */
	Xil_Out32(UART_BASEADDR + 0x14, 0xFF);

	/* Set RX trigger level */
	Xil_Out32(UART_BASEADDR + 0x20, 0x01);

	/* Enable interrupts in the processor */
	Xil_ExceptionEnable();
}

/******************************************************************************
* @brief Uart Interrupt Handler
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7UartIntrHandler(void)
{
	// Read data from RX Fifo
	rxData = Xil_In32(UART_BASEADDR + 0x30);

	// Clear interrupt
	Xil_Out32(UART_BASEADDR + 0x14, 0x01);
}

/******************************************************************************
* @brief Enable Timer Interrupts
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7TimerIntrEnable(void)
{
	/* Connect device driver handler to be called when interrupt occures */
	XScuGic_Connect(&IntcInstance, TIMER_INTERRUPT, (Xil_ExceptionHandler)Ps7TimerIntrHandler, (void *)0);

	/* Enable interrupts for the device */
	XScuGic_Enable(&IntcInstance, TIMER_INTERRUPT);

	// cnt cntrl
	Xil_Out32(TIMER_BASEADDR + 0x0C, 0x07);
	// clock control
	Xil_Out32(TIMER_BASEADDR + 0x00, 0x1B);
	// interval value
	Xil_Out32(TIMER_BASEADDR + 0x24, timeStepValue);
	// intr en
	Xil_Out32(TIMER_BASEADDR + 0x60, 0x01);

	/* Enable interrupts in the processor */
	Xil_ExceptionEnable();
}

/******************************************************************************
* @brief Timer Interrupt Handler
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7TimerIntrHandler(void)
{
	char interrupt;

	interrupt = Xil_In32(TIMER_BASEADDR + 0x54);

	if((interrupt & 0x01) == 0x01)
	{
		TimerIntrFunction();
	}
}

/******************************************************************************
* @brief Enable External Interrupts
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7ExtIntrEnable(void)
{
	/* Connect device driver handler to be called when interrupt occures */
	XScuGic_Connect(&IntcInstance, XPS_FPGA11_INT_ID, (Xil_ExceptionHandler)Ps7ExtIntrHandler, (void *)0);
	XScuGic_Connect(&IntcInstance, XPS_FPGA12_INT_ID, (Xil_ExceptionHandler)Ps7ExtIntrHandler, (void *)0);

	/* Enable interrupts for the device */
	XScuGic_Enable(&IntcInstance, XPS_FPGA11_INT_ID);
	XScuGic_Enable(&IntcInstance, XPS_FPGA12_INT_ID);

	/* Enable interrupts in the processor */
	Xil_ExceptionEnable();
}

/******************************************************************************
* @brief External Interrupt Handler
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7ExtIntrHandler(void)
{
	ExtIntrFunction();
}

/******************************************************************************
* @brief Initialize Interrupt System
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7IntrInit(void)
{
	// Initialize interrupt controller
	Ps7IntcInit();

	// Enable UART Interrupts
	Ps7UartIntrEnable();

	if(USE_TIMER == 1)
	{
		// Enable Timer Interrupts
		Ps7TimerIntrEnable();
	}

	if(USE_EXTERNAL == 1)
	{
		// Enable External Interrupts
		Ps7ExtIntrEnable();
	}
}

/******************************************************************************
* @brief Enable Interrupt
*
* @param intrNr - interrupt number.
*
* @return None.
******************************************************************************/
void Ps7IntrEnable(int intrNr)
{
	// Enable interrupts
	XScuGic_Enable(&IntcInstance, intrNr);
}

/******************************************************************************
* @brief Disable Interrupts
*
* @param intrNr - interrupt number
*
* @return None.
******************************************************************************/
void Ps7IntrDisable(int intrNr)
{
	// Disable interrupts in order to use normal polling
	XScuGic_Disable(&IntcInstance, intrNr);
}

/******************************************************************************
* @brief Read a single char from Uart
*
* @param None.
*
* @return None.
******************************************************************************/
char Ps7UartReadChar(void)
{
	char rx;

	// Check if data is available on the UART
	while (!((Xil_In32(UART_BASEADDR + 0x14)) & 0x01));
	rx = Xil_In32(UART_BASEADDR + 0x30);
	Xil_Out32(UART_BASEADDR + 0x14, 0x01);

	return(rx);
}
#endif
