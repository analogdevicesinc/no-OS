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
#include "platform.h"
#include "socal/socal.h"
#include "alt_spi.h"
#include "alt_address_space.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
ALT_SPI_DEV_t *spim0_dev;
ALT_SPI_CONFIG_t *spim0_cfg;

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
int32_t altera_bridge_init(ALT_BRIDGE_t bridge)
{
	int32_t status = 0;

	status = alt_bridge_init(bridge, NULL, NULL);

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
int32_t altera_bridge_uninit(ALT_BRIDGE_t bridge)
{
	int32_t status = 0;

    status = alt_bridge_uninit(bridge, NULL, NULL);

    return status;
}

/***************************************************************************//**
 * @brief altera_spi_init
*******************************************************************************/
int32_t altera_spi_init(uint32_t spi_base_address)
{
	int32_t status = 0;

	spim0_dev = (ALT_SPI_DEV_t *)malloc(sizeof(*spim0_dev));
	spim0_cfg = (ALT_SPI_CONFIG_t *)malloc(sizeof(*spim0_cfg));

    status = alt_spi_init(ALT_SPI_SPIM0, spim0_dev);
    if(status != ALT_E_SUCCESS)
    	printf("Error\n\r");

    status = alt_spi_config_get(spim0_dev, spim0_cfg);
    if(status != ALT_E_SUCCESS)
    	printf("Error\n\r");
    spim0_cfg->clk_phase = ALT_SPI_SCPH_TOGGLE_START;
    spim0_cfg->loopback_mode = false;

    status = alt_spi_config_set(spim0_dev, spim0_cfg);
    if(status != ALT_E_SUCCESS)
    	printf("Error\n\r");

    status = alt_spi_speed_set(spim0_dev, 5000000);
    if(status != ALT_E_SUCCESS)
    	printf("Error\n\r");

    status = alt_spi_enable(spim0_dev);
    if(status != ALT_E_SUCCESS)
    	printf("Error\n\r");

    return status;
}

/***************************************************************************//**
 * @brief altera_spi_read
*******************************************************************************/
int32_t altera_spi_read(uint8_t *data,
						uint8_t bytes_number)
{
    uint16_t tx_buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t rx_buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t index;

    for(index = 0; index < bytes_number; index++)
    	tx_buf[index] = data[index];
	alt_spi_master_tx_rx_transfer(spim0_dev, 1, bytes_number, tx_buf, rx_buf);
    for(index = 0; index < bytes_number; index++)
    	data[index] = rx_buf[index];

	return 0;
}

/***************************************************************************//**
 * @brief spi_init
*******************************************************************************/
int32_t spi_init(uint32_t device_id,
				 uint8_t  clk_pha,
				 uint8_t  clk_pol)
{
	int32_t status;

	status = altera_spi_init(device_id);

	return status;
}

/***************************************************************************//**
 * @brief spi_read
*******************************************************************************/
int32_t spi_read(uint8_t *data,
				 uint8_t bytes_number)
{
	altera_spi_read(data, bytes_number);

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

}

/***************************************************************************//**
 * @brief gpio_is_valid
*******************************************************************************/
bool gpio_is_valid(int number)
{
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
	gpio_data(gpio, value);
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
