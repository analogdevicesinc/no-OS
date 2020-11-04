/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodTMP2.
 *   @author DNechita (Dan.Nechita@analog.com)
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
#include "AD5933.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/*!< List of available commands */
const char* cmdList[] = { "help?",
                          "temperature?",
                          "startFreq=",
                          "incFreq=",
                          "incNum=",
                          "sweepParam?",
                          "calibImpedance=",
                          "impedance?",
                          "currentFreq?"};
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Displays the ambient device temperature.",
"  -  Start frequency parameter. Accepted values:\r\n\
\t1 000 .. 100 000 [Hz].",
"  -  Frequency increment parameter. Accepted values:\r\n\
\t0 .. 100 000 [Hz].",
"  -  Number of increments parameter. Accepted values:\r\n\
\t0 .. 511.",
"  -  Displays all three sweep parameters.",
"  -  Calibration impedance value. Accepted values:\r\n\
\t1 000 .. 10 000 000 [Ohm].",
"  -  Displays the measured impedance.",
"  -  Displays the current frequency."};
const char* cmdExample[] = {
"",
"",
"To set the start frequency to 1000 [Hz], type: startFreq=1000",
"To set the frequency increment to 100 [Hz], type: incFreq=100",
"To set the number of increments to 10, type: incNum=10",
"",
"To set the calibration impedance to 10000[Ohm], type calibImpedance=10000",
"",
""};
const char  cmdNo          = (sizeof(cmdList) / sizeof(const char*));
const long  MHZ_4          = 4000000;    /*!< 16MHz / 4 */
const long  P_2_27         = 134217728;  /*!< 2 to the power of 27 */
const long  MAX_START_FREQ = 100000;     /*!< 100 KHz maximum frequency */
const short MIN_START_FREQ = 1000;       /*!< 1KHz minimum frequency */
const short MAX_INC_NUM    = 511;        /*!< Value is represented by a 9-bit word */
const short MIN_IMPEDANCE  = 1000;       /*!< 1KOhm minimum impedance */
const long  MAX_IMPEDANCE  = 10000000;   /*!< 10MOhms maximum impedance */

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[9] = {GetHelp, GetTemperature, SetStartFreq, SetIncFreq,
                               SetIncNum, GetSweepParam, SetCalibImpedance, 
                               GetImpedance, GetCurrentFreq};
/*!< Variables holding information about the device */
float          temperature = 0.0;  /*!< Last temperature read from the device */
unsigned long  startFreq   = 1000; /*!< Start frequency sweep */
unsigned long  incFreq     = 0;    /*!< Increment frequency */
unsigned short incNum      = 0;    /*!< Number of increments */
unsigned long  calibImped  = 0;    /*!< Calibration impedance */
double         gainFactor  = 0;    /*!< Stores the value of the gain factor */
double         impedance   = 0;    /*!< Measured impedance */
unsigned long  currentFreq = 0;    /*!< Signal frequency used during a measurement */

/***************************************************************************//**
 * @brief Displays all available commands.
 *
 * @return None.
*******************************************************************************/
void GetHelp(double* param, char paramNo) /*!< "help?" command */
{
    char displayCmd;

    CONSOLE_Print("Available commands:\r\n");
    for(displayCmd = 0; displayCmd < cmdNo; displayCmd++)
    {
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[displayCmd],
                                  (char*)cmdDescription[displayCmd]);
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
    if(AD5933_Init() == 0)
    {
        CONSOLE_Print("AD5933 OK\r\n");
        GetHelp(NULL, 0);

        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5933 Error\r\n");

        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Displays the temperature.
 *
 * @return None.
*******************************************************************************/
void GetTemperature(double* param, char paramNo) /*!< "temperature?" command */
{
    /*!< Read the temperature from the device */
    temperature = AD5933_GetTemperature();
    /*!< Send the requested value to user */
    CONSOLE_Print("temperature=%.1f [C]\r\n",temperature);
}

/***************************************************************************//**
 * @brief Sets the start frequency parameter for sweep operation.
 *
 * @return None.
*******************************************************************************/
void SetStartFreq(double* param, char paramNo) /*!< "startFreq=" command */
{
    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < MIN_START_FREQ)
        {
            param[0] = MIN_START_FREQ;
        }
        if(param[0] > MAX_START_FREQ)
        {
            param[0] = MAX_START_FREQ;
        }

        startFreq = (unsigned long)(param[0] + 0.5);
        
        /*!< Configure the sweep parameters */
        AD5933_ConfigSweep(startFreq,
                           incFreq,
                           incNum);
        /*!< Start the sweep operation */
        AD5933_StartSweep();
        /*!< Update the currentFrequrncy */
        currentFreq = startFreq;
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d Hz\r\n",(char*)cmdList[2], startFreq);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[2], (char*)cmdDescription[2]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[2]);
    }
}

/***************************************************************************//**
 * @brief Sets the frequency increment parameter for sweep operation.
 *
 * @return None.
*******************************************************************************/
void SetIncFreq(double* param, char paramNo) /*!< "incFreq=" command */
{

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > MAX_START_FREQ)
        {
            param[0] = MAX_START_FREQ;
        }
    
        incFreq = (unsigned long)(param[0] + 0.5);
        /*!< Configure the sweep parameters */
        AD5933_ConfigSweep(startFreq,
                           incFreq,
                           incNum);
        /*!< Start the sweep operation */
        AD5933_StartSweep();
         /*!< Send feedback to user */
        CONSOLE_Print("%s%d Hz\r\n",(char*)cmdList[3], incFreq);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[3], (char*)cmdDescription[3]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[3]);
    }
}

/***************************************************************************//**
 * @brief Sets the number of increments parameter for sweep operation.
 *
 * @return None.
*******************************************************************************/
void SetIncNum(double* param, char paramNo) /*!< "incNum=" command */
{
    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > MAX_INC_NUM)
        {
            param[0] = MAX_INC_NUM;
        }
        incNum = (unsigned short)param[0];
        /*!< Configure the sweep parameters */
        AD5933_ConfigSweep(startFreq,
                           incFreq,
                           incNum);
        /*!< Start the sweep operation */
        AD5933_StartSweep();
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d \r\n",(char*)cmdList[4], incNum);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[4], (char*)cmdDescription[4]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[4]);
    }
}

/***************************************************************************//**
 * @brief Displays all three sweep parameters.
 *
 * @return None.
*******************************************************************************/
void GetSweepParam(double* param, char paramNo) /*!< "sweepParam?" command */
{
    /*!< Send the requested values to user */
    CONSOLE_Print("%s%d Hz\r\n",cmdList[2], startFreq);
    CONSOLE_Print("%s%d Hz\r\n",cmdList[3], incFreq);
    CONSOLE_Print("%s%d\r\n",cmdList[4], incNum);
}

/***************************************************************************//**
 * @brief Sets the calibration impedance value.
 *
 * @return None.
*******************************************************************************/
void SetCalibImpedance(double* param, char paramNo) /*!< "calibImpedance=" command */
{
    /*!< Check if the parameter is valid */
     if(paramNo >= 1)
    {
        if(param[0] < MIN_IMPEDANCE)
        {
            param[0] = MIN_IMPEDANCE;
        }
        if(param[0] > MAX_IMPEDANCE)
        {
            param[0] = MAX_IMPEDANCE;
        }
        calibImped = (unsigned long)param[0];

        /*!< Calculate the gain factor for the selected impedance */
        gainFactor = AD5933_CalculateGainFactor(calibImped,
                                       AD5933_FUNCTION_REPEAT_FREQ);
        /*!< Send feedback to user */
        CONSOLE_Print("Gain was calculated for Z=%d [Ohm]\r\n",calibImped);
    }
    else
     {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[6], (char*)cmdDescription[6]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[6]);
    }
}

/***************************************************************************//**
 * @brief Displays the measured impedance.
 *
 * @return None.
*******************************************************************************/
void GetImpedance(double* param, char paramNo) /*!< "impedance?" command */
{
    double tempValue = 0;
    
    /*!< Calculates the impedance between the VOUT and VIN pins. */
    impedance = AD5933_CalculateImpedance(gainFactor,
                                       AD5933_FUNCTION_INC_FREQ);
    /*!< Send the requested value to user */
    if(impedance >= 1000000)
    {
        tempValue = (double)impedance / 1000000;
        CONSOLE_Print("impedance=%.3f [MOhm]\r\n",tempValue);
    }
    else if (impedance >= 1000)
    {
        tempValue = (double)impedance / 1000;
        CONSOLE_Print("impedance=%.3f [KOhm]\r\n",tempValue);
    }
    else
    {
        tempValue = (double)impedance;
        CONSOLE_Print("impedance=%.3f [Ohm]\r\n",tempValue);
    }
    /*!< Update the currentFrequrncy */
    currentFreq = currentFreq + incFreq;
}

/***************************************************************************//**
 * @brief Displays the current frequency.
 *
 * @return None.
*******************************************************************************/
void GetCurrentFreq(double* param, char paramNo) /*!< "currentFreq?" command */
{

 CONSOLE_Print("%d [Hz]\r\n",currentFreq);

}
