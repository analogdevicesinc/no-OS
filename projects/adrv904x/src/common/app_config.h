/***************************************************************************//**
 *   @file   adrv902x/src/common/app_config.h
 *   @brief  Config file for the ADRV902x project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

//#define IIO_SUPPORT

//#define DMA_EXAMPLE

#define UART_BAUDRATE                                   115200

#define ADRV9025_DEVICE_CLK_KHZ                         245760
#define ADRV9025_LANE_RATE_KHZ				16220160

#define ADRV9025_TX_JESD_OCTETS_PER_FRAME		8 // F 4
#define ADRV9025_TX_JESD_FRAMES_PER_MULTIFRAME		32// K 64
#define ADRV9025_TX_JESD_CONVS_PER_DEVICE		16
#define ADRV9025_TX_JESD_CONV_RESOLUTION		16
#define ADRV9025_TX_JESD_BITS_PER_SAMPLE		16
#define ADRV9025_TX_JESD_HIGH_DENSITY			1
#define ADRV9025_TX_JESD_CTRL_BITS_PER_SAMPLE		0
#define ADRV9025_TX_JESD_SUBCLASS			1

#define ADRV9025_RX_JESD_OCTETS_PER_FRAME		4
#define ADRV9025_RX_JESD_FRAMES_PER_MULTIFRAME		64
#define ADRV9025_RX_JESD_SUBCLASS		        1

#endif /* APP_CONFIG_H_ */
