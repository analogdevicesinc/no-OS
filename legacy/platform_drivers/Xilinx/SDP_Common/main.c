/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DBogdan (Dragos.Bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Console.h"
#include "Command.h"
#include "Communication.h"

/******************************************************************************/
/************************* Variables Definitions ******************************/
/******************************************************************************/
extern const struct cmd_info cmdList[];
extern char cmdNo;
extern cmdFunction cmdFunctions[11];

/***************************************************************************//**
 * @brief Main function.
 *
 * @return None.
*******************************************************************************/
int main(void)
{

    char          receivedCmd[30] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char cmd             =  0;
    double        param[5]        = {0, 0, 0, 0, 0};
    char          paramNo         =  0;
    char          cmdType         = -1;
    char          invalidCmd      =  0;


    /*!< Select and initialize the platform. */
    if (PLATFORM_Init(XILINX_KC705) < 0)
    {
        return -1;
    }
    /*!< Initialize the console with selected baud rate for the platform used. */
    CONSOLE_Init(UART_BAUDRATE);
    /*!< Initialize the device. */
    DoDeviceInit();

    while(1)
    {
        /*!< Read the command entered by user through console. */
        CONSOLE_GetCommand(receivedCmd);
        invalidCmd = 0;
        for(cmd = 0; cmd < cmdNo; cmd++)
        {
            paramNo = 0;
            cmdType = CONSOLE_CheckCommands(receivedCmd, cmdList[cmd].name, \
                                            param, &paramNo);
            if(cmdType == UNKNOWN_CMD)
            {
                invalidCmd++;
            }
            else
            {
                cmdFunctions[cmd](param, paramNo);
            }
        }
        /*!< Send feedback to user, if the command entered by user is not a valid one. */
        if(invalidCmd == cmdNo)
        {
            CONSOLE_Print("Invalid command!\r\n");
        }
    }

    return 0;
}
