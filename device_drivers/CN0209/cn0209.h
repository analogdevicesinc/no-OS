/***************************************************************************//**
*   @file   cn0209.c
*   @brief  Header file of CN0209 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/
#ifndef _CN0209_H_
#define _CN0209_H_

/*****************************************************************************/
/* Include Files                                                             */
/*****************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/* SPI Configuration                                                          */
/******************************************************************************/
#define AD7193_RDY_PIN          (1 << 1)

/******************************************************************************/
/* CN0209 GPIO                                                                */
/******************************************************************************/
#define ADG1414_CS_PIN          (1 << 0) // ADG1414 CS Pin.
#define ADT7310_CS_PIN          (1 << 1) // ADT7310 CS Pin.
#define AD7193_CS_PIN           (1 << 2) // AD7193 CS Pin.
#define AD7193_INT1_PIN         (1 << 3) // INT1 Pin.
#define AD7193_INT2_PIN         (1 << 5) // INT2 Pin.
#define AD7193_READY_PIN        (1 << 7) // Data Ready Pin.

#define AD7193_CS_OUT           GPIO2_PIN_OUT
#define AD7193_CS_LOW           GPIO2_LOW
#define AD7193_CS_HIGH          GPIO2_HIGH

#define ADT7310_CS_OUT          GPIO1_PIN_OUT
#define ADT7310_CS_LOW          GPIO1_LOW
#define ADT7310_CS_HIGH         GPIO1_HIGH

#define ADG1414_CS_OUT          GPIO0_PIN_OUT
#define ADG1414_CS_LOW          GPIO0_LOW
#define ADG1414_CS_HIGH         GPIO0_HIGH

/******************************************************************************/
/* AD7193                                                                     */
/******************************************************************************/
/* AD7193 Registers */
#define AD7193_REG_COMM         0 // Communications Register(WO, 8-bit)
#define AD7193_REG_STAT         0 // Status Register        (RO, 8-bit)
#define AD7193_REG_MODE         1 // Mode Register          (RW, 24-bit)
#define AD7193_REG_CONF         2 // Configuration Register (RW, 24-bit)
#define AD7193_REG_DATA         3 // Data Register          (RO, 24-bit)
#define AD7193_REG_ID           4 // ID Register            (RO, 8-bit)
#define AD7193_REG_GPOCON       5 // GPOCON Register        (RW, 8-bit)
#define AD7193_REG_OFFSET       6 // Offset Register        (RW, 24-bit)
#define AD7192_REG_FULLSCALE    7 // Full-Scale Register    (RW, 24-bit)

/* AD7193 Communications Register Bits */
#define AD7193_COMM_WEN         (1 << 7)            // Write Enable
#define AD7193_COMM_WRITE       (0 << 6)            // Write Operation
#define AD7193_COMM_READ        (1 << 6)            // Read Operation
#define AD7193_COMM_ADDR(x)     (((x) & 0x7) << 3)  // Register Address
#define AD7193_COMM_CREAD       (1 << 2)            // Continuous Read

/* AD7193 Status Register Bits */
#define AD7193_STAT_RDY         (1 << 7) // Ready
#define AD7193_STAT_ERR         (1 << 6) // Error (Overrange, Underrange)
#define AD7193_STAT_NOREF       (1 << 5) // Error no external reference
#define AD7193_STAT_PARITY      (1 << 4) // Parity
#define AD7193_STAT_CH3         (1 << 2) // Channel 3
#define AD7193_STAT_CH2         (1 << 2) // Channel 2
#define AD7193_STAT_CH1         (1 << 1) // Channel 1
#define AD7193_STAT_CH0         (1 << 0) // Channel 0

/* AD7193 Mode Register Bits */
#define AD7193_MODE_SEL(x)      (((x) & 0x7) << 21) // Operation Mode Select
#define AD7193_MODE_DAT_STA     (1 << 20)           // Status Register transmission
#define AD7193_MODE_CLKSRC(x)   (((x) & 0x3) << 18) // Clock Source Select
#define AD7193_MODE_AVG(x)      (((x) & 0x3) << 16) // Fast settling filter
#define AD7193_MODE_SINC3       (1 << 15)           // SINC3 Filter Select
#define AD7193_MODE_ENPAR       (1 << 13)           // Parity Enable
#define AD7193_MODE_CLKDIV      (1 << 12)           // Clock divide by 2
#define AD7193_MODE_SCYCLE      (1 << 11)           // Single cycle conversion
#define AD7193_MODE_REJ60       (1 << 10)           // 50/60Hz notch filter
#define AD7193_MODE_RATE(x)     ((x) & 0x3FF)       // Filter Update Rate Select

/* AD7193_MODE_SEL(x) options */
#define AD7193_MODE_CONT            0 // Continuous Conversion Mode
#define AD7193_MODE_SINGLE          1 // Single Conversion Mode
#define AD7193_MODE_IDLE            2 // Idle Mode
#define AD7193_MODE_PWRDN           3 // Power-Down Mode
#define AD7193_MODE_CAL_INT_ZERO    4 // Internal Zero-Scale Calibration
#define AD7193_MODE_CAL_INT_FULL    5 // Internal Full-Scale Calibration
#define AD7193_MODE_CAL_SYS_ZERO    6 // System Zero-Scale Calibration
#define AD7193_MODE_CAL_SYS_FULL    7 // System Full-Scale Calibration

/* AD7193_MODE_CLKSRC(x) options */
#define AD7193_MODE_CLK_EXT_MCLK1_2 0 // External 4.92 MHz Clock connected from MCLK1 to MCLK2
#define AD7193_MODE_CLK_EXT_MCLK2   1 // External Clock applied to MCLK2
#define AD7193_MODE_CLK_INT         2 // Internal 4.92 MHz Clock not available at the MCLK2 pin
#define AD7193_MODE_CLK_INT_CO      3 // Internal 4.92 MHz Clock available at the MCLK2 pin

/* AD7193 Configuration Register Bits */
#define AD7193_CONF_CHOP        (1 << 23)           // CHOP enable
#define AD7193_CONF_REFSEL      (1 << 20)           // REFIN1/REFIN2 Reference Select
#define AD7193_CONF_PSEUDO      (1 << 18)           // Pseudo differential analog inputs
#define AD7193_CONF_SHORT       (1 << 17)           // Short channel
#define AD7193_CONF_TEMP        (1 << 16)           // Temperature sensor
#define AD7193_CONF_CHAN(x)     (((x) & 0xFF) << 8) // Channel select
#define AD7193_CONF_BURN        (1 << 7)            // Burnout current enable
#define AD7193_CONF_REFDET      (1 << 6)            // Reference detect enable
#define AD7193_CONF_BUF         (1 << 4)            // Buffered Mode Enable
#define AD7193_CONF_UNIPOLAR    (1 << 3)            // Unipolar/Bipolar Enable
#define AD7193_CONF_GAIN(x)     ((x) & 0x7)         // Gain Select

/* AD7193_CONF_GAIN(x) options */
#define AD7193_CONF_GAIN_1      0 // Gain 1, ADC Input Range +-2.5 V
#define AD7193_CONF_GAIN_8      3 // Gain 8, ADC Input Range +-312.5 mV
#define AD7193_CONF_GAIN_16     4 // Gain 16, ADC Input Range +-156.2 mV
#define AD7193_CONF_GAIN_32     5 // Gain 32, ADC Input Range +-78.125 mV
#define AD7193_CONF_GAIN_64     6 // Gain 64, ADC Input Range +-39.06 mV
#define AD7193_CONF_GAIN_128    7 // Gain 128, ADC Input Range +-19.53 mV

/* GPOCON Register Bits */
#define AD7193_GPOCON_BPDSW     (1 << 6) // Bridge power-down switch enable
#define AD7193_GPOCON_GP32EN    (1 << 5) // Digital Output P3 and P2 enable
#define AD7193_GPOCON_GP10EN    (1 << 4) // Digital Output P1 and P0 enable
#define AD7193_GPOCON_P3DAT     (1 << 3) // P3 state
#define AD7193_GPOCON_P2DAT     (1 << 2) // P2 state
#define AD7193_GPOCON_P1DAT     (1 << 1) // P1 state
#define AD7193_GPOCON_P0DAT     (1 << 0) // P0 state

/******************************************************************************/
/* ADT7310                                                                    */
/******************************************************************************/
/* ADT7310 Registers */
#define ADT7310_REG_STATUS      0x00 // Status Register         (RO, 8-bit)
#define ADT7310_REG_CONFIG      0x01 // Configuration Register  (RW, 8-bit)
#define ADT7310_REG_TEMPERATURE 0x02 // Temperature value       (RO, 16-bit)
#define ADT7310_REG_ID          0x03 // ID                      (RO, 8-bit)
#define ADT7310_REG_TCRIT       0x04 // TCRIT setpoint          (RW, 16-bit)
#define ADT7310_REG_THYST       0x05 // THYST setpoint          (RW, 16-bit)
#define ADT7310_REG_THIGH       0x06 // THIGH setpoint          (RW, 16-bit)
#define ADT7310_REG_TLOW        0x07 // TLOW setpoint           (RW, 16-bit)

/* ADT7310 Command */
#define ADT7310_CMD_WRITE       (0 << 6)            // Write Operation
#define ADT7310_CMD_READ        (1 << 6)            // Read Operation
#define ADT7310_CMD_ADDR(x)     (((x) & 0x7) << 3)  // Register Address
#define ADT7310_CMD_CREAD       (1 << 2)            // Continuous Read

/******************************************************************************/
/* Functions Prototypes                                                       */
/******************************************************************************/
/* Reads data from ADT7310. */
unsigned short ADT7310_Read(unsigned char regAddress,
                            unsigned char bytesNumber);

/* Writes data to ADT7310. */
void ADT7310_Write(unsigned char regAddress,
                   unsigned char bytesNumber,
                   unsigned short regData);

/* Reads the temperature from ADT7310 device. */
float ADT7310_TemperatureRead(void);

/* Resets the serial interface with the ADT7310. */
void ADT7310_Reset(void);

/* Reads data from AD7193. */
unsigned long AD7193_Read(unsigned char regAddress,
                           unsigned char bytesNumber);

/* Writes data to AD7193. */
void AD7193_Write(unsigned char regAddress,
                  unsigned char bytesNumber,
                  unsigned long  regData);

/* Returns the result of a single conversion from AD7193. */
unsigned long AD7193_SingleConversion(void);

/* Reads the temperature from AD7193 device. */
float AD7193_TemperatureRead(void);

/* Converts 24-bit raw data to millivolts. */
float AD7193_ConvertToVolts(unsigned long rawData,
                            float         vRef,
                            unsigned char gain,
                            unsigned char polarity);

/* Reset the serial interface with the AD7193. */
void AD7193_Reset(void);

/* Updates the state of the ADG1414's switches. */
void ADG1414_State (unsigned char switches);

/* Initializes SPI and initial values for CN0209 board. */
char CN0209_Init(void);

#endif /* CN0209_H_ */
