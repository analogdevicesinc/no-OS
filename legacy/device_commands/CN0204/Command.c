/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for CN0204.
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
#include "ad5662.h"
#include "ad5751.h"
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
        .name = "register=",
        .description = "Write a value to the DAC register.",
        .acceptedValue = "Accepted values:\r\n\
\t0 .. 65535 - value to be written in register.",
        .example = "To write 20000 to DAC register, type: register=20000",
    },
    [2] = {
        .name = "register?",
        .description = "Displays the last value written to the DAC register.",
        .acceptedValue = "",
        .example = "",
    },
    [3] = {
        .name = "ad5751clrPin=",
        .description = "Sets the output value of CLR pin.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - sets the CLR pin low.(default)\r\n\
\t1 - sets the CLR pin high",
        .example = "To set the CLR pin high, type: ad5751clrPin=1",
    },
    [4] = {
        .name = "ad5751clrPin?",
        .description = "Displays the output value of CLR pin.",
        .acceptedValue = "",
        .example = "",
    },
    [5] = {
        .name = "addressA0=",
        .description = "Sets the value of A0 address bit(JP1).",
        .acceptedValue = "Accepted values:\r\n\
\t0 - address is 0b000.(default)\r\n\
\t1 - address is 0b001.",
        .example = "To set the ad5751 address bit to 0b001, type: addressA0=1",
    },
    [6] = {
        .name = "addressA0?",
        .description = "Displays the value of A0 address bit(JP1).",
        .acceptedValue = "",
        .example = "",
    },
    [7] = {
        .name = "range=",
        .description = "Sets the output range for ad5751.",
        .acceptedValue = "Accepted values:r\n\
\t0 -> 0V to 5V.\r\n\
\t1 -> 0V to 10V.\r\n\
\t2 -> 0V to 6V.\r\n\
\t3 -> 0V to 12V.\r\n\
\t4 -> 0V to 40V.\r\n\
\t5 -> 0V to 44V.\r\n\
\t6 -> 4mA to 20mA(external).\r\n\
\t7 -> 4mA to 20mA(internal).\r\n\
\t8 -> 0mA to 20mA(external).\r\n\
\t9 -> 0mA to 20mA(internal).\r\n\
\t10 -> 0mA to 24mA(external).\r\n\
\t11 -> 0mA to 24mA(internal).\r\n\
\t12 -> 3.92mA to 20.4mA(internal).\r\n\
\t13 -> 0mA to 20.4mA(internal).\r\n\
\t14 -> 0mA to 24.5mA(internal).",
        .example = "To set the output range to 0V to 10V, type: range=2",
    },
   [8] = {
        .name = "range?",
        .description = "Displays the current output range.",
        .acceptedValue = "",
        .example = "",
    },
    [9] = {
        .name = "fault?",
        .description = "Displays the list of possible faults.",
        .acceptedValue = "",
        .example = "",
    },
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

const unsigned long MAX_DAC_VALUE = 0xFFFF;
extern const unsigned char RANGE_CONFIG[22];

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp, SetRegisterDac, GetRegisterDac,
                              Setad5751clrPin,Getad5751clrPin,Setad5751AddressA0,
                              Getad5751AddressA0,Setad5751Range,Getad5751Range,
                              Getad5751Fault};

/* Variables holding information about the device */
unsigned char clr      = 0;
unsigned char bitA0    = 0;
unsigned char rangeVal = 0;
unsigned long dacValue = 0;

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/

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
    if(ad5751_Init() == 0)
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

/***************************************************************************//*
 * @brief Sets the shift register of the DAC with the given value.
 *
 * @param The value which will be written to the shift register.
 *
 * @return None.
******************************************************************************/
void SetRegisterDac(double* param, char paramNo) // "register=" command
{
    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > MAX_DAC_VALUE)
        {
            param[0] = MAX_DAC_VALUE;
        }

        dacValue = (unsigned long)param[0];
        dacValue |= AD5662_LOAD << 16;  // normal operation
        ad5662_Write24Bits(dacValue);
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[1].name, dacValue);
    }
    else
    {
        /* Display error messages */
        DisplayError(1);
    }
}

/***************************************************************************//**
 * @brief Displays the last value written to the DAC register.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetRegisterDac(double* param, char paramNo) // "register?" command
{
    /* Send requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[1].name, dacValue);
}

/***************************************************************************//**
 * @brief Sets the output value of ad5751 CLR pin.
 *
 * @param param[0] - value to be set for CLR pin.
 *
 * @return None.
*******************************************************************************/
void Setad5751clrPin(double* param, char paramNo) // "ad5751clrPin=" command
{
    /* Check if the parameter is valid */
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
        clr = (unsigned char)param[0];
        if (clr == 1)
        {
            ad5751_CLR_HIGH;
        }
        else
        {
            ad5751_CLR_LOW;
        }
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[3].name, clr);
    }
    else
    {
        /* Display error messages */
        DisplayError(3);
    }
}

/***************************************************************************//**
 * @brief Displays the value of ad5751 CLR pin.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void Getad5751clrPin(double* param, char paramNo) // "ad5751clrPin?" command
{
    /* Send requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[3].name, clr);
}

/***************************************************************************//**
 * @brief Sets the A0 address bit from ad5751(from JP1 on the board).
 *
 * @param param[0] - value to be set for A0 bit.
 *
 * @return None.
*******************************************************************************/
void Setad5751AddressA0(double* param, char paramNo) //"addressA0=" command
{
    /* Check if the parameter is valid */
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
        bitA0 = (unsigned char)param[0];
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[5].name, bitA0);
    }
    else
    {
        /* Display error messages */
        DisplayError(5);
    }
}

/***************************************************************************//**
 * @brief Displays the value of A0 address bit from ad5751.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void Getad5751AddressA0(double* param, char paramNo) //"addressA0?" command
{
    /* Send feedback to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[5].name, bitA0);
}

/***************************************************************************//**
 * @brief Sets the value of ad5751 output range.
 *
 * @param param[0] - value to be set for output range.
 *
 * @return None.
*******************************************************************************/
void Setad5751Range(double* param, char paramNo) //"range=" command
{
    unsigned short serialWord = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 14)
        {
            param[0] = 14;
        }
        rangeVal = (unsigned char)param[0];
        serialWord = ad5751_CONTROL_ADDR(bitA0) +
                     ad5751_CONTROL_WRITE +
                     ad5751_CONTROL_RANGE(RANGE_CONFIG[rangeVal] & 0xF) +
                     ad5751_CONTROL_OUTEN +
                     ((RANGE_CONFIG[rangeVal] & 0x10) >> 1);
        ad5751_Write16Bits(serialWord);
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[7].name, rangeVal);
    }
    else
    {
        /* Display error messages */
        DisplayError(6);
    }
}

/***************************************************************************//**
 * @brief Displays the value of ad5751 output range.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void Getad5751Range(double* param, char paramNo) //"range?" command
{
    /* Send feedback to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[7].name, rangeVal);
}

/***************************************************************************//**
 * @brief Displays the list of possible faults.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void Getad5751Fault(double* param, char paramNo) //"fault?" command
{
    unsigned short serialWord     = 0;
    unsigned short statusRegister = 0;
    unsigned char  pecError       = 0;
    unsigned char  overTemp       = 0;
    unsigned char  iOutFault      = 0;
    unsigned char  vOutFault      = 0;

    serialWord = ad5751_STATUS_ADDR(bitA0) + ad5751_STATUS_READ;
    statusRegister = ad5751_Read16Bits(serialWord);
    pecError   = (statusRegister & ad5751_STATUS_PEC_ERR)    >> 3;
    overTemp   = (statusRegister & ad5751_STATUS_OVER_TEMP)  >> 2;
    iOutFault  = (statusRegister & ad5751_STATUS_IOUT_FAULT) >> 1;
    vOutFault  = (statusRegister & ad5751_STATUS_VOUT_FAULT) >> 0;

    /* Send feedback to user */
    /* "Error = 0" means there is no error. */
    /* "Error = 1" means there is an error. */
    CONSOLE_Print("PEC Error=%d\r\n", pecError);
    CONSOLE_Print("OVERTEMP Error=%d\r\n", overTemp);
    CONSOLE_Print("IOUT Fault=%d\r\n", iOutFault);
    CONSOLE_Print("VOUT Fault=%d\r\n", vOutFault);
}
