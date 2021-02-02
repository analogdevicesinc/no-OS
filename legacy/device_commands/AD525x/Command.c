/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for ad525x driver.
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
#include "ad525x.h"
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
        .name = "rdac=",
        .description = "Load the wiper register with a given value.",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t0 to 3 in function of the device type\r\n\
\tvalue:\r\n\
\t0 .. 255 or 0 .. 1024 - wiper register value",
        .example = "To load the wiper register of channel B with 128, \
type: rdac=1 128",
    },
    [2] = {
        .name = "rdac?",
        .description = "Read back the value of the wiper register.",
        .acceptedValue = "Accepted values:\r\n\
\tchannel:\r\n\
\t0 .. 3 in function of the device type",
        .example = "To read back the value of the wiper register of channel B, \
type: rdac?1",
    },
    [3] = {
        .name = "reset!",
        .description = "Reset all wiper register to its stored values",
        .acceptedValue = "None",
        .example = "",
    },
    [4] = {
        .name = "restore=",
        .description = "Restore the specified wiper register setting form the \
memory",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t0 to 3 in function of the device type",
        .example = "To restore the wiper register of channel B, \
type: restore=1",
    },
    [5] = {
        .name = "save=",
        .description = "Save the given wiper register settings to the memory",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t0 to 3 in function of the device type",
        .example = "To save the wiper register of channel B, \
type: save=1",
    },
    [6] = {
        .name = "writemem=",
        .description = "Write to one of the user memory address",
        .acceptedValue = "Accepted value:\r\n\
\taddress:\r\n\
\t0x0 to 0xF (may vary at some devices)\r\n\
\tdata:\r\n\
\t0 .. 255 or 0 .. 1023 in function of the position number of the current device.",
        .example = "To write 127 to address 0x8 into the EEMEM memory, type: \
writemem=0x8 127",
    },
    [7] = {
        .name = "readmem=",
        .description = "Read data from the EEMEM memory.",
        .acceptedValue = "Accepted value:\r\n\
\taddress:\r\n\
\t0x0 to 0xF (may vary at some devices)",
        .example = "To read the 0x8 address from the EEMEM memory, type: \
readmem=0x8",
    },
    [8] = {
        .name = "decrdacdb=",
        .description = "Decrement a given wiper register by 6dB.",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t0 to 3 in function of the device type",
        .example = "To decrement the wiper register of channel A by 6dB, \
type: decrdacdb=0",
    },
    [9] = {
        .name = "decrdacdball!",
        .description = "Decrement all wiper register by 6dB.",
        .acceptedValue = "None",
        .example = "To decrement all the wiper register by 6dB, \
type: decrdacdball!",
    },
    [10] = {
        .name = "decrdac=",
        .description = "Decrement a given wiper register by one.",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t0 to 3 in function of the device type",
        .example = "To decrement the wiper register of channel A by one, \
type: decrdac=0",
    },
    [11] = {
        .name = "decrdacall!",
        .description = "Decrement all wiper register by one.",
        .acceptedValue = "None",
        .example = "To decrement all the wiper register by one, \
type: decrdacall!",
    },
    [12] = {
        .name = "incrdacdb=",
        .description = "Increment a given wiper register by 6dB.",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t0 to 3 in function of the device type",
        .example = "To increment the wiper register of channel A by 6dB, \
type: incrdacdb=0",
    },
    [13] = {
        .name = "incrdacdball!",
        .description = "Increment all wiper register by 6dB.",
        .acceptedValue = "None",
        .example = "To increment all the wiper register by 6dB, \
type: incrdacdball!",
    },
    [14] = {
        .name = "incrdac=",
        .description = "Increment a given wiper register by one.",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t0 to 3 in function of the device type",
        .example = "To increment the wiper register of channel A by one, \
type: incrdacdb=0",
    },
    [15] = {
        .name = "incrdacall!",
        .description = "Increment all wiper register by one.",
        .acceptedValue = "None",
        .example = "To increment all the wiper register by one, \
type: incrdacall!",
    },
    [16] = {
        .name = "setwp=",
        .description = "Set the state of the Write Protect (WP) pin",
        .acceptedValue = "Accepted value:\r\n\
\tdesire state of the pin:\r\n\
\t0 - inactive\r\n\
\t1 - active",
        .example = "To activate the WP pin, type: setwp=1",
    },
    [17] = {
        .name = "getwp?",
        .description = "Return the current value of the Write Protect (WP) pin",
        .acceptedValue = "None",
        .example = "",
    },
    [18] = {
        .name = "sethwreset=",
        .description = "Set the state of the Hardware Override Preset (PR) pin",
        .acceptedValue = "Accepted value:\r\n\
\tdesire state of the pin:\r\n\
\t0 - inactive\r\n\
\t1 - active",
        .example = "To activate the hardware RESET pin, type: sethwreset=1",
    },
    [19] = {
        .name = "gethwreset?",
        .description = "Return the current value of the Hardware Override \
Preset (PR) pin",
        .acceptedValue = "None",
        .example = "",
    },
    [20] = {
        .name = "tolerance=",
        .description = "Read one of the Tolerance register.",
        .acceptedValue = "Accepted value:\r\n\
\tchannel:\r\n\
\t0 to 7 in function of the device type",
        .example = "To read the value of the first tolerance register, \
type: tolerance=0",
    }
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[21] = {GetHelp, SetRdac, GetRdac, DoReset, DoRestore, \
                               SaveSettings, WriteToMem, ReadFromMem, \
                               DecRdac6dB, DecAllRdac6dB, DecRdac, DecAllRdac,\
                               IncRdac6dB, IncAllRdac6dB, IncRdac, IncAllRdac, \
                               SetWrPin, GetWrPin, SetPrPin, GetPrPin, \
                               GetTolerance};

AD525X_type_t deviceType;

unsigned char deviceSlaveId = 1;
unsigned char channelNo = 0;
unsigned short maxPosition = 0;
/* Memory address limits */
unsigned char lowerAddressLimit = 0;
unsigned char upperAddressLimit = 0;
unsigned char nrToleranceReg = 0;
/* Buffers to store control pin's voltage level */
unsigned char writeProtectBuff = 0;
unsigned char overridePreset = 0;

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
For AD5232 type: 0\r\n\
For AD5235 type: 1\r\n\
For AD5252 type: 2\r\n\
For AD5254 type: 3\r\n\
For ADN2850 type: 4\r\n");

        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x34))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                    deviceType = ID_AD5232;
                    channelNo = 2;
                    maxPosition = 255;
                    deviceSlaveId = 1;
                    lowerAddressLimit = 0x2;
                    upperAddressLimit = 0xF;
                    nrToleranceReg = 0;
                    break;
                case 0x31 :
                    deviceType = ID_AD5235;
                    channelNo = 2;
                    maxPosition = 1023;
                    deviceSlaveId = 1;
                    lowerAddressLimit = 0x2;
                    upperAddressLimit = 0xE;
                    nrToleranceReg = 1;
                    break;
                case 0x32 :
                    deviceType = ID_AD5252;
                    channelNo = 2;
                    maxPosition = 255;
                    deviceSlaveId = 1;
                    lowerAddressLimit = 0x4;
                    upperAddressLimit = 0xF;
                    nrToleranceReg = 4;
                    break;
                case 0x33 :
                    deviceType = ID_AD5254;
                    channelNo = 4;
                    maxPosition = 255;
                    deviceSlaveId = 0;
                    lowerAddressLimit = 0x4;
                    upperAddressLimit = 0xF;
                    nrToleranceReg = 8;
                    break;
                case 0x34 :
                    deviceType = ID_ADN2850;
                    channelNo = 2;
                    maxPosition = 1023;
                    deviceSlaveId = 1;
                    lowerAddressLimit = 0x2;
                    upperAddressLimit = 0xE;
                    nrToleranceReg = 1;
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
    DoDeviceLock();

    if(AD525X_Init(deviceType) == 0)
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
 * @brief Set wiper register value.
 *
 * @param - param[0] - The number of the channel, which have to updated
 *        - param[1] - The desired value of the resistor
 *
 * @return - NONE
 *
*******************************************************************************/
void SetRdac(double* param, char paramNo)
{
    unsigned char channel = 0;
    unsigned short data = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 2)
    {
        paramLimit(&param[0], 0, channelNo-1);
        paramLimit(&param[1], 0, maxPosition);
        channel = (unsigned char)param[0];
        data = (unsigned short)param[1];
        /* Write to RDAC register */
        AD525X_WriteRdac(deviceSlaveId, channel, data);
        CONSOLE_Print("RDAC[%d]=%d\r\n", channel, data);
    }
    else
    {
        /* Display error messages */
        DisplayError(1);
    }
}

/***************************************************************************//**
 * @brief Get wiper register value.
 *
 * @param - param[0] - The number of the channel, which have to updated
 *
 * @return - NONE
 *
*******************************************************************************/
void GetRdac(double* param, char paramNo)
{
    unsigned char channel = 0;
    unsigned short data = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, channelNo-1);
        channel = (unsigned char)param[0];
        data = AD525X_ReadRdac(deviceSlaveId, channel);
        CONSOLE_Print("RDAC[%d]=%d\r\n", channel, data);
    }
    else
    {
        /* Display error messages */
        DisplayError(2);
    }
}

/***************************************************************************//**
 * @brief Reset all wiper register to its stored values.
 *
 * @param - NONE
 *
 * @return - NONE
 *
*******************************************************************************/
void DoReset(double* param, char paramNo)
{
    AD525X_WriteCommand(deviceSlaveId,
                        AD525X_CMD_RESET,
                        0x0);
    CONSOLE_Print("Reset done.\r\n");
}

/***************************************************************************//**
 * @brief Restore the specified wiper register setting form the memory
 *
 * @param - param[0] - The number of the channel, which have to updated
 *
 * @return - NONE
 *
*******************************************************************************/
void DoRestore(double* param, char paramNo)
{
    unsigned char channel = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, (channelNo - 1));
        channel = (unsigned char)param[0];
        AD525X_WriteCommand(deviceSlaveId,
                            AD525X_CMD_MEM2RDAC,
                            channel);
        CONSOLE_Print("The channel %d was restored to its saved value.\r\n", \
                                                                    channel);
    }
    else
    {
        /* Display error messages */
        DisplayError(4);
    }
}

/***************************************************************************//**
 * @brief Save the current wiper register settings to the memory
 *
 * @param - param[0] - The number of the channel, which have to updated
 *
 * @return - NONE
 *
*******************************************************************************/
void SaveSettings(double* param, char paramNo)
{
    unsigned char channel = 0;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, (channelNo - 1));
        channel = (unsigned char)param[0];
        AD525X_WriteCommand(deviceSlaveId,
                            AD525X_CMD_RDAC2MEM,
                            channel);
        CONSOLE_Print("The value of channel %d was saved to memory.\r\n", \
                                                                    channel);
    }
    else
    {
        /* Display error messages */
        DisplayError(5);
    }
}

/***************************************************************************//**
 * @brief Write to a given memory address
 *
 * @param - param[0] - EEMEM memory address
 *        - param[1] - the value which will be written into the memory
 *
 * @return - NONE
 *
*******************************************************************************/
void WriteToMem(double* param, char paramNo)
{
    unsigned char address = 0;
    unsigned short data = 0;

    if(paramNo >= 2)
    {
        paramLimit(&param[0], lowerAddressLimit, upperAddressLimit);
        paramLimit(&param[1], 0, maxPosition);
        address = (unsigned char)param[0];
        data = (unsigned short)param[1];
        AD525X_WriteMem(deviceSlaveId,
                        address,
                        data);
        CONSOLE_Print("EEMEM[0x%.2x]=%d\r\n", address, data);
    }
    else
    {
        /* Display error messages */
        DisplayError(6);
    }
}

/***************************************************************************//**
 * @brief Read from a given memory address
 *
 * @param - param[0] - EEMEM memory address
  *
 * @return - The data stored at the given memory address
 *
*******************************************************************************/
void ReadFromMem(double* param, char paramNo)
{
    unsigned char address = 0;
    unsigned short data = 0;

    if(paramNo >= 1)
    {
        paramLimit(&param[0], lowerAddressLimit, upperAddressLimit);
        address = (unsigned char)param[0];
        data = AD525X_ReadMem(deviceSlaveId,
                              address);
        CONSOLE_Print("EEMEM[0x%.2x]=%d\r\n", address, data);
    }
    else
    {
        /* Display error messages */
        DisplayError(7);
    }
}

/***************************************************************************//**
 * @brief Decrement a wiper register by 6dB
 *
 * @param - param[0] - The number of the channel, which have to updated
 *
 * @return - NONE
 *
*******************************************************************************/
void DecRdac6dB(double* param, char paramNo)
{
    unsigned char channel = 0;

    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, (channelNo - 1));
        channel = (unsigned char)param[0];
        AD525X_WriteCommand(deviceSlaveId,
                            AD525X_CMD_DECRDAC_6DB,
                            channel);
        CONSOLE_Print("The value of channel %d was decremented by 6 dB.\r\n", \
                                                                    channel);
    }
    else
    {
        /* Display error messages */
        DisplayError(8);
    }
}

/***************************************************************************//**
 * @brief Decrement all wiper register by 6dB
 *
 * @param - NONE
 *
 * @return - NONE
 *
*******************************************************************************/
void DecAllRdac6dB(double* param, char paramNo)
{
    AD525X_WriteCommand(deviceSlaveId,
                        AD525X_CMD_DECALLRDAC_6DB,
                        0x0);
    CONSOLE_Print("All channels were decremented by 6 dB.\r\n");
}

/***************************************************************************//**
 * @brief Decrement a wiper register by one
 *
 * @param - param[0] - The number of the channel, which have to updated
 *
 * @return - NONE
 *
*******************************************************************************/
void DecRdac(double* param, char paramNo)
{
    unsigned char channel = 0;

    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, (channelNo - 1));
        channel = (unsigned char)param[0];
        AD525X_WriteCommand(deviceSlaveId,
                            AD525X_CMD_DECRDAC_ONE,
                            channel);
        CONSOLE_Print("The value of channel %d was decremented by one.\r\n", \
                                                                    channel);
    }
    else
    {
        /* Display error messages */
        DisplayError(10);
    }
}

/***************************************************************************//**
 * @brief Decrement all wiper register by one
 *
 * @param - NONE
 *
 * @return - NONE
 *
*******************************************************************************/
void DecAllRdac(double* param, char paramNo)
{
    AD525X_WriteCommand(deviceSlaveId,
                        AD525X_CMD_DECALLRDAC_ONE,
                        0x0);
    CONSOLE_Print("All channels were decremented by one.\r\n");
}

/***************************************************************************//**
 * @brief Increment a wiper register by 6dB
 *
 * @param - param[0] - The number of the channel, which have to updated
 *
 * @return - NONE
 *
*******************************************************************************/
void IncRdac6dB(double* param, char paramNo)
{
    unsigned char channel = 0;

    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, (channelNo - 1));
        channel = (unsigned char)param[0];
        AD525X_WriteCommand(deviceSlaveId,
                            AD525X_CMD_INCRDAC_6DB,
                            channel);
        CONSOLE_Print("The value of channel %d was incremented by 6 dB.\r\n", \
                                                                    channel);
    }
    else
    {
        /* Display error messages */
        DisplayError(12);
    }
}

/***************************************************************************//**
 * @brief Increment all wiper register by 6dB
 *
 * @param - NONE
 *
 * @return - NONE
 *
*******************************************************************************/
void IncAllRdac6dB(double* param, char paramNo)
{
    AD525X_WriteCommand(deviceSlaveId,
                        AD525X_CMD_INCALLRDAC_6DB,
                        0x0);
    CONSOLE_Print("All channels were incremented by 6 dB.\r\n");
}

/***************************************************************************//**
 * @brief Increment a wiper register by one
 *
 * @param - param[0] - The number of the channel, which have to updated
 *
 * @return - NONE
 *
*******************************************************************************/
void IncRdac(double* param, char paramNo)
{
    unsigned char channel = 0;

    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, (channelNo - 1));
        channel = (unsigned char)param[0];
        AD525X_WriteCommand(deviceSlaveId,
                            AD525X_CMD_INCRDAC_ONE,
                            channel);
        CONSOLE_Print("The value of channel %d was incremented by one.\r\n", \
                                                                    channel);
    }
    else
    {
        /* Display error messages */
        DisplayError(14);
    }
}

/***************************************************************************//**
 * @brief Increment all wiper register by one
 *
 * @param - param[0] - The number of the channel, which have to updated
 *
 * @return - NONE
 *
*******************************************************************************/
void IncAllRdac(double* param, char paramNo)
{
    AD525X_WriteCommand(deviceSlaveId,
                        AD525X_CMD_INCALLRDAC_ONE,
                        0x0);
    CONSOLE_Print("All channels were incremented by one.\r\n");
}

/***************************************************************************//**
 * @brief Set the state of the Write Protect (WP) pin. When active low, WP
 *        prevents any changes to the present register contents, except PR,
 *        Command Instruction 1, and Command Instruction 8, which refresh the
 *        RDACx register from EEMEM.
 *
 * @param - param[0] - The desired state of the WR pin
 *                   - '0' - WR pin is inactive, voltage level high
 *                   - '1' - WR pin is active, voltage level low
 *
 * @return - NONE
 *
*******************************************************************************/
void SetWrPin(double* param, char paramNo)
{
    if(paramNo >= 1)
    {
        paramLimit(&param[0], 0, 1);
        if((param[0] == 0) && (writeProtectBuff == 1)) // inactivate
        {
            /* NOP instruction before returning WP to HIGH */
            AD525X_WriteCommand(deviceSlaveId,
                                AD525X_CMD_NOP,
                                0);
            TIME_DelayUs(50);
            AD525X_WP_BF_HIGH;      // change the logical state of the pin
            writeProtectBuff = 0;   // save the new state into the buffer
            CONSOLE_Print("WP is inactive.\r\n");
        }
        else if((param[0] == 1) && (writeProtectBuff == 0)) // activate
        {
            AD525X_WP_BF_LOW;
            writeProtectBuff = 1;
            CONSOLE_Print("WP was activated.\r\n");
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(16);
    }
}

/***************************************************************************//**
 * @brief Get the current state of the Write Protect (WP) pin.
 *
 * @param - NONE
 *
 * @return - NONE
 *
*******************************************************************************/
void GetWrPin(double* param, char paramNo)
{
    if(writeProtectBuff)
    {
        CONSOLE_Print("The WR pin is at active low state.\r\n");
    }
    else
    {
        CONSOLE_Print("The WR pin is at inactive high state.\r\n");
    }
}

/***************************************************************************//**
 * @brief Set the state of the Hardware Override Preset (PR) pin. Refresh the
 *        scratch pad register with current contents of the EEMEMx register.
 *
 * @param - param[0] - The desired state of the WR pin
 *                   - '1' - WR pin is inactive, voltage level high
 *                   - '0' - WR pin is active, voltage level low
 *
 * @return - NONE
 *
*******************************************************************************/
void SetPrPin(double* param, char paramNo)
{
    if((deviceType == ID_AD5252) || (deviceType == ID_AD5254))
    {
        CONSOLE_Print("This function is not supported by the current device.\r\n");
    }
    else
    {
        if(paramNo >= 1)
        {
            paramLimit(&param[0], 0, 1);
            if((param[0] == 1) && (overridePreset == 0))        // activate
            {
                AD525X_RESET_LOW;
                overridePreset = 1;
                CONSOLE_Print("The hardware RESET is activated.\r\n");
            } else if((param[0] == 0) && (overridePreset == 1)) // inactivate
            {
                AD525X_RESET_HIGH;
                overridePreset = 0;
                CONSOLE_Print("The hardware RESET is inactive.\r\n");
            }
        }
        else
        {
            /* Display error messages */
            DisplayError(18);
        }
    }

}

/***************************************************************************//**
 * @brief Set the state of the Hardware Override Preset (PR) pin.
 *
 * @param - NONE
 *
 * @return - NONE
 *
*******************************************************************************/
void GetPrPin(double* param, char paramNo)
{
    if((deviceType == ID_AD5252) || (deviceType == ID_AD5254))
    {
        CONSOLE_Print("This function is not supported by the current device.\r\n");
    }
    else
    {
        if(overridePreset)
        {
            CONSOLE_Print("The RESET pin is at active low state.\r\n");
        }
        else
        {
            CONSOLE_Print("The RESET pin is at inactive high state.\r\n");
        }
    }
}

/***************************************************************************//**
 * @brief Read one of the Tolerance register.
 *
 * @param - param[0] - The relative address of the tolerance register.
 *                     The number of available tolerance register varying in
 *                     case of every device. Accepted value: (0 .. NrTolReg - 1)
 *
 * @return - NONE
 *
 * Note : To get a usable data, have to make some conversation with the result.
 *        You can find more information in the data sheet.
*******************************************************************************/
void GetTolerance(double* param, char paramNo)
{
    unsigned char address = 0;
    unsigned short data = 0;

    if(deviceType == ID_AD5232)
    {
        CONSOLE_Print("This function is not supported by the current device.\r\n");
    }
    else
    {
        if(paramNo >= 1)
        {
            /* Define the address in function of the device type */
            paramLimit(&param[0], 0, (nrToleranceReg - 1));
            switch(deviceType)
            {
                case ID_AD5235 :
                    address = 0xF + (unsigned char)param[0];
                    break;
                case ID_AD5252 :
                    address = (param[0]>1) ? (0x1C + (unsigned char)param[0]) :
                                             (0x1A + (unsigned char)param[0]);
                    break;
                case ID_AD5254 :
                    address = 0x18 + (unsigned char)param[0];
                    break;
                case ID_ADN2850 :
                    address = 0xF + (unsigned char)param[0];
                    break;
                default :
                    break;
            }
            data = AD525X_ReadMem(deviceSlaveId,
                                  address);
            CONSOLE_Print("TolReg[0x%02x]=0x%4x\r\n", \
                                                (unsigned char)param[0], data);
        }
        else
        {
            /* Display error messages */
            DisplayError(20);
        }
    }
}
