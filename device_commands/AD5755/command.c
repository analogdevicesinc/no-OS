/**************************************************************************//**
*   @file   command.c
*   @brief  Implementation of command functions
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
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

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "command.h"
#include "AD5755.h"
#include "Console.h"

/***************************************************************************/
/************************ Constants Definitions ****************************/
/***************************************************************************/

#define MAXVALUE 65535

const struct cmd_info cmdList[] = {
    [0] = {
        .name = "help?",
        .description = "Displays all available commands.",
        .acceptedValue = "",
        .example = "",
    },
    [1] = {
        .name = "register=",
        .description = "Writes to the a data register.",
        .acceptedValue = "Accepted values:\r\n\
\tRegister address : \t0 - DAC Data Reg,\r\n\
\t\t\t\t2 -> Gain Register,\r\n\
\t\t\t\t3 -> Gain Register All DACs,\r\n\
\t\t\t\t4 -> Offset Register,\r\n\
\t\t\t\t5 -> Offset Register All DACs,\r\n\
\t\t\t\t6 -> Clear Code Register,\r\n\
\t\t\t\t7 -> Control Register.\r\n\
\tChannel : 0 .. 3 - channel A .. D.\r\n\
\tValue : 0 .. 65535 - the value written to the DAC.",
        .example = "To write 3200 to DAC data register of channel A, type: \
dataReg=0 0 3200",
    },
    [2] = {
        .name = "control=",
        .description = "Writes to the a control register.",
        .acceptedValue = "Accepted values:\r\n\
\tRegister address : \t0 - Slew Rate Register,\r\n\
\t\t\t\t1 -> Main Control Register,\r\n\
\t\t\t\t2 -> DAC Control Register,\r\n\
\t\t\t\t3 -> Dc-to-dc Control Register,\r\n\
\t\t\t\t4 -> Software Register.\r\n\
\tChannel : 0 .. 3 - channel A .. D.\r\n\
\tValue : 0 .. 65535 - the value written to the DAC.",
        .example = "To write 0x1001 to DAC control register of channel A, \
type : controlReg=2 0 0x1001",
    },
    [3] = {
        .name = "register?",
        .description = "Read back the value of a specified register.",
        .acceptedValue = "Accepted values:\r\n\
\tRegister address : 0x00 .. 0x1A.",
        .example = "",
    },
    [4] = {
        .name = "power=",
        .description = "Set the power state of the dc-to-dc converters, \
DAC and internal amplifiers for the selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\tChannel : 0 .. 3 - channel A .. D.\r\n\
\tvalue : 0 - turn off; 1 - turn on.",
        .example = "To turn ON the power of channel C, type: power=2 1",
    },
    [5] = {
        .name = "power?",
        .description = "Displays the power state of the dc-to-dc converters,\
DAC and internal amplifiers for the selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\tChannel : 0 .. 3 - channel A .. D.",
        .example = "To display the power status of channel C, type: power?2",
    },
    [6] = {
        .name = "range=",
        .description = "Set the range of the selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\tChannel : 0 .. 3 - channel A .. D.\r\n\
\tRange : \t0 ->  0 V to 5 V voltage\r\n\
\t\t\t1 -> 0 V to 10 V voltage\r\n\
\t\t\t2 -> -5 V to +5 V voltage\r\n\
\t\t\t3 -> -10 V to +10 V voltage\r\n\
\t\t\t4 -> 4 mA to 20 mA current\r\n\
\t\t\t5 -> 0 mA to 20 mA current\r\n\
\t\t\t6 -> 0 mA to 24 mA current",
        .example = "To set the range of channel A to 4 mA to 20 mA, \
type: range=0 4",
    },
    [7] = {
        .name = "range?",
        .description = "Displays the range of the selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\tChannel : 0 .. 3 - channel A .. D.",
        .example = "To display the range of channel A, type: range?0",
    },
    [8] = {
        .name = "voltage=",
        .description = "Sets the output voltage for a selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\tChannel : 0 .. 3 - channel A .. D.\r\n\
\tDesired voltage value : Vmin*1000 .. Vmax*1000",
        .example = "To set 3.5V on channel B, type : voltage=1 3500",
    },
    [9] = {
        .name = "voltage?",
        .description = "Displays the output voltage for a selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\tChannel : 0 .. 3 - channel A .. D.",
        .example = "To display the output voltage for channel B, type: \
voltage?1",
    },
    [10] = {
        .name = "current=",
        .description = "Sets the output current for a selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\tChannel : 0 .. 3 - channel A .. D.\r\n\
\tDesired current value (unit is mA) multiplied by 1000",
        .example = "To set 15mA on channel B, type : current=1 15000",
    },
    [11] = {
        .name = "current?",
        .description = "Displays the output current for a selected channel.",
        .acceptedValue = "Accepted values:\r\n\
\tChannel : 0 .. 3 - channel A .. D.",
        .example = "To display the output current for channel D, \
type: current?3",
    },
    [12] = {
        .name = "getStatus!",
        .description = "Read back the Status register and print any faults or \
errors.",
        .acceptedValue = "",
        .example = "",
    },
    [13] = {
        .name = "testSPI!",
        .description = "Ensure that the SPI pins are working correctly.",
        .acceptedValue = "",
        .example = "",
    }
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/***************************************************************************/
/************************ Variables Definitions ****************************/
/***************************************************************************/

cmdFunction cmdFunctions[14] = {GetHelp, SetDataRegister, SetControlRegister,
                                GetRegister, SetPower, GetPower, SetRange,
                                GetRange, SetVoltage, GetVoltage, SetCurrent,
                                GetCurrent, EvaluateStatus, TestSpi};

/* Variables holding information about the device */
AD5755_type_t deviceType;
short minLimit[4] = {0, 0, 0, 0};
short maxLimit[4] = {0, 0, 0, 0};
/*****************************************************************************/
/************************ Functions Definitions ******************************/
/*****************************************************************************/

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
For AD5755   type: 0\r\n\
For AD5755-1 type: 1\r\n\
For AD5757   type: 2\r\n\
");
        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x32))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                    deviceType = ID_AD5755;
                    break;
                }
                case 0x31 :
                {
                    deviceType = ID_AD5755_1;
                    break;
                }
                case 0x32 :
                {
                    deviceType = ID_AD5757;
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

    if(AD5755_Init(deviceType) == 0)
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

    /* Set the Gain for channel A. */
    AD5755_SetRegisterValue(AD5755_DREG_WR_GAIN,
                            AD5755_DAC_A,
                            0xFFFF);

    /* Set the Offset for channel A. */
    AD5755_SetRegisterValue(AD5755_DREG_WR_OFFSET,
                            AD5755_DAC_A,
                            0x8000);

    /* Set the Clear Code for channel A. */
    AD5755_SetRegisterValue(AD5755_DREG_WR_CLR_CODE,
                            AD5755_DAC_A,
                            0x0000);
    /* Select the output range */
    if( (deviceType == ID_AD5755) ||
        (deviceType == ID_AD5755_1) )
    {   /* 0V to 5V. */
        AD5755_SetChannelRange(AD5755_DAC_A, AD5755_R_0_5_V);
        AD5755_SetChannelRange(AD5755_DAC_B, AD5755_R_0_5_V);
        AD5755_SetChannelRange(AD5755_DAC_C, AD5755_R_0_5_V);
        AD5755_SetChannelRange(AD5755_DAC_D, AD5755_R_0_5_V);
    }
    else //ID_AD5757
    {   /* 0 mA to 20 mA */
        AD5755_SetChannelRange(AD5755_DAC_A, AD5755_R_0_20_MA);
        AD5755_SetChannelRange(AD5755_DAC_B, AD5755_R_0_20_MA);
        AD5755_SetChannelRange(AD5755_DAC_C, AD5755_R_0_20_MA);
        AD5755_SetChannelRange(AD5755_DAC_D, AD5755_R_0_20_MA);
    }

    /* Enable only channel one to be cleared when CLEAR bit is asserted. */
    AD5755_ChannelClearEnable(AD5755_DAC_A, 1);

    /* Set the output voltage for channels B, C and D. */
    TIME_DelayUs(10000);
    if( (deviceType == ID_AD5755) ||
        (deviceType == ID_AD5755_1) )
    {
        AD5755_SetVoltage(AD5755_DAC_B, 1.25);
        AD5755_SetVoltage(AD5755_DAC_C, 2.5);
        AD5755_SetVoltage(AD5755_DAC_D, 4.9);
    }
    else
    {
        AD5755_SetCurrent(AD5755_DAC_B, 3);
        AD5755_SetCurrent(AD5755_DAC_C, 12);
        AD5755_SetCurrent(AD5755_DAC_D, 19.5);
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

/**************************************************************************//**
* @brief Function of the dataReg= command. Sets a value for a data register.
*
* @param param      - array of the command arguments, which should be:
*        param[0]   - The address of the data register. Accepted values:
*                       0 - DAC Data Reg
*                       2 - Gain Register
*                       3 - Gain Register All DACs
*                       4 - Offset Register
*                       5 - Offset Register All DACs
*                       6 - Clear Code Register
*                       7 - Control Register
*        param[1]   - The selected channel. Accepted value:
*                       0 - channel A
*                       1 - channel B
*                       2 - channel C
*                       3 - channel D
*        param[2]   - the value of the register, should be between [0 .. 65535]
*
* @param paramNo    - number of the arguments
*
* @return void
******************************************************************************/
void SetDataRegister(double* param, char paramNo)
{
    unsigned char   address   = 0;
    unsigned char   channel   = 0;
    unsigned short   data      = 0;

    if(paramNo >= 3)
    {
        /* Ensure that the address is not 1 */
        if(param[0] == 1)
        {
            param[0] = 0;
        }
        /* Keep the parameters between valid limits */
        paramLimit(&param[0], 0, 7);
        paramLimit(&param[1], 0, 3);
        paramLimit(&param[2], 0, MAXVALUE);

        address = (char)param[0];
        channel = (char)param[1];
        data    = (unsigned short)param[2];

        AD5755_SetRegisterValue(address, channel, data);
        /* Send feedback to user */
        CONSOLE_Print("Register[0x%02x] at channel 0x%02x is 0x%4x\r\n", \
                                                        address, channel, data);
    }
    else
    {
        DisplayError(1);
    }
}

/**************************************************************************//**
* @brief Function of the controlReg= command. Sets a value for a control
*           register.
* @param param      - array of the command arguments, which should be:
*        param[0]   - The address of the data register. Accepted values:
*                       0 - Slew Rate control register (one per channel)
*                       1 - Main control register
*                       2 - DAC control register (one per channel)
*                       3 - DC-to-DC control register
*                       4 - Software register
*        param[1]   - The selected channel. Accepted value:
*                       0 - channel A
*                       1 - channel B
*                       2 - channel C
*                       3 - channel D
*        param[2]   - the value of the register, should be between [0 .. 65535]
*
* @param paramNo    - number of the arguments
*
* @return void
******************************************************************************/
void SetControlRegister(double* param, char paramNo)
{
    unsigned char address = 0;
    unsigned char channel = 0;
    unsigned short data = 0;

    if(paramNo >= 3)
    {
        /* Keep the parameters between valid limits */
        paramLimit(&param[0], 0, 4);
        paramLimit(&param[1], 0, 3);
        paramLimit(&param[2], 0, 8191);

        address = (unsigned char)param[0];
        channel = (unsigned char)param[1];
        data    = (unsigned short)param[2];

        AD5755_SetControlRegisters(address, channel, data);
        /* Send feedback to user */
        CONSOLE_Print("Control register[0x%02x] for channel 0x%02x = 0x%4x\r\n",\
                                                address, channel, data);
    }
    else
    {
        DisplayError(2);
    }
}

/**************************************************************************//**
* @brief Function of the register? command.
*
* @param param      - array of the command arguments
*        param[0]   - the address of the register, which will be read
* @param paramNo    - number of the arguments
* @return void
******************************************************************************/
void GetRegister(double* param, char paramNo)
{
    unsigned short regValue = 0;
    unsigned char registerAddr = 0;

    if(paramNo >= 1)
    {   /* Validate address */
        paramLimit(&param[0], 0, 26);
        /* Address wrapping */
        switch((int)param[0])
        {
            case  0 :
                registerAddr = AD5755_RD_DATA_REG(0);
                break;
            case  1 :
                registerAddr = AD5755_RD_DATA_REG(1);
                break;
            case  2 :
                registerAddr = AD5755_RD_DATA_REG(2);
                break;
            case  3 :
                registerAddr = AD5755_RD_DATA_REG(3);
                break;
            case  4 :
                registerAddr = AD5755_RD_CTRL_REG(0);
                break;
            case  5 :
                registerAddr = AD5755_RD_CTRL_REG(1);
                break;
            case  6 :
                registerAddr = AD5755_RD_CTRL_REG(2);
                break;
            case  7 :
                registerAddr = AD5755_RD_CTRL_REG(3);
                break;
            case  8 :
                registerAddr = AD5755_RD_GAIN_REG(0);
                break;
            case  9 :
                registerAddr = AD5755_RD_GAIN_REG(1);
                break;
            case 10 :
                registerAddr = AD5755_RD_GAIN_REG(2);
                break;
            case 11 :
                registerAddr = AD5755_RD_GAIN_REG(3);
                break;
            case 12 :
                registerAddr = AD5755_RD_OFFSET_REG(0);
                break;
            case 13 :
                registerAddr = AD5755_RD_OFFSET_REG(1);
                break;
            case 14 :
                registerAddr = AD5755_RD_OFFSET_REG(2);
                break;
            case 15 :
                registerAddr = AD5755_RD_OFFSET_REG(3);
                break;
            case 16 :
                registerAddr = AD5755_RD_CODE_REG(0);
                break;
            case 17 :
                registerAddr = AD5755_RD_CODE_REG(1);
                break;
            case 18 :
                registerAddr = AD5755_RD_CODE_REG(2);
                break;
            case 19 :
                registerAddr = AD5755_RD_CODE_REG(3);
                break;
            case 20 :
                registerAddr = AD5755_RD_SR_CTRL_REG(0);
                break;
            case 21 :
                registerAddr = AD5755_RD_SR_CTRL_REG(1);
                break;
            case 22 :
                registerAddr = AD5755_RD_SR_CTRL_REG(2);
                break;
            case 23 :
                registerAddr = AD5755_RD_SR_CTRL_REG(3);
                break;
            case 24 :
                registerAddr = AD5755_RD_STATUS_REG;
                break;
            case 25 :
                registerAddr = AD5755_RD_MAIN_CTRL_REG;
                break;
            case 26 :
                registerAddr = AD5755_RD_Dc_DC_CTRL_REG;
                break;
            default:
                registerAddr = 0;
                break;
        }
        regValue = (short)AD5755_GetRegisterValue(registerAddr);
        if(regValue == -1)
        {
            CONSOLE_Print("AD5755 CRC ERROR!\r\n");
        }
        else
        {
            CONSOLE_Print("register[0x%02x]=0x%02x\r\n", \
                                            (unsigned char)param[0], regValue);
        }
    }
    else
    {
        DisplayError(3);
    }
}

/**************************************************************************//**
* @brief Function of the power= command.
*
* @param param      - array of the command arguments
*        param[0]   - selected channel
*        param[1]   - switch ON(1) or OFF(0)
* @param paramNo    - number of the arguments
* @return void
******************************************************************************/
void SetPower(double* param, char paramNo)
{
    short regValue = 0;
    char channel = 0;
    char toggleSw = 0;
    /* Validate parameters */
    if(paramNo >= 2)
    {   /* Validate channel */
        paramLimit(&param[0], 0, 3);
        paramLimit(&param[1], 0, 1);
        channel = (unsigned char)param[0];
        toggleSw   = (unsigned char)param[1];

        /* Read the DAC control register */
        regValue = (short)AD5755_GetRegisterValue(AD5755_RD_CTRL_REG((char)channel));
        if(regValue == -1)
        {
            CONSOLE_Print("AD5755 CRC ERROR!\r\n");
        }
        else
        {
            if(toggleSw == 1)
            {   /* Enable the AD5755_DAC_DC_DC and channel output */
                AD5755_SetControlRegisters( AD5755_CREG_DAC,
                                            channel,
                                            (regValue | AD5755_DAC_INT_ENABLE \
                                                      | AD5755_DAC_DC_DC \
                                                      | AD5755_DAC_OUTEN) );
                CONSOLE_Print("Channel %d is ON\r\n", \
                                (int)channel, (regValue | AD5755_DAC_INT_ENABLE \
                                                        | AD5755_DAC_DC_DC \
                                                        | AD5755_DAC_OUTEN) );
            }
            else
            {   /* Disable the AD5755_DAC_DC_DC and channel output */
                AD5755_SetControlRegisters(AD5755_CREG_DAC,
                                          channel,
                                          (regValue & ~(AD5755_DAC_INT_ENABLE | \
                                                        AD5755_DAC_DC_DC | \
                                                        AD5755_DAC_OUTEN)) );
                CONSOLE_Print("Channel %d is OFF\r\n", \
                                (int)channel, (regValue & ~(AD5755_DAC_INT_ENABLE \
                                                          | AD5755_DAC_DC_DC \
                                                          | AD5755_DAC_OUTEN)));
            }
        }
    }
    else
    {
        DisplayError(4);
    }
}

/**************************************************************************//**
* @brief Function of the power? command, verify if the selected channel is ON
*
* @param param      - array of the command arguments
*        param[0]   - selected channel
* @param paramNo    - number of the arguments
* @return void
******************************************************************************/
void GetPower(double* param, char paramNo)
{
    char channel = 0;
    short regValue = 0;
    /* Validate parameters */
    if(paramNo >= 1)
    {   /* Validate channel */
        paramLimit(&param[0], 0, 3);
        channel = param[0];

        regValue = (short)AD5755_GetRegisterValue(AD5755_RD_CTRL_REG((char)channel));
        if(regValue == -1)
        {
            CONSOLE_Print("AD5755 CRC ERROR!\r\n");
        }
        else
        {
            if((regValue & AD5755_DAC_DC_DC) &&
               (regValue & AD5755_DAC_OUTEN) &&
               (regValue & AD5755_DAC_INT_ENABLE))
            {
                CONSOLE_Print("Channel %d is ON\r\n", \
                                            (int)channel, regValue );
            }
            else
            {
                CONSOLE_Print("Channel %d is OFF\r\n", \
                                            (int)channel, regValue );
            }
        }
    }
    else
    {
        DisplayError(5);
    }
}

/**************************************************************************//**
* @brief Function of the range= command. Changing and reprogramming the output
*        range, the value of the output will be reseted.
*
* @param param      - array of the command arguments
*        param[0]   - selected channel
*        param[1]   - selected range, accepted value:
*                   'AD5755_R_0_5_V' - 0V to 5V voltage range (default)
*                   'AD5755_R_0_10_V' - 0V to 10V voltage range
*                   'AD5755_R_M5_P5_V' - +/-5V voltage range
*                   'AD5755_R_M10_P10_V' - +/-10V voltage range
*                   'AD5755_R_4_20_MA' - 4 mA to 20 mA current range
*                   'AD5755_R_0_20_MA' - 0 mA to 20 mA current range
*                   'AD5755_R_0_24_MA' - 0 mA to 24 mA current range
* @param paramNo    - number of the arguments
* @return void
******************************************************************************/
void SetRange(double* param, char paramNo)
{
    unsigned short regValue = 0;
    unsigned char channel = 0;
    unsigned short range = 0;

    if(paramNo >= 2)
    {   /* Validate channel */
        paramLimit(&param[0], 0, 3);
        paramLimit(&param[1], 0, 6);
        channel = (unsigned char)param[0];
        range = (unsigned char)param[1];

        regValue = (unsigned short)AD5755_GetRegisterValue(AD5755_RD_CTRL_REG((unsigned char)channel));
        if(regValue == -1)
        {
            CONSOLE_Print("AD5755 CRC ERROR!\r\n");
        }
        else
        {
            /* set the output to 0V/0mA */
            AD5755_SetRegisterValue(AD5755_DREG_WR_DAC, channel, 0x0);
            // Clear the DAC_R bits

            /* Disable the output */
            regValue &= ~AD5755_DAC_OUTEN;
            AD5755_SetControlRegisters( AD5755_CREG_DAC,
                                        channel,
                                        regValue);
            /* Set up the new output range */
            regValue &= 0xFFFFFFF8;
            regValue |= range;
            AD5755_SetControlRegisters( AD5755_CREG_DAC,
                                        channel,
                                        regValue);
            /* Write to data DAC register */
            AD5755_SetRegisterValue(AD5755_DREG_WR_DAC, channel, 0x0);
            TIME_DelayUs(200);
            /* Enable the data output */
            regValue |= AD5755_DAC_OUTEN;
            AD5755_SetControlRegisters( AD5755_CREG_DAC,
                                        channel,
                                        regValue);
            CONSOLE_Print("The range of channel %d was changed\r\n", channel);
        }
    }
    else
    {
        DisplayError(6);
    }
}

/**************************************************************************//**
* @brief Function of the range? command.
*
* @param param      - array of the command arguments
*        param[0]   - selected channel
* @param paramNo    - number of the arguments
* @return void
******************************************************************************/
void GetRange(double* param, char paramNo)
{
    short regValue = 0;
    short range = 0;
    char channel = 0;
    if(paramNo >= 1)
    {   /* Validate channel */
        paramLimit(&param[0], 0, 3);
        channel = param[0];

        regValue = (short)AD5755_GetRegisterValue(AD5755_RD_CTRL_REG((char)channel));
        if(regValue == -1)
        {
            CONSOLE_Print("AD5755 CRC ERROR!\r\n");
        }
        else
        {
            range = regValue & 0x07;
            switch(range)
            {
                case AD5755_R_0_5_V :
                {
                    CONSOLE_Print("Range of channel %d is 0V to 5V.\r\n", \
                                                                (int)channel);
                    break;
                }
                case AD5755_R_0_10_V:
                {
                    CONSOLE_Print("Range of channel %d is 0V to 10V.\r\n", \
                                                                (int)channel);
                    break;
                }
                case AD5755_R_M5_P5_V:
                {
                    CONSOLE_Print("Range of channel %d is +/-5V.\r\n", \
                                                                (int)channel);
                    break;
                }
                case AD5755_R_M10_P10_V:
                {
                    CONSOLE_Print("Range of channel %d is +/-10V.\r\n", \
                                                                (int)channel);
                    break;
                }
                case AD5755_R_4_20_MA:
                {
                    CONSOLE_Print("Range of channel %d is 4mA to 20mA.\r\n", \
                                                                (int)channel);
                    break;
                }
                case AD5755_R_0_20_MA:
                {
                    CONSOLE_Print("Range of channel %d is 0mA to 20mA.\r\n", \
                                                                (int)channel);
                    break;
                }
                case AD5755_R_0_24_MA:
                {
                    CONSOLE_Print("Range of channel %d is 0mA to 24mA.\r\n", \
                                                                (int)channel);
                    break;
                }
                default:
                {
                    CONSOLE_Print("Invalid range!\r\n");
                    break;
                }
            }
        }
    }
    else
    {
        DisplayError(7);
    }
}

/**************************************************************************//**
* @brief Function of the voltage? command. Set up a desired voltage level for a
*        selected channel. If the channel is powered off or it's configured
*        to a current output the function throws an error message.
*
* @param param      - array of the command arguments
*        param[0]   - selected channel
*        param[1]   - the value of the voltage multiplied by 1000
* @param paramNo    - number of the arguments
* @return void
******************************************************************************/
void SetVoltage(double* param, char paramNo)
{
    long buffer = 0;
    unsigned short cntrValue = 0;
    short lowerLimit = 0;
    short upperLimit = 0;
    unsigned char channel = 0;
    float voltage = 0;

    if(paramNo >= 2)
        {/* Validate channel */
        paramLimit(&param[0], 0, 3);
        channel = param[0];

        voltage = param[1] / 1000;

        /* Validate data */
        buffer = AD5755_GetRegisterValue(AD5755_RD_CTRL_REG((char)channel));
        if(buffer == -1)
        {
            CONSOLE_Print("AD5755 CRC ERROR!\r\n");
        }
        else
        {
            cntrValue = (unsigned short)buffer;
            if( ((cntrValue & AD5755_DAC_INT_ENABLE) != 0) && \
                ((cntrValue & AD5755_DAC_OUTEN) != 0) && \
                ((cntrValue & AD5755_DAC_DC_DC) != 0) && \
                ((cntrValue & AD5755_DAC_R(4)) == 0) )
            {   /* Define the lower and upper limit in function of the range */
                lowerLimit = ((cntrValue & AD5755_DAC_R(2)) == 0) ? 0 :\
                             ((cntrValue & AD5755_DAC_R(1)) == 0) ? -5 :\
                               -10;
                upperLimit = ((cntrValue & AD5755_DAC_R(1)) == 0) ? 5 : 10;
                if(voltage < (float)lowerLimit)
                {
                    voltage = lowerLimit;
                }
                if(voltage > (float)upperLimit)
                {
                    voltage = upperLimit;
                }
                /* Write data to register */
                AD5755_SetVoltage(channel, voltage);
                CONSOLE_Print("Output channel %d was updated to %1.2fV\r\n", \
                                                            channel, voltage);
            }
            else
            {
                CONSOLE_Print("Can not perform the command, incompatible \
configuration. Check out if the \r\n channel is enabled, or the output is set \
up to current mode.\r\n");
            }
        }
    }
    else
    {
        DisplayError(8);
    }
}

/**************************************************************************//**
* @brief Function of the voltage? command. Read back the voltage level of a
*        selected channel. If the channel is powered off or it's configured
*        to a current output the function throws an error message.
*
* @param param      - array of the command arguments
*        param[0]   - selected channel
* @param paramNo    - number of the arguments
*
* @return void
******************************************************************************/
void GetVoltage(double* param, char paramNo)
{
    unsigned short cntrValue = -1;
    unsigned long regValue = 0;
    unsigned char channel = 0;
    unsigned short range = 0;
    float realValue = 0;

    if(paramNo >= 1)
    {   /* Validate channel */
        paramLimit(&param[0], 0, 3);
        channel = param[0];

        cntrValue = (short)AD5755_GetRegisterValue(AD5755_RD_CTRL_REG((char)channel));
        range     = cntrValue & 0x7;
        /* Check the output range of the channel */
        if( ((cntrValue & AD5755_DAC_INT_ENABLE) != 0) && \
            ((cntrValue & AD5755_DAC_OUTEN) != 0) && \
            ((cntrValue & AD5755_DAC_DC_DC) != 0) && \
            ((cntrValue & AD5755_DAC_R(4)) == 0) )
        {
            regValue = AD5755_GetRegisterValue(AD5755_RD_DATA_REG((char)channel));
            if(regValue == -1)
            {
                CONSOLE_Print("AD5755 CRC ERROR!\r\n");
            }
            else
            {
                switch(range)
                {
                    case 0:
                    {
                        realValue = (float)(regValue * 5) / MAXVALUE;
                        CONSOLE_Print("Channel %d = %.2f V\r\n", (int)channel,\
                                             realValue);
                        break;
                    }
                    case 1:
                    {
                        realValue = (float)(regValue * 10) / MAXVALUE;
                        CONSOLE_Print("Channel %d = %.2f V\r\n", (int)channel,\
                                            realValue);
                        break;
                    }
                    case 2:
                    {
                        realValue = ((float)(regValue * 10) / MAXVALUE) - 5;
                        CONSOLE_Print("Channel %d = %.2fV\r\n", (int)channel,\
                                            realValue);
                        break;
                    }
                    case 3:
                    {
                        realValue = ((float)(regValue * 20) / MAXVALUE) - 10;
                        CONSOLE_Print("Channel %d = %.2fV\r\n", (int)channel,\
                                            realValue );
                        break;
                    }
                    default:
                    {
                        CONSOLE_Print("Invalid range!\r\n");
                        break;
                    }
                }
            }
        }
        else
        {
            CONSOLE_Print("Can not perform the command, incompatible \
configuration. Check out if the \r\n channel is enabled, or the output is set \
up to current mode.\r\n");
        }
    }
    else
    {
        DisplayError(9);
    }
}

/**************************************************************************//**
* @brief Function of the current? command. Set up a desired current for a
*        selected channel. If the channel is powered off or it's configured
*        to voltage output the function throws an error message.
*
* @param param      - array of the command arguments
*        param[0]   - selected channel
*        param[1]   - the value of the current(mA) multiplied by 1000
* @param paramNo    - number of the arguments
* @return void
******************************************************************************/
void SetCurrent(double* param, char paramNo)
{
    unsigned short cntrValue = -1;
    short lowerLimit =  0;
    short upperLimit =  0;
    unsigned char channel =  0;
    float current =  0;
    if(paramNo >= 2)
    {   /* Validate channel */
        paramLimit(&param[0], 0, 3);
        channel = param[0];
        current = param[1] / 1000;

        cntrValue = (short)AD5755_GetRegisterValue(AD5755_RD_CTRL_REG((char)channel));
        if(cntrValue == -1)
        {
            CONSOLE_Print("AD5755 CRC ERROR!\r\n");
        }
        else
        {
            if( ((cntrValue & AD5755_DAC_INT_ENABLE) != 0) && \
                ((cntrValue & AD5755_DAC_OUTEN) != 0) && \
                ((cntrValue & AD5755_DAC_DC_DC) != 0) && \
                ((cntrValue & AD5755_DAC_R(4)) != 0) )
            {   /* Define the lower and upper limit in function of the range */
                lowerLimit = ( ((cntrValue & AD5755_DAC_R(2)) == 0) &&
                               ((cntrValue & AD5755_DAC_R(1)) == 0) ) ? 4 : 0;
                upperLimit = ((cntrValue & AD5755_DAC_R(2)) != 0) ? 24 : 20;
                if(current < lowerLimit)
                {
                    current = lowerLimit;
                }
                if(current > upperLimit)
                {
                    current = upperLimit;
                }
                AD5755_SetCurrent(channel, current);
                CONSOLE_Print("Output channel %d was updated to %1.2fmA\r\n", \
                                                            channel, current);
            }
            else
            {
                CONSOLE_Print("Can not perform the command, incompatible \
configuration. Check out if the \r\n channel is enabled, or the output is set \
up to current mode.\r\n");
            }
        }
    }
    else
    {
        DisplayError(10);
    }
}

/**************************************************************************//**
* @brief Function of the current? command. Read back the current of a
*        selected channel. If the channel is powered off or it's configured
*        to a voltage output the function throws an error message.
*
* @param param      - array of the command arguments
*        param[0]   - selected channel
* @param paramNo    - number of the arguments
*
* @return void
******************************************************************************/
void GetCurrent(double* param, char paramNo)
{
    unsigned short   cntrValue   = -1;
    unsigned short   regValue    =  0;
    unsigned char    channel     =  0;
    unsigned short   range       =  0;
    float   realValue   =  0;

    if(paramNo >= 1)
    {   /* Validate channel */
        paramLimit(&param[0], 0, 3);
        channel = param[0];

        cntrValue = (short)AD5755_GetRegisterValue(AD5755_RD_CTRL_REG((char)channel));
        range = cntrValue & 0x7;
        if(cntrValue == -1)
        {
            CONSOLE_Print("AD5755 CRC ERROR!\r\n");
        }
        else
        {
            if( ((cntrValue & AD5755_DAC_INT_ENABLE) != 0) && \
                ((cntrValue & AD5755_DAC_OUTEN) != 0) && \
                ((cntrValue & AD5755_DAC_DC_DC) != 0) && \
                ((cntrValue & AD5755_DAC_R(4)) != 0) )
            {
                regValue = AD5755_GetRegisterValue(AD5755_RD_DATA_REG((char)channel));
                if(regValue == -1)
                {
                    CONSOLE_Print("AD5755 CRC ERROR!\r\n");
                }
                else
                {
                    switch(range)
                    {
                        case 4:
                        {
                            realValue = ((float)(regValue * 16) / MAXVALUE) + 4;
                            CONSOLE_Print("Channel %d = %0.2fmA\r\n", \
                                (int)channel, realValue);
                            break;
                        }
                        case 5:
                        {
                            realValue = (float)(regValue * 20) / MAXVALUE;
                            CONSOLE_Print("Channel %d = %0.2fmA\r\n", \
                                (int)channel, realValue);
                            break;
                        }
                        case 6:
                        {
                            realValue = (float)(regValue * 24) / MAXVALUE;
                            CONSOLE_Print("Channel %d = %0.2fmA\r\n", \
                                (int)channel, realValue);
                            break;
                        }
                        default:
                        {
                            CONSOLE_Print("Invalid range!\r\n");
                            break;
                        }
                    }
                }
            }
            else
            {
                CONSOLE_Print("Can not perform the command, incompatible \
configuration. Check out if the \r\n channel is enabled, or the output is set \
up to current mode.\r\n");
            }
        }
    }
    else
    {
        DisplayError(11);
    }
}

/**************************************************************************//**
* @brief Function of the getStatus! command. Read back the Status register and
*        throws an error message is there is any faults or errors in the system.
*
* @param param      - None
* @param paramNo    - number of the arguments
*
* @return void
******************************************************************************/
void EvaluateStatus(double* param, char paramNo)
{
    unsigned short range[4] = {0, 0, 0, 0};
    unsigned short status = 0;
    char crcError = 0;
    int i = 0;
    char fault = 0;

    /* Read back the range of each channel */
    for(i=0; i<4; i++)
    {
        long rangeBuf = AD5755_GetRegisterValue(AD5755_RD_CTRL_REG(i)) & 0x7;
        if(rangeBuf != ERROR)
        {
            range[i] = (unsigned short)rangeBuf;
        }
        else
        {
            CONSOLE_Print("AD5755 CRC ERROR!\r\n");
            crcError = ERROR;
            break;
        }
    }
    if(crcError == SUCCESS)
    {
        /* Read back the status register */
        status = (unsigned short)AD5755_GetRegisterValue(AD5755_RD_STATUS_REG);
        for(i=0; i<4; i++)
        {   /* Check if there any dc-to-dc converter warnings */
            if( (status & AD5755_STATUS_DC_DC(i)) != 0 )
            {
                /* channel has voltage output */
                if(range[i] <= 3)
                {
                    CONSOLE_Print("WARNING [ch%d]: The dc-to-dc converter is \
unable to regulate to 15V as expected!\r\n", i);
                    fault++;
                }
                else /* channel has current output */
                {
                    CONSOLE_Print("WARNING [ch%d]: The dc-to-dc converter \
cannot maintain compliance (it may be reaching its Vmax voltage)\r\n", i);
                    fault++;
                }
            }
            /* Check if there are any fault detection at Vout pins */
            if( (status & AD5755_STATUS_VOUT_FAULT(i)) != 0)
            {
                CONSOLE_Print("ERROR [ch%d]: Fault detected on Vout pin!\r\n",\
                                                                         i);
                fault++;
            }
            /* Check if there are any fault detection at Iout pins */
            if( (status & AD5755_STATUS_IOUT_FAULT(i)) != 0)
            {
                CONSOLE_Print("ERROR [ch%d]: Fault detected on Iout pin!\r\n\
You have an open-loop circuit, put a resistor(1kOhm) between Iout and AGND.\r\n",\
                                                                             i);
                fault++;
            }
        }
        if( (status & AD5755_STATUS_PEC_ERROR) != 0 )
        {
            CONSOLE_Print("PEC ERROR: Denotes a PEC Error on the last data-word\
 received over the SPI\r\n");
            fault++;
        }
        if( (status & AD5755_STATUS_RAMP_ACTIVE) != 0 )
        {
            CONSOLE_Print("WARNING : One of the output channel is slewing. \
(slew rate control is enabled on at least one channel)\r\n");
            fault++;
        }
        if( (status & AD5755_STATUS_OVER_TEMP) != 0 )
        {
            CONSOLE_Print("ERROR : The device core temperature exceeds \
approximately 150`C\r\n");
            fault++;
        }
        if(fault == 0)
        {
            CONSOLE_Print("No faults or warnings.\r\n");
        }
    }
}

/**************************************************************************//**
* @brief Function of the testSPI! command. Read back the Status register and
*        throws an error message is there is any faults or errors in the system.
*
* @param param      - None
* @param paramNo    - number of the arguments
*
* @return void
******************************************************************************/
void TestSpi(double* param, char paramNo)
{
    long statusBuf = 0;
    unsigned short statusReg = 0;
    /* Set the D12 bit in the Software register */
    AD5755_SetControlRegisters( AD5755_CREG_SOFT,
                                0,
                               (1 << 12));
    /* Read back the status register to check the 'User Toggle' bit */
    statusBuf = AD5755_GetRegisterValue(AD5755_RD_STATUS_REG);
    if(statusBuf >= 0)
    {
        statusReg = (unsigned short)statusBuf;
        if(statusReg & AD5755_STATUS_USER_BIT)
        {
            CONSOLE_Print("SPI test PASSED!\r\n");
        }
        else
        {
            CONSOLE_Print("SPI test FAILED!\r\n");
        }
    }
    else
    {
        CONSOLE_Print("AD5755 CRC ERROR!\r\n");
    }
}
