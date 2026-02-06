/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Hockey Puck platform.
 *   @author Mark Sapungan (Mark.Sapungan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/* Board form factor */
#define HOCKEY_PUCK 1
#define FORM_FACTOR HOCKEY_PUCK

/* IRQ Controller ID */
#define INTC_DEVICE_ID  0

/* Debug UART parameters */
#define DEBUG_UART_DEVICE_ID	3
#define DEBUG_UART_IRQ_ID	UART3_IRQn
#define DEBUG_UART_ASYNC_RX	false
#define DEBUG_UART_BAUDRATE	115200

/* IIO UART parameters */
#define IIO_UART_DEVICE_ID	2
#define IIO_UART_IRQ_ID		UART2_IRQn
#define IIO_UART_ASYNC_RX	true
#define IIO_UART_BAUDRATE	57600

/* ADIN1110 parameters */
#if defined(HP_IS)
#define ADIN1110_SPI_DEVICE_ID	3
#define ADIN1110_SPI_CLK_SPEED	25000000
#define ADIN1110_SPI_CHIP_SEL	0

#define ADIN1110_RESET_PORT	0
#define ADIN1110_RESET_PIN	15
#define ADIN1110_INT_PORT	0
#define ADIN1110_INT_PIN	22
#define ADIN1110_LINK_ST_PORT	0
#define ADIN1110_LINK_ST_PIN	23

#else

#define ADIN1110_SPI_DEVICE_ID	4
#define ADIN1110_SPI_CLK_SPEED	25000000
#define ADIN1110_SPI_CHIP_SEL	0

#define ADIN1110_RESET_PORT	1
#define ADIN1110_RESET_PIN	4
#define ADIN1110_INT_PORT	1
#define ADIN1110_INT_PIN	5
#define ADIN1110_LINK_ST_PORT	1
#define ADIN1110_LINK_ST_PIN	6
#endif

#if (FORM_FACTOR == HOCKEY_PUCK)
#define ADIN1110_GPIO_VSSEL	MXC_GPIO_VSSEL_VDDIO
#else
#define ADIN1110_GPIO_VSSEL	MXC_GPIO_VSSEL_VDDIOH
#endif

/* AD71274-4 parameters */
#if (FORM_FACTOR == HOCKEY_PUCK)
#define AD7124_SPI_DEVICE_ID	0
#define AD7124_SPI_CHIP_SEL	1
#else
#define AD7124_SPI_DEVICE_ID	4
#define AD7124_SPI_CHIP_SEL	0
#endif
#define AD7124_SPI_CLK_SPEED	1000000
#define AD7124_SPI_RDY_POL_CNT	25000
#define AD7124_BIPOLAR_MODE	false
#define AD7124_BUFF_REF		true
#define AD7124_BUFF_AIN		true
#define AD7124_CH0_ENABLE	true
#define AD7124_CH0_AIN_PINS	{AD7124_AIN5, AD7124_AIN6}
#define AD7124_CH0_SETUP_SEL	0
#define AD7124_PGA_GAIN		0
#define AD7124_IEXC_IOUT0	3
#define AD7124_IIO_BUFF_SIZE	4096
#define AD7124_GPIO_VSSEL	MXC_GPIO_VSSEL_VDDIOH

/* MAX42500 parameters */
#define MAX42500_I2C_DEVICE_ID	(1)
#define MAX42500_I2C_CLK_SPEED	(400000)
#define MAX42500_I2C_ADDR	MAX42500_ADDR(0)
#define MAX42500_EN0_PORT	(2)
#define MAX42500_EN0_PIN	(7)
#define MAX42500_EN1_PORT	(2)
#define MAX42500_EN1_PIN	(8)
#if (FORM_FACTOR == HOCKEY_PUCK)
#define MAX42500_ADDR_PORT	(1)
#define MAX42500_ADDR_PIN	(5)
#else
#define MAX42500_ADDR_PORT	(2)
#define MAX42500_ADDR_PIN	(9)
#endif
#define MAX42500_ADDR_SEL	(0)
#define MAX42500_PECE		(true)
#if (FORM_FACTOR == HOCKEY_PUCK)
#define MAX42500_VMON_EN	(MAX42500_VMON_IN_MASK(MAX42500_VM2) | \
				 MAX42500_VMON_IN_MASK(MAX42500_VM3) | \
				 MAX42500_VMON_IN_MASK(MAX42500_VM4))
#else
#define MAX42500_VMON_EN	(0x00)
#endif
#define MAX42500_VMON_VMPD	(true)
#define MAX42500_RESET_MAP	(MAX42500_RSTMAP_IN_MASK(MAX42500_VM2) | \
				 MAX42500_RSTMAP_IN_MASK(MAX42500_VM3) | \
				 MAX42500_RSTMAP_IN_MASK(MAX42500_VM4) | \
				 MAX42500_RSTMAP_PARM_MASK)
#define MAX42500_WD_MODE	MAX42500_WD_MODE_SIMPLE
#define MAX42500_WD_CDIV	(0x00)
#define MAX42500_WD_CLOSE	(0x00)
#define MAX42500_WD_OPEN	(0x00)
#define MAX42500_WD_1UD		(0x00)
#define MAX42500_WD_EN		(true)
#define MAX42500_TIMER_ID	(0)
#define MAX42500_TIMER_FREQ	(32000)
#define MAX42500_TIMER_COUNT	(52)
#define MAX42500_IRQ_ID		(0)

#endif /* __PARAMETERS_H__ */
