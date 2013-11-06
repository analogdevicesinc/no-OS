/**************************************************************************//**
*   @file   adf4153.h
*   @brief  Header file of adf4153 driver.
*
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
*******************************************************************************
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
******************************************************************************/
#ifndef __ADF4153_H__
#define __ADF4153_H__

#include "Communication.h"

/*****************************************************************************/
/*  Device specific MACROs                                                   */
/*****************************************************************************/
/* GPIOs */
#define ADF4153_LE_OUT                      GPIO0_PIN_OUT
#define ADF4153_LE_LOW                      GPIO0_LOW
#define ADF4153_LE_HIGH                     GPIO0_HIGH

#define ADF4153_CE_OUT                      GPIO2_PIN_OUT
#define ADF4153_CE_LOW                      GPIO2_LOW
#define ADF4153_CE_HIGH                     GPIO2_HIGH

#define ADF4153_LE2_OUT                     GPIO6_PIN_OUT
#define ADF4153_LE2_LOW                     GPIO6_LOW
#define ADF4153_LE2_HIGH                    GPIO6_HIGH

#define ADF4153_CE2_OUT                     GPIO7_PIN_OUT
#define ADF4153_CE2_LOW                     GPIO7_LOW
#define ADF4153_CE2_HIGH                    GPIO7_HIGH

/* Control Bits */
#define ADF4153_CTRL_MASK                   0x3

#define ADF4153_CTRL_N_DIVIDER              0          /* N Divider Register */
#define ADF4153_CTRL_R_DIVIDER              1          /* R Divider Register */
#define ADF4153_CTRL_CONTROL                2          /* Control Register */
#define ADF4153_CTRL_NOISE_SPUR             3          /* Noise and Spur Reg*/

/* N Divider Register */

/* 12-bit fractional value */
#define ADF4153_R0_FRAC_OFFSET              2
#define ADF4153_R0_FRAC_MASK                0xFFF
#define ADF4153_R0_FRAC(x)                  ((x) & ADF4153_R0_FRAC_MASK) \
                                              << ADF4153_R0_FRAC_OFFSET
/* 9-bit integer value */
#define ADF4153_R0_INT_OFFSET               14
#define ADF4153_R0_INT_MASK                 0x1FF
#define ADF4153_R0_INT(x)                   ((x) & ADF4153_R0_INT_MASK) \
                                              << ADF4153_R0_INT_OFFSET

/* Fast-Lock */
#define ADF4153_R0_FASTLOCK_OFFSET          23
#define ADF4153_R0_FASTLOCK_MASK            0x1
#define ADF4153_R0_FASTLOCK(x)              ((x) & ADF4153_R0_FASTLOCK_MASK) \
                                              << ADF4153_R0_FASTLOCK_OFFSET

/* R Divider Register */

/* 12-bit interpolator modulus value */
#define ADF4153_R1_MOD_OFFSET               2
#define ADF4153_R1_MOD_MASK                 0xFFF
#define ADF4153_R1_MOD(x)                   ((x) & ADF4153_R1_MOD_MASK) \
                                              << ADF4153_R1_MOD_OFFSET
/* 4-bit R Counter */
#define ADF4153_R1_RCOUNTER_OFFSET          14
#define ADF4153_R1_RCOUNTER_MASK            0xF
#define ADF4153_R1_RCOUNTER(x)              ((x) & ADF4153_R1_RCOUNTER_MASK) \
                                              << ADF4153_R1_RCOUNTER_OFFSET
/* Prescale */
#define ADF4153_R1_PRESCALE_OFFSET          18
#define ADF4153_R1_PRESCALE_MASK            0x1
#define ADF4153_R1_PRESCALE(x)              ((x) & ADF4153_R1_PRESCALE_MASK) \
                                              << ADF4153_R1_PRESCALE_OFFSET
/* MUXOUT */
#define ADF4153_R1_MUXOUT_OFFSET            20
#define ADF4153_R1_MUXOUT_MASK              0x7
#define ADF4153_R1_MUXOUT(x)                ((x) & ADF4153_R1_MUXOUT_MASK) \
                                              << ADF4153_R1_MUXOUT_OFFSET
/* Load Control */
#define ADF4153_R1_LOAD_OFFSET              23
#define ADF4153_R1_LOAD_MASK                0x1
#define ADF4153_R1_LOAD(x)                  ((x) & ADF4153_R1_LOAD_MASK) \
                                              << ADF4153_R1_LOAD_OFFSET

/* Control Register */

/* Counter Reset */
#define ADF4153_R2_COUNTER_RST_OFFSET       2
#define ADF4153_R2_COUNTER_RST_MASK         0x1
#define ADF4153_R2_COUNTER_RST(x)           ((x) & ADF4153_R2_COUNTER_RST_MASK)\
                                               << ADF4153_R2_COUNTER_RST_OFFSET
/* CP Three-State */
#define ADF4153_R2_CP_3STATE_OFFSET         3
#define ADF4153_R2_CP_3STATE_MASK           0x1
#define ADF4153_R2_CP_3STATE(x)             ((x) & ADF4153_R2_CP_3STATE_MASK) \
                                               << ADF4153_R2_CP_3STATE_OFFSET
/* Power-down */
#define ADF4153_R2_POWER_DOWN_OFFSET        4
#define ADF4153_R2_POWER_DOWN_MASK          0x1
#define ADF4153_R2_POWER_DOWN(x)            ((x) & ADF4153_R2_POWER_DOWN_MASK) \
                                               <<   ADF4153_R2_POWER_DOWN_OFFSET
/* LDP */
#define ADF4153_R2_LDP_OFFSET               5
#define ADF4153_R2_LDP_MASK                 0x1
#define ADF4153_R2_LDP(x)                   ((x) & ADF4153_R2_LDP_MASK) \
                                               << ADF4153_R2_LDP_OFFSET
/* PD Polarity */
#define ADF4153_R2_PD_POL_OFFSET            6
#define ADF4153_R2_PD_POL_MASK              0x1
#define ADF4153_R2_PD_POL(x)                ((x) & ADF4153_R2_PD_POL_MASK) \
                                               << ADF4153_R2_PD_POL_OFFSET
/* CP Current Settings and CP/2 */
#define ADF4153_R2_CP_CURRENT_OFFSET        7
#define ADF4153_R2_CP_CURRENT_MASK          0xF
#define ADF4153_R2_CP_CURRENT(x)            ((x) & ADF4153_R2_CP_CURRENT_MASK) \
                                               << ADF4153_R2_CP_CURRENT_OFFSET
/* Reference doubler */
#define ADF4153_R2_REF_DOUBLER_OFFSET       11
#define ADF4153_R2_REF_DOUBLER_MASK         0x1
#define ADF4153_R2_REF_DOUBLER(x)           ((x) & ADF4153_R2_REF_DOUBLER_MASK)\
                                              << ADF4153_R2_REF_DOUBLER_OFFSET
/* Resync */
#define ADF4153_R2_RESYNC_OFFSET            12
#define ADF4153_R2_RESYNC_MASK              0x7
#define ADF4153_R2_RESYNC(x)                ((x) & ADF4153_R2_RESYNC_MASK) \
                                              << ADF4153_R2_RESYNC_OFFSET

/* Noise and spur register */

/* Noise and spur mode */
#define ADF4153_R3_NOISE_SPURG_MASK         0x3C4
#define ADF4153_R3_NOISE_SPURG(x)           ( (((x) << 0x2) & 0x7) | \
                                              (((x) >> 0x1) << 0x6) ) &\
                                               ADF4153_R3_NOISE_SPURG_MASK

/* Fast-Lock definitions */
#define ADF4153_FASTLOCK_DISABLED           0
#define ADF4153_FASTLOCK_ENABLED            1
/* Prescale definitions */
#define ADF4153_PRESCALER_4_5               0
#define ADF4153_PRESCALER_8_9               1
/* Muxout definitions */
#define ADF4153_MUXOUT_THREESTATE           0
#define ADF4153_MUXOUT_DIGITAL_LOCK         1
#define ADF4153_MUXOUT_NDIV_OUTPUT          2
#define ADF4153_MUXOUT_LOGICHIGH            3
#define ADF4153_MUXOUT_RDIV_OUTPUT          4
#define ADF4153_MUXOUT_ANALOG_LOCK          5
#define ADF4153_MUXOUT_FASTLOCK             6
#define ADF4153_MUXOUT_LOGICLOW             7
/* Load Control definitions */
#define ADF4153_LOAD_NORMAL                 0
#define ADF4153_LOAD_RESYNC                 1
/* Counter Reset Definitions */
#define ADF4153_CR_DISABLED                 0
#define ADF4153_CR_ENABLED                  1
/* CP Three-state definitions */
#define ADF4153_CP_DISABLED                 0
#define ADF4153_CP_THREE_STATE              1
/* Power-down definitions */
#define ADF4153_PD_DISABLED                 0
#define ADF4153_PD_ENABLED                  1
/* LDP definitions */
#define ADF4153_LDP_24                      0
#define ADF4153_LDP_40                      1
/* PD Polarity definitions */
#define ADF4153_PD_POL_NEGATIV              0
#define ADF4153_PD_POL_POSITIVE             1
/* CR Current Settings definitions */
#define ADF4153_CP_CURRENT_0_63             0
#define ADF4153_CP_CURRENT_1_25             1
#define ADF4153_CP_CURRENT_1_88             2
#define ADF4153_CP_CURRENT_2_50             3
#define ADF4153_CP_CURRENT_3_13             4
#define ADF4153_CP_CURRENT_3_75             5
#define ADF4153_CP_CURRENT_4_38             6
#define ADF4153_CP_CURRENT_5_00             7
#define ADF4153_CP2_CURRENT_0_31            8
#define ADF4153_CP2_CURRENT_0_63            9
#define ADF4153_CP2_CURRENT_0_94            10
#define ADF4153_CP2_CURRENT_1_25            11
#define ADF4153_CP2_CURRENT_1_57            12
#define ADF4153_CP2_CURRENT_1_88            13
#define ADF4153_CP2_CURRENT_2_19            14
#define ADF4153_CP2_CURRENT_2_50            15

/* Reference doubler definition */
#define ADF4153_REF_DOUBLER_DIS             0
#define ADF4153_REF_DOUBLER_EN              1
/* Noise and Spur mode definitions */
#define ADF4153_LOW_SPUR_MODE               0b00000
#define ADF4153_LOW_NOISE_SPUR              0b11100
#define ADF4153_LOWEST_NOISE                0b11111

/* The default slave ID for SPI interface */
#define ADF4153_SLAVE_ID                    1

/*****************************************************************************/
/************************** Types Declarations *******************************/
/*****************************************************************************/

/**
*   struct ADF4106_settings_t - store the value of all the latch and the input
*                               reference frequency
*
*                               N Divider Register
*   @fracValue  - these 12 bits control what is loaded as the FRAC value into
*                 the fractional interpolator.
*   @intValue   - these nine bits control what is loaded as the INT value, this
*                 is used to determine the overall division factor
*   @fastlock   - when set to logic high fast-lock is enabled
*
*                               R Divider Register
*   @modValue   - set the fractional modulus, this is the ratio of the PFD
*                 frequency to the channel step resolution on the RF output
*   @rCounter   - the r counter allows the input reference frequency to
*                 be divided down to produce the reference clock to phase
*                 frequency detector
*   @prescaler  - the dual-modulus prescaler, along with the INT, FRAC and MOD
*                 counters, determines the overall division ratio from the RFin
*                 to PFD input
*   @muxout     - the on chip multiplexer selection bits
*   @loadControl - when set to logic high the value being programmed in the
*                  modulus is not loaded into the modulus. Instead, it sets the
*                  resync delay of the Sigma-Delta.
*
*                                 Control Register
*   @counterReset - resets the R and N counters
*   @cpThreeState - puts the charge pump into three-state mode when programmed
*                   to 1
*   @powerDown  - activate power down mode
*   @ldp        - lock detect precision
*   @pdPolarity - phase detector polarity
*   @cpCurrent  - Charge Pump Current settings, this should be set to the charge
*                 pump current that the loop filter is designed with
*   @refDoubler - REFin Doubler, when the doubler is enabled, both the rising
*                 and falling edges of REFin become active edges at the PFD input
*   @resync     - define the time between two resync, if it is zero, than
*                 the phase resync feature is disabled
*
*                              Noise and Spur register
*   @noiseSpur  - allows the user to optimize a design either for improved
*                 spurious performance or for improved phase noise performance
*/

typedef struct _ADF4153_settings_t {

    /* Reference Input Frequency*/
    unsigned long refIn;
    /* Channel resolution or Channel spacing */
    unsigned long channelSpacing;

    /* N Divider */
    unsigned short fracValue : 12;
    unsigned short intValue : 9;
    unsigned char fastlock : 1;

    /* R Divider */
    unsigned short modValue : 12;
    unsigned char rCounter : 4;
    unsigned char prescaler : 1;
    unsigned char muxout : 3;
    unsigned char loadControl : 1;

    /* Control Register */
    unsigned char counterReset : 1;
    unsigned char cpThreeState : 1;
    unsigned char powerDown : 1;
    unsigned char ldp : 1;
    unsigned char pdPolarity : 1;
    unsigned char cpCurrent : 4;
    unsigned char refDoubler : 1;
    unsigned char resync : 4;

    /* Noise and Spur register */
    unsigned char noiseSpur : 5;

} ADF4153_settings_t;

/*****************************************************************************/
/*  Functions Prototypes                                                     */
/*****************************************************************************/
/* Initialize the communication with the device */
char ADF4153_Init(ADF4153_settings_t ADF4153_st);

/* Update register function */
void ADF4153_UpdateLatch(unsigned long latchData);

/* Return the value of a desired latch */
unsigned long ADF4153_ReadLatch(unsigned char latchType);

/* Set the frequency to a desired value */
unsigned long long ADF4153_SetFrequency(unsigned long long frequency);

/* Return the value of the channel spacing */
unsigned long ADF4153_GetChannelSpacing( void );

#endif // __ADF4153_H__
