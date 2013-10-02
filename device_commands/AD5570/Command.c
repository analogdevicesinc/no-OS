/**************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5570.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
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
#include "AD5570.h"
#include "TIME.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
#define MAX_VALUE       0xFFFF

const struct cmd_info cmdList[] = {
[0] = {
    .name = "help?",
    .description = "Displays all available commands.",
    .acceptedValue = "",
    .example = "",
},
[1] = {
    .name = "register=",
    .description = "Loads the DAC input register with a given value.",
    .acceptedValue = "Accepted values:\r\n\
\tvalue:\r\n \
\t0 .. 65535 - value to be written into the register.",
    .example = "To load DAC B input register with 128, type: register=128",
},
[2] = {
    .name = "ldacPin=",
    .description = "Sets the output value of LDAC_n pin.",
    .acceptedValue = "Accepted values:\r\n\
\t0 - sets LDAC_n pin low.(default)\r\n\
\t1 - sets LDAC_n pin high.",
    .example = "To set the LDAC_n pin high, type: ldacPin=1",
},
[3] = {
    .name = "ldacPin?",
    .description = "Displays the value of LDAC_n pin.",
    .acceptedValue = "",
    .example = "",
},
[4] = {
    .name = "clrPin=",
    .description = "Sets the output value of CLR_n pin.",
    .acceptedValue = "Accepted values:\r\n\
\t0 - sets CLR_n pin low.\r\n\
\t1 - sets CLR_n pin high (default).",
.example = "To set the CLR_n pin low, type: clrPin=0",
},
[5] = {
    .name = "clrPin?",
    .description = "Displays the value of CLR_n pin.",
    .acceptedValue = "",
    .example = "",
},
[6] = {
    .name = "pdPin=",
    .description = "Sets the output value of PD_n pin.",
    .acceptedValue = "Accepted values:\r\n\
\t0 - sets PD_n pin low.\r\n\
\t1 - sets PD_n pin high (default).",
    .example = "To set the PD_n pin high, type: pdPin=1",
},
[7] = {
    .name = "pdPin?",
    .description = "Displays the value of PD_n pin.",
    .acceptedValue = "",
    .example = "",
}
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[8] = {GetHelp, SetRegister, SetLdacPin, GetLdacPin, \
                                SetClrPin, GetClrPin, SetPdPin, GetPdPin};

/* Variables holding information about the device */
unsigned char ldac_n = 0;
unsigned char clr_n = 1;
unsigned char pd_n = 1;

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
        CONSOLE_Print("Invalid parameter!\r\n");
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
    if(AD5570_Init() == 0)
    {
        CONSOLE_Print("AD5570 OK\r\n");
        DisplayCmdList();
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5570 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Loads the DAC register with a given value.
 *
 * @param param[0] - value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) // "loadAndUpdate=" command
{
    unsigned short dacValue = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, MAX_VALUE);
        dacValue = (unsigned short)param[0];
        AD5570_SetRegister(dacValue);
        /* Send feedback to user */
        CONSOLE_Print("value=%d\r\n", dacValue);
    }
    else
    {
        /* Display error messages */
        DisplayError(1);
    }
}

/***************************************************************************//**
 * @brief Sets the output value of LDAC_n pin.
 *
 * @param param[0] - value to be set for LDAC_n pin.
 *
 * @return None.
*******************************************************************************/
void SetLdacPin(double* param, char paramNo) // "ldacPin=" command
{
    unsigned char status = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, 1);

        status = (unsigned char) param[0];

        if (status == 0)
        {
            AD5570_LDAC_LOW;
            ldac_n = 0;
        }
        else
        {
            if (status == 1)
            {
                AD5570_LDAC_HIGH;
                ldac_n = 1;
            }
        }
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[2], status);
     }
     else
     {
        /* Display error messages */
        DisplayError(2);
     }
}

/***************************************************************************//**
 * @brief Displays the value of LDAC_n pin.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetLdacPin(double* param, char paramNo) // "ldacPin?" command
{

    /* Send requested value to user */
    CONSOLE_Print("LDAC_n=%d\r\n",ldac_n);
}

/***************************************************************************//**
 * @brief Sets the output value of CLR_n pin.
 *
 * @param param[0] - value to be set for CLR_n pin.
 *
 * @return None.
*******************************************************************************/
void SetClrPin(double* param, char paramNo) // "clrPin=" command
{
    unsigned char status = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, 1);

        status = (unsigned char) param[0];

        if (status == 0)
        {
            AD5570_CLR_LOW;
            clr_n = 0;
        }
        else
        {
            if (status == 1)
            {
                AD5570_CLR_HIGH;
                clr_n = 1;
            }
        }
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[4], status);
     }
     else
     {
        /* Display error messages */
        DisplayError(4);
     }
}

/***************************************************************************//**
 * @brief Displays the value of CLR_n pin.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetClrPin(double* param, char paramNo) // "clrPin?" command
{

    /* Send requested value to user */
    CONSOLE_Print("CLR_n=%d\r\n",clr_n);
}

/***************************************************************************//**
 * @brief Sets the output value of PD_n pin.
 *
 * @param param[0] - value to be set for PD_n pin.
 *
 * @return None.
*******************************************************************************/
void SetPdPin(double* param, char paramNo) // "pdPin=" command
{
    unsigned char status = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, 1);

        status = (unsigned char) param[0];

        if (status == 0)
        {
            AD5570_PD_LOW;
            pd_n = 0;
        }
        else
        {
            if (status == 1)
            {
                AD5570_PD_HIGH;
                pd_n = 1;
            }
        }
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[6], status);
     }
     else
     {
        /* Display error messages */
        DisplayError(6);
     }
}

/***************************************************************************//**
 * @brief Displays the value of PD_n pin.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetPdPin(double* param, char paramNo) // "pdPin?" command
{

    /* Send requested value to user */
    CONSOLE_Print("PD_n=%d\r\n",pd_n);
}
