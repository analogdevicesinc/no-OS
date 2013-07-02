/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the program's main function.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Command.h"
#include "ADP5589.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/*! List of available commands */
const char* cmdList[] = {"help?",
                         "pwmOnTime=",
                         "pwmOffTime=",
                         "keyPressed?",
                         "keyReleased?"
                        };
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  On-time of the PWM pulse. Accepted values: \r\n\
\t1 .. 65535[us] - time when the signal is high.",
"  -  Off-time of the PWM pulse. Accepted values: \r\n\
\t1 .. 65535[us] - time when the signal is low.",
"  -  Displays the last pressed key.",
"  -  Displays the last released key."};
const char* cmdExample[] = {"",
                            "To set pe PWM on-time to 1000us, type pwmOnTime=1000.",
                            "To set pe PWM off-time to 40000us, type pwmOffTime=40000.",
                            "",
                            "",
                            };

const char cmdNo = (sizeof(cmdList) / sizeof(const char*));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[5] = {GetHelp, SetPwmOnTime, SetPwmOffTime,
                               GetKeyPressed, GetKeyReleased};
unsigned short pwmOnTime  = 1;
unsigned short pwmOffTime = 1;
char           lastKeyR   = 0;
char           lastKeyP   = 0;
char           keyR       = 0;
char           keyP       = 0;

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
    if(ADP5589_Init() == 0)
    {
        CONSOLE_Print("ADP5589 OK\r\n");
        GetHelp(NULL, 0);
        ADP5589_InitKey(PMOD_IOXP_J2);	/*!< Init ADP5589 as keyboard decoder */
        ADP5589_InitPwm();              /*!< Init PWM on pin R3 */
        ADP5589_SetPwm(1,1);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("ADP5589 Error\r\n");

        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Sets the PWM on-time; PWM is mapped on pin R3.
 *
 * @param param[0] - set power-on time in us;
 *        paramNo  - number of parameters (must be >=1); only the first parameter is taken into account;
 *
 * @return None.
*******************************************************************************/
void SetPwmOnTime(double* param, char paramNo) /*!< "pwmOnTime=" command */
{
    if(paramNo >= 1)
    {
        if(param[0] < 1)
        {
            param[0] = 1;
        }
        if(param[0] > 65535)
        {
            param[0] = 65535;
        }
        pwmOnTime = (unsigned short)param[0];
        /*!< Configure the PWM on/off registers */
        ADP5589_SetPwm(pwmOffTime, pwmOnTime);
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d \r\n",(char*)cmdList[1],pwmOnTime);
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
 * @brief Sets the PWM off-time; PWM is mapped on pin R3.
 *
 * @param param[0] - set power-on time in us;
 *        paramNo  - number of parameters (must be >=1); only the first parameter is taken into account;
 *
 * @return None.
*******************************************************************************/
void SetPwmOffTime(double* param, char paramNo) /*!< "pwmOffTime=" command */
{
    if(paramNo >= 1)
    {
        if(param[0] < 1)
        {
            param[0] = 1;
        }
        if(param[0] > 65535)
        {
            param[0] = 65535;
        }
        pwmOffTime = (unsigned short)param[0];
        /*!< Configure the PWM on/off registers */
        ADP5589_SetPwm(pwmOffTime, pwmOnTime);
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d \r\n",(char*)cmdList[2],pwmOffTime);
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
 * @brief Displays the last pressed key.
 *
 * @return None.
*******************************************************************************/
void GetKeyPressed(double* param, char paramNo) /*!< "keyPressed?" command */
{
    unsigned char statusReg  = 0;
    unsigned char eventCount = 0;
    unsigned char eventIndex = 0;
    unsigned char event      = 0;

    /*!< Determine the FIFO events number */
    statusReg = ADP5589_GetRegisterValue(ADP5589_ADR_STATUS);
    eventCount = (statusReg & ADP5589_STATUS_EC(0x1F));
    /*!< Verify if a key press or key release took place */
    for(eventIndex = 0; eventIndex < eventCount; eventIndex++)
    {
        event = ADP5589_GetRegisterValue(ADP5589_ADR_FIFO1);
        keyP  = ADP5589_KeyDecode(event,ADP5589_EVENT_KEY_PRESSED,PMOD_IOXP_J2);
        keyR  = ADP5589_KeyDecode(event,ADP5589_EVENT_KEY_RELEASED,PMOD_IOXP_J2);
        if (keyP != 'x')
        {
            lastKeyP = keyP;
        }
        if (keyR != 'x')
        {
            lastKeyR = keyR;
        }
    }
    /*!< Send feedback to user */
    if (lastKeyP == 0)
    {
         CONSOLE_Print("Last key pressed: none. \r\n");
    }
    else
    CONSOLE_Print("Last key pressed: %c. \r\n",lastKeyP);
}

/***************************************************************************//**
 * @brief Displays the last released key.
 *
 * @return None.
*******************************************************************************/
void GetKeyReleased(double* param, char paramNo) /*!< "keyReleased?" command */
{
    unsigned char statusReg  = 0;
    unsigned char eventCount = 0;
    unsigned char eventIndex = 0;
    unsigned char event      = 0;

    /*!< Determine the FIFO events number */
    statusReg = ADP5589_GetRegisterValue(ADP5589_ADR_STATUS);
    eventCount = (statusReg & ADP5589_STATUS_EC(0x1F));
    /*!< Verify if a key press or key release took place */
    for(eventIndex = 0; eventIndex < eventCount; eventIndex++)
    {
        event = ADP5589_GetRegisterValue(ADP5589_ADR_FIFO1);
        keyP = ADP5589_KeyDecode(event,ADP5589_EVENT_KEY_PRESSED,PMOD_IOXP_J2);
        keyR = ADP5589_KeyDecode(event,ADP5589_EVENT_KEY_RELEASED,PMOD_IOXP_J2);
        if (keyP != 'x')
        {
            lastKeyP = keyP;
        }
        if (keyR != 'x')
        {
            lastKeyR = keyR;
        }
    }
    if (lastKeyR == 0)
    {
         CONSOLE_Print("Last key released: none. \r\n");
    }
    else
    CONSOLE_Print("Last key released: %c. \r\n",lastKeyR);
}
