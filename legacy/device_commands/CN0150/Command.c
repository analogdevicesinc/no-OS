/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for CN0150.
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
#include "ad7887.h"
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
        .name = "calibration=",
        .description = "Makes a two points calibration.",
        .acceptedValue = "Accepted values:\r\n\
\tpower input1: \r\n\
\t-50 .. -5 - first point input power in [dBm].\r\n\
\tpower input2: \r\n\
\t-50 .. -5 - second point input power in [dBm].",
        .example = "To choose -30[dBm] and -20[dBm] as the two calibration \
points, type: calibration=-30 -20",
    },
    [2] = {
        .name = "pinCalc?",
        .description = "Displays the calculated input power.",
        .acceptedValue = "",
        .example = "",
    },
    [3] = {
        .name = "error?",
        .description = "Displays the error associated with the last input \
power calculation.",
        .acceptedValue = "Accepted values: \r\n\
\t-50 .. -5 - true input power in [dBm].",
        .example = "To display the error between the last calculated input \
power and the last true \r\n input power=-20[dBm], type: error?-20",
    },
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp, DoCalibration, GetPinCalc, GetError};

/* Variables holding information about the device */
float          slope     = -24;          // nominal value
float          intercept = 20;           // nominal value
float          pinCalc   = 0;
unsigned char  data[2]   = {0x21, 0x00}; /* control bytes for ADC
0x21 - the on-chip reference is disabled; mode 2; single channel. */

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
    if(AD7887_Init() == 0)
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
 * @brief Makes a two points calibration.
 *
 * @param param[0] - first point input power in [dBm].
 *        param[1] - second point input power in [dBm].
 *
 * @return None.
*******************************************************************************/
void DoCalibration(double* param, char paramNo) // "calibration=" command
{
    char           pin1     = 0;
    char           pin2     = 0;
    unsigned short code1    = 0;
    unsigned short code2    = 0;
    unsigned char  count    = 0;
    unsigned long  average1 = 0;
    unsigned long  average2 = 0;

    /* Check if the parameters are valid. */
    if(paramNo >= 2)
    {
        if (param[0] < -50)
        {
            param[0] = -50;
        }
        if (param[0] > -5)
        {
            param[0] = -5;
        }
        pin1 = (char)param[0];

        if (param[1] < -50)
        {
            param[1] = -50;
        }
        if (param[1] > -5)
        {
            param[1] = -5;
        }
        pin2 = (char)param[1];

        CONSOLE_Print("First set the input power to %d[dBm].\r\n", pin1);
        CONSOLE_Print("You have 10 seconds for that.\r\n");
        /* Make a 10 seconds delay. */
        for (count = 0; count < 10; count++)
        {
            TIME_DelayMs(1000);
        }
        /* Calculate the average ADC code from 50 samples. */
        for (count = 0; count < 50; count++)
        {
            code1 = (AD7887_Read16Bits(data)) & 0x0FFF;
            average1 += code1;
        }
        average1 /= 50;
        code1 = (unsigned short)average1;

        CONSOLE_Print("Now set the input power to %d[dBm].\r\n", pin2);
        CONSOLE_Print("You have 10 seconds for that.\r\n");
        /* Make a 10 seconds delay. */
        for (count = 0; count < 10; count++)
        {
            TIME_DelayMs(1000);
        }
        /* Calculate the average ADC code from 50 samples. */
        for (count = 0; count < 50; count++)
        {
            code2 = (AD7887_Read16Bits(data)) & 0x0FFF;
            average2 += code2;
        }
        average2 /= 50;
        code2 = (unsigned short)average2;
        /* Calculate the slope and intercept according to datasheet formulas. */
        slope = (float)(code2 - code1) / (pin2 - pin1);
        intercept = (float)pin2 - (float)(code2 / slope);
        /* Send feedback to user */
        CONSOLE_Print("The slope is %.3f.\r\n", slope);
        CONSOLE_Print("The intercept is %.3f.\r\n", intercept);
    }
    else
    {
         /* Display error messages */
         DisplayError(1);
    }
}

/***************************************************************************//**
 * @brief Displays the calculated input power.
 *
 * @return None.
*******************************************************************************/
void GetPinCalc(double* param, char paramNo) // "pinCalc?" command
{
    unsigned char  count   = 0;
    unsigned short code    = 0;
    unsigned long  average = 0;

    /* Calculate the average ADC code from 50 samples. */
    for (count = 0; count < 50; count++)
    {
        code = (AD7887_Read16Bits(data)) & 0x0FFF;
        average += code;
    }
    average /= 50;
    code = (unsigned short)average;
    /* Calculate the input power according to datasheet formula. */
    pinCalc = (float)(code / slope) + intercept;
    /* Send feedback to user */
    CONSOLE_Print("ADC Code=%d. \r\n", code);
    CONSOLE_Print("The calculated input power is %.3f[dBm]. \r\n", pinCalc);
}

/***************************************************************************//**
 * @brief Displays the error associated with the last input power calculation.
 *
 * @param param[0] - last true input power.
 *
 * @return None.
*******************************************************************************/
void GetError(double* param, char paramNo) // "error?" command
{
    float error  = 0;

    if(paramNo >= 1)
    {
        if (param[0] < -50)
        {
            param[0] = -50;
        }
        if (param[0] > -5)
        {
            param[0] = -5;
        }
        /* Calculate the error according to datasheet formula. */
        error = pinCalc - param[0];
        /* Send feedback to user */
        CONSOLE_Print("The error between the last calculated input power and \
the last true input power is %.3f. \r\n", error);
    }
    else
    {
         /* Display error messages */
         DisplayError(3);
    }
}
