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
#include <stdio.h>
#include "platform_drivers.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define CLKGEN_REG_RESETN		0x40
#define CLKGEN_MMCM_RESETN		(1 << 1)
#define CLKGEN_RESETN			(1 << 0)

#define CLKGEN_REG_STATUS		0x5c
#define CLKGEN_STATUS			(1 << 0)

#define XCVR_REG_RESETN			0x0010
#define XCVR_RESETN				(1 << 0)

#define XCVR_REG_STATUS			0x0014
#define XCVR_STATUS				(1 << 0)

#define XCVR_REG_CONTROL		0x0020
#define XCVR_LPM_DFE_N			(1 << 12)
#define XCVR_RATE(x)			(((x) & 0x7) << 8)
#define XCVR_SYSCLK_SEL(x)		(((x) & 0x3) << 4)
#define XCVR_OUTCLK_SEL(x)		(((x) & 0x7) << 0)

#define JESD_REG_TRX_VERSION				0x00
#define JESD_REG_TRX_RESET					0x04
#define JESD_REG_TRX_ILA_SUPPORT			0x08
#define JESD_REG_TRX_SCRAMBLING				0x0c
#define JESD_REG_TRX_SYSREF_HANDLING		0x10
#define JESD_REG_TX_ILA_MULTIFRAMES			0x14
#define JESD_REG_TRX_TEST_MODES				0x18
#define JESD_REG_RX_LINK_ERROR_STATUS		0x1c
#define JESD_REG_TRX_OCTETS_PER_FRAME		0x20
#define JESD_REG_TRX_FRAMES_PER_MULTIFRAME	0x24
#define JESD_REG_TRX_LANES_IN_USE			0x28
#define JESD_REG_TRX_SUBCLASS_MODE			0x2c
#define JESD_REG_TRX_SYNC_STATUS			0x38

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

XGpioPs_Config	*gpio_config;
XGpioPs			gpio_instance;

/***************************************************************************//**
 * @brief platform_init
*******************************************************************************/
int32_t platform_init(void)
{
	gpio_init(GPIO_DEVICE_ID);

	gpio_direction_output(AD9371_RESET_B, 1);
	gpio_direction_output(AD9528_RESET_B, 1);
	gpio_direction_output(AD9528_SYSREF_REQ, 0);

	return 0;
}

/***************************************************************************//**
 * @brief spi_write_and_read
*******************************************************************************/
int32_t spi_write_and_read(spi_device *dev,
					   	   uint8_t *data,
						   uint8_t bytes_number)
{
	uint32_t initss;

	XSpiPs  spi_instance;
	XSpiPs_Config  *spi_config;

	spi_config = XSpiPs_LookupConfig(dev->device_id);

	if (spi_config == NULL)
		return -1;

	if (XSpiPs_CfgInitialize(&spi_instance, spi_config, spi_config->BaseAddress) != 0)
		return -1;

	initss = XSpiPs_ReadReg(dev->base_address, XSPIPS_CR_OFFSET);
	initss = initss & (uint32_t)(~XSPIPS_CR_SSCTRL_MASK);
	initss = initss | (0x7 << XSPIPS_CR_SSCTRL_SHIFT);
	XSpiPs_WriteReg(dev->base_address, XSPIPS_CR_OFFSET, initss);
	XSpiPs_SetOptions(&spi_instance, XSPIPS_MASTER_OPTION |
			XSPIPS_DECODE_SSELECT_OPTION | XSPIPS_FORCE_SSELECT_OPTION |
			((dev->cpol == 1) ? XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			((dev->cpha == 1) ? XSPIPS_CLK_PHASE_1_OPTION : 0));
	XSpiPs_SetSlaveSelect(&spi_instance, (uint8_t) 0x7);
	XSpiPs_SetClkPrescaler(&spi_instance, XSPIPS_CLK_PRESCALE_64);
	XSpiPs_SetSlaveSelect(&spi_instance,  (uint8_t) dev->chip_select);
	XSpiPs_PolledTransfer(&spi_instance, data, data, bytes_number);
	XSpiPs_SetSlaveSelect(&spi_instance,  (uint8_t) 0x7);

	return 0;
}

/***************************************************************************//**
 * @brief gpio_init
 *******************************************************************************/
int32_t gpio_init(uint16_t device_id)
{
	gpio_config = XGpioPs_LookupConfig(device_id);
	XGpioPs_CfgInitialize(&gpio_instance, gpio_config, gpio_config->BaseAddr);

	return 0;
}

/***************************************************************************//**
 * @brief gpio_direction_output
 *******************************************************************************/
int32_t gpio_direction_output(uint8_t gpio, uint8_t value)
{
	XGpioPs_SetDirectionPin(&gpio_instance, gpio, 1);
	XGpioPs_SetOutputEnablePin(&gpio_instance, gpio, 1);
	XGpioPs_WritePin(&gpio_instance, gpio, value);

	return 0;
}

/***************************************************************************//**
 * @brief gpio_set_value
 *******************************************************************************/
int32_t gpio_set_value(uint8_t gpio, uint8_t value)
{
	XGpioPs_WritePin(&gpio_instance, gpio, value);

	return 0;
}

/***************************************************************************//**
 * @brief clkgen_write
 *******************************************************************************/
int32_t clkgen_write(clkgen_device dev,
					 uint32_t reg_addr,
					 uint32_t reg_val)
{
	Xil_Out32((dev.base_addr + reg_addr), reg_val);

	return 0;
}

/***************************************************************************//**
 * @brief clkgen_read
 *******************************************************************************/
int32_t clkgen_read(clkgen_device dev,
					uint32_t reg_addr,
					uint32_t *reg_val)
{
	*reg_val = Xil_In32(dev.base_addr + reg_addr);

	return 0;
}

/***************************************************************************//**
 * @brief clkgen_setup
 *******************************************************************************/
int32_t clkgen_setup(void)
{
	clkgen_device	rx_clkgen;
	clkgen_device	tx_clkgen;
	clkgen_device	rx_os_clkgen;
	uint32_t		reg_val;
	int32_t			status = 0;

	rx_clkgen.base_addr = XPAR_AXI_AD9371_RX_CLKGEN_BASEADDR;
	tx_clkgen.base_addr = XPAR_AXI_AD9371_TX_CLKGEN_BASEADDR;
	rx_os_clkgen.base_addr = XPAR_AXI_AD9371_RX_OS_CLKGEN_BASEADDR;

	clkgen_write(rx_clkgen, CLKGEN_REG_RESETN,
			CLKGEN_MMCM_RESETN | CLKGEN_RESETN);
	clkgen_write(tx_clkgen, CLKGEN_REG_RESETN,
			CLKGEN_MMCM_RESETN | CLKGEN_RESETN);
	clkgen_write(rx_os_clkgen, CLKGEN_REG_RESETN,
			CLKGEN_MMCM_RESETN | CLKGEN_RESETN);
	mdelay(1);

	clkgen_read(rx_clkgen, CLKGEN_REG_STATUS, &reg_val);
	if ((reg_val & CLKGEN_STATUS) == 0x0) {
		printf("RX MMCM-PLL NOT locked");
		status--;
	}
	clkgen_read(tx_clkgen, CLKGEN_REG_STATUS, &reg_val);
	if ((reg_val & CLKGEN_STATUS) == 0x0) {
		printf("TX MMCM-PLL NOT locked");
		status--;
	}
	clkgen_read(rx_os_clkgen, CLKGEN_REG_STATUS, &reg_val);
	if ((reg_val & CLKGEN_STATUS) == 0x0) {
		printf("RX OS MMCM-PLL NOT locked");
		status--;
	}

	return status;
}

/***************************************************************************//**
 * @brief xcvr_write
 *******************************************************************************/
int32_t xcvr_write(xcvr_device dev,
				   uint32_t reg_addr,
				   uint32_t reg_val)
{
	Xil_Out32((dev.base_addr + reg_addr), reg_val);

	return 0;
}

/***************************************************************************//**
 * @brief xcvr_read
 *******************************************************************************/
int32_t xcvr_read(xcvr_device dev,
				  uint32_t reg_addr,
				  uint32_t *reg_val)
{
	*reg_val = Xil_In32(dev.base_addr + reg_addr);

	return 0;
}

/***************************************************************************//**
 * @brief xcvr_setup
 *******************************************************************************/
int32_t xcvr_setup(void)
{
	xcvr_device	rx_xcvr;
	xcvr_device	tx_xcvr;
	xcvr_device	rx_os_xcvr;
	uint32_t	status;
	int32_t		timeout;
	int32_t		ret = 0;

	rx_xcvr.base_addr = XPAR_AXI_AD9371_RX_XCVR_BASEADDR;
	tx_xcvr.base_addr = XPAR_AXI_AD9371_TX_XCVR_BASEADDR;
	rx_os_xcvr.base_addr = XPAR_AXI_AD9371_RX_OS_XCVR_BASEADDR;

	xcvr_write(rx_xcvr, XCVR_REG_RESETN, 0);
	xcvr_write(rx_xcvr, XCVR_REG_CONTROL, XCVR_LPM_DFE_N | XCVR_OUTCLK_SEL(3));
	xcvr_write(rx_xcvr, XCVR_REG_RESETN, XCVR_RESETN);

	xcvr_write(tx_xcvr, XCVR_REG_RESETN, 0);
	xcvr_write(tx_xcvr, XCVR_REG_CONTROL, XCVR_SYSCLK_SEL(3) | XCVR_OUTCLK_SEL(3));
	xcvr_write(tx_xcvr, XCVR_REG_RESETN, XCVR_RESETN);

	xcvr_write(rx_os_xcvr, XCVR_REG_RESETN, 0);
	xcvr_write(rx_os_xcvr, XCVR_REG_CONTROL, XCVR_LPM_DFE_N | XCVR_OUTCLK_SEL(3));
	xcvr_write(rx_os_xcvr, XCVR_REG_RESETN, XCVR_RESETN);

	mdelay(1);

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		xcvr_read(rx_xcvr, XCVR_REG_STATUS, &status);
		if (status == XCVR_STATUS)
			break;
	}
	if (status == 0) {
		printf("RX_XCVR initialization failed\n");
		ret--;
	} else
		printf("RX_XCVR initialization OK\n");

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		xcvr_read(tx_xcvr, XCVR_REG_STATUS, &status);
		if (status == XCVR_STATUS)
			break;
	}
	if (status == 0) {
		printf("TX_XCVR initialization failed\n");
		ret--;
	} else
		printf("TX_XCVR initialization OK\n");

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		xcvr_read(rx_os_xcvr, XCVR_REG_STATUS, &status);
		if (status == XCVR_STATUS)
			break;
	}
	if (status == 0) {
		printf("RX_OS_XCVR initialization failed\n");
		ret--;
	} else
		printf("RX_OS_XCVR initialization OK\n");
ret = 0;
	return ret;
}

/***************************************************************************//**
 * @brief jesd_write
 *******************************************************************************/
int32_t jesd_write(jesd_device dev,
				   uint32_t reg_addr,
				   uint32_t reg_val)
{
	Xil_Out32((dev.base_addr + reg_addr), reg_val);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_read
 *******************************************************************************/
int32_t jesd_read(jesd_device dev,
				  uint32_t reg_addr,
				  uint32_t *reg_val)
{
	*reg_val = Xil_In32(dev.base_addr + reg_addr);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_setup
 *******************************************************************************/
int32_t jesd_setup(void)
{
	jesd_device	rx_jesd;
	jesd_device	tx_jesd;
	jesd_device	rx_os_jesd;

	rx_jesd.base_addr = RX_JESD_BASEADDR;
	tx_jesd.base_addr = TX_JESD_BASEADDR;
	rx_os_jesd.base_addr = RX_OS_JESD_BASEADDR;

	jesd_write(rx_jesd, JESD_REG_TRX_RESET, 0x01);
	jesd_write(rx_jesd, JESD_REG_TRX_ILA_SUPPORT, 0x01);
	jesd_write(rx_jesd, JESD_REG_TRX_SCRAMBLING, 0x01);
	jesd_write(rx_jesd, JESD_REG_TRX_SYSREF_HANDLING, 0x00);
	jesd_write(rx_jesd, JESD_REG_TX_ILA_MULTIFRAMES, 0x1f);
	jesd_write(rx_jesd, JESD_REG_TRX_OCTETS_PER_FRAME, 0x03);
	jesd_write(rx_jesd, JESD_REG_TRX_FRAMES_PER_MULTIFRAME, 0x1f);
	jesd_write(rx_jesd, JESD_REG_TRX_SUBCLASS_MODE, 0x01);

	jesd_write(tx_jesd, JESD_REG_TRX_RESET, 0x01);
	jesd_write(tx_jesd, JESD_REG_TRX_ILA_SUPPORT, 0x01);
	jesd_write(tx_jesd, JESD_REG_TRX_SCRAMBLING, 0x01);
	jesd_write(tx_jesd, JESD_REG_TRX_SYSREF_HANDLING, 0x00);
	jesd_write(tx_jesd, JESD_REG_TX_ILA_MULTIFRAMES, 0x1f);
	jesd_write(tx_jesd, JESD_REG_TRX_OCTETS_PER_FRAME, 0x01);
	jesd_write(tx_jesd, JESD_REG_TRX_FRAMES_PER_MULTIFRAME, 0x1f);
	jesd_write(tx_jesd, JESD_REG_TRX_SUBCLASS_MODE, 0x01);

	jesd_write(rx_os_jesd, JESD_REG_TRX_RESET, 0x01);
	jesd_write(rx_os_jesd, JESD_REG_TRX_ILA_SUPPORT, 0x01);
	jesd_write(rx_os_jesd, JESD_REG_TRX_SCRAMBLING, 0x01);
	jesd_write(rx_os_jesd, JESD_REG_TRX_SYSREF_HANDLING, 0x00);
	jesd_write(rx_os_jesd, JESD_REG_TX_ILA_MULTIFRAMES, 0x1f);
	jesd_write(rx_os_jesd, JESD_REG_TRX_OCTETS_PER_FRAME, 0x01);
	jesd_write(rx_os_jesd, JESD_REG_TRX_FRAMES_PER_MULTIFRAME, 0x1f);
	jesd_write(rx_os_jesd, JESD_REG_TRX_SUBCLASS_MODE, 0x01);

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
