/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5629R.
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
#include "ad5629r.h"
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
        .description = "Makes a power-on reset.",
        .acceptedValue = "",
        .example = "",
    },
    [2] = {
        .name = "powerMode=",
        .description = "Selects a given power mode for selected DAC.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 7 - selected DAC A .. H.\r\n\
\tpower mode:\r\n\
\t0 - normal operation.\r\n\
\t1 - 1KOhm to GND.\r\n\
\t2 - 100KOhms to GND.\r\n\
\t3 - three-state.",
        .example = "To select 1KOhm to GND power mode for DAC A, type: \
powerMode=0 1",
    },
    [3] = {
        .name = "powerMode?",
        .description = "Displays the power mode for one selected DAC.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 7 - selected DAC A .. H.",
        .example = "To display the power mode for DAC B, type:powerMode?1",
    },
    [4] = {
        .name = "intRef=",
        .description = "Turns on/off the internal reference.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - turns off the internal reference.\r\n\
\t1 - turns on the internal reference.",
        .example = "To power on the internal reference, type: intRef=1",
    },
    [5] = {
        .name = "intRef?",
        .description = "Displays the status of the internal reference.",
        .acceptedValue = "",
        .example = "",
    },
    [6] = {
        .name = "loadN=",
        .description = "Loads selected DAC register with a given value.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 7 - selected DAC A .. H.\r\n\
\t15 - all DACs. \r\n\
\tvalue:\r\n\
\t0 .. (2^DacResolution)-1 - value to be written in register.",
        .example = "To load DAC B register with 1000, type: loadN=1 1000",
    },
    [7] = {
        .name = "updateN=",
        .description = "Updates the selected DAC with the last value written in register.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 7 - selected DAC A .. H.\r\n\
\t15 - all DACs. \r\n\
\tvalue:\r\n\
\t0 .. (2^DacResolution)-1 - value to be written in register.",
        .example = "To update DAC H output, type: updateN=7",
    },
    [8] = {
        .name = "loadNUpdateN=",
        .description = "Loads and updates the selected DAC with a given value.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 7 - selected DAC A .. H.\r\n\
\t15 - all DACs. \r\n\
\tvalue:\r\n\
\t0 .. (2^DacResolution)-1 - value to be written in register.",
        .example = "To load and update DAC A with 2000, type: loadNUpdateN=0 \
2000",
    },
    [9] = {
        .name = "loadNUpdateAll=",
        .description = "Loads the selected DAC with a given value and updates \
all DACs.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 7 - selected DAC A .. H.\r\n\
\t15 - all DACs. \r\n\
\tvalue:\r\n\
\t0 .. (2^DacResolution)-1 - value to be written in register.",
        .example = "To load DAC C with 4000 and update all DAC outputs, type: \
loadNUpdateAll=2 4000",
    },
    [10] = {
        .name = "enLdacPin=",
        .description = "Enables/Disables the LDAC pin for selected DAC.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 7 - selected DAC A .. H.\r\n\
\tvalue:\r\n\
\t1 - disable LDAC pin.\r\n\
\t0 - enable LDAC pin.",
        .example = "To disable the LDAC pin for DAC H, type: enLdacPin=7 1",
    },
    [11] = {
        .name = "enLdacPin?",
        .description = "Displays the status(enabled or disabled) of the LDAC \
pin for a selected DAC.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 7 - selected DAC A .. H.",
        .example = "To display the status of LDAC pin for DAC D, type: \
enLdacPin?3",
    },
    [12] = {
        .name = "clrCode=",
        .description = "Loads Clear Code Register with specific clear code.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - clears code to zero scale when CLR pin goes from high to low.\r\n\
\t1 - clears code to midscale when CLR pin goes from high to low.\r\n\
\t2 - clears code to full scale when CLR pin goes from high to low.\r\n\
\t3 - no operation.",
        .example = "To set the active clear code to midscale, type: clrCode=1",
    },
    [13] = {
        .name = "clrCode?",
        .description = "Displays the active clear code.",
        .acceptedValue = "",
        .example = "",
    },
    [14] = {
        .name = "ldacPin=",
        .description = "Sets the output value of LDAC pin.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - sets LDAC pin low.(default)\r\n\
\t1 - sets LDAC pin high.",
        .example = "To set the LDAC pin high, type: ldacPin=1",
    },
    [15] = {
        .name = "ldacPin?",
        .description = "Displays the value of LDAC pin.",
        .acceptedValue = "",
        .example = "",
    },
    [16] = {
        .name = "clrPin=",
        .description = "Sets the output value of CLR pin.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - sets CLR pin low.\r\n\
\t1 - sets CLR pin high.(default)",
        .example = "To set the CLR pin low, type: clrPin=0",
    },
    [17] = {
        .name = "clrPin?",
        .description = "Displays the value of CLR pin.",
        .acceptedValue = "",
        .example = "",
    }
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[18] = {GetHelp, DoReset, SetPowerMode, GetPowerMode,
                                SetIntRef,GetIntRef, SetLoadN, SetUpdateN,
                                SetLoadNAndUpdateN, SetLoadNAndUpdateAll,
                                SetEnLdacPin, GetEnLdacPin, SetClrCode,
                                GetClrCode, SetLdacPin, GetLdacPin,
                                SetClrPin, GetClrPin};

/* Variables holding information about the device */
AD5629R_type   deviceType;
unsigned char  ldac         = 0;
unsigned char  clr          = 1;
unsigned short max_value    = 0;
unsigned char  ref          = 0;
unsigned char  clrCode      = 0;
unsigned char  powerMode[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char  ldacRegVal   = 0;


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
For AD5629R type: 0\r\n\
For AD5669R type: 1\r\n\
For AD5668  type: 2\r\n\
For AD5648  type: 3\r\n\
For AD5628  type: 4\r\n\
");
        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x34))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                    deviceType = ID_AD5629R;
                    max_value = 0xFFF;
                    break;
                }
                case 0x31 :
                {
                    deviceType = ID_AD5669R;
                    max_value = 0xFFFF;
                    break;
                }
                case 0x32 :
                {
                    deviceType = ID_AD5668;
                    max_value = 0xFFFF;
                    break;
                }
                case 0x33 :
                {
                    deviceType = ID_AD5648;
                    max_value = 0x3FFF;
                    break;
                }
                case 0x34 :
                {
                    deviceType = ID_AD5628;
                    max_value = 0xFFF;
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

    if(AD5629R_Init(AD5629R_I2C_ADDR_2, deviceType) == 0)
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
 * @brief  Makes a power-on reset.
 *
 * @param  - none.
 *
 * @return - none.
 *
*******************************************************************************/
void DoReset(double* param, char paramNo) // "reset!" command
{
    AD5629R_Reset();
    /* Send feedback to user */
    CONSOLE_Print("Device was reset.\r\n");
}

/***************************************************************************//**
 * @brief Selects a given power mode for selected DAC.
 *
 * @param param[0] - selected DAC.
 *        param[1] - selected power mode.
 *
 * @return None.
*******************************************************************************/
void SetPowerMode(double* param, char paramNo) // "powerMode=" command
{

    unsigned char channel = 0;
    unsigned char mode    = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 7)
            {
                param[0] = 7;
            }
        }

        if(param[1] < 0)
        {
            param[1] = 0;
        }
        else
        {
            if(param[1] > 3)
            {
                param[1] = 3;
            }
        }

        channel = (unsigned char)param[0];
        mode    = (unsigned char)param[1];
        AD5629R_SetPowerMode((1 << channel), mode);
        powerMode[channel] = mode;
        /* Send feedback to user */
        CONSOLE_Print("channel=%d; powerMode=%d\r\n",channel, mode);
    }
    else
    {
        /* Display error messages */
        DisplayError(2);
    }
}

/***************************************************************************//**
 * @brief  Displays the power mode for one selected DAC.
 *
 * @param param[0] - selected DAC.
 *
 * @return None.
*******************************************************************************/
void GetPowerMode(double* param, char paramNo) // "powerMode?" command
{
    unsigned char  channel  = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 7)
            {
                param[0] = 7;
            }
        }

        channel = (unsigned char)param[0];
        /* Send requested value to user */
        CONSOLE_Print("channel=%d; powerMode=%d\r\n",channel, powerMode[channel]);
    }
    else
    {
        /* Display error messages */
        DisplayError(3);
    }
}

/***************************************************************************//**
 * @brief Turns on/off the internal reference.
 *
 * @param param[0] - status of internal reference(on/off).
 *
 * @return None.
*******************************************************************************/
void SetIntRef(double* param, char paramNo) // "intRef=" command
{
    unsigned char status = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 1)
            {
                param[0] = 1;
            }
        }

        status = (unsigned char)param[0];
        AD5629R_SetRef(status);
        ref = status;
        /* Send feedback to user */
        if (status == 0)
        {
            CONSOLE_Print("The internal reference is disabled.\r\n");
        }
        else
        {
            CONSOLE_Print("The internal reference is enabled.\r\n");
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(4);
    }
}

/***************************************************************************//**
 * @brief Displays the status of the internal reference.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetIntRef(double* param, char paramNo) // "IntRef?" command
{
    /* Send requested value to user */
    if (ref == 0)
    {
        CONSOLE_Print("The internal reference is disabled.\r\n");
    }
    else
    {
        CONSOLE_Print("The internal reference is enabled.\r\n");
    }
}

/***************************************************************************//**
 * @brief Loads selected DAC register with a given value.
 *
 * @param param[0] - selected DAC.
 *        param[1] - value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetLoadN(double* param, char paramNo) // "loadN=" command
{

    unsigned char  channel  = 0;
    unsigned short dacValue = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 7)
            {
                if(param[0] < 15)
                {
                    param[0] = 7;
                }
                else
                {
                    param[0] = 15;
                }
            }
        }

        if(param[1] < 0)
        {
            param[1] = 0;
        }
        else
        {
            if(param[1] > max_value)
            {
                param[1] = max_value;
            }
        }

        channel  = (unsigned char)param[0];
        dacValue = (unsigned short)param[1];
        AD5629R_WriteRegN(channel, dacValue);
        /* Send feedback to user */
        if (channel == 15)
        {
            CONSOLE_Print("channel=all; value=%d\r\n", dacValue);
        }
        else
        {
            CONSOLE_Print("channel=%d; value=%d\r\n",channel, dacValue);
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(6);
    }
}


/***************************************************************************//**
 * @brief Updates the selected DAC with the last value written in register.
 *
 * @param param[0] - selected DAC.
 *
 * @return None.
*******************************************************************************/
void SetUpdateN(double* param, char paramNo) // "updateN=" command
{
    unsigned char  channel  = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 7)
            {
                if(param[0] < 15)
                {
                    param[0] = 7;
                }
                else
                {
                    param[0] = 15;
                }
            }
        }

        channel = (unsigned char)param[0];
        AD5629R_UpdateDacN(channel);
        /* Send feedback to user */

        if (channel == 15)
        {
            CONSOLE_Print("all channels were updated\r\n");
        }
        else
        {
            CONSOLE_Print("channel %d was updated\r\n",channel);
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(7);
    }
}

/***************************************************************************//**
 * @brief Loads and updates the selected DAC with a given value.
 *
 * @param param[0] - selected DAC.
 *        param[1] - value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetLoadNAndUpdateN(double* param, char paramNo)//"loadNUpdateN=" command
{

    unsigned char  channel  = 0;
    unsigned short dacValue = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 7)
            {
                if(param[0] < 15)
                {
                    param[0] = 7;
                }
                else
                {
                    param[0] = 15;
                }
            }
        }

        if(param[1] < 0)
        {
            param[1] = 0;
        }
        else
        {
            if(param[1] > max_value)
            {
                param[1] = max_value;
            }
        }

        channel  = (unsigned char)param[0];
        dacValue = (unsigned short)param[1];
        AD5629R_WriteRegNUpdateN(channel, dacValue);
        /* Send feedback to user */
        if (channel == 15)
        {
            CONSOLE_Print("channel=all; value=%d\r\n", dacValue);
        }
        else
        {
            CONSOLE_Print("channel=%d; value=%d\r\n",channel, dacValue);
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(8);
    }
}

/***************************************************************************//**
 * @brief Loads the selected DAC with a given value and updates all DACs.
 *
 * @param param[0] - selected DAC.
 *        param[1] - value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetLoadNAndUpdateAll(double* param, char paramNo)//"loadNUpdateAll=" command
{

    unsigned char  channel  = 0;
    unsigned short dacValue = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 7)
            {
                if(param[0] < 15)
                {
                    param[0] = 7;
                }
                else
                {
                    param[0] = 15;
                }
            }
        }

        if(param[1] < 0)
        {
            param[1] = 0;
        }
        else
        {
            if(param[1] > max_value)
            {
                param[1] = max_value;
            }
        }

        channel  = (unsigned char)param[0];
        dacValue = (unsigned short)param[1];
        AD5629R_WriteRegNUpdateAll(channel, dacValue);
        /* Send feedback to user */
        if (channel == 15)
        {
            CONSOLE_Print("channel=all; value=%d\r\n", dacValue);
        }
        else
        {
            CONSOLE_Print("channel=%d; value=%d\r\n",channel, dacValue);
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(9);
    }
}

/***************************************************************************//**
 * @brief Enables/Disables the LDAC pin for selected DAC.
 *
 * @param param[0] - selected DAC.
 *        param[1] - enable/disable LDAC pin.
 *
 * @return None.
*******************************************************************************/
void SetEnLdacPin(double* param, char paramNo) // "enLdacPin=" command
{

    unsigned char channel = 0;
    unsigned char status  = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 7)
            {
                param[0] = 7;
            }
        }

        if(param[1] < 0)
        {
            param[1] = 0;
        }
        else
        {
            if(param[1] > 1)
            {
                param[1] = 1;
            }
        }
        channel  = (unsigned char)param[0];
        status   = (unsigned short)param[1];
        ldacRegVal &= ~(1 << channel);
        ldacRegVal |=  (status == 0) ? 0 : (1 << channel);
        AD5629R_LoadLdacReg(ldacRegVal);

        /* Send feedback to user */
        if (status == 1)
        {
            CONSOLE_Print("For channel %d, LDAC pin is disabled. It sees LDAC pin as 0.\r\n",channel);
        }
        else
        {
            CONSOLE_Print("For channel %d, LDAC pin is enabled.\r\n",channel);
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(10);
    }
}

/***************************************************************************//**
 * @brief  Displays the status(enabled or disabled) of the LDAC pin for a
 *         selected DAC.
 *
 * @param param[0] - selected DAC.
 *
 * @return None.
*******************************************************************************/
void GetEnLdacPin(double* param, char paramNo) // "enLdacPin?" command
{
    unsigned char  channel  = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 7)
            {
                param[0] = 7;
            }
        }

        channel = (unsigned char)param[0];
        /* Send requested value to user */
        if (ldacRegVal & (1 << channel))
        {
            CONSOLE_Print("For channel %d, LDAC pin is disabled. It sees LDAC pin as 0.\r\n",channel);
        }
        else
        {
            CONSOLE_Print("For channel %d, LDAC pin is enabled.\r\n",channel);
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(11);
    }
}

/***************************************************************************//**
 * @brief Loads Clear Code Register with specific clear code.
 *
 * @param param[0] - selected clear code.
 *
 * @return None.
*******************************************************************************/
void SetClrCode(double* param, char paramNo) // "clrCode=" command
{
    unsigned char  code  = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 3)
            {
                param[0] = 3;
            }
        }

        code = (unsigned char)param[0];
        AD5629R_LoadClearCodeReg(code);
        clrCode = code;
        /* Send feedback to user */
        CONSOLE_Print("clearCode=%d\r\n",code);
    }
    else
    {
        /* Display error messages */
        DisplayError(12);
    }
}

/***************************************************************************//**
 * @brief Displays the active clear code.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetClrCode(double* param, char paramNo) // "clrCode?" command
{
    /* Send requested value to user */
    CONSOLE_Print("clearCode=%d\r\n",clrCode);
}

/***************************************************************************//**
 * @brief Sets the output value of LDAC pin.
 *
 * @param param[0] - value to be set for LDAC pin.
 *
 * @return None.
*******************************************************************************/
void SetLdacPin(double* param, char paramNo) // "ldacPin=" command
{
    unsigned char status = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {
            if(param[0] > 1)
            {
                param[0] = 1;
            }
        }

        status = (unsigned char) param[0];

        if (status == 0)
        {
            AD5629R_LDAC_LOW
            ldac = 0;
        }
        else
        {
            if (status == 1)
            {
                AD5629R_LDAC_HIGH
                ldac = 1;
            }
        }

        /* Send feedback to user */
        CONSOLE_Print("LDAC=%d\r\n",status);
     }
     else
     {
        /* Display error messages */
        DisplayError(14);
     }
}

/***************************************************************************//**
 * @brief Displays the value of LDAC pin.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetLdacPin(double* param, char paramNo) // "ldacPin?" command
{
    /* Send requested value to user */
    CONSOLE_Print("LDAC=%d\r\n",ldac);
}

/***************************************************************************//**
 * @brief Sets the output value of CLR pin.
 *
 * @param param[0] - value to be set for CLR pin.
 *
 * @return None.
*******************************************************************************/
void SetClrPin(double* param, char paramNo) // "clrPin=" command
{
    unsigned char status = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        else
        {   if(param[0] > 1)
            {
                param[0] = 1;
            }
        }

        status = (unsigned char) param[0];

        if (status == 0)
        {
            AD5629R_CLR_LOW;
            clr = 0;
        }
        else
        {
            if (status == 1)
            {
                AD5629R_CLR_HIGH;
                clr = 1;
            }
        }

        /* Send feedback to user */
        CONSOLE_Print("CLR=%d\r\n",status);
     }
     else
     {
        /* Display error messages */
        DisplayError(16);
     }
}

/***************************************************************************//**
 * @brief Displays the value of CLR pin.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetClrPin(double* param, char paramNo) // "clrPin?" command
{
    /* Send requested value to user */
    CONSOLE_Print("CLR=%d\r\n",clr);
}
