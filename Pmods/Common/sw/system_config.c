/**************************************************************************//**
*   @file   system_config.c
*   @brief  System configuration driver file
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

#if(USE_TIMER == 1)

/******************************************************************************
* @brief Start PS7 Timer
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7StartTimer(void)
{
	// Start Timer
	Xil_Out32(TIMER_BASEADDR + 0x0C, 0x06);
}

/******************************************************************************
* @brief Stop PS7 Timer
*
* @param None.
*
* @return None.
******************************************************************************/
void Ps7StopTimer(void)
{
	while(Xil_In32(TIMER_BASEADDR + 0x18) == 0x00);
	Xil_Out32(TIMER_BASEADDR + 0x0C, 0x07);
}

/******************************************************************************
* @brief Load PS7 Timer
*
* @param value - value to be loaded in timer.
*
* @return None.
******************************************************************************/
void Ps7LoadTimer(int value)
{
	Xil_Out32(TIMER_BASEADDR + 0x24, value);
}

/******************************************************************************
* @brief Start AXI Timer
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiStartTimer(void)
{
	// Start Timer
	Xil_Out32(TIMER_BASEADDR + 0x00, 0x000001C0);
}

/******************************************************************************
* @brief Stop AXI Timer
*
* @param None.
*
* @return None.
******************************************************************************/
void AxiStopTimer(void)
{
    // Stop Timer
	Xil_Out32(TIMER_BASEADDR + 0x00, 0x00000000);
}

/******************************************************************************
* @brief Load AXI Timer
*
* @param value - value to be loaded in timer.
*
* @return None.
******************************************************************************/
void AxiLoadTimer(int value)
{
	Xil_Out32(TIMER_BASEADDR + 0x04, value);
	Xil_Out32(TIMER_BASEADDR + 0x00, 0x60);
}

#else
	void ADITimerIntrFunction(void)
	{

	}
	volatile int timeStepValue = 0;
#endif

#if(USE_EXTERNAL == 1)

#else
	void ADIExtIntrFunction(void)
	{

	}
#endif
/******************************************************************************
* @brief Output GPIO PS7
*
* @param value - value to be output
*
* @return None.
******************************************************************************/
void Ps7GpioOut(int addr, int value)
{
	Xil_Out32(addr + 0x48, value);
}

/******************************************************************************
* @brief Output GPIO AXI
*
* @param value - value to be output
*
* @return None.
******************************************************************************/
void AxiGpioOut(int addr, int value)
{
	Xil_Out32(addr, value);
}

/******************************************************************************
* @brief Select which SPI functions to use according to system (AXI or PS7)
*
* @param None.
*
* @return None.
******************************************************************************/
void SystemSpiConfiguration(void)
{
	// Select between HW SPI or AXI SPI Core
	if(USE_PS7 == 1)
	{
		SPI_Init = &SPI_Init_ps7;
		SPI_TransferData = &SPI_TransferData_ps7;
	}
	else
	{
		SPI_Init = &SPI_Init_axi;
		SPI_TransferData = &SPI_TransferData_axi;
	}
}

/******************************************************************************
* @brief Select which Interrupt functions to use according to system (AXI or PS7)
*
* @param None.
*
* @return None.
******************************************************************************/
void SystemConfiguration(void)
{
	// Select between PS7 or AXI interfaces
	if(USE_PS7 == 1)
	{
		// Interrupt management functions
		InterruptsInit = &Ps7IntrInit;
		DisableInterrupts = &Ps7IntrDisable;
		EnableInterrupts = &Ps7IntrEnable;

		// Read character from UART function
		UartReadChar = &Ps7UartReadChar;

		// SPI driver
		if(USE_SPI == 1)
		{
			SPI_Init = &SPI_Init_ps7;
			SPI_TransferData = &SPI_TransferData_ps7;
		}

		// I2C driver
		if(USE_I2C == 1)
		{
			I2C_Init = &I2C_Init_axi;
			I2C_Write = &I2C_Write_axi;
			I2C_Read = &I2C_Read_axi;
		}

		// Timer driver
		if(USE_TIMER == 1)
		{
			TimerIntrFunction = &ADITimerIntrFunction;
			StartTimer = &Ps7StartTimer;
			StopTimer = &Ps7StopTimer;
			LoadTimer = &Ps7LoadTimer;
		}

		// External interrupts handling
		if(USE_EXTERNAL == 1)
		{
			ExtIntrFunction = &ADIExtIntrFunction;
		}

		// GPIO manipulation
		if(USE_GPIO == 1)
		{
			GpioOut = &Ps7GpioOut;
		}
	}
	else
	{
		// Interrupt management functions
		InterruptsInit = &AxiIntrInit;
		DisableInterrupts = &AxiIntrDisable;
		EnableInterrupts = &AxiIntrEnable;

		// Read character from UART function
		UartReadChar = &AxiUartReadChar;

		// SPI driver
		if(USE_SPI == 1)
		{
			SPI_Init = &SPI_Init_axi;
			SPI_TransferData = &SPI_TransferData_axi;
		}

		// I2C driver
		if(USE_I2C == 1)
		{
			I2C_Init = &I2C_Init_axi;
			I2C_Write = &I2C_Write_axi;
			I2C_Read = &I2C_Read_axi;
		}

		// Timer driver
		if(USE_TIMER == 1)
		{
			TimerIntrFunction = &ADITimerIntrFunction;
			StartTimer = &AxiStartTimer;
			StopTimer = &AxiStopTimer;
			LoadTimer = &AxiLoadTimer;
		}

		// External interrupts handling
		if(USE_EXTERNAL == 1)
		{
			ExtIntrFunction = &ADIExtIntrFunction;
		}
        
		// GPIO manipulation
		if(USE_GPIO == 1)
		{
			GpioOut = &AxiGpioOut;
		}
	}
}
