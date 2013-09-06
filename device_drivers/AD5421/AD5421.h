/**************************************************************************//**
*   @file   AD5421.h
*   @brief  Header file of AD5421 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
******************************************************************************/
#ifndef _AD5421_H_
#define _AD5421_H_

/*****************************************************************************/
/******************** Macros and Constants Definitions ***********************/
/*****************************************************************************/
/* COMMAND Bytes */
#define AD5421_CMDWRDAC         1
#define AD5421_CMDWRCTRL        2
#define AD5421_CMDWROFFSET      3
#define AD5421_CMDWRGAIN        4
#define AD5421_CMDRST			7
#define AD5421_CMDMEASVTEMP     8
#define AD5421_CMDRDDAC         129
#define AD5421_CMDRDCTRL		130
#define AD5421_CMDRDOFFSET      131
#define AD5421_CMDRDGAIN        132
#define AD5421_CMDRDFAULT       133

/* AD5421 COMMAND mask */
#define AD5421_CMD(x)			((x & 0xFF) << 16)

/* AD5421 GPIO */
#define AD5421_LDAC_OUT			GPIO0_PIN_OUT
#define AD5421_LDAC_LOW			GPIO0_LOW
#define AD5421_LDAC_HIGH		GPIO0_HIGH
#define AD5421_FAULT_IN 		GPIO1_PIN_IN

/* CONTROL register bits */
#define CTRL_SPI_WATCHDOG		(1 << 12)
#define CTRL_AUTO_FAULT_RDBK    (1 << 11)
#define CTRL_SEL_ADC_INPUT      (1 << 8)
#define CTRL_ONCHIP_ADC         (1 << 7)

/* SPI Configuration */
#define AD5421_SLAVE_ID			1

/*****************************************************************************/
/************************* Functions Declarations ****************************/
/*****************************************************************************/
/* Initialize the communication with the device. */
int   AD5421_Init(void);
/* Initialize the communication with the device. */
int   AD5421_UpdateParameters(void);
/* Set the value of DAC register. */
void  AD5421_SetDac(int dacValue);
/* Set the value of OFFSET register. */
void  AD5421_SetOffset(int offsetValue);
/* Set the value of GAIN register. */
void  AD5421_SetGain(int gainValue);
/* Read the DAC register. */
int   AD5421_GetDac(void);
/* Read OFFSET register. */
int   AD5421_GetOffset(void);
/* Read GAIN register. */
int   AD5421_GetGain(void);
/* Read FAULT register. */
int   AD5421_GetFault(void);
/* Read the temperature from Fault register. */
int   AD5421_GetTemp(void);
/* Read VLoop-COM from Fault register. */
float AD5421_GetVloop(void);
/* Send command via SPI. */
int   AD5421_Set(int *iValue);
/* Receive value via SPI. */
int   AD5421_Get(void);
/* Resets the AD5421 device. */
void  AD5421_Reset(void);

#endif /* _AD5421_H_ */
