/*
 * myk_clock_init.c
 *
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#ifndef MYK_CLOCK_INIT_C_
#define MYK_CLOCK_INIT_C_
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "common.h"

#include "ad9528.h"
#include "t_ad9528.h"


static ad9528pll1Settings_t clockPll1Settings =
{
		30720000,
		1,
		3,
		0,
		1,
		0,
		122880000,
		2,
		4
};

static ad9528pll2Settings_t clockPll2Settings =
{
		3,
		30
};

static ad9528outputSettings_t clockOutputSettings =
{
		53237,
		{0,0,0,2,0,0,0,0,0,0,0,0,2,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{10,10,10,10,10,10,10,10,10,10,10,10,10,10},
		{122880000, 122880000, 122880000, 122880000, 122880000, 122880000, 122880000, 122880000, 122880000, 122880000, 122880000, 122880000, 122880000, 122880000}
};

static ad9528sysrefSettings_t clockSysrefSettings =
{
		0,
		2,
		0,
		0,
		0,
		0,
		512
};

static spiSettings_t clockSpiSettings =
{
        2, //chip select Index
        0, //Write bit polarity
        1, //16bit instruction word
        1, //MSB first
        0, //Clock phase
        0, //Clock polarity
        0,//uint8_t enSpiStreaming;
        1,//uint8_t autoIncAddrUp;
        1 //uint8_t fourWireMode;
};

ad9528Device_t ad9528Clock =
{
	&clockSpiSettings,
	&clockPll1Settings,
	&clockPll2Settings,
	&clockOutputSettings,
	&clockSysrefSettings
};

#endif /* MYK_CLOCK_INIT_C_ */
