/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DBogdan (Dragos.Bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Console.h"
#include "Command.h"

/******************************************************************************/
/*********************** Device configuration words ***************************/
/******************************************************************************/
/*!< DEVCFG3 */
/*!< USERID = No Setting */

/*!< DEVCFG2 */
#pragma config FPLLIDIV = DIV_2  /*!< PLL Input Divider (2x Divider) */
#pragma config FPLLMUL = MUL_20  /*!< PLL Multiplier (20x Multiplier) */
#pragma config FPLLODIV = DIV_1  /*!< System PLL Output Clock Divider (PLL Divide by 1) */

/* DEVCFG1 */
#pragma config FNOSC = PRIPLL    /*!< Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL)) */
#pragma config FSOSCEN = OFF     /*!< Secondary Oscillator Enable (Disabled) */
#pragma config IESO = ON         /*!< Internal/External Switch Over (Enabled) */
#pragma config POSCMOD = XT      /*!< Primary Oscillator Configuration (XT osc mode) */
#pragma config OSCIOFNC = OFF    /*!< CLKO Output Signal Active on the OSCO Pin (Disabled) */
#pragma config FPBDIV = DIV_1    /*!< Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1) */
#pragma config FCKSM = CSDCMD    /*!< Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled) */
#pragma config WDTPS = PS1048576 /*!< Watchdog Timer Postscaler (1:1048576) */
#pragma config FWDTEN = OFF      /*!< Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls)) */

/* DEVCFG0 */
#pragma config DEBUG = OFF       /*!< Background Debugger Enable (Debugger is disabled) */
#pragma config ICESEL = ICS_PGx2 /*!< ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2) */
#pragma config PWP = OFF         /*!< Program Flash Write Protect (Disable) */
#pragma config BWP = OFF         /*!< Boot Flash Write Protect bit (Protection Disabled) */
#pragma config CP = OFF          /*!< Code Protect (Protection Disabled) */

/******************************************************************************/
/************************* Variables Definitions ******************************/
/******************************************************************************/
extern const char* cmdList[];
extern const char* cmdDescription[];
extern const char* cmdExample[];
extern const char  cmdNo;
extern cmdFunction cmdFunctions[];

/***************************************************************************//**
 * @brief Main function.
 *
 * @return None.
*******************************************************************************/
void main(void)
{
    char   receivedCmd[20];
    double param[5];
    char   cmd        = 0;
    char   paramNo    = 0;
    char   cmdType    = -1;
    char   invalidCmd = 0;

	/*!< Initialize the console. */
    CONSOLE_Init(9600);
    TIME_DelayMs(10);

    /*!< Initialize the device. */
    DoDeviceInit();
        
    while(1)
    {
        /*!< Read the command entered by user through UART. */
        CONSOLE_GetCommand(receivedCmd);
        
        invalidCmd = 0;
        for(cmd = 0; cmd < cmdNo; cmd++)
        {
            paramNo = 0;
            cmdType = CONSOLE_CheckCommands(receivedCmd, cmdList[cmd], param, &paramNo);
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
}
