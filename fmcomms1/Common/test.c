/**************************************************************************//**
*   @file   test.c
*   @brief  Test functions implementations.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include <xparameters.h>
#include <xil_io.h>
#include "timer.h"
#include "test.h"

/*****************************************************************************/
/***************************** External Functions ****************************/
/*****************************************************************************/
extern int32_t ad9122_write(uint8_t registerAddress, uint8_t registerValue);
extern int32_t ad9122_read(uint8_t registerAddress);
extern void xil_printf(const char *ctrl1, ...);

/*****************************************************************************/
/************************ Constants Definitions ******************************/
/*****************************************************************************/
const int32_t dac_dma_data[1024] =
{
		0x7fff0000, 0x5dc6571d, 0x096a7fa6, 0xb00663ee, 0x816412c7, 0x9675b797,
		0xe3f5831e, 0x4073916b, 0x7a7cdad9, 0x73063824, 0x2e10776b, 0xd07a76d8,
		0x8c4b36b9, 0x85fcd959, 0xc0ea90a2, 0x1d938378, 0x6a6cb8e4, 0x7e5e1455,
		0x4ebf64e7, 0xf5057f86, 0xa12a55f4, 0x8004fe6e, 0xa34da7be, 0xf827803f,
		0x51339d0f, 0x7ed4eec7, 0x68a549b3, 0x1a827d38, 0xbe336dc9, 0x851223a6,
		0x8dacc674, 0xd3688807, 0x30fb89c0, 0x745ecab3, 0x79892826, 0x3db77022,
		0xe0e77c29, 0x94b645cc, 0x81e4ea1f, 0xb2809a23, 0x0c8b809f, 0x5fe2ab37,
		0x7ff50324, 0x5b9c5963, 0x06477fd7, 0xad9861f0, 0x80f80fab, 0x9844b506,
		0xe7088277, 0x43259308, 0x7b5cdddd, 0x719d3af2, 0x2b1e7883, 0xcd9375a4,
		0x8afd33de, 0x86f8d65d, 0xc3aa8f1f, 0x209f843c, 0x6c23bb86, 0x7dd5176d,
		0x4c3f66ce, 0xf1e57f37, 0x9f15539a, 0x8018fb4a, 0xa57fa57f, 0xfb4a8018,
		0x539a9f15, 0x7f37f1e5, 0x66ce4c3f, 0x176d7dd5, 0xbb866c23, 0x843c209f,
		0x8f1fc3aa, 0xd65d86f8, 0x33de8afd, 0x75a4cd93, 0x78832b1e, 0x3af2719d,
		0xdddd7b5c, 0x93084325, 0x8277e708, 0xb5069844, 0x0fab80f8, 0x61f0ad98,
		0x7fd70647, 0x59635b9c, 0x03247ff5, 0xab375fe2, 0x809f0c8b, 0x9a23b280,
		0xea1f81e4, 0x45cc94b6, 0x7c29e0e7, 0x70223db7, 0x28267989, 0xcab3745e,
		0x89c030fb, 0x8807d368, 0xc6748dac, 0x23a68512, 0x6dc9be33, 0x7d381a82,
		0x49b368a5, 0xeec77ed4, 0x9d0f5133, 0x803ff827, 0xa7bea34d, 0xfe6e8004,
		0x55f4a12a, 0x7f86f505, 0x64e74ebf, 0x14557e5e, 0xb8e46a6c, 0x83781d93,
		0x90a2c0ea, 0xd95985fc, 0x36b98c4b, 0x76d8d07a, 0x776b2e10, 0x38247306,
		0xdad97a7c, 0x916b4073, 0x831ee3f5, 0xb7979675, 0x12c78164, 0x63eeb006,
		0x7fa6096a, 0x571d5dc6, 0x00007ffe, 0xa8e35dc6, 0x805a096a, 0x9c12b006,
		0xed398164, 0x48699675, 0x7ce2e3f5, 0x6e954073, 0x25277a7c, 0xc7dc7306,
		0x88952e10, 0x8928d07a, 0xc9478c4b, 0x26a785fc, 0x6f5ec0ea, 0x7c881d93,
		0x471c6a6c, 0xebab7e5e, 0x9b194ebf, 0x807af505, 0xaa0ca12a, 0x01928004,
		0x5842a34d, 0x7fc1f827, 0x62f15133, 0x11397ed4, 0xb64d68a5, 0x82c81a82,
		0x9237be33, 0xdc5a8512, 0x398c8dac, 0x77f9d368, 0x764030fb, 0x354d745e,
		0xd7da7989, 0x8fde3db7, 0x83d7e0e7, 0xba3494b6, 0x15e181e4, 0x65ddb280,
		0x7f610c8b, 0x54c95fe2, 0xfcdc7ff5, 0xa69d5b9c, 0x80290647, 0x9e10ad98,
		0xf05580f8, 0x4afa9844, 0x7d89e708, 0x6cf84325, 0x22237b5c, 0xc50e719d,
		0x877d2b1e, 0x8a5ccd93, 0xcc228afd, 0x29a386f8, 0x70e1c3aa, 0x7bc4209f,
		0x447a6c23, 0xe8937dd5, 0x99324c3f, 0x80c9f1e5, 0xac669f15, 0x04b68018,
		0x5a81a57f, 0x7fe8fb4a, 0x60eb539a, 0x0e1b7f37, 0xb3c166ce, 0x822b176d,
		0x93ddbb86, 0xdf61843c, 0x3c568f1f, 0x7908d65d, 0x750333de, 0x326d75a4,
		0xd4e27883, 0x8e633af2, 0x84a4dddd, 0xbcdb9308, 0x18f88277, 0x67bcb506,
		0x7f080fab, 0x526861f0, 0xf9b97fd7, 0xa4645963, 0x800b0324, 0xa01eab37,
		0xf375809f, 0x4d809a23, 0x7e1cea1f, 0x6b4a45cc, 0x1f197c29, 0xc2497022,
		0x86772826, 0x8ba2cab3, 0xcf0589c0, 0x2c988807, 0x7254c674, 0x7aee23a6,
		0x41cd6dc9, 0xe57e7d38, 0x975b49b3, 0x812ceec7, 0xaecd9d0f, 0x07d9803f,
		0x5cb3a7be, 0x7ffcfe6e, 0x5ed655f4, 0x0afb7f86, 0xb14164e7, 0x81a21455,
		0x9594b8e4, 0xe26d8378, 0x3f1690a2, 0x7a04d959, 0x73b536b9, 0x2f8676d8,
		0xd1f0776b, 0x8cfa3824, 0x8584dad9, 0xbf8d916b, 0x1c0b831e, 0x698bb797,
		0x7e9c12c7, 0x4ffa63ee, 0xf6967fa6, 0xa23a571d, 0x80020000, 0xa23aa8e3,
		0xf696805a, 0x4ffa9c12, 0x7e9ced39, 0x698b4869, 0x1c0b7ce2, 0xbf8d6e95,
		0x85842527, 0x8cfac7dc, 0xd1f08895, 0x2f868928, 0x73b5c947, 0x7a0426a7,
		0x3f166f5e, 0xe26d7c88, 0x9594471c, 0x81a2ebab, 0xb1419b19, 0x0afb807a,
		0x5ed6aa0c, 0x7ffc0192, 0x5cb35842, 0x07d97fc1, 0xaecd62f1, 0x812c1139,
		0x975bb64d, 0xe57e82c8, 0x41cd9237, 0x7aeedc5a, 0x7254398c, 0x2c9877f9,
		0xcf057640, 0x8ba2354d, 0x8677d7da, 0xc2498fde, 0x1f1983d7, 0x6b4aba34,
		0x7e1c15e1, 0x4d8065dd, 0xf3757f61, 0xa01e54c9, 0x800bfcdc, 0xa464a69d,
		0xf9b98029, 0x52689e10, 0x7f08f055, 0x67bc4afa, 0x18f87d89, 0xbcdb6cf8,
		0x84a42223, 0x8e63c50e, 0xd4e2877d, 0x326d8a5c, 0x7503cc22, 0x790829a3,
		0x3c5670e1, 0xdf617bc4, 0x93dd447a, 0x822be893, 0xb3c19932, 0x0e1b80c9,
		0x60ebac66, 0x7fe804b6, 0x5a815a81, 0x04b67fe8, 0xac6660eb, 0x80c90e1b,
		0x9932b3c1, 0xe893822b, 0x447a93dd, 0x7bc4df61, 0x70e13c56, 0x29a37908,
		0xcc227503, 0x8a5c326d, 0x877dd4e2, 0xc50e8e63, 0x222384a4, 0x6cf8bcdb,
		0x7d8918f8, 0x4afa67bc, 0xf0557f08, 0x9e105268, 0x8029f9b9, 0xa69da464,
		0xfcdc800b, 0x54c9a01e, 0x7f61f375, 0x65dd4d80, 0x15e17e1c, 0xba346b4a,
		0x83d71f19, 0x8fdec249, 0xd7da8677, 0x354d8ba2, 0x7640cf05, 0x77f92c98,
		0x398c7254, 0xdc5a7aee, 0x923741cd, 0x82c8e57e, 0xb64d975b, 0x1139812c,
		0x62f1aecd, 0x7fc107d9, 0x58425cb3, 0x01927ffc, 0xaa0c5ed6, 0x807a0afb,
		0x9b19b141, 0xebab81a2, 0x471c9594, 0x7c88e26d, 0x6f5e3f16, 0x26a77a04,
		0xc94773b5, 0x89282f86, 0x8895d1f0, 0xc7dc8cfa, 0x25278584, 0x6e95bf8d,
		0x7ce21c0b, 0x4869698b, 0xed397e9c, 0x9c124ffa, 0x805af696, 0xa8e3a23a,
		0x00008002, 0x571da23a, 0x7fa6f696, 0x63ee4ffa, 0x12c77e9c, 0xb797698b,
		0x831e1c0b, 0x916bbf8d, 0xdad98584, 0x38248cfa, 0x776bd1f0, 0x76d82f86,
		0x36b973b5, 0xd9597a04, 0x90a23f16, 0x8378e26d, 0xb8e49594, 0x145581a2,
		0x64e7b141, 0x7f860afb, 0x55f45ed6, 0xfe6e7ffc, 0xa7be5cb3, 0x803f07d9,
		0x9d0faecd, 0xeec7812c, 0x49b3975b, 0x7d38e57e, 0x6dc941cd, 0x23a67aee,
		0xc6747254, 0x88072c98, 0x89c0cf05, 0xcab38ba2, 0x28268677, 0x7022c249,
		0x7c291f19, 0x45cc6b4a, 0xea1f7e1c, 0x9a234d80, 0x809ff375, 0xab37a01e,
		0x0324800b, 0x5963a464, 0x7fd7f9b9, 0x61f05268, 0x0fab7f08, 0xb50667bc,
		0x827718f8, 0x9308bcdb, 0xdddd84a4, 0x3af28e63, 0x7883d4e2, 0x75a4326d,
		0x33de7503, 0xd65d7908, 0x8f1f3c56, 0x843cdf61, 0xbb8693dd, 0x176d822b,
		0x66ceb3c1, 0x7f370e1b, 0x539a60eb, 0xfb4a7fe8, 0xa57f5a81, 0x801804b6,
		0x9f15ac66, 0xf1e580c9, 0x4c3f9932, 0x7dd5e893, 0x6c23447a, 0x209f7bc4,
		0xc3aa70e1, 0x86f829a3, 0x8afdcc22, 0xcd938a5c, 0x2b1e877d, 0x719dc50e,
		0x7b5c2223, 0x43256cf8, 0xe7087d89, 0x98444afa, 0x80f8f055, 0xad989e10,
		0x06478029, 0x5b9ca69d, 0x7ff5fcdc, 0x5fe254c9, 0x0c8b7f61, 0xb28065dd,
		0x81e415e1, 0x94b6ba34, 0xe0e783d7, 0x3db78fde, 0x7989d7da, 0x745e354d,
		0x30fb7640, 0xd36877f9, 0x8dac398c, 0x8512dc5a, 0xbe339237, 0x1a8282c8,
		0x68a5b64d, 0x7ed41139, 0x513362f1, 0xf8277fc1, 0xa34d5842, 0x80040192,
		0xa12aaa0c, 0xf505807a, 0x4ebf9b19, 0x7e5eebab, 0x6a6c471c, 0x1d937c88,
		0xc0ea6f5e, 0x85fc26a7, 0x8c4bc947, 0xd07a8928, 0x2e108895, 0x7306c7dc,
		0x7a7c2527, 0x40736e95, 0xe3f57ce2, 0x96754869, 0x8164ed39, 0xb0069c12,
		0x096a805a, 0x5dc6a8e3, 0x7ffe0000, 0x5dc6571d, 0x096a7fa6, 0xb00663ee,
		0x816412c7, 0x9675b797, 0xe3f5831e, 0x4073916b, 0x7a7cdad9, 0x73063824,
		0x2e10776b, 0xd07a76d8, 0x8c4b36b9, 0x85fcd959, 0xc0ea90a2, 0x1d938378,
		0x6a6cb8e4, 0x7e5e1455, 0x4ebf64e7, 0xf5057f86, 0xa12a55f4, 0x8004fe6e,
		0xa34da7be, 0xf827803f, 0x51339d0f, 0x7ed4eec7, 0x68a549b3, 0x1a827d38,
		0xbe336dc9, 0x851223a6, 0x8dacc674, 0xd3688807, 0x30fb89c0, 0x745ecab3,
		0x79892826, 0x3db77022, 0xe0e77c29, 0x94b645cc, 0x81e4ea1f, 0xb2809a23,
		0x0c8b809f, 0x5fe2ab37, 0x7ff50324, 0x5b9c5963, 0x06477fd7, 0xad9861f0,
		0x80f80fab, 0x9844b506, 0xe7088277, 0x43259308, 0x7b5cdddd, 0x719d3af2,
		0x2b1e7883, 0xcd9375a4, 0x8afd33de, 0x86f8d65d, 0xc3aa8f1f, 0x209f843c,
		0x6c23bb86, 0x7dd5176d, 0x4c3f66ce, 0xf1e57f37, 0x9f15539a, 0x8018fb4a,
		0xa57fa57f, 0xfb4a8018, 0x539a9f15, 0x7f37f1e5, 0x66ce4c3f, 0x176d7dd5,
		0xbb866c23, 0x843c209f, 0x8f1fc3aa, 0xd65d86f8, 0x33de8afd, 0x75a4cd93,
		0x78832b1e, 0x3af2719d, 0xdddd7b5c, 0x93084325, 0x8277e708, 0xb5069844,
		0x0fab80f8, 0x61f0ad98, 0x7fd70647, 0x59635b9c, 0x03247ff5, 0xab375fe2,
		0x809f0c8b, 0x9a23b280, 0xea1f81e4, 0x45cc94b6, 0x7c29e0e7, 0x70223db7,
		0x28267989, 0xcab3745e, 0x89c030fb, 0x8807d368, 0xc6748dac, 0x23a68512,
		0x6dc9be33, 0x7d381a82, 0x49b368a5, 0xeec77ed4, 0x9d0f5133, 0x803ff827,
		0xa7bea34d, 0xfe6e8004, 0x55f4a12a, 0x7f86f505, 0x64e74ebf, 0x14557e5e,
		0xb8e46a6c, 0x83781d93, 0x90a2c0ea, 0xd95985fc, 0x36b98c4b, 0x76d8d07a,
		0x776b2e10, 0x38247306, 0xdad97a7c, 0x916b4073, 0x831ee3f5, 0xb7979675,
		0x12c78164, 0x63eeb006, 0x7fa6096a, 0x571d5dc6, 0x00007ffe, 0xa8e35dc6,
		0x805a096a, 0x9c12b006, 0xed398164, 0x48699675, 0x7ce2e3f5, 0x6e954073,
		0x25277a7c, 0xc7dc7306, 0x88952e10, 0x8928d07a, 0xc9478c4b, 0x26a785fc,
		0x6f5ec0ea, 0x7c881d93, 0x471c6a6c, 0xebab7e5e, 0x9b194ebf, 0x807af505,
		0xaa0ca12a, 0x01928004, 0x5842a34d, 0x7fc1f827, 0x62f15133, 0x11397ed4,
		0xb64d68a5, 0x82c81a82, 0x9237be33, 0xdc5a8512, 0x398c8dac, 0x77f9d368,
		0x764030fb, 0x354d745e, 0xd7da7989, 0x8fde3db7, 0x83d7e0e7, 0xba3494b6,
		0x15e181e4, 0x65ddb280, 0x7f610c8b, 0x54c95fe2, 0xfcdc7ff5, 0xa69d5b9c,
		0x80290647, 0x9e10ad98, 0xf05580f8, 0x4afa9844, 0x7d89e708, 0x6cf84325,
		0x22237b5c, 0xc50e719d, 0x877d2b1e, 0x8a5ccd93, 0xcc228afd, 0x29a386f8,
		0x70e1c3aa, 0x7bc4209f, 0x447a6c23, 0xe8937dd5, 0x99324c3f, 0x80c9f1e5,
		0xac669f15, 0x04b68018, 0x5a81a57f, 0x7fe8fb4a, 0x60eb539a, 0x0e1b7f37,
		0xb3c166ce, 0x822b176d, 0x93ddbb86, 0xdf61843c, 0x3c568f1f, 0x7908d65d,
		0x750333de, 0x326d75a4, 0xd4e27883, 0x8e633af2, 0x84a4dddd, 0xbcdb9308,
		0x18f88277, 0x67bcb506, 0x7f080fab, 0x526861f0, 0xf9b97fd7, 0xa4645963,
		0x800b0324, 0xa01eab37, 0xf375809f, 0x4d809a23, 0x7e1cea1f, 0x6b4a45cc,
		0x1f197c29, 0xc2497022, 0x86772826, 0x8ba2cab3, 0xcf0589c0, 0x2c988807,
		0x7254c674, 0x7aee23a6, 0x41cd6dc9, 0xe57e7d38, 0x975b49b3, 0x812ceec7,
		0xaecd9d0f, 0x07d9803f, 0x5cb3a7be, 0x7ffcfe6e, 0x5ed655f4, 0x0afb7f86,
		0xb14164e7, 0x81a21455, 0x9594b8e4, 0xe26d8378, 0x3f1690a2, 0x7a04d959,
		0x73b536b9, 0x2f8676d8, 0xd1f0776b, 0x8cfa3824, 0x8584dad9, 0xbf8d916b,
		0x1c0b831e, 0x698bb797, 0x7e9c12c7, 0x4ffa63ee, 0xf6967fa6, 0xa23a571d,
		0x80020000, 0xa23aa8e3, 0xf696805a, 0x4ffa9c12, 0x7e9ced39, 0x698b4869,
		0x1c0b7ce2, 0xbf8d6e95, 0x85842527, 0x8cfac7dc, 0xd1f08895, 0x2f868928,
		0x73b5c947, 0x7a0426a7, 0x3f166f5e, 0xe26d7c88, 0x9594471c, 0x81a2ebab,
		0xb1419b19, 0x0afb807a, 0x5ed6aa0c, 0x7ffc0192, 0x5cb35842, 0x07d97fc1,
		0xaecd62f1, 0x812c1139, 0x975bb64d, 0xe57e82c8, 0x41cd9237, 0x7aeedc5a,
		0x7254398c, 0x2c9877f9, 0xcf057640, 0x8ba2354d, 0x8677d7da, 0xc2498fde,
		0x1f1983d7, 0x6b4aba34, 0x7e1c15e1, 0x4d8065dd, 0xf3757f61, 0xa01e54c9,
		0x800bfcdc, 0xa464a69d, 0xf9b98029, 0x52689e10, 0x7f08f055, 0x67bc4afa,
		0x18f87d89, 0xbcdb6cf8, 0x84a42223, 0x8e63c50e, 0xd4e2877d, 0x326d8a5c,
		0x7503cc22, 0x790829a3, 0x3c5670e1, 0xdf617bc4, 0x93dd447a, 0x822be893,
		0xb3c19932, 0x0e1b80c9, 0x60ebac66, 0x7fe804b6, 0x5a815a81, 0x04b67fe8,
		0xac6660eb, 0x80c90e1b, 0x9932b3c1, 0xe893822b, 0x447a93dd, 0x7bc4df61,
		0x70e13c56, 0x29a37908, 0xcc227503, 0x8a5c326d, 0x877dd4e2, 0xc50e8e63,
		0x222384a4, 0x6cf8bcdb, 0x7d8918f8, 0x4afa67bc, 0xf0557f08, 0x9e105268,
		0x8029f9b9, 0xa69da464, 0xfcdc800b, 0x54c9a01e, 0x7f61f375, 0x65dd4d80,
		0x15e17e1c, 0xba346b4a, 0x83d71f19, 0x8fdec249, 0xd7da8677, 0x354d8ba2,
		0x7640cf05, 0x77f92c98, 0x398c7254, 0xdc5a7aee, 0x923741cd, 0x82c8e57e,
		0xb64d975b, 0x1139812c, 0x62f1aecd, 0x7fc107d9, 0x58425cb3, 0x01927ffc,
		0xaa0c5ed6, 0x807a0afb, 0x9b19b141, 0xebab81a2, 0x471c9594, 0x7c88e26d,
		0x6f5e3f16, 0x26a77a04, 0xc94773b5, 0x89282f86, 0x8895d1f0, 0xc7dc8cfa,
		0x25278584, 0x6e95bf8d, 0x7ce21c0b, 0x4869698b, 0xed397e9c, 0x9c124ffa,
		0x805af696, 0xa8e3a23a, 0x00008002, 0x571da23a, 0x7fa6f696, 0x63ee4ffa,
		0x12c77e9c, 0xb797698b, 0x831e1c0b, 0x916bbf8d, 0xdad98584, 0x38248cfa,
		0x776bd1f0, 0x76d82f86, 0x36b973b5, 0xd9597a04, 0x90a23f16, 0x8378e26d,
		0xb8e49594, 0x145581a2, 0x64e7b141, 0x7f860afb, 0x55f45ed6, 0xfe6e7ffc,
		0xa7be5cb3, 0x803f07d9, 0x9d0faecd, 0xeec7812c, 0x49b3975b, 0x7d38e57e,
		0x6dc941cd, 0x23a67aee, 0xc6747254, 0x88072c98, 0x89c0cf05, 0xcab38ba2,
		0x28268677, 0x7022c249, 0x7c291f19, 0x45cc6b4a, 0xea1f7e1c, 0x9a234d80,
		0x809ff375, 0xab37a01e, 0x0324800b, 0x5963a464, 0x7fd7f9b9, 0x61f05268,
		0x0fab7f08, 0xb50667bc, 0x827718f8, 0x9308bcdb, 0xdddd84a4, 0x3af28e63,
		0x7883d4e2, 0x75a4326d, 0x33de7503, 0xd65d7908, 0x8f1f3c56, 0x843cdf61,
		0xbb8693dd, 0x176d822b, 0x66ceb3c1, 0x7f370e1b, 0x539a60eb, 0xfb4a7fe8,
		0xa57f5a81, 0x801804b6, 0x9f15ac66, 0xf1e580c9, 0x4c3f9932, 0x7dd5e893,
		0x6c23447a, 0x209f7bc4, 0xc3aa70e1, 0x86f829a3, 0x8afdcc22, 0xcd938a5c,
		0x2b1e877d, 0x719dc50e, 0x7b5c2223, 0x43256cf8, 0xe7087d89, 0x98444afa,
		0x80f8f055, 0xad989e10, 0x06478029, 0x5b9ca69d, 0x7ff5fcdc, 0x5fe254c9,
		0x0c8b7f61, 0xb28065dd, 0x81e415e1, 0x94b6ba34, 0xe0e783d7, 0x3db78fde,
		0x7989d7da, 0x745e354d, 0x30fb7640, 0xd36877f9, 0x8dac398c, 0x8512dc5a,
		0xbe339237, 0x1a8282c8, 0x68a5b64d, 0x7ed41139, 0x513362f1, 0xf8277fc1,
		0xa34d5842, 0x80040192, 0xa12aaa0c, 0xf505807a, 0x4ebf9b19, 0x7e5eebab,
		0x6a6c471c, 0x1d937c88, 0xc0ea6f5e, 0x85fc26a7, 0x8c4bc947, 0xd07a8928,
		0x2e108895, 0x7306c7dc, 0x7a7c2527, 0x40736e95, 0xe3f57ce2, 0x96754869,
		0x8164ed39, 0xb0069c12, 0x096a805a, 0x5dc6a8e3
};

/**************************************************************************//**
* @brief Delays the program execution with the specified number of ms.
*
* @param ns_count Number of ms with which the program must be delayed.
* @return None.
******************************************************************************/
void delay_ms(uint32_t ms_count)
{
#ifdef _XPARAMETERS_PS_H_
	volatile uint32_t i;
	for(i = 0; i < ms_count*1000; i++);
#else
	TIMER0_WAIT(XPAR_AXI_TIMER_0_BASEADDR, ms_count*1000000);
#endif
}

/**************************************************************************//**
* @brief Computes the DDS increment and offset.
*
* @return DDS increment and offset.
******************************************************************************/
uint32_t dds_pf(uint32_t phase, uint32_t sin_clk, uint32_t dac_clk)
{
	uint32_t p_offset;
	uint32_t p_incr;

	p_offset = (phase*0xffff)/360;
	p_incr = ((sin_clk*0xffff)/dac_clk) | 0x1;

	return((p_offset<<16) | p_incr);
}

/**************************************************************************//**
* @brief Initializes the DDS core.
*
* @return None.
******************************************************************************/
void dds_setup(uint32_t sel, uint32_t f1, uint32_t f2)
{
	uint32_t baddr;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;
	Xil_Out32((baddr + 0x04), 0x1);
	Xil_Out32((baddr + 0x08), dds_pf(0, f1, 500));
	Xil_Out32((baddr + 0x0c), dds_pf(0, f2, 500));
	Xil_Out32((baddr + 0x10), dds_pf(90, f1, 500));
	Xil_Out32((baddr + 0x14), dds_pf(90, f2, 500));
	Xil_Out32((baddr + 0x04), 0x3);

	xil_printf("dac_dds: f1(%dMHz), f2(%dMHz).\n\r", f1, f2);
}

/**************************************************************************//**
* @brief Initializes the DAC DMA.
*
* @return None.
******************************************************************************/
void dac_dma_setup(uint32_t sel)
{
	uint32_t dac_baseaddr;
	uint32_t vdma_baseaddr;
	uint32_t index;
	uint32_t status;

	dac_baseaddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;
	vdma_baseaddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? VDMA9122_1_BASEADDR : VDMA9122_0_BASEADDR;

	for(index = 0; index < 1024; index++)
	{
		Xil_Out32((DDRDAC_BASEADDR + (4*index)), dac_dma_data[index]);
	}
#ifdef _XPARAMETERS_PS_H_
	Xil_DCacheFlush();
#else
	microblaze_flush_dcache();
	microblaze_invalidate_dcache();
#endif
	xil_printf("dac_dma: buffer-count(%d).\n\r", index);
	Xil_Out32((dac_baseaddr + 0x2c), (index/2));
	Xil_Out32((dac_baseaddr + 0x04), 0x0);
	Xil_Out32((vdma_baseaddr + 0x000), 0x4); // reset
	Xil_Out32((vdma_baseaddr + 0x000), 0x0); // reset
	Xil_Out32((vdma_baseaddr + 0x000), 0x3); // enable circular mode
	Xil_Out32((vdma_baseaddr + 0x05c), DDRDAC_BASEADDR); // start address
	Xil_Out32((vdma_baseaddr + 0x060), DDRDAC_BASEADDR); // start address
	Xil_Out32((vdma_baseaddr + 0x064), DDRDAC_BASEADDR); // start address
	Xil_Out32((vdma_baseaddr + 0x058), ((index/2)*4));
	Xil_Out32((vdma_baseaddr + 0x054), ((index/2)*4));
	Xil_Out32((vdma_baseaddr + 0x050), 2);
	Xil_Out32((dac_baseaddr + 0x18), 0x2e715a01);
	Xil_Out32((dac_baseaddr + 0x04), 0x0);
	Xil_Out32((dac_baseaddr + 0x04), 0x1);
	Xil_Out32((dac_baseaddr + 0x04), 0xf);
	Xil_Out32((dac_baseaddr + 0x28), 0x3); // clear status
	xil_printf("dac_dma: f(60MHz).\n\r");
	delay_ms(10);
	status = Xil_In32((dac_baseaddr + 0x28));
	if (status != 0x0)
	{
		xil_printf("dma_setup: status(%x)\n\r", status);
	}
}

/**************************************************************************//**
* @brief Puts the DAC in SED mode and verifies the correctness of the samples
*
* @return None.
******************************************************************************/
void dac_sed(uint32_t sel, uint32_t s0, uint32_t s1)
{
	uint32_t baddr;
	uint32_t rdata;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;

	ad9122_write(0x68, ((s0>> 0) & 0xff));
	ad9122_write(0x69, ((s0>> 8) & 0xff));
	ad9122_write(0x6a, ((s1>> 0) & 0xff));
	ad9122_write(0x6b, ((s1>> 8) & 0xff));
	ad9122_write(0x6c, ((s0>>16) & 0xff));
	ad9122_write(0x6d, ((s0>>24) & 0xff));
	ad9122_write(0x6e, ((s1>>16) & 0xff));
	ad9122_write(0x6f, ((s1>>24) & 0xff));
	ad9122_write(0x67, 0x00);
	ad9122_write(0x67, 0x80);

	Xil_Out32((baddr + 0x40), s0);
	Xil_Out32((baddr + 0x44), s1);
	Xil_Out32((baddr + 0x04), 0x11);
	Xil_Out32((baddr + 0x04), 0x13);
	delay_ms(10);

	ad9122_write(0x67, 0xa3);
	ad9122_write(0x07, 0x1c);
	delay_ms(100);

	xil_printf("dac_sed: s0(0x%08x), s1(0x%08x)\n\r", s0, s1);
	rdata = ad9122_read(0x67);
	if ((rdata & 0x20) == 0x20)
	{
		xil_printf("ERROR: Addr(0x67) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x07);
	if ((rdata & 0x02) == 0x02)
	{
		xil_printf("ERROR: Addr(0x07) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x70);
	if (rdata != 0x00)
	{
		xil_printf("ERROR: Addr(0x70) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x71);
	if (rdata != 0x00)
	{
		xil_printf("ERROR: Addr(0x71) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x72);
	if (rdata != 0x00)
	{
		xil_printf("ERROR: Addr(0x72) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x73);
	if (rdata != 0x00)
	{
		xil_printf("ERROR: Addr(0x73) Data(0x%02x)!\n\r", rdata);
	}

	return;
}

/**************************************************************************//**
* @brief Tests the communication with the DAC
*
* @return None.
******************************************************************************/
void dac_test(uint32_t sel)
{
	uint32_t baddr;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;

	Xil_Out32((baddr + 0x04), 0x0);
	Xil_Out32((baddr + 0x04), 0x1);
	Xil_Out32((baddr + 0x20), 0x1111);

	ad9122_write(0x16, 0x01);
	ad9122_read(0x16);

	dac_sed(sel, 0x0000aaaa, 0x00000000);
	dac_sed(sel, 0x00005555, 0x00000000);
	dac_sed(sel, 0xaaaa0000, 0x00000000);
	dac_sed(sel, 0x55550000, 0x00000000);
	dac_sed(sel, 0x00000000, 0x0000aaaa);
	dac_sed(sel, 0x00000000, 0x00005555);
	dac_sed(sel, 0x00000000, 0xaaaa0000);
	dac_sed(sel, 0x00000000, 0x55550000);
	dac_sed(sel, 0x00000000, 0x00000000);
	dac_sed(sel, 0xaaaaaaaa, 0x55555555);
	dac_sed(sel, 0x55555555, 0xaaaaaaaa);

	return;
}

/**************************************************************************//**
* @brief Captures data from the ADC
*
* @return None.
******************************************************************************/
void adc_capture(uint32_t sel, uint32_t qwcnt, uint32_t sa)
{
	uint32_t baddr;
	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? DMA9643_1_BASEADDR : DMA9643_0_BASEADDR;

	Xil_Out32((baddr + 0x030), 0); // clear dma operations
	Xil_Out32((baddr + 0x030), 1); // enable dma operations
	Xil_Out32((baddr + 0x048), sa); // capture start address
	Xil_Out32((baddr + 0x058), (qwcnt * 8)); // number of bytes

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9643_1_BASEADDR : CFAD9643_0_BASEADDR;
	Xil_Out32((baddr + 0x008), 0x03); // channel enables
	Xil_Out32((baddr + 0x00c), 0x0); // capture disable
	Xil_Out32((baddr + 0x010), 0xf); // clear status
	Xil_Out32((baddr + 0x014), 0xf); // clear status
	Xil_Out32((baddr + 0x00c), (0x10000 | (qwcnt-1))); // start capture

	do
	{
		delay_ms(1);
	}
	while ((Xil_In32(baddr + 0x010) & 0x1) == 1);

	if ((Xil_In32(baddr + 0x010) & 0x02) == 0x02)
	{
		xil_printf("adc_capture: overflow occured, data may be corrupted\n\r");
	}

#ifdef _XPARAMETERS_PS_H_
	Xil_DCacheFlush();
#else
	microblaze_flush_dcache();
	microblaze_invalidate_dcache();
#endif
}

/**************************************************************************//**
* @brief Tests the communication with the ADC
*
* @return None.
******************************************************************************/
void adc_test(uint32_t sel, uint32_t mode, uint32_t format)
{
	uint32_t n;
	uint32_t rdata;
	uint32_t adata;
	uint32_t bdata;
	uint32_t edata;
	uint32_t baddr;

	Xil_Out32(XPAR_AXI_ADC_2C_0_BASEADDR + 0x2C, 0);

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9643_1_BASEADDR : CFAD9643_0_BASEADDR;
	adc_capture(sel, 16, DDR_BASEADDR);
	delay_ms(10);

	xil_printf("adc_test: mode(%2d), format(%2d)\n\r", mode, format);

	/* Alternating Checkerboard */
	if (mode == 0x4)
	{
		for (n = 0; n < 32; n++)
		{
			rdata = Xil_In32(DDR_BASEADDR+(n*4));
			if (n == 0)
			{
				adata = ((rdata & 0xffff) == 0x2aaa) ? 0x2aaa : 0x1555;
				bdata = (((rdata>>16) & 0xffff) == 0x2aaa) ? 0x2aaa : 0x1555;
			}
			else
			{
				adata = (adata == 0x2aaa) ? 0x1555 : 0x2aaa;
				bdata = (bdata == 0x2aaa) ? 0x1555 : 0x2aaa;
			}
			edata = (bdata<<16) | adata;
			if (rdata != edata)
			{
				xil_printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata, edata);
			}
		}
		goto end;
	}

	/* PnLongSeq or PnShortSeq */
	if ((mode == 0x5) || (mode == 0x6))
	{
		if (format == 0x1)
			goto end;
		Xil_Out32((baddr+0x24), ((mode == 0x5) ? 0x3 : 0x0));
		delay_ms(10);
		Xil_Out32((baddr+0x14), 0xff);
		delay_ms(100);
		if ((Xil_In32(baddr+0x14) & 0x3c) != 0)
		{
			xil_printf("  ERROR: PN status(%02x).\n\r", Xil_In32(baddr+0x14));
		}
		goto end;
	}

	/* One Zero Toggle */
	if (mode == 0x7)
	{
		for (n = 0; n < 32; n++)
		{
			rdata = Xil_In32(DDR_BASEADDR+(n*4));
			if (n == 0)
			{
				adata = ((rdata & 0xffff) == 0x3fff) ? 0x3fff : 0x0000;
				bdata = (((rdata>>16) & 0xffff) == 0x3fff) ? 0x3fff : 0x0000;
			}
			else
			{
				adata = (adata == 0x3fff) ? 0x0000 : 0x3fff;
				bdata = (bdata == 0x3fff) ? 0x0000 : 0x3fff;
			}
			edata = (bdata<<16) | adata;
			if (rdata != edata)
			{
				xil_printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata, edata);
			}
		}
		goto end;
	}

	/* Ramp */
	if (mode == 0xf)
	{
		for (n = 0; n < 16; n++)
		{
			rdata = Xil_In32(DDR_BASEADDR+(n*8));
			if (n == 0)
			{
				adata = rdata & 0xffff;
				bdata = (rdata>>16) & 0xffff;
			}
			else
			{
				adata = (adata+1) & 0x3fff;
				bdata = (bdata+1) & 0x3fff;
			}
			edata = (bdata<<16) | adata;
			if (rdata != edata)
			{
				xil_printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata, edata);
			}
		}
		goto end;
	}

	/* MidscaleShort, PositiveFs, NegativeFs */
	edata = 0xffffffff;
	if ((mode == 0x1) && (format == 0))
		edata = 0x20002000;
	if ((mode == 0x2) && (format == 0))
		edata = 0x3fff3fff;
	if ((mode == 0x3) && (format == 0))
		edata = 0x00000000;
	if ((mode == 0x1) && (format == 1))
		edata = 0x00000000;
	if ((mode == 0x2) && (format == 1))
		edata = 0x1fff1fff;
	if ((mode == 0x3) && (format == 1))
		edata = 0x20002000;
	for (n = 0; n < 32; n++)
	{
		rdata = Xil_In32(DDR_BASEADDR+(n*4));
		if (rdata != edata)
		{
			xil_printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata, edata);
		}
	}
end:
	Xil_Out32(XPAR_AXI_ADC_2C_0_BASEADDR + 0x2C, 0x02);
}
