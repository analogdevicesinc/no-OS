/***************************************************************************//**
 *   @file   ADT7420.h
 *   @brief  Header file of ADT7420 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
#ifndef __ADT7420_H__
#define __ADT7420_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"

/******************************************************************************/
/************************** ADT7420 Definitions *******************************/
/******************************************************************************/

/* ADT7420 address */
#define ADT7420_A0_PIN          1 // I2C Serial Bus Address Selection Pin
#define ADT7420_A1_PIN          1 // I2C Serial Bus Address Selection Pin
#define ADT7420_ADDRESS         (0x48 + (ADT7420_A1_PIN << 1) + ADT7420_A0_PIN)

/* ADT7420 registers */
#define ADT7420_REG_TEMP_MSB    0x00 // Temperature value MSB
#define ADT7420_REG_TEMP_LSB    0x01 // Temperature value LSB
#define ADT7420_REG_STATUS      0x02 // Status
#define ADT7420_REG_CONFIG      0x03 // Configuration
#define ADT7420_REG_T_HIGH_MSB  0x04 // Temperature HIGH setpoint MSB
#define ADT7420_REG_T_HIGH_LSB  0x05 // Temperature HIGH setpoint LSB
#define ADT7420_REG_T_LOW_MSB   0x06 // Temperature LOW setpoint MSB
#define ADT7420_REG_T_LOW_LSB   0x07 // Temperature LOW setpoint LSB
#define ADT7420_REG_T_CRIT_MSB  0x08 // Temperature CRIT setpoint MSB
#define ADT7420_REG_T_CRIT_LSB  0x09 // Temperature CRIT setpoint LSB
#define ADT7420_REG_HIST        0x0A // Temperature HYST setpoint
#define ADT7420_REG_ID          0x0B // ID
#define ADT7420_REG_RESET       0x2F // Software reset

/* ADT7420_REG_STATUS definition */
#define ADT7420_STATUS_T_LOW            (1 << 4)
#define ADT7420_STATUS_T_HIGH           (1 << 5)
#define ADT7420_STATUS_T_CRIT           (1 << 6)
#define ADT7420_STATUS_RDY              (1 << 7)

/* ADT7420_REG_CONFIG definition */
#define ADT7420_CONFIG_FAULT_QUEUE(x)   (x & 0x3)
#define ADT7420_CONFIG_CT_POL           (1 << 2)
#define ADT7420_CONFIG_INT_POL          (1 << 3)
#define ADT7420_CONFIG_INT_CT_MODE      (1 << 4)
#define ADT7420_CONFIG_OP_MODE(x)       ((x & 0x3) << 5)
#define ADT7420_CONFIG_RESOLUTION       (1 << 7)

/* ADT7420_CONFIG_FAULT_QUEUE(x) options */
#define ADT7420_FAULT_QUEUE_1_FAULT     0
#define ADT7420_FAULT_QUEUE_2_FAULTS    1
#define ADT7420_FAULT_QUEUE_3_FAULTS    2
#define ADT7420_FAULT_QUEUE_4_FAULTS    3

/* ADT7420_CONFIG_OP_MODE(x) options */
#define ADT7420_OP_MODE_CONT_CONV       0
#define ADT7420_OP_MODE_ONE_SHOT        1
#define ADT7420_OP_MODE_1_SPS           2
#define ADT7420_OP_MODE_SHUTDOWN        3

/* ADT7420 default ID */
#define ADT7420_DEFAULT_ID  0xCB

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Reads the value of a register. */
unsigned char ADT7420_GetRegisterValue(unsigned char registerAddress);

/*! Sets the value of a register. */
void ADT7420_SetRegisterValue(unsigned char registerAddress,
                              unsigned char registerValue);

/*! Initializes the comm. peripheral and checks if the device is present. */
char ADT7420_Init(void);

/*! Resets the ADT7420. */
void ADT7420_Reset(void);

/*! Sets the operational mode for ADT7420. */
void ADT7420_SetOperationMode(unsigned char mode);

/*! Sets the resolution for ADT7420. */
void ADT7420_SetResolution(unsigned char resolution);

/*! Reads the temperature data and converts it to Celsius degrees. */
float ADT7420_GetTemperature(void);

#endif	/* __ADT7420_H__ */
