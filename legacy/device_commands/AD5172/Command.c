/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for ad5172 driver.
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
#include "ad5172.h"
#include "TIME.h"

#define VDD     3.2
#define VDD_VSS VDD/2
#define MAXPOS  255

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
const struct cmd_info cmdList[] = {
    [0] = {
        .name = "help?",
        .description = "Displays all available commands.",
        .acceptedValue = "None",
        .example = "",
    },
    [1] = {
        .name = "rdac=",
        .description = "Load the wiper register with a give value.",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t0 to 3 in function of the device type\r\n\
\tvalue:\r\n\
\t0 .. 255 - wiper register value",
        .example = "To load the wiper register of channel B with 128, \
type: rdac=1 128",
    },
    [2] = {
        .name = "rdac?",
        .description = "Read back the value of the wiper register.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 3 in function of the device type",
        .example = "To read back the value of the wiper register of channel B, \
type: rdac?1",
    },
    [3] = {
        .name = "wbuf1?",
        .description = "Read back the value of the Wiper Buffer in voltage. \
(VDD=3.2V)",
        .acceptedValue = "None",
        .example = "",
    },
    [4] = {
        .name = "shutdown=",
        .description = "Shutdown connects wiper to B terminal and open circuits\
 the A terminal.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 3 in function of the device type",
        .example = "To shutdown (or bring back from shutdown) the RDAC of the channel 0, type: shutdown=0",
    },
    [5] = {
        .name = "shutdown?",
        .description = "Notify about the state of the selected RDAC. (It is in shutdown or not.)",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 3 in function of the device type",
        .example = "To verify the state of RDAC at channel 1, type shutdown?1",
    }
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[6] = {GetHelp, SetRdac, GetRdac, GetWBuf1, SetShutdown,
                               GetShutdown};

AD5172_type_t deviceType;

unsigned char channelNo = 0;
unsigned char channelState[4] = {0,};
unsigned short rdacBuffer[4] = {0,};

/***************************************************************************//**
 * @brief Internal function for displaying error messages.
 *
 * @return None.
*******************************************************************************/
void DisplayError(unsigned char funcNo)
{
    /* Display error messages */
    CONSOLE_Print("Invalid parameter!\r\n");
    CONSOLE_Print("%s - %s %s\r\n", (char*)cmdList[funcNo].name, \
                                    (char*)cmdList[funcNo].description,
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
 *        Note: This function is internal!
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
For AD5162 type: 0\r\n\
For AD5172 type: 1\r\n\
For AD8403 type: 2\r\n");

        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x33))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                    deviceType = ID_AD5162;
                    channelNo = 2;
                    break;
                }
                case 0x31 :
                {
                    deviceType = ID_AD5172;
                    channelNo = 2;
                    break;
                }
                case 0x32 :
                {
                    deviceType = ID_AD8403;
                    channelNo = 4;
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

    if(AD5172_Init(deviceType) == 0)
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
 * @brief Set resistor value.
 *
 * @param - param[0] - The number of the channel, which have to updated
 *        - param[1] - The desired value of the resistor
 *
 * @return - NONE
 *
*******************************************************************************/
void SetRdac(double* param, char paramNo)
{
    unsigned char channel = 0;
    unsigned char data8 = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 2)
    {
        paramLimit(&param[0], 0, channelNo-1);
        paramLimit(&param[1], 0, MAXPOS);
        channel = (unsigned char)param[0];
        data8 = (unsigned char)param[1];
        /* Write to RDAC register */
        AD5172_Write(channel, 0, data8);
        rdacBuffer[channel] = data8;
        /* Send feedback to user */
        CONSOLE_Print("RDAC[%d]=%d\r\n", channel, data8);
    }
    else
    {
        /* Display error messages */
        DisplayError(1);
    }
}

/***************************************************************************//**
 * @brief Get resistor value.
 *
 * @param - param[0] - The number of the channel, which have to updated
 *
 * @return - NONE
 *
*******************************************************************************/
void GetRdac(double* param, char paramNo)
{
    unsigned char channel = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, channelNo-1);
        channel = (unsigned char)param[0];
        /* Print the value of RDAC to UART */
        if(deviceType == ID_AD5172)
        {
            CONSOLE_Print("RDAC[%d]=%d\r\n", channel, \
                                             AD5172_Read(channel, NULL));
        }
        else
        {
            CONSOLE_Print("RDAC[%d]=%d\r\n", channel, \
                                             rdacBuffer[channel]);
        }

    }
    else
    {
        /* Display error messages */
        DisplayError(2);
    }
}

/***************************************************************************//**
 * @brief Get WBUF_1 voltage value.
 *
 * @param - NONE
 *
 * @return - NONE
 *
*******************************************************************************/
void GetWBuf1(double* param, char paramNo)
{
    float wBuf1 = 0;

    wBuf1 = VDD_VSS * ((float)rdacBuffer[0]/256);
    CONSOLE_Print("WBUF1 = %1.2fV\n\r", wBuf1);
}

/***************************************************************************//**
 * @brief Shutdown the RDACs. Shutdown connects wiper to B terminal and open
 *        circuits the A terminal. It does not change the contents of the
 *        wiper register.
 *
 * @param - param[0] - The number of the channel (0.. 3 in function of the
 *                     actual device)
 *
 * @return - NONE
 *
*******************************************************************************/
void SetShutdown(double* param, char paramNo)
{
    unsigned char channel = 0;
    unsigned char command = 0;

    if((deviceType == ID_AD5172) ||
       (deviceType == ID_AD8403))
    {
        /* Check if the parameter is valid */
        if(paramNo >= 1)
        {
            paramLimit(&param[0], 0, channelNo-1);
            channel = (unsigned char)param[0];
            /* Toggle the shutdown bit */
            if(channelState[channel])       // setup the previous state
            {
                command |= AD5172_CNTR_SD;
            }
            command ^= AD5172_CNTR_SD;      // toggle the shutdown bit
            channelState[channel] ^= 0x1;
            if(deviceType == ID_AD5172)
            {
                AD5172_Write(channel, command, 0);
            }
            else //ID_AD8403
            {
                if(channelState[channel])
                {
                    AD5172_SHUTDOWN_LOW;
                }
                else
                {
                    AD5172_SHUTDOWN_HIGH;
                }
            }
            /* Send feedback to user */
            if((command & AD5172_CNTR_SD) != 0)
            {
                CONSOLE_Print("RDACs OFF.\r\n");
            }
            else
            {
                CONSOLE_Print("RDACs ON.\r\n");
            }
        }
        else
        {
            /* Display error messages */
            DisplayError(4);
        }
    }
    else
    {
        CONSOLE_Print("This function is not supported by the current \
device.\r\n");
    }
}

/***************************************************************************//**
 * @brief Notify the user from the state of an RDAC
 *
 * @param - NONE
 *
 * @return - NONE
 *
*******************************************************************************/
void GetShutdown(double* param, char paramNo)
{
    unsigned char channel = 0;

    if((deviceType == ID_AD5172) ||
       (deviceType == ID_AD8403))
    {
        /* Check if the parameter is valid */
        if(paramNo >= 1)
        {
            paramLimit(&param[0], 0, channelNo-1);
            channel = (unsigned char)param[0];
            /* Send feedback to user */
            if(channelState[channel] != 0)
            {
                CONSOLE_Print("Shutdown pin is in active state.\r\n");
            }
            else
            {
                CONSOLE_Print("Shutdown pin is in inactive state.\r\n");
            }
        }
        else
        {
            /* Display error messages */
            DisplayError(4);
        }
    }
    else
    {
        CONSOLE_Print("This function is not supported by the current \
device.\r\n");
    }
}
