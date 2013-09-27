/***************************************************************************//**
*   @file   AD7193.c
*   @brief  Implementation of AD7193 Driver.
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
#include "xil_io.h"
#include "xparameters.h"
#include "stdio.h"
#include "stdlib.h"
#include "system_config.h"
#include "AD7193.h"

volatile int rxData = 0x00;

/******************************************************************************
* @brief Write data to the Comm Register.
*
* @param txData - Data to be written in the Comm Register.
*
* @return None.
******************************************************************************/
void WriteCommReg(char txData)
{
	char txBuffer[1] = {0x00};
	char rxBuffer[1] = {0x00};

	txBuffer[0] = txData;
	SPI_TransferData(SPI_BASEADDR, CommunicationsRegLen, txBuffer, CommunicationsRegLen, rxBuffer, 1);
}

/******************************************************************************
* @brief Read data from the Status Register.
*
* @param None.
*
* @return rxData - Read data.
******************************************************************************/
unsigned char ReadStatusReg(void)
{
	char txBuffer[1] = {0x00};
	char rxBuffer[1] = {0x00};
	char commValue   = 0;
	unsigned char rxData = 0;
    
	commValue = (0 << WEN) |
			    (1 << RnW) |
			    (0 << RS2) |
			    (0 << RS1) |
			    (0 << RS0) |
			    (0 << CREAD);

    WriteCommReg(commValue);
    
    txBuffer[0] = 0xFF;
    SPI_TransferData(SPI_BASEADDR, StatusRegLen, txBuffer, StatusRegLen, rxBuffer, 1);
	rxData = rxBuffer[0];

	return(rxData);
}

unsigned char WaitForReadyBit(void)
{
	unsigned char rxData = 0x00;

	rxData = ((ReadStatusReg() & (1 << RDY)) >> RDY);

	return(rxData);
}

/******************************************************************************
* @brief Write data to the Mode Register.
*
* @param txData - Data to be written in the Mode Register.
*
* @return None.
******************************************************************************/
void WriteModeReg(int txData)
{
	char txBuffer[3] = {0x00, 0x00, 0x00};
	char rxBuffer[3] = {0x00, 0x00, 0x00};
	char commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (0 << RnW) |
			    (0 << RS2) |
			    (0 << RS1) |
			    (1 << RS0) |
			    (0 << CREAD);
                
	WriteCommReg(commValue);

	txBuffer[0] = (txData & 0xFF0000) >> 16;
	txBuffer[1] = (txData & 0x00FF00) >> 8;
	txBuffer[2] = (txData & 0x0000FF);

	SPI_TransferData(SPI_BASEADDR, ModeRegLen, txBuffer, ModeRegLen, rxBuffer, 1);
}

/******************************************************************************
* @brief Read data from the Mode Register.
*
* @param None.
*
* @return rxData - Read data.
******************************************************************************/
unsigned int ReadModeReg(void)
{
	unsigned int rxData      = 0;
	char         txBuffer[3] = {0x00, 0x00, 0x00};
	char         rxBuffer[3] = {0x00, 0x00, 0x00};
	char         commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (1 << RnW) |
			    (0 << RS2) |
			    (0 << RS1) |
			    (1 << RS0) |
			    (0 << CREAD);

	WriteCommReg(commValue);

	txBuffer[0] = 0xFF;
	txBuffer[1] = 0xFF;
	txBuffer[2] = 0xFF;

	SPI_TransferData(SPI_BASEADDR, ModeRegLen, txBuffer, ModeRegLen, rxBuffer, 1);

	rxData = ((rxBuffer[0] & 0xFF) << 16) |
			 ((rxBuffer[1] & 0xFF) << 8)  |
			 ((rxBuffer[2] & 0xFF));

	return(rxData);
}

/******************************************************************************
* @brief Write data to the Config Register.
*
* @param txData - Data to be written in the Config Register.
*
* @return None.
******************************************************************************/
void WriteConfigReg(int txData)
{
	char txBuffer[3] = {0x00, 0x00, 0x00};
	char rxBuffer[3] = {0x00, 0x00, 0x00};
	char commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (0 << RnW) |
			    (0 << RS2) |
			    (1 << RS1) |
			    (0 << RS0) |
			    (0 << CREAD);
    
	WriteCommReg(commValue);

	txBuffer[0] = (txData & 0xFF0000) >> 16;
	txBuffer[1] = (txData & 0x00FF00) >> 8;
	txBuffer[2] = (txData & 0x0000FF);

	SPI_TransferData(SPI_BASEADDR, ConfigurationRegLen, txBuffer, ConfigurationRegLen, rxBuffer, 1);
}

/******************************************************************************
* @brief Read data from the Config Register.
*
* @param None.
*
* @return rxData - Read data.
******************************************************************************/
unsigned int ReadConfigReg(void)
{
	unsigned int rxData      = 0;
	char 		 txBuffer[3] = {0x00, 0x00, 0x00};
	char 		 rxBuffer[3] = {0x00, 0x00, 0x00};
	char 		 commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (1 << RnW) |
			    (0 << RS2) |
			    (1 << RS1) |
			    (0 << RS0) |
			    (0 << CREAD);

	WriteCommReg(commValue);

	txBuffer[0] = 0xFF;
	txBuffer[1] = 0xFF;
	txBuffer[2] = 0xFF;

	SPI_TransferData(SPI_BASEADDR, ConfigurationRegLen, txBuffer, ConfigurationRegLen, rxBuffer, 1);

	rxData = ((rxBuffer[0] & 0xFF) << 16) |
			 ((rxBuffer[1] & 0xFF) << 8)  |
			 ((rxBuffer[2] & 0xFF));

	return(rxData);
}

/******************************************************************************
* @brief Read data from the Data Register.
*
* @param None.
*
* @return rxData - Read data.
******************************************************************************/
unsigned int ReadDataReg(void)
{
	unsigned int rxData      = 0;
	char 		 txBuffer[3] = {0x00, 0x00, 0x00};
	char 		 rxBuffer[3] = {0x00, 0x00, 0x00};
	char 		 commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (1 << RnW) |
			    (0 << RS2) |
			    (1 << RS1) |
			    (1 << RS0) |
			    (0 << CREAD);
                
	WriteCommReg(commValue);

	txBuffer[0] = 0xFF;
	txBuffer[1] = 0xFF;
	txBuffer[2] = 0xFF;

	SPI_TransferData(SPI_BASEADDR, DataRegLen, txBuffer, DataRegLen, rxBuffer, 1);

	rxData = ((rxBuffer[0] & 0xFF) << 16) |
			 ((rxBuffer[1] & 0xFF) << 8)  |
			 ((rxBuffer[2] & 0xFF));

	return(rxData);
}

/******************************************************************************
* @brief Read data from the Data + Status Register.
*
* @param None.
*
* @return rxData - Read data.
******************************************************************************/
unsigned int ReadDataInfoReg(void)
{
	unsigned int rxData      = 0;
	char 		 txBuffer[4] = {0x00, 0x00, 0x00, 0x00};
	char 		 rxBuffer[4] = {0x00, 0x00, 0x00, 0x00};
	char 		 commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (1 << RnW) |
			    (0 << RS2) |
			    (1 << RS1) |
			    (1 << RS0) |
			    (0 << CREAD);
                
	WriteCommReg(commValue);

	txBuffer[0] = 0xFF;
	txBuffer[1] = 0xFF;
	txBuffer[2] = 0xFF;
	txBuffer[3] = 0xFF;

	SPI_TransferData(SPI_BASEADDR, DataInfoRegLen, txBuffer, DataInfoRegLen, rxBuffer, 1);

	rxData = ((rxBuffer[0] & 0xFF) << 24) |
			 ((rxBuffer[1] & 0xFF) << 16) |
			 ((rxBuffer[2] & 0xFF) << 8)  |
			 ((rxBuffer[3] & 0xFF));
    
	return(rxData);
}

/******************************************************************************
* @brief Read data from the ID Register.
*
* @param None.
*
* @return rxData - Read data.
******************************************************************************/
unsigned char ReadIDReg(void)
{
	unsigned char rxData 	  = 0;
	char 		  txBuffer[1] = {0x00};
	char 		  rxBuffer[1] = {0x00};
	char 		  commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (1 << RnW) |
			    (1 << RS2) |
			    (0 << RS1) |
			    (0 << RS0) |
			    (0 << CREAD);
    
	WriteCommReg(commValue);

	txBuffer[0] = 0xFF;

	SPI_TransferData(SPI_BASEADDR, IDRegLen, txBuffer, IDRegLen, rxBuffer, 1);

	rxData = rxBuffer[0];

	return(rxData);
}

/******************************************************************************
* @brief Write data to the GPOCON Register.
*
* @param txData - Data to be written in the GPOCON Register.
*
* @return None.
******************************************************************************/
void WriteGPOCONReg(char txData)
{
	char txBuffer[1] = {0x00};
	char rxBuffer[1] = {0x00};
	char commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (0 << RnW) |
			    (1 << RS2) |
			    (0 << RS1) |
			    (1 << RS0) |
			    (0 << CREAD);
                
	WriteCommReg(commValue);

	txBuffer[0] = txData;

	SPI_TransferData(SPI_BASEADDR, GPOCONRegLen, txBuffer, GPOCONRegLen, rxBuffer, 1);
}

/******************************************************************************
* @brief Read data from the GPOCON Register.
*
* @param None.
*
* @return rxData - Read data.
******************************************************************************/
unsigned char ReadGPOCONReg(void)
{
	unsigned char rxData      = 0x00;
	char 		  txBuffer[1] = {0x00};
	char 		  rxBuffer[1] = {0x00};
	char 		  commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (1 << RnW) |
			    (1 << RS2) |
			    (0 << RS1) |
			    (1 << RS0) |
			    (0 << CREAD);

	WriteCommReg(commValue);

	txBuffer[0] = 0xFF;

	SPI_TransferData(SPI_BASEADDR, GPOCONRegLen, txBuffer, GPOCONRegLen, rxBuffer, 1);

	rxData = rxBuffer[0];
    
	return(rxData);
}

/******************************************************************************
* @brief Write data to the Offset Register.
*
* @param txData - Data to be written in the Offset Register.
*
* @return None.
******************************************************************************/
void WriteOffsetReg(int txData)
{
	char txBuffer[3] = {0x00, 0x00, 0x00};
	char rxBuffer[3] = {0x00, 0x00, 0x00};
	char commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (0 << RnW) |
			    (1 << RS2) |
			    (1 << RS1) |
			    (0 << RS0) |
			    (0 << CREAD);

	WriteCommReg(commValue);

	txBuffer[0] = (txData & 0xFF0000) >> 16;
	txBuffer[1] = (txData & 0x00FF00) >> 8;
	txBuffer[2] = (txData & 0x0000FF);

	SPI_TransferData(SPI_BASEADDR, OffsetRegLen, txBuffer, OffsetRegLen, rxBuffer, 1);
}

/******************************************************************************
* @brief Read data from the Offset Register.
*
* @param None.
*
* @return rxData - Read data.
******************************************************************************/
unsigned int ReadOffsetReg(void)
{
	unsigned int rxData 	 = 0;
	char 		 txBuffer[3] = {0x00, 0x00, 0x00};
	char 		 rxBuffer[3] = {0x00, 0x00, 0x00};
	char 		 commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (1 << RnW) |
			    (1 << RS2) |
			    (1 << RS1) |
			    (0 << RS0) |
			    (0 << CREAD);
    
	WriteCommReg(commValue);

	txBuffer[0] = 0xFF;
	txBuffer[1] = 0xFF;
	txBuffer[2] = 0xFF;

	SPI_TransferData(SPI_BASEADDR, OffsetRegLen, txBuffer, OffsetRegLen, rxBuffer, 1);
    
	rxData = ((rxBuffer[0] & 0xFF) << 16) |
			 ((rxBuffer[1] & 0xFF) << 8)  |
			 ((rxBuffer[2] & 0xFF));

	return(rxData);
}

/******************************************************************************
* @brief Write data to the FullScale Register.
*
* @param txData - Data to be written in the FullScale Register.
*
* @return None.
******************************************************************************/
void WriteFullScaleReg(int txData)
{
	char txBuffer[3] = {0x00, 0x00, 0x00};
	char rxBuffer[3] = {0x00, 0x00, 0x00};
	char commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (0 << RnW) |
			    (1 << RS2) |
			    (1 << RS1) |
			    (1 << RS0) |
			    (0 << CREAD);
    
	WriteCommReg(commValue);

	txBuffer[0] = (txData & 0xFF0000) >> 16;
	txBuffer[1] = (txData & 0x00FF00) >> 8;
	txBuffer[2] = (txData & 0x0000FF);

	SPI_TransferData(SPI_BASEADDR, FullScaleRegLen, txBuffer, FullScaleRegLen, rxBuffer, 1);
}

/******************************************************************************
* @brief Read data from the FullScale Register.
*
* @param None.
*
* @return rxData - Read data.
******************************************************************************/
unsigned int ReadFullScaleReg(void)
{
	unsigned int rxData  = 0;
	char 	 txBuffer[3] = {0x00, 0x00, 0x00};
	char     rxBuffer[3] = {0x00, 0x00, 0x00};
	char 	 commValue   = 0x00;
    
	commValue = (0 << WEN) |
			    (1 << RnW) |
			    (1 << RS2) |
			    (1 << RS1) |
			    (1 << RS0) |
			    (0 << CREAD);
    
	WriteCommReg(commValue);

	txBuffer[0] = 0xFF;
	txBuffer[1] = 0xFF;
	txBuffer[2] = 0xFF;

	SPI_TransferData(SPI_BASEADDR, FullScaleRegLen, txBuffer, FullScaleRegLen, rxBuffer, 1);
    
	rxData = ((rxBuffer[0] & 0xFF) << 16) |
			 ((rxBuffer[1] & 0xFF) << 8)  |
			 ((rxBuffer[2] & 0xFF));

	return(rxData);
}

/******************************************************************************
* @brief Display Voltage read by the AD7193.
*
* @param mode - Function mode of AD7193.
*
* @return None.
******************************************************************************/
void AD7193_PrintValue(char mode)
{
	unsigned int rxData        = 0;
	float 		 value	       = 0.0;
	int			 code		   = 0;
	int 		 whole	       = 0;
	int 		 thousands     = 0;
	float 		 valueTemp     = 0.0;
	int 		 wholeTemp     = 0;
	int 		 thousandsTemp = 0;
	int 		 vfs           = 16777216;
	int 		 vfs2          = 8388608;
	int          channel       = 0;


	rxData = ReadDataInfoReg();

	//xil_printf("%x\n\r",rxData);

	channel = rxData & 0x0000000F;

	code = (rxData & 0xFFFFFF00) >> 8;
	valueTemp = (rxData & 0xFFFFFF00) >> 8;

	if((mode == 1)||(mode == 3)||(mode == 5))
	{
		value = code * 2.5 / vfs;
	}
	else if((mode == 2)||(mode == 4))
	{
		value = ((code * 2.5) / vfs2) - 2.5;
	}

	if((mode == 1)||(mode == 2)||(mode == 5))
	{
		switch(channel)
		{
		case 0x00:
			xil_printf("CH1 = ");
			break;
		case 0x01:
			xil_printf("CH2 = ");
			break;
		case 0x02:
			xil_printf("CH3 = ");
			break;
		case 0x03:
			xil_printf("CH4 = ");
			break;
		case 0x04:
			xil_printf("CH5 = ");
			break;
		case 0x05:
			xil_printf("CH6 = ");
			break;
		case 0x06:
			xil_printf("CH7 = ");
			break;
		case 0x07:
			xil_printf("CH8 = ");
			break;
		case 0x08:
			xil_printf("Temperature = ");
			break;
		default:
			xil_printf("Error");
			break;
		}
	}
	else if((mode == 3)||(mode == 4))
	{
		switch(channel)
		{
		case 0x00:
			xil_printf("CH1 = ");
			break;
		case 0x01:
			xil_printf("CH2 = ");
			break;
		case 0x02:
			xil_printf("CH3 = ");
			break;
		case 0x03:
			xil_printf("CH4 = ");
			break;
		default:
			xil_printf("Error");
			break;
		}
	}

	if(channel == 0x08)
	{
		valueTemp = ((valueTemp - 0x800000) / 2815) - 273;
		wholeTemp = (int)valueTemp;
		thousandsTemp = (valueTemp - wholeTemp) * 1000;

		if(thousandsTemp < 10)
		{
			xil_printf("%d.00%dC\n\r", wholeTemp,thousandsTemp);
		}
		else if(thousandsTemp < 100)
		{
			xil_printf("%d.0%0dC\n\r", wholeTemp,thousandsTemp);
		}
		else
		{
			xil_printf("%d.%dC\n\r", wholeTemp,thousandsTemp);
		}
	}
	else
	{
		if ((rxData & 0x40) == 0x00)
		{
		if(value < 0)
		{
			value = value * (-1);
			whole = (int)value;
			thousands = (value - whole) * 1000;
			xil_printf("-");
		}
		else
		{
			whole = (int)value;
			thousands = (value - whole) * 1000;
			xil_printf("+");
		}
		if(thousands < 10)
		{
			xil_printf("%d.00%dV\n\r", whole,thousands);
		}
		else if(thousands < 100)
		{
			xil_printf("%d.0%0dV\n\r", whole,thousands);
		}
		else
		{
			xil_printf("%d.%dV\n\r", whole,thousands);
		}
		}
		else
		{
			xil_printf("out of range\n\r");
		}

	}
}

/******************************************************************************
* @brief Demo Mode 1. Perform conversions referenced to AINCOM, UNIPOLAR mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7193_DemoMode1(void)
{
	int cfgValue  = 0x00;
	int modeValue = 0x00;
	int nrConv    = 0x00;
	int rxTest 	  = 0x00;
	int prevCh 	  = 0xFF;
	int pin;

	xil_printf("\n\r>>> Demo Mode 1 <<<\n\r");
	xil_printf("Settings: Status Register added to Data Register\n\r");
	xil_printf("          Inputs referenced to AINCOM\n\r");
	xil_printf("          All 8 Inputs scanned\n\r");
	xil_printf("          UNIPOLAR MODE\n\r");
	xil_printf("\n\r");


	// Set Mode Register Value
	modeValue = (1 << DAT_STA) |
				(1 << CLK1)    |
				(1 << FS6)     |
				(1 << FS5);
	WriteModeReg(modeValue);

	// Set Configuration Register Value
	cfgValue = (1 << Pseudo) |
			   (1 << CH7) 	 |
			   (1 << CH6) 	 |
			   (1 << CH5) 	 |
			   (1 << CH4) 	 |
			   (1 << CH3) 	 |
			   (1 << CH2) 	 |
			   (1 << CH1) 	 |
			   (1 << CH0) 	 |
			   (1 << BUF) 	 |
			   (1 << UnB);
	WriteConfigReg(cfgValue);

	// Start Conversions in Demo Mode 1
	xil_printf("Demo program will make 4 conversions in this mode...\n\r");
	xil_printf("Conversions remaining: 4. Press any key to start\n\r");
	xil_printf("\n\r");

	// Read pressed key
	UartReadChar();

	nrConv = 0;

	while(nrConv < 4)
	{
		prevCh = 0x00;

		while(prevCh < 8)
		{
			do
			{
				pin = WaitForReadyBit();
			}
			while(pin == 1);

			rxTest = ReadStatusReg();

			if((rxTest & 0x0F) == prevCh)
			{
				AD7193_PrintValue(1);
				prevCh++;
			}
		}

		if(nrConv < 0x03)
		{
			xil_printf("\n\r");
			xil_printf("Conversions remaining: %x. Press any key to continue...\n\r",3 - nrConv);
			xil_printf("\n\r");

			// Read pressed key
			UartReadChar();
		}
		nrConv++;

		if(nrConv == 0x04)
		{
			xil_printf("\n\r");
			xil_printf("Demo Mode 1 Complete. Press any key to continue\n\r");
			xil_printf("\n\r");

			// Read pressed key
			UartReadChar();
		}
	}
}

/******************************************************************************
* @brief Demo Mode 3. Perform differential conversions.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7193_DemoMode3(void)
{
	int cfgValue = 0x00;
	int nrConv   = 0x00;
	int rxTest   = 0x00;
	int prevCh   = 0xFF;
	int pin;

	xil_printf("\n\r>>> Demo Mode 3 <<<\n\r");
	xil_printf("Changing settings...\n\r");
	xil_printf("Settings: Status Register added to Data Register\n\r");
	xil_printf("          Differential Inputs Mode\n\r");
	xil_printf("          IN1-IN2 IN3-IN4 IN5-IN6 IN7-IN8 scanned\n\r");
	xil_printf("          UNIPOLAR MODE\n\r");
	xil_printf("\n\r");

	// Set Configuration Register Value
	cfgValue = (0 << Pseudo) |
			   (0 << CH7) 	 |
			   (0 << CH6) 	 |
			   (0 << CH5) 	 |
			   (0 << CH4) 	 |
			   (1 << CH3) 	 |
			   (1 << CH2) 	 |
			   (1 << CH1) 	 |
			   (1 << CH0) 	 |
			   (1 << BUF) 	 |
			   (1 << UnB);
	WriteConfigReg(cfgValue);

	// Start Conversions in Demo Mode 3
	xil_printf("Demo program will make 4 conversions in this mode...\n\r");
	xil_printf("Conversions remaining: 4. Press any key to start\n\r",3 - nrConv);
	xil_printf("\n\r");

	// Read pressed key
	UartReadChar();

	while(nrConv < 4)
	{
		prevCh = 0x00;

		while(prevCh < 4)
		{
			do
			{
				pin = WaitForReadyBit();
			}
			while(pin == 1);

			rxTest = ReadStatusReg();

			if((rxTest & 0x0F) == prevCh)
			{
				AD7193_PrintValue(3);
				prevCh++;
			}
		}

		if(nrConv < 3)
		{
			xil_printf("\n\r");
			xil_printf("Conversions remaining: %x. Press any key to continue...\n\r",3 - nrConv);
			xil_printf("\n\r");
			// Read pressed key
			UartReadChar();
		}

		nrConv++;

		if(nrConv == 4)
		{
			xil_printf("\n\r");
			xil_printf("Demo Mode 3 Complete. Press any key to continue\n\r");
			xil_printf("\n\r");
			// Read pressed key
			UartReadChar();
		}
	}
}

/******************************************************************************
* @brief Demo Mode 5. Perform temperature read.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7193_DemoMode5(void)
{
	int cfgValue = 0x00;
	//int nrConv = 0x00;
	int pin;

	xil_printf("\n\r>>> Demo Mode 5 <<<\n\r");
	xil_printf("Changing settings...\n\r");
	xil_printf("Settings: Read internal Temperature sensor\n\r");

	// Set Configuration Register Value
	cfgValue = (1 << TEMP)   |
			   (0 << Pseudo) |
			   (0 << CH7) 	 |
			   (0 << CH6) 	 |
			   (0 << CH5) 	 |
			   (0 << CH4) 	 |
			   (0 << CH3) 	 |
			   (0 << CH2) 	 |
			   (0 << CH1) 	 |
			   (0 << CH0) 	 |
			   (0 << BUF) 	 |
			   (0 << UnB);
	WriteConfigReg(cfgValue);

	// Start Conversions in Demo Mode 5
	xil_printf("Demo program will make 1 conversion in this mode...\n\r");
	xil_printf("Press any key to start\n\r");
	xil_printf("\n\r");

	// Read pressed key
	UartReadChar();

	do
	{
		pin = WaitForReadyBit();
	}
	while(pin == 1);

	if((ReadStatusReg() & 0x0F) == 0x08)
	{
		AD7193_PrintValue(5);
	}

	xil_printf("\n\r");
	xil_printf("Demo Mode 5 Complete. Press any key to continue with Demo Mode 1\n\r");
	xil_printf("\n\r");
	// Read pressed key
	UartReadChar();
}

/******************************************************************************
* @brief Demo Mode 2. Perform conversions referenced to AINCOM, BIPOLAR mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7193_DemoMode2(void)
{
	int cfgValue  = 0x00;
	int modeValue = 0x00;
	int nrConv    = 0x00;
	int rxTest 	  = 0x00;
	int prevCh    = 0xFF;
	int pin;

	xil_printf("\n\r>>> Demo Mode 2 <<<\n\r");
	xil_printf("Settings: Status Register added to Data Register\n\r");
	xil_printf("          Inputs referenced to AINCOM\n\r");
	xil_printf("          All 8 Inputs scanned\n\r");
	xil_printf("          BIPOLAR MODE\n\r");
	xil_printf("\n\r");


	// Set Mode Register Value
	modeValue = (1 << DAT_STA) |
				(1 << CLK1)    |
				(1 << FS6)     |
				(1 << FS5);
	WriteModeReg(modeValue);

	// Set Configuration Register Value
	cfgValue = (1 << Pseudo) |
			   (1 << CH7) 	 |
			   (1 << CH6) 	 |
			   (1 << CH5) 	 |
			   (1 << CH4) 	 |
			   (1 << CH3) 	 |
			   (1 << CH2) 	 |
			   (1 << CH1) 	 |
			   (1 << CH0) 	 |
			   (1 << BUF) 	 |
			   (0 << UnB);
	WriteConfigReg(cfgValue);

	// Start Conversions in Demo Mode 2
	xil_printf("Demo program will make 4 conversions in this mode...\n\r");
	xil_printf("Conversions remaining: 4. Press any key to start\n\r");
	xil_printf("\n\r");

	// Read pressed key
	UartReadChar();

	nrConv = 0;

	while(nrConv < 4)
	{
		prevCh = 0x00;

		while(prevCh < 8)
		{
			do
			{
				pin = WaitForReadyBit();
			}
			while(pin == 1);

			rxTest = ReadStatusReg();

			if((rxTest & 0x0F) == prevCh)
			{
				AD7193_PrintValue(2);
				prevCh++;
			}
		}

		if(nrConv < 0x03)
		{
			xil_printf("\n\r");
			xil_printf("Conversions remaining: %x. Press any key to continue...\n\r",3 - nrConv);
			xil_printf("\n\r");

			// Read pressed key
			UartReadChar();
		}
		nrConv++;

		if(nrConv == 0x04)
		{
			xil_printf("\n\r");
			xil_printf("Demo Mode 2 Complete. Press any key to continue\n\r");
			xil_printf("\n\r");

			// Read pressed key
			UartReadChar();
		}
	}
}

/******************************************************************************
* @brief Demo Mode 4. Perform differential conversions.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7193_DemoMode4(void)
{
	int cfgValue = 0x00;
	int nrConv   = 0x00;
	int rxTest   = 0x00;
	int prevCh   = 0xFF;
	int pin;

	xil_printf("\n\r>>> Demo Mode 4 <<<\n\r");
	xil_printf("Changing settings...\n\r");
	xil_printf("Settings: Status Register added to Data Register\n\r");
	xil_printf("          Differential Inputs Mode\n\r");
	xil_printf("          IN1-IN2 IN3-IN4 IN5-IN6 IN7-IN8 scanned\n\r");
	xil_printf("          BIPOLAR MODE\n\r");
	xil_printf("\n\r");

	// Set Configuration Register Value
	cfgValue = (0 << Pseudo) |
			   (0 << CH7) 	 |
			   (0 << CH6) 	 |
			   (0 << CH5) 	 |
			   (0 << CH4) 	 |
			   (1 << CH3) 	 |
			   (1 << CH2) 	 |
			   (1 << CH1) 	 |
			   (1 << CH0) 	 |
			   (1 << BUF) 	 |
			   (0 << UnB);
	WriteConfigReg(cfgValue);

	// Start Conversions in Demo Mode 4
	xil_printf("Demo program will make 4 conversions in this mode...\n\r");
	xil_printf("Conversions remaining: 4. Press any key to start\n\r",3 - nrConv);
	xil_printf("\n\r");

	// Read pressed key
	UartReadChar();

	while(nrConv < 4)
	{
		prevCh = 0x00;

		while(prevCh < 4)
		{
			do
			{
				pin = WaitForReadyBit();
			}
			while(pin == 1);

			rxTest = ReadStatusReg();

			if((rxTest & 0x0F) == prevCh)
			{
				AD7193_PrintValue(4);
				prevCh++;
			}
		}

		if(nrConv < 3)
		{
			xil_printf("\n\r");
			xil_printf("Conversions remaining: %x. Press any key to continue...\n\r",3 - nrConv);
			xil_printf("\n\r");
			// Read pressed key
			UartReadChar();
		}

		nrConv++;

		if(nrConv == 4)
		{
			xil_printf("\n\r");
			xil_printf("Demo Mode 4 Complete. Press any key to continue\n\r");
			xil_printf("\n\r");
			// Read pressed key
			UartReadChar();
		}
	}
}

/******************************************************************************
* @brief Demo Program.
*
* @param None.
*
* @return None.
******************************************************************************/
void DemoProgram(void)
{
	xil_printf("\n\rPmodAD5 Demonstration Program\n\r\n\r");
	xil_printf(">>> Initial Register Values for AD7193 <<<\n\r");
	xil_printf("Status Register: 0x%x\n\r", ReadStatusReg());
	xil_printf("Mode Register: 0x%x\n\r",ReadModeReg());
	xil_printf("Config Register: 0x%x\n\r",ReadConfigReg());
	xil_printf("ID Register: 0x%x\n\r",ReadIDReg());
	xil_printf("GPOCON Register: 0x%x\n\r",ReadGPOCONReg());
	xil_printf("Offset Register: 0x%x\n\r",ReadOffsetReg());
	xil_printf("Full scale Register: 0x%x\n\r",ReadFullScaleReg());
	xil_printf("Press any key to continue...\n\r");

	xil_printf("\n\r");

	// Read pressed key
	UartReadChar();

	while(1)
	{
		AD7193_DemoMode1();
		AD7193_DemoMode2();
		AD7193_DemoMode3();
		AD7193_DemoMode4();
		AD7193_DemoMode5();
	}
}
