/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodACL.
 *   @author Mihai Bancisor (Mihai.Bancisor@analog.com)
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
#include "ADXL345.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

/*!< List of available commands */
const char* cmdList[] = {
                        "help?",
                        "communication=",
                        "communication?",
                        "acceleration?",
                        "accelerationX?",
                        "accelerationY?",
                        "accelerationZ?",
                        "interrupts?"};

/*!< Description of available commands */
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Selects the communication interface. Accepted values:\r\n\
\t0 - SPI. \r\n\
\t1 - I2C.",
"  -  Displays the selected communication interface.",
"  -  Displays the acceleration on XYZ axis.",
"  -  Displays the acceleration on X axis.",
"  -  Displays the acceleration on Y axis.",
"  -  Displays the acceleration on Z axis.",
"  -  Displays the state of the interrupts."};

/*!< Usage example of available commands */
const char* cmdExample[] = {
"",
"To select the I2C communication port, type communication=1",
"",
"",
"",
"",
"",
""};

const char cmdNo = (sizeof(cmdList) / sizeof(const char*));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/

cmdFunction cmdFunctions[8] = {GetHelp, SetCommunication,
                               GetCommunication, GetAcceleration,
                               GetAccelerationX, GetAccelerationY,
                               GetAccelerationZ, GetInterrupts};

float          x                 = 0; /*!< X-axis's output data. */
float          y                 = 0; /*!< Y-axis's output data. */
float          z                 = 0; /*!< Z-axis's output data. */
unsigned char  intSource         = 0; /*!< Value of the ADXL345_INT_SOURCE register. */
unsigned char  communicationPort = 0; /*!< Type of communication port. */
unsigned char  adxlInitializedOk = 0; /*!< Initialization status. */
unsigned short interruptsOccured = 0; /*!< Interrupts occured. */

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
    char   receivedChar = 0;
    char   charNumber   = 0;
    double commProtocol = 0;
    char   command[10];

    CONSOLE_Print("\nPlease select the communication interface. Accepted values:\r\n\
\t0 - SPI. \r\n\
\t1 - I2C.\r\n");
    CONSOLE_Print("%s",cmdList[1]);

    /*!< Read the character entered by user through UART. */
    while((receivedChar != '\n') && (receivedChar != '\r'))
    {
        UART_ReadChar(&receivedChar);
        command[charNumber++] = receivedChar;
    }
    commProtocol = (double)(command[0] - 0x30);
    
    /*!< Initialize the device with desired interface. */
    if(ADXL345_Init(commProtocol) == 0)
    {
        SetCommunication(&commProtocol, 1);
        /*!< Display all available commands. */
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("ADXL345 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Selects the communication port for ADXL345.
 *
 * @param param[0] - selected communication port;
 *        paramNo  - number of parameters (must be >=1); only the first parameter is taken into account;
 *
 * @return None.
*******************************************************************************/
void SetCommunication(double* param, char paramNo) /*!< "communication=" command */
{
    if(paramNo >= 1)
    {
        if(param[0] == 0)
        {
            communicationPort = ADXL345_SPI_COMM;
        }
        else if(param[0] == 1)
        {
            communicationPort = ADXL345_I2C_COMM;
        }
        else
        {
            if(param[0] < 0)
            {
                communicationPort = ADXL345_SPI_COMM;
            }
            else
            {
                communicationPort = ADXL345_I2C_COMM;
            }
        }


         /*!< Send feedback to user */
        CONSOLE_Print("%s",(char*)cmdList[1]);
        if(communicationPort == ADXL345_I2C_COMM)
        {
            CONSOLE_Print("I2C\r\n");
        }
        else
        {
            CONSOLE_Print("SPI\r\n");
        }
         /*!< Initialize device */
        if(ADXL345_Init(communicationPort) == 0)
        {
            CONSOLE_Print("ADXL345 OK\r\n");
            adxlInitializedOk = 1;
        }
        else
        {
            CONSOLE_Print("ADXL345 Error\r\n");
            adxlInitializedOk = 0;
        }
        /*!< Configure device*/
        ADXL345_SetTapDetection(ADXL345_SINGLE_TAP |
                                ADXL345_DOUBLE_TAP,  /*!< Tap type. */
                                ADXL345_TAP_X_EN,    /*!< Axis control. */
                                0x64,		/*!< Tap duration.--10 */
                                0x20,		/*!< Tap latency.--10 */
                                0x40,		/*!< Tap window. */
                                0x10,		/*!< Tap threshold. */
                                0x00);		/*!< Interrupt Pin. */
        ADXL345_SetFreeFallDetection(0x01,	/*!< Free-fall detection enabled. */
                                     0x05,	/*!< Free-fall threshold. */
                                     0x14,	/*!< Time value for free-fall detection. */
                                     0x00);	/*!< Interrupt Pin. */
        ADXL345_SetPowerMode(0x1);		/*!< Measure mode. */
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
 * @brief Displays the selected communication port.
 *
 * @return None.
*******************************************************************************/
void GetCommunication(double* param, char paramNo) /*!< "communication?" command */
{
     if(adxlInitializedOk)
    {
        /*!< Send the requested value to user */
        CONSOLE_Print("%s",cmdList[1]);
        if(communicationPort == ADXL345_I2C_COMM)
        {
            CONSOLE_Print("I2C\r\n");
        }
        else
        {
            CONSOLE_Print("SPI\r\n");
        }
    }
    else
    {
        CONSOLE_Print("ADXL345 was not succesfully initialized. \
Select the ADXL345 comm. port using the 'communication=' command.\r\n ");
    }
}

/***************************************************************************//**
 * @brief Displays the acceleration on all 3 axes.
 *
 * @return None.
*******************************************************************************/
void GetAcceleration(double* param, char paramNo) /*!< "acceleration?" command */
{
unsigned short samplesNumber = 0;

    if(adxlInitializedOk)
    {
        /*!< Read and displays 30 samples from the accelerometer. */
        samplesNumber = 30;
        while(samplesNumber)
        {
            /*!< Read the requested value from the device */
            ADXL345_GetGxyz(&x,&y,&z);
            /*!< Send the requested value to user */
            CONSOLE_Print("x axis: %.3f\r\n",x);
            CONSOLE_Print("y axis: %.3f\r\n",y);
            CONSOLE_Print("z axis: %.3f\r\n",z);
            TIME_DelayMs(200);
            samplesNumber--;
        }
    }
    else
    {
        CONSOLE_Print("ADXL345 was not succesfully initialized. \
Select the ADXL345 comm. port using the 'communication=' command.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the acceleration on X axis.
 *
 * @return None.
*******************************************************************************/
void GetAccelerationX(double* param, char paramNo) /*!< "accelerationX?" command */
{
unsigned short samplesNumber = 0;

    if(adxlInitializedOk)
    {
        /*!< Read and displays 30 samples from the accelerometer. */
        samplesNumber = 30;
        while(samplesNumber)
        {
            /*!< Read the requested value from the device */
            ADXL345_GetGxyz(&x,&y,&z);
            /*!< Send the requested value to user */
            CONSOLE_Print("x axis: %.3f\r\n",x);
            TIME_DelayMs(200);
            samplesNumber--;
        }
    }
    else
    {
         CONSOLE_Print("ADXL345 was not succesfully initialized. \
Select the ADXL345 comm. port using the 'communication=' command.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the acceleration on Y axis.
 *
 * @return None.
*******************************************************************************/
void GetAccelerationY(double* param, char paramNo) /*!< "accelerationY?" command */
{
unsigned short samplesNumber = 0;

    if(adxlInitializedOk)
    {
        /*!< Read and displays 30 samples from the accelerometer. */
        samplesNumber = 30;
        while(samplesNumber)
        {
            /*!< Read the requested value from the device */
            ADXL345_GetGxyz(&x,&y,&z);
            /*!< Send the requested value to user */
            CONSOLE_Print("y axis: %.3f\r\n",y);
            TIME_DelayMs(200);
            samplesNumber--;
        }
    }
    else
    {
          CONSOLE_Print("ADXL345 was not succesfully initialized. \
Select the ADXL345 comm. port using the 'communication=' command.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the acceleration on Z axis.
 *
 * @return None.
*******************************************************************************/
void GetAccelerationZ(double* param, char paramNo) /*!< "accelerationZ?" command */
{
unsigned short samplesNumber = 0;

    if(adxlInitializedOk)
    {
        /*!< Read and displays 30 samples from the accelerometer. */
        samplesNumber = 30;
        while(samplesNumber)
        {
            /*!< Read the requested value from the device */
            ADXL345_GetGxyz(&x,&y,&z);
            /*!< Send the requested value to user */
            CONSOLE_Print("z axis: %.3f\r\n",z);
            TIME_DelayMs(200);
            samplesNumber--;
        }
    }
    else
    {
          CONSOLE_Print("ADXL345 was not succesfully initialized. \
Select the ADXL345 comm. port using the 'communication=' command.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the state of the interrupts.
 *
 * @return None.
*******************************************************************************/
void GetInterrupts(double* param, char paramNo) /*!< "interrupts?" command */
{
    if(adxlInitializedOk)
    {
        interruptsOccured = 0;
        /*!< Read the requested value from the device */
        intSource = ADXL345_GetRegisterValue(ADXL345_INT_SOURCE);
        /*!< Send the requested value to user */
        if((intSource & ADXL345_SINGLE_TAP) != 0)
        {
            CONSOLE_Print("Single Tap.\n");
            interruptsOccured = 1;
        }
        if((intSource & ADXL345_DOUBLE_TAP) != 0)
        {
            CONSOLE_Print("Double Tap.\n");
            interruptsOccured = 1;
        }
        if((intSource & ADXL345_FREE_FALL) != 0)
        {
            CONSOLE_Print("Free Fall.\n");
            interruptsOccured = 1;
        }
        if(interruptsOccured == 0)
        {
             /*!< Send feedback to user */
             CONSOLE_Print("None.\n");
        }
    }
    else
    {
          CONSOLE_Print("ADXL345 was not succesfully initialized. \
Select the ADXL345 comm. port using the 'communication=' command.\r\n");
    }

}
