/***************************************************************************//**
*   @file   AD5933.h
*   @brief  Header File for AD5933 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AD5933_H_
#define _AD5933_H_

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/*****************************************************************************/
/******************* ADP5589 Registers Definitions ***************************/
/*****************************************************************************/
// AD5933 IIC Address
#define I2C_BASEADDR 					XPAR_AXI_IIC_1_BASEADDR
#define AD5933_I2C_ADDR      			0x0D
// Registers Address
#define AD5933_CONTROL_REG_HB           0x80
#define AD5933_CONTROL_REG_LB           0x81
#define AD5933_START_FREQ_REG_HB        0x82
#define AD5933_START_FREQ_REG_MB        0x83
#define AD5933_START_FREQ_REG_LB        0x84
#define AD5933_FREQ_INCR_REG_HB         0x85
#define AD5933_FREQ_INCR_REG_MB         0x86
#define AD5933_FREQ_INCR_REG_LB         0x87
#define AD5933_NR_INCR_REG_HB           0x88
#define AD5933_NR_INCR_REG_LB           0x89
#define AD5933_NR_SETTLE_REG_HB         0x8A
#define AD5933_NR_SETTLE_REG_LB         0x8B
#define AD5933_STATUS_REG               0x8F
#define AD5933_TEMP_REG_HB              0x92
#define AD5933_TEMP_REG_LB              0x93
#define AD5933_REAL_REG_HB              0x94
#define AD5933_REAL_REG_LB              0x95
#define AD5933_IMG_REG_HB               0x96
#define AD5933_IMG_REG_LB               0x97

/*****************************************************************************/
/******************* ADP5589 Registers Bits **********************************/
/*****************************************************************************/
// Control Register High Bits 0x80
#define AD5933_CONTROL_FUNCTION(x)      ((x) << 4)
// AD5933_CONTROL_FUNCTION(x) options
#define AD5933_INIT_START_FREQ			0x01
#define AD5933_START_FREQ_SWEEP			0x02
#define AD5933_INCR_FREQ				0x03
#define AD5933_REPEAT_FREQ				0x04
#define AD5933_MEAS_TEMP				0x09
#define AD5933_POWER_DOWN				0x0A
#define AD5933_STANDBY					0x0B

#define AD5933_CONTROL_RANGE(x)			((x) << 1)
// AD5933_CONTROL_RANGE(x) options
#define AD5933_2Vpp_RANGE				0x00
#define AD5933_200mVpp_RANGE			0x01
#define AD5933_400mVpp_RANGE			0x02
#define AD5933_1Vpp_RANGE				0x03

#define AD5933_PGA_GAIN(x)				((x) << 0)
// AD5933_PGA_GAIN(x) options
#define AD5933_PGA_GAIN_X5				0x00
#define AD5933_PGA_GAIN_X1				0x01

// Control Register Low Bits 0x81
#define AD5933_RESET					(1 << 4)
#define AD5933_INT_CLK					(0 << 3)
#define AD5933_EXT_CLK					(1 << 3)

// Status Register 0x8F
#define AD5933_STATUS_TEMP_VALID		1
#define AD5933_STATUS_DATA_VALID		(1 << 1)
#define AD5933_STATUS_SWEEP_DONE		(1 << 2)

/*****************************************************************************/
/**************************** Command Codes **********************************/
/*****************************************************************************/
#define AD5933_BLOCK_WRITE				0xA0
#define AD5933_BLOCK_READ    			0xA1
#define AD5933_ADDR_POINTER 			0xB0

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
void AD5933_SetRegisterValue(int registerAddress,
							 int registerValue,
							 char numberOfBytes);
int AD5933_GetRegisterValue(int registerAddress,
							char numberOfBytes);
int AD5933_GetTemperature(void);
void AD5933_ConfigSweep(int startFreq,
						int incSteps,
						int incFreq);
void AD5933_StartSweep(void);
double AD5933_CalculateGainFactor(unsigned long calibrationImpedance,
							   char freqFunction);
double AD5933_CalculateImpedance(double gainFactor,
								 char freqFunction);
#endif	// _AD5933_H_

