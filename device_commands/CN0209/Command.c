/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for CN0209.
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
#include "cn0209.h"
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
        .name = "ad7193Temp?",
        .description = "Displays the temperature from AD7193 device.",
        .acceptedValue = "",
        .example = "",
    },
    [2] = {
        .name = "ad7193id?",
        .description = "Displays the AD7193 device ID.",
        .acceptedValue = "",
        .example = "",
    },
    [3] = {
        .name = "ad7193Reset!",
        .description = "Reset the serial interface with the AD7193.",
        .acceptedValue = "",
        .example = "",
    },
    [4] = {
        .name = "adt7310Temp?",
        .description = "Displays the temperature from ADT7310 device.",
        .acceptedValue = "",
        .example = "",
    },
    [5] = {
        .name = "adt7310id?",
        .description = "Displays the ADT7310 device ID.",
        .acceptedValue = "",
        .example = "",
    },
    [6] = {
        .name = "adt7310Reset!",
        .description = "Resets the serial interface with the ADT7310.",
        .acceptedValue = "",
        .example = "",
    },
    [7] = {
        .name = "voltage?",
        .description = "Displays the input voltage on selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\t1 - channel 1.\r\n\
\t2 - channel 2.",
        .example = "",
    },
    [8] = {
        .name = "current?",
        .description = "Displays the input current on selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\t1 - channel 1.\r\n\
\t2 - channel 2.",
        .example = "",
    },
    [9] = {
        .name = "tempTC?",
        .description = "Displays the temperature detected by the thermocouple \
connected on selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\t1 - channel 1.\r\n\
\t2 - channel 2.",
        .example = "",
    },
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));
float      VREF  = 4.5;

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp,GetAd7193Temp,GetAd7193id,DoAd7193Reset,
                              GetAdt7310Temp,GetAdt7310id,DoAdt7310Reset,
                              GetVoltage,GetCurrent,GetTempTC};

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
    if(CN0209_Init() == 0)
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
 * @brief Displays the temperature from AD7193 device.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetAd7193Temp(double* param, char paramNo) // "ad7193Temp?" command
{
    float temp = 0;

    temp = AD7193_TemperatureRead();
    /* Send feedback to user */
    CONSOLE_Print("AD7193 temperature=%.2f[C].\r\n",temp);
}

/***************************************************************************//**
 * @brief Displays the AD7193 device ID.
 *
 * @param None.
 *
 * @return ad7193Id - requested device id.
*******************************************************************************/
void GetAd7193id(double* param, char paramNo) // "ad7193id?" command
{
    unsigned long ad7193id;

    ad7193id = AD7193_Read(AD7193_REG_ID, 1);
    CONSOLE_Print("AD7193 ID = %d.\r\n",ad7193id);

}

/***************************************************************************//**
 * @brief Reset the serial interface with the AD7193.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void DoAd7193Reset(double* param, char paramNo) // "ad7193Reset!" command
{
    AD7193_Reset();
    /* Send feedback to user */
    CONSOLE_Print("AD7193 was reset.\r\n");
}

/***************************************************************************//**
 * @brief Displays the temperature from ADT7310 device.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetAdt7310Temp(double* param, char paramNo) // "adt7310Temp?" command
{
    float temp = 0;

    temp = ADT7310_TemperatureRead();
    /* Send feedback to user */
    CONSOLE_Print("ADT7310 temperature=%.2f[C].\r\n",temp);
}

/***************************************************************************//**
 * @brief Displays the ADT7310 device ID.
 *
 * @param None.
 *
 * @return ad7193Id - requested device id.
*******************************************************************************/
void GetAdt7310id(double* param, char paramNo) // "adt7310id?" command
{
    unsigned long adt7310id;

    adt7310id = ADT7310_Read(ADT7310_REG_ID, 1);

    CONSOLE_Print("ADT7310 ID = %d.\r\n",adt7310id);
}

/***************************************************************************//**
 * @brief Resets the serial interface with the ADT7310.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void DoAdt7310Reset(double* param, char paramNo) // "adt7310Reset!" command
{
    ADT7310_Reset();
    /* Send feedback to user */
    CONSOLE_Print("ADT7310 was reset.\r\n");
}


/***************************************************************************//**
 * @brief Displays the input voltage on selected channel.
 *
 * @param param[0] - selected channel.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) // "voltage?" command
{
    unsigned char channel;
    unsigned char switchSel;
    unsigned long value        = 0;
    float         voltageValue = 0;

    /* Check if the parameter is valid */
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
        channel = (unsigned char)param[0];

        if (channel == 1)
        {
            switchSel = 0x50;
        }
        else
        {
            switchSel = 0xA0;
        }
        ADG1414_State(switchSel);
        AD7193_Write(AD7193_REG_CONF, 3, AD7193_CONF_PSEUDO +
                                         AD7193_CONF_CHAN(0x04) +
                                         AD7193_CONF_UNIPOLAR +
                                         AD7193_CONF_GAIN(AD7193_CONF_GAIN_1));
        value = AD7193_SingleConversion();
        ADG1414_State(0x0);
        voltageValue  = (AD7193_ConvertToVolts(value,VREF,1,1)) * 1000; // uV
        voltageValue  = (2250000 - voltageValue);
        voltageValue *= 5;

        /* Send feedback to user */
        CONSOLE_Print("Input voltage=%d[uV] for channel %d.\r\n",
                      (unsigned long)voltageValue, channel);
    }
    else
    {
         /* Display error messages */
         DisplayError(7);
    }
}

/***************************************************************************//**
 * @brief Displays the input current on selected channel.
 *
 * @param param[0] - selected channel.
 *
 * @return None.
*******************************************************************************/
void GetCurrent(double* param, char paramNo) // "current?" command
{
    unsigned char channel;
    unsigned char channelSel;
    unsigned long value        = 0;
    float         currentValue = 0;

    /* Check if the parameter is valid */
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
        channel = (unsigned char)param[0];

        if (channel == 2)
        {
            channelSel = 0x08;
        }
        else
        {
            channelSel = 0x80;
        }

        ADG1414_State(0x0);
        AD7193_Write(AD7193_REG_CONF, 3,
                     AD7193_CONF_PSEUDO +
                     AD7193_CONF_CHAN(channelSel) +
                     AD7193_CONF_UNIPOLAR +
                     AD7193_CONF_GAIN(AD7193_CONF_GAIN_1));

        value         = AD7193_SingleConversion();
        currentValue  = AD7193_ConvertToVolts(value,VREF,1,1) * 1000; // uV
        currentValue /= 200;    // uA

        /* Send feedback to user */
        CONSOLE_Print("Current=%.2f[uA] for channel %d.\r\n",currentValue,
                       channel);
    }
    else
    {
         /* Display error messages */
         DisplayError(8);
    }
}

/***************************************************************************//**
 * @brief Displays the temperature detected by the thermocouple connected on
 *        selected channel.
 *
 * @param param[0] - selected channel.
 *
 * @return None.
*******************************************************************************/
void GetTempTC(double* param, char paramNo) // "tempTC?" command
{
    unsigned char channel;
    unsigned char switchSel  = 0;
    unsigned char channelSel = 0;
    unsigned long value      = 0;
    float         tcValue    = 0;
    float         tempRef    = 0;

    /* Check if the parameter is valid */
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
        channel = (unsigned char)param[0];

        if (channel == 1)
        {
            switchSel  = 0x03;
            channelSel = 0x01;
            ADG1414_State(switchSel);
            AD7193_Write(AD7193_REG_GPOCON, 1, AD7193_GPOCON_GP32EN +
                         AD7193_GPOCON_P2DAT);    // Close S1, S2.
            TIME_DelayMs(100);
            AD7193_Write(AD7193_REG_CONF, 3, AD7193_CONF_CHAN(channelSel) +
                         AD7193_CONF_GAIN(AD7193_CONF_GAIN_128));
            TIME_DelayMs(100);
        }
        else
        {
            switchSel  = 0x0C;
            channelSel = 0x04;
            ADG1414_State(switchSel);
            AD7193_Write(AD7193_REG_GPOCON, 1, AD7193_GPOCON_GP32EN +
                         AD7193_GPOCON_P3DAT);    // Close S3, S4.*/
            TIME_DelayMs(100);
            AD7193_Write(AD7193_REG_CONF, 3, AD7193_CONF_CHAN(channelSel) +
                         AD7193_CONF_GAIN(AD7193_CONF_GAIN_128));
            TIME_DelayMs(100);
        }

        value = AD7193_SingleConversion();
        tcValue = AD7193_ConvertToVolts(value,VREF,128,0) * 1000; // uV
        tcValue /= 40;  // Type K - Degrees C.

        tempRef = ADT7310_TemperatureRead();
        tcValue += tempRef;

        AD7193_Write(AD7193_REG_GPOCON, 1, 0x0);
        ADG1414_State(0x0);
        /* Send feedback to user */
        CONSOLE_Print("TempTC=%.2f[C] for channel %d.\r\n",tcValue,channel);
    }
    else
    {
         /* Display error messages */
         DisplayError(9);
    }
}
