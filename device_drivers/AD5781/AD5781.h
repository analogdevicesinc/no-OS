/***************************************************************************//**
 *   @file   AD5781.h
 *   @brief  Header file of AD5781 Driver.
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
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/
#ifndef __AD5781_H__
#define __AD5781_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"    // Communication definitions

/******************************************************************************/
/*********************************** GPIO *************************************/
/******************************************************************************/
#define AD5781_RESET_OUT       GPIO2_PIN_OUT
#define AD5781_RESET_LOW       GPIO2_LOW
#define AD5781_RESET_HIGH      GPIO2_HIGH

#define AD5781_CLR_OUT         GPIO3_PIN_OUT
#define AD5781_CLR_LOW         GPIO3_LOW
#define AD5781_CLR_HIGH        GPIO3_HIGH

#define AD5781_LDAC_OUT        GPIO4_PIN_OUT
#define AD5781_LDAC_LOW        GPIO4_LOW
#define AD5781_LDAC_HIGH       GPIO4_HIGH

/******************************************************************************/
/********************************** AD5781 ************************************/
/******************************************************************************/

/* SPI slave device ID */
#define AD5781_SLAVE_ID    1

/* Register Map */
#define AD5781_NOP                 0  // No operation (NOP).
#define AD5781_REG_DAC             1  // DAC register.
#define AD5781_REG_CTRL            2  // Control register.
#define AD5781_REG_CLR_CODE        3  // Clearcode register.
#define AD5781_CMD_WR_SOFT_CTRL    4  // Software control register(Write only).

/* Input Shift Register bit definition. */
#define AD5781_READ                (1ul << 23)
#define AD5781_WRITE               (0ul << 23)
#define AD5781_ADDR_REG(x)         (((unsigned long)(x) & 0x7) << 20)

/* DAC Register bit definition */
#define AD5781_DAC_DATA(x)         (((unsigned long)(x) & 0x3FFFF) << 2)

/* Control Register bit Definition */
#define AD5781_CTRL_LINCOMP(x)   (((x) & 0xF) << 6) // Linearity error compensation.
#define AD5781_CTRL_SDODIS       (1 << 5) // SDO pin enable/disable control.
#define AD5781_CTRL_BIN2SC       (1 << 4) // DAC register coding selection.
#define AD5781_CTRL_DACTRI       (1 << 3) // DAC tristate control.
#define AD5781_CTRL_OPGND        (1 << 2) // Output ground clamp control.
#define AD5781_CTRL_RBUF         (1 << 1) // Output amplifier configuration control.

/* Clearcode Register bit definition */
#define AD5781_CLR_CODE_DATA(x)    (((unsigned long)(x) & 0x3FFFF) << 2)

/* Software Control Register bit definition */
#define AD5781_SOFT_CTRL_RESET      (1 << 2) // RESET function.
#define AD5781_SOFT_CTRL_CLR        (1 << 1) // CLR function.
#define AD5781_SOFT_CTRL_LDAC       (1 << 0) // LDAC function.

/* DAC OUTPUT STATES */
#define AD5781_OUT_NORMAL            0x0
#define AD5781_OUT_CLAMPED_6K        0x1
#define AD5781_OUT_TRISTATE          0x2

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the communication with the device. */
long AD5781_Init(void);

/*! Writes data into a register. */
 long AD5781_SetRegisterValue(unsigned char registerAddress,
                              unsigned long registerValue);

/*! Reads the value of a register. */
long AD5781_GetRegisterValue(unsigned char registerAddress);

/*! Sets the DAC output in one of the three states. */
long AD5781_DacOuputState(unsigned char state);

/*! Writes to the DAC register. */
long AD5781_SetDacValue(unsigned long value);

/*! Asserts RESET, CLR or LDAC in a software manner. */
long AD5781_SoftInstruction(unsigned char instructionBit);

/*! Configures the output amplifier, DAC coding, SDO state and the linearity
    error compensation. */
long AD5781_Setup(unsigned long setupWord);

#endif /* __AD5781_H__ */
