/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for AD5446.
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
#include "AD5446.h"
#include "TIME.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

const struct cmd_info cmdList[] = {
    [0] = {
        .name = "help?",
        .description = "  -  Displays all available commands.",
        .acceptedValue = "",
        .example = "",
    },
    [1] = {
        .name = "register=",
        .description = "Writes to the DAC register.",
        .acceptedValue = "Accepted values:\r\n\
\t0 .. 2^DAC_resolution - the value written to the DAC.",
        .example = "To set the DAC register to 20000, type: register=20000",
    },
    [2] = {
        .name = "register?",
        .description = "Displays last written value in the DAC register.",
        .acceptedValue = "",
        .example = "",
    },
    [3] = {
        .name = "voltage=",
        .description = "Sets the DAC output voltage.",
        .acceptedValue = "Accepted values:\r\n\
\t-Vref .. +Vref - desired output voltage in milivolts.",
        .example = "To set the output voltage to 2350.25 [mV], type: voltage = \
2350.25",
    },
    [4] = {
        .name = "voltage?",
        .description = "Displays last written voltage value to the DAC.",
        .acceptedValue = "",
        .example = "",
    },
    [5] = {
        .name = "ldacPin=",
        .description = "Sets the output value of LDAC pin.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - sets LDAC pin low.(default)\r\n\
\t1 - sets LDAC pin high.",
        .example = "To set the LDAC pin high, type: ldacPin=1",
    },
    [6] = {
        .name = "ldacPin?",
        .description = "Displays the value of LDAC pin.",
        .acceptedValue = "",
        .example = "",
    },
    [7] = {
        .name = "clrPin=",
        .description = "Sets the output value of CLR pin.",
        .acceptedValue = "Accepted values:\r\n\
\t0 - sets CLR pin low.\r\n\
\t1 - sets CLR pin high.(default)",
        .example = "To set the CLR pin low, type: clrPin=0",
    },
    [8] = {
        .name = "clrPin?",
        .description = "Displays the value of CLR pin.",
        .acceptedValue = "",
        .example = "",
    }
};

/* Define the output type of each evaluation board */
const vout_type_t boardOutput[] = {
        unipolar_inv,   // ID_AD5553
        unipolar_inv,   // ID_AD5543
        bipolar,        // ID_AD5542A
        unipolar,       // ID_AD5541A
        unipolar,       // ID_AD5512A
        unipolar_inv,  	// ID_AD5453
        unipolar,	    // ID_AD5452
        unipolar,   	// ID_AD5451
        unipolar,   	// ID_AD5450
        unipolar_inv,   // ID_AD5446
        unipolar_inv    // ID_AD5444
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[9] = {GetHelp, SetRegister, GetRegister,
                               SetVoltage, GetVoltage, SetLdacPin, GetLdacPin,
                               SetClrPin, GetClrPin};

/* Variables holding information about the device */
AD5446_type_t deviceType;
unsigned char ldac = 0;
unsigned char clr  = 1;
unsigned short registerValue = 0;
/* Device specific constants */
unsigned short max_value = 0;
float vref = 0;

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
For AD5553 type 0\r\n\
For AD5543 type 1\r\n\
For AD5542A type 2\r\n\
For AD5541A type 3\r\n\
For AD5453 type 4\r\n\
For AD5446 type 5\r\n");
        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x35))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                    deviceType = ID_AD5553;
                    max_value = 0x3FFF;
                    vref = 5;
                    break;
                }
                case 0x31 :
                {
                    deviceType = ID_AD5543;
                    max_value = 0xFFFF;
                    vref = 5;
                    break;
                }
                case 0x32 :
                {
                    deviceType = ID_AD5542A;
                    max_value = 0xFFFF;
                    vref = 2.5;
                    break;
                }
                case 0x33 :
                {
                    deviceType = ID_AD5541A;
                    max_value = 0xFFFF;
                    vref = 2.5;
                    break;
                }
                case 0x34 :
                {
                    deviceType = ID_AD5453;
                    max_value = 0x3FFF;
                    vref = 10;
                    break;
                }
                case 0x35 :
                {
                    deviceType = ID_AD5446;
                    max_value = 0x3FFF;
                    vref = 10;
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

    if(AD5446_Init(deviceType) == 0)
    {
        CONSOLE_Print("Device OK\r\n");
        CONSOLE_Print("Slave device ID: %s\r\n", AD5446_SLAVE_ID);
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
 * @brief Writes to the DAC register.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) /*!< "register=" command */
{

    /*!< Check if the parameter is valid */
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
        CONSOLE_Print("1.Debug: %f\r\n", param[0]);
        registerValue = (unsigned short)param[0];
        CONSOLE_Print("2.Debug: %d\r\n", registerValue);
        /*!< Write to DAC register */
        AD5446_SetRegister(0, registerValue);
        /*!< Send the requested value to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[1].name, registerValue);
    }
    else
    {
        /*!< Display error messages */
        DisplayError(1);
    }
}

/***************************************************************************//**
 * @brief Displays last written value in the DAC register.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[1].name, registerValue);
}

/***************************************************************************//**
 * @brief Sets the DAC output voltage.
 *
 * @param param[0] value of the desired voltage. The value can be between:
 * 						- (0 .. Vref), if the output is unipolar
 * 						- (-Vref .. 0) if the output is unipolar inverted
 * 						- (-Vref .. Vref) if the output is bipolar
 *
 * @return None.
*******************************************************************************/
void SetVoltage(double* param, char paramNo) /*!< "voltage=" command */
{
    double outVoltage = 0;
    short min_voltage = 0;
    short max_voltage = (vref * 1000) - 0.001;

    /* Define the lower and upper limits in function of the output polarity */
    if(boardOutput[deviceType] == bipolar)
    {
        min_voltage = (-1) * max_voltage;
    }
    if(boardOutput[deviceType] == unipolar_inv)
    {
        min_voltage = (-1) * max_voltage;
        max_voltage = 0;
    }

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < min_voltage)
        {
        	param[0] = min_voltage;
        }
        if(param[0] > max_voltage)
        {
        	param[0] = max_voltage;
        }

        outVoltage = param[0] / 1000;
        outVoltage = AD5446_SetVoltage(outVoltage, vref, boardOutput[deviceType]);
        /*!< Get the registerValue using value of the outVoltage */
        switch(boardOutput[deviceType])
        {
            case unipolar :
            {
                registerValue = (outVoltage * max_value) / vref;
                break;
            }
            case unipolar_inv :
            {
                registerValue = (-1) * (outVoltage * max_value) / vref;
                break;
            }
            case bipolar :
            {
                registerValue = ((outVoltage + vref) * (max_value/2)) / vref;
                break;
            }
        }
        /*!< Send feedback to user */
        CONSOLE_Print("%s%.3f [mV]\r\n",(char*)cmdList[3].name, outVoltage);
    }
    else
    {
        /*!< Display error messages */
        DisplayError(3);
    }
}

/***************************************************************************//**
 * @brief Displays last written voltage value to the DAC.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    double outVoltage = 0;

    /*!< Get the output voltage using data from registerValue */
    CONSOLE_Print("Vout type: %d\r\n", boardOutput[deviceType]);
    switch(boardOutput[deviceType])
    {
        case unipolar :
        {
            outVoltage = ((float)registerValue / max_value) * vref;
            break;
        }
        case unipolar_inv :
        {
            outVoltage = (-1) * ((float)registerValue / max_value) * vref;
            break;
        }
        case bipolar :
        {
            outVoltage = (vref * (float)registerValue / (max_value/2)) - vref;
            break;
        }
    }

    /*!< Send feedback to user */
    CONSOLE_Print("%s%.3f [mV]\r\n",(char*)cmdList[3].name, outVoltage);
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

    if((deviceType == ID_AD5542A) || (deviceType == ID_AD5541A))
    {
        /* Check if the parameter is valid */
        if(paramNo >= 1)
        {
            paramLimit(&param[0], 0, 1);

            status = (unsigned char) param[0];

            if (status == 0)
            {
                AD5446_LDAC_LOW;
                ldac = 0;
            }
            else
            {
                if (status == 1)
                {
                    AD5446_LDAC_HIGH;
                    ldac = 1;
                }
            }
            /* Send feedback to user */
            CONSOLE_Print("%s%d\r\n",(char*)cmdList[5].name, status);
        }
        else
        {
            /* Display error messages */
            DisplayError(5);
        }
    }
    else
    {
        CONSOLE_Print("This function is not supported \
by the current device!\n\r");
    }
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

    if((deviceType == ID_AD5542A))
    {
        /* Check if the parameter is valid */
        if(paramNo >= 1)
        {
            paramLimit(&param[0], 0, 1);

            status = (unsigned char)param[0];

            if (status == 0)
            {
                AD5446_CLR_LOW;
                clr = 0;
            }
            else
            {
                if (status == 1)
                {
                    AD5446_CLR_HIGH;
                    clr = 1;
                }
            }

            /* Send feedback to user */
            CONSOLE_Print("%s%d\r\n",(char*)cmdList[7].name, status);
            }
            else
            {
                /* Display error messages */
                DisplayError(7);
            }
    }
    else
    {
        CONSOLE_Print("This function is not supported \
by the current device!\n\r");
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

    if((deviceType == ID_AD5542A) || (deviceType == ID_AD5541A))
    {
        /* Send requested value to user */
        CONSOLE_Print("LDAC=%d\r\n",ldac);
    }
    else
    {
        CONSOLE_Print("This function is not supported \
by the current device!\n\r");
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
    if(deviceType == ID_AD5542A)
    {
        /* Send requested value to user */
        CONSOLE_Print("CLR=%d\r\n",clr);
    }
    else
    {
        CONSOLE_Print("This function is not supported \
by the current device!\n\r");
    }
}
