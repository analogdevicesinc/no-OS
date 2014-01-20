/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for CN0235.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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
#include "ad7280a.h"
#include "Communication.h"
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
        .name = "adcCode?",
        .description = "Displays the ADC code of one channel from selected \
device.",
        .acceptedValue = "Accepted values:\r\n\
\tdevice:\r\n\
\t0 - master\r\n\
\t1 - slave\r\n\
\tregister address:\r\n\
\t 0 - Cell Voltage 1\r\n\
\t 1 - Cell Voltage 2\r\n\
\t 2 - Cell Voltage 3\r\n\
\t 3 - Cell Voltage 4\r\n\
\t 4 - Cell Voltage 5\r\n\
\t 5 - Cell Voltage 6\r\n\
\t 6 - AUX ADC 1\r\n\
\t 7 - AUX ADC 2\r\n\
\t 8 - AUX ADC 3\r\n\
\t 9 - AUX ADC 4\r\n\
\t10 - AUX ADC 5\r\n\
\t11 - AUX ADC 6",
        .example = "To display the ADC code for Cell Voltage 4, master device,\
 type: adcCode?0 3",
    },
    [2] = {
        .name = "voltage?",
        .description = "Displays the input voltage of one channel from \
selected device.",
        .acceptedValue = "Accepted values:\r\n\
\tdevice:\r\n\
\t0 - master\r\n\
\t1 - slave\r\n \
\tregister address:\r\n\
\t 0 - Cell Voltage 1\r\n\
\t 1 - Cell Voltage 2\r\n\
\t 2 - Cell Voltage 3\r\n\
\t 3 - Cell Voltage 4\r\n\
\t 4 - Cell Voltage 5\r\n\
\t 5 - Cell Voltage 6\r\n\
\t 6 - AUX ADC 1\r\n\
\t 7 - AUX ADC 2\r\n\
\t 8 - AUX ADC 3\r\n\
\t 9 - AUX ADC 4\r\n\
\t10 - AUX ADC 5\r\n\
\t11 - AUX ADC 6",
        .example = "To display the input voltage for AUX ADC 5, slave device, \
type: voltage?1 10",
    },
    [3] = {
        .name = "register?",
        .description = "Displays the content of one register from selected \
device.",
        .acceptedValue = "Accepted values:\r\n\
\tdevice:\r\n\
\t0 - master\r\n\
\t1 - slave\r\n \
\tregister address:\r\n\
\t13 - Control high byte\r\n\
\t14 - Control low byte\r\n\
\t15 - Cell Overvoltage\r\n\
\t16 - Cell Undervoltage\r\n\
\t ... (see AD7280A_Datasheet p.28)\r\n\
\t29 - CNVST Control",
        .example = "To display the content of Alert Register(address=19), \
master device, type: register?0 19",
    },
    [4] = {
        .name = "register=",
        .description = "Sets the content of one register from selected \
device.",
        .acceptedValue = "Accepted values:\r\n\
\tdevice:\r\n\
\t0 - master\r\n\
\t1 - slave\r\n \
\tregister address:\r\n\
\t13 - Control high byte\r\n\
\t14 - Control low byte\r\n\
\t15 - Cell Overvoltage\r\n\
\t16 - Cell Undervoltage\r\n\
\t ... (see AD7280A_Datasheet p.28)\r\n\
\t29 - CNVST Control\r\n\
\tregister value:\r\n\
\t 0 .. 255 - value to be written inside the register.",
        .example = "To set the content of Cell Overvoltage Register(address=15), \
master device,  to 100, type: register=0 15 100",
},
    [5] = {
        .name = "selfTestAD7280A!",
        .description = "Performs the self test for both AD7280A devices on the \
board (one master and one slave).",
        .acceptedValue = "",
        .example = "",
    },
    [6] = {
        .name = "alertPinAD7280A?",
        .description = "Reads the status of Alert Pin from AD7280A.",
        .acceptedValue = "",
        .example = "",
    },
    [7] = {
        .name = "enableAD8280=",
        .description = "Enables/disables the AD8280 device.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - disable the AD8280 device\r\n\
\t1 - enable the AD8280 device",
        .example = "To enable AD8280 device, type: enableAD8280=1",
    },
    [8] = {
        .name = "alarmPinOvAD8280?",
        .description = "Reads the status of Overvoltage Alarm Pin from AD8280.",
        .acceptedValue = "",
        .example = "",
    },
    [9] = {
        .name = "alarmPinUvAD8280?",
        .description = "Reads the status of Undervoltage Alarm Pin from AD8280.",
        .acceptedValue = "",
        .example = "",
    },
    [10] = {
        .name = "selfTestAD8280!",
        .description = "Performs the self test for both AD8280 devices on the \
board (one master and one slave).",
        .acceptedValue = "",
        .example = "",
    },
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp, GetAdcCode, GetVoltage, GetRegister,
                             SetRegister, DoSelfTestAD7280A, GetAlertPinAD7280A,
                             SetEnableAD8280, GetAlarmPinOvAD8280,
                             GetAlarmPinUvAD8280, DoSelfTestAD8280};

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
    if(AD7280A_Init() == 0)
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
 * @brief Displays the output ADC code of one channel from selected device.
 *
 * @param param[0] - device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *        param[1] - register address
 *                   Example: 0x0 - Cell Voltage 1
 *                            0x1 - Cell Voltage 2
 *                            ...
 *                            0x5 - Cell Voltage 6
 *                            0x6 - AUX ADC 1
 *                            0x7 - AUX ADC 2
 *                            ...
 *                            0xB - AUX ADC 6
 *
 * @return None.
*******************************************************************************/
void GetAdcCode(double* param, char paramNo) // "adcCode?" command
{
    unsigned short adcCode = 0;
    unsigned char  devAddr;
    unsigned char  readReg;

    if (paramNo >= 2)
    {
        if (param[0] < 0)
        {
            param[0] = 0;
        }
        if (param[0] > 1)
        {
            param[0] = 1;
        }
        if (param[1] < 0)
        {
            param[1] = 0;
        }
        if (param[1] > 0xB)
        {
            param[1] = 0xB;
        }

        devAddr = (unsigned char)param[0];
        readReg = (unsigned char)param[1];
        adcCode = AD7280A_Read_Conversion(devAddr, readReg);
        if (adcCode >= 0)
        {
            /* Send feedback to user */
            CONSOLE_Print("ADC Code=%d for register address 0x%x, device %d.\r\n",
                          adcCode, readReg, devAddr);
        }
        else
        {
            CONSOLE_Print("An error occured during transmission.\r\n");
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(1);
    }
}

/***************************************************************************//**
 * @brief Displays the input voltage of one channel from selected device.
 *
 * @param param[0] - device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *        param[1] - register address
 *                   Example: 0x0 - Cell Voltage 1
 *                            0x1 - Cell Voltage 2
 *                            ...
 *                            0x5 - Cell Voltage 6
 *                            0x6 - AUX ADC 1
 *                            0x7 - AUX ADC 2
 *                            ...
 *                            0xB - AUX ADC 6
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) // "voltage?" command
{
    unsigned short adcCode = 0;
    unsigned char  devAddr;
    unsigned char  readReg;
    float          result = 0;

    if (paramNo >= 2)
    {
        if (param[0] < 0)
        {
            param[0] = 0;
        }
        if (param[0] > 1)
        {
            param[0] = 1;
        }
        if (param[1] < 0)
        {
            param[1] = 0;
        }
        if (param[1] > 0xB)
        {
            param[1] = 0xB;
        }

        devAddr = (unsigned char)param[0];
        readReg = (unsigned char)param[1];
        adcCode = AD7280A_Read_Conversion(devAddr, readReg);
        if (adcCode >= 0)
        {
            /*CONSOLE_Print("ADC Code=%d for register address 0x%x, device %d.\
\r\n", adcCode, readReg, devAddr);*/
            if (readReg <= 5)
            {
                result = AD7280A_Convert_Data(0, adcCode);
                /* Send feedback to user */
                CONSOLE_Print("Voltage=%.3f[V] for Cell Voltage %d, device %d.\
\r\n", result, (readReg + 1), devAddr);
            }
            else
            {
                result = AD7280A_Convert_Data(1, adcCode);
                /* Send feedback to user */
                CONSOLE_Print("Voltage=%.3f[V] for AUX ADC %d, device %d.\r\n",
                              result, (readReg - 5), devAddr);
            }
        }
        else
        {
            CONSOLE_Print("An error occured during transmission.\r\n");
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(2);
    }
}

/***************************************************************************//**
 * @brief Displays the content of one register from selected device.
 *
 * @param param[0] - device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *        param[1] - register address (only registers from Register Map with an
 *                   address equal or higher than 0x0D)
 *                   Example: 0x0D - Control high byte
 *                            0x0E - Control low byte
 *                            ... (see AD7280A_Datasheet p.28)
 *                            0x1D - CNVST Control
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) // "register?" command
{
    unsigned short contentReg = 0;
    unsigned char  devAddr;
    unsigned char  readReg;

    if (paramNo >= 2)
    {
        if (param[0] < 0)
        {
            param[0] = 0;
        }
        if (param[0] > 1)
        {
            param[0] = 1;
        }
        if (param[1] < 0x0D)
        {
            param[1] = 0x0D;
        }
        if (param[1] > 0x1D)
        {
            param[1] = 0x1D;
        }

        devAddr = (unsigned char)param[0];
        readReg = (unsigned char)param[1];
        contentReg = AD7280A_Read_Register(devAddr, readReg);
        if (contentReg >= 0)
        {
            /* Send feedback to user */
            CONSOLE_Print("Register content=0x%x for register address 0x%x, device \
%d.\r\n", contentReg, readReg, devAddr);
        }
        else
        {
            CONSOLE_Print("An error occured during transmission.\r\n");
        }
    }
    else
    {
        /* Display error messages */
        DisplayError(3);
    }
}

/***************************************************************************//**
 * @brief Sets the content of one register from selected device.
 *
 * @param param[0] - device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *        param[1] - register address (only registers from Register Map with an
 *                   address equal or higher than 0x0D)
 *                   Example: 0x0D - Control high byte
 *                            0x0E - Control low byte
 *                            ... (see AD7280A_Datasheet p.28)
 *                            0x1D - CNVST Control
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) // "register=" command
{
    unsigned char  devAddr;
    unsigned char  readReg;
    unsigned char  regVal;

    if (paramNo >= 3)
    {
        if (param[0] < 0)
        {
            param[0] = 0;
        }
        if (param[0] > 1)
        {
            param[0] = 1;
        }
        if (param[1] < 0x0D)
        {
            param[1] = 0x0D;
        }
        if (param[1] > 0x1D)
        {
            param[1] = 0x1D;
        }
        if (param[2] < 0)
        {
            param[2] = 0;
        }
        if (param[2] > 0xFF)
        {
            param[2] = 0xFF;
        }

        devAddr = (unsigned char)param[0];
        readReg = (unsigned char)param[1];
        regVal  = (unsigned char)param[2];
        AD7280A_Write_Register(devAddr, readReg, regVal);
        /* Send feedback to user */
        CONSOLE_Print("Register content=0x%x for register address 0x%x, device \
%d.\r\n", regVal, readReg, devAddr);
    }
    else
    {
        /* Display error messages */
        DisplayError(4);
    }
}

/***************************************************************************//**
 * @brief Performs the self test for both AD7280A devices on the board (one
 *        master and one slave).
 *
 * @param none.
 *
 * @return none.
*******************************************************************************/
void DoSelfTestAD7280A(double* param, char paramNo) // "selfTestAD7280A!" command
{
    float selfTestRegA = 0;
    float selfTestRegB = 0;

    AD7280A_Selftest_All(&selfTestRegA, &selfTestRegB);
    /* Send feedback to user */
    CONSOLE_Print("Self Test Master voltage=%.3f[V]\r\n",selfTestRegA);
    CONSOLE_Print("Self Test Slave  voltage=%.3f[V]\r\n",selfTestRegB);
}

/***************************************************************************//**
 * @brief Reads the status of Alert Pin from AD7280A.
 *
 * @param none.
 *
 * @return none.
*******************************************************************************/
void GetAlertPinAD7280A(double* param, char paramNo) // "alertPinAD7280A?" command
{
    char alert = -1;

    alert = AD7280A_Alert_Pin();
    /* Send feedback to user */
    CONSOLE_Print("Alert Pin=%d\r\n", alert);
}

/***************************************************************************//**
 * @brief Enables/disables the AD8280 device.
 *
 * @param param[0] - Enable pin status.
 *
 * @return none.
*******************************************************************************/
void SetEnableAD8280(double* param, char paramNo) // "enableAD8280=" command
{
    unsigned char enable;

    if (paramNo >= 1)
    {
        if (param[0] < 0)
        {
            param[0] = 0;
        }
        if (param[0] > 1)
        {
            param[0] = 1;
        }

        enable = (unsigned char)param[0];
        if (enable == 1)
        {
            GPIO3_HIGH; // ENBL_IN active
        }
        else
        {
            if (enable == 0)
            {
                GPIO3_LOW; // ENBL_IN inactive
            }
        }
        /* Send feedback to user */
        CONSOLE_Print("AD8280 ENABLE Pin=%d\r\n", enable);
    }
    else
    {
        /* Display error messages */
        DisplayError(7);
    }
}

/***************************************************************************//**
 * @brief Reads the status of Overvoltage Alarm Pin from AD8280.
 *
 * @param none.
 *
 * @return none.
*******************************************************************************/
void GetAlarmPinOvAD8280(double* param, char paramNo) // "alarmPinOvAD8280?" command
{
    char alarm = -1;

    alarm = (GPIO_GET_DATA(GPIO_BASEADDR) & GPIO_1_MASK) ? 1 : 0;
    /* Send feedback to user */
    CONSOLE_Print("Overvoltage Alarm Pin=%d\r\n", alarm);
}

/***************************************************************************//**
 * @brief Reads the status of Undervoltage Alarm Pin from AD8280.
 *
 * @param none.
 *
 * @return none.
*******************************************************************************/
void GetAlarmPinUvAD8280(double* param, char paramNo) // "alarmPinUvAD8280?" command
{
    char alarm = -1;

    alarm = (GPIO_GET_DATA(GPIO_BASEADDR) & GPIO_4_MASK) ? 1 : 0;
    /* Send feedback to user */
    CONSOLE_Print("Undervoltage Alarm Pin=%d\r\n", alarm);
}

/***************************************************************************//**
 * @brief Performs the self test for both AD8280 devices on the board (one
 *        master and one slave).
 *
 * @param none.
 *
 * @return none.
*******************************************************************************/
void DoSelfTestAD8280(double* param, char paramNo) // "selfTestAD8280!" command
{
    char alarmUV = -1;
    char alarmOV = -1;

    GPIO0_HIGH; // TEST_IN active
    /* Wait 4 seconds */
    TIME_DelayMs(2000);
    TIME_DelayMs(2000);
    GPIO0_LOW; // TEST_IN inactive

    alarmUV = (GPIO_GET_DATA(GPIO_BASEADDR) & GPIO_4_MASK) ? 1 : 0;
    alarmOV = (GPIO_GET_DATA(GPIO_BASEADDR) & GPIO_1_MASK) ? 1 : 0;

    /* Send feedback to user */
    if ((alarmUV == 0) && (alarmOV == 0))
    {
        CONSOLE_Print("AD8280 Self Test passed.\r\n");
    }
    else
    {
        CONSOLE_Print("AD8280 Self Test failed.\r\n");
    }
}
