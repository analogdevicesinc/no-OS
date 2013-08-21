/***************************************************************************//**
 *   @file   ADXL345.h
 *   @brief  Header file of ADXL345 Driver.
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
#ifndef ADXL345_H_
#define ADXL345_H_

#include "xparameters.h"
#include "xil_cache.h"
#include "xil_io.h"

/******************************************************************************/
/* ADXL345                                                                    */
/******************************************************************************/
#define ADXL345_RnW					7
#define ADXL345_MB					6
// ADXL345 Registers
#define ADXL345_DEVID				0x00
#define ADXL345_THRESH_TAP			0x1D
#define ADXL345_OFSX				0x1E
#define ADXL345_OFSY				0x1F
#define ADXL345_OFSZ				0x20
#define ADXL345_DUR					0x21
#define ADXL345_Latent				0x22
#define ADXL345_Window				0x23
#define ADXL345_THRESH_ACT			0x24
#define ADXL345_THRESH_INACT    	0x25
#define ADXL345_TIME_INACT			0x26
#define ADXL345_ACT_INACT_CTL		0x27
#define ADXL345_THRESH_FF			0x28
#define ADXL345_TIME_FF				0x29
#define ADXL345_TAP_AXES			0x2A
#define ADXL345_ACT_TAP_STATUS		0x2B
#define ADXL345_BW_RATE				0x2C
#define ADXL345_POWER_CTL			0x2D
#define ADXL345_INT_ENABLE			0x2E
#define ADXL345_INT_MAP				0x2F
#define ADXL345_INT_SOURCE			0x30
#define ADXL345_DATA_FORMAT			0x31
#define ADXL345_DATA_X0				0x32
#define ADXL345_DATA_X1				0x33
#define ADXL345_DATA_Y0				0x34
#define ADXL345_DATA_Y1				0x35
#define ADXL345_DATA_Z0				0x36
#define ADXL345_DATA_Z1				0x37
#define ADXL345_FIFO_CTL			0x38
#define ADXL345_FIFO_STATUS			0x39

// Register specific bits
// Device ID Register 0x00 - Read value should be 0xE5
// Tap Threshold Register 0x1D - Holds Threshold values for tap interrupts
// Offset X Register 0x1E |
// Offset Y Register 0x1F |- 15.6 mg/LSB, added to the acceleration data
// Offset Z Register 0x20 |
// Tap Duration Register 0x21 - 625us/LSB. Holds time that an event must be above THRESH_TAP to qualify as event
// Latent Register 0x22 - Time from detection of tap to time window. 1.25ms/LSB
// Window Register 0x23 - Time when a second tap can be detected. 1.25ms/LSB
// Activity Threshold Register 0x24 - Threshold value for detecting activity. 62.5mg/LSB
// Inactivity Threshold Register 0x25 - Threshold value for detecting inactivity. 62.5mg/LSB
// Inactivity Time Register 0x26 - Time for which acceleration must be less than THRESH_INACT. 1s/LSB
// Activity/Inactivity Control Register 0x27
#define ADXL345_ACT_ac_dc			7
#define ADXL345_ACT_X_en			6
#define ADXL345_ACT_Y_en			5
#define ADXL345_ACT_Z_en			4
#define ADXL345_INACT_ac_dc			3
#define ADXL345_INACT_X_en			2
#define ADXL345_INACT_Y_en			1
#define ADXL345_INACT_Z_en			0
// Threshold Free Fall Register 0x28 - Value for free fall detection. 62.5mg/LSB 0x05 to 0x09 are recommended
// Free Fall Time Register 0x29 - Time for which axes value must be lower than THRESH_FF to generate fall interrupt. 5ms/LSB. 0x14 to 0x46 recommended
// Axes Control For Single/Double Tap Register 0x2A
#define ADXL345_Suppress 			3
#define ADXL345_TAP_X_en			2
#define ADXL345_TAP_Y_en			1
#define ADXL345_TAP_Z_en			0
// Source Of Single/Double Tap Register 0x2B
#define ADXL345_ACT_X_Source		6
#define ADXL345_ACT_Y_Source		5
#define ADXL345_ACT_Z_Source		4
#define ADXL345_ASleep				3
#define ADXL345_TAP_X_Source		2
#define ADXL345_TAP_Y_Source		1
#define ADXL345_TAP_Z_Source		0
// Data Rate and Power Mode Control Register 0x2C
#define ADXL345_LOW_POWER			4
#define ADXL345_RATE				0
// Power Savings Features Control Register 0x2D
#define ADXL345_Link				5
#define ADXL345_AUTO_SLEEP			4
#define ADXL345_Measure				3
#define ADXL345_Sleep				2
#define ADXL345_Wakeup				0
// Interrupt Enable Control Register 0x2E
// Interrupt Mapping Control Register 0x2F
// Interrupt Source Register 0x30
#define ADXL345_DATA_READY			7
#define ADXL345_SINGLE_TAP			6
#define ADXL345_DOUBLE_TAP			5
#define ADXL345_Activity			4
#define ADXL345_Inactivity			3
#define ADXL345_FREE_FALL			2
#define ADXL345_Watermark			1
#define ADXL345_Overrun				0
// Data Format Control Register 0x31
#define ADXL345_SELF_TEST			7
#define ADXL345_SPI					6
#define ADXL345_INT_INVERT			5
#define ADXL345_FULL_RES			3
#define ADXL345_Justify				2
#define ADXL345_Range				0
// X-Axis Data0 Register 0x32
// X-Axis Data1 Register 0x33
// Y-Axis Data0 Register 0x34
// Y-Axis Data1 Register 0x35
// Z-Axis Data0 Register 0x36
// Z-Axis Data1 Register 0x37
// FIFO Control Register 0x38
#define ADXL345_FIFO_MODE			6
#define ADXL345_Trigger				5
#define ADXL345_Samples				0
// FIFO Status Register	0x39
#define ADXL345_FIFO_TRIG			7
#define ADXL345_Entries				0

void ADXL345_DisplayMainMenu(void);
void ADXL345_DisplayMenu(void);
void ADXL345_DisplayRangeMenu(void);
void ADXL345_DisplayRateMenu(void);
void ADXL345_DisplayTapMenu(void);
void delay_ms(u32 ms_count);
void ADXL345_WriteReg(unsigned char addr, unsigned char txData);
int ADXL345_ReadReg(unsigned char addr);
int ADXL345_BurstReadReg(unsigned char addr);
void ADXL345_Init(void);
void ADXL345_Display_G_Force(char axis);
void ADXL345_DisplayAllAxes(void);
void ADXL345_ReadAllAxes(void);
void ADXL345_Run(void);
void ADXL345_Stop(void);
void ADXL345_DisplayAccel(void);
void ADXL345_SelectRange(void);
void ADXL345_AcquisitionRate(void);
void ADXL345_SelectTap(void);
void DemoProgram(void);



#endif /* ADXL345_H_ */
