/***************************************************************************//**
 *   @file   adrv9009_xilinx_wrappers.c
 *   @brief  Source file of ADRV9009 Xilinx Wrapper.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#include "adrv9009_xilinx_wrappers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the Xilinx specific SPI parameters.
 * @param param - The structure that contains the SPI parameters.
 * @return none
 */
void xilinx_spi_wrapper(spi_init_param *param){
	xil_spi_init_param *xil_spi_param = calloc(1, sizeof(xil_spi_init_param));
	xil_spi_param->id = 0;
	xil_spi_param->flags = SPI_CS_DECODE;
	param->extra = xil_spi_param;
}

/**
 * @brief Wrap Xilinx specific read function.
 * @param base - Base address of the register.
 * @param offset - Address offset of the register.
 * @return data
 */
int32_t xilinx_read_wrapper(uint32_t base, uint32_t offset){
	return Xil_In32(base + offset);
}

/**
 * @brief Wrap Xilinx specific write function.
 * @param base - Base address of the register.
 * @param offset - Address offset of the register.
 * @param data - data to be written.
 * @return none
 */
void xilinx_write_wrapper(uint32_t base, uint32_t offset, uint32_t data){
	Xil_Out32(base + offset, data);
}

/**
 * @brief Wrap Xilinx cache flush function.
 * @return none
 */
void xilinx_cache_flush_wrapper(){
	Xil_DCacheFlush();
}
