/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define CF_AD9361_RX_BASEADDR		0
#define CF_AD9361_TX_BASEADDR		0
#define CF_AD9361_RX_DMA_BASEADDR	0
#define CF_AD9361_TX_DMA_BASEADDR	0

#define ADC_DDR_BASEADDR		0
#define DAC_DDR_BASEADDR		0

#define GPIO_DEVICE_ID			0
#define GPIO_CHIP_BASE			906
#define GPIO_RESET_PIN			(GPIO_CHIP_BASE + 100)
#define GPIO_RESET_PIN_2		(GPIO_CHIP_BASE + 113)
#define GPIO_SYNC_PIN			(GPIO_CHIP_BASE + 99)
#define GPIO_CAL_SW1_PIN		(GPIO_CHIP_BASE + 107)
#define GPIO_CAL_SW2_PIN		(GPIO_CHIP_BASE + 108)
#define SPI_DEVICE_ID			0

#define AD9361_UIO_DEV			"/dev/uio0"
#define AD9361_UIO_SIZE			"/sys/class/uio/uio0/maps/map0/size"
#define AD9361_UIO_ADDR			"/sys/class/uio/uio0/maps/map0/addr"
#define RX_DMA_UIO_DEV			"/dev/uio1"
#define RX_DMA_UIO_SIZE			"/sys/class/uio/uio1/maps/map0/size"
#define RX_DMA_UIO_ADDR			"/sys/class/uio/uio1/maps/map0/addr"
#define RX_BUFF_MEM_SIZE		"/sys/class/uio/uio1/maps/map1/size"
#define RX_BUFF_MEM_ADDR		"/sys/class/uio/uio1/maps/map1/addr"
#define TX_DMA_UIO_DEV			"/dev/uio2"
#define TX_DMA_UIO_SIZE			"/sys/class/uio/uio2/maps/map0/size"
#define TX_DMA_UIO_ADDR			"/sys/class/uio/uio2/maps/map0/addr"
#define TX_BUFF_MEM_SIZE		"/sys/class/uio/uio2/maps/map1/size"
#define TX_BUFF_MEM_ADDR		"/sys/class/uio/uio2/maps/map1/addr"
#define SPIDEV_DEV			"/dev/spidev32766.0"
#define AD9361_B_UIO_DEV		"/dev/uio3"
#define AD9361_B_UIO_SIZE		"/sys/class/uio/uio3/maps/map0/size"
#define AD9361_B_UIO_ADDR		"/sys/class/uio/uio3/maps/map0/addr"
#define SPIDEV_B_DEV			"/dev/spidev32766.1"

#endif // __PARAMETERS_H__
