/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for AD2S1205.
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
#include "ad2s1205.h"

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
        .name = "position?",
        .description = "Displays the current angular position.",
        .acceptedValue = "",
        .example = "",
    },
    [2] = {
        .name = "velocity?",
        .description = "Displays the current angular velocity.",
        .acceptedValue = "",
        .example = "",
    },
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));
const float TC  = 0.001f;

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp, GetPosition, GetVelocity};

/* Variables holding information about the device */
unsigned short position     = 0;
unsigned short maxPosition  = 0;
unsigned short minPosition  = 0;
float          meanPosition = 0;
short          velocity     = 0;
short          maxVelocity  = 0;
short          minVelocity  = 0;
float          meanVelocity = 0;


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
    if(AD2S1205_Init() == 0)
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
 * @brief Displays the current angular position.
 *
 * @return None.
*******************************************************************************/
void GetPosition(double* param, char paramNo) // "position?" command
{
    u32 count;

    AD2S1205_Pos_Vel(&position, &velocity);
    maxPosition = position;
    minPosition = position;

    for(count = 0; count < 2000; count++)
    {
        AD2S1205_Pos_Vel(&position, &velocity);
        /* Compute the min, max and average position values */
        if(position > maxPosition)
        {
            maxPosition = position;
        }
        if(position < minPosition)
        {
            minPosition = position;
        }
        meanPosition = meanPosition * (1-TC) + (float)position * TC;
    }
    /* Send the requested value to user */
    CONSOLE_Print("Position = %d\r\n", position);
    CONSOLE_Print("Min Position = %d\r\n", minPosition);
    CONSOLE_Print("Max Position = %d\r\n", maxPosition);
    CONSOLE_Print("Mean Position = %.3f\r\n", meanPosition);
}

/***************************************************************************//**
 * @brief Displays the current angular velocity.
 *
 * @return None.
*******************************************************************************/
void GetVelocity(double* param, char paramNo) // "velocity?" command
{
    u32 count;

    AD2S1205_Pos_Vel(&position, &velocity);
    maxVelocity = velocity;
    minVelocity = velocity;

    for(count = 0; count < 2000; count++)
    {
        AD2S1205_Pos_Vel(&position, &velocity);
        /* Compute the min, max and average velocity values */
        if(velocity > maxVelocity)
        {
            maxVelocity = velocity;
        }
        if(velocity < minVelocity)
        {
            minVelocity = velocity;
        }
        meanVelocity = meanVelocity * (1-TC) + (float)velocity * TC;
    }
    /* Send the requested value to user */
    CONSOLE_Print("Velocity = %d\r\n", velocity);
    CONSOLE_Print("Min Velocity = %d\r\n", minVelocity);
    CONSOLE_Print("Max Velocity = %d\r\n", maxVelocity);
    CONSOLE_Print("Mean Velocity = %.3f\r\n", meanVelocity);
}
