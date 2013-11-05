/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for
 *           ADF4153 driver.
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
#include "adf4153.h"
#include "adf4153_cfg.h"
#include "TIME.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

const struct cmd_info cmdList[] = {
    [0] = {
        .name = "help?",
        .description = "Displays all available commands.",
        .acceptedValue = "None",
        .example = "",
    },
    [1] = {
        .name = "register=",
        .description = "Update the selected latch with the current set ups.",
        .acceptedValue = "Accepted value:\r\n\
\tlatch:\r\n\
\t0 - N divider register\r\n\
\t1 - R divider register\r\n\
\t2 - Control register\r\n\
\t3 - Noise and spur register\r\n\
\tdesired value of the register",
        .example = "To load 0x1042 to the Control register, \
type: register=2 0x1042",
    },
    [2] = {
        .name = "register?",
        .description = "Print the specified latch values in a human \
readable format",
        .acceptedValue = "Accepted value:\r\n\
\tlatch:\r\n\
\t0 - N divider register\r\n\
\t1 - R divider register\r\n\
\t2 - Control register\r\n\
\t3 - Noise and spur register",
        .example = "To print the functional latch, type: register?2",
    },
    [3] = {
        .name = "frequency=",
        .description = "Set the VCO frequency.",
        .acceptedValue = "Accepted value:\r\n\
\tfrequency:\r\n\
\t500 .. 4000 Mhz in function of the device type",
        .example = "To set the VCO frequency to 620 Mhz, \
type frequency=620",
    },
    [4] = {
        .name = "frequency?",
        .description = "Print the actual VCO frequency.",
        .acceptedValue = "None",
        .example = "",
    },
    [5] = {
        .name = "channelspacing?",
        .description = "Print the actual value of the channel spacing.",
        .acceptedValue = "None",
        .example = "",
    }
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/* Define each latch's name for better user experience */
const char* latchName[4] = {"N divider register",
                            "R divider register",
                            "control register",
                            "noise and spur register"};

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[6] = {GetHelp, SetRegister, GetRegister, \
                               SetVcoFrequency, GetVcoFrequency, \
                               GetChannelSpacing};

/* Variables which stores the current register settings */
unsigned long  r0Buff = 0;            // N Divider
unsigned long  r1Buff = 0;            // R Divider
unsigned long  r2Buff = 0;            // Control Register
unsigned long  r3Buff = 0;            // Noise and spur register

/* The minimum and maximum VCO frequency */
unsigned long vcoMinFrequency = 0;          // unit in Mhz
unsigned long vcoMaxFrequency = 0;          // unit in Mhz

/* Actual VCO frequency */
unsigned long long actualVcoFreq = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Internal function for displaying error messages.
 *
 * @param funcNo - the index of the function in 'cmdFunctions' constant array
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
 * @brief Internal function for verify if the given parameter is between his
 *        valid limits.
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
    char initStatus = 0;

    initStatus = ADF4153_Init(ADF4153_st);

    if(initStatus == 0)
    {
        /* Update the latch buffers */
        r0Buff = ADF4153_ReadLatch(ADF4153_CTRL_N_DIVIDER);
        r1Buff = ADF4153_ReadLatch(ADF4153_CTRL_R_DIVIDER);
        r2Buff = ADF4153_ReadLatch(ADF4153_CTRL_CONTROL);
        r3Buff = ADF4153_ReadLatch(ADF4153_CTRL_NOISE_SPUR);
        /* set up the VCO frequency limits */
        vcoMinFrequency = 500;
        vcoMaxFrequency = 4000;

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

/**************************************************************************//**
 * @brief Update the specified latch with the current settings
 *
 * @param param[0] - type of the latch, which will be updated
 *                  'ADF4153_CTRL_N_DIVIDER' - N Divider
 *                  'ADF4153_CTRL_R_DIVIDER' - R Divider
 *                  'ADF4153_CTRL_CONTROL' - Control Register
 *                  'ADF4153_CTRL_NOISE_SPUR' - Noise and spur register
 *
 *        param[1] - the desired value of the specified latch
 *
 * @return
******************************************************************************/
void SetRegister(double* param, char paramNo)
{
    unsigned char latch = 0;
    unsigned long value = 0;

    if(paramNo >= 2)
    {
        paramLimit(&param[0], 0, 3);
        if(param[1] < 0)
        {
            param[1] = 0;
        }
        else
        {
            if(param[1] > 0xFFFFFF)
            {
                param[1] = 0xFFFFFF;
            }
        }

        latch = (unsigned char)param[0];
        value = (unsigned long)param[1];

        switch(latch)
        {
            case ADF4153_CTRL_N_DIVIDER :
                ADF4153_UpdateLatch(ADF4153_CTRL_N_DIVIDER |
                                   (value & ~ADF4153_CTRL_MASK));
                r0Buff = ADF4153_ReadLatch(ADF4153_CTRL_N_DIVIDER);
                break;
            case ADF4153_CTRL_R_DIVIDER :
                ADF4153_UpdateLatch(ADF4153_CTRL_R_DIVIDER |
                                    (value & ~ADF4153_CTRL_MASK));
                r1Buff = ADF4153_ReadLatch(ADF4153_CTRL_R_DIVIDER);
                break;
            case ADF4153_CTRL_CONTROL :
                ADF4153_UpdateLatch(ADF4153_CTRL_CONTROL |
                                   (value & ~ADF4153_CTRL_MASK));

                r2Buff = ADF4153_ReadLatch(ADF4153_CTRL_CONTROL);
                break;
            case ADF4153_CTRL_NOISE_SPUR :
                ADF4153_UpdateLatch(ADF4153_CTRL_NOISE_SPUR |
                                    (value & ~ADF4153_CTRL_MASK));
                r3Buff = ADF4153_ReadLatch(ADF4153_CTRL_NOISE_SPUR);
                break;
        }
        /* Send user feedback */
        CONSOLE_Print("The %s was updated with 0x%.4x\r\n", \
                                                    latchName[latch], value);
    }
    else
    {
        /* Display error messages */
        DisplayError(1);
    }
}

/**************************************************************************//**
 * @brief Print the specified latch current value in a human readable manner
 *
 * @param - param[0] - type of the latch, which will be updated
 *                  'ADF4153_CTRL_N_DIVIDER' - N Divider
 *                  'ADF4153_CTRL_R_DIVIDER' - R Divider
 *                  'ADF4153_CTRL_CONTROL' - Control Register
 *                  'ADF4153_CTRL_NOISE_SPUR' - Noise and spur register
 * @return - None
******************************************************************************/
void GetRegister(double* param, char paramNo)
{
    unsigned char latch = 0;

    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, 3);
        latch = (unsigned char)param[0];
        switch(latch)
        {
            case ADF4153_CTRL_N_DIVIDER :
                CONSOLE_Print("Bellow are the %s content : 0x%x\r\n", \
                                                latchName[latch], r0Buff);
                CONSOLE_Print("\t+ Fractional value : 0x%4x\r\n", \
                    ((r0Buff & ADF4153_R0_FRAC(ADF4153_R0_FRAC_MASK)) >> \
                     ADF4153_R0_FRAC_OFFSET));
                CONSOLE_Print("\t+ Integer value : 0x%4x\r\n", \
                    ((r0Buff & ADF4153_R0_INT(ADF4153_R0_INT_MASK)) >> \
                     ADF4153_R0_INT_OFFSET));
                CONSOLE_Print("\t+ Fast-lock : 0x%1x\r\n", \
                    ((r0Buff & ADF4153_R0_FASTLOCK(ADF4153_R0_FASTLOCK_MASK)) >> \
                     ADF4153_R0_FASTLOCK_OFFSET));
                break;
            case ADF4153_CTRL_R_DIVIDER :
                CONSOLE_Print("Bellow are the %s content : 0x%x\r\n", \
                                                latchName[latch], r1Buff);
                CONSOLE_Print("\t+ Interpolator modulus value : 0x%3x\r\n", \
                    ((r1Buff & ADF4153_R1_MOD(ADF4153_R1_MOD_MASK)) >> \
                     ADF4153_R1_MOD_OFFSET));
                CONSOLE_Print("\t+ R Counter : 0x%1x\r\n", \
                    ((r1Buff & ADF4153_R1_RCOUNTER(ADF4153_R1_RCOUNTER_MASK)) >> \
                     ADF4153_R1_RCOUNTER_OFFSET));
                CONSOLE_Print("\t+ Prescaler : 0x%1x\r\n", \
                    ((r1Buff & ADF4153_R1_PRESCALE(ADF4153_R1_PRESCALE_MASK)) >> \
                     ADF4153_R1_PRESCALE_OFFSET));
                CONSOLE_Print("\t+ Muxout : 0x%1x\r\n", \
                    ((r1Buff & ADF4153_R1_MUXOUT(ADF4153_R1_MUXOUT_MASK)) >> \
                    ADF4153_R1_MUXOUT_OFFSET));
                CONSOLE_Print("\t+ Load Control : 0x%1x\r\n", \
                    ((r1Buff & ADF4153_R1_LOAD(ADF4153_R1_LOAD_MASK)) >> \
                    ADF4153_R1_LOAD_OFFSET));
                break;
            case ADF4153_CTRL_CONTROL :
                CONSOLE_Print("Bellow are the %s content : 0x%x\r\n", \
                                                latchName[latch], r2Buff);
                CONSOLE_Print("\t+ Counter Reset : 0x%1x\r\n", ((r2Buff & \
                    ADF4153_R2_COUNTER_RST(ADF4153_R2_COUNTER_RST_MASK)) >> \
                    ADF4153_R2_COUNTER_RST_OFFSET));
                CONSOLE_Print("\t+ CP Three-State : 0x%1x\r\n", ((r2Buff & \
                    ADF4153_R2_CP_3STATE(ADF4153_R2_CP_3STATE_MASK)) >> \
                    ADF4153_R2_CP_3STATE_OFFSET));
                CONSOLE_Print("\t+ Power-Down : 0x%1x\r\n", ((r2Buff & \
                    ADF4153_R2_POWER_DOWN(ADF4153_R2_POWER_DOWN_MASK)) >> \
                    ADF4153_R2_POWER_DOWN_OFFSET));
                CONSOLE_Print("\t+ LDP : 0x%1x\r\n", \
                    ((r2Buff & ADF4153_R2_LDP(ADF4153_R2_LDP_MASK)) >> \
                    ADF4153_R2_LDP_OFFSET));
                CONSOLE_Print("\t+ PD Polarity : 0x%1x\r\n", \
                    ((r2Buff & ADF4153_R2_PD_POL(ADF4153_R2_PD_POL_MASK)) >> \
                    ADF4153_R2_PD_POL_OFFSET));
                CONSOLE_Print("\t+ CP Current Settings : 0x%1x\r\n", ((r2Buff & \
                    ADF4153_R2_CP_CURRENT(ADF4153_R2_CP_CURRENT_MASK)) >> \
                    ADF4153_R2_CP_CURRENT_OFFSET));
                CONSOLE_Print("\t+ Reference doubler : 0x%1x\r\n", ((r2Buff & \
                    ADF4153_R2_REF_DOUBLER(ADF4153_R2_REF_DOUBLER_MASK)) >> \
                    ADF4153_R2_REF_DOUBLER_OFFSET));
                CONSOLE_Print("\t+ Resync : 0x%1x\r\n", \
                    ((r2Buff & ADF4153_R2_RESYNC(ADF4153_R2_RESYNC_MASK)) >> \
                    ADF4153_R2_RESYNC_OFFSET));
                break;
            case ADF4153_CTRL_NOISE_SPUR :
                CONSOLE_Print("Bellow are the %s content : 0x%x\r\n", \
                                                latchName[latch], r3Buff);
                CONSOLE_Print("\t+ Noise and spur value: 0x%2x\r\n", \
                    (((r3Buff >> 0x6) << 0x1) | ((r3Buff >> 0x2) & 0x1)));
                break;
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(2);
    }
}

/**************************************************************************//**
 * @brief Set a desire frequency (the minimum and maximum values vary at each
 *        device), the unit of the frequency is Mhz
 *
 * @param - param[0] - the value of the desired frequency in Mhz
 *
 * @return - None
******************************************************************************/
void SetVcoFrequency(double* param, char paramNo)
{
    unsigned long long vcoFrequency = 0;

    if(paramNo >= 1)
    {
        paramLimit(&param[0], vcoMinFrequency, vcoMaxFrequency);
        /* convert to Hz*/
        vcoFrequency = (unsigned long long)param[0] * 1000000;
        vcoFrequency = ADF4153_SetFrequency(vcoFrequency);
        /* Send feedback to user */
        CONSOLE_Print("The VCO frequency was configured to %.2fMhz\r\n",
                                                ((float)vcoFrequency/1000000));
        actualVcoFreq = vcoFrequency;
        /* Update the latch buffers */
        r0Buff = ADF4153_ReadLatch(ADF4153_CTRL_N_DIVIDER);
        r1Buff = ADF4153_ReadLatch(ADF4153_CTRL_R_DIVIDER);
        r2Buff = ADF4153_ReadLatch(ADF4153_CTRL_CONTROL);
        r3Buff = ADF4153_ReadLatch(ADF4153_CTRL_NOISE_SPUR);
    }
    else
    {
        /* Display error messages */
        DisplayError(3);
    }
}

/**************************************************************************//**
 * @brief Get the current VCO frequency (the minimum and maximum values vary at
  *       each device), the unit of the frequency is Mhz
 *
 * @param - None
 *
 * @return - None
******************************************************************************/
void GetVcoFrequency(double* param, char paramNo)
{
    CONSOLE_Print("The actual VCO frequency is %.2fMHz\r\n", \
                                                ((float)actualVcoFreq/1000000));
}

/**************************************************************************//**
 * @brief Get the value of channel spacing
 *
 * @param - None
 *
 * @return - None
******************************************************************************/
void GetChannelSpacing(double* param, char paramNo)
{
    unsigned long channelSpacing = 0;

    channelSpacing = ADF4153_GetChannelSpacing();
    CONSOLE_Print("The actual value of channel spacing is %.2fKHz",\
                                                ((float)channelSpacing/1000));
}
