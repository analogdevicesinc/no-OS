/***************************************************************************//**
*   @file   ADP5589.c
*   @brief  ADP5589 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
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
*
********************************************************************************
*   SVN Revision: $WCREV$
*******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdio.h>
#include "system_config.h"
#include "ADP5589.h"

volatile int rxData;

/******************************************************************************
 * @brief Decodes the key pressed on the Pmod-KYPD.
 *
 * @param rxData - Value received from ADP5589
 *
 * @return c - Actual Key on the Pmod-KYPD.
******************************************************************************/
char KeyDecode(int rxData)
{
    char c;
    
    switch(rxData)
    {
        case 0x25:
            c = '1';
            break;
        case 0x24:
            c = '4';
            break;
        case 0x23:
            c = '7';
            break;
        case 0x1A:
            c = '2';
            break;
        case 0x19:
            c = '5';
            break;
        case 0x18:
            c = '8';
            break;
        case 0x0F:
            c = '3';
            break;
        case 0x0E:
            c = '6';
            break;
        case 0x0D:
            c = '9';
            break;
        case 0x04:
            c = 'A';
            break;
        case 0x22:
            c = '0';
            break;
        case 0x17:
            c = 'F';
            break;
        case 0x0C:
            c = 'E';
            break;
        case 0x01:
            c = 'D';
            break;
        case 0x02:
            c = 'C';
            break;
        case 0x03:
            c = 'B';
            break;
        default:
            break;
    }
    return(c);
}

/******************************************************************************
 * @brief Flush the internal FIFO on the ADP5589.
 *
 * @param None.
 *
 * @return None.
******************************************************************************/
void FlushFIFO(void)
{
	int evCnt;
	unsigned char rxBuffer[3] = {0x00, 0x00, 0x00};
    
	I2C_Read_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, StatusReg, 1, rxBuffer);

    evCnt = (rxBuffer[0] & 0x1F);
    
    while(evCnt != 0)
    {
    	I2C_Read_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, FIFO1Reg, 1, rxBuffer);
    	I2C_Read_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, StatusReg, 1, rxBuffer);
	    evCnt = (rxBuffer[0] & 0x1F);
    }
    
    xil_printf("   Flushing Event FIFO...\n\r");
}

/******************************************************************************
 * @brief Initialize the internal registers on the ADP5589.
 *
 * @param None.
 *
 * @return None.
******************************************************************************/
void ADP5589_Init(void)
{
	unsigned char rxBuffer[3] = {0x00, 0x00, 0x00};
	unsigned char txBuffer[3] = {0x00, 0x00, 0x00};
    
	// Display Manufacturer ID and Revision ID
	I2C_Read_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, IDReg, 1, rxBuffer);
	xil_printf("PmodIOXP demonstration starting...\n\r");
	xil_printf("\n\r");
	xil_printf("------ ADP5589 ------\n\r");
	xil_printf("Manufacturer ID: %x\n\r", ((rxBuffer[0] & 0xF0) >> 4));
	xil_printf("Revision ID:     %x\n\r", (rxBuffer[0] & 0x0F));
    
	xil_printf("Setting internal registers...\n\r");
	txBuffer[0] = 0x0F;
	I2C_Write_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, PinCfgAReg, 1, txBuffer);
	I2C_Write_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, PinCfgBReg, 1, txBuffer);
    
	xil_printf("Keypad set to connector J1: R0-R3, C0-C3\n\r");
	txBuffer[0] = 0x80;
	I2C_Write_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, GenCfgBReg, 1, txBuffer);
    
	xil_printf("Internal oscillator enabled\n\r");
}

/******************************************************************************
 * @brief Prints the Pressed/Released Key on the UART.
 *
 * @param None.
 *
 * @return None.
******************************************************************************/
void ADP5589_KeyDecodeTest(void)
{
	unsigned char rxBuffer[3] = {0x00, 0x00, 0x00};
	unsigned char txBuffer[3] = {0x00, 0x00, 0x00};
	int evCnt;
    
	xil_printf("\n\r");
	xil_printf("Entering Key Decoder Test Mode...\n\r");
	xil_printf("Press / Release any key to test its response:\n\r");
	xil_printf("--- Pressing [F] will exit Key Decoder Test Mode.\n\r");

	while(KeyDecode((rxBuffer[0] & 0x7F)) != 'F')
	{
		do
		{
		   I2C_Read_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, INTStatusReg, 1, rxBuffer);
		}
		while((rxBuffer[0] & 0x01) == 0);

		I2C_Read_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, StatusReg, 1, rxBuffer);

		evCnt = (rxBuffer[0] & 0x1F);

		if(evCnt != 0)
		{
		   I2C_Read_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, FIFO1Reg, 1, rxBuffer);
		   if((rxBuffer[0] & 0x80) == 0x80)
		   {
			   xil_printf("Key pressed:  %c\n\r", KeyDecode((rxBuffer[0] & 0x7F)));
		   }
		   else
		   {
			   xil_printf("Key released: %c\n\r", KeyDecode((rxBuffer[0] & 0x7F)));
		   }
		}

		txBuffer[0] = 0x01;
		I2C_Write_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, INTStatusReg, 1, txBuffer);
	}

	xil_printf("Exiting Key Decoder Test Mode...\n\r");
}

/******************************************************************************
 * @brief Locks the ADP5589 and requests Password for unlock.
 *
 * @param None
 *
 * @return None.
******************************************************************************/
void ADP5589_KeyLockTest(void)
{
	unsigned char rxBuffer[3] = {0x00, 0x00, 0x00};
	unsigned char txBuffer[3] = {0x00, 0x00, 0x00};

	FlushFIFO();
	xil_printf("\n\r");
	xil_printf("Entering Key Lock Test Mode...\n\r");

	xil_printf("--- The correct combination for unlock is [1][A].\n\r");
	txBuffer[0] = 0x80 | 0x25;
	I2C_Write_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, Unlock1Reg, 1, txBuffer);
	txBuffer[0] = 0x80 | 0x04;
	I2C_Write_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, Unlock2Reg, 1, txBuffer);
	txBuffer[0] = 0x01;
	I2C_Write_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, LockCfgReg, 1, txBuffer);

	do
	{
		I2C_Read_axi(I2C_BASEADDR, ADP5589_I2C_ADDR, StatusReg, 1, rxBuffer);
	}
	while((rxBuffer[0] & 0x20) != 0);

	FlushFIFO();

	xil_printf("Correct unlock combination entered. FIFO is now unlocked.\n\r");
}

/******************************************************************************
 * @brief Demo program for the ADP5589.
 *
 * @param None
 *
 * @return None.
******************************************************************************/
void DemoProgram(void)
{
	ADP5589_KeyDecodeTest();

	ADP5589_KeyLockTest();
}
