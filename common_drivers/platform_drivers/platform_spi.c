// ***************************************************************************
// ***************************************************************************
// Copyright 2014 - 2017 (c) Analog Devices, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// - Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in
// the documentation and/or other materials provided with the
// distribution.
// - Neither the name of Analog Devices, Inc. nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
// - The use of this software may or may not infringe the patent rights
// of one or more patent holders. This license does not release you
// from the requirement that you obtain separate licenses from these
// patent holders to use this software.
// - Use of the software either in source or binary form, must be run
// on or directly connected to an Analog Devices Inc. component.
//
// THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ***************************************************************************
// ***************************************************************************

#include "platform_spi.h"

// ***************************************************************************
// ***************************************************************************
/* altera nios-ii */

#ifdef NIOS_II

int32_t ad_spi_init(struct spi_device *dev) {

	dev->base_address = 0;
	dev->device_id = 0;
	dev->chip_select = 0xff;
	dev->cpha = 0;
	dev->cpol = 0;
}

int32_t ad_spi_xfer(struct spi_device *dev, uint8_t *data, uint8_t no_of_bytes) {

	uint32_t i;

	ad_reg_write((SYS_SPI_BASE + 0x0c), 0x400);
	ad_reg_write((SYS_SPI_BASE + 0x14), ~(dev->chip_select));
	for (i = 0; i < no_of_bytes; i++) {
		while ((ad_reg_read(SYS_SPI_BASE + 0x08) & 0x40) == 0x00) {}
		ad_reg_write((SYS_SPI_BASE + 0x04), *(data + i));
		while ((ad_reg_read(SYS_SPI_BASE + 0x08) & 0x80) == 0x00) {}
		*(data + i) = ad_reg_read(SYS_SPI_BASE + 0x00);
	}
	ad_reg_write((SYS_SPI_BASE + 0x14), 0x000);
	ad_reg_write((SYS_SPI_BASE + 0x0c), 0x000);

	return(0);
}

#endif

// ***************************************************************************
// ***************************************************************************
/* xilinx zynq ultrascale */

#ifdef ZYNQ_PSU

#include <xspips.h>

XSpiPs  m_spi;
XSpiPs_Config  *m_spi_config;

int32_t ad_spi_init(struct spi_device *dev) {

	dev->base_address = XPAR_PSU_SPI_0_BASEADDR;
	dev->device_id = XPAR_PSU_SPI_0_DEVICE_ID;
	dev->chip_select = 0xff;
	dev->cpha = 0;
	dev->cpol = 0;

	m_spi_config = XSpiPs_LookupConfig(dev->device_id);
	if (m_spi_config == NULL) {
		return(-1);
	}
	if (XSpiPs_CfgInitialize(&m_spi, m_spi_config, m_spi_config->BaseAddress) != 0) {
		return(-1);
	}

	return(0);
}

int32_t ad_spi_xfer(struct spi_device *dev, uint8_t *data, uint8_t no_of_bytes) {

	uint32_t initss;

	initss = XSpiPs_ReadReg(dev->base_address, XSPIPS_CR_OFFSET);
	initss = initss & (uint32_t)(~XSPIPS_CR_SSCTRL_MASK);
	initss = initss | (0x7 << XSPIPS_CR_SSCTRL_SHIFT);
	XSpiPs_WriteReg(dev->base_address, XSPIPS_CR_OFFSET, initss);
	XSpiPs_SetOptions(&m_spi, XSPIPS_MASTER_OPTION |
			XSPIPS_DECODE_SSELECT_OPTION | XSPIPS_FORCE_SSELECT_OPTION |
			((dev->cpol == 1) ? XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			((dev->cpha == 1) ? XSPIPS_CLK_PHASE_1_OPTION : 0));
	XSpiPs_SetSlaveSelect(&m_spi,  (uint8_t) 0xf);
	XSpiPs_SetClkPrescaler(&m_spi, XSPIPS_CLK_PRESCALE_16);
	XSpiPs_SetSlaveSelect(&m_spi,  (uint8_t) dev->chip_select);
	XSpiPs_PolledTransfer(&m_spi, data, data, no_of_bytes);
	XSpiPs_SetSlaveSelect(&m_spi,  (uint8_t) 0xf);

	return(0);
}

#endif

// ***************************************************************************
// ***************************************************************************
/* xilinx zynq */

#ifdef ZYNQ_PS7

#include <xspips.h>

XSpiPs  m_spi;
XSpiPs_Config  *m_spi_config;

int32_t ad_spi_init(struct spi_device *dev) {

	dev->base_address = XPAR_PS7_SPI_0_BASEADDR;
	dev->device_id = XPAR_PS7_SPI_0_DEVICE_ID;
	dev->chip_select = 0xff;
	dev->cpha = 0;
	dev->cpol = 0;

	m_spi_config = XSpiPs_LookupConfig(dev->device_id);
	if (m_spi_config == NULL) {
		return(-1);
	}
	if (XSpiPs_CfgInitialize(&m_spi, m_spi_config, m_spi_config->BaseAddress) != 0) {
		return(-1);
	}

	return(0);
}

int32_t ad_spi_xfer(struct spi_device *dev, uint8_t *data, uint8_t no_of_bytes) {

	uint32_t initss;

	initss = XSpiPs_ReadReg(dev->base_address, XSPIPS_CR_OFFSET);
	initss = initss & (uint32_t)(~XSPIPS_CR_SSCTRL_MASK);
	initss = initss | (0x7 << XSPIPS_CR_SSCTRL_SHIFT);
	XSpiPs_WriteReg(dev->base_address, XSPIPS_CR_OFFSET, initss);
	XSpiPs_SetOptions(&m_spi, XSPIPS_MASTER_OPTION |
			XSPIPS_DECODE_SSELECT_OPTION | XSPIPS_FORCE_SSELECT_OPTION |
			((dev->cpol == 1) ? XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			((dev->cpha == 1) ? XSPIPS_CLK_PHASE_1_OPTION : 0));
	XSpiPs_SetSlaveSelect(&m_spi,  (uint8_t) 0xf);
	XSpiPs_SetClkPrescaler(&m_spi, XSPIPS_CLK_PRESCALE_16);
	XSpiPs_SetSlaveSelect(&m_spi,  (uint8_t) dev->chip_select);
	XSpiPs_PolledTransfer(&m_spi, data, data, no_of_bytes);
	XSpiPs_SetSlaveSelect(&m_spi,  (uint8_t) 0xf);

	return(0);
}

#endif

// ***************************************************************************
// ***************************************************************************
/* xilinx microblaze */

#ifdef MICROBLAZE

int32_t ad_spi_init(struct spi_device *dev) {

	dev->base_address = 0;
	dev->device_id = 0;
	dev->chip_select = 0xff;
	dev->cpha = 0;
	dev->cpol = 0;
}

int32_t ad_spi_xfer(struct spi_device *dev, uint8_t *data, uint8_t no_of_bytes) {

	uint32_t i;

	ad_reg_write((XPAR_SPI_0_BASEADDR + 0x70), dev->chip_select);
	ad_reg_write((XPAR_SPI_0_BASEADDR + 0x60), (0x086 | (dev->cpol<<3) | (dev->cpha<<4)));
	for (i = 0; i < no_of_bytes; i++) {
		ad_reg_write((XPAR_SPI_0_BASEADDR + 0x68), *(data + i));
		while ((ad_reg_read(XPAR_SPI_0_BASEADDR + 0x64) & 0x1) == 0x1) {}
		*(data + i) = ad_reg_read(XPAR_SPI_0_BASEADDR + 0x6c) & 0xff;
	}
	ad_reg_write((XPAR_SPI_0_BASEADDR + 0x70), 0xff);
	ad_reg_write((XPAR_SPI_0_BASEADDR + 0x60), (0x186 | (dev->cpol<<3) | (dev->cpha<<4)));

	return(0);
}

#endif

// ***************************************************************************
// ***************************************************************************
