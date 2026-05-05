/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  Application configuration parameters for K project.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#define NO_OS_LWIP_NETWORKING

/******************************************************************************/
/* Clock and lane rate configuration                                          */
/*                                                                            */
/* From DTS: JESD204C (64B66B), 6.6 Gbps lane rate                           */
/*   device_clk = 100 MHz  (fixed_100)                                       */
/*   refclk     = 200 MHz  (si5391 OUT9)                                     */
/*   sampl_clk  = 400 MHz  (fixed_400)                                       */
/******************************************************************************/
#define AD9081K_DEVICE_CLK_KHZ			100000
#define AD9081K_LANE_RATE_KHZ			6600000
#define AD9081K_SAMPLE_CLK_KHZ			1600000

/******************************************************************************/
/* JESD204 framer/deframer parameters                                         */
/******************************************************************************/
#define AD9081K_TX_JESD_OCTETS_PER_FRAME	1
#define AD9081K_TX_JESD_FRAMES_PER_MULTIFRAME	256
#define AD9081K_RX_JESD_OCTETS_PER_FRAME	1
#define AD9081K_RX_JESD_FRAMES_PER_MULTIFRAME	256

#define AD9081K_TX_JESD_SUBCLASS		0
#define AD9081K_TX_JESD_CONVS_PER_DEVICE	8
#define AD9081K_TX_JESD_CONV_RESOLUTION		16
#define AD9081K_TX_JESD_BITS_PER_SAMPLE		16
#define AD9081K_TX_JESD_HIGH_DENSITY		0
#define AD9081K_TX_JESD_CTRL_BITS_PER_SAMPLE	0
#define AD9081K_TX_JESD_LANES_PER_DEVICE	8
#define AD9081K_TX_JESD_SAMPLES_PER_FRAME	4

#define AD9081K_RX_JESD_SUBCLASS		0

/******************************************************************************/
/* ADXCVR reference rate                                                      */
/* From DTS: si5391 OUT9 = 200 MHz                                            */
/******************************************************************************/
#define AD9081K_ADXCVR_REF_RATE_KHZ		200000

/******************************************************************************/
/* Si5391 Clock Synthesizer configuration                                     */
/* From DTS: I2C addr 0x77, 54 MHz xtal input, PLL M = 8000/30               */
/******************************************************************************/
#define SI5391_INPUT_CLK_FREQ_HZ		54000000ULL
#define SI5391_ACTIVE_INPUT			3	/* XTAL on XA/XB pins */
#define SI5391_I2C_ADDR				0x77
/* PLL M: 54 MHz × (8000/30) = 14.4 GHz VCO */
#define SI5391_PLL_M_NUM_VAL			8000
#define SI5391_PLL_M_DEN_VAL			30

/******************************************************************************/
/* UART                                                                       */
/******************************************************************************/
#define UART_BAUDRATE				921600

#endif /* APP_CONFIG_H_ */
