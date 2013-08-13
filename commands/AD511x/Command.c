/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for AD5110.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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
******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Command.h"
#include "Console.h"
#include "AD511x.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

/*!< List of available commands */
const char* cmdList[] = {"help?",
						 "reset!",
                         "rdac=",
                         "rdac?",
                         "rdacToEeprom!",
                         "wiper?",
                         "tolerance?",
						 "power=",
						 "power?"};
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Makes a software reset of the device.",
"  -  Writes to the RDAC register. Accepted values:\r\n\
\t0 .. 127 - the value written to RDAC.",
"  -  Displays the last written value in RDAC register.",
"  -  Writes the content of RDAC register to EEPROM.",
"  -  Displays the wiper resistance from EEPROM.",
"  -  Displays the resistance tolerance from EEPROM.",
"  -  Turns on/off the device. Accepted values:\r\n\
\t0 - turns off the device.\r\n\
\t1 - turns on the device.",
"  -  Displays the power status of the device."};

const char* cmdExample[] = {
"",
"",
"To set the rdac value to 100, type: rdac=100",
"",
"",
"",
"",
"To turn off the device, type power=0",
""};

const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[9] = {GetHelp, DoReset, SetRdac, GetRdac,
							   DoRdacToEeprom, GetWiper, GetTolerance, SetPower,
							   GetPower};
/*!< Variables holding information about the device */
unsigned char power = 1; /*!< Power status of the device */

/***************************************************************************//**
 * @brief Displays all available commands.
 *
 * @return None.
*******************************************************************************/
void GetHelp(double* param, char paramNo) /*!< "help?" command */
{
    unsigned char displayCmd;

    CONSOLE_Print("Available commands:\r\n");
    for(displayCmd = 0; displayCmd < cmdNo; displayCmd++)
    {
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
    if(AD511x_Init(AD511x_I2C_ADDR) == 0)
    {
        CONSOLE_Print("AD5110 OK\r\n");
        GetHelp(NULL, 0);

        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5110 Error\r\n");

        return ERROR;
    }
}

/***************************************************************************//**
 * @brief  Makes a software reset of the device.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) /*!< "reset!" command */
{
	AD511x_Reset();
	/*!< Send feedback to user */
	CONSOLE_Print("Device was reset.\r\n");
}

/***************************************************************************//**
 * @brief Writes to the RDAC register.
 *
 * @param param[0] = value to be written to RDAC.
 *
 * @return None.
*******************************************************************************/
void SetRdac(double* param, char paramNo) /*!< "rdac=" command */
{
    unsigned char rdac;

	/*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 127)
        {
            param[0] = 127;
        }
        
        rdac = (unsigned char)param[0];
        /*!< Write to RDAC register */
        AD511x_WriteRdac(rdac);
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[2],rdac);
    }
     else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[2], (char*)cmdDescription[2]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[2]);
    }
}

/***************************************************************************//**
 * @brief Displays the last written value in RDAC register.
 *
 * @return None.
*******************************************************************************/
void GetRdac(double* param, char paramNo) /*!< "rdac?" command */
{
	unsigned char rdac;

	rdac = AD511x_ReadRdac();
	/*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[2],rdac);
}

/***************************************************************************//**
 * @brief Writes the content of RDAC register to EEPROM.
 *
 * @return None.
*******************************************************************************/
void DoRdacToEeprom(double* param, char paramNo) /*!< "rdacToEeprom!" command*/
{
	AD511x_WriteRdacEeprom();
	/*!< Send feedback to user */
	CONSOLE_Print("RDAC register was written to EEPROM.\r\n");
}

/***************************************************************************//**
 * @brief Displays the wiper resistance from EEPROM.
 *
 * @return None.
*******************************************************************************/
void GetWiper(double* param, char paramNo) /*!< "wiper?" command*/
{
	unsigned char wiper;

	wiper = AD511x_ReadWiper();
	/*!< Send the requested value to user */
	CONSOLE_Print("wiper=%d\r\n",wiper);
}

/***************************************************************************//**
 * @brief Displays the resistance tolerance from EEPROM.
 *
 * @return None.
*******************************************************************************/
void GetTolerance(double* param, char paramNo) /*!< "tolerance?" command */
{
	unsigned char tolerance;

	tolerance = AD511x_ReadResTolerance();
	/*!< Send the requested value to user */
	CONSOLE_Print("tolerance=%d\r\n",tolerance);
}

/***************************************************************************//**
 * @brief Turns on/off the device.
 *
 * @param param[0] - power status of the device.
 * 					 Example: 0 - device turned off;
 * 					          1 - device turned on.
 *
 * @return None.
*******************************************************************************/
void SetPower(double* param, char paramNo) /*!< "power=" command */
{
	/*!< Check if the parameter is valid */
	if(paramNo >= 1)
	{
		if(param[0] < 0)
		{
			param[0] = 0;
		}
		if(param[0] > 1)
		{
			param[0] = 1;
		}
		power = (unsigned char)param[0];
		/*!< Turns on/off the device. */
		AD511x_ShutDown(!power);
		/*!< Send feedback to user */
		CONSOLE_Print("%s%d\r\n",(char*)cmdList[7],power);
	}
	else
	{
		/*!< Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[7], (char*)cmdDescription[7]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[7]);
	}
}

/***************************************************************************//**
 * @brief Displays the power status of the device.
 *
 * @return None.
*******************************************************************************/
void GetPower(double* param, char paramNo) /*!< "power?" command */
{
	/*!< Send feedback to user */
	CONSOLE_Print("%s%d\r\n",(char*)cmdList[7],power);
}
