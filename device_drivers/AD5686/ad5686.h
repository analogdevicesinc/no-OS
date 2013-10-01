/***************************************************************************//**
*   @file   AD5686.h
*   @brief  Header file of AD5686 Driver. This driver supporting the following
*              devices: AD5684R, AD5685R, AD5686R, AD5694R, AD5695R, AD5696R,
*
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"

/* Supported devices */
typedef enum {
    ID_AD5684R,
    ID_AD5685R,
    ID_AD5686R,
    ID_AD5694R,
    ID_AD5695R,
    ID_AD5696R,
} AD5686_type_t;

/* Control Bits */
#define AD5686_CTRL_NOP          0
#define AD5686_CTRL_WRITE        1
#define AD5686_CTRL_UPDATE       2
#define AD5686_CTRL_WRITEUPDATE  3
#define AD5686_CTRL_PWR          4
#define AD5686_CTRL_LDAC_MASK    5
#define AD5686_CTRL_SWRESET      6
#define AD5686_CTRL_IREF_REG     7
#define AD5686_CTRL_DCEN         8
#define AD5686_CTRL_RB_REG       9

/* AD5686 channels */
#define AD5686_CH_A              1
#define AD5686_CH_B              2
#define AD5686_CH_C              4
#define AD5686_CH_D              8
#define AD5686_CH_ALL            0xF

/* Power-down operation modes masks */
#define AD5686_PWRM_NORMAL       0
#define AD5686_PWRM_1K           1
#define AD5686_PWRM_100K         2
#define AD5686_PWRM_THREESTATE   3

#define AD5686_PWRM_MASK         3

/* Power-down operation modes offset for each channel */
#define AD5686_PWRM_CHA_OFFSET   0
#define AD5686_PWRM_CHB_OFFSET   2
#define AD5686_PWRM_CHC_OFFSET   4
#define AD5686_PWRM_CHD_OFFSET   6

/* AD5449 GPIO */
#define AD5686_LDAC_OUT          GPIO0_PIN_OUT
#define AD5686_LDAC_LOW          GPIO0_LOW
#define AD5686_LDAC_HIGH         GPIO0_HIGH
#define AD5686_RESET_OUT         GPIO1_PIN_OUT
#define AD5686_RESET_LOW         GPIO1_LOW
#define AD5686_RESET_HIGH        GPIO1_HIGH

/* Enable/disable defines */
#define AD5686_INTREF_EN        1
#define AD5686_INTREF_DIS       0
#define AD5686_DC_EN            1
#define AD5686_DC_DIS           0
#define AD5686_RB_EN            1
#define AD5686_RB_DIS           0

/* Define I2C hardwired address */
#define AD5686_I2C_ADDR          0x0E

/* SPI slave device ID */
#define AD5686_SLAVE_ID          1

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize SPI and Initial Values for AD5449 Board. */
char AD5686_Init(AD5686_type_t device);
/* Write to input register */
unsigned short AD5686_SetShiftReg(unsigned char command,
                        unsigned char address,
                        unsigned short data);

/* Write to Input Register n (dependent on LDAC) */
void AD5686_WriteRegister(unsigned char address,
                          unsigned short data);

/* Update DAC Register n with contents of Input Register n */
void AD5686_UpdateRegister(unsigned char address);

/* Write to and update DAC channel n */
void AD5686_WriteUpdateRegister(unsigned char address,
                                unsigned short data);

/* Read back Input Register n */
unsigned short AD5686_ReadBackRegister(unsigned char address);

/* Power down / power up DAC */
void AD5686_PowerMode(unsigned char address,
                     unsigned char mode);

/* Set up LDAC mask register */
void AD5686_LdacMask(unsigned char ldacMask);

/* Software reset (power-on reset) */
void AD5686_SoftwareReset();

/* Write to Internal reference setup register */
void AD5686_InternalReference(unsigned char value);

/* Set up DCEN register (daisy-chain enable) */
void AD5686_DaisyChainEn(unsigned char value);

/* Set up readback register (readback enable) */
void AD5686_ReadBackEn(unsigned char value);
