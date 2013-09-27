/***************************************************************************//**
*   @file   AD7193.h
*   @brief  Header file of AD7193 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
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
********************************************************************************
*   SVN Revision: $WCREV$
*******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/*****************************************************************************/
/******************* AD7193 Definitions **************************************/
/*****************************************************************************/
#define CommunicationsReg		0x00
#define StatusReg				0x00
#define ModeReg					0x01
#define ConfigurationReg		0x02
#define DataReg					0x03
#define IDReg					0x04
#define GPOCONReg				0x05
#define OffsetReg				0x06
#define FullScaleReg        	0x07
// Register size in bytes
#define CommunicationsRegLen	1
#define StatusRegLen			1
#define ModeRegLen              3
#define ConfigurationRegLen     3
#define DataRegLen              3
#define DataInfoRegLen          4
#define IDRegLen                1
#define GPOCONRegLen            1
#define OffsetRegLen            3
#define FullScaleRegLen         3

/*****************************************************************************/
/******************* AD7193 Registers Bits ***********************************/
/*****************************************************************************/
// Communications Register 0x00
#define WEN                     7
#define RnW                     6
#define RS2                     5
#define RS1                     4
#define RS0                     3
#define CREAD                   2
#define CR1                     1 // Must be programmed to 0
#define CR0                     0 // Must be programmed to 0
// Status Register 0x00
#define RDY                     7
#define ERR                     6
#define NOREF                   5
#define Parity                  4
#define CHD3                    3
#define CHD2                    2
#define CHD1                    1
#define CHD0                    0
// Mode Register 0x01
#define MD2                     23
#define MD1                     22
#define MD0                     21
#define DAT_STA                 20
#define CLK1                    19
#define CLK0                    18
#define AVG1                    17
#define AVG0                    16
#define SINC3                   15
#define ENPAR                   13
#define CLK_DIV                 12
#define Single                  11
#define REJ60                   10
#define FS9                     9   
#define FS8                     8
#define FS7                     7
#define FS6                     6
#define FS5                     5
#define FS4                     4
#define FS3                     3
#define FS2                     2
#define FS1                     1
#define FS0                     0    
// Configuration Register 0x02
#define Chop                    23
#define REFSEL                  20
#define Pseudo                  18
#define Short                   17
#define TEMP                    16
#define CH7                     15
#define CH6                     14
#define CH5                     13
#define CH4                     12
#define CH3                     11
#define CH2                     10
#define CH1                     9   
#define CH0                     8
#define Burn                    7
#define REFDET                  6
#define BUF                     4
#define UnB                     3
#define G2                      2
#define G1                      1
#define G0                      0    
// GPOCON Register 0x05
#define BPDSW                   6
#define GP32EN                  5
#define GP10EN                  4
#define P3DAT                   3
#define P2DAT                   2
#define P1DAT                   1
#define P0DAT                   0

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/

