/***************************************************************************//**
 *   @file   mux_board.h
 *   @brief  Cross-point switch handling code.
 *   @author Kister Jimenez (kister.jimenez@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#ifndef MUXBOARD_H_
#define MUXBOARD_H_
#include <stdint.h>
#include "no_os_i2c.h"
#include "ad5940.h"
#define ADG2128_MUX_SIZE 16
#define MUXBOARD_SIZE ADG2128_MUX_SIZE
enum muxbrd_variant {
	ADG2128MUXBOARD,
	ADG731MUXBOARD,
};

//Structure for electrode combinations. Each variable corresponds to each of the 4-pin measurement electrodes.
//example, if F_plus=0,  S_plus=1, S_minus=2, F_minus=3,means that F+, S+, S-, and F-are connected to electrodes
//0, 1, 2, and 3 respectively

struct electrode_combo {
	uint16_t F_plus;
	uint16_t S_plus;
	uint16_t S_minus;
	uint16_t F_minus;
};

void setMuxSwitch(struct no_os_i2c_desc *i2c, struct ad5940_dev *dev,
		  struct electrode_combo sw, uint16_t nElCount);

#endif /* MUXBOARD_H_ */
