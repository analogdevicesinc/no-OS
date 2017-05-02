/***************************************************************************//**
 * @file platform_drivers.h
 * @brief Header file of Platform Drivers.
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
#ifndef PLATFORM_DRIVERS_H_
#define PLATFORM_DRIVERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <sleep.h>
#include <xspips.h>
#include <xgpiops.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD9528_RESET_B      59
#define AD9528_SYSREF_REQ   58
#define AD9371_TX1_ENABLE   57
#define AD9371_TX2_ENABLE   56
#define AD9371_RX1_ENABLE   55
#define AD9371_RX2_ENABLE   54
#define AD9371_TEST         53
#define AD9371_RESET_B      52
#define AD9371_GPINT        51
#define AD9371_GPIO_00      50
#define AD9371_GPIO_01      49
#define AD9371_GPIO_02      48
#define AD9371_GPIO_03      47
#define AD9371_GPIO_04      46
#define AD9371_GPIO_05      45
#define AD9371_GPIO_06      44
#define AD9371_GPIO_07      43
#define AD9371_GPIO_15      42
#define AD9371_GPIO_08      41
#define AD9371_GPIO_09      40
#define AD9371_GPIO_10      39
#define AD9371_GPIO_11      38
#define AD9371_GPIO_12      37
#define AD9371_GPIO_14      36
#define AD9371_GPIO_13      35
#define AD9371_GPIO_17      34
#define AD9371_GPIO_16      33
#define AD9371_GPIO_18      32

#define AD9528_CHIP_SELECT	0
#define AD9371_CHIP_SELECT	1

#define SPI_BASEADDR		XPAR_PS7_SPI_0_BASEADDR
#define SPI_DEVICE_ID		XPAR_PS7_SPI_0_DEVICE_ID

#define RX_CLKGEN_BASEADDR		XPAR_AXI_AD9371_RX_CLKGEN_BASEADDR
#define TX_CLKGEN_BASEADDR		XPAR_AXI_AD9371_TX_CLKGEN_BASEADDR
#define RX_OS_CLKGEN_BASEADDR	XPAR_AXI_AD9371_RX_OS_CLKGEN_BASEADDR

#define RX_XCVR_BASEADDR		XPAR_AXI_AD9371_RX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_AD9371_TX_XCVR_BASEADDR
#define RX_OS_XCVR_BASEADDR		XPAR_AXI_AD9371_RX_OS_XCVR_BASEADDR

#define RX_JESD_BASEADDR		XPAR_AXI_AD9371_RX_JESD_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_AD9371_TX_JESD_BASEADDR
#define RX_OS_JESD_BASEADDR		XPAR_AXI_AD9371_RX_OS_JESD_BASEADDR

#define mdelay(msecs)		usleep(1000*msecs)
#define udelay(usecs)		usleep(usecs)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t	base_address;
	uint32_t	device_id;
	uint8_t		chip_select;
	uint32_t	cpha;
	uint32_t	cpol;
	uint8_t		id_no;
} spi_device;

typedef struct {
	uint32_t		device_id;
	XGpioPs_Config	*ps7_config;
	XGpioPs			ps7_instance;
} gpio_device;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t spi_write_and_read(spi_device *dev,
						   uint8_t *data,
						   uint8_t bytes_number);
int32_t gpio_set(uint8_t pin,
				 uint8_t data);
int32_t gpio_get(uint8_t pin,
				 uint8_t *data);
int32_t clkgen_setup(void);
int32_t xcvr_setup(void);
int32_t jesd_setup(void);

#endif
