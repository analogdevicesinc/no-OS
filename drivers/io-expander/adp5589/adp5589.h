/***************************************************************************//**
 *   @file   ADP5589.h
 *   @brief  Header file of ADP5589 Driver.
 *   @author Mihai Bancisor (Mihai.Bancisor@analog.com)
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
*******************************************************************************/
#ifndef __ADP5589_H__
#define __ADP5589_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_i2c.h"

/******************************************************************************/
/************************** ADP5589 Definitions *******************************/
/******************************************************************************/

#define PMOD_IOXP_J1		0	// J1 port of PmodIOXP
#define PMOD_IOXP_J2		1	// J2 port of PmodIOXP
#define ADP5589_ADDRESS		0x34	// I2C ADDRESS
#define ADP5589_ID		0x10	// Manufacturer ID

/* Register address definitions */
#define ADP5589_ADR_ID               	0x00
#define ADP5589_ADR_INT_STATUS       	0x01
#define ADP5589_ADR_STATUS           	0x02
#define ADP5589_ADR_FIFO1            	0x03
#define ADP5589_ADR_FIFO2            	0x04
#define ADP5589_ADR_FIFO3            	0x05
#define ADP5589_ADR_FIFO4            	0x06
#define ADP5589_ADR_FIFO5            	0x07
#define ADP5589_ADR_FIFO6            	0x08
#define ADP5589_ADR_FIFO7            	0x09
#define ADP5589_ADR_FIFO8            	0x0A
#define ADP5589_ADR_FIFO9            	0x0B
#define ADP5589_ADR_FIFO10           	0x0C
#define ADP5589_ADR_FIFO11           	0x0D
#define ADP5589_ADR_FIFO12           	0x0E
#define ADP5589_ADR_FIFO13           	0x0F
#define ADP5589_ADR_FIFO14           	0x10
#define ADP5589_ADR_FIFO15           	0x11
#define ADP5589_ADR_FIFO16           	0x12
#define ADP5589_ADR_GPI_INT_STATUS_A   	0x13
#define ADP5589_ADR_GPI_INT_STATUS_B   	0x14
#define ADP5589_ADR_GPI_INT_STATUS_C   	0x15
#define ADP5589_ADR_GPI_STATUS_A       	0x16
#define ADP5589_ADR_GPI_STATUS_B       	0x17
#define ADP5589_ADR_GPI_STATUS_C       	0x18
#define ADP5589_ADR_RPULL_CONFIG_A   	0x19
#define ADP5589_ADR_RPULL_CONFIG_B   	0x1A
#define ADP5589_ADR_RPULL_CONFIG_C   	0x1B
#define ADP5589_ADR_RPULL_CONFIG_D   	0x1C
#define ADP5589_ADR_RPULL_CONFIG_E	0x1D
#define ADP5589_ADR_GPI_INT_LEVEL_A	0x1E
#define ADP5589_ADR_GPI_INT_LEVEL_B	0x1F
#define ADP5589_ADR_GPI_INT_LEVEL_C    	0x20
#define ADP5589_ADR_GPI_EVENT_EN_A     	0x21
#define ADP5589_ADR_GPI_EVENT_EN_B     	0x22
#define ADP5589_ADR_GPI_EVENT_EN_C     	0x23
#define ADP5589_ADR_GPI_INTERRUPT_EN_A 	0x24
#define ADP5589_ADR_GPI_INTERRUPT_EN_B 	0x25
#define ADP5589_ADR_GPI_INTERRUPT_EN_C 	0x26
#define ADP5589_ADR_DEBOUNCE_DIS_A     	0x27
#define ADP5589_ADR_DEBOUNCE_DIS_B     	0x28
#define ADP5589_ADR_DEBOUNCE_DIS_C     	0x29
#define ADP5589_ADR_GPO_DATA_OUT_A     	0x2A
#define ADP5589_ADR_GPO_DATA_OUT_B     	0x2B
#define ADP5589_ADR_GPO_DATA_OUT_C     	0x2C
#define ADP5589_ADR_GPO_OUT_MODE_A     	0x2D
#define ADP5589_ADR_GPO_OUT_MODE_B     	0x2E
#define ADP5589_ADR_GPO_OUT_MODE_C     	0x2F
#define ADP5589_ADR_GPIO_DIRECTION_A 	0x30
#define ADP5589_ADR_GPIO_DIRECTION_B   	0x31
#define ADP5589_ADR_GPIO_DIRECTION_C	0x32
#define ADP5589_ADR_UNLOCK1          	0x33
#define ADP5589_ADR_UNLOCK2          	0x34
#define ADP5589_ADR_EXT_LOCK_EVENT   	0x35
#define ADP5589_ADR_UNLOCK_TIMERS	0x36
#define ADP5589_ADR_LOCK_CFG          	0x37
#define ADP5589_ADR_RESET1_EVENT_A     	0x38
#define ADP5589_ADR_RESET1_EVENT_B     	0x39
#define ADP5589_ADR_RESET1_EVENT_C     	0x3A
#define ADP5589_ADR_RESET2_EVENT_A     	0x3B
#define ADP5589_ADR_RESET2_EVENT_B     	0x3C
#define ADP5589_ADR_RESET_CFG         	0x3D
#define ADP5589_ADR_PWM_OFFT_LOW       	0x3E
#define ADP5589_ADR_PWM_OFFT_HIGH      	0x3F
#define ADP5589_ADR_PWM_ONT_LOW        	0x40
#define ADP5589_ADR_PWM_ONT_HIGH      	0x41
#define ADP5589_ADR_PWM_CFG           	0x42
#define ADP5589_ADR_CLOCK_DIV_CFG      	0x43
#define ADP5589_ADR_LOGIC_1_CFG        	0x44
#define ADP5589_ADR_LOGIC_2_CFG        	0x45
#define ADP5589_ADR_LOGIC_FF_CFG       	0x46
#define ADP5589_ADR_LOGIC_INT_EVENT    	0x47
#define ADP5589_ADR_POLL_TIME_CFG      	0x48
#define ADP5589_ADR_PIN_CONFIG_A       	0x49
#define ADP5589_ADR_PIN_CONFIG_B       	0x4A
#define ADP5589_ADR_PIN_CONFIG_C       	0x4B
#define ADP5589_ADR_PIN_CONFIG_D       	0x4C
#define ADP5589_ADR_GENERAL_CFG_B      	0x4D
#define ADP5589_ADR_INT_EN            	0x4E


/* Register Bit Mask Definitions. */
/* ID Register bits 0x00. */
#define ADP5589_ID_MAN_ID  			(0xF0)
#define ADP5589_ID_REV_ID  			(0x0F)
/* INT_STATUS Register bits 0x01. */
#define ADP5589_INT_STATUS_EVENT_INT   		(1 << 0)
#define ADP5589_INT_STATUS_GPI_INT     		(1 << 1)
#define ADP5589_INT_STATUS_OVERFLOW_INT 	(1 << 2)
#define ADP5589_INT_STATUS_LOCK_INT   		(1 << 3)
#define ADP5589_INT_STATUS_LOGIC1_INT  	 	(1 << 4)
#define ADP5589_INT_STATUS_LOGIC2_INT  		(1 << 5)
/* STATUS Register bits 0x02. */
#define ADP5589_STATUS_EC(x)			(((x) & 0x1F) << 0)
#define ADP5589_STATUS_LOCK_STAT  	 	(1 << 5)
#define ADP5589_STATUS_LOGIC1_STAT  	  	(1 << 6)
#define ADP5589_STATUS_LOGIC2_STAT  	 	(1 << 7)
/* INT_EN Register bits 0x4E. */
#define ADP5589_INT_EN_EVENT_IEN   		(1 << 0)
#define ADP5589_INT_EN_GPI_IEN     		(1 << 1)
#define ADP5589_INT_EN_OVERFLOW_IEN 		(1 << 2)
#define ADP5589_INT_EN_LOCK_IEN   		(1 << 3)
#define ADP5589_INT_EN_LOGIC1_INT  		(1 << 4)
#define ADP5589_INT_EN_LOGIC2_INT  		(1 << 5)
/* GENERAL_CFG_B Register bits  0x4D. */
#define ADP5589_GENERAL_CFG_B_RST_CFG   	(1 << 0)
#define ADP5589_GENERAL_CFG_B_INT_CFG   	(1 << 1)
#define ADP5589_GENERAL_CFG_B_LCK_TRK_GPI   	(1 << 3)
#define ADP5589_GENERAL_CFG_B_LCK_TRK_LOGIC  	(1 << 4)
#define ADP5589_GENERAL_CFG_B_CORE_FREQ(x)  	(((x) & 0x03) << 5)
#define ADP5589_GENERAL_CFG_B_OSC_EN  		(1 << 7)
/* PIN_CONFIG_D Register bits 0x4C. */
#define ADP5589_PIN_CONFIG_D_R0_EXTEND  	(1 << 0)
#define ADP5589_PIN_CONFIG_D_C9_EXTEND  	(1 << 1)
#define ADP5589_PIN_CONFIG_D_R3_EXTEND(x)	(((x) & 0x03) << 2)
#define ADP5589_PIN_CONFIG_D_C6_EXTEND	  	(1 << 4)
#define ADP5589_PIN_CONFIG_D_R4_EXTEND	  	(1 << 5)
#define ADP5589_PIN_CONFIG_D_C4_EXTEND	  	(1 << 6)
#define ADP5589_PIN_CONFIG_D_PULL_SELECT  	(1 << 7)
/* GPI_STATUS_A Register bits 0x16. */
#define ADP5589_GPI_STATUS_GPI_1_STAT 		(1 << 0)
#define ADP5589_GPI_STATUS_GPI_2_STAT 		(1 << 1)
#define ADP5589_GPI_STATUS_GPI_3_STAT 		(1 << 2)
#define ADP5589_GPI_STATUS_GPI_4_STAT 		(1 << 3)
#define ADP5589_GPI_STATUS_GPI_5_STAT 		(1 << 4)
#define ADP5589_GPI_STATUS_GPI_6_STAT 		(1 << 5)
#define ADP5589_GPI_STATUS_GPI_7_STAT 		(1 << 6)
#define ADP5589_GPI_STATUS_GPI_8_STAT 		(1 << 7)
/* GPI_STATUS_B Register bits 0x17. */
#define ADP5589_GPI_STATUS_GPI_9_STAT  		(1 << 0)
#define ADP5589_GPI_STATUS_GPI_10_STAT  	(1 << 1)
#define ADP5589_GPI_STATUS_GPI_11_STAT  	(1 << 2)
#define ADP5589_GPI_STATUS_GPI_12_STAT  	(1 << 3)
#define ADP5589_GPI_STATUS_GPI_13_STAT  	(1 << 4)
#define ADP5589_GPI_STATUS_GPI_14_STAT  	(1 << 5)
#define ADP5589_GPI_STATUS_GPI_15_STAT  	(1 << 6)
#define ADP5589_GPI_STATUS_GPI_16_STAT  	(1 << 7)
/* GPI_STATUS_C Register bits 0x18. */
#define ADP5589_GPI_STATUS_GPI_17_STAT  	(1 << 0)
#define ADP5589_GPI_STATUS_GPI_18_STAT  	(1 << 1)
#define ADP5589_GPI_STATUS_GPI_19_STAT  	(1 << 2)
/* GPI_EVENT_EN_A Register bits 0x21. */
#define ADP5589_GPI_EVENT_EN_GPI_1_STAT  	(1 << 0)
#define ADP5589_GPI_EVENT_EN_GPI_2_STAT  	(1 << 1)
#define ADP5589_GPI_EVENT_EN_GPI_3_STAT  	(1 << 2)
#define ADP5589_GPI_EVENT_EN_GPI_4_STAT  	(1 << 3)
#define ADP5589_GPI_EVENT_EN_GPI_5_STAT  	(1 << 4)
#define ADP5589_GPI_EVENT_EN_GPI_6_STAT  	(1 << 5)
#define ADP5589_GPI_EVENT_EN_GPI_7_STAT  	(1 << 6)
#define ADP5589_GPI_EVENT_EN_GPI_8_STAT  	(1 << 7)
/* GPI_EVENT_EN_B Register bits 0x22. */
#define ADP5589_GPI_EVENT_EN_GPI_9_STAT  	(1 << 0)
#define ADP5589_GPI_EVENT_EN_GPI_10_STAT  	(1 << 1)
#define ADP5589_GPI_EVENT_EN_GPI_11_STAT  	(1 << 2)
#define ADP5589_GPI_EVENT_EN_GPI_12_STAT  	(1 << 3)
#define ADP5589_GPI_EVENT_EN_GPI_13_STAT  	(1 << 4)
#define ADP5589_GPI_EVENT_EN_GPI_14_STAT  	(1 << 5)
#define ADP5589_GPI_EVENT_EN_GPI_15_STAT  	(1 << 6)
#define ADP5589_GPI_EVENT_EN_GPI_16_STAT  	(1 << 7)
/* GPI_EVENT_EN_C Register bits 0x23. */
#define ADP5589_GPI_EVENT_EN_GPI_17_STAT  	(1 << 0)
#define ADP5589_GPI_EVENT_EN_GPI_18_STAT  	(1 << 1)
#define ADP5589_GPI_EVENT_EN_GPI_19_STAT  	(1 << 2)

/* UNLOCK1 Register bits 0x33. */
#define ADP5589_UNLOCK1_UNLOCK1_STATE	  	(1 << 7)
#define ADP5589_UNLOCK1_UNLOCK1_UNLOCK1(x) 	(((x) & 0x7F) << 0)
/* UNLOCK2 Register bits 0x34. */
#define ADP5589_UNLOCK2_UNLOCK2_STATE	  	(1 << 7)
#define ADP5589_UNLOCK2_UNLOCK2_UNLOCK2(x) 	(((x) & 0x7F) << 0)
/* EXT_LOCK_EVENT Register bits 0x35. */
#define ADP5589_EXT_LOCK_EXT_LOCK_STATE		(1 << 7)
#define ADP5589_EXT_LOCK_EXT_LOCK_EVENT(x) 	(((x) & 0x7F) << 0)
/* UNLOCK_TIMERS Register bits 0x36. */
#define ADP5589_UNLOCK_TIMERS_INT_MASK_TIMER(x)	(((x) & 0xF8) << 3)
#define ADP5589_UNLOCK_TIMERS_UNLOCK_TIMER(x)  	(((x) & 0x07) << 0)
/* UNLOCK_TIMER bits. */
#define ADP5589_UNLOCK_TIMER_DIS	        0
#define ADP5589_UNLOCK_TIMER_1SEC	        1
#define ADP5589_UNLOCK_TIMER_2SEC	        2
#define ADP5589_UNLOCK_TIMER_3SEC	        3
#define ADP5589_UNLOCK_TIMER_4SEC	        4
#define ADP5589_UNLOCK_TIMER_5SEC	        5
#define ADP5589_UNLOCK_TIMER_6SEC	        6
#define ADP5589_UNLOCK_TIMER_7SEC	        7
/* INT_MASK_TIMER bits. */
#define ADP5589_INT_MASTER_TIMER_DIS	        0
#define ADP5589_INT_MASTER_TIMER_1SEC	        1
#define ADP5589_INT_MASTER_TIMER_2SEC	        2
#define ADP5589_INT_MASTER_TIMER_30SEC	        (0X1E)
#define ADP5589_INT_MASTER_TIMER_31SEC	        (0X1F)
/* LOCK_CFG Register bits 0x37. */
#define ADP5589_LOCK_CFG_LOCK_EN		(1 << 0)
/* RESET_CFG Register bits 0x3D. */
#define ADP5589_RESET_CFG_RESET_PULSE_WIDTH(x)	(((x) & 0x03) << 0)
#define ADP5589_RESET_CFG_RESET_TRIGGER_TIME(x)	(((x) & 0x07) << 2)
#define ADP5589_RESET_CFG_RST_PASSTHRU_EN	(1 << 5)
#define ADP5589_RESET_CFG_RESET1_POL		(1 << 6)
#define ADP5589_RESET_CFG_RESET2_POL		(1 << 7)
/* RESET_TRIGGER_TIME bits. */
#define ADP5589_RESET_CFG_RESET_TRIGGER_TIME_IMMED	0
#define ADP5589_RESET_CFG_RESET_TRIGGER_TIME_1D0SEC	1
#define ADP5589_RESET_CFG_RESET_TRIGGER_TIME_1D5SEC	2
#define ADP5589_RESET_CFG_RESET_TRIGGER_TIME_2D0SEC	3
#define ADP5589_RESET_CFG_RESET_TRIGGER_TIME_2D5SEC	4
#define ADP5589_RESET_CFG_RESET_TRIGGER_TIME_3D0SEC	5
#define ADP5589_RESET_CFG_RESET_TRIGGER_TIME_3D5SEC	6
#define ADP5589_RESET_CFG_RESET_TRIGGER_TIME_4D0SEC	7
/* RESET_PULSE_WIDTH bits. */
#define ADP5589_RESET_CFG_RESET_PULSE_WIDTH_500US	0
#define ADP5589_RESET_CFG_RESET_PULSE_WIDTH_1MS		1
#define ADP5589_RESET_CFG_RESET_PULSE_WIDTH_2MS		2
#define ADP5589_RESET_CFG_RESET_PULSE_WIDTH_10MS	3
/* PWM_CFG Register bits 0x42. */
#define ADP5589_PWM_CFG_PWM_EN				(1 << 0)
#define ADP5589_PWM_CFG_PWM_MODE			(1 << 1)
#define ADP5589_PWM_CFG_PWM_IN_AND			(1 << 2)
/* CLOCK_DIV_CFG Register bits 0x43. */
#define ADP5589_CLOCK_DIV_CFG_CLK_INV			(1 << 6)
#define ADP5589_CLOCK_DIV_CFG_CLK_DIV(x)		(((x) & 0x1F) << 1)
#define ADP5589_CLOCK_DIV_CFG_CLK_DIV_EN		(1 << 0)
/* CLK_DIV bits. */
#define ADP5589_CLOCK_DIV_CFG_CLK_DIV_DIV1		(0X00)
#define ADP5589_CLOCK_DIV_CFG_CLK_DIV_DIV2		(0X01)
#define ADP5589_CLOCK_DIV_CFG_CLK_DIV_DIV3		(0X02)
#define ADP5589_CLOCK_DIV_CFG_CLK_DIV_DIV4		(0X03)
#define ADP5589_CLOCK_DIV_CFG_CLK_DIV_DIV32		(0X1F)
/* LOGIC_1_CFG Register bits 0x44. */
#define ADP5589_LOGIC_1_CFG_LOGIC1_SEL(x)		(((x) & 0x07) << 0)
#define ADP5589_LOGIC_1_LA1_INV				(1 << 3)
#define ADP5589_LOGIC_1_LB1_INV				(1 << 4)
#define ADP5589_LOGIC_1_LC1_INV				(1 << 5)
#define ADP5589_LOGIC_1_LY1_INV				(1 << 6)
/* LOGIC1_SEL bits. */
#define ADP5589_LOGIC_CFG_LOGIC_SEL_OFF			(0x00)
#define ADP5589_LOGIC_CFG_LOGIC_SEL_AND			(0x01)
#define ADP5589_LOGIC_CFG_LOGIC_SEL_OR			(0x02)
#define ADP5589_LOGIC_CFG_LOGIC_SEL_XOR			(0x03)
#define ADP5589_LOGIC_CFG_LOGIC_SEL_FF			(0x04)
#define ADP5589_LOGIC_CFG_LOGIC_SEL_IN_LA		(0x05)
#define ADP5589_LOGIC_CFG_LOGIC_SEL_IN_LB		(0x06)
#define ADP5589_LOGIC_CFG_LOGIC_SEL_IN_LC		(0x07)
/* LOGIC_2_CFG Register bits 0x45. */
#define ADP5589_LOGIC_2_CFG_LOGIC2_SEL(x)		(((x) & 0x07) << 0)
#define ADP5589_LOGIC_2_LA2_INV				(1 << 3)
#define ADP5589_LOGIC_2_LB2_INV				(1 << 4)
#define ADP5589_LOGIC_2_LC2_INV				(1 << 5)
#define ADP5589_LOGIC_2_LY2_INV				(1 << 6)
#define ADP5589_LOGIC_2_LY1_CASCADE			(1 << 7)
/* LOGIC_FF_CFG Register bits 0x46. */
#define ADP5589_LOGIC_FF_CFG_FF1_CLR			(1 << 0)
#define ADP5589_LOGIC_FF_CFG_FF1_SET			(1 << 1)
#define ADP5589_LOGIC_FF_CFG_FF2_CLR			(1 << 2)
#define ADP5589_LOGIC_FF_CFG_FF2_SET			(1 << 3)
/* LOGIC_INT_EVENT_EN Register bits 0x47. */
#define ADP5589_LOGIC_INT_EVENT_EN_LOGIC1_INT_LEVEL	(1 << 0)
#define ADP5589_LOGIC_INT_EVENT_EN_LOGIC1_EVENT_EN	(1 << 1)
#define ADP5589_LOGIC_INT_EVENT_EN_LY1_DBNC_DIS		(1 << 2)
#define ADP5589_LOGIC_INT_EVENT_EN_LOGIC2_INT_LEVEL	(1 << 3)
#define ADP5589_LOGIC_INT_EVENT_EN_LOGIC2_EVENT_EN	(1 << 4)
#define ADP5589_LOGIC_INT_EVENT_EN_LY2_DBNC_DIS		(1 << 5)
/* POLL_TIME_CFG Register bits 0x48. */
#define ADP5589_POLL_TIME_CFG_KEY_POLL_TIME(x)		(((x) & 0x03) << 0)
/* KEY_POLL_TIME bits. */
#define ADP5589_POLL_TIME_CFG_KEY_POLL_TIME_10MS	(0x00)
#define ADP5589_POLL_TIME_CFG_KEY_POLL_TIME_20MS	(0x01)
#define ADP5589_POLL_TIME_CFG_KEY_POLL_TIME_30MS	(0x02)
#define ADP5589_POLL_TIME_CFG_KEY_POLL_TIME_40MS	(0x03)

#define ADP5589_EVENT_KEY_RELEASED                      0
#define ADP5589_EVENT_KEY_PRESSED                       1

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct adp5589_dev {
	/* I2C */
	struct no_os_i2c_desc	*i2c_desc;
};

struct adp5589_init_param {
	/* I2C */
	struct no_os_i2c_init_param	i2c_init;
};


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Writes data into a register. */
void adp5589_set_register_value(struct adp5589_dev *dev,
				uint8_t register_address,
				uint8_t register_value);

/*! Reads the value of a register. */
uint8_t adp5589_get_register_value(struct adp5589_dev *dev,
				   uint8_t register_address);

/*! Initializes the communication peripheral and checks if the ADP5589
	part is present. */
int8_t adp5589_init(struct adp5589_dev **device,
		    struct adp5589_init_param init_param);

/*! Free the resources allocated by adp5589_init(). */
int32_t adp5589_remove(struct adp5589_dev *dev);

/*! Initializes the PWM generator in continuous mode. */
void adp5589_init_pwm(struct adp5589_dev *dev);

/*! Sets the PWM On and Off times. */
void adp5589_set_pwm(struct adp5589_dev *dev,
		     uint16_t pwm_off_time,
		     uint16_t pwm_on_time);

/*! Sets the direction of the pins. */
void adp5589_gpio_direction(struct adp5589_dev *dev,
			    uint8_t reg,
			    uint8_t val);

/*! Reads the state of the pins. */
uint8_t adp5589_get_pin_state(struct adp5589_dev *dev,
			      uint8_t reg);

/*! Sets the state of the pins.*/
void adp5589_set_pin_state(struct adp5589_dev *dev,
			   uint8_t reg,
			   uint8_t state);

/*! Initializes keyboard decoder. */
void adp5589_init_key(struct adp5589_dev *dev,
		      uint8_t pmod_port);

/*! Decodes the key on the Pmod-KYPD. */
uint8_t adp5589_key_decode(uint8_t reg,
			   uint8_t event_type,
			   uint8_t pmod_port);

/*! Locks the ADP5589 and requests Password for unlock. */
void adp5589_key_lock(struct adp5589_dev *dev,
		      uint8_t first_event,
		      uint8_t second_event,
		      uint8_t pmod_port);

#endif	/* __ADP5589_H__ */
