/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for
 *           ADF4106 driver.
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
#include "adf4106.h"
#include "adf4106_cfg.h"
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
\t0 - Reference latch\r\n\
\t1 - N Counter latch\r\n\
\t2 - Function latch\r\n\
\t3 - Initialization latch\r\n\
\tdesired value of the register",
        .example = "To load 0xD8082 to the Function Latch, \
type: register=2 0xD8082",
    },
    [2] = {
        .name = "register?",
        .description = "Print the specified latch values in a human \
readable format",
        .acceptedValue = "Accepted value:\r\n\
\tlatch:\r\n\
\t0 - Reference latch\r\n\
\t1 - N Counter latch\r\n\
\t2 - Function latch\r\n\
\t3 - Initialization latch",
        .example = "To print the functional latch, type: register?2",
    },
    [3] = {
        .name = "frequency=",
        .description = "Set the VCO frequency.",
        .acceptedValue = "Accepted value:\r\n\
\tfrequency:\r\n\
\t5/500 .. 200/400/6000 Mhz in function of the device type",
        .example = "To set the VCO frequency to 120 Mhz, \
type frequency=120",
    },
    [4] = {
        .name = "frequency?",
        .description = "Print the actual VCO frequency.",
        .acceptedValue = "None",
        .example = "",
    }
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/* Possible initialization methods :    * INIT_LATCH
 *                                      * INIT_CEPIN
 *                                      * INIT_COUNTER_RESET
 * Change the constant bellow to setup a different initialization method
 * For further information about each method see the data sheet.
 */
const ADF4106_init_t initMethod = INIT_LATCH;

/* Define each latch's name for better user experience */
const char* latchName[4] = {"reference latch",
                            "n latch",
                            "function latch",
                            "initialization latch"};

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[5] = {GetHelp, SetRegister, GetRegister, \
                               SetVcoFrequency, GetVcoFrequency };

ADF4106_type_t deviceType;

/* Variables which stores the current register settings */
unsigned long rLatchBuff = 0;               // reference latch
unsigned long nLatchBuff = 0;               // N Counter latch
unsigned long fiLatchBuff = 0;              // Function/Initialization latch

/* The minimum and maximum VCO frequency */
unsigned long vcoMinFrequency = 0;          // unit in Mhz
unsigned long vcoMaxFrequency = 0;          // unit in Mhz

/* Actual VCO frequency */
unsigned long actualVcoFreq = 0;

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
For ADF4001 type: 0\r\n\
For ADF4002 type: 1\r\n\
For ADF4106 type: 2\r\n");

        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x34))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                    deviceType = ID_ADF4001;
                    vcoMinFrequency = 5;
                    vcoMaxFrequency = 200;
                    break;
                case 0x31 :
                    deviceType = ID_ADF4002;
                    vcoMinFrequency = 5;
                    vcoMaxFrequency = 400;
                    break;
                case 0x32 :
                    deviceType = ID_ADF4106;
                    vcoMinFrequency = 500;
                    vcoMaxFrequency = 6000;
                    break;
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
    char initStatus = 0;

    DoDeviceLock();

    initStatus = ADF4106_Init(deviceType, initMethod, ADF4106_st);

    if(initStatus == 0)
    {
        /* Update the latch buffers */
        rLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_R_COUNTER);
        nLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_N_COUNTER);
        fiLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_FUNCTION_LATCH);

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
 *                  'ADF4106_CTRL_R_COUNTER' - Reference Counter Latch
 *                  'ADF4106_CTRL_N_COUNTER' - N Counter Latch
 *                  'ADF4106_CTRL_FUNCTION_LATCH' - Function Latch
 *                  'ADF4106_CTRL_INIT_LATCH' - Initialization latch
 *
 *        param[1] - the desired value of the specified latch
 *
 *        Note : The function and initialization latch essentially is the same,
 *               however, when the initialization latch is programmed, there is
 *               an additional internal reset pulse applied to the R and N
 *               counters.
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
            case ADF4106_CTRL_R_COUNTER :
                // Enable the counter reset
                ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH |
                                    fiLatchBuff |
                                    ADF4106_CR_RESET);
                ADF4106_UpdateLatch(ADF4106_CTRL_R_COUNTER |
                                   (value & ~ADF4106_CTRL_MASK));
                ADF4106_UpdateLatch(ADF4106_CTRL_N_COUNTER |
                                    nLatchBuff);
                // Disable counter reset
                ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH |
                                    fiLatchBuff |
                                    ADF4106_CR_NORMAL);
                rLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_R_COUNTER);
                break;
            case ADF4106_CTRL_N_COUNTER :
                // Enable the counter reset
                ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH |
                                    fiLatchBuff |
                                    ADF4106_CR_RESET);
                ADF4106_UpdateLatch(ADF4106_CTRL_R_COUNTER |
                                    rLatchBuff);
                ADF4106_UpdateLatch(ADF4106_CTRL_N_COUNTER |
                                   (value & ~ADF4106_CTRL_MASK));
                // Disable counter reset
                ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH |
                                    fiLatchBuff |
                                    ADF4106_CR_NORMAL);
                nLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_N_COUNTER);
                break;
            case ADF4106_CTRL_FUNCTION_LATCH :
                ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH |
                                   (value & ~ADF4106_CTRL_MASK));
                fiLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_FUNCTION_LATCH);
                break;
            case ADF4106_CTRL_INIT_LATCH :
                ADF4106_UpdateLatch(ADF4106_CTRL_INIT_LATCH |
                                   (value & ~ADF4106_CTRL_MASK));
                fiLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_FUNCTION_LATCH);
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
 *                  'ADF4106_CTRL_R_COUNTER' - Reference Counter Latch
 *                  'ADF4106_CTRL_N_COUNTER' - N Counter Latch
 *                  'ADF4106_CTRL_FUNCTION_LATCH' - Function Latch
 *
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
            case ADF4106_CTRL_R_COUNTER :
                CONSOLE_Print("Bellow are the %s content : 0x%4x\r\n", \
                                                latchName[latch], rLatchBuff);
                CONSOLE_Print("\t+ Reference Counter Register : %4x\r\n", \
                    (rLatchBuff & ADF4106_R_COUNTER(ADF4106_R_COUNTER_MASK)) \
                        >> ADF4106_R_COUNTER_OFFSET);
                CONSOLE_Print("\t+ Anti-backlash Width : 0x%1x\r\n", \
                            (rLatchBuff & ADF4106_R_ABP(ADF4106_R_ABP_MASK)) \
                            >> ADF4106_R_ABP_OFFSET);
                CONSOLE_Print("\t+ Test Modes Bits : 0x%1x\r\n", \
                            (rLatchBuff & ADF4106_R_TMB(ADF4106_R_TMB_MASK)) \
                            >> ADF4106_R_TMB_OFFSET);
                CONSOLE_Print("\t+ Lock Detect Precision : 0x%1x\r\n", \
                            (rLatchBuff & ADF4106_R_LDP(ADF4106_R_LDP_MASK)) \
                            >> ADF4106_R_LDP_OFFSET);
                break;
            case ADF4106_CTRL_N_COUNTER :
                CONSOLE_Print("Bellow are the %s content : 0x%4x\r\n", \
                                                latchName[latch], nLatchBuff);
                if(deviceType == ID_ADF4106)
                {
                    CONSOLE_Print("\t+ 6-bit A Counter : %4x\r\n", \
                    (nLatchBuff & ADF4106_N_COUNTER_A(ADF4106_N_COUNTER_A_MASK)) \
                    >> ADF4106_N_COUNTER_A_OFFSET);
                }
                CONSOLE_Print("\t+ 13-bit B Counter : %4x\r\n", \
                    (nLatchBuff & ADF4106_N_COUNTER_B(ADF4106_N_COUNTER_B_MASK)) \
                    >> ADF4106_N_COUNTER_B_OFFSET);
                CONSOLE_Print("\t+ Charge Pump Gain : 0x%1x\r\n", \
                                (nLatchBuff & ADF4106_N_CP(ADF4106_N_CP_MASK)) \
                                >> ADF4106_N_CP_OFFSET);
                break;
            case ADF4106_CTRL_FUNCTION_LATCH :
            case ADF4106_CTRL_INIT_LATCH :
                CONSOLE_Print("Bellow are the %s content : 0x%4x\r\n", \
                                                latchName[latch], fiLatchBuff);
                CONSOLE_Print("\t+ Counter Reset : 0x%1x\r\n", \
                                    (fiLatchBuff & ADF4106_CR(ADF4106_CR_MASK)) \
                                    >> ADF4106_CR_OFFSET);
                CONSOLE_Print("\t+ Power Down 1 : 0x%1x\r\n", \
                                (fiLatchBuff & ADF4106_PD1(ADF4106_PD1_MASK)) \
                                    >> ADF4106_PD1_OFFSET);
                CONSOLE_Print("\t+ Muxout Control : 0x%1x\r\n", \
                            (fiLatchBuff & ADF4106_MUXOUT(ADF4106_MUXOUT_MASK)) \
                                >> ADF4106_MUXOUT_OFFSET);
                CONSOLE_Print("\t+ PD Polarity : 0x%1x\r\n", \
                            (fiLatchBuff & ADF4106_PDPOL(ADF4106_PDPOL_MASK)) \
                                >> ADF4106_PDPOL_OFFSET);
                CONSOLE_Print("\t+ CP Three-State : 0x%1x\r\n",\
                                    (fiLatchBuff & ADF4106_CP(ADF4106_CP_MASK)) \
                                        >> ADF4106_CP_OFFSET);
                CONSOLE_Print("\t+ FastLock Mode : 0x%1x\r\n", \
                        (fiLatchBuff & ADF4106_FASTLOCK(ADF4106_FASTLOCK_MASK)) \
                            >> ADF4106_FASTLOCK_OFFSET);
                CONSOLE_Print("\t+ Timer Counter Control : 0x%1x\r\n", \
                                (fiLatchBuff & ADF4106_TCC(ADF4106_TCC_MASK)) \
                                    >> ADF4106_TCC_OFFSET);
                CONSOLE_Print("\t+ Current Settings 1 : 0x%1x\r\n", \
                                (fiLatchBuff & ADF4106_CS1(ADF4106_CS1_MASK)) \
                                    >> ADF4106_CS1_OFFSET);
                CONSOLE_Print("\t+ Current Settings 2 : 0x%1x\r\n", \
                                (fiLatchBuff & ADF4106_CS2(ADF4106_CS2_MASK)) \
                                    >> ADF4106_CS2_OFFSET);
                CONSOLE_Print("\t+ Power Down 2 : 0x%1x\r\n", \
                                (fiLatchBuff & ADF4106_PD2(ADF4106_PD2_MASK)) \
                                    >> ADF4106_PD2_OFFSET);
                CONSOLE_Print("\t+ Prescale Value : 0x%1x\r\n", \
                                    (fiLatchBuff & ADF4106_PS(ADF4106_PS_MASK)) \
                                        >> ADF4106_PS_OFFSET);
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
        /* convert the unit to Hz */
        vcoFrequency = (unsigned long long)param[0] * 1000000;
        vcoFrequency = ADF4106_SetFrequency(vcoFrequency);
        /* Send feedback to user */
        CONSOLE_Print("The VCO frequency was configured to %dMhz\r\n",
                                                        vcoFrequency/1000000);
        actualVcoFreq = vcoFrequency;
        /* Update the latch buffers */
        rLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_R_COUNTER);
        nLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_N_COUNTER);
        fiLatchBuff = ADF4106_ReadLatch(ADF4106_CTRL_FUNCTION_LATCH);
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
    CONSOLE_Print("The actual VCO frequency is %dMHz\r\n", \
                                actualVcoFreq / 1000000);
}
