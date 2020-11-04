/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodAD3.
 *   @author DNechita (dan.nechita@analog.com)
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
#include "AD7780.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/*!< List of available commands */
const char* cmdList[] = { "help?",
                          "power=",
                          "filter=",
                          "gain=",
                          "status=",
                          "register?",
                          "voltage?",
                          "samples?"};
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Power off/on the device. Accepted values:\r\n\
\t0 - Power off.\r\n\
\t1 - Power on.",
"  -  Select the filter frequency. Accepted values: \r\n\
\t0 - Filter frequency is 16.7 Hz. \r\n\
\t1 - Filter frequency is 10 Hz.",
"  -  Select gain. Accepted values:\r\n\
\t0 - Gain is set to 128.(Input voltage range must be +/-25.78mV)\r\n\
\t1 - Gain is set to 1.(Input voltage range must be +/-3300mV)",
"  -  Displays the conversion status for 'register?' and 'voltage?' commands.\
 Accepted values:\r\n\
\t0 - Disable status display.\r\n\
\t1 - Enable status display.",
"  -  Reads one sample from the input channel and displays the data register.",
"  -  Reads one sample from the input channel, converts it to voltage and displays the voltage.",
"  -  Reads a number of samples of the input channel and displays the voltage.\
 Accepted values: \r\n\
\t1 .. 1024 - Number of samples."};
const char* cmdExample[] = {
"",
"To power on the device, type power=1",
"To set filter frequency to 10 Hz, type filter=1",
"To set the gain to 128, type gain=0",
"To enable the conversion status display, type status=1",
"",
"",
"To display 30 samples, type samples?30"};
const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float V_REF = 3.3;    /*!< Vref = 3.3V */

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[8] = {GetHelp, SetPower, SetFilter, SetGain, SetStatus,
                               GetRegister, GetVoltage, GetSamples};
                               
/*!< Variables holding information about the device */
unsigned char  gain             = 1;
unsigned char  conversionStatus = 0;
unsigned char  showStatus       = 0;
unsigned char  power            = 1;

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
    if(AD7780_Init() == 0)
    {
        CONSOLE_Print("AD7780 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD7780 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Power off/on the device.
 *
 * @return None.
*******************************************************************************/
void SetPower(double* param, char paramNo) /*!< "power=" command */
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

        power = (unsigned char)param[0];
        if (power == 1)
        {
            /* Power-up the device. */
            AD7780_PDRST_HIGH;
        }
        else
        {
            /* Power-down the device. */
            AD7780_PDRST_LOW;
        }
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[1],power);
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
 * @brief Select the filter frequency.
 *
 * @return None.
*******************************************************************************/
void SetFilter(double* param, char paramNo) /*!< "fliter=" command */
{
    unsigned char filter = 0;

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

        filter = (unsigned char)param[0];
        if (filter == 1)
        {
             AD7780_FILTER_HIGH;   /*!< The update rate is set to 10 Hz. */
        }
        else
        {
            AD7780_FILTER_LOW;   /*!< The update rate is set to 16.7 Hz. */
        }
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[2],filter);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[2], (char*)cmdDescription[2]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[2]);
    }
}

/***************************************************************************//**
 * @brief Select gain.
 *
 * @return None.
*******************************************************************************/
void SetGain(double* param, char paramNo) /*!< "gain=" command */
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
        gain = (unsigned char)param[0];
        if(gain == 1)
        {
            AD7780_GAIN_HIGH;   /*!< Gain is set to 1. */
            gain = 1;
        }
        else
        {
            AD7780_GAIN_LOW;   /*!< Gain is set to 128. */
            gain = 128;
        }
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[3],(unsigned char)param[0]);
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
 * @brief Displays the conversion status for 'register?' and 'voltage?' commands.
 *
 * @return None.
*******************************************************************************/
void SetStatus(double* param, char paramNo) /*!< "status=" command */
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
        showStatus = (unsigned char)param[0];
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[4],gain);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[4], (char*)cmdDescription[4]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[4]);
    }
}
 
/***************************************************************************//**
 * @brief Reads one sample from the input channel and displays the data register.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    unsigned long data = 0;

    if (power == 1)
    {
       /*!< Read one sample form the device. */
       data = AD7780_ReadSample(&conversionStatus);
       /*!< Send the requested value to user */
       CONSOLE_Print("register=%d\r\n",data);
       if(showStatus)
       {
           /*!< Check gain flag. */
           if(conversionStatus & AD7780_STAT_GAIN)
           {
               CONSOLE_Print("gain=1\r\n");
           }
           else
           {
               CONSOLE_Print("gain=128\r\n");
           }
           /*!< Check filter flag. */
           if(conversionStatus & AD7780_STAT_FILTER)
           {
               CONSOLE_Print("filter=10 Hz\r\n");
           }
           else
           {
               CONSOLE_Print("filter=16.7 Hz\r\n");
           }
           /*!< Check for conversion error. */
           if(conversionStatus & AD7780_STAT_ERR)
           {
               CONSOLE_Print("Conversion error occurred.\r\n");
           }
           else
           {
               CONSOLE_Print("No conversion error.\r\n");
           }
           /*!< Check for serial transfer error. */
           if((conversionStatus &
              (AD7780_STAT_PAT1 | AD7780_STAT_PAT0)) != 0x01)
           {
               CONSOLE_Print("Serial transfer error occurred.\r\n");
           }
           else
           {
               CONSOLE_Print("No serial transfer error.\r\n");
           }
       }
    }
    else
   {
       CONSOLE_Print("The device is powered off. Please use the 'power=1' \
command in order to power on the device.\r\n");
   }
}

/***************************************************************************//**
 * @brief Reads one sample from the input channel, converts it voltage and displays the voltage.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    unsigned long data    = 0;
    double        voltage = 0;

  
    if (power == 1)
    {
        /*!< Read one sample from the device. */
        data = AD7780_ReadSample(&conversionStatus);
        /*!< Convert raw data to voltage. */
        voltage = AD7780_ConvertToVoltage(data, V_REF, gain);
        /*!< Send the requested value to user */
        CONSOLE_Print("voltage=%.3f [mV]\r\n",voltage);
        if(showStatus)
        {
            /*!< Check gain flag. */
            if(conversionStatus & AD7780_STAT_GAIN)
            {
                CONSOLE_Print("gain=1\r\n");
            }
            else
            {
                CONSOLE_Print("gain=128\r\n");
            }
            /*!< Check filter flag. */
            if(conversionStatus & AD7780_STAT_FILTER)
            {
                CONSOLE_Print("filter=10 Hz\r\n");
            }
            else
            {
                CONSOLE_Print("filter=16.7 Hz\r\n");
            }
            /*!< Check for conversion error. */
            if(conversionStatus & AD7780_STAT_ERR)
            {
                CONSOLE_Print("Conversion error occurred.\r\n");
            }
            else
            {
                CONSOLE_Print("No conversion error.\r\n");
            }
            /*!< Check for serial transfer error. */
            if((conversionStatus &
               (AD7780_STAT_PAT1 | AD7780_STAT_PAT0)) != 0x01)
            {
                CONSOLE_Print("Serial transfer error occurred.\r\n");
            }
            else
            {
                CONSOLE_Print("No serial transfer error.\r\n");
            }
       }
    }
   else
   {
       CONSOLE_Print("The device is powered off. Please use the 'power=1' \
command in order to power on the device.\r\n");
   }
}

/***************************************************************************//**
 * @brief Reads a number of samples of the input channel and displays the voltage.
 *
 * @return None.
*******************************************************************************/
void GetSamples(double* param, char paramNo) /*!< "samples?" command */
{
    unsigned long data    = 0;
    double        voltage = 0;
    long          samples = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 1)
        {
            param[0] = 1;
        }
        if(param[0] > 1024)
        {
            param[0] = 11024;
        }
        samples = (unsigned char)param[0];

        if (power == 1)
        {
            while(samples--)
            {
                AD7780_WaitRdyGoLow();
                /* Read one sample from the device. */
                data = AD7780_ReadSample(&conversionStatus);
                /* Convert raw data to voltage. */
                voltage = AD7780_ConvertToVoltage(data, V_REF, gain);
                /*!< Send the requested value to user */
                CONSOLE_Print("voltage=%.3f [mV]\r\n",voltage);
            }
        }
       else
       {
           CONSOLE_Print("The device is powered off. Please use the 'power=1' \
command in order to power on the device.\r\n");
       }
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[7], (char*)cmdDescription[7]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[7]);
    }
}