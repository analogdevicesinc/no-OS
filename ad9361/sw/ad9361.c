/*
 * AD9361 Agile RF Transceiver
 *
 * Copyright 2013 Analog Devices Inc.
 *
 * Licensed under the GPL-2.
 */
#define DEBUG
//#define _DEBUG

#include "ad9361.h"
#include "linux.h"
#include "no_os_port.h"

#define SYNTH_LUT_SIZE	53

const struct SynthLUT SynthLUT_FDD[LUT_FTDD_ENT][SYNTH_LUT_SIZE] = {
{
	{12605, 10, 0, 4, 0, 15, 8, 8, 12, 3, 14, 15, 11}, /* 40 MHz */
	{12245, 10, 0, 4, 0, 15, 8, 9, 12, 3, 14, 15, 11},
	{11906, 10, 0, 4, 0, 15, 8, 9, 12, 3, 14, 15, 11},
	{11588, 10, 0, 4, 0, 15, 8, 10, 12, 3, 14, 15, 11},
	{11288, 10, 0, 4, 0, 15, 8, 11, 12, 3, 14, 15, 11},
	{11007, 10, 0, 4, 0, 15, 8, 11, 12, 3, 14, 15, 11},
	{10742, 10, 0, 4, 0, 14, 8, 12, 12, 3, 14, 15, 11},
	{10492, 10, 0, 5, 1, 14, 9, 13, 12, 3, 14, 15, 11},
	{10258, 10, 0, 5, 1, 14, 9, 13, 12, 3, 14, 15, 11},
	{10036, 10, 0, 5, 1, 14, 9, 14, 12, 3, 14, 15, 11},
	{9827, 10, 0, 5, 1, 14, 9, 15, 12, 3, 14, 15, 11},
	{9631, 10, 0, 5, 1, 14, 9, 15, 12, 3, 14, 15, 11},
	{9445, 10, 0, 5, 1, 14, 9, 16, 12, 3, 14, 15, 11},
	{9269, 10, 0, 5, 1, 14, 9, 17, 12, 3, 14, 15, 11},
	{9103, 10, 0, 5, 1, 14, 9, 17, 12, 3, 14, 15, 11},
	{8946, 10, 0, 5, 1, 14, 9, 18, 12, 3, 14, 15, 11},
	{8797, 10, 1, 6, 1, 15, 11, 13, 12, 3, 14, 15, 11},
	{8655, 10, 1, 6, 1, 15, 11, 14, 12, 3, 14, 15, 11},
	{8520, 10, 1, 6, 1, 15, 11, 14, 12, 3, 14, 15, 11},
	{8392, 10, 1, 6, 1, 15, 11, 15, 12, 3, 14, 15, 11},
	{8269, 10, 1, 6, 1, 15, 11, 15, 12, 3, 14, 15, 11},
	{8153, 10, 1, 6, 1, 15, 11, 16, 12, 3, 14, 15, 11},
	{8041, 10, 1, 6, 1, 15, 11, 16, 12, 3, 14, 15, 11},
	{7934, 10, 1, 6, 1, 15, 11, 17, 12, 3, 14, 15, 11},
	{7831, 10, 1, 6, 1, 15, 11, 17, 12, 3, 14, 15, 11},
	{7733, 10, 1, 6, 1, 15, 11, 17, 12, 3, 14, 15, 11},
	{7638, 10, 1, 6, 1, 15, 11, 18, 12, 3, 14, 15, 11},
	{7547, 10, 1, 6, 1, 15, 11, 18, 12, 3, 14, 15, 11},
	{7459, 10, 1, 6, 1, 15, 11, 19, 12, 3, 14, 15, 11},
	{7374, 10, 1, 7, 2, 15, 12, 19, 12, 3, 14, 15, 11},
	{7291, 10, 1, 7, 2, 15, 12, 20, 12, 3, 14, 15, 11},
	{7212, 10, 1, 7, 2, 15, 12, 20, 12, 3, 14, 15, 11},
	{7135, 10, 1, 7, 2, 15, 14, 21, 12, 3, 14, 15, 11},
	{7061, 10, 1, 7, 2, 15, 14, 21, 12, 3, 14, 15, 11},
	{6988, 10, 1, 7, 2, 15, 14, 22, 12, 3, 14, 15, 11},
	{6918, 10, 1, 7, 2, 15, 14, 22, 12, 3, 14, 15, 11},
	{6850, 10, 1, 7, 2, 15, 14, 23, 12, 3, 14, 15, 11},
	{6784, 10, 1, 7, 2, 15, 14, 23, 12, 3, 14, 15, 11},
	{6720, 10, 1, 7, 2, 15, 14, 24, 12, 3, 14, 15, 11},
	{6658, 10, 1, 7, 2, 15, 14, 24, 12, 3, 14, 15, 11},
	{6597, 10, 1, 7, 2, 15, 14, 25, 12, 3, 14, 15, 11},
	{6539, 10, 1, 7, 2, 15, 14, 25, 12, 3, 14, 15, 11},
	{6482, 10, 1, 7, 2, 15, 14, 26, 12, 3, 14, 15, 11},
	{6427, 10, 1, 7, 2, 15, 14, 26, 12, 3, 14, 15, 11},
	{6373, 10, 3, 7, 3, 15, 12, 17, 12, 3, 14, 15, 11},
	{6321, 10, 3, 7, 3, 15, 12, 17, 12, 3, 14, 15, 11},
	{6270, 10, 3, 7, 3, 15, 12, 17, 12, 3, 14, 15, 11},
	{6222, 10, 3, 7, 3, 15, 12, 18, 12, 3, 14, 15, 11},
	{6174, 10, 3, 7, 3, 15, 12, 18, 12, 3, 14, 15, 11},
	{6128, 10, 3, 7, 3, 15, 12, 18, 12, 3, 14, 15, 11},
	{6083, 10, 3, 7, 3, 15, 12, 18, 12, 3, 14, 15, 11},
	{6040, 10, 3, 7, 3, 15, 12, 19, 12, 3, 14, 15, 11},
	{5997, 10, 3, 7, 3, 15, 12, 19, 12, 3, 14, 15, 11},
}, {
	{12605, 10, 0, 4, 0, 15, 8, 10, 15, 4, 13, 15, 10},  /* 60 MHz */
	{12245, 10, 0, 4, 0, 15, 8, 11, 15, 4, 13, 15, 10},
	{11906, 10, 0, 4, 0, 15, 8, 11, 15, 4, 13, 15, 10},
	{11588, 10, 0, 4, 0, 15, 8, 12, 15, 4, 13, 15, 10},
	{11288, 10, 0, 4, 0, 15, 8, 13, 15, 4, 13, 15, 10},
	{11007, 10, 0, 4, 0, 14, 8, 14, 15, 4, 13, 15, 10},
	{10742, 10, 0, 4, 0, 14, 8, 15, 15, 4, 13, 15, 10},
	{10492, 10, 0, 5, 1, 14, 9, 15, 15, 4, 13, 15, 10},
	{10258, 10, 0, 5, 1, 14, 9, 16, 15, 4, 13, 15, 10},
	{10036, 10, 0, 5, 1, 14, 9, 17, 15, 4, 13, 15, 10},
	{9827, 10, 0, 5, 1, 14, 9, 18, 15, 4, 13, 15, 10},
	{9631, 10, 0, 5, 1, 14, 9, 19, 15, 4, 13, 15, 10},
	{9445, 10, 0, 5, 1, 14, 9, 19, 15, 4, 13, 15, 10},
	{9269, 10, 0, 5, 1, 14, 9, 20, 15, 4, 13, 15, 10},
	{9103, 10, 0, 5, 1, 13, 9, 21, 15, 4, 13, 15, 10},
	{8946, 10, 0, 5, 1, 13, 9, 22, 15, 4, 13, 15, 10},
	{8797, 10, 1, 6, 1, 15, 11, 16, 15, 4, 13, 15, 10},
	{8655, 10, 1, 6, 1, 15, 11, 17, 15, 4, 13, 15, 10},
	{8520, 10, 1, 6, 1, 15, 11, 17, 15, 4, 13, 15, 10},
	{8392, 10, 1, 6, 1, 15, 11, 18, 15, 4, 13, 15, 10},
	{8269, 10, 1, 6, 1, 15, 11, 18, 15, 4, 13, 15, 10},
	{8153, 10, 1, 6, 1, 15, 11, 19, 15, 4, 13, 15, 10},
	{8041, 10, 1, 6, 1, 15, 11, 19, 15, 4, 13, 15, 10},
	{7934, 10, 1, 6, 1, 15, 11, 20, 15, 4, 13, 15, 10},
	{7831, 10, 1, 6, 1, 15, 11, 21, 15, 4, 13, 15, 10},
	{7733, 10, 1, 6, 1, 15, 11, 21, 15, 4, 13, 15, 10},
	{7638, 10, 1, 6, 1, 15, 11, 22, 15, 4, 13, 15, 10},
	{7547, 10, 1, 6, 1, 15, 11, 22, 15, 4, 13, 15, 10},
	{7459, 10, 1, 6, 1, 15, 11, 23, 15, 4, 13, 15, 10},
	{7374, 10, 1, 7, 2, 15, 12, 23, 15, 4, 13, 15, 10},
	{7291, 10, 1, 7, 2, 15, 12, 24, 15, 4, 13, 15, 10},
	{7212, 10, 1, 7, 2, 15, 12, 25, 15, 4, 13, 15, 10},
	{7135, 10, 1, 7, 2, 15, 14, 25, 15, 4, 13, 15, 10},
	{7061, 10, 1, 7, 2, 15, 14, 26, 15, 4, 13, 15, 10},
	{6988, 10, 1, 7, 2, 15, 14, 26, 15, 4, 13, 15, 10},
	{6918, 10, 1, 7, 2, 15, 14, 27, 15, 4, 13, 15, 10},
	{6850, 10, 1, 7, 2, 15, 14, 27, 15, 4, 13, 15, 10},
	{6784, 10, 1, 7, 2, 15, 14, 28, 15, 4, 13, 15, 10},
	{6720, 10, 1, 7, 2, 15, 14, 29, 15, 4, 13, 15, 10},
	{6658, 10, 1, 7, 2, 15, 14, 29, 15, 4, 13, 15, 10},
	{6597, 10, 1, 7, 2, 15, 14, 30, 15, 4, 13, 15, 10},
	{6539, 10, 1, 7, 2, 15, 14, 30, 15, 4, 13, 15, 10},
	{6482, 10, 1, 7, 2, 15, 14, 31, 15, 4, 13, 15, 10},
	{6427, 10, 1, 7, 2, 15, 14, 32, 15, 4, 13, 15, 10},
	{6373, 10, 3, 7, 3, 15, 12, 20, 15, 4, 13, 15, 10},
	{6321, 10, 3, 7, 3, 15, 12, 21, 15, 4, 13, 15, 10},
	{6270, 10, 3, 7, 3, 15, 12, 21, 15, 4, 13, 15, 10},
	{6222, 10, 3, 7, 3, 15, 12, 21, 15, 4, 13, 15, 10},
	{6174, 10, 3, 7, 3, 15, 12, 22, 15, 4, 13, 15, 10},
	{6128, 10, 3, 7, 3, 15, 12, 22, 15, 4, 13, 15, 10},
	{6083, 10, 3, 7, 3, 15, 12, 22, 15, 4, 13, 15, 10},
	{6040, 10, 3, 7, 3, 15, 12, 23, 15, 4, 13, 15, 10},
	{5997, 10, 3, 7, 3, 15, 12, 23, 15, 4, 13, 15, 10},
}, {
	{12605, 10, 0, 4, 0, 15, 8, 8, 13, 4, 13, 15, 9},   /* 80 MHz */
	{12245, 10, 0, 4, 0, 15, 8, 9, 13, 4, 13, 15, 9},
	{11906, 10, 0, 4, 0, 15, 8, 10, 13, 4, 13, 15, 9},
	{11588, 10, 0, 4, 0, 15, 8, 11, 13, 4, 13, 15, 9},
	{11288, 10, 0, 4, 0, 15, 8, 11, 13, 4, 13, 15, 9},
	{11007, 10, 0, 4, 0, 14, 8, 12, 13, 4, 13, 15, 9},
	{10742, 10, 0, 4, 0, 14, 8, 13, 13, 4, 13, 15, 9},
	{10492, 10, 0, 5, 1, 14, 9, 13, 13, 4, 13, 15, 9},
	{10258, 10, 0, 5, 1, 14, 9, 14, 13, 4, 13, 15, 9},
	{10036, 10, 0, 5, 1, 14, 9, 15, 13, 4, 13, 15, 9},
	{9827, 10, 0, 5, 1, 14, 9, 15, 13, 4, 13, 15, 9},
	{9631, 10, 0, 5, 1, 13, 9, 16, 13, 4, 13, 15, 9},
	{9445, 10, 0, 5, 1, 13, 9, 17, 13, 4, 13, 15, 9},
	{9269, 10, 0, 5, 1, 13, 9, 18, 13, 4, 13, 15, 9},
	{9103, 10, 0, 5, 1, 13, 9, 18, 13, 4, 13, 15, 9},
	{8946, 10, 0, 5, 1, 13, 9, 19, 13, 4, 13, 15, 9},
	{8797, 10, 1, 6, 1, 15, 11, 14, 13, 4, 13, 15, 9},
	{8655, 10, 1, 6, 1, 15, 11, 14, 13, 4, 13, 15, 9},
	{8520, 10, 1, 6, 1, 15, 11, 15, 13, 4, 13, 15, 9},
	{8392, 10, 1, 6, 1, 15, 11, 15, 13, 4, 13, 15, 9},
	{8269, 10, 1, 6, 1, 15, 11, 16, 13, 4, 13, 15, 9},
	{8153, 10, 1, 6, 1, 15, 11, 16, 13, 4, 13, 15, 9},
	{8041, 10, 1, 6, 1, 15, 11, 17, 13, 4, 13, 15, 9},
	{7934, 10, 1, 6, 1, 15, 11, 17, 13, 4, 13, 15, 9},
	{7831, 10, 1, 6, 1, 15, 11, 18, 13, 4, 13, 15, 9},
	{7733, 10, 1, 6, 1, 15, 11, 18, 13, 4, 13, 15, 9},
	{7638, 10, 1, 6, 1, 15, 11, 19, 13, 4, 13, 15, 9},
	{7547, 10, 1, 6, 1, 15, 11, 19, 13, 4, 13, 15, 9},
	{7459, 10, 1, 6, 1, 15, 11, 20, 13, 4, 13, 15, 9},
	{7374, 10, 1, 7, 2, 15, 12, 20, 13, 4, 13, 15, 9},
	{7291, 10, 1, 7, 2, 15, 12, 21, 13, 4, 13, 15, 9},
	{7212, 10, 1, 7, 2, 15, 12, 21, 13, 4, 13, 15, 9},
	{7135, 10, 1, 7, 2, 15, 14, 22, 13, 4, 13, 15, 9},
	{7061, 10, 1, 7, 2, 15, 14, 22, 13, 4, 13, 15, 9},
	{6988, 10, 1, 7, 2, 15, 14, 23, 13, 4, 13, 15, 9},
	{6918, 10, 1, 7, 2, 15, 14, 23, 13, 4, 13, 15, 9},
	{6850, 10, 1, 7, 2, 15, 14, 24, 13, 4, 13, 15, 9},
	{6784, 10, 1, 7, 2, 15, 14, 24, 13, 4, 13, 15, 9},
	{6720, 10, 1, 7, 2, 15, 14, 25, 13, 4, 13, 15, 9},
	{6658, 10, 1, 7, 2, 15, 14, 25, 13, 4, 13, 15, 9},
	{6597, 10, 1, 7, 2, 15, 14, 26, 13, 4, 13, 15, 9},
	{6539, 10, 1, 7, 2, 15, 14, 26, 13, 4, 13, 15, 9},
	{6482, 10, 1, 7, 2, 15, 14, 27, 13, 4, 13, 15, 9},
	{6427, 10, 1, 7, 2, 15, 14, 27, 13, 4, 13, 15, 9},
	{6373, 10, 3, 7, 3, 15, 12, 18, 13, 4, 13, 15, 9},
	{6321, 10, 3, 7, 3, 15, 12, 18, 13, 4, 13, 15, 9},
	{6270, 10, 3, 7, 3, 15, 12, 18, 13, 4, 13, 15, 9},
	{6222, 10, 3, 7, 3, 15, 12, 19, 13, 4, 13, 15, 9},
	{6174, 10, 3, 7, 3, 15, 12, 19, 13, 4, 13, 15, 9},
	{6128, 10, 3, 7, 3, 15, 12, 19, 13, 4, 13, 15, 9},
	{6083, 10, 3, 7, 3, 15, 12, 19, 13, 4, 13, 15, 9},
	{6040, 10, 3, 7, 3, 15, 12, 20, 13, 4, 13, 15, 9},
	{5997, 10, 3, 7, 3, 15, 12, 20, 13, 4, 13, 15, 9},
}};

struct SynthLUT SynthLUT_TDD[LUT_FTDD_ENT][SYNTH_LUT_SIZE] = {
{
	{12605, 13, 0, 4, 0, 14, 0, 10, 12, 3, 14, 15, 11},   /* 40 MHz */
	{12245, 13, 0, 4, 0, 13, 0, 10, 12, 3, 14, 15, 11},
	{11906, 13, 0, 4, 0, 12, 0, 11, 12, 3, 14, 15, 11},
	{11588, 13, 0, 4, 0, 12, 0, 12, 12, 3, 14, 15, 11},
	{11288, 13, 0, 4, 0, 12, 0, 13, 12, 3, 14, 15, 11},
	{11007, 13, 0, 4, 0, 11, 0, 13, 12, 3, 14, 15, 11},
	{10742, 13, 0, 4, 0, 11, 0, 14, 12, 3, 14, 15, 11},
	{10492, 13, 0, 5, 1, 11, 0, 15, 12, 3, 14, 15, 11},
	{10258, 13, 0, 5, 1, 10, 0, 16, 12, 3, 14, 15, 11},
	{10036, 13, 0, 5, 1, 10, 0, 17, 12, 3, 14, 15, 11},
	{9827, 13, 0, 5, 1, 10, 0, 17, 12, 3, 14, 15, 11},
	{9631, 13, 0, 5, 1, 10, 0, 18, 12, 3, 14, 15, 11},
	{9445, 13, 0, 5, 1, 9, 0, 19, 12, 3, 14, 15, 11},
	{9269, 10, 0, 6, 1, 12, 0, 17, 12, 3, 14, 15, 11},
	{9103, 10, 0, 6, 1, 12, 0, 17, 12, 3, 14, 15, 11},
	{8946, 10, 0, 6, 1, 11, 0, 18, 12, 3, 14, 15, 11},
	{8797, 10, 0, 6, 1, 11, 0, 19, 12, 3, 14, 15, 11},
	{8655, 10, 0, 6, 1, 11, 0, 19, 12, 3, 14, 15, 11},
	{8520, 10, 0, 6, 1, 11, 0, 20, 12, 3, 14, 15, 11},
	{8392, 10, 0, 6, 1, 11, 0, 21, 12, 3, 14, 15, 11},
	{8269, 10, 0, 6, 1, 11, 0, 21, 12, 3, 14, 15, 11},
	{8153, 10, 0, 6, 1, 11, 0, 22, 12, 3, 14, 15, 11},
	{8041, 10, 0, 6, 1, 11, 0, 23, 12, 3, 14, 15, 11},
	{7934, 10, 0, 6, 1, 11, 0, 23, 12, 3, 14, 15, 11},
	{7831, 10, 0, 6, 1, 10, 0, 24, 12, 3, 14, 15, 11},
	{7733, 10, 0, 6, 1, 10, 0, 25, 12, 3, 14, 15, 11},
	{7638, 10, 0, 6, 1, 10, 0, 25, 12, 3, 14, 15, 11},
	{7547, 10, 0, 6, 1, 10, 0, 26, 12, 3, 14, 15, 11},
	{7459, 10, 0, 6, 1, 10, 0, 27, 12, 3, 14, 15, 11},
	{7374, 10, 0, 7, 2, 10, 0, 27, 12, 3, 14, 15, 11},
	{7291, 10, 0, 7, 2, 9, 0, 28, 12, 3, 14, 15, 11},
	{7212, 10, 0, 7, 2, 9, 0, 29, 12, 3, 14, 15, 11},
	{7135, 10, 0, 7, 2, 9, 0, 29, 12, 3, 14, 15, 11},
	{7061, 10, 0, 7, 2, 9, 0, 30, 12, 3, 14, 15, 11},
	{6988, 10, 0, 7, 2, 9, 0, 31, 12, 3, 14, 15, 11},
	{6918, 10, 0, 7, 2, 9, 0, 31, 12, 3, 14, 15, 11},
	{6850, 10, 0, 7, 2, 8, 0, 32, 12, 3, 14, 15, 11},
	{6784, 10, 0, 7, 2, 8, 0, 33, 12, 3, 14, 15, 11},
	{6720, 10, 0, 7, 2, 8, 0, 33, 12, 3, 14, 15, 11},
	{6658, 10, 0, 7, 2, 8, 0, 34, 12, 3, 14, 15, 11},
	{6597, 10, 0, 7, 2, 8, 0, 35, 12, 3, 14, 15, 11},
	{6539, 10, 0, 7, 2, 8, 0, 35, 12, 3, 14, 15, 11},
	{6482, 10, 0, 7, 2, 8, 0, 36, 12, 3, 14, 15, 11},
	{6427, 10, 0, 7, 2, 7, 0, 37, 12, 3, 14, 15, 11},
	{6373, 10, 0, 7, 3, 7, 0, 37, 12, 3, 14, 15, 11},
	{6321, 10, 0, 7, 3, 7, 0, 38, 12, 3, 14, 15, 11},
	{6270, 10, 0, 7, 3, 7, 0, 39, 12, 3, 14, 15, 11},
	{6222, 10, 0, 7, 3, 7, 0, 39, 12, 3, 14, 15, 11},
	{6174, 10, 0, 7, 3, 7, 0, 40, 12, 3, 14, 15, 11},
	{6128, 10, 0, 7, 3, 7, 0, 41, 12, 3, 14, 15, 11},
	{6083, 10, 0, 7, 3, 7, 0, 41, 12, 3, 14, 15, 11},
	{6040, 10, 0, 7, 3, 6, 0, 42, 12, 3, 14, 15, 11},
	{5997, 10, 0, 7, 3, 6, 0, 42, 12, 3, 14, 15, 11},
}, {
	{12605, 13, 0, 4, 0, 14, 0, 12, 15, 4, 13, 15, 10},   /* 60 MHz */
	{12245, 13, 0, 4, 0, 13, 0, 13, 15, 4, 13, 15, 10},
	{11906, 13, 0, 4, 0, 13, 0, 14, 15, 4, 13, 15, 10},
	{11588, 13, 0, 4, 0, 12, 0, 14, 15, 4, 13, 15, 10},
	{11288, 13, 0, 4, 0, 12, 0, 15, 15, 4, 13, 15, 10},
	{11007, 13, 0, 4, 0, 12, 0, 16, 15, 4, 13, 15, 10},
	{10742, 13, 0, 4, 0, 11, 0, 17, 15, 4, 13, 15, 10},
	{10492, 13, 0, 5, 1, 11, 0, 18, 15, 4, 13, 15, 10},
	{10258, 13, 0, 5, 1, 11, 0, 19, 15, 4, 13, 15, 10},
	{10036, 13, 0, 5, 1, 10, 0, 20, 15, 4, 13, 15, 10},
	{9827, 13, 0, 5, 1, 10, 0, 21, 15, 4, 13, 15, 10},
	{9631, 13, 0, 5, 1, 10, 0, 22, 15, 4, 13, 15, 10},
	{9445, 13, 0, 5, 1, 10, 0, 23, 15, 4, 13, 15, 10},
	{9269, 10, 0, 6, 1, 12, 0, 20, 15, 4, 13, 15, 10},
	{9103, 10, 0, 6, 1, 12, 0, 21, 15, 4, 13, 15, 10},
	{8946, 10, 0, 6, 1, 12, 0, 22, 15, 4, 13, 15, 10},
	{8797, 10, 0, 6, 1, 12, 0, 23, 15, 4, 13, 15, 10},
	{8655, 10, 0, 6, 1, 12, 0, 23, 15, 4, 13, 15, 10},
	{8520, 10, 0, 6, 1, 12, 0, 24, 15, 4, 13, 15, 10},
	{8392, 10, 0, 6, 1, 12, 0, 25, 15, 4, 13, 15, 10},
	{8269, 10, 0, 6, 1, 12, 0, 26, 15, 4, 13, 15, 10},
	{8153, 10, 0, 6, 1, 12, 0, 27, 15, 4, 13, 15, 10},
	{8041, 10, 0, 6, 1, 12, 0, 27, 15, 4, 13, 15, 10},
	{7934, 10, 0, 6, 1, 11, 0, 28, 15, 4, 13, 15, 10},
	{7831, 10, 0, 6, 1, 11, 0, 29, 15, 4, 13, 15, 10},
	{7733, 10, 0, 6, 1, 11, 0, 30, 15, 4, 13, 15, 10},
	{7638, 10, 0, 6, 1, 11, 0, 31, 15, 4, 13, 15, 10},
	{7547, 10, 0, 6, 1, 11, 0, 31, 15, 4, 13, 15, 10},
	{7459, 10, 0, 6, 1, 11, 0, 32, 15, 4, 13, 15, 10},
	{7374, 10, 0, 7, 2, 11, 0, 33, 15, 4, 13, 15, 10},
	{7291, 10, 0, 7, 2, 11, 0, 34, 15, 4, 13, 15, 10},
	{7212, 10, 0, 7, 2, 11, 0, 35, 15, 4, 13, 15, 10},
	{7135, 10, 0, 7, 2, 10, 0, 35, 15, 4, 13, 15, 10},
	{7061, 10, 0, 7, 2, 10, 0, 36, 15, 4, 13, 15, 10},
	{6988, 10, 0, 7, 2, 10, 0, 37, 15, 4, 13, 15, 10},
	{6918, 10, 0, 7, 2, 10, 0, 38, 15, 4, 13, 15, 10},
	{6850, 10, 0, 7, 2, 10, 0, 39, 15, 4, 13, 15, 10},
	{6784, 10, 0, 7, 2, 10, 0, 39, 15, 4, 13, 15, 10},
	{6720, 10, 0, 7, 2, 10, 0, 40, 15, 4, 13, 15, 10},
	{6658, 10, 0, 7, 2, 9, 0, 41, 15, 4, 13, 15, 10},
	{6597, 10, 0, 7, 2, 9, 0, 42, 15, 4, 13, 15, 10},
	{6539, 10, 0, 7, 2, 9, 0, 43, 15, 4, 13, 15, 10},
	{6482, 10, 0, 7, 2, 9, 0, 43, 15, 4, 13, 15, 10},
	{6427, 10, 0, 7, 2, 9, 0, 44, 15, 4, 13, 15, 10},
	{6373, 10, 0, 7, 3, 9, 0, 45, 15, 4, 13, 15, 10},
	{6321, 10, 0, 7, 3, 9, 0, 46, 15, 4, 13, 15, 10},
	{6270, 10, 0, 7, 3, 9, 0, 47, 15, 4, 13, 15, 10},
	{6222, 10, 0, 7, 3, 9, 0, 47, 15, 4, 13, 15, 10},
	{6174, 10, 0, 7, 3, 9, 0, 48, 15, 4, 13, 15, 10},
	{6128, 10, 0, 7, 3, 9, 0, 49, 15, 4, 13, 15, 10},
	{6083, 10, 0, 7, 3, 9, 0, 50, 15, 4, 13, 15, 10},
	{6040, 10, 0, 7, 3, 9, 0, 51, 15, 4, 13, 15, 10},
	{5997, 10, 0, 7, 3, 8, 0, 51, 15, 4, 13, 15, 10},
}, {
	{12605, 13, 0, 4, 0, 14, 0, 10, 13, 4, 13, 15, 9},   /* 80 MHz */
	{12245, 13, 0, 4, 0, 13, 0, 11, 13, 4, 13, 15, 9},
	{11906, 13, 0, 4, 0, 12, 0, 12, 13, 4, 13, 15, 9},
	{11588, 13, 0, 4, 0, 12, 0, 13, 13, 4, 13, 15, 9},
	{11288, 13, 0, 4, 0, 12, 0, 13, 13, 4, 13, 15, 9},
	{11007, 13, 0, 4, 0, 11, 0, 14, 13, 4, 13, 15, 9},
	{10742, 13, 0, 4, 0, 11, 0, 15, 13, 4, 13, 15, 9},
	{10492, 13, 0, 5, 1, 11, 0, 16, 13, 4, 13, 15, 9},
	{10258, 13, 0, 5, 1, 10, 0, 17, 13, 4, 13, 15, 9},
	{10036, 13, 0, 5, 1, 10, 0, 17, 13, 4, 13, 15, 9},
	{9827, 13, 0, 5, 1, 10, 0, 18, 13, 4, 13, 15, 9},
	{9631, 13, 0, 5, 1, 10, 0, 19, 13, 4, 13, 15, 9},
	{9445, 13, 0, 5, 1, 9, 0, 20, 13, 4, 13, 15, 9},
	{9269, 10, 0, 6, 1, 12, 0, 18, 13, 4, 13, 15, 9},
	{9103, 10, 0, 6, 1, 12, 0, 18, 13, 4, 13, 15, 9},
	{8946, 10, 0, 6, 1, 11, 0, 19, 13, 4, 13, 15, 9},
	{8797, 10, 0, 6, 1, 11, 0, 20, 13, 4, 13, 15, 9},
	{8655, 10, 0, 6, 1, 11, 0, 20, 13, 4, 13, 15, 9},
	{8520, 10, 0, 6, 1, 11, 0, 21, 13, 4, 13, 15, 9},
	{8392, 10, 0, 6, 1, 11, 0, 22, 13, 4, 13, 15, 9},
	{8269, 10, 0, 6, 1, 11, 0, 22, 13, 4, 13, 15, 9},
	{8153, 10, 0, 6, 1, 11, 0, 23, 13, 4, 13, 15, 9},
	{8041, 10, 0, 6, 1, 11, 0, 24, 13, 4, 13, 15, 9},
	{7934, 10, 0, 6, 1, 11, 0, 25, 13, 4, 13, 15, 9},
	{7831, 10, 0, 6, 1, 10, 0, 25, 13, 4, 13, 15, 9},
	{7733, 10, 0, 6, 1, 10, 0, 26, 13, 4, 13, 15, 9},
	{7638, 10, 0, 6, 1, 10, 0, 27, 13, 4, 13, 15, 9},
	{7547, 10, 0, 6, 1, 10, 0, 27, 13, 4, 13, 15, 9},
	{7459, 10, 0, 6, 1, 10, 0, 28, 13, 4, 13, 15, 9},
	{7374, 10, 0, 7, 2, 10, 0, 29, 13, 4, 13, 15, 9},
	{7291, 10, 0, 7, 2, 9, 0, 29, 13, 4, 13, 15, 9},
	{7212, 10, 0, 7, 2, 9, 0, 30, 13, 4, 13, 15, 9},
	{7135, 10, 0, 7, 2, 9, 0, 31, 13, 4, 13, 15, 9},
	{7061, 10, 0, 7, 2, 9, 0, 32, 13, 4, 13, 15, 9},
	{6988, 10, 0, 7, 2, 9, 0, 32, 13, 4, 13, 15, 9},
	{6918, 10, 0, 7, 2, 9, 0, 33, 13, 4, 13, 15, 9},
	{6850, 10, 0, 7, 2, 8, 0, 34, 13, 4, 13, 15, 9},
	{6784, 10, 0, 7, 2, 8, 0, 34, 13, 4, 13, 15, 9},
	{6720, 10, 0, 7, 2, 8, 0, 35, 13, 4, 13, 15, 9},
	{6658, 10, 0, 7, 2, 8, 0, 36, 13, 4, 13, 15, 9},
	{6597, 10, 0, 7, 2, 8, 0, 36, 13, 4, 13, 15, 9},
	{6539, 10, 0, 7, 2, 8, 0, 37, 13, 4, 13, 15, 9},
	{6482, 10, 0, 7, 2, 8, 0, 38, 13, 4, 13, 15, 9},
	{6427, 10, 0, 7, 2, 7, 0, 39, 13, 4, 13, 15, 9},
	{6373, 10, 0, 7, 3, 7, 0, 39, 13, 4, 13, 15, 9},
	{6321, 10, 0, 7, 3, 7, 0, 40, 13, 4, 13, 15, 9},
	{6270, 10, 0, 7, 3, 7, 0, 41, 13, 4, 13, 15, 9},
	{6222, 10, 0, 7, 3, 7, 0, 41, 13, 4, 13, 15, 9},
	{6174, 10, 0, 7, 3, 7, 0, 42, 13, 4, 13, 15, 9},
	{6128, 10, 0, 7, 3, 7, 0, 43, 13, 4, 13, 15, 9},
	{6083, 10, 0, 7, 3, 7, 0, 44, 13, 4, 13, 15, 9},
	{6040, 10, 0, 7, 3, 6, 0, 44, 13, 4, 13, 15, 9},
	{5997, 10, 0, 7, 3, 6, 0, 44, 13, 4, 13, 15, 9},
}};

/* Rx Gain Tables */

#define SIZE_FULL_TABLE		77

const u8 full_gain_table[RXGAIN_TBLS_END][SIZE_FULL_TABLE][3] =
{{  /* 800 MHz */
	{0x00, 0x00, 0x20}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
	{0x00, 0x01, 0x00}, {0x00, 0x02, 0x00}, {0x00, 0x03, 0x00},
	{0x00, 0x04, 0x00}, {0x00, 0x05, 0x00}, {0x01, 0x03, 0x20},
	{0x01, 0x04, 0x00}, {0x01, 0x05, 0x00}, {0x01, 0x06, 0x00},
	{0x01, 0x07, 0x00}, {0x01, 0x08, 0x00}, {0x01, 0x09, 0x00},
	{0x01, 0x0A, 0x00}, {0x01, 0x0B, 0x00}, {0x01, 0x0C, 0x00},
	{0x01, 0x0D, 0x00}, {0x01, 0x0E, 0x00}, {0x02, 0x09, 0x20},
	{0x02, 0x0A, 0x00}, {0x02, 0x0B, 0x00}, {0x02, 0x0C, 0x00},
	{0x02, 0x0D, 0x00}, {0x02, 0x0E, 0x00}, {0x02, 0x0F, 0x00},
	{0x02, 0x10, 0x00}, {0x02, 0x2B, 0x20}, {0x02, 0x2C, 0x00},
	{0x04, 0x27, 0x20}, {0x04, 0x28, 0x00}, {0x04, 0x29, 0x00},
	{0x04, 0x2A, 0x00}, {0x04, 0x2B, 0x00}, {0x24, 0x21, 0x20},
	{0x24, 0x22, 0x00}, {0x44, 0x20, 0x20}, {0x44, 0x21, 0x00},
	{0x44, 0x22, 0x00}, {0x44, 0x23, 0x00}, {0x44, 0x24, 0x00},
	{0x44, 0x25, 0x00}, {0x44, 0x26, 0x00}, {0x44, 0x27, 0x00},
	{0x44, 0x28, 0x00}, {0x44, 0x29, 0x00}, {0x44, 0x2A, 0x00},
	{0x44, 0x2B, 0x00}, {0x44, 0x2C, 0x00}, {0x44, 0x2D, 0x00},
	{0x44, 0x2E, 0x00}, {0x44, 0x2F, 0x00}, {0x44, 0x30, 0x00},
	{0x44, 0x31, 0x00}, {0x64, 0x2E, 0x20}, {0x64, 0x2F, 0x00},
	{0x64, 0x30, 0x00}, {0x64, 0x31, 0x00}, {0x64, 0x32, 0x00},
	{0x64, 0x33, 0x00}, {0x64, 0x34, 0x00}, {0x64, 0x35, 0x00},
	{0x64, 0x36, 0x00}, {0x64, 0x37, 0x00}, {0x64, 0x38, 0x00},
	{0x65, 0x38, 0x20}, {0x66, 0x38, 0x20}, {0x67, 0x38, 0x20},
	{0x68, 0x38, 0x20}, {0x69, 0x38, 0x20}, {0x6A, 0x38, 0x20},
	{0x6B, 0x38, 0x20}, {0x6C, 0x38, 0x20}, {0x6D, 0x38, 0x20},
	{0x6E, 0x38, 0x20}, {0x6F, 0x38, 0x20}
},{  /* 2300 MHz */
	{0x00, 0x00, 0x20}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
	{0x00, 0x01, 0x00}, {0x00, 0x02, 0x00}, {0x00, 0x03, 0x00},
	{0x00, 0x04, 0x00}, {0x00, 0x05, 0x00}, {0x01, 0x03, 0x20},
	{0x01, 0x04, 0x00}, {0x01, 0x05, 0x00}, {0x01, 0x06, 0x00},
	{0x01, 0x07, 0x00}, {0x01, 0x08, 0x00}, {0x01, 0x09, 0x00},
	{0x01, 0x0A, 0x00}, {0x01, 0x0B, 0x00}, {0x01, 0x0C, 0x00},
	{0x01, 0x0D, 0x00}, {0x01, 0x0E, 0x00}, {0x02, 0x09, 0x20},
	{0x02, 0x0A, 0x00}, {0x02, 0x0B, 0x00}, {0x02, 0x0C, 0x00},
	{0x02, 0x0D, 0x00}, {0x02, 0x0E, 0x00}, {0x02, 0x0F, 0x00},
	{0x02, 0x10, 0x00}, {0x02, 0x2B, 0x20}, {0x02, 0x2C, 0x00},
	{0x04, 0x28, 0x20}, {0x04, 0x29, 0x00}, {0x04, 0x2A, 0x00},
	{0x04, 0x2B, 0x00}, {0x24, 0x20, 0x20}, {0x24, 0x21, 0x00},
	{0x44, 0x20, 0x20}, {0x44, 0x21, 0x00}, {0x44, 0x22, 0x00},
	{0x44, 0x23, 0x00}, {0x44, 0x24, 0x00}, {0x44, 0x25, 0x00},
	{0x44, 0x26, 0x00}, {0x44, 0x27, 0x00}, {0x44, 0x28, 0x00},
	{0x44, 0x29, 0x00}, {0x44, 0x2A, 0x00}, {0x44, 0x2B, 0x00},
	{0x44, 0x2C, 0x00}, {0x44, 0x2D, 0x00}, {0x44, 0x2E, 0x00},
	{0x44, 0x2F, 0x00}, {0x44, 0x30, 0x00}, {0x44, 0x31, 0x00},
	{0x44, 0x32, 0x00}, {0x64, 0x2E, 0x20}, {0x64, 0x2F, 0x00},
	{0x64, 0x30, 0x00}, {0x64, 0x31, 0x00}, {0x64, 0x32, 0x00},
	{0x64, 0x33, 0x00}, {0x64, 0x34, 0x00}, {0x64, 0x35, 0x00},
	{0x64, 0x36, 0x00}, {0x64, 0x37, 0x00}, {0x64, 0x38, 0x00},
	{0x65, 0x38, 0x20}, {0x66, 0x38, 0x20}, {0x67, 0x38, 0x20},
	{0x68, 0x38, 0x20}, {0x69, 0x38, 0x20}, {0x6A, 0x38, 0x20},
	{0x6B, 0x38, 0x20}, {0x6C, 0x38, 0x20}, {0x6D, 0x38, 0x20},
	{0x6E, 0x38, 0x20}, {0x6F, 0x38, 0x20},
},{  /* 5500 MHz */
	{0x00, 0x00, 0x20}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x01, 0x00},
	{0x00, 0x02, 0x00}, {0x00, 0x03, 0x00}, {0x01, 0x01, 0x20},
	{0x01, 0x02, 0x00}, {0x01, 0x03, 0x00}, {0x01, 0x04, 0x20},
	{0x01, 0x05, 0x00}, {0x01, 0x06, 0x00}, {0x01, 0x07, 0x00},
	{0x01, 0x08, 0x00}, {0x01, 0x09, 0x00}, {0x01, 0x0A, 0x00},
	{0x01, 0x0B, 0x00}, {0x01, 0x0C, 0x00}, {0x02, 0x08, 0x20},
	{0x02, 0x09, 0x00}, {0x02, 0x0A, 0x00}, {0x02, 0x0B, 0x20},
	{0x02, 0x0C, 0x00}, {0x02, 0x0D, 0x00}, {0x02, 0x0E, 0x00},
	{0x02, 0x0F, 0x00}, {0x02, 0x2A, 0x20}, {0x02, 0x2B, 0x00},
	{0x04, 0x27, 0x20}, {0x04, 0x28, 0x00}, {0x04, 0x29, 0x00},
	{0x04, 0x2A, 0x00}, {0x04, 0x2B, 0x00}, {0x04, 0x2C, 0x00},
	{0x04, 0x2D, 0x00}, {0x24, 0x20, 0x20}, {0x24, 0x21, 0x00},
	{0x24, 0x22, 0x00}, {0x44, 0x20, 0x20}, {0x44, 0x21, 0x00},
	{0x44, 0x22, 0x00}, {0x44, 0x23, 0x00}, {0x44, 0x24, 0x00},
	{0x44, 0x25, 0x00}, {0x44, 0x26, 0x00}, {0x44, 0x27, 0x00},
	{0x44, 0x28, 0x00}, {0x44, 0x29, 0x00}, {0x44, 0x2A, 0x00},
	{0x44, 0x2B, 0x00}, {0x44, 0x2C, 0x00}, {0x44, 0x2D, 0x00},
	{0x44, 0x2E, 0x00}, {0x64, 0x2E, 0x20}, {0x64, 0x2F, 0x00},
	{0x64, 0x30, 0x00}, {0x64, 0x31, 0x00}, {0x64, 0x32, 0x00},
	{0x64, 0x33, 0x00}, {0x64, 0x34, 0x00}, {0x64, 0x35, 0x00},
	{0x64, 0x36, 0x00}, {0x64, 0x37, 0x00}, {0x64, 0x38, 0x00},
	{0x65, 0x38, 0x20}, {0x66, 0x38, 0x20}, {0x67, 0x38, 0x20},
	{0x68, 0x38, 0x20}, {0x69, 0x38, 0x20}, {0x6A, 0x38, 0x20},
	{0x6B, 0x38, 0x20}, {0x6C, 0x38, 0x20}, {0x6D, 0x38, 0x20},
	{0x6E, 0x38, 0x20}, {0x6F, 0x38, 0x20}
}};

#define SIZE_SPLIT_TABLE		41

const u8 split_gain_table[RXGAIN_TBLS_END][SIZE_SPLIT_TABLE][3] =
{{  /* 800 MHz */
	{0x00, 0x18, 0x20}, {0x01, 0x18, 0x20}, {0x02, 0x18, 0x20},
	{0x03, 0x18, 0x20}, {0x04, 0x18, 0x20}, {0x05, 0x18, 0x20},
	{0x06, 0x18, 0x20}, {0x07, 0x18, 0x20}, {0x08, 0x18, 0x20},
	{0x09, 0x18, 0x20}, {0x0A, 0x18, 0x20}, {0x0B, 0x18, 0x20},
	{0x0C, 0x18, 0x20}, {0x0D, 0x18, 0x20}, {0x23, 0x18, 0x20},
	{0x24, 0x18, 0x20}, {0x43, 0x18, 0x20}, {0x44, 0x18, 0x20},
	{0x45, 0x18, 0x20}, {0x46, 0x18, 0x20}, {0x47, 0x18, 0x20},
	{0x48, 0x18, 0x20}, {0x43, 0x38, 0x20}, {0x44, 0x38, 0x20},
	{0x45, 0x38, 0x20}, {0x46, 0x38, 0x20}, {0x47, 0x38, 0x20},
	{0x48, 0x38, 0x20}, {0x63, 0x38, 0x20}, {0x64, 0x38, 0x20},
	{0x65, 0x38, 0x20}, {0x66, 0x38, 0x20}, {0x67, 0x38, 0x20},
	{0x68, 0x38, 0x20}, {0x69, 0x38, 0x20}, {0x6A, 0x38, 0x20},
	{0x6B, 0x38, 0x20}, {0x6C, 0x38, 0x20}, {0x6D, 0x38, 0x20},
	{0x6E, 0x38, 0x20}, {0x6F, 0x38, 0x20},
},{  /* 2300 MHz */

	{0x00, 0x18, 0x20}, {0x01, 0x18, 0x20}, {0x02, 0x18, 0x20},
	{0x03, 0x18, 0x20}, {0x04, 0x18, 0x20}, {0x05, 0x18, 0x20},
	{0x06, 0x18, 0x20}, {0x07, 0x18, 0x20}, {0x08, 0x18, 0x20},
	{0x09, 0x18, 0x20}, {0x0A, 0x18, 0x20}, {0x0B, 0x18, 0x20},
	{0x0C, 0x18, 0x20}, {0x0D, 0x18, 0x20}, {0x23, 0x18, 0x20},
	{0x24, 0x18, 0x20}, {0x43, 0x18, 0x20}, {0x44, 0x18, 0x20},
	{0x45, 0x18, 0x20}, {0x46, 0x18, 0x20}, {0x47, 0x18, 0x20},
	{0x48, 0x18, 0x20}, {0x43, 0x38, 0x20}, {0x44, 0x38, 0x20},
	{0x45, 0x38, 0x20}, {0x46, 0x38, 0x20}, {0x47, 0x38, 0x20},
	{0x48, 0x38, 0x20}, {0x63, 0x38, 0x20}, {0x64, 0x38, 0x20},
	{0x65, 0x38, 0x20}, {0x66, 0x38, 0x20}, {0x67, 0x38, 0x20},
	{0x68, 0x38, 0x20}, {0x69, 0x38, 0x20}, {0x6A, 0x38, 0x20},
	{0x6B, 0x38, 0x20}, {0x6C, 0x38, 0x20}, {0x6D, 0x38, 0x20},
	{0x6E, 0x38, 0x20}, {0x6F, 0x38, 0x20},
},{  /* 5500 MHz */
	{0x00, 0x18, 0x20}, {0x01, 0x18, 0x20}, {0x02, 0x18, 0x20},
	{0x03, 0x18, 0x20}, {0x04, 0x18, 0x20}, {0x05, 0x18, 0x20},
	{0x06, 0x18, 0x20}, {0x07, 0x18, 0x20}, {0x08, 0x18, 0x20},
	{0x09, 0x18, 0x20}, {0x0A, 0x18, 0x20}, {0x0B, 0x18, 0x20},
	{0x0C, 0x18, 0x20}, {0x0D, 0x18, 0x20}, {0x23, 0x18, 0x20},
	{0x24, 0x18, 0x20}, {0x43, 0x18, 0x20}, {0x44, 0x18, 0x20},
	{0x45, 0x18, 0x20}, {0x46, 0x18, 0x20}, {0x47, 0x18, 0x20},
	{0x48, 0x18, 0x20}, {0x43, 0x38, 0x20}, {0x44, 0x38, 0x20},
	{0x45, 0x38, 0x20}, {0x46, 0x38, 0x20}, {0x47, 0x38, 0x20},
	{0x48, 0x38, 0x20}, {0x63, 0x38, 0x20}, {0x64, 0x38, 0x20},
	{0x65, 0x38, 0x20}, {0x66, 0x38, 0x20}, {0x67, 0x38, 0x20},
	{0x68, 0x38, 0x20}, {0x69, 0x38, 0x20}, {0x6A, 0x38, 0x20},
	{0x6B, 0x38, 0x20}, {0x6C, 0x38, 0x20}, {0x6D, 0x38, 0x20},
	{0x6E, 0x38, 0x20}, {0x6F, 0x38, 0x20},
}};

/* Mixer GM Sub-table */

const u8 gm_st_gain[16]= {0x78, 0x74, 0x70, 0x6C, 0x68, 0x64, 0x60,
			 0x5C, 0x58, 0x54, 0x50, 0x4C, 0x48, 0x30, 0x18, 0x0};
const u8 gm_st_ctrl[16]= {0x0, 0xD, 0x15, 0x1B, 0x21, 0x25, 0x29,
			 0x2C, 0x2F, 0x31, 0x33, 0x34, 0x35, 0x3A, 0x3D, 0x3E};


const s8 lna_table[] = {6, 17, 19, 25};
const s8 tia_table[] = {-6, 0};
const s8 mixer_table[] = {0, 5, 11, 16,
			17, 18, 19, 20,
			21, 22, 23, 24,
			25, 26,	27, 28};

const char ad9361_ensm_states[][10] = { "sleep", "", "", "", "", "alert", "tx", "tx flush",
	"rx", "rx_flush", "fdd", "fdd_flush"};


int ad9361_reset(struct ad9361_rf_phy *phy)
{
	if (gpio_is_valid(phy->pdata->gpio_resetb)) {
		gpio_set_value(phy->pdata->gpio_resetb, 0);
		udelay(2);
		gpio_set_value(phy->pdata->gpio_resetb, 1);
		return 1;
	}

	return -ENODEV;
}

static int ad9361_spi_readm(u32 reg, u8 *rbuf, u32 num)
{
	u8 buf[2];
	int ret;
	u16 cmd;

	if (num > MAX_MBYTE_SPI)
		return -EINVAL;

	cmd = AD_READ | AD_CNT(num) | AD_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;

	ret = spi_write_then_read(&buf[0], 2, rbuf, num);
	if (ret < 0)
		return ret;

#ifdef _DEBUG
	{
		int i;
		for (i = 0; i < num; i++)
			dev_dbg("%s: reg 0x%X val 0x%X\n",
				__func__, reg--, rbuf[i]);
	}
#endif

	return 0;
}

int ad9361_spi_read(u32 reg)
{
	u8 buf;
	int ret;

	ret = ad9361_spi_readm(reg, &buf, 1);
	if (ret < 0)
		return ret;

	return buf;
}

static int __ad9361_spi_readf(u32 reg, u32 mask, u32 offset)
{
	u8 buf;
	int ret;

	if (!mask)
		return -EINVAL;

	ret = ad9361_spi_readm(reg, &buf, 1);
	if (ret < 0)
		return ret;

	buf &= mask;
	buf >>= offset;

	return buf;
}

#define ad9361_spi_readf(reg, mask) \
	__ad9361_spi_readf(reg, mask, __ffs(mask))

int ad9361_spi_write(u32 reg, u32 val)
{
	u8 buf[3];
	int ret;
	u16 cmd;

	cmd = AD_WRITE | AD_CNT(1) | AD_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
	buf[2] = val;

	ret = spi_write_then_read(buf, 3, NULL, 0);
	if (ret < 0)
		return ret;

#ifdef _DEBUG
	dev_dbg("%s: reg 0x%X val 0x%X\n", __func__, reg, buf[2]);
#endif

	return 0;
}

static int __ad9361_spi_writef(u32 reg,
				 u32 mask, u32 offset, u32 val)
{
	u8 buf;
	int ret;

	if (!mask)
		return -EINVAL;

	ret = ad9361_spi_readm(reg, &buf, 1);
	if (ret < 0)
		return ret;

	buf &= ~mask;
	buf |= ((val << offset) & mask);

	return ad9361_spi_write(reg, buf);
}

#define ad9361_spi_writef(reg, mask, val) \
	__ad9361_spi_writef(reg, mask, __ffs(mask), val)

static int ad9361_spi_writem(u32 reg, u8 *tbuf, u32 num)
{
	u8 buf[10];
	int ret;
	u16 cmd;

	if (num > MAX_MBYTE_SPI)
		return -EINVAL;

	cmd = AD_WRITE | AD_CNT(num) | AD_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;

	memcpy(&buf[2], tbuf, num);

	ret = spi_write_then_read(buf, num + 2, NULL, 0);
	if (ret < 0)
		return ret;

#ifdef _DEBUG
	{
		int i;
		for (i = 0; i < num; i++)
			printk("%s: reg 0x%X val 0x%X\n", __func__, reg--, tbuf[i]);
	}
#endif

	return 0;
}

static int ad9361_check_cal_done(struct ad9361_rf_phy *phy, u32 reg,
				 u32 mask, bool done_state)
{
	u32 timeout = 500;
	u32 state;

	do {
		state = ad9361_spi_readf(reg, mask);
		if (state == done_state)
			return 0;

		msleep_interruptible(1);
	} while (timeout--);

	dev_err("Calibration TIMEOUT (0x%X, 0x%X)", reg, mask);

	return -ETIMEDOUT;
}

static int ad9361_run_calibration(struct ad9361_rf_phy *phy, u32 mask)
{
	int ret = ad9361_spi_write(REG_CALIBRATION_CTRL, mask);
	if (ret < 0)
		return ret;

	dev_dbg("%s: CAL Mask 0x%X\n", __func__, mask);

	return ad9361_check_cal_done(phy, REG_CALIBRATION_CTRL, mask, 0);
}

enum rx_gain_table_name ad9361_gt_tableindex(u64 freq)
{
	if (freq <= 1300000000ULL)
		return TBL_200_1300_MHZ;

	if (freq <= 4000000000ULL)
		return TBL_1300_4000_MHZ;

	return TBL_4000_6000_MHZ;
}

/* PLL operates between 47 .. 6000 MHz which is > 2^32 */

unsigned long ad9361_to_clk(u64 freq)
{
	return (unsigned long)(freq >> 1);
}

u64 ad9361_from_clk(unsigned long freq)
{
	return ((u64)freq << 1);
}

static int ad9361_load_gt(struct ad9361_rf_phy *phy, u64 freq, u32 dest)
{
	const u8 (*tab)[3];
	u32 band, index_max, i;

	dev_dbg("%s: frequency %llu\n", __func__, freq);

	band = ad9361_gt_tableindex(freq);

	dev_dbg("%s: frequency %llu (band %d)\n",
		__func__, freq, band);

	/* check if table is present */
	if (phy->current_table == band)
		return 0;

	ad9361_spi_writef(REG_AGC_CONFIG_2,
			  AGC_USE_FULL_GAIN_TABLE, !phy->pdata->split_gt);

	if (phy->pdata->split_gt) {
		tab = &split_gain_table[band][0];
		index_max = SIZE_SPLIT_TABLE;
	} else {
		tab = &full_gain_table[band][0];
		index_max = SIZE_FULL_TABLE;
	}

	ad9361_spi_write(REG_GAIN_TABLE_CONFIG, START_GAIN_TABLE_CLOCK |
			RECEIVER_SELECT(dest)); /* Start Gain Table Clock */

	for (i = 0; i < index_max; i++) {
		ad9361_spi_write(REG_GAIN_TABLE_ADDRESS, i); /* Gain Table Index */
		ad9361_spi_write(REG_GAIN_TABLE_WRITE_DATA1, tab[i][0]); /* Ext LNA, Int LNA, & Mixer Gain Word */
		ad9361_spi_write(REG_GAIN_TABLE_WRITE_DATA2, tab[i][1]); /* TIA & LPF Word */
		ad9361_spi_write(REG_GAIN_TABLE_WRITE_DATA3, tab[i][2]); /* DC Cal bit & Dig Gain Word */
		ad9361_spi_write(REG_GAIN_TABLE_CONFIG,
				START_GAIN_TABLE_CLOCK |
				WRITE_GAIN_TABLE |
				RECEIVER_SELECT(dest)); /* Gain Table Index */
		ad9361_spi_write(REG_GAIN_TABLE_READ_DATA1, 0); /* Dummy Write to delay 3 ADCCLK/16 cycles */
		ad9361_spi_write(REG_GAIN_TABLE_READ_DATA1, 0); /* Dummy Write to delay ~1u */
	}

	ad9361_spi_write(REG_GAIN_TABLE_CONFIG, START_GAIN_TABLE_CLOCK |
			RECEIVER_SELECT(dest)); /* Clear Write Bit */
	ad9361_spi_write(REG_GAIN_TABLE_READ_DATA1, 0); /* Dummy Write to delay ~1u */
	ad9361_spi_write(REG_GAIN_TABLE_READ_DATA1, 0); /* Dummy Write to delay ~1u */
	ad9361_spi_write(REG_GAIN_TABLE_CONFIG, 0); /* Stop Gain Table Clock */

	phy->current_table = band;

	return 0;
}

static int ad9361_setup_ext_lna(struct ad9361_rf_phy *phy, u32 gain_mdB,
				u32 bypass_loss_mdB)
{
	ad9361_spi_write(REG_EXT_LNA_HIGH_GAIN,
			EXT_LNA_HIGH_GAIN(gain_mdB / 500));

	return ad9361_spi_write(REG_EXT_LNA_LOW_GAIN,
			EXT_LNA_LOW_GAIN(bypass_loss_mdB / 500));
}

static int ad9361_load_mixer_gm_subtable(struct ad9361_rf_phy *phy)
{
	int i, addr;

	dev_dbg("%s\n", __func__);

	ad9361_spi_write(REG_GM_SUB_TABLE_CONFIG,
			 START_GM_SUB_TABLE_CLOCK); /* Start Clock */

	for (i = 0, addr = ARRAY_SIZE(gm_st_ctrl); i < ARRAY_SIZE(gm_st_ctrl); i++) {
		ad9361_spi_write(REG_GAIN_TABLE_ADDRESS, --addr); /* Gain Table Index */
		ad9361_spi_write(REG_GM_SUB_TABLE_BIAS_WRITE, 0); /* Bias */
		ad9361_spi_write(REG_GM_SUB_TABLE_GAIN_WRITE, gm_st_gain[i]); /* Gain */
		ad9361_spi_write(REG_GM_SUB_TABLE_CTRL_WRITE, gm_st_ctrl[i]); /* Control */
		ad9361_spi_write(REG_GM_SUB_TABLE_CONFIG,
				 WRITE_GM_SUB_TABLE | START_GM_SUB_TABLE_CLOCK); /* Write Words */
		ad9361_spi_write(REG_GM_SUB_TABLE_GAIN_READ, 0); /* Dummy Delay */
		ad9361_spi_write(REG_GM_SUB_TABLE_GAIN_READ, 0); /* Dummy Delay */
	}

	ad9361_spi_write(REG_GM_SUB_TABLE_CONFIG, START_GM_SUB_TABLE_CLOCK); /* Clear Write */
	ad9361_spi_write(REG_GM_SUB_TABLE_GAIN_READ, 0); /* Dummy Delay */
	ad9361_spi_write(REG_GM_SUB_TABLE_GAIN_READ, 0); /* Dummy Delay */
	ad9361_spi_write(REG_GM_SUB_TABLE_CONFIG, 0); /* Stop Clock */

	return 0;
}

int ad9361_set_tx_atten(struct ad9361_rf_phy *phy, u32 atten_mdb,
			       bool tx1, bool tx2)
{
	u8 buf[2];
	int ret = 0;

	dev_dbg("%s : attenuation %u mdB tx1=%d tx2=%d\n",
		__func__, atten_mdb, tx1, tx2);

	if (atten_mdb > 89750) /* 89.75 dB */
		return -EINVAL;

	atten_mdb /= 250; /* Scale to 0.25dB / LSB */

	buf[0] = atten_mdb >> 8;
	buf[1] = atten_mdb & 0xFF;

	if (tx1)
		ret = ad9361_spi_writem(REG_TX1_ATTEN_1, buf, 2);

	if (tx2)
		ret = ad9361_spi_writem(REG_TX2_ATTEN_1, buf, 2);

	ad9361_spi_writef(REG_TX2_DIG_ATTEN,
			  IMMEDIATELY_UPDATE_TPC_ATTEN, 1);

	return ret;
}

int ad9361_get_tx_atten(struct ad9361_rf_phy *phy, u32 tx_num)
{
	u8 buf[2];
	int ret = 0;
	u32 code;

	ret = ad9361_spi_readm((tx_num == 1) ?
			REG_TX1_ATTEN_1 : REG_TX2_ATTEN_1, buf, 2);

	if (ret < 0)
		return ret;

	code = (buf[0] << 8) | buf[1];

	code *= 250;

	return code;
}

u32 ad9361_rfvco_tableindex(unsigned long freq)
{
	if (freq < 50000000UL)
		return LUT_FTDD_40;

	if (freq <= 70000000UL)
		return LUT_FTDD_60;

	return LUT_FTDD_80;
}

static int ad9361_rfpll_vco_init(struct ad9361_rf_phy *phy,
				 bool tx, u64 vco_freq,
				 unsigned long ref_clk)
{
	const struct SynthLUT (*tab);
	int i = 0;
	u32 range, offs = 0;

	range = ad9361_rfvco_tableindex(ref_clk);

	dev_dbg("%s : vco_freq %llu : ref_clk %lu : range %d\n",
		__func__, vco_freq, ref_clk, range);

	do_div(vco_freq, 1000000UL); /* vco_freq in MHz */

	if (phy->pdata->fdd) {
		tab = &SynthLUT_FDD[range][0];
	} else {
		tab = &SynthLUT_TDD[range][0];

	}

	if (tx)
		offs = REG_TX_VCO_OUTPUT - REG_RX_VCO_OUTPUT;

	while (i < SYNTH_LUT_SIZE && tab[i].VCO_MHz > vco_freq)
		i++;

	dev_dbg("%s : freq %d MHz : index %d\n",
		__func__, tab[i].VCO_MHz, i);


	ad9361_spi_write(REG_RX_VCO_OUTPUT + offs,
			       VCO_OUTPUT_LEVEL(tab[i].VCO_Output_Level) |
			       PORB_VCO_LOGIC);
	ad9361_spi_writef(REG_RX_ALC_VARACTOR + offs,
			       VCO_VARACTOR(~0), tab[i].VCO_Varactor);
	ad9361_spi_write(REG_RX_VCO_BIAS_1 + offs,
			       VCO_BIAS_REF(tab[i].VCO_Bias_Ref) |
			       VCO_BIAS_TCF(tab[i].VCO_Bias_Tcf));

	ad9361_spi_write(REG_RX_FORCE_VCO_TUNE_1 + offs,
			       VCO_CAL_OFFSET(tab[i].VCO_Cal_Offset));
	ad9361_spi_write(REG_RX_VCO_VARACTOR_CTRL_1 + offs,
			       VCO_VARACTOR_REFERENCE(
			       tab[i].VCO_Varactor_Reference));

	ad9361_spi_write(REG_RX_VCO_CAL_REF, VCO_CAL_REF_TCF(0));

	ad9361_spi_write(REG_RX_VCO_VARACTOR_CTRL_0,
				VCO_VARACTOR_OFFSET(0) |
				VCO_VARACTOR_REFERENCE_TCF(7));

	ad9361_spi_writef(REG_RX_CP_CURRENT + offs, CHARGE_PUMP_CURRENT(~0),
			       tab[i].Charge_Pump_Current);
	ad9361_spi_write(REG_RX_LOOP_FILTER_1 + offs,
			 LOOP_FILTER_C2(tab[i].LF_C2) |
			 LOOP_FILTER_C1(tab[i].LF_C1));
	ad9361_spi_write(REG_RX_LOOP_FILTER_2 + offs,
			 LOOP_FILTER_R1(tab[i].LF_R1) |
			 LOOP_FILTER_C3(tab[i].LF_C3));
	ad9361_spi_write(REG_RX_LOOP_FILTER_3 + offs,
			 LOOP_FILTER_R3(tab[i].LF_R3));

	return 0;
}

static int ad9361_get_split_table_gain(struct ad9361_rf_phy *phy, u32 idx_reg,
		struct rf_rx_gain *rx_gain)
{
	u32 val, tbl_addr, lna_index, tia_index, mixer_index;
	int rc = 0;

	rx_gain->lmt_index = ad9361_spi_readf(idx_reg,
					     FULL_TABLE_GAIN_INDEX(~0));
	tbl_addr = ad9361_spi_read(REG_GAIN_TABLE_ADDRESS);

	ad9361_spi_write(REG_GAIN_TABLE_ADDRESS, rx_gain->lmt_index);

	val = ad9361_spi_read(REG_GAIN_TABLE_READ_DATA1);
	lna_index = TO_LNA_GAIN(val);
	mixer_index = TO_MIXER_GM_GAIN(val);

	tia_index = ad9361_spi_readf(REG_GAIN_TABLE_READ_DATA2, TIA_GAIN);

	rx_gain->lmt_gain = lna_table[lna_index] +
				mixer_table[mixer_index] +
				tia_table[tia_index];

	ad9361_spi_write(REG_GAIN_TABLE_ADDRESS, tbl_addr);

	/* Read LPF Index */
	rx_gain->lpf_gain = ad9361_spi_readf(idx_reg + 1, LPF_GAIN_RX(~0));

	/* Read Digital Gain */
	rx_gain->digital_gain = ad9361_spi_readf(idx_reg + 2,
						DIGITAL_GAIN_RX(~0));

	rx_gain->gain_db = rx_gain->lmt_gain + rx_gain->lpf_gain +
				rx_gain->digital_gain;
	return rc;
}

static int ad9361_get_full_table_gain(struct ad9361_rf_phy *phy, u32 idx_reg,
		struct rf_rx_gain *rx_gain)
{
	u32 val;
	enum rx_gain_table_name tbl;
	struct rx_gain_info *gain_info;
	int rc = 0, rx_gain_db;

	tbl = ad9361_gt_tableindex(
		ad9361_from_clk(clk_get_rate(phy, phy->ref_clk_scale[RX_RFPLL])));

	val = ad9361_spi_readf(idx_reg, FULL_TABLE_GAIN_INDEX(~0));

	gain_info = &phy->rx_gain[tbl];
	if (val > gain_info->idx_step_offset) {
		val = val - gain_info->idx_step_offset;
		rx_gain_db = gain_info->starting_gain_db +
			((val) * gain_info->gain_step_db);
	} else {
		rx_gain_db = gain_info->starting_gain_db;
	}

	rx_gain->gain_db = rx_gain_db;

	return rc;
}
int ad9361_get_rx_gain(struct ad9361_rf_phy *phy,
		u32 rx_id, struct rf_rx_gain *rx_gain)
{
	u32 val, idx_reg;
	u8 gain_ctl_shift, rx_enable_mask;
	u8 fast_atk_shift;
	int rc = 0;

	if (rx_id == 1) {
		gain_ctl_shift = RX1_GAIN_CTRL_SHIFT;
		idx_reg = REG_GAIN_RX1;
		rx_enable_mask = RX_CHANNEL_ENABLE(RX_1);
		fast_atk_shift = RX1_FAST_ATK_SHIFT;

	} else if (rx_id == 2) {
		gain_ctl_shift = RX2_GAIN_CTRL_SHIFT;
		idx_reg = REG_GAIN_RX2;
		rx_enable_mask = RX_CHANNEL_ENABLE(RX_2);
		fast_atk_shift = RX2_FAST_ATK_SHIFT;
	} else {
		dev_err("Unknown Rx path %d\n", rx_id);
		rc = -EINVAL;
		goto out;
	}

	val = ad9361_spi_readf(REG_RX_ENABLE_FILTER_CTRL, rx_enable_mask);

	if (!val) {
		dev_err("Rx%d is not enabled\n", rx_gain->ant);
		rc = -EAGAIN;
		goto out;
	}

	val = ad9361_spi_read(REG_AGC_CONFIG_1);

	val = (val >> gain_ctl_shift) & RX_GAIN_CTL_MASK;

	if (val == RX_GAIN_CTL_AGC_FAST_ATK) {
		/* In fast attack mode check whether Fast attack state machine
		 * has locked gain, if not then we can not read gain.
		 */
		val = ad9361_spi_read(REG_FAST_ATTACK_STATE);
		val = (val >> fast_atk_shift) & FAST_ATK_MASK;
		if (val != FAST_ATK_GAIN_LOCKED) {
			dev_err("Failed to read gain, state m/c at %x\n",
				val);
			rc = -EAGAIN;
			goto out;
		}
	}

	if (phy->pdata->split_gt)
		rc = ad9361_get_split_table_gain(phy, idx_reg, rx_gain);
	else
		rc = ad9361_get_full_table_gain(phy, idx_reg, rx_gain);

out:
	return rc;
}

void ad9361_ensm_force_state(struct ad9361_rf_phy *phy, u8 ensm_state)
{
	u8 dev_ensm_state;
	int rc;
	u32 val;

	dev_ensm_state = ad9361_spi_readf(REG_STATE, ENSM_STATE(~0));

	phy->prev_ensm_state = dev_ensm_state;

	if (dev_ensm_state == ensm_state) {
		dev_dbg("Nothing to do, device is already in %d state\n",
			ensm_state);
		goto out;
	}

	dev_dbg("Device is in %x state, forcing to %x\n", dev_ensm_state,
			ensm_state);

	val = ad9361_spi_read(REG_ENSM_CONFIG_1);

	/* Enable control through SPI writes, and take out from
	 * Alert
	 */
	if (val & ENABLE_ENSM_PIN_CTRL) {
		val &= ~ENABLE_ENSM_PIN_CTRL;
		phy->ensm_pin_ctl_en = 1;
	} else {
		phy->ensm_pin_ctl_en = 0;
	}

	if (dev_ensm_state & dev_ensm_state)
		val &= ~(TO_ALERT);

	switch (ensm_state) {

	case ENSM_STATE_TX:
		val |= FORCE_TX_ON;
		break;
	case ENSM_STATE_RX:
		val |= FORCE_RX_ON;
		break;
	case ENSM_STATE_FDD:
		val |= (FORCE_TX_ON | FORCE_RX_ON);
		break;
	case ENSM_STATE_ALERT:
		val &= ~(FORCE_TX_ON | FORCE_RX_ON);
		val |= TO_ALERT | FORCE_ALERT_STATE;
		break;
	default:
		dev_err("No handling for forcing %d ensm state\n",
		ensm_state);
		goto out;
	}

	rc = ad9361_spi_write(REG_ENSM_CONFIG_1, val);
	if (rc)
		dev_err("Failed to restore state\n");

out:
	return;

}

static void ad9361_ensm_restore_prev_state(struct ad9361_rf_phy *phy)
{
	int rc;
	u32 val;

	val = ad9361_spi_read(REG_ENSM_CONFIG_1);

	/* We are restoring state only, so clear State bits first
	 * which might have set while forcing a particular state
	 */
	val &= ~(FORCE_TX_ON | FORCE_RX_ON |
			TO_ALERT | FORCE_ALERT_STATE);

	switch (phy->prev_ensm_state) {

	case ENSM_STATE_TX:
		val |= FORCE_TX_ON;
		break;
	case ENSM_STATE_RX:
		val |= FORCE_RX_ON;
		break;
	case ENSM_STATE_FDD:
		val |= (FORCE_TX_ON | FORCE_RX_ON);
		break;
	case ENSM_STATE_ALERT:
		val |= TO_ALERT;
		break;
	case ENSM_STATE_INVALID:
		dev_dbg("No need to restore, ENSM state wasn't saved\n");
		goto out;
	default:
		dev_dbg("Could not restore to %d ENSM state\n",
		phy->prev_ensm_state);
		goto out;
	}

	rc = ad9361_spi_write(REG_ENSM_CONFIG_1, val);
	if (rc) {
		dev_err("Failed to write ENSM_CONFIG_1");
		goto out;
	}

	if (phy->ensm_pin_ctl_en) {
		val |= ENABLE_ENSM_PIN_CTRL;
		rc = ad9361_spi_write(REG_ENSM_CONFIG_1, val);
		if (rc)
			dev_err("Failed to write ENSM_CONFIG_1");
	}

out:
	return;
}

static int set_split_table_gain(struct ad9361_rf_phy *phy, u32 idx_reg,
		struct rf_rx_gain *rx_gain)
{
	int rc = 0;

	if ((rx_gain->lmt_index > MAX_LMT_INDEX) ||
			(rx_gain->lpf_gain > MAX_LPF_GAIN) ||
			(rx_gain->digital_gain > MAX_DIG_GAIN)) {
		dev_err("LMT_INDEX missing or greater than max value %d",
				MAX_LMT_INDEX);
		dev_err("LPF_GAIN missing or greater than max value %d",
				MAX_LPF_GAIN);
		dev_err("DIGITAL_GAIN cannot be more than %d",
				MAX_DIG_GAIN);
		rc = -EINVAL;
		goto out;
	}
	if (rx_gain->gain_db > 0)
		dev_dbg("Ignoring rx_gain value in split table mode.");
	if (rx_gain->lmt_index == 0 && rx_gain->lpf_gain == 0 &&
			rx_gain->digital_gain == 0) {
		dev_err("In split table mode, All LMT/LPF/digital gains cannot be 0");
		rc = -EINVAL;
		goto out;
	}

	ad9361_spi_writef(idx_reg, RX_FULL_TBL_IDX_MASK, rx_gain->lmt_index);
	ad9361_spi_writef(idx_reg + 1, RX_LPF_IDX_MASK, rx_gain->lpf_gain);

	if (phy->pdata->gain_ctrl.dig_gain_en) {
		ad9361_spi_writef(idx_reg + 2, RX_DIGITAL_IDX_MASK, rx_gain->digital_gain);

	} else if (rx_gain->digital_gain > 0) {
		dev_err("Digital gain is disabled and cannot be set");
	}
out:
	return rc;
}

static int set_full_table_gain(struct ad9361_rf_phy *phy, u32 idx_reg,
		struct rf_rx_gain *rx_gain)
{
	enum rx_gain_table_name tbl;
	struct rx_gain_info *gain_info;
	u32 val;
	int rc = 0;

	if (rx_gain->lmt_index != ~0 || rx_gain->lpf_gain != ~0 ||
			rx_gain->digital_gain > 0)
		dev_dbg("Ignoring lmt/lpf/digital gains in Single Table mode");

	tbl = ad9361_gt_tableindex(
		ad9361_from_clk(clk_get_rate(phy, phy->ref_clk_scale[RX_RFPLL])));

	gain_info = &phy->rx_gain[tbl];
	if ((rx_gain->gain_db < gain_info->starting_gain_db) ||
		(rx_gain->gain_db > gain_info->max_gain_db)) {

		dev_err("Invalid gain %d, supported range [%d - %d]\n",
			rx_gain->gain_db, gain_info->starting_gain_db,
			gain_info->max_gain_db);
		rc = -EINVAL;
		goto out;

	}

	val = ((rx_gain->gain_db - gain_info->starting_gain_db) /
		gain_info->gain_step_db) + gain_info->idx_step_offset;
	ad9361_spi_writef(idx_reg, RX_FULL_TBL_IDX_MASK, val);

out:
	return rc;
}

int ad9361_set_rx_gain(struct ad9361_rf_phy *phy,
		u32 rx_id, struct rf_rx_gain *rx_gain)
{
	u32 val, idx_reg;
	u8 gain_ctl_shift, ensm_state;
	int rc = 0;

	if (rx_id == 1) {
		gain_ctl_shift = RX1_GAIN_CTRL_SHIFT;
		idx_reg = REG_RX1_MANUAL_LMT_FULL_GAIN;

	} else if (rx_id == 2) {
		gain_ctl_shift = RX2_GAIN_CTRL_SHIFT;
		idx_reg = REG_RX2_MANUAL_LMT_FULL_GAIN;
	} else {
		dev_err("Unknown Rx path %d\n", rx_id);
		rc = -EINVAL;
		goto out;

	}

	val = ad9361_spi_read(REG_AGC_CONFIG_1);
	val = (val >> gain_ctl_shift) & RX_GAIN_CTL_MASK;

	if (val != RX_GAIN_CTL_MGC) {
		dev_dbg("Rx gain can be set in MGC mode only\n");
		goto out;
	}

	if (phy->pdata->fdd)
		ensm_state = ENSM_STATE_FDD;
	else
		ensm_state = ENSM_STATE_RX;

	/* RX must be enabled while changing Gain */
	ad9361_ensm_force_state(phy, ensm_state);

	if (phy->pdata->split_gt)
		rc = set_split_table_gain(phy, idx_reg, rx_gain);
	else
		rc = set_full_table_gain(phy, idx_reg, rx_gain);

	/* Restore is done intentionally before checking rc, because
	 * we need to restore PHY to previous state even if write failed
	 */
	ad9361_ensm_restore_prev_state(phy);

	if (rc) {
		dev_err("Unable to write gain tbl idx reg: %d\n", idx_reg);
		goto out;
	}

out:
	return rc;

}

void ad9361_init_gain_info(struct rx_gain_info *rx_gain,
	enum rx_gain_table_type type, int starting_gain,
	int max_gain, int gain_step, int max_idx, int idx_offset)
{
	rx_gain->tbl_type = type;
	rx_gain->starting_gain_db = starting_gain;
	rx_gain->max_gain_db = max_gain;
	rx_gain->gain_step_db = gain_step;
	rx_gain->max_idx = max_idx;
	rx_gain->idx_step_offset = idx_offset;
}

int ad9361_init_gain_tables(struct ad9361_rf_phy *phy)
{
	struct rx_gain_info *rx_gain;

	/* Intialize Meta data according to default gain tables
	 * of AD9631. Changing/Writing of gain tables is not
	 * supported yet.
	 */
	rx_gain = &phy->rx_gain[TBL_200_1300_MHZ];
	ad9361_init_gain_info(rx_gain, RXGAIN_FULL_TBL, 1, 77, 1,
		SIZE_FULL_TABLE, 0);

	rx_gain = &phy->rx_gain[TBL_1300_4000_MHZ];
	ad9361_init_gain_info(rx_gain, RXGAIN_FULL_TBL, -4, 71, 1,
		SIZE_FULL_TABLE, 1);

	rx_gain = &phy->rx_gain[TBL_4000_6000_MHZ];
	ad9361_init_gain_info(rx_gain, RXGAIN_FULL_TBL, -10, 62, 1,
		SIZE_FULL_TABLE, 4);

	return 0;
}

int ad9361_en_dis_tx(struct ad9361_rf_phy *phy, u32 tx_if, u32 enable)
{
	if (tx_if == 2 && !phy->pdata->rx2tx2)
		return -EINVAL;

	return ad9361_spi_writef(REG_TX_ENABLE_FILTER_CTRL,
			TX_CHANNEL_ENABLE(tx_if), enable);
}

int ad9361_en_dis_rx(struct ad9361_rf_phy *phy, u32 rx_if, u32 enable)
{
	if (rx_if == 2 && !phy->pdata->rx2tx2)
		return -EINVAL;

	return ad9361_spi_writef(REG_RX_ENABLE_FILTER_CTRL,
			  RX_CHANNEL_ENABLE(rx_if), enable);
}

int ad9361_set_gain_ctrl_mode(struct ad9361_rf_phy *phy,
		struct rf_gain_ctrl *gain_ctrl)
{
	int rc = 0;
	u32 gain_ctl_shift, mode;
	u8 val;

	rc = ad9361_spi_readm(REG_AGC_CONFIG_1, &val, 1);
	if (rc) {
		dev_err("Unable to read AGC config1 register: %x\n",
			REG_AGC_CONFIG_1);
		goto out;
	}

	switch (gain_ctrl->mode) {
	case RF_GAIN_MGC:
		mode = RX_GAIN_CTL_MGC;
		break;
	case RF_GAIN_FASTATTACK_AGC:
		mode = RX_GAIN_CTL_AGC_FAST_ATK;
		break;
	case RF_GAIN_SLOWATTACK_AGC:
		mode = RX_GAIN_CTL_AGC_SLOW_ATK;
		break;
	case RF_GAIN_HYBRID_AGC:
		mode = RX_GAIN_CTL_AGC_SLOW_ATK_HYBD;
		break;
	default:
		rc = -EINVAL;
		goto out;
	}

	if (gain_ctrl->ant == 1) {
		gain_ctl_shift = RX1_GAIN_CTRL_SHIFT;
	} else if (gain_ctrl->ant == 2) {
		gain_ctl_shift = RX2_GAIN_CTRL_SHIFT;
	} else {
		dev_err("Unknown Rx path %d\n", gain_ctrl->ant);
		rc = -EINVAL;
		goto out;
	}

	rc = ad9361_en_dis_rx(phy, gain_ctrl->ant, RX_DISABLE);
	if (rc) {
		dev_err("Unable to disable rx%d\n", gain_ctrl->ant);
		goto out;
	}

	val &= ~(RX_GAIN_CTL_MASK << gain_ctl_shift);
	val |= mode << gain_ctl_shift;
	if (mode == RX_GAIN_CTL_AGC_SLOW_ATK_HYBD)
		val |= SLOW_ATTACK_HYBRID_MODE;
	else
		val &= ~SLOW_ATTACK_HYBRID_MODE;

	rc = ad9361_spi_write(REG_AGC_CONFIG_1, val);
	if (rc) {
		dev_err("Unable to write AGC config1 register: %x\n",
				REG_AGC_CONFIG_1);
		goto out;
	}

	rc = ad9361_en_dis_rx(phy, gain_ctrl->ant, RX_ENABLE);
out:
	return rc;
}

int ad9361_read_rssi(struct ad9361_rf_phy *phy, struct rf_rssi *rssi)
{
	u8 reg_val_buf[6];
	int rc;

	rc = ad9361_spi_readm(REG_PREAMBLE_LSB,
			reg_val_buf, ARRAY_SIZE(reg_val_buf));
	if (rssi->ant == 1) {
		rssi->symbol = RSSI_RESOLUTION *
				((reg_val_buf[5] << RSSI_LSB_SHIFT) +
				 (reg_val_buf[1] & RSSI_LSB_MASK1));
		rssi->preamble = RSSI_RESOLUTION *
				((reg_val_buf[4] << RSSI_LSB_SHIFT) +
				 (reg_val_buf[0] & RSSI_LSB_MASK1));
	} else if (rssi->ant == 2) {
		rssi->symbol = RSSI_RESOLUTION *
				((reg_val_buf[3] << RSSI_LSB_SHIFT) +
				 ((reg_val_buf[1] & RSSI_LSB_MASK2) >> 1));
		rssi->preamble = RSSI_RESOLUTION *
				((reg_val_buf[2] << RSSI_LSB_SHIFT) +
				 ((reg_val_buf[0] & RSSI_LSB_MASK2) >> 1));
	} else
		rc = -EFAULT;

	rssi->multiplier = RSSI_MULTIPLIER;

	return rc;
}

static int ad9361_rx_adc_setup(struct ad9361_rf_phy *phy, unsigned long bbpll_freq,
			 unsigned long adc_sampl_freq_Hz)
{

	unsigned long scale_snr_1e3, maxsnr, sqrt_inv_rc_tconst_1e3, tmp_1e3,
		scaled_adc_clk_1e6, inv_scaled_adc_clk_1e3, sqrt_term_1e3,
		min_sqrt_term_1e3, bb_bw_Hz;
	u64 tmp, invrc_tconst_1e6;
	u8 data[40];
	u32 i;
	int ret;

	u8 c3_msb = ad9361_spi_read(REG_RX_BBF_C3_MSB);
	u8 c3_lsb = ad9361_spi_read(REG_RX_BBF_C3_LSB);
	u8 r2346 = ad9361_spi_read(REG_RX_BBF_R2346);

	/*
	 * BBBW = (BBPLL / RxTuneDiv) * ln(2) / (1.4 * 2PI )
	 * We assume ad9361_rx_bb_analog_filter_calib() is always run prior
	 */

	tmp = bbpll_freq * 10000ULL;
	do_div(tmp, 126906UL * phy->rxbbf_div);
	bb_bw_Hz = tmp;

	dev_dbg("%s : BBBW %lu : ADCfreq %lu\n",
		__func__, bb_bw_Hz, adc_sampl_freq_Hz);

	dev_dbg("c3_msb 0x%X : c3_lsb 0x%X : r2346 0x%X : \n",
		c3_msb, c3_lsb, r2346);

	bb_bw_Hz = clamp(bb_bw_Hz, 200000UL, 28000000UL);

	if (adc_sampl_freq_Hz < 80000000)
		scale_snr_1e3 = 1000;
	else
		scale_snr_1e3 = 1585; /* pow(10, scale_snr_dB/10); */

 	if (bb_bw_Hz >= 18000000) {
		invrc_tconst_1e6 = (160975ULL * r2346 *
			(160 * c3_msb + 10 * c3_lsb + 140) *
			(bb_bw_Hz) * (1000 + (10 * (bb_bw_Hz - 18000000) / 1000000)));

		do_div(invrc_tconst_1e6, 1000UL);

	} else {
		invrc_tconst_1e6 = (160975ULL * r2346 *
			(160 * c3_msb + 10 * c3_lsb + 140) *
			(bb_bw_Hz));
	}

	do_div(invrc_tconst_1e6, 1000000000UL);

	if (invrc_tconst_1e6 > ULONG_MAX)
		dev_err("invrc_tconst_1e6 > ULONG_MAX\n");

	sqrt_inv_rc_tconst_1e3 = int_sqrt((u32)invrc_tconst_1e6);
	maxsnr = 640/160;
	scaled_adc_clk_1e6 = DIV_ROUND_CLOSEST(adc_sampl_freq_Hz, 640);
	inv_scaled_adc_clk_1e3 = DIV_ROUND_CLOSEST(640000000,
			DIV_ROUND_CLOSEST(adc_sampl_freq_Hz, 1000));
	tmp_1e3 = DIV_ROUND_CLOSEST(980000 + 20 * max_t(u32, 1000U,
			DIV_ROUND_CLOSEST(inv_scaled_adc_clk_1e3, maxsnr)), 1000);
	sqrt_term_1e3 = int_sqrt(scaled_adc_clk_1e6);
	min_sqrt_term_1e3 = min_t(u32, 1000U,
			int_sqrt(maxsnr * scaled_adc_clk_1e6));

	dev_dbg("invrc_tconst_1e6 %llu, sqrt_inv_rc_tconst_1e3 %lu\n",
		invrc_tconst_1e6, sqrt_inv_rc_tconst_1e3);
	dev_dbg("scaled_adc_clk_1e6 %lu, inv_scaled_adc_clk_1e3 %lu\n",
		scaled_adc_clk_1e6, inv_scaled_adc_clk_1e3);
	dev_dbg("tmp_1e3 %lu, sqrt_term_1e3 %lu, min_sqrt_term_1e3 %lu\n",
		tmp_1e3, sqrt_term_1e3, min_sqrt_term_1e3);

	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0x24;
	data[4] = 0x24;
	data[5] = 0;
	data[6] = 0;

	tmp = -50000000 + 8ULL * scale_snr_1e3 * sqrt_inv_rc_tconst_1e3 *
		min_sqrt_term_1e3;
	do_div(tmp, 100000000UL);
	data[7] = min_t(u64, 124U, tmp);

	tmp = (invrc_tconst_1e6 >> 1) + 20 * inv_scaled_adc_clk_1e3 *
		data[7] / 80 * 1000ULL;
	do_div(tmp, invrc_tconst_1e6);
	data[8] = min_t(u64, 255U, tmp);

	tmp = (-500000 + 77ULL * sqrt_inv_rc_tconst_1e3 * min_sqrt_term_1e3);
	do_div(tmp, 1000000UL);
	data[10] = min_t(u64, 127U, tmp);

	data[9] = min_t(u32, 127U, ((800 * data[10]) / 1000));
	tmp = ((invrc_tconst_1e6 >> 1) + (20 * inv_scaled_adc_clk_1e3 *
		data[10] * 1000ULL));
	do_div(tmp, invrc_tconst_1e6 * 77);
	data[11] = min_t(u64, 255U, tmp);
	data[12] = min_t(u32, 127U, (-500000 + 80 * sqrt_inv_rc_tconst_1e3 *
		min_sqrt_term_1e3) / 1000000UL);

	tmp = -3*(long)(invrc_tconst_1e6 >> 1) + inv_scaled_adc_clk_1e3 *
		data[12] * (1000ULL * 20 / 80);
	do_div(tmp, invrc_tconst_1e6);
	data[13] = min_t(u64, 255, tmp);

	data[14] = 21 * (inv_scaled_adc_clk_1e3 / 10000);
	data[15] = min_t(u32, 127U, (500 + 1025 * data[7]) / 1000);
	data[16] = min_t(u32, 127U, (data[15] * tmp_1e3) / 1000);
	data[17] = data[15];
	data[18] = min_t(u32, 127U, (500 + 975 * data[10]) / 1000);
	data[19] = min_t(u32, 127U, (data[18] * tmp_1e3) / 1000);
	data[20] = data[18];
	data[21] = min_t(u32, 127U, (500 + 975 * data[12]) / 1000);
	data[22] = min_t(u32, 127, (data[21] * tmp_1e3) / 1000);
	data[23] = data[21];
	data[24] = 0x2E;
	data[25] = (128 + min_t(u32, 63000U, DIV_ROUND_CLOSEST(63 *
		scaled_adc_clk_1e6, 1000)) / 1000);
	data[26] = min_t(u32, 63U,63 * scaled_adc_clk_1e6 / 1000000 *
		(920 + 80 * inv_scaled_adc_clk_1e3 / 1000) / 1000);
	data[27] = min_t(u32, 63,(32 * sqrt_term_1e3) / 1000);
	data[28] = data[25];
	data[29] = data[26];
	data[30] = data[27];
	data[31] = data[25];
	data[32] = data[26];
	data[33] = min_t(u32, 63U, 63 * sqrt_term_1e3 / 1000);
	data[34] = min_t(u32, 127U, 64 * sqrt_term_1e3 / 1000);
	data[35] = 0x40;
	data[36] = 0x40;
	data[37] = 0x2C;
	data[38] = 0x00;
	data[39] = 0x00;

 	for (i = 0; i < 40; i++) {
		ret = ad9361_spi_write(0x200 + i, data[i]);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int ad9361_rx_tia_calib(struct ad9361_rf_phy *phy, unsigned long bb_bw_Hz)
{
	unsigned long Cbbf, R2346;
	u64 CTIA_fF;

	u8 reg1EB = ad9361_spi_read(REG_RX_BBF_C3_MSB);
	u8 reg1EC = ad9361_spi_read(REG_RX_BBF_C3_LSB);
	u8 reg1E6 = ad9361_spi_read(REG_RX_BBF_R2346);
	u8 reg1DB, reg1DF, reg1DD, reg1DC, reg1DE, temp;

	dev_dbg("%s : bb_bw_Hz %lu\n",
		__func__, bb_bw_Hz);

	bb_bw_Hz = clamp(bb_bw_Hz, 200000UL, 20000000UL);

	Cbbf = (reg1EB * 160) + (reg1EC * 10) + 140; /* fF */
	R2346 = 18300 * RX_BBF_R2346(reg1E6);

	CTIA_fF = Cbbf * R2346 * 560ULL;
	do_div(CTIA_fF, 3500000UL);

	if (bb_bw_Hz <= 3000000UL)
		reg1DB = 0xE0;
	else if (bb_bw_Hz <= 10000000UL)
		reg1DB = 0x60;
	else
		reg1DB = 0x20;

	if (CTIA_fF > 2920ULL) {
		reg1DC = 0x40;
		reg1DE = 0x40;
		temp = min(127U, DIV_ROUND_CLOSEST((u32)CTIA_fF - 400, 320U));
		reg1DD = temp;
		reg1DF = temp;
	} else {
		temp = DIV_ROUND_CLOSEST((u32)CTIA_fF - 400, 40U) + 0x40;
		reg1DC = temp;
		reg1DE = temp;
		reg1DD = 0;
		reg1DF = 0;
	}

	ad9361_spi_write(REG_RX_TIA_CONFIG, reg1DB);
	ad9361_spi_write(REG_TIA1_C_LSB, reg1DC);
	ad9361_spi_write(REG_TIA1_C_MSB, reg1DD);
	ad9361_spi_write(REG_TIA2_C_LSB, reg1DE);
	ad9361_spi_write(REG_TIA2_C_MSB, reg1DF);

	return 0;
}

/* BASEBAND RX ANALOG FILTER CALIBRATION */

static int ad9361_rx_bb_analog_filter_calib(struct ad9361_rf_phy *phy,
					    unsigned long rx_bb_bw,
					    unsigned long bbpll_freq)
{
	unsigned long target;
	u8 tmp;
	int ret;

	dev_dbg("%s : rx_bb_bw %lu bbpll_freq %lu\n",
		__func__, rx_bb_bw, bbpll_freq);

	rx_bb_bw = clamp(rx_bb_bw, 200000UL, 28000000UL);

	/* 1.4 * BBBW * 2PI / ln(2) */
	target =  126906UL * (rx_bb_bw / 10000UL);
	phy->rxbbf_div = min_t(unsigned long, 511UL, DIV_ROUND_UP(bbpll_freq, target));

	/* Set RX baseband filter divide value */
	ad9361_spi_write(REG_RX_BBF_TUNE_DIVIDE, phy->rxbbf_div);
	ad9361_spi_writef(REG_RX_BBF_TUNE_CONFIG, BIT(0), phy->rxbbf_div >> 8);

	/* Write the BBBW into registers 0x1FB and 0x1FC */
	ad9361_spi_write(REG_RX_BBBW_MHZ, rx_bb_bw / 1000000UL);

	tmp = DIV_ROUND_CLOSEST((rx_bb_bw % 1000000UL) * 128, 1000000UL);
	ad9361_spi_write(REG_RX_BBBW_KHZ, min_t(u8, 127, tmp));

	ad9361_spi_write(REG_RX_MIX_LO_CM, RX_MIX_LO_CM(0x3F)); /* Set Rx Mix LO CM */
	ad9361_spi_write(REG_RX_MIX_GM_CONFIG, RX_MIX_GM_PLOAD(3)); /* Set GM common mode */

	/* Enable the RX BBF tune circuit by writing 0x1E2=0x02 and 0x1E3=0x02 */
	ad9361_spi_write(REG_RX1_TUNE_CTRL, RX1_TUNE_RESAMPLE);
	ad9361_spi_write(REG_RX2_TUNE_CTRL, RX2_TUNE_RESAMPLE);

	/* Start the RX Baseband Filter calibration in register 0x016[7] */
	/* Calibration is complete when register 0x016[7] self clears */
	ret = ad9361_run_calibration(phy, RX_BB_TUNE_CAL);

	/* Disable the RX baseband filter tune circuit, write 0x1E2=3, 0x1E3=3 */
	ad9361_spi_write(REG_RX1_TUNE_CTRL,
			RX1_TUNE_RESAMPLE | RX1_PD_TUNE);
	ad9361_spi_write(REG_RX2_TUNE_CTRL,
			RX2_TUNE_RESAMPLE | RX2_PD_TUNE);

	return ret;
}

/* BASEBAND TX ANALOG FILTER CALIBRATION */

static int ad9361_tx_bb_analog_filter_calib(struct ad9361_rf_phy *phy,
					    unsigned long tx_bb_bw,
					    unsigned long bbpll_freq)
{
	unsigned long target, txbbf_div;
	int ret;

	dev_dbg("%s : tx_bb_bw %lu bbpll_freq %lu\n",
		__func__, tx_bb_bw, bbpll_freq);

	tx_bb_bw = clamp(tx_bb_bw, 625000UL, 20000000UL);

	/* 1.6 * BBBW * 2PI / ln(2) */
	target =  132345 * (tx_bb_bw / 10000UL);
	txbbf_div = min_t(unsigned long, 511UL, DIV_ROUND_UP(bbpll_freq, target));

	/* Set TX baseband filter divide value */
	ad9361_spi_write(REG_TX_BBF_TUNE_DIVIDER, txbbf_div);
	ad9361_spi_writef(REG_TX_BBF_TUNE_MODE,
			  TX_BBF_TUNE_DIVIDER, txbbf_div >> 8);

	/* Enable the TX baseband filter tune circuit by setting 0x0CA=0x22. */
	ad9361_spi_write(REG_TX_TUNE_CTRL, TUNER_RESAMPLE | TUNE_CTRL(1));

	/* Start the TX Baseband Filter calibration in register 0x016[6] */
	/* Calibration is complete when register 0x016[] self clears */
	ret = ad9361_run_calibration(phy, TX_BB_TUNE_CAL);

	/* Disable the TX baseband filter tune circuit by writing 0x0CA=0x26. */
	ad9361_spi_write(REG_TX_TUNE_CTRL,
			 TUNER_RESAMPLE | TUNE_CTRL(1) | PD_TUNE);

	return ret;
}

/* BASEBAND TX SECONDARY FILTER */

static int ad9361_tx_bb_second_filter_calib(struct ad9361_rf_phy *phy,
					   unsigned long tx_rf_bw)
{
	u64 cap;
	unsigned long corner, res, div;
	u32 reg_conf, reg_res;
	int ret, i;

	dev_dbg("%s : tx_rf_bw %lu\n",
		__func__, tx_rf_bw);

	tx_rf_bw = clamp(tx_rf_bw, 1060000UL, 40000000UL);

	/* BBBW * 5PI */
	corner = 15708 * (tx_rf_bw / 20000UL);

	for (i = 0, res = 1; i < 4; i++) {
		div = corner * res;
		cap = (500000000ULL) + (div >> 1);
		do_div(cap, div);
		cap -= 12ULL;
		if (cap < 64ULL)
			break;

		res <<= 1;
	}

	if (cap > 63ULL)
		cap = 63ULL;

	if(tx_rf_bw <= 9000000UL )
		reg_conf = 0x59;
	else if (tx_rf_bw <= 24000000UL)
		reg_conf = 0x56;
	else
		reg_conf = 0x57;

	switch (res) {
	case 1:
		reg_res = 0x0C;
		break;
	case 2:
		reg_res = 0x04;
		break;
	case 4:
		reg_res = 0x03;
		break;
	case 8:
		reg_res = 0x01;
		break;
	default:
		reg_res = 0x01;
	}

	ret = ad9361_spi_write(REG_CONFIG0, reg_conf);
	ret |= ad9361_spi_write(REG_RESISTOR, reg_res);
	ret |= ad9361_spi_write(REG_CAPACITOR, (u8)cap);

	return ret;
}

/* RF SYNTHESIZER CHARGE PUMP CALIBRATION */

static int ad9361_txrx_synth_cp_calib(struct ad9361_rf_phy *phy,
					   unsigned long ref_clk_hz, bool tx)
{
	u32 offs = tx ? 0x40 : 0;
	u32 vco_cal_cnt;
	int ret;

	dev_dbg("%s : ref_clk_hz %lu : is_tx %d\n",
		__func__, ref_clk_hz, tx);

	ad9361_spi_write(REG_RX_LO_GEN_POWER_MODE + offs, 0x00);
	ad9361_spi_write(REG_RX_VCO_LDO + offs, 0x0B);
	ad9361_spi_write(REG_RX_VCO_PD_OVERRIDES + offs, 0x02);
	ad9361_spi_write(REG_RX_CP_CURRENT + offs, 0x80);
	ad9361_spi_write(REG_RX_CP_CONFIG + offs, 0x00);

	/* see Table 70 Example Calibration Times for RF VCO Cal */
	if (phy->pdata->fdd) {
		vco_cal_cnt = 0x8E;
	} else {
		if (ref_clk_hz >= 50000000UL)
			vco_cal_cnt = 0x86;
		else
			vco_cal_cnt = 0x82;
	}

	ad9361_spi_write(REG_RX_VCO_CAL + offs, vco_cal_cnt);

	/* Enable FDD mode during calibrations */

	if (!phy->pdata->fdd)
		ad9361_spi_write(REG_PARALLEL_PORT_CONF_3, LVDS_MODE);

	ad9361_spi_write(REG_ENSM_CONFIG_2, DUAL_SYNTH_MODE);
	ad9361_spi_write(REG_ENSM_CONFIG_1,
			FORCE_ALERT_STATE |
			TO_ALERT);
	ad9361_spi_write(REG_ENSM_MODE, FDD_MODE);

	ad9361_spi_write(REG_RX_CP_CONFIG + offs, CP_CAL_ENABLE);

	ret = ad9361_check_cal_done(phy, REG_RX_CAL_STATUS + offs,
				    CP_CAL_VALID, 1);
	ad9361_spi_write(REG_RX_CP_CONFIG + offs, 0x0);

	return ret;
}

/* BASEBAND DC OFFSET CALIBRATION */
static int ad9361_bb_dc_offset_calib(struct ad9361_rf_phy *phy)
{
	dev_dbg("%s\n", __func__);

	ad9361_spi_write(REG_BB_DC_OFFSET_COUNT, 0x3F);
	ad9361_spi_write(REG_BB_DC_OFFSET_SHIFT, BB_DC_M_SHIFT(0xF));
	ad9361_spi_write(REG_BB_DC_OFFSET_ATTEN, BB_DC_OFFSET_ATTEN(1));

	return ad9361_run_calibration(phy, BBDC_CAL);
}

/* RF DC OFFSET CALIBRATION */

static int ad9361_rf_dc_offset_calib(struct ad9361_rf_phy *phy,
				     u64 rx_freq)
{
	dev_dbg("%s : rx_freq %llu\n",
		__func__, rx_freq);

// 	ad9361_spi_write(REG_ENSM_CONFIG_1,
// 			ENABLE_ENSM_PIN_CTRL |
// 			FORCE_ALERT_STATE |
// 			TO_ALERT);

	ad9361_spi_write(REG_WAIT_COUNT, 0x20);

	if(rx_freq <= 4000000000ULL) {
		ad9361_spi_write(REG_RF_DC_OFFSET_COUNT, 0x32);
		ad9361_spi_write(REG_RF_DC_OFFSET_CONFIG_1,
				 RF_DC_CALIBRATION_COUNT(4) | DAC_FS(2));
		ad9361_spi_write(REG_RF_DC_OFFSET_ATTEN,
				 RF_DC_OFFSET_ATTEN(5));
	} else {
		ad9361_spi_write(REG_RF_DC_OFFSET_COUNT, 0x28);
		ad9361_spi_write(REG_RF_DC_OFFSET_CONFIG_1,
				 RF_DC_CALIBRATION_COUNT(4) | DAC_FS(3));
		ad9361_spi_write(REG_RF_DC_OFFSET_ATTEN,
				 RF_DC_OFFSET_ATTEN(6));
	}

	ad9361_spi_write(REG_DC_OFFSET_CONFIG2,
			 USE_WAIT_COUNTER_FOR_RF_DC_INIT_CAL |
			 DC_OFFSET_UPDATE(3));

	ad9361_spi_write(REG_INVERT_BITS,
			 INVERT_RX1_RF_DC_CGOUT_WORD |
			 INVERT_RX2_RF_DC_CGOUT_WORD);

	return ad9361_run_calibration(phy, RFDC_CAL);
}

/* TX QUADRATURE CALIBRATION */

static int ad9361_tx_quad_calib(struct ad9361_rf_phy *phy,
					   unsigned long bw, int rx_phase)
{
	unsigned long clktf, clkrf;
	int txnco_word, rxnco_word;
	u8 __rx_phase = 0;
	const u8 (*tab)[3];
	u32 index_max, i , lpf_tia_mask;
	/*
	 * Find NCO frequency that matches this equation:
	 * BW / 4 = Rx NCO freq = Tx NCO freq:
	 * Rx NCO = ClkRF * (rxNCO <1:0> + 1) / 32
	 * Tx NCO = ClkTF * (txNCO <1:0> + 1) / 32
	 */

	clkrf = clk_get_rate(phy, phy->ref_clk_scale[CLKRF_CLK]);
	clktf = clk_get_rate(phy, phy->ref_clk_scale[CLKTF_CLK]);

	dev_dbg("%s : bw %lu clkrf %lu clktf %lu\n",
		__func__, bw, clkrf, clktf);

	txnco_word = DIV_ROUND_CLOSEST(bw * 8, clktf) - 1;
	txnco_word = clamp_t(int, txnco_word, 0, 3);

 	dev_dbg("Tx NCO frequency: %lu (BW/4: %lu) txnco_word %d\n",
		clktf * (txnco_word + 1) / 32, bw / 4, txnco_word);

	rxnco_word = txnco_word;

	if (clkrf == (2 * clktf)) {
		__rx_phase = 0x0E;
		switch (txnco_word) {
		case 0:
			txnco_word++;
			break;
		case 1:
			rxnco_word--;
			break;
		case 2:
			rxnco_word-=2;
			txnco_word--;
			break;
		case 3:
			rxnco_word-=2;	/* REVISIT */
			__rx_phase = 0x08;
			break;
		}
	} else if (clkrf == clktf) {
		switch (txnco_word) {
		case 0:
		case 3:
			__rx_phase = 0x15;
			break;
		case 2:
			__rx_phase = 0x1F;
			break;
		case 1:
			if (ad9361_spi_readf(REG_TX_ENABLE_FILTER_CTRL, 0x3F) == 0x22)
				__rx_phase = 0x15; 	/* REVISIT */
			else
				__rx_phase = 0x1A;
			break;
		}
	} else
		dev_err("Error in %s line %d clkrf %lu clktf %lu\n",
			__func__, __LINE__, clkrf, clktf);

	if (rx_phase >= 0)
	  __rx_phase = rx_phase;

	ad9361_spi_write(REG_QUAD_CAL_NCO_FREQ_PHASE_OFFSET,
			 RX_NCO_FREQ(rxnco_word) | RX_NCO_PHASE_OFFSET(__rx_phase));
	ad9361_spi_writef(REG_KEXP_2, TX_NCO_FREQ(~0), txnco_word);

	ad9361_spi_write(REG_QUAD_CAL_CTRL,
			 SETTLE_MAIN_ENABLE | DC_OFFSET_ENABLE |
			 GAIN_ENABLE | PHASE_ENABLE | M_DECIM(3));
	ad9361_spi_write(REG_QUAD_CAL_COUNT, 0xFF);
	ad9361_spi_write(REG_KEXP_1, KEXP_TX(1) | KEXP_TX_COMP(3) |
			 KEXP_DC_I(3) | KEXP_DC_Q(3));
	ad9361_spi_write(REG_MAG_FTEST_THRESH, 0x01);
	ad9361_spi_write(REG_MAG_FTEST_THRESH_2, 0x01);

	if (phy->pdata->split_gt) {
		tab = &split_gain_table[phy->current_table][0];
		index_max = SIZE_SPLIT_TABLE;
		lpf_tia_mask = 0x20;
	} else {
		tab = &full_gain_table[phy->current_table][0];
		index_max = SIZE_FULL_TABLE;
		lpf_tia_mask = 0x3F;
	}

	for (i = 0; i < index_max; i++)
		if ((tab[i][1] & lpf_tia_mask) == 0x20) {
			ad9361_spi_write(REG_TX_QUAD_FULL_LMT_GAIN, i);
			break;
		}

	if (i >= index_max)
		dev_err("failed to find suitable LPF TIA value in gain table\n");

	ad9361_spi_write(REG_QUAD_SETTLE_COUNT, 0xF0);
	ad9361_spi_write(REG_TX_QUAD_LPF_GAIN, 0x00);

	return ad9361_run_calibration(phy, TX_QUAD_CAL);
}

static int ad9361_rx_quad_calib(struct ad9361_rf_phy *phy,
					   unsigned long bw)
{
	return -EINVAL; /* TODO */
}

static int ad9361_tracking_control(struct ad9361_rf_phy *phy, bool bbdc_track,
				   bool rfdc_track, bool rxquad_track)
{
	u32 qtrack = 0;

	dev_dbg("%s : bbdc_track=%d, rfdc_track=%d, rxquad_track=%d\n",
		__func__, bbdc_track, rfdc_track, rxquad_track);

	ad9361_spi_write(REG_CALIBRATION_CONFIG_2,
			 CALIBRATION_CONFIG2_DFLT  | K_EXP_PHASE(0x15));
	ad9361_spi_write(REG_CALIBRATION_CONFIG_3,
			 PREVENT_POS_LOOP_GAIN | K_EXP_AMPLITUDE(0x15));

	ad9361_spi_writef(REG_DC_OFFSET_CONFIG2,
			  DC_OFFSET_UPDATE(~0), 0x5); /* Gain change + Rx exit */
	ad9361_spi_writef(REG_DC_OFFSET_CONFIG2,
			  ENABLE_BB_DC_OFFSET_TRACKING, bbdc_track);
	ad9361_spi_writef(REG_DC_OFFSET_CONFIG2,
			  ENABLE_RF_OFFSET_TRACKING, rfdc_track);

	if (rxquad_track)
		qtrack = ENABLE_TRACKING_MODE_CH1 |
			(phy->pdata->rx2tx2 ? ENABLE_TRACKING_MODE_CH2 : 0);

	ad9361_spi_write(REG_CALIBRATION_CONFIG_1,
			 ENABLE_PHASE_CORR | ENABLE_GAIN_CORR |
			 FREE_RUN_MODE | ENABLE_CORR_WORD_DECIMATION |
			 qtrack);

	return 0;
}


/* REFERENCE CLOCK DELAY UNIT COUNTER REGISTER */
static int ad9361_set_ref_clk_cycles(struct ad9361_rf_phy *phy,
				    unsigned long ref_clk_hz)
{
	dev_dbg("%s : ref_clk_hz %lu\n",
		__func__, ref_clk_hz);

	return ad9361_spi_write(REG_REFERENCE_CLOCK_CYCLES,
		REFERENCE_CLOCK_CYCLES_PER_US((ref_clk_hz / 1000000UL) - 1));
}

static int ad9361_set_dcxo_tune(struct ad9361_rf_phy *phy,
				    u32 coarse, u32 fine)
{
	dev_dbg("%s : coarse %u fine %u\n",
		__func__, coarse, fine);

	ad9361_spi_write(REG_DCXO_COARSE_TUNE,
			DCXO_TUNE_COARSE(coarse));
	ad9361_spi_write(REG_DCXO_FINE_TUNE_LOW,
			DCXO_TUNE_FINE_LOW(fine));
	return ad9361_spi_write(REG_DCXO_FINE_TUNE_HIGH,
			DCXO_TUNE_FINE_HIGH(fine));
}

/* val
 * 0	(RX1A_N &  RX1A_P) and (RX2A_N & RX2A_P) enabled; balanced
 * 1	(RX1B_N &  RX1B_P) and (RX2B_N & RX2B_P) enabled; balanced
 * 2	(RX1C_N &  RX1C_P) and (RX2C_N & RX2C_P) enabled; balanced
 *
 * 3	RX1A_N and RX2A_N enabled; unbalanced
 * 4	RX1A_P and RX2A_P enabled; unbalanced
 * 5	RX1B_N and RX2B_N enabled; unbalanced
 * 6	RX1B_P and RX2B_P enabled; unbalanced
 * 7	RX1C_N and RX2C_N enabled; unbalanced
 * 8	RX1C_P and RX2C_P enabled; unbalanced
 */

static int ad9361_rf_port_setup(struct ad9361_rf_phy *phy,
				    u32 rx_inputs, u32 txb)
{
	u32 val;

	if (rx_inputs > 8)
		return -EINVAL;

	if (rx_inputs < 3)
		val = 3 <<  (rx_inputs * 2);
	else
		val = 1 <<  (rx_inputs - 3);

	if (txb)
		val |= TX_OUTPUT; /* Select TX1B, TX2B */

	dev_dbg("%s : INPUT_SELECT 0x%X\n",
		__func__, val);

	return ad9361_spi_write(REG_INPUT_SELECT, val);
}

/*
 * Setup the Parallel Port (Digital Data Interface)
 */
static int ad9361_pp_port_setup(struct ad9361_rf_phy *phy, bool restore_c3)
{
	struct ad9361_phy_platform_data *pd = phy->pdata;

	dev_dbg("%s\n", __func__);

	if (!pd->fdd) /* REVISIT */
		pd->port_ctrl.pp_conf[2] |= HALF_DUPLEX_MODE;

	if (restore_c3) {
		return ad9361_spi_write(REG_PARALLEL_PORT_CONF_3,
					pd->port_ctrl.pp_conf[2]);
	}

	ad9361_spi_write(REG_PARALLEL_PORT_CONF_1, pd->port_ctrl.pp_conf[0]);
	ad9361_spi_write(REG_PARALLEL_PORT_CONF_2, pd->port_ctrl.pp_conf[1]);
	ad9361_spi_write(REG_PARALLEL_PORT_CONF_3, pd->port_ctrl.pp_conf[2]);
	ad9361_spi_write(REG_RX_CLOCK_DATA_DELAY, pd->port_ctrl.rx_clk_data_delay);
	ad9361_spi_write(REG_TX_CLOCK_DATA_DELAY, pd->port_ctrl.tx_clk_data_delay);

	ad9361_spi_write(REG_LVDS_BIAS_CTRL, pd->port_ctrl.lvds_bias_ctrl);
//	ad9361_spi_write(REG_DIGITAL_IO_CTRL, pd->port_ctrl.digital_io_ctrl);
	ad9361_spi_write(REG_LVDS_INVERT_CTRL1, pd->port_ctrl.lvds_invert[0]);
	ad9361_spi_write(REG_LVDS_INVERT_CTRL2, pd->port_ctrl.lvds_invert[1]);

	return 0;
}

static int ad9361_gc_setup(struct ad9361_rf_phy *phy, struct gain_control *ctrl)
{
	u32 reg, tmp1, tmp2;

	dev_dbg("%s\n", __func__);

	reg = DEC_PWR_FOR_GAIN_LOCK_EXIT | DEC_PWR_FOR_LOCK_LEVEL |
		DEC_PWR_FOR_LOW_PWR;

	if (ctrl->rx1_mode == RF_GAIN_HYBRID_AGC ||
		ctrl->rx2_mode == RF_GAIN_HYBRID_AGC)
		reg |= SLOW_ATTACK_HYBRID_MODE;

	reg |= RX1_GAIN_CTRL_SETUP(ctrl->rx1_mode) |
		RX2_GAIN_CTRL_SETUP(ctrl->rx2_mode);

	phy->agc_mode[0] = ctrl->rx1_mode;
	phy->agc_mode[1] = ctrl->rx2_mode;

	ad9361_spi_write(REG_AGC_CONFIG_1, reg); // Gain Control Mode Select

	/* AGC_USE_FULL_GAIN_TABLE handled in ad9361_load_gt() */
	ad9361_spi_writef(REG_AGC_CONFIG_2, MAN_GAIN_CTRL_RX1,
			  ctrl->mgc_rx1_ctrl_inp_en);
	ad9361_spi_writef(REG_AGC_CONFIG_2, MAN_GAIN_CTRL_RX2,
			  ctrl->mgc_rx2_ctrl_inp_en);
	ad9361_spi_writef(REG_AGC_CONFIG_2, DIG_GAIN_EN,
			  ctrl->dig_gain_en);

	ctrl->adc_ovr_sample_size = clamp_t(u8, ctrl->adc_ovr_sample_size, 1U, 8U);
	reg = ADC_OVERRANGE_SAMPLE_SIZE(ctrl->adc_ovr_sample_size - 1);

	if (phy->pdata->split_gt &&
		(ctrl->mgc_rx1_ctrl_inp_en || ctrl->mgc_rx2_ctrl_inp_en)) {
		switch (ctrl->mgc_split_table_ctrl_inp_gain_mode) {
		case 1:
			reg &= ~INCDEC_LMT_GAIN;
			break;
		case 2:
			reg |= INCDEC_LMT_GAIN;
			break;
		default:
		case 0:
			reg |= USE_AGC_FOR_LMTLPF_GAIN;
			break;
		}
	}

	ctrl->mgc_inc_gain_step = clamp_t(u8, ctrl->mgc_inc_gain_step, 1U, 8U);
	reg |= MANUAL_INCR_STEP_SIZE(ctrl->mgc_inc_gain_step - 1);
	ad9361_spi_write(REG_AGC_CONFIG_3, reg); // Incr Step Size, ADC Overrange Size

	if (phy->pdata->split_gt) {
		reg = SIZE_SPLIT_TABLE - 1;
	} else {
		reg = SIZE_FULL_TABLE - 1;
	}
	ad9361_spi_write(REG_MAX_LMT_FULL_GAIN, reg); // Max Full/LMT Gain Table Index
	ad9361_spi_write(REG_RX1_MANUAL_LMT_FULL_GAIN, reg); // Rx1 Full/LMT Gain Index
	ad9361_spi_write(REG_RX2_MANUAL_LMT_FULL_GAIN, reg); // Rx2 Full/LMT Gain Index

	ctrl->analog_settling_time = clamp_t(u8, ctrl->analog_settling_time, 0U, 31U);
	ctrl->mgc_dec_gain_step = clamp_t(u8, ctrl->mgc_dec_gain_step, 1U, 8U);
	reg = PEAK_OVERLOAD_WAIT_TIME(ctrl->analog_settling_time);
	reg |= MANUAL_CTRL_IN_DECR_GAIN_STP_SIZE(ctrl->mgc_dec_gain_step);
	ad9361_spi_write(REG_PEAK_WAIT_TIME, reg); // Decr Step Size, Peak Overload Time

	if (ctrl->dig_gain_en)
		ad9361_spi_write(REG_DIGITAL_GAIN,
				MAXIMUM_DIGITAL_GAIN(ctrl->max_dig_gain) |
				DIG_GAIN_STP_SIZE(ctrl->dig_gain_step_size));

	if (ctrl->adc_large_overload_thresh >= ctrl->adc_small_overload_thresh) {
		ad9361_spi_write(REG_ADC_SMALL_OVERLOAD_THRESH,
				 ctrl->adc_small_overload_thresh); // ADC Small Overload Threshold
		ad9361_spi_write(REG_ADC_LARGE_OVERLOAD_THRESH,
				 ctrl->adc_large_overload_thresh); // ADC Large Overload Threshold
	} else {
		ad9361_spi_write(REG_ADC_SMALL_OVERLOAD_THRESH,
				 ctrl->adc_large_overload_thresh); // ADC Small Overload Threshold
		ad9361_spi_write(REG_ADC_LARGE_OVERLOAD_THRESH,
				 ctrl->adc_small_overload_thresh); // ADC Large Overload Threshold
	}

	reg = (ctrl->lmt_overload_high_thresh / 16) - 1;
	reg = clamp(reg, 0U, 63U);
	ad9361_spi_write(REG_LARGE_LMT_OVERLOAD_THRESH, reg);
	reg = (ctrl->lmt_overload_low_thresh / 16) - 1;
	reg = clamp(reg, 0U, 63U);
	ad9361_spi_writef(REG_SMALL_LMT_OVERLOAD_THRESH,
			  SMALL_LMT_OVERLOAD_THRESH(~0), reg);

	if (phy->pdata->split_gt) {
		/* REVIST */
		ad9361_spi_write(REG_RX1_MANUAL_LPF_GAIN, 0x58); // Rx1 LPF Gain Index
		ad9361_spi_write(REG_RX2_MANUAL_LPF_GAIN, 0x18); // Rx2 LPF Gain Index
		ad9361_spi_write(REG_FAST_INITIAL_LMT_GAIN_LIMIT, 0x27); // Initial LMT Gain Limit
	}

	ad9361_spi_write(REG_RX1_MANUAL_DIGITALFORCED_GAIN, 0x00); // Rx1 Digital Gain Index
	ad9361_spi_write(REG_RX2_MANUAL_DIGITALFORCED_GAIN, 0x00); // Rx2 Digital Gain Index

	reg = clamp_t(u8, ctrl->low_power_thresh, 0U, 64U) * 2;
	ad9361_spi_write(REG_FAST_LOW_POWER_THRESH, reg); // Low Power Threshold
	ad9361_spi_write(REG_TX_SYMBOL_ATTEN_CONFIG, 0x00); // Tx Symbol Gain Control

	ad9361_spi_writef(REG_DEC_POWER_MEASURE_DURATION_0,
			  USE_HB1_OUT_FOR_DEC_PWR_MEAS, 1); // Power Measurement Duration

	ad9361_spi_writef(REG_DEC_POWER_MEASURE_DURATION_0,
			  ENABLE_DEC_PWR_MEAS, 1); // Power Measurement Duration

	reg = ilog2(ctrl->dec_pow_measuremnt_duration / 16);
	ad9361_spi_writef(REG_DEC_POWER_MEASURE_DURATION_0,
			  DEC_POWER_MEASUREMENT_DURATION(~0), reg); // Power Measurement Duration

	/* AGC */

	reg = clamp_t(u8, ctrl->agc_attack_delay_us, 0U, 31U);
	ad9361_spi_writef(REG_AGC_ATTACK_DELAY,
			  AGC_ATTACK_DELAY(~0), reg);

	/* For Fast AGC upper bits may be used */
	reg = clamp_t(u8, ctrl->agc_settling_delay, 0U, 31U);
	ad9361_spi_write(REG_FAST_CONFIG_2_SETTLING_DELAY, reg);

	tmp1 = reg = clamp_t(u8, ctrl->agc_inner_thresh_high, 0U, 127U);
	ad9361_spi_writef(REG_AGC_LOCK_LEVEL,
			  AGC_LOCK_LEVEL_FAST_AGC_INNER_HIGH_THRESH_SLOW(~0),
			  reg);

	tmp2 = reg = clamp_t(u8, ctrl->agc_inner_thresh_low, 0U, 127U);
	reg |= (ctrl->adc_lmt_small_overload_prevent_gain_inc ?
		PREVENT_GAIN_INC : 0);
	ad9361_spi_write(REG_AGC_INNER_LOW_THRESH, reg);

	reg = AGC_OUTER_HIGH_THRESH(tmp1 - ctrl->agc_outer_thresh_high) |
		AGC_OUTER_LOW_THRESH(ctrl->agc_outer_thresh_low - tmp2);
	ad9361_spi_write(REG_OUTER_POWER_THRESHS, reg);

	reg = AGC_OUTER_HIGH_THRESH_EXED_STP_SIZE(ctrl->agc_outer_thresh_high_dec_steps) |
		AGC_OUTER_LOW_THRESH_EXED_STP_SIZE(ctrl->agc_outer_thresh_low_inc_steps);
	ad9361_spi_write(REG_GAIN_STP_2, reg);

	reg = ((ctrl->immed_gain_change_if_large_adc_overload) ?
		IMMED_GAIN_CHANGE_IF_LG_ADC_OVERLOAD : 0) |
		((ctrl->immed_gain_change_if_large_lmt_overload) ?
		IMMED_GAIN_CHANGE_IF_LG_LMT_OVERLOAD : 0) |
		AGC_INNER_HIGH_THRESH_EXED_STP_SIZE(ctrl->agc_inner_thresh_high_dec_steps) |
		AGC_INNER_LOW_THRESH_EXED_STP_SIZE(ctrl->agc_inner_thresh_low_inc_steps);
	ad9361_spi_write(REG_GAIN_STP1, reg);

	reg = LARGE_ADC_OVERLOAD_EXED_COUNTER(ctrl->adc_large_overload_exceed_counter) |
		SMALL_ADC_OVERLOAD_EXED_COUNTER(ctrl->adc_small_overload_exceed_counter);
	ad9361_spi_write(REG_ADC_OVERLOAD_COUNTERS, reg);

	ad9361_spi_writef(REG_GAIN_STP_CONFIG_2, LARGE_LPF_GAIN_STEP(~0),
			 LARGE_LPF_GAIN_STEP(ctrl->adc_large_overload_inc_steps));

	reg = LARGE_LMT_OVERLOAD_EXED_COUNTER(ctrl->lmt_overload_large_exceed_counter) |
		SMALL_LMT_OVERLOAD_EXED_COUNTER(ctrl->lmt_overload_small_exceed_counter);
	ad9361_spi_write(REG_LMT_OVERLOAD_COUNTERS, reg);

	ad9361_spi_writef(REG_GAIN_STP_CONFIG1,
			DEC_STP_SIZE_FOR_LARGE_LMT_OVERLOAD(~0),
			ctrl->lmt_overload_large_inc_steps);

	reg = DIG_SATURATION_EXED_COUNTER(ctrl->dig_saturation_exceed_counter) |
		(ctrl->sync_for_gain_counter_en ? ENABLE_SYNC_FOR_GAIN_COUNTER : 0) |
		(ctrl->gain_update_counter > 131070 ? DOUBLE_GAIN_COUNTER : 0);
	ad9361_spi_write(REG_DIGITAL_SAT_COUNTER, reg);

	tmp1 = (ctrl->gain_update_counter > 131070) ?
		ctrl->gain_update_counter / 4 :
		ctrl->gain_update_counter / 2;

	ad9361_spi_write(REG_GAIN_UPDATE_COUNTER1, tmp1 & 0xFF);
	ad9361_spi_write(REG_GAIN_UPDATE_COUNTER2, tmp1 >> 8);

	return 0;
}

  //************************************************************
  // Setup AuxDAC
  //************************************************************
static int ad9361_auxdac_setup(struct ad9361_rf_phy *phy)
{
	/* FIXME later */

	dev_dbg("%s\n", __func__);

	ad9361_spi_write(0x018, 0x00); // AuxDAC1 Word[9:2]
	ad9361_spi_write(0x019, 0x00); // AuxDAC2 Word[9:2]
	ad9361_spi_write(0x01a, 0x00); // AuxDAC1 Config and Word[1:0]
	ad9361_spi_write(0x01b, 0x00); // AuxDAC2 Config and Word[1:0]
	ad9361_spi_write(0x023, 0xff); // AuxDAC Manaul/Auto Control
	/* FIXME eLNA control */
	ad9361_spi_write(0x026, 0x00); // AuxDAC Manual Select Bit/GPO Manual Select
	ad9361_spi_write(0x030, 0x00); // AuxDAC1 Rx Delay
	ad9361_spi_write(0x031, 0x00); // AuxDAC1 Tx Delay
	ad9361_spi_write(0x032, 0x00); // AuxDAC2 Rx Delay
	ad9361_spi_write(0x033, 0x00); // AuxDAC2 Tx Delay

	return 0;
}
  //************************************************************
  // Setup AuxADC
  //************************************************************

static int ad9361_auxadc_setup(struct ad9361_rf_phy *phy,
			       struct auxadc_control *ctrl,
			       unsigned long bbpll_freq)
{
	u32 val;

	/* FIXME this function needs to be called whenever BBPLL changes */

	dev_dbg("%s\n", __func__);

	val = DIV_ROUND_CLOSEST(ctrl->temp_time_inteval_ms *
		(bbpll_freq / 1000UL), (1 << 29));

	ad9361_spi_write(REG_TEMP_OFFSET, ctrl->offset);
	ad9361_spi_write(REG_START_TEMP_READING, 0x00);
	ad9361_spi_write(REG_TEMP_SENSE2,
			 MEASUREMENT_TIME_INTERVAL(val) |
			 (ctrl->periodic_temp_measuremnt ?
			 TEMP_SENSE_PERIODIC_ENABLE : 0));
	ad9361_spi_write(REG_TEMP_SENSOR_CONFIG,
			 TEMP_SENSOR_DECIMATION(
			 ilog2(ctrl->temp_sensor_decimation) - 8));
	ad9361_spi_write(REG_AUXADC_CLOCK_DIVIDER,
			 bbpll_freq / ctrl->auxadc_clock_rate);
	ad9361_spi_write(REG_AUXADC_CONFIG,
			 AUX_ADC_DECIMATION(
			ilog2(ctrl->auxadc_decimation) - 8));

	return 0;
}

static int ad9361_get_temp(struct ad9361_rf_phy *phy)
{
	u32 val = ad9361_spi_read(REG_TEMPERATURE);

	return DIV_ROUND_CLOSEST(val * 1000, 1140);
}

  //************************************************************
  // Setup Control Outs
  //************************************************************

static int ad9361_ctrl_outs_setup(struct ad9361_rf_phy *phy,
				  struct ctrl_outs_control *ctrl)
{
	dev_dbg("%s\n", __func__);

	ad9361_spi_write(REG_CTRL_OUTPUT_POINTER, ctrl->index); // Ctrl Out index
	return ad9361_spi_write(REG_CTRL_OUTPUT_ENABLE, ctrl->en_mask); // Ctrl Out [7:0] output enable
}
  //************************************************************
  // Setup GPO
  //************************************************************

static int ad9361_gpo_setup(struct ad9361_rf_phy *phy)
{
	/* FIXME later */

	dev_dbg("%s\n", __func__);

	ad9361_spi_write(0x020, 0x00); // GPO Auto Enable Setup in RX and TX
	ad9361_spi_write(0x027, 0x03); // GPO Manual and GPO auto value in ALERT
	ad9361_spi_write(0x028, 0x00); // GPO_0 RX Delay
	ad9361_spi_write(0x029, 0x00); // GPO_1 RX Delay
	ad9361_spi_write(0x02a, 0x00); // GPO_2 RX Delay
	ad9361_spi_write(0x02b, 0x00); // GPO_3 RX Delay
	ad9361_spi_write(0x02c, 0x00); // GPO_0 TX Delay
	ad9361_spi_write(0x02d, 0x00); // GPO_1 TX Delay
	ad9361_spi_write(0x02e, 0x00); // GPO_2 TX Delay
	ad9361_spi_write(0x02f, 0x00); // GPO_3 TX Delay

	return 0;
}

static int ad9361_rssi_setup(struct ad9361_rf_phy *phy,
			     struct rssi_control *ctrl,
			     bool is_update)
{
	u32 total_weight, weight[4], total_dur = 0, temp;
	u8 dur_buf[4] = {0};
	int val, ret, i, j = 0;
	u32 rssi_delay;
	u32 rssi_wait;
	u32 rssi_duration;
	unsigned long rate;

	dev_dbg("%s\n", __func__);

	if (ctrl->rssi_unit_is_rx_samples) {
		if (is_update)
			return 0; /* no update required */

		rssi_delay = ctrl->rssi_delay;
		rssi_wait = ctrl->rssi_wait;
		rssi_duration = ctrl->rssi_duration;
	} else {
		/* update sample based on RX rate */
		rate = DIV_ROUND_CLOSEST(
			clk_get_rate(phy, phy->ref_clk_scale[RX_SAMPL_CLK]), 1000);
		/* units are in us */
		rssi_delay = DIV_ROUND_CLOSEST(ctrl->rssi_delay * rate, 1000);
		rssi_wait = DIV_ROUND_CLOSEST(ctrl->rssi_wait * rate, 1000);
		rssi_duration = DIV_ROUND_CLOSEST(
			ctrl->rssi_duration * rate, 1000);
	}

	if (ctrl->restart_mode == EN_AGC_PIN_IS_PULLED_HIGH)
		rssi_delay = 0;

	rssi_delay = clamp(rssi_delay / 8, 0U, 255U);
	rssi_wait = clamp(rssi_wait / 4, 0U, 255U);

	do {
		for (i = 14; rssi_duration > 0 && i >= 0 ; i--) {
			val = 1 << i;
			if (rssi_duration >= val) {
				dur_buf[j++] = i;
				total_dur += val;
				rssi_duration -= val;
				break;
			}
		}

	} while (j < 4 && rssi_duration > 0);

	for (i = 0, total_weight = 0; i < 4; i++)
		total_weight += weight[i] =
			DIV_ROUND_CLOSEST(RSSI_MAX_WEIGHT *
				(1 << dur_buf[i]), total_dur);

	/* total of all weights must be 0xFF */
	val = total_weight - 0xFF;
	weight[j - 1] -= val;

	ad9361_spi_write(REG_MEASURE_DURATION_01,
			 (dur_buf[1] << 4) | dur_buf[0]); // RSSI Measurement Duration 0, 1
	ad9361_spi_write(REG_MEASURE_DURATION_23,
			 (dur_buf[3] << 4) | dur_buf[2]); // RSSI Measurement Duration 2, 3
	ad9361_spi_write(REG_RSSI_WEIGHT_0, weight[0]); // RSSI Weighted Multiplier 0
	ad9361_spi_write(REG_RSSI_WEIGHT_1, weight[1]); // RSSI Weighted Multiplier 1
	ad9361_spi_write(REG_RSSI_WEIGHT_2, weight[2]); // RSSI Weighted Multiplier 2
	ad9361_spi_write(REG_RSSI_WEIGHT_3, weight[3]); // RSSI Weighted Multiplier 3
	ad9361_spi_write(REG_RSSI_DELAY, rssi_delay); // RSSI Delay
	ad9361_spi_write(REG_RSSI_WAIT_TIME, rssi_wait); // RSSI Wait

	temp = RSSI_MODE_SELECT(ctrl->restart_mode);
	if (ctrl->restart_mode == SPI_WRITE_TO_REGISTER)
		temp |= START_RSSI_MEAS;

	ret = ad9361_spi_write(REG_RSSI_CONFIG, temp); // RSSI Mode Select

	if (ret < 0)
		dev_err("Unable to write rssi config\n");

	return 0;
}

static int ad9361_ensm_set_state(struct ad9361_rf_phy *phy, u8 ensm_state)
{
	int rc = 0;
	u32 val;

	if (phy->curr_ensm_state == ensm_state) {
		dev_dbg("Nothing to do, device is already in %d state\n",
			ensm_state);
		goto out;
	}

	dev_dbg("Device is in %x state, moving to %x\n", phy->curr_ensm_state,
			ensm_state);

	val = phy->ensm_conf1;

	switch (ensm_state) {
	case ENSM_STATE_TX:
		val |= FORCE_TX_ON;
		if (phy->pdata->fdd)
			rc = -EINVAL;
		else if (phy->curr_ensm_state != ENSM_STATE_ALERT)
			rc = -EINVAL;
		break;
	case ENSM_STATE_RX:
		val |= FORCE_RX_ON;
		if (phy->pdata->fdd)
			rc = -EINVAL;
		else if (phy->curr_ensm_state != ENSM_STATE_ALERT)
			rc = -EINVAL;
		break;
	case ENSM_STATE_FDD:
		val |= (FORCE_TX_ON | FORCE_RX_ON);
		if (!phy->pdata->fdd)
			rc = -EINVAL;
		break;
	case ENSM_STATE_ALERT:
		val &= ~(FORCE_TX_ON | FORCE_RX_ON);
		val |= TO_ALERT | FORCE_ALERT_STATE;
		break;
	case ENSM_STATE_SLEEP_WAIT:
		break;
	default:
		dev_err("No handling for forcing %d ensm state\n",
		ensm_state);
		goto out;
	}

	if (rc) {
		dev_err("Invalid ENSM state transition in %s mode\n",
			phy->pdata->fdd ? "FDD" : "TDD");
		goto out;
	}

	rc = ad9361_spi_write(REG_ENSM_CONFIG_1, val);
	if (rc)
		dev_err("Failed to restore state\n");

	phy->curr_ensm_state = ensm_state;

out:
	return rc;

}

int ad9361_set_trx_clock_chain(struct ad9361_rf_phy *phy,
				      unsigned long *rx_path_clks,
				      unsigned long *tx_path_clks)
{
	int ret, i, j, n;

	dev_dbg("%s\n", __func__);

	dev_dbg("rx_path_clks = {%lu, %lu, %lu, %lu, %lu, %lu}\n", rx_path_clks[0], rx_path_clks[1], rx_path_clks[2], rx_path_clks[3], rx_path_clks[4], rx_path_clks[5]);
	dev_dbg("tx_path_clks = {%lu, %lu, %lu, %lu, %lu, %lu}\n", tx_path_clks[0], tx_path_clks[1], tx_path_clks[2], tx_path_clks[3], tx_path_clks[4], tx_path_clks[5]);

	if (!rx_path_clks || !tx_path_clks)
		return -EINVAL;

	ret = clk_set_rate(phy, phy->ref_clk_scale[BBPLL_CLK], rx_path_clks[BBPLL_FREQ]);
	if (ret < 0)
		return ret;

	for (i = ADC_CLK, j = DAC_CLK, n = ADC_FREQ;
		i <= RX_SAMPL_CLK; i++, j++, n++) {
		ret = clk_set_rate(phy, phy->ref_clk_scale[i], rx_path_clks[n]);
		if (ret < 0) {
			dev_err("Failed to set BB ref clock rate (%d)\n",
				ret);
			return ret;
		}
		ret = clk_set_rate(phy, phy->ref_clk_scale[j], tx_path_clks[n]);
		if (ret < 0) {
			dev_err("Failed to set BB ref clock rate (%d)\n",
				ret);
			return ret;
		}
	}

	return 0;
}

static int ad9361_get_trx_clock_chain(struct ad9361_rf_phy *phy, unsigned long *rx_path_clks,
				      unsigned long *tx_path_clks)
{
	int i, j, n;
	unsigned long bbpll_freq;

	if (!rx_path_clks && !tx_path_clks)
		return -EINVAL;

	bbpll_freq = clk_get_rate(phy, phy->ref_clk_scale[BBPLL_CLK]);

	if (rx_path_clks)
		rx_path_clks[BBPLL_FREQ] = bbpll_freq;

	if (tx_path_clks)
		tx_path_clks[BBPLL_FREQ] = bbpll_freq;

	for (i = ADC_CLK, j = DAC_CLK, n = ADC_FREQ;
		i <= RX_SAMPL_CLK; i++, j++, n++) {
		if (rx_path_clks)
			rx_path_clks[n] = clk_get_rate(phy, phy->ref_clk_scale[i]);
		if (tx_path_clks)
			tx_path_clks[n] = clk_get_rate(phy, phy->ref_clk_scale[j]);
	}

	return 0;
}

int ad9361_calculate_rf_clock_chain(struct ad9361_rf_phy *phy,
				      unsigned long tx_sample_rate,
				      u32 low_power,
				      unsigned long *rx_path_clks,
				      unsigned long *tx_path_clks)
{
	unsigned long clktf, clkrf, adc_rate = 0, dac_rate = 0, tmp;
	u64 bbpll_rate;
	int i, index_rx = -1, index_tx = -1;
	u32 div, tx_intdec, rx_intdec;
	const char clk_dividers[][4] = {
		{12,3,2,2},
		{8,2,2,2},
		{6,3,1,2},
		{4,2,2,1},
		{3,3,1,1},
		{2,2,1,1},
		{1,1,1,1},
	};


	if (phy->bypass_rx_fir)
		rx_intdec = 1;
	else
		rx_intdec = phy->rx_fir_dec;

	if (phy->bypass_tx_fir)
		tx_intdec = 1;
	else
		tx_intdec = phy->tx_fir_int;


	dev_dbg("%s: requested rate %lu TXFIR int %d RXFIR dec %d mode %s\n",
		__func__, tx_sample_rate, tx_intdec, rx_intdec,
		low_power ? "Medium PWR" : "Highest OSR");

	if (tx_sample_rate > (phy->pdata->rx2tx2 ? 61440000UL : 122880000UL))
		return -EINVAL;

	clktf = tx_sample_rate * tx_intdec;
	clkrf = tx_sample_rate * rx_intdec * (phy->rx_eq_2tx ? 2 : 1);

	for (i = low_power; i < 7; i++) {
		adc_rate = clkrf * clk_dividers[i][0];
		dac_rate = clktf * clk_dividers[i][0];
		if ((adc_rate <= MAX_ADC_CLK) && (adc_rate >= MIN_ADC_CLK)) {
			tmp = adc_rate / dac_rate;
			if (adc_rate <= MAX_DAC_CLK) {
				index_rx = i;
				index_tx = i - ((tmp == 1) ? 0 : tmp);
				dac_rate = adc_rate; /* ADC_CLK */
				break;
			} else {
				dac_rate = adc_rate / 2;  /* ADC_CLK/2 */
				index_tx = i + 2 - ((tmp == 1) ? 0 : tmp);
				index_rx = i;
				break;
			}
		}
	}

	if ((index_tx < 0 || index_tx > 6 || index_rx < 0 || index_rx > 6) && low_power < 7) {
		return ad9361_calculate_rf_clock_chain(phy, tx_sample_rate,
			++low_power, rx_path_clks, tx_path_clks);
	} else if ((index_tx < 0 || index_tx > 6 || index_rx < 0 || index_rx > 6)) {
		dev_err("%s: Failed to find suitable dividers: %s\n",
		__func__, (adc_rate < MIN_ADC_CLK) ? "ADC clock below limit" : "BBPLL rate above limit");

		return -EINVAL;
	}

	/* Calculate target BBPLL rate */
	div = MAX_BBPLL_DIV;

	do {
		bbpll_rate = (u64)adc_rate * div;
		div >>= 1;

	} while ((bbpll_rate > MAX_BBPLL_FREQ) && (div >= MIN_BBPLL_DIV));

	rx_path_clks[BBPLL_FREQ] = bbpll_rate;
	rx_path_clks[ADC_FREQ] = adc_rate;
	rx_path_clks[R2_FREQ] = rx_path_clks[ADC_FREQ] / clk_dividers[index_rx][1];
	rx_path_clks[R1_FREQ] = rx_path_clks[R2_FREQ] / clk_dividers[index_rx][2];
	rx_path_clks[CLKRF_FREQ] = rx_path_clks[R1_FREQ] / clk_dividers[index_rx][3];
	rx_path_clks[RX_SAMPL_FREQ] = rx_path_clks[CLKRF_FREQ] / 	rx_intdec;

	tx_path_clks[BBPLL_FREQ] = bbpll_rate;
	tx_path_clks[DAC_FREQ] = dac_rate;
	tx_path_clks[T2_FREQ] = tx_path_clks[DAC_FREQ] / clk_dividers[index_tx][1];
	tx_path_clks[T1_FREQ] =tx_path_clks[T2_FREQ] / clk_dividers[index_tx][2];
	tx_path_clks[CLKTF_FREQ] = tx_path_clks[T1_FREQ] / clk_dividers[index_tx][3];
	tx_path_clks[TX_SAMPL_FREQ] = tx_path_clks[CLKTF_FREQ] / 	tx_intdec;

	dev_dbg("%s: %lu %lu %lu %lu %lu %lu\n",
		__func__, rx_path_clks[BBPLL_FREQ], rx_path_clks[ADC_FREQ],
		rx_path_clks[R2_FREQ], rx_path_clks[R1_FREQ],
		rx_path_clks[CLKRF_FREQ], rx_path_clks[RX_SAMPL_FREQ]);

	dev_dbg("%s: %lu %lu %lu %lu %lu %lu\n",
		__func__, tx_path_clks[BBPLL_FREQ], tx_path_clks[ADC_FREQ],
		tx_path_clks[R2_FREQ], tx_path_clks[R1_FREQ],
		tx_path_clks[CLKRF_FREQ], tx_path_clks[RX_SAMPL_FREQ]);

	return 0;
}

int ad9361_setup(struct ad9361_rf_phy *phy)
{
	unsigned long refin_Hz, ref_freq, bbpll_freq;
	struct ad9361_phy_platform_data *pd = phy->pdata;
	int ret;
	u32 real_rx_bandwidth = pd->rf_rx_bandwidth_Hz / 2;
	u32 real_tx_bandwidth = pd->rf_tx_bandwidth_Hz / 2;

	dev_dbg("%s\n", __func__);

	if (pd->port_ctrl.pp_conf[2] & FDD_RX_RATE_2TX_RATE)
		phy->rx_eq_2tx = true;

	phy->ensm_conf1 =
		(pd->ensm_pin_level_mode ? LEVEL_MODE : 0) |
		(pd->ensm_pin_ctrl ? ENABLE_ENSM_PIN_CTRL : 0) |
		TO_ALERT;

	ad9361_spi_write(REG_SPI_CONF, SOFT_RESET | _SOFT_RESET); /* RESET */
	ad9361_spi_write(REG_SPI_CONF, 0x0);

	ad9361_spi_write(REG_CTRL, CTRL_ENABLE);
	ad9361_spi_write(REG_BANDGAP_CONFIG0, MASTER_BIAS_TRIM(0x0E)); /* Enable Master Bias */
	ad9361_spi_write(REG_BANDGAP_CONFIG1, BANDGAP_TEMP_TRIM(0x0E)); /* Set Bandgap Trim */

	ad9361_set_dcxo_tune(phy, pd->dcxo_coarse, pd->dcxo_fine);

	//refin_Hz = clk_get_rate(phy->clk_refin);
	refin_Hz = 40000000UL;

	if (refin_Hz < 40000000UL)
		ref_freq = 2 * refin_Hz;
	else if (refin_Hz < 80000000UL)
		ref_freq = refin_Hz;
	else if (refin_Hz < 160000000UL)
		ref_freq = refin_Hz / 2;
	else if (refin_Hz < 320000000UL)
		ref_freq = refin_Hz / 4;
	else
		return -EINVAL;

	ret = clk_set_rate(phy, phy->ref_clk_scale[RX_REFCLK], ref_freq);
	if (ret < 0) {
		dev_err("Failed to set RX Synth ref clock rate (%d)\n", ret);
		return ret;
	}

	ret = clk_set_rate(phy, phy->ref_clk_scale[TX_REFCLK], ref_freq);
	if (ret < 0) {
		dev_err("Failed to set TX Synth ref clock rate (%d)\n", ret);
		return ret;
	}

 	ret = clk_prepare_enable(phy->clks[RX_REFCLK]);
	if (ret < 0) {
		dev_err("Failed to enable RX Synth ref clock (%d)\n", ret);
		return ret;
	}

 	ret = clk_prepare_enable(phy->clks[TX_REFCLK]);
	if (ret < 0) {
		dev_err("Failed to enable TX Synth ref clock (%d)\n", ret);
		return ret;
	}

	ad9361_spi_writef(REG_REF_DIVIDE_CONFIG_1, RX_REF_RESET_BAR, 1);
	ad9361_spi_writef(REG_REF_DIVIDE_CONFIG_2,
			  TX_REF_DOUBLER_FB_DELAY(~0), 3); /* FB DELAY */
	ad9361_spi_writef(REG_REF_DIVIDE_CONFIG_2,
			  RX_REF_DOUBLER_FB_DELAY(~0), 3); /* FB DELAY */

	ad9361_spi_write(REG_CLOCK_ENABLE,
			DIGITAL_POWER_UP | CLOCK_ENABLE_DFLT | BBPLL_ENABLE |
			(pd->use_extclk ? XO_BYPASS : 0)); /* Enable Clocks */

	ret = clk_set_rate(phy, phy->ref_clk_scale[BB_REFCLK], ref_freq);
	if (ret < 0) {
		dev_err("Failed to set BB ref clock rate (%d)\n",
			ret);
		return ret;
	}

	ret = clk_prepare_enable(phy->clks[BB_REFCLK]);
	if (ret < 0) {
		dev_err("Failed to enable BB ref clock rate (%d)\n",
			ret);
		return ret;
	}

	ret = ad9361_set_trx_clock_chain(phy, pd->rx_path_clks,
				   pd->tx_path_clks);
	if (ret < 0)
		return ret;

	ad9361_en_dis_tx(phy, 1, TX_ENABLE);
	ad9361_en_dis_rx(phy, 1, RX_ENABLE);

	if (pd->rx2tx2) {
		ad9361_en_dis_tx(phy, 2, TX_ENABLE);
		ad9361_en_dis_rx(phy, 2, RX_ENABLE);
	}

	ret = ad9361_rf_port_setup(phy, pd->rf_rx_input_sel,
				   pd->rf_tx_output_sel);
	if (ret < 0)
		return ret;

	ret = ad9361_pp_port_setup(phy, false);
	if (ret < 0)
		return ret;

	ret = ad9361_auxdac_setup(phy);
	if (ret < 0)
		return ret;

	bbpll_freq = clk_get_rate(phy, phy->ref_clk_scale[BBPLL_CLK]);

	ret = ad9361_auxadc_setup(phy, &pd->auxadc_ctrl, bbpll_freq);
	if (ret < 0)
		return ret;

	ret = ad9361_ctrl_outs_setup(phy, &pd->ctrl_outs_ctrl);
	if (ret < 0)
		return ret;

	ret = ad9361_gpo_setup(phy);
	if (ret < 0)
		return ret;

	ret = ad9361_set_ref_clk_cycles(phy, refin_Hz);
	if (ret < 0)
		return ret;

	ret = ad9361_setup_ext_lna(phy, pd->elna_ctrl.gain_mdB,
			pd->elna_ctrl.bypass_loss_mdB);
	if (ret < 0)
		return ret;

	ret = ad9361_txrx_synth_cp_calib(phy, ref_freq, false); /* RXCP */
	if (ret < 0)
		return ret;

	ret = ad9361_txrx_synth_cp_calib(phy, ref_freq, true); /* TXCP */
	if (ret < 0)
		return ret;

	ret = clk_set_rate(phy, phy->ref_clk_scale[RX_RFPLL], ad9361_to_clk(pd->rx_synth_freq));
	if (ret < 0) {
		dev_err("Failed to set RX Synth rate (%d)\n",
			ret);
		return ret;
	}

	ret = clk_prepare_enable(phy->clks[RX_RFPLL]);
	if (ret < 0)
		return ret;

	ret = clk_set_rate(phy, phy->ref_clk_scale[TX_RFPLL], ad9361_to_clk(pd->tx_synth_freq));
	if (ret < 0) {
		dev_err("Failed to set TX Synth rate (%d)\n",
			ret);
		return ret;
	}

	ret = clk_prepare_enable(phy->clks[TX_RFPLL]);
	if (ret < 0)
		return ret;

	ret = ad9361_load_mixer_gm_subtable(phy);
	if (ret < 0)
		return ret;

	ret = ad9361_gc_setup(phy, &pd->gain_ctrl);
	if (ret < 0)
		return ret;

	ret = ad9361_rx_bb_analog_filter_calib(phy,
				real_rx_bandwidth,
				bbpll_freq);
	if (ret < 0)
		return ret;

	ret = ad9361_tx_bb_analog_filter_calib(phy,
				real_tx_bandwidth,
				bbpll_freq);
	if (ret < 0)
		return ret;

	ret = ad9361_rx_tia_calib(phy, real_rx_bandwidth);
	if (ret < 0)
		return ret;

	ret = ad9361_tx_bb_second_filter_calib(phy, pd->rf_tx_bandwidth_Hz);
	if (ret < 0)
		return ret;

	ret = ad9361_rx_adc_setup(phy,
				bbpll_freq,
				clk_get_rate(phy, phy->ref_clk_scale[ADC_CLK]));
	if (ret < 0)
		return ret;

	ret = ad9361_bb_dc_offset_calib(phy);
	if (ret < 0)
		return ret;

	ret = ad9361_rf_dc_offset_calib(phy,
			ad9361_from_clk(clk_get_rate(phy, phy->ref_clk_scale[RX_RFPLL])));
	if (ret < 0)
		return ret;

	ret = ad9361_tx_quad_calib(phy, real_tx_bandwidth, -1);
	if (ret < 0)
		return ret;

	ret = ad9361_tracking_control(phy, phy->bbdc_track_en,
			phy->rfdc_track_en, phy->quad_track_en);
	if (ret < 0)
		return ret;

	if (!pd->fdd)
		ad9361_run_calibration(phy, TXMON_CAL);

	ad9361_pp_port_setup(phy, true);

	ad9361_spi_write(REG_ENSM_MODE, pd->fdd ? FDD_MODE : 0x00);

	if (pd->fdd)
		ad9361_spi_write(REG_ENSM_CONFIG_2,
			DUAL_SYNTH_MODE |
			(pd->ensm_pin_ctrl ? FDD_EXTERNAL_CTRL_ENABLE : 0)); /* Dual Synth */
	 else    /* For now in TDD always use Dual Synth */
		ad9361_spi_write(REG_ENSM_CONFIG_2, DUAL_SYNTH_MODE);

	ret = ad9361_set_tx_atten(phy, pd->tx_atten, true, true);
	if (ret < 0)
		return ret;

	ret = ad9361_rssi_setup(phy, &pd->rssi_ctrl, false);
	if (ret < 0)
		return ret;

	phy->curr_ensm_state = ad9361_spi_readf(REG_STATE, ENSM_STATE(~0));
	ad9361_ensm_set_state(phy, pd->fdd ? ENSM_STATE_FDD : ENSM_STATE_RX);

	phy->current_rx_bw_Hz = pd->rf_rx_bandwidth_Hz;
	phy->current_tx_bw_Hz = pd->rf_tx_bandwidth_Hz;
	phy->auto_cal_en = true;
	phy->cal_threshold_freq = 100000000ULL; /* 100 MHz */

	return 0;

}

static int ad9361_do_calib_run(struct ad9361_rf_phy *phy, u32 cal, int arg)
{
	int ret;

	ret = ad9361_tracking_control(phy, false, false, false);
	if (ret < 0)
		return ret;

	ad9361_ensm_force_state(phy, ENSM_STATE_ALERT);

	switch (cal) {
	case TX_QUAD_CAL:
		ret = ad9361_tx_quad_calib(phy, phy->current_tx_bw_Hz / 2, arg);
		break;
	case RX_QUAD_CAL:
		ret = ad9361_rx_quad_calib(phy, phy->current_rx_bw_Hz / 2);
		break;
	case RFDC_CAL:
		ret = ad9361_rf_dc_offset_calib(phy,
			ad9361_from_clk(clk_get_rate(phy, phy->ref_clk_scale[RX_RFPLL])));
		break;
	default:
		ret = -EINVAL;
		break;
	}

	ret = ad9361_tracking_control(phy, phy->bbdc_track_en,
			phy->rfdc_track_en, phy->quad_track_en);
	ad9361_ensm_restore_prev_state(phy);

	return ret;
}

int ad9361_update_rf_bandwidth(struct ad9361_rf_phy *phy,
				     u32 rf_rx_bw, u32 rf_tx_bw)
{
	unsigned long bbpll_freq;
	u32 real_rx_bandwidth = rf_rx_bw / 2;
	u32 real_tx_bandwidth = rf_tx_bw / 2;
	int ret;

	bbpll_freq = clk_get_rate(phy, phy->ref_clk_scale[BBPLL_CLK]);

	ret = ad9361_tracking_control(phy, false, false, false);
	if (ret < 0)
		return ret;


	ad9361_ensm_force_state(phy, ENSM_STATE_ALERT);

	ret = ad9361_rx_bb_analog_filter_calib(phy,
				real_rx_bandwidth,
				bbpll_freq);
	if (ret < 0)
		return ret;

	ret = ad9361_tx_bb_analog_filter_calib(phy,
				real_tx_bandwidth,
				bbpll_freq);
	if (ret < 0)
		return ret;

	ret = ad9361_rx_tia_calib(phy, real_rx_bandwidth);
	if (ret < 0)
		return ret;

	ret = ad9361_tx_bb_second_filter_calib(phy, rf_tx_bw);
	if (ret < 0)
		return ret;

	ret = ad9361_rx_adc_setup(phy,
				bbpll_freq,
				clk_get_rate(phy, phy->ref_clk_scale[ADC_CLK]));
	if (ret < 0)
		return ret;

	ret = ad9361_tx_quad_calib(phy, real_tx_bandwidth, -1);
	if (ret < 0)
		return ret;

	phy->current_rx_bw_Hz = rf_rx_bw;
	phy->current_tx_bw_Hz = rf_tx_bw;

	ret = ad9361_tracking_control(phy, phy->bbdc_track_en,
			phy->rfdc_track_en, phy->quad_track_en);
	if (ret < 0)
		return ret;

	ad9361_ensm_restore_prev_state(phy);

	return 0;
}

int ad9361_load_fir_filter_coef(struct ad9361_rf_phy *phy,
				       enum fir_dest dest, int gain_dB,
				       u32 ntaps, short *coef)
{
	u32 val, offs = 0, fir_conf = 0;

	dev_dbg("%s: TAPS %d, gain %d, dest %d",
		__func__, ntaps, gain_dB, dest);

	if (coef == NULL || !ntaps || ntaps > 128 || ntaps % 16) {
		dev_err("%s: Invalid parameters: TAPS %d, gain %d, dest 0x%X",
			__func__, ntaps, gain_dB, dest);

		return -EINVAL;
	}

	if (dest & FIR_IS_RX) {
		val = 3 - (gain_dB + 12) / 6;
		ad9361_spi_write(REG_RX_FILTER_GAIN, val & 0x3);
		offs = REG_RX_FILTER_COEF_ADDR - REG_TX_FILTER_COEF_ADDR;
		phy->rx_fir_ntaps = ntaps;
	} else {
		if (gain_dB == -6)
			fir_conf = TX_FIR_GAIN_6DB;
		phy->tx_fir_ntaps = ntaps;
	}

	val = ntaps / 16 - 1;

	fir_conf |= FIR_NUM_TAPS(val) | FIR_SELECT(dest) | FIR_START_CLK;

	ad9361_spi_write(REG_TX_FILTER_CONF + offs, fir_conf);

	for (val = 0; val < ntaps; val++) {
		ad9361_spi_write(REG_TX_FILTER_COEF_ADDR + offs, val);
		ad9361_spi_write(REG_TX_FILTER_COEF_WRITE_DATA_1 + offs,
				 coef[val] & 0xFF);
		ad9361_spi_write(REG_TX_FILTER_COEF_WRITE_DATA_2 + offs,
				 coef[val] >> 8);
		ad9361_spi_write(REG_TX_FILTER_CONF + offs,
				 fir_conf | FIR_WRITE);
		ad9361_spi_write(REG_TX_FILTER_COEF_READ_DATA_2 + offs, 0);
		ad9361_spi_write(REG_TX_FILTER_COEF_READ_DATA_2 + offs, 0);
	}

	ad9361_spi_write(REG_TX_FILTER_CONF + offs, fir_conf);
	fir_conf &= ~FIR_START_CLK;
	ad9361_spi_write(REG_TX_FILTER_CONF + offs, fir_conf);

	return 0;
}

static int ad9361_parse_fir(struct ad9361_rf_phy *phy,
				 char *data, u32 size)
{
	char *line;
	int i = 0, ret, txc, rxc;
	int tx = -1, tx_gain, tx_int;
	int rx = -1, rx_gain, rx_dec;
	short coef_tx[128];
	short coef_rx[128];
	char *ptr = data;

	while ((line = strsep(&ptr, "\n"))) {
		if (line >= data + size) {
			break;
		}

		if (line[0] == '#')
			continue;

		if (tx < 0) {
			ret = sscanf(line, "TX %d GAIN %d INT %d",
				     &tx, &tx_gain, &tx_int);
			if (ret == 3)
				continue;
			else
				tx = -1;
		}
		if (rx < 0) {
			ret = sscanf(line, "RX %d GAIN %d DEC %d",
				     &rx, &rx_gain, &rx_dec);
			if (ret == 3)
				continue;
			else
				tx = -1;
		}
		ret = sscanf(line, "%d,%d", &txc, &rxc);
		if (ret == 1) {
			coef_tx[i] = coef_rx[i] = (short)txc;
			i++;
			continue;
		} else if (ret == 2) {
			coef_tx[i] = (short)txc;
			coef_rx[i] = (short)rxc;
			i++;
			continue;
		}
	}

	switch (tx) {
	case FIR_TX1:
	case FIR_TX2:
	case FIR_TX1_TX2:
		ret = ad9361_load_fir_filter_coef(phy, tx, tx_gain, i, coef_tx);
		phy->tx_fir_int = tx_int;
		break;
	default:
		ret = -EINVAL;
	}

	switch (rx | FIR_IS_RX) {
	case FIR_RX1:
	case FIR_RX2:
	case FIR_RX1_RX2:
		ret = ad9361_load_fir_filter_coef(phy, rx | FIR_IS_RX,
						  rx_gain, i, coef_rx);
		phy->rx_fir_dec = rx_dec;
		break;
	default:
		ret = -EINVAL;
	}

	if (ret < 0)
		return ret;


	return size;
}


int ad9361_validate_enable_fir(struct ad9361_rf_phy *phy)
{
	int ret;
	unsigned long rx[6], tx[6];
	u32 max;

	dev_dbg("%s: TX FIR EN=%d/TAPS%d/INT%d, RX FIR EN=%d/TAPS%d/DEC%d\n",
		__func__, !phy->bypass_tx_fir, phy->tx_fir_ntaps, phy->tx_fir_int,
		!phy->bypass_rx_fir, phy->rx_fir_ntaps, phy->rx_fir_dec);

	if (!phy->bypass_tx_fir) {
		if (!(phy->tx_fir_int == 1 || phy->tx_fir_int == 2 ||
			phy->tx_fir_int == 4)) {
			dev_err("%s: Invalid: Interpolation %d in filter config\n",
				__func__, phy->tx_fir_int);
			return -EINVAL;
		}


		if (phy->tx_fir_int == 1 && phy->tx_fir_ntaps > 64) {
			dev_err("%s: Invalid: TAPS > 64 and Interpolation = 1\n",
				__func__);
			return -EINVAL;
		}
	}

	if (!phy->bypass_rx_fir) {
		if (!(phy->rx_fir_dec == 1 || phy->rx_fir_dec == 2 ||
			phy->rx_fir_dec == 4)) {
			dev_err("%s: Invalid: Decimation %d in filter config\n",
				__func__, phy->rx_fir_dec);

			return -EINVAL;
		}
	}

	ret = ad9361_calculate_rf_clock_chain(phy,
			clk_get_rate(phy, phy->ref_clk_scale[TX_SAMPL_CLK]),
			phy->rate_governor, rx, tx);

	if (ret < 0) {
		dev_err("%s: Calculating filter rates failed %d\n",
			__func__, ret);

		return ret;
	}

	if (!phy->bypass_tx_fir) {
		max = ((rx[ADC_FREQ] / 2) / tx[TX_SAMPL_FREQ]) * 16;
		if (phy->tx_fir_ntaps > max) {
			dev_err("%s: Invalid: ratio ADC/2 / TX_SAMPL * 16 > TAPS"
				"(max %d, adc %lu, tx %lu)",
				__func__, max, rx[ADC_FREQ], tx[TX_SAMPL_FREQ]);
			return -EINVAL;
		}
	}

	if (!phy->bypass_rx_fir) {
		max = ((rx[ADC_FREQ] / 2) / rx[RX_SAMPL_FREQ]) * 16;
		if (phy->rx_fir_ntaps > max) {
			dev_err("%s: Invalid: ratio ADC/2 / RX_SAMPL * 16 > TAPS (max %d)",
				__func__, max);
			return -EINVAL;
		}
	}

	ret = ad9361_set_trx_clock_chain(phy, rx, tx);
	if (ret < 0)
		return ret;

	/*
	 * Workaround for clock framework since clocks don't change the we
	 * manually need to enable the filter
	 */

	if (phy->rx_fir_dec == 1 || phy->bypass_rx_fir) {
		ad9361_spi_writef(REG_RX_ENABLE_FILTER_CTRL,
			RX_FIR_ENABLE_DECIMATION(~0), !phy->bypass_rx_fir);
	}

	if (phy->tx_fir_int == 1 || phy->bypass_tx_fir) {
		ad9361_spi_writef(REG_TX_ENABLE_FILTER_CTRL,
			TX_FIR_ENABLE_INTERPOLATION(~0), !phy->bypass_tx_fir);
	}

	return ad9361_update_rf_bandwidth(phy, phy->current_rx_bw_Hz,
			phy->current_tx_bw_Hz);
}
#ifdef LINUX
void ad9361_work_func(struct work_struct *work)
{
	struct ad9361_rf_phy *phy =
		container_of(work, struct ad9361_rf_phy, work);
	int ret;

	dev_dbg("%s:", __func__);

	ret = ad9361_do_calib_run(phy, TX_QUAD_CAL, -1);
	if (ret < 0)
		dev_err("%s: TX QUAD cal failed", __func__);

	complete_all(&phy->complete);
	clear_bit(0, &phy->flags);
}
#endif
/*
 * AD9361 Clocks
 */

#define to_clk_priv(_hw) container_of(_hw, struct refclk_scale, hw)

static inline int ad9361_set_muldiv(struct refclk_scale *priv, u32 mul, u32 div)
{
	priv->mult = mul;
	priv->div = div;
	return 0;
}

static int ad9361_get_clk_scaler(struct refclk_scale *clk_priv)
{
//	struct refclk_scale *clk_priv = to_clk_priv(hw);
	u32 tmp, tmp1;

	switch (clk_priv->source) {
	case BB_REFCLK:
		tmp = ad9361_spi_read(REG_CLOCK_CTRL);
		tmp &= 0x3;
		break;
	case RX_REFCLK:
		tmp = ad9361_spi_readf(REG_REF_DIVIDE_CONFIG_1,
					RX_REF_DIVIDER_MSB);
		tmp1 = ad9361_spi_readf(REG_REF_DIVIDE_CONFIG_2,
					RX_REF_DIVIDER_LSB);
		tmp = (tmp << 1) | tmp1;
		break;
	case TX_REFCLK:
		tmp = ad9361_spi_readf(REG_REF_DIVIDE_CONFIG_2,
				       TX_REF_DIVIDER(~0));
		break;
	case ADC_CLK:
		tmp = ad9361_spi_read(REG_BBPLL);
		return ad9361_set_muldiv(clk_priv, 1, 1 << (tmp & 0x7));
	case R2_CLK:
		tmp = ad9361_spi_readf(REG_RX_ENABLE_FILTER_CTRL,
				       DEC3_ENABLE_DECIMATION(~0));
		return ad9361_set_muldiv(clk_priv, 1, tmp + 1);
	case R1_CLK:
		tmp = ad9361_spi_readf(REG_RX_ENABLE_FILTER_CTRL, RHB2_EN);
		return ad9361_set_muldiv(clk_priv, 1, tmp + 1);
	case CLKRF_CLK:
		tmp = ad9361_spi_readf(REG_RX_ENABLE_FILTER_CTRL, RHB1_EN);
		return ad9361_set_muldiv(clk_priv, 1, tmp + 1);
	case RX_SAMPL_CLK:
		tmp = ad9361_spi_readf(REG_RX_ENABLE_FILTER_CTRL,
				       RX_FIR_ENABLE_DECIMATION(~0));

		if (!tmp)
			tmp = 1; /* bypass filter */
		else
			tmp = (1 << (tmp - 1));

		return ad9361_set_muldiv(clk_priv, 1, tmp);
	case DAC_CLK:
		tmp = ad9361_spi_readf(REG_BBPLL, BIT(3));
		return ad9361_set_muldiv(clk_priv, 1, tmp + 1);
	case T2_CLK:
		tmp = ad9361_spi_readf(REG_TX_ENABLE_FILTER_CTRL,
				       THB3_ENABLE_INTERP(~0));
		return ad9361_set_muldiv(clk_priv, 1, tmp + 1);
	case T1_CLK:
		tmp = ad9361_spi_readf(REG_TX_ENABLE_FILTER_CTRL, THB2_EN);
		return ad9361_set_muldiv(clk_priv, 1, tmp + 1);
	case CLKTF_CLK:
		tmp = ad9361_spi_readf(REG_TX_ENABLE_FILTER_CTRL, THB1_EN);
		return ad9361_set_muldiv(clk_priv, 1, tmp + 1);
	case TX_SAMPL_CLK:
		tmp = ad9361_spi_readf(REG_TX_ENABLE_FILTER_CTRL,
				       TX_FIR_ENABLE_INTERPOLATION(~0));

		if (!tmp)
			tmp = 1; /* bypass filter */
		else
			tmp = (1 << (tmp - 1));

		return ad9361_set_muldiv(clk_priv, 1, tmp);
	default:
		return -EINVAL;
	}

	/* REFCLK Scaler */
	switch (tmp) {
	case 0:
		ad9361_set_muldiv(clk_priv, 1, 1);
		break;
	case 1:
		ad9361_set_muldiv(clk_priv, 1, 2);
		break;
	case 2:
		ad9361_set_muldiv(clk_priv, 1, 4);
		break;
	case 3:
		ad9361_set_muldiv(clk_priv, 2, 1);
		break;
	default:
		return -EINVAL;

	}

	return 0;
}

static int ad9361_to_refclk_scaler(struct refclk_scale *clk_priv)
{
	/* REFCLK Scaler */
	switch (((clk_priv->mult & 0xF) << 4) | (clk_priv->div & 0xF)) {
	case 0x11:
		return 0;
	case 0x12:
		return 1;
	case 0x14:
		return 2;
	case 0x21:
		return 3;
	default:
		return -EINVAL;
	}
};

static int ad9361_set_clk_scaler(struct refclk_scale *clk_priv, bool set)
{
//	struct refclk_scale *clk_priv = to_clk_priv(hw);
	u32 tmp;
	int ret;

	switch (clk_priv->source) {
	case BB_REFCLK:
		ret = ad9361_to_refclk_scaler(clk_priv);
		if (ret < 0)
			return ret;
		if (set)
			return ad9361_spi_writef(REG_CLOCK_CTRL, 0x3, ret);
		break;

	case RX_REFCLK:
		ret = ad9361_to_refclk_scaler(clk_priv);
		if (ret < 0)
			return ret;
		if (set) {
			tmp = ret;
			ret = ad9361_spi_writef(REG_REF_DIVIDE_CONFIG_1,
						RX_REF_DIVIDER_MSB, tmp >> 1);
			ret |= ad9361_spi_writef(REG_REF_DIVIDE_CONFIG_2,
						 RX_REF_DIVIDER_LSB, tmp & 1);
			return ret;
		}
		break;
	case TX_REFCLK:
		ret = ad9361_to_refclk_scaler(clk_priv);
		if (ret < 0)
			return ret;
		if (set)
			return ad9361_spi_writef(REG_REF_DIVIDE_CONFIG_2,
						0x0C, ret);
		break;
	case ADC_CLK:
		tmp = ilog2((u8)clk_priv->div);
		if (clk_priv->mult != 1 || tmp > 6 || tmp < 1)
			return -EINVAL;

		if (set)
			return ad9361_spi_writef(REG_BBPLL, 0x7, tmp);
		break;
	case R2_CLK:
		if (clk_priv->mult != 1 || clk_priv->div > 3 || clk_priv->div < 1)
			return -EINVAL;
		if (set)
			return ad9361_spi_writef(REG_RX_ENABLE_FILTER_CTRL,
						 DEC3_ENABLE_DECIMATION(~0),
						 clk_priv->div - 1);
		break;
	case R1_CLK:
		if (clk_priv->mult != 1 || clk_priv->div > 2 || clk_priv->div < 1)
			return -EINVAL;
		if (set)
			return ad9361_spi_writef(REG_RX_ENABLE_FILTER_CTRL,
						 RHB2_EN, clk_priv->div - 1);
		break;
	case CLKRF_CLK:
		if (clk_priv->mult != 1 || clk_priv->div > 2 || clk_priv->div < 1)
			return -EINVAL;
		if (set)
			return ad9361_spi_writef(REG_RX_ENABLE_FILTER_CTRL,
						 RHB1_EN, clk_priv->div - 1);
		break;
	case RX_SAMPL_CLK:
		if (clk_priv->mult != 1 || clk_priv->div > 4 ||
			clk_priv->div < 1 || clk_priv->div == 3)
			return -EINVAL;

		if (clk_priv->phy->bypass_rx_fir)
			tmp = 0;
		else
			tmp = ilog2(clk_priv->div) + 1;

		if (set)
			return ad9361_spi_writef(REG_RX_ENABLE_FILTER_CTRL,
						 RX_FIR_ENABLE_DECIMATION(~0), tmp);
		break;
	case DAC_CLK:
		if (clk_priv->mult != 1 || clk_priv->div > 2 || clk_priv->div < 1)
			return -EINVAL;
		if (set)
			return ad9361_spi_writef(REG_BBPLL,
						 BIT(3), clk_priv->div - 1);
		break;
	case T2_CLK:
		if (clk_priv->mult != 1 || clk_priv->div > 3 || clk_priv->div < 1)
			return -EINVAL;
		if (set)
			return ad9361_spi_writef(REG_TX_ENABLE_FILTER_CTRL,
						 THB3_ENABLE_INTERP(~0),
						 clk_priv->div - 1);
		break;
	case T1_CLK:
		if (clk_priv->mult != 1 || clk_priv->div > 2 || clk_priv->div < 1)
			return -EINVAL;
		if (set)
			return ad9361_spi_writef(REG_TX_ENABLE_FILTER_CTRL,
						 THB2_EN, clk_priv->div - 1);
		break;
	case CLKTF_CLK:
		if (clk_priv->mult != 1 || clk_priv->div > 2 || clk_priv->div < 1)
			return -EINVAL;
		if (set)
			return ad9361_spi_writef(REG_TX_ENABLE_FILTER_CTRL,
						 THB1_EN, clk_priv->div - 1);
		break;
	case TX_SAMPL_CLK:
		if (clk_priv->mult != 1 || clk_priv->div > 4 ||
			clk_priv->div < 1 || clk_priv->div == 3)
			return -EINVAL;

		if (clk_priv->phy->bypass_tx_fir)
			tmp = 0;
		else
			tmp = ilog2(clk_priv->div) + 1;

		if (set)
			return ad9361_spi_writef(REG_TX_ENABLE_FILTER_CTRL,
					TX_FIR_ENABLE_INTERPOLATION(~0), tmp);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

unsigned long ad9361_clk_factor_recalc_rate(struct refclk_scale *clk_priv,
		unsigned long parent_rate)
{
//	struct refclk_scale *clk_priv = to_clk_priv(hw);
	u64 rate;

	ad9361_get_clk_scaler(clk_priv);
	rate = (parent_rate * clk_priv->mult) / clk_priv->div;

	return (unsigned long)rate;
}

long ad9361_clk_factor_round_rate(struct refclk_scale *clk_priv, unsigned long rate,
				unsigned long *prate)
{
//	struct refclk_scale *clk_priv = to_clk_priv(hw);
	int ret;

	if (rate >= *prate) {
		clk_priv->mult = DIV_ROUND_CLOSEST(rate, *prate);
		clk_priv->div = 1;

	} else {
		clk_priv->div = DIV_ROUND_CLOSEST(*prate, rate);
		clk_priv->mult = 1;
	}

	ret = ad9361_set_clk_scaler(clk_priv, false);
	if (ret < 0)
		return ret;

	return (*prate / clk_priv->div) * clk_priv->mult;
}

int ad9361_clk_factor_set_rate(struct refclk_scale *clk_priv, unsigned long rate,
				unsigned long parent_rate)
{
//	struct refclk_scale *clk_priv = to_clk_priv(hw);
	char   message[16][12] = {"BB_REFCLK", "RX_REFCLK", "TX_REFCLK", "BBPLL_CLK", "ADC_CLK",
		"R2_CLK", "R1_CLK", "CLKRF_CLK", "RX_SAMPL_CLK", "DAC_CLK", "T2_CLK",
		"T1_CLK", "CLKTF_CLK", "TX_SAMPL_CLK", "RX_RFPLL", "TX_RFPLL"};

	dev_dbg("%s: Source %s Rate %lu Hz Parent Rate %lu Hz\n",
		__func__, message[clk_priv->source], rate, parent_rate);

	if (rate >= parent_rate) {
		clk_priv->mult = DIV_ROUND_CLOSEST(rate, parent_rate);
		clk_priv->div = 1;
	} else {
		clk_priv->div = DIV_ROUND_CLOSEST(parent_rate, rate);
		clk_priv->mult = 1;
	}

	return ad9361_set_clk_scaler(clk_priv, true);
}
#if 0
struct clk_ops refclk_scale_ops = {
	.round_rate = ad9361_clk_factor_round_rate,
	.set_rate = ad9361_clk_factor_set_rate,
	.recalc_rate = ad9361_clk_factor_recalc_rate,
};
#endif
/*
 * BBPLL
 */

unsigned long ad9361_bbpll_recalc_rate(struct refclk_scale *clk_priv,
		unsigned long parent_rate)
{
//	struct refclk_scale *clk_priv = to_clk_priv(hw);
	u64 rate;
	unsigned long fract, integer;
	u8 buf[4];

	ad9361_spi_readm(REG_INTEGER_BB_FREQ_WORD, &buf[0],
			REG_INTEGER_BB_FREQ_WORD - REG_FRACT_BB_FREQ_WORD_1 + 1);

	fract = (buf[3] << 16) | (buf[2] << 8) | buf[1];
	integer = buf[0];

	rate = ((u64)parent_rate * fract);
	do_div(rate, BBPLL_MODULUS);
	rate += (u64)parent_rate * integer;

	return (unsigned long)rate;
}

long ad9361_bbpll_round_rate(struct refclk_scale *clk_priv, unsigned long rate,
				unsigned long *prate)
{
	u64 tmp;
	u32 fract, integer;

	if (rate > MAX_BBPLL_FREQ)
		return MAX_BBPLL_FREQ;

	if (rate < MIN_BBPLL_FREQ)
		return MIN_BBPLL_FREQ;

	tmp = do_div(rate, *prate);
	tmp = tmp * BBPLL_MODULUS + (*prate >> 1);
	do_div(tmp, *prate);

	integer = rate;
	fract = tmp;

	tmp = *prate * (u64)fract;
	do_div(tmp, BBPLL_MODULUS);
	tmp += *prate * integer;

	return tmp;
}

int ad9361_bbpll_set_rate(struct refclk_scale *clk_priv, unsigned long rate,
				unsigned long parent_rate)
{
//	struct refclk_scale *clk_priv = to_clk_priv(hw);
	u64 tmp;
	u32 fract, integer;
	int icp_val;
	u8 lf_defaults[3] = {0x35, 0x5B, 0xE8};
	char   message[16][12] = {"BB_REFCLK", "RX_REFCLK", "TX_REFCLK", "BBPLL_CLK", "ADC_CLK",
		"R2_CLK", "R1_CLK", "CLKRF_CLK", "RX_SAMPL_CLK", "DAC_CLK", "T2_CLK",
		"T1_CLK", "CLKTF_CLK", "TX_SAMPL_CLK", "RX_RFPLL", "TX_RFPLL"};

	dev_dbg("%s: Source %s Rate %lu Hz Parent Rate %lu Hz\n",
		__func__, message[clk_priv->source], rate, parent_rate);

	/*
	 * Setup Loop Filter and CP Current
	 * Scale is 150uA @ (1280MHz BBPLL, 40MHz REFCLK)
	 */
	tmp = (rate >> 7) * 150ULL;
	do_div(tmp, (parent_rate >> 7) * 32UL + (tmp >> 1));

	/* 25uA/LSB, Offset 25uA */
	icp_val = DIV_ROUND_CLOSEST((u32)tmp, 25U) - 1;

	icp_val = clamp(icp_val, 1, 64);

	ad9361_spi_write(REG_CP_CURRENT, icp_val);
	ad9361_spi_writem(REG_LOOP_FILTER_3, lf_defaults,
			  ARRAY_SIZE(lf_defaults));

	/* Allow calibration to occur and set cal count to 1024 for max accuracy */
	ad9361_spi_write(REG_VCO_CTRL,
			 FREQ_CAL_ENABLE | FREQ_CAL_COUNT_LENGTH(3));
	/* Set calibration clock to REFCLK/4 for more accuracy */
	ad9361_spi_write(REG_SDM_CTRL, 0x10);

	/* Calculate and set BBPLL frequency word */
	tmp = do_div(rate, parent_rate);
	tmp = tmp *(u64) BBPLL_MODULUS + (parent_rate >> 1);
	do_div(tmp, parent_rate);

	integer = rate;
	fract = tmp;

	ad9361_spi_write(REG_INTEGER_BB_FREQ_WORD, integer);
	ad9361_spi_write(REG_FRACT_BB_FREQ_WORD_3, fract);
	ad9361_spi_write(REG_FRACT_BB_FREQ_WORD_2, fract >> 8);
	ad9361_spi_write(REG_FRACT_BB_FREQ_WORD_1, fract >> 16);

	ad9361_spi_write(REG_SDM_CTRL_1, INIT_BB_FO_CAL | BBPLL_RESET_BAR); /* Start BBPLL Calibration */
	ad9361_spi_write(REG_SDM_CTRL_1, BBPLL_RESET_BAR); /* Clear BBPLL start calibration bit */

	ad9361_spi_write(REG_VCO_PROGRAM_1, 0x86); /* Increase BBPLL KV and phase margin */
	ad9361_spi_write(REG_VCO_PROGRAM_2, 0x01); /* Increase BBPLL KV and phase margin */
	ad9361_spi_write(REG_VCO_PROGRAM_2, 0x05); /* Increase BBPLL KV and phase margin */

	return ad9361_check_cal_done(clk_priv->phy, REG_CH_1_OVERFLOW,
				     BBPLL_LOCK, 1);
}
#if 0
struct clk_ops bbpll_clk_ops = {
	.round_rate = ad9361_bbpll_round_rate,
	.set_rate = ad9361_bbpll_set_rate,
	.recalc_rate = ad9361_bbpll_recalc_rate,
};
#endif
/*
 * RFPLL
 */

static u64 ad9361_calc_rfpll_freq(u64 parent_rate,
				   u64 integer,
				   u64 fract, u32 vco_div)
{
	u64 rate;

	rate = parent_rate * fract;
	do_div(rate, RFPLL_MODULUS);
	rate += parent_rate * integer;

	return rate >> (vco_div + 1);
}

static int ad9361_calc_rfpll_divder(u64 freq,
			     u64 parent_rate, u32 *integer,
			     u32 *fract, int *vco_div, u64 *vco_freq)
{
	u64 tmp;
	int div;

	if (freq > MAX_CARRIER_FREQ_HZ || freq < MIN_CARRIER_FREQ_HZ)
		return -EINVAL;

	div = -1;

	while (freq <= MIN_VCO_FREQ_HZ) {
		freq <<= 1;
		div++;
	}

	*vco_div = div;
	*vco_freq = freq;
	tmp = do_div(freq, parent_rate);
	tmp = tmp * RFPLL_MODULUS + (parent_rate >> 1);
	do_div(tmp, parent_rate);
	*integer = freq;
	*fract = tmp;

	return 0;
}

unsigned long ad9361_rfpll_recalc_rate(struct refclk_scale *clk_priv,
		unsigned long parent_rate)
{
//	struct refclk_scale *clk_priv = to_clk_priv(hw);
	unsigned long fract, integer;
	u8 buf[5];
	u32 reg, div_mask, vco_div;

	switch (clk_priv->source) {
	case RX_RFPLL:
		reg = REG_RX_FRACT_BYTE_2;
		div_mask = RX_VCO_DIVIDER(~0);
		break;
	case TX_RFPLL:
		reg = REG_TX_FRACT_BYTE_2;
		div_mask = TX_VCO_DIVIDER(~0);
		break;
	default:
		return -EINVAL;
	}

	ad9361_spi_readm(reg, &buf[0], ARRAY_SIZE(buf));

	vco_div = ad9361_spi_readf(REG_RFPLL_DIVIDERS, div_mask);

	fract = (buf[0] << 16) | (buf[1] << 8) | buf[2];
	integer = buf[3] << 8 | buf[4];

	return ad9361_to_clk(ad9361_calc_rfpll_freq(parent_rate, integer,
					      fract, vco_div));
}

long ad9361_rfpll_round_rate(struct refclk_scale *clk_priv, unsigned long rate,
				unsigned long *prate)
{
	u64 vco;
	u32 fract, integer;
	int vco_div, ret;

	ret = ad9361_calc_rfpll_divder(ad9361_from_clk(rate), *prate, &integer, &fract, &vco_div, &vco);
	if (ret < 0)
		return ret;

	return ad9361_to_clk(ad9361_calc_rfpll_freq(*prate, integer, fract, vco_div));
}

int ad9361_rfpll_set_rate(struct refclk_scale *clk_priv, unsigned long rate,
				unsigned long parent_rate)
{
//	struct refclk_scale *clk_priv = to_clk_priv(hw);
	struct ad9361_rf_phy *phy = clk_priv->phy;
	u64 vco;
	u8 buf[5];
	u32 reg, div_mask, lock_reg, fract, integer;
	int vco_div, ret;
	char   message[16][12] = {"BB_REFCLK", "RX_REFCLK", "TX_REFCLK", "BBPLL_CLK", "ADC_CLK",
		"R2_CLK", "R1_CLK", "CLKRF_CLK", "RX_SAMPL_CLK", "DAC_CLK", "T2_CLK",
		"T1_CLK", "CLKTF_CLK", "TX_SAMPL_CLK", "RX_RFPLL", "TX_RFPLL"};

	dev_dbg("%s: Source %s Rate %lu Hz Parent Rate %lu Hz\n",
		__func__, message[clk_priv->source], rate, parent_rate);

	ret = ad9361_calc_rfpll_divder(ad9361_from_clk(rate), parent_rate,
				&integer, &fract, &vco_div, &vco);
	if (ret < 0)
		return ret;

	switch (clk_priv->source) {
	case RX_RFPLL:
		reg = REG_RX_FRACT_BYTE_2;
		lock_reg = REG_RX_CP_OVERRANGE_VCO_LOCK;
		div_mask = RX_VCO_DIVIDER(~0);
		break;
	case TX_RFPLL:
		reg = REG_TX_FRACT_BYTE_2;
		lock_reg = REG_TX_CP_OVERRANGE_VCO_LOCK;
		div_mask = TX_VCO_DIVIDER(~0);
		break;
	default:
		return -EINVAL;

	}

	ad9361_rfpll_vco_init(phy, div_mask == TX_VCO_DIVIDER(~0),
			      vco, parent_rate);

	buf[0] = fract >> 16;
	buf[1] = fract >> 8;
	buf[2] = fract & 0xFF;
	buf[3] = integer >> 8;
	buf[4] = integer & 0xFF;

	ad9361_spi_writem(reg, buf, 5);
	ad9361_spi_writef(REG_RFPLL_DIVIDERS, div_mask, vco_div);

	/* Load Gain Table */
	if (clk_priv->source == RX_RFPLL) {
		ret = ad9361_load_gt(phy, ad9361_from_clk(rate), GT_RX1 + GT_RX2);
		if (ret < 0)
			return ret;
	}

	/* For RX LO we typically have the tracking option enabled
	 * so for now do nothing here.
	 */
	if (phy->auto_cal_en && (clk_priv->source == TX_RFPLL))
		if (abs(phy->last_tx_quad_cal_freq - ad9361_from_clk(rate)) >
			phy->cal_threshold_freq) {
#ifdef LINUX
			set_bit(0, &phy->flags);
			INIT_COMPLETION(phy->complete);
			schedule_work(&phy->work);
#else
			ret = ad9361_do_calib_run(phy, TX_QUAD_CAL, -1);
			if (ret < 0)
				dev_err("%s: TX QUAD cal failed", __func__);
#endif
			phy->last_tx_quad_cal_freq = ad9361_from_clk(rate);
		}

	return ad9361_check_cal_done(phy, lock_reg, BIT(1), 1);
}
#if 0
struct clk_ops rfpll_clk_ops = {
	.round_rate = ad9361_rfpll_round_rate,
	.set_rate = ad9361_rfpll_set_rate,
	.recalc_rate = ad9361_rfpll_recalc_rate,
};
#endif
static struct clk *ad9361_clk_register(struct ad9361_rf_phy *phy, const char *name,
		const char *parent_name, unsigned long flags,
		u32 source, u32 parent_source)
{
	struct refclk_scale *clk_priv;
	//struct clk_init_data init;
	struct clk *clk;

	//clk_priv = kmalloc(sizeof(*clk_priv), GFP_KERNEL);
	clk_priv = malloc(sizeof(*clk_priv));
	if (!clk_priv) {
		pr_err("%s: could not allocate fixed factor clk\n", __func__);
		return ERR_PTR(-ENOMEM);
	}

	/* struct refclk_scale assignments */
	clk_priv->source = source;
	clk_priv->parent_source = parent_source;
	//clk_priv->hw.init = &init;
	clk_priv->phy = phy;

	//init.name = name;
#if 0
	switch (source) {
	case BBPLL_CLK:
		init.ops = &bbpll_clk_ops;
		break;
	case RX_RFPLL:
	case TX_RFPLL:
		init.ops = &rfpll_clk_ops;
		break;
	default:
		init.ops = &refclk_scale_ops;
	}
#endif
	//init.flags = flags;
	//init.parent_names = &parent_name;
	//init.num_parents = 1;

	phy->ref_clk_scale[source] = clk_priv;

	clk = malloc(sizeof(*clk));
	if (!clk) {
		return ERR_PTR(-ENOMEM);
	}

	switch (source) {
		case TX_REFCLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clk_refin->rate);
			break;
		case RX_REFCLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clk_refin->rate);
			break;
		case BB_REFCLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clk_refin->rate);
			break;
		case BBPLL_CLK:
			clk->rate = ad9361_bbpll_recalc_rate(clk_priv, phy->clks[BB_REFCLK]->rate);
			break;
		case ADC_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[BBPLL_CLK]->rate);
			break;
		case R2_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[ADC_CLK]->rate);
			break;
		case R1_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[R2_CLK]->rate);
			break;
		case CLKRF_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[R1_CLK]->rate);
			break;
		case RX_SAMPL_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[CLKRF_CLK]->rate);
			break;
		case DAC_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[ADC_CLK]->rate);
			break;
		case T2_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[DAC_CLK]->rate);
			break;
		case T1_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[T2_CLK]->rate);
			break;
		case CLKTF_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[T1_CLK]->rate);
			break;
		case TX_SAMPL_CLK:
			clk->rate = ad9361_clk_factor_recalc_rate(clk_priv, phy->clks[CLKTF_CLK]->rate);
			break;
		case RX_RFPLL:
			clk->rate = ad9361_rfpll_recalc_rate(clk_priv, phy->clks[RX_REFCLK]->rate);
			break;
		case TX_RFPLL:
			clk->rate = ad9361_rfpll_recalc_rate(clk_priv, phy->clks[TX_REFCLK]->rate);
			break;
	}

//	clk = clk_register(&phy->spi->dev, &clk_priv->hw);
//	phy->clk_data.clks[source] = clk;

//	if (IS_ERR(clk))
//		kfree(clk_priv);

	return clk;
}

int register_clocks(struct ad9361_rf_phy *phy)
{
	u32 flags = 0;

	//phy->clk_data.clks = devm_kzalloc(&phy->spi->dev,
	//				 sizeof(*phy->clk_data.clks) *
	//				 NUM_AD9361_CLKS, GFP_KERNEL);
	phy->clk_data.clks =malloc(sizeof(*phy->clk_data.clks) *
					NUM_AD9361_CLKS);
	if (!phy->clk_data.clks) {
		dev_err("could not allocate memory\n");
		return -ENOMEM;
	}

	if (phy->pdata->debug_mode)
		flags = CLK_GET_RATE_NOCACHE;

	phy->clk_data.clk_num = NUM_AD9361_CLKS;

	/* Scaled Reference Clocks */
	phy->clks[TX_REFCLK] = ad9361_clk_register(phy,
					"tx_refclk", "ad9361_ext_refclk",
					flags | CLK_IGNORE_UNUSED,
					TX_REFCLK, EXT_REF_CLK);

	phy->clks[RX_REFCLK] = ad9361_clk_register(phy,
					"rx_refclk", "ad9361_ext_refclk",
					flags | CLK_IGNORE_UNUSED,
					RX_REFCLK, EXT_REF_CLK);

	phy->clks[BB_REFCLK] = ad9361_clk_register(phy,
					"bb_refclk", "ad9361_ext_refclk",
					flags | CLK_IGNORE_UNUSED,
					BB_REFCLK, EXT_REF_CLK);

	/* Base Band PLL Clock */
	phy->clks[BBPLL_CLK] = ad9361_clk_register(phy,
					"bbpll_clk", "bb_refclk",
					flags | CLK_IGNORE_UNUSED,
					BBPLL_CLK, BB_REFCLK);

	phy->clks[ADC_CLK] = ad9361_clk_register(phy,
					"adc_clk", "bbpll_clk",
					flags | CLK_IGNORE_UNUSED,
					ADC_CLK, BBPLL_CLK);

	phy->clks[R2_CLK] = ad9361_clk_register(phy,
					"r2_clk", "adc_clk",
					flags | CLK_IGNORE_UNUSED,
					R2_CLK, ADC_CLK);

	phy->clks[R1_CLK] = ad9361_clk_register(phy,
					"r1_clk", "r2_clk",
					flags | CLK_IGNORE_UNUSED,
					R1_CLK, R2_CLK);

	phy->clks[CLKRF_CLK] = ad9361_clk_register(phy,
					"clkrf_clk", "r1_clk",
					flags | CLK_IGNORE_UNUSED,
					CLKRF_CLK, R1_CLK);

	phy->clks[RX_SAMPL_CLK] = ad9361_clk_register(phy,
					"rx_sampl_clk", "clkrf_clk",
					flags | CLK_IGNORE_UNUSED,
					RX_SAMPL_CLK, CLKRF_CLK);


	phy->clks[DAC_CLK] = ad9361_clk_register(phy,
					"dac_clk", "adc_clk",
					flags | CLK_IGNORE_UNUSED,
					DAC_CLK, ADC_CLK);

	phy->clks[T2_CLK] = ad9361_clk_register(phy,
					"t2_clk", "dac_clk",
					flags | CLK_IGNORE_UNUSED,
					T2_CLK, DAC_CLK);

	phy->clks[T1_CLK] = ad9361_clk_register(phy,
					"t1_clk", "t2_clk",
					flags | CLK_IGNORE_UNUSED,
					T1_CLK, T2_CLK);

	phy->clks[CLKTF_CLK] = ad9361_clk_register(phy,
					"clktf_clk", "t1_clk",
					flags | CLK_IGNORE_UNUSED,
					CLKTF_CLK, T1_CLK);

	phy->clks[TX_SAMPL_CLK] = ad9361_clk_register(phy,
					"tx_sampl_clk", "clktf_clk",
					flags | CLK_IGNORE_UNUSED,
					TX_SAMPL_CLK, CLKTF_CLK);

	phy->clks[RX_RFPLL] = ad9361_clk_register(phy,
					"rx_rfpll", "rx_refclk",
					flags | CLK_IGNORE_UNUSED,
					RX_RFPLL, RX_REFCLK);

	phy->clks[TX_RFPLL] = ad9361_clk_register(phy,
					"tx_rfpll", "tx_refclk",
					flags | CLK_IGNORE_UNUSED,
					TX_RFPLL, TX_REFCLK);


	return 0;
}
