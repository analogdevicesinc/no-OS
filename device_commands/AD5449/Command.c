/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5449.
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
#include "AD5449.h"
#include "TIME.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/* List of available commands */
const char* cmdList[] ={"help?",
        				"load=",
                        "loadBoth=",
                        "loadAndUpdate=",
                        "updateBoth!",
                        "readback?",
                        "clearToZero!",
                        "clearToMid!",
                        "ldacPin=",
                        "ldacPin?",
                        "clrPin=",
                        "clrPin?"
};

const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Loads selected DAC input register with a given value. Accepted values:\r\n\
\tchannel:\r\n\
\t0 - select DAC A input register.\r\n\
\t1 - select DAC B input register.\r\n\
\tvalue:\r\n\
\t0 .. 4095 - value to be written in register.",
"  -  Loads both DAC input registers. Accepted values:\r\n\
\t0 .. 4095 - value to be written in register.",
"  -  Loads and updates the selected DAC with a given value. Accepted values:\r\n\
\tchannel:\r\n\
\t0 - select DAC A.\r\n\
\t1 - select DAC B.\r\n\
\tvalue:\r\n\
\t0 .. 4095 - value to be written in register.",
"  -  Updates both DAC outputs.",
"  -  Reads from the selected DAC register. Accepted values:\r\n\
\tchannel:\r\n\
\t0 - read from DAC A.\r\n\
\t1 - read from DAC B.",
"  -  Clears both DAC outputs to zero scale.",
"  -  Clears both DAC outputs to midscale.",
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
"To load DAC B input register with 500, type: load=1 500",
"To load both DAC input registers with 3525, type: loadBoth=3525",
"To load and update DAC A with 2000, type: loadAndUpdate=0 2000",
"",
"To read from the DAC A register, type: readback?0",
"",
"",
"To set the LDAC pin high, type: ldacPin=1",
"",
"To set the CLR pin low, type: clrPin=0",
""};

const char cmdNo = (sizeof(cmdList) / sizeof(const char*));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[12] = {GetHelp, SetLoad, SetLoadBoth, SetLoadAndUpdate,
								DoUpdateBoth, GetReadback, DoClearToZero,
								DoClearToMid, SetLdacPin, GetLdacPin, SetClrPin,
								GetClrPin};

/* Variables holding information about the device */
unsigned char ldac = 0;
unsigned char clr  = 1;

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
    if(AD5449_Init() == 0)
    {
        CONSOLE_Print("AD5449 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5449 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Loads and updates the selected DAC with a given value.
 *
 * @param param[0] - selected DAC.
 * 		  param[1] - value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetLoadAndUpdate(double* param, char paramNo) // "loadAndUpdate=" command
{

	unsigned char  channel  = 0;
	unsigned short dacValue = 0;

	/* Check if the parameters are valid */
    if(paramNo >= 2)
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

        if(param[1] < 0)
        {
        	param[1] = 0;
        }
        else
        {
        	if(param[1] > 4095)
        	{
        		param[1] = 4095;
        	}
        }

        channel = (unsigned char)param[0];
        dacValue = (unsigned short)param[1];
        AD5449_LoadUpdate(channel, dacValue);
        /* Send feedback to user */
        CONSOLE_Print("channel=%d; value=%d\r\n",channel, dacValue);
    }
    else
    {
        /* Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[3], (char*)cmdDescription[3]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[3]);
    }
}


/***************************************************************************//**
 * @brief Loads selected DAC input register with a given value.
 *
 * @param param[0] - selected DAC.
 * 		  param[1] - value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetLoad(double* param, char paramNo) // "load=" command
{

	unsigned char  channel  = 0;
	unsigned short dacValue = 0;

	/* Check if the parameters are valid */
    if(paramNo >= 2)
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

        if(param[1] < 0)
        {
        	param[1] = 0;
        }
        else
        {
        	if(param[1] > 4095)
        	{
        		param[1] = 4095;
        	}
        }

        channel = (unsigned char)param[0];
        dacValue = (unsigned short)param[1];
        AD5449_Load(channel, dacValue);
        /* Send feedback to user */
        CONSOLE_Print("channel=%d; value=%d\r\n",channel, dacValue);
    }
    else
    {
        /* Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[1], (char*)cmdDescription[1]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[1]);
    }
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
			AD5449_LDAC_LOW;
			ldac = 0;
		}
		else
		{
			if (status == 1)
			{
				AD5449_LDAC_HIGH;
				ldac = 1;
			}
		}

		/* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[8], status);
	 }
	 else
	 {
		/* Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[8], (char*)cmdDescription[8]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[8]);
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
			AD5449_CLR_LOW;
			clr = 0;
		}
		else
		{
			if (status == 1)
			{
				AD5449_CLR_HIGH;
				clr = 1;
			}
		}

		/* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[10], status);
	 }
	 else
	 {
		/* Display error messages */
		CONSOLE_Print("Invalid parameter!\r\n");
		CONSOLE_Print("%s%s\r\n", (char*)cmdList[10], (char*)cmdDescription[10]);
		CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[10]);
	 }

}

/***************************************************************************//**
 * @brief Reads from the selected DAC register.
 *
 * @param param[0] - selected DAC.
 *
 * @return None.
*******************************************************************************/
void GetReadback(double* param, char paramNo) // "readback?" command
{
	unsigned char channel = 0;
	unsigned short dacValue = 0;

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
		channel = (unsigned char)param[0];
		dacValue = AD5449_Readback(channel);
		/* Send feedback to user */
        CONSOLE_Print("register=%d for channel %d\r\n", dacValue, channel);
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
 * @brief Loads both DAC input registers with a given value.
 *
 * @param param[0] - value to be written in both registers.
 *
 * @return None.
*******************************************************************************/
void SetLoadBoth(double* param, char paramNo) // "loadBoth=" command
{
	unsigned short dacValue = 0;

	/* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
        	if(param[0] > 4095)
           	{
           		param[0] = 4095;
        	}
        }

        dacValue = (unsigned short)param[0];
        AD5449_LoadBoth(dacValue);
        /* Send feedback to user */
        CONSOLE_Print("value=%d\r\n", dacValue);
    }
    else
    {
        /* Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[2], (char*)cmdDescription[2]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[2]);
    }
}


/***************************************************************************//**
 * @brief Updates both DAC outputs.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void DoUpdateBoth(double* param, char paramNo) // "updateBoth!" command
{
	AD5449_UpdateBoth();
    /* Send feedback to user */
    CONSOLE_Print("Both DAC outputs were updated.\r\n");
}

/***************************************************************************//**
 * @brief Clears both DAC outputs to zero scale.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void DoClearToZero(double* param, char paramNo) // "clearToZero!" command
{
	AD5449_Clear(ZERO_SCALE);
    /* Send feedback to user */
    CONSOLE_Print("Both DAC outputs were cleared to zero scale.\r\n");
}

/***************************************************************************//**
 * @brief Clears both DAC outputs to midscale.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void DoClearToMid(double* param, char paramNo) // "clearToMid!" command
{
	AD5449_Clear(MIDSCALE);
    /* Send feedback to user */
    CONSOLE_Print("Both DAC outputs were cleared to midscale.\r\n");
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

