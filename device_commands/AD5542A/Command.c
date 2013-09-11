/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5542A.
 *   @author Lucian Sin (Lucian.Sin@analog.com)
 *
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
#include "Console.h"
#include "AD5542A.h"
#include "TIME.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/* List of available commands */
const char* cmdList[] ={"help?",
        				"register=",
        				"register?",
        				"voltage=",
        				"voltage?",
                        "ldacPin=",
                        "ldacPin?",
                        "clrPin=",
                        "clrPin?"
						};

const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Writes to the DAC register. Accepted values:\r\n\
\t0 .. 65535 - the value written to the DAC.",
"  -  Displays last written value in the DAC register.",
"  -  Sets the DAC output voltage. Accepted values:\r\n\
\t-2500 .. +2500 - desired output voltage in milivolts.",
"  -  Displays last written voltage value to the DAC.",
"  -  Sets the output value of LDAC pin. Accepted values:\r\n\
\t0 - sets LDAC pin low.(default)\r\n\
\t1 - sets LDAC pin high.",
"  -  Displays the value of LDAC pin.",
"  -  Sets the output value of CLR pin. Accepted values:\r\n\
\t0 - sets CLR pin low.\r\n\
\t1 - sets CLR pin high.(default)",
"  -  Displays the value of CLR pin."
};


const char* cmdExample[] = {
"",
"To set the DAC register to 20000, type: register=20000",
"",
"To set the output voltage to 2350.25 [mV], type: voltage=2350.25",
"",
"To set the LDAC pin high, type: ldacPin=1",
"",
"To set the CLR pin low, type: clrPin=0",
"",
};

const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float VREF  = 2.5; // reference voltage = 2.5V

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[9] = {GetHelp, SetRegister, GetRegister, SetVoltage,
								GetVoltage, SetLdacPin, GetLdacPin, SetClrPin,
								GetClrPin};
                               
/* Variables holding information about the device */
unsigned char  ldac 		 = 0;
unsigned char  clr  		 = 1;
unsigned short registerValue = 0;

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
    if(AD5542A_Init() == 0)
    {
        CONSOLE_Print("AD5542A OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5542A Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Writes to the DAC register.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) /*!< "register=" command */
{

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 65535)
        {
            param[0] = 65535;
        }

        registerValue = (unsigned short)param[0];
        /*!< Write to DAC register */
        AD5542A_SetRegisterValue(registerValue);
        /*!< Send the requested value to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[1],registerValue);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[1], (char*)cmdDescription[1]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[1]);
    }
}

/***************************************************************************//**
 * @brief Displays last written value in the DAC register.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[1],registerValue);
}

/***************************************************************************//**
 * @brief Sets the DAC output voltage.
 *
 * @return None.
*******************************************************************************/
void SetVoltage(double* param, char paramNo) /*!< "voltage=" command */
{
    double outVoltage = 0;
    double tempFloat  = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 2499.999)
        {
            param[0] = 2499.999;
        }

        outVoltage = param[0] / 1000;
        /* Get raw data from the user's desired voltage value. */
        tempFloat = ((outVoltage + VREF) * 32768) / VREF;
		/* Find out register value. */
		registerValue = (unsigned short)tempFloat;
		/* Check to value which will be written to register. */
		if (registerValue > 65535)
		{
			registerValue = 65535;
		}
		 /* Write to DAC register. */
		AD5542A_SetRegisterValue(registerValue);
		/* Calculate the output voltage value. */
		outVoltage = ((VREF * (float)registerValue / 32768) - VREF) * 1000;

        /*!< Send feedback to user */
        CONSOLE_Print("%s%.3f [mV]\r\n",(char*)cmdList[3],outVoltage);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[3], (char*)cmdDescription[3]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[3]);
    }
}

/***************************************************************************//**
 * @brief Displays last written voltage value to the DAC.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    double outVoltage = 0;

    /*!< Get the output voltage using data from registerValue */
    outVoltage = ((VREF * (float)registerValue / 32768) - VREF) * 1000;
    /*!< Send feedback to user */
    CONSOLE_Print("%s%.3f [mV]\r\n",(char*)cmdList[3],outVoltage);
}

/***************************************************************************//**
 * @brief Sets the output value of LDAC pin.
 *
 * @param param[0] - value to be set for LDAC pin.
 *
 * @return None.
*******************************************************************************/
void SetLdacPin(double* param, char paramNo) // "ldacPin=" command
{
	unsigned char status = 0;

	/* Check if the parameter is valid */
	if(paramNo >= 1)
	{
		if(param[0] < 0)
		{
			param[0] = 0;
		}
		else
		{
			if(param[0] > 1)
			{
				param[0] = 1;
			}
		}

		status = (unsigned char) param[0];

		if (status == 0)
		{
			AD5542A_LDAC_LOW;
			ldac = 0;
		}
		else
		{
			if (status == 1)
			{
				AD5542A_LDAC_HIGH;
				ldac = 1;
			}
		}

		/* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[5], status);
	 }
	 else
	 {
		/* Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[5], (char*)cmdDescription[5]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[5]);
	 }

}

/***************************************************************************//**
 * @brief Sets the output value of CLR pin.
 *
 * @param param[0] - value to be set for CLR pin.
 *
 * @return None.
*******************************************************************************/
void SetClrPin(double* param, char paramNo) // "clrPin=" command
{
	unsigned char status = 0;

	/* Check if the parameter is valid */
	if(paramNo >= 1)
	{
		if(param[0] < 0)
		{
			param[0] = 0;
		}
		else
		{	if(param[0] > 1)
			{
				param[0] = 1;
			}
		}

		status = (unsigned char) param[0];

		if (status == 0)
		{
			AD5542A_CLR_LOW;
			clr = 0;
		}
		else
		{
			if (status == 1)
			{
				AD5542A_CLR_HIGH;
				clr = 1;
			}
		}

		/* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[7], status);
	 }
	 else
	 {
		/* Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[7], (char*)cmdDescription[7]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[7]);
	 }

}

/***************************************************************************//**
 * @brief Displays the value of LDAC pin.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetLdacPin(double* param, char paramNo) // "ldacPin?" command
{

    /* Send requested value to user */
    CONSOLE_Print("LDAC=%d\r\n",ldac);
}

/***************************************************************************//**
 * @brief Displays the value of CLR pin.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetClrPin(double* param, char paramNo) // "clrPin?" command
{

    /* Send requested value to user */
    CONSOLE_Print("CLR=%d\r\n",clr);
}
