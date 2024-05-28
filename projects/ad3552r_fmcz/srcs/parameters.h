/***************************************************************************//**
 *   @file   ad3552r_fmcz/srcs/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#ifdef XILINX_PLATFORM
#include <xparameters.h>
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* GPIO Indexes */
#ifndef XILINX_PLATFORM
#define GPIO_RESET_N				0
#define GPIO_LDAC_N				1
#define GPIO_SPI_QPI				2
#define GPIO_ALERT_N				3
#define GPIO_SYNC_EVENTS			4
#define GPIO_RED				5 //GPIO_6
#define GPIO_GREEN				6 //GPIO_7
#define GPIO_BLUE				7 //GPIO_8
#define TOTAL_GPIOS				8
#else
#define GPIO_LDAC_N		0
#define GPIO_ALERT_N		1
#define GPIO_RED		2 /* GPIO_6 */
#define GPIO_GREEN		3 /* GPIO_7 */
#define GPIO_BLUE		4 /* GPIO_8 */
#define GPIO_9			5
#define GPIO_RESET_N		6
#define GPIO_SPI_QPI		7
#define TOTAL_GPIOS		8
#endif /* XILINX_PLATFORM */

#define GPIO_BANK_0_PINS	32
#define GPIO_BANK_1_PINS	22
#define GPIO_BANK_2_PINS	32
#define GPIO_BANK_3_OFFSET	(GPIO_BANK_0_PINS + GPIO_BANK_1_PINS + \
				 GPIO_BANK_2_PINS)

#define GPIO_OFFSET		GPIO_BANK_3_OFFSET

#define MAX_BUFF_SAMPLES	10000
#define UART_BAUDRATE		115200

#define SPI_DEVICE_ID		XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID		XPAR_PS7_GPIO_0_DEVICE_ID
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR

#endif // __PARAMETERS_H__
