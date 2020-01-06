/**
 * \file mykonos_user.c
 * \brief Contains Mykonos default gain table values for Rx, ObsRx, and SnRx
 *
 * Mykonos API version: 1.5.2.3566
 */

/**
* Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#include <stdint.h>
#include "t_mykonos.h"
#include "mykonos_user.h"

/**
 * \brief Default Rx gain table settings
 */
uint8_t RxGainTable [61][4] =
{
	/* Order: {FE table, External Ctl, Digital Gain/Atten, Enable Atten} */
        {0, 0, 0, 0},  /* Gain index 255 */
        {3, 0, 2, 1},  /* Gain index 254 */
        {6, 0, 3, 1},  /* Gain index 253 */
        {10, 0, 0, 0}, /* Gain index 252 */
        {13, 0, 1, 1}, /* Gain index 251 */
        {16, 0, 0, 0}, /* Gain index 250 */
        {18, 0, 3, 1}, /* Gain index 249 */
        {21, 0, 1, 1}, /* Gain index 248 */
        {23, 0, 3, 1}, /* Gain index 247 */
        {25, 0, 4, 1}, /* Gain index 246 */
        {28, 0, 0, 0}, /* Gain index 245 */
        {30, 0, 0, 0}, /* Gain index 244 */
        {31, 0, 5, 1}, /* Gain index 243 */
        {33, 0, 4, 1}, /* Gain index 242 */
        {35, 0, 2, 1}, /* Gain index 241 */
        {37, 0, 0, 0}, /* Gain index 240 */
        {38, 0, 4, 1}, /* Gain index 239 */
        {39, 0, 7, 1}, /* Gain index 238 */
        {41, 0, 2, 1}, /* Gain index 237 */
        {42, 0, 4, 1}, /* Gain index 236 */
        {43, 0, 6, 1}, /* Gain index 235 */
        {44, 0, 8, 1}, /* Gain index 234 */
        {45, 0, 9, 1},  /* Gain index 233 */
        {46, 0, 10, 1}, /* Gain index 232 */
        {47, 0, 10, 1}, /* Gain index 231 */
        {48, 0, 9, 1}, /* Gain index 230 */
        {49, 0, 8, 1}, /* Gain index 229 */
        {50, 0, 6, 1}, /* Gain index 228 */
        {51, 0, 3, 1}, /* Gain index 227 */
        {51, 0, 13, 1}, /* Gain index 226 */
        {52, 0, 9, 1}, /* Gain index 225 */
        {53, 0, 5, 1}, /* Gain index 224 */
        {53, 0, 15, 1}, /* Gain index 223 */
        {54, 0, 9, 1}, /* Gain index 222 */
        {54, 0, 19, 1}, /* Gain index 221 */
        {55, 0, 11, 1}, /* Gain index 220 */
        {55, 0, 21, 1}, /* Gain index 219 */
        {56, 0, 11, 1}, /* Gain index 218 */
        {56, 0, 21, 1}, /* Gain index 217 */
        {57, 0, 9, 1}, /* Gain index 216 */
        {57, 0, 19, 1}, /* Gain index 215 */
        {57, 0, 29, 1}, /* Gain index 214 */
        {58, 0, 12, 1}, /* Gain index 213 */
        {58, 0, 22, 1}, /* Gain index 212 */
        {58, 0, 32, 1}, /* Gain index 211 */
        {59, 0, 11, 1}, /* Gain index 210 */
        {59, 0, 21, 1}, /* Gain index 209 */
        {59, 0, 31, 1}, /* Gain index 208 */
        {59, 0, 41, 1}, /* Gain index 207 */
        {60, 0, 13, 1}, /* Gain index 206 */
        {60, 0, 23, 1}, /* Gain index 205 */
        {60, 0, 33, 1}, /* Gain index 204 */
        {60, 0, 43, 1}, /* Gain index 203 */
        {60, 0, 53, 1}, /* Gain index 202 */
        {61, 0, 14, 1}, /* Gain index 201 */
        {61, 0, 24, 1}, /* Gain index 200 */
        {61, 0, 34, 1}, /* Gain index 199 */
        {61, 0, 44, 1}, /* Gain index 198 */
        {61, 0, 54, 1}, /* Gain index 197 */
        {61, 0, 64, 1}, /* Gain index 196 */
        {61, 0, 74, 1}, /* Gain index 195 */
};

/**
 * \brief Default ORx gain table settings
 */
uint8_t ORxGainTable [19][4] =
{
	/* Order: {FE table, External Ctl, Digital Gain/Atten, Enable Atten} */
	{0, 0, 0, 0},	/* Gain index 255 */
	{7, 0, 0, 0},	/* Gain index 254 */
	{13, 0, 1, 1},	/* Gain index 253 */
	{18, 0, 3, 1},	/* Gain index 252 */
	{23, 0, 3, 1},	/* Gain index 251 */
	{28, 0, 0, 0},	/* Gain index 250 */
	{32, 0, 0, 0},	/* Gain index 249 */
	{35, 0, 2, 1},	/* Gain index 248 */
	{38, 0, 4, 1},	/* Gain index 247 */
	{41, 0, 2, 1},	/* Gain index 246 */
	{43, 0, 6, 1},	/* Gain index 245 */
	{46, 0, 0, 0},	/* Gain index 244 */
	{47, 0, 10, 1},	/* Gain index 243 */
	{49, 0, 8, 1},	/* Gain index 242 */
	{51, 0, 3, 1},	/* Gain index 241 */
	{52, 0, 9, 1},	/* Gain index 240 */
	{53, 0, 14, 1},	/* Gain index 239 */
	{54, 0, 18, 1},	/* Gain index 238 */
	{56, 0, 0, 0}	/* Gain index 237 */

};

/**
 * \brief Default SnRx gain table settings
 */
uint8_t SnRxGainTable [53][4] =
{
	/* Order: {FE table, LNA Bypass, Digital Gain/Atten, Enable Atten} */
    {0,  0,  0, 0},  /* Gain index 255 */
    {1,  0,  7, 1},  /* Gain index 254 */
    {3,  0,  1, 0},  /* Gain index 253 */
    {3,  0, 15, 1}, /* Gain index 252 */
    {5,  0,  0, 0},  /* Gain index 251 */
    {6,  0,  2, 1},  /* Gain index 250 */
    {7,  0,  2, 1},  /* Gain index 249 */
    {8,  0, 12, 1}, /* Gain index 248 */
    {9,  0,  7, 1},  /* Gain index 247 */
    {11, 0,  1, 0}, /* Gain index 246 */
    {11, 0, 15, 1},/* Gain index 245 */
    {13, 0,  1, 0}, /* Gain index 244 */
    {14, 0,  2, 0}, /* Gain index 243 */
    {14, 0, 10, 1},/* Gain index 242 */
    {15, 0,  0, 0}, /* Gain index 241 */
    {15, 0, 20, 1},/* Gain index 240 */
    {16, 0,  2, 1}, /* Gain index 239 */
    {16, 0, 22, 1},/* Gain index 238 */
    {17, 0,  1, 0}, /* Gain index 237 */
    {17, 0, 15, 1},/* Gain index 236 */
    {17, 0, 35, 1},/* Gain index 235 */
    {17, 0, 55, 1},/* Gain index 234 */
    {18, 0,  7, 1}, /* Gain index 233 */
    {18, 0, 27, 1},/* Gain index 232 */
    { 6, 1,  7, 1},  /* Gain index 231 */
    { 7, 1,  7, 1},  /* Gain index 230 */
    { 8, 1, 17, 1}, /* Gain index 229 */
    { 9, 1, 12, 1}, /* Gain index 228 */
    {11, 1,  0, 0}, /* Gain index 227 */
    {12, 1,  3, 1}, /* Gain index 226 */
    {13, 1,  0, 0}, /* Gain index 225 */
    {14, 1,  1, 0}, /* Gain index 224 */
    {14, 1, 15, 1},/* Gain index 223 */
    {15, 1,  5, 1}, /* Gain index 222 */
    {15, 1, 25, 1},/* Gain index 221 */
    {16, 1,  7, 1}, /* Gain index 220 */
    {16, 1, 27, 1},/* Gain index 219 */
    {17, 1,  0, 0}, /* Gain index 218 */
    {17, 1, 20, 1},/* Gain index 217 */
    {17, 1, 40, 1},/* Gain index 216 */
    {17, 1, 60, 1},/* Gain index 215 */
    {18, 1, 12, 1},/* Gain index 214 */
    {18, 1, 32, 1},/* Gain index 213 */
    {18, 1, 52, 1},/* Gain index 212 */
    {18, 1, 72, 1},/* Gain index 211 */
    {18, 1, 92, 1},/* Gain index 210 */
    {19, 1,  1, 0}, /* Gain index 209 */
    {19, 1, 15, 1},/* Gain index 208 */
    {19, 1, 35, 1},/* Gain index 207 */
    {19, 1, 55, 1},/* Gain index 206 */
    {19, 1, 75, 1},/* Gain index 205 */
    {19, 1, 95, 1},/* Gain index 204 */
    {19, 1, 115, 1}/* Gain index 203 */
};

