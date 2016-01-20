/***************************************************************************//**
* @file ad9517.c
* @brief Implementation of AD9517 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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
#include <xil_printf.h>
#include "platform_drivers.h"
#include "ad9517.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
uint8_t ad9517_slave_select;

/***************************************************************************//**
* @brief ad9517_spi_read
*******************************************************************************/
int32_t ad9517_spi_read(uint16_t reg_addr, uint8_t *reg_data)
{
	uint8_t buf[4];
	int32_t ret;

	buf[0] = 0x84;
	buf[1] = 0x80 | (reg_addr >> 8);
	buf[2] = reg_addr & 0xFF;
	buf[3] = 0x00;

	ret = spi_write_and_read(ad9517_slave_select, buf, 4);
	*reg_data = buf[3];

	return ret;
}

/***************************************************************************//**
* @brief ad9517_spi_write
*******************************************************************************/
int32_t ad9517_spi_write(uint16_t reg_addr, uint8_t reg_data)
{
	uint8_t buf[4];
	int32_t ret;

	buf[0] = 0x84;
	buf[1] = reg_addr >> 8;
	buf[2] = reg_addr & 0xFF;
	buf[3] = reg_data;

	ret = spi_write_and_read(ad9517_slave_select, buf, 4);

	return ret;
}

/***************************************************************************//**
* @brief ad9517_setup
*******************************************************************************/
int32_t ad9517_setup(uint32_t spi_device_id, uint8_t slave_select)
{
	uint8_t stat;

	ad9517_slave_select = slave_select;
	spi_init(spi_device_id, 0, 0);

	ad9517_spi_write(0x0010, 0x7c);
	ad9517_spi_write(0x0014, 0x05);
	ad9517_spi_write(0x0016, 0x05);
	ad9517_spi_write(0x001c, 0x02);
	ad9517_spi_write(0x00f1, 0x08);
	ad9517_spi_write(0x00f5, 0x08);
	ad9517_spi_write(0x0140, 0x42);
	ad9517_spi_write(0x0141, 0x42);
	ad9517_spi_write(0x0142, 0x42);
	ad9517_spi_write(0x0143, 0x43);
	ad9517_spi_write(0x0190, 0x21);
	ad9517_spi_write(0x0191, 0x00);
	ad9517_spi_write(0x0197, 0x80);
	ad9517_spi_write(0x0198, 0x02);
	ad9517_spi_write(0x0199, 0x21);
	ad9517_spi_write(0x019c, 0x20);
	ad9517_spi_write(0x019e, 0x21);
	ad9517_spi_write(0x01a0, 0x11);
	ad9517_spi_write(0x01a1, 0x20);
	ad9517_spi_write(0x01e0, 0x00);
	ad9517_spi_write(0x01e1, 0x02);
	ad9517_spi_write(0x0232, 0x01);
	ad9517_spi_write(0x0018, 0x06);
	ad9517_spi_write(0x0232, 0x01);
	mdelay(50);

	ad9517_spi_write(0x0018, 0x07);
	ad9517_spi_write(0x0232, 0x01);
	mdelay(50);

	ad9517_spi_read(0x001f, &stat);
	xil_printf("AD9517 PLL %s.\n", stat & 0x01 ? "ok" : "errors");

	return 0;
}
