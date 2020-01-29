/***************************************************************************//**
 *   @file   ad7616_core.h
 *   @brief  Header file of AD7616 Core Driver.
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
#ifndef AD7616_CORE_H_
#define AD7616_CORE_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7616_REG_PCORE_VERSION		0x400
#define AD7616_REG_ID					0x404
#define AD7616_REG_UP_SCRATCH			0x408
#define AD7616_REG_UP_IF_TYPE			0x40C
#define AD7616_REG_UP_CTRL				0x440
#define AD7616_REG_UP_CONV_RATE			0x444
#define AD7616_REG_UP_BURST_LENGTH		0x448
#define AD7616_REG_UP_READ_DATA			0x44C
#define AD7616_REG_UP_WRITE_DATA		0x450

/* AD7616_REG_UP_CTRL */
#define AD7616_CTRL_RESETN				(1 << 0)
#define AD7616_CTRL_CNVST_EN			(1 << 1)

#define ADC_DMAC_REG_IRQ_MASK			0x80
#define ADC_DMAC_REG_IRQ_PENDING		0x84
#define ADC_DMAC_REG_IRQ_SOURCE			0x88

#define ADC_DMAC_REG_CTRL				0x400
#define ADC_DMAC_REG_TRANSFER_ID		0x404
#define ADC_DMAC_REG_START_TRANSFER		0x408
#define ADC_DMAC_REG_FLAGS				0x40c
#define ADC_DMAC_REG_DEST_ADDRESS		0x410
#define ADC_DMAC_REG_SRC_ADDRESS		0x414
#define ADC_DMAC_REG_X_LENGTH			0x418
#define ADC_DMAC_REG_Y_LENGTH			0x41c
#define ADC_DMAC_REG_DEST_STRIDE		0x420
#define ADC_DMAC_REG_SRC_STRIDE			0x424
#define ADC_DMAC_REG_TRANSFER_DONE		0x428
#define ADC_DMAC_REG_ACTIVE_TRANSFER_ID 0x42c
#define ADC_DMAC_REG_STATUS				0x430
#define ADC_DMAC_REG_CURRENT_DEST_ADDR	0x434
#define ADC_DMAC_REG_CURRENT_SRC_ADDR	0x438
#define ADC_DMAC_REG_DBG0				0x43c
#define ADC_DMAC_REG_DBG1				0x440

#define ADC_DMAC_CTRL_ENABLE			(1 << 0)
#define ADC_DMAC_CTRL_PAUSE				(1 << 1)

#define ADC_DMAC_IRQ_SOT				(1 << 0)
#define ADC_DMAC_IRQ_EOT				(1 << 1)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t	adc_baseaddr;
	uint32_t	dmac_baseaddr;
	uint8_t		no_of_channels;
	uint8_t		resolution;
} adc_core;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad7616_core_read(adc_core core,
			 uint32_t reg_addr,
			 uint32_t *reg_data);
int32_t ad7616_core_write(adc_core core,
			  uint32_t reg_addr,
			  uint32_t reg_data);
int32_t ad7616_dma_read(adc_core core,
			uint32_t reg_addr,
			uint32_t *reg_data);
int32_t ad7616_dma_write(adc_core core,
			 uint32_t reg_addr,
			 uint32_t reg_data);
int32_t ad7616_core_setup(adc_core core);
int32_t ad7616_capture_serial(adc_core core,
			      uint32_t no_of_samples,
			      uint32_t start_address);
int32_t ad7616_capture_parallel(adc_core core,
				uint32_t no_of_samples,
				uint32_t start_address);
#endif
