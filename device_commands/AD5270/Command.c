/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for AD527x.
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
#include "ad527x.h"
#include "TIME.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

const struct cmd_info cmdList[] = {
    [0] = {
        .name = "help?",
        .description = "Displays all available commands.",
        .acceptedValue = "",
        .example = "",
    },
    [1] = {
		.name = "reset!",
		.description = "Makes a software reset of the device.",
		.acceptedValue = "",
		.example = "",
    },
    [2] = {
		.name = "rdac=",
		.description = "Writes to the RDAC register.",
		.acceptedValue = "Accepted values:\r\n\
\t0 .. (2^DPotBitsNumber)-1 - the value written to RDAC.",
		.example = "To set the rdac value to 150, type: rdac=150",
    },
    [3] = {
		.name = "rdac?",
		.description = "Displays the last written value in RDAC register.",
		.acceptedValue = "",
		.example = "",
    },
    [4] = {
		.name = "store!",
		.description = "Stores the RDAC setting to 50-TP.",
		.acceptedValue = "",
		.example = "",
    },
    [5] = {
		.name = "50TPValue?",
		.description = "Displays the contents of the selected 50-TP register.",
		.acceptedValue = "Accepted values:\r\n\
\t0 .. 50 - selected 50-TP register",
		.example = "To display the value of the 10th 50-TP register, type:\
50TPValue?10",
    },
    [6] = {
		.name = "50TPAddress?",
		.description = "Displays the address of the last programmed 50-TP \
register.",
		.acceptedValue = "",
		.example = "",
    },
    [7] = {
		.name = "power=",
		.description = "Turns on/off the device.",
		.acceptedValue = "Accepted values:\r\n\
\t1 - normal mode.(default)\r\n\
\t0 - shutdown mode.",
		.example = "To turn off the device, type: power=0",
    },
    [8] ={
		.name = "power?",
		.description = "Displays the power status of the device.",
		.acceptedValue = "",
		.example = "",
    },
};


const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[9] = {GetHelp, DoReset, SetRdac, GetRdac, DoStore,
							  Get50TPValue, Get50TPAddress, SetPower, GetPower};
/* Variables holding information about the device */
unsigned short power     = 1; // Power status of the device
unsigned short max_value = 0; // Maximum value for RDAC
AD527x_type deviceType;


/***************************************************************************//**
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
 * @brief Sets the current device type.
 *
 * @return None.
*******************************************************************************/
void DoDeviceLock(void)
{
    char device = 0;
    char deviceLocked = -1;

    while(deviceLocked < 0)
    {
        CONSOLE_Print("Please specify your device.\r\n\
For AD5270 type: 0\r\n\
For AD5271 type: 1\r\n\
For AD5272 type: 2\r\n\
For AD5274 type: 3\r\n\
");
        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x33))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                    deviceType = ID_AD5270;
                    max_value = 0x3FF;
                    break;
                }
                case 0x31 :
                {
                    deviceType = ID_AD5271;
                    max_value = 0xFF;
                    break;
                }
                case 0x32 :
                {
                    deviceType = ID_AD5272;
                    max_value = 0x3FF;
                    break;
                }
                case 0x33 :
                {
                    deviceType = ID_AD5274;
                    max_value = 0xFF;
                    break;
                }
            }
        }
        else
        {
            CONSOLE_Print("Please type the number corresponding for \
your device!\r\n");
        }
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
    DoDeviceLock();

	if(AD527x_Init(AD527x_I2C_ADDR_0, deviceType) == 0)
    {
        CONSOLE_Print("Device OK\r\n");
        DisplayCmdList();
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("Device Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief  Makes a software reset of the device.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) // "reset!" command
{
	AD527x_SetRegister(AD527x_RESET, 0x0);
	TIME_DelayMs(10);
	/* setup AD527x */
    /* enable 50TP, allow update of RDAC register */
	AD527x_SetRegister(AD527x_WRITE_CONTROL,(AD527x_C1 | AD527x_C0));
	/* Send feedback to user */
	CONSOLE_Print("Device was reset.\r\n");
}

/***************************************************************************//**
 * @brief Writes to the RDAC register.
 *
 * @param param[0] = value to be written to RDAC.
 *
 * @return None.
*******************************************************************************/
void SetRdac(double* param, char paramNo) // "rdac=" command
{
	unsigned short rdac;

	/* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > max_value)
        {
            param[0] = max_value;
        }
        
        rdac = (unsigned short)param[0];
        /* Write to RDAC register */
        AD527x_SetRegister(AD527x_WRITE_RDAC, rdac);
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[2].name,rdac);
    }
     else
    {
    	 /* Display error messages */
    	 DisplayError(2);

    }
}

/***************************************************************************//**
 * @brief Displays the last written value in RDAC register.
 *
 * @return None.
*******************************************************************************/
void GetRdac(double* param, char paramNo) // "rdac?" command
{
	unsigned short rdac;

	rdac = AD527x_GetRegister(AD527x_READ_RDAC, 0x0);
	/* Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[2].name,rdac);
}

/***************************************************************************//**
 * @brief Stores the RDAC setting to 50-TP.
 *
 * @return None.
*******************************************************************************/
void DoStore(double* param, char paramNo) // "store!" command
{
	AD527x_SetRegister(AD527x_STORE_RDAC_50TP, 0x0);
	TIME_DelayMs(400);
	/* Send feedback to user */
	CONSOLE_Print("RDAC register was stored to 50-TP.\r\n");
}

/***************************************************************************//**
 * @brief Displays the contents of the selected 50-TP register.
 *
 * @param param[0] - selected 50-TP register.
 *
 * @return None.
*******************************************************************************/
void Get50TPValue(double* param, char paramNo) // "50TPValue?" command
{
	unsigned short value50TP;
	unsigned short wiper;

	/* Check if the parameter is valid */
	if(paramNo >= 1)
	{
		if(param[0] < 0)
		{
			param[0] = 0;
		}
		if(param[0] > 50)
		{
			param[0] = 50;
		}

		value50TP = (unsigned short)param[0];
		/* Write to RDAC register */
		wiper = AD527x_GetRegister(AD527x_READ_CONTENT_50TP, value50TP);
		/* Send the requested value to user */
		CONSOLE_Print("wiper=%d stored in 50-TPRegister=%d\r\n",wiper,value50TP);
	}
	 else
	{
		 /* Display error messages */
		 DisplayError(5);

	}
}

/***************************************************************************//**
 * @brief Displays the address of the last programmed 50-TP register.
 *
 * @return None.
*******************************************************************************/
void Get50TPAddress(double* param, char paramNo) // "50TPAddress?" command
{
	unsigned short address;

	address = AD527x_GetRegister(AD527x_READ_ADDRESS_50TP, 0x0);
	/* Send the requested value to user */
	CONSOLE_Print("address of the last programmed 50-TPRegister=%d\r\n",
			       address);
}

/***************************************************************************//**
 * @brief Turns on/off the device.
 *
 * @param param[0] - power status of the device.
 * 					 Example: 0 - normal mode;
 * 					          1 - shut down mode.
 *
 * @return None.
*******************************************************************************/
void SetPower(double* param, char paramNo) // "power=" command
{
	/* Check if the parameter is valid */
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
		power = (unsigned short)param[0];
		/* Turns on/off the device. */
		AD527x_SetRegister(AD527x_SHUTDOWN, !power);
		/* Send feedback to user */
		CONSOLE_Print("%s%d\r\n",(char*)cmdList[7].name,power);
	}
	else
	{
   	 /* Display error messages */
   	 DisplayError(7);
	}
}

/***************************************************************************//**
 * @brief Displays the power status of the device.
 *
 * @return None.
*******************************************************************************/
void GetPower(double* param, char paramNo) // "power?" command
{
	/* Send feedback to user */
	CONSOLE_Print("%s%d\r\n",(char*)cmdList[7].name,power);
}
