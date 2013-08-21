/**************************************************************************//**
*   @file   axi_interrupts.h
*   @brief  AXI Interrupts handling functions header file.
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

#ifndef AXI_INTERRUPTS_H_
#define AXI_INTERRUPTS_H_

#if(USE_PS7 == 0)
#include "xparameters.h"
#include "mb_interface.h"
#include "xintc_l.h"
#include "Xintc_l.h"

// Interrupt controller
#define INTC_BASEADDR			XPAR_AXI_INTC_0_BASEADDR
// UART
#define UART_BASEADDR			XPAR_USB_UART_BASEADDR
#define UART_INTERRUPT			XPAR_AXI_INTC_0_USB_UART_INTERRUPT_INTR
#define UART_INTERRUPT_MASK		XPAR_USB_UART_INTERRUPT_MASK
// Timer
#define TIMER_BASEADDR			XPAR_AXI_TIMER_0_BASEADDR
#define TIMER_INTERRUPT			XPAR_AXI_INTC_0_AXI_TIMER_0_INTERRUPT_INTR
#define TIMER_INTERRUPT_MASK	XPAR_AXI_TIMER_0_INTERRUPT_MASK
// External Interrupts
#define EXT_BASEADDR			XPAR_AXI_GPIO_0_BASEADDR
#define EXT_INTERRUPT			XPAR_AXI_INTC_0_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define EXT_INTERRUPT_MASK		XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK
// LEDs
#define LED_BASEADDR 			XPAR_LEDS_4BITS_BASEADDR

void AxiUartIntrHandler(void);
void AxiTimerIntrHandler(void);
void AxiIntcInit(void);
void AxiUartIntrEnable(void);
void AxiTimerIntrEnable(void);
void AxiIntrInit(void);
void AxiIntrEnable(int intrNr);
void AxiIntrDisable(int intrNr);
char AxiUartReadChar(void);
void AxiExtIntrEnable(void);
void AxiExtIntrHandler(void);
#endif

#endif /* AXI_INTERRUPTS_H_ */
