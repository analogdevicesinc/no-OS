/***************************************************************************//**
 *   @file   AD9643.h
 *   @brief  Header file of AD9643 Driver.
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
#ifndef _AD9643_H_
#define _AD9643_H_

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>

/******************************************************************************/
/***************************** AD9643 *****************************************/
/******************************************************************************/
/* Chip configuration registers */
#define AD9643_REG_SPI_CONFIG	            0x00
#define AD9643_REG_CHIP_ID	                0x01
#define AD9643_REG_CHIP_GRADE               0x02

/* Channel index and transfer registers */
#define AD9643_REG_CHANNEL_IDX              0x05	
#define AD9643_REG_TRANSFER                 0xFF	

/* ADC functions registers */
#define AD9643_REG_PWR_MODES                0x08	
#define AD9643_REG_GLOBAL_CLK               0x09
#define AD9643_REG_CLK_DIV                  0x0B
#define AD9643_REG_TEST_MODE                0x0D
#define AD9643_REG_BIST_EN                  0x0E
#define AD9643_REG_OFFSET_ADJ               0x10
#define AD9643_REG_OUTPUT_MODE              0x14
#define AD9643_REG_OUTPUT_ADJ               0x15
#define AD9643_REG_CLK_PHASE_CTRL           0x16
#define AD9643_REG_DCO_OUTPUT_DELAY         0x17
#define AD9643_REG_INPUT_SPAN_SEL           0x18
#define AD9643_REG_USER_TEST_PATTERN_1LSB   0x19
#define AD9643_REG_USER_TEST_PATTERN_1MSB   0x1A
#define AD9643_REG_USER_TEST_PATTERN_2LSB   0x1B
#define AD9643_REG_USER_TEST_PATTERN_2MSB   0x1C
#define AD9643_REG_USER_TEST_PATTERN_3LSB   0x1D
#define AD9643_REG_USER_TEST_PATTERN_3MSB   0x1E
#define AD9643_REG_USER_TEST_PATTERN_4LSB   0x1F
#define AD9643_REG_USER_TEST_PATTERN_4MSB   0x20
#define AD9643_REG_BIST_SIGNATURE_LSB       0x24
#define AD9643_REG_BIST_SIGNATURE_MSB       0x25
#define AD9643_REG_SYNC_CONTROL             0x3A

/* SPI_CONFIG Bit Definitions */
#define AD9643_SPI_CONFIG_LSB_FIRST     ((1 << 1) | (1 << 6))
#define AD9643_SPI_CONFIG_SOFT_RESET    ((1 << 2) | (1 << 5))

/* CHIP_GRADE Bit Definitions */
#define AD9643_CHIP_GRADE_SPEED_GRADE_ID(x) ((x) & (0x03 << 4))    

/* CHANNEL_IDX  Bit Definitions */
#define AD9643_CHANNEL_IDX_ADC_A        (1 << 0)
#define AD9643_CHANNEL_IDX_ADC_B        (1 << 1)
#define AD9643_CHANNEL_IDX_ADC_AB      	(0x03)

/* REG_TRANSFER Bit Definitions */
#define AD9643_TRANSFER_EN              (1 << 0)

/* REG_PWR_MODES Bit Definitions */
#define AD9643_PWR_MODES_INT_PWR_DOWN_MODE(x)   ((x) & 0x03)
#define AD9643_PWR_MODES_EXT_PWR_DOWN_PIN_FNC   (1 << 5)

/* GLOBAL_CLK Bit Definitions */
#define AD9643_GLOBAL_CLK_DUTY_CYCLE_STABILIZER_EN  (1 << 0)

/* CLK_DIV Bit Definitions */
#define AD9643_CLK_DIV_RATIO(x)             ((x) & 0x07)
#define AD9643_CLK_IN_CLK_DIV_PHASE_ADJ(x)  (((x) & 0x07) << 3)

/* TEST_MODE Bit Definitions */
#define AD9643_TEST_MODE_OUTPUT_MODE(x)         ((x) & 0x0F)
#define AD9643_TEST_MODE_RESET_PN_SHORT_GEN     (1 << 4)
#define AD9643_TEST_MODE_RESET_PN_LONG_GEN      (1 << 5)
#define AD9643_TEST_MODE_CTRL                   (1 << 7)

/* Test Modes Definitions */
#define AD9643_TEST_MODE_OFF                        0x00
#define AD9643_TEST_MODE_MIDSCALE_SHORT             0x01
#define AD9643_TEST_MODE_POSITIVE_FS                0x02
#define AD9643_TEST_MODE_NEGATIVE_FS                0x03
#define AD9643_TEST_MODE_ALTERNATING_CHECKERBOARD   0x04
#define AD9643_TEST_MODE_PN23_SEQ                	0x05
#define AD9643_TEST_MODE_PN9_SEQ               		0x06
#define AD9643_TEST_MODE_ONE_ZERO_TOGGLE            0x07
#define AD9643_TEST_MODE_USER_MODE                  0x08
#define AD9643_TEST_MODE_RAMP                       0x0F

/* BIST_EN Bit Definitions */
#define AD9643_BIST_EN                  (1 << 0)
#define AD9643_BIST_EN_RESET_BISRT_SEQ  (1 << 1)

/* OFFSET_ADJ Bit Definitions */
#define AD9643_OFFSET_ADJ(x)    ((x) & 0x3F)

/* OUTPUT_MODE Bit Definitions */
#define AD9643_OUTPUT_MODE_DEF       		0x00
#define AD9643_OUTPUT_MODE_OFFSET_BINARY	0x0
#define AD9643_OUTPUT_MODE_TWOS_COMPLEMENT	0x1
#define AD9643_OUTPUT_MODE_GRAY_CODE		0x2
#define AD9643_OUTPUT_MODE_OUTPUT_FORMAT(x) ((x) & 0x03)
#define AD9643_OUTPUT_MODE_OUTPUT_INVERT    (1 << 2)
#define AD9643_OUTPUT_MODE_OUTPUT_EN        (1 << 4)

/* OUTPUT_ADJ Bit Definitions */
#define AD9643_REG_OUTPUT_ADJ_VAL(x) ((x) & 0x0F)

/* CLK_PHASE_CTRL Bit Definitions */
#define AD9643_CLK_PHASE_CTRL_EVEN_ODD_MODE_EN		0x20
#define AD9643_CLK_PHASE_CTRL_MODE              (1 << 5)
#define AD9643_CLK_PHASE_CTRL_INVERT_DCO_CLK    (1 << 7)

/* DCO_OUTPUT_DELAY Bit Definitions */
#define AD9643_DCO_OUTPUT_DELAY_DCO_CLK_DELAY(x)   ((x) & 0x1F)
#define AD9643_DCO_OUTPUT_DELAY_EN_DCO_CLK_DELAY   (1 << 7)

/* INPUT_SPAN_SEL Bit Definitions */
#define AD9643_INPUT_SPAN_SEL_FULL_SCALE_VOLTAGE(x)   ((x) & 0x1F)

/* SYNC_CONTROL Bit Definitions */
#define AD9643_SYNC_CONTROL_MASTER_SYNC_BUF_EN      (1 << 0)
#define AD9643_SYNC_CONTROL_CLK_DIV_SYNC_EN         (1 << 1)
#define AD9643_SYNC_CONTROL_CLK_DIV_NEXT_SYNC_ONLY  (1 << 2)

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/** Initializes the AD9643. */
int32_t ad9643_setup();
/** Resets the device. */
int32_t ad9643_reset();
/** Configures the external power-down pin function. */
int32_t ad9643_ext_pwd_pin_fnc(int32_t fnc);
/** Configures the power mode. */
int32_t ad9643_pwd_mode(int32_t mode);
/** Enables (0) or disables (1) the duty cycle stabilizer.  */
int32_t ad9643_clock_duty_cycle_stabilizer(int32_t en);
/** Configures the input clock divide ratio and returns the set divide ratio. */
int32_t ad9643_clock_divide_ratio(int32_t ratio);
/** Configures the phase adjustment in clock cycles delay and returns the set phase adjustment. */
int32_t ad9643_clock_phase_adj(int32_t adj);
/** Sets the offset adjustment. Returns the set offset adjustment. */
int32_t ad9643_offset_adj(int32_t adj);
/** Enables (1) or disables (0) the data output. Returns output enable state. */
int32_t ad9643_output_enable(int32_t en);
/** Activates the normal (1) or inverted output mode. Returns the set output mode. */
int32_t ad9643_output_invert(int32_t invert);
/** Specifies the output formatReturns the set output format. */
int32_t ad9643_output_format(int32_t format);
/** Sets the output current adjustment. Returns the set current adjustment. */
int32_t ad9643_output_current_adj(int32_t adj);
/** Activates the normal (0) or inverted (1) DCO clock. Returns the DCO clock inversion status. */
int32_t ad9643_dco_clock_invert(int32_t invert);
/** Enables (0) or disables (1) the even/odd mode output. Returns the set clock mode. */
int32_t ad9643_dco_clock_mode(int32_t mode);
/** Configures the clock delay setting. Returns the set clock delay. */
int32_t ad9643_dco_output_clock_delay(int32_t delay);
/** Configures the full-scale input voltage selection. Returns the set input voltage selection. */
int32_t ad9643_input_span(int32_t span);
/** Sets the ADC's test mode. Returns the set test mode. */
int32_t ad9643_test_mode(int32_t mode, int32_t chan_mask);
/** Sets the ADC's user test pattern. */
int32_t ad9643_user_test_pattern(uint8_t* pattern);
/** Calibrates the DCO clock delay. */
int32_t ad9643_dco_calibrate_2c();
/** Checks if the DCO is locked. */
int32_t ad9643_is_dco_locked();

#endif /* __AD9643_H__ */
