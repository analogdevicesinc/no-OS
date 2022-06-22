// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_user.c
 * \brief Contains Talise API default gain table values for Rx and ObsRx channels.  This file contains information which can be modified by the user.
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_user.h"

/**
 * \brief Default Rx gain table settings
 *
 * Order: {FE table, External Ctl, TIA/ADC, signed Digital Gain/Atten[10:0], Phase offset}
 */
taliseRxGainTable_t rxGainTable [61] = {
	{0, 0, 0, 14, 0},     /* Gain index 255 */
	{14, 0, 0, 14, 0},    /* Gain index 254 */
	{27, 0, 0, 15, 0},    /* Gain index 253 */
	{41, 0, 0, 14, 0},    /* Gain index 252 */
	{53, 0, 0, 13, 0},    /* Gain index 251 */
	{67, 0, 0, 14, 0},    /* Gain index 250 */
	{78, 0, 0, 15, 0},    /* Gain index 249 */
	{88, 0, 0, 14, 0},    /* Gain index 248 */
	{100, 0, 0, 14, 0},   /* Gain index 247 */
	{109, 0, 0, 14, 0},   /* Gain index 246 */
	{118, 0, 0, 14, 0},   /* Gain index 245 */
	{126, 0, 0, 14, 0},   /* Gain index 244 */
	{135, 0, 0, 15, 0},   /* Gain index 243 */
	{142, 0, 0, 15, 0},   /* Gain index 242 */
	{149, 0, 0, 14, 0},   /* Gain index 241 */
	{155, 0, 0, 14, 0},   /* Gain index 240 */
	{163, 0, 0, 15, 0},   /* Gain index 239 */
	{169, 0, 0, 15, 0},   /* Gain index 238 */
	{174, 0, 0, 14, 0},   /* Gain index 237 */
	{179, 0, 0, 15, 0},   /* Gain index 236 */
	{183, 0, 0, 13, 0},   /* Gain index 235 */
	{187, 0, 0, 13, 0},   /* Gain index 234 */
	{193, 0, 0, 15, 0},   /* Gain index 233 */
	{196, 0, 0, 13, 0},   /* Gain index 232 */
	{200, 0, 0, 14, 0},   /* Gain index 231 */
	{203, 0, 0, 13, 0},   /* Gain index 230 */
	{206, 0, 0, 13, 0},   /* Gain index 229 */
	{209, 0, 0, 13, 0},   /* Gain index 228 */
	{212, 0, 0, 14, 0},   /* Gain index 227 */
	{214, 0, 0, 12, 0},   /* Gain index 226 */
	{217, 0, 0, 14, 0},   /* Gain index 225 */
	{219, 0, 0, 13, 0},   /* Gain index 224 */
	{221, 0, 0, 13, 0},   /* Gain index 223 */
	{224, 0, 0, 13, 0},   /* Gain index 222 */
	{226, 0, 0, 14, 0},   /* Gain index 221 */
	{228, 0, 0, 15, 0},   /* Gain index 220 */
	{229, 0, 0, 12, 0},   /* Gain index 219 */
	{231, 0, 0, 16, 0},   /* Gain index 218 */
	{232, 0, 0, 12, 0},   /* Gain index 217 */
	{234, 0, 0, 17, 0},   /* Gain index 216 */
	{235, 0, 0, 16, 0},   /* Gain index 215 */
	{236, 0, 0, 14, 0},   /* Gain index 214 */
	{237, 0, 0, 14, 0},   /* Gain index 213 */
	{238, 0, 0, 13, 0},   /* Gain index 212 */
	{239, 0, 0, 14, 0},   /* Gain index 211 */
	{240, 0, 0, 14, 0},   /* Gain index 210 */
	{241, 0, 0, 17, 0},   /* Gain index 209 */
	{242, 0, 0, 19, 0},   /* Gain index 208 */
	{242, 0, 0, 9, 0},    /* Gain index 207 */
	{243, 0, 0, 13, 0},   /* Gain index 206 */
	{244, 0, 0, 18, 0},   /* Gain index 205 */
	{244, 0, 0, 8, 0},    /* Gain index 204 */
	{245, 0, 0, 15, 0},   /* Gain index 203 */
	{246, 0, 0, 23, 0},   /* Gain index 202 */
	{246, 0, 0, 13, 0},   /* Gain index 201 */
	{247, 0, 0, 25, 0},   /* Gain index 200 */
	{247, 0, 0, 15, 0},   /* Gain index 199 */
	{247, 0, 0, 5, 0},    /* Gain index 198 */
	{248, 0, 0, 16, 0},   /* Gain index 197 */
	{248, 0, 0, 6, 0},    /* Gain index 196 */
	{249, 0, 0, 24, 0}   /* Gain index 195 */
};

/**
* \brief Default ORx gain table settings
*/
taliseOrxGainTable_t orxGainTable [61] = {
	{0, 0, 0, 14},     /* Gain index 255 */
	{14, 0, 0, 14},    /* Gain index 254 */
	{27, 0, 0, 15},    /* Gain index 253 */
	{41, 0, 0, 14},    /* Gain index 252 */
	{53, 0, 0, 13},    /* Gain index 251 */
	{67, 0, 0, 14},    /* Gain index 250 */
	{78, 0, 0, 15},    /* Gain index 249 */
	{88, 0, 0, 14},    /* Gain index 248 */
	{100, 0, 0, 14},   /* Gain index 247 */
	{109, 0, 0, 14},   /* Gain index 246 */
	{118, 0, 0, 14},   /* Gain index 245 */
	{126, 0, 0, 14},   /* Gain index 244 */
	{135, 0, 0, 15},   /* Gain index 243 */
	{142, 0, 0, 15},   /* Gain index 242 */
	{149, 0, 0, 14},   /* Gain index 241 */
	{155, 0, 0, 14},   /* Gain index 240 */
	{163, 0, 0, 15},   /* Gain index 239 */
	{169, 0, 0, 15},   /* Gain index 238 */
	{174, 0, 0, 14},   /* Gain index 237 */
	{179, 0, 0, 15},   /* Gain index 236 */
	{183, 0, 0, 13},   /* Gain index 235 */
	{187, 0, 0, 13},   /* Gain index 234 */
	{193, 0, 0, 15},   /* Gain index 233 */
	{196, 0, 0, 13},   /* Gain index 232 */
	{200, 0, 0, 14},   /* Gain index 231 */
	{203, 0, 0, 13},   /* Gain index 230 */
	{206, 0, 0, 13},   /* Gain index 229 */
	{209, 0, 0, 13},   /* Gain index 228 */
	{212, 0, 0, 14},   /* Gain index 227 */
	{214, 0, 0, 12},   /* Gain index 226 */
	{217, 0, 0, 14},   /* Gain index 225 */
	{219, 0, 0, 13},   /* Gain index 224 */
	{221, 0, 0, 13},   /* Gain index 223 */
	{224, 0, 0, 13},   /* Gain index 222 */
	{226, 0, 0, 14},   /* Gain index 221 */
	{228, 0, 0, 15},   /* Gain index 220 */
	{229, 0, 0, 12},   /* Gain index 219 */
	{231, 0, 0, 16},   /* Gain index 218 */
	{232, 0, 0, 12},   /* Gain index 217 */
	{234, 0, 0, 17},   /* Gain index 216 */
	{235, 0, 0, 16},   /* Gain index 215 */
	{236, 0, 0, 14},   /* Gain index 214 */
	{237, 0, 0, 14},   /* Gain index 213 */
	{238, 0, 0, 13},   /* Gain index 212 */
	{239, 0, 0, 14},   /* Gain index 211 */
	{240, 0, 0, 14},   /* Gain index 210 */
	{241, 0, 0, 17},   /* Gain index 209 */
	{242, 0, 0, 19},   /* Gain index 208 */
	{242, 0, 0, 9},    /* Gain index 207 */
	{243, 0, 0, 13},   /* Gain index 206 */
	{244, 0, 0, 18},   /* Gain index 205 */
	{244, 0, 0, 8},    /* Gain index 204 */
	{245, 0, 0, 15},   /* Gain index 203 */
	{246, 0, 0, 23},   /* Gain index 202 */
	{246, 0, 0, 13},   /* Gain index 201 */
	{247, 0, 0, 25},   /* Gain index 200 */
	{247, 0, 0, 15},   /* Gain index 199 */
	{247, 0, 0, 5},    /* Gain index 198 */
	{248, 0, 0, 16},   /* Gain index 197 */
	{248, 0, 0, 6},    /* Gain index 196 */
	{249, 0, 0, 24}   /* Gain index 195 */
};
