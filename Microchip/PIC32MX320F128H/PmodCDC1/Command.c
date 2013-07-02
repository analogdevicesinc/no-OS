/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodCDC1.
 *   @author Dan Nechita (Dan.Nechita@analog.com)
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
#include "AD7156.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/*!< List of available commands */
const char* cmdList[] = {"help?",
                         "channelRange?",
                         "channelRange=",
                         "powerMode?",
                         "powerMode=",
                         "channelEnable=",
                         "samples?",
                         "buttons?",
                         "reset!"};

/*!< List of commands' descriptions */
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Displays the set range for the selected channel. Accepted values: \r\n\
\t1 - Displays the channel 1's range. \r\n\
\t2 - Displays the channel 2's range.",
"  -  Selects the range of the selected channel. Accepted values: \r\n\
\tchannel:\r\n\
\t1 - Select channel 1. \r\n\
\t2 - Select channel 2. \r\n\
\trange:\r\n\
\t0 - 0 .. 2 pF. \r\n\
\t1 - 0 .. 0.5 pF. \r\n\
\t2 - 0 .. 1 pF. \r\n\
\t3 - 0 .. 4 pF.",
"  -  Displays the current power mode.",
"  -  Selects the power mode of the device. Accepted values: \r\n\
\t0 - Idle. \r\n\
\t1 - Continuous conversions. \r\n\
\t2 - Single conversion. \r\n\
\t3 - Power-down.",
"  -  Enables or disabled conversions on selected channel. Accepted values: \r\n\
\tchannel:\r\n\
\t1 - Select channel 1. \r\n\
\t2 - Select channel 2. \r\n\
\tenable:\r\n\
\t0 - Disable conversions on selected channel. \r\n\
\t1 - Enable conversions on selected channel.",
"  -  Reads 50 samples from the enabled channels and displays the data in pF.",
"  -  Checks if any of the two buttons are pressed or released.",
"  -  Resets the device."};


const char* cmdExample[] = {"",
                            "To get the channel 1's range, type: channelRange?1",
                            "To set the channel 1's range to 2, type: channelRange=1 2",
                            "",
                            "To set the power mode to idle, type: powerMode=0",
                            "To enable channel2, type: channelEnable=2 1",
                            ""};

const char cmdNo = (sizeof(cmdList) / sizeof(const char*));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[9] = {GetHelp, GetRange, SetRange, GetPowerMode,\
                               SetPowerMode, EnableCh, GetSamples, \
                               GetButtonsStatus, DoReset};
unsigned char powerMode   = 1;
unsigned char chRange[2]  = {0, 0};
unsigned char chEnable[2] = {1, 1};

/******************************************************************************/
/************************ Functions Description *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Displays all available commands.
 *
 * @return None.
*******************************************************************************/
void GetHelp(double* param, char paramNo) /*!< "help?" command */
{
    char displayCmd;

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
    if(AD7156_Init() == 0)
    {
        CONSOLE_Print("AD7156 OK\r\n");
        GetHelp(NULL, 0);

        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD7156 Error\r\n");

        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Displays the set range for the selected channel.
 *
 * @param param[0] - selected channel;
 *        paramNo  - number of parameters (must be >=1); only the first parameter is taken into account;
 *
 * @return None.
*******************************************************************************/
void GetRange(double* param, char paramNo) /*!< "channelrange?" command */
{
    if(paramNo >= 1)
    {
        if(param[0] < 1)
        {
            param[0] = 1;
        }
        if(param[0] > 2)
        {
            param[0] = 2;
        }
        CONSOLE_Print("Channel %d range: %d\r\n",
                      (char)param[0],
                      chRange[(char)param[0]-1]);
    }
    else
    {
        /*!< Display error messages. */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[1], (char*)cmdDescription[1]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[1]);
    }
}

/***************************************************************************//**
 * @brief Selects the range of the selected channel.
 *
 * @param param[0] - selected channel;
 *        param[1] - selected range;
 *        paramNo  - number of parameters (must be >=2); only the first 2 parameters are taken into account;
 *
 * @return None.
*******************************************************************************/
void SetRange(double* param, char paramNo) /*!< "channelrange=" command */
{
    if(paramNo >= 2)
    {
        if(param[0] < 1)
        {
            param[0] = 1;
        }
        if(param[0] > 2)
        {
            param[0] = 2;
        }
        if(param[1] < 0)
        {
            param[1] = 0;
        }
        if(param[1] > 3)
        {
            param[1] = 3;
        }

        chRange[(char)param[0]-1] = param[1];
        /*!< Send feedback to user. */
        CONSOLE_Print("Channel %d range: %d\r\n",
                      (char)param[0],
                      chRange[(char)param[0]-1]);
    }
    else
    {
        /*!< Display error messages. */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[2], (char*)cmdDescription[2]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[2]);
    }
}

/***************************************************************************//**
 * @brief Displays the current power mode.
 *
 * @return None.
*******************************************************************************/
void GetPowerMode(double* param, char paramNo) /*!< "powermode?" command */
{
    /*!< Send feedback to user. */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[4],powerMode);
}

/***************************************************************************//**
 * @brief Selects the power mode of the device.
 *
 * @param param[0] - selected power mode;
 *        paramNo  - number of parameters (must be >=1); only the first parameter is taken into account;
 *
 * @return None.
*******************************************************************************/
void SetPowerMode(double* param, char paramNo) /*!< "powermode=" command */
{
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 3)
        {
            param[0] = 3;
        }
        powerMode = (char)param[0];
        AD7156_SetPowerMode(powerMode);
        /*!< Send feedback to user. */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[4],powerMode);
    }
    else
    {
        /*!< Display error messages. */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[4], (char*)cmdDescription[4]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[4]);
    }
}

/***************************************************************************//**
 * @brief Enables or disabled conversions on selected channel.
 *
 * @param param[0] - selected channel;
 *        param[1] - 0 - disable selected channel;
 *                 - 1 - enable delected channel;
 *        paramNo  - number of parameters (must be >=2); only the first 2 parameters are taken into account;
 *
 * @return None.
*******************************************************************************/
void EnableCh(double* param, char paramNo) /*!< "channelenable=" command */
{
    if(paramNo >= 2)
    {
        if(param[0] < 1)
        {
            param[0] = 1;
        }
        if(param[0] > 2)
        {
            param[0] = 2;
        }
         if(param[1] < 0)
        {
            param[1] = 0;
        }
        if(param[1] > 1)
        {
            param[1] = 1;
        }

        if (param[0] == 1)
        {
            AD7156_ChannelState(AD7156_CHANNEL1, param[1]);
        }
        else
        {
            AD7156_ChannelState(AD7156_CHANNEL2, param[1]);
        }
        chEnable[(char)param[0]-1] = (char)param[1];
         /*!< Send feedback to user. */
        CONSOLE_Print("Channel %d status: %d\r\n", (char)param[0],
                      chEnable[(char)param[0]-1]);
    }
    else
    {
        /*!< Display error messages. */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[5], (char*)cmdDescription[5]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[5]);
    }

}

/***************************************************************************//**
 * @brief Reads 50 samples from the enabled channels and displays the data in pF.
 *
 * @return None.
*******************************************************************************/
void GetSamples(double* param, char paramNo) /*!< "samples?" command */
{
    float ch1       = 0; /*!< Value of the capacitance measured(pF). */
    float ch2       = 0; /*!< Value of the capacitance measured(pF). */
    long  samplesNr = 0; /*!< Number of samples to be read. */

    if (powerMode == 3) /*!< power-down mode */
    {
         CONSOLE_Print("Device in power-down mode. Please change the power mode \
in order to display the samples.\r\n");
    }
    else if (powerMode == 1) /*!< Continuous conversions mode */
    {
        /*!< Display 50 samples from channel1 and channel2. */
        for(samplesNr = 0; samplesNr < 50; samplesNr++)
        {
            if (chEnable[0] == 1)
            {
                ch1 = AD7156_WaitReadChannelCapacitance(AD7156_CHANNEL1);
            }
            else
            {
                ch1 = AD7156_ReadChannelCapacitance(AD7156_CHANNEL1);
            }

            if (chEnable[1] == 1)
            {
                ch2 = AD7156_WaitReadChannelCapacitance(AD7156_CHANNEL2);
            }
            else
            {
                ch2 = AD7156_ReadChannelCapacitance(AD7156_CHANNEL2);
            }

            CONSOLE_Print("Ch1: %.3f pF\r\n",ch1);
            CONSOLE_Print("Ch2: %.3f pF\r\n",ch2);
            TIME_DelayMs(100);
         }
    }
    else /*!< Idle mode or single conversion mode */
    {
        /*!< Display 50 samples from channel1 and channel2. */
        for(samplesNr = 0; samplesNr < 50; samplesNr++)
        {
            ch1 = AD7156_ReadChannelCapacitance(AD7156_CHANNEL1);
            ch2 = AD7156_ReadChannelCapacitance(AD7156_CHANNEL2);
            CONSOLE_Print("Ch1: %.3f pF\r\n",ch1);
            CONSOLE_Print("Ch2: %.3f pF\r\n",ch2);
            TIME_DelayMs(100);
        }
    }
}

/***************************************************************************//**
 * @brief Checks if any of the two buttons are pressed or released.
 *
 * @return None.
*******************************************************************************/
void GetButtonsStatus(double* param, char paramNo) /*!< "buttons?" command */
{
    unsigned char  status         = 0; /*!< The content of the STATUS register. */
    unsigned char  btnEvent       = 0; /*!< Holds the number of button actions. */
    unsigned char  prevBtn1State  = 0; /*!< Previous state of Button1(ON/OFF). */
    unsigned char  prevBtn2State  = 0; /*!< Previous state of Button2(ON/OFF). */

    /*!< Configure device. */
    AD7156_SetPowerMode(AD7156_CONV_MODE_IDLE);
    AD7156_SetThresholdMode(AD7156_THR_MODE_NEGATIVE,
                            AD7156_FIXED_THRESHOLD);
    AD7156_SetThreshold(AD7156_CHANNEL1, 1.0); /*!< Threshold = 1.0pF */
    AD7156_SetThreshold(AD7156_CHANNEL2, 1.0); /*!< Threshold = 1.0pF */
    AD7156_SetPowerMode(AD7156_CONV_MODE_CONT_CONV);
    /*!< Start polling buttons(channels) state. */
    CONSOLE_Print("Start button polling.\r\n");
    btnEvent = 0;
    prevBtn1State = 0;
    prevBtn2State = 0;
    /*!< After 15 button actions the execution returns to the menu. */
    while(btnEvent <= 15)
    {
        /*!< Read the STATUS register. */
        AD7156_GetRegisterValue(&status, AD7156_REG_STATUS, 1);
        /*!< Check for changes in the button state. */
        if(((status & AD7156_STATUS_OUT1) !=0) &&
           (prevBtn1State == 0))
        {
            CONSOLE_Print("BTN1 - pressed.\r\n");
            prevBtn1State = 1;
            btnEvent++;
        }
        else if(((status & AD7156_STATUS_OUT1) == 0) &&
                (prevBtn1State == 1))
        {
            CONSOLE_Print("BTN1 - released.\r\n");
            prevBtn1State = 0;
            btnEvent++;
        }
        if(((status & AD7156_STATUS_OUT2) !=0) &&
           (prevBtn2State == 0))
        {
            CONSOLE_Print("BTN2 - pressed.\r\n");
            prevBtn2State = 1;
            btnEvent++;
        }
        else if(((status & AD7156_STATUS_OUT2) == 0) &&
                (prevBtn2State == 1))
        {
            CONSOLE_Print("BTN2 - released.\r\n");
            prevBtn2State = 0;
            btnEvent++;
        }
    }
    CONSOLE_Print("Button polling finished.\r\n");
    /*!< Return to previous power mode. */
    AD7156_SetPowerMode(powerMode);
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) /*!< "reset!" command */
{
    AD7156_Reset();
    powerMode   = 1;
    chEnable[0] = 1;
    chEnable[1] = 1;
    /* Send feedback to user. */
    CONSOLE_Print("The device was reset.\r\n");
}
