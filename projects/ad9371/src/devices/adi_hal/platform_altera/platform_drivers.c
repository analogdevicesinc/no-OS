/***************************************************************************//**
 * @file platform_drivers.c
 * @brief Implementation of Platform Drivers.
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "platform_drivers.h"
#include "parameters.h"

/* Altera Avalon SPI Registers Definition */

#define ALT_AVL_SPI_RXDATA_REG				 0x0000
#define ALT_AVL_SPI_TXDATA_REG				 0x0004
#define ALT_AVL_SPI_STATUS_REG				 0x0008
#define ALT_AVL_SPI_CONTROL_REG				 0x000C
#define ALT_AVL_SPI_SLAVE_SEL_REG   		 0x0014
#define ALT_AVL_SPI_CONTROL_SSO_MSK			ALTERA_AVALON_SPI_CONTROL_SSO_MSK
#define ALT_AVL_SPI_STATUS_TMT_MSK			ALTERA_AVALON_SPI_STATUS_TMT_MSK
#define ALT_AVL_SPI_STATUS_TRDY_MSK			ALTERA_AVALON_SPI_STATUS_TRDY_MSK
#define ALT_AVL_SPI_STATUS_RRDY_MSK			ALTERA_AVALON_SPI_STATUS_RRDY_MSK

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
spi_device spi_dev = {
	SPI_BASEADDR, 	// base_address
	SPI_DEVICE_ID, 	// device_id
	0xff, 			// chip_select
	0, 				// cpha
	0, 				// cpol
	0				// id_no
};

/***************************************************************************//**
 * @brief platform_init
*******************************************************************************/
int32_t platform_init(void)
{
	gpio_set_value(AD9371_RESET_B, 1);
	gpio_set_value(AD9528_RESET_B, 1);
	gpio_set_value(AD9528_SYSREF_REQ, 0);

	return 0;
}

/***************************************************************************//**
 * @brief spi_init
*******************************************************************************/
int32_t spi_init(uint32_t device_id,
				 uint8_t  clk_pha,
				 uint8_t  clk_pol)
{
	return 0;
}

/***************************************************************************//**
 * @brief spi_read
*******************************************************************************/
int32_t spi_write_and_read(spi_device *dev,
		                   uint8_t *data,
				           uint8_t bytes_number)
{
	uint32_t i;

	IOWR_32DIRECT(SPI_BASEADDR, ALT_AVL_SPI_CONTROL_REG, ALTERA_AVALON_SPI_CONTROL_SSO_MSK);
	IOWR_32DIRECT(SPI_BASEADDR, ALT_AVL_SPI_SLAVE_SEL_REG, ~(dev->chip_select));
	for (i = 0; i < bytes_number; i++) {
		while ((IORD_32DIRECT(SPI_BASEADDR, ALT_AVL_SPI_STATUS_REG) & ALTERA_AVALON_SPI_STATUS_TRDY_MSK) == 0x00) {}
		IOWR_32DIRECT(SPI_BASEADDR, ALT_AVL_SPI_TXDATA_REG, *(data + i));
		while ((IORD_32DIRECT(SPI_BASEADDR, ALT_AVL_SPI_STATUS_REG) & ALTERA_AVALON_SPI_STATUS_RRDY_MSK) == 0x00) {}
		*(data + i) = IORD_32DIRECT(SPI_BASEADDR, 0x00);
	}
	IOWR_32DIRECT(SPI_BASEADDR, ALT_AVL_SPI_SLAVE_SEL_REG, 0x000);
	IOWR_32DIRECT(SPI_BASEADDR, ALT_AVL_SPI_CONTROL_REG, 0x000);

	return 0;
}

/***************************************************************************//**
 * @brief gpio_init
*******************************************************************************/
void gpio_init(uint32_t device_id)
{

}


/***************************************************************************//**
 * @brief gpio_data
*******************************************************************************/
void gpio_data(uint8_t pin, uint8_t data)
{

}

/***************************************************************************//**
 * @brief gpio_set_value
*******************************************************************************/
int32_t gpio_set_value(unsigned gpio, int value)
{
	uint32_t ppos;
	uint32_t pdata;
	uint32_t pmask;

	if (gpio < 32) {
		return(-1);
	}

	ppos = gpio - 32;
	pmask = 0x1 << ppos;

	pdata = IORD_32DIRECT(GPIO_BASEADDR, 0x0);
	IOWR_32DIRECT(GPIO_BASEADDR, 0x0, ((pdata & ~pmask) | (value << ppos)));

	return 0;
}

/***************************************************************************//**
 * @brief udelay
*******************************************************************************/
void udelay(unsigned long usecs)
{
	usleep(usecs);
}

/***************************************************************************//**
 * @brief mdelay
*******************************************************************************/
void mdelay(unsigned long msecs)
{
	usleep(msecs * 1000);
}

/***************************************************************************//**
 * @brief msleep_interruptible
*******************************************************************************/
unsigned long msleep_interruptible(unsigned int msecs)
{
	mdelay(msecs);

	return 0;
}

/***************************************************************************//**
 * @brief ad_pow2 Create a mask for a given number of bit
 *******************************************************************************/
uint32_t ad_pow2(uint32_t number) {

	uint32_t index;
	uint32_t mask = 1;

	for (index=1; index < number; index++) {
		mask = (mask << 1) ^ 1;
	}

	return mask;
}
