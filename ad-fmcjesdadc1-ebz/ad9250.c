/***************************************************************************//**
* @file ad9250.c
* @brief Implementation of AD9250 Driver.
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
#include <stdio.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "ad9250.h"

/***************************************************************************//**
* @brief ad9250_spi_read
*******************************************************************************/
int32_t ad9250_spi_read(ad9250_dev *dev,
						uint16_t reg_addr,
						uint8_t *reg_data)
{
	uint8_t buf[4];
	int32_t ret;

	buf[0] = 0x80 | dev->id_no;
	buf[1] = 0x80 | (reg_addr >> 8);
	buf[2] = reg_addr & 0xFF;
	buf[3] = 0x00;

	ret = spi_write_and_read(&dev->spi_dev, buf, 4);
	*reg_data = buf[3];

	return ret;
}

/***************************************************************************//**
* @brief ad9250_spi_write
*******************************************************************************/
int32_t ad9250_spi_write(ad9250_dev *dev,
						 uint16_t reg_addr,
						 uint8_t reg_data)
{
	uint8_t buf[4];
	int32_t ret;

	buf[0] = 0x80 | dev->id_no;
	buf[1] = reg_addr >> 8;
	buf[2] = reg_addr & 0xFF;
	buf[3] = reg_data;

	ret = spi_write_and_read(&dev->spi_dev, buf, 4);

	return ret;
}

/***************************************************************************//**
* @brief ad9250_setup
*******************************************************************************/
int32_t ad9250_setup(ad9250_dev **device,
					 ad9250_init_param init_param)
{
	ad9250_dev *dev;
	uint8_t chip_id;
	uint8_t stat;
	int32_t ret;

	dev = (ad9250_dev *)malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	dev->spi_dev.chip_select = init_param.spi_chip_select;
	dev->spi_dev.mode = init_param.spi_mode;
	dev->spi_dev.device_id = init_param.spi_device_id;
	dev->spi_dev.type = init_param.spi_type;
	ret = spi_init(&dev->spi_dev);

	dev->id_no = init_param.id_no;

	ad9250_spi_read(dev, 0x01, &chip_id);
	printf("AD9250 CHIP ID %s (0x%x).\n",
			(chip_id == 0xB9) ? "ok" : "errors", chip_id);

	ad9250_spi_write(dev, 0x5f, (0x16 | 0x1));
	ad9250_spi_write(dev, 0x5e, 0x22);
	ad9250_spi_write(dev, 0x66, ((dev->id_no*2)+0));
	ad9250_spi_write(dev, 0x67, ((dev->id_no*2)+1));
	ad9250_spi_write(dev, 0x6e, 0x81);
	ad9250_spi_write(dev, 0x70, 0x1f);
	ad9250_spi_write(dev, 0x3a, 0x1f);
	ad9250_spi_write(dev, 0x5f, (0x16 | 0x0));
	ad9250_spi_write(dev, 0x14, 0x00);
	ad9250_spi_write(dev, 0x0d, 0x00);
	ad9250_spi_write(dev, 0xff, 0x01);
	mdelay(10);
	ad9250_spi_read(dev, 0x0a, &stat);
	printf("AD9250 PLL/link %s (0x%x).\n",
			(stat == 0x81) ? "ok" : "errors", stat);

	*device = dev;

	return ret;
}
