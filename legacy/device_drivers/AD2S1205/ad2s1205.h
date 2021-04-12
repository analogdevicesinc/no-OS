/***************************************************************************//**
*   @file   ad2s1205.h
*   @brief  Header file of ad2s1205 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
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
*******************************************************************************/

#ifndef _AD2S1205_H_
#define _AD2S1205_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/* PARALEL PORT                                                               */
/******************************************************************************/
#define PAR_RDVEL       (1 << 0)
#define PAR_SAMPLE      (1 << 1)
#define PAR_CS          (1 << 5)
#define PAR_DATA        (1 << 6)
#define PAR_RD          (1 << 26)

#define PAR_DATA_MASK   (0x00000FFF << 6)

/******************************************************************************/
/* Timing constants                                                           */
/******************************************************************************/
#define F_CLKIN         8192 //KHz
#define T2              ((int)6000000/F_CLKIN + 20)/2 //(6/fCLKIN)+20 [ns]
#define T3              18  //[ns]
#define T4              5   //[ns]
#define T5              7   //[ns]
#define T6              30  //[ns]
#define T7              18  //[ns]

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/

/* Initializes the parallel communication with the device. */
char AD2S1205_Init(void);

/* Reads the position and velocity information through parallel port. */
void AD2S1205_Pos_Vel(unsigned short* position, short* velocity);

#endif /* AD2S1205_H_ */
