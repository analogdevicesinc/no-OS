/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodAD5.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
#include "AD7193.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/*!< List of available commands */
const char* cmdList[] =
    {"help?",
     "reset!",
     "polarity?",
     "polarity=",
     "range?",
     "range=",
     "pseudoBit?",
     "pseudoBit=",
     "register?",
     "voltage?",
     "temperature?"};
const char* cmdDescription[] ={
"  -  Displays all available commands.",
"  -  Resets the AD7193.",
"  -  Displays the polarity value.",
"  -  Sets the polarity value. Accepted values: \r\n\
\t0 - Bipolar mode. \r\n\
\t1 - Unipolar mode.",
"  -  Displays the range value.",
"  -  Sets the range value. Accepted values: \r\n\
\t0 - Gain=1.  (Input voltage range must be +/-2.5V) \r\n\
\t3 - Gain=8.  (Input voltage range must be +/-312.5mV)\r\n\
\t4 - Gain=16. (Input voltage range must be +/-156.2mV)\r\n\
\t5 - Gain=32. (Input voltage range must be +/-78.12mV) \r\n\
\t6 - Gain=64. (Input voltage range must be +/-39.06mV) \r\n\
\t7 - Gain=128. (Input voltage range must be +/-19.53mV)",
"  -  Displays the Pseudo Bit value (AD7193_REG_CONF).",
"  -  Sets the Pseudo Bit Value (AD7193_REG_CONF). Accepted values: \r\n\
\t0 - Disables the pseudo differential measuring. \r\n\
\t1 - Enables the pseudo differential measuring.",
"  -  Displays the value of the data register (AD7193_REG_DATA) for specified \
channel. Accepted values: \r\n\
\t0..7 - selected channel.",
"  -  Displays the voltage applied to specified channel. Accepted values: \r\n\
\t0..7 - selected channel.",
"  -  Displays the temperature."};
const char* cmdExample[] = {
"",
"",
"",
"To set the bipolar mode, type: polarity=0",
"",
"To set the gain=16, type range=4",
"",
"To enable the pseudo differential measuring, type: pseudoBit=1",
"To display the data register for channel 1, type: register?1",
"To display the voltage applied to channel 3, type: voltage?3",
""};
const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float V_REF = 2.5;    /*!< Vref = 2.5V */

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[13] = {GetHelp, DoReset, GetPolarity, SetPolarity,
                               GetRange, SetRange, GetPseudoBit, SetPseudoBit,
                               GetRegister, GetVoltage, GetTemperature};
                               
/*!< Variables holding information about the device */
unsigned char channel          = 0;    /*!< Enabled channel */
unsigned char pseudoBit        = 0;    /*!< Pseudo Bit value */
unsigned char selectedPolarity = 0;    /*!< Selected polarity value */
unsigned char selectedRange    = 0;    /*!< Selected range value */

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
    if(AD7193_Init() == 0)
    {
        CONSOLE_Print("AD7193 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD7193 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Resets the AD7193.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) /*!< "reset!" command */
{
    AD7193_Reset();
    /*!< Selects bipolar operation and ADC's input range to +-2.5V. */
    //AD7193_RangeSetup(0, AD7193_CONF_GAIN_1);
    selectedPolarity = 0;
    selectedRange    = 7;
    pseudoBit        = 0;
    /*!< Send feedback to user */
    CONSOLE_Print("Device was reset.\r\n");
}

/***************************************************************************//**
 * @brief Displays the polarity value.
 *
 * @return None.
*******************************************************************************/
void GetPolarity(double* param, char paramNo) /*!< "polarity?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[3],selectedPolarity);
}

/***************************************************************************//**
 * @brief Sets the polarity value.
 *
 * @return None.
*******************************************************************************/
void SetPolarity(double* param, char paramNo) /*!< "polarity=" command */
{
    /*!< Check if the parameter is valid */
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

        selectedPolarity = (unsigned char)param[0];

        AD7193_RangeSetup(selectedPolarity, selectedRange);

        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[3],selectedPolarity);
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
 * @brief Displays the range value.
 *
 * @return None.
*******************************************************************************/
void GetRange(double* param, char paramNo) /*!< "range?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[5],selectedRange);
}

/***************************************************************************//**
 * @brief Sets the range value.
 *
 * @return None.
*******************************************************************************/
void SetRange(double* param, char paramNo) /*!< "range=" command */
{
    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 3)
        {
            param[0] = 0;
        }
        if(param[0] > 7)
        {
            param[0] = 7;
        }

        selectedRange = (unsigned char)param[0];

        AD7193_RangeSetup(selectedPolarity, selectedRange);

        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[5],selectedRange);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[5], (char*)cmdDescription[5]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[5]);
    }
}

/***************************************************************************//**
 * @brief Displays the Pseudo Bit value (AD7193_REG_CONF).
 *
 * @return None.
*******************************************************************************/
void GetPseudoBit(double* param, char paramNo) /*!< "pseudoBit?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[7],pseudoBit);
}

/***************************************************************************//**
 * @brief Sets the Pseudo Bit Value (AD7193_REG_CONF).
 *
 * @return None.
*******************************************************************************/
void SetPseudoBit(double* param, char paramNo) /*!< "pseudoBit=" command */
{
    unsigned long regValue = 0;
    
    /*!< Check if the parameter is valid */
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

       switch((unsigned char)param[0])
       {
        case 1:
            pseudoBit = 1;
            regValue = AD7193_GetRegisterValue(AD7193_REG_CONF, 3, 1);
            regValue |= AD7193_CONF_PSEUDO;
            AD7193_SetRegisterValue(AD7193_REG_CONF, regValue, 3, 1);
            break;
        default:
            pseudoBit = 0;
            regValue = AD7193_GetRegisterValue(AD7193_REG_CONF, 3, 1);
            regValue &= ~ AD7193_CONF_PSEUDO;
            AD7193_SetRegisterValue(AD7193_REG_CONF, regValue, 3, 1);
            break;
        }

        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[7],pseudoBit);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[7], (char*)cmdDescription[7]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[7]);
    }
}

/***************************************************************************//**
 * @brief Displays the value of the data register (AD7193_REG_DATA) for the specified channel.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    unsigned long data = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 7)
        {
            param[0] = 7;
        }

        channel = (unsigned char)param[0];
        AD7193_ChannelSelect(channel);
         /*!< Start a conversion */
        data = AD7193_SingleConversion();
        /*!< Send the requested value to user */
        CONSOLE_Print("Register=%d for channel %d.\r\n",data,channel);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[8], (char*)cmdDescription[8]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[8]);
    }
}

/***************************************************************************//**
 * @brief Displays the temperature.
 *
 * @return None.
*******************************************************************************/
void GetTemperature(double* param, char paramNo) /*!< "temperature?" command */
{
    float temperatureValue = 0;

    /*!< Start a conversion */
    temperatureValue = AD7193_TemperatureRead();
    /*!< Send the requested value to user */
    CONSOLE_Print("temperature=%.2f [C]\r\n",temperatureValue);
    /*!< Return to previous polarity and gain*/
    AD7193_RangeSetup(selectedPolarity, selectedRange);
}

/***************************************************************************//**
 * @brief Displays the voltage applied to the specified channel.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    unsigned long data    = 0;
    double        voltage = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 7)
        {
            param[0] = 7;
        }

        channel = (unsigned char)param[0];
        AD7193_ChannelSelect(channel);
        /*!< Start a conversion */
        data = AD7193_SingleConversion();
        voltage = AD7193_ConvertToVolts(data, V_REF);
        /*!< Send the requested value to user */
        CONSOLE_Print("Voltage=%.3f [V] for channel %d.\r\n",voltage,channel);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[9], (char*)cmdDescription[9]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[9]);
    }
}