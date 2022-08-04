/**************************************************************************//**
*   @file   adf4106.h
*   @brief  Header file of ADF4106 driver. This driver supporting the following
*           devices : ADF4001, ADF4002, ADF4106
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
******************************************************************************/
#ifndef __ADF4106_H__
#define __ADF4106_H__

/*****************************************************************************/
/****************************** Include Files ********************************/
/*****************************************************************************/
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

/*****************************************************************************/
/*  Device specific MACROs                                                   */
/*****************************************************************************/
/* GPIOs */
#define ADF4106_LE_OUT                      no_os_gpio_direction_output(dev->gpio_le,  \
			                    NO_OS_GPIO_HIGH)
#define ADF4106_LE_LOW                      no_os_gpio_set_value(dev->gpio_le,         \
			                    NO_OS_GPIO_LOW)
#define ADF4106_LE_HIGH                     no_os_gpio_set_value(dev->gpio_le,         \
			                    NO_OS_GPIO_HIGH)

#define ADF4106_CE_OUT                      no_os_gpio_direction_output(dev->gpio_ce,  \
			                    NO_OS_GPIO_HIGH)
#define ADF4106_CE_LOW                      no_os_gpio_set_value(dev->gpio_ce,         \
			                    NO_OS_GPIO_LOW)
#define ADF4106_CE_HIGH                     no_os_gpio_set_value(dev->gpio_ce,         \
			                    NO_OS_GPIO_HIGH)

#define ADF4106_LE2_OUT                     no_os_gpio_direction_output(dev->gpio_le2, \
			                    NO_OS_GPIO_HIGH)
#define ADF4106_LE2_LOW                     no_os_gpio_set_value(dev->gpio_le2,        \
			                    NO_OS_GPIO_LOW)
#define ADF4106_LE2_HIGH                    no_os_gpio_set_value(dev->gpio_le2,        \
			                    NO_OS_GPIO_HIGH)

#define ADF4106_CE2_OUT                     no_os_gpio_direction_output(dev->gpio_ce2, \
			                    NO_OS_GPIO_HIGH)
#define ADF4106_CE2_LOW                     no_os_gpio_set_value(dev->gpio_ce2,        \
			                    NO_OS_GPIO_LOW)
#define ADF4106_CE2_HIGH                    no_os_gpio_set_value(dev->gpio_ce2,        \
			                    NO_OS_GPIO_HIGH)

/* Control Bits */
#define ADF4106_CTRL_MASK                   0x3

#define ADF4106_CTRL_R_COUNTER              0          /* Reference Counter */
#define ADF4106_CTRL_N_COUNTER              1          /* N Counter */
#define ADF4106_CTRL_FUNCTION_LATCH         2          /* Function Latch*/
#define ADF4106_CTRL_INIT_LATCH             3          /* Initialization Latch*/

/* Reference Counter Latch */

#define ADF4106_R_COUNTER_OFFSET            2
#define ADF4106_R_COUNTER_MASK              0x3FFFul
#define ADF4106_R_COUNTER(x)                ((x) & ADF4106_R_COUNTER_MASK) \
                                            << ADF4106_R_COUNTER_OFFSET
/* Anti-backlash Pulse Width options */
#define ADF4106_R_ABP_OFFSET                16
#define ADF4106_R_ABP_MASK                  0x3
#define ADF4106_R_ABP(x)                    ((x) & ADF4106_R_ABP_MASK) \
                                            << ADF4106_R_ABP_OFFSET
#define ADF4106_R_ABP_2_9NS                 0
#define ADF4106_R_ABP_1_3NS                 1
#define ADF4106_R_ABP_6_0NS                 2

/* Test Mode Bits */
#define ADF4106_R_TMB_OFFSET                18
#define ADF4106_R_TMB_MASK                  0x1
#define ADF4106_R_TMB(x)                    ((x) & ADF4106_R_TMB_MASK) \
                                            << ADF4106_R_TMB_OFFSET
#define ADF4106_R_TMB_NORMAL                0
/* Lock Detect Precision */
#define ADF4106_R_LDP_OFFSET                20
#define ADF4106_R_LDP_MASK                  0x1
#define ADF4106_R_LDP(x)                    ((x) & ADF4106_R_LDP_MASK) \
                                            <<  ADF4106_R_LDP_OFFSET
#define ADF4106_R_LDP_3                     0
#define ADF4106_R_LDP_5                     1

/* N Counter Latch */
#define ADF4106_N_COUNTER_A_OFFSET          2
#define ADF4106_N_COUNTER_A_MASK            0x3F
#define ADF4106_N_COUNTER_A(x)              ((x) & ADF4106_N_COUNTER_A_MASK) \
                                            << ADF4106_N_COUNTER_A_OFFSET
#define ADF4106_N_COUNTER_B_OFFSET          8
#define ADF4106_N_COUNTER_B_MASK            0x1FFF
#define ADF4106_N_COUNTER_B(x)              ((x) & ADF4106_N_COUNTER_B_MASK) \
                                            << ADF4106_N_COUNTER_B_OFFSET
/* Charge Pump Gain Settings */
#define ADF4106_N_CP_OFFSET                 21
#define ADF4106_N_CP_MASK                   0x1
#define ADF4106_N_CP(x)                     ((x) & ADF4106_N_CP_MASK) \
                                            << ADF4106_N_CP_OFFSET
#define ADF4106_N_CP_GAIN_1                 0
#define ADF4106_N_CP_GAIN_2                 1

/********* Function and Initialization register offsets and masks ************/
/* Counter Reset Bit */
#define ADF4106_CR_OFFSET                   2
#define ADF4106_CR_MASK                     0x1ul
#define ADF4106_CR(x)                       ((x) & ADF4106_CR_MASK) \
                                            << ADF4106_CR_OFFSET
/* Power Down Bit 1 */
#define ADF4106_PD1_OFFSET                  3
#define ADF4106_PD1_MASK                    0x1
#define ADF4106_PD1(x)                      ((x) & ADF4106_PD1_MASK) \
                                            << ADF4106_PD1_OFFSET
/* Muxout Control */
#define ADF4106_MUXOUT_OFFSET               4
#define ADF4106_MUXOUT_MASK                 0x7
#define ADF4106_MUXOUT(x)                   ((x) & ADF4106_MUXOUT_MASK) \
                                            << ADF4106_MUXOUT_OFFSET
/* Phase Detector Polarity */
#define ADF4106_PDPOL_OFFSET                7
#define ADF4106_PDPOL_MASK                  0x1
#define ADF4106_PDPOL(x)                    ((x) & ADF4106_PDPOL_MASK) \
                                            << ADF4106_PDPOL_OFFSET
/* Charge Pump Output*/
#define ADF4106_CP_OFFSET                   8
#define ADF4106_CP_MASK                     0x1
#define ADF4106_CP(x)                       ((x) & ADF4106_CP_MASK) \
                                            << ADF4106_CP_OFFSET
/* Fast-lock Mode */
#define ADF4106_FASTLOCK_OFFSET             9
#define ADF4106_FASTLOCK_MASK               0x3
#define ADF4106_FASTLOCK(x)                 ((x) & ADF4106_FASTLOCK_MASK) \
                                            << ADF4106_FASTLOCK_OFFSET
/* Timer Counter Control */
#define ADF4106_TCC_OFFSET                  11
#define ADF4106_TCC_MASK                    0xF
#define ADF4106_TCC(x)                      ((x) & ADF4106_TCC_MASK) \
                                            << ADF4106_TCC_OFFSET
/* Current Setting Position */
#define ADF4106_CS1_OFFSET                  15
#define ADF4106_CS1_MASK                    0x7
#define ADF4106_CS1(x)                      ((x) << ADF4106_CS1_OFFSET)
#define ADF4106_CS2_OFFSET                  18
#define ADF4106_CS2_MASK                    0x7
#define ADF4106_CS2(x)                      ((x) << ADF4106_CS2_OFFSET)

/* Synchronous or asynchronous power down*/
#define ADF4106_PD2_OFFSET                  21
#define ADF4106_PD2_MASK                    0x1
#define ADF4106_PD2(x)                      ((x) & ADF4106_PD2_MASK) \
                                            << ADF4106_PD2_OFFSET
/* Prescaler value */
#define ADF4106_PS_OFFSET                   22
#define ADF4106_PS_MASK                     0x3
#define ADF4106_PS(x)                       ((x) & ADF4106_PS_MASK) \
                                            << ADF4106_PS_OFFSET

/* Counter Reset Bit Definition */
#define ADF4106_CR_NORMAL                 0
#define ADF4106_CR_RESET                  1
/* Power Down Bit 1 Definition */
#define ADF4106_PD1_NORMAL                0
#define ADF4106_PD1_POWER_DOWN            1
/* Muxout Control Definition */
#define ADF4106_MUXOUT_3STATE             0
#define ADF4106_MUXOUT_DLOCK_DETECT       1
#define ADF4106_MUXOUT_NDIV_OUTPUT        2
#define ADF4106_MUXOUT_AVDD               3
#define ADF4106_MUXOUT_RDIV_OUTPUT        4
#define ADF4106_MUXOUT_NCH_OPENDRAIN      5
#define ADF4106_MUXOUT_SERIAL_OUTPUT      6
#define ADF4106_MUXOUT_DGND               7
/* Phase Detector Polarity Definition */
#define ADF4106_PDPOL_NEGATIVE            0
#define ADF4106_PDPOL_POSITIVE            1
/* Charge Pump Output Definition */
#define ADF4106_CP_NORMAL                 0
#define ADF4106_CP_THREE_STATE            1
/* Fast-lock Mode Definition */
#define ADF4106_FASTLOCK_DISABLE          0
#define ADF4106_FASTLOCK_MODE1            1
#define ADF4106_FASTLOCK_MODE2            3
/* Timer Counter Control Definition */
#define ADF4106_TCC_3                     0
#define ADF4106_TCC_7                     1
#define ADF4106_TCC_11                    2
#define ADF4106_TCC_15                    3
#define ADF4106_TCC_19                    4
#define ADF4106_TCC_23                    5
#define ADF4106_TCC_27                    6
#define ADF4106_TCC_31                    7
#define ADF4106_TCC_35                    8
#define ADF4106_TCC_39                    9
#define ADF4106_TCC_43                    10
#define ADF4106_TCC_47                    11
#define ADF4106_TCC_51                    12
#define ADF4106_TCC_55                    13
#define ADF4106_TCC_59                    14
#define ADF4106_TCC_63                    15
/* Current Settings Definitions */
#define ADF4106_CS_0_62                   0
#define ADF4106_CS_1_25                   1
#define ADF4106_CS_1_87                   2
#define ADF4106_CS_2_5                    3
#define ADF4106_CS_3_12                   4
#define ADF4106_CS_3_75                   5
#define ADF4106_CS_4_37                   6
#define ADF4106_CS_5_0                    7
/* Synchronous or asynchronous power down Definition */
#define ADF4106_ASYNC_PWD                 0
#define ADF4106_SYNC_PWD                  1
/* Prescaler value Definition */
#define ADF4106_PS_8_9                    0
#define ADF4106_PS_16_17                  1
#define ADF4106_PS_32_33                  2
#define ADF4106_PS_64_65                  3

/* Default prescaler for ADF4001 and ADF4002 */
#define ADF4106_PRESCALE(x)                 (8 << (x))

/*****************************************************************************/
/************************** Types Declarations *******************************/
/*****************************************************************************/

/**
*   @struct adf4106_settings_t
*   @brief store the value of all the latch and the input
*                               reference frequency
*/

struct adf4106_settings_t {

	/** Reference Input Frequency */
	uint32_t ref_in;

	/** PFD max frequency */
	uint32_t pfd_max;

	/** The initial value of the 14 bit Reference Counter register */
	uint16_t ref_counter : 14;
	/** The width of the anti-backlash pulse, this pulse
	 * ensures that no dead zone is in the PFD transfer function and minimizes
	 * phase noise and reference spurs.
	 */
	uint8_t anti_backlash_width : 2;
	/** Should be set to zero for Normal operation */
	uint8_t test_mode_bits : 1;
	/** determines the number of consecutive cycles of phase
	 * delay, that must occur before lock detect is set
	 */
	uint8_t lock_detect_precision : 1;

	/* N Latch */
	/** a 6 bits counter is supported at ADF4106 */
	uint8_t a_n_counter : 6;
	/** a 13 bits counter */
	uint16_t b_n_counter : 13;
	/** determines which charge pump current settings is used */
	uint8_t cp_gain : 1;

	/* Functional/Initialization latch */
	/** resets the R and N counters */
	uint8_t counter_reset : 1;
	/** activate power down mode */
	uint8_t power_down1 : 1;
	/** the type of the MUXOUT output */
	uint8_t muxout_control : 3;
	/** the polarity of the Phase Detector */
	uint8_t phase_detector_pol : 1;
	/** the type of the Charge Pump output */
	uint8_t cp_type : 1;
	/** set the desired Fast Lock Mode */
	uint8_t fast_lock_mode : 2;
	/** how long will be the secondary charge pump current
	 * active, before reverting to the primary current
	 */
	uint8_t timer_counter_control : 4;
	/** is used when the RF output is stable and the system is
	 * in static state
	 */
	uint8_t current_setting1 : 3;
	/** is meant to be used when the system is dynamic and in a
	 * state of change (i.e., when a new output frequency is programmed)
	 */
	uint8_t current_setting2 : 3;
	/** define the type of the power down */
	uint8_t power_down2 : 1;
	/** the value of the prescaler */
	uint8_t prescaler_value : 2;

};

/* Supported devices */
enum adf4106_type_t {
	ID_ADF4001,
	ID_ADF4002,
	ID_ADF4106
};

/* Initialization methods */
enum adf4106_init_t {
	INIT_LATCH,
	INIT_CEPIN,
	INIT_COUNTER_RESET
};

struct adf4106_chip_info {
	uint64_t vco_max_frequency;
	uint32_t pfd_max_frequency;
	uint32_t vco_min_frequency;
	uint32_t pfd_min_frequency;
};

struct adf4106_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_le;
	struct no_os_gpio_desc	*gpio_ce;
	struct no_os_gpio_desc	*gpio_le2;
	struct no_os_gpio_desc	*gpio_ce2;
	/* Device Settings */
	struct adf4106_chip_info chip_info;
	struct adf4106_settings_t adf4106_st;
	enum adf4106_type_t this_device;
	/* Internal buffers for each latch */
	uint32_t r_latch;
	uint32_t n_latch;
	uint32_t f_latch;
	uint32_t i_latch;
};

struct adf4106_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param		gpio_le;
	struct no_os_gpio_init_param		gpio_ce;
	struct no_os_gpio_init_param		gpio_le2;
	struct no_os_gpio_init_param		gpio_ce2;
	/* Device Settings */
	enum adf4106_type_t this_device;
	enum adf4106_init_t init_method;
	struct adf4106_settings_t adf4106_st;
};

/*****************************************************************************/
/*  Functions Prototypes                                                     */
/*****************************************************************************/
/* Initialize the communication with the device */
int8_t adf4106_init(struct adf4106_dev **device,
		    struct adf4106_init_param init_param);

/* Free the resources allocated by adf4106_init(). */
int32_t adf4106_remove(struct adf4106_dev *dev);

/* Update register function */
void adf4106_update_latch(struct adf4106_dev *dev,
			  uint32_t latch_data);

/* Return the value of a desired latch */
uint32_t adf4106_read_latch(struct adf4106_dev *dev,
			    uint8_t latch_type);

/* PLL initialization functions */
void adf4106_init_latch_method(struct adf4106_dev *dev);
void adf4106_init_cepin_method(struct adf4106_dev *dev);
void adf4106_init_counte_reset_method(struct adf4106_dev *dev);

/* Set the frequency to a desired value */
uint64_t adf4106_set_frequency(struct adf4106_dev *dev,
			       uint64_t frequency);

#endif // __ADF4106_H__
