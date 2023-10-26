/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by fmcdaq3 examples.
 *   @author 
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "ad9152.h"
#include "ad9528.h"
#include "ad9680.h"
#include "axi_dac_core.h"
#include "axi_jesd204_tx.h"
#include "axi_jesd204_rx.h"
#include "axi_dmac.h"

#ifdef IIO_SUPPORT
#endif

extern struct no_os_uart_init_param iio_uart_ip;
extern struct no_os_spi_init_param ad9528_spi_param;
extern struct no_os_spi_init_param ad9152_spi_param;
extern struct no_os_spi_init_param ad9680_spi_param;
extern struct no_os_gpio_init_param dac_txen_param;
extern struct no_os_gpio_init_param adc_pd_param;
extern struct ad9528_init_param ad9528_param;
extern struct ad9152_init_param ad9152_param;
extern struct ad9680_init_param ad9680_param;
extern struct axi_adc_init ad9680_core_param;
extern struct axi_dac_init ad9152_core_param;
extern struct axi_dmac_init ad9680_dmac_param;
extern struct axi_dmac_init ad9152_dmac_param;
extern struct jesd204_tx_init ad9152_jesd_param;
extern struct jesd204_rx_init  ad9680_jesd_param;

#endif /* __COMMON_DATA_H__ */
