/***************************************************************************//**
 *   @file   Platform.c
 *   @brief  Implementation of Platform Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "util.h"
#include "adc_core.h"
#include "dac_core.h"
#include "parameters.h"
#include "platform.h"
#include "socal/socal.h"
#include "alt_spi.h"
#include "alt_address_space.h"
#include "alt_bridge_manager.h"

/* Altera Avalon SPI Registers Definition */

#define ALT_AVL_SPI_RXDATA_REG				0x0000
#define ALT_AVL_SPI_TXDATA_REG				0x0004
#define ALT_AVL_SPI_STATUS_REG				0x0008
#define ALT_AVL_SPI_CONTROL_REG				0x000C
#define ALT_AVL_SPI_CONTROL_SSO_MSK			(1 << 10)
#define ALT_AVL_SPI_SLAVE_SEL_REG   		0x0014
#define ALT_AVL_SPI_STATUS_TMT_MSK			(1 << 5)
#define ALT_AVL_SPI_STATUS_TRDY_MSK			(1 << 6)
#define ALT_AVL_SPI_STATUS_RRDY_MSK			(1 << 7)

/* Altera Avalon GPIO Registers Definition */

#define ALT_AVL_PIO_DATA_REG				0x0000
#define ALT_AVL_PIO_DIRECTION_REG			0x0004

/***************************************************************************//**
 * @brief usleep
*******************************************************************************/
static inline void usleep(unsigned long usleep)
{
	unsigned long delay = 0;

	for(delay = 0; delay < usleep * 10; delay++);
}

/***************************************************************************//**
 * @brief altera_bridge_init
*******************************************************************************/
int32_t altera_bridge_init(void)
{
	int32_t status = 0;

	status = alt_bridge_init(ALT_BRIDGE_LWH2F, NULL, NULL);

	if (status == 0)
	{
		status = alt_addr_space_remap(ALT_ADDR_SPACE_MPU_ZERO_AT_BOOTROM,
									  ALT_ADDR_SPACE_NONMPU_ZERO_AT_OCRAM,
									  ALT_ADDR_SPACE_H2F_ACCESSIBLE,
									  ALT_ADDR_SPACE_LWH2F_ACCESSIBLE);
	}

	return status;
}

/***************************************************************************//**
 * @brief altera_bridge_uninit
*******************************************************************************/
int32_t altera_bridge_uninit(void)
{
	int32_t status = 0;

    status = alt_bridge_uninit(ALT_BRIDGE_LWH2F, NULL, NULL);

    return status;
}

/***************************************************************************//**
 * @brief alt_avl_spi_read
*******************************************************************************/
uint32_t alt_avl_spi_read(uint32_t reg_addr)
{
	uint32_t reg_data;

	reg_data = alt_read_word(SPI_BASEADDR + reg_addr);

	return reg_data;
}

/***************************************************************************//**
 * @brief alt_avl_spi_write
*******************************************************************************/
void alt_avl_spi_write(uint32_t reg_addr, uint32_t reg_data)
{
	alt_write_word(SPI_BASEADDR + reg_addr, reg_data);
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
int32_t spi_read(uint8_t *data,
				 uint8_t bytes_number)
{
	uint32_t cnt = 0;

	/* Enable Slave Select mask. */
	alt_avl_spi_write(ALT_AVL_SPI_SLAVE_SEL_REG, 1);
	/* Set the SSO bit (force chip select). */
	alt_avl_spi_write(ALT_AVL_SPI_CONTROL_REG, ALT_AVL_SPI_CONTROL_SSO_MSK);
	/* Discard any stale data, in case previous communication was interrupted. */
	alt_avl_spi_read(ALT_AVL_SPI_RXDATA_REG);

	while(cnt < bytes_number)
	{
		/* Wait until txdata register is empty. */
		while((alt_avl_spi_read(ALT_AVL_SPI_STATUS_REG) &
					ALT_AVL_SPI_STATUS_TRDY_MSK) == 0);
		/* Write data to txdata register. */
		alt_avl_spi_write(ALT_AVL_SPI_TXDATA_REG, data[cnt]);
		/* Wait until rxdata register is full. */
		while ((alt_avl_spi_read(ALT_AVL_SPI_STATUS_REG)
					& ALT_AVL_SPI_STATUS_RRDY_MSK) == 0);
		/* Read data from rxdata register. */
		data[cnt] = alt_avl_spi_read(ALT_AVL_SPI_RXDATA_REG);
		cnt++;
	}

	/* Wait until the interface has finished transmitting. */
	while((alt_avl_spi_read(ALT_AVL_SPI_STATUS_REG) &
				ALT_AVL_SPI_STATUS_TMT_MSK) == 0);
	/* Clear the SSO bit (release chip select). */
	alt_avl_spi_write(ALT_AVL_SPI_CONTROL_REG, 0);

	return 0;
}

/***************************************************************************//**
 * @brief spi_write_then_read
*******************************************************************************/
int spi_write_then_read(struct spi_device *spi,
		const unsigned char *txbuf, unsigned n_tx,
		unsigned char *rxbuf, unsigned n_rx)
{
	uint8_t buffer[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						  0x00, 0x00, 0x00, 0x00};
	uint8_t byte;

	for(byte = 0; byte < n_tx; byte++)
	{
		buffer[byte] = (unsigned char)txbuf[byte];
	}
	spi_read(buffer, n_tx + n_rx);
	for(byte = n_tx; byte < n_tx + n_rx; byte++)
	{
		rxbuf[byte - n_tx] = buffer[byte];
	}

	return SUCCESS;
}

/***************************************************************************//**
 * @brief alt_avl_gpio_read
*******************************************************************************/
uint32_t alt_avl_gpio_read(uint32_t reg_addr)
{
	uint32_t reg_data;

	reg_data = alt_read_word(GPIO_BASEADDR + reg_addr);

	return reg_data;
}

/***************************************************************************//**
 * @brief alt_avl_gpio_write
*******************************************************************************/
void alt_avl_gpio_write(uint32_t reg_addr, uint32_t reg_data)
{
	alt_write_word(GPIO_BASEADDR + reg_addr, reg_data);
}

/***************************************************************************//**
 * @brief gpio_init
*******************************************************************************/
void gpio_init(uint32_t device_id)
{

}

/***************************************************************************//**
 * @brief gpio_direction
*******************************************************************************/
void gpio_direction(uint8_t pin, uint8_t direction)
{
	uint32_t reg_val;

	reg_val = alt_avl_gpio_read(ALT_AVL_PIO_DIRECTION_REG);
	if (direction)
		reg_val |= (1 << pin);
	else
		reg_val &= ~(1 << pin);
	alt_avl_gpio_write(ALT_AVL_PIO_DIRECTION_REG, reg_val);
}

/***************************************************************************//**
 * @brief gpio_is_valid
*******************************************************************************/
bool gpio_is_valid(int number)
{
	if(number >= 0)
		return 1;
	else
		return 0;
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
void gpio_set_value(unsigned gpio, int value)
{
	uint32_t reg_val;

	reg_val = alt_avl_gpio_read(ALT_AVL_PIO_DATA_REG);
	if (value)
		reg_val |= (1 << gpio);
	else
		reg_val &= ~(1 << gpio);
	alt_avl_gpio_write(ALT_AVL_PIO_DATA_REG, reg_val);
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
 * @brief axiadc_init
*******************************************************************************/
void axiadc_init(struct ad9361_rf_phy *phy)
{
	adc_init();
	dac_init(phy, DATA_SEL_DDS);
}

/***************************************************************************//**
 * @brief axiadc_post_setup
*******************************************************************************/
int axiadc_post_setup(struct ad9361_rf_phy *phy)
{
	return ad9361_post_setup(phy);
}

/***************************************************************************//**
 * @brief axiadc_read
*******************************************************************************/
unsigned int axiadc_read(struct axiadc_state *st, unsigned long reg)
{
	uint32_t val;

	adc_read(reg, &val);

	return val;
}

/***************************************************************************//**
 * @brief axiadc_write
*******************************************************************************/
void axiadc_write(struct axiadc_state *st, unsigned reg, unsigned val)
{
	adc_write(reg, val);
}

/***************************************************************************//**
 * @brief axiadc_set_pnsel
*******************************************************************************/
int axiadc_set_pnsel(struct axiadc_state *st, int channel, enum adc_pn_sel sel)
{
	unsigned reg;

	uint32_t version = axiadc_read(st, 0x4000);

	if (PCORE_VERSION_MAJOR(version) > 7) {
		reg = axiadc_read(st, ADI_REG_CHAN_CNTRL_3(channel));
		reg &= ~ADI_ADC_PN_SEL(~0);
		reg |= ADI_ADC_PN_SEL(sel);
		axiadc_write(st, ADI_REG_CHAN_CNTRL_3(channel), reg);
	} else {
		reg = axiadc_read(st, ADI_REG_CHAN_CNTRL(channel));

		if (sel == ADC_PN_CUSTOM) {
			reg |= ADI_PN_SEL;
		} else if (sel == ADC_PN9) {
			reg &= ~ADI_PN23_TYPE;
			reg &= ~ADI_PN_SEL;
		} else {
			reg |= ADI_PN23_TYPE;
			reg &= ~ADI_PN_SEL;
		}

		axiadc_write(st, ADI_REG_CHAN_CNTRL(channel), reg);
	}

	return 0;
}

/***************************************************************************//**
 * @brief axiadc_idelay_set
*******************************************************************************/
void axiadc_idelay_set(struct axiadc_state *st,
				unsigned lane, unsigned val)
{
	if (PCORE_VERSION_MAJOR(st->pcore_version) > 8) {
		axiadc_write(st, ADI_REG_DELAY(lane), val);
	} else {
		axiadc_write(st, ADI_REG_DELAY_CNTRL, 0);
		axiadc_write(st, ADI_REG_DELAY_CNTRL,
				ADI_DELAY_ADDRESS(lane)
				| ADI_DELAY_WDATA(val)
				| ADI_DELAY_SEL);
	}
}
