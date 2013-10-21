/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for AD9833.
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
#include "ad9833.h"

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
        .name = "output=",
        .description = "Selects the type of output.",
        .acceptedValue = "Accepted values:\r\n\
\tFor AD9833 & AD9837: \r\n\
\t0 - Sinusoid.(default)\r\n\
\t1 - Triangle.\r\n\
\t2 - DAC Data MSB/2.\r\n\
\t3 - DAC Data MSB.\r\n\
\tFor AD9834 & AD9838: \r\n\
\t0 - Sinusoid.(default)\r\n\
\t1 - Triangle.",
        .example = "To select the triangle output, type: output=1",
    },
    [2] = {
            .name = "output?",
            .description = "Displays the type of output.",
            .acceptedValue = "",
            .example = "",
        },
    [3] = {
        .name = "loadFreqReg=",
        .description = "Loads a frequency value in one selected register.",
        .acceptedValue = "Accepted values:\r\n\
\tRegister number:\r\n\
\t0 - Frequency Register 0.\r\n\
\t1 - Frequency Register 1.\r\n\
\tValue:\r\n\
\tAD9833: 0 .. 12 500 000 - the frequency value in Hz.\r\n\
\tAD9834: 0 .. 37 500 000 - the frequency value in Hz.\r\n\
\tAD9837 & AD9838: 0 .. 8 000 000 - the frequency value in Hz.",
        .example = "To load a frequency of 10KHz to register 0, type: \
loadFreqReg=0 10000",
    },
    [4] = {
        .name = "freqRegVal?",
        .description = "Displays the value from one selected frequency register.",
        .acceptedValue = "Accepted values:\r\n\
\tRegister number:\r\n\
\t0 - Frequency Register 0.\r\n\
\t1 - Frequency Register 1.",
        .example = "To display the value from frequency register 1, type: freqReg?1",
    },
    [5] = {
        .name = "loadPhaseReg=",
        .description = "Loads a phase value in one selected register.",
        .acceptedValue = "Accepted values:\r\n\
\tRegister number:\r\n\
\t0 - Phase Register 0.\r\n\
\t1 - Phase Register 1.\r\n\
\tValue: 0 .. 2PI - the phase value in radians.",
        .example = "To load a phase of 3.141 to register 0, type: \
loadPhaseReg=0 3.141",
    },
    [6] = {
        .name = "phaseRegVal?",
        .description = "Displays the value from one selected phase register.",
        .acceptedValue = "Accepted values:\r\n\
\tRegister number:\r\n\
\t0 - Phase Register 0.\r\n\
\t1 - Phase Register 1.",
        .example = "To display the value from phase register 1, type: phaseReg?1",
    },
    [7] = {
        .name = "freqRegNo=",
        .description = "Select the frequency register to be used.",
        .acceptedValue = "Accepted values:\r\n\
\tRegister number:\r\n\
\t0 - Frequency Register 0.\r\n\
\t1 - Frequency Register 1.",
        .example = "To select frequency register 0, type: freqRegNo=0",
    },
    [8] = {
        .name = "freqRegNo?",
        .description = "Displays the selected frequency register.",
        .acceptedValue = "",
        .example = "",
    },
    [9] = {
        .name = "phaseRegNo=",
        .description = "Select the phase register to be used.",
        .acceptedValue = "Accepted values:\r\n\
\tRegister number:\r\n\
\t0 - Phase Register 0.\r\n\
\t1 - Phase Register 1.",
        .example = "To select phase register 1, type: freqRegNo=1",
    },
    [10] = {
        .name = "phaseRegNo?",
        .description = "Displays the selected phase register.",
        .acceptedValue = "",
        .example = "",
    },
    [11] = {
        .name = "sleepMode=",
        .description = "Select one sleep mode.",
        .acceptedValue = "Accepted values:\r\n\
\tSleep mode:\r\n\
\tSoft method(all devices):\r\n\
\t0 - No power-down.(default)\r\n\
\t1 - DAC powered down.\r\n\
\t2 - Internal clock disabled.\r\n\
\t3 - DAC powered down and Internal clock disabled.\r\n\
\tHard method(AD9834 & AD9838):\r\n\
\t0 - No power-down.(default)\r\n\
\t1 - DAC powered down.",
        .example = "To select DAC powered down, type: sleepMode=1",
    },
    [12] = {
        .name = "sleepMode?",
        .description = "Displays the selected sleep mode.",
        .acceptedValue = "",
        .example = "",
    },
    [13] = {
        .name = "program=",
        .description = "Sets the programming method (only for AD9834 & \
AD9838).",
        .acceptedValue ="Accepted values:\r\n\
\tMethod:\r\n\
\t0 - Soft programming method.(default)\r\n\
\t1 - Hard programming method.",
        .example = "To set the hardware programming method, type:program=1",
    },
    [14] = {
        .name = "program?",
        .description = "Displays the programming method.",
        .acceptedValue = "",
        .example = "",
    },
    [15] = {
        .name = "logicOut=",
        .description = "Sets the logic output type (only for AD9834 & AD9838).",
        .acceptedValue = "Accepted values:\r\n\
\tLogic output type:\r\n\
 \t0 - High impedance.(default)\r\n\
 \t1 - DAC data MSB/2.\r\n\
 \t2 - DAC data MSB.\r\n\
 \t3 - Comparator.",
.example = "To set the output logic type to DAC data MBS, type:logicOut=2",
    },
    [16] = {
        .name = "logicOut?",
        .description = "Displays the logic output type (only for AD9834 & \
AD9838).",
        .acceptedValue = "",
        .example = "",
    },
};


const char cmdNo     = (sizeof(cmdList) / sizeof(struct cmd_info));
float      PHASE_MAX = 6.28318530f;

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp, SetOutput, GetOutput, LoadFreqReg,
                              GetFreqRegVal, LoadPhaseReg, GetPhaseRegVal,
                              SelFreqRegNo, GetFreqRegNo, SelPhaseRegNo,
                              GetPhaseRegNo, SetSleepMode, GetSleepMode,
                              SetProgram, GetProgram, SetLogicOut, GetLogicOut};
/* Variables holding information about the device */
unsigned char  freqRegNo    = 0;        // Frequency Register value for soft programming
unsigned char  fselPin      = 0;        // Frequency Bit value for hard programming
unsigned long  freqReg[2]   = {0, 0};   // Values from both Frequency Registers
unsigned char  phaseRegNo   = 0;        // Phase Register value for soft programming
unsigned char  pselPin      = 0;        // Phase Bit value for hard programming
float          phaseReg[2]  = {0, 0};   // Values from both Phase Registers
unsigned char  sleepMode    = 0;        // Sleep Mode Register value for soft programming
unsigned char  sleepPin     = 0;        // Sleep Bit value for hard programming
unsigned char  output       = 0;        // Output type
unsigned long  max_freq_out = 0;        // Max output frequency for a device type
unsigned char  program      = 0;        // Programming method
unsigned char  logicOut     = 0;        // Logic output type

AD9833_type deviceType;

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
 * @brief Sets the current device type.
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
For AD9833 type: 0\r\n\
For AD9834 type: 1\r\n\
For AD9837 type: 2\r\n\
For AD9838 type: 3\r\n\
");
        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x33))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                    deviceType = ID_AD9833;
                    max_freq_out = 25000000 / 2;
                    break;
                }
                case 0x31 :
                {
                    deviceType = ID_AD9834;
                    max_freq_out = 75000000 / 2;
                    break;
                }
                case 0x32 :
                {
                    deviceType = ID_AD9837;
                    max_freq_out = 16000000 / 2;
                    break;
                }
                case 0x33 :
                {
                    deviceType = ID_AD9838;
                    max_freq_out = 16000000 / 2;
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

    if(AD9833_Init(deviceType) == 0)
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
 * @brief Selects the type of output.
 *
 * @param param[0] - type of output
 *                   Example AD9833&AD9837: 0 - Sinusoid.
 *                                          1 - Triangle.
 *                                          2 - DAC Data MSB/2.
 *                                          3 - DAC Data MSB.
 *                   Example AD9834&AD9838: 0 - Sinusoid.
 *                                          1 - Triangle.
 *
 * @return None.
*******************************************************************************/
void SetOutput(double* param, char paramNo) // "output=" command
{
    char result = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if ((deviceType == ID_AD9833) || (deviceType == ID_AD9837))
        {
            if(param[0] > 3)
            {
                param[0] = 3;
            }
        }
        else
        {
            if ((deviceType == ID_AD9834) || (deviceType == ID_AD9838))
            {
                if(param[0] > 1)
                {
                    param[0] = 1;
                }
            }
        }
        output = (unsigned char)param[0];
        /* Set the type of output. */
        result = AD9833_OutMode(output);
        /* Send feedback to user */
        if (result == 0)
        {
            GetOutput(NULL, 0);
        }
        else
        {
            CONSOLE_Print("Triangle output not supported by this configuration.\
 You must disable\r\n the logic output and then choose the triangle output.\r\n");
            output = 0;
        }
    }
     else
    {
         /* Display error messages */
         DisplayError(1);
    }
}

/***************************************************************************//**
 * @brief Displays the type of output.
 *
 * @return None.
*******************************************************************************/
void GetOutput(double* param, char paramNo) // "output?" command
{
    /* Send feedback to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[1].name,output);
}

/***************************************************************************//**
 * @brief Loads a frequency value in one selected register.
 *
 * @param param[0] - register number. (0 / 1)
 *        param[1] - frequency value in Hz. (0 .. 12 500 000)
 *
 * @return None.
*******************************************************************************/
void LoadFreqReg(double* param, char paramNo) // "loadFreqReg=" command
{
    unsigned char regNo = 0;
    /* Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 1)
        {
            param[0] = 1;
        }
        regNo = (unsigned char)param[0];

        if(param[1] < 0)
        {
            param[1] = 0;
        }
        if(param[1] > max_freq_out)
        {
            param[1] = max_freq_out;
        }
        freqReg[regNo] = (unsigned long)param[1];
        /* Set the desired frequency value to selected register. */
        AD9833_SetFreq(regNo, freqReg[regNo]);
        /* Send feedback to user */
        CONSOLE_Print("register=%d; frequency=%d [Hz]\r\n",regNo,freqReg[regNo]);
    }
     else
    {
         /* Display error messages */
         DisplayError(3);
    }
}

/***************************************************************************//**
 * @brief Displays the value from one selected frequency register.
 *
 * @param param[0] - register number. (0 / 1)
 *
 * @return None.
*******************************************************************************/
void GetFreqRegVal(double* param, char paramNo) // "freqRegVal?" command
{
    unsigned char regNo = 0;
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
        regNo = (unsigned char)param[0];
        /* Send feedback to user */
        CONSOLE_Print("register=%d; frequency=%d [Hz]\r\n",regNo,freqReg[regNo]);
    }
     else
    {
         /* Display error messages */
         DisplayError(4);
    }
}

/***************************************************************************//**
 * @brief Loads a phase value in one selected register.
 *
 * @param param[0] - register number. (0 / 1)
 *        param[1] - phase value in degrees. (0 .. 2PI)
 *
 * @return None.
*******************************************************************************/
void LoadPhaseReg(double* param, char paramNo) // "LoadPhaseReg=" command
{
    unsigned char regNo = 0;
    /* Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 1)
        {
            param[0] = 1;
        }
        regNo = (unsigned char)param[0];

        if(param[1] < 0)
        {
            param[1] = 0;
        }
        if(param[1] > PHASE_MAX)
        {
            param[1] = PHASE_MAX;
        }
        phaseReg[regNo] = (float)param[1];
        /* Set the desired phase value to selected register. */
        AD9833_SetPhase(regNo, phaseReg[regNo]);
        /* Send feedback to user */
        CONSOLE_Print("register=%d; phase=%.3f [radians]\r\n",regNo,phaseReg[regNo]);
    }
     else
    {
         /* Display error messages */
         DisplayError(5);
    }
}

/***************************************************************************//**
 * @brief Displays the value from one selected phase register.
 *
 * @param param[0] - register number. (0 / 1)
 *
 * @return None.
*******************************************************************************/
void GetPhaseRegVal(double* param, char paramNo) // "phaseRegVal?" command
{
    unsigned char regNo = 0;
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
        regNo = (unsigned char)param[0];
        /* Send feedback to user */
        CONSOLE_Print("register=%d; phase=%.3f [radians]\r\n",regNo,phaseReg[regNo]);
    }
     else
    {
         /* Display error messages */
         DisplayError(6);
    }
}

/***************************************************************************//**
 * @brief Select the frequency register to be used.
 *
 * @param param[0] - register number. (0 / 1)
 *
 * @return None.
*******************************************************************************/
void SelFreqRegNo(double* param, char paramNo) // "freqRegNo=" command
{
    unsigned char freq = 0;

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
        freq = (unsigned char)param[0];
        /* Make the selection of frequency register. */
        AD9833_SelectFreqReg(freq);
        if (program == 1)
        {
            fselPin = freq;
        }
        else
        {
            if (program == 0)
            {
                freqRegNo = freq;
            }
        }
        /* Send feedback to user */
        GetFreqRegNo(NULL, 0);
    }
     else
    {
         /* Display error messages */
         DisplayError(7);
    }
}

/***************************************************************************//**
 * @brief Displays the selected frequency register.
 *
 * @return None.
*******************************************************************************/
void GetFreqRegNo(double* param, char paramNo) // "freqRegNo?" command
{
    /* Send feedback to user */
    if (program == 0)
    {
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[7].name,freqRegNo);
    }
    else
    {
        if (program == 1)
        {
            CONSOLE_Print("%s%d\r\n",(char*)cmdList[7].name,fselPin);
        }
    }
}

/***************************************************************************//**
 * @brief Select the phase register to be used.
 *
 * @param param[0] - register number. (0 / 1)
 *
 * @return None.
*******************************************************************************/
void SelPhaseRegNo(double* param, char paramNo) // "phaseRegNo=" command
{
    unsigned char phase = 0;

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
        phase = (unsigned char)param[0];
        /* Make the selection of the phase register */
        AD9833_SelectPhaseReg(phase);
        if (program == 1)
        {
            pselPin = phase;
        }
        else
        {
            if (program == 0)
            {
                phaseRegNo = phase;
            }
        }
        /* Send feedback to user */
        GetPhaseRegNo(NULL, 0);
    }
     else
    {
         /* Display error messages */
         DisplayError(9);
    }
}

/***************************************************************************//**
 * @brief Displays the selected phase register.
 *
 * @return None.
*******************************************************************************/
void GetPhaseRegNo(double* param, char paramNo) // "phaseRegNo?" command
{
    /* Send feedback to user */
    if (program == 0)
    {
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[9].name,phaseRegNo);
    }
    else
    {
        if (program == 1)
        {
            CONSOLE_Print("%s%d\r\n",(char*)cmdList[9].name,pselPin);
        }
    }
}

/***************************************************************************//**
 * @brief Select one sleep mode.
 *
 * @param param[0] - type of sleep
 *                    Example soft method(all devices):
 *                              0 - No power-down.
 *                              1 - DAC powered down.
 *                              2 - Internal clock disabled.
 *                              3 - DAC powered down and Internal
 *                                  clock disabled.
 *                    Example hard method(AD9834 & AD9838):
 *                              0 - No power-down.
 *                              1 - DAC powered down.
 *
 * @return None.
*******************************************************************************/
void SetSleepMode(double* param, char paramNo) // "sleepMode=" command
{
    unsigned char sleep = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 3)
        {
            param[0] = 3;
        }
        sleep = (unsigned char)param[0];
        /* Set the selected sleep mode. */
        AD9833_SleepMode(sleep);
        if (program == 1)
        {
            if ((sleep == 0) || (sleep == 1))
            {
                sleepPin = sleep;
            }
        }
        else
        {
            if (program == 0)
            {
                sleepMode = sleep;
            }
        }
        /* Send feedback to user */
        GetSleepMode(NULL, 0);
    }
     else
    {
         /* Display error messages */
         DisplayError(11);
    }
}

/***************************************************************************//**
 * @brief Displays the selected sleep mode.
 *
 * @return None.
*******************************************************************************/
void GetSleepMode(double* param, char paramNo) // "sleepMode?" command
{
    /* Send feedback to user */
    if (program == 0)
    {
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[11].name,sleepMode);
    }
    else
    {
        if (program == 1)
        {
            CONSOLE_Print("%s%d\r\n",(char*)cmdList[11].name,sleepPin);
        }
    }
}

/***************************************************************************//**
 * @brief Sets the programming method. (only for AD9834 & AD9838)
 *
 * @param param[0] - method.
 *                   Example: 0 - software method.
 *                            1 - hardware method.
 *
 * @return None.
*******************************************************************************/
void SetProgram(double* param, char paramNo) // "program=" command
{
    /* Verify the device */
    if ((deviceType == ID_AD9834) || (deviceType == ID_AD9838))
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
            program = (unsigned char)param[0];
            /* Set the selected sleep mode. */
            AD9834_SelectProgMethod(program);
            /* Send feedback to user */
            GetProgram(NULL, 0);
        }
        else
        {
            /* Display error messages */
            DisplayError(13);
        }
    }
    else
    {
        CONSOLE_Print("Function not compatible with this device.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the programming method.
 *
 * @return None.
*******************************************************************************/
void GetProgram(double* param, char paramNo) // "program?" command
{
    /* Send feedback to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[13].name,program);
}

/***************************************************************************//**
 * @brief Sets the logic output type. (only for AD9834 & AD9838)
 *
 * @param param[0] - logic output type.
 *                   Example: 0 - High impedance.
 *                            1 - DAC data MSB/2.
 *                            2 - DAC data MSB.
 *                            3 - Comparator.
 *
 * @return None.
*******************************************************************************/
void SetLogicOut(double* param, char paramNo) // "logicOut=" command
{
    /* Verify the device */
    if ((deviceType == ID_AD9834) || (deviceType == ID_AD9838))
    {
        /* Check if the parameter is valid */
        if(paramNo >= 1)
        {
            if(param[0] < 0)
            {
                param[0] = 0;
            }
            if(param[0] > 3)
            {
                param[0] = 3;
            }
            logicOut = (unsigned char)param[0];

            switch(logicOut)
            {
                case 0:     // High impedance
                    AD9834_LogicOutput(0,0,0);
                    break;
                case 1:     // DAC data MSB/2
                    AD9834_LogicOutput(1,0,0);
                    output = 0;
                    break;
                case 2:     // DAC data MSB
                    AD9834_LogicOutput(1,0,1);
                    output = 0;
                    break;
                case 3:     // Comparator
                    AD9834_LogicOutput(1,1,1);
                    output = 0;
                    break;
                default:    // Do nothing
                    break;
            }
            /* Send feedback to user */
            CONSOLE_Print("%s%d\r\n",(char*)cmdList[15].name,logicOut);
        }
        else
        {
            /* Display error messages */
            DisplayError(15);
        }
    }
    else
    {
        CONSOLE_Print("Function not compatible with this device.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the logic output type. (only for AD9834 & AD9838)
 *
 * @return None.
*******************************************************************************/
void GetLogicOut(double* param, char paramNo) // "logicOut?" command
{
    if ((deviceType == ID_AD9834) || (deviceType == ID_AD9838))
    {
        /* Send feedback to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[15].name,logicOut);
    }
    else
    {
        CONSOLE_Print("Function not compatible with this device.\r\n");
    }
}
