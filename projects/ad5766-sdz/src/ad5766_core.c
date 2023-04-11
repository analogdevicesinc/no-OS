/***************************************************************************//**
 *   @file   ad5766_core.c
 *   @brief  Implementation of AD5766 Core Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include <stdlib.h>
#include "spi_engine.h"
#include "spi_engine_private.h"
#include <xil_io.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "ad5766_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
const uint16_t sine_lut[512] = {
	0x8000, 0x8192, 0x8324, 0x84b6, 0x8647, 0x87d9, 0x896a, 0x8afb,
	0x8c8b, 0x8e1b, 0x8fab, 0x9139, 0x92c7, 0x9455, 0x95e1, 0x976d,
	0x98f8, 0x9a82, 0x9c0b, 0x9d93, 0x9f19, 0xa09f, 0xa223, 0xa3a6,
	0xa527, 0xa6a7, 0xa826, 0xa9a3, 0xab1f, 0xac98, 0xae10, 0xaf87,
	0xb0fb, 0xb26e, 0xb3de, 0xb54d, 0xb6b9, 0xb824, 0xb98c, 0xbaf2,
	0xbc56, 0xbdb7, 0xbf17, 0xc073, 0xc1cd, 0xc325, 0xc47a, 0xc5cc,
	0xc71c, 0xc869, 0xc9b3, 0xcafb, 0xcc3f, 0xcd81, 0xcebf, 0xcffb,
	0xd133, 0xd268, 0xd39a, 0xd4c9, 0xd5f5, 0xd71d, 0xd842, 0xd964,
	0xda82, 0xdb9c, 0xdcb3, 0xddc7, 0xded7, 0xdfe3, 0xe0eb, 0xe1f0,
	0xe2f1, 0xe3ee, 0xe4e8, 0xe5dd, 0xe6cf, 0xe7bc, 0xe8a6, 0xe98b,
	0xea6d, 0xeb4a, 0xec23, 0xecf8, 0xedc9, 0xee96, 0xef5e, 0xf022,
	0xf0e2, 0xf19d, 0xf254, 0xf307, 0xf3b5, 0xf45f, 0xf504, 0xf5a5,
	0xf641, 0xf6d8, 0xf76b, 0xf7fa, 0xf884, 0xf909, 0xf989, 0xfa05,
	0xfa7c, 0xfaee, 0xfb5c, 0xfbc5, 0xfc29, 0xfc88, 0xfce3, 0xfd39,
	0xfd89, 0xfdd5, 0xfe1d, 0xfe5f, 0xfe9c, 0xfed5, 0xff09, 0xff37,
	0xff61, 0xff86, 0xffa6, 0xffc1, 0xffd8, 0xffe9, 0xfff5, 0xfffd,
	0xffff, 0xfffd, 0xfff5, 0xffe9, 0xffd8, 0xffc1, 0xffa6, 0xff86,
	0xff61, 0xff37, 0xff09, 0xfed5, 0xfe9c, 0xfe5f, 0xfe1d, 0xfdd5,
	0xfd89, 0xfd39, 0xfce3, 0xfc88, 0xfc29, 0xfbc5, 0xfb5c, 0xfaee,
	0xfa7c, 0xfa05, 0xf989, 0xf909, 0xf884, 0xf7fa, 0xf76b, 0xf6d8,
	0xf641, 0xf5a5, 0xf504, 0xf45f, 0xf3b5, 0xf307, 0xf254, 0xf19d,
	0xf0e2, 0xf022, 0xef5e, 0xee96, 0xedc9, 0xecf8, 0xec23, 0xeb4a,
	0xea6d, 0xe98b, 0xe8a6, 0xe7bc, 0xe6cf, 0xe5dd, 0xe4e8, 0xe3ee,
	0xe2f1, 0xe1f0, 0xe0eb, 0xdfe3, 0xded7, 0xddc7, 0xdcb3, 0xdb9c,
	0xda82, 0xd964, 0xd842, 0xd71d, 0xd5f5, 0xd4c9, 0xd39a, 0xd268,
	0xd133, 0xcffb, 0xcebf, 0xcd81, 0xcc3f, 0xcafb, 0xc9b3, 0xc869,
	0xc71c, 0xc5cc, 0xc47a, 0xc325, 0xc1cd, 0xc073, 0xbf17, 0xbdb7,
	0xbc56, 0xbaf2, 0xb98c, 0xb824, 0xb6b9, 0xb54d, 0xb3de, 0xb26e,
	0xb0fb, 0xaf87, 0xae10, 0xac98, 0xab1f, 0xa9a3, 0xa826, 0xa6a7,
	0xa527, 0xa3a6, 0xa223, 0xa09f, 0x9f19, 0x9d93, 0x9c0b, 0x9a82,
	0x98f8, 0x976d, 0x95e1, 0x9455, 0x92c7, 0x9139, 0x8fab, 0x8e1b,
	0x8c8b, 0x8afb, 0x896a, 0x87d9, 0x8647, 0x84b6, 0x8324, 0x8192,
	0x8000, 0x7e6d, 0x7cdb, 0x7b49, 0x79b8, 0x7826, 0x7695, 0x7504,
	0x7374, 0x71e4, 0x7054, 0x6ec6, 0x6d38, 0x6baa, 0x6a1e, 0x6892,
	0x6707, 0x657d, 0x63f4, 0x626c, 0x60e6, 0x5f60, 0x5ddc, 0x5c59,
	0x5ad8, 0x5958, 0x57d9, 0x565c, 0x54e0, 0x5367, 0x51ef, 0x5078,
	0x4f04, 0x4d91, 0x4c21, 0x4ab2, 0x4946, 0x47db, 0x4673, 0x450d,
	0x43a9, 0x4248, 0x40e8, 0x3f8c, 0x3e32, 0x3cda, 0x3b85, 0x3a33,
	0x38e3, 0x3796, 0x364c, 0x3504, 0x33c0, 0x327e, 0x3140, 0x3004,
	0x2ecc, 0x2d97, 0x2c65, 0x2b36, 0x2a0a, 0x28e2, 0x27bd, 0x269b,
	0x257d, 0x2463, 0x234c, 0x2238, 0x2128, 0x201c, 0x1f14, 0x1e0f,
	0x1d0e, 0x1c11, 0x1b17, 0x1a22, 0x1930, 0x1843, 0x1759, 0x1674,
	0x1592, 0x14b5, 0x13dc, 0x1307, 0x1236, 0x1169, 0x10a1, 0xfdd,
	0xf1d,  0xe62,  0xdab,  0xcf8,  0xc4a,  0xba0,  0xafb,  0xa5a,
	0x9be,  0x927,  0x894,  0x805,  0x77b,  0x6f6,  0x676,  0x5fa,
	0x583,  0x511,  0x4a3,  0x43a,  0x3d6,  0x377,  0x31c,  0x2c6,
	0x276,  0x22a,  0x1e2,  0x1a0,  0x163,  0x12a,  0xf6,   0xc8,
	0x9e,   0x79,   0x59,   0x3e,   0x27,   0x16,   0xa,    0x2,
	0x0,    0x2,    0xa,    0x16,   0x27,   0x3e,   0x59,   0x79,
	0x9e,   0xc8,   0xf6,   0x12a,  0x163,  0x1a0,  0x1e2,  0x22a,
	0x276,  0x2c6,  0x31c,  0x377,  0x3d6,  0x43a,  0x4a3,  0x511,
	0x583,  0x5fa,  0x676,  0x6f6,  0x77b,  0x805,  0x894,  0x927,
	0x9be,  0xa5a,  0xafb,  0xba0,  0xc4a,  0xcf8,  0xdab,  0xe62,
	0xf1d,  0xfdd,  0x10a1, 0x1169, 0x1236, 0x1307, 0x13dc, 0x14b5,
	0x1592, 0x1674, 0x1759, 0x1843, 0x1930, 0x1a22, 0x1b17, 0x1c11,
	0x1d0e, 0x1e0f, 0x1f14, 0x201c, 0x2128, 0x2238, 0x234c, 0x2463,
	0x257d, 0x269b, 0x27bd, 0x28e2, 0x2a0a, 0x2b36, 0x2c65, 0x2d97,
	0x2ecc, 0x3004, 0x3140, 0x327e, 0x33c0, 0x3504, 0x364c, 0x3796,
	0x38e3, 0x3a33, 0x3b85, 0x3cda, 0x3e32, 0x3f8c, 0x40e8, 0x4248,
	0x43a9, 0x450d, 0x4673, 0x47db, 0x4946, 0x4ab2, 0x4c21, 0x4d91,
	0x4f04, 0x5078, 0x51ef, 0x5367, 0x54e0, 0x565c, 0x57d9, 0x5958,
	0x5ad8, 0x5c59, 0x5ddc, 0x5f60, 0x60e6, 0x626c, 0x63f4, 0x657d,
	0x6707, 0x6892, 0x6a1e, 0x6baa, 0x6d38, 0x6ec6, 0x7054, 0x71e4,
	0x7374, 0x7504, 0x7695, 0x7826, 0x79b8, 0x7b49, 0x7cdb, 0x7e6d,
};

/***************************************************************************//**
* @brief ad5766_core_write
*******************************************************************************/
int32_t ad5766_core_write(ad5766_core *core,
			  uint32_t reg_addr,
			  uint32_t reg_data)
{
	Xil_Out32((core->core_baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief ad5766_core_read
*******************************************************************************/
void ad5766_core_read(ad5766_core *core,
		      uint32_t reg_addr,
		      uint32_t *reg_data)
{
	*reg_data = Xil_In32(core->core_baseaddr + reg_addr);
}

/***************************************************************************//**
* @brief ad5766_core_write
*******************************************************************************/
int32_t ad5766_dma_write(ad5766_core *core,
			 uint32_t reg_addr,
			 uint32_t reg_data)
{
	Xil_Out32((core->dma_baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief ad5766_core_read
*******************************************************************************/
void ad5766_dma_read(ad5766_core *core,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
{
	*reg_data = Xil_In32(core->dma_baseaddr + reg_addr);
}

/***************************************************************************//**
* @brief ad5766_core_setup
*******************************************************************************/
int32_t ad5766_core_setup(struct spi_engine_desc *eng_desc,
			  ad5766_core **ad_core,
			  ad5766_core_init_param init_param)
{
	ad5766_core	*core;
	uint32_t	ref_clk_hz = 100000000;
	uint32_t	rate_reg;
	uint32_t	clk_div = 0;
	uint8_t		spi_config = SPI_ENGINE_CONFIG_CPHA;
	uint8_t		chip_select = 0;
	uint32_t	sync_id = 0;
	uint8_t		size = 3;

	core = (ad5766_core *)no_os_malloc(sizeof(*core));
	if (!core)
		return -1;

	core->core_baseaddr = init_param.core_baseaddr;
	core->dma_baseaddr = init_param.dma_baseaddr;
	core->dma_source_addr = init_param.dma_source_addr;
	core->rate_hz = init_param.rate_hz;
	core->spi_clk_hz = init_param.spi_clk_hz;

	rate_reg = ref_clk_hz / core->rate_hz;
	if (rate_reg > 0xFFFF) {
		no_os_free(core);
		return -1;
	}

	ad5766_core_write(core, 0x00001100, 0x00020);	// DAC0
	ad5766_core_write(core, 0x00001104, 0x00021);	// DAC1
	ad5766_core_write(core, 0x00001108, 0x00022);	// DAC2
	ad5766_core_write(core, 0x0000110C, 0x00023);	// DAC3
	ad5766_core_write(core, 0x00001110, 0x00024);	// DAC4
	ad5766_core_write(core, 0x00001114, 0x00025);	// DAC5
	ad5766_core_write(core, 0x00001118, 0x00026);	// DAC6
	ad5766_core_write(core, 0x0000111C, 0x00027);	// DAC7
	ad5766_core_write(core, 0x00001120, 0x00028);	// DAC8
	ad5766_core_write(core, 0x00001124, 0x00029);	// DAC9
	ad5766_core_write(core, 0x00001128, 0x0002A);	// DAC10
	ad5766_core_write(core, 0x0000112C, 0x0002B);	// DAC11
	ad5766_core_write(core, 0x00001130, 0x0002C);	// DAC12
	ad5766_core_write(core, 0x00001134, 0x0002D);	// DAC13
	ad5766_core_write(core, 0x00001138, 0x0002E);	// DAC14
	ad5766_core_write(core, 0x0000113C, 0x1002F);	// DAC15

	ad5766_core_write(core, 0x00000040, 0x0003);
	ad5766_core_write(core, 0x0000004C, rate_reg);

	clk_div = (ref_clk_hz / core->spi_clk_hz) - 1;

	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_RESET(0), 0x0001);

	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
			 SPI_ENGINE_CMD_CONFIG(SPI_ENGINE_CMD_REG_CONFIG, spi_config));

	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
			 SPI_ENGINE_CMD_CONFIG(SPI_ENGINE_CMD_REG_CLK_DIV, clk_div));

	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
			 SPI_ENGINE_CMD_ASSERT(1, (0xff ^ NO_OS_BIT(chip_select))));

	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
			 SPI_ENGINE_CMD_TRANSFER(1, size -1));

	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
			 SPI_ENGINE_CMD_SYNC(sync_id));

	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
			 SPI_ENGINE_CMD_ASSERT(1, 0xff));

	ad5766_dma_write(core, 0x400, 0x0);

	uint32_t no_of_samples;
	uint32_t index;
	uint32_t index_mem = 0;
	uint32_t phase_offset;

	no_of_samples = sizeof(sine_lut) / sizeof(typeof(sine_lut[0]));
	phase_offset = no_of_samples/16;

	for (index = 0; index < no_of_samples; index ++) {
		Xil_Out16(core->dma_source_addr + (index_mem + 0)  * 2,
			  sine_lut[(index + ( 0 * phase_offset)) % no_of_samples]);	// DAC0
		Xil_Out16(core->dma_source_addr + (index_mem + 1)  * 2,
			  sine_lut[(index + ( 1 * phase_offset)) % no_of_samples]);	// DAC1
		Xil_Out16(core->dma_source_addr + (index_mem + 2)  * 2,
			  sine_lut[(index + ( 2 * phase_offset)) % no_of_samples]);	// DAC2
		Xil_Out16(core->dma_source_addr + (index_mem + 3)  * 2,
			  sine_lut[(index + ( 3 * phase_offset)) % no_of_samples]);	// DAC3
		Xil_Out16(core->dma_source_addr + (index_mem + 4)  * 2,
			  sine_lut[(index + ( 4 * phase_offset)) % no_of_samples]);	// DAC4
		Xil_Out16(core->dma_source_addr + (index_mem + 5)  * 2,
			  sine_lut[(index + ( 5 * phase_offset)) % no_of_samples]);	// DAC5
		Xil_Out16(core->dma_source_addr + (index_mem + 6)  * 2,
			  sine_lut[(index + ( 6 * phase_offset)) % no_of_samples]);	// DAC6
		Xil_Out16(core->dma_source_addr + (index_mem + 7)  * 2,
			  sine_lut[(index + ( 7 * phase_offset)) % no_of_samples]);	// DAC7
		Xil_Out16(core->dma_source_addr + (index_mem + 8)  * 2,
			  sine_lut[(index + ( 8 * phase_offset)) % no_of_samples]);	// DAC8
		Xil_Out16(core->dma_source_addr + (index_mem + 9)  * 2,
			  sine_lut[(index + ( 9 * phase_offset)) % no_of_samples]);	// DAC9
		Xil_Out16(core->dma_source_addr + (index_mem + 10) * 2,
			  sine_lut[(index + (10 * phase_offset)) % no_of_samples]);	// DAC10
		Xil_Out16(core->dma_source_addr + (index_mem + 11) * 2,
			  sine_lut[(index + (11 * phase_offset)) % no_of_samples]);	// DAC11
		Xil_Out16(core->dma_source_addr + (index_mem + 12) * 2,
			  sine_lut[(index + (12 * phase_offset)) % no_of_samples]);	// DAC12
		Xil_Out16(core->dma_source_addr + (index_mem + 13) * 2,
			  sine_lut[(index + (13 * phase_offset)) % no_of_samples]);	// DAC13
		Xil_Out16(core->dma_source_addr + (index_mem + 14) * 2,
			  sine_lut[(index + (14 * phase_offset)) % no_of_samples]);	// DAC14
		Xil_Out16(core->dma_source_addr + (index_mem + 15) * 2,
			  sine_lut[(index + (15 * phase_offset)) % no_of_samples]);	// DAC15
		index_mem += 16;
	}

	ad5766_dma_write(core, 0x400, 0x0);
	ad5766_dma_write(core, 0x400, 0x1);
	ad5766_dma_write(core, 0x414, core->dma_source_addr);
	ad5766_dma_write(core, 0x424, 0x0);
	ad5766_dma_write(core, 0x418, (16 * sizeof(sine_lut)) - 1);
	ad5766_dma_write(core, 0x41C, 0x0);
	ad5766_dma_write(core, 0x40C, 0x1);
	ad5766_dma_write(core, 0x408, 0x1);

	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0x0001);

	*ad_core = core;

	return 0;
}
