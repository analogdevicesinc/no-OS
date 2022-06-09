/*
 * MuxBoard.h
 *
 *  Created on: Mar 4, 2019
 *      Author: kjimenez
 */

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
