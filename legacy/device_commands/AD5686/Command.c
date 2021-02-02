/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5686.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
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
#include "AD5686.h"
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
        .description = "Activate a power-on reset.",
        .acceptedValue = "",
        .example = "",
    },
    [2] = {
        .name = "load=",
        .description = "Loads selected DAC input register with a given value.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t\t0 - select channel A.\r\n\
\t\t1 - select channel B.\r\n\
\t\t2 - select channel C.\r\n\
\t\t3 - select channel D.\r\n\
\tvalue:\r\n\
\t\t0 .. 2^DAC_Resolution-1 - value to be written in register.",
        .example = "To load DAC C input register with 500, type: load=2 500",
    },
    [3] = {
        .name = "update=",
        .description = "Update the selected DAC channel with the input register.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t\t0 - select channel A.\r\n\
\t\t1 - select channel B.\r\n\
\t\t2 - select channel C.\r\n\
\t\t3 - select channel D.\r\n\
\t\t4 - select all channel.",
        .example = "To update channel C with the input registers, \
type: update=2",
    },
    [4] = {
        .name = "loadAndUpdate=",
        .description = "Loads and updates the selected DAC with a given value.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t\t0 - select channel A.\r\n\
\t\t1 - select channel B.\r\n\
\t\t2 - select channel C.\r\n\
\t\t3 - select channel D.\r\n\
\tvalue:\r\n\
\t\t0 .. 2^DAC_Resolution-1 - value to be written in register.",
        .example = "To load and update DAC A with 2000, \
type: loadAndUpdate=0 2000",
    },
    [5] = {
        .name = "readBack=",
        .description = "Read back the selected DAC channels value.",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t\t0 - select channel A.\r\n\
\t\t1 - select channel B.\r\n\
\t\t2 - select channel C.\r\n\
\t\t3 - select channel D.\r\n",
        .example = "To read back the value of the channel B, \
type : readBack=1",
    },
    [6] = {
        .name = "pwrMode=",
        .description = "Set up the Power Mode of a selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t\t0 - select channel A.\r\n\
\t\t1 - select channel B.\r\n\
\t\t2 - select channel C.\r\n\
\t\t3 - select channel D.\r\n\
\t\t4 - select all channel.\r\n\
\tpower mode:\r\n\
\t\t0 - normal operation.\r\n\
\t\t1 - 1KOhm to GND.\r\n\
\t\t2 - 100KOhms to GND.\r\n\
\t\t3 - three-state.",
        .example = "To select 1KOhm to GND power mode for DAC A, type: \
pwrMode=0 1",
    },
    [7] = {
        .name = "pwrMode?",
        .description = "Displays the power mode for one selected DAC.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t\t0 - select channel A.\r\n\
\t\t1 - select channel B.\r\n\
\t\t2 - select channel C.\r\n\
\t\t3 - select channel D.\r\n",
        .example = "To display the power mode for DAC B, type:pwrMode?1",
    },
    [8] = {
        .name = "ldacMask=",
        .description = "Set up the LDAC mask register",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t\t0 - select channel A.\r\n\
\t\t1 - select channel B.\r\n\
\t\t2 - select channel C.\r\n\
\t\t3 - select channel D.\r\n\
\t\t4 - select all channel.\r\n\
\tset/reset mask:\r\n\
\t\t0 - reset LDAC mask for the selected channel.\r\n\
\t\t1 - set LDAC mask for the selected channel.",
        .example = "To set LDAC mask to channel C, type : ldacMask=2 1",
    },
    [9] = {
        .name = "ldacMask?",
        .description = "Display the LDAC register.",
        .acceptedValue = "",
        .example = "",
    },
    [10] = {
        .name = "intRef=",
        .description = "Turns ON or OFF the internal reference.",
        .acceptedValue = "Accepted values:\r\n\
\t\t0 - turn OFF the internal reference. (default)\r\n\
\t\t1 - turn ON the internal reference.",
        .example = "To power on the internal reference, type: intRef=1",
    },
    [11] = {
        .name = "intRef?",
        .description = "Displays the value of the internal reference register.",
        .acceptedValue = "",
        .example = "",
    },
    [12] = {
        .name = "ldacPin=",
        .description = "Sets the output value of LDAC pin.",
        .acceptedValue = "Accepted values:\r\n\
\t\t0 - sets LDAC pin low.(default)\r\n\
\t\t1 - sets LDAC pin high.",
        .example = "To set the LDAC pin high, type: ldacPin=1",
    },
    [13] = {
        .name = "ldacPin?",
        .description = "Displays the value of the internal reference register.",
        .acceptedValue = "Displays the value of LDAC pin.",
        .example = "",
    },
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[14] = {GetHelp, SoftReset, SetLoad, SetUpdate,
                                SetLoadAndUpdate, GetReadback, SetPowerMode,
                                GetPowerMode, SetLdacMask, GetLdacMask,
                                SetInternalReference, GetInternalReference,
                                SetLdacPin, GetLdacPin};

/* Variables holding information about the device */
AD5686_type_t deviceType;
unsigned short max_value = 0;
/* control pins */
unsigned char ldac = 0;
/* control registers */
unsigned char powerReg = 0;
unsigned char ldacMaskReg = 0;
unsigned char internalRefReg = 0;

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
 * @brief Verify if the given parameter is between his valid limits.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void paramLimit(double* param,
                unsigned short lowerLimit,
                unsigned short upperLimit)
{
    if(*param < lowerLimit)
    {
        *param = lowerLimit;
    }
    else
    {
        if(*param > upperLimit)
        {
            *param = upperLimit;
        }
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
For AD5684R type 0\r\n\
For AD5686R type 1\r\n\
For AD5694R type 2\r\n\
For AD5696R type 3\r\n");
        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x33))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                    deviceType = ID_AD5684R;
                    max_value = 0xFFF;
                    break;
                }
                case 0x31 :
                {
                    deviceType = ID_AD5686R;
                    max_value = 0xFFFF;
                    break;
                }
                case 0x32 :
                {
                    deviceType = ID_AD5694R;
                    max_value = 0xFFF;
                    break;
                }
                case 0x33 :
                {
                    deviceType = ID_AD5696R;
                    max_value = 0xFFFF;
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

    if(AD5686_Init(deviceType) == 0)
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
 * @brief Activate a software reset.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void SoftReset(double* param, char paramNo)
{
    AD5686_SoftwareReset();
    /* Send requested value to user */
    CONSOLE_Print("Software reset was activated!\r\n");
}

/***************************************************************************//**
 * @brief Loads selected DAC input register with a given value.
 *
 * @param param[0] - selected DAC channel.
 *        param[1] - value to be written in register.
 *
 * @return None.
*******************************************************************************/
void SetLoad(double* param, char paramNo) // "load=" command
{

    unsigned char  channel  = 0;
    unsigned short dacValue = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 2)
    {
        /* Keep the parameter values between predefined limits */
        paramLimit(&param[0], 0, 3);
        paramLimit(&param[1], 0, max_value);

        channel = (unsigned char)param[0];
        dacValue = (unsigned short)param[1];

        channel = (channel == 0) ? AD5686_CH_A : \
                  (channel == 1) ? AD5686_CH_B : \
                  (channel == 2) ? AD5686_CH_C : \
                  (channel == 3) ? AD5686_CH_D : AD5686_CH_A;
        AD5686_WriteRegister(channel, dacValue);
        /* Send feedback to user */
        CONSOLE_Print("channel=%d; value=%d\r\n", \
                                            (unsigned char)param[0], dacValue);
    }
    else
    {
        /* Display error messages */
        DisplayError(2);
    }
}


/***************************************************************************//**
 * @brief Update selected DAC input register with a given value.
 *
 * @param param[0] - selected DAC channel.
 *
 * @return None.
*******************************************************************************/
void SetUpdate(double* param, char paramNo) // "update=" command
{
    unsigned char  channel  = 0;

    /* Check if the parameters are valid */
    if(paramNo >= 1)
    {
        /* Keep the parameter values between predefined limits */
        paramLimit(&param[0], 0, 4);

        channel = (unsigned char)param[0];

        channel = (channel == 0) ? AD5686_CH_A : \
                  (channel == 1) ? AD5686_CH_B : \
                  (channel == 2) ? AD5686_CH_C : \
                  (channel == 3) ? AD5686_CH_D : \
                  (channel == 4) ? AD5686_CH_ALL : AD5686_CH_A;
        AD5686_UpdateRegister(channel);
        /* Send feedback to user */
        CONSOLE_Print("Channel=%d was updated!\r\n", (unsigned char)param[0]);
    }
    else
    {
        /* Display error messages */
        DisplayError(3);
    }
}

/***************************************************************************//**
 * @brief Loads and updates the selected DAC with a given value..
 *
 * @param param[0] - value to be set for LDAC pin.
 *        param[1] - value to be written in register.
 * @return None.
*******************************************************************************/
void SetLoadAndUpdate(double* param, char paramNo) // "loadAndUpdate=" command
{
    unsigned char  channel  = 0;
    unsigned short dacValue = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 2)
    {
        /* Keep the parameter values between predefined limits */
        paramLimit(&param[0], 0, 3);
        paramLimit(&param[1], 0, max_value);

        channel = (unsigned char)param[0];
        dacValue = (unsigned short)param[1];

        channel = (channel == 0) ? AD5686_CH_A : \
                  (channel == 1) ? AD5686_CH_B : \
                  (channel == 2) ? AD5686_CH_C : \
                  (channel == 3) ? AD5686_CH_D : AD5686_CH_A;
        AD5686_WriteUpdateRegister(channel, dacValue);
        /* Send feedback to user */
        CONSOLE_Print("channel=%d; value=%d\r\n", \
                                            (unsigned char)param[0], dacValue);
     }
     else
     {
        /* Display error messages */
        DisplayError(4);
     }
}

/***************************************************************************//**
 * @brief Reads from the selected DAC register.
 *
 * @param param[0] - selected DAC channel.
 *
 * @return None.
*******************************************************************************/
void GetReadback(double* param, char paramNo) // "readback?" command
{
    unsigned char channel = 0;
    unsigned short dacValue = 0;

    if((deviceType == ID_AD5694R) ||
       (deviceType == ID_AD5695R) ||
       (deviceType == ID_AD5696R))
    {
        CONSOLE_Print("This function is not supported \
by the current device!\n\r");
    }
    else
    {
        /* Check if the parameter is valid */
        if(paramNo >= 1)
        {
            /* Keep the parameter values between predefined limits */
            paramLimit(&param[0], 0, 3);
            channel = (unsigned char)param[0];
            dacValue = AD5686_ReadBackRegister(channel);
            channel = (channel == 0) ? AD5686_CH_A : \
                      (channel == 1) ? AD5686_CH_B : \
                      (channel == 2) ? AD5686_CH_C : \
                      (channel == 3) ? AD5686_CH_D : AD5686_CH_A;
            /* Send feedback to user */
            CONSOLE_Print("Register=%d for channel %d\r\n", \
                                     dacValue, (unsigned char)param[0]);
        }
        else
        {
            /* Display error messages */
            DisplayError(5);
        }
    }
}

/***************************************************************************//**
 * @brief Sets the output value of CLR pin.
 *
 * @param param[0] - selected DAC channel.
 *        param[1] - power mode option. Accepted values:
                     'AD5686_PWRM_NORMAL' - Normal operation mode
                     'AD5686_PWRM_1K' - 1KOhm to GND
                     'AD5686_PWRM_100K' - 100KOhm to GND
                     'AD5686_PWRM_THREESTATE' - Three-State
 *
 * @return None.
*******************************************************************************/
void SetPowerMode(double* param, char paramNo) // "pwrMode=" command
{
    unsigned char channel = 0;
    unsigned char pwrMode = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 2)
    {
        /* Keep the parameter values between predefined limits */
        paramLimit(&param[0], 0, 4);
        paramLimit(&param[1], 0, 3);
        channel = (unsigned char) param[0];
        pwrMode = (unsigned char) param[1];
        channel = (channel == 0) ? AD5686_CH_A : \
                  (channel == 1) ? AD5686_CH_B : \
                  (channel == 2) ? AD5686_CH_C : \
                  (channel == 3) ? AD5686_CH_D : \
                  (channel == 4) ? AD5686_CH_ALL : AD5686_CH_A;
        AD5686_PowerMode(channel, pwrMode);
        /* Reset the corresponding bits of the internal power register */
        powerReg &= (channel == AD5686_CH_A) ? \
                                (AD5686_PWRM_MASK << AD5686_PWRM_CHA_OFFSET) : \
                    (channel == AD5686_CH_B) ? \
                                (AD5686_PWRM_MASK << AD5686_PWRM_CHB_OFFSET) : \
                    (channel == AD5686_CH_C) ? \
                                (AD5686_PWRM_MASK << AD5686_PWRM_CHB_OFFSET) : \
                    (channel == AD5686_CH_D) ? \
                                (AD5686_PWRM_MASK << AD5686_PWRM_CHB_OFFSET) : \
                                0x0;
        /* Set the corresponding bits of the internal power register */
        powerReg |= (channel == AD5686_CH_A) ? \
                                (pwrMode << AD5686_PWRM_CHA_OFFSET) : \
                    (channel == AD5686_CH_B) ? \
                                (pwrMode << AD5686_PWRM_CHB_OFFSET) : \
                    (channel == AD5686_CH_C) ? \
                                (pwrMode << AD5686_PWRM_CHB_OFFSET) : \
                    (channel == AD5686_CH_D) ? \
                                (pwrMode << AD5686_PWRM_CHB_OFFSET) : \
                    (channel == AD5686_CH_ALL) ? \
                                ((pwrMode << AD5686_PWRM_CHA_OFFSET) |\
                                 (pwrMode << AD5686_PWRM_CHB_OFFSET) |\
                                 (pwrMode << AD5686_PWRM_CHC_OFFSET) |\
                                 (pwrMode << AD5686_PWRM_CHD_OFFSET)) : 0x0;
        /* Send feedback to user */
        if(channel == AD5686_CH_ALL)
        {
            CONSOLE_Print("All channel has power mode %d\r\n", pwrMode);
        }
        else
        {
            CONSOLE_Print("Channel %d has power mode %d\r\n", \
                                            (unsigned char)param[0], pwrMode);
        }
     }
     else
     {
        /* Display error messages */
        DisplayError(6);
     }
}

/***************************************************************************//**
 * @brief Display the value of the Power Mode register.
 *
 * @param param[0] - selected DAC channel.
 *                   'AD5686_CH_A'   - for channel 1
 *                   'AD5686_CH_B'   - for channel 2
 *                   'AD5686_CH_C'   - for channel 3
 *                   'AD5686_CH_D'   - for channel 4
 *
 * @return None.
*******************************************************************************/
void GetPowerMode(double* param, char paramNo)
{
    unsigned char pwrMode = 0;
    unsigned char channel = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, 3);
        channel = (unsigned char) param[0];
        channel = (channel == 0) ? AD5686_CH_A : \
                  (channel == 1) ? AD5686_CH_B : \
                  (channel == 2) ? AD5686_CH_C : \
                  (channel == 3) ? AD5686_CH_D : AD5686_CH_A;
        /* Send requested value to user */
        switch(channel)
        {
            case AD5686_CH_A :
            {
                pwrMode = powerReg & AD5686_PWRM_MASK;
                CONSOLE_Print("Channel %d has power mode %d\r\n", \
                                            (unsigned char)param[0], pwrMode);
                break;
            }
            case AD5686_CH_B :
            {
                pwrMode = powerReg & (AD5686_PWRM_MASK << AD5686_PWRM_CHB_OFFSET);
                pwrMode >>= AD5686_PWRM_CHB_OFFSET;
                CONSOLE_Print("Channel %d has power mode %d\r\n", \
                                            (unsigned char)param[0], pwrMode);
                break;
            }
            case AD5686_CH_C :
            {
                pwrMode = powerReg & (AD5686_PWRM_MASK << AD5686_PWRM_CHC_OFFSET);
                pwrMode >>= AD5686_PWRM_CHC_OFFSET;
                CONSOLE_Print("Channel %d has power mode %d\r\n", \
                                            (unsigned char)param[0], pwrMode);
                break;
            }
            case AD5686_CH_D :
            {
                pwrMode = powerReg & (AD5686_PWRM_MASK << AD5686_PWRM_CHD_OFFSET);
                pwrMode >>= AD5686_PWRM_CHD_OFFSET;
                CONSOLE_Print("Channel %d has power mode %d\r\n", \
                                            (unsigned char)param[0], pwrMode);
                break;
            }
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(7);
    }
}

/***************************************************************************//**
 * @brief Sets the LDAC Mask register..
 *
 * @param param[0] - selected DAC channel. Accepted value :
 *                   'AD5686_CH_A'   - for channel 1
 *                   'AD5686_CH_B'   - for channel 2
 *                   'AD5686_CH_C'   - for channel 3
 *                   'AD5686_CH_D'   - for channel 4
 *                   'AD5686_CH_ALL' - for all channel
 *        param[1] - set or reset mask ('1' for set and '0' for reset)
 * @return None.
*******************************************************************************/
void SetLdacMask(double* param, char paramNo)
{
    unsigned char channel = 0;
    unsigned char value = 0;
    unsigned char ldacMask = ldacMaskReg;

    /* Check if the parameter is valid */
    if((unsigned char)paramNo >= 2)
    {
        paramLimit(&param[0], 0, 4);
        paramLimit(&param[1], 0, 1);
        channel = (unsigned char) param[0];
        value = (unsigned char) param[1];
        channel = (channel == 0) ? AD5686_CH_A : \
                  (channel == 1) ? AD5686_CH_B : \
                  (channel == 2) ? AD5686_CH_C : \
                  (channel == 3) ? AD5686_CH_D : \
                  (channel == 4) ? AD5686_CH_ALL : AD5686_CH_A;
        /* Set the corresponding register bit */
        if(value == 0x1)
        {
            ldacMask |= (channel == AD5686_CH_A) ? AD5686_CH_A : \
                   (channel == AD5686_CH_B) ? AD5686_CH_B : \
                   (channel == AD5686_CH_C) ? AD5686_CH_C : \
                   (channel == AD5686_CH_D) ? AD5686_CH_D : \
                   (channel == AD5686_CH_ALL) ? AD5686_CH_ALL : 0x0;
        }
        else /* Reset the corresponding register bit */
        {
            ldacMask &= (channel == AD5686_CH_A) ? ~AD5686_CH_A : \
                   (channel == AD5686_CH_B) ? ~AD5686_CH_B : \
                   (channel == AD5686_CH_C) ? ~AD5686_CH_C : \
                   (channel == AD5686_CH_D) ? ~AD5686_CH_D : \
                   (channel == AD5686_CH_ALL) ? ~AD5686_CH_ALL : 0xF;
        }

        AD5686_LdacMask(ldacMask);
        ldacMaskReg = ldacMask;
        CONSOLE_Print("ldacMask=%d for channel %d\r\n", \
                                    (unsigned char)param[1], (unsigned char)param[0]);
    }
    else
    {
        /* Display error messages */
        DisplayError(8);
    }
}

/***************************************************************************//**
 * @brief Display the LDAC Mask register.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetLdacMask(double* param, char paramNo)
{
    unsigned char channel = 0;

    /* Send requested value to user */
    for(channel=0; channel < 4; channel++)
    {
        if( (ldacMaskReg & (1 << channel)) > 0 )
        {
            CONSOLE_Print("LDAC pin at channel %d is masked.\r\n", \
                                                                    channel);
        }
        else
        {
            CONSOLE_Print("LDAC pin at channel %d is not masked.\r\n", \
                                                                    channel);
        }
    }
}

/***************************************************************************//**
 * @brief Set up the Internal Reference register.
 *
 * @param param[0] - turn ON ('1') or OFF ('0') the internal voltage reference
 *
 * @return None.
*******************************************************************************/
void SetInternalReference(double* param, char paramNo)
{
    unsigned char intref = 0;

    /* Check if the parameter is valid */
    if((unsigned char)paramNo >= 1)
    {
        paramLimit(&param[0], 0, 1);
        intref = (unsigned char)param[0];
        AD5686_InternalReference(intref);
        internalRefReg = intref;
        /* Send feedback to user */
        if(intref)
        {
            CONSOLE_Print("Internal reference voltage is ON.\r\n");
        }
        else
        {
            CONSOLE_Print("Internal reference voltage is OFF.\r\n");
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(10);
    }
}

/***************************************************************************//**
 * @brief Display the Internal Reference register.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void GetInternalReference(double* param, char paramNo)
{
    /* Send feedback to user */
    if(internalRefReg)
    {
        CONSOLE_Print("Internal reference voltage is ON.\r\n");
    }
    else
    {
        CONSOLE_Print("Internal reference voltage is OFF.\r\n");
    }
}

/***************************************************************************//**
 * @brief Sets the output value of LDAC pin.
 *
 * @param param[0] - The voltage level of the LDAC pin. Accepted value:
 *                   '0' - Low
 *                   '1' - High
 *
 * @return None.
*******************************************************************************/
void SetLdacPin(double* param, char paramNo)
{
    unsigned char value = 0;

    /* Check if the parameter is valid */
    if((unsigned char)paramNo >= 1)
    {
        paramLimit(&param[0], 0, 1);
        value = (unsigned char)param[0];
        if(value)
        {
            AD5686_LDAC_HIGH;
            ldac = 1;
        }
        else
        {
            AD5686_LDAC_LOW;
            ldac = 0;
        }
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[12].name, ldac);
    }
    else
    {
        /* Display error messages */
        DisplayError(12);
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
