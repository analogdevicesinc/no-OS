/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for ADF4156.
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
#include "adf4156.h"
#include "adf4156_cfg.h"

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
        .name = "frequency=",
        .description = "Sets the desired output frequency in MHz.",
        .acceptedValue = "Accepted values: \r\n\
\t500 ... 6200 - desired output frequency in MHz.",
        .example = "To set the output frequency to 1000MHz, type: \
frequency=1000",
    },
    [2] = {
        .name = "frequency?",
        .description = "Displays the output frequency in MHz.",
        .acceptedValue = "",
        .example = "",
    },
    [3] = {
        .name = "register=",
        .description = "Sets the value of the desired register on 29 bits.",
        .acceptedValue = "Accepted values: \r\n\
\tregister: \r\n\
\t0 .. 4 - the selected register.\r\n\
\tvalue: \r\n\
\t0 .. 0x1FFFFFFF - the new value of the register.",
        .example = "To set the value 0x11111111 to register 2, type: \
register= 2 0x11111111",
    },
    [4] = {
        .name = "register?",
        .description = "Displays the value of the desired register.",
        .acceptedValue = "Accepted values: \r\n\
\t0 .. 4 - the selected register.",
        .example = "To display the value of register 0, type register?0",
    },
    [5] = {
        .name = "PFDfreq?",
        .description = "Displays the current PFD frequency in MHz.",
        .acceptedValue = "",
        .example = "",
    },
};


const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp, SetFrequency, GetFrequency, SetRegister,
                              GetRegister,GetPFDFrequency};

adf4156_state adf4156_st =
                                {
                                &adf4156_pdata_lpc,
                                0,
                                0,
                                0,
                                0,
                                0
                                };

/* Variables holding information about the device */
double freqSet = 0;

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
    if(ADF4156_Init() == 0)
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
 * @brief Sets the desired output frequency in MHz.
 *
 * @param param[0] = output frequency to be set in MHz.
 *
 * @return None.
*******************************************************************************/
void SetFrequency(double* param, char paramNo) // "frequency=" command
{
    //unsigned long long freq;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < ADF4156_MIN_OUT_FREQ)
        {
            param[0] = ADF4156_MIN_OUT_FREQ;
        }
        if(param[0] > ADF4156_MAX_OUT_FREQ)
        {
            param[0] = ADF4156_MAX_OUT_FREQ;
        }
        
      //  freq = (unsigned long long)param[0];
        /* Sets the desired frequency */
        freqSet = adf4156_set_freq(&adf4156_st, param[0]);
        /* Send feedback to user */
        CONSOLE_Print("%s%.3f[MHz]\r\n",(char*)cmdList[1].name,freqSet);
    }
     else
    {
         /* Display error messages */
         DisplayError(1);
    }
}

/***************************************************************************//**
 * @brief Displays the output frequency in MHz.
 *
 * @return None.
*******************************************************************************/
void GetFrequency(double* param, char paramNo) // "frequency?" command
{
    /* Send the requested value to user */
    CONSOLE_Print("%s%.3f[MHz]\r\n",(char*)cmdList[1].name,freqSet);
}

/***************************************************************************//**
 * @brief Sets the value of the desired register on 29 bits.
 *
 * @param param[0] = selected register.
 * @param param[1] = new value of the register.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) // "register=" command
{
    unsigned long regVal;
    unsigned char regNo;

    /* Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 4)
        {
            param[0] = 4;
        }
        if(param[1] < 0)
        {
            param[1] = 0;
        }
        if(param[1] > ADF4156_MAX_REG_VAL)
        {
            param[1] = ADF4156_MAX_REG_VAL;
        }

        regNo  = (unsigned char)param[0];
        regVal = (unsigned long)param[1];
        /* Calculate the register value with control bits */
        regVal = (regVal << 3) | regNo;
        ADF4156_Set(regVal);
        adf4156_st.reg_val[regNo] = regVal;

        /* Send feedback to user */
        CONSOLE_Print("register=%d; value=0x%x\r\n",regNo,regVal);
    }
     else
    {
         /* Display error messages */
         DisplayError(3);
    }
}

/***************************************************************************//**
 * @brief Displays the value of the desired register.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) // "register?" command
{
    unsigned char  regNo;
    unsigned long  regVal;
    unsigned char  muxCtrl,noiseMode,csrEn,curSet,prescaler,refDiv2,refDoubler;
    unsigned char  rCounter,sdRst,ldp,pdPol,pd,cpZ,counterRst,clkDivMode;
    unsigned short intVal,fracVal,phaseVal,modulus,clkDivVal;

    /* Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 4)
        {
            param[0] = 4;
        }

        regNo = (unsigned char)param[0];
        /* Send feedback to user */
        CONSOLE_Print("register=%d; value=0x%x\r\n",regNo,
                      adf4156_st.reg_val[regNo]);

        regVal = adf4156_st.reg_val[regNo];
        if (regNo == 0)
        {
            muxCtrl = (0x78000000 & regVal) >> 27;
            CONSOLE_Print("\tMuxout Control=%d\r\n", muxCtrl);
            intVal = (0x07FF8000 & regVal) >> 15;
            CONSOLE_Print("\tInteger Value=%d\r\n", intVal);
            fracVal = (0x00007FF8 & regVal) >> 3;
            CONSOLE_Print("\tFractional Value=%d\r\n", fracVal);
        }
        if (regNo == 1)
        {
            phaseVal = (0x00007FF8 & regVal) >> 3;
            CONSOLE_Print("\tPhase Value=%d\r\n", phaseVal);
        }
        if (regNo == 2)
        {
            noiseMode = (0x60000000 & regVal) >> 29;
            CONSOLE_Print("\tNoise Mode=%d\r\n", noiseMode);
            csrEn = (0x10000000 & regVal) >> 28;
            CONSOLE_Print("\tCSR Enable Bit=%d\r\n", csrEn);
            curSet = (0x0F000000 & regVal) >> 24;
            CONSOLE_Print("\tCurrent Setting=%d\r\n", curSet);
            prescaler = (0x00400000 & regVal) >> 22;
            CONSOLE_Print("\tPrescaler Bit=%d\r\n", prescaler);
            refDiv2 = (0x00200000 & regVal) >> 21;
            CONSOLE_Print("\tReference/2 Bit=%d\r\n", refDiv2);
            refDoubler = (0x00100000 & regVal) >> 20;
            CONSOLE_Print("\tReference*2 Bit=%d\r\n", refDoubler);
            rCounter = (0x000F8000 & regVal) >> 15;
            CONSOLE_Print("\tR Counter=%d\r\n", rCounter);
            modulus = (0x00007FF8 & regVal) >> 3;
            CONSOLE_Print("\tModulus Value=%d\r\n", modulus);
        }
        if (regNo == 3)
        {
            sdRst = (0x00004000 & regVal) >> 14;
            CONSOLE_Print("\tSigma-Delta Reset Bit=%d\r\n", sdRst);
            ldp = (0x00000080 & regVal) >> 7;
            CONSOLE_Print("\tLock Detect Precision Bit=%d\r\n", ldp);
            pdPol = (0x00000040 & regVal) >> 6;
            CONSOLE_Print("\tPhase Detect Polarity Bit=%d\r\n", pdPol);
            pd = (0x00000020 & regVal) >> 5;
            CONSOLE_Print("\tRF Power Down Bit=%d\r\n", pd);
            cpZ = (0x00000010 & regVal) >> 4;
            CONSOLE_Print("\tCharge Pump Three-State Bit=%d\r\n", cpZ);
            counterRst = (0x00000008 & regVal) >> 3;
            CONSOLE_Print("\tCounter Reset Bit=%d\r\n", counterRst);
        }
        if (regNo == 4)
        {
            clkDivMode = (0x00180000 & regVal) >> 19;
            CONSOLE_Print("\tClock Divider Mode=%d\r\n", clkDivMode);
            clkDivVal = (0x0007FF80 & regVal) >> 7;
            CONSOLE_Print("\tClock Divider Value=%d\r\n", clkDivVal);
        }
     }
     else
     {
         /* Display error messages */
         DisplayError(4);
     }
}
/***************************************************************************//**
 * @brief Displays the current PFD frequency in MHz.
 *
 * @return None.
*******************************************************************************/
void GetPFDFrequency(double* param, char paramNo) // "PFDfreq?" command
{
    /* Send the requested value to user */
    CONSOLE_Print("PFD%s%d[MHz]\r\n",(char*)cmdList[1].name, adf4156_st.fpfd/1000000);
}
