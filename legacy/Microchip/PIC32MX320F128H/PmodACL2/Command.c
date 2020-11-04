/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodACL.
 *   @author DNechita (Dan.Nechita@analog.com)
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
#include "Communication.h"
#include "Command.h"
#include "ADXL362.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

/*!< List of available commands */
const char* cmdList[] = { "help?",
                          "id?",
                          "measure=",
                          "temperature?",
                          "reset!",
                          "acceleration?",
                          "accelerationX?",
                          "accelerationY?",
                          "accelerationZ?",
                          "activity?"};
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Display device details.",
"  -  Start/stop the measurement process of the device. Accepted values:\r\n\
\t0 - Stop measurement. \r\n\
\t1 - Start measurement.",
"  -  Displays the temperature.",
"  -  Resets the device.",
"  -  Displays the accelerations on XYZ axes.",
"  -  Displays the acceleration on X axis.",
"  -  Displays the acceleration on Y axis.",
"  -  Displays the acceleration on Z axis.",
"  -  Displays the activity status of the device. It runs for 5 motion detections."};
const char* cmdExample[] = {
"",
"",
"To turn on the measurement process, type: measure=1.",
"",
"",
"",
"",
"",
"",
""};

const char cmdNo = (sizeof(cmdList) / sizeof(const char*));

/***************************************************************************/
/************************ Variables Definitions ****************************/
/***************************************************************************/
cmdFunction cmdFunctions[10] = {GetHelp, GetId, SetMeasurement, GetTemperature,
                               DoReset, GetAcceleration, GetAccelerationX,
                               GetAccelerationY, GetAccelerationZ, GetActivity};
char  measureSetting = 0;

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

    if(ADXL362_Init() == 0)
    {
        CONSOLE_Print("ADXL362 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("ADXL362 Error\r\n");

        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Displays device details.
 *
 * @return None.
*******************************************************************************/
void GetId(double* param, char paramNo) /*!< "id?" command */
{
    unsigned char  regVal = 0;

        /*!< Send the requested value to user. */
        CONSOLE_Print("Analog Devices device ID: ");
        ADXL362_GetRegisterValue(&regVal, ADXL362_REG_DEVID_AD, 1);
        CONSOLE_Print("%d\r\n",regVal);

        CONSOLE_Print("Analog Devices MEMS ID: ");
        ADXL362_GetRegisterValue(&regVal, ADXL362_REG_DEVID_MST, 1);
        CONSOLE_Print("%d\r\n",regVal);

        CONSOLE_Print("Analog Devices PART ID: ");
        ADXL362_GetRegisterValue(&regVal, ADXL362_REG_PARTID, 1);
        CONSOLE_Print("%d\r\n",regVal);

        CONSOLE_Print("Analog Devices REVISION ID: ");
        ADXL362_GetRegisterValue(&regVal, ADXL362_REG_REVID, 1);
        CONSOLE_Print("%d\r\n",regVal);
}

/***************************************************************************//**
 * @brief Turn on/off the measurement process of the device.
 *
 * @return None.
*******************************************************************************/
void SetMeasurement(double* param, char paramNo) /*!< "measure=" command */
{
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

        measureSetting = param[0];
        /*!< Turn on/off the measurement process of the device. */
        ADXL362_SetPowerMode(param[0]);
        /*!< Send feedback to user. */
        CONSOLE_Print("%s%d\r\n",cmdList[2],(short)param[0]);
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
 * @brief Displays the temperature.
 *
 * @return None.
*******************************************************************************/
void GetTemperature(double* param, char paramNo) /*!< "temperature?" command */
{
    float temperature = 0; /*!< Last read value. */

    if(measureSetting)
    {
         /*!< Read temperature from the device */
        temperature = ADXL362_ReadTemperature();
        /*!< Send the requested value to user */
        CONSOLE_Print("temperature=%.2f [C]\r\n",temperature);
    }
    else
    {
        CONSOLE_Print("Device is in Standby mode. Use the 'measure=1' command \
to enter Measurement mode.\r\n");
    }
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) /*!< "reset!" command */
{
   /*!< Resets the device(software reset). */
    measureSetting = 0;
    ADXL362_SoftwareReset();

    /*!< Send feedback to user */
    CONSOLE_Print("Device was reset.\r\n");
}

/***************************************************************************//**
 * @brief Displays the accelerations on XYZ axes.
 *
 * @return None.
*******************************************************************************/
void GetAcceleration(double* param, char paramNo) /*!< "acceleration?" command */
{
unsigned char samplesNumber = 0;
float         xAxisAcc      = 0;
float         yAxisAcc      = 0;
float         zAxisAcc      = 0;

    if(measureSetting)
    {
        /*!< Read and displays 30 samples from the accelerometer. */
        samplesNumber = 30;
        while(samplesNumber)
        {
            /*!< Read the requested value from the device. */
            ADXL362_GetGxyz(&xAxisAcc, &yAxisAcc, &zAxisAcc);
            /*!< Send the requested value to user. */
            CONSOLE_Print("x axis:%.3f\r\n",xAxisAcc);
            CONSOLE_Print("y axis:%.3f\r\n",yAxisAcc);
            CONSOLE_Print("z axis:%.3f\r\n",zAxisAcc);

            TIME_DelayMs(200);
            samplesNumber--;
        }
    }
    else
    {
        CONSOLE_Print("Device is in Standby mode. Use the 'measure=1' command \
to enter Measurement mode.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the acceleration on X axis.
 *
 * @return None.
*******************************************************************************/
void GetAccelerationX(double* param, char paramNo) /*!< "accelerationX?" command */
{
unsigned char samplesNumber = 0;
float         xAxisAcc      = 0;
float         yAxisAcc      = 0;
float         zAxisAcc      = 0;

    if(measureSetting)
    {
        /*!< Read and displays 30 samples from the accelerometer. */
        samplesNumber = 30;
        while(samplesNumber)
        {
            /*!< Read the requested value from the device. */
            ADXL362_GetGxyz(&xAxisAcc, &yAxisAcc, &zAxisAcc);
            /*!< Send the requested value to user. */
            CONSOLE_Print("x axis:%.3f\r\n",xAxisAcc);

            TIME_DelayMs(200);
            samplesNumber--;
        }
    }
    else
    {
        CONSOLE_Print("Device is in Standby mode. Use the 'measure=1' command \
to enter Measurement mode.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the acceleration on Y axis.
 *
 * @return None.
*******************************************************************************/
void GetAccelerationY(double* param, char paramNo) /*!< "accelerationY?" command */
{
unsigned char samplesNumber = 0;
float         xAxisAcc      = 0;
float         yAxisAcc      = 0;
float         zAxisAcc      = 0;

    if(measureSetting)
    {
        /*!< Read and displays 30 samples from the accelerometer. */
        samplesNumber = 30;
        while(samplesNumber)
        {
            /*!< Read the requested value from the device. */
            ADXL362_GetGxyz(&xAxisAcc, &yAxisAcc, &zAxisAcc);
            /*!< Send the requested value to user. */
            CONSOLE_Print("y axis:%.3f\r\n",yAxisAcc);

            TIME_DelayMs(200);
            samplesNumber--;
        }
    }
    else
    {
        CONSOLE_Print("Device is in Standby mode. Use the 'measure=1' command \
to enter Measurement mode.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the acceleration on Z axis.
 *
 * @return None.
*******************************************************************************/
void GetAccelerationZ(double* param, char paramNo) /*!< "accelerationZ?" command */
{
unsigned char samplesNumber = 0;
float         xAxisAcc      = 0;
float         yAxisAcc      = 0;
float         zAxisAcc      = 0;

    if(measureSetting)
    {
        /*!< Read and displays 30 samples from the accelerometer. */
        samplesNumber = 30;
        while(samplesNumber)
        {
            /*!< Read the requested value from the device. */
            ADXL362_GetGxyz(&xAxisAcc, &yAxisAcc, &zAxisAcc);
            /*!< Send the requested value to user. */
            CONSOLE_Print("z axis:%.3f\r\n",zAxisAcc);

            TIME_DelayMs(200);
            samplesNumber--;
        }
    }
    else
    {
        CONSOLE_Print("Device is in Standby mode. Use the 'measure=1' command \
to enter Measurement mode.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the activity status of the device. 
 *
 * @return None.
*******************************************************************************/
void GetActivity(double* param, char paramNo) /*!< "activity?" command */
{
    unsigned char detections = 0;
    unsigned char regVal     = 0;

    if(measureSetting)
    {
        /*!< Setup the activity and inactivity detection. */
        CONSOLE_Print("Activity polling enabled.\r\n");

        ADXL362_SetPowerMode(0);
        ADXL362_SetOutputRate(ADXL362_ODR_100_HZ);
        ADXL362_SetRegisterValue(
                      ADXL362_ACT_INACT_CTL_LINKLOOP(ADXL362_MODE_LINK),
                      ADXL362_REG_ACT_INACT_CTL,
                      1);
        ADXL362_SetupActivityDetection(1, 60, 4);
        ADXL362_SetupInactivityDetection(1, 700, 250);
        ADXL362_SetPowerMode(1);
        /*!< Clear ACT and INACT bits by reading the Status Register. */
        ADXL362_GetRegisterValue(&regVal,
                                 ADXL362_REG_STATUS,
                                 1);
        /*!< Exit polling after 5 detections. */
        detections = 5;
        while(detections)
        {
            do /*!< Wait for the detection of an activity or inactivity. */
            {
                ADXL362_GetRegisterValue(&regVal,
                                         ADXL362_REG_STATUS,
                                         1);
            }while(!(regVal & ADXL362_STATUS_ACT) &&
                   !(regVal & ADXL362_STATUS_INACT));
            detections--;
            if(regVal & ADXL362_STATUS_ACT)
            {
                CONSOLE_Print("Activity\r\n");
            }
            if(regVal & ADXL362_STATUS_INACT)
            {
                CONSOLE_Print("Inactivity\r\n");
            }
        }
        CONSOLE_Print("Finished activity polling.\r\n");
    }
    else
    {
        CONSOLE_Print("Device is in Standby mode. Use the 'measure=1' command \
to enter Measurement mode.\r\n");
    }
}
