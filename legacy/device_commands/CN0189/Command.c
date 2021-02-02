/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for CN0189.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Command.h"
#include "Console.h"
#include "ad7887.h"
#include "TIME.h"
#include <math.h>

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
        .name = "calibX+1G!",
        .description = "Acquire data for calibration, when Xacc=+1[g].",
        .acceptedValue = "",
        .example = "",
    },
    [2] = {
        .name = "calibX-1G!",
        .description = "Acquire data for calibration, when Xacc=-1[g].",
        .acceptedValue = "",
        .example = "",
    },
    [3] = {
        .name = "calibY+1G!",
        .description = "Acquire data for calibration, when Yacc=+1[g].",
        .acceptedValue = "",
        .example = "",
    },
    [4] = {
        .name = "calibY-1G!",
        .description = "Acquire data for calibration, when Yacc=-1[g].",
        .acceptedValue = "",
        .example = "",
    },
    [5] = {
        .name = "calibCalculateParam!",
        .description = "Calculates the parameters for calibration.",
        .acceptedValue = "",
        .example = "",
    },
    [6] = {
        .name = "rawData?",
        .description = "Displays the raw data an both axes.(40 samples)",
        .acceptedValue = "",
        .example = "",
    },
    [7] = {
        .name = "acceleration?",
        .description = "Displays the acceleration on both axes.(40 samples)",
        .acceptedValue = "",
        .example = "",
    },
    [8] = {
        .name = "tilt?",
        .description = "Displays the tilt on both axes.(40 samples)",
        .acceptedValue = "",
        .example = "",
    },
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp,GetCalibrationXplus1G,
                             GetCalibrationXminus1G,GetCalibrationYplus1G,
                             GetCalibrationYminus1G,DoCalibrationCalculateParam,
                             GetRawData,GetAcceleration,GetTilt};

/* Variables holding information about the device */
unsigned char dataX[2]      = {0x00, 0x00};
unsigned char dataY[2]      = {0x00, 0x00};
float         offsetX       = 72.5f;
float         gainX         = 1.253f;
float         offsetY       = 57.5f;
float         gainY         = 1.2789f;
float         samples[4][2] = {{0,0},{0,0},{0,0},{0,0}}; //calibration readings

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
        /* Give a value to control words for X and Y axes for AD7887_Read16Bits()
        function. */
        dataX[0] = ad7887_CTRL_REF | ad7887_CTRL_SIN_DUAL | ad7887_CTRL_PM0;
        dataY[0] = ad7887_CTRL_REF | ad7887_CTRL_SIN_DUAL | ad7887_CTRL_CH |
                   ad7887_CTRL_PM0;
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
 * @brief Displays the raw data on both axes.(40 samples)
 *
 * @return None.
*******************************************************************************/
void GetRawData(double* param, char paramNo) // "rawData?" command
{
    unsigned short ch0RawData = 0;
    unsigned short ch1RawData = 0;
    signed short   rawDataX   = 0;
    signed short   rawDataY   = 0;
    unsigned char  count      = 0;

    for (count = 0; count < 40; count++)
    {
        ch0RawData = (AD7887_Read16Bits(dataX)) & 0x0FFF;
        ch1RawData = (AD7887_Read16Bits(dataY)) & 0x0FFF;
        /* Be able to have also negative values. */
        rawDataX = (signed short)ch0RawData - 2048;
        rawDataY = (signed short)ch1RawData - 2048;
        /* Send feedback to user */
        CONSOLE_Print("Raw Data X axis=%d          ",rawDataX);
        CONSOLE_Print("Raw Data Y axis=%d\r\n",rawDataY);
        /* Time delay in order to be able to see the changes in real time */
        TIME_DelayMs(200);
    }
}

/***************************************************************************//**
 * @brief Displays the acceleration on both axes.(40 samples)
 *
 * @return None.
*******************************************************************************/
void GetAcceleration(double* param, char paramNo) // "acceleration?" command
{
    unsigned short ch0RawData       = 0;
    unsigned short ch1RawData       = 0;
    float          rawDataX         = 0;
    float          rawDataY         = 0;
    float          ch0CorrectedData = 0;
    float          ch1CorrectedData = 0;
    float          accelX           = 0;
    float          accelY           = 0;
    unsigned char  count            = 0;

    for (count = 0; count < 40; count++)
    {
        ch0RawData = (AD7887_Read16Bits(dataX)) & 0x0FFF;
        ch1RawData = (AD7887_Read16Bits(dataY)) & 0x0FFF;
        /* Be able to have also negative values. */
        rawDataX = (float)ch0RawData - 2048;
        rawDataY = (float)ch1RawData - 2048;
        /* Compute the corrected data using offset and gain. */
        ch0CorrectedData = (rawDataX - offsetX) * gainX;
        ch1CorrectedData = (rawDataY - offsetY) * gainY;
        /* Compute the acceleration values on the X and Y axis. */
        accelX = ((float)ch0CorrectedData) / ((1 << 11));
        accelY = ((float)ch1CorrectedData) / ((1 << 11));
        /* Limit the X acceleration to 1g. */
        if(fabs(accelX) > 1.0f)
        {
            accelX = accelX / fabs(accelX);
        }
        /* Limit the Y acceleration to 1g. */
        if(fabs(accelY) > 1.0f)
        {
            accelY = accelY / fabs(accelY);
        }
        /* Send feedback to user */
        CONSOLE_Print("Acceleration X axis=%.3f[g]          ",accelX);
        CONSOLE_Print("Acceleration Y axis=%.3f[g]\r\n",accelY);
        /* Time delay in order to be able to see the changes in real time. */
        TIME_DelayMs(200);
    }
}

/***************************************************************************//**
 * @brief Displays the tilt on both axes.(40 samples)
 *
 * @return None.
*******************************************************************************/
void GetTilt(double* param, char paramNo) // "tilt?" command
{
    unsigned short ch0RawData       = 0;
    unsigned short ch1RawData       = 0;
    float          rawDataX         = 0;
    float          rawDataY         = 0;
    float          ch0CorrectedData = 0;
    float          ch1CorrectedData = 0;
    float          accelX           = 0;
    float          accelY           = 0;
    float          asinX            = 0;
    float          acosY            = 0;
    unsigned char  count            = 0;

    for (count = 0; count < 40; count++)
    {
        ch0RawData = (AD7887_Read16Bits(dataX)) & 0x0FFF;
        ch1RawData = (AD7887_Read16Bits(dataY)) & 0x0FFF;
        /* Be able to have also negative values. */
        rawDataX = (float)ch0RawData - 2048;
        rawDataY = (float)ch1RawData - 2048;
        /* Compute the corrected data using offset and gain. */
        ch0CorrectedData = (rawDataX - offsetX) * gainX;
        ch1CorrectedData = (rawDataY - offsetY) * gainY;
        /* Compute the acceleration values on the X and Y axis. */
        accelX = ((float)ch0CorrectedData) / ((1 << 11));
        accelY = ((float)ch1CorrectedData) / ((1 << 11));
        /* Limit the X acceleration to 1g. */
        if(fabs(accelX) > 1.0f)
        {
            accelX = accelX / fabs(accelX);
        }
        /* Limit the Y acceleration to 1g. */
        if(fabs(accelY) > 1.0f)
        {
            accelY = accelY / fabs(accelY);
        }
        /* Compute the tilt. */
        asinX = (float)asin(accelX) / M_PI * 180;
        acosY = (float)acos(accelY) / M_PI * 180;
        /* Send feedback to user */
        CONSOLE_Print("Tilt X axis=%.3f[deg]          ",asinX);
        CONSOLE_Print("Tilt Y axis=%.3f[deg]\r\n",acosY);
        /* Time delay in order to be able to see the changes in real time. */
        TIME_DelayMs(200);
    }
}

/***************************************************************************//**
 * @brief Acquire data for calibration, when Xacc=+1[g].
 *
 * @return None.
*******************************************************************************/
void GetCalibrationXplus1G(double* param, char paramNo) //"calibX+1G!!" command
{
    unsigned short ch0RawData = 0;
    unsigned short ch1RawData = 0;

    ch0RawData = (AD7887_Read16Bits(dataX)) & 0x0FFF;
    ch1RawData = (AD7887_Read16Bits(dataY)) & 0x0FFF;

    samples[0][0] = (float)ch0RawData - 2048;
    samples[0][1] = (float)ch1RawData - 2048;
    /* Send feedback to user */
    CONSOLE_Print("Data was acquired for Xacc=+1[g].\r\n");
}

/***************************************************************************//**
 * @brief Acquire data for calibration, when Xacc=-1[g].
 *
 * @return None.
*******************************************************************************/
void GetCalibrationXminus1G(double* param, char paramNo) //"calibX-1G!!" command
{
    unsigned short ch0RawData = 0;
    unsigned short ch1RawData = 0;

    ch0RawData = (AD7887_Read16Bits(dataX)) & 0x0FFF;
    ch1RawData = (AD7887_Read16Bits(dataY)) & 0x0FFF;

    samples[1][0] = (float)ch0RawData - 2048;
    samples[1][1] = (float)ch1RawData - 2048;
    /* Send feedback to user */
    CONSOLE_Print("Data was acquired for Xacc=-1[g].\r\n");
}

/***************************************************************************//**
 * @brief Acquire data for calibration, when Yacc=+1[g].
 *
 * @return None.
*******************************************************************************/
void GetCalibrationYplus1G(double* param, char paramNo) //"calibY+1G!!" command
{
    unsigned short ch0RawData = 0;
    unsigned short ch1RawData = 0;

    ch0RawData = (AD7887_Read16Bits(dataX)) & 0x0FFF;
    ch1RawData = (AD7887_Read16Bits(dataY)) & 0x0FFF;

    samples[2][0] = (float)ch0RawData - 2048;
    samples[2][1] = (float)ch1RawData - 2048;
    /* Send feedback to user */
    CONSOLE_Print("Data was acquired for Yacc=+1[g].\r\n");
}
/***************************************************************************//**
 * @brief Acquire data for calibration, when Yacc=-1[g].
 *
 * @return None.
*******************************************************************************/
void GetCalibrationYminus1G(double* param, char paramNo) //"calibY-1G!!" command
{
    unsigned short ch0RawData = 0;
    unsigned short ch1RawData = 0;

    ch0RawData = (AD7887_Read16Bits(dataX)) & 0x0FFF;
    ch1RawData = (AD7887_Read16Bits(dataY)) & 0x0FFF;

    samples[3][0] = (float)ch0RawData - 2048;
    samples[3][1] = (float)ch1RawData - 2048;
    /* Send feedback to user */
    CONSOLE_Print("Data was acquired for Yacc=-1[g].\r\n");
}

/***************************************************************************//**
 * @brief Calculates the parameters for calibration.
 *
 * @return None.
*******************************************************************************/
void DoCalibrationCalculateParam(double* param, char paramNo) //"calibCalculateParam!" command
{
    /* Compute the new X and Y offsets and gains */
    offsetX = (samples[2][0] + samples[3][0]) / 2;
    offsetY = (samples[0][1] + samples[1][1]) / 2;
    gainX   = ((2048.0f / (samples[0][0] - offsetX)) +
              (-2048.0f / (samples[1][0] - offsetX))) / 2;
    gainY   = ((2048.0f / (samples[2][1] - offsetY)) +
              (-2048.0f / (samples[3][1] - offsetY))) / 2;
    /* Send feedback to user */
    CONSOLE_Print("Offset X axis=%.3f\r\n",offsetX);
    CONSOLE_Print("Offset Y axis=%.3f\r\n",offsetY);
    CONSOLE_Print("Gain X axis=%.3f\r\n",gainX);
    CONSOLE_Print("Gain Y axis=%.3f\r\n",gainY);
}
