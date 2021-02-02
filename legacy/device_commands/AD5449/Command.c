/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5449.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
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

const struct cmd_info cmdList[] = {
    [0] = {
        .name = "help?",
        .description = "Displays all available commands.",
        .acceptedValue = "",
        .example = "",
    },
    [1] = {
        .name = "load=",
        .description = "Loads selected DAC input register with a given value.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 - select DAC A input register.\r\n\
\t1 - select DAC B input register.\r\n\
\tvalue:\r\n\
\t0 .. (2^DAC_Resolution)-1 - value to be written in register.",
        .example = "To load DAC B input register with 500, type: load=1 500",
    },
    [2] = {
        .name = "loadAll=",
        .description = "Loads both DAC input registers.",
        .acceptedValue = "Accepted values:\r\n\
\t0 .. (2^DAC_Resolution)-1 - value to be written in register.",
        .example = "To load both DAC input registers with 3525, \
type: loadBoth=3525",
    },
    [3] = {
        .name = "loadAndUpdate=",
        .description = "Loads and updates the selected DAC with a given value.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 - select DAC A.\r\n\
\t1 - select DAC B.\r\n\
\tvalue:\r\n\
\t0 .. (2^DAC_Resolution)-1 - value to be written in register.",
        .example = "To load and update DAC A with 2000, \
type: loadAndUpdate=0 2000",
    },
    [4] = {
        .name = "updateAll!",
        .description = "Updates both DAC outputs.",
        .acceptedValue = "",
        .example = "",
    },
    [5] = {
        .name = "readback?",
        .description = "Reads from the selected DAC register.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 - read from DAC A.\r\n\
\t1 - read from DAC B.",
        .example = "To read from the DAC A register, type: readback?0",
    },
    [6] = {
        .name = "clearToZero!",
        .description = "Clears both DAC outputs to zero scale.",
        .acceptedValue = "",
        .example = "",
    },
    [7] = {
        .name = "clearToMid!",
        .description = "Clears both DAC outputs to midscale.",
        .acceptedValue = "",
        .example = "",
    },
    [8] = {
        .name = "ldacPin=",
        .description = "Sets the output value of LDAC pin.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - sets LDAC pin low.(default)\r\n\
\t1 - sets LDAC pin high.",
        .example = "To set the LDAC pin high, type: ldacPin=1",
    },
    [9] = {
        .name = "ldacPin?",
        .description = "Displays the value of LDAC pin.",
        .acceptedValue = "",
        .example = "",
    },
    [10] = {
        .name = "clrPin=",
        .description = "Sets the output value of CLR pin.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - sets CLR pin low.\r\n\
\t1 - sets CLR pin high.(default)",
        .example = "To set the CLR pin low, type: clrPin=0",
    },
    [11] = {
        .name = "clrPin?",
        .description = "Displays the value of CLR pin.",
        .acceptedValue = "",
        .example = "",
    }
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[12] = {GetHelp, SetLoad, SetLoadAll, SetLoadAndUpdate,
                                SetUpdateAll, GetReadback, SetClearToZero,
                                SetClearToMid, SetLdacPin, GetLdacPin,
                                SetClrPin, GetClrPin};

/* Variables holding information about the device */
AD5449_type_t deviceType;
unsigned char ldac = 0;
unsigned char clr  = 1;
unsigned short max_value = 0;

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
 * @brief Set the current device type.
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
For AD5415 type 0\r\n\
For AD5426 type 1\r\n\
For AD5429 type 2\r\n\
For AD5432 type 3\r\n\
For AD5439 type 4\r\n\
For AD5443 type 5\r\n\
For AD5449 type 6\r\n");
        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x36))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                    deviceType = ID_AD5415;
                    max_value = 0xFFF;
                    break;
                }
                case 0x31 :
                {
                    deviceType = ID_AD5426;
                    max_value = 0xFF;
                    break;
                }
                case 0x32 :
                {
                    deviceType = ID_AD5429;
                    max_value = 0xFF;
                    break;
                }
                case 0x33 :
                {
                    deviceType = ID_AD5432;
                    max_value = 0x3FF;
                    break;
                }
                case 0x34 :
                {
                    deviceType = ID_AD5439;
                    max_value = 0x3FF;
                    break;
                }
                case 0x35 :
                {
                    deviceType = ID_AD5443;
                    max_value = 0xFFF;
                    break;
                }
                case 0x36 :
                {
                    deviceType = ID_AD5449;
                    max_value = 0xFFF;
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

    if(AD5449_Init(deviceType) == 0)
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
 * @brief Loads and updates the selected DAC with a given value.
 *
 * @param param[0] - selected DAC.
 *        param[1] - value to be written in register.
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
        paramLimit(&param[0], 0, 1);
        paramLimit(&param[1], 0, max_value);

        channel = (unsigned char)param[0];
        dacValue = (unsigned short)param[1];

        AD5449_LoadUpdateChannel(channel, dacValue);
        /* Send feedback to user */
        CONSOLE_Print("channel=%d; value=%d\r\n",channel, dacValue);
    }
    else
    {
        /* Display error messages */
        DisplayError(3);
    }
}


/***************************************************************************//**
 * @brief Loads selected DAC input register with a given value.
 *
 * @param param[0] - selected DAC.
 *        param[1] - value to be written in register.
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
        paramLimit(&param[0], 0, 1);
        paramLimit(&param[1], 0, max_value);

        channel = (unsigned char)param[0];
        dacValue = (unsigned short)param[1];

        AD5449_LoadChannel(channel, dacValue);
        /* Send feedback to user */
        CONSOLE_Print("channel=%d; value=%d\r\n",channel, dacValue);
    }
    else
    {
        /* Display error messages */
        DisplayError(1);
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
        paramLimit(&param[0], 0, 1);

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
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[8].name, status);
     }
     else
     {
        /* Display error messages */
        DisplayError(8);
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
        paramLimit(&param[0], 0, 1);

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
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[10].name, status);
     }
     else
     {
        /* Display error messages */
        DisplayError(10);
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
        paramLimit(&param[0], 0, 1);
        channel = (unsigned char)param[0];
        dacValue = AD5449_ReadbackChannel(channel);
        /* Send feedback to user */
        CONSOLE_Print("register=%d for channel %d\r\n", dacValue, channel);
    }
    else
    {
        /* Display error messages */
        DisplayError(5);
    }
}

/***************************************************************************//**
 * @brief Loads both DAC input registers with a given value.
 *
 * @param param[0] - value to be written in both registers.
 *
 * @return None.
*******************************************************************************/
void SetLoadAll(double* param, char paramNo) // "loadBoth=" command
{
    unsigned short dacValue = 0;

    if((deviceType == ID_AD5426) |
       (deviceType == ID_AD5432) |
       (deviceType == ID_AD5443) )
    {
        CONSOLE_Print("This function is not supported \
                            by the current device!\n\r");
    }
    else
    {
        /* Check if the parameter is valid */
        if(paramNo >= 1)
        {
            paramLimit(&param[0], 0, max_value);
            dacValue = (unsigned short)param[0];
            AD5449_LoadAll(dacValue);
            /* Send feedback to user */
            CONSOLE_Print("value=%d\r\n", dacValue);
        }
        else
        {
            /* Display error messages */
            DisplayError(2);
        }
    }
}

/***************************************************************************//**
 * @brief Updates both DAC outputs.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void SetUpdateAll(double* param, char paramNo) // "updateBoth!" command
{
    if((deviceType == ID_AD5426) |
       (deviceType == ID_AD5432) |
       (deviceType == ID_AD5443) )
    {
        CONSOLE_Print("This function is not supported \
                            by the current device!\n\r");
    }
    else
    {
        AD5449_UpdateAll();
        /* Send feedback to user */
        CONSOLE_Print("Both DAC outputs were updated.\r\n");
    }
}

/***************************************************************************//**
 * @brief Clears both DAC outputs to zero scale.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void SetClearToZero(double* param, char paramNo) // "clearToZero!" command
{
    AD5449_ClearScaleSetup(AD5449_ZERO_SCALE);
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
void SetClearToMid(double* param, char paramNo) // "clearToMid!" command
{
    AD5449_ClearScaleSetup(AD5449_MID_SCALE);
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
