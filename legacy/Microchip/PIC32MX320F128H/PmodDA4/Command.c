/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodDA4.
 *   @author DNechita (dan.nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Command.h"
#include "AD5628.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/*!< List of available commands */
const char* cmdList[] = {"help?",
			 "mode=",
			 "mode?",
			 "voltage=",
			 "voltage?",
			 "register=",
			 "register?"
			};
const char* cmdDescription[] = {
	"  -  Displays all available commands.",
	"  -  Selects a mode of operation for selected channel. Accepted values:\r\n\
\tchannel: \r\n\
\t0 .. 7 - Channel A .. H(one channel)\r\n\
\t15 - All channels. \r\n\
\tmode:\r\n\
\t0 - Normal operation. \r\n\
\t1 - 1 KOhm to GND (Power down mode). \r\n\
\t2 - 100 KOhm to GND (Power down mode). \r\n\
\t3 - Three-state (Power down mode).",
	"  -  Displays the selected mode of operation for the selected channel. Accepted\
 values: \r\n\
\t0 .. 7 - Channel A .. H(one channel)\r\n\
\t15 - All channels.",
	"  -  Sets the output voltage for the selected channel. Accepted values: \r\n\
\tchannel: \r\n\
\t0 .. 7 - Channel A .. H(one channel)\r\n\
\t15 - All channels. \r\n\
\toutput voltage:\r\n\
\t0 .. 2500 - desired output voltage in milivolts.",
	"  -  Displays the output voltage of the selected channel. Accepted values: \r\n\
\t0 .. 7 - Channel A .. H(one channel)\r\n\
\t15 - All channels.",
	"  -  Writes the value into the DAC register of the selected channel.\
 Accepted values: \r\n\
\tchannel: \r\n\
\t0 .. 7 - Channel A .. H(one channel)\r\n\
\t15 - All channels. \r\n\
\tregister value:\r\n\
\t0 .. 4095 - the value written to the DAC.",
	"  -  Displays the value written in the DAC register of the selected channel. \
Accepted values: \r\n\
\t0 .. 7 - Channel A .. H(one channel)\r\n\
\t15 - All channels."
};
const char* cmdExample[] = {
	"",
	"To select the power down mode 1KOhm to GND for channel C, type: mode=2 1",
	"To display the mode of operation for channel A, type: mode?0",
	"To set the output voltage to 1250mV for all channels, type: voltage=15 1250",
	"To display the output voltage for channel H, type: voltage?7",
	"To set DAC register value to 2500 for channel D, type: register=3 2500",
	"To display the value of the DAC register for channel G, type: register?6"
};
const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float VREF = 1.25; /*!< Vref = 1.25V */

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[7] = {GetHelp, SetMode, GetMode, SetVoltage,
			       GetVoltage, SetRegister, GetRegister
			      };
/*!< Variables holding information about the device */
unsigned char  powerMode       =
	0; /*!< Last powerMode value written to the device */
unsigned char  currentChannel  = 0; /*!< Last channel selected */
unsigned short registerValue   =
	0; /*!< Last register value written to the device */
unsigned short dacRegArray[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char  powerArray[16]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/***************************************************************************//**
 * @brief Displays all available commands.
 *
 * @return None.
*******************************************************************************/
void GetHelp(double* param, char paramNo) /*!< "help?" command */
{
	char displayCmd;

	CONSOLE_Print("Available commands:\r\n");
	for(displayCmd = 0; displayCmd < cmdNo; displayCmd++) {
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[displayCmd],
			      (char*)cmdDescription[displayCmd]);
	}
}

/***************************************************************************//**
 * @brief Initializes the device.
 *
 * @return - The result of the initialization.
 *              Example: ERROR  - the device was not initialized or the device
 *                              is not present.
 *                       SUCCES - the device was initialized and the device
 *                              is present.
*******************************************************************************/
char DoDeviceInit(void)
{
	if(AD5628_Init() == 0) {
		CONSOLE_Print("AD5628 OK\r\n");
		GetHelp(NULL, 0);
		return SUCCESS;
	} else {
		CONSOLE_Print("AD5628 Error\r\n");
		return ERROR;
	}
}

/***************************************************************************//**
 * @brief Selects a mode of operation for selected channel.
 *
 * @return None.
*******************************************************************************/
void SetMode(double* param, char paramNo) /*!< "mode=" command */
{

	/*!< Check if the parameters are valid */
	if(paramNo >= 2) {
		if(param[0] < 0) {
			param[0] = 0;
		}
		if(param[0] > 7) {
			param[0] = 15;
		}
		currentChannel = (char)param[0];

		if(param[1] < 0) {
			param[1] = 0;
		}
		if(param[1] > 4) {
			param[1] = 4;
		}
		powerMode = (unsigned char)param[1];

		powerArray[currentChannel] = powerMode;
		/*!< Set the power mode of the selected channel. */
		AD5628_PowerMode(powerMode, currentChannel);
		/*!< Send feedback to user */
		CONSOLE_Print("%s%d for ",(char*)cmdList[1],powerMode);
		switch (currentChannel) {
		case 0:
			CONSOLE_Print("channel A\r\n");
			break;
		case 1:
			CONSOLE_Print("channel B\r\n");
			break;
		case 2:
			CONSOLE_Print("channel C\r\n");
			break;
		case 3:
			CONSOLE_Print("channel D\r\n");
			break;
		case 4:
			CONSOLE_Print("channel E\r\n");
			break;
		case 5:
			CONSOLE_Print("channel F\r\n");
			break;
		case 6:
			CONSOLE_Print("channel G\r\n");
			break;
		case 7:
			CONSOLE_Print("channel H\r\n");
			break;
		default:
			CONSOLE_Print("all channels\r\n");
			break;
		}
	} else {
		/*!< Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[1], (char*)cmdDescription[1]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[1]);
	}
}

/***************************************************************************//**
 * @brief  Displays the selected mode of operation for the selected channel.
 *
 * @return None.
*******************************************************************************/
void GetMode(double* param, char paramNo) /*!< "mode?" command */
{
	/*!< Check if the parameter is valid */
	if(paramNo >= 1) {
		if(param[0] < 0) {
			param[0] = 0;
		}
		if(param[0] > 7) {
			param[0] = 15;
		}
		currentChannel = (char)param[0];
		/*!< Send feedback to user */
		CONSOLE_Print("%s%d for ",(char*)cmdList[1],
			      powerArray[currentChannel]);
		switch (currentChannel) {
		case 0:
			CONSOLE_Print("channel A\r\n");
			break;
		case 1:
			CONSOLE_Print("channel B\r\n");
			break;
		case 2:
			CONSOLE_Print("channel C\r\n");
			break;
		case 3:
			CONSOLE_Print("channel D\r\n");
			break;
		case 4:
			CONSOLE_Print("channel E\r\n");
			break;
		case 5:
			CONSOLE_Print("channel F\r\n");
			break;
		case 6:
			CONSOLE_Print("channel G\r\n");
			break;
		case 7:
			CONSOLE_Print("channel H\r\n");
			break;
		default:
			CONSOLE_Print("all channels\r\n");
			break;
		}
	} else {
		/*!< Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[2], (char*)cmdDescription[2]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[2]);
	}
}

/***************************************************************************//**
 * @brief Sets the output voltage of the selected channel.
 *
 * @return None.
*******************************************************************************/
void SetVoltage(double* param, char paramNo) /*!< "voltage=" command */
{
	double outVoltage = 0;
	double tempFloat  = 0;

	/*!< Check if the parameters are valid */
	if(paramNo >= 2) {
		if(param[0] < 0) {
			param[0] = 0;
		}
		if(param[0] > 7) {
			param[0] = 15;
		}
		currentChannel = (char)param[0];

		if(param[1] < 0) {
			param[1] = 0;
		}
		if(param[1] > 2499.999) {
			param[1] = 2499.999;
		}
		outVoltage = param[1] / 1000;

		/*!< Find the binary value corresponding to the output voltage*/
		tempFloat = (outVoltage / (double)(2 * VREF)) * 4096;
		registerValue = (short) tempFloat;
		dacRegArray[currentChannel] = registerValue;
		/*!< Select a channel and write to its register */
		AD5628_SetInputRegister(
			AD5628_CMD(AD5628_CMD_WRITE_INPUT_N_UPDATE_N)|
			AD5628_ADDR(currentChannel)|
			AD5628_DATA_BITS(registerValue));
		outVoltage = 2000 * VREF * ((double)registerValue / 4096);
		/*!< Send feedback to user */
		if (powerArray[currentChannel] > 0) {
			CONSOLE_Print("The selected channel is powered down. Please use \
'mode=%d 0' command in order to power it up.\r\n",currentChannel);
		} else {
			CONSOLE_Print("%s%.1f [mV] for ",(char*)cmdList[3], outVoltage);
			switch (currentChannel) {
			case 0:
				CONSOLE_Print("channel A\r\n");
				break;
			case 1:
				CONSOLE_Print("channel B\r\n");
				break;
			case 2:
				CONSOLE_Print("channel C\r\n");
				break;
			case 3:
				CONSOLE_Print("channel D\r\n");
				break;
			case 4:
				CONSOLE_Print("channel E\r\n");
				break;
			case 5:
				CONSOLE_Print("channel F\r\n");
				break;
			case 6:
				CONSOLE_Print("channel G\r\n");
				break;
			case 7:
				CONSOLE_Print("channel H\r\n");
				break;
			default:
				CONSOLE_Print("all channels\r\n");
				break;
			}
		}
	} else {
		/*!< Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[3], (char*)cmdDescription[3]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[3]);
	}
}

/***************************************************************************//**
 * @brief Displays the output voltage of the selected channel.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
	double outVoltage = 0;

	/*!< Check if the parameter is valid */
	if(paramNo >= 1) {
		if(param[0] < 0) {
			param[0] = 0;
		}
		if(param[0] > 7) {
			param[0] = 15;
		}

		currentChannel = (char)param[0];
		registerValue = dacRegArray[currentChannel];
		/*!< Get the output voltage using data from registerValue */
		outVoltage = 2000 * VREF * ((double)registerValue / 4096);
		/*!< Send the requested value to user */
		if (powerArray[currentChannel] > 0) {
			CONSOLE_Print("The selected channel is powered down. Please use \
'mode=%d 0' command in order to power it up.\r\n",currentChannel);
		} else {
			CONSOLE_Print("%s%.1f [mV] for ",(char*)cmdList[3], outVoltage);
			switch (currentChannel) {
			case 0:
				CONSOLE_Print("channel A\r\n");
				break;
			case 1:
				CONSOLE_Print("channel B\r\n");
				break;
			case 2:
				CONSOLE_Print("channel C\r\n");
				break;
			case 3:
				CONSOLE_Print("channel D\r\n");
				break;
			case 4:
				CONSOLE_Print("channel E\r\n");
				break;
			case 5:
				CONSOLE_Print("channel F\r\n");
				break;
			case 6:
				CONSOLE_Print("channel G\r\n");
				break;
			case 7:
				CONSOLE_Print("channel H\r\n");
				break;
			default:
				CONSOLE_Print("all channels\r\n");
				break;
			}
		}
	} else {
		/*!< Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[4], (char*)cmdDescription[4]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[4]);
	}
}

/***************************************************************************//**
 * @brief Writes the value into the DAC register of the selected channel.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) /*!< "register=" command */
{
	double outVoltage = 0;

	/*!< Check if the parameters are valid */
	if(paramNo >= 2) {
		if(param[0] < 0) {
			param[0] = 0;
		}
		if(param[0] > 7) {
			param[0] = 15;
		}
		currentChannel = (char)param[0];
		if(param[1] < 0) {
			param[1] = 0;
		}
		if(param[1] > 4095) {
			param[1] = 4095;
		}
		registerValue = (unsigned short)param[1];

		dacRegArray[currentChannel] = registerValue;
		/*!< Select a channel and write to its register */
		AD5628_SetInputRegister(
			AD5628_CMD(AD5628_CMD_WRITE_INPUT_N_UPDATE_N)|
			AD5628_ADDR(currentChannel)|
			AD5628_DATA_BITS(registerValue));
		/*!< Send feedback to user */
		if (powerArray[currentChannel] > 0) {
			CONSOLE_Print("The selected channel is powered down. Please use \
'mode=%d 0' command in order to power it up.\r\n",currentChannel);
		} else {
			CONSOLE_Print("%s%d for ",(char*)cmdList[5],registerValue);
			switch (currentChannel) {
			case 0:
				CONSOLE_Print("channel A\r\n");
				break;
			case 1:
				CONSOLE_Print("channel B\r\n");
				break;
			case 2:
				CONSOLE_Print("channel C\r\n");
				break;
			case 3:
				CONSOLE_Print("channel D\r\n");
				break;
			case 4:
				CONSOLE_Print("channel E\r\n");
				break;
			case 5:
				CONSOLE_Print("channel F\r\n");
				break;
			case 6:
				CONSOLE_Print("channel G\r\n");
				break;
			case 7:
				CONSOLE_Print("channel H\r\n");
				break;
			default:
				CONSOLE_Print("all channels\r\n");
				break;
			}
		}
	} else {
		/*!< Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[5], (char*)cmdDescription[5]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[5]);
	}
}

/***************************************************************************//**
 * @brief Displays the value written in the DAC register of the selected channel.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{

	/*!< Check if the parameter is valid */
	if(paramNo >= 1) {
		if(param[0] < 0) {
			param[0] = 0;
		}
		if(param[0] > 7) {
			param[0] = 15;
		}
		currentChannel = (char)param[0];
		registerValue = dacRegArray[currentChannel];
		/*!< Send the requested value to user */
		if (powerArray[currentChannel] > 0) {
			CONSOLE_Print("The selected channel is powered down. Please use \
'mode=%d 0' command in order to power it up.\r\n",currentChannel);
		} else {
			CONSOLE_Print("%s%d for ",(char*)cmdList[5],registerValue);
			switch (currentChannel) {
			case 0:
				CONSOLE_Print("channel A\r\n");
				break;
			case 1:
				CONSOLE_Print("channel B\r\n");
				break;
			case 2:
				CONSOLE_Print("channel C\r\n");
				break;
			case 3:
				CONSOLE_Print("channel D\r\n");
				break;
			case 4:
				CONSOLE_Print("channel E\r\n");
				break;
			case 5:
				CONSOLE_Print("channel F\r\n");
				break;
			case 6:
				CONSOLE_Print("channel G\r\n");
				break;
			case 7:
				CONSOLE_Print("channel H\r\n");
				break;
			default:
				CONSOLE_Print("all channels\r\n");
				break;
			}
		}
	} else {
		/*!< Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[6], (char*)cmdDescription[6]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[6]);
	}
}

