/***************************************************************************//**
 *   @file   ad7616_core.c
 *   @brief  Implementation of AD7616 Core Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#include <xparameters.h>
#include <stdint.h>
#include <stdio.h>
#include <xil_io.h>
#include "ad7616_core.h"
#include "platform_drivers.h"
#include "spi_engine.h"

/***************************************************************************//**
* @brief ad7616_core_read
*******************************************************************************/
int32_t ad7616_core_read(adc_core core,
						 uint32_t reg_addr,
						 uint32_t *reg_data)
{
	*reg_data = Xil_In32((core.adc_baseaddr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief ad7616_core_write
*******************************************************************************/
int32_t ad7616_core_write(adc_core core,
						  uint32_t reg_addr,
						  uint32_t reg_data)
{
	Xil_Out32((core.adc_baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief ad7616_core_read
*******************************************************************************/
int32_t ad7616_dma_read(adc_core core,
						uint32_t reg_addr,
						uint32_t *reg_data)
{
	*reg_data = Xil_In32((core.dmac_baseaddr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief ad7616_core_write
*******************************************************************************/
int32_t ad7616_dma_write(adc_core core,
						 uint32_t reg_addr,
						 uint32_t reg_data)
{
	Xil_Out32((core.dmac_baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief ad7616_core_setup
*******************************************************************************/
int32_t ad7616_core_setup(adc_core core)
{
	uint32_t type;

	ad7616_core_write(core, AD7616_REG_UP_CTRL, 0x00);
	mdelay(10);
	ad7616_core_write(core, AD7616_REG_UP_CTRL, AD7616_CTRL_RESETN);
	ad7616_core_write(core, AD7616_REG_UP_CONV_RATE, 100);
	ad7616_core_write(core, AD7616_REG_UP_CTRL,
								AD7616_CTRL_RESETN | AD7616_CTRL_CNVST_EN);
	mdelay(10);
	ad7616_core_write(core, AD7616_REG_UP_CTRL, AD7616_CTRL_RESETN);
	mdelay(10);

	ad7616_core_read(core, AD7616_REG_UP_IF_TYPE, &type);
	printf("AD7616 IP Core (%s interface) successfully initialized\n",
			type ? "parallel" : "serial");

	return 0;
}

/***************************************************************************//**
 * @brief ad7616_capture_serial
*******************************************************************************/
int32_t ad7616_capture_serial(adc_core core,
							  uint32_t no_of_samples,
							  uint32_t start_address)
{
	uint32_t reg_val;
	uint32_t transfer_id;
	uint32_t length;

	mdelay(10);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0x0);
	mdelay(10);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_RESET(0), 0x1);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_SDO_MEM(0), 0x00);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), 0x2103);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), 0x2000);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), 0x11fe);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), 0x0201);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), 0x3000);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), 0x11ff);
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0x1);

	ad7616_core_write(core, AD7616_REG_UP_CTRL,
								AD7616_CTRL_RESETN | AD7616_CTRL_CNVST_EN);

	length = no_of_samples * core.no_of_channels * ((core.resolution + 7) / 8);

	ad7616_dma_write(core, ADC_DMAC_REG_CTRL, 0x0);
	ad7616_dma_write(core, ADC_DMAC_REG_CTRL, ADC_DMAC_CTRL_ENABLE);

	ad7616_dma_write(core, ADC_DMAC_REG_IRQ_MASK, 0x0);

	ad7616_dma_read(core, ADC_DMAC_REG_TRANSFER_ID, &transfer_id);
	ad7616_dma_read(core, ADC_DMAC_REG_IRQ_PENDING, &reg_val);
	ad7616_dma_write(core, ADC_DMAC_REG_IRQ_PENDING, reg_val);

	ad7616_dma_write(core, ADC_DMAC_REG_DEST_ADDRESS, start_address);
	ad7616_dma_write(core, ADC_DMAC_REG_DEST_STRIDE, 0x0);
	ad7616_dma_write(core, ADC_DMAC_REG_X_LENGTH, length - 1);
	ad7616_dma_write(core, ADC_DMAC_REG_Y_LENGTH, 0x0);

	ad7616_dma_write(core, ADC_DMAC_REG_START_TRANSFER, 0x1);
	/* Wait until the new transfer is queued. */
	do {
		ad7616_dma_read(core, ADC_DMAC_REG_START_TRANSFER, &reg_val);
	} while (reg_val == 1);

	/* Wait until the current transfer is completed. */
	do {
		ad7616_dma_read(core, ADC_DMAC_REG_IRQ_PENDING, &reg_val);
	} while ((reg_val != (ADC_DMAC_IRQ_SOT | ADC_DMAC_IRQ_EOT)));
	ad7616_dma_write(core, ADC_DMAC_REG_IRQ_PENDING, reg_val);

	/* Wait until the transfer with the ID transfer_id is completed. */
	do {
		ad7616_dma_read(core, ADC_DMAC_REG_TRANSFER_DONE, &reg_val);
	} while ((reg_val & (1 << transfer_id)) != (1 << transfer_id));

	ad7616_core_write(core, AD7616_REG_UP_CTRL, AD7616_CTRL_RESETN);

	return 0;
}

/***************************************************************************//**
 * @brief ad7616_capture_parallel
*******************************************************************************/
int32_t ad7616_capture_parallel(adc_core core,
								uint32_t no_of_samples,
								uint32_t start_address)
{
	uint32_t reg_val;
	uint32_t transfer_id;
	uint32_t length;

	ad7616_core_write(core, AD7616_REG_UP_CTRL,
								AD7616_CTRL_RESETN | AD7616_CTRL_CNVST_EN);

	length = no_of_samples * core.no_of_channels * ((core.resolution + 7) / 8);

	ad7616_dma_write(core, ADC_DMAC_REG_CTRL, 0x0);
	ad7616_dma_write(core, ADC_DMAC_REG_CTRL, ADC_DMAC_CTRL_ENABLE);

	ad7616_dma_write(core, ADC_DMAC_REG_IRQ_MASK, 0x0);

	ad7616_dma_read(core, ADC_DMAC_REG_TRANSFER_ID, &transfer_id);
	ad7616_dma_read(core, ADC_DMAC_REG_IRQ_PENDING, &reg_val);
	ad7616_dma_write(core, ADC_DMAC_REG_IRQ_PENDING, reg_val);

	ad7616_dma_write(core, ADC_DMAC_REG_DEST_ADDRESS, start_address);
	ad7616_dma_write(core, ADC_DMAC_REG_DEST_STRIDE, 0x0);
	ad7616_dma_write(core, ADC_DMAC_REG_X_LENGTH, length - 1);
	ad7616_dma_write(core, ADC_DMAC_REG_Y_LENGTH, 0x0);

	ad7616_dma_write(core, ADC_DMAC_REG_START_TRANSFER, 0x1);
	/* Wait until the new transfer is queued. */
	do {
		ad7616_dma_read(core, ADC_DMAC_REG_START_TRANSFER, &reg_val);
	} while (reg_val == 1);

	/* Wait until the current transfer is completed. */
	do {
		ad7616_dma_read(core, ADC_DMAC_REG_IRQ_PENDING, &reg_val);
	} while ((reg_val != (ADC_DMAC_IRQ_SOT | ADC_DMAC_IRQ_EOT)));
	ad7616_dma_write(core, ADC_DMAC_REG_IRQ_PENDING, reg_val);

	/* Wait until the transfer with the ID transfer_id is completed. */
	do {
		ad7616_dma_read(core, ADC_DMAC_REG_TRANSFER_DONE, &reg_val);
	} while ((reg_val & (1 << transfer_id)) != (1 << transfer_id));

	ad7616_core_write(core, AD7616_REG_UP_CTRL, AD7616_CTRL_RESETN);

	return 0;
}
