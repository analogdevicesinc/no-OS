/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5791.
 *   @author Lucian Sin (Lucian.Sin@analog.com)
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
#include "AD5791.h"

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
            .description = "Resets the device.",
            .acceptedValue = "",
            .example = "",
    },
    [2] = {
            .name = "coding=",
            .description = "Selects the coding style.",
            .acceptedValue = "Accepted values:\r\n\
\t0 - Two's complement coding.(default)\r\n\
\t1 - Offset binary coding.",
            .example = "To select two's complement coding, type: coding=0",
    },
    [3] = {
            .name = "coding?",
            .description = " Display the current coding style.",
            .acceptedValue = "",
            .example = "",
    },
    [4] = {
            .name = "register=",
            .description = "Writes to the DAC register",
            .acceptedValue = "Accepted values:\r\n\
\t0 .. (2^DacResolution)-1 - the value written to the DAC.",
            .example = "To set the register value to 10000, type: \
register=10000",
    },
    [5] ={
            .name = "register?",
            .description = " Displays last written value to the DAC \
register.",
            .acceptedValue = "",
            .example = "",
    },
    [6] = {
            .name = "voltage=",
            .description = "Sets the DAC output voltage.",
            .acceptedValue = "Accepted values:\r\n\
\t-10 .. +10 - desired output voltage in volts.",
            .example = "To set the output voltage to 5V, type: voltage=5",
    },
    [7] = {
            .name =  "voltage?",
            .description = "Displays the output voltage.",
            .acceptedValue = "",
            .example = "",
    },
    [8] = {
            .name = "output=",
            .description = "Selects the DAC output state.",
            .acceptedValue =  "Accepted values:\r\n\
\t0 - Normal state.\r\n\
\t1 - Clamped via 6KOhm to AGND.(default)\r\n\
\t2 - Tristate.",
            .example = "To set the DAC output state to normal, type: output=0",
    },
    [9] = {
            .name = "output?",
            .description = "Displays the DAC output state.",
            .acceptedValue = "",
            .example = "",
    },
    [10] = {
            .name = "rbuf=",
            .description = "Sets/resets the RBUF bit from control register.",
            .acceptedValue = "Accepted values:\r\n\
\t0 - RBUF is reset.\r\n\
\t1 - RBUF is set.(default)",
            .example = "To set the RBUF bit to 0, type: rbuf=0",
    },
    [11] = {
            .name =  "rbuf?",
            .description = "Displays the value of RBUF bit from control \
register.",
            .acceptedValue = "",
            .example = "",
    }
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));
const char VREFN = 0;
const char VREFP = 10;

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[12] = {GetHelp, DoReset, SetCoding, GetCoding,
                               SetRegister, GetRegister, SetVoltage, GetVoltage,
                               SetOutput, GetOutput, SetRbuf, GetRbuf};

/* Variables holding information about the device */
AD5791_type   deviceType;
unsigned long max_value     = 0; // Maximum DAC value
unsigned long sign          = 0; // Sign bit
unsigned char offset        = 0; // Offset depending on device resolution
unsigned long registerValue = 0; // Last register value written to the device
long          codingStyle   = 0; // Selected coding style
unsigned char output        = 1; // Selected output state
unsigned char rbuf          = 1; // RBUF bit

/***************************************************************************//**
 * @brief Internal function for displaying error messages.
 *
 * @return None.
*******************************************************************************/
void DisplayError(unsigned char funcNo)
{
    /* Display error messages */
    CONSOLE_Print("Invalid parameter!\r\n");
    CONSOLE_Print("%s - %s %s\r\n", (char*)cmdList[funcNo].name, \
                              (char*)cmdList[funcNo].description,
                              (char*)cmdList[funcNo].acceptedValue);
    CONSOLE_Print("Example: %s\r\n", (char*)cmdList[funcNo].example);
}

/***************************************************************************//**
 * @brief Internal function for displaying all the commands with their description.
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
 * @brief Set the current device type.
 *
 * @return None.
*******************************************************************************/
void DoDeviceLock(void)
{
    char device = 0;
    char deviceLocked = -1;

    while(deviceLocked < 0)
    {
        CONSOLE_Print("Please specify your device.\r\n\
For AD5760 type: 0\r\n\
For AD5780 type: 1\r\n\
For AD5781 type: 2\r\n\
For AD5790 type: 3\r\n\
For AD5791 type: 4\r\n\
");
        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x34))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                    deviceType = ID_AD5760;
                    max_value = 0xFFFF;
                    sign = 0x8000;
                    offset = 4;
                    break;
                }
                case 0x31 :
                {
                    deviceType = ID_AD5780;
                    max_value = 0x3FFFF;
                    sign = 0x20000;
                    offset = 2;
                    break;
                }
                case 0x32 :
                {
                    deviceType = ID_AD5781;
                    max_value = 0x3FFFF;
                    sign = 0x20000;
                    offset = 2;
                    break;
                }
                case 0x33 :
                {
                    deviceType = ID_AD5790;
                    max_value = 0xFFFFF;
                    sign = 0x80000;
                    offset = 0;
                    break;
                }
                case 0x34 :
                {
                    deviceType = ID_AD5791;
                    max_value = 0xFFFFF;
                    sign = 0x80000;
                    offset = 0;
                    break;
                }
            }
        }
        else
        {
            CONSOLE_Print("Please type the number corresponding for \
your device!\r\n");
        }
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
    DoDeviceLock();

    if(AD5791_Init(deviceType) == 0)
    {
        CONSOLE_Print("Device OK\r\n");
        DisplayCmdList();
        registerValue = (AD5791_GetRegisterValue(AD5791_REG_DAC) &
                        ~(AD5791_ADDR_REG(-1))) >> offset;
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("Device Error\r\n");
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
    /* Resets the device to its power-on state. */
    AD5791_SoftInstruction(AD5791_SOFT_CTRL_RESET);
    codingStyle   = 0;
    output        = 1;
    rbuf          = 1;
    /* Send feedback to user */
    CONSOLE_Print("Device was reset.\r\n");
    registerValue = (AD5791_GetRegisterValue(AD5791_REG_DAC) &
                    ~(AD5791_ADDR_REG(-1))) >> offset;
}

/***************************************************************************//**
 * @brief Selects the coding style.
 *
 * @return None.
*******************************************************************************/
void SetCoding(double* param, char paramNo) // "coding=" command
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
        codingStyle = (char)param[0];

        if(codingStyle == 0)
        {
            AD5791_Setup(rbuf * AD5791_CTRL_RBUF_MASK);
            /* Write to DAC register */
            AD5791_SetDacValue(registerValue);
            CONSOLE_Print("%s%d(two's complement)\r\n",cmdList[2].name,
                            codingStyle);
        }
        else
        {
            AD5791_Setup(AD5791_CTRL_BIN2SC_MASK | rbuf * AD5791_CTRL_RBUF_MASK);
            /* Write to DAC register */
            AD5791_SetDacValue(registerValue);
            /* Send feedback to user for coding change */
            CONSOLE_Print("%s%d(offset binary)\r\n",cmdList[2].name,
                            codingStyle);
        }

        GetVoltage(0, 0);
    }
    else
    {
        /* Display error messages */
        DisplayError(2);
    }
}

/***************************************************************************//**
 * @brief Display the current coding style.
 *
 * @return None.
*******************************************************************************/
void GetCoding(double* param, char paramNo) // "coding?" command
{
   /* Send the requested value to user */
   if (codingStyle == 1)
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
void SetRegister(double* param, char paramNo) // "register=" command
{
    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > max_value)
        {
            param[0] = max_value;
        }
        registerValue = (long)param[0];
        /* Write to DAC register */
        AD5791_SetDacValue(registerValue);
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",cmdList[4].name, registerValue);
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
    /* Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",cmdList[4].name,registerValue);
}

/***************************************************************************//**
 * @brief Sets the DAC output voltage.
 *
 * @return None.
*******************************************************************************/
void SetVoltage(double* param, char paramNo) // "voltage=" command
{
    double         volt     = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        /* Check which formula to use according to rbuf value */
        if (rbuf == 1)
        {
            /* Check if the limits of the parameter */
            if(param[0] < VREFN)
            {
                param[0] = VREFN;
            }
            if(param[0] > VREFP)
            {
                param[0] = VREFP;
            }
            volt = (float)param[0];
            /* Calculate the register value */
            registerValue = (unsigned long)((volt - VREFN) * max_value
                             / (VREFP - VREFN));

            volt = ((float)((VREFP - VREFN) * registerValue) / max_value) +
                    VREFN ;

            /* Make a correction to register value */
            if (codingStyle == 0)
            {
                if (volt <= 0)
                {
                    registerValue += sign;
                }
                else
                {
                    registerValue -= sign;
                }
            }
            /* Check the limits of the value to be written in register */
            if (registerValue > max_value)
            {
                registerValue = max_value;
            }
            else
            {
                if (registerValue < 0)
                {
                    registerValue = 0;
                }
            }
            /* Write to DAC register */
            AD5791_SetDacValue(registerValue);
            /* Send feedback to user with the set voltage */
            GetVoltage(0, 0);

        }
        else
        {
            /* Check which formula to use according to rbuf value */
            if (rbuf == 0)
            {
                /* Check if the limits of the parameter */
                if(param[0] < ((2 * VREFN) - VREFP))
                {
                    param[0] = (2 * VREFN) - VREFP;
                }
                if(param[0] > VREFP)
                {
                    param[0] = VREFP;
                }
                volt = (float)param[0];
                /* Calculate the register value */
                registerValue = (unsigned long)((volt - 2 * VREFN + VREFP)
                                * max_value / (2 * (VREFP - VREFN)));
                volt = ((float)(2 * (VREFP - VREFN) * registerValue) /
                        max_value) + 2 * VREFN - VREFP;
                /* Make a correction to register value */
                if (codingStyle == 0)
                {
                    if (volt <= 0)
                    {
                        registerValue += sign;
                    }
                    else
                    {
                        registerValue -= sign;
                    }
                }
                /* Check the limits of the value to be written in register */
                if (registerValue > max_value)
                {
                    registerValue = max_value;
                }
                else
                {
                    if (registerValue < 0)
                    {
                        registerValue = 0;
                    }
                }
                /* Write to DAC register */
                AD5791_SetDacValue(registerValue);
                /* Send feedback to user with the set voltage */
                GetVoltage(0, 0);
            }
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(6);
    }
}

/***************************************************************************//**
 * @brief Displays the output voltage.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) // "voltage?" command
{
    double         volt     = 0;
    unsigned long  regValue = 0;

    /* Get the output voltage using data from regValue */
    regValue = registerValue;
    if (codingStyle == 0)
    {
        regValue = (regValue & sign) ?
                   (regValue & ~sign) :
                   (regValue | sign);
    }
    if (rbuf == 1)
    {
        volt = ((float)((VREFP - VREFN) * regValue) / max_value) + VREFN ;
    }
    else
    {
        if (rbuf == 0)
        {
            volt = ((float)(2 * (VREFP - VREFN) * regValue) / max_value) + 2 *
                    VREFN - VREFP;
        }
    }
    /* Send the requested value to user */
    CONSOLE_Print("voltage=%.3f [V]\r\n",volt);
}

/***************************************************************************//**
 * @brief Selects the DAC output state.
 *
 * @return None.
*******************************************************************************/
void SetOutput(double* param, char paramNo) // "output=" command
{
    /* Check if the parameter is valid */
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
        /* Set the DAC output to the selected operating mode. */
        AD5791_DacOuputState(output);
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",cmdList[8].name,output);
    }
     else
    {
        /* Display error messages */
        DisplayError(8);
    }
}

/***************************************************************************//**
 * @brief Displays the DAC output state.
 *
 * @return None.
*******************************************************************************/
void GetOutput(double* param, char paramNo) // "output?" command
{
    /* Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",cmdList[8].name,output);

}

/***************************************************************************//**
 * @brief Sets/resets the RBUF bit from control register.
 *
 * @return None.
*******************************************************************************/
void SetRbuf(double* param, char paramNo) // "rbuf=" command
{
    unsigned long regValue = 0;

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
        rbuf = (unsigned char)param[0];

        regValue = AD5791_GetRegisterValue(AD5791_REG_CTRL);
        regValue &= ~AD5791_CTRL_RBUF_MASK;
        regValue |= rbuf * AD5791_CTRL_RBUF_MASK;
        AD5791_SetRegisterValue(AD5791_REG_CTRL,regValue);

        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",cmdList[10].name,rbuf);
    }
      else
    {
        /* Display error messages */
        DisplayError(10);
    }
}

/***************************************************************************//**
 * @brief Displays the value of RBUF bit from control register.
 *
 * @return None.
*******************************************************************************/
void GetRbuf(double* param, char paramNo) // "rbuf?" command
{
    /* Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",cmdList[10].name,rbuf);

}
