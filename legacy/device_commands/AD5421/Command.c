/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5421.
 *   @author Lucian Sin (Lucian.Sin@analog.com)
 *
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
#include "Console.h"
#include "AD5421.h"
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
        .name = "reset!",
        .description = "Resets the AD5421 device.",
        .acceptedValue = "",
        .example = "",
    },
    [2] = {
        .name = "current=",
        .description = "Sets the output current.",
        .acceptedValue = "Accepted values: \r\n\
\t4 .. 20 - the desired output current in milliamps.",
        .example = "To set the output current to 7.5mA, type: current=7.5",
    },
    [3] = {
        .name = "current?",
        .description = "Displays the output current.",
        .acceptedValue = "",
        .example = "",
    },
    [4] = {
        .name = "register=",
        .description = "Writes to the DAC register.",
        .acceptedValue = "Accepted values: \r\n\
\t0 .. 65535 - the value written to the DAC.",
        .example = "To set the DAC register value to 20000, \
type: register=20000",
    },
    [5] = {
        .name = "register?",
        .description = "Displays the last written value to the DAC register.",
        .acceptedValue = "",
        .example = "",
    },
    [6] = {
        .name = "offset=",
        .description = "Sets the offset.",
        .acceptedValue = "Accepted values: \r\n\
\t -32768 .. +32767 - digital offset adjustment(LSBs)",
        .example = "To set the Offset register value to -10000, \
type: offset=-10000",
    },
    [7] = {
        .name = "offset?",
        .description = "Displays the offset.",
        .acceptedValue = "",
        .example = "",
    },
    [8] = {
        .name = "gain=",
        .description = "Sets the gain.",
        .acceptedValue = "Accepted values: \r\n\
\t-65535 .. 0 - digital gain adjustment(LSBs)",
        .example = "To set the Gain register value to -30000, \
type: gain=-30000",
    },
    [9] = {
        .name = "gain?",
        .description = "Displays the gain.",
        .acceptedValue = "",
        .example = "",
    },
    [10] = {
        .name = "temp?",
        .description = "Displays the die temperature.",
        .acceptedValue = "",
        .example = "",
    },
    [11] = {
        .name = "vloop?",
        .description = "Displays the Vloop - COM voltage.",
        .acceptedValue = "",
        .example = "",
    },
    [12] = {
        .name = "faultReg?",
        .description = "Displays the Fault register.",
        .acceptedValue = "",
        .example = "",
    }
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[13] = {GetHelp, DoReset, SetCurrent, GetCurrent,
                               SetRegister, GetRegister, SetOffset, GetOffset,
                               SetGain, GetGain, GetTemp, GetVloop, GetFaultReg};

/* Variables holding information about the device */
int dacReg    = 0; // Content of DAC register
int offsetReg = 0; // Content of Offset Adjust register
int gainReg   = 0; // Content of Gain Adjust register

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
    if(AD5421_Init() == 0)
    {
        CONSOLE_Print("AD5421 OK\r\n");
        dacReg    = AD5421_GetDac();
        offsetReg = AD5421_GetOffset();
        gainReg   = AD5421_GetGain();
        DisplayCmdList();
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5421 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) // "reset!" command
{
    int spiData  = 0;

    AD5421_Reset();
    dacReg    = 0;
    offsetReg = 32768;
    gainReg   = 65535;
    /* Setup AD5421 control register. */
    /* Write to the control register. */
    spiData = AD5421_CMD(AD5421_CMDWRCTRL);
    /* Set certain bits. */
    spiData |= (CTRL_AUTO_FAULT_RDBK |
                CTRL_SEL_ADC_INPUT   |
                CTRL_ONCHIP_ADC      |
                CTRL_SPI_WATCHDOG);
    /* Send data via SPI. */
    AD5421_Set(&spiData);
    TIME_DelayUs(100);
    /* Send feedback to user */
    CONSOLE_Print("Device was reset.\r\n");
}

/***************************************************************************//**
 * @brief Writes to the DAC register.
 *
 * @param param[0] - desired value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) // "register=" command
{
    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 65535)
        {
            param[0] = 65535;
        }
        dacReg = (int)param[0];
        /* Write to DAC register */
        AD5421_SetDac(dacReg);
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",cmdList[4].name, dacReg);
    }
    else
    {
        /* Display error messages */
        DisplayError(4);
    }
}

/***************************************************************************//**
 * @brief Displays last written value to the DAC register.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) // "register?" command
{
    dacReg = AD5421_GetDac();
    /* Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",cmdList[4].name, dacReg);
}

/***************************************************************************//**
 * @brief Writes to the Offset Adjust register.
 *
 * @param param[0] - desired value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetOffset(double* param, char paramNo) // "offset=" command
{
    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < -32768)
        {
            param[0] = -32768;
        }
        if(param[0] > 32767)
        {
            param[0] = 32767;
        }
        offsetReg = (int)param[0] + 32768;
        /* Write to Offset Adjust register */
        AD5421_SetOffset(offsetReg);
        /* Write to DAC register in order to validate the Offset register value change */
        AD5421_SetDac(dacReg);
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n", cmdList[6].name, offsetReg - 32768);
    }
    else
    {
        /* Display error messages */
        DisplayError(6);
    }
}

/***************************************************************************//**
 * @brief Displays last written value to the Offset Adjust register.
 *
 * @return None.
*******************************************************************************/
void GetOffset(double* param, char paramNo) // "offset?" command
{
    int value = 0;

    offsetReg = AD5421_GetOffset();
    /* Calculate offset according to the datasheet formula. */
    value = offsetReg - 32768;
    /* Send the requested value to user */
    CONSOLE_Print("%s%d [LSBs]\r\n", cmdList[6].name, value);
}


/***************************************************************************//**
 * @brief Writes to the Gain Adjust register.
 *
 * @param param[0] - desired value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetGain(double* param, char paramNo) // "gain=" command
{
    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < -65535)
        {
            param[0] = -65535;
        }
        if(param[0] > 0)
        {
            param[0] = 0;
        }
        gainReg = (int)param[0] + 65535;
        /* Write to Gain Adjust register */
        AD5421_SetGain(gainReg);
        /* Write to DAC register in order to validate the Gain register value change */
        AD5421_SetDac(dacReg);
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n", cmdList[8].name, gainReg - 65535);
    }
    else
    {
        /* Display error messages */
        DisplayError(8);
    }
}

/***************************************************************************//**
 * @brief Displays last gain value from the Gain Adjust register.
 *
 * @return None.
*******************************************************************************/
void GetGain(double* param, char paramNo) // "gain?" command
{
    int value = 0;

    gainReg = AD5421_GetGain();
    /* Calculate according to the datasheet formula. */
    value = gainReg - 65535;
    /* Send the requested value to user */
    CONSOLE_Print("%s%d [LSBs]\r\n", cmdList[8].name, value);
}

/***************************************************************************//**
 * @brief Displays the die temperature.
 *
 * @return None.
*******************************************************************************/
void GetTemp(double* param, char paramNo) // "temp?" command
{
    int value = 0;

    value = AD5421_GetTemp();
    /* Send the requested value to user */
    CONSOLE_Print("temperature=%d [C]\r\n", value);
}

/***************************************************************************//**
 * @brief Displays the Vloop - COM voltage.
 *
 * @return None.
*******************************************************************************/
void GetVloop(double* param, char paramNo) // "vloop?" command
{
    float value = 0;

    value = AD5421_GetVloop();
    /* Send the requested value to user */
    CONSOLE_Print("Vloop - COM = %.3f [V]\r\n", value);
}

/***************************************************************************//**
 * @brief Displays the Fault register.
 *
 * @return None.
*******************************************************************************/
void GetFaultReg(double* param, char paramNo) // "faultReg?" command
{
    int value = 0;

    value = AD5421_GetFault();
    /* Send the requested value to user */
    CONSOLE_Print("Fault register = 0x%x\r\n", value);
}

/***************************************************************************//**
 * @brief Displays the output current in milliamps.
 *
 * @return None.
*******************************************************************************/
void GetCurrent(double* param, char paramNo) // "current?" command
{
    float value = 0;

    /* Datasheet formula */
    value = (16 * ((float)gainReg / 65536) * ((float)dacReg / 65536)) +
            4 + (16 * ((float)offsetReg - 32768) / 65536);
     /* Establish the current limits in software, in hardware is already done */
    if (value < 4)
    {
        value = 4;
    }
    else if (value > 20)
    {
        value = 20;
    }
    /* Send the requested value to user */
    CONSOLE_Print("%s%.3f [mA]\r\n", cmdList[2].name, value);
}

/***************************************************************************//**
 * @brief Sets the output current.
 *
 * @param param[0] - desired value to be set in milliamps.
 *
 * @return None.
*******************************************************************************/
void SetCurrent(double* param, char paramNo) // "current=" command
{
    float floatValue = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 4)
        {
            param[0] = 4;
        }
        if(param[0] > 20)
        {
            param[0] = 20;
        }
        /* Datasheet formula */
        floatValue = ((float)param[0] - (4 + (16 * ((float)offsetReg - 32768)) /
                      65536)) * 65536 * 65536 / (16 * (float)gainReg);
        /* Verify the limits of the value to be written in DAC register */
        if (floatValue > 65535)
        {
            floatValue = 65535;
        }
        else if (floatValue < 0)
        {
            floatValue = 0;
        }

        dacReg = (int)floatValue;
        /* Write to DAC register */
        AD5421_SetDac(dacReg);
        /* Datasheet formula */
        floatValue = (16 * ((float)gainReg / 65536) * ((float)dacReg / 65536))
                    + 4 + (16 * ((float)offsetReg - 32768) / 65536);
        /* Establish the current limits in software, in hardware is already done */
        if (floatValue < 4)
        {
            floatValue = 4;
        }
        else if (floatValue > 20)
        {
            floatValue = 20;
        }
        /* Send the requested value to user */
        CONSOLE_Print("%s%.3f [mA]\r\n", cmdList[2].name, floatValue);
    }
    else
    {
        /* Display error messages */
        DisplayError(2);
    }
}
