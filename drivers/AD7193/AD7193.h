/***************************************************************************//**
 *   @file   AD7193.h
 *   @brief  Header file of AD7193 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
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

#ifndef __AD7193_H__
#define __AD7193_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"

/******************************************************************************/
/******************************** AD7193 **************************************/
/******************************************************************************/

/* SPI slave device ID */
#define AD7193_SLAVE_ID         1

/* AD74XX Chip Select Pin declaration */
#define AD7193_CS_LOW           SPI_CS_LOW
#define AD7193_CS_HIGH          SPI_CS_HIGH

/* AD7193 GPIO */
#define AD7193_RDY_STATE       SPI_MISO

/* AD7193 Register Map */
#define AD7193_REG_COMM         0 // Communications Register (WO, 8-bit) 
#define AD7193_REG_STAT         0 // Status Register         (RO, 8-bit) 
#define AD7193_REG_MODE         1 // Mode Register           (RW, 24-bit 
#define AD7193_REG_CONF         2 // Configuration Register  (RW, 24-bit)
#define AD7193_REG_DATA         3 // Data Register           (RO, 24/32-bit) 
#define AD7193_REG_ID           4 // ID Register             (RO, 8-bit) 
#define AD7193_REG_GPOCON       5 // GPOCON Register         (RW, 8-bit) 
#define AD7193_REG_OFFSET       6 // Offset Register         (RW, 24-bit 
#define AD7193_REG_FULLSCALE    7 // Full-Scale Register     (RW, 24-bit)

/* Communications Register Bit Designations (AD7193_REG_COMM) */
#define AD7193_COMM_WEN         (1 << 7)           // Write Enable. 
#define AD7193_COMM_WRITE       (0 << 6)           // Write Operation.
#define AD7193_COMM_READ        (1 << 6)           // Read Operation. 
#define AD7193_COMM_ADDR(x)     (((x) & 0x7) << 3) // Register Address. 
#define AD7193_COMM_CREAD       (1 << 2)           // Continuous Read of Data Register.

/* Status Register Bit Designations (AD7193_REG_STAT) */
#define AD7193_STAT_RDY         (1 << 7) // Ready.
#define AD7193_STAT_ERR         (1 << 6) // ADC error bit.
#define AD7193_STAT_NOREF       (1 << 5) // Error no external reference. 
#define AD7193_STAT_PARITY      (1 << 4) // Parity check of the data register. 
#define AD7193_STAT_CH3         (1 << 3) // Channel 3. 
#define AD7193_STAT_CH2         (1 << 2) // Channel 2. 
#define AD7193_STAT_CH1         (1 << 1) // Channel 1. 
#define AD7193_STAT_CH0         (1 << 0) // Channel 0. 

/* Mode Register Bit Designations (AD7193_REG_MODE) */
#define AD7193_MODE_SEL(x)      (((unsigned long)(x) & 0x7) << 21) // Operation Mode Select.
#define AD7193_MODE_DAT_STA     ((unsigned long)1 << 20)           // Status Register transmission.
#define AD7193_MODE_CLKSRC(x)   (((unsigned long)(x) & 0x3) << 18) // Clock Source Select.
#define AD7193_MODE_AVG(x)      (((unsigned long)(x) & 0x3) << 16) // Fast settling filter.
#define AD7193_MODE_SINC3       (1 << 15)                          // SINC3 Filter Select.
#define AD7193_MODE_ENPAR       (1 << 13)                          // Parity Enable.
#define AD7193_MODE_CLKDIV      (1 << 12)                          // Clock divide by 2 (AD7190/2 only).
#define AD7193_MODE_SCYCLE      (1 << 11)                          // Single cycle conversion.
#define AD7193_MODE_REJ60       (1 << 10)                          // 50/60Hz notch filter.
#define AD7193_MODE_RATE(x)     ((x) & 0x3FF)                      // Filter Update Rate Select.

/* Mode Register: AD7193_MODE_SEL(x) options */
#define AD7193_MODE_CONT                0 // Continuous Conversion Mode.
#define AD7193_MODE_SINGLE              1 // Single Conversion Mode.
#define AD7193_MODE_IDLE                2 // Idle Mode.
#define AD7193_MODE_PWRDN               3 // Power-Down Mode.
#define AD7193_MODE_CAL_INT_ZERO        4 // Internal Zero-Scale Calibration.
#define AD7193_MODE_CAL_INT_FULL        5 // Internal Full-Scale Calibration.
#define AD7193_MODE_CAL_SYS_ZERO        6 // System Zero-Scale Calibration.
#define AD7193_MODE_CAL_SYS_FULL        7 // System Full-Scale Calibration.

/* Mode Register: AD7193_MODE_CLKSRC(x) options */
#define AD7193_CLK_EXT_MCLK1_2          0 // External crystal. The external crystal
                                          // is connected from MCLK1 to MCLK2.
#define AD7193_CLK_EXT_MCLK2            1 // External Clock applied to MCLK2 
#define AD7193_CLK_INT                  2 // Internal 4.92 MHz clock. 
                                          // Pin MCLK2 is tristated.
#define AD7193_CLK_INT_CO               3 // Internal 4.92 MHz clock. The internal
                                          // clock is available on MCLK2.

/* Mode Register: AD7193_MODE_AVG(x) options */
#define AD7193_AVG_NONE                 0 // No averaging (fast settling mode disabled).
#define AD7193_AVG_BY_2                 1 // Average by 2.
#define AD7193_AVG_BY_8                 2 // Average by 8.
#define AD7193_AVG_BY_16                3 // Average by 16.

/* Configuration Register Bit Designations (AD7193_REG_CONF) */
#define AD7193_CONF_CHOP        ((unsigned long)1 << 23)            // CHOP enable.
#define AD7193_CONF_REFSEL      ((unsigned long)1 << 20)            // REFIN1/REFIN2 Reference Select.
#define AD7193_CONF_PSEUDO      ((unsigned long)1 << 18)            // Pseudo differential analog inputs.
#define AD7193_CONF_CHAN(x)     ((unsigned long)((x) & 0x3FF) << 8) // Channel select.
#define AD7193_CONF_BURN        (1 << 7)                            // Burnout current enable.
#define AD7193_CONF_REFDET      (1 << 6)                            // Reference detect enable.
#define AD7193_CONF_BUF         (1 << 4)                            // Buffered Mode Enable.
#define AD7193_CONF_UNIPOLAR    (1 << 3)                            // Unipolar/Bipolar Enable.
#define AD7193_CONF_GAIN(x)     ((x) & 0x7)                         // Gain Select.

/* Configuration Register: AD7193_CONF_CHAN(x) options */
//                            Pseudo Bit = 0           Pseudo Bit = 1
#define AD7193_CH_0      0 // AIN1(+) - AIN2(-);       AIN1 - AINCOM
#define AD7193_CH_1      1 // AIN3(+) - AIN4(-);       AIN2 - AINCOM
#define AD7193_CH_2      2 // AIN5(+) - AIN6(-);       AIN3 - AINCOM
#define AD7193_CH_3      3 // AIN7(+) - AIN8(-);       AIN4 - AINCOM
#define AD7193_CH_4      4 // AIN1(+) - AIN2(-);       AIN5 - AINCOM
#define AD7193_CH_5      5 // AIN3(+) - AIN4(-);       AIN6 - AINCOM
#define AD7193_CH_6      6 // AIN5(+) - AIN6(-);       AIN7 - AINCOM
#define AD7193_CH_7      7 // AIN7(+) - AIN8(-);       AIN8 - AINCOM
#define AD7193_CH_TEMP   8 //           Temperature sensor
#define AD7193_CH_SHORT  9 // AIN2(+) - AIN2(-);       AINCOM(+) - AINCOM(-) 

/* Configuration Register: AD7193_CONF_GAIN(x) options */
//                              ADC Input Range (5 V Reference)
#define AD7193_CONF_GAIN_1	0 // Gain 1    +-2.5 V
#define AD7193_CONF_GAIN_8	3 // Gain 8    +-312.5 mV
#define AD7193_CONF_GAIN_16	4 // Gain 16   +-156.2 mV
#define AD7193_CONF_GAIN_32	5 // Gain 32   +-78.125 mV
#define AD7193_CONF_GAIN_64	6 // Gain 64   +-39.06 mV
#define AD7193_CONF_GAIN_128	7 // Gain 128  +-19.53 mV

/* ID Register Bit Designations (AD7193_REG_ID) */
#define ID_AD7193               0x2
#define AD7193_ID_MASK          0x0F

/* GPOCON Register Bit Designations (AD7193_REG_GPOCON) */
#define AD7193_GPOCON_BPDSW     (1 << 6) // Bridge power-down switch enable
#define AD7193_GPOCON_GP32EN    (1 << 5) // Digital Output P3 and P2 enable
#define AD7193_GPOCON_GP10EN    (1 << 4) // Digital Output P1 and P0 enable
#define AD7193_GPOCON_P3DAT     (1 << 3) // P3 state
#define AD7193_GPOCON_P2DAT     (1 << 2) // P2 state
#define AD7193_GPOCON_P1DAT     (1 << 1) // P1 state
#define AD7193_GPOCON_P0DAT     (1 << 0) // P0 state

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Checks if the AD7139 part is present. */
char AD7193_Init(void);

/*! Writes data into a register. */
void AD7193_SetRegisterValue(unsigned char registerAddress,
                             unsigned long registerValue,
                             unsigned char bytesNumber,
                             unsigned char modifyCS);

/*! Reads the value of a register. */
unsigned long AD7193_GetRegisterValue(unsigned char registerAddress,
                                      unsigned char bytesNumber,
                                      unsigned char modifyCS);

/*! Resets the device. */
void AD7193_Reset(void);

/*! Set device to idle or power-down. */
void AD7193_SetPower(unsigned char pwrMode);

/*! Waits for RDY pin to go low. */
void AD7193_WaitRdyGoLow(void);

/*! Selects the channel to be enabled. */
void AD7193_ChannelSelect(unsigned short channel);

/*! Performs the given calibration to the specified channel. */
void AD7193_Calibrate(unsigned char mode, unsigned char channel);

/*! Selects the polarity of the conversion and the ADC input range. */
void AD7193_RangeSetup(unsigned char polarity, unsigned char range);

/*! Returns the result of a single conversion. */
unsigned long AD7193_SingleConversion(void);

/*! Returns the average of several conversion results. */
unsigned long AD7193_ContinuousReadAvg(unsigned char sampleNumber);

/*! Read data from temperature sensor and converts it to Celsius degrees. */
float AD7193_TemperatureRead(void);

/*! Converts 24-bit raw data to volts. */
float AD7193_ConvertToVolts(unsigned long rawData, float vRef);

#endif /* __AD7193_H__ */
