/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for CN0178.
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
#include "ad7466.h"
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
        .description = "Makes a four points calibration.",
        .acceptedValue = "Accepted values:\r\n\
\tpower input1(lowest value): \r\n\
\t-60 .. 0 - first point input power in [dBm].\r\n\
\tpower input2: \r\n\
\t-60 .. 0 - second point input power in [dBm].\r\n\
\tpower input3: \r\n\
\t-60 .. 0 - third point input power in [dBm].\r\n\
\tpower input4(highest value): \r\n\
\t-60 .. 0 - fourth point input power in [dBm].",
        .example = "To choose -30[dBm], -20[dBm], -10[dBm] and 0[dBm as the \
two calibration points, type: calibration=-30 -20 -10 0",
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
\t-60 .. 0 - true input power in [dBm].",
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
float          slope[3]     = { 60, 60, 60};        // nominal values
float          intercept[3] = {-50,-50,-50};        // nominal values
char           pin[4]       = {0, 0, 0, 0};
unsigned short code[4]      = {0, 0, 0, 0};
float          pinCalc      = 0;

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
    if(AD7466_Init() == 0)
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
 * @brief Makes a four points calibration.
 *
 * @param param[0] - first point input power in [dBm].
 *        param[1] - second point input power in [dBm].
 *        param[2] - third point input power in [dBm].
 *        param[3] - fourth point input power in [dBm].
 *
 * @return None.
*******************************************************************************/
void DoCalibration(double* param, char paramNo) // "calibration=" command
{
    unsigned char  count      = 0;
    unsigned long  average[4] = {0,0,0,0};

    /* Check if the parameters are valid. */
    if(paramNo >= 4)
    {
        if (param[0] < -60)
        {
            param[0] = -60;
        }
        if (param[0] > 0)
        {
            param[0] = 0;
        }
        pin[0] = (char)param[0];
        if (param[1] < -60)
        {
            param[1] = -60;
        }
        if (param[1] > 0)
        {
            param[1] = 0;
        }
        pin[1] = (char)param[1];
        if (param[2] < -60)
        {
            param[2] = -60;
        }
        if (param[2] > 0)
        {
            param[2] = 0;
        }
        pin[2] = (char)param[2];
        if (param[3] < -60)
        {
            param[3] = -60;
        }
        if (param[3] > 0)
        {
            param[3] = 0;
        }
        pin[3] = (char)param[3];

        CONSOLE_Print("First set the input power to %d[dBm].\r\n", pin[0]);
        CONSOLE_Print("You have 10 seconds for that.\r\n");
        /* Make a 10 seconds delay. */
        for (count = 0; count < 10; count++)
        {
            TIME_DelayMs(1000);
        }
        /* Calculate the average ADC code from 50 samples. */
        for (count = 0; count < 50; count++)
        {
            code[0] = (AD7466_Read16Bits()) & 0x0FFF;
            average[0] += code[0];
        }
        average[0] /= 50;
        code[0] = (unsigned short)average[0];

        CONSOLE_Print("Now set the input power to %d[dBm].\r\n", pin[1]);
        CONSOLE_Print("You have 10 seconds for that.\r\n");
        /* Make a 10 seconds delay. */
        for (count = 0; count < 10; count++)
        {
            TIME_DelayMs(1000);
        }
        /* Calculate the average ADC code from 50 samples. */
        for (count = 0; count < 50; count++)
        {
            code[1] = (AD7466_Read16Bits()) & 0x0FFF;
            average[1] += code[1];
        }
        average[1] /= 50;
        code[1] = (unsigned short)average[1];
        /* Calculate the first slope and intercept according to datasheet formulas. */
        slope[0]     = (float)(code[1] - code[0]) / (pin[1] - pin[0]);
        intercept[0] = (float)pin[1] - (float)(code[1] / slope[0]);
        /* Send feedback to user */
        CONSOLE_Print("The first calculated slope is %.3f.\r\n", slope[0]);
        CONSOLE_Print("The first calculated intercept is %.3f.\r\n", intercept[0]);
        CONSOLE_Print("\r\n");

        CONSOLE_Print("Now set the input power to %d[dBm].\r\n", pin[2]);
        CONSOLE_Print("You have 10 seconds for that.\r\n");
        /* Make a 10 seconds delay. */
        for (count = 0; count < 10; count++)
        {
            TIME_DelayMs(1000);
        }
        /* Calculate the average ADC code from 50 samples. */
        for (count = 0; count < 50; count++)
        {
            code[2] = (AD7466_Read16Bits()) & 0x0FFF;
            average[2] += code[2];
        }
        average[2] /= 50;
        code[2] = (unsigned short)average[2];
        /* Calculate the second slope and intercept according to datasheet formulas. */
        slope[1] = (float)(code[2] - code[1]) / (pin[2] - pin[1]);
        intercept[1] = (float)pin[2] - (float)(code[2] / slope[1]);
        /* Send feedback to user */
        CONSOLE_Print("The second calculated slope is %.3f.\r\n", slope[1]);
        CONSOLE_Print("The second calculated intercept is %.3f.\r\n", intercept[1]);
        CONSOLE_Print("\r\n");

        CONSOLE_Print("Now set the input power to %d[dBm].\r\n", pin[3]);
        CONSOLE_Print("You have 10 seconds for that.\r\n");
        /* Make a 10 seconds delay. */
        for (count = 0; count < 10; count++)
        {
            TIME_DelayMs(1000);
        }
        /* Calculate the average ADC code from 50 samples. */
        for (count = 0; count < 50; count++)
        {
            code[3] = (AD7466_Read16Bits()) & 0x0FFF;
            average[3] += code[3];
        }
        average[3] /= 50;
        code[3] = (unsigned short)average[3];
        /* Calculate the third slope and intercept according to datasheet formulas. */
        slope[2] = (float)(code[3] - code[2]) / (pin[3] - pin[2]);
        intercept[2] = (float)pin[3] - (float)(code[3] / slope[2]);
        /* Send feedback to user */
        CONSOLE_Print("The last calculated slope is %.3f.\r\n", slope[2]);
        CONSOLE_Print("The last calculated intercept is %.3f.\r\n", intercept[2]);
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
    unsigned char   count           = 0;
    unsigned short  codeRead        = 0;
    unsigned long   average         = 0;
    float           slopeChosen     = 0;
    float           interceptChosen = 0;

    /* Calculate the average ADC code from 50 samples. */
    for (count = 0; count < 50; count++)
    {
        codeRead = (AD7466_Read16Bits()) & 0x0FFF;
        average += codeRead;
    }
    average /= 50;
    codeRead = (unsigned short)average;
    /* Verify if the code read is inside the calibration range. */
    if ((codeRead < (code[0] - 5)) || (codeRead > (code[3] + 5)))
    {
        CONSOLE_Print("ADC Code=%d. \r\n", codeRead);
        CONSOLE_Print("Input power is outside calibration range! \r\n");
    }
    else
    {
        /* Choose which slope and intercept to use. */
        if ((codeRead >= (code[0] - 5)) && (codeRead < code[1]))
        {
            slopeChosen = slope[0];
            interceptChosen = intercept[0];
        }
        if ((codeRead >= code[1]) && (codeRead < code[2]))
        {
            slopeChosen = slope[1];
            interceptChosen = intercept[1];
        }
        if ((codeRead >= code[2]) && (codeRead <= (code[3] + 5)))
        {
            slopeChosen = slope[2];
            interceptChosen = intercept[2];
        }
        /* Calculate the input power according to datasheet formula. */
        pinCalc = (float)(codeRead / slopeChosen) + interceptChosen;
        /* Send feedback to user */
        CONSOLE_Print("ADC Code=%d. \r\n", codeRead);
        CONSOLE_Print("The calculated input power is %.3f[dBm]. \r\n", pinCalc);
    }
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
        if (param[0] < -60)
        {
            param[0] = -60;
        }
        if (param[0] > 0)
        {
            param[0] = 0;
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
