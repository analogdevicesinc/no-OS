/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodDA5.
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
#include "AD5781.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/*!< List of available commands */
const char* cmdList[] ={"help?",
                        "reset!",
                        "coding=",
                        "coding?",
                        "register=",
                        "register?",
                        "voltage=",
                        "voltage?",
                        "output=",
                        "output?",
                        "rbuf=",
                        "rbuf?"};
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Resets the AD5781 device.",
"  -  Selects the coding style. Accepted values: \r\n\
\t0 - Offset binary coding. \r\n\
\t1 - Two's complement coding.(default)",
"  -  Display the current coding style.",
"  -  Writes to the DAC register. Accepted values: \r\n\
\t0 .. 262143 - the value written to the DAC.",
"  -  Displays last written value to the DAC register.",
"  -  Sets the DAC output voltage. Accepted values: \r\n\
\t-10 .. +10 - desired output voltage in volts.",
"  -  Displays the output voltage.",
"  -  Selects the DAC output state. Accepted values:\r\n\
\t0 - Normal state.\r\n\
\t1 - Clamped via 6KOhm to AGND.\r\n\
\t2 - Tristate.(default)",
"  -  Displays the DAC output state.",
"  -  Sets/resets the RBUF bit from control register. Accepted values:\r\n\
\t0 - RBUF is reset.\r\n\
\t1 - RBUF is set.(default)",
"  -  Displays the value of RBUF bit from control register."};
const char* cmdExample[] = {
"",
"",
"To select two's complement coding, type: coding=1",
"",
"To set the register value to 100000, type: register=100000",
"",
"To set the output voltage to 5V, type: voltage=5",
"",
"To set the DAC output state to normal, type: output=0",
"",
"To set the RBUF bit to 0, type: rbuf=0",
""};
const char          cmdNo     = (sizeof(cmdList) / sizeof(const char*));
const char          VREFN     = -10;
const char          VREFP     = 10;
const unsigned long FULLSCALE = 262144;

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[12] = {GetHelp, DoReset, SetCoding, GetCoding,
                               SetRegister, GetRegister, SetVoltage, GetVoltage,
                               SetOutput, GetOutput, SetRbuf, GetRbuf};
                               
/*!< Variables holding information about the device */
unsigned long registerValue = 0; /*!< Last register value written to the device */
long          codingStyle   = 1; /*!< Selected coding style */
unsigned char output        = 1; /*!< Selected output state */
unsigned char rbuf          = 1; /*!< RBUF bit */

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
    if(AD5781_Init() == 0)
    {
        CONSOLE_Print("AD5781 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5781 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) /*!< "reset!" command */
{
    /*!< Resets the device to its power-on state. */
    AD5781_SoftInstruction(AD5781_SOFT_CTRL_RESET);
    codingStyle   = 1;
    registerValue = 0;
    output        = 1;
    rbuf          = 1;
    /*!< Send feedback to user */
    CONSOLE_Print("Device was reset.\r\n");
}

/***************************************************************************//**
 * @brief Selects the coding style.
 *
 * @return None.
*******************************************************************************/
void SetCoding(double* param, char paramNo) /*!< "coding=" command */
{
    double         volt     = 0;
    unsigned long  regValue = 0;
    
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
        codingStyle = (char)param[0];

        if(codingStyle == 1)
        {
            AD5781_Setup(0);
            /*!< Write to DAC register */
            AD5781_SetDacValue(registerValue);
            regValue = registerValue;
            regValue = (regValue & 0x20000) ?
                       (regValue & ~0x20000) :
                       (regValue | 0x20000);
            /*!< Send feedback to user for coding change */
            CONSOLE_Print("%s%d(two's complement)\r\n",cmdList[2],codingStyle);
        }
        else
        {
            AD5781_Setup(AD5781_CTRL_BIN2SC);
            /*!< Write to DAC register */
            AD5781_SetDacValue(registerValue);
            regValue = registerValue;
            /*!< Send feedback to user for coding change */
            CONSOLE_Print("%s%d(offset binary)\r\n",cmdList[2], codingStyle);
        }
        volt = ((float)((VREFP - VREFN) * regValue) / (FULLSCALE - 1)) + VREFN ;
        /*!< Send the requested value to user for implicit voltage change */
        CONSOLE_Print("voltage=%.3f [V]\r\n",volt);
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
 * @brief Display the current coding style.
 *
 * @return None.
*******************************************************************************/
void GetCoding(double* param, char paramNo) /*!< "coding?" command */
{
   /*!< Send the requested value to user */
   if (codingStyle == 0)
   {
        CONSOLE_Print("coding=%d(offset binary)\r\n", codingStyle);
   }
   else
   {
        CONSOLE_Print("coding=%d(two's complement)\r\n", codingStyle);
   }
}

/***************************************************************************//**
 * @brief Writes to the DAC register.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) /*!< "register=" command */
{
    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 262143)
        {
            param[0] = 262143;
        }
        registerValue = (long)param[0];
        /*!< Write to DAC register */
        AD5781_SetDacValue(registerValue);
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",cmdList[4],registerValue);
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
 * @brief Displays last written value to the DAC register.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",cmdList[4],registerValue);
}

/***************************************************************************//**
 * @brief Sets the DAC output voltage.
 *
 * @return None.
*******************************************************************************/
void SetVoltage(double* param, char paramNo) /*!< "voltage=" command */
{
    double volt = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < -10)
        {
            param[0] = -10;
        }
        if(param[0] > 10)
        {
            param[0] = 10;
        }
        volt = (float)param[0];

        registerValue = (unsigned long)((volt - VREFN) * (FULLSCALE - 1) 
                         / (VREFP - VREFN) + 0.5);
        /*!< Computes the actual voltage that will be set. */
        volt = ((float)((VREFP - VREFN) * registerValue) / (FULLSCALE - 1))
               + VREFN ;
        if (codingStyle == 1)
        {
            if(volt < 0)
            {
                registerValue += 0x20000;
            }
            else
            {
                registerValue -= 0x20000;
            }
        }
        /*!< Write to DAC register */
        AD5781_SetDacValue(registerValue);
        /*!< Send feedback to user */
        CONSOLE_Print("%s%.3f [V]\r\n",cmdList[6], volt);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[6], (char*)cmdDescription[6]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[6]);
    }
}

/***************************************************************************//**
 * @brief Displays the output voltage.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    double         volt     = 0;
    unsigned long  regValue = 0;

    /*!< Get the output voltage using data from regValue */
    regValue = registerValue;
    if (codingStyle == 1)
    {
        regValue = (regValue & 0x20000) ?
                   (regValue & ~0x20000) :
                   (regValue | 0x20000);
    }
    volt = ((float)((VREFP - VREFN) * regValue) / (FULLSCALE - 1)) + VREFN ;
    /*!< Send the requested value to user */
    CONSOLE_Print("voltage=%.3f [V]\r\n",volt);
}

/***************************************************************************//**
 * @brief Selects the DAC output state.
 *
 * @return None.
*******************************************************************************/
void SetOutput(double* param, char paramNo) /*!< "output=" command */
{
    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 2)
        {
            param[0] = 2;
        }
        output = (unsigned char)param[0];
        /*!< Set the DAC output to the selected operating mode. */
        AD5781_DacOuputState(output);
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",cmdList[8],output);
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
 * @brief Displays the DAC output state.
 *
 * @return None.
*******************************************************************************/
void GetOutput(double* param, char paramNo) /*!< "output?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",cmdList[8],output);

}

/***************************************************************************//**
 * @brief Sets/resets the RBUF bit from control register.
 *
 * @return None.
*******************************************************************************/
void SetRbuf(double* param, char paramNo) /*!< "rbuf=" command */
{
    long regValue = 0;

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
        rbuf = (unsigned char)param[0];

        regValue = AD5781_GetRegisterValue(AD5781_REG_CTRL);
        regValue &= ~AD5781_CTRL_RBUF;
        regValue |= rbuf * AD5781_CTRL_RBUF;
        regValue = AD5781_SetRegisterValue(AD5781_REG_CTRL,regValue);

        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[10],rbuf);
    }
      else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[10], (char*)cmdDescription[10]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[10]);
    }
}

/***************************************************************************//**
 * @brief Displays the value of RBUF bit from control register.
 *
 * @return None.
*******************************************************************************/
void GetRbuf(double* param, char paramNo) /*!< "rbuf?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",cmdList[10],rbuf);

}