/***************************************************************************//**
* @file ad9265.c
* @brief Implementation of AD9265 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include "axi_adc_core.h"
#include "ad9265.h"
#include "no-os/print_log.h"
#define DCO_DEBUG



/***************************************************************************//**
* @brief ad9265_spi_write
*******************************************************************************/
int32_t ad9265_spi_write(struct ad9265_dev *dev,
			 uint8_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[2];

	int32_t ret;

	buf[0] = reg_addr;
	buf[1] = reg_data;

	ret = spi_write_and_read(dev->spi_desc,buf,2);

	return ret;
}

/***************************************************************************//**
* @brief ad9265_setup
*******************************************************************************/
int32_t ad9265_testmode_set(struct ad9265_dev *dev,
			    uint8_t mode)
{
	ad9265_spi_write(dev, 0x04, mode);


	return 0;
}


/***************************************************************************//**
* @brief ad9265_setup
*******************************************************************************/
int32_t ad9265_setup(struct ad9265_dev **device,
		     struct ad9265_init_param init_param,
		     struct axi_adc core)
{
	int32_t ret;
	struct ad9265_dev *dev;

	dev = (struct ad9265_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = spi_init(&dev->spi_desc, &init_param.spi_init);

	ad9265_spi_write(dev, 0x00, 0x80);
	ad9265_spi_write(dev, 0x01, 0x00);
	ad9265_spi_write(dev, 0x02, 0x00);
	ad9265_spi_write(dev, 0x03, 0x01);

	*device = dev;

	printf("AD9265 successfully initialized.\n");

	return ret;
}

/***************************************************************************//**
* @brief ad9265_remove
*******************************************************************************/
int32_t ad9265_remove(struct ad9265_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	free(dev);

	return ret;
}
