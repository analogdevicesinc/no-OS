/***************************************************************************//**
 *   @file   ad5766_core.h
 *   @brief  Header file of AD5766 Core Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AD5766_CORE_H_
#define AD5766_CORE_H_

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t core_baseaddr;
	uint32_t dma_baseaddr;
	uint32_t dma_source_addr;
	uint32_t rate_hz;
	uint32_t spi_clk_hz;
} ad5766_core;

typedef struct {
	uint32_t core_baseaddr;
	uint32_t dma_baseaddr;
	uint32_t dma_source_addr;
	uint32_t rate_hz;
	uint32_t spi_clk_hz;
} ad5766_core_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad5766_core_setup(struct spi_engine_desc *eng_desc,
			  ad5766_core **ad_core,
			  ad5766_core_init_param init_param);
#endif
