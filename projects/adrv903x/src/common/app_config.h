/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  Application configuration parameters for ADRV903X project.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders. This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

/******************************************************************************/
/* UART baud rate for IIO example                                             */
/******************************************************************************/
#define UART_BAUDRATE				921600

/******************************************************************************/
/* Clock and lane rate configuration                                          */
/* TODO: update from selected profile in profiles/ directory of API package  */
/******************************************************************************/
#define ADRV903X_DEVICE_CLK_KHZ			245760
#define ADRV903X_LANE_RATE_KHZ			16220160

/******************************************************************************/
/* JESD204 framer/deframer parameters                                        */
/* TODO: update from selected profile                                        */
/******************************************************************************/
#define ADRV903X_TX_JESD_OCTETS_PER_FRAME	4
#define ADRV903X_TX_JESD_FRAMES_PER_MULTIFRAME	64
#define ADRV903X_RX_JESD_OCTETS_PER_FRAME	4
#define ADRV903X_RX_JESD_FRAMES_PER_MULTIFRAME	64

#endif /* APP_CONFIG_H_ */
