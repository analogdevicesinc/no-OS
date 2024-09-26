/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to XILINX platform used by eval-adxl367z
 *           project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "xparameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define BUF_LENGTH 				32U
#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define SPI_OPS 				&xil_spi_ops
#define GPIO_OPS				&xil_gpio_ops
#define GPIO_OFFSET				54U

#define RDY_PIN					GPIO_OFFSET + 32U
#define SYNC_PIN				GPIO_OFFSET + 33U

extern struct xil_spi_init_param spi_extra;
extern struct xil_gpio_init_param gpio_extra;

#endif //__PARAMETERS_H__
