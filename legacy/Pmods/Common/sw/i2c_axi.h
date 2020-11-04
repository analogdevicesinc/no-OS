/**************************************************************************//**
*   @file   i2c_axi.h
*   @brief  I2C header file.
*   @author acozma (andrei.cozma@analog.com)
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

#ifndef __I2C_AXI_H__
#define __I2C_AXI_H__

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "xil_types.h"

/*****************************************************************************/
/******************* I2C Registers Definitions *******************************/
/*****************************************************************************/
#define GIE          0x01C
#define ISR       	 0x020
#define IER       	 0x028
#define SOFTR      	 0x040
#define CR      	 0x100
#define SR      	 0x104
#define TX_FIFO  	 0x108
#define RX_FIFO  	 0x10C
#define ADR       	 0x110
#define TX_FIFO_OCY	 0x114
#define RX_FIFO_OCY	 0x118
#define TEN_ADDR     0x11C
#define RX_FIFO_PIRQ 0x120
#define GPO			 0x124

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
u32	I2C_Init_axi(u32 axiBaseAddr, u32 i2cAddr);
u32 I2C_Read_axi(u32 axiBaseAddr, u32 i2cAddr, u32 regAddr, u32 rxSize, unsigned char* rxBuf);
u32 I2C_Write_axi(u32 axiBaseAddr, u32 i2cAddr, u32 regAddr, u32 txSize, unsigned char* txBuf);

#endif /* __I2C_H__ */
