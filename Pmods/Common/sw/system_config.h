/**************************************************************************//**
*   @file   system_config.h
*   @brief  System configuration header file
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

#ifndef SYSTEM_CONFIG_H_
#define SYSTEM_CONFIG_H_

// Select between PS7 or AXI Interface
#define USE_PS7 	 1
// SPI used in the design
#define USE_SPI		 1
// I2C used in the design
#define USE_I2C		 0
// Timer (+interrupts) used in the design
#define USE_TIMER	 1
// External interrupts used in the design
#define USE_EXTERNAL 0
// GPIO used in the design
#define USE_GPIO     0

#if(USE_PS7 == 1)
	#include "spi_ps7.h"
	#include "i2c_axi.h"
	#include "ps7_interrupts.h"
	#include "xscugic.h"
	#include "xil_exception.h"
#else
	#include "spi_axi.h"
	#include "i2c_axi.h"
	#include "axi_interrupts.h"
#endif

// Generic functions pointers
u32 (*SPI_Init)(u32, char, char, char);
u32 (*SPI_TransferData)(u32, char, char*, char, char*, char);
u32	(*I2C_Init)(u32, u32);
u32 (*I2C_Read)(u32, u32, u32, u32, unsigned char*);
u32 (*I2C_Write)(u32, u32, u32, u32, unsigned char*);
void (*InterruptsInit)(void);
void (*DisableInterrupts)(int);
void (*EnableInterrupts)(int);
char (*UartReadChar)(void);
void (*TimerIntrFunction)(void);
void (*StartTimer)(void);
void (*StopTimer)(void);
void (*LoadTimer)(int);
void (*ExtIntrFunction)(void);
void (*GpioOut)(int, int);

// System configuration functions
void SystemSpiConfiguration(void);
void SystemConfiguration(void);

// AXI Functions
u32 SPI_Init_axi(u32 axiBaseAddr, char lsbFirst, char cpha, char cpol);
u32 SPI_TransferData_axi(u32 axiBaseAddr, char txSize, char* txBuf, char rxSize, char* rxBuf, char ssNo);
u32	I2C_Init_axi(u32 axiBaseAddr, u32 i2cAddr);
u32 I2C_Read_axi(u32 axiBaseAddr, u32 i2cAddr, u32 regAddr, u32 rxSize, unsigned char* rxBuf);
u32 I2C_Write_axi(u32 axiBaseAddr, u32 i2cAddr, u32 regAddr, u32 txSize, unsigned char* txBuf);
void AxiIntrInit(void);
void AxiIntrEnable(int intrNr);
void AxiIntrDisable(int intrNr);
char AxiUartReadChar(void);
void AxiTimerIntrFunction(void);
void AxiStartTimer(void);
void AxiStopTimer(void);
void AxiLoadTimer(int value);
void AxiGpioOut(int addr, int value);

// PS7 Functions
u32 SPI_Init_ps7(u32 axiBaseAddr, char lsbFirst, char cpha, char cpol);
u32 SPI_TransferData_ps7(u32 axiBaseAddr, char txSize, char* txBuf, char rxSize, char* rxBuf, char ssNo);
void Ps7IntrInit(void);
void Ps7IntrEnable(int intrNr);
void Ps7IntrDisable(int intrNr);
char Ps7UartReadChar(void);
void Ps7imerIntrFunction(void);
void Ps7StartTimer(void);
void Ps7StopTimer(void);
void Ps7LoadTimer(int value);
void Ps7GpioOut(int addr, int value);

// Interrupt Handlers
void ADITimerIntrFunction(void);
void ADIExtIntrFunction(void);

#endif /* SYSTEM_CONFIG_H_ */
