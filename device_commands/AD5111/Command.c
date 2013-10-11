/**************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5111.
 *   @author Lucian Sin (Lucian.Sin@analog.com)
*******************************************************************************
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
#include "Console.h"
#include "AD5111.h"
#include "TIME.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
#define MAX_POS			128

const struct cmd_info cmdList[] = {
    [0] = {
        .name = "help?",
        .description = "Displays all available commands.",
        .acceptedValue = "",
        .example = "",
    },
    [1] = {
        .name = "increment=",
        .description = "Increases the resistor value with a number of steps.",
        .acceptedValue = "Accepted values:\r\n\
\t0 .. 128 - number of steps to increment RDAC value.",
        .example = "To increment RDAC value by 10 steps, type: increment=10",
    },
    [2] = {
        .name = "decrement=",
        .description = "Decreases the resistor value with a number of steps.",
        .acceptedValue = "Accepted values:\r\n\
\t0 .. 128 - number of steps to decrement RDAC value.",
        .example = "To decrement RDAC value by 25 steps, type: decrement=25",
    },
    [3] = {
	   .name = "store!",
	   .description = "Device memorizes the current RDAC value in EEPROM.",
	   .acceptedValue = "",
	   .example = "",
	},
    [4] = {
        .name = "shutdown!",
        .description = " Device enters in shutdown mode.",
        .acceptedValue = "",
        .example = "",
    },
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[5] = {GetHelp, DoIncrement, DoDecrement, DoStore,
							   DoShutdown};

/***************************************************************************//**
 * @brief Displays error message.
 *
 * @return None.
*******************************************************************************/
void DisplayError(unsigned char funcNo)
{
    /* Display error messages */
    CONSOLE_Print("Invalid parameter!\r\n");
    CONSOLE_Print("%s - %s %s\r\n", (char*)cmdList[funcNo].name, \
                                    (char*)cmdList[funcNo].description, \
                                    (char*)cmdList[funcNo].acceptedValue);
    CONSOLE_Print("Example: %s\r\n", (char*)cmdList[funcNo].example);
}

/***************************************************************************//**
 * @brief Internal function for displaying all the command with its description.
 *
 * @return None.
*******************************************************************************/
void DisplayCmdList()
{
    unsigned char displayCmd;

    for(displayCmd = 0; displayCmd < cmdNo; displayCmd++)
    {
        CONSOLE_Print("%s - %s\r\n", (char*)cmdList[displayCmd].name, \
                                     (char*)cmdList[displayCmd].description);
    }
}

/***************************************************************************//**
 * @brief Verify if the given parameter is between his valid limits.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void paramLimit(double* param,
                unsigned short lowerLimit,
                unsigned short upperLimit)
{
    if(*param < lowerLimit)
    {
        *param = lowerLimit;
    }
    else
    {
        if(*param > upperLimit)
        {
            *param = upperLimit;
        }
    }
}

/**************************************************************************//***
 * @brief Displays all available commands.
 *
 * @return None.
*******************************************************************************/
void GetHelp(double* param, char paramNo) // "help?" command
{
    unsigned char displayCmd;

    CONSOLE_Print("Available commands:\r\n");
    for(displayCmd = 0; displayCmd < cmdNo; displayCmd++)
    {
        CONSOLE_Print("%s - %s %s\r\n", (char*)cmdList[displayCmd].name,
                                    (char*)cmdList[displayCmd].description,
                                    (char*)cmdList[displayCmd].acceptedValue);
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
    if(AD5111_Init() == 0)
    {
        CONSOLE_Print("AD5111 OK\r\n");
        DisplayCmdList();
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5111 Error\r\n");
        return ERROR;
    }
}

/**************************************************************************//***
 * @brief Executes the step-up of the resistance.
 *
 * @return None.
*******************************************************************************/
void DoIncrement(double* param, char paramNo) // "increment=" command
{
	unsigned char stepNo  = 0;
	unsigned char counter = 0;

	if(paramNo >= 1)
	{
		paramLimit(&param[0], 0, MAX_POS);
		stepNo = (unsigned char)param[0];
		for (counter = 0; counter < stepNo; counter++)
		{
			incValue();
		}
		CONSOLE_Print("Resistor value was increased with %d steps.\r\n", stepNo);
	}
	else
	{
		/* Display error messages */
		DisplayError(1);
	}
}

/**************************************************************************//***
 * @brief Executes the step-down of the resistance.
 *
 * @return None.
*******************************************************************************/
void DoDecrement(double* param, char paramNo) // "decrement=" command
{
	unsigned char stepNo  = 0;
	unsigned char counter = 0;

	if(paramNo >= 1)
	{
		paramLimit(&param[0], 0, MAX_POS);
		stepNo = (unsigned char)param[0];
		for (counter = 0; counter < stepNo; counter++)
		{
			decValue();
		}
		CONSOLE_Print("Resistor value was decreased with %d steps.\r\n", stepNo);
	}
	else
	{
		/* Display error messages */
		DisplayError(2);
	}
}
/**************************************************************************//***
 * @brief Executes a shutdown operation on the device.
 *
 * @return None.
*******************************************************************************/
void DoShutdown(double* param, char paramNo) // "shutdown!" command
{
	shutDown();
	CONSOLE_Print("Device entered in shutdown mode.\r\n");
}

/**************************************************************************//***
 * @brief Executes a write operation to EEPROM of RDAC value.
 *
 * @return None.
*******************************************************************************/
void DoStore(double* param, char paramNo) // "store!" command
{
	saveMem();
	CONSOLE_Print("Device memorized the current RDAC value in EEPROM.\r\n");
}
